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
#ifndef MYVAR__H
#define MYVAR__H

#include "mystring.h"

using namespace Sasha;

// Размер табуляции в файле = 4 символа

//////////////////////////////////////////////////////////////////////////////
//
//                                    КЛАССЫ
//
//////////////////////////////////////////////////////////////////////////////


// Класс реализующий нетипизированные переменные
// для скрипта. Плюс реализует кучу ненужных возможностей, необходимых
// либо для работы парсера, либо для отладки.
// Class which describes one parser variable. It is also widely used internally 
// by parser

// Поддерживаемые типы
//  double (T_Number) он же int и bool
//  string (T_String)
// Supported types
//  double (T_Number) it can also be int & bool
//  string (T_String)

// Внутренние типы
//   имя переменной/функции - возвращается yylex (T_Identifier)
//   имя класса - имя соответствующего класса    (T_Class)
//   неопределено - тип сразу после создания     (T_Unknown)
//   ошибка - возвращается после деления на 0, присвоения к константе... (T_Error)
// Internal types
//   var/func name returned by yylex (T_Identifier)
//   class (T_Class)
//   undefined type (T_Unknown)
//   error (T_Error)

///////////////////////////
//
//      TVariable
//
///////////////////////////

class TVariable
{
public:

//
// Перeменные/типы
//

    enum VarType {T_Error, T_Class, T_Number, T_String, T_Array, T_Unknown, T_Identifier};
// Тип хранимого значения
    VarType Type;

// Само хранимое значение. Лучше бы было как union, но Боря Си не дает классы
// туда ложить...
// Надо, чтобы типы лежащие здесь не зависили от вызова конструкторов
// и деструкторов, и дублировались бы memcpy() - т.е. без указателей...
    struct _silly_name_for_watcom_ {
        double AsNumber;
        TString AsString;
        void *UserData; // левые данные для класса или для T_Array
    } Data;

//
// Конструкторы
//

    TVariable ()
    {
        Type=T_Error;
        Data.AsNumber=0; //Data.AsString="";
        Data.UserData=0;
    }

    TVariable (const TVariable &v) {  *this=v; }

    TVariable (const bool &v)
    {
        Type=T_Number;
        Data.AsNumber=v;
    //    Data.AsString="";
        Data.UserData=0;
    }

    TVariable (const double &v)
    {
        Type=T_Number;
        Data.AsNumber=v;
    //    Data.AsString="";
        Data.UserData=0;
    }

    TVariable (const char *v)
    {
        Type=T_String;
        Data.AsNumber=0;
        Data.AsString=v;
        Data.UserData=0;
    }

    ~TVariable() {}

//
// Функции
//

    bool Error() {return Type==T_Error;}

    int ErrorCode() {return Type==T_Error?(int)Data.AsNumber:0;}

    bool IsTrue()
    {
        return Type==T_Number? Data.AsNumber:
            Data.AsString.Length();
    }

//
// Операторы
//

// Внутренний оп. присвоения. Для поддержки массива эл-тов класса
    TVariable& operator= (const TVariable &v)
    {
        Type=v.Type;
        Data.AsNumber=v.Data.AsNumber;
        Data.AsString=v.Data.AsString;
        Data.UserData=v.Data.UserData;
        return *this;
    }

//
// Остальное
//

friend int yyparse (void *);
friend int yylex (TVariable*, void *);
};

TVariable operator+ (const TVariable &,const TVariable &);
TVariable operator- (const TVariable &,const TVariable &);
TVariable operator* (const TVariable &,const TVariable &);
TVariable operator/ (const TVariable &,const TVariable &);
TVariable operator- (const TVariable &);
bool operator== (const TVariable &,const TVariable &);
bool operator< (const TVariable &,const TVariable &);

//
//
//
TVariable CreateClass(const char *Cl);
TVariable Error(int ErrNo=1);
TVariable Error(const char *Error);

TVariable &TakeArrayValue(const TVariable &Array, int Pos);

#endif