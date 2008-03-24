# Microsoft Developer Studio Project File - Name="luazip_dll" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=luazip_dll - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "luazip_dll.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "luazip_dll.mak" CFG="luazip_dll - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "luazip_dll - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "luazip_dll - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "luazip_dll - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib/vc6"
# PROP Intermediate_Dir "luazip_dll/Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LUAZIP_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../external-src/lua50/include" /I "../../external-src/zlib/include" /I "../../external-src/zziplib-0.12.83" /I "../../compat/src" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LUAZIP_EXPORTS" /D LUAZIP_API=__declspec(dllexport) /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x416 /d "NDEBUG"
# ADD RSC /l 0x416 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 lua50.lib zdll.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../bin/vc6/zip.dll" /libpath:"../../external-src/lua50/lib/dll" /libpath:"../../external-src/zlib/lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=cd ../bin/vc6	zip.exe luazip-1.2.1-win32.zip zip.dll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "luazip_dll - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib/vc6"
# PROP Intermediate_Dir "luazip_dll/Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LUAZIP_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../external-src/lua50/include" /I "../../external-src/zlib/include" /I "../../external-src/zziplib-0.12.83" /I "../../compat/src" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LUAZIP_EXPORTS" /D LUAZIP_API=__declspec(dllexport) /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x416 /d "_DEBUG"
# ADD RSC /l 0x416 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 lua50.lib zdll.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../bin/vc6/zipd.dll" /pdbtype:sept /libpath:"../../external-src/lua50/lib/dll" /libpath:"../../external-src/zlib/lib"

!ENDIF 

# Begin Target

# Name "luazip_dll - Win32 Release"
# Name "luazip_dll - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\..\compat\src\compat-5.1.c"
# End Source File
# Begin Source File

SOURCE=..\src\luazip.c
# End Source File
# Begin Source File

SOURCE=.\luazip.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="..\..\compat\src\compat-5.1.h"
# End Source File
# Begin Source File

SOURCE=..\src\luazip.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "zziplib Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\external-src\zziplib-0.12.83\zzip\dir.c"
# End Source File
# Begin Source File

SOURCE="..\..\external-src\zziplib-0.12.83\zzip\err.c"
# End Source File
# Begin Source File

SOURCE="..\..\external-src\zziplib-0.12.83\zzip\file.c"
# End Source File
# Begin Source File

SOURCE="..\..\external-src\zziplib-0.12.83\zzip\info.c"
# End Source File
# Begin Source File

SOURCE="..\..\external-src\zziplib-0.12.83\zzip\plugin.c"
# End Source File
# Begin Source File

SOURCE="..\..\external-src\zziplib-0.12.83\zzip\stat.c"
# End Source File
# Begin Source File

SOURCE="..\..\external-src\zziplib-0.12.83\zzip\write.c"
# End Source File
# Begin Source File

SOURCE="..\..\external-src\zziplib-0.12.83\zzip\zip.c"
# End Source File
# End Group
# End Target
# End Project
