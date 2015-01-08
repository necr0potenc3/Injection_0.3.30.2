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

// 1.0
//---------------------------------------------------------------------------
#ifndef mycsubsH
#define mycsubsH

#include <pshpack4.h>

// Этот заголовочный файл должен включаться в файлы DLL, содержащие
// реализацию классов/функциий для скрипта
// Соответствующий mycsubs.cpp включать в проект с DLL НЕЛЬЗЯ!!!
// он должен включаться в DLL/EXE с парсером

// This file is used for writing functions/classes accessible from script
// on non-C++ language. It is also used to move these functions to DLL
// compiled with other C-dialect. Exporting classes would cause problems.


//---------------------------------------------------------------------------

struct LibraryFunctions;
// Тип переменной парсера совершенно не отражающий реальную структуру данных
typedef struct __ParserVariable {} ParserVariable;
typedef struct __ParserObject {} ParserObject;

// тип функции, реализующей метод внешнего класса. Возвращает NULL = Ok,
// в случае ошибки - указатель на строку с ошибкой
typedef const char* __cdecl ExternalCFunction(LibraryFunctions *Table,
    ParserVariable *Result, ParserVariable *Params[], int ParamCount, ParserObject *Parser);

#ifndef __has_c_func_table
#define __has_c_func_table
// Находится здесь, чтоб не нужно было включать myparser.h
struct CFuncTable
{
    const char *Name;
    ExternalCFunction *Function;
    int ParamCount;    // -1 - не проверять их кол-во
};
struct CPropTable
{
    const char *Name;
    ExternalCFunction *Get;
    ExternalCFunction *Set;
};
#endif

// Эта структурка передается в функции, реализующие классы
// Они предназначениы для работы с ParserVariable

typedef int __cdecl __RtlFi(const ParserVariable*);
typedef const char* __cdecl __RtlFs(const ParserVariable*);
typedef double __cdecl __RtlFd(const ParserVariable*);
typedef void* __cdecl __RtlFv(const ParserVariable*);
typedef void __cdecl __RtlFe(const ParserVariable*, int);
typedef void __cdecl __RtlFss(const ParserVariable*, const char*);
typedef void __cdecl __RtlFsd(const ParserVariable*, double);
typedef void __cdecl __RtlFsv(const ParserVariable*, const void *);
typedef ParserVariable* __cdecl __RtlFsp(const ParserVariable*, int);
typedef ParserVariable* __cdecl __RtlFp();
typedef void __cdecl __RtlFpd(const ParserVariable*);
typedef ParserVariable* __cdecl __RtlFpp(const ParserVariable*);

typedef bool __cdecl __RtlPex(ParserObject *Parser, ParserVariable *Result,
    const char *Func, ParserVariable *Params[], int ParamCount);
typedef void __cdecl __RtlPv(ParserObject *Parser);
typedef void __cdecl __RtlPcp(ParserObject *Parser, const char* Name, const ParserVariable *Val);
typedef ParserVariable* __cdecl __RtlPcg(ParserObject *Parser, const char* Name);

typedef void __cdecl __RtlPpf(ParserObject *Parser, const struct CFuncTable *Table);
typedef void __cdecl __RtlPcf(ParserObject *Parser, const char *Class,const struct CFuncTable *Table);
typedef void __cdecl __RtlPpp(ParserObject *Parser, const char * Class, const struct CPropTable * Prop);

// Должен частично соответствовать енуму из myvar.h
enum ParserVarType {T_Error, T_Class, T_Number, T_String, T_Array};

struct LibraryFunctions
{
    int Size;   // размер структуры для проверки версий
// Возвращает тип переменной ParserVariable, переданной параметром
    __RtlFi *GetType;
// Установить тип переменной
    __RtlFe *SetType;

// Вернуть указатель на const char* соответствующий данной ParserVariable
    __RtlFs *GetString;
// Вернуть double соответствующй ...
    __RtlFd *GetNumber;
// Получить свои данные
    __RtlFv *GetUserData;

// Установить строку в переменной в данное значение (тип пер. не меняется!!!)
    __RtlFss *SetString;
// Установить значение числа ...
    __RtlFsd *SetNumber;
// Установить свои данные
    __RtlFsv *SetUserData;

// Заполнить ParserVariable, в соответствии с именем класса
    __RtlFss *CreateClass;

// Получить значение эл-та массива
    __RtlFsp *TakeArrayValue;

// Создать новый объект ParserVariable
    __RtlFp  *NewVar;
// Удалить созданный объект
    __RtlFpd  *DelVar;
// Сделать новый объект ParserVariable являющися копией переданного
// Указатель после использования нужно удалить DelVar
    __RtlFpp  *DupVar;


// Вызов функции из скрипта для данного объекта парсера с передачей параметров
// Возвращает true==Ok, false - Result содержит значение ошибки
    __RtlPex *Execute;

// Завершает зациклившийся скрипт имитируя синтакс. ошибку
    __RtlPv *Terminate;

// Копии соответствующих функций из TParser
    __RtlPcp *SetGlobalVariable;
    __RtlPpf *SetFunctions;
    __RtlPcf *SetClass;
    __RtlPpp *SetProperties;

// Получить значение гл. переменной. Возвращенный указатель
// нужно удалить DelVar
    __RtlPcg *GetGlobalVariable;

// Вернуть TVariable с данной ошибкой
    __RtlFss *Error;
};

#include <poppack.h>

LibraryFunctions *GetLibraryFunctions();

#endif
