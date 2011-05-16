# Microsoft Developer Studio Project File - Name="GsTLwidgets" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=GsTLwidgets - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "qtplugins.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qtplugins.mak" CFG="GsTLwidgets - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GsTLwidgets - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "GsTLwidgets - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GsTLwidgets - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C:\GsTL\GsTLAppli/plugins/designer"
# PROP Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/extra/qtplugins"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GR /O2 /I "." /I "$(QTDIR)\tools\designer\interfaces" /I "C:\GsTL\GsTL" /I "C:\GsTL\GsTLAppli" /I "$(QTDIR)\include" /I "c:\GsTL\GsTLAppli\GsTLAppli\extra\qtplugins" /I ".moc\\" /I "$(QTDIR)\mkspecs\win32-msvc" /D "GSTLAPPLI_NDEBUG" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "QT_DLL" /D "SOQT_DLL" /D "COIN_DLL" /D "QWT_DLL" /D "QTPLUGINS_EXPORTS" /D "QT_PLUGIN" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /FD -Zm200 /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "qt-mt334.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_utils.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_appli.lib" "$(QTDIR)\lib\qui.lib" "kernel32.lib" /nologo /dll /machine:IX86 /out:"C:\GsTL\GsTLAppli\plugins\designer\GsTLwidgets.dll" /libpath:"$(QTDIR)\lib" /DELAYLOAD:comdlg32.dll /DELAYLOAD:oleaut32.dll /DELAYLOAD:winmm.dll /DELAYLOAD:wsock32.dll /DELAYLOAD:winspool.dll

!ELSEIF  "$(CFG)" == "GsTLwidgets - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "C:\GsTL\GsTLAppli/plugins/designer"
# PROP BASE Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/extra/qtplugins"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C:\GsTL\GsTLAppli/plugins/designer"
# PROP Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/extra/qtplugins"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GR /GX /Z7 /Od /I "." /I "$(QTDIR)\tools\designer\interfaces" /I "C:\GsTL\GsTL" /I "C:\GsTL\GsTLAppli" /I "$(QTDIR)\include" /I "c:\GsTL\GsTLAppli\GsTLAppli\extra\qtplugins" /I ".moc\\" /I "$(QTDIR)\mkspecs\win32-msvc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "QT_DLL" /D "SOQT_DLL" /D "COIN_DLL" /D "QWT_DLL" /D "QTPLUGINS_EXPORTS" /D "QT_PLUGIN" /D "QT_THREAD_SUPPORT" /FD /GZ -Zm200 /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "qt-mt334.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_utils.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_appli.lib" "$(QTDIR)\lib\qui.lib" "kernel32.lib" /nologo /dll /debug /machine:IX86 /out:"C:\GsTL\GsTLAppli\plugins\designer\GsTLwidgets.dll" /pdbtype:sept /libpath:"$(QTDIR)\lib"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "GsTLwidgets - Win32 Release"
# Name "GsTLwidgets - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=filechooser.cpp
# End Source File
# Begin Source File

SOURCE=gstl_table.cpp
# End Source File
# Begin Source File

SOURCE=gstl_widgets_plugin.cpp
# End Source File
# Begin Source File

SOURCE=kriging_type_selector.cpp
# End Source File
# Begin Source File

SOURCE=kriging_type_selector_base.cpp
# End Source File
# Begin Source File

SOURCE=selectors.cpp
# End Source File
# Begin Source File

SOURCE=variogram_input.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=common.h
# End Source File
# Begin Source File

SOURCE=filechooser.h

!IF  "$(CFG)" == "GsTLwidgets - Win32 Release"

USERDEP__FILEC="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing filechooser.h...
InputPath=filechooser.h

".moc\moc_filechooser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc filechooser.h -o .moc\moc_filechooser.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLwidgets - Win32 Debug"

USERDEP__FILEC="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing filechooser.h...
InputPath=filechooser.h

".moc\moc_filechooser.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc filechooser.h -o .moc\moc_filechooser.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=gstl_table.h

!IF  "$(CFG)" == "GsTLwidgets - Win32 Release"

USERDEP__GSTL_="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing gstl_table.h...
InputPath=gstl_table.h

".moc\moc_gstl_table.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc gstl_table.h -o .moc\moc_gstl_table.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLwidgets - Win32 Debug"

USERDEP__GSTL_="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing gstl_table.h...
InputPath=gstl_table.h

".moc\moc_gstl_table.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc gstl_table.h -o .moc\moc_gstl_table.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=gstl_widgets_plugin.h
# End Source File
# Begin Source File

SOURCE=kriging_type_selector.h

!IF  "$(CFG)" == "GsTLwidgets - Win32 Release"

USERDEP__KRIGI="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing kriging_type_selector.h...
InputPath=kriging_type_selector.h

".moc\moc_kriging_type_selector.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc kriging_type_selector.h -o .moc\moc_kriging_type_selector.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLwidgets - Win32 Debug"

USERDEP__KRIGI="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing kriging_type_selector.h...
InputPath=kriging_type_selector.h

".moc\moc_kriging_type_selector.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc kriging_type_selector.h -o .moc\moc_kriging_type_selector.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=kriging_type_selector_base.h

!IF  "$(CFG)" == "GsTLwidgets - Win32 Release"

USERDEP__KRIGIN="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing kriging_type_selector_base.h...
InputPath=kriging_type_selector_base.h

".moc\moc_kriging_type_selector_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc kriging_type_selector_base.h -o .moc\moc_kriging_type_selector_base.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLwidgets - Win32 Debug"

USERDEP__KRIGIN="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing kriging_type_selector_base.h...
InputPath=kriging_type_selector_base.h

".moc\moc_kriging_type_selector_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc kriging_type_selector_base.h -o .moc\moc_kriging_type_selector_base.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=selectors.h

!IF  "$(CFG)" == "GsTLwidgets - Win32 Release"

USERDEP__SELEC="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing selectors.h...
InputPath=selectors.h

".moc\moc_selectors.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc selectors.h -o .moc\moc_selectors.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLwidgets - Win32 Debug"

USERDEP__SELEC="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing selectors.h...
InputPath=selectors.h

".moc\moc_selectors.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc selectors.h -o .moc\moc_selectors.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=icons\selectors_icons.h
# End Source File
# Begin Source File

SOURCE=variogram_input.h

!IF  "$(CFG)" == "GsTLwidgets - Win32 Release"

USERDEP__VARIO="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram_input.h...
InputPath=variogram_input.h

".moc\moc_variogram_input.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram_input.h -o .moc\moc_variogram_input.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLwidgets - Win32 Debug"

USERDEP__VARIO="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram_input.h...
InputPath=variogram_input.h

".moc\moc_variogram_input.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram_input.h -o .moc\moc_variogram_input.cpp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Forms"

# PROP Default_Filter "ui"
# Begin Source File

SOURCE=order_properties_dialog.ui

!IF  "$(CFG)" == "GsTLwidgets - Win32 Release"

USERDEP__ORDER="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing order_properties_dialog.ui...
InputPath=order_properties_dialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic order_properties_dialog.ui -o order_properties_dialog.h \
	$(QTDIR)\bin\uic order_properties_dialog.ui -i order_properties_dialog.h -o order_properties_dialog.cpp \
	$(QTDIR)\bin\moc order_properties_dialog.h -o .moc\moc_order_properties_dialog.cpp \
	

"order_properties_dialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"order_properties_dialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_order_properties_dialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLwidgets - Win32 Debug"

USERDEP__ORDER="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing order_properties_dialog.ui...
InputPath=order_properties_dialog.ui

BuildCmds= \
	$(QTDIR)\bin\uic order_properties_dialog.ui -o order_properties_dialog.h \
	$(QTDIR)\bin\uic order_properties_dialog.ui -i order_properties_dialog.h -o order_properties_dialog.cpp \
	$(QTDIR)\bin\moc order_properties_dialog.h -o .moc\moc_order_properties_dialog.cpp \
	

"order_properties_dialog.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"order_properties_dialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_order_properties_dialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variogram_input_base.ui

!IF  "$(CFG)" == "GsTLwidgets - Win32 Release"

USERDEP__VARIOG="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variogram_input_base.ui...
InputPath=variogram_input_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic variogram_input_base.ui -o variogram_input_base.h \
	$(QTDIR)\bin\uic variogram_input_base.ui -i variogram_input_base.h -o variogram_input_base.cpp \
	$(QTDIR)\bin\moc variogram_input_base.h -o .moc\moc_variogram_input_base.cpp \
	

"variogram_input_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variogram_input_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_variogram_input_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLwidgets - Win32 Debug"

USERDEP__VARIOG="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variogram_input_base.ui...
InputPath=variogram_input_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic variogram_input_base.ui -o variogram_input_base.h \
	$(QTDIR)\bin\uic variogram_input_base.ui -i variogram_input_base.h -o variogram_input_base.cpp \
	$(QTDIR)\bin\moc variogram_input_base.h -o .moc\moc_variogram_input_base.cpp \
	

"variogram_input_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variogram_input_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_variogram_input_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variogram_structure_base.ui

!IF  "$(CFG)" == "GsTLwidgets - Win32 Release"

USERDEP__VARIOGR="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variogram_structure_base.ui...
InputPath=variogram_structure_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic variogram_structure_base.ui -o variogram_structure_base.h \
	$(QTDIR)\bin\uic variogram_structure_base.ui -i variogram_structure_base.h -o variogram_structure_base.cpp \
	$(QTDIR)\bin\moc variogram_structure_base.h -o .moc\moc_variogram_structure_base.cpp \
	

"variogram_structure_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variogram_structure_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_variogram_structure_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLwidgets - Win32 Debug"

USERDEP__VARIOGR="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variogram_structure_base.ui...
InputPath=variogram_structure_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic variogram_structure_base.ui -o variogram_structure_base.h \
	$(QTDIR)\bin\uic variogram_structure_base.ui -i variogram_structure_base.h -o variogram_structure_base.cpp \
	$(QTDIR)\bin\moc variogram_structure_base.h -o .moc\moc_variogram_structure_base.cpp \
	

"variogram_structure_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variogram_structure_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_variogram_structure_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.moc\moc_filechooser.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_gstl_table.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_kriging_type_selector.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_kriging_type_selector_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_order_properties_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_selectors.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_variogram_input.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_variogram_input_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_variogram_structure_base.cpp
# End Source File
# Begin Source File

SOURCE=order_properties_dialog.cpp
# End Source File
# Begin Source File

SOURCE=order_properties_dialog.h
# End Source File
# Begin Source File

SOURCE=variogram_input_base.cpp
# End Source File
# Begin Source File

SOURCE=variogram_input_base.h
# End Source File
# Begin Source File

SOURCE=variogram_structure_base.cpp
# End Source File
# Begin Source File

SOURCE=variogram_structure_base.h
# End Source File
# End Group
# End Target
# End Project
