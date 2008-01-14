@echo off
echo USAGE: To setup a debug build use "setupsak D"
if /I "%1" EQU "D" (set folder=Debug) else (set folder=Release)
if /I "%1" EQU "D" set d=d 

if EXIST "..\..\externals\STlport\bin\stlport%d%.5.1.dll" if EXIST ".\%folder%\stlport%d%.5.1.dll" (echo stlport%d%.5.1.dll already exists) else copy "..\..\externals\STlport\bin\stlport%d%.5.1.dll" ".\%folder%\stlport%d%.5.1.dll"

if /I "%d%" EQU "d" set e=gd

REM Execute libsak-res-test if not already.
if EXIST "%AppData%\sakusen\data\test" (echo libsak-res-test already run) else ".\%folder%\libsakusen-resources-test"

REM Here we should tar testsrc.dir, but including 7za or tar would be stupid. However, since all it does at present is check for the eistence of the file, we just create an empty one.
if EXIST "%AppData%\sakusen\data\test\testsrc.sakusensource" (echo testsrc.sakusensource already exists) else ( echo 1 > "%AppData%\sakusen\data\test\testsrc.sakusensource" )


REM Get a tedomari config if there isn't one.
if EXIST "%Appdata%\sakusen\tedomari\config" (echo tedomari config already exists) else copy "..\..\tedomari\doc\user\config" "%Appdata%\sakusen\tedomari\config"
if EXIST "%Appdata%\sakusen\tedomari\ui.conf" (echo ui.conf already exists) else copy "..\..\tedomari\doc\user\ui.conf" "%Appdata%\sakusen\tedomari\ui.conf"

