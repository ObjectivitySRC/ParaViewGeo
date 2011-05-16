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

#ifndef __GSTLAPPLI_QT_ALGO_CONTROL_PANEL_H__ 
#define __GSTLAPPLI_QT_ALGO_CONTROL_PANEL_H__ 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/appli/controls_widget.h> 
 
#include <qwidget.h> 
#include <qlistview.h> 
#include <qframe.h>
 
#include <string> 
#include <vector>
#include <map>
 
class Geostat_algo; 
class GsTL_project; 
class Error_messages_handler_xml;
 
class QFrame; 
class QVBoxLayout; 
class QListViewItem; 
class QScrollView; 
 
 
//====================================== 
 
class GUI_DECL GsTL_QListView : public QListView { 
 
  Q_OBJECT 
 
 public: 
  GsTL_QListView( QWidget* parent = 0, const char* name = 0 ); 
 
  /** Add a new item to the listview. The item name is a string of the  
   * following format: 
   *   /item1/item1_1/item1_1_1   (if sep=="/" ) 
   * If one of the elements of the path doesn't exist, it is added to the  
   * listview 
   */ 
  virtual QListViewItem* addItem( const QString& item, 
				  const QString& sep="/", 
				  const QString& text_col2 = QString::null );  

}; 


 
//====================================== 
 
class GUI_DECL Parameters_manager {
public:
  Parameters_manager() {}
  ~Parameters_manager() {}

  std::pair<std::string*,bool> parameters( const std::string& algo_name );

  /** add new parameters in the manager. If the parameters for that algo
  * already exist, they are replaced by the new ones.
  */
  bool add_parameters( const std::string& algo_name, 
                       const std::string& parameters);

private:
  typedef std::map<std::string, std::string > ParamMap;
  ParamMap map_;
  std::string empty_;
};

//====================================== 
 
/** The controls panel is the widget containing the buttons to 
 * load, save parameters, run the algorithm... 
 */ 
class GUI_DECL Controls_panel : public ControlsWidget { 
 
  Q_OBJECT 
 
 public: 
  Controls_panel( QWidget* parent = 0,  
		  const char* name = 0, WFlags f = 0 ); 
 
 signals: 
  void run_algo_selected(); 
  void clear_selected(); 
  void load_selected(); 
  void save_selected(); 
};  
 
 
 
//====================================== 
 
/** The algo control panel is the widget that provides the list 
 * of all algorithms, prompts for parameters input and has the 
 * the "controls panel". 
 */ 
class GUI_DECL Algo_control_panel : public QWidget { 
 
  Q_OBJECT 
 
 public: 
  Algo_control_panel( GsTL_project* project, 
		      QWidget* parent = 0,  
		      const char* name = 0, WFlags f = 0 ); 
  virtual ~Algo_control_panel() {} 
   
  virtual void set_project( GsTL_project* project ) { project_ = project; } 
 
 
 public slots: 
  void prompt_for_parameters( int button, 
                              QListViewItem* item, const QPoint& pnt, int c ); 
  void run_geostat_algo(); 
  void load_parameters(); 
  void load_parameters( const QString& ); 
  void clear_parameters(); 
  void save_parameters(); 
 
 
 protected: 
  virtual void setup_algo_selector(); 
  virtual void add_algorithm( Geostat_algo* algo, const std::string& entry_name ); 
  virtual void report_errors( Error_messages_handler_xml* errors_handler,
                              QWidget* parent );
  virtual void reset_param_input_widget();

 protected:
  void dragEnterEvent( QDragEnterEvent * );
  void dragMoveEvent( QDragMoveEvent * );
  void dragLeaveEvent( QDragLeaveEvent * );
  void dropEvent( QDropEvent * );

 private:
  void set_widget_color( QWidget* widget, QColor color );

 protected: 
  GsTL_project* project_; 
  QWidget* param_input_; 
  std::string selected_algo_; 
 
  std::vector< QWidget* > flagged_widgets_;
  Parameters_manager parameters_manager_;

 protected: 
  GsTL_QListView* algo_selector_; 
  Controls_panel* controls_; 
  QScrollView* param_input_frame_; 
  QVBoxLayout* layout_; 
 
 private: 
  // The name of the manager to use to access the Geostat Parameters Utilities 
  // The current manager is the XML-based utilities manager. 
  static const std::string utils_manager_; 
}; 
 
 
#endif 
