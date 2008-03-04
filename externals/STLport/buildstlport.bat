cd "build\lib"
REM Point this at your MSVC directory.
if EXIST "C:\program files\Microsoft Visual Studio 8\VC\bin\vcvars32.bat" ( call "C:\program files\Microsoft Visual Studio 8\VC\bin\vcvars32.bat") else (echo "Your VC directory is not the standard. Please edit the batch file")
call configure.bat -c msvc8
nmake /fmsvc.mak
nmake /fmsvc.mak install
nmake /fmsvc.mak clobber
REM We can clobber after an install and save space.
cd "..\..\"
copy user_config.h ".\stlport\stl\config\user_config.h"
