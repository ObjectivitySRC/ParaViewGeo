/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "gui" module of the Geostatistical Earth
** Modeling Software (GEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/

#ifndef __GSTLAPPLI_QT_SP_APPLICATION_H__ 
#define __GSTLAPPLI_QT_SP_APPLICATION_H__ 
 
#include <GsTLAppli/gui/common.h>
#include <qmainwindow.h> 
#include <qfiledialog.h> 
#include <qcheckbox.h>
 
#include <vector>
#include <string>

#include <qstring.h>

class GsTL_project; 
class Algo_control_panel; 
class CLI_commands_panel;
class Oinv_view; 
class QHBoxLayout; 
class QDockWindow; 
class QComboBox; 
class QString; 
class GridSelector;
class SinglePropertySelector;
class QCheckBox;

//TL modified
class QListBox;
class QCheckBox;
 
/** QT-single-project-application. 
 */ 
class GUI_DECL QSP_application : public QMainWindow { 
 
  Q_OBJECT 
 
 public: 
  QSP_application( QWidget* parent = 0 ); 
  virtual ~QSP_application(); 
 
  /** Ideally, this function should be called automatically by the constructor. 
   * Don't forget to call it !! 
   * It is not called by the constructor because of some issues with  
   * Open Inventor: the Oinv_view of the QSP_application can not be created 
   * before SoQt::init is called, and SoQt::init must be called after the 
   * QSP_application is created (maybe there is another way around, but this 
   * is the only solution I've found). 
   */ 
  void init(); 
 
 public slots: 
  void load_project(); 
  void save_project();
  void save_project_as();
  void save_project( const QString& name );
  bool close_project();
  void load_object(); 
  void save_object();  
  void new_cartesian_grid(); 
  void copy_property();

  void show_algo_panel(); 
  void show_commands_panel();
  void show_algo_panel( bool ); 
  void show_commands_panel( bool );

  void show_histogram_dialog();
  void show_qpplot_dialog(); 
  void show_scatterplot_dialog(); 
  void show_variogram_analyser();

  void delete_geostat_objects( const QStringList& name );
  void delete_geostat_objects();
  void delete_object_properties( const QString& qgrid_name,
                                 const QStringList& prop_names );
  void delete_object_properties();

  void show_script_editor();
  void run_script();

  void about_slot();
  void quit_slot();
  void save_app_preferences();

  void list_managers(); 
  void show_prop_val(); 
  void save_scenegraph(); 

  void new_camera();

 protected slots:
   virtual void closeEvent( QCloseEvent* e );
   
 protected:
   virtual void dragEnterEvent( QDragEnterEvent* );
   virtual void dropEvent ( QDropEvent * );


 private: 
  void init_menu_bar(); 
  QPopupMenu* view_menu_;
  int ap_;
  int cli_panel_id_;

 private: 
  GsTL_project* project_; 
   
  Oinv_view* default_3dview_; 
  Algo_control_panel* algo_panel_; 
  CLI_commands_panel* cli_panel_;

  QDockWindow* dock_controls_; 
  QDockWindow* dock_cli_; 
 
  QHBoxLayout* hlayout_; 

  // nico: code for additional Oinv views:
//  std::vector< Oinv_view* > additional_views_;

 private:
   struct Temp_preferences {
     QString last_input_filter;
     QString last_output_filter;
     QString last_load_directory;
     QString last_save_directory;
     //QString project_name;
   };
   Temp_preferences preferences_;
}; 
 



//-----------------------------------------------
/** Dialog box to save a grid. 
 */ 
class GUI_DECL Save_grid_dialog : public QFileDialog { 
 
  Q_OBJECT 
 
 public: 
  Save_grid_dialog( const GsTL_project* project,  
		    QWidget * parent = 0, const char * name = 0 ); 
  QString selected_grid() const; 
  const std::vector<QString> & selected_p() const { return _selected; }
  bool maskToRegular() {
	  if (!_saveRegular->isEnabled()) return false;
	  return _saveRegular->isChecked();
  }
 
  protected slots: 
    virtual void gridChanged(const QString &);
    virtual void accept(); 
 
 private: 
  QComboBox* grid_selector_; 
  QListBox * _propList;
  QCheckBox * _saveRegular;
  std::vector<QString> _selected;

}; 
 


//------------------------------------------------
class QComboBox;
class Copy_property_dialog : public QDialog {
  Q_OBJECT

public:
  Copy_property_dialog( GsTL_project* proj,
                        QWidget* parent = 0, const char* name = 0 );
  QString source() const;
  QString source_property() const ;
  QString destination() const;
  QString destination_property() const;
  bool overwrite() const;
  bool mark_as_hard() const;

  std::string parameters() const;

public slots:
  void copy_property();

protected:
  GridSelector* source_;
  SinglePropertySelector* source_property_;
  GridSelector* destination_;
  SinglePropertySelector* destination_property_;
//  QComboBox* destination_property_;
  QCheckBox* overwrite_;
  QCheckBox* mark_as_hard_;

  GsTL_project* project_;

};

 
#endif 
