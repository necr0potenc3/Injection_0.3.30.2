////////////////////////////////////////////////////////////////////////////////
//
// ilconfig.cpp
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
//  Handles loading/saving of configuration information.
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <errno.h>

#include <windows.h>
#include <expat.h>

#include "ilconfig.h"

////////////////////////////////////////////////////////////////////////////////

const int BUFFER_SIZE = 4096;

class ExpatParser
{
private:
	XML_Parser m_parser;
	enum XML_Error m_error;
	int m_line;

	static void s_start_element(void * user, const XML_Char * name,
		const XML_Char ** attrs);
	static void s_end_element(void * user, const XML_Char * name);

protected:
	virtual void start_element(const XML_Char * name,
		const XML_Char ** attrs) = 0;
	virtual void end_element(const XML_Char * name) = 0;

public:
	ExpatParser();
	virtual ~ExpatParser();

	bool parse_file(FILE * fp);
	const char * get_error_msg() const;
	int get_error_line() const { return m_line; }
};

ExpatParser::ExpatParser()
{
	m_parser = XML_ParserCreate(NULL);
	XML_SetUserData(m_parser, this);
	XML_SetStartElementHandler(m_parser, ExpatParser::s_start_element);
	XML_SetEndElementHandler(m_parser, ExpatParser::s_end_element);
}

ExpatParser::~ExpatParser()
{
	XML_ParserFree(m_parser);
}

// static, private
void ExpatParser::s_start_element(void * user, const XML_Char * name,
	const XML_Char ** attrs)
{
	reinterpret_cast<ExpatParser *>(user)->start_element(name, attrs);
}

// static, private
void ExpatParser::s_end_element(void * user, const XML_Char * name)
{
	reinterpret_cast<ExpatParser *>(user)->end_element(name);
}

bool ExpatParser::parse_file(FILE * fp)
{
	while(!feof(fp))
	{
		void * buf = XML_GetBuffer(m_parser, BUFFER_SIZE);
		if(buf == NULL)
			return false;

		int n = fread(buf, 1, BUFFER_SIZE, fp);

		if(XML_ParseBuffer(m_parser, n, feof(fp)) == 0)
		{
			m_error = XML_GetErrorCode(m_parser);
			m_line = XML_GetCurrentLineNumber(m_parser);
			return false;
		}
	}
	return true;
}

const char * ExpatParser::get_error_msg() const
{
	return XML_ErrorString(m_error);
}

////////////////////////////////////////////////////////////////////////////////

class ILaunchConfigParser : public ExpatParser
{
private:
	ILaunchConfig & m_config;
	enum { TOP, CONFIG, SERVER, CLIENT } m_level;

	void begin_config(const XML_Char ** attrs);
	void begin_server(const XML_Char ** attrs);
	void begin_client(const XML_Char ** attrs);

protected:
	virtual void start_element(const XML_Char * name,
		const XML_Char ** attrs);
	virtual void end_element(const XML_Char * name);

public:
	ILaunchConfigParser(ILaunchConfig & config);
};

////////////////////////////////////////////////////////////////////////////////

ILaunchConfig::ILaunchConfig()
: m_loaded(false)
{
}

string ILaunchConfig::escape_attribute(const string & key)
{
	string esc;

	for(string::const_iterator i = key.begin(); i != key.end(); i++)
		switch(*i)
		{
		case '<':
			esc += "&lt;";
			break;
		case '&':
			esc += "&amp;";
			break;
		case '"':
			esc += "&quot;";
			break;
		case '\'':
			esc += "&apos;";
			break;
		default:
			if(isprint(*i))		// Strip unprintable characters
				esc += *i;
			break;
		}
	return esc;
}

string ILaunchConfig::crypt_pw(const string & str)
{
//	char table[]={32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,78,79,80,81,82,83,84,85,86,87,88,89,90,65,66,67,68,69,70,71,72,73,74,75,76,77,91,92,93,94,95,96,110,111,112,113,114,115,116,117,118,119,120,121,122,97,98,99,100,101,102,103,104,105,106,107,108,109,123,124,125,126};
	char *rot=new char[str.length()+1];
	for(unsigned int i = 0; i<str.length(); i++)
	{
		int c = str[i];
		c += 13;
		if (c > 126) c -= 95;
		if(c==32) c = 127;
		rot[i] = c;
	}
	rot[str.length()] = 0;
	string S(rot);
	delete rot;
	return S;
}

// private
void ILaunchConfig::find_client_dir()
{
	HKEY key;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			"Software\\Origin Worlds Online\\Ultima Online\\1.0",
			0, KEY_READ, &key) != ERROR_SUCCESS)
	{
		OutputDebugString("RegOpenKeyEx failed");
		return;
	}
	char exe_path[MAX_PATH + 1];
	DWORD type, path_len = sizeof(exe_path);
	if(RegQueryValueEx(key, "ExePath", NULL, &type,
			reinterpret_cast<LPBYTE>(exe_path), &path_len)
			!= ERROR_SUCCESS)
	{
		OutputDebugString("RegQueryKeyEx failed");
		return;
	}
	if(type != REG_SZ)
	{
		OutputDebugString("ExePath value not a string");
		RegCloseKey(key);
		return;
	}
	// The client directory is the exepath up to the last backslash.
	string str(exe_path);
	string::size_type i = str.find_last_of('\\');
	if(i == string::npos)
	{
		OutputDebugString("ExePath value contains no backslash");
		RegCloseKey(key);
		return;
	}
	m_client_dir.assign(str.begin(), i);
	RegCloseKey(key);
}

#if 0	// not used
// private
void ILaunchConfig::find_ignition_cfg()
{
	char mypath[MAX_PATH + 1];

	if(GetModuleFileName(NULL, mypath, sizeof(mypath)) == 0)
	{
		OutputDebugString("GetModuleFileName failed");
		return;
	}
	// Look for Ignition.cfg in the directory containing ilaunch.exe
	string str(mypath);
	string::size_type i = str.find_last_of('\\');
	if(i == string::npos)
	{
		OutputDebugString("My module filename contains no backslash");
		return;
	}
	m_ignition_cfg.assign(str.begin(), i);
	m_ignition_cfg += "\\Ignition.cfg";
}
#endif

void ILaunchConfig::load(const char * filename) throw(ILaunchConfigException)
{
	// Defaults in case they aren't in the config file, or it isn't found
	m_servers.erase(m_servers.begin(), m_servers.end());
	m_last_server = 0;
	m_clients.erase(m_clients.begin(), m_clients.end());
	m_last_client = 0;
	m_client_dir = "C:\\Program Files\\Ultima Online";
	char cwd[MAX_PATH + 1];
	DWORD r = GetCurrentDirectory(sizeof(cwd), cwd);
	if(r == 0 || r > sizeof(cwd))
		m_ignition_cfg = "Ignition.cfg";
	else
	{
		m_ignition_cfg = cwd;
		m_ignition_cfg += "\\Ignition.cfg";
	}
	m_use_injection = true;
	m_close = true;
	find_client_dir();

	m_filename = filename;
	FILE * fp = fopen(filename, "rt");
	if(fp == NULL)
	{
		// If the file just doesn't exist, its not an error because
		// defaults will be used and the file will be created later.
		if(errno == ENOENT)
		{
			add_server("Zulu Australia", "203.109.172.21,2593", "", "");
			add_server("Local", "127.0.0.1,2593", "", "");
			add_client("Default", "e:\\games\\uo\\client.exe");
			//trace_printf("Configuration file not found.\n");
			m_loaded = true;
			return;
		}
		// Any other errno is fatal
		throw ILaunchConfigException(
			string("Cannot open configuration file: ") + filename, errno);
	}
	ILaunchConfigParser parser(*this);
	bool success = parser.parse_file(fp);
	fclose(fp);
	if(!success)
	{
		char buf[200];
		sprintf(buf, "Error loading configuration file: line %d: %s",
			parser.get_error_line(), parser.get_error_msg());
		throw ILaunchConfigException(buf);
	}
	else
	{
		//trace_printf("Loaded config file: %s\n", filename);
		m_loaded = true;
	}
}

void ILaunchConfig::save() const throw(ILaunchConfigException)
{
	if(!m_loaded)
	{
		OutputDebugString("Not saving configuration.\n");
		return;
	}
	FILE * fp = fopen(m_filename.c_str(), "wt");
	if(fp == NULL)
		throw ILaunchConfigException(
			string("Cannot create config file: " + m_filename), errno);

	fprintf(fp, "<?xml version='1.0'?>\n\n");
	fprintf(fp, "<config\n");
	fprintf(fp, "\t\tlast_server=\"%d\"\n", m_last_server);
	fprintf(fp, "\t\tlast_client=\"%d\"\n", m_last_client);
	fprintf(fp, "\t\tclient_dir=\"%s\"\n",
		escape_attribute(m_client_dir).c_str());
	fprintf(fp, "\t\tignition_cfg=\"%s\"\n",
		escape_attribute(m_ignition_cfg).c_str());
	fprintf(fp, "\t\tuse_injection=\"%s\"\n",
		m_use_injection ? "true" : "false");
	fprintf(fp, "\t\tclose=\"%s\"\n",
		m_close ? "true" : "false");
	fprintf(fp, "\t\t>\n\n");
	for(serverlist_t::const_iterator i = m_servers.begin();
			i != m_servers.end(); i++)
	{
		fprintf(fp, "\t<server name=\"%s\" address=\"%s\" username=\"%s\" password=\"%s\"/>\n",
			ILaunchConfig::escape_attribute(i->m_name).c_str(),
			ILaunchConfig::escape_attribute(i->m_address).c_str(),
			ILaunchConfig::escape_attribute(i->m_username).c_str(),
			ILaunchConfig::escape_attribute(i->m_password).c_str());

	}
	{for(clientlist_t::const_iterator i = m_clients.begin();
			i != m_clients.end(); i++)
	{
		fprintf(fp, "\t<client name=\"%s\" path=\"%s\"/>\n",
			ILaunchConfig::escape_attribute(i->m_name).c_str(),
			ILaunchConfig::escape_attribute(i->m_path).c_str());
	}}
	fprintf(fp, "</config>\n\n");
	fclose(fp);
}

void ILaunchConfig::add_server(const char * name, const char * address,
								const char * username, const char * password)
{
	m_servers.push_back(ServerItem(name, address, username, password));
}

void ILaunchConfig::add_client(const char * name, const char * path)
{
	m_clients.push_back(ClientItem(name, path));
}

////////////////////////////////////////////////////////////////////////////////

// Methods of ILaunchConfigParser:

bool string_to_bool(const char * s, bool & b)
{
	if(strcmp(s, "true") == 0)
		b = true;
	else if(strcmp(s, "false") == 0)
		b = false;
	else
		return false;
	return true;
}

bool string_to_int(const char * s, int & n)
{
	char * end;
	// If the string starts with "0x", treat it as hex.
	if(s[0] == '0' && s[1] != '\0' && s[1] == 'x')
		n = strtol(s + 2, &end, 16);
	else
		n = strtol(s, &end, 10);
	return end == s + strlen(s);
}

ILaunchConfigParser::ILaunchConfigParser(ILaunchConfig & config)
: m_config(config), m_level(TOP)
{
}

void ILaunchConfigParser::start_element(const XML_Char * name,
	const XML_Char ** attrs)
{
	switch(m_level)
	{
	case TOP:
		if(strcmp(name, "config") == 0)
		{
			m_level = CONFIG;
			begin_config(attrs);
		}
		break;
	case CONFIG:
		if(strcmp(name, "server") == 0)
		{
			m_level = SERVER;
			begin_server(attrs);
		}
		else if(strcmp(name, "client") == 0)
		{
			m_level = CLIENT;
			begin_client(attrs);
		}
		break;
	case SERVER:
		break;
	case CLIENT:
		break;
	}
}

void ILaunchConfigParser::end_element(const XML_Char * name)
{
	switch(m_level)
	{
	case TOP:
		break;
	case CONFIG:
		if(strcmp(name, "config"))
			m_level = TOP;
		break;
	case SERVER:
		if(strcmp(name, "server") == 0)
			m_level = CONFIG;
		break;
	case CLIENT:
		if(strcmp(name, "client") == 0)
			m_level = CONFIG;
		break;
	}
}

// private
void ILaunchConfigParser::begin_config(const XML_Char ** attrs)
{
	while(*attrs != NULL)
	{
		const XML_Char * key = *attrs;
		const XML_Char * value = *(attrs + 1);

		if(strcmp(key, "last_server") == 0)
		{
			int i;
			if(string_to_int(value, i))
				m_config.set_last_server(i);
		}
		else if(strcmp(key, "last_client") == 0)
		{
			int i;
			if(string_to_int(value, i))
				m_config.set_last_client(i);
		}
		else if(strcmp(key, "ignition_cfg") == 0)
			m_config.set_ignition_cfg(value);
		else if(strcmp(key, "client_dir") == 0)
			m_config.set_client_dir(value);
		else if(strcmp(key, "use_injection") == 0)
		{
			bool b;
			if(string_to_bool(value, b))
				m_config.set_use_injection(b);
			//else
			//	warning_printf("Invalid boolean: use_injection\n");
		}
		else if(strcmp(key, "close") == 0)
		{
			bool b;
			if(string_to_bool(value, b))
				m_config.set_close(b);
		}
		//else
		//	warning_printf("config attribute ignored: %s\n", key);
		attrs += 2;
	}
}

// private
void ILaunchConfigParser::begin_server(const XML_Char ** attrs)
{
	const XML_Char * name = 0, * address = 0, * username = 0, * password = 0;

	while(*attrs != NULL)
	{
		const XML_Char * key = *attrs;
		const XML_Char * value = *(attrs + 1);

		if(strcmp(key, "name") == 0)
			name = value;
		else if(strcmp(key, "address") == 0)
			address = value;
		else if(strcmp(key, "username") == 0)
			username = value;
		else if(strcmp(key, "password") == 0)
			password = value;
		//else
		//	warning_printf("server attribute ignored: %s\n", key);
		attrs += 2;
	}
	if(name != 0 && address != 0){
		if(username == 0) username = "";
		if(password == 0) password = "";
		m_config.add_server(name, address, username, password);
	}
	//else
	//	error_printf("config file: server without name/address\n");
}

// private
void ILaunchConfigParser::begin_client(const XML_Char ** attrs)
{
	const XML_Char * name = 0, * path = 0;

	while(*attrs != NULL)
	{
		const XML_Char * key = *attrs;
		const XML_Char * value = *(attrs + 1);

		if(strcmp(key, "name") == 0)
			name = value;
		else if(strcmp(key, "path") == 0)
			path = value;
		//else
		//	warning_printf("client attribute ignored: %s\n", key);
		attrs += 2;
	}

	if(name != 0 && path != 0)
		m_config.add_client(name, path);
	//else
	//	error_printf("config file: client without name/path\n");
}


