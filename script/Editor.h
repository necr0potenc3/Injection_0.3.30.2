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

#ifndef EditorH
#define EditorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ToolWin.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TEditForm : public TForm
{
__published:    // IDE-managed Components
    TMemo *Script;
    TStatusBar *StatusBar;
    TControlBar *ControlBar1;
    TPanel *Panel1;
    TSpeedButton *SpeedButton1;
    TSpeedButton *SpeedButton2;
    TEdit *Edit1;
    TPanel *Panel2;
    THotKey *HotKey;
    TSpeedButton *SpeedButton3;
    TEdit *Edit2;
    TPanel *Panel3;
    TSpeedButton *SpeedButton5;
    TSpeedButton *SpeedButton4;
    TPanel *Panel4;
    TSpeedButton *SpeedButton6;
    TEdit *Edit3;
    TSpeedButton *SpeedButton7;
    TPopupMenu *PopupMenu1;
    TMenuItem *Cut1;
    TMenuItem *Copy1;
    TMenuItem *Paste1;
    TMenuItem *N1;
    TMenuItem *UOPrint1;
    TMenuItem *UOExec1;
    TMenuItem *N2;
    TMenuItem *UOPrint2;
    TMenuItem *UOExec2;
    TMenuItem *UOSay2;
    TMenuItem *UOPress1;
    TMenuItem *UOLClick1;
    TMenuItem *UORClick1;
    TMenuItem *UOLDblClick1;
    TMenuItem *UORDblClick1;
    TMenuItem *UODrag1;
    TMenuItem *Undo1;
    TMenuItem *Other1;
    TMenuItem *Wait1;
    TMenuItem *MessageBox1;
    TMenuItem *UOCount1;
    TMenuItem *Str1;
    TMenuItem *Val1;
    TMenuItem *Len1;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ScriptKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ScriptClick(TObject *Sender);
    void __fastcall SpeedButton1Click(TObject *Sender);
    void __fastcall SpeedButton3Click(TObject *Sender);
    void __fastcall SpeedButton4Click(TObject *Sender);
    void __fastcall SpeedButton7Click(TObject *Sender);
    void __fastcall SpeedButton2Click(TObject *Sender);
    void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall SpeedButton5Click(TObject *Sender);
    void __fastcall SpeedButton6Click(TObject *Sender);
    void __fastcall Cut1Click(TObject *Sender);
    void __fastcall Copy1Click(TObject *Sender);
    void __fastcall Paste1Click(TObject *Sender);
    void __fastcall Undo1Click(TObject *Sender);
    void __fastcall UOInsertText(TObject *Sender);
private:    // User declarations
public:     // User declarations
    __fastcall TEditForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TEditForm *EditForm;
//---------------------------------------------------------------------------
#endif
