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

#ifndef __GSTLAPPLI_VARIOGRAM_CONTROLS_H__
#define __GSTLAPPLI_VARIOGRAM_CONTROLS_H__

#include <GsTL/geometry/covariance.h>

#include <GsTLAppli/gui/variogram2/variogram_controls_base.h>
#include <GsTLAppli/gui/variogram2/variogram_structure_controls_base.h>


#include <GsTLAppli/math/gstlpoint.h>
#include <GsTLAppli/math/gstlvector.h>


#include <qwidget.h>
#include <qslider.h>
#include <qstring.h>
#include <qtable.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qframe.h>
#include <qfont.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qgroupbox.h>
#include <vector>

class GsTL_slider;
class Variogram_structure_controls;
class Line_separator;
class QVBoxLayout;
class QLineEdit;
class QSpinBox;
class QVBox;



#include <qwidget.h>

class QLineEdit;
class QSpinBox;
class QVBox;


class Variogram_controls : public QWidget {

  Q_OBJECT

public:
  Variogram_controls( Covariance<GsTLPoint>* model = 0,
                      QWidget* parent = 0, const char* name = 0,
                      double default_max_range = 10.0);

  float nugget() const;
  int structures_count() const;
  const Variogram_structure_controls* structure( int id ) const;
  Variogram_structure_controls* structure( int id );
  
//  virtual QSize sizeHint() const { return QSize( 300, 300 ); }

signals:
  void variogram_changed();
  
     
public slots:
  void nugget_changed(const QString &s)   ;
  void update_structures_count( int val );
  void load_model();
  void save_model();

protected:
  typedef std::pair< Line_separator*, Variogram_structure_controls* > Struct_pair;
  typedef std::vector< Struct_pair > Struct_vector;
  Struct_vector structures_;

protected:
  QLineEdit* nugget_effect_edit_;
  QSpinBox* structures_count_;
  QVBox* structures_frame_;

private:
  Covariance<GsTLPoint>* model_;
  double default_max_range_;
};


//=============================================

class Variogram_structure_controls : public Variogram_structure_controls_base {

  Q_OBJECT

 public:
  Variogram_structure_controls(){}
  Variogram_structure_controls( Covariance<GsTLPoint>* model, 
                                QWidget* parent = 0, const char* name = 0,
                                double default_max_range = 10.0 );

  virtual QSize sizeHint() const { return QSize( 300, 250 ); }

  void set_id(int id);
  
  QString type() const;
  double sill() const;
  void ranges( double& range1, double& range2, double& range3 );
  void angles( double& angle1, double& angle2, double& angle3 );

 signals:
  void variogram_structure_changed(); 
 
 public slots:

  void range_changed(double d);    
  void range1_changed(double d);    
  void range2_changed(double d);    
  void range3_changed(double d);    
  void sill_changed(const QString &str);
  void angle_changed(const QString &str);
  void type_changed(const QString &str);

  /*
  void set_sill( double s );
  void set_angles( double a, double b, double c );
  void set_ranges( double a, double b, double c );
  void set_type( const QString &str );
  */
  void set_structure( double sill, const QString& type,
                      double range1, double range2, double range3,
                      double angle1, double angle2, double angle3 );

 protected:
  GsTL_slider* range_slider_1_;
  GsTL_slider* range_slider_2_;
  GsTL_slider* range_slider_3_;

  
  
 private:
  Covariance<GsTLPoint>* model_;
  int id_;

};



//=============================================

class Line_separator : public QWidget {

  Q_OBJECT

 public:
  Line_separator( const QString& label,
                  QWidget* parent = 0, const char* name = 0 );

};




#endif
