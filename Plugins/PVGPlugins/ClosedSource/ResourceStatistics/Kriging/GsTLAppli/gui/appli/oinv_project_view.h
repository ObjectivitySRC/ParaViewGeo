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

#ifndef __GSTLAPPLI_GUI_APP_PROJECT_VIEW_H__ 
#define __GSTLAPPLI_GUI_APP_PROJECT_VIEW_H__ 
 
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/utils/named_interface.h> 
#include <GsTLAppli/appli/project.h> 
#include <GsTLAppli/gui/oinv_description/oinv_description.h> 
 
#include <GsTLAppli/gui/appli/Project_view_form.h>  // QT generated file 
#include <GsTLAppli/gui/appli/general_display_pref_panel.h>
 
#include <map> 
#include <qstring.h>
 
class Display_pref_panel; 
class General_display_pref_panel; 
class SoQtGsTLViewer; 
class SoSelection; 
class SoSeparator; 
class SoGroup; 
class SoTransform; 
class SbVec3f; 
class QString; 
class QVBox; 
class QScrollView; 
class QWidget; 
class GridSelector;
 
 
/** This class is a convenience class that behaves essentially as an std::map. 
 * An Oinv_description_map is a set of descriptions of currently available 
 * grid objects: given a grid object, it can output a pointer to an  
 * oinv description of the grid. The main difference with std::map is that 
 * if asked for a description which is not already in the  
 * map, a new description will be created and added to the map and to the Open  
 * Inventor scene-graph; while std::map::find(...) would simply have signaled 
 * that no such map was available. 
 */ 
class GUI_DECL Oinv_description_map {  
 private: 
  typedef std::map< std::string, SmartPtr<Oinv_description> > Desc_Map; 
 
 public: 
  typedef Desc_Map::iterator iterator; 
  typedef Desc_Map::const_iterator const_iterator; 
  typedef std::pair<bool,Oinv_description*> pair; 
 
 public:   
 
  Oinv_description_map( SoGroup* scene_root = 0 ); 
  ~Oinv_description_map() {} 
 
  void scene_graph( SoGroup* scene_root ); 
 
  /** Gets the description corresponding to object "obj_name". If the description 
   * is not already in the database of descriptions, a new description is created 
   * and added to the database and to the Open Inventor scene-graph. 
   * It returns a pair: 
   *   - a bool which is true if the description was in the map before 
   *   - a pointer to the requested description 
   */ 
  std::pair<bool,Oinv_description*> description( const std::string& obj_name ); 

  /** Removes the description of \c obj_name from the database and from the 
  * Open Inventor scene graph. It returns true if the deletion was successful.
  */
  bool delete_description( const std::string& obj_name ); 
  
  iterator find( const std::string& obj_name );
  iterator begin() { return available_descriptions_.begin(); } 
  const_iterator begin() const { return available_descriptions_.begin(); } 
  iterator end() { return available_descriptions_.end(); } 
  const_iterator end() const { return available_descriptions_.end(); } 
 
 
 private: 
  Desc_Map available_descriptions_ ; 
  SoGroup* scene_root_; 
}; 
 
 
 
//=================================== 
 
/** The gui part of the Open Inventor view. It includes the display zone  
 * and the control panel (ie the set of controls to select what object  
 * to display, to set display preferences, etc). 
 */ 
 
class GUI_DECL Project_view_gui : public Project_view_form { 
 
  Q_OBJECT 
 
 public: 
  Project_view_gui( QWidget* parent = 0 ); 
  void set_parent( QWidget* parent ); 

  //TL modified
  void load_colorbar(QString & s){ general_pref_panel_->load_colorbar_fromfile(s); }
 
  virtual ~Project_view_gui(); 
 
  /** Adds an object in the object-tree 
   */ 
  void add_object( std::string obj_name ); 
 
  const SoQtGsTLViewer* get_render_area() const { return oinv_viewer_; }
  SoQtGsTLViewer* get_render_area() { return oinv_viewer_; }

  void set_object_displayed( const QString& obj ); 
  void set_object_undisplayed( const QString& obj ); 
  void set_property_displayed( const QString& grid, const QString& prop ); 
  void set_property_undisplayed( const QString& grid, const QString& prop ); 
  void set_colormap( const QString& cmap_name,  
                     const QString& grid, const QString& prop,
                     const QString& min = QString::null, 
                     const QString& max = QString::null );

 public slots: 
  void update_display( QListViewItem* ); 
  void object_clicked_slot( int button, QListViewItem*, const QPoint&, int ); 
  void show_preference_panel( const QString& obj ); 
  void show_info_panel( const QString& obj ); 
  //  void z_scaling( int factor ); 
  void toggle_grid_property( const QString& grid, const QString& prop ); 
  void view_all(); 
  void set_home_view();
  void home_view();
  void face_view(); 
  void side_view(); 
  void top_view(); 
  void snapshot(); 
  void save_scenegraph_to_file( const QString& filename );


 protected: 
  virtual void init_objects_selector(); 
  virtual void set_view_plane( const SbVec3f& newNormal, const SbVec3f& newRight ); 
  virtual void display_object( const QString& obj ); 
  virtual void undisplay_object( const QString& obj ); 
  virtual void display_property( const QString& grid, const QString& prop ); 
  virtual void undisplay_property( const QString& grid, const QString& prop ); 

  QListViewItem* get_grid_listitem( const QString& grid ); 
  QListViewItem* get_property_listitem( const QString& grid, 
                                        const QString& prop ); 

 protected: 
  typedef std::map< std::string, Display_pref_panel* > Pref_Panel_Map; 
  Oinv_description_map displayed_objects_ ; 
  Pref_Panel_Map display_pref_panels_; 
  QWidget* current_pref_panel_; 
  General_display_pref_panel* general_pref_panel_; 
 
 protected: 
  GridSelector* pref_object_selector_;
  QVBox* pref_panel_frame_; 
  static const QString general_pref_panel_name_; 
 
  GridSelector* info_object_selector_;
  QVBox* info_panel_frame_; 
  QWidget* current_info_panel_;

 protected: 
  SoQtGsTLViewer* oinv_viewer_; 
  SoSeparator* scene_root_; 
  SoSeparator* colormap_root_node_; 
  
  //TL modified
  QString _oldName;



}; 
 
 
 
 
//=================================== 
 
/** A view for Open Inventor and QT. 
 * This class inherits from a QWidget, and implements the Project_view 
 * interface. 
 * Note: All the parts relative to the gui, eg update_display, show_preference_panel, 
 * the map of descriptions, etc, should be moved to Project_view_gui. 
 */ 
 
class GUI_DECL Oinv_view : public Project_view_gui, public Project_view { 
 
  Q_OBJECT 
 
 public: 
 
  Oinv_view( GsTL_project* project = 0, QWidget* parent = 0 ) ; 
  virtual void initialize( GsTL_project* project, QWidget* parent ) ; 
  virtual ~Oinv_view() ; 
  virtual void update( std::string obj ); 
  virtual void new_object( std::string obj ); 
  virtual void deleted_object( std::string obj ); 
 
public slots:
	  //TL modified
  void object_rename_slot(QListViewItem * , int , const QString &);

protected:
  void dragEnterEvent( QDragEnterEvent * );
  void dragMoveEvent( QDragMoveEvent * );
  void dragLeaveEvent( QDragLeaveEvent * );
  void dropEvent( QDropEvent * );


}; 
 
 
Named_interface* Create_oinv_view( std::string& ); 
 
 
 
 
 
#endif 
