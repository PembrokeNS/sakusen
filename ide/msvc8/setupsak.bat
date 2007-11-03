@echo off
echo USAGE: To setup a debug build use "setupsak D"
if /I "%1" EQU "D" (set folder=Debug) else (set folder=Release)
if /I "%1" EQU "D" set d=d

REM Copy dlls across.
if EXIST ".\%folder%\pcre3.dll" (echo pcre3.dll already exists) else copy "C:\Program Files\GNUWin32\bin\pcre3.dll" ".\%folder%\pcre3.dll"
if EXIST ".\%folder%\pcre3.dll" (echo SDL.dll already exists) else copy "..\..\externals\SDL\bin\SDL.dll" ".\%folder%\SDL.dll"

REM Some shenanigans since two versions of STLPort seem to be around.
if EXIST "..\..\externals\STlport\bin\stlport%d%.5.1.dll" if EXIST ".\%folder%\stlport%d%.5.1.dll" (echo stlport%d%.5.1.dll already exists) else copy "..\..\externals\STlport\bin\stlport%d%.5.1.dll" ".\%folder%\stlport%d%.5.1.dll"
if EXIST "..\..\externals\STlport\bin\stlport%d%.5.0.dll" if EXIST ".\%folder%\stlport%d%.5.0.dll" (echo stlport%d%.5.0.dll already exists) else copy "..\..\externals\STlport\bin\stlport%d%.5.0.dll" ".\%folder%\stlport%d%.5.0.dll"

REM Execute libsak-res-test if not already.
if EXIST "%AppData%\.sakusen\data\universe" (echo libsak-res-test already run) else ".\%folder%\libsakusen-resources-test"

REM Here we should tar testsrc.dir, but including 7za or tar would be stupid and violate the license without the source.  

REM Get a tedomari config if there isn't one.
if EXIST "%Appdata%\.sakusen\tedomari\config" (echo tedomari config already exists) else copy "..\..\tedomari\doc\user\config" "%Appdata%\.sakusen\tedomari\config"
if EXIST "%Appdata%\.sakusen\tedomari\ui.conf" (echo ui.conf already exists) else copy "..\..\tedomari\doc\user\ui.conf" "%Appdata%\.sakusen\tedomari\ui.conf"

