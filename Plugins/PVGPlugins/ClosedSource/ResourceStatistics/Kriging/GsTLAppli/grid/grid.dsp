# Microsoft Developer Studio Project File - Name="GsTLAppli_grid" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=GsTLAppli_grid - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "grid.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "grid.mak" CFG="GsTLAppli_grid - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GsTLAppli_grid - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "GsTLAppli_grid - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GsTLAppli_grid - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C:\GsTL\GsTLAppli/lib/win32"
# PROP Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/grid"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GR /O2 /Ob2 /I "." /I "C:\GsTL\GsTL" /I "C:\GsTL\GsTLAppli" /I "c:\GsTL\GsTLAppli\GsTLAppli\grid" /I "$(QTDIR)\mkspecs\win32-msvc" /D "USE_ANN_KDTREE" /D "GSTL_NDEBUG" /D "GSTLAPPLI_NDEBUG" /D "GSTLAPPLI_NO_GVAL_DEBUG" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "QT_DLL" /D "SOQT_DLL" /D "COIN_DLL" /D "QWT_DLL" /D "GRID_EXPORTS" /FD -Zm200 /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_utils.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_math.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_appli.lib" "kernel32.lib" /nologo /dll /machine:IX86 /out:"C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_grid.dll" /DELAYLOAD:comdlg32.dll /DELAYLOAD:oleaut32.dll /DELAYLOAD:winmm.dll /DELAYLOAD:wsock32.dll /DELAYLOAD:winspool.dll

!ELSEIF  "$(CFG)" == "GsTLAppli_grid - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "C:\GsTL\GsTLAppli/lib/win32"
# PROP BASE Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/grid"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C:\GsTL\GsTLAppli/lib/win32"
# PROP Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/grid"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GR /GX /Z7 /Od /I "." /I "C:\GsTL\GsTL" /I "C:\GsTL\GsTLAppli" /I "c:\GsTL\GsTLAppli\GsTLAppli\grid" /I "$(QTDIR)\mkspecs\win32-msvc" /D "USE_ANN_KDTREE" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "QT_DLL" /D "SOQT_DLL" /D "COIN_DLL" /D "QWT_DLL" /D "GRID_EXPORTS" /FD /GZ -Zm200 /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 msvcrt.lib "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_utils.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_math.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_appli.lib" "kernel32.lib" /nologo /dll /debug /machine:IX86 /nodefaultlib:"msvcrt.lib" /out:"C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_grid.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "GsTLAppli_grid - Win32 Release"
# Name "GsTLAppli_grid - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=grid_model\cartesian_grid.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\combined_neighborhood.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\cross_variog_computer.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\geostat_grid.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\geovalue.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\grid_initializer.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\grid_property.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\grid_property_manager.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\grid_variog_computer.cpp
# End Source File
# Begin Source File

SOURCE=library_grid_init.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\neighborhood.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\point_set.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\point_set_neighborhood.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\property_copier.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\pset_variog_computer.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\rgrid.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\rgrid_geometry.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\rgrid_gval_accessor.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\rgrid_neighborhood.cpp
# End Source File
# Begin Source File

SOURCE=grid_model\superblock.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=grid_model\cartesian_grid.h
# End Source File
# Begin Source File

SOURCE=grid_model\combined_neighborhood.h
# End Source File
# Begin Source File

SOURCE=common.h
# End Source File
# Begin Source File

SOURCE=grid_model\cross_variog_computer.h
# End Source File
# Begin Source File

SOURCE=grid_model\geostat_grid.h
# End Source File
# Begin Source File

SOURCE=grid_model\geovalue.h
# End Source File
# Begin Source File

SOURCE=grid_model\grid_initializer.h
# End Source File
# Begin Source File

SOURCE=grid_model\grid_property.h
# End Source File
# Begin Source File

SOURCE=grid_model\grid_property_manager.h
# End Source File
# Begin Source File

SOURCE=grid_model\grid_topology.h
# End Source File
# Begin Source File

SOURCE=grid_model\grid_variog_computer.h
# End Source File
# Begin Source File

SOURCE=grid_model\gval_iterator.h
# End Source File
# Begin Source File

SOURCE=library_grid_init.h
# End Source File
# Begin Source File

SOURCE=grid_model\neighborhood.h
# End Source File
# Begin Source File

SOURCE=grid_model\point_set.h
# End Source File
# Begin Source File

SOURCE=grid_model\point_set_neighborhood.h
# End Source File
# Begin Source File

SOURCE=grid_model\property_copier.h
# End Source File
# Begin Source File

SOURCE=grid_model\pset_variog_computer.h
# End Source File
# Begin Source File

SOURCE=grid_model\rgrid.h
# End Source File
# Begin Source File

SOURCE=grid_model\rgrid_geometry.h
# End Source File
# Begin Source File

SOURCE=grid_model\rgrid_gval_accessor.h
# End Source File
# Begin Source File

SOURCE=grid_model\rgrid_iterator.h
# End Source File
# Begin Source File

SOURCE=grid_model\rgrid_neighborhood.h
# End Source File
# Begin Source File

SOURCE=grid_model\sgrid_cursor.h
# End Source File
# Begin Source File

SOURCE=grid_model\strati_grid.h
# End Source File
# Begin Source File

SOURCE=grid_model\superblock.h
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
