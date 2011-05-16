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

#ifndef __GSTLAPPLI_GUI_QT_GRID_SUMMARY_H__ 
#define __GSTLAPPLI_GUI_QT_GRID_SUMMARY_H__
 
#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/utils/named_interface.h> 

class Geostat_grid; 
class QWidget;
 
/** QtGrid_summary is a desciptor of a geostat grid. As its name implies,
 * it gives summary information about the grid, in the form of a QWdiget,
 * ie a QWidget displaying the dimensions of the grid, etc
 *
 * Note: This class should derive from Project_view instead of Named_interface 
 * so that when the project is updated, the view is updated as well
 */

class GUI_DECL QtGrid_summary : public Named_interface { 
 public: 
  QtGrid_summary(); 
  virtual ~QtGrid_summary(); 
  virtual void init( const Geostat_grid* grid ) = 0; 
  virtual const Geostat_grid* described_grid() const {return geostat_grid_; } 
 
  virtual QWidget* description() { return desc_widget_; } 


 protected: 
  const Geostat_grid* geostat_grid_; 
 
  QWidget* desc_widget_; 
 
}; 
 


class GUI_DECL Qt_cartesian_grid_summary : public QtGrid_summary { 
public: 
  static Named_interface* create_new_interface( std::string& ); 
 
public: 
  Qt_cartesian_grid_summary(); 
  virtual ~Qt_cartesian_grid_summary() {}; 
  virtual void init( const Geostat_grid* grid ); 
 
}; 

//TL modified
class GUI_DECL Qt_masked_grid_summary : public QtGrid_summary { 
public: 
  static Named_interface* create_new_interface( std::string& ); 
 
public: 
  Qt_masked_grid_summary(); 
  virtual ~Qt_masked_grid_summary() {}; 
  virtual void init( const Geostat_grid* grid ); 
 
}; 

class GUI_DECL Qt_pointset_summary : public QtGrid_summary { 
public: 
  static Named_interface* create_new_interface( std::string& ); 
 
public: 
  Qt_pointset_summary(); 
  virtual ~Qt_pointset_summary() {}; 
  virtual void init( const Geostat_grid* grid ); 
 
}; 


#endif 
