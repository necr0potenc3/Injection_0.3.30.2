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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include "myfuncs.h"

//---------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
//
//      Функции по работе с числами
//

#define Imax(x,y) (((x)>(y))?(x):(y))
struct NPOS
{
  int pos;
  char sym;
};

#define N_MONEY_NPOS 7
static const NPOS money_npos[N_MONEY_NPOS]={{3,'.'},{7,0x27},{11,0x27},
                                  {15,0x27},{19,0x27},{23,0x27},{27,0x27}};
#define NOKOP_N_MONEY_NPOS 7
static const NPOS nokop_money_npos[N_MONEY_NPOS]={{4,0x27},{8,0x27},
                                  {12,0x27},{16,0x27},{20,0x27},{24,0x27}};
#define N_DATE_NPOS 2
//const NPOS date_npos[N_DATE_NPOS]={{2,'/'},{5,'/'}};

static NPOS __money_pos(int pos,int Decimals,int kop)
{
NPOS np={0,0};
  if(kop)
  {
    if((pos>=0)&&(pos<=N_MONEY_NPOS))
    {
      np=money_npos[pos];
      np.pos+=Decimals-2;
    }
  }
  else
  {
    if((pos>=0)&&(pos<=NOKOP_N_MONEY_NPOS)) np=nokop_money_npos[pos];
  }
  return np;
}

#pragma warn -inl
static inline double RRR(double value,int n)
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

// Вывод в буфер s размером по крайней мере width+1 числа d в формате
// шириной width с выравниванием по левому краю, если left==true
char *PrintNumber(char *s,double d,int width,bool left)
{
int maxlen=width+1;
char s1[80];
int len;
double absd;
  if(width>=maxlen) width=maxlen-1;
  absd=fabs(d);
  absd=absd-int(absd);
  if(absd<0.0000001)
  {
    sprintf(s1,"%.0lf",d);
  }
  else
  {
    sprintf(s1,"%lf",d);
    len=strlen(s1);
    while(s1[--len]=='0') s1[len]=0;
    if(s1[len]=='.') s1[len]=0;
  }
  if((len=strlen(s1))<=width)
  {
    len=width-len;
    if(left||(!len)) strcpy(s,s1);
    else
    {
      memset(s,' ',len);
      strcpy(s+len,s1);
    }
  }
  else
  {
    memset(s,'*',width);
    s[width]=0;
  }
  return s;
}

// напечатать в буфер s2 число d шириной width с выравниванию по
// левому краю, если left==true и числом дес. знаков decimals после ','
// Разбивание на группы по 3 цифры
char *PrintMoney(char *s2,double d,int width,bool left,int decimals)
{
bool kop=true;
int maxlen=width+1;
char s1[512],*p,s[256],fs[64];
int i,j,l,ip,k;
double d1,mn,an;
int minus=0;
  d=RRR(d,decimals);
  if(decimals<=0) kop=0;
  else if(decimals>40) decimals=40;
  if(width>=maxlen) width=maxlen-1;
  if(width>3)
  {
    if(d<0)
    {
      if(d<=(-0.01)) minus=1;
      d=-d;
    }
    an=0.001;
    if(kop&&(decimals>2)) for(k=2;k<decimals;k++) an/=10;
    d1=floor(d+an);
    if(kop)        //"%.0lf.%2.2i"
    {
      for(k=0,mn=1;k<decimals;k++) mn*=10;
      sprintf(fs,"%%.0lf.%%%i.%ii",decimals,decimals);
      sprintf(s1,fs,d1,Imax(0,int((d-d1)*mn+0.5)));
//      MessageBoxFmt("format=%s,str=%s",fs,s1);
    }
    else sprintf(s1,"%.0lf",d1);
    memset(s,' ',sizeof(s));
    i=0;
    s[width-i++]=0;
    if(kop)
    {
      k=0;
      while((k++)<decimals) s[width-i++]='0';
      s[width-i++]='.';
    }
    s[width-i++]='0';
    if((p=strchr(s1,'.'))!=0&&kop)
    {
      k=0;
      while((k++)<decimals) s[width-k]=p[decimals-k+1];
      l=p-s1;
    }
    else l=strlen(s1);
    for(i=l-1,j=(kop)?2+decimals:0;(i>=0)&&(j<width);i--,j++)
    {
      for(ip=0;ip<((kop)?N_MONEY_NPOS:NOKOP_N_MONEY_NPOS);ip++)
        if(j==(__money_pos(ip,decimals,kop).pos))
        {
          s[width-j]=__money_pos(ip,decimals,kop).sym;
          j++;
        }
      if(j<=width) s[width-j]=s1[i];
    }
    if(j<width) s[width-j]=(minus)?'-':' ';
    if(left)
    {
  //    p=strrchr(s,'-');
  //    if(!p)
      p=strrchr(s,' ');
      strncpyz(s2,(p)?p+1:s,maxlen);
    }
    else strncpyz(s2,s,maxlen);
    if(((j>=width)&&minus)||(i>=0)) strset(s2,'*');
  }
  else
  {
    memset(s2,'*',width);
    s2[width]=0;
  }
  return s2;
}

// Получить число из сгенеренной PrintMoney строки
double GetMoney(const char s[])
{
int i,l;
int pnt_sign=0,pi=0;
double d=0;
  l=strlen(s);
  i=0;
  while(i<l)
  {
    if(isdigit(s[i]))
    {
      d=d*10+(s[i]-'0');
      if(pnt_sign) pi++;
    }
    else
    {
      if((s[i]!=0x27)&&(s[i]>' ')&&(s[i]!='-'))
      {
        if((s[i]=='.')&&(!pnt_sign)) pnt_sign=1;
        else  break;
      }
    }
    i++;
  }
  if(strchr(s,'-')) d=-d;
  while((pi--)>0) d=d/10;
  return d;
}

// Преобразование даты в строку s
char *PrintDate(char *s,int d,int m,int y,int width)
{
int maxlen=12;
char s1[20];
  if(width<8)
  {
    memset(s,'*',width);
    s[width]=0;
    return s;
  }
  if(width>=10) sprintf(s1,"%2.2i/%2.2i/%4.4i",d,m,y);
  else sprintf(s1,"%2.2i/%2.2i/%2.2i",d,m,y%100);
  strncpyz(s,s1,maxlen);
  return s;
}

// Получение даты из строки
int GetDate(const char s[],int &d,int &m,int &y)
{
  char *e;
  d=strtod(s,&e);
  if(e&&*e)
      m=strtod(e+1,&e);
  if(e&&*e)
      y=atoi(e+1);
  if((y>50)&&(y<=99)) y+=1900;
  if((y>=0)&&(y<=50)) y+=2000;
  if((d>0)&&(d<=31)&&(m>0)&&(m<=12)&&(y>=1920)&&(y<=2020)) return 0;
  else return 1;
}

////////////////////////////////////////////////////////////////////////
//
//  Функции по переводу даты/времени в double == TDateTime
//

static const double SecsPerDay = 24 * 60 * 60;
static const double MSecsPerDay = SecsPerDay * 1000;

//{ Days between 1/1/0001 and 12/31/1899 }
static const double DateDelta = 693594;

static int MonthTable[][13] =
    {{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
     {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

bool IsLeapYear(int Year)
{
  return (Year % 4 == 0) && ((Year % 100 != 0) || (Year % 400 == 0));
}

double EncodeTime(int Hour, int Min, int Sec, int MSec)
{
double Time;
  if((Hour < 24) && (Min < 60) && (Sec < 60) && (MSec < 1000))
  {
    Time = (Hour * 3600000 + Min * 60000 + Sec * 1000 + MSec) / MSecsPerDay;
  }
  return Time;
}

double EncodeDate(int Year,int  Month,int Day)
{
  int I;
  int *DayTable;
  double Date=-1;
  DayTable = MonthTable[IsLeapYear(Year)];
  if ((Year >= 1) && (Year <= 9999) && (Month >= 1) && (Month <= 12) &&
    (Day >= 1) && (Day <= DayTable[Month]))
  {
    for (I = 1; I<Month; I++)
        Day+=DayTable[I];
    I = Year - 1;
    Date = I * 365 + I / 4 - I / 100 + I / 400 + Day - DateDelta;
  }
  return Date;
}

static void DivMod(int Dividend, int Divisor,
  int &Result, int &Remainder)
{
    Result = Dividend/Divisor;
    Remainder = Dividend%Divisor;
}

void DecodeTime(double Time,int &Hour, int &Min, int &Sec, int &MSec)
{
int MinCount, MSecCount,TimeS;
  TimeS=int(Time*MSecsPerDay)%int(MSecsPerDay);
  MinCount=TimeS/60000;
  MSecCount=TimeS%60000;
  Hour=MinCount/60;
  Min=MinCount%60;
  Sec=MSecCount/1000;
  MSec=MSecCount%1000;
}

void DecodeDate(double Date, int &Year, int &Month, int &Day)
{
const int D1 = 365;
const int  D4 = D1 * 4 + 1;
const int  D100 = D4 * 25 - 1;
const int  D400 = D100 * 4 + 1;
  int Y, M, D, I;
  int T;
  int *DayTable;

  T = int(Date+DateDelta);
  if (T <= 0)
  {
    Year = 0;
    Month = 0;
    Day = 0;
  } else
  {
    T--;
    Y = 1;
    while(T >= D400)
    {
      T-=D400;
      Y+=400;
    }
    DivMod(T, D100, I, D);
    if (I == 4)
    {
      I--;
      D+=D100;
    }
    Y+=I * 100;
    DivMod(D, D4, I, D);
    Y+=I * 4;
    DivMod(D, D1, I, D);
    if (I == 4)
    {
      I--;
      D+=D1;
    };
    Y+=I;
    DayTable = MonthTable[IsLeapYear(Y)];
    M = 1;
    while (true)
    {
      I = DayTable[M];
      if (D < I) break;
      D-=I;
      M++;
    }
    Year = Y;
    Month = M;
    Day = D + 1;
  }
}
