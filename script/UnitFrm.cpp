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

#include <vcl.h>
#pragma hdrstop

#include "UnitFrm.h"
#include "Editor.h"
#include "myparser.h"
#include "ScrRun.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm=0;
HWND UOWindow=0;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnMessage(TMessage &M)
{
    UO->DoCommand((char*)M.LParam);
}

void __fastcall TMainForm::Button1Click(TObject *Sender)
{
    if(OD->Execute())
    {
        EditForm->Script->Lines->Clear();
        EditForm->Script->Lines->LoadFromFile(OD->FileName);
        SD->FileName=OD->FileName;
        EditForm->StatusBar->Panels->Items[1]->Text=OD->FileName;
        UO->ClientPrint("Script loaded.");
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Button3Click(TObject *Sender)
{
    EditForm->Show();
    OpenIcon(EditForm->Handle);
}
//---------------------------------------------------------------------------
extern "C" void __export __cdecl RunFunction(const char *Name)
{
    TRunner *tmp=new TRunner(true,Name);
    MainForm->RunList->Items->Add(IntToHex((int)tmp,8)+" "+Name);
    tmp->Resume();
}

void __fastcall TMainForm::Button4Click(TObject *Sender)
{
    RunFunction(Edit1->Text.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
    EditForm=new TEditForm(this);

    HWND hwnd = FindWindowEx(0, 0, "Ultima Online", 0);
    while(hwnd != 0)
    {
        // Make sure the window is owned by the current thread
        DWORD threadid = GetWindowThreadProcessId(hwnd, NULL);
        if(threadid == GetCurrentThreadId())
        {
            if(UOWindow == 0)
                UOWindow = hwnd;
        }
        // Find the next sibling.
        hwnd = FindWindowEx(0, hwnd, "Ultima Online", 0);
    }
    if(UOWindow==0)
        MessageBox(0,"Unable to find UO window!\n",0,0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Button5Click(TObject *Sender)
{
    if(RunList->ItemIndex==-1)
        return;

    int t1=RunList->ItemIndex;
    int t2=RunList->Items->Count;
    AnsiString s="0x"+RunList->Items->Strings[RunList->ItemIndex];
    s.SetLength(10);
    TRunner *R=(TRunner*)s.ToIntDef(0);

    if(!R->IsTerminated())
        if(IDYES!=MessageBox(Handle,"Do you really want to terminate function?","Warning",MB_YESNO|MB_ICONQUESTION))
            return;

    if(t1!=RunList->ItemIndex || t2!=RunList->Items->Count)
    {
        MessageBox(Handle,"Some function has been terminated. Try again.","Warning",MB_ICONSTOP);
        return;
    }

    __try {
        if(R->IsTerminated())
        {
            if(IDYES!=MessageBox(Handle,"Function does not respond. Really terminate it?","Warning",MB_YESNO|MB_ICONQUESTION))
                return;
            R->Suspend();   // I don't call TerminateThread because it may crash 9x
            R->RemoveFromList();
            delete R;
        }
        else
            R->StopRunning();
    } __except(EXCEPTION_EXECUTE_HANDLER) {}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::Button2Click(TObject *Sender)
{
    if(SD->Execute())
    {
        EditForm->Script->Lines->SaveToFile(SD->FileName);
        OD->FileName=SD->FileName;
        EditForm->StatusBar->Panels->Items[1]->Text=OD->FileName;
        UO->ClientPrint("Script saved.");
    }
}
//---------------------------------------------------------------------------

