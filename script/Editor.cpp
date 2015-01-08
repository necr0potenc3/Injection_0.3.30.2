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

#include "Editor.h"
#include "UnitFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditForm *EditForm=0;
//---------------------------------------------------------------------------
__fastcall TEditForm::TEditForm(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TEditForm::FormCreate(TObject *Sender)
{
    DWORD T=16;     // TAB == 4 spaces
    SendMessage(Script->Handle,EM_SETTABSTOPS,1,(DWORD)&T);
}
//---------------------------------------------------------------------------
void __fastcall TEditForm::ScriptKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
    if(Script->CaretPos.y && Key==VK_RETURN)
    {
        try {
            AnsiString A=Script->Lines->Strings[Script->CaretPos.y-1];
            int t=0;
            while((A[t+1]==' ' || A[t+1]=='\t') && t<A.Length())
                t++;
            A.SetLength(t);
            Script->Lines->Strings[Script->CaretPos.y]=A;
            Script->CaretPos.x=A.Length();
        } catch(...) {}
    }
    StatusBar->Panels->Items[0]->Text="Line: "+IntToStr(Script->CaretPos.y+1)+" Col: "+IntToStr(Script->CaretPos.x+1);
}
//---------------------------------------------------------------------------
void __fastcall TEditForm::ScriptClick(TObject *Sender)
{
    StatusBar->Panels->Items[0]->Text="Line: "+IntToStr(Script->CaretPos.y+1)+" Col: "+IntToStr(Script->CaretPos.x+1);
}
//---------------------------------------------------------------------------
void __fastcall TEditForm::SpeedButton1Click(TObject *Sender)
{
    Edit1->SelectAll();
    Edit1->CopyToClipboard();
    Script->PasteFromClipboard();
}
//---------------------------------------------------------------------------
void __fastcall TEditForm::SpeedButton3Click(TObject *Sender)
{
    Edit2->Text=IntToStr(HotKey->HotKey);
    Edit2->SelectAll();
    Edit2->CopyToClipboard();
    Script->PasteFromClipboard();
}
//---------------------------------------------------------------------------
void __fastcall TEditForm::SpeedButton4Click(TObject *Sender)
{
    if(IDYES==MessageBox(Handle,"Do you really want to clear the script?\nYou will lose it if it is not saved.","Warning",MB_ICONQUESTION|MB_YESNO))
    {
        Script->Lines->Clear();
        Script->Lines->Add("sub main()");
        Script->Lines->Add("\t");
        Script->Lines->Add("end sub");
        PostMessage(Script->Handle,WM_KEYDOWN,VK_UP,1);
        PostMessage(Script->Handle,WM_KEYUP,VK_UP,1);
        PostMessage(Script->Handle,WM_KEYDOWN,VK_UP,1);
        PostMessage(Script->Handle,WM_KEYUP,VK_UP,1);
        PostMessage(Script->Handle,WM_KEYDOWN,VK_END,1);
        PostMessage(Script->Handle,WM_KEYUP,VK_END,1);
    }
}
//---------------------------------------------------------------------------
void __fastcall TEditForm::SpeedButton7Click(TObject *Sender)
{
    MainForm->Button2Click(Sender);
}
//---------------------------------------------------------------------------
bool InCapture=false;
void __fastcall TEditForm::SpeedButton2Click(TObject *Sender)
{
    static bool Showed=false;
    if(!Showed)
        if(IDOK!=MessageBox(Handle,"Move mouse over UO window and click mouse button to select a point.","Select",MB_ICONINFORMATION|MB_OKCANCEL))
            return;
    Showed=true;
    SetCapture(Handle);
    CloseWindow(Handle);
    InCapture=true;
}
//---------------------------------------------------------------------------
void __fastcall TEditForm::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if(InCapture)
    {
        POINT p;
        GetCursorPos(&p);
        OpenIcon(Handle);
        ReleaseCapture();
        InCapture=false;
        ::ScreenToClient(UOWindow,&p);
        Edit1->Text=IntToStr(p.x)+","+IntToStr(p.y);
    }
}
//---------------------------------------------------------------------------
void __fastcall TEditForm::SpeedButton5Click(TObject *Sender)
{
    MainForm->Button1Click(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TEditForm::SpeedButton6Click(TObject *Sender)
{
    RunFunction(Edit3->Text.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TEditForm::Cut1Click(TObject *Sender)
{
    Script->CutToClipboard();
}
//---------------------------------------------------------------------------
void __fastcall TEditForm::Copy1Click(TObject *Sender)
{
    Script->CopyToClipboard();
}
//---------------------------------------------------------------------------
void __fastcall TEditForm::Paste1Click(TObject *Sender)
{
    Script->PasteFromClipboard();
}
//---------------------------------------------------------------------------
void __fastcall TEditForm::Undo1Click(TObject *Sender)
{
    Script->Undo();
}
//---------------------------------------------------------------------------
void AddStr(AnsiString S)
{
    int t=EditForm->Script->CaretPos.x+1;
    AnsiString Str=EditForm->Script->Lines->Strings[EditForm->Script->CaretPos.y];
    Str.Insert(S+"()",t);
    EditForm->Script->Lines->Strings[EditForm->Script->CaretPos.y]=Str;
    PostMessage(EditForm->Script->Handle,WM_KEYDOWN,VK_HOME,1);
    PostMessage(EditForm->Script->Handle,WM_KEYUP,VK_HOME,1);
    for(int i=0; i<t+S.Length(); i++)
    {
        PostMessage(EditForm->Script->Handle,WM_KEYDOWN,VK_RIGHT,1);
        PostMessage(EditForm->Script->Handle,WM_KEYUP,VK_RIGHT,1);
    }
}
void __fastcall TEditForm::UOInsertText(TObject *Sender)
{
    AddStr(((TMenuItem*)Sender)->Caption);
}
//---------------------------------------------------------------------------

