@echo off
if exist script.cpp del script.cpp
if exist script.obj del script.obj
if exist ..\script.obj del ..\script.obj
if exist script.h del script.h

bison.exe -S bison.my_simple script.y -d -o script.cpp --no-lines 

if not exist script.cpp goto Bad

ren script.cpp.h script.h
copy script.cpp .. >nul
copy script.h .. >nul

cd ..
bcc32 -c script.cpp
cd y

if not exist ..\script.obj goto Bad
del ..\script.obj
exit

:Bad
pause
