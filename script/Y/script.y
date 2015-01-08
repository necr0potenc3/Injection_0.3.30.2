/* Интерпретатор скрипта */

%{
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "myvar.h"
#include "myparser.h"

#define YYSTYPE TVariable
#define YYPARSE_PARAM This
#define YYLEX_PARAM This
#define yyerror(a) _yyerror(This,a)

// начальное значение размера стека
// для экономии стека при рекурсии
#define YYINITDEPTH 15

#ifdef THIS
#undef THIS
#endif

#define THIS ((TParser*)This)

void _yyerror(void *, char*);
//int yylex (YYSTYPE *lval, void *This);
int yyparse (void *);
void SkipToEndif (void *This);
void SkipToElseOrEndif (void *This);
void SkipToWend (void *This);
void SkipToNext (void *This);

%}

/* BISON Declarations */

// сделать reentarant
%pure_parser

// определения бизоновских token

// просто число или просто строковая константа
%token NUM

// слово "VAR"
%token VAR

// слово "SUB"
%token SUB

// слово "ENDSUB"
%token END_SUB

// слово "IF"
%token IF

// слово "THEN"
%token THEN

// слово "ELSE"
%token ELSE

// слово "ENDIF"
%token END_IF

// слово "GOTO"
%token GOTO

// слово "RETURN"
%token RETURN

// слово "DIM"
%token DIM

// слово "REPEAT"
%token REPEAT

// слово "UNTIL"
%token UNTIL

// слово "WHILE"
%token WHILE

// слово "WEND"
%token WEND

// слово "FOR"
%token FOR

// слово "TO"
%token TO

// слово "STEP"
%token STEP

// слово "NEXT"
%token NEXT

// слово "SAFECALL"
%token SAFECALL

// слово "FATALERROR"
%token FATALERROR

// слово "RETURN ERROR"
%token RETURNERROR

// название переменной/функции
%token IDENTIFIER

// название операторов и порядок действий
%token OR
%token AND

%left OR
%left AND
%left NOT
%left '<' '>' '='
%left '-' '+' 
%left '*' '/' 
%left NEG

/* Grammar follows */
%%
input:    /* empty string */
        | input line
;

// строка это: пусто, метка, метка и команды или просто команды
line:     '\n'
        | label '\n'
        | label commands '\n'
        | commands '\n'
;

// в случае метки - добавить ее. $1 устанавливается в yylex в TVariable(имя метки)
label:    IDENTIFIER ':'        { THIS->AddLabel($1);
                                  if(THIS->Error)
                                    YYABORT;
                                }
;

commands:
// Объявление преременных
          VAR list_of_variables

// Массивы
        | DIM list_of_arrays

// For .. To .. Step ... Next
// общую часть для всех циклов я вынес в for_closure
// тут для текущего цикла устанавливается шаг и конечное значение.
// Запоминаие имени переменной находится в for_closure
        | for_closure TO exp    { if(!THIS->IsPreprocessing)
                                  {
                                      THIS->SetStepForLastFor(1.0);
                                      THIS->SetEndForLastFor($3); 
                                      THIS->SetForPos();
                                  }
                                  THIS->CycleDepth++;
                                }
        | for_closure TO exp STEP exp
                                { if(!THIS->IsPreprocessing)
                                  {
                                      THIS->SetStepForLastFor($5);
                                      THIS->SetEndForLastFor($3);
                                      THIS->SetForPos();
                                  }
                                  THIS->CycleDepth++;
                                }

// проверка условия выхода из FOR:
// все циклы игнорируются при IsPreprocessing
        | NEXT                  { if(!THIS->IsPreprocessing)
                                  {
                                    if(!THIS->LastForIsEqual())  // если не выход из цикла
                                      THIS->GoToLastFor();       // то перейти на строчку после FOR
                                    else
                                    {
                                      THIS->CycleDepth--;        // иначе убрать этот FOR
                                      THIS->PopFor();
                                    }
                                  } else
                                      THIS->CycleDepth--;
                                }
// While ... Wend
        | WHILE exp             { if(THIS->IsPreprocessing)
                                  {
                                    THIS->CycleDepth++;
                                  }
                                  else if(!$2.IsTrue()) 	// если условие ложно,
                                  {
                                    THIS->CycleDepth++;     // увеличить счетчик вложенности циклов (для меток)
                                    SkipToWend(This);       // проскипать до WEND
                                    THIS->CycleDepth--;
                                  }
                                  else
                                  {
                                    THIS->PushWhile();      // ну а если истинно - запомнить позицию перед WHILE
                                    THIS->CycleDepth++;
                                  }
                                }

        | WEND                  { if(!THIS->IsPreprocessing) // грубо говоря: перейти на позицию 
                                    THIS->PopWhile();        // перед соответствующим WHILE - а там разберемся
                                  THIS->CycleDepth--;
                                }

// Цикл REPEAT ... UNTIL
        | REPEAT                { THIS->PushRepeat(); THIS->CycleDepth++;  }


        | UNTIL exp             { if(!$2.IsTrue()&&!THIS->IsPreprocessing)
                                  {
                                    THIS->GoToLastRepeat(); // если условие ложно то перейти на REPEAT
                                  }
                                  else
                                  {
                                    THIS->PopRepeat();      // если истинно - забыть о последнем REPEATе 
                                    THIS->CycleDepth--;
                                  }
                                }
// Условие
        | IF exp THEN           { if(!THIS->IsPreprocessing)
                                    if(!($2.IsTrue()))
                                      SkipToElseOrEndif(This);
                                   // если условие ложно - скипать до ELSE, а если ELSE нету, то до ENDIF
                                   // ну а если истинно - ничего не делать
                                }

// слово ELSE при выполнении yylex встречается ТОЛЬКО в случае если условие в IF было истинно
// (во всех других случаях оно обходится мимо в функции SkipToElseOrEndif)
        | ELSE                  { if(!THIS->IsPreprocessing)
                                    SkipToEndif(This);  // пропустить "ложную" часть
                                }

// в принципе, ENDIF нужен только для функций SkipToEndif/SkipToElseOrEndif
        | END_IF

// Goto
        | GOTO IDENTIFIER       { THIS->GoTo($2); }

// Вызов функции (подробнее ниже)
        | safe_function_call    // вызов функции, которая возможно вернет ошибку
        | function_call         // вызов функции. Если она вернет ошибку - скрипт остановится

// Объявление функций
        | SUB IDENTIFIER        { THIS->BeginCountingParameters($2);}
                                // как только встречается слово "SUB имя_пр."
                                // начать считать ее параметры (это объявление ф-и)
            '(' sub_parameters ')'
                                { THIS->AddSub($2);  // этот код выполняется после
                                  if(THIS->Error)    // завершения разбора sub_parameters
                                    YYABORT;
                                  THIS->BeginSub($2);// сказать парсеру, что вошли в процедуру
                                  if(THIS->Error)
                                    YYABORT;
                                }

        | END_SUB               { THIS->EndSub();    // вышли из процедуры
                                  if(THIS->FinishedSub)
                                    YYACCEPT;        // завершить yyparse, если он был вызван из TParser::Execute
                                }

// Возврат из процедуры (установка возвращаемого значения идет в ххх_stmt)
        | RETURN ret_stmt       { if(!THIS->IsPreprocessing)
                                    YYACCEPT;  // завершить yyparse
                                }

        | RETURNERROR err_stmt  { if(!THIS->IsPreprocessing)
                                    YYACCEPT;  // завершить yyparse
                                }

// Присвоение значения переменной
        | IDENTIFIER '=' exp    { THIS->SetVar ($1, $3);
                                  if(THIS->Error)
                                    YYABORT;
                                }
// ... массиву
        | IDENTIFIER '[' exp ']' '=' exp
                                { THIS->SetArray ($1, $3, $6);
                                  if(THIS->Error)
                                    YYABORT;
                                }
;

// собссно FOR
for_closure: FOR for_exp        { if(!THIS->IsPreprocessing)
                                    THIS->PushFor();    } // тут мы запоминаем позицию в скрипте начала FOR
;

// ну а тут мы смотрим переменную...
for_exp:  IDENTIFIER '=' exp    { THIS->SetVar ($1, $3);
                                  if(THIS->Error)
                                    YYABORT;
                                  THIS->SetLastForVar($1);
                                }
        | VAR IDENTIFIER '=' exp
                                { THIS->AddVar ($2);
                                  if(THIS->Error)
                                    YYABORT;
                                  THIS->SetVar ($2, $4);
                                  if(THIS->Error)
                                    YYABORT;
                                  THIS->SetLastForVar($2);
                                }
;

// тут устанавливается возвращаемое значение 
ret_stmt: /* empty */
        | exp                   { $$ = THIS->Result = $1; 
                                  if($1.ErrorCode()&&!THIS->IsPreprocessing)
                                    THIS->SetRuntimeError($1);
                                }
;

// а тут возвращается сигнал об ошибке
err_stmt: /* empty */
        | exp                   {
                                    if($1.Type==TVariable::T_String)
                                        THIS->Result = ::Error($1.Data.AsString.c_str());
                                    else
                                        THIS->Result = ::Error($1.Data.AsNumber);
                                }
;

// разбор параметров процедуры при объявлении
sub_parameters:
          /* empty */
        | parameter_list
;

// при вызове
call_parameters:
          /* empty */
        | call_parameter_list
;

// разборка параметров и запоминание их имен при объявлении
parameter_list:
// Увеличение счетчика параметров функции при объявлении
          IDENTIFIER            { THIS->AddParameter($1);
                                  if(THIS->Error)
                                    YYABORT;
                                }
        | parameter_list ','
          IDENTIFIER            { THIS->AddParameter($3);
                                  if(THIS->Error)
                                    YYABORT;
                                }
;

// разборка параметров и запоминание их значений при вызове ф-и
call_parameter_list:
// Увеличение счетчика параметров функции при объявлении
          exp                   { THIS->StoreParameter($1);
                                  if(THIS->Error)
                                    YYABORT;
                                }
        | call_parameter_list ','
          exp                   { THIS->StoreParameter($3);
                                  if(THIS->Error)
                                    YYABORT;
                                }
;


// это для VAR - обьявление просто переменной и переменной с присвоением ей значения
list_of_variables:
          IDENTIFIER            { THIS->AddVar ($1);
                                  if(THIS->Error)
                                    YYABORT;
                                }
        | list_of_variables ',' IDENTIFIER
                                { THIS->AddVar ($3);
                                  if(THIS->Error)
                                    YYABORT;
                                }
        | assignment_identifier
        | list_of_variables ',' assignment_identifier
;

// для DIM
list_of_arrays:
          IDENTIFIER '[' exp ']'{ THIS->AddArray ($1, $3);
                                  if(THIS->Error)
                                    YYABORT;
                                }
        | list_of_arrays ',' IDENTIFIER '[' exp ']'
                                { THIS->AddArray ($3, $5);
                                  if(THIS->Error)
                                    YYABORT;
                                }
;

// присвоение значения переменной при ее объявлении
assignment_identifier:
          IDENTIFIER '=' exp    { THIS->AddVar ($1);
                                  if(THIS->Error)
                                    YYABORT;
                                  THIS->SetVar ($1, $3);
                                  if(THIS->Error)
                                    YYABORT;
                                }       
;

// Вызов функции
function_call:
          IDENTIFIER '('        { THIS->BeginCollectingParameters();}
                                // как только встречается '(', начинается сбор 
                                // параметров
           call_parameters ')'  { $$ = THIS->CallSub($1); // при встрече ')' идет вызов процедуры
                                  THIS->EndCollectingParameters();
                                  if(THIS->Error)
                                    YYABORT;
                                  if($$.ErrorCode())
                                    THIS->SetRuntimeError($$);
                                }
;

// Safe Вызов функции (как простой вызов, только без сообщ. об ошибке)
safe_function_call: SAFECALL
          IDENTIFIER '('        { THIS->BeginCollectingParameters();}
           call_parameters ')'  { TVariable tmp = THIS->CallSub($2);
                                  THIS->EndCollectingParameters();
                                  if(THIS->Error)
                                    YYABORT;
                                  if(tmp.ErrorCode())   // тут запоминается значение
                                  {                     // ошибки (если была)
                                    THIS->FatalError=tmp;
                                    THIS->FatalError.Type=tmp.Data.AsString.Length()?TVariable::T_String :
                                        TVariable::T_Number;
                                    $$=THIS->FatalError;
                                  } else
                                  {
                                    THIS->FatalError=0.0;
                                    $$=tmp;
                                  }
                                }
;

// а это разбор математического выражения. См пример калькулятора из доки по Бизону
exp:      NUM                   { $$ = $1;         }
        | IDENTIFIER            { $$ = THIS->GetVar($1);
                                  if(THIS->Error)
                                    YYABORT;
                                }
        | IDENTIFIER '[' exp ']'{ $$ = THIS->GetArray($1,$3);
                                  if(THIS->Error)
                                    YYABORT;
                                }
        | function_call         { $$ = $1;         }
        | safe_function_call    { $$ = $1;         }
        | FATALERROR            { $$ = THIS->FatalError;    }
        | exp '+' exp           { $$ = $1 + $3;    }
        | exp '-' exp           { $$ = $1 - $3;    }
        | exp '*' exp           { $$ = $1 * $3;    }
        | exp '/' exp           { $$ = $1 / $3;    }
        | exp AND exp           { $$ = $1.IsTrue() && $3.IsTrue(); }
        | exp OR  exp           { $$ = $1.IsTrue() || $3.IsTrue(); }
        | '-' exp  %prec NEG    { $$ = -$2;        }
        | NOT exp  %prec NOT    { $$ = !$2.IsTrue(); }
        | '(' exp ')'           { $$ = $2;         }
        | exp '=' '=' exp       { $$ = $1 == $4;   }
        | exp '<' '=' exp       { $$ = ($1 < $4) || ($1 == $4);   }
        | exp '>' '=' exp       { $$ = !($1 < $4); }
        | exp '<' exp           { $$ = $1 < $3;    }
        | exp '<' '>' exp       { $$ = !($1 == $4);}
        | exp '>' exp           { $$ = !($1 < $3) && !($1 == $3); }
;
%%


#include <stdio.h>

void _yyerror (void *This, char *s)  /* Called by yyparse on error */
{
    THIS->SetParserError(s);
}

