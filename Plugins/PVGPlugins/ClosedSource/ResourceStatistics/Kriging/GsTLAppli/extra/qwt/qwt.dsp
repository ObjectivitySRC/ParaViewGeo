# Microsoft Developer Studio Project File - Name="qwt" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=qwt - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "qwt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qwt.mak" CFG="qwt - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qwt - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "qwt - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "qwt - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C:\GsTL\GsTLAppli/lib/win32"
# PROP Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/extra/qwt"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GR /O1 /I "." /I "C:\GsTL\GsTL" /I "C:\GsTL\GsTLAppli" /I "$(QTDIR)\include" /I "c:\GsTL\GsTLAppli\GsTLAppli\extra\qwt" /I "$(QTDIR)\mkspecs\win32-msvc" /D "GSTLAPPLI_NDEBUG" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "QT_DLL" /D "SOQT_DLL" /D "COIN_DLL" /D "QWT_DLL" /D "QWT_MAKEDLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /FD -Zm200 /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 delayimp.lib "qt-mt334.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "uuid.lib" "imm32.lib" /nologo /dll /machine:IX86 /libpath:"$(QTDIR)\lib" /DELAYLOAD:comdlg32.dll /DELAYLOAD:oleaut32.dll /DELAYLOAD:winmm.dll /DELAYLOAD:wsock32.dll /DELAYLOAD:winspool.dll

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "C:\GsTL\GsTLAppli/lib/win32"
# PROP BASE Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/extra/qwt"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C:\GsTL\GsTLAppli/lib/win32"
# PROP Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/extra/qwt"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GR /GX /Z7 /Od /I "." /I "C:\GsTL\GsTL" /I "C:\GsTL\GsTLAppli" /I "$(QTDIR)\include" /I "c:\GsTL\GsTLAppli\GsTLAppli\extra\qwt" /I "$(QTDIR)\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "QT_DLL" /D "SOQT_DLL" /D "COIN_DLL" /D "QWT_DLL" /D "QWT_MAKEDLL" /D "QT_THREAD_SUPPORT" /FD /GZ -Zm200 /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "oleaut32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" "qt-mt334.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "uuid.lib" "imm32.lib" /nologo /dll /debug /machine:IX86 /pdbtype:sept /libpath:"$(QTDIR)\lib"

!ENDIF 

# Begin Target

# Name "qwt - Win32 Release"
# Name "qwt - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=qwt_autoscl.cpp
# End Source File
# Begin Source File

SOURCE=qwt_curve.cpp
# End Source File
# Begin Source File

SOURCE=qwt_dimap.cpp
# End Source File
# Begin Source File

SOURCE=qwt_grid.cpp
# End Source File
# Begin Source File

SOURCE=qwt_legend.cpp
# End Source File
# Begin Source File

SOURCE=qwt_marker.cpp
# End Source File
# Begin Source File

SOURCE=qwt_math.cpp
# End Source File
# Begin Source File

SOURCE=qwt_plot.cpp
# End Source File
# Begin Source File

SOURCE=qwt_plot_axis.cpp
# End Source File
# Begin Source File

SOURCE=qwt_plot_canvas.cpp
# End Source File
# Begin Source File

SOURCE=qwt_plot_curve.cpp
# End Source File
# Begin Source File

SOURCE=qwt_plot_grid.cpp
# End Source File
# Begin Source File

SOURCE=qwt_plot_item.cpp
# End Source File
# Begin Source File

SOURCE=qwt_plot_marker.cpp
# End Source File
# Begin Source File

SOURCE=qwt_plot_print.cpp
# End Source File
# Begin Source File

SOURCE=qwt_plot_printfilter.cpp
# End Source File
# Begin Source File

SOURCE=qwt_rect.cpp
# End Source File
# Begin Source File

SOURCE=qwt_scale.cpp
# End Source File
# Begin Source File

SOURCE=qwt_scldiv.cpp
# End Source File
# Begin Source File

SOURCE=qwt_scldraw.cpp
# End Source File
# Begin Source File

SOURCE=qwt_spline.cpp
# End Source File
# Begin Source File

SOURCE=qwt_symbol.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=qwt.h
# End Source File
# Begin Source File

SOURCE=qwt_autoscl.h
# End Source File
# Begin Source File

SOURCE=qwt_curve.h
# End Source File
# Begin Source File

SOURCE=qwt_dimap.h
# End Source File
# Begin Source File

SOURCE=qwt_global.h
# End Source File
# Begin Source File

SOURCE=qwt_grid.h
# End Source File
# Begin Source File

SOURCE=qwt_legend.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_L="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing qwt_legend.h...
InputPath=qwt_legend.h

"moc_qwt_legend.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc qwt_legend.h -o moc_qwt_legend.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_L="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing qwt_legend.h...
InputPath=qwt_legend.h

"moc_qwt_legend.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc qwt_legend.h -o moc_qwt_legend.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qwt_marker.h
# End Source File
# Begin Source File

SOURCE=qwt_math.h
# End Source File
# Begin Source File

SOURCE=qwt_plot.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_P="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing qwt_plot.h...
InputPath=qwt_plot.h

"moc_qwt_plot.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc qwt_plot.h -o moc_qwt_plot.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_P="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing qwt_plot.h...
InputPath=qwt_plot.h

"moc_qwt_plot.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc qwt_plot.h -o moc_qwt_plot.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qwt_plot_canvas.h

!IF  "$(CFG)" == "qwt - Win32 Release"

USERDEP__QWT_PL="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing qwt_plot_canvas.h...
InputPath=qwt_plot_canvas.h

"moc_qwt_plot_canvas.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc qwt_plot_canvas.h -o moc_qwt_plot_canvas.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "qwt - Win32 Debug"

USERDEP__QWT_PL="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing qwt_plot_canvas.h...
InputPath=qwt_plot_canvas.h

"moc_qwt_plot_canvas.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc qwt_plot_canvas.h -o moc_qwt_plot_canvas.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=qwt_plot_classes.h
# End Source File
# Begin Source File

SOURCE=qwt_plot_dict.h
# End Source File
# Begin Source File

SOURCE=qwt_plot_item.h
# End Source File
# Begin Source File

SOURCE=qwt_plot_printfilter.h
# End Source File
# Begin Source File

SOURCE=qwt_rect.h
# End Source File
# Begin Source File

SOURCE=qwt_scale.h
# End Source File
# Begin Source File

SOURCE=qwt_scldiv.h
# End Source File
# Begin Source File

SOURCE=qwt_scldraw.h
# End Source File
# Begin Source File

SOURCE=qwt_spline.h
# End Source File
# Begin Source File

SOURCE=qwt_symbol.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=moc_qwt_legend.cpp
# End Source File
# Begin Source File

SOURCE=moc_qwt_plot.cpp
# End Source File
# Begin Source File

SOURCE=moc_qwt_plot_canvas.cpp
# End Source File
# End Group
# End Target
# End Project
