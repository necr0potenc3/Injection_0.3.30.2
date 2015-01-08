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
#ifndef MY_PARSER_H
#define MY_PARSER_H

#include "myvar.h"
#include "myutil.h"
#include "mycsubs.h"

#include <pshpack4.h>

#ifndef __has_c_func_table
#define __has_c_func_table
// из-за возникающих глюков при включении mycsubs.h переношу копию определения
// структур сюда. При изменении mycsubs.h надо обновить и это определение
// The copy of structures from mycsubs.h, because one file should never include
// both mycsubs.h and myparser.h and I don't want to move them to separate header
struct CFuncTable
{
    const char *Name;
    ExternalCFunction *Function;
    int ParamCount;    	// -1 - не проверять их кол-во
};						// -1 - variable number of args

struct CPropTable
{
    const char *Name;
    ExternalCFunction *Get;
    ExternalCFunction *Set;
};
#endif

#include <poppack.h>

namespace Sasha
{

// Размер табуляции в файле = 4 символа
// TAB size = 4

//////////////////////////////////////////////////////////////////////////////
//
//                                    КЛАССЫ
//                                    CLASSES
//
//////////////////////////////////////////////////////////////////////////////

// Тип функции, вызываемой из скрипта. Функция должна сама проверять количество
// переданных параметров, и если их больше/меньше - возвр. Error
// Typedef of a function callable from script. Function should check types of
// parameters itself and return "Error" if they are invalid.
class TParser;
typedef TVariable __cdecl ExternalFunction(TVariable *Params[], int ParamCount, TParser *Parser);

//
// Класс, реализующий выполнение скрипта.
// Script parser class
//

class TParser
{
//
// Внутренние классы
// Internal structures
//

// Структурка по хранению списка переменных имеющихся в данный момент в скрипте
// имена переменных, функций и всего остального могут быть одинаковыми (напр.)
// в процедуре aaa может быть метка aaa и переменная aaa.
// Struct which contains info about one variable currently accessible from script
    struct Variable
    {
        TString Name;       // имя пер.
        					// var name
// Если переменная локальна в процедуре, ее имя ввиде:
// "имя_процедуры::имя_переменной-". Для глобальной - просто "::имя"
//                               ^- только при рекурсии
// Local vars are named "func_name::var_name-". "-" is added every time function 
// is called resursevely. Global vars are "::name"
        TVariable Var;      // значение
        					// value
        Variable (const TString &N) : Name(N) {Var.Type=TVariable::T_Unknown;}
        Variable (const Variable &V) : Name(V.Name), Var(V.Var) {}
    };

// Список всех переменных.
// List of all active variables
    TList <Variable> Variables;

// Структурка для хранения инфы об одной функции определенной в скрипте
// Struct describing one script function
    struct Function
    {
        TString Name;   // имя ф-и как оно определено в скрипте
        				// func name
        int Position;   // смещение в Script в байтах до начала тела ф-и
        				// offset to function start in bytes in script
        TList<TString> Arguments;   // названия всех аргументов
        							// argument names
        Function() {}
        Function(const Function &f) : Name(f.Name),
            Position(f.Position), Arguments(f.Arguments) {}
    };

// Список всех функций и названий их параметров
// List of all functions & their params
    TList <Function> Functions;

// инфа о метке
// info about label 
    struct Label
    {
        TString Name;   // имя
        				// label name. 
        int Position;   // в это значение будет установлен ScriptPos
                        // после goto на эту метку
                        // ScriptPos would take this value after GoTo this label
        int Depth;      // вложенность метки в цикл. Используется для поддержки
                        // goto наружу из цикла и предотвращения goto внутрь
                        // цикла. Например в скрипте:
                        // Depth of this label inside loops. Used to jump 
                        // outside loops. Example:
// sub aaa()
//   L1:        ; Depth==0
//   for a=1 to 10
//     L2:      ; Depth==1
//     for b=1 to 10
//       L3:    ; Depth==2
//       while c<10
//         L4:  ; Depth==3
//       wend
//     next
//   next
//
        Label() {}
        Label(const Label &L) : Name(L.Name), Position(L.Position), Depth(L.Depth) {}
    };

// Список меток и их положение
// List of all labels
    TList <Label> Labels;

// Внутренний временный список значении аргументов для передачи их функции
// (мог бы сделать его локальным в процедуре)
// Internal. Temporary list of parameters passed to function.
    TList <TVariable> Arguments;

// Хранит значения Arguments при рекурсии
// Function arguments stack. Used on recursion.
    TStack <TList <TVariable> > ArgStk;

// Инфа о внешнем классе используемом из скрипта
// на один класс может заводиться несколько таких структур, хотя можно
// все положить и в одну
// Info about class/functions accessible from script. There can be several such
// structs describing one class, though they could be joined into one.
    struct Class
    {
        TString Name;   // имя класса или "" если просто список глобальных ф-й
        				// Class name or "" if contains lict of functions
        				// (i.e. global functions)
        struct Metod    // инфа об одном методе
        				// describes one method/function
        {
            TString Name;               // имя метода
            							// method/func name
            ExternalFunction *Func;     // один из этих указателей кажет
            ExternalCFunction *CFunc;   // на тело процедуры
            							// one of these ptrs points to the 
            							// function. The second is 0

            int ParamCount;             // точное количество параметров
            // для передачи в процедуру. Без учета "this". Если==-1, то
            // не проверяется
            // Exact number of parameters (without "this"). If -1 - not checked

            Metod() : Func(0), CFunc(0), ParamCount(0), Name("") {}
            Metod(const Metod&m) : Func(m.Func), CFunc(m.CFunc),
                Name(m.Name), ParamCount(m.ParamCount) {}
        };
        struct Property // инфа об одной property
        				// info about one property
        {
            TString Name;   // имя (может совпадать с мененм)
                            // функции 
                            // property name. The class may have both function 
                            // and a property with equal names
            ExternalFunction *Get;  // ф-я для получения значения property (или 0)
            						// Get property function. Or 0
            ExternalFunction *Set;  // ф-я для установки значения (или 0 если только для чтения)
            						// Set function. Or 0
            ExternalCFunction *CGet;    // то же самое, только в случае с С-функциями
            ExternalCFunction *CSet;	// the same but if property is defined in C file (not C++)
            // В случае С++ функций (Get||Set)!=0 && CGet==CSet==0
            // при С-функциях Get==Set==0, (CGet||CSet)!=0
            Property() : Get(0), Set(0), CGet(0), CSet(0), Name("") {}
            Property(const Property &P) : Get(P.Get), Set(P.Set),
                CGet(P.CGet), CSet(P.CSet), Name(P.Name) {}
        };
        TList <Metod> Metods;
        TList <Property> Properties;
        Class() {}
        Class(const Class &c) : Name(c.Name), Metods(c.Metods), Properties(c.Properties) {}
    };

// Внутренняя табличка классов/функций/propert-ей
// Internal list of classes, funcions, properties
    TList <Class> Classes;

// Позиции для циклов While..Wend и Repeat..Until
// запоминают позиции куда будет установлено ScriptPos после выполнения Wend/Until
// Stack of loops. Stores ScriptPos of the first line after While/Repeat
    TStack <int> Whiles;
    TStack <int> RUntils;

// инфа о цикле FOR
// FOR loop
    struct For
    {
        TVariable Var;      // имя переменной по которой идет цикл
        					// the loop variable
        TVariable EndValue; // ее конечное значение
        					// its end value
        TVariable Step;     // шаг
        					// step
        int Position;       // сюда устанавливается ScriptPos при след. итерации
        					// ScriptPos of next iteration
        For() {}
        For(const For &f) : Var(f.Var), EndValue(f.EndValue), Step(f.Step),
            Position(f.Position) {}
    };

    TStack <For> Fors;

//
// Внутренние функции
// Internal functions
//

// внутренняя: устатавливает строку ошибки по данному коду
// sets the runtime error code 
    void SetError(int errCode, const char *Str=0, const char *Param=0);

//
// Внутренние переменные
// Internal vars
//

// Имя текущей выполняющейся функции
// Current running function
    TString FunctionName;

public:
// Т.к. кривой Борланд глючит с namespace (причем в GCC все работает)
// приходится объявить переменные для yylex как public и убрать friend
// Made this stuff public because C++Builder4's support for namespaces & friends
// is not so perfect as in GCC. So all these vars should be considered private

// Указатель на текст скрипта. Выделяется динамически в SetScript.
// Script text. Allocated in SetScript.
    char *Script;
// Размер этого скрипта
// Script size in bytes
    int ScriptSize;
// Текущая позиция интерпретатора yylex в *Script. Отсчитывается в байтах от начала
// Script.
// Current script position in bytes from "Script" beginning
    int ScriptPos;

// Когда true - yyparse завершает интерпретацию данной процедуры и возвращается
// используется только при втором проходе (т.е. при выполнении)
// Flag used in yyparse to stop parsing SUB on return
    bool FinishedSub;

// Хранит возвращенный функцией результат или строчку об ошибке, если функция
// ничего не вернула
// Function result or "Error"
    TVariable Result;

// Флажок принудительного убийства интерпретатора.
// Когда true - парсер прекращает работу (чтоб остановить зациклившийся скрипт)
// Set this to true to force parser to terminate script when it hangs
    bool Terminated;

// Говорит yylex вернуть одно '\n' (для некоторых циклов и поддержки меток)     
// Forces yylex to think that next char is '\n'. Used in loops/labels
    bool ReturnCR;

// Текущий уровень вложенности циклов (в это значение устанавливается Depth
// для меток)
// Current depth for labels
    int CycleDepth;

private:

// позиция в скрипте, при выполнении которой была обнаружена ошибка. (хотя обычно
// реальная позиция ошибки немного раньше)
// Error position in script
    int ErrPos;

// Буфер для хранения сообщения об ошибке
// Buffer with error string
    char *TmpErrorString;

// Счетчик количества параметров, передаваемых функции
// Хранит так же их названия. Нужен во время PreProcess для первоначального сбора информации
// о параметрах функции
// List of function parameters used during PreProcess
    TList<TString> Parameters;

public:
// Строки, соответствующие кодам ошибок (см. myparser.cpp)
// Error codes & strings
    static const char *ErrorStrings[];

    enum ErrorCodes {
        P_NoError,      // нету
        				// none
        P_Syntax,       // синт. ошибка
        				// syntax error
        P_VarDefined,   // переменная уже определена
        				// var with the same name is already defined
        P_VarUndefined, // переменная не определена
        				// undefined var
        P_InternalError,// внутренняя ошибка
        				// internal parser bug
        P_BadResult,    // неверный результат или неправильная операция для данного типа
        				// invalid operation for this type
        P_LabelDefined, // метка уже определена в другом месте процедуры
        				// label with the same name already defined
        P_BadLabelPlace,// определение метки вне процедуры
        				// trying to define label outside function
        P_FunctionNotFound, // функция не найдена
        					// function not found
        P_BadArgsCount, // неправильное кол-во параметров при вызове ф-и
        				// invalid number of params to a function in script
        P_InvalidClass, // использование неправильной переменной при выз. метода
        				// Trying to call class member function on a non-class var
        P_BadArray,     // использование переменной в кач. массива
        				// trying to use normal var as an array
        P_RuntimeError, // внешняя ф-я вернула ошибку
        				// external function returned error
        P_GotoError,    // попытка прыгнуть внутрь цикла
        				// trying to goto inside loop
        P_LabelNotFound,// метки нету
        				// no such label
        P_InvalidParameters // неправ. кол-во параметров для внешней ф-и
        					// invalid number of params to external func
    };

// код первой встретившийся ошибки, 0 == нет ошибки
// error code
    int Error;

// Значение последней ошибки для SAFECALL.
// last error value on SAFECALL
    TVariable FatalError;

// Описание ошибки в том виде как оно будет возвращено
// (хранит либо строковую константу, либо указывает на TmpErrorString) 
// Full error description
    const char *ErrString;

//
// Конструктор
// Constructor
//

    TParser();
    ~TParser();

////////////////////////////////////////////////////////////////////////
//
// Функции для пользователей этого класса
// Functions which should be used to control script execution
//
////////////////////////////////////////////////////////////////////////

// Выполниене файлика с язычком:
// Order of function calls to run script:
//  1. Вгрузить текст в память
//  1. Load script somehow
//  2. Создать TParser
//  2. Create TParser instance
//  3. TParser.SetScript (указатель на счит. файл, размер)
//  3. TParser.SetScript (ptr to loaded script, script size)
//  4. если надо: GetEnvironmentFrom(парсер)
//  5. Use GetEnvironmentFrom(parser) to set global var information from other instance
//     SetFunctions() - установить внешние функции
//                    - set external functions callable from script
//     SetClass() - установить внешний класс
//				  - set external classes
//     ....
//     SetGlobalVariable() - установить константы (могут быть классами)
//                         - add global variables and class instances
//     PreProcess() - именно в таком порядке!!! PreProcess устанавливает
//      глобальные переменные. При этом может понадобиьтся вызов внешних классов
//                  - should be called after all these funcstions
//  5. TParser.Execute (выполняемая функция)
//                     (execute function from script)
//   ......
//  6. TParser.Execute (выполняемая функция)
//  между вызовами Execute значения пер-х сохраняются.
//  TParser instance remembers values of script global vars between calls to
//  Execute


// Установить выполняемый код. Создает копию переданного буфера
//  Script - в процедуре делается его копия.
//  ScriptSize - в байтах.
// Sets the script text. Functions makes a copy of a passed buffer.
    bool SetScript(const char *Script, int ScriptSize);

// Разобрать скрипт и подготовить его к выполнению.
// Возвр. false == ошибка
// Preprocess script. Returns false on syntax error.
    bool PreProcess();

// Выполнить. Возвр. результат возвращенный функцией или в случае если ничего не вернуто
// тип: T_String, значение "Result is undefined". В случае ошибки: T_Error, AsNumber
// или AsString - код или текст ошибки
// Начинает выполнение с заданной функции, возвращается результат
// Runs specified function. Returns function result.
    TVariable Execute(const char *Func);

// Выполнить с передачей параметров функции
// Начинает выполнение с заданной функции с данными параметрами.
// Runs the given function passing it parameters
    TVariable Execute(const char *Func, TVariable *Params[], int ParamCount);

// Прервать зациклившийся скрипт. После вызова этой ф-и Execute()
// завершается с какой-нибудь ошибкой (обычно Parse error).
// Force script to terminate. May be used to stop hang scripts. This would
// simulate syntax error on next executing command.
    void Terminate() {Terminated=true;}

// Вернуть номер строки с ошибкой. Отсчет от 1
// Return the line with error. First line == 1
    int GetErrorLine();

// Установить значения всех глобальных переменных, классов, внешних функций
// данного парсера из другого парсера. Предварительно свои таблицы функций и т.д.
// очищаются
// Copy global variables from the other script instance.
    void GetEnvironmentFrom(const TParser &P);

// Добавить и установить значение глобальной переменной. Таким образом
// устанавливаются значения констант True, False, PI...
// Sets the value of a predefined global var. TRUE and FALSE constants
// are set in this way
    void SetGlobalVariable(const char *Name, const TVariable &v);

// Получить значение глобальной пер. (локальные не возвращаются).
// если не найдена вернется Тип T_Error 
// Return the global var value. If no such var - return type==T_Error
    TVariable GetGlobalVariable(const char *Name);

// Табличка функций
// Передается адрес массива таких структур. В случае класса - должна
// быть в таблице функция, совпадающая по имени с именем класса.
// Она будет возвращать переменную TVariable с типом T_Class, с
// Data.AsString="имя_класса", Data.AsNumber и Data.UserData - заполняет
// своими данными, необходимыми для идентификации объекта.
// Парсеру все равно, как называть фактические
// конструктор и деструктор, т.к. они вызываются вручную из
// скрипта, например:
// VAR Variable = TMySuperClass() - объявление переменной типа TMySuperClass
//     Variable.Create()  - вызвать конструктор
//     Variable.Destroy() - вызов деструктора
// Конструктор должен возвращать TVariable с Type==T_Class
// и Data.AsString == имя класса.
// Функции передается массив указателей на TVariable. Последний элемент -
// тот, через которого производится вызов (вроде this). Конструктору
// он не передается!!!
// Последний элемент массива должен содержать оба поля структуры = 0
//
    struct FuncTable
    {
        char *Name;
        ExternalFunction *Function;
        int ParamCount;    // -1 - не проверять их кол-во
    };

// Установить адреса и названия функций для вызова из интерпретируемой
// программы.
// Добавляет переданные адреса во внутр. таблицу.
// Set the external functions
    void SetFunctions(const struct FuncTable *Table) {SetClass("",Table);}

// Установить адреса и названия методов класса для вызова из интерпретируемой
// программы. Для каждого класса - свой вызов SetClass
// Addes external class methods
    void SetClass(const char *ClassName, const struct FuncTable *Metods);

// Табличка пропертей. Первая функция вызывается при чтении значения, вторая -
// при его записи. Формат при вызове - такой же, как при вызове методов класса.
// Последний эл-т массива для передачи в SetProperties - 0,0,0
// Функция Get принимает единственным аргументом класс,
// Функция Set принимает первым аргументом устанавливаемое значение,
//  вторым - класс.
    struct PropTable
    {
        char *Name;
        ExternalFunction *Get;
        ExternalFunction *Set;
    };

// Добавить таблицу пропертей для данного класса.
// Adds properties to external class
    void SetProperties(const char * Class, const struct PropTable * Prop);

// Убрать класс, более не требующийся, или перед заменой адресов его функций.
// Если ClassName=="" - убрать глобальные функции
// Removes external class (both methods and properties)
    void RemoveClass(const char *ClassName);

// Возвращает имя параметра #ParamNo для функции Function, определенной
// в скрипте. Возвращает 0, если ParamNo слишком большой или функция не найдена
// Returns the name of ParamNo paremeter to Function. Or zero 
    const char *GetParameterName(const char *Function, int ParamNo);

// Функции для написания внешних классов скрипта на Си
// не использующие классов. Так же они могут лежать в DLL писаных на
// всяких левых языках - см mycsubs.h
// Functions to set external class definition from DLL written on non-C++ language
    void SetCFunctions(const struct CFuncTable *Table) {SetCClass("",Table);}
    void SetCClass(const char *ClassName, const struct CFuncTable *Metods);
    void SetCProperties(const char * Class, const struct CPropTable * Prop);

// Установить функцию, которая будет вызываться после выполнеия каждой строки
// скрипта.
// Функция возвращает TRUE==продолжить выполнение, FALSE - прервать
// Принимает параметры Parser, который ее вызва, № строки которая
// будет выполняться следующей.
//  Для отключения отладки - SetDebuggerFunction(0);
// Set the function called after each line of script is executed
    typedef bool DebuggerFunction(TParser &P, int Line);
    void SetDebuggerFunction(DebuggerFunction *F) {DbgFun=F;}

// Очистить статус ошибки
    void ClearError();

//////////////////////////////////////////////////////////////////////
//
// Внутренние функции
// Internal functions used in yyparse(). Should be considered private
//
//////////////////////////////////////////////////////////////////////

public: // had to make this public to access from yyparse generated with BISON

// void DbgFun(TParser *THIS,int CurrentLine);
// функция, которая вызывается после (или перед - не помню) выполнения строки CurrentLine
    DebuggerFunction *DbgFun;

// возвращает номер строки по заданной позиции в Script[] 
// Returns the line number from position in script
    int GetLineFromPos(int Pos);

// Флажок, указывающий, yylex что идет предварительный разбор.
// Когда он true - выполнение операторов пропускается.
    bool IsPreprocessing;

// Функции, вызывающиеся из кода, сгенерированного по script.y    

// По работе с переменными

// Присвоение значения переменой с именем хранящимся в Dest
// Устанавливает ошибку - если Dest не хранит имя переменной (T_Variable) или
// тип Source не значение переменной
    void SetVar (TVariable &Dest, const TVariable &Source);

// Получить значение переменной
    TVariable GetVar (TVariable &Name);

// Добавление новой переменной. С учетом локальных переменных в функции.
    void AddVar (TVariable &New);

// Установить флаг о синтаксической ошибке. Вызывается из ^этих^ ф-й при ошибке 
    void SetParserError(char *Err);

// Функции использующиеся для объявления процедурок в скрипте    

// Начинает подсчет количества параметров процедуры при ее объявлении
    TString TmpFunction;    // используется во время первого прохода вместо FunctionName
    void BeginCountingParameters(TVariable &v);

// Добавляет новый параметр в функцию
    void AddParameter(TVariable &New);

// Добавление функции. Она же EndCountingParameters()
    void AddSub (TVariable &New);

// Указывает о начале функции для обработки локальных переменных
// Устанавливает FunctionName
    void BeginSub (TVariable &Sub);

// Завершает текущую процедурку. Грохает локальные переменные
    void EndSub();

// Добавляет метку, локальную для процедуры. Не ругается, если метка добавлена
// несколько раз. (например если метка лежит внутри цикла)
    void AddLabel(TVariable & Var);

// Внутренняя работа с массивами    

// Добавить массив    
    void AddArray(TVariable & Var,TVariable &Size);

// Удалить массив
    void DeleteArray(TVariable &Var);

// Получить значение эл-та массива
    TVariable GetArray(TVariable & Var,TVariable &Pos);

// Установить значение эл-та массива
    void SetArray(TVariable & Var,TVariable &Pos,TVariable & Val);

// Вызов процедуры из скрипта (локальной или внешней)    

// Начать запоминание параметров вызова процедуры
    void BeginCollectingParameters();
    void EndCollectingParameters();

// Запомнить значение параметра
    void StoreParameter(TVariable &Var);

// Внутренний вызов процедурки
    TVariable CallSub(TVariable &Sub);

// Метки

// Переход на метку
    void GoTo(const TVariable & V);

// Функции вызываемые перед выполнением скрипта    

// Инициализировать RunTime функции
    void InitFunctions();

// Классы
    void InitClasses();

// Переменные    
    void InitGlobals();

// Поддержка циклов    

// Запомнить текущую позицию для работы Repeat ... Until()
    void PushRepeat();

// Убрать из стека последнюю запомненную позицию Repeat без перехода на нее
    void PopRepeat();

// Перейти на Repeat
    void GoToLastRepeat();

// Эапомнить позицию While для возврата на нее
    void PushWhile();
    int LastWhilePos;  // позиция перед словом WHILE (устанавливаю в yylex) 

// Вернуться на последнюю позицию While и убрать ее из стека
    void PopWhile();

// Запомнить позицию перед TO с переменной LastForVar
    TVariable LastForVar;
    void PushFor();

// Установить LastForVar
    void SetLastForVar(const TVariable &Var);

// Установить шаг для последнего FOR
    void SetStepForLastFor(const TVariable &Var);

// Установить TO для последнего FOR
    void SetEndForLastFor(const TVariable &Var);

// true == надо выйти из FOR (вызывается при NEXT)
    bool LastForIsEqual();

// Перейти на TO от последнего FOR (вызывается при NEXT)
    void GoToLastFor();

// Убрать последний FOR
    void PopFor();

// запомнить позицию последнего FOR
    void SetForPos();

// Установить внутреннее состояние об ошибке по заданной переменной    
    void SetRuntimeError(TVariable &V);

// убрано из-за глючности борланда:    
//    friend int ::yylex (TVariable *lval, void *This);
};


}; // namespace
#endif
