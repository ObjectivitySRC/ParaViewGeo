/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "extra" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_QTPLUGINS_VARIOGRAM_INPUT_H__
#define __GSTLAPPLI_QTPLUGINS_VARIOGRAM_INPUT_H__

#include <GsTLAppli/extra/qtplugins/common.h>
#include <GsTLAppli/extra/qtplugins/variogram_structure_base.h>
#include <GsTLAppli/extra/qtplugins/variogram_input_base.h>
#include <GsTLAppli/extra/qtplugins/ellipsoid_input.h>

#include <qwidget.h>

#include <vector>

class GsTLTable;
class Variogram_structure_input;
class Line_separator;
class QVBoxLayout;
class QLineEdit;
class QSpinBox;
class QVBox;



class QTPLUGINS_DECL VariogramInput : public Variogram_input_base {

  Q_OBJECT

 public:
  VariogramInput( QWidget* parent = 0, const char* name = 0 );
  virtual ~VariogramInput() {}
//  virtual QSize sizeHint() const { return QSize( 260, 270 ); }

  float nugget() const;
  int structures_count() const;
  const Variogram_structure_input* structure( int id ) const;
  Variogram_structure_input* structure( int id );

 public slots:
  void set_nugget( float f );
  void update_structures_count( int val );
  void load_model();

 protected:
  typedef std::pair< Line_separator*, Variogram_structure_input* > Struct_pair;
  typedef std::vector< Struct_pair > Struct_vector;
  Struct_vector structures_;

 protected:
  QVBox* structures_frame_;

 private:
  bool init_values_from_xml_string( QString& str );
};



//=============================================

class QTPLUGINS_DECL Variogram_structure_input : public Variogram_structure_base {

  Q_OBJECT

 public:
  Variogram_structure_input( QWidget* parent = 0, const char* name = 0 );
  
  float contribution() const;
  QString variogram_type() const;

  
  void ranges( float& max, float& medium, float& min ) const ; 
  float max_range() const;
  float medium_range() const;
  float min_range() const;
  
  void angles( float& x, float& y, float& z ) const ; 
  float x_angle() const;
  float y_angle() const;
  float z_angle() const;
  
 signals:
  void anisotropy_value_changed();
  void variogram_type_changed( const QString& );
  void contribution_changed( float );


 public slots:
  void set_contribution( float c );
  void set_variogram_type( const QString& type ); 
  void set_ranges( float max, float medium, float min );
  void max_range( float a );
  void medium_range( float a );
  void min_range( float a );
  void set_angles( float x, float y, float z );
  void x_angle( float a );
  void y_angle( float a );
  void z_angle( float a );
 

 private slots:
  void contribution_change_slot( const QString& );

 protected:
   EllipsoidInput* anisotropy_table_;
};



//=============================================

class QTPLUGINS_DECL Line_separator : public QWidget {

  Q_OBJECT

 public:
  Line_separator( const QString& label, 
		  QWidget* parent = 0, const char* name = 0 );
  
};


#endif
