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

#include <malloc.h>
#define __STD_STRING
// ^^ ÷òîá íåáûëî stl
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "mystring.h"

namespace Sasha
{
//---------------------------------------------------------------------------

static int Big[256];
static bool IsA[256];

static char *low="¸éöóêåíãøùçõúôûâàïğîëäæıÿ÷ñìèòüáşqwertyuiopasdfghjklzxcvbnm";
static char *hi ="¨ÉÖÓÊÅÍÃØÙÇÕÚÔÛÂÀÏĞÎËÄÆİß×ÑÌÈÒÜÁŞQWERTYUIOPASDFGHJKLZXCVBNM";
void MyToUpper_slow(char *s)
{
    while (*s)
    {
        char *tmp=strchr(low,*s);
        if(tmp)
            *s=hi[tmp-low];
        s++;
    }
}

void Init_Toupper()
{
    for(int i=0; i<256; i++)
    {
        char s[2]=" ";
        s[0]=char(i);
        MyToUpper_slow(s);
        Big[i]=s[0] ;
        IsA[i]=strchr(low,i)||strchr(hi,i);
    }
}

struct __DoInit
{
    __DoInit() {Init_Toupper();}
};

static __DoInit tmp;

void MyToUpper(register char *s)
{
    while (*s)
    {
        *s=char(Big[(unsigned char)(*s)]);
        s++;
    }
}

bool MyIsAlpha(int c)
{
    return IsA[(unsigned char)c];
}

//
// Sasha::TString
//

TString::TString(const TString &o)
{
    Buff=strdup(o.c_str());
    StringLen = strlen(Buff);
    BuffSize=StringLen+1;
}

TString::TString(const char * s)
{
    Buff=strdup(s);
    StringLen = strlen(s);
    BuffSize=StringLen+1;
}

TString::TString(const double d, int z)
{
    Buff=(char*)malloc(50);
    gcvt(d,z,Buff);
    StringLen = strlen(Buff);
    BuffSize=50;
}

TString::TString(const char c)
{
    Buff=(char*)malloc(2);
    Buff[0]=c;
    Buff[1]=0;
    StringLen = 1;
    BuffSize=StringLen+1;
}

TString &TString::operator=(const TString &o)
{
    if(Buff)
        free(Buff);
    Buff=strdup(o.c_str());
    StringLen = strlen(Buff);
    BuffSize=StringLen+1;
    return *this;
}

TString &Sasha::operator+=(TString &i, const TString &o)
{
    int NewLen=i.Length()+o.Length()+1;
    char *Tmp=(char*)malloc(NewLen);

    strcpy(Tmp,i.c_str());
    if(i.Buff)
        free(i.Buff);

    i.Buff=Tmp;
    strcat(Tmp,o.c_str());

    i.StringLen = strlen(i.Buff);

    return i;
}

TString Sasha::operator+(const TString &i, const char *o)
{
    TString Str;
    int NewLen=i.Length()+strlen(o)+1;
    char *Tmp=(char*)malloc(NewLen);

    strcpy(Tmp,i.c_str());

    Str.Buff=Tmp;
    strcat(Tmp,o);

    Str.StringLen = strlen(Str.Buff);

    return Str;
}

TString Sasha::operator+(const TString &i, const TString &o)
{
    TString Str;
    int NewLen=i.Length()+o.Length()+1;
    char *Tmp=(char*)malloc(NewLen);

    strcpy(Tmp,i.c_str());

    Str.Buff=Tmp;
    strcat(Tmp,o.c_str());

    Str.StringLen = strlen(Str.Buff);

    return Str;
}

bool Sasha::operator< (const TString &i,const TString &o)
{
    return strcmp(i.c_str(),o.c_str())<0;
}

};
