////////////////////////////////////////////////////////////////////////////////
//
// ilconfig.h
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
//	Declarations for classes in ilconfig.cpp
//
//  Handles loading/saving of configuration information.
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _ILCONFIG_H_
#define _ILCONFIG_H_

#include <string>
#include <vector>
using std::string;
using std::vector;

class ServerItem
{
public:
	string m_name, m_address, m_username, m_password;
	ServerItem(const char * name, const char * address,	const char * username, const char * password)
	: m_name(name), m_address(address), m_username(username), m_password(password)
	{
	}
};

class ClientItem
{
public:
	string m_name, m_path;

	ClientItem(const char * name, const char * path)
	: m_name(name), m_path(path)
	{
	}
};

class ILaunchConfigException
{
private:
	string m_message;
	int m_errno;

public:
	// NOTE: errno can't be used for a parameter name because it is a macro.
	ILaunchConfigException(const string & message, int p_errno = 0)
	: m_message(message), m_errno(p_errno)
	{
	}

	ILaunchConfigException(const char * message, int p_errno = 0)
	: m_message(message), m_errno(p_errno)
	{
	}

	string get_message() const
	{
		if(m_errno == 0)
			return m_message;
		else
			return m_message + '(' + strerror(m_errno) + ')';
	}
};

class ILaunchConfig
{
public:
	typedef vector<ServerItem> serverlist_t;
	typedef vector<ClientItem> clientlist_t;

private:
	bool m_loaded;
	string m_filename;
	serverlist_t m_servers;
	int m_last_server;	// index of last selected server
	clientlist_t m_clients;
	int m_last_client;
	string m_client_dir, m_ignition_cfg;
	bool m_use_injection, m_close;

	void find_client_dir();

public:
	ILaunchConfig();

	static string escape_attribute(const string & key);
	static string crypt_pw(const string & str);

	void load(const char * filename) throw(ILaunchConfigException);
	void save() const throw(ILaunchConfigException);

	serverlist_t & get_servers() { return m_servers; }
	void set_servers(const serverlist_t & servers)
	{
		m_servers = servers;
	}
	void add_server(const char * name, const char * address, const char * username, const char * password);
	int get_last_server() const { return m_last_server; }
	void set_last_server(int i) { m_last_server = i; }

	clientlist_t & get_clients() { return m_clients; }
	void set_clients(const clientlist_t & clients)
	{
		m_clients = clients;
	}
	void add_client(const char * name, const char * path);
	int get_last_client() const { return m_last_client; }
	void set_last_client(int i) { m_last_client = i; }

	const char * get_client_dir() const { return m_client_dir.c_str(); }
	void set_client_dir(const char * s) { m_client_dir = s; }

	const char * get_ignition_cfg() const { return m_ignition_cfg.c_str(); }
	void set_ignition_cfg(const char * s) { m_ignition_cfg = s; }

	bool get_use_injection() const { return m_use_injection; }
	void set_use_injection(bool b) { m_use_injection = b; }

	bool get_close() const { return m_close; }
	void set_close(bool b) { m_close = b; }

};

#endif

