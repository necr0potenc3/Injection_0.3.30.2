////////////////////////////////////////////////////////////////////////////////
//
// target.cpp
//
// Copyright (C) 2001 Wayne 'chiphead' Hogue
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
//  Handles automated target input
//
////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <time.h>
#include "common.h"
#include "client.h"
#include "target.h"


TargetHandler::TargetHandler(ClientInterface & client)
: m_client(client), m_state(NORMAL)
{
}

TargetHandler::~TargetHandler()
{
}


void TargetHandler::send_target(uint32 target)
{
    uint8 buf[19];
    memcpy(buf, m_requested_target, 19);
    pack_big_uint32(buf+7, target);
    buf[18] = 0x0f;
    m_client.send_server(buf, 19);
}

// This function is called for 0x1c messages
bool TargetHandler::handle_server_talk(uint8 * buf, int size)
{
    bool resend = true;
    if(m_state == WAITING)
    {
        // Select Target
        if(size > 57)
        {
            if(strncmp((char *)(buf+44), "Select Target", 13) == 0)
            {
                // Select Target sent
                resend = false;
            }
        }
    }
    return resend;
}

// This function is called for 0x6c messages
bool TargetHandler::handle_target(uint8 * buf, int /*size*/)
{
    bool resend = true;
    if(m_state == WAITING)
    {
        memcpy(m_requested_target, buf, 19);
        resend = false;
        if(m_has_second_target)
        {
            uint32 t = m_target;
            m_target = m_target2;
            m_has_second_target = false;
            m_state = WAITING;
            send_target(t);
        }
        else
        {
            m_state = NORMAL;
            send_target(m_target);
        }
    }
    else if(m_state == TARGETING)
    {
        m_client.client_print("Warning: second target request, targeting cancelled");
        m_state = NORMAL;
    }
    // In state NORMAL, silently pass the target request to the client.
    return resend;
}

void TargetHandler::wait_target(uint32 target)
{
    if(m_state == WAITING)
    {
        m_client.client_print("Auto target cancelled");
        send_target(0x0);
    }
    else if(m_state == TARGETING)
    {
        m_client.client_print("Previous target cancelled");
        send_target(0x0);
    }
    m_target = target;
    m_has_second_target = false;
    m_state = WAITING;
}

void TargetHandler::wait_target(uint32 target, uint32 target2)
{
    if(m_state == WAITING)
    {
        m_client.client_print("Previous target cancelled");
        send_target(0x0);
    }
    else if(m_state == TARGETING)
    {
        m_client.client_print("Previous target cancelled");
        send_target(0x0);
    }
    m_target = target;
    m_target2 = target2;
    m_has_second_target = true;
    m_state = WAITING;
}

void TargetHandler::cancel_target()
{
    if(m_state == NORMAL)
        m_client.client_print("Error: no target to cancel");
    else if(m_state == WAITING)
        m_client.client_print("Targeting cancelled");
    else if(m_state == TARGETING)
    {
        send_target(0x0);
    }
    m_state = NORMAL;
}

bool TargetHandler::waiting()
{
    return(m_state == WAITING);
}