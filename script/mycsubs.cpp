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

#include "mycsubs.h"
#include "myvar.h"
#include "myparser.h"

//---------------------------------------------------------------------------

// потом, возможно, добавлю всякие проверки...
static int __cdecl GetType(const ParserVariable *v)
{
    TVariable *Var=(TVariable*)v;
    return Var->Type;
}

static void __cdecl SetType(const ParserVariable *v, int Type)
{
    TVariable *Var=(TVariable*)v;
    Var->Type=(TVariable::VarType)Type;
}

static const char* __cdecl GetString(const ParserVariable *v)
{
    TVariable *Var=(TVariable*)v;
    return Var->Data.AsString.c_str();
}

static double __cdecl GetNumber(const ParserVariable *v)
{
    TVariable *Var=(TVariable*)v;
    return Var->Data.AsNumber;
}

static void* __cdecl GetUserData(const ParserVariable *v)
{
    TVariable *Var=(TVariable*)v;
    return Var->Data.UserData;
}

static void __cdecl SetString(const ParserVariable *v, const char* Str)
{
    TVariable *Var=(TVariable*)v;
    Var->Type=TVariable::T_String;
    Var->Data.AsString=Str;
}

static void __cdecl SetNumber(const ParserVariable *v, double Num)
{
    TVariable *Var=(TVariable*)v;
    Var->Type=TVariable::T_Number;
    Var->Data.AsNumber=Num;
}

static void __cdecl SetUserData(const ParserVariable *v, const void *Data)
{
    TVariable *Var=(TVariable*)v;
    Var->Data.UserData=(void*)Data;
}

static void __cdecl CCreateClass(const ParserVariable *v, const char* Str)
{
    TVariable *Var=(TVariable*)v;
    *Var=CreateClass(Str);
}

static void __cdecl CError(const ParserVariable *v, const char* Str)
{
    TVariable *Var=(TVariable*)v;
    *Var=::Error(Str);
}

static ParserVariable* __cdecl CTakeArrayValue(const ParserVariable *v, int pos)
{
    TVariable *Var=(TVariable*)v;
    return (ParserVariable*)&TakeArrayValue(*Var,pos);
}

static ParserVariable* __cdecl NewVar()
{
    return (ParserVariable*)new TVariable;
}

static void __cdecl DelVar(const ParserVariable *P)
{
    delete ((TVariable*)P);
}

static ParserVariable* __cdecl DupVar(const ParserVariable *P)
{
    TVariable *v=new TVariable;
    *v=*(TVariable*)P;
    return (ParserVariable*)v;
}

static bool __cdecl PExecute(ParserObject *Parser, ParserVariable *Result,
    const char *Func, ParserVariable *Params[], int ParamCount)
{
    *((TVariable*)Result)=((TParser*)Parser)->Execute(Func,
        (TVariable**)Params,ParamCount);
    return !((TVariable*)Result)->Error();
}

static void __cdecl PTerminate(ParserObject *Parser)
{
    ((TParser*)Parser)->Terminate();
}

static void __cdecl PSetGlobVar(ParserObject *Parser, const char* Name, const ParserVariable *Val)
{
    ((TParser*)Parser)->SetGlobalVariable(Name,*((TVariable*)Val));
}

static void __cdecl PSetFunctions(ParserObject *Parser, const struct CFuncTable *Table)
{
    ((TParser*)Parser)->SetCFunctions(Table);
}

static void __cdecl PSetClass(ParserObject *Parser, const char *Class,const struct CFuncTable *Table)
{
    ((TParser*)Parser)->SetCClass(Class,Table);
}

static void __cdecl PSetProperties(ParserObject *Parser, const char * Class, const struct CPropTable * Prop)
{
    ((TParser*)Parser)->SetCProperties(Class,Prop);
}

static ParserVariable* __cdecl PGetGlobVar(ParserObject *Parser, const char* Name)
{
    TVariable v=((TParser*)Parser)->GetGlobalVariable(Name);
    return DupVar((ParserVariable*)&v);
}

////////////////////////////////////////////////////////////////////////
LibraryFunctions *GetLibraryFunctions()
{
    static LibraryFunctions Fun={
        sizeof(LibraryFunctions),
        GetType,
        SetType,
        GetString,
        GetNumber,
        GetUserData,
        SetString,
        SetNumber,
        SetUserData,
        CCreateClass,
        CTakeArrayValue,
        NewVar,
        DelVar,
        DupVar,
        PExecute,
        PTerminate,
        PSetGlobVar,
        PSetFunctions,
        PSetClass,
        PSetProperties,
        PGetGlobVar,
        CError
    };
    return &Fun;
}

