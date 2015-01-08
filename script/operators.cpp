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

#include "operators.h"
#include "script.h"

// В этом файле можно менять названия операторов или добавлять новые

//---------------------------------------------------------------------------
// Длина названия оператора БОЛЬШЕ одного символа!!!
// Названия операторов - заглавными буквами!!!

TOperator Operators[]=
{
    {"VAR", VAR},
    {"SUB", SUB},
    {"ENDSUB", END_SUB},
    {"END SUB", END_SUB},
    {"IF", IF},
    {"THEN", THEN},
    {"ELSE", ELSE},
    {"ENDIF", END_IF},
    {"END IF", END_IF},
    {"GOTO", GOTO},
    {"GO TO", GOTO},
    {"RETURNERROR",RETURNERROR},
    {"RETURN ERROR",RETURNERROR},
    {"RETURN", RETURN},
    {"AND", AND},
    {"OR", OR},
    {"NOT", NOT},
    {"&&", AND},
    {"||", OR},
    {"DIM", DIM},
    {"REPEAT", REPEAT},
    {"UNTIL", UNTIL},
    {"WHILE", WHILE},
    {"WEND", WEND},
    {"FOR", FOR},
    {"TO", TO},
    {"STEP", STEP},
    {"NEXT", NEXT},
    {"SAFECALL", SAFECALL},
    {"SAFE CALL", SAFECALL},
    {"FATALERROR",FATALERROR},
    {"FATAL ERROR",FATALERROR},
    {0, 0 }
};

