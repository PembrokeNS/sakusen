# Microsoft Developer Studio Project File - Name="libsakusen" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libsakusen - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libsakusen.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libsakusen.mak" CFG="libsakusen - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libsakusen - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libsakusen - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libsakusen - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBSAKUSEN_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "." /I "stlfixes" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBSAKUSEN_EXPORTS" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "libsakusen - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBSAKUSEN_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\..\STLport\stlport" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBSAKUSEN_EXPORTS" /D "WIN32" /FD /GZ /LDd /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"..\..\..\STLport\libs"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "libsakusen - Win32 Release"
# Name "libsakusen - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Main sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\action.cpp
# End Source File
# Begin Source File

SOURCE=..\..\action.h
# End Source File
# Begin Source File

SOURCE=..\..\angle.h
# End Source File
# Begin Source File

SOURCE=..\..\ballistic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ballistic.h
# End Source File
# Begin Source File

SOURCE=..\..\ballistictype.h
# End Source File
# Begin Source File

SOURCE=..\..\ballistictypeid.h
# End Source File
# Begin Source File

SOURCE=..\..\beam.cpp
# End Source File
# Begin Source File

SOURCE=..\..\beam.h
# End Source File
# Begin Source File

SOURCE=..\..\beamtype.h
# End Source File
# Begin Source File

SOURCE=..\..\changeownerreason.h
# End Source File
# Begin Source File

SOURCE=..\..\client.cpp
# End Source File
# Begin Source File

SOURCE=..\..\client.h
# End Source File
# Begin Source File

SOURCE=..\..\debuggingclient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\debuggingclient.h
# End Source File
# Begin Source File

SOURCE=..\..\effect.h
# End Source File
# Begin Source File

SOURCE=..\..\fifo_priority_queue.h
# End Source File
# Begin Source File

SOURCE=..\..\gnu_extensions.h
# End Source File
# Begin Source File

SOURCE=..\..\heightfield.cpp
# End Source File
# Begin Source File

SOURCE=..\..\heightfield.h
# End Source File
# Begin Source File

SOURCE=..\..\iarchive.cpp
# End Source File
# Begin Source File

SOURCE=..\..\iarchive.h
# End Source File
# Begin Source File

SOURCE="..\..\libsakusen-global.cpp"
# End Source File
# Begin Source File

SOURCE="..\..\libsakusen-global.h"
# End Source File
# Begin Source File

SOURCE=..\..\map.cpp
# End Source File
# Begin Source File

SOURCE=..\..\map.h
# End Source File
# Begin Source File

SOURCE=..\..\mapplaymode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\mapplaymode.h
# End Source File
# Begin Source File

SOURCE=..\..\moveorderdata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\moveorderdata.h
# End Source File
# Begin Source File

SOURCE=..\..\oarchive.cpp
# End Source File
# Begin Source File

SOURCE=..\..\oarchive.h
# End Source File
# Begin Source File

SOURCE=..\..\order.cpp
# End Source File
# Begin Source File

SOURCE=..\..\order.h
# End Source File
# Begin Source File

SOURCE=..\..\ordercondition.h
# End Source File
# Begin Source File

SOURCE=..\..\orderdata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\orderdata.h
# End Source File
# Begin Source File

SOURCE=..\..\ordermessage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\ordermessage.h
# End Source File
# Begin Source File

SOURCE=..\..\ordertype.h
# End Source File
# Begin Source File

SOURCE=..\..\orientation.cpp
# End Source File
# Begin Source File

SOURCE=..\..\orientation.h
# End Source File
# Begin Source File

SOURCE=..\..\player.cpp
# End Source File
# Begin Source File

SOURCE=..\..\player.h
# End Source File
# Begin Source File

SOURCE=..\..\playerid.h
# End Source File
# Begin Source File

SOURCE=..\..\quadratic.cpp
# End Source File
# Begin Source File

SOURCE=..\..\quadratic.h
# End Source File
# Begin Source File

SOURCE=..\..\ray.h
# End Source File
# Begin Source File

SOURCE=..\..\region.cpp
# End Source File
# Begin Source File

SOURCE=..\..\region.h
# End Source File
# Begin Source File

SOURCE=..\..\sensorreturn.h
# End Source File
# Begin Source File

SOURCE=..\..\setvelocityorderdata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\setvelocityorderdata.h
# End Source File
# Begin Source File

SOURCE=..\..\targettype.h
# End Source File
# Begin Source File

SOURCE=..\..\topology.h
# End Source File
# Begin Source File

SOURCE=..\..\unit.cpp
# End Source File
# Begin Source File

SOURCE=..\..\unit.h
# End Source File
# Begin Source File

SOURCE=..\..\unitid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\unitid.h
# End Source File
# Begin Source File

SOURCE=..\..\unittype.cpp
# End Source File
# Begin Source File

SOURCE=..\..\unittype.h
# End Source File
# Begin Source File

SOURCE=..\..\universe.cpp
# End Source File
# Begin Source File

SOURCE=..\..\universe.h
# End Source File
# Begin Source File

SOURCE=..\..\update.cpp
# End Source File
# Begin Source File

SOURCE=..\..\update.h
# End Source File
# Begin Source File

SOURCE=..\..\updatedata.cpp
# End Source File
# Begin Source File

SOURCE=..\..\updatedata.h
# End Source File
# Begin Source File

SOURCE=..\..\updatetype.cpp
# End Source File
# Begin Source File

SOURCE=..\..\updatetype.h
# End Source File
# Begin Source File

SOURCE=..\..\vectors.h
# End Source File
# Begin Source File

SOURCE=..\..\visibility.cpp
# End Source File
# Begin Source File

SOURCE=..\..\visibility.h
# End Source File
# Begin Source File

SOURCE=..\..\weapon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\weapon.h
# End Source File
# Begin Source File

SOURCE=..\..\weapontype.h
# End Source File
# Begin Source File

SOURCE=..\..\world.cpp
# End Source File
# Begin Source File

SOURCE=..\..\world.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
