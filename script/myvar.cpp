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

#include "myvar.h"

TVariable operator+ (const TVariable &i,const TVariable &v)
{
    TVariable Temp=i;
    if(i.Type==v.Type)
    {
        if(i.Type==TVariable::T_Number)
            Temp.Data.AsNumber+=v.Data.AsNumber;
        else if(i.Type==TVariable::T_String)
            Temp.Data.AsString+=v.Data.AsString;
    } else
        return ::Error("Invalid operation for this type");
    return Temp;
}

TVariable operator- (const TVariable &i,const TVariable &v)
{
    TVariable Temp=i;
    if(i.Type==v.Type)
    {
        if(i.Type==TVariable::T_Number)
            Temp.Data.AsNumber-=v.Data.AsNumber;
        else
            return ::Error("Invalid operation for this type");
    } else
        return ::Error("Invalid operation for this type");
    return Temp;
}

TVariable operator* (const TVariable &i,const TVariable &v)
{
    TVariable Temp=i;
    if(i.Type==v.Type)
    {
        if(i.Type==TVariable::T_Number)
            Temp.Data.AsNumber*=v.Data.AsNumber;
        else
            return ::Error("Invalid operation for this type");
    } else
        return ::Error("Invalid operation for this type");
    return Temp;
}

TVariable operator/ (const TVariable &i,const TVariable &v)
{
    TVariable Temp=i;
    if(i.Type==v.Type)
    {
        if(i.Type==TVariable::T_Number)
            Temp.Data.AsNumber/=v.Data.AsNumber;
        else
            return ::Error("Invalid operation for this type");
    } else
        return ::Error("Invalid operation for this type");
    return Temp;
}

TVariable operator- (const TVariable &i)
{
    TVariable Temp=i;
    if(i.Type==TVariable::T_Number)
        Temp.Data.AsNumber=-Temp.Data.AsNumber;
    else
        return ::Error("Invalid operation for this type");
    return Temp;
}

bool operator== (const TVariable &i,const TVariable &v)
{
    if(i.Type!=v.Type)
        return false;
    if(i.Type==TVariable::T_Number)
        return i.Data.AsNumber == v.Data.AsNumber;
    else
        return i.Data.AsString == v.Data.AsString;
}

bool operator< (const TVariable &i,const TVariable &v)
{
    if(i.Type!=v.Type)
        return false;
    if(i.Type==TVariable::T_Number)
        return i.Data.AsNumber < v.Data.AsNumber;
    else
        return i.Data.AsString < v.Data.AsString;
}

TVariable CreateClass(const char *Cl)
{
    TVariable tmp;
    tmp.Type=TVariable::T_Class;
    tmp.Data.AsString=Cl;
    return tmp;
}

TVariable Error(int ErrNo)
{
    TVariable Tmp((double)ErrNo);
    Tmp.Type=TVariable::T_Error;
    return Tmp;
}

TVariable Error(const char *Error)
{
    TVariable Tmp(Error);
    Tmp.Type=TVariable::T_Error;
    Tmp.Data.AsNumber=1;
    return Tmp;
}

struct MyArray
{
    int Size;
    TVariable *Var;
};

void *CreateArray(int size)
{
    if(size<=0)
        return 0;

    MyArray *a=new MyArray;

    a->Size=size+1;
    a->Var=new TVariable[size+1];

    return a;
}

TVariable &GetArrayValue(void *Array,int pos)
{
    static TVariable Error;
    Error=::Error("Attempting to access array out of bounds");

    MyArray *a=(MyArray*)Array;

    if(a->Size<pos)
        return Error;

    return a->Var[pos];
}

void DeleteArray(void *Array)
{
    MyArray *a=(MyArray*)Array;
    delete[] a->Var;
    delete a;
}

TVariable &TakeArrayValue(const TVariable &v, int Pos)
{
    if(v.Type!=TVariable::T_Array)
    {
        static TVariable Err=::Error("Array is not TVariable::T_Array");
        return Err;
    }
    return GetArrayValue(v.Data.UserData,Pos);
}

