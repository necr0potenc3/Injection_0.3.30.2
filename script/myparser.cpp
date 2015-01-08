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

#include "myparser.h"
#include "operators.h"
#include <stdlib.h>

extern int yyparse(void*);

// прототипы для местных функций, недостойных быть положенными в заголовок
void *CreateArray(int size);
TVariable &GetArrayValue(void *Array,int pos);
void DeleteArray(void *Array);
LibraryFunctions *GetLibraryFunctions();

namespace Sasha
{

// Должны соответствовать TParser::ErrorCodes
const char *TParser::ErrorStrings[] =
{
    "No error",
    "Syntax error",
    "Variable already defined",
    "Variable undefined",
    "Internal ERROR!!!!!!!!!!!!",
    "Bad operation for this type of variable",
    "Label already defined in this proc",
    "Label defined outside proc",
    "Function not found",
    "Invalid number of arguments",
    "Invalid variable used in method call",
    "Variable used as an array",
    "Runtime error",
    "Attempt to jump inside for/while/repeat",
    "Label not found",
    "Invalid number of parameters to external function"
};

TParser::TParser() : Script(0), ScriptSize(0), Error(0), ScriptPos(0),
    FinishedSub(false), Terminated(false)
{
    ErrPos=0;
    ReturnCR=false;
    TmpErrorString=0;
    CycleDepth=0;
    FatalError=0.0;
    DbgFun=0;
}

TParser::~TParser()
{
    if(Script)
        delete Script;
    if(TmpErrorString)
        free(TmpErrorString);
}

void TParser::SetVar (TVariable &Dest, const TVariable &Source)
{
// Пропустить установку локальных переменные при первом проходе
    if(IsPreprocessing&&FunctionName.Length())
        return;

// проверить тип устанавливаемого значения. В принципе нафиг не надо, т.к.
// из yylex эта ф-я вызывается всегда с правильными параметрами        
    if(Source.Type!=TVariable::T_Number && Source.Type!=TVariable::T_String
        && Source.Type!=TVariable::T_Class && Source.Type!=TVariable::T_Array)
    {
        SetError(P_BadResult,0,Dest.Data.AsString.c_str());
        return;
    }
    if(Dest.Type != TVariable::T_Identifier)
    {
        SetError(P_InternalError,"TParser::SetVar - variable is not T_Identifier");
        return;
    }
    TString Tmp1=FunctionName+"::"+Dest.Data.AsString;  // "настоящее" имя переменной
    for(int i=0; i<Variables.Count; i++)
    {
        if(Variables[i].Name == Tmp1)
        {
            Variables[i].Var = Source;
            return;
        }
    }
    TString Tmp2="::"+Dest.Data.AsString;       // или это глобальная переменная
    for(int i1=0; i1<Variables.Count; i1++)
    {
        if(Variables[i1].Name == Tmp2)
        {
            Variables[i1].Var = Source;
            return;
        }
    }
// проверить может это все-таки property (в случае, если имя содержит '.')
    TVariable &New=Dest;
    if(strchr(New.Data.AsString.c_str(),'.')!=0)
    {
        if(IsPreprocessing&&FunctionName.Length())
            return;

        TString VarName=New.Data.AsString;
        // Так делать нельзя, но мне можно: (отрезаю имя класса)
        *(char*)strchr(VarName.c_str(),'.')=0;
        VarName=VarName.c_str();
        TString Func=strchr(New.Data.AsString.c_str(),'.')+1; // и имя property

        TVariable Tmp;                                   // <-+- подготовка переменной
        Tmp.Type=TVariable::T_Identifier;                //   |  типа T_Class для
        Tmp.Data.AsString=VarName;                       //   |  передачи ее в качестве
        TVariable Cls=GetVar(Tmp);                       //   |  "this" в property
        if(Cls.Type!=TVariable::T_Class)                 //   |
        {                                                //   |
            SetError(P_InvalidClass,0,VarName.c_str());  //   |
            return;                                      //   |
        }                                                // <-+
        Cls.Data.AsString.Uppercase();
        for(int i=0; i<Classes.Count; i++)               // перебор всех классов
        {                                                // для поиска нужного можно
            if(!(Classes[i].Name==Cls.Data.AsString))    // заменить на более быстрый алгоритм поиска
                continue;

            for(int j=0; j<Classes[i].Properties.Count; j++)    // перебор property в этом классе
                if(Classes[i].Properties[j].Name==Func)
                {
                    TVariable *pCls[2]={(TVariable*)&Source,(TVariable*)&Cls}; // установка массива параметров
                    if(Classes[i].Properties[j].Set)
                    {
                        Classes[i].Properties[j].Set(pCls,2,this);
                    } else if(Classes[i].Properties[j].CSet)
                    {
                        TVariable R;
                        const char *r=Classes[i].Properties[j].CSet(GetLibraryFunctions(),
                            (ParserVariable*)&R,
                            (ParserVariable**)&pCls,2,(ParserObject*)this);
                        if(r)
                        {
                            SetError(P_RuntimeError,r,Dest.Data.AsString.c_str());
                            return;
                        }
                    } else
                    {
                        SetError(P_BadResult,"Setting the ReadOnly property",Dest.Data.AsString.c_str());
                        return;
                    }
                    SetVar(Tmp,Cls);
                    return;
                }
        }
    }
    // значит не найдена
    SetError(P_VarUndefined,0,Dest.Data.AsString.c_str());
    return;
}

TVariable TParser::GetVar (TVariable &New)
{
int i,j,i1;
// проверка корректности параметров
    if(New.Type!=TVariable::T_Identifier)
    {
        SetError(P_InternalError,"TParser::GetVar - variable is not T_Identifier");
        return ::Error();
    }
    if(New.Data.AsString=="FATALERROR" ||
        New.Data.AsString=="FATAL ERROR")
        return FatalError;

// поиск заданной переменной в списке. Лучше бы хранить список отсортированным и
// использовать бинарный поиск...        
    TString Tmp1=FunctionName+"::"+New.Data.AsString;   // локальная?
    for(i=0; i<Variables.Count; i++)
    {
        if(Variables[i].Name == Tmp1)
        {
            return Variables[i].Var;
        }
    }
    TString Tmp2="::"+New.Data.AsString;                // глобальная?
    for(i1=0; i1<Variables.Count; i1++)
    {
        if(Variables[i1].Name == Tmp2)
        {
            return Variables[i1].Var;
        }
    }

// проверить проперти (см код в SetVar)
    if(strchr(New.Data.AsString.c_str(),'.')!=0)
    {
        if(IsPreprocessing&&FunctionName.Length())
        {
            TVariable t;
            t.Type=TVariable::T_Number;
            t.Data.AsNumber=0;
            return t;
        }

        TString VarName=New.Data.AsString;
        // Так делать нельзя, но мне можно:
        *(char*)strchr(VarName.c_str(),'.')=0;
        VarName=VarName.c_str();
        TString Func=strchr(New.Data.AsString.c_str(),'.')+1;

        TVariable Tmp;
        Tmp.Type=TVariable::T_Identifier;
        Tmp.Data.AsString=VarName;
        TVariable Cls=GetVar(Tmp);
        if(Cls.Type!=TVariable::T_Class)
        {
            SetError(P_InvalidClass, 0,VarName.c_str());
            return ::Error();
        }
        Cls.Data.AsString.Uppercase();
        for(i=0; i<Classes.Count; i++)
        {
            if(!(Classes[i].Name==Cls.Data.AsString))
                continue;

            for(j=0; j<Classes[i].Properties.Count; j++)
                if(Classes[i].Properties[j].Name==Func)
                {
                    TVariable *pCls=&Cls;
                    TVariable v;
                    if(Classes[i].Properties[j].Get)
                    {
                        v=Classes[i].Properties[j].Get(
                            &pCls,1,this);
                    } else if(Classes[i].Properties[j].CGet)
                    {
                        const char *r=Classes[i].Properties[j].CGet(GetLibraryFunctions(),
                            (ParserVariable*)&v,
                            (ParserVariable**)&pCls,1,(ParserObject*)this);
                        if(r)
                            return ::Error(r);
                    } else
                    {
                        SetError(P_BadResult,"Getting the WriteOnly property",New.Data.AsString.c_str());
                        return ::Error("Getting the WriteOnly property");
                    }
                    SetVar(Tmp,Cls);
                    return v;
                }
        }
    }

// теперь можно вместо переменной указавать имя функции, тогда оно будет
// считаться строковой переменной со значением равным имени функции    
    for(i=0; i<Functions.Count; i++)
        if(Functions[i].Name == New.Data.AsString)
            return Functions[i].Name.c_str();

    SetError(P_VarUndefined,0,New.Data.AsString.c_str());
    return ::Error();
}

void TParser::SetError(int ErrCode, const char *Str, const char *Param)
{
    if(ErrCode==0||Error)
        return;
    if(TmpErrorString)
        free(TmpErrorString);
    TmpErrorString=(char*)malloc(1000);  // надеюсь хватит...
    Error=ErrCode;
    if(ErrPos==0)
        ErrPos=ScriptPos;
    if(Str)
        ErrString=Str;
    else
        ErrString=ErrorStrings[Error];
    strcpy(TmpErrorString,"Line ") ;
    itoa(GetErrorLine(),TmpErrorString+strlen(TmpErrorString),10);
    strcat(TmpErrorString,": ");
    strcat(TmpErrorString,ErrString);
    if(Param)
    {
        strcat(TmpErrorString," - ");
        strcat(TmpErrorString,Param);
    }
    ErrString=TmpErrorString;
}

void TParser::AddVar (TVariable &New)
{
    if(New.Type!=TVariable::T_Identifier)
    {
        SetError(P_InternalError,"TParser::AddVar - variable is not T_Identifier");
        return;
    }

    TString Tmp=FunctionName+"::"+New.Data.AsString;
    for(int i=0; i<Variables.Count; i++)
    {
        if(Variables[i].Name == Tmp)
        {
            SetError(P_VarDefined,0,New.Data.AsString.c_str());
            return;
        }
    }

    Variables.Add(Variable(FunctionName+"::"+New.Data.AsString));
}

void TParser::SetParserError(char *Err)
{
    if(ErrPos==0)
        ErrPos=ScriptPos;
    SetError(P_Syntax, Err);
}

bool TParser::SetScript(const char *Scr, int ScrSize)
{
    if(Script)
        delete Script;
    Script = new char [ScrSize+10]; // +10, чтоб не добавлять везде проверки
                                    // на выход за пределы массива
    memcpy(Script, Scr, ScrSize);
    Script[ScrSize]='\n';  
    Script[ScrSize+1]=0;  // на всякий случай...
    Script[ScrSize+2]=0;  // на всякий случай...
    ScriptSize = ScrSize+1;

    InitFunctions();
    InitClasses();

    return true;
}

TVariable TParser::Execute(const char *Func)
{
    return Execute(Func, 0, 0);
}

TVariable TParser::Execute(const char *Func, TVariable *Params[], int ParamCount)
{
    int Position=-1;

    TString Tmp=Func;
    Tmp.Uppercase();
    for(int i=0; i<Functions.Count; i++)
        if(Functions[i].Name == Tmp)
            Position=i;

    if(Position==-1)
    {
        SetError(P_FunctionNotFound,0,Func);
        return ::Error();
    }

    if(ParamCount!=Functions[Position].Arguments.Count)
    {
        SetError(P_BadArgsCount,0,Func);
        return ::Error();
    }

// Запомнить тут все переменные, изменяемые Execute и yylex !!!!!!!!!!!
// (для поддержки рекурсии)
    bool _IsPreprocessing=IsPreprocessing;
    int _ScriptPos=ScriptPos;
    TString _FunctionName=FunctionName;
    TStack <int> _RUntils=RUntils;
    TStack <int> _Whiles=Whiles;
    TStack <For> _Fors=Fors;
    int _CycleDepth=CycleDepth;

// Для поддержки рекурсии - ко всем локальным переменным дописать в конец '-'
    if(_FunctionName.Length())
    {
        TString Tmp1=FunctionName+"::";
        for(int i=0; i<Variables.Count; i++)
            if(strncmpi(Variables[i].Name.c_str(),
              Tmp1.c_str(),Tmp1.Length())==0)
                Variables[i].Name+="-";
    }

    IsPreprocessing=false;
    Result="Result is undefined";

// запомнить параметры в локальных переменных скрипта
    TVariable tmp;
    tmp.Type=TVariable::T_Identifier;
    tmp.Data.AsString=Func;
    IsPreprocessing=false;
    BeginSub(tmp);
    {
    for(int i=0; i<ParamCount; i++)
    {
        TVariable Tmp;
        Tmp.Type=TVariable::T_Identifier;
        Tmp.Data.AsString=Functions[Position].Arguments[i];
        AddVar(Tmp);
        TVariable Var=*Params[i];
        if(Var.Type==TVariable::T_Array)
            Var.Data.AsNumber++;
        SetVar(Tmp,Var);
    }
    }    
    ScriptPos = Functions[Position].Position;
    if(yyparse((void*)this)&&!Error)
        SetError(P_InternalError,"Execute - yyparse returned error. Maybe external function returned error?");
    EndSub();

// А теперь убрать '-'
    if(_FunctionName.Length())
    {
        TString Tmp1=_FunctionName+"::";
        for(int i=0; i<Variables.Count; i++)
            if(strncmpi(Variables[i].Name.c_str(),
                Tmp1.c_str(),Tmp1.Length())==0 &&
                Variables[i].Name.c_str()[Variables[i].Name.Length()-1]=='-')
                {   // Так делать нельзя, но мне можно:
                    ((char*)Variables[i].Name.c_str())[Variables[i].Name.Length()-1]=0;
                    Variables[i].Name=Variables[i].Name.c_str();
                }
    }
// Восстановить все эти переменные !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    IsPreprocessing=_IsPreprocessing;
    ScriptPos=_ScriptPos;
    FunctionName=_FunctionName;
    FinishedSub=false;
    RUntils=_RUntils;
    Whiles=_Whiles;
    Fors=_Fors;
    CycleDepth=_CycleDepth;

    TVariable tmp1=Result;
    Result="Result is undefined";     
    return tmp1;
}

bool TParser::PreProcess()
{
// Разобрать глобальные переменные, сделать таблицу функций, меток, ...
//
//    Variables.Clear(); - не имеет смысла
    Error=0;
    ErrString=0;

    InitGlobals();
    FunctionName="";

    IsPreprocessing=true;
    if(yyparse((void*)this)&&!Error)
        SetError(P_InternalError,"PreProcess - yyparse returned error");
    IsPreprocessing=false;

    return Error;
}

void TParser::BeginCountingParameters(TVariable &v)
{
    if(v.Type!=TVariable::T_Identifier)
    {
        SetError(P_InternalError,"TParser::BeginCountingParameters - variable is not T_Identifier");
        return;
    }
    Parameters.Clear();
    TmpFunction=v.Data.AsString;
}

void TParser::AddSub (TVariable &New)
{
    if(New.Type!=TVariable::T_Identifier)
    {
        SetError(P_InternalError,"TParser::AddSub - variable is not T_Identifier");
        return;
    }
    Function f;
    f.Name=New.Data.AsString;
    f.Position=ScriptPos;
    f.Arguments=Parameters;
    Functions.Add(f);
    FunctionName="";
}

void TParser::AddParameter(TVariable &New)
{
    if(New.Type!=TVariable::T_Identifier)
    {
        SetError(P_InternalError,"TParser::AddParameter - variable is not T_Identifier");
        return;
    }
    Parameters.Add(New.Data.AsString);
    TString tmp=FunctionName;
    FunctionName=TmpFunction;
    FunctionName.Uppercase();
    AddVar(New);
    FunctionName=tmp;
}

void TParser::BeginSub (TVariable &Sub)
{
    if(Sub.Type!=TVariable::T_Identifier)
    {
        SetError(P_InternalError,"TParser::BeginSub - variable is not T_Identifier");
        return;
    }
    FunctionName=Sub.Data.AsString;
    FunctionName.Uppercase();
}

void TParser::EndSub()
{
    if(FunctionName=="")   // позволить звать EndSub 2 раза подряд:
        return;            // после RETURN и в конце Execute()

    TString Tmp=FunctionName+"::";
    for(int i=0; i<Variables.Count; i++)
        if(strncmpi(Variables[i].Name.c_str(),
            Tmp.c_str(),Tmp.Length())==0 &&
            Variables[i].Name.c_str()[Variables[i].Name.Length()-1]!='-')
            {
                if(Variables[i].Var.Type==TVariable::T_Array)
                {
                    DeleteArray(Variables[i].Var);
                    if(Variables[i].Var.Data.AsNumber)
                        Variables[i].Var.Data.AsNumber--;
                }
                Variables.Delete(i--);
            }

    FunctionName="";
}

void TParser::AddLabel(TVariable &New)
{
    if(New.Type!=TVariable::T_Identifier)
    {
        SetError(P_InternalError,"TParser::AddLabel - New is not T_Identifier");
        return;
    }

    if(!FunctionName.Length()||CycleDepth<0)
    {
        SetError(P_BadLabelPlace,0,New.Data.AsString.c_str());
        return;
    }

    New.Data.AsString.Uppercase();
    TString Tmp=FunctionName+"::"+New.Data.AsString;
    for(int i=0; i<Labels.Count; i++)
    {
        if(Labels[i].Name == Tmp)
        {
            // Уже есть - Ok, если не определена в другом месте
            if(Labels[i].Position!=ScriptPos)
                SetError(P_LabelDefined,0,Tmp.c_str());
            return;
        }
    }

    Label L;
    L.Name = FunctionName+"::"+New.Data.AsString;
    L.Position = ScriptPos;
    L.Depth = CycleDepth;

    Labels.Add(L);
}

void TParser::BeginCollectingParameters()
{
    ArgStk.Push(Arguments);
    Arguments.Clear();
}

void TParser::EndCollectingParameters()
{
    Arguments=ArgStk.Pop();
}

void TParser::StoreParameter(TVariable &Var)
{
    if(Var.Type!=TVariable::T_Number && Var.Type!=TVariable::T_String &&
       Var.Type!=TVariable::T_Class && Var.Type!=TVariable::T_Array
         && !IsPreprocessing)
    {
//        SetError(P_InternalError,"TParser::StoreParameter - Var is of wrong type");
        SetRuntimeError(Var);
        return;
    }

    Arguments.Add(Var);
}

TVariable TParser::CallSub(TVariable &Sub)
{
// Пропустить вызов процедур внутри функций при первом проходе
    if(IsPreprocessing&&FunctionName.Length())
    {
        TVariable t;
        t.Type=TVariable::T_Number;
        t.Data.AsNumber=0;
        return t;
    }

    if(Sub.Type!=TVariable::T_Identifier)
    {
        SetError(P_InternalError,"TParser::CallSub - Sub is not T_Identifier");
        return ::Error();
    }

    for(int i=0; i<Functions.Count; i++)
    {
        if(Functions[i].Name == Sub.Data.AsString)
        {
            TVariable v=Execute(Sub.Data.AsString.c_str(),Arguments.Items,Arguments.Count);
            return v;
        }
    }

// Вызвать глобальные функции, с учетом классов!
    if(strchr(Sub.Data.AsString.c_str(),'.')==0)
    { // просто функция
        for(int i=0; i<Classes.Count; i++)
        {
            if(Classes[i].Name.Length()&&!(Classes[i].Name==Sub.Data.AsString))
                continue;

            for(int j=0; j<Classes[i].Metods.Count; j++)
                if(Classes[i].Metods[j].Name==Sub.Data.AsString)
                {
                    if(Arguments.Count!=Classes[i].Metods[j].ParamCount &&
                        Classes[i].Metods[j].ParamCount!=-1)
                    {
                        SetError(P_InvalidParameters,0,Sub.Data.AsString.c_str());
                        return ::Error();
                    }
                    if(Classes[i].Metods[j].Func)
                        return Classes[i].Metods[j].Func(
                            Arguments.Items,Arguments.Count,this);
                    else
                    {
                        TVariable R;
                        const char *r=Classes[i].Metods[j].CFunc(GetLibraryFunctions(),
                            (ParserVariable*)&R,
                            (ParserVariable**)Arguments.Items,Arguments.Count,(ParserObject*)this);
                        if(r)
                            return ::Error(r);
                        return R;
                    }
                }
        }
    } else
    { // класс
        TString VarName=Sub.Data.AsString;
        // Так делать нельзя, но мне можно:
        *(char*)strchr(VarName.c_str(),'.')=0;
        VarName=VarName.c_str();
        TString Func=strchr(Sub.Data.AsString.c_str(),'.')+1;

        TVariable Tmp;
        Tmp.Type=TVariable::T_Identifier;
        Tmp.Data.AsString=VarName;
        TVariable Cls=GetVar(Tmp);
        if(Cls.Type!=TVariable::T_Class)
        {
            SetError(P_InvalidClass,0,VarName.c_str());
            return ::Error();
        }
        Cls.Data.AsString.Uppercase();
        Arguments.Add(Cls);
        for(int i=0; i<Classes.Count; i++)
        {
            if(!(Classes[i].Name==Cls.Data.AsString))
                continue;

            for(int j=0; j<Classes[i].Metods.Count; j++)
                if(Classes[i].Metods[j].Name==Func)
                {
                    if(Arguments.Count-1!=Classes[i].Metods[j].ParamCount&&
                        Classes[i].Metods[j].ParamCount!=-1)
                    {
                        SetError(P_InvalidParameters,0,Sub.Data.AsString.c_str());
                        return ::Error();
                    }

                    TVariable v;
                    if(Classes[i].Metods[j].Func)
                        v=Classes[i].Metods[j].Func(
                            Arguments.Items,Arguments.Count,this);
                    else
                    {
                        const char *r=Classes[i].Metods[j].CFunc(GetLibraryFunctions(),
                            (ParserVariable*)&v,
                            (ParserVariable**)Arguments.Items,Arguments.Count,(ParserObject*)this);
                        if(r)
                            return ::Error(r);
                    }
                    SetVar(Tmp,Arguments[Arguments.Count-1]);
                    return v;
                }
        }
    }

    SetError(P_FunctionNotFound,0,Sub.Data.AsString.c_str());
    return ::Error();
}

void TParser::GoTo(const TVariable &Var)
{
    if(Var.Type!=TVariable::T_Identifier)
    {
        SetError(P_InternalError,"TParser::GoTo - Label is not T_Identifier");
        return;
    }

    if(IsPreprocessing)
        return;

    TString Tmp=FunctionName+"::"+Var.Data.AsString;
    Tmp.Uppercase();
    for(int i=0; i<Labels.Count; i++)
    {
        if(Labels[i].Name == Tmp)
        {
            if(CycleDepth == Labels[i].Depth)
                ScriptPos = Labels[i].Position;
            else if(CycleDepth < Labels[i].Depth)
            {
                SetError(P_GotoError);
            }
            else  // > - выйти на неск. уровней из цикла
            {
                while(CycleDepth!=Labels[i].Depth)
                {
                    // 1. Определить тип текущего цикла (т.е. найти последний из вложенных циклов)
                    int w=99999999;
                    if(Whiles.Count)
                        w=ScriptPos-Whiles.Last();
                    int r=99999999;
                    if(RUntils.Count)
                        r=ScriptPos-RUntils.Last();
                    int f=99999999;
                    if(Fors.Count)
                        f=ScriptPos-Fors.Last().Position;

                    if(w<0 || r<0 || f<0)
                        SetError(P_InternalError,"GoTo - некорректное определение цикла в скрипте?!!");

                    if(w==99999999 && r==99999999 && f==99999999)
                        SetError(P_InternalError,"GoTo - полетел счетчик вложенности циклов!!!");

                    // 2. Убрать его из соответств. стека
                    if(w<r && w<f)          // While
                    {
                        PopWhile();         // пофиг, что он меняет ScriptPos
                    } else if(r<w && r<f)   // RUntil
                    {
                        PopRepeat();
                    } else if(f<w && f<r)   // For
                    {
                        PopFor();
                    } else
                        SetError(P_InternalError,"GoTo - или я глючу, или одно из двух...");
                        
                    CycleDepth--;
                }
                ScriptPos = Labels[i].Position;
            }
            return;
        }
    }
}

void TParser::SetClass(const char *ClassName, const struct FuncTable *Metods)
{
    Class Tmp;
    Tmp.Name=ClassName;
    Tmp.Name.Uppercase();

    int i=0;
    while(Metods[i].Name)
    {
        Class::Metod t;
        t.Name=Metods[i].Name;
        t.Name.Uppercase();
        t.Func=Metods[i].Function;
        t.ParamCount=Metods[i].ParamCount;
        Tmp.Metods.Add(t);
        i++;
    }
    Classes.Add(Tmp);
}

void TParser::SetGlobalVariable(const char *Name, const TVariable &vv)
{
    if(Error)
        return;

    TString n=FunctionName;
    FunctionName="";
    TVariable v;
    v.Type=TVariable::T_Identifier;
    v.Data.AsString=Name;
    v.Data.AsString.Uppercase();
    AddVar(v);
    Error=0;
    ErrString=0;
    SetVar(v,vv);
    FunctionName=n;
}

void TParser::AddArray(TVariable & Var,TVariable &Size)
{
    if(Var.Type!=TVariable::T_Identifier)
    {
        SetError(P_InternalError,"TParser::AddArray - array is not T_Identifier");
        return;
    }
    if(Size.Type!=TVariable::T_Number)
    {
        SetError(P_BadResult,0,Var.Data.AsString.c_str());
        return;
    }
    AddVar(Var);
    if(Error)
        return;
    TVariable Tmp;
    Tmp.Type=TVariable::T_Array;
    Tmp.Data.AsNumber=0;
    Tmp.Data.AsString=Var.Data.AsString;

    if(!IsPreprocessing)
    {
        Tmp.Data.UserData=CreateArray(int(Size.Data.AsNumber));

        if(Tmp.Data.UserData==0)
            SetError(P_InternalError,"TParser::AddArray - cannot create array!");
    }

    SetVar(Var,Tmp);
}

TVariable TParser::GetArray(TVariable & Var,TVariable &Pos)
{
// Пропустить лoкальные массивы при 1 прох.
    if(IsPreprocessing&&FunctionName.Length())
        return TVariable();

    if(Pos.Type!=TVariable::T_Number)
    {
        SetError(P_BadResult,0,Var.Data.AsString.c_str());
        return ::Error();
    }
    TVariable v=GetVar(Var);
    if(Error)
        return ::Error();

    if(v.Type==TVariable::T_String)
    {
        if(Pos.Data.AsNumber>v.Data.AsString.Length())
            return ::Error();
        char tmp[2]=" ";
        tmp[0]=v.Data.AsString.c_str()[int(Pos.Data.AsNumber)];
        return TVariable(tmp);
    }

    if(v.Type!=TVariable::T_Array)
    {
        SetError(P_BadArray,0,Var.Data.AsString.c_str());
        return ::Error();
    }
    return GetArrayValue(v.Data.UserData,int(Pos.Data.AsNumber));
}

void TParser::SetArray(TVariable & Var,TVariable &Pos,TVariable & Val)
{
// Пропустить лркальные массивы при 1 прох.
    if(IsPreprocessing&&FunctionName.Length())
        return;
    if(Pos.Type!=TVariable::T_Number)
    {
        SetError(P_BadResult,0,Var.Data.AsString.c_str());
        return;
    }
    TVariable v=GetVar(Var);
    if(Error)
        return;
    if(v.Type!=TVariable::T_Array)
    {
        SetError(P_BadArray,0,Var.Data.AsString.c_str());
        return;
    }
    GetArrayValue(v.Data.UserData,int(Pos.Data.AsNumber)) = Val;
}

void TParser::DeleteArray(TVariable &Var)
{
    if(Var.Type!=TVariable::T_Array)
    {
        SetError(P_InternalError,"Attempt to delete var as array! (type changed somewhere?)");
        return;
    }
    if(Var.Data.AsNumber==0)
        ::DeleteArray(Var.Data.UserData);
}

int TParser::GetErrorLine()
{
    if(Error==0)
        return -1;

    int erl=1;
    for(int i=0; i<ErrPos-1; i++)
        if(Script[i]=='\n')
            erl++;

    return erl;
}

const char * TParser::GetParameterName(const char * Func, int ParamNo)
{
    int Position=-1;

    TString Tmp=Func;
    Tmp.Uppercase();
    for(int i=0; i<Functions.Count; i++)
        if(Functions[i].Name == Tmp)
            Position=i;

    if(Position==-1)
        return 0;

    if(ParamNo>=Functions[Position].Arguments.Count)
        return 0;

    return Functions[Position].Arguments[ParamNo].c_str();
}

void TParser::PushRepeat()
{
    RUntils.Push(ScriptPos);
}

void TParser::PopRepeat()
{
    RUntils.Pop();
}

void TParser::GoToLastRepeat()
{
    ScriptPos=RUntils.Last();
}

void TParser::RemoveClass(const char *ClassName)
{
    for(int i=0; i<Classes.Count; i++)
    {
        if(Classes[i].Name==TString(ClassName))
            Classes.Delete(i--);
    }
}

void TParser::SetProperties(const char *ClassName, const struct PropTable * Prop)
{
    Class Tmp;
    Tmp.Name=ClassName;
    Tmp.Name.Uppercase();

    int i=0;
    while(Prop[i].Name)
    {
        Class::Property t;
        t.Name=Prop[i].Name;
        t.Name.Uppercase();
        t.Set=Prop[i].Set;
        t.Get=Prop[i].Get;
        Tmp.Properties.Add(t);
        i++;
    }
    Classes.Add(Tmp);
}

void TParser::PushWhile()
{
    Whiles.Push(LastWhilePos);
}

void TParser::PopWhile()
{
    ScriptPos=Whiles.Pop();
}

void TParser::PushFor()
{
    For f;
    f.Var=LastForVar;
    Fors.Push(f);
}

void TParser::SetLastForVar(const TVariable &Var)
{
    LastForVar=Var;
}

void TParser::SetStepForLastFor(const TVariable &Var)
{
    if(Var.Type!=TVariable::T_Number&&Var.Type!=TVariable::T_String)
    {
        SetError(P_InternalError,"TParser::SetStepForLastFor - Step is not T_Number");
        return;
    }

    Fors.Last().Step=Var.Data.AsNumber;
}

void TParser::SetEndForLastFor(const TVariable &Var)
{
    if(Var.Type!=TVariable::T_Number&&Var.Type!=TVariable::T_String)
    {
        SetError(P_InternalError,"TParser::SetEndForLastFor - Step is not T_Number");
        return;
    }

    Fors.Last().EndValue=Var.Data.AsNumber;
}

bool TParser::LastForIsEqual()
{
    return GetVar(Fors.Last().Var)==Fors.Last().EndValue;
}
                                       
void TParser::GoToLastFor()
{
    ScriptPos=Fors.Last().Position;
    ReturnCR=true;
    SetVar(Fors.Last().Var,GetVar(Fors.Last().Var)+Fors.Last().Step);
}

void TParser::PopFor()
{
    Fors.Pop();
}

void TParser::SetForPos()
{
    Fors.Last().Position = ScriptPos;
}

void TParser::SetRuntimeError(TVariable &V)
{
    if(V.Data.AsString.Length())
    {
        SetError(P_RuntimeError,0,V.Data.AsString.c_str());
    } else
        SetError(P_RuntimeError);
}

void TParser::GetEnvironmentFrom(const TParser &P)
{
    Classes=P.Classes;
}

int TParser::GetLineFromPos(int ErrPos)
{
    int erl=1;
    for(int i=0; i<ErrPos-1; i++)
        if(Script[i]=='\n')
            erl++;

    return erl;
}

void TParser::SetCClass(const char *ClassName, const struct CFuncTable *Metods)
{
    Class Tmp;
    Tmp.Name=ClassName;
    Tmp.Name.Uppercase();

    int i=0;
    while(Metods[i].Name)
    {
        Class::Metod t;
        t.Name=Metods[i].Name;
        t.Name.Uppercase();
        t.CFunc=Metods[i].Function;
        t.ParamCount=Metods[i].ParamCount;
        Tmp.Metods.Add(t);
        i++;
    }
    Classes.Add(Tmp);
}

void TParser::SetCProperties(const char *ClassName, const struct CPropTable * Prop)
{
    Class Tmp;
    Tmp.Name=ClassName;
    Tmp.Name.Uppercase();

    int i=0;
    while(Prop[i].Name)
    {
        Class::Property t;
        t.Name=Prop[i].Name;
        t.Name.Uppercase();
        t.CSet=Prop[i].Set;
        t.CGet=Prop[i].Get;
        Tmp.Properties.Add(t);
        i++;
    }
    Classes.Add(Tmp);
}

TVariable TParser::GetGlobalVariable(const char *Name)
{
    if(Error)
        return ::Error();

    TString n=FunctionName;
    FunctionName="";

    TVariable v;
    v.Type=TVariable::T_Identifier;
    v.Data.AsString=Name;
    v.Data.AsString.Uppercase();
    v = GetVar(v);
    
    FunctionName=n;
    return v;
}

void TParser::ClearError()
{
    Error=0;
    if(TmpErrorString)
        free(TmpErrorString);
    TmpErrorString=0;
    ErrString=0;
    FatalError=0.0;
}

} // namespace


