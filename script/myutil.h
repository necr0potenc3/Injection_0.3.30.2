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
#ifndef myutilH
#define myutilH

#include <malloc.h>
#include <string.h>

//
// Классики для всякой мути... контейнеры
// Container classes
//

//////////////////////////////////////////////////////////////////////////////
//
//                                    КЛАССЫ
//
//////////////////////////////////////////////////////////////////////////////


// чтоб небыло конфликтов с чужими классами

#ifdef __BCPLUSPLUS__
# if __BCPLUSPLUS__ >= 0x400
namespace Sasha
{
#pragma warn -inl
# else
typedef int bool;
#pragma warn -inl
#define throw
# endif
#else
namespace Sasha
{
#endif

#ifdef __WATCOMC__
template <class T>
bool operator==(const T&, const T&);
#endif

//
// TList. Просто шаблонный динамический массивчик.
// Хранит элементы класса T. Сам выделяет и освобождает память.
// Непригоден для очень больших объемов, т.к. выделяет
// память для каждого элемента отдельно.
// Надеюсь, правильно вызывает деструкторы.
//
// Функции
//  Add(T &element) - добавить
//  Delete(int nomer)  - убрать #nomer
//  Empty() - true == пустой
//  Clear() - очистить
//  T& [int nomer] - вернуть эл-т #nomer (не проверяет значение nomer)
//
// Хранимые классы/обьекты должны ОБЯЗАТЕЛЬНО иметь:
//  Конструктор простой
//  Конструктор для копирования
//  оператор ==

template <class T> class TList
{
    int Size;
public:
    T **Items;
    int Count;
    TList()
    {
      Count=0;
      Size=128; Items=(T**)malloc(Size*sizeof(T*));
    }
    TList(const TList &t)
    {
      Count=0;
      Size=t.Size; Items=(T**)malloc(Size*sizeof(T*));
      for(int i=0; i<t.Count; i++)
        Add(t[i]);
    }
    TList &operator=(const TList &t)
    {
      Clear();
      free(Items);
      Count=0;
      Size=t.Size; Items=(T**)malloc(Size*sizeof(T*));
      for(int i=0; i<t.Count; i++)
        Add(t[i]);
      return *this;  
    }
    void Add(const T &w)
    {
      Items[Count]=new T(w);
      Count++;
      if(Count==Size)
      {
          Size+=128;
          Items=(T**)realloc(Items,Size*(sizeof(T*)));
      }
    }
    void Delete(int Who)
    {
      if(Who>=Count||Who<0)
          return;
      delete Items[Who];
      memmove(&Items[Who],&Items[Who+1],(Count-Who-1)*sizeof(T*));
// ^^^ сохранить порядок следования значений, если не надо, то быстрее
//    if(Count)
//      Items[Who]=Items[Count-1];
      Count--;
      if(Size-Count>128)
      {
        Size=Count+32;
        Items=(T**)realloc(Items,Size*(sizeof(T*)));
      }
    }
    int Find(const T & Who)
    {
        for (int i=0; i<Count; i++)
            if((*Items[i])==Who)
                return i;
        return -1;
    }
    int IndexOf(const T & Who) {return Find(Who);}
    int FindFrom(int Pos, const T & Who)
    {
        for (int i=Pos; i<Count; i++)
            if((*Items[i])==Who)
                return i;
        return -1;
    }
    void Clear()
    {
        for (int i=0; i<Count; i++)
            delete Items[i];
        Count=0;
    }
    ~TList()
    {
      Clear();
      free(Items);
    }
    bool IsEmpty() {return !Count;}
    T& operator[](int V) const
    {
        return *Items[V];
    }
    T& Last()
    {
        if(IsEmpty())
            throw "Sasha::TList - taking last value from empty list!!!";
        return (*this)[Count-1];
    }
};

//
// TStack. Стек. Порожден от TList
//
// Функции
//  Push(T &w)
//  Pop()

template <class T> class TStack : protected TList <T>
{
public:
    TList<T>::Count;

    TStack () : TList<T>() {}
    TStack (TStack &s) : TList<T> (s) {}

    bool IsEmpty() {return !Count;}

    void Push(const T &w) {Add(w);}
    T Pop()
    {
        if(!IsEmpty())
        {
            T t=(*this)[Count-1];
            Delete(Count-1);
            return t;
        } else
            throw "Sasha::TStack - popping from empty stack!!!";
    }
    T& Last()
    {
        if(IsEmpty())
            throw "Sasha::TStack - taking last value from empty stack!!!";
        return (*this)[Count-1];
    }
};



#ifdef __BCPLUSPLUS__
# if __BCPLUSPLUS__ >= 0x400
};   // namespace Sasha
# endif
#else
};   // namespace Sasha
#endif

#endif //#ifndef myutilH
