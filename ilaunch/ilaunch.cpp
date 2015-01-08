////////////////////////////////////////////////////////////////////////////////
//
// ilaunch.cpp
//
// Copyright (C) 2001 Luke 'Infidel' Dunstan
//
// Parts based on Ignition:
// Copyright (C) 2000 Bruno 'Beosil' Heidelberger
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
//	This file is the main module of ILauncher (Injection Launcher)
//
////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <stdio.h>

#include <windows.h>
#include <COMMDLG.H>
#include <process.h>	// for beginthreadex, etc.

// These headers reside in the Injection source directory:
#include <gui.h>
#include <patch.h>

// Since we are not using any OLE interfaces, there is no need to compile
// with -fvtable-thunks. The Makefile #defines CINTERFACE to prevent warnings.
#include <shlobj.h>		// for SHBrowseForFolder()

WINOLEAPI  CoInitializeEx(IN LPVOID pvReserved, IN DWORD dwCoInit);

#include "resource.h"
#include "crc.h"
#include "ilconfig.h"

////////////////////////////////////////////////////////////////////////////////

class EditServersDialog : public ModalDialog
{
private:
	typedef ILaunchConfig::serverlist_t serverlist_t;

	serverlist_t m_servers;
	ListBoxWrapper * m_list_box;

	void selection_changed();

protected:
	virtual void event_init_dialog();
	virtual BOOL event_command(int control_id, int notify_code);
	virtual void event_destroy();

public:
	EditServersDialog(const serverlist_t & servers);
	virtual ~EditServersDialog();

	const serverlist_t & get_list() const { return m_servers; }
};

EditServersDialog::EditServersDialog(const serverlist_t & servers)
: ModalDialog(ID_DLG_SERVERS), m_servers(servers), m_list_box(0)
{
}

EditServersDialog::~EditServersDialog()
{
	delete m_list_box;
}

void EditServersDialog::event_init_dialog()
{
	m_list_box = new ListBoxWrapper(m_hwnd, ID_LB_SERVERS);
	for(serverlist_t::iterator i = m_servers.begin();
			i != m_servers.end(); i++)
		m_list_box->add_string(i->m_name.c_str());

	EnableWindow(GetDlgItem(m_hwnd, ID_BT_DELETESERVER), FALSE);
	EnableWindow(GetDlgItem(m_hwnd, ID_BT_SAVESERVER), FALSE);
}

void EditServersDialog::selection_changed()
{
	// When a list item is single clicked, update the fields.
	int i = m_list_box->get_current_selection();
	if(i >= 0 && static_cast<size_t>(i) < m_servers.size())
	{
		SetDlgItemText(m_hwnd, ID_EB_SERVERNAME,
			m_servers[i].m_name.c_str());
		SetDlgItemText(m_hwnd, ID_EB_SERVERADDRESS,
			m_servers[i].m_address.c_str());
		SetDlgItemText(m_hwnd, ID_EB_SERVERUSERNAME,
			m_servers[i].m_username.c_str());
		SetDlgItemText(m_hwnd, ID_EB_SERVERPASSWORD,
			m_servers[i].m_password.c_str());
		EnableWindow(GetDlgItem(m_hwnd, ID_BT_DELETESERVER), TRUE);
		EnableWindow(GetDlgItem(m_hwnd, ID_BT_SAVESERVER), TRUE);
	}
}

BOOL EditServersDialog::event_command(int control_id, int notify_code)
{
	if(control_id == IDCANCEL)
	{
		end(0);
		return TRUE;
	}
	else if(control_id == IDOK)
	{
		end(1);
		return TRUE;
	}
	else if(notify_code == LBN_SELCHANGE && control_id == ID_LB_SERVERS)
		selection_changed();
	else if(notify_code == BN_CLICKED)
		switch(control_id)
		{
		case ID_BT_DELETESERVER:
		{
			int i = m_list_box->get_current_selection();
			if(i >= 0 && static_cast<size_t>(i) < m_servers.size())
			{
				m_servers.erase(m_servers.begin() + i);
				m_list_box->delete_string(i);
				// There is no longer a selection
				EnableWindow(GetDlgItem(m_hwnd, ID_BT_DELETESERVER), FALSE);
				EnableWindow(GetDlgItem(m_hwnd, ID_BT_SAVESERVER), FALSE);
			}
			return TRUE;
		}
		case ID_BT_SAVESERVER:
		{
			int i = m_list_box->get_current_selection();
			if(i >= 0 && static_cast<size_t>(i) < m_servers.size())
			{
				EditBoxWrapper name_box(m_hwnd, ID_EB_SERVERNAME),
					addr_box(m_hwnd, ID_EB_SERVERADDRESS),
					uname_box(m_hwnd, ID_EB_SERVERUSERNAME),
					passwd_box(m_hwnd, ID_EB_SERVERPASSWORD);				;
				string name;
				name_box.get_text(name);
				m_servers[i].m_name = name;
				addr_box.get_text(m_servers[i].m_address);
				uname_box.get_text(m_servers[i].m_username);
				passwd_box.get_text(m_servers[i].m_password);
				// Name may change, so update the list box
				m_list_box->set_string(i, name.c_str());
				// set_string() deselects, so select it again.
				m_list_box->set_current_selection(i);
			}
			return TRUE;
		}
		case ID_BT_ADDSERVER:
		{
			EditBoxWrapper name_box(m_hwnd, ID_EB_SERVERNAME),
				addr_box(m_hwnd, ID_EB_SERVERADDRESS),
				uname_box(m_hwnd, ID_EB_SERVERUSERNAME),
				passwd_box(m_hwnd, ID_EB_SERVERPASSWORD);				;
			string name, address, username, password;
			name_box.get_text(name);
			addr_box.get_text(address);
			uname_box.get_text(username);
			passwd_box.get_text(password);
			if(name.length() == 0 || address.length() == 0)
				MessageBox(m_hwnd, "You must enter a name and address",
					"Error", MB_OK | MB_ICONINFORMATION);
			else
			{
				m_servers.push_back(ServerItem(name.c_str(), address.c_str(), username.c_str(), password.c_str()));
				int i = m_list_box->add_string(name.c_str());
				if(i != -1)
				{
					m_list_box->set_current_selection(i);
					selection_changed();
				}
			}
			return TRUE;
		}
		}
	return FALSE;
}

void EditServersDialog::event_destroy()
{
	delete m_list_box;
	m_list_box = 0;
}

////////////////////////////////////////////////////////////////////////////////

class EditClientsDialog : public ModalDialog
{
private:
	typedef ILaunchConfig::clientlist_t clientlist_t;

	clientlist_t m_clients;
	ListBoxWrapper * m_list_box;
	string m_path;

	void choose_path();
	void selection_changed();

protected:
	virtual void event_init_dialog();
	virtual BOOL event_command(int control_id, int notify_code);
	virtual void event_destroy();

public:
	EditClientsDialog(const clientlist_t & clients);
	virtual ~EditClientsDialog();

	const clientlist_t & get_list() const { return m_clients; }
};

EditClientsDialog::EditClientsDialog(const clientlist_t & clients)
: ModalDialog(ID_DLG_CLIENTS), m_clients(clients), m_list_box(0)
{
}

EditClientsDialog::~EditClientsDialog()
{
	delete m_list_box;
}

// private
void EditClientsDialog::choose_path()
{
	char path[MAX_PATH + 1];
	strcpy(path, m_path.c_str());
	OPENFILENAME ofn;
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hwnd;
	ofn.lpstrFilter =
		"Executable Files (*.exe)\0*.exe\0All Files (*.*)\0*.*\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = path;
	ofn.nMaxFile = sizeof(path);
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = "Select File";
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR |
		OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = NULL;
	if(GetOpenFileName(&ofn) != 0)
	{
		m_path = path;
		SetDlgItemText(m_hwnd, ID_ST_CLIENTPATH, path);
	}
}

void EditClientsDialog::event_init_dialog()
{
	m_list_box = new ListBoxWrapper(m_hwnd, ID_LB_CLIENTS);
	for(clientlist_t::iterator i = m_clients.begin();
			i != m_clients.end(); i++)
		m_list_box->add_string(i->m_name.c_str());

	EnableWindow(GetDlgItem(m_hwnd, ID_BT_DELETECLIENT), FALSE);
	EnableWindow(GetDlgItem(m_hwnd, ID_BT_SAVECLIENT), FALSE);
}

void EditClientsDialog::selection_changed()
{
	// When a list item is single clicked, update the fields.
	int i = m_list_box->get_current_selection();
	if(i >= 0 && static_cast<size_t>(i) < m_clients.size())
	{
		SetDlgItemText(m_hwnd, ID_EB_CLIENTNAME,
			m_clients[i].m_name.c_str());
		m_path = m_clients[i].m_path;
		SetDlgItemText(m_hwnd, ID_ST_CLIENTPATH, m_path.c_str());
		EnableWindow(GetDlgItem(m_hwnd, ID_BT_DELETECLIENT), TRUE);
		EnableWindow(GetDlgItem(m_hwnd, ID_BT_SAVECLIENT), TRUE);
	}
}

BOOL EditClientsDialog::event_command(int control_id, int notify_code)
{
	if(control_id == IDCANCEL)
	{
		end(0);
		return TRUE;
	}
	else if(control_id == IDOK)
	{
		end(1);
		return TRUE;
	}
	else if(notify_code == LBN_SELCHANGE && control_id == ID_LB_CLIENTS)
		selection_changed();
	else if(notify_code == BN_CLICKED)
		switch(control_id)
		{
		case ID_BT_DELETECLIENT:
		{
			int i = m_list_box->get_current_selection();
			if(i >= 0 && static_cast<size_t>(i) < m_clients.size())
			{
				m_clients.erase(m_clients.begin() + i);
				m_list_box->delete_string(i);
				// There is no longer a selection
				EnableWindow(GetDlgItem(m_hwnd, ID_BT_DELETECLIENT), FALSE);
				EnableWindow(GetDlgItem(m_hwnd, ID_BT_SAVECLIENT), FALSE);
			}
			return TRUE;
		}
		case ID_BT_SAVECLIENT:
		{
			int i = m_list_box->get_current_selection();
			if(i >= 0 && static_cast<size_t>(i) < m_clients.size())
			{
				EditBoxWrapper name_box(m_hwnd, ID_EB_CLIENTNAME);
				string name;
				name_box.get_text(name);
				m_clients[i].m_name = name;
				m_clients[i].m_path = m_path;
				// Name may change, so update the list box
				m_list_box->set_string(i, name.c_str());
				// set_string() deselects, so select it again.
				m_list_box->set_current_selection(i);
			}
			return TRUE;
		}
		case ID_BT_ADDCLIENT:
		{
			EditBoxWrapper name_box(m_hwnd, ID_EB_CLIENTNAME);
			string name;
			name_box.get_text(name);
			if(name.length() == 0 || m_path.length() == 0)
				MessageBox(m_hwnd, "You must enter a name and select a path",
					"Error", MB_OK | MB_ICONINFORMATION);
			else
			{
				m_clients.push_back(ClientItem(name.c_str(), m_path.c_str()));
				int i = m_list_box->add_string(name.c_str());
				if(i != -1)
				{
					m_list_box->set_current_selection(i);
					selection_changed();
				}
			}
			return TRUE;
		}
		case ID_BT_CHOOSEPATH:
			choose_path();
			return TRUE;
		}
	return FALSE;
}

void EditClientsDialog::event_destroy()
{
	delete m_list_box;
	m_list_box = 0;
}

////////////////////////////////////////////////////////////////////////////////

class Thread
{
private:
	unsigned int m_thread_id;
	HANDLE m_thread;

	static unsigned __stdcall thread_function(void * arg);

protected:
	virtual unsigned run() = 0;

public:
	Thread();
	virtual ~Thread();

	// Returns false for failure.
	bool start();
	// Wait for the thread to exit:
	void wait();
	bool is_running();
};

Thread::Thread()
: m_thread_id(0), m_thread(0)
{
}

Thread::~Thread()
{
	if(m_thread != 0)
		CloseHandle(m_thread);
}

unsigned __stdcall Thread::thread_function(void * arg)
{
	Thread * obj = reinterpret_cast<Thread *>(arg);
	assert(obj != 0);
	return obj->run();
}

bool Thread::start()
{
	m_thread = reinterpret_cast<HANDLE>(
		_beginthreadex(NULL, 0, thread_function, this, 0, &m_thread_id));
	return m_thread != 0;
}

void Thread::wait()
{
	WaitForSingleObject(m_thread, 10000);
}

bool Thread::is_running()
{
	if(m_thread == 0)
		return false;
	DWORD code;
	return GetExitCodeThread(m_thread, &code) == STILL_ACTIVE;
}

////////////////////////////////////////////////////////////////////////////////

const int NUM_STAGES = 7;

struct Stage
{
	int m_id1, m_id2;
};

Stage g_stages[NUM_STAGES] =
{
	{ ID_ST_CALCULATINGCRC, ID_ST_DONE4 },
	{ ID_ST_LOADINGIGNITIONCFG, ID_ST_DONE5 },
	{ ID_ST_LOADINGILPATCHCFG, ID_ST_DONE6 },
	{ ID_ST_SAVINGLOGINCFG, ID_ST_DONE7 },
	{ ID_ST_LOADINGINJECTION, ID_ST_DONE1 },
	{ ID_ST_STARTINGCLIENT, ID_ST_DONE2 },
	{ ID_ST_PATCHINGCLIENT, ID_ST_DONE3 }
};

const int WM_LAUNCH_BEGINSTAGE = WM_USER + 1;
const int WM_LAUNCH_DONESTAGE = WM_USER + 2;
const int WM_LAUNCH_ERRORSTAGE = WM_USER + 3;
const int WM_LAUNCH_SKIPPEDSTAGE = WM_USER + 4;
const int FIRST_WM_LAUNCH = WM_USER + 1;
const int LAST_WM_LAUNCH = WM_USER + 4;

const int STAGE_CRC = 0;
const int STAGE_IGNITIONCFG = 1;
const int STAGE_ILPATCHCFG = 2;
const int STAGE_LOGINCFG = 3;
const int STAGE_LOADINJECTION = 4;
const int STAGE_STARTCLIENT = 5;
const int STAGE_PATCHCLIENT = 6;

typedef bool (*patch_setup_t)(unsigned long client_thread_id,
	const Patch & patch, bool use_injection);
typedef const char * (*patch_getmessage_t)();

class LaunchThread : public Thread
{
private:
	HWND m_dialog;
	const ILaunchConfig & m_config;
	const char * m_address, * m_username, * m_password, * m_client_path;
	char m_message_buf[200];
	const char * m_message;

	void begin_stage(int i)
	{
		PostMessage(m_dialog, WM_LAUNCH_BEGINSTAGE, i, 0);
	}
	void done_stage(int i)
	{
		PostMessage(m_dialog, WM_LAUNCH_DONESTAGE, i, 0);
	}
	void error_stage(int i)
	{
		PostMessage(m_dialog, WM_LAUNCH_ERRORSTAGE, i, 0);
	}
	void skipped_stage(int i)
	{
		PostMessage(m_dialog, WM_LAUNCH_SKIPPEDSTAGE, i, 0);
	}

	void crtl_error(const char * message);
	void api_error(const char * message);

protected:
	virtual unsigned run();

public:
	LaunchThread(HWND dialog, const ILaunchConfig & config,
		const char * address, const char * username, const char * password, const char * client_path)
	: m_dialog(dialog), m_config(config), m_address(address),
	  m_username(username), m_password(password),
	  m_client_path(client_path), m_message("No error")
	{
	}

	const char * get_message() const { return m_message; }
};

void LaunchThread::api_error(const char * message)
{
	int err = GetLastError();
	LPVOID msg_buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPTSTR>(&msg_buf), 0, NULL);
	wsprintf(m_message_buf, "%s\nReason: (%d)\n%s", message, err, msg_buf);
	LocalFree(msg_buf);
	m_message = m_message_buf;
}

void LaunchThread::crtl_error(const char * message)
{
	wsprintf(m_message_buf, "%s\nReason: (%d)\n%s",
		message, errno, strerror(errno));
	m_message = m_message_buf;
}

/*
	Thread actions:
	- Write login.cfg to client directory
	- Start the client executable with CreateProcess()
	- WaitForInputIdle()
	- call patch_setup() in Injection DLL
	- Post a message to the client thread (param: this thread id)
	- Wait for a message with GetMessage()
*/
unsigned LaunchThread::run()
{
	// Calculate CRC of client executable.
	unsigned int checksum, length;
	CCrc crc;
	begin_stage(STAGE_CRC);
	if(!crc.GetCrc(m_client_path, &checksum, &length))
	{
		crtl_error("Cannot read client executable");
		error_stage(STAGE_CRC);
		return 0;
	}
	done_stage(STAGE_CRC);

	// Load Ignition.cfg
	Patch patch;
	// Lookup CRC to find patch information.
	// First try Ignition.cfg
	begin_stage(STAGE_IGNITIONCFG);
	if(!patch.load(m_config.get_ignition_cfg(), checksum, length))
	{
		crtl_error("Cannot get patch from Ignition.cfg");
		//error_stage(STAGE_IGNITIONCFG);
		// If not found, also try ilpatch.cfg
		begin_stage(STAGE_ILPATCHCFG);
		if(!patch.load("ilpatch.cfg", checksum, length))
		{
			sprintf(m_message_buf, "This client verion is not supported.\n"
				"(No patch information for selected client.)\n"
				"Checksum: %08x  Length: %x", checksum, length);
			m_message = m_message_buf;
			error_stage(STAGE_ILPATCHCFG);
			return 0;
		}
		else
			done_stage(STAGE_ILPATCHCFG);
	}
	else
	{
		done_stage(STAGE_IGNITIONCFG);
		skipped_stage(STAGE_ILPATCHCFG);
	}

	// Write login.cfg to client directory.
	string login_cfg(m_config.get_client_dir());
	login_cfg += "\\login.cfg";
	begin_stage(STAGE_LOGINCFG);
	FILE * fp = fopen(login_cfg.c_str(), "wt");
	if(fp == NULL)
	{
		crtl_error("Cannot write login.cfg");
		error_stage(STAGE_LOGINCFG);
		return 0;
	}
	fprintf(fp, "; login.cfg\n; created by Injection Launcher\n");
	fprintf(fp, "LoginServer=%s\n", m_address);
	fclose(fp);

	//patch uo.cfg with username and password for login
	if(strcmp(m_username, "") != 0) // no username so skip this
	{
		string RememberAcctPW("on");
		string AcctID(m_username);
		string AcctPassword(ILaunchConfig::crypt_pw(m_password));
		if(AcctPassword == "") RememberAcctPW = "off";

		string uo_cfg(m_config.get_client_dir());
		uo_cfg += "\\uo.cfg";
		FILE * ifp = fopen(uo_cfg.c_str(), "rt");
		if(ifp == NULL)
		{
			crtl_error("Cannot read uo.cfg");
			error_stage(STAGE_LOGINCFG);
			return 0;
		}
		string uo_cfg_tmp(uo_cfg);
		uo_cfg_tmp += ".tmp";
		FILE * ofp = fopen(uo_cfg_tmp.c_str(), "wt");
		if(ofp == NULL)
		{
			crtl_error("Cannot write uo.cfg.tmp");
			error_stage(STAGE_LOGINCFG);
			return 0;
		}
		char buf[100];
		while(fgets(buf, 100, ifp) != NULL)
		{
			// strip out our fields
			if(strncmp(buf,"AcctID",6) == 0) {}
			else if(strncmp(buf,"AcctPassword",12) == 0) {}
			else if(strncmp(buf,"RememberAcctPW",14) == 0) {}
			else fputs(buf, ofp);
		}

		// build new config strings
		string AcctID_str("AcctID=");
		AcctID_str += AcctID+"\n";
		string AcctPassword_str("AcctPassword=");
		AcctPassword_str += AcctPassword+"\n";
		string RememberAcctPW_str("RememberAcctPW=");
		RememberAcctPW_str += RememberAcctPW+"\n";

		// write our new config strings
		fputs(AcctID_str.c_str(), ofp);
		fputs(AcctPassword_str.c_str(), ofp);
		fputs(RememberAcctPW_str.c_str(), ofp);

		fclose(ofp);
		fclose(ifp);

		// delete orig uo.cfg
		remove(uo_cfg.c_str());
		// rename uo.cfg.tmp to uo.cfg
		rename(uo_cfg_tmp.c_str(), uo_cfg.c_str());
	}

	done_stage(STAGE_LOGINCFG);

	// Load injection.dll to patch client
	begin_stage(STAGE_LOADINJECTION);
	HMODULE hmodule = LoadLibrary("injection.dll");
	if(hmodule == 0)
	{
		api_error("Failed to load injection.dll in launcher");
		error_stage(STAGE_LOADINJECTION);
		return 0;
	}
	patch_setup_t patch_setup =
		reinterpret_cast<patch_setup_t>(GetProcAddress(hmodule,
			"patch_setup"));
	patch_getmessage_t patch_getmessage =
		reinterpret_cast<patch_getmessage_t>(GetProcAddress(hmodule,
			"patch_getmessage"));
	if(patch_setup == 0 || patch_getmessage == 0)
	{
		api_error("Failed to find function in injection.dll");
		error_stage(STAGE_LOADINJECTION);
		return 0;
	}
	done_stage(STAGE_LOADINJECTION);

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;

	// Run the client executable
	begin_stage(STAGE_STARTCLIENT);
	if(CreateProcess(m_client_path, NULL, NULL, NULL, FALSE, 0, NULL, NULL,
		&si, &pi) == 0)
	{
		api_error("Failed to execute client");
		error_stage(STAGE_STARTCLIENT);
		return 0;
	}
	if(WaitForInputIdle(pi.hProcess, INFINITE) != 0)
	{
		api_error("Failed waiting for client window to open");
		error_stage(STAGE_STARTCLIENT);
		return 0;
	}
	done_stage(STAGE_STARTCLIENT);

	begin_stage(STAGE_PATCHCLIENT);
	if(!patch_setup(pi.dwThreadId, patch, m_config.get_use_injection()))
	{
		m_message = patch_getmessage();
		error_stage(STAGE_PATCHCLIENT);
		return 0;
	}

	// Post a message to the client thread
	if(PostThreadMessage(pi.dwThreadId, WM_USER, GetCurrentThreadId(), 0) == 0)
	{
		api_error("Failed posting message to client");
		error_stage(STAGE_PATCHCLIENT);
		return 0;
	}
	MSG msg;
	// Wait for a message back from the client thread
	if(GetMessage(&msg, NULL, WM_USER, WM_USER) == -1)
	{
		api_error("Failed waiting for message from client");
		error_stage(STAGE_PATCHCLIENT);
		return 0;
	}
	if(msg.wParam != 0)
	{
		TerminateProcess(pi.hProcess, 0xffffffff);
		m_message = patch_getmessage();
		error_stage(STAGE_PATCHCLIENT);
		return 0;
	}
	m_message = "Success";
	done_stage(STAGE_PATCHCLIENT);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return 1;
}

////////////////////////////////////////////////////////////////////////////////

class LaunchingDialog : public ModalDialog
{
private:
	const ILaunchConfig * m_config;
	const char * m_address, * m_username, * m_password, * m_client_path;
	LaunchThread * m_thread;
	const char * m_message;

	void show_control(int id);

protected:
	virtual void event_init_dialog();
	virtual BOOL event_command(int control_id, int notify_code);
	virtual void event_user(int msg, WPARAM wparam, LPARAM lparam);

public:
	LaunchingDialog();
	virtual ~LaunchingDialog();

	// Returns true if the program should be closed.
	bool open(HWND parent, const ILaunchConfig & config,
		const char * address, const char * username, const char * password, const char * client_path);
};

LaunchingDialog::LaunchingDialog()
: ModalDialog(ID_DLG_LAUNCHING),
  m_address(0), m_username(0), m_password(0), m_client_path(0),
  m_thread(0), m_message("No error")
{
}

LaunchingDialog::~LaunchingDialog()
{
	delete m_thread;
}

// private
void LaunchingDialog::show_control(int id)
{
	ShowWindow(GetDlgItem(m_hwnd, id), TRUE);
}

void LaunchingDialog::event_init_dialog()
{
	assert(m_thread == 0);
	// Start a new thread
	m_thread = new LaunchThread(m_hwnd, *m_config, m_address, m_username, m_password,
		m_client_path);
	if(!m_thread->start())
	{
		delete m_thread;
		m_thread = 0;
		char buf[300];
		wsprintf(buf, "Cannot create thread\nReason: (%d)\n%s",
			errno, strerror(errno));
		MessageBox(m_hwnd, buf, "Error", MB_OK | MB_ICONSTOP);
		// Close the dialog
		end(0);
	}
}

BOOL LaunchingDialog::event_command(int control_id, int notify_code)
{
	if(control_id == IDCANCEL)
	{
		if(m_thread == 0)
			end(0);
		else if(!m_thread->is_running())
		{
			m_thread->wait();
			delete m_thread;
			m_thread = 0;
			end(0);
		}
		else
			MessageBeep(MB_ICONASTERISK);
		return TRUE;
	}
	else if(notify_code == BN_CLICKED && control_id == ID_BT_DETAILS)
	{
		MessageBox(m_hwnd, m_message, "Error Details", MB_OK);
		// TODO: Yes/No prompt to display Injection log file?
	}
	return FALSE;
}

void LaunchingDialog::event_user(int msg, WPARAM wparam, LPARAM /*lparam*/)
{
	if(msg < FIRST_WM_LAUNCH || msg > LAST_WM_LAUNCH)
		return;
	int stage = wparam;
	assert(stage >= 0 && stage < NUM_STAGES);
	switch(msg)
	{
	case WM_LAUNCH_BEGINSTAGE:
		show_control(g_stages[stage].m_id1);
		break;
	case WM_LAUNCH_DONESTAGE:
		SetDlgItemText(m_hwnd, g_stages[stage].m_id2, "Done");
		if(stage == NUM_STAGES - 1)
		{
			m_thread->wait();
			delete m_thread;
			m_thread = 0;
			end(1);
		}
		break;
	case WM_LAUNCH_ERRORSTAGE:
		SetDlgItemText(m_hwnd, g_stages[stage].m_id2, "Error");
		m_message = m_thread->get_message();
		EnableWindow(GetDlgItem(m_hwnd, ID_BT_DETAILS), TRUE);
		MessageBeep(MB_ICONHAND);

		m_thread->wait();
		delete m_thread;
		m_thread = 0;
		break;
	case WM_LAUNCH_SKIPPEDSTAGE:
		// Do nothing.
		//SetDlgItemText(m_hwnd, g_stages[stage].m_id2, "Skipped");
		break;
	}
}

bool LaunchingDialog::open(HWND parent, const ILaunchConfig & config,
	const char * address, const char * username, const char * password, const char * client_path)
{
	m_config = &config;
	m_address = address;
	m_username = username;
	m_password = password;
	m_client_path = client_path;
	return ModalDialog::open(parent) != 0;
}

////////////////////////////////////////////////////////////////////////////////

class ILaunchDialog : public ModelessDialog
{
private:
	ILaunchConfig m_config;
	LaunchingDialog m_launching_dialog;
	string m_address, m_username, m_password, m_client_path;

	void do_launch();
	void choose_client_dir();
	void choose_ignition_cfg();
	void update_servers_box();
	void update_clients_box();

protected:
	virtual void event_init_dialog();
	virtual BOOL event_command(int control_id, int notify_code);
	virtual void event_destroy();

public:
	ILaunchDialog();

	void api_error(const char * message);
	void crtl_error(const char * message);
};

ILaunchDialog::ILaunchDialog()
: ModelessDialog(ID_DLG_MAIN)
{
}

void ILaunchDialog::api_error(const char * message)
{
	int err = GetLastError();
	LPVOID msg_buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPTSTR>(&msg_buf), 0, NULL);
	char buf[200];
	wsprintf(buf, "%s\nReason: (%d)\n%s", message, err, msg_buf);
	MessageBox(m_hwnd, buf, "Error", MB_OK | MB_ICONEXCLAMATION);
	LocalFree(msg_buf);
}

void ILaunchDialog::crtl_error(const char * message)
{
	char buf[200];
	wsprintf(buf, "%s\nReason: (%d)\n%s", message, errno, strerror(errno));
	MessageBox(m_hwnd, buf, "Error", MB_OK | MB_ICONEXCLAMATION);
}

void ILaunchDialog::event_init_dialog()
{
	try
	{
		m_config.load("ilaunch.xml");
	}
	catch(const ILaunchConfigException & e)
	{
		MessageBox(m_hwnd, e.get_message().c_str(),
			"Error Loading Configuration", MB_OK | MB_ICONSTOP);
		destroy();
		return;
	}

	// Set the icon for the main window.
	HICON hicon = LoadIcon(g_hinstance, MAKEINTRESOURCE(ID_ICO_IGNITION));
	SendMessage(m_hwnd, WM_SETICON, ICON_SMALL,
		reinterpret_cast<LPARAM>(hicon));
	SendMessage(m_hwnd, WM_SETICON, ICON_BIG,
		reinterpret_cast<LPARAM>(hicon));

	// Show the client directory.
	SetDlgItemText(m_hwnd, ID_ST_CLIENTDIR, m_config.get_client_dir());
	SetDlgItemText(m_hwnd, ID_ST_IGNITIONCFG, m_config.get_ignition_cfg());
	// Initialise checkboxes.
	ButtonWrapper check_box(m_hwnd, ID_CH_INJECTION);
	check_box.set_check(m_config.get_use_injection());
	ButtonWrapper check_box2(m_hwnd, ID_CH_CLOSEWHENDONE);
	check_box2.set_check(m_config.get_close());

	// Fill combo boxes and activate last selections.
	ComboBoxWrapper scombo(m_hwnd, ID_CB_SERVER);
	update_servers_box();
	int i = m_config.get_last_server();
	if(i >= 0 && static_cast<size_t>(i) < m_config.get_servers().size())
	{
		scombo.set_current_selection(i);
		m_address = m_config.get_servers()[i].m_address;
		m_username = m_config.get_servers()[i].m_username;
		m_password = m_config.get_servers()[i].m_password;
	}
	else
	{
		m_address = "";
		m_username = "";
		m_password = "";
	}
	// Show the address
	SetDlgItemText(m_hwnd, ID_ST_ADDRESS, m_address.c_str());

	ComboBoxWrapper ccombo(m_hwnd, ID_CB_CLIENT);
	update_clients_box();
	i = m_config.get_last_client();
	if(i >= 0 && static_cast<size_t>(i) < m_config.get_clients().size())
	{
		ccombo.set_current_selection(i);
		m_client_path = m_config.get_clients()[i].m_path;
	}
	else
		m_client_path = "";
	// Show the executable path
	SetDlgItemText(m_hwnd, ID_ST_EXEPATH, m_client_path.c_str());
}

BOOL ILaunchDialog::event_command(int control_id, int notify_code)
{
	if(control_id == IDCANCEL)
	{
		destroy();
		return TRUE;
	}
	if(notify_code == BN_CLICKED)
	{
		switch(control_id)
		{
		case ID_BT_CLIENTDIR:
			choose_client_dir();
			return TRUE;
		case ID_BT_SERVERS:
		{
			EditServersDialog dialog(m_config.get_servers());
			// Returns zero if the dialog was cancelled.
			if(dialog.open(m_hwnd) != 0)
			{
				m_config.set_servers(dialog.get_list());
				update_servers_box();
			}
			return TRUE;
		}
		case ID_BT_CLIENTS:
		{
			EditClientsDialog dialog(m_config.get_clients());
			// Returns zero if the dialog was cancelled.
			if(dialog.open(m_hwnd) != 0)
			{
				m_config.set_clients(dialog.get_list());
				update_clients_box();
			}
			return TRUE;
		}
		case ID_BT_IGNITIONCFG:
			choose_ignition_cfg();
			return TRUE;
		case ID_CH_INJECTION:
		{
			ButtonWrapper check_box(m_hwnd, ID_CH_INJECTION);
			m_config.set_use_injection(check_box.get_check());
			return TRUE;
		}
		case ID_CH_CLOSEWHENDONE:
		{
			ButtonWrapper check_box(m_hwnd, ID_CH_CLOSEWHENDONE);
			m_config.set_close(check_box.get_check());
			return TRUE;
		}
		case ID_BT_LAUNCH:
		{
			ButtonWrapper check_box(m_hwnd, ID_CH_INJECTION);
			m_config.set_use_injection(check_box.get_check());
			// Open launch dialog.
			if(m_launching_dialog.open(m_hwnd, m_config, m_address.c_str(),
					m_username.c_str(), m_password.c_str(),
					m_client_path.c_str()) && m_config.get_close())
				// Close the program.
				destroy();
			return TRUE;
		}
		}
	}
	else if(notify_code == CBN_SELCHANGE)
	{
		if(control_id == ID_CB_SERVER)
		{
			ComboBoxWrapper combo(m_hwnd, ID_CB_SERVER);
			int i = combo.get_current_selection();

			if(i != -1)
			{
				ILaunchConfig::serverlist_t & servers=(m_config.get_servers());
				if(i >= 0 && static_cast<size_t>(i) < servers.size())
				{
					m_address = servers[i].m_address;
					m_username = servers[i].m_username;
					m_password = servers[i].m_password;
					m_config.set_last_server(i);
					SetDlgItemText(m_hwnd, ID_ST_ADDRESS, m_address.c_str());
				}
			}
			return TRUE;
		}
		else if(control_id == ID_CB_CLIENT)
		{
			ComboBoxWrapper combo(m_hwnd, ID_CB_CLIENT);
			int i = combo.get_current_selection();

			if(i != -1)
			{
				ILaunchConfig::clientlist_t & clients=(m_config.get_clients());
				if(i >= 0 && static_cast<size_t>(i) < clients.size())
				{
					m_client_path = clients[i].m_path;
					m_config.set_last_client(i);
					SetDlgItemText(m_hwnd, ID_ST_EXEPATH,
						m_client_path.c_str());
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}

void ILaunchDialog::event_destroy()
{
	try
	{
		m_config.save();
	}
	catch(const ILaunchConfigException & e)
	{
		MessageBox(m_hwnd, e.get_message().c_str(),
			"Error Saving Configuration", MB_OK | MB_ICONSTOP);
	}
	PostQuitMessage(0);
}

// private
void ILaunchDialog::choose_client_dir()
{
	char path[MAX_PATH + 1];
	BROWSEINFO bi;

	bi.hwndOwner = m_hwnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = path;	// This is just for display: not useful
	bi.lpszTitle = "Choose Client Directory";
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.lParam = 0;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if(pidl != NULL && SHGetPathFromIDList(pidl, path))
	{
		m_config.set_client_dir(path);
		SetDlgItemText(m_hwnd, ID_ST_CLIENTDIR, path);
	}
}

// private
void ILaunchDialog::choose_ignition_cfg()
{
	char path[MAX_PATH + 1];
	strcpy(path, m_config.get_ignition_cfg());
	OPENFILENAME ofn;
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hwnd;
	ofn.lpstrFilter =
		"Configuration Files (*.cfg)\0*.cfg\0All Files (*.*)\0*.*\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = path;
	ofn.nMaxFile = sizeof(path);
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = "Select File";
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR |
		OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = NULL;
	if(GetOpenFileName(&ofn) != 0)
	{
		m_config.set_ignition_cfg(path);
		SetDlgItemText(m_hwnd, ID_ST_IGNITIONCFG, path);
	}
}

// private
void ILaunchDialog::update_servers_box()
{
	// Clear and populate servers combo box
	ComboBoxWrapper scombo(m_hwnd, ID_CB_SERVER);
	scombo.clear();
	ILaunchConfig::serverlist_t & servers=(m_config.get_servers());
	for(ILaunchConfig::serverlist_t::iterator i = servers.begin();
			i != servers.end(); i++)
		scombo.add_string(i->m_name.c_str());
	scombo.set_current_selection(0);
}

// private
void ILaunchDialog::update_clients_box()
{
	// Clear and populate clients combo box
	ComboBoxWrapper ccombo(m_hwnd, ID_CB_CLIENT);
	ccombo.clear();
	ILaunchConfig::clientlist_t & clients=(m_config.get_clients());
	for(ILaunchConfig::clientlist_t::iterator i = clients.begin();
			i != clients.end(); i++)
		ccombo.add_string(i->m_name.c_str());
	ccombo.set_current_selection(0);
}

////////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE /*hprev_instance*/,
	char * /*command_line*/, int /*cmd_show*/)
{
	// needed for SHBrowseForFolder():
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	g_hinstance = hinstance;

	ILaunchDialog dialog;
	if(!dialog.create())
	{
		dialog.api_error("CreateDialog failed");
		g_hinstance = 0;
		return 1;
	}

	MSG msg;
	int status;
	while((status = GetMessage(&msg, 0, 0, 0)) > 0)
	{
		if(!IsDialogMessage(dialog.get_hwnd(), &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	g_hinstance = 0;
	CoUninitialize();
	return msg.wParam;
}

