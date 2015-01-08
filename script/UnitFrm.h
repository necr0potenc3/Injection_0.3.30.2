////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2001 mamaich
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

#ifndef UnitFrmH
#define UnitFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include "..\extdll.h"
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:    // IDE-managed Components
    TButton *Button1;
    TButton *Button2;
    TButton *Button3;
    TButton *Button5;
    TEdit *Edit1;
    TButton *Button4;
    TLabel *Label2;
    TListBox *RunList;
    TLabel *Label1;
    TOpenDialog *OD;
    TSaveDialog *SD;
    TLabel *Label3;
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall Button3Click(TObject *Sender);
    void __fastcall Button4Click(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall Button5Click(TObject *Sender);
    void __fastcall Button2Click(TObject *Sender);
private:    // User declarations
public:     // User declarations
    __fastcall TMainForm(TComponent* Owner);
    void __fastcall OnMessage(TMessage &Message);
#pragma warn -inl    
BEGIN_MESSAGE_MAP
  MESSAGE_HANDLER(WM_USER+0x1000, TMessage, OnMessage)
END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
typedef void __cdecl CharFunc(const char*);

extern DllInterface *UO;

extern HWND UOWindow;
extern "C" void __export __cdecl RunFunction(const char *Name);

#endif
