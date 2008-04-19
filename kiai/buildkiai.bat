REM This should point to wherever you put python and pyuic
set Python="C:\Program Files\python25"

for %%i in (*.ui) do call %Python%\pyuic4.bat -o %%~ni.py %%i

for %%i in (..\ide\msvc8\release\*.pyd) do copy %%i . 

for %%i in (..\ide\msvc8\release\lib*.dll) do copy %%i .

for %%i in (..\ide\msvc8\release\pcre*.dll) do copy %%i .

copy ..\ide\msvc8\release\stlport.5.1.dll .
copy ..\ide\msvc8\release\zlib1.dll .
copy ..\ide\msvc8\release\boost_filesystem-vc80-mt-p-1_34_1.dll .
REM This will pick up some things we don't need. I don't much care.
