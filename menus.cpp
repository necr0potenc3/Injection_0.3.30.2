////////////////////////////////////////////////////////////////////////////////
//
// menus.cpp
//
// Copyright (C) 2001 Luke 'Infidel' Dunstan
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//
//  Handles automated menu gump input
//
////////////////////////////////////////////////////////////////////////////////

#include "common.h"
#include "client.h"

#include "menus.h"


class MenuOption
{
public:
    uint16 m_graphic;
    string m_description;
};

MenuHandler::MenuHandler(ClientInterface & client)
: m_client(client), m_state(NORMAL), m_options(0)
{
}

MenuHandler::~MenuHandler()
{
    delete [] m_options;
}

// (private)
// Index should be 0-based, or -1 to cancel the menu
void MenuHandler::send_choice(int index)
{
    uint8 buf[13];
    buf[0] = CODE_MENU_CHOICE;
    pack_big_uint32(buf + 1, m_id);
    pack_big_uint16(buf + 5, m_gump);
    pack_big_uint16(buf + 7, index + 1);
    if(index == -1)
        pack_big_uint16(buf + 9, 0);
    else
        pack_big_uint16(buf + 9, m_options[index].m_graphic);
    pack_big_uint16(buf + 11, 0);
    m_client.send_server(buf, sizeof(buf));
}

// private
void MenuHandler::choose_menu(const char * desc)
{
    if(m_state == CHOOSING)
    {
        for(int i = 0; i < m_num_options; i++)
            if(m_options[i].m_description.find(desc) != string::npos)
            {
                m_client.client_print("Menu choice successful.");
                if(m_has_third_menu)
                {
                    m_menu_prompt = m_menu_prompt2;
                    m_menu_choice = m_menu_choice2;
                    m_menu_prompt2 = m_menu_prompt3;
                    m_menu_choice2 = m_menu_choice3;
                    m_has_third_menu = false;
                    m_has_second_menu = true;
                    m_state = WAITING;
                    send_choice(i);
                    return;
                }
                else if(m_has_second_menu)
                {
                    m_menu_prompt = m_menu_prompt2;
                    m_menu_choice = m_menu_choice2;
                    m_has_second_menu = false;
                    m_state = WAITING;
                    send_choice(i);
                    return;
                }
                else
                {
                    send_choice(i);
                    m_state = NORMAL;
                    return;
                }
            }
        m_client.client_print("Menu choice not found: menu cancelled.");
        send_choice(-1);
        m_state = NORMAL;
    }
    else if(m_state == WAITING)
    {
        m_client.client_print("Cannot choose: no menu open, waiting cancelled.");
        m_state = NORMAL;
    }
    else
        m_client.client_print("Cannot choose: no menu open.");
}

// This function is called for 0x7c messages
bool MenuHandler::handle_open_menu_gump(uint8 * buf, int /*size*/)
{
    bool resend = true;
    if(m_state == WAITING)
    {
        int prompt_length = buf[9];
        string prompt(reinterpret_cast<char *>(buf + 10), prompt_length);
        // Look for the desired substring within the prompt
        if(prompt.find(m_menu_prompt) == string::npos)
        {
            m_client.client_print(string("Warning: menu '") + prompt +
                string("' opened, waiting cancelled"));
            m_state = NORMAL;
        }
        else
        {
            m_id = unpack_big_uint32(buf + 3);
            m_gump = unpack_big_uint16(buf + 7);
            uint8 * ptr = buf + 10 + prompt_length;
            m_num_options = *ptr++;
            delete [] m_options;
            m_options = new MenuOption[m_num_options];
            for(int i = 0; i < m_num_options; i++)
            {
                m_options[i].m_graphic = unpack_big_uint16(ptr);
                int desc_len = ptr[4];
                ptr += 5;
                m_options[i].m_description.assign(
                    reinterpret_cast<char *>(ptr), desc_len);
                ptr += desc_len;
            }
            m_state = CHOOSING;
            choose_menu(m_menu_choice.c_str());
            resend = false;
        }
    }
    else if(m_state == CHOOSING)
    {
        m_client.client_print("Warning: menu opened, choosing cancelled");
        send_choice(-1);
        m_state = NORMAL;
    }
    // In state NORMAL, silently pass the menu to the client.
    return resend;
}

void MenuHandler::wait_menu(const char * prompt, const char * choice)
{
    if(m_state == WAITING)
        m_client.client_print(string("Previous waitmenu cancelled: ") +
            m_menu_prompt);
    else if(m_state == CHOOSING)
    {
        m_client.client_print(string("Previous menu gump cancelled: ") +
            m_menu_prompt);
        send_choice(-1);
    }
    m_state = WAITING;
    m_menu_prompt = prompt;
    m_menu_choice = choice;
    m_has_second_menu = false;
    m_client.client_print("Now waiting for menu...");
}

void MenuHandler::wait_menu(const char * prompt, const char * choice, const char * prompt2, const char * choice2)
{
    if(m_state == WAITING)
        m_client.client_print(string("Previous waitmenu cancelled: ") +
            m_menu_prompt);
    else if(m_state == CHOOSING)
    {
        m_client.client_print(string("Previous menu gump cancelled: ") +
            m_menu_prompt);
        send_choice(-1);
    }
    m_state = WAITING;
    m_menu_prompt = prompt;
    m_menu_choice = choice;
    m_has_second_menu = true;
    m_menu_prompt2 = prompt2;
    m_menu_choice2 = choice2;
    m_client.client_print("Now waiting for menu...");
}

void MenuHandler::wait_menu(const char * prompt, const char * choice, const char * prompt2, const char * choice2, const char * prompt3, const char * choice3)
{
    if(m_state == WAITING)
        m_client.client_print(string("Previous waitmenu cancelled: ") +
            m_menu_prompt);
    else if(m_state == CHOOSING)
    {
        m_client.client_print(string("Previous menu gump cancelled: ") +
            m_menu_prompt);
        send_choice(-1);
    }
    m_state = WAITING;
    m_menu_prompt = prompt;
    m_menu_choice = choice;
    m_has_second_menu = true;
    m_menu_prompt2 = prompt2;
    m_menu_choice2 = choice2;
    m_has_third_menu = true;
    m_menu_prompt3 = prompt3;
    m_menu_choice3 = choice3;
    m_client.client_print("Now waiting for menu...");
}

void MenuHandler::cancel_menu()
{
    if(m_state == NORMAL)
        m_client.client_print("Error: no menu to cancel");
    else if(m_state == WAITING)
        m_client.client_print(string("waitmenu cancelled: ") + m_menu_prompt);
    else if(m_state == CHOOSING)
    {
        m_client.client_print(string("Menu gump cancelled: ") + m_menu_prompt);
        send_choice(-1);
    }
    m_state = NORMAL;
}
