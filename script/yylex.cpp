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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "myvar.h"
#include "myparser.h"
#include "operators.h"

#define YYSTYPE TVariable
#define YYPARSE_PARAM This
#define YYLEX_PARAM This

#ifdef THIS
#undef THIS
#endif

#define THIS ((TParser*)This)

#include "script.h"

// THIS->IsPreprocessing - первый проход:
// Игнорирую всё между token-ами:
// IF - до ENDIF (с учетом вложенности IF-ов)
// GOTO - до '\n'

void SkipToCR (void *This)
{
Restart:
    int c=THIS->Script[THIS->ScriptPos];
    while(c!='\n' && c!='\\' && THIS->ScriptPos<THIS->ScriptSize)
        c=THIS->Script[++THIS->ScriptPos];
// Продолжение на следующей строке?
    if(c=='\\')
    {
        do {
            c = THIS->Script[++THIS->ScriptPos];
        } while(c!='\n' && THIS->ScriptPos < THIS->ScriptSize);
        ++THIS->ScriptPos;
        goto Restart;
    }

}

void PreCompile(char *Str, int Oper, char *Name)
{
    *Str=1; Str++;
    *Str=char(Oper-256);  Str++;
    int p=2;
    while(Name[p])
    {
        *Str=' ';
        Str++;
        p++;
    }
}

void SkipToEndif (void *This)    // Возвращается после ENDIF
{
    int c;

Loop:
    while (((c=THIS->Script[THIS->ScriptPos]) == ' ' || c == '\t' || c == '\r')
        && THIS->ScriptPos<THIS->ScriptSize)
        THIS->ScriptPos++;

    // Проверить на комментарий
    if(c=='#')
    {
        SkipToCR(This);
        goto Loop;
    }

    // Проверить на комментарий
    if(c==';')
    {
        SkipToCR(This);
        goto Loop;
    }

// Продолжение на следующей строке?
    if(c=='\\')
    {
        do {
            c = THIS->Script[++THIS->ScriptPos];
        } while(c!='\n' && THIS->ScriptPos < THIS->ScriptSize);
        ++THIS->ScriptPos;
        goto Loop;
    }

    // Пропустить строки
    if(c=='\"')
    {
        do {
            c = THIS->Script[++THIS->ScriptPos];
        } while(c!='\"' && THIS->ScriptPos<THIS->ScriptSize);
        ++THIS->ScriptPos;
        goto Loop;
    }
    if(c=='\'')
    {
        do {
            c = THIS->Script[++THIS->ScriptPos];
        } while(c!='\'' && THIS->ScriptPos<THIS->ScriptSize);
        ++THIS->ScriptPos;
        goto Loop;
    }

    // Оператор?
    if(c==1) // прекомпилированный?
    {
        THIS->ScriptPos+=2;
        int tmp=THIS->Script[THIS->ScriptPos-1]+256;

        if (THIS->ScriptPos >= THIS->ScriptSize) // конец???
            return;

        if(tmp==IF)
        {
            SkipToEndif(This);
            goto Loop;
        }

        if(tmp==END_IF)
            return;
    }
    int t=0;
    if((MyIsAlpha(c)||c=='&'||c=='|')&&isspace(THIS->Script[THIS->ScriptPos-1]))
    while (Operators[t].Name)
    {
        if(strncmpi(THIS->Script+THIS->ScriptPos,Operators[t].Name,
            strlen(Operators[t].Name))==0 &&
            isspace(THIS->Script[THIS->ScriptPos+strlen(Operators[t].Name)]))
            {
                THIS->ScriptPos+=strlen(Operators[t].Name);

                if (THIS->ScriptPos >= THIS->ScriptSize) // конец???
                    return;

                if(Operators[t].Type==IF)
                {
                    SkipToEndif(This);
                    goto Loop;
                }

                if(Operators[t].Type==END_IF)
                {
                    return;
                }
            }
        t++;
    }

// Это метка?
    if(MyIsAlpha(c))
    {
        int tmp=THIS->ScriptPos;

        TString VarName;
        do {
            VarName += TString((char)c);
            c = THIS->Script[++THIS->ScriptPos];
        } while((MyIsAlpha(c) || isdigit(c) || c=='.' || c=='_'
            || c=='$')&&THIS->ScriptPos<THIS->ScriptSize);

        while (((c=THIS->Script[THIS->ScriptPos]) == ' ' || c == '\t' || c == '\r')
            &&THIS->ScriptPos<THIS->ScriptSize)
            THIS->ScriptPos++;

        if(c==':')
        {
            TVariable Temp;

            Temp.Type = TVariable::T_Identifier;
            Temp.Data.AsString = VarName;
            THIS->ScriptPos++;
            THIS->AddLabel(Temp);
            goto Loop;
        } else
            THIS->ScriptPos=tmp;
    }

    // Пропустить
    THIS->ScriptPos++;
    goto Loop;
}

void SkipToElseOrEndif (void *This)    // Возвращается после ELSE/ENDIF
{
    int c;

Loop:
    while (((c=THIS->Script[THIS->ScriptPos]) == ' ' || c == '\t' || c == '\r')
        &&THIS->ScriptPos<THIS->ScriptSize)
        THIS->ScriptPos++;

    // Проверить на комментарий
    if(c=='#')
    {
        SkipToCR(This);
        goto Loop;
    }

    // Проверить на комментарий
    if(c==';')
    {
        SkipToCR(This);
        goto Loop;
    }

// Продолжение на следующей строке?
    if(c=='\\')
    {
        do {
            c = THIS->Script[++THIS->ScriptPos];
        } while(c!='\n' && THIS->ScriptPos < THIS->ScriptSize);
        ++THIS->ScriptPos;
        goto Loop;
    }

    // Пропустить строки
    if(c=='\"')
    {
        do {
            c = THIS->Script[++THIS->ScriptPos];
        } while(c!='\"'&&THIS->ScriptPos<THIS->ScriptSize);
        ++THIS->ScriptPos;
        goto Loop;
    }
    if(c=='\'')
    {
        do {
            c = THIS->Script[++THIS->ScriptPos];
        } while(c!='\''&&THIS->ScriptPos<THIS->ScriptSize);
        ++THIS->ScriptPos;
        goto Loop;
    }

    // Оператор?
    if(c==1) // прекомпилированный?
    {
        THIS->ScriptPos+=2;
        int tmp=THIS->Script[THIS->ScriptPos-1]+256;

        if (THIS->ScriptPos >= THIS->ScriptSize) // конец???
            return;

        if(tmp==IF)
        {
            SkipToEndif(This);
            goto Loop;
        }

        if(tmp==ELSE)
            return;

        if(tmp==END_IF)
            return;
    }
    int t=0;
    if((MyIsAlpha(c)||c=='&'||c=='|')&&isspace(THIS->Script[THIS->ScriptPos-1]))
    while (Operators[t].Name)
    {
        if(strncmpi(THIS->Script+THIS->ScriptPos,Operators[t].Name,
            strlen(Operators[t].Name))==0 &&
            isspace(THIS->Script[THIS->ScriptPos+strlen(Operators[t].Name)]))
            {
                THIS->ScriptPos+=strlen(Operators[t].Name);

                if (THIS->ScriptPos >= THIS->ScriptSize) // конец???
                    return;

                if(Operators[t].Type==IF)
                {
                    SkipToEndif(This);
                    goto Loop;
                }

                if(Operators[t].Type==ELSE)
                {
//                    THIS->ScriptPos-=strlen(Operators[t].Name);
                    return;
                }

                if(Operators[t].Type==END_IF)
                {
                    return;
                }
            }
        t++;
    }

    // Пропустить
    THIS->ScriptPos++;
    goto Loop;
}


void SkipToWend (void *This)    // Возвращается после ENDIF
{
    int c;

Loop:
    while (((c=THIS->Script[THIS->ScriptPos]) == ' ' || c == '\t' || c == '\r')
        && THIS->ScriptPos<THIS->ScriptSize)
        THIS->ScriptPos++;

    // Проверить на комментарий
    if(c=='#')
    {
        SkipToCR(This);
        goto Loop;
    }

    // Проверить на комментарий
    if(c==';')
    {
        SkipToCR(This);
        goto Loop;
    }

// Продолжение на следующей строке?
    if(c=='\\')
    {
        do {
            c = THIS->Script[++THIS->ScriptPos];
        } while(c!='\n' && THIS->ScriptPos < THIS->ScriptSize);
        ++THIS->ScriptPos;
        goto Loop;
    }

    // Пропустить строки
    if(c=='\"')
    {
        do {
            c = THIS->Script[++THIS->ScriptPos];
        } while(c!='\"' && THIS->ScriptPos<THIS->ScriptSize);
        ++THIS->ScriptPos;
        goto Loop;
    }
    if(c=='\'')
    {
        do {
            c = THIS->Script[++THIS->ScriptPos];
        } while(c!='\'' && THIS->ScriptPos<THIS->ScriptSize);
        ++THIS->ScriptPos;
        goto Loop;
    }

    // Оператор?
    if(c==1) // прекомпилированный?
    {
        THIS->ScriptPos+=2;
        int tmp=THIS->Script[THIS->ScriptPos-1]+256;

        if (THIS->ScriptPos >= THIS->ScriptSize) // конец???
            return;

        if(tmp==WHILE)
        {
            THIS->CycleDepth++;
            SkipToWend(This);
            THIS->CycleDepth--;
            goto Loop;
        }

        if(tmp==WEND)
            return;
    }
    int t=0;
    if((MyIsAlpha(c)||c=='&'||c=='|')&&isspace(THIS->Script[THIS->ScriptPos-1]))
    while (Operators[t].Name)
    {
        if(strncmpi(THIS->Script+THIS->ScriptPos,Operators[t].Name,
            strlen(Operators[t].Name))==0 &&
            isspace(THIS->Script[THIS->ScriptPos+strlen(Operators[t].Name)]))
            {
                THIS->ScriptPos+=strlen(Operators[t].Name);

                if (THIS->ScriptPos >= THIS->ScriptSize) // конец???
                    return;

                if(Operators[t].Type==WHILE)
                {
                    THIS->CycleDepth++;
                    SkipToWend(This);
                    THIS->CycleDepth--;
                    goto Loop;
                }

                if(Operators[t].Type==WEND)
                {
                    return;
                }
            }
        t++;
    }

// Это метка?
    if(MyIsAlpha(c))
    {
        int tmp=THIS->ScriptPos;

        TString VarName;
        do {
            VarName += TString((char)c);
            c = THIS->Script[++THIS->ScriptPos];
        } while((MyIsAlpha(c) || isdigit(c) || c=='.' || c=='_'
            || c=='$')&&THIS->ScriptPos<THIS->ScriptSize);

        while (((c=THIS->Script[THIS->ScriptPos]) == ' ' || c == '\t' || c == '\r')
            &&THIS->ScriptPos<THIS->ScriptSize)
            THIS->ScriptPos++;

        if(c==':')
        {
            TVariable Temp;

            Temp.Type = TVariable::T_Identifier;
            Temp.Data.AsString = VarName;
            THIS->ScriptPos++;
            THIS->AddLabel(Temp);
            goto Loop;
        } else
            THIS->ScriptPos=tmp;
    }

    // Пропустить
    THIS->ScriptPos++;
    goto Loop;
}

void SkipToNext (void *This)
{
    int c;

Loop:
    while (((c=THIS->Script[THIS->ScriptPos]) == ' ' || c == '\t' || c == '\r')
        && THIS->ScriptPos<THIS->ScriptSize)
        THIS->ScriptPos++;

    // Проверить на комментарий
    if(c=='#')
    {
        SkipToCR(This);
        goto Loop;
    }

    // Проверить на комментарий
    if(c==';')
    {
        SkipToCR(This);
        goto Loop;
    }

// Продолжение на следующей строке?
    if(c=='\\')
    {
        do {
            c = THIS->Script[++THIS->ScriptPos];
        } while(c!='\n' && THIS->ScriptPos < THIS->ScriptSize);
        ++THIS->ScriptPos;
        goto Loop;
    }

    // Пропустить строки
    if(c=='\"')
    {
        do {
            c = THIS->Script[++THIS->ScriptPos];
        } while(c!='\"' && THIS->ScriptPos<THIS->ScriptSize);
        ++THIS->ScriptPos;
        goto Loop;
    }
    if(c=='\'')
    {
        do {
            c = THIS->Script[++THIS->ScriptPos];
        } while(c!='\'' && THIS->ScriptPos<THIS->ScriptSize);
        ++THIS->ScriptPos;
        goto Loop;
    }

    // Оператор?
    if(c==1) // прекомпилированный?
    {
        THIS->ScriptPos+=2;
        int tmp=THIS->Script[THIS->ScriptPos-1]+256;

        if (THIS->ScriptPos >= THIS->ScriptSize) // конец???
            return;

        if(tmp==FOR)
        {
            SkipToNext(This);
            goto Loop;
        }

        if(tmp==NEXT)
            return;
    }
    int t=0;
    if((MyIsAlpha(c)||c=='&'||c=='|')&&isspace(THIS->Script[THIS->ScriptPos-1]))
    while (Operators[t].Name)
    {
        if(strncmpi(THIS->Script+THIS->ScriptPos,Operators[t].Name,
            strlen(Operators[t].Name))==0 &&
            isspace(THIS->Script[THIS->ScriptPos+strlen(Operators[t].Name)]))
            {
                THIS->ScriptPos+=strlen(Operators[t].Name);

                if (THIS->ScriptPos >= THIS->ScriptSize) // конец???
                    return;

                if(Operators[t].Type==FOR)
                {
                    SkipToNext(This);
                    goto Loop;
                }

                if(Operators[t].Type==NEXT)
                {
                    return;
                }
            }
        t++;
    }

// Это метка?
    if(MyIsAlpha(c))
    {
        int tmp=THIS->ScriptPos;

        TString VarName;
        do {
            VarName += TString((char)c);
            c = THIS->Script[++THIS->ScriptPos];
        } while((MyIsAlpha(c) || isdigit(c) || c=='.' || c=='_'
            || c=='$')&&THIS->ScriptPos<THIS->ScriptSize);

        while (((c=THIS->Script[THIS->ScriptPos]) == ' ' || c == '\t' || c == '\r')
            &&THIS->ScriptPos<THIS->ScriptSize)
            THIS->ScriptPos++;

        if(c==':')
        {
            TVariable Temp;

            Temp.Type = TVariable::T_Identifier;
            Temp.Data.AsString = VarName;
            THIS->ScriptPos++;
            THIS->AddLabel(Temp);
            goto Loop;
        } else
            THIS->ScriptPos=tmp;
    }

    // Пропустить
    THIS->ScriptPos++;
    goto Loop;
}

int FindPrevCR(char *Script,int ScriptPos)
{
    for(int i=ScriptPos; i>0; i--)
        if(Script[i]=='\n')
            return i;
    return 0;
}

//////////////////////////////////////////////////////////////////////
//
//                              YYLEX
//

int yylex (YYSTYPE *lval, void *This)
{
    TVariable Temp;
    int c;
Restart:
    if(THIS->ReturnCR)
    {
        THIS->ReturnCR=false;
        return '\n';
    }

    if(THIS->Terminated)
        return 0;

    *lval = Temp; // очистить

    /* skip white space  */
    while (((c=THIS->Script[THIS->ScriptPos]) == ' ' || c == '\t' || c == '\r')
        &&THIS->ScriptPos<THIS->ScriptSize)
        THIS->ScriptPos++;

    /* return end-of-file  */
    if (THIS->ScriptPos >= THIS->ScriptSize)
        return 0;

// Проверить на комментарий
    if(c=='#')
    {
        SkipToCR(This);
        goto Restart;
    }

    // Проверить на комментарий
    if(c==';')
    {
        SkipToCR(This);
        goto Restart;
    }

// Проверить зарезервированные слова
    if(c==1) // прекомпилированный?
    {
        int tmp=THIS->Script[THIS->ScriptPos+1]+256;
        if(tmp==WHILE)
            THIS->LastWhilePos=FindPrevCR(THIS->Script,THIS->ScriptPos);
        THIS->ScriptPos+=2;
        if(THIS->IsPreprocessing)
        {
            if(tmp==IF)
            {
//                SkipToEndif(This);
//                goto Restart;
            } else
            if(tmp==GOTO)
            {
                SkipToCR(This);
                goto Restart;
            }
        } else
        {
            if(tmp==END_SUB)
            {
                THIS->FinishedSub=true;
            } else
            if(tmp==RETURN)
            {
                THIS->FinishedSub=true;
            }
        }
        return tmp;
    }

    int t=0;    // операторы не могут быть русскими!!!
    if(isalpha(c)||c=='&'||c=='|')
    while (Operators[t].Name)
    {
        if(toupper(THIS->Script[THIS->ScriptPos])!=Operators[t].Name[0])
        {
            t++;
            continue;
        }
        if(strncmpi(THIS->Script+THIS->ScriptPos,Operators[t].Name,
            strlen(Operators[t].Name))==0 &&
            isspace(THIS->Script[THIS->ScriptPos+strlen(Operators[t].Name)]))
            {
                PreCompile(THIS->Script+THIS->ScriptPos,Operators[t].Type,Operators[t].Name);
                if(Operators[t].Type==WHILE)
                    THIS->LastWhilePos=FindPrevCR(THIS->Script,THIS->ScriptPos);
                THIS->ScriptPos+=strlen(Operators[t].Name);
                if(THIS->IsPreprocessing)
                {
                    if(Operators[t].Type==IF)
                    {
//                        SkipToEndif(This);
//                        goto Restart;
                    } else
                    if(Operators[t].Type==GOTO)
                    {
                        SkipToCR(This);
                        goto Restart;
                    }
                } else
                {
                    if(Operators[t].Type==END_SUB)
                    {
                        THIS->FinishedSub=true;
                    } else
                    if(Operators[t].Type==RETURN)
                    {
                        THIS->FinishedSub=true;
                    }
                }
                return Operators[t].Type;
            }
        t++;
    }

// Это идентификатор?
    if(MyIsAlpha(c))
    {
        TString VarName;
        do {
            VarName += TString((char)c);
            c = THIS->Script[++THIS->ScriptPos];
        } while((MyIsAlpha(c) || isdigit(c) || c=='.' || c=='_'
            || c=='$')&&THIS->ScriptPos<THIS->ScriptSize);

        VarName.Uppercase();
        Temp.Type = TVariable::T_Identifier;
        Temp.Data.AsString = VarName;
        *lval = Temp;
        return IDENTIFIER;
    }

// Число? (может начинаться с '.')
    if(isdigit(c) || c=='.')
    {
    	if(c=='0' && THIS->Script[THIS->ScriptPos+1]=='x')
    	{
	        Temp.Type = TVariable::T_Number;
    	    char *ep;
        	Temp.Data.AsNumber = strtoul (THIS->Script+THIS->ScriptPos,&ep,0);
	        THIS->ScriptPos = ep-THIS->Script;
    	    *lval = Temp;
    	} else
    	{
	        Temp.Type = TVariable::T_Number;
    	    char *ep;
        	Temp.Data.AsNumber = strtod (THIS->Script+THIS->ScriptPos,&ep);
	        THIS->ScriptPos = ep-THIS->Script;
    	    *lval = Temp;
    	}
        return NUM;
    }

// Строковая константа?
    if(c=='\"')
    {
        TString Var;
        c = THIS->Script[++THIS->ScriptPos];
        while(c!='\"'&&THIS->ScriptPos<THIS->ScriptSize)
        {
            Var += TString((char)c);
            c = THIS->Script[++THIS->ScriptPos];
        }
        ++THIS->ScriptPos;

        Temp.Type = TVariable::T_String;
        Temp.Data.AsString = Var;
        *lval = Temp;
        return NUM;
    }
    if(c=='\'')
    {
        TString Var;
        c = THIS->Script[++THIS->ScriptPos];
        while(c!='\''&&THIS->ScriptPos<THIS->ScriptSize)
        {
            Var += TString((char)c);
            c = THIS->Script[++THIS->ScriptPos];
        }
        ++THIS->ScriptPos;

        Temp.Type = TVariable::T_String;
        Temp.Data.AsString = Var;
        *lval = Temp;
        return NUM;
    }

// Продолжение на следующей строке?
    if(c=='\\')
    {
        do {
            c = THIS->Script[++THIS->ScriptPos];
        } while(c!='\n' && THIS->ScriptPos < THIS->ScriptSize);
        ++THIS->ScriptPos;
        goto Restart;
    }

    THIS->ScriptPos++;

    if(!THIS->IsPreprocessing && THIS->DbgFun)
        THIS->DbgFun(*THIS,THIS->GetLineFromPos(THIS->ScriptPos));

    /* return single chars */
    return c&255;
}

