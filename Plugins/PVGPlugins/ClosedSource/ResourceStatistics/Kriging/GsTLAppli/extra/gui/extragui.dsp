# Microsoft Developer Studio Project File - Name="GsTLAppli_extragui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=GsTLAppli_extragui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "extragui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "extragui.mak" CFG="GsTLAppli_extragui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GsTLAppli_extragui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "GsTLAppli_extragui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GsTLAppli_extragui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C:\GsTL\GsTLAppli/lib/win32"
# PROP Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/extra/gui"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GR /Od /I "." /I "C:\GsTL\GsTL" /I "C:\GsTL\GsTLAppli" /I "$(QTDIR)\include" /I "c:\GsTL\GsTLAppli\GsTLAppli\extra\gui" /I "$(QTDIR)\mkspecs\win32-msvc" /D "GSTLAPPLI_NDEBUG" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "QT_DLL" /D "SOQT_DLL" /D "COIN_DLL" /D "QWT_DLL" /D "EXTRAGUI_EXPORTS" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /FD -Zm200 /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "qt-mt334.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_utils.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_appli.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_gui.lib" "C:\GsTL\GsTLAppli\plugins\designer\GsTLwidgets.lib" "kernel32.lib" /nologo /dll /machine:IX86 /out:"C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_extragui.dll" /libpath:"$(QTDIR)\lib" /DELAYLOAD:comdlg32.dll /DELAYLOAD:oleaut32.dll /DELAYLOAD:winmm.dll /DELAYLOAD:wsock32.dll /DELAYLOAD:winspool.dll

!ELSEIF  "$(CFG)" == "GsTLAppli_extragui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "C:\GsTL\GsTLAppli/lib/win32"
# PROP BASE Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/extra/gui"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C:\GsTL\GsTLAppli/lib/win32"
# PROP Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/extra/gui"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GR /GX /Z7 /Od /I "." /I "C:\GsTL\GsTL" /I "C:\GsTL\GsTLAppli" /I "$(QTDIR)\include" /I "c:\GsTL\GsTLAppli\GsTLAppli\extra\gui" /I "$(QTDIR)\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "QT_DLL" /D "SOQT_DLL" /D "COIN_DLL" /D "QWT_DLL" /D "EXTRAGUI_EXPORTS" /D "QT_THREAD_SUPPORT" /FD /GZ -Zm200 /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "qt-mt334.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_utils.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_appli.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_gui.lib" "C:\GsTL\GsTLAppli\plugins\designer\GsTLwidgets.lib" "kernel32.lib" /nologo /dll /debug /machine:IX86 /out:"C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_extragui.dll" /pdbtype:sept /libpath:"$(QTDIR)\lib"

!ENDIF 

# Begin Target

# Name "GsTLAppli_extragui - Win32 Release"
# Name "GsTLAppli_extragui - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=library_extragui_init.cpp
# End Source File
# Begin Source File

SOURCE=qt_accessors.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=common.h
# End Source File
# Begin Source File

SOURCE=library_extragui_init.h
# End Source File
# Begin Source File

SOURCE=qt_accessors.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Generated"

# PROP Default_Filter "moc"
# End Group
# End Target
# End Project
