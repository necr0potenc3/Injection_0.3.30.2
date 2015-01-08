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

//1.0
#ifndef mystringH
#define mystringH
                            
#include <string.h>
#include <malloc.h>


#ifdef __WATCOMC__
#define strncmpi strnicmp
#endif

#ifdef __GNUC__
#define strncmpi strncasecmp
#endif

//---------------------------------------------------------------------------
namespace Sasha
{
// Functions which support russian letters
void MyToUpper(char *str); // Перевод строки в верх. рег. с учетом русских букв
bool MyIsAlpha(int c);    // true - символ - буква (русская или англ.)

//
// Класс, реализующий всё то, что уже реализовано в std::string, только
// не так тормозно компиляющийся... да и без наворотов
// Something like std::string from STL but which does not require STL.
//

class TString
{
private:
    int StringLen;  // не включая 0
    char *Buff;
    int BuffSize;
public:
    TString() : StringLen(0), BuffSize(0)    { Buff=0; }
    TString(const TString &Other);
    TString(const char * s);
    TString(const double d, int z=8);
    TString(const char c);
    ~TString() {if(Buff) free(Buff);}

//
// А теперь функции...
//

    const char * c_str() const
    {
        if(Buff)
            return Buff;
        else
            return ""; 
    }
    int Length() const {return StringLen;}
    void Uppercase() {MyToUpper(Buff);}

//
// Самое главное - операторы
//

    char& operator[](int V) const
    {
        return ((char*)c_str())[V];
    }
    TString &operator= (const TString &o);
friend TString &operator+= (TString &,const TString &);
friend TString operator+ (const TString &,const TString &);
friend TString &operator+= (TString &i,const char *o);
friend TString operator+ (const TString &i,const char *o);
friend bool operator< (const TString &i,const TString &o);
friend bool operator== (const TString &i,const TString &o);
};

//TString &operator+= (TString &,const TString &);
//TString operator+ (const TString &,const TString &);
//bool operator< (const TString &i,const TString &o);
inline
bool operator== (const TString &i,const TString &o)
{
    return strcmp(i.c_str(),o.c_str())==0;
}

inline
bool operator!= (const TString &i,const TString &o) {return !(i==o);}
inline
bool operator>= (const TString &i,const TString &o) {return !(i<o);}
inline
bool operator<= (const TString &i,const TString &o) {return (i<o)||(i==o);}
inline
bool operator> (const TString &i,const TString &o) {return !(i<=o);}

inline
TString &operator+= (TString &i,const char *o) {return (i += TString(o));}
//TString operator+ (const TString &i,const char *o);

}; // namespace
#endif
