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

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>


#include "myparser.h"
#include "operators.h"
#include "my_rtl.h"
#include "myfuncs.h"

//---------------------------------------------------------------------------
namespace Sasha
{
#define Imax(x,y) (((x)>(y))?(x):(y))

//////////////////////////////////////////////////////////////////
//
//                   ќписание RunTime
//       функции не провер€ют переданные параметры
//      так что могут вызвать GPF/деление на 0/глюки
//

TVariable __cdecl MySin(TVariable *v[], int n, TParser *)
{
    return sin(v[0]->Data.AsNumber);
}

TVariable __cdecl MyCos(TVariable *v[], int n, TParser *)
{
    return cos(v[0]->Data.AsNumber);
}

TVariable __cdecl MyMod(TVariable *v[], int n, TParser *)
{
    return (double)((int)v[0]->Data.AsNumber % (int)v[1]->Data.AsNumber);
}

//округление вообще до n знаков после зап€той
double ___RRR(double value,short n)
{
double coef,rs,i;
  if(n!=0)
  {
    coef=1;
    for(i=0;i<abs(n);i++) coef*=(n>0)?10:0.1;
    rs=floor(value*coef+0.5)/coef;
  }
  else rs=floor(value);
  return rs;
}

TVariable __cdecl TWround(TVariable *v[], int n, TParser *)
{
    return ___RRR(v[0]->Data.AsNumber,v[1]->Data.AsNumber);
}

TVariable __cdecl TWint(TVariable *v[], int n, TParser *)
{
    return (double)(int)v[0]->Data.AsNumber;
}

TVariable __cdecl MyStrLen(TVariable *v[], int n, TParser *)
{
    return (double)v[0]->Data.AsString.Length();
}

TVariable __cdecl MyStr(TVariable *v[], int n, TParser *)
{
    char Buff[256];
    if(n<1||n>2)
        return Error("Invalid number of parameters to STR");
    if(n==2)
        sprintf(Buff,"%.*f",int(v[1]->Data.AsNumber),v[0]->Data.AsNumber);
    else
    {
        sprintf(Buff,"%f",v[0]->Data.AsNumber);
        while(strlen(Buff)>1&&Buff[strlen(Buff)-1]=='0')
            Buff[strlen(Buff)-1]=0;
        if(strlen(Buff)>1&&Buff[strlen(Buff)-1]=='.')
            Buff[strlen(Buff)-1]=0;
    }
    return TVariable(Buff);
}

TVariable __cdecl MyVal(TVariable *v[], int n, TParser *)
{
    return atof(v[0]->Data.AsString.c_str());
}

TVariable __cdecl MyClass(TVariable *v[], int n, TParser *)
{
    TVariable tmp;
    tmp.Type=TVariable::T_Class;
    tmp.Data.AsString=v[0]->Data.AsString;
    return tmp;
}

TVariable __cdecl MyInt(TVariable *v[], int n, TParser *)
{
    return floor(v[0]->Data.AsNumber);
}

TVariable __cdecl MyIsString(TVariable *v[], int n, TParser *)
{
    return v[0]->Type==TVariable::T_String;
}

TVariable __cdecl MyIsNumber(TVariable *v[], int n, TParser *)
{
    return v[0]->Type==TVariable::T_Number;
}

TVariable __cdecl MyIsArray(TVariable *v[], int n, TParser *)
{
    return v[0]->Type==TVariable::T_Array;
}

TVariable __cdecl MyIsClass(TVariable *v[], int n, TParser *)
{
    return v[0]->Type==TVariable::T_Class;
}

#if defined(_WIN32) && !defined(__GNUC__)
extern "C" void __stdcall OutputDebugStringA(const char*);
#else
#define OutputDebugStringA puts
extern "C" void puts(const char*);
#endif

TVariable __cdecl DbgMsg(TVariable *v[], int n, TParser *)
{
    for(int i=0; i<n; i++)
    {
        if(v[i]->Type==TVariable::T_Number)
            OutputDebugStringA(gcvt(v[i]->Data.AsNumber,8,"sdfsdfsdfdsfsdfdsfsdfdsf"));
        else
            OutputDebugStringA(v[i]->Data.AsString.c_str());
    }
    return TVariable(true);
}

TVariable __cdecl MyStrToMoney(TVariable *v[], int n, TParser *)
{
    if(v[0]->Type!=TVariable::T_String)
        return ::Error("Invalid parameter to 'StrToMoney'");
    return GetMoney(v[0]->Data.AsString.c_str());
}

TVariable __cdecl MyMoneyToStr(TVariable *v[], int n, TParser *)
{
    if(n<1 || n>4)
        return ::Error("Invalid number of parameters to 'MoneyToStr'");
    if(v[0]->Type!=TVariable::T_Number)
        return ::Error("Invalid parameter(s) to 'MoneyToStr'");
    double d=v[0]->Data.AsNumber;
    int w=22;
    bool l=true;
    int de=2;
    if(n!=1)
        l=false;
    if(n>1)                                       
        w=v[1]->Data.AsNumber;
    if(n>2)
        l=v[2]->IsTrue();
    if(n>3)
        de=v[3]->Data.AsNumber;
    char Buff[100]="***Error";

    return PrintMoney(Buff,d,w,l,de);
}

TVariable __cdecl MyNumberToStr(TVariable *v[], int n, TParser *)
{
    if(n<1 || n>3)
        return ::Error("Invalid number of parameters to 'NumberToStr'");
    if(v[0]->Type!=TVariable::T_Number)
        return ::Error("Invalid parameter(s) to 'NumberToStr'");
    double d=v[0]->Data.AsNumber;
    int w=22;
    bool l=true;
    if(n!=1)
        l=false;
    if(n>1)
        w=v[1]->Data.AsNumber;
    if(n>2)
        l=v[2]->IsTrue();
    char Buff[100]="***Error";

    return PrintNumber(Buff,d,w,l);
}

TVariable __cdecl MyToday(TVariable *v[], int n, TParser *)
{
  time_t tt;
  time(&tt);
  tm *t=localtime(&tt);
  return EncodeDate(t->tm_year,t->tm_mon,t->tm_mday);
}

TVariable __cdecl MyCurrentTime(TVariable *v[], int n, TParser *)
{
  time_t tt;
  time(&tt);
  tm *t=localtime(&tt);
  return EncodeTime(t->tm_hour,t->tm_min,t->tm_sec,0);
}

TVariable __cdecl MyHour(TVariable *v[], int n, TParser *)
{
int h,m,s,ms;
  if(n!=1)
        return ::Error("Invalid number of parameters to 'Hour'");
  DecodeTime(v[0]->Data.AsNumber,h,m,s,ms);
  return (double)h;
}

TVariable __cdecl MyMinute(TVariable *v[], int n, TParser *)
{
int h,m,s,ms;
  if(n!=1)
        return ::Error("Invalid number of parameters to 'Hour'");
  DecodeTime(v[0]->Data.AsNumber,h,m,s,ms);
  return (double)m;
}

TVariable __cdecl MySecond(TVariable *v[], int n, TParser *)
{
int h,m,s,ms;
  if(n!=1)
        return ::Error("Invalid number of parameters to 'Hour'");
  DecodeTime(v[0]->Data.AsNumber,h,m,s,ms);
  return (double)s;
}

TVariable __cdecl MyDate(TVariable *v[], int n, TParser *)
{
    if(n!=1 && n!=3)
        return ::Error("Invalid number of parameters to 'Date'");
    int d=0;
    int m=0;
    int y=0;
    if(n==1)
    {
        GetDate(v[0]->Data.AsString.c_str(),d,m,y);
    } else
    {
        d=v[0]->Data.AsNumber;
        m=v[1]->Data.AsNumber;
        y=v[2]->Data.AsNumber;
    }
    double tmp=EncodeDate(y,m,d);
    if(tmp==-1)
        return ::Error("Invalid date passed to 'Date'");
    return tmp;
}

TVariable __cdecl MyDateToStr(TVariable *v[], int n, TParser *)
{
    int y,m,d;
    char s[15];
    int w=2+1+2+1+4;
    if(n>2||n==0)
        return ::Error("Invalid number of parameters to 'DateToStr'");
    DecodeDate(v[0]->Data.AsNumber,y,m,d);
    if(n>1)
        w=v[1]->Data.AsNumber;
    return PrintDate(s,d,m,y,w);
}

TVariable __cdecl MyDayOf(TVariable *v[], int n, TParser *)
{
    int y,m,d;
    DecodeDate(v[0]->Data.AsNumber,y,m,d);
    return (double)d;
}

TVariable __cdecl MyMonthOf(TVariable *v[], int n, TParser *)
{
    int y,m,d;
    DecodeDate(v[0]->Data.AsNumber,y,m,d);
    return (double)m;
}

TVariable __cdecl MyYearOf(TVariable *v[], int n, TParser *)
{
    int y,m,d;
    DecodeDate(v[0]->Data.AsNumber,y,m,d);
    return (double)y;
}

TVariable __cdecl MyIsLeapYear(TVariable *v[], int n, TParser *)
{
    return IsLeapYear(v[0]->Data.AsNumber);
}

TVariable __cdecl MyFormatStr(TVariable *v[], int n, TParser *)
{
    if(n!=2&&n!=3)
        return ::Error("Invalid number of argumentss to 'FormatStr'");

    int nn=(int)v[1]->Data.AsNumber;
    bool Right=false;
    if(nn<0)
    {
        nn=-nn;
        Right=true;
    }
    char *Buff=new char[Imax(v[0]->Data.AsString.Length(),nn)+1];
    strcpy(Buff,v[0]->Data.AsString.c_str());
    if(!Right)
        while((int)strlen(Buff)<nn)
            strcat(Buff," ");
    else
    {
        TString S=Buff;
        while(S.Length()<nn)
        {
            S=" "+S;
        }
        strcpy(Buff,S.c_str());
    }
    if(int(strlen(Buff))>nn)
    {
        Buff[nn]=0;
        if(n==2 ||(n==3 && !v[2]->IsTrue()))
        {
            if(nn>3)
            {
                Buff[nn-1]='.';
                Buff[nn-2]='.';
                Buff[nn-3]='.';
            }
            else
                memset(Buff,'*',nn);
        }
    }
    TVariable tmp(Buff);
    delete Buff;
    return tmp;
}

TVariable __cdecl MyLTrim(TVariable *v[], int n, TParser *)
{
    const char *s=v[0]->Data.AsString.c_str();
    while(*s==' ')
        s++;
    return s;
}

TVariable __cdecl MyRTrim(TVariable *v[], int n, TParser *)
{
    char *Buff=new char[v[0]->Data.AsString.Length()+2];
    Buff[0]=0;
    strcpy(Buff+1,v[0]->Data.AsString.c_str());
    char *s=Buff+strlen(Buff+1);
    while(*s==' ')
        s--;
    s[1]=0;
    TVariable vv(Buff+1);
    delete Buff;
    return vv;
}

TVariable __cdecl MyTrim(TVariable *v[], int n, TParser *)
{
    const char *s=v[0]->Data.AsString.c_str();
    while(*s==' ')
        s++;
    char *Buff=new char[strlen(s)+2];
    Buff[0]=0;
    strcpy(Buff+1,s);
    char *ss=Buff+strlen(Buff+1);
    while(*ss==' ')
        ss--;
    ss[1]=0;
    TVariable vv(Buff+1);
    delete Buff;
    return vv;
}

TVariable __cdecl MyLeft(TVariable *v[], int n, TParser *)
{
    int l=v[1]->Data.AsNumber;
    if(l>v[0]->Data.AsString.Length())
        l=v[0]->Data.AsString.Length();
    TString s(v[0]->Data.AsString);
    s[l]=0;
    return s.c_str();
}

TVariable __cdecl MyRight(TVariable *v[], int n, TParser *)
{
    int r=v[1]->Data.AsNumber;
    if(r>v[0]->Data.AsString.Length())
        r=v[0]->Data.AsString.Length();
    return v[0]->Data.AsString.c_str()+(v[0]->Data.AsString.Length()-r);
}

TVariable __cdecl MyMid(TVariable *v[], int n, TParser *)
{
    int l=v[1]->Data.AsNumber;
    if(l>v[0]->Data.AsString.Length())
        l=v[0]->Data.AsString.Length();
    TString s(v[0]->Data.AsString.c_str()+l);
    int len=v[2]->Data.AsNumber;
    if (len>s.Length())
        len=s.Length();
    s[len]=0;
    return s.c_str();
}

//////////////////////////////////////////////////////////////////Sasha::
TParser::FuncTable __RunTime[]=
{
    {"sin", MySin, 1},
    {"cos", MyCos, 1},
    {"mod", MyMod, 2},
    {"int",TWint, 1},
    {"round",TWround, 2},
    {"strlen", MyStrLen, 1},
    {"len", MyStrLen, 1},
    {"str", MyStr, -1},
    {"val", MyVal, 1},
    {"MoneyToStr",MyMoneyToStr,-1},
    {"StrToMoney",MyStrToMoney,1},
    {"NumberToStr",MyNumberToStr,-1},
    {"int", MyInt, 1},
    {"class", MyClass, 1},    // только дл€ тестовых целей!!! реальные классы должны иметь конструкторы
    {"DbgMsg", DbgMsg, -1},
    {"IsString", MyIsString, 1},
    {"IsNumber", MyIsNumber, 1},
    {"IsArray", MyIsArray, 1},
    {"IsClass", MyIsClass, 1},
    {"Date", MyDate, -1},
    {"IsLeapYear", MyIsLeapYear, 1},
    {"Day", MyDayOf, 1},
    {"Month", MyMonthOf, 1},
    {"Year", MyYearOf, 1},
    {"DateToStr", MyDateToStr, -1},
    {"FormatStr", MyFormatStr, -1},
    {"Trim", MyTrim, 1},
    {"LTrim", MyLTrim, 1},
    {"RTrim", MyRTrim, 1},
    {"Right", MyRight, 2},
    {"Left", MyLeft, 2},
    {"Mid", MyMid, 3},
    {"Today",MyToday,0},
    {"CurrentTime",MyCurrentTime,0},
    {"Hour",MyHour,1},
    {"Minute",MyMinute,1},
    {"Second",MySecond,1},
    {0, 0, 0}
};

//////////////////////////////////////////////////////////////////
//
//              —лужебные процедуры
//

void TParser::InitGlobals()
{
    SetGlobalVariable("pi",TVariable(3.14159265358979323846));
    SetGlobalVariable("true",TVariable(1.0));
    SetGlobalVariable("false",TVariable(0.0));
}

void TParser::InitFunctions()
{
    SetFunctions(__RunTime);
}

///////////////////////////////////////////////////////////////////////////
//
//             лассики
//

class TMyFile
{
    FILE *File;
    TString FileName;
    TMyFile(const TMyFile&);
public:
    TMyFile(const char *FileN) : FileName(FileN), File(0) {}
    ~TMyFile() {if(File) fclose(File);}
    bool Open() {return (File=fopen(FileName.c_str(),"r+t"))!=0;}  // true = Ok, false = ошибочка
    bool Create() {return (File=fopen(FileName.c_str(),"w+t"))!=0;}

    bool Eof() {return feof(File);}  
    bool Write(const TString &S) {return fputs(S.c_str(),File)!=EOF;}
    bool WriteLn(const TString &S) {return Write(S)&&Write("\n");}
    bool Read(TString &S);       // читает всю строку до конца
    bool ReadString(TString &S); // читает слово до пробела/TAB
    bool ReadNumber(double &S);  // читает число

    void Close() {if(File) fclose(File); File=0;}
};

bool TMyFile::Read(TString &S)
{
    char Buff[10240]="";
    bool Bad=fgets(Buff,10240,File);
    S=Buff;
    return Bad;
}

bool TMyFile::ReadString(TString &S)
{
    char Buff[10240]="";
    bool Bad=fscanf(File,"%s",Buff)==EOF;
    S=Buff;
    return !Bad;
}

bool TMyFile::ReadNumber(double &S)
{
    bool Bad=fscanf(File,"%lf",&S)==EOF;
    return !Bad;
}

TVariable __cdecl MyFile(TVariable *v[], int n, TParser *)
{
    TVariable vv=CreateClass("File");
    vv.Data.UserData=new TMyFile(v[0]->Data.AsString.c_str());
    return vv;
}

TVariable __cdecl MyOpen(TVariable *v[], int n, TParser *)
{
    TMyFile *F=(TMyFile*)v[n-1]->Data.UserData;
    if(F==0) return ::Error("Invalid file");
    return F->Open();
}

TVariable __cdecl MyCreate(TVariable *v[], int n, TParser *)
{
    TMyFile *F=(TMyFile*)v[n-1]->Data.UserData;
    if(F==0) return ::Error("Invalid file");
    return F->Create();
}

TVariable __cdecl MyClose(TVariable *v[], int n, TParser *)
{
    TMyFile *F=(TMyFile*)v[n-1]->Data.UserData;
    if(F==0) return ::Error("Invalid file");
    F->Close();
    delete F;
    v[0]->Data.UserData=0;
    return TVariable(0.0);
}

TVariable __cdecl MyRead(TVariable *v[], int n, TParser *)
{
    TMyFile *F=(TMyFile*)v[n-1]->Data.UserData;
    TString s="";
    if(F==0) return ::Error("Invalid file");
    if(!F->Read(s))
        return ::Error("File read error"); 
    return s.c_str();
}

TVariable __cdecl MyReadNumber(TVariable *v[], int n, TParser *)
{
    TMyFile *F=(TMyFile*)v[n-1]->Data.UserData;
    double d=0;
    if(F==0) return ::Error("Invalid file");
    if(!F->ReadNumber(d))
        return ::Error("File read error");
    return d;
}

TVariable __cdecl MyReadString(TVariable *v[], int n, TParser *)
{
    TMyFile *F=(TMyFile*)v[n-1]->Data.UserData;
    TString s="";
    if(F==0) return ::Error("Invalid file");
    if(!F->ReadString(s))
        return ::Error("File read error");
    return s.c_str();
}

TVariable __cdecl MyWrite(TVariable *v[], int n, TParser *)
{
    TMyFile *F=(TMyFile*)v[n-1]->Data.UserData;
    if(F==0) return ::Error("Invalid file");
    if(v[0]->Type==TVariable::T_String)
        return F->Write(v[0]->Data.AsString);
    else if(v[0]->Type==TVariable::T_Number)
    {
        char Buff[256];
        return F->Write(gcvt(v[0]->Data.AsNumber,8,Buff));
    } else
        return ::Error("Invalid type passed to File.Write - should be string or number");
}

TVariable __cdecl MyEof(TVariable *v[], int n, TParser *)
{
    TMyFile *F=(TMyFile*)v[n-1]->Data.UserData;
    if(F==0) return ::Error("Invalid file");
    return F->Eof();
}

TVariable __cdecl MyWriteLn(TVariable *v[], int n, TParser *)
{
    TMyFile *F=(TMyFile*)v[n-1]->Data.UserData;
    if(F==0) return ::Error("Invalid file");
    if(n==1)
        return F->WriteLn("");
    else if(n==2)
    {
        if(v[0]->Type==TVariable::T_String)
            return F->WriteLn(v[0]->Data.AsString);
        else if(v[0]->Type==TVariable::T_Number)
        {
            char Buff[256];
            return F->WriteLn(gcvt(v[0]->Data.AsNumber,8,Buff));
        } else
            return ::Error("Invalid type passed to File.WriteLn - should be string or number");
    }

    return ::Error("Extra parameter(s)");
}

TParser::FuncTable __File[] =
{
    {"File",MyFile,1},
    {"Open",MyOpen,0},
    {"Create",MyCreate,0},
    {"Close",MyClose,0},      // он же деструктор
    {"ReadLn",MyRead,0},
    {"ReadNumber",MyReadNumber,0},
    {"ReadString",MyReadString,0},
    {"Write",MyWrite,1},
    {"Eof",MyEof,0},
    {"WriteLn",MyWriteLn,-1}, // 0 или 1
    {0,0,0}
};

void TParser::InitClasses()
{
    SetClass("File",__File);
}

} //namespace
