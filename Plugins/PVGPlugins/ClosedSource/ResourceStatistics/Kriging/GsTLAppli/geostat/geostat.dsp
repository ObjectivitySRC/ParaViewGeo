# Microsoft Developer Studio Project File - Name="GsTLAppli_geostat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=GsTLAppli_geostat - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "geostat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "geostat.mak" CFG="GsTLAppli_geostat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GsTLAppli_geostat - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "GsTLAppli_geostat - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GsTLAppli_geostat - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C:\GsTL\GsTLAppli/lib/win32"
# PROP Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/geostat"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GR /O2 /Ob2 /I "." /I "C:\GsTL\GsTL" /I "C:\GsTL\GsTLAppli" /I "C:\GsTL\GsTL\src" /I "$(QTDIR)\include" /I "c:\GsTL\GsTLAppli\GsTLAppli\geostat" /I "$(QTDIR)\mkspecs\win32-msvc" /D "GSTL_NDEBUG" /D "GSTLAPPLI_NDEBUG" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "QT_DLL" /D "SOQT_DLL" /D "COIN_DLL" /D "QWT_DLL" /D "GEOSTAT_EXPORTS" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /FD -Zm200 /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "qt-mt334.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_utils.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_math.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_grid.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_appli.lib" "kernel32.lib" /nologo /dll /machine:IX86 /out:"C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_geostat.dll" /libpath:"$(QTDIR)\lib" /DELAYLOAD:comdlg32.dll /DELAYLOAD:oleaut32.dll /DELAYLOAD:winmm.dll /DELAYLOAD:wsock32.dll /DELAYLOAD:winspool.dll

!ELSEIF  "$(CFG)" == "GsTLAppli_geostat - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "C:\GsTL\GsTLAppli/lib/win32"
# PROP BASE Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/geostat"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C:\GsTL\GsTLAppli/lib/win32"
# PROP Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/geostat"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GR /GX /Z7 /Od /I "." /I "C:\GsTL\GsTL" /I "C:\GsTL\GsTLAppli" /I "C:\GsTL\GsTL\src" /I "$(QTDIR)\include" /I "c:\GsTL\GsTLAppli\GsTLAppli\geostat" /I "$(QTDIR)\mkspecs\win32-msvc" /D "GSTL_NOWARNING_2" /D "GSTL_NOCHECK_2" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "QT_DLL" /D "SOQT_DLL" /D "COIN_DLL" /D "QWT_DLL" /D "GEOSTAT_EXPORTS" /D "QT_THREAD_SUPPORT" /FD /GZ -Zm200 /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "qt-mt334.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_utils.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_math.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_grid.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_appli.lib" "kernel32.lib" /nologo /dll /debug /machine:IX86 /out:"C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_geostat.dll" /pdbtype:sept /libpath:"$(QTDIR)\lib"

!ENDIF 

# Begin Target

# Name "GsTLAppli_geostat - Win32 Release"
# Name "GsTLAppli_geostat - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=cokriging.cpp
# End Source File
# Begin Source File

SOURCE=cosgsim.cpp
# End Source File
# Begin Source File

SOURCE=cosisim.cpp
# End Source File
# Begin Source File

SOURCE=efrosim.cpp
# End Source File
# Begin Source File

SOURCE=indicator_kriging.cpp
# End Source File
# Begin Source File

SOURCE=kriging.cpp
# End Source File
# Begin Source File

SOURCE=library_geostat_init.cpp
# End Source File
# Begin Source File

SOURCE=parameters_handler_impl.cpp
# End Source File
# Begin Source File

SOURCE=sgsim.cpp
# End Source File
# Begin Source File

SOURCE=sisim.cpp
# End Source File
# Begin Source File

SOURCE=snesim.cpp
# End Source File
# Begin Source File

SOURCE=new_snesim\snesim_gslib.cpp
# End Source File
# Begin Source File

SOURCE=.\three_points_correl.cpp
# End Source File
# Begin Source File

SOURCE=utilities.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=cokriging.h
# End Source File
# Begin Source File

SOURCE=common.h
# End Source File
# Begin Source File

SOURCE=cosgsim.h
# End Source File
# Begin Source File

SOURCE=cosisim.h
# End Source File
# Begin Source File

SOURCE=efrosim.h
# End Source File
# Begin Source File

SOURCE=geostat_algo.h
# End Source File
# Begin Source File

SOURCE=indicator_kriging.h
# End Source File
# Begin Source File

SOURCE=kriging.h
# End Source File
# Begin Source File

SOURCE=library_geostat_init.h
# End Source File
# Begin Source File

SOURCE=parameters_handler.h
# End Source File
# Begin Source File

SOURCE=parameters_handler_impl.h
# End Source File
# Begin Source File

SOURCE=sgsim.h
# End Source File
# Begin Source File

SOURCE=sisim.h
# End Source File
# Begin Source File

SOURCE=snesim.h
# End Source File
# Begin Source File

SOURCE=new_snesim\snesim_gslib.h
# End Source File
# Begin Source File

SOURCE=.\three_points_correl.h
# End Source File
# Begin Source File

SOURCE=new_snesim\tree_list.h
# End Source File
# Begin Source File

SOURCE=utilities.h
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
