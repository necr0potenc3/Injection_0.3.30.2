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

#include "ScrRun.h"
#include "UnitFrm.h"
#include "Editor.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------
TVariable __cdecl DoPrint(TVariable *v[], int ParamCount,Sasha::TParser *Parser)
{
    UO->ClientPrint(v[0]->Data.AsString.c_str());
    return TVariable(true);
}

TVariable __cdecl DoExec(TVariable *v[], int ParamCount,Sasha::TParser *Parser)
{
//    UO->DoCommand(v[0]->Data.AsString.c_str());
// The command should be executed from the main thread. So use SendMessage to synchronize
    SendMessage(MainForm->Handle,WM_USER+0x1000,0,(int)v[0]->Data.AsString.c_str());
    return TVariable(true);
}

TVariable __cdecl DoSendString(TVariable *v[], int ParamCount,Sasha::TParser *Parser)
{
    LPWSTR Line=new wchar_t[v[0]->Data.AsString.Length()+1];
    MultiByteToWideChar(CP_ACP,0,v[0]->Data.AsString.c_str(),
        -1,Line,v[0]->Data.AsString.Length()+1);

    for(int i=0; i<v[0]->Data.AsString.Length(); i++)
    {
        SendMessageW(UOWindow,WM_CHAR,Line[i],1);
    }
    SendMessage(UOWindow,WM_CHAR,13,1);
    return TVariable(true);
}

TVariable __cdecl DoSendKey(TVariable *v[], int ParamCount,Sasha::TParser *Parser)
{
    int Count=1;
    int SleepT=100;
    if(ParamCount>2)
        Count=v[1]->Data.AsNumber;

    if(Count<1)
        Count=1;

    if(ParamCount>3)
        SleepT=v[2]->Data.AsNumber;

    for(int i=0; i<Count; i++)
    {
        SendMessageW(UOWindow,WM_KEYDOWN,v[0]->Data.AsNumber,1);
        SendMessageW(UOWindow,WM_KEYUP,v[0]->Data.AsNumber,1|(3<<30));
        Sleep(SleepT);
    }

    return TVariable(true);
}

TVariable __cdecl DoLClick(TVariable *v[], int ParamCount,Sasha::TParser *Parser)
{
    int X=v[0]->Data.AsNumber;
    int Y=v[1]->Data.AsNumber;
    SendMessage(UOWindow,WM_SETCURSOR,(int)UOWindow,MAKELPARAM(HTCLIENT,WM_MOUSEMOVE));
    SendMessage(UOWindow,WM_MOUSEMOVE,0,MAKELPARAM(X,Y));
    SendMessage(UOWindow,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(X,Y));
    SendMessage(UOWindow,WM_LBUTTONUP,MK_LBUTTON,MAKELPARAM(X,Y));

    return TVariable(true);
}

TVariable __cdecl DoRClick(TVariable *v[], int ParamCount,Sasha::TParser *Parser)
{
    int X=v[0]->Data.AsNumber;
    int Y=v[1]->Data.AsNumber;
    SendMessage(UOWindow,WM_SETCURSOR,(int)UOWindow,MAKELPARAM(HTCLIENT,WM_MOUSEMOVE));
    SendMessage(UOWindow,WM_MOUSEMOVE,0,MAKELPARAM(X,Y));
    SendMessage(UOWindow,WM_RBUTTONDOWN,MK_RBUTTON,MAKELPARAM(X,Y));
    SendMessage(UOWindow,WM_RBUTTONUP,MK_RBUTTON,MAKELPARAM(X,Y));

    return TVariable(true);
}

TVariable __cdecl DoLDblClick(TVariable *v[], int ParamCount,Sasha::TParser *Parser)
{
    int X=v[0]->Data.AsNumber;
    int Y=v[1]->Data.AsNumber;
    SendMessage(UOWindow,WM_SETCURSOR,(int)UOWindow,MAKELPARAM(HTCLIENT,WM_MOUSEMOVE));
    SendMessage(UOWindow,WM_MOUSEMOVE,0,MAKELPARAM(X,Y));
    SendMessage(UOWindow,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(X,Y));
    SendMessage(UOWindow,WM_LBUTTONUP,MK_LBUTTON,MAKELPARAM(X,Y));
// Hmmm... Seems that UO ignores WM_*DBLCLICK and processes single clicks
    SendMessage(UOWindow,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(X,Y));
    SendMessage(UOWindow,WM_LBUTTONUP,MK_LBUTTON,MAKELPARAM(X,Y));

    return TVariable(true);
}

TVariable __cdecl DoRDblClick(TVariable *v[], int ParamCount,Sasha::TParser *Parser)
{
    int X=v[0]->Data.AsNumber;
    int Y=v[1]->Data.AsNumber;
    SendMessage(UOWindow,WM_SETCURSOR,(int)UOWindow,MAKELPARAM(HTCLIENT,WM_MOUSEMOVE));
    SendMessage(UOWindow,WM_MOUSEMOVE,0,MAKELPARAM(X,Y));
    SendMessage(UOWindow,WM_RBUTTONDOWN,MK_RBUTTON,MAKELPARAM(X,Y));
    SendMessage(UOWindow,WM_RBUTTONUP,MK_RBUTTON,MAKELPARAM(X,Y));
    SendMessage(UOWindow,WM_RBUTTONDOWN,MK_RBUTTON,MAKELPARAM(X,Y));
    SendMessage(UOWindow,WM_RBUTTONUP,MK_RBUTTON,MAKELPARAM(X,Y));

    return TVariable(true);
}

TVariable __cdecl DoDrag(TVariable *v[], int ParamCount,Sasha::TParser *Parser)
{
    int X1=v[0]->Data.AsNumber;
    int Y1=v[1]->Data.AsNumber;
    int X2=v[2]->Data.AsNumber;
    int Y2=v[3]->Data.AsNumber;
    int N=0;
    if(ParamCount>5)
        N=v[4]->Data.AsNumber;
    AnsiString Str;
    if(N>0)
        Str=IntToStr(N);
    Str+="\r";

    SendMessage(UOWindow,WM_SETCURSOR,(int)UOWindow,MAKELPARAM(HTCLIENT,WM_MOUSEMOVE));
    SendMessage(UOWindow,WM_MOUSEMOVE,0,MAKELPARAM(X1,Y1));
    SendMessage(UOWindow,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(X1,Y1));
    SendMessage(UOWindow,WM_SETCURSOR,(int)UOWindow,MAKELPARAM(HTCLIENT,WM_MOUSEMOVE));
    SendMessage(UOWindow,WM_MOUSEMOVE,0,MAKELPARAM(X1,Y1));
    Sleep(400);
    SendMessage(UOWindow,WM_LBUTTONUP,MK_LBUTTON,MAKELPARAM(X1,Y1));

    for(int i=0; i<Str.Length(); i++)
    {
        SendMessage(UOWindow,WM_CHAR,Str[i+1],1);
    }

    SendMessage(UOWindow,WM_SETCURSOR,(int)UOWindow,MAKELPARAM(HTCLIENT,WM_MOUSEMOVE));
    SendMessage(UOWindow,WM_MOUSEMOVE,0,MAKELPARAM(X2,Y2));
    SendMessage(UOWindow,WM_LBUTTONDOWN,MK_LBUTTON,MAKELPARAM(X2,Y2));
    SendMessage(UOWindow,WM_LBUTTONUP,MK_LBUTTON,MAKELPARAM(X2,Y2));

    return TVariable(true);
}

TVariable __cdecl Invalid(TVariable *v[], int ParamCount,Sasha::TParser *Parser)
{
    return Error("This property is read only");
}

TVariable __cdecl DoCHR(TVariable *v[], int ParamCount,Sasha::TParser *Parser)
{
    return TVariable(Sasha::TString((char)v[0]->Data.AsNumber).c_str());
}

TVariable __cdecl DoASC(TVariable *v[], int ParamCount,Sasha::TParser *Parser)
{
    return TVariable((double)StrToIntDef(v[0]->Data.AsString.c_str(),0));
}

TVariable __cdecl DoWait(TVariable *v[], int ParamCount,Sasha::TParser *Parser)
{
    Sleep(v[0]->Data.AsNumber);
    return TVariable(true);
}

TVariable __cdecl DoMessage(TVariable *v[], int ParamCount,Sasha::TParser *Parser)
{
    return TVariable((double)MessageBox(0,v[0]->Data.AsString.c_str(),(ParamCount>2?v[1]->Data.AsString.c_str():"Message"),(ParamCount>3?v[2]->Data.AsNumber:0)));
}

#define FUN(fun)   \
TVariable __cdecl Get##fun(TVariable *v[], int ParamCount,Sasha::TParser *Parser) \
{                                                                                 \
    return TVariable((double)*UO->fun);                                           \
}


TVariable __cdecl DoCount(TVariable *v[], int ParamCount,Sasha::TParser *Parser)
{
    AnsiString Obj;
    if(v[0]->Type==T_Number)
        Obj="0x"+IntToHex((int)v[0]->Data.AsNumber,8);
    else
        Obj=v[0]->Data.AsString.c_str();

    if(ParamCount>2)
    {
        AnsiString Color;
        if(v[1]->Type==T_Number)
            Color="0x"+IntToHex((int)v[1]->Data.AsNumber,4);
        else
            Color=v[1]->Data.AsString.c_str();

//        MessageBox(0,Obj.c_str(),Color.c_str(),0);
        return TVariable((double)UO->CountTypeColor(Obj.c_str(),
            Color.c_str()));
    }
//    MessageBox(0,Obj.c_str(),0,0);
    return TVariable((double)UO->CountType(Obj.c_str()));
}

Sasha::TParser::FuncTable UOFunctions[]={
    {"Print",DoPrint,1},
    {"Exec",DoExec,1},
    {"Count",DoCount,-1},
    {"Say",DoSendString,1},
    {"Press",DoSendKey,-1},
    {"KeyPress",DoSendKey,-1},
    {"LClick",DoLClick,2},
    {"RClick",DoRClick,2},
    {"LDblClick",DoLDblClick,2},
    {"RDblClick",DoRDblClick,2},
    {"Drag",DoDrag,-1},
    {0,0,0}
};

Sasha::TParser::FuncTable Func[]={
    {"wait",DoWait,1},
    {"sleep",DoWait,1},
    {"message",DoMessage,-1},
    {"messagebox",DoMessage,-1},
    {"chr",DoCHR,1},
    {"asc",DoASC,1},
    {0,0,0}
};

FUN(Life)
FUN(STR)
FUN(Mana)
FUN(INT)
FUN(Stamina)
FUN(DEX)
FUN(Armor)
FUN(Weight)
FUN(Gold)
FUN(BM)
FUN(BP)
FUN(GA)
FUN(GS)
FUN(MR)
FUN(NS)
FUN(SA)
FUN(SS)
FUN(VA)
FUN(EN)
FUN(WH)
FUN(FD)
FUN(BR)
FUN(H)
FUN(C)
FUN(M)
FUN(L)
FUN(B)
FUN(AR)
FUN(BT)

Sasha::TParser::PropTable UOProperties[]=
{
    "Life",GetLife,Invalid,
    "STR",GetSTR,Invalid,
    "Mana",GetMana,Invalid,
    "INT",GetINT,Invalid,
    "Stamina",GetStamina,Invalid,
    "DEX",GetDEX,Invalid,
    "Armor",GetArmor,Invalid, 
    "Weight",GetWeight,Invalid, 
    "Gold",GetGold,Invalid, 
    "BM",GetBM,Invalid, 
    "BP",GetBP,Invalid, 
    "GA",GetGA,Invalid, 
    "GS",GetGS,Invalid,
    "MR",GetMR,Invalid,
    "NS",GetNS,Invalid,
    "SA",GetSA,Invalid,
    "SS",GetSS,Invalid,
    "VA",GetVA,Invalid,
    "EN",GetEN,Invalid,
    "WH",GetWH,Invalid,
    "FD",GetFD,Invalid,
    "BR",GetBR,Invalid,
    "H",GetH,Invalid,
    "C",GetC,Invalid,
    "M",GetM,Invalid,
    "L",GetL,Invalid,
    "B",GetB,Invalid,
    "AR",GetAR,Invalid,
    "BT",GetBT,Invalid,
    0,0,0
};

__fastcall TRunner::TRunner(bool CreateSuspended, const char* Nam)
    : TThread(CreateSuspended),Name(Nam)
{
    FreeOnTerminate=true;
}
//---------------------------------------------------------------------------
void __fastcall TRunner::Execute()
{
  __try {
    Parser=new Sasha::TParser;
    Sasha::TParser &P=*Parser;

    P.SetClass("InternalUoClass",UOFunctions);
    P.SetProperties("InternalUoClass",UOProperties);
    P.SetFunctions(Func);
    TVariable UO;
    UO.Type=(TVariable::VarType)T_Class; UO.Data.AsString="InternalUoClass";
    P.SetGlobalVariable("UO",UO);

    ::UO->AddClasses((ParserObject*)Parser,GetLibraryFunctions());

    P.SetScript(EditForm->Script->Text.c_str(),EditForm->Script->Text.Length());
    P.PreProcess();
    if(P.Error)
        MessageBox(MainForm->Handle,P.ErrString,(AnsiString("SetScript at ")+P.GetErrorLine()).c_str(),0);
    else
    {
        TVariable v=P.Execute(Name.c_str());
        if(P.Error && !P.Terminated)
            MessageBox(MainForm->Handle,P.ErrString,(AnsiString("Execute at ")+P.GetErrorLine()).c_str(),0);
    }
  } __except(EXCEPTION_EXECUTE_HANDLER) {MessageBox(0,"Unhandled exception in parser.",0,0);}
  Synchronize(RemoveFromList);
}
//---------------------------------------------------------------------------
void __fastcall TRunner::RemoveFromList()
{
    int t=MainForm->RunList->Items->IndexOf(IntToHex((int)this,8)+" "+Name);
    if(t!=-1)
        MainForm->RunList->Items->Delete(t);
    delete Parser;
}

void __fastcall TRunner::StopRunning()
{
    Parser->Terminate();
    Terminate();
}

