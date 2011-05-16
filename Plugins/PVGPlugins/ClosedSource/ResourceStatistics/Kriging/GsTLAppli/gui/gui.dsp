# Microsoft Developer Studio Project File - Name="GsTLAppli_gui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=GsTLAppli_gui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gui.mak" CFG="GsTLAppli_gui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GsTLAppli_gui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "GsTLAppli_gui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "C:\GsTL\GsTLAppli/lib/win32"
# PROP Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/gui"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GR /O1 /I "." /I "C:\Python23\include" /I "C:\GsTL\GsTL" /I "C:\GsTL\GsTLAppli" /I "C:\GsTL\Coin3D\include\Qt\viewers" /I "C:\GsTL\Coin3D\include" /I "$(QTDIR)\include" /I "appli" /I "appli\\" /I "utils" /I "utils\\" /I "variogram2" /I "variogram2\\" /I "c:\GsTL\GsTLAppli\GsTLAppli\gui" /I ".moc\\" /I "$(QTDIR)\mkspecs\win32-msvc" /D "GSTLAPPLI_NDEBUG" /D "NDEBUG" /D "QT_NO_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "QT_DLL" /D "SOQT_DLL" /D "COIN_DLL" /D "SIMVOLEON_DLL" /D "QWT_DLL" /D "GUI_EXPORTS" /D "QT_THREAD_SUPPORT" /FD -Zm200 /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "kernel32.lib" "simvoleon2.lib" "soqt1.lib" "coin2.lib" "C:\Python23\libs\python23.lib" "qt-mt334.lib" "C:\GsTL\GsTLAppli\lib\win32\qwt.lib" "$(QTDIR)\lib\qui.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_actions.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_appli.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_utils.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_grid.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_math.lib" "C:\GsTL\GsTLAppli\plugins\designer\GsTLwidgets.lib" /nologo /dll /machine:IX86 /out:"C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_gui.dll" /libpath:"$(QTDIR)\lib" /libpath:"C:\GsTL\Coin3D\lib" /DELAYLOAD:comdlg32.dll /DELAYLOAD:oleaut32.dll /DELAYLOAD:winmm.dll /DELAYLOAD:wsock32.dll /DELAYLOAD:winspool.dll

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "C:\GsTL\GsTLAppli/lib/win32"
# PROP BASE Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/gui"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "C:\GsTL\GsTLAppli/lib/win32"
# PROP Intermediate_Dir "C:\GsTL\GsTLAppli/lib/win32/obj/gui"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GR /GX /Z7 /Od /I "." /I "C:\Python23\include" /I "C:\GsTL\GsTL" /I "C:\GsTL\GsTLAppli" /I "C:\GsTL\Coin3D\include\Qt\viewers" /I "C:\GsTL\Coin3D\include" /I "$(QTDIR)\include" /I "appli" /I "appli\\" /I "utils" /I "utils\\" /I "variogram2" /I "variogram2\\" /I "c:\GsTL\GsTLAppli\GsTLAppli\gui" /I ".moc\\" /I "$(QTDIR)\mkspecs\win32-msvc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNICODE" /D "QT_DLL" /D "SOQT_DLL" /D "COIN_DLL" /D "SIMVOLEON_DLL" /D "QWT_DLL" /D "GUI_EXPORTS" /D "QT_THREAD_SUPPORT" /FD /GZ -Zm200 /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 simvoleon2.lib soqt1.lib coin2.lib "C:\Python23\libs\python23.lib" "qt-mt334.lib" "C:\GsTL\GsTLAppli\lib\win32\qwt.lib" "$(QTDIR)\lib\qui.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_actions.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_appli.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_utils.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_grid.lib" "C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_math.lib" "C:\GsTL\GsTLAppli\plugins\designer\GsTLwidgets.lib" /nologo /dll /debug /machine:IX86 /out:"C:\GsTL\GsTLAppli\lib\win32\GsTLAppli_gui.dll" /pdbtype:sept /libpath:"$(QTDIR)\lib" /libpath:"C:\GsTL\Coin3D\lib"

!ENDIF 

# Begin Target

# Name "GsTLAppli_gui - Win32 Release"
# Name "GsTLAppli_gui - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=appli\cli_commands_panel.cpp
# End Source File
# Begin Source File

SOURCE=utils\colormap.cpp
# End Source File
# Begin Source File

SOURCE=oinv_description\colormap_node.cpp
# End Source File
# Begin Source File

SOURCE=utils\colors.cpp
# End Source File
# Begin Source File

SOURCE=utils\colorscale.cpp
# End Source File
# Begin Source File

SOURCE=variogram2\cross_variogram_modeler_gui.cpp
# End Source File
# Begin Source File

SOURCE=appli\data_analysis_gui.cpp
# End Source File
# Begin Source File

SOURCE=utils\data_analysis_save_dialog.cpp
# End Source File
# Begin Source File

SOURCE=QWidget_value_accessors\default_accessor.cpp
# End Source File
# Begin Source File

SOURCE=utils\delete_properties_dialog.cpp
# End Source File
# Begin Source File

SOURCE=appli\display_pref_panel.cpp
# End Source File
# Begin Source File

SOURCE=appli\general_display_pref_panel.cpp
# End Source File
# Begin Source File

SOURCE=utils\gstl_qlistviewitem.cpp
# End Source File
# Begin Source File

SOURCE=utils\gstl_slider.cpp
# End Source File
# Begin Source File

SOURCE=.\oinv_description\gstl_SoClipPlaneManip.cpp
# End Source File
# Begin Source File

SOURCE=oinv_description\gstl_SoNode.cpp
# End Source File
# Begin Source File

SOURCE=utils\gstl_widget_factory.cpp
# End Source File
# Begin Source File

SOURCE=appli\histogram_control_panel.cpp
# End Source File
# Begin Source File

SOURCE=appli\histogram_gui.cpp
# End Source File
# Begin Source File

SOURCE=library_gui_init.cpp
# End Source File
# Begin Source File

SOURCE=utils\multichoice_dialog.cpp
# End Source File
# Begin Source File

SOURCE=appli\new_cartesian_grid_dialog.cpp
# End Source File
# Begin Source File

SOURCE=oinv_description\oinv_cgrid.cpp
# End Source File
# Begin Source File

SOURCE=oinv_description\oinv_common.cpp
# End Source File
# Begin Source File

SOURCE=oinv_description\oinv_description.cpp
# End Source File
# Begin Source File

SOURCE=oinv_description\oinv_pointset.cpp
# End Source File
# Begin Source File

SOURCE=appli\oinv_project_view.cpp
# End Source File
# Begin Source File

SOURCE=oinv_description\oinv_slice.cpp
# End Source File
# Begin Source File

SOURCE=variogram2\pset_params.cpp
# End Source File
# Begin Source File

SOURCE=QWidget_value_accessors\qbuttongroup_accessor.cpp
# End Source File
# Begin Source File

SOURCE=QWidget_value_accessors\qcheckbox_accessor.cpp
# End Source File
# Begin Source File

SOURCE=QWidget_value_accessors\qcombobox_accessor.cpp
# End Source File
# Begin Source File

SOURCE=utils\qdirdialog.cpp
# End Source File
# Begin Source File

SOURCE=QWidget_value_accessors\qlineedit_accessor.cpp
# End Source File
# Begin Source File

SOURCE=appli\qpplot_control_panel.cpp
# End Source File
# Begin Source File

SOURCE=appli\qpplot_gui.cpp
# End Source File
# Begin Source File

SOURCE=QWidget_value_accessors\qradiobutton_accessor.cpp
# End Source File
# Begin Source File

SOURCE=QWidget_value_accessors\qspin_box_accessor.cpp
# End Source File
# Begin Source File

SOURCE=appli\qt_algo_control_panel.cpp
# End Source File
# Begin Source File

SOURCE=appli\qt_sp_application.cpp
# End Source File
# Begin Source File

SOURCE=QWidget_value_accessors\qtable_accessor.cpp
# End Source File
# Begin Source File

SOURCE=utils\qtbased_scribes.cpp
# End Source File
# Begin Source File

SOURCE=utils\qtprogress_notifier.cpp
# End Source File
# Begin Source File

SOURCE=utils\qwidget_value_accessor.cpp
# End Source File
# Begin Source File

SOURCE=utils\qwidget_value_collector.cpp
# End Source File
# Begin Source File

SOURCE=variogram2\rgrid_params.cpp
# End Source File
# Begin Source File

SOURCE=appli\scatterplot_control_panel.cpp
# End Source File
# Begin Source File

SOURCE=appli\scatterplot_gui.cpp
# End Source File
# Begin Source File

SOURCE=utils\script_editor.cpp
# End Source File
# Begin Source File

SOURCE=utils\script_syntax_highlighter.cpp
# End Source File
# Begin Source File

SOURCE=appli\snapshot_dialog.cpp
# End Source File
# Begin Source File

SOURCE=appli\SoQtGsTLViewer.cpp
# End Source File
# Begin Source File

SOURCE=appli\splash_screen.cpp
# End Source File
# Begin Source File

SOURCE=variogram2\variog_plot.cpp
# End Source File
# Begin Source File

SOURCE=variogram2\variogram_controls.cpp
# End Source File
# Begin Source File

SOURCE=variogram2\variogram_controls_base.cpp
# End Source File
# Begin Source File

SOURCE=variogram2\variogram_modeler_gui.cpp
# End Source File
# Begin Source File

SOURCE=variogram2\variogram_modeling_screen.cpp
# End Source File
# Begin Source File

SOURCE=appli\visu_actions.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=appli\cli_commands_panel.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__CLI_C="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\cli_commands_panel.h...
InputPath=appli\cli_commands_panel.h

".moc\moc_cli_commands_panel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\cli_commands_panel.h -o .moc\moc_cli_commands_panel.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__CLI_C="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\cli_commands_panel.h...
InputPath=appli\cli_commands_panel.h

".moc\moc_cli_commands_panel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\cli_commands_panel.h -o .moc\moc_cli_commands_panel.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=utils\color.h
# End Source File
# Begin Source File

SOURCE=utils\colormap.h
# End Source File
# Begin Source File

SOURCE=oinv_description\colormap_node.h
# End Source File
# Begin Source File

SOURCE=utils\colors.h
# End Source File
# Begin Source File

SOURCE=utils\colorscale.h
# End Source File
# Begin Source File

SOURCE=common.h
# End Source File
# Begin Source File

SOURCE=variogram2\cross_first_scr.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__CROSS="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\cross_first_scr.h...
InputPath=variogram2\cross_first_scr.h

".moc\moc_cross_first_scr.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\cross_first_scr.h -o .moc\moc_cross_first_scr.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__CROSS="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\cross_first_scr.h...
InputPath=variogram2\cross_first_scr.h

".moc\moc_cross_first_scr.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\cross_first_scr.h -o .moc\moc_cross_first_scr.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variogram2\cross_pset_rgrid_params.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__CROSS_="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\cross_pset_rgrid_params.h...
InputPath=variogram2\cross_pset_rgrid_params.h

".moc\moc_cross_pset_rgrid_params.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\cross_pset_rgrid_params.h -o .moc\moc_cross_pset_rgrid_params.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__CROSS_="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\cross_pset_rgrid_params.h...
InputPath=variogram2\cross_pset_rgrid_params.h

".moc\moc_cross_pset_rgrid_params.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\cross_pset_rgrid_params.h -o .moc\moc_cross_pset_rgrid_params.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variogram2\cross_variogram_modeler_gui.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__CROSS_V="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\cross_variogram_modeler_gui.h...
InputPath=variogram2\cross_variogram_modeler_gui.h

".moc\moc_cross_variogram_modeler_gui.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\cross_variogram_modeler_gui.h -o .moc\moc_cross_variogram_modeler_gui.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__CROSS_V="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\cross_variogram_modeler_gui.h...
InputPath=variogram2\cross_variogram_modeler_gui.h

".moc\moc_cross_variogram_modeler_gui.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\cross_variogram_modeler_gui.h -o .moc\moc_cross_variogram_modeler_gui.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\data_analysis_gui.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__DATA_="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\data_analysis_gui.h...
InputPath=appli\data_analysis_gui.h

".moc\moc_data_analysis_gui.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\data_analysis_gui.h -o .moc\moc_data_analysis_gui.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__DATA_="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\data_analysis_gui.h...
InputPath=appli\data_analysis_gui.h

".moc\moc_data_analysis_gui.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\data_analysis_gui.h -o .moc\moc_data_analysis_gui.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=utils\data_analysis_save_dialog.h
# End Source File
# Begin Source File

SOURCE=QWidget_value_accessors\default_accessor.h
# End Source File
# Begin Source File

SOURCE=utils\delete_properties_dialog.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__DELET="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing utils\delete_properties_dialog.h...
InputPath=utils\delete_properties_dialog.h

".moc\moc_delete_properties_dialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc utils\delete_properties_dialog.h -o .moc\moc_delete_properties_dialog.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__DELET="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing utils\delete_properties_dialog.h...
InputPath=utils\delete_properties_dialog.h

".moc\moc_delete_properties_dialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc utils\delete_properties_dialog.h -o .moc\moc_delete_properties_dialog.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\display_pref_panel.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__DISPL="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\display_pref_panel.h...
InputPath=appli\display_pref_panel.h

".moc\moc_display_pref_panel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\display_pref_panel.h -o .moc\moc_display_pref_panel.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__DISPL="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\display_pref_panel.h...
InputPath=appli\display_pref_panel.h

".moc\moc_display_pref_panel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\display_pref_panel.h -o .moc\moc_display_pref_panel.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variogram2\first_scr.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__FIRST="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\first_scr.h...
InputPath=variogram2\first_scr.h

".moc\moc_first_scr.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\first_scr.h -o .moc\moc_first_scr.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__FIRST="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\first_scr.h...
InputPath=variogram2\first_scr.h

".moc\moc_first_scr.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\first_scr.h -o .moc\moc_first_scr.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\general_display_pref_panel.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__GENER="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\general_display_pref_panel.h...
InputPath=appli\general_display_pref_panel.h

".moc\moc_general_display_pref_panel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\general_display_pref_panel.h -o .moc\moc_general_display_pref_panel.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__GENER="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\general_display_pref_panel.h...
InputPath=appli\general_display_pref_panel.h

".moc\moc_general_display_pref_panel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\general_display_pref_panel.h -o .moc\moc_general_display_pref_panel.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=utils\gstl_qlistviewitem.h
# End Source File
# Begin Source File

SOURCE=utils\gstl_slider.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__GSTL_="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing utils\gstl_slider.h...
InputPath=utils\gstl_slider.h

".moc\moc_gstl_slider.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc utils\gstl_slider.h -o .moc\moc_gstl_slider.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__GSTL_="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing utils\gstl_slider.h...
InputPath=utils\gstl_slider.h

".moc\moc_gstl_slider.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc utils\gstl_slider.h -o .moc\moc_gstl_slider.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\oinv_description\gstl_SoClipPlaneManip.h
# End Source File
# Begin Source File

SOURCE=oinv_description\gstl_SoNode.h
# End Source File
# Begin Source File

SOURCE=utils\gstl_widget_factory.h
# End Source File
# Begin Source File

SOURCE=appli\histogram_control_panel.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__HISTO="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\histogram_control_panel.h...
InputPath=appli\histogram_control_panel.h

".moc\moc_histogram_control_panel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\histogram_control_panel.h -o .moc\moc_histogram_control_panel.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__HISTO="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\histogram_control_panel.h...
InputPath=appli\histogram_control_panel.h

".moc\moc_histogram_control_panel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\histogram_control_panel.h -o .moc\moc_histogram_control_panel.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\histogram_gui.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__HISTOG="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\histogram_gui.h...
InputPath=appli\histogram_gui.h

".moc\moc_histogram_gui.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\histogram_gui.h -o .moc\moc_histogram_gui.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__HISTOG="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\histogram_gui.h...
InputPath=appli\histogram_gui.h

".moc\moc_histogram_gui.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\histogram_gui.h -o .moc\moc_histogram_gui.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=library_gui_init.h
# End Source File
# Begin Source File

SOURCE=utils\multichoice_dialog.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__MULTI="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing utils\multichoice_dialog.h...
InputPath=utils\multichoice_dialog.h

".moc\moc_multichoice_dialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc utils\multichoice_dialog.h -o .moc\moc_multichoice_dialog.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__MULTI="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing utils\multichoice_dialog.h...
InputPath=utils\multichoice_dialog.h

".moc\moc_multichoice_dialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc utils\multichoice_dialog.h -o .moc\moc_multichoice_dialog.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\new_cartesian_grid_dialog.h
# End Source File
# Begin Source File

SOURCE=oinv_description\oinv_cgrid.h
# End Source File
# Begin Source File

SOURCE=oinv_description\oinv_common.h
# End Source File
# Begin Source File

SOURCE=oinv_description\oinv_description.h
# End Source File
# Begin Source File

SOURCE=oinv_description\oinv_pointset.h
# End Source File
# Begin Source File

SOURCE=appli\oinv_project_view.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__OINV_="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\oinv_project_view.h...
InputPath=appli\oinv_project_view.h

".moc\moc_oinv_project_view.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\oinv_project_view.h -o .moc\moc_oinv_project_view.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__OINV_="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\oinv_project_view.h...
InputPath=appli\oinv_project_view.h

".moc\moc_oinv_project_view.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\oinv_project_view.h -o .moc\moc_oinv_project_view.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=oinv_description\oinv_slice.h
# End Source File
# Begin Source File

SOURCE=oinv_description\oinv_strati_grid.h
# End Source File
# Begin Source File

SOURCE=variogram2\pset_params.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__PSET_="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\pset_params.h...
InputPath=variogram2\pset_params.h

".moc\moc_pset_params.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\pset_params.h -o .moc\moc_pset_params.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__PSET_="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\pset_params.h...
InputPath=variogram2\pset_params.h

".moc\moc_pset_params.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\pset_params.h -o .moc\moc_pset_params.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=QWidget_value_accessors\qbuttongroup_accessor.h
# End Source File
# Begin Source File

SOURCE=QWidget_value_accessors\qcheckbox_accessor.h
# End Source File
# Begin Source File

SOURCE=QWidget_value_accessors\qcombobox_accessor.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__QCOMB="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing QWidget_value_accessors\qcombobox_accessor.h...
InputPath=QWidget_value_accessors\qcombobox_accessor.h

".moc\moc_qcombobox_accessor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc QWidget_value_accessors\qcombobox_accessor.h -o .moc\moc_qcombobox_accessor.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__QCOMB="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing QWidget_value_accessors\qcombobox_accessor.h...
InputPath=QWidget_value_accessors\qcombobox_accessor.h

".moc\moc_qcombobox_accessor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc QWidget_value_accessors\qcombobox_accessor.h -o .moc\moc_qcombobox_accessor.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=utils\qdirdialog.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__QDIRD="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing utils\qdirdialog.h...
InputPath=utils\qdirdialog.h

".moc\moc_qdirdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc utils\qdirdialog.h -o .moc\moc_qdirdialog.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__QDIRD="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing utils\qdirdialog.h...
InputPath=utils\qdirdialog.h

".moc\moc_qdirdialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc utils\qdirdialog.h -o .moc\moc_qdirdialog.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=QWidget_value_accessors\qlineedit_accessor.h
# End Source File
# Begin Source File

SOURCE=appli\qpplot_control_panel.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__QPPLO="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\qpplot_control_panel.h...
InputPath=appli\qpplot_control_panel.h

".moc\moc_qpplot_control_panel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\qpplot_control_panel.h -o .moc\moc_qpplot_control_panel.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__QPPLO="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\qpplot_control_panel.h...
InputPath=appli\qpplot_control_panel.h

".moc\moc_qpplot_control_panel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\qpplot_control_panel.h -o .moc\moc_qpplot_control_panel.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\qpplot_gui.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__QPPLOT="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\qpplot_gui.h...
InputPath=appli\qpplot_gui.h

".moc\moc_qpplot_gui.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\qpplot_gui.h -o .moc\moc_qpplot_gui.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__QPPLOT="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\qpplot_gui.h...
InputPath=appli\qpplot_gui.h

".moc\moc_qpplot_gui.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\qpplot_gui.h -o .moc\moc_qpplot_gui.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=QWidget_value_accessors\qradiobutton_accessor.h
# End Source File
# Begin Source File

SOURCE=QWidget_value_accessors\qspin_box_accessor.h
# End Source File
# Begin Source File

SOURCE=appli\qt_algo_control_panel.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__QT_AL="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\qt_algo_control_panel.h...
InputPath=appli\qt_algo_control_panel.h

".moc\moc_qt_algo_control_panel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\qt_algo_control_panel.h -o .moc\moc_qt_algo_control_panel.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__QT_AL="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\qt_algo_control_panel.h...
InputPath=appli\qt_algo_control_panel.h

".moc\moc_qt_algo_control_panel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\qt_algo_control_panel.h -o .moc\moc_qt_algo_control_panel.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\qt_sp_application.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__QT_SP="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\qt_sp_application.h...
InputPath=appli\qt_sp_application.h

".moc\moc_qt_sp_application.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\qt_sp_application.h -o .moc\moc_qt_sp_application.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__QT_SP="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\qt_sp_application.h...
InputPath=appli\qt_sp_application.h

".moc\moc_qt_sp_application.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\qt_sp_application.h -o .moc\moc_qt_sp_application.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=QWidget_value_accessors\qtable_accessor.h
# End Source File
# Begin Source File

SOURCE=utils\qtbased_scribes.h
# End Source File
# Begin Source File

SOURCE=utils\qtprogress_notifier.h
# End Source File
# Begin Source File

SOURCE=utils\qwidget_value_accessor.h
# End Source File
# Begin Source File

SOURCE=utils\qwidget_value_collector.h
# End Source File
# Begin Source File

SOURCE=variogram2\rgrid_params.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__RGRID="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\rgrid_params.h...
InputPath=variogram2\rgrid_params.h

".moc\moc_rgrid_params.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\rgrid_params.h -o .moc\moc_rgrid_params.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__RGRID="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\rgrid_params.h...
InputPath=variogram2\rgrid_params.h

".moc\moc_rgrid_params.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\rgrid_params.h -o .moc\moc_rgrid_params.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\scatterplot_control_panel.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__SCATT="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\scatterplot_control_panel.h...
InputPath=appli\scatterplot_control_panel.h

".moc\moc_scatterplot_control_panel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\scatterplot_control_panel.h -o .moc\moc_scatterplot_control_panel.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__SCATT="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\scatterplot_control_panel.h...
InputPath=appli\scatterplot_control_panel.h

".moc\moc_scatterplot_control_panel.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\scatterplot_control_panel.h -o .moc\moc_scatterplot_control_panel.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\scatterplot_gui.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__SCATTE="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\scatterplot_gui.h...
InputPath=appli\scatterplot_gui.h

".moc\moc_scatterplot_gui.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\scatterplot_gui.h -o .moc\moc_scatterplot_gui.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__SCATTE="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\scatterplot_gui.h...
InputPath=appli\scatterplot_gui.h

".moc\moc_scatterplot_gui.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\scatterplot_gui.h -o .moc\moc_scatterplot_gui.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=utils\script_editor.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__SCRIP="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing utils\script_editor.h...
InputPath=utils\script_editor.h

".moc\moc_script_editor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc utils\script_editor.h -o .moc\moc_script_editor.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__SCRIP="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing utils\script_editor.h...
InputPath=utils\script_editor.h

".moc\moc_script_editor.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc utils\script_editor.h -o .moc\moc_script_editor.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=utils\script_syntax_highlighter.h
# End Source File
# Begin Source File

SOURCE=appli\snapshot_dialog.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__SNAPS="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\snapshot_dialog.h...
InputPath=appli\snapshot_dialog.h

".moc\moc_snapshot_dialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\snapshot_dialog.h -o .moc\moc_snapshot_dialog.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__SNAPS="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing appli\snapshot_dialog.h...
InputPath=appli\snapshot_dialog.h

".moc\moc_snapshot_dialog.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc appli\snapshot_dialog.h -o .moc\moc_snapshot_dialog.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\SoQtGsTLViewer.h
# End Source File
# Begin Source File

SOURCE=appli\splash_screen.h
# End Source File
# Begin Source File

SOURCE=variogram2\variog_plot.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__VARIO="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\variog_plot.h...
InputPath=variogram2\variog_plot.h

".moc\moc_variog_plot.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\variog_plot.h -o .moc\moc_variog_plot.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__VARIO="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\variog_plot.h...
InputPath=variogram2\variog_plot.h

".moc\moc_variog_plot.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\variog_plot.h -o .moc\moc_variog_plot.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variogram2\variogram_controls.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__VARIOG="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\variogram_controls.h...
InputPath=variogram2\variogram_controls.h

".moc\moc_variogram_controls.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\variogram_controls.h -o .moc\moc_variogram_controls.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__VARIOG="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\variogram_controls.h...
InputPath=variogram2\variogram_controls.h

".moc\moc_variogram_controls.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\variogram_controls.h -o .moc\moc_variogram_controls.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variogram2\variogram_controls_base.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__VARIOGR="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\variogram_controls_base.h...
InputPath=variogram2\variogram_controls_base.h

".moc\moc_variogram_controls_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\variogram_controls_base.h -o .moc\moc_variogram_controls_base.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__VARIOGR="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\variogram_controls_base.h...
InputPath=variogram2\variogram_controls_base.h

".moc\moc_variogram_controls_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\variogram_controls_base.h -o .moc\moc_variogram_controls_base.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variogram2\variogram_modeler_gui.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__VARIOGRA="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\variogram_modeler_gui.h...
InputPath=variogram2\variogram_modeler_gui.h

".moc\moc_variogram_modeler_gui.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\variogram_modeler_gui.h -o .moc\moc_variogram_modeler_gui.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__VARIOGRA="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\variogram_modeler_gui.h...
InputPath=variogram2\variogram_modeler_gui.h

".moc\moc_variogram_modeler_gui.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\variogram_modeler_gui.h -o .moc\moc_variogram_modeler_gui.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variogram2\variogram_modeling_screen.h

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__VARIOGRAM="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\variogram_modeling_screen.h...
InputPath=variogram2\variogram_modeling_screen.h

".moc\moc_variogram_modeling_screen.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\variogram_modeling_screen.h -o .moc\moc_variogram_modeling_screen.cpp

# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__VARIOGRAM="$(QTDIR)\bin\moc.exe"	
# Begin Custom Build - Moc'ing variogram2\variogram_modeling_screen.h...
InputPath=variogram2\variogram_modeling_screen.h

".moc\moc_variogram_modeling_screen.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\moc variogram2\variogram_modeling_screen.h -o .moc\moc_variogram_modeling_screen.cpp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\visu_actions.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Forms"

# PROP Default_Filter "ui"
# Begin Source File

SOURCE=appli\controls_widget.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__CONTR="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\controls_widget.ui...
InputPath=appli\controls_widget.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\controls_widget.ui -o appli\controls_widget.h \
	$(QTDIR)\bin\uic appli\controls_widget.ui -i controls_widget.h -o appli\controls_widget.cpp \
	$(QTDIR)\bin\moc appli\controls_widget.h -o .moc\moc_controls_widget.cpp \
	

"appli\controls_widget.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\controls_widget.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_controls_widget.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__CONTR="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\controls_widget.ui...
InputPath=appli\controls_widget.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\controls_widget.ui -o appli\controls_widget.h \
	$(QTDIR)\bin\uic appli\controls_widget.ui -i controls_widget.h -o appli\controls_widget.cpp \
	$(QTDIR)\bin\moc appli\controls_widget.h -o .moc\moc_controls_widget.cpp \
	

"appli\controls_widget.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\controls_widget.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_controls_widget.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variogram2\cross_first_scr_base.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__CROSS_F="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variogram2\cross_first_scr_base.ui...
InputPath=variogram2\cross_first_scr_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic variogram2\cross_first_scr_base.ui -o variogram2\cross_first_scr_base.h \
	$(QTDIR)\bin\uic variogram2\cross_first_scr_base.ui -i cross_first_scr_base.h -o variogram2\cross_first_scr_base.cpp \
	$(QTDIR)\bin\moc variogram2\cross_first_scr_base.h -o .moc\moc_cross_first_scr_base.cpp \
	

"variogram2\cross_first_scr_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variogram2\cross_first_scr_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_cross_first_scr_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__CROSS_F="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variogram2\cross_first_scr_base.ui...
InputPath=variogram2\cross_first_scr_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic variogram2\cross_first_scr_base.ui -o variogram2\cross_first_scr_base.h \
	$(QTDIR)\bin\uic variogram2\cross_first_scr_base.ui -i cross_first_scr_base.h -o variogram2\cross_first_scr_base.cpp \
	$(QTDIR)\bin\moc variogram2\cross_first_scr_base.h -o .moc\moc_cross_first_scr_base.cpp \
	

"variogram2\cross_first_scr_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variogram2\cross_first_scr_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_cross_first_scr_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variogram2\cross_pset_rgrid_base.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__CROSS_P="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variogram2\cross_pset_rgrid_base.ui...
InputPath=variogram2\cross_pset_rgrid_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic variogram2\cross_pset_rgrid_base.ui -o variogram2\cross_pset_rgrid_base.h \
	$(QTDIR)\bin\uic variogram2\cross_pset_rgrid_base.ui -i cross_pset_rgrid_base.h -o variogram2\cross_pset_rgrid_base.cpp \
	$(QTDIR)\bin\moc variogram2\cross_pset_rgrid_base.h -o .moc\moc_cross_pset_rgrid_base.cpp \
	

"variogram2\cross_pset_rgrid_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variogram2\cross_pset_rgrid_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_cross_pset_rgrid_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__CROSS_P="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variogram2\cross_pset_rgrid_base.ui...
InputPath=variogram2\cross_pset_rgrid_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic variogram2\cross_pset_rgrid_base.ui -o variogram2\cross_pset_rgrid_base.h \
	$(QTDIR)\bin\uic variogram2\cross_pset_rgrid_base.ui -i cross_pset_rgrid_base.h -o variogram2\cross_pset_rgrid_base.cpp \
	$(QTDIR)\bin\moc variogram2\cross_pset_rgrid_base.h -o .moc\moc_cross_pset_rgrid_base.cpp \
	

"variogram2\cross_pset_rgrid_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variogram2\cross_pset_rgrid_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_cross_pset_rgrid_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\data_analysis_gui_base.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__DATA_A="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\data_analysis_gui_base.ui...
InputPath=appli\data_analysis_gui_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\data_analysis_gui_base.ui -o appli\data_analysis_gui_base.h \
	$(QTDIR)\bin\uic appli\data_analysis_gui_base.ui -i data_analysis_gui_base.h -o appli\data_analysis_gui_base.cpp \
	$(QTDIR)\bin\moc appli\data_analysis_gui_base.h -o .moc\moc_data_analysis_gui_base.cpp \
	

"appli\data_analysis_gui_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\data_analysis_gui_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_data_analysis_gui_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__DATA_A="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\data_analysis_gui_base.ui...
InputPath=appli\data_analysis_gui_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\data_analysis_gui_base.ui -o appli\data_analysis_gui_base.h \
	$(QTDIR)\bin\uic appli\data_analysis_gui_base.ui -i data_analysis_gui_base.h -o appli\data_analysis_gui_base.cpp \
	$(QTDIR)\bin\moc appli\data_analysis_gui_base.h -o .moc\moc_data_analysis_gui_base.cpp \
	

"appli\data_analysis_gui_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\data_analysis_gui_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_data_analysis_gui_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=utils\delete_properties_dialog_base.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__DELETE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing utils\delete_properties_dialog_base.ui...
InputPath=utils\delete_properties_dialog_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic utils\delete_properties_dialog_base.ui -o utils\delete_properties_dialog_base.h \
	$(QTDIR)\bin\uic utils\delete_properties_dialog_base.ui -i delete_properties_dialog_base.h -o utils\delete_properties_dialog_base.cpp \
	$(QTDIR)\bin\moc utils\delete_properties_dialog_base.h -o .moc\moc_delete_properties_dialog_base.cpp \
	

"utils\delete_properties_dialog_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"utils\delete_properties_dialog_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_delete_properties_dialog_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__DELETE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing utils\delete_properties_dialog_base.ui...
InputPath=utils\delete_properties_dialog_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic utils\delete_properties_dialog_base.ui -o utils\delete_properties_dialog_base.h \
	$(QTDIR)\bin\uic utils\delete_properties_dialog_base.ui -i delete_properties_dialog_base.h -o utils\delete_properties_dialog_base.cpp \
	$(QTDIR)\bin\moc utils\delete_properties_dialog_base.h -o .moc\moc_delete_properties_dialog_base.cpp \
	

"utils\delete_properties_dialog_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"utils\delete_properties_dialog_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_delete_properties_dialog_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\display_pref_panel_base.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__DISPLA="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\display_pref_panel_base.ui...
InputPath=appli\display_pref_panel_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\display_pref_panel_base.ui -o appli\display_pref_panel_base.h \
	$(QTDIR)\bin\uic appli\display_pref_panel_base.ui -i display_pref_panel_base.h -o appli\display_pref_panel_base.cpp \
	$(QTDIR)\bin\moc appli\display_pref_panel_base.h -o .moc\moc_display_pref_panel_base.cpp \
	

"appli\display_pref_panel_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\display_pref_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_display_pref_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__DISPLA="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\display_pref_panel_base.ui...
InputPath=appli\display_pref_panel_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\display_pref_panel_base.ui -o appli\display_pref_panel_base.h \
	$(QTDIR)\bin\uic appli\display_pref_panel_base.ui -i display_pref_panel_base.h -o appli\display_pref_panel_base.cpp \
	$(QTDIR)\bin\moc appli\display_pref_panel_base.h -o .moc\moc_display_pref_panel_base.cpp \
	

"appli\display_pref_panel_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\display_pref_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_display_pref_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variogram2\first_scr_base.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__FIRST_="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variogram2\first_scr_base.ui...
InputPath=variogram2\first_scr_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic variogram2\first_scr_base.ui -o variogram2\first_scr_base.h \
	$(QTDIR)\bin\uic variogram2\first_scr_base.ui -i first_scr_base.h -o variogram2\first_scr_base.cpp \
	$(QTDIR)\bin\moc variogram2\first_scr_base.h -o .moc\moc_first_scr_base.cpp \
	

"variogram2\first_scr_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variogram2\first_scr_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_first_scr_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__FIRST_="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variogram2\first_scr_base.ui...
InputPath=variogram2\first_scr_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic variogram2\first_scr_base.ui -o variogram2\first_scr_base.h \
	$(QTDIR)\bin\uic variogram2\first_scr_base.ui -i first_scr_base.h -o variogram2\first_scr_base.cpp \
	$(QTDIR)\bin\moc variogram2\first_scr_base.h -o .moc\moc_first_scr_base.cpp \
	

"variogram2\first_scr_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variogram2\first_scr_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_first_scr_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\general_display_pref_panel_base.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__GENERA="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\general_display_pref_panel_base.ui...
InputPath=appli\general_display_pref_panel_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\general_display_pref_panel_base.ui -o appli\general_display_pref_panel_base.h \
	$(QTDIR)\bin\uic appli\general_display_pref_panel_base.ui -i general_display_pref_panel_base.h -o appli\general_display_pref_panel_base.cpp \
	$(QTDIR)\bin\moc appli\general_display_pref_panel_base.h -o .moc\moc_general_display_pref_panel_base.cpp \
	

"appli\general_display_pref_panel_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\general_display_pref_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_general_display_pref_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__GENERA="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\general_display_pref_panel_base.ui...
InputPath=appli\general_display_pref_panel_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\general_display_pref_panel_base.ui -o appli\general_display_pref_panel_base.h \
	$(QTDIR)\bin\uic appli\general_display_pref_panel_base.ui -i general_display_pref_panel_base.h -o appli\general_display_pref_panel_base.cpp \
	$(QTDIR)\bin\moc appli\general_display_pref_panel_base.h -o .moc\moc_general_display_pref_panel_base.cpp \
	

"appli\general_display_pref_panel_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\general_display_pref_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_general_display_pref_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\histogram_control_panel_base.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__HISTOGR="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\histogram_control_panel_base.ui...
InputPath=appli\histogram_control_panel_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\histogram_control_panel_base.ui -o appli\histogram_control_panel_base.h \
	$(QTDIR)\bin\uic appli\histogram_control_panel_base.ui -i histogram_control_panel_base.h -o appli\histogram_control_panel_base.cpp \
	$(QTDIR)\bin\moc appli\histogram_control_panel_base.h -o .moc\moc_histogram_control_panel_base.cpp \
	

"appli\histogram_control_panel_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\histogram_control_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_histogram_control_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__HISTOGR="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\histogram_control_panel_base.ui...
InputPath=appli\histogram_control_panel_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\histogram_control_panel_base.ui -o appli\histogram_control_panel_base.h \
	$(QTDIR)\bin\uic appli\histogram_control_panel_base.ui -i histogram_control_panel_base.h -o appli\histogram_control_panel_base.cpp \
	$(QTDIR)\bin\moc appli\histogram_control_panel_base.h -o .moc\moc_histogram_control_panel_base.cpp \
	

"appli\histogram_control_panel_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\histogram_control_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_histogram_control_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\MainWindow.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__MAINW="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\MainWindow.ui...
InputPath=appli\MainWindow.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\MainWindow.ui -o appli\MainWindow.h \
	$(QTDIR)\bin\uic appli\MainWindow.ui -i MainWindow.h -o appli\MainWindow.cpp \
	$(QTDIR)\bin\moc appli\MainWindow.h -o .moc\moc_MainWindow.cpp \
	

"appli\MainWindow.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\MainWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_MainWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__MAINW="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\MainWindow.ui...
InputPath=appli\MainWindow.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\MainWindow.ui -o appli\MainWindow.h \
	$(QTDIR)\bin\uic appli\MainWindow.ui -i MainWindow.h -o appli\MainWindow.cpp \
	$(QTDIR)\bin\moc appli\MainWindow.h -o .moc\moc_MainWindow.cpp \
	

"appli\MainWindow.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\MainWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_MainWindow.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=utils\multichoice_dialog_base.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__MULTIC="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing utils\multichoice_dialog_base.ui...
InputPath=utils\multichoice_dialog_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic utils\multichoice_dialog_base.ui -o utils\multichoice_dialog_base.h \
	$(QTDIR)\bin\uic utils\multichoice_dialog_base.ui -i multichoice_dialog_base.h -o utils\multichoice_dialog_base.cpp \
	$(QTDIR)\bin\moc utils\multichoice_dialog_base.h -o .moc\moc_multichoice_dialog_base.cpp \
	

"utils\multichoice_dialog_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"utils\multichoice_dialog_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_multichoice_dialog_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__MULTIC="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing utils\multichoice_dialog_base.ui...
InputPath=utils\multichoice_dialog_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic utils\multichoice_dialog_base.ui -o utils\multichoice_dialog_base.h \
	$(QTDIR)\bin\uic utils\multichoice_dialog_base.ui -i multichoice_dialog_base.h -o utils\multichoice_dialog_base.cpp \
	$(QTDIR)\bin\moc utils\multichoice_dialog_base.h -o .moc\moc_multichoice_dialog_base.cpp \
	

"utils\multichoice_dialog_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"utils\multichoice_dialog_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_multichoice_dialog_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\new_cartesian_grid_dialog_base.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__NEW_C="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\new_cartesian_grid_dialog_base.ui...
InputPath=appli\new_cartesian_grid_dialog_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\new_cartesian_grid_dialog_base.ui -o appli\new_cartesian_grid_dialog_base.h \
	$(QTDIR)\bin\uic appli\new_cartesian_grid_dialog_base.ui -i new_cartesian_grid_dialog_base.h -o appli\new_cartesian_grid_dialog_base.cpp \
	$(QTDIR)\bin\moc appli\new_cartesian_grid_dialog_base.h -o .moc\moc_new_cartesian_grid_dialog_base.cpp \
	

"appli\new_cartesian_grid_dialog_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\new_cartesian_grid_dialog_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_new_cartesian_grid_dialog_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__NEW_C="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\new_cartesian_grid_dialog_base.ui...
InputPath=appli\new_cartesian_grid_dialog_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\new_cartesian_grid_dialog_base.ui -o appli\new_cartesian_grid_dialog_base.h \
	$(QTDIR)\bin\uic appli\new_cartesian_grid_dialog_base.ui -i new_cartesian_grid_dialog_base.h -o appli\new_cartesian_grid_dialog_base.cpp \
	$(QTDIR)\bin\moc appli\new_cartesian_grid_dialog_base.h -o .moc\moc_new_cartesian_grid_dialog_base.cpp \
	

"appli\new_cartesian_grid_dialog_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\new_cartesian_grid_dialog_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_new_cartesian_grid_dialog_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variogram2\point_set_base.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__POINT="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variogram2\point_set_base.ui...
InputPath=variogram2\point_set_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic variogram2\point_set_base.ui -o variogram2\point_set_base.h \
	$(QTDIR)\bin\uic variogram2\point_set_base.ui -i point_set_base.h -o variogram2\point_set_base.cpp \
	$(QTDIR)\bin\moc variogram2\point_set_base.h -o .moc\moc_point_set_base.cpp \
	

"variogram2\point_set_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variogram2\point_set_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_point_set_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__POINT="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variogram2\point_set_base.ui...
InputPath=variogram2\point_set_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic variogram2\point_set_base.ui -o variogram2\point_set_base.h \
	$(QTDIR)\bin\uic variogram2\point_set_base.ui -i point_set_base.h -o variogram2\point_set_base.cpp \
	$(QTDIR)\bin\moc variogram2\point_set_base.h -o .moc\moc_point_set_base.cpp \
	

"variogram2\point_set_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variogram2\point_set_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_point_set_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\Project_view_form.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__PROJE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\Project_view_form.ui...
InputPath=appli\Project_view_form.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\Project_view_form.ui -o appli\Project_view_form.h \
	$(QTDIR)\bin\uic appli\Project_view_form.ui -i Project_view_form.h -o appli\Project_view_form.cpp \
	$(QTDIR)\bin\moc appli\Project_view_form.h -o .moc\moc_Project_view_form.cpp \
	

"appli\Project_view_form.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\Project_view_form.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_Project_view_form.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__PROJE="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\Project_view_form.ui...
InputPath=appli\Project_view_form.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\Project_view_form.ui -o appli\Project_view_form.h \
	$(QTDIR)\bin\uic appli\Project_view_form.ui -i Project_view_form.h -o appli\Project_view_form.cpp \
	$(QTDIR)\bin\moc appli\Project_view_form.h -o .moc\moc_Project_view_form.cpp \
	

"appli\Project_view_form.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\Project_view_form.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_Project_view_form.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\qpplot_control_panel_base.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__QPPLOT_="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\qpplot_control_panel_base.ui...
InputPath=appli\qpplot_control_panel_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\qpplot_control_panel_base.ui -o appli\qpplot_control_panel_base.h \
	$(QTDIR)\bin\uic appli\qpplot_control_panel_base.ui -i qpplot_control_panel_base.h -o appli\qpplot_control_panel_base.cpp \
	$(QTDIR)\bin\moc appli\qpplot_control_panel_base.h -o .moc\moc_qpplot_control_panel_base.cpp \
	

"appli\qpplot_control_panel_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\qpplot_control_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_qpplot_control_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__QPPLOT_="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\qpplot_control_panel_base.ui...
InputPath=appli\qpplot_control_panel_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\qpplot_control_panel_base.ui -o appli\qpplot_control_panel_base.h \
	$(QTDIR)\bin\uic appli\qpplot_control_panel_base.ui -i qpplot_control_panel_base.h -o appli\qpplot_control_panel_base.cpp \
	$(QTDIR)\bin\moc appli\qpplot_control_panel_base.h -o .moc\moc_qpplot_control_panel_base.cpp \
	

"appli\qpplot_control_panel_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\qpplot_control_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_qpplot_control_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variogram2\rgrid_base.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__RGRID_="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variogram2\rgrid_base.ui...
InputPath=variogram2\rgrid_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic variogram2\rgrid_base.ui -o variogram2\rgrid_base.h \
	$(QTDIR)\bin\uic variogram2\rgrid_base.ui -i rgrid_base.h -o variogram2\rgrid_base.cpp \
	$(QTDIR)\bin\moc variogram2\rgrid_base.h -o .moc\moc_rgrid_base.cpp \
	

"variogram2\rgrid_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variogram2\rgrid_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_rgrid_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__RGRID_="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variogram2\rgrid_base.ui...
InputPath=variogram2\rgrid_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic variogram2\rgrid_base.ui -o variogram2\rgrid_base.h \
	$(QTDIR)\bin\uic variogram2\rgrid_base.ui -i rgrid_base.h -o variogram2\rgrid_base.cpp \
	$(QTDIR)\bin\moc variogram2\rgrid_base.h -o .moc\moc_rgrid_base.cpp \
	

"variogram2\rgrid_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variogram2\rgrid_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_rgrid_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\scatterplot_control_panel_base.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__SCATTER="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\scatterplot_control_panel_base.ui...
InputPath=appli\scatterplot_control_panel_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\scatterplot_control_panel_base.ui -o appli\scatterplot_control_panel_base.h \
	$(QTDIR)\bin\uic appli\scatterplot_control_panel_base.ui -i scatterplot_control_panel_base.h -o appli\scatterplot_control_panel_base.cpp \
	$(QTDIR)\bin\moc appli\scatterplot_control_panel_base.h -o .moc\moc_scatterplot_control_panel_base.cpp \
	

"appli\scatterplot_control_panel_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\scatterplot_control_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_scatterplot_control_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__SCATTER="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\scatterplot_control_panel_base.ui...
InputPath=appli\scatterplot_control_panel_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\scatterplot_control_panel_base.ui -o appli\scatterplot_control_panel_base.h \
	$(QTDIR)\bin\uic appli\scatterplot_control_panel_base.ui -i scatterplot_control_panel_base.h -o appli\scatterplot_control_panel_base.cpp \
	$(QTDIR)\bin\moc appli\scatterplot_control_panel_base.h -o .moc\moc_scatterplot_control_panel_base.cpp \
	

"appli\scatterplot_control_panel_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\scatterplot_control_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_scatterplot_control_panel_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=appli\snapshot_dialog_base.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__SNAPSH="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\snapshot_dialog_base.ui...
InputPath=appli\snapshot_dialog_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\snapshot_dialog_base.ui -o appli\snapshot_dialog_base.h \
	$(QTDIR)\bin\uic appli\snapshot_dialog_base.ui -i snapshot_dialog_base.h -o appli\snapshot_dialog_base.cpp \
	$(QTDIR)\bin\moc appli\snapshot_dialog_base.h -o .moc\moc_snapshot_dialog_base.cpp \
	

"appli\snapshot_dialog_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\snapshot_dialog_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_snapshot_dialog_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__SNAPSH="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing appli\snapshot_dialog_base.ui...
InputPath=appli\snapshot_dialog_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic appli\snapshot_dialog_base.ui -o appli\snapshot_dialog_base.h \
	$(QTDIR)\bin\uic appli\snapshot_dialog_base.ui -i snapshot_dialog_base.h -o appli\snapshot_dialog_base.cpp \
	$(QTDIR)\bin\moc appli\snapshot_dialog_base.h -o .moc\moc_snapshot_dialog_base.cpp \
	

"appli\snapshot_dialog_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"appli\snapshot_dialog_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_snapshot_dialog_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=variogram2\variogram_structure_controls_base.ui

!IF  "$(CFG)" == "GsTLAppli_gui - Win32 Release"

USERDEP__VARIOGRAM_="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variogram2\variogram_structure_controls_base.ui...
InputPath=variogram2\variogram_structure_controls_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic variogram2\variogram_structure_controls_base.ui -o variogram2\variogram_structure_controls_base.h \
	$(QTDIR)\bin\uic variogram2\variogram_structure_controls_base.ui -i variogram_structure_controls_base.h -o variogram2\variogram_structure_controls_base.cpp \
	$(QTDIR)\bin\moc variogram2\variogram_structure_controls_base.h -o .moc\moc_variogram_structure_controls_base.cpp \
	

"variogram2\variogram_structure_controls_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variogram2\variogram_structure_controls_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_variogram_structure_controls_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "GsTLAppli_gui - Win32 Debug"

USERDEP__VARIOGRAM_="$(QTDIR)\bin\moc.exe"	"$(QTDIR)\bin\uic.exe"	
# Begin Custom Build - Uic'ing variogram2\variogram_structure_controls_base.ui...
InputPath=variogram2\variogram_structure_controls_base.ui

BuildCmds= \
	$(QTDIR)\bin\uic variogram2\variogram_structure_controls_base.ui -o variogram2\variogram_structure_controls_base.h \
	$(QTDIR)\bin\uic variogram2\variogram_structure_controls_base.ui -i variogram_structure_controls_base.h -o variogram2\variogram_structure_controls_base.cpp \
	$(QTDIR)\bin\moc variogram2\variogram_structure_controls_base.h -o .moc\moc_variogram_structure_controls_base.cpp \
	

"variogram2\variogram_structure_controls_base.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"variogram2\variogram_structure_controls_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".moc\moc_variogram_structure_controls_base.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Generated"

# PROP Default_Filter ""
# Begin Source File

SOURCE=appli\controls_widget.cpp
# End Source File
# Begin Source File

SOURCE=appli\controls_widget.h
# End Source File
# Begin Source File

SOURCE=variogram2\cross_first_scr_base.cpp
# End Source File
# Begin Source File

SOURCE=variogram2\cross_first_scr_base.h
# End Source File
# Begin Source File

SOURCE=variogram2\cross_pset_rgrid_base.cpp
# End Source File
# Begin Source File

SOURCE=variogram2\cross_pset_rgrid_base.h
# End Source File
# Begin Source File

SOURCE=appli\data_analysis_gui_base.cpp
# End Source File
# Begin Source File

SOURCE=appli\data_analysis_gui_base.h
# End Source File
# Begin Source File

SOURCE=utils\delete_properties_dialog_base.cpp
# End Source File
# Begin Source File

SOURCE=utils\delete_properties_dialog_base.h
# End Source File
# Begin Source File

SOURCE=appli\display_pref_panel_base.cpp
# End Source File
# Begin Source File

SOURCE=appli\display_pref_panel_base.h
# End Source File
# Begin Source File

SOURCE=variogram2\first_scr_base.cpp
# End Source File
# Begin Source File

SOURCE=variogram2\first_scr_base.h
# End Source File
# Begin Source File

SOURCE=appli\general_display_pref_panel_base.cpp
# End Source File
# Begin Source File

SOURCE=appli\general_display_pref_panel_base.h
# End Source File
# Begin Source File

SOURCE=appli\histogram_control_panel_base.cpp
# End Source File
# Begin Source File

SOURCE=appli\histogram_control_panel_base.h
# End Source File
# Begin Source File

SOURCE=appli\MainWindow.cpp
# End Source File
# Begin Source File

SOURCE=appli\MainWindow.h
# End Source File
# Begin Source File

SOURCE=.moc\moc_cli_commands_panel.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_controls_widget.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_cross_first_scr.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_cross_first_scr_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_cross_pset_rgrid_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_cross_pset_rgrid_params.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_cross_variogram_modeler_gui.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_data_analysis_gui.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_data_analysis_gui_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_delete_properties_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_delete_properties_dialog_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_display_pref_panel.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_display_pref_panel_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_first_scr.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_first_scr_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_general_display_pref_panel.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_general_display_pref_panel_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_gstl_slider.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_histogram_control_panel.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_histogram_control_panel_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_histogram_gui.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_MainWindow.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_multichoice_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_multichoice_dialog_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_new_cartesian_grid_dialog_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_oinv_project_view.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_point_set_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_Project_view_form.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_pset_params.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_qcombobox_accessor.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_qdirdialog.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_qpplot_control_panel.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_qpplot_control_panel_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_qpplot_gui.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_qt_algo_control_panel.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_qt_sp_application.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_rgrid_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_rgrid_params.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_scatterplot_control_panel.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_scatterplot_control_panel_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_scatterplot_gui.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_script_editor.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_snapshot_dialog.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_snapshot_dialog_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_variog_plot.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_variogram_controls.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_variogram_controls_base.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_variogram_modeler_gui.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_variogram_modeling_screen.cpp
# End Source File
# Begin Source File

SOURCE=.moc\moc_variogram_structure_controls_base.cpp
# End Source File
# Begin Source File

SOURCE=utils\multichoice_dialog_base.cpp
# End Source File
# Begin Source File

SOURCE=utils\multichoice_dialog_base.h
# End Source File
# Begin Source File

SOURCE=appli\new_cartesian_grid_dialog_base.cpp
# End Source File
# Begin Source File

SOURCE=appli\new_cartesian_grid_dialog_base.h
# End Source File
# Begin Source File

SOURCE=variogram2\point_set_base.cpp
# End Source File
# Begin Source File

SOURCE=variogram2\point_set_base.h
# End Source File
# Begin Source File

SOURCE=appli\Project_view_form.cpp
# End Source File
# Begin Source File

SOURCE=appli\Project_view_form.h
# End Source File
# Begin Source File

SOURCE=appli\qpplot_control_panel_base.cpp
# End Source File
# Begin Source File

SOURCE=appli\qpplot_control_panel_base.h
# End Source File
# Begin Source File

SOURCE=variogram2\rgrid_base.cpp
# End Source File
# Begin Source File

SOURCE=variogram2\rgrid_base.h
# End Source File
# Begin Source File

SOURCE=appli\scatterplot_control_panel_base.cpp
# End Source File
# Begin Source File

SOURCE=appli\scatterplot_control_panel_base.h
# End Source File
# Begin Source File

SOURCE=appli\snapshot_dialog_base.cpp
# End Source File
# Begin Source File

SOURCE=appli\snapshot_dialog_base.h
# End Source File
# Begin Source File

SOURCE=variogram2\variogram_structure_controls_base.cpp
# End Source File
# Begin Source File

SOURCE=variogram2\variogram_structure_controls_base.h
# End Source File
# End Group
# End Target
# End Project
