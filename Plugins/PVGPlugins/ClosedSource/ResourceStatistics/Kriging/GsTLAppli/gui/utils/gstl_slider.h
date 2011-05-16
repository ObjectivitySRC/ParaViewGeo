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

#ifndef __GSTLAPPLI_GUI_UTILS_GSTL_SLIDER_H__
#define __GSTLAPPLI_GUI_UTILS_GSTL_SLIDER_H__


#include <qhbox.h>
#include <qslider.h>

class QLineEdit;
class QPushButton;
class QLabel;

class GsTL_slider : public QHBox {

  Q_OBJECT 

 public:
  GsTL_slider( double default_max, const QString& title, 
	             QWidget* parent = 0, const char* name = 0 );  
  GsTL_slider( double min_value, double max_value, 
	             double value, double default_max, 
	             const QString& title, 
	             QWidget* parent = 0, const char* name = 0 );  

  virtual ~GsTL_slider() {}

  void setMinValue( double val ) { min_value_ = val; }
  double minValue() const { return min_value_; }
  double maxValue() const { return max_value_; }
  double defaultMaxValue() const { return default_max_ ; }
  void setDefaultMaxValue( double val ) { default_max_ = val; }

  double value() const { return value_; }


 public slots:
  void setValue( double );
  void setMaxValue( double val );
  void reset();

 signals:
  void valueChanged( double value );
  void maxValueChanged( double val );
  
 protected slots: 
  void updateLineedit( int slider_val );
  void updateSliderValue();

 protected:
  double default_max_;
  double value_, min_value_, max_value_;

  QLabel* label_;
  QLineEdit* lineedit_;
  QSlider* slider_;
  QPushButton* reset_button_;

  static const int slider_max_ = 100;
  static const int slider_step_ = 10;

 protected:
  virtual void init( const QString& title );
  void set_slider_value( double val );
};

/*
class GsTL_slider : public QHBox {

  Q_OBJECT 

 public:
  GsTL_slider( int default_max, const QString& title, 
	       QWidget* parent = 0, const char* name = 0 );  
  GsTL_slider( int min_value, int max_value, int page_step,
	       int value, int default_max, 
	       const QString& title, 
	       QWidget* parent = 0, const char* name = 0 );  

  virtual ~GsTL_slider() {}

  void setMinValue( int val ) { slider_->setMinValue( val ); }
  int minValue() const { return slider_->minValue(); }
  int maxValue() const { return slider_->minValue(); }
  int defaultMaxValue() const { return default_max_; }
  void setDefaultMaxValue( int val ) { default_max_ = val; }
  void setPageStep( int val ) { slider_->setPageStep( val ); }
  int pageStep() const { return slider_->pageStep(); }

  int value() const { return slider_->value(); }


 public slots:
  void setValue( int );
  void setMaxValue( int val );
  void reset();

 signals:
  void valueChanged( int value );
  void maxValueChanged( int val );
  
 protected slots: 
  void setValue( const QString& );
  void setSliderValue();

 protected:
  int default_max_;

  QLabel* label_;
  QLineEdit* lineedit_;
  QSlider* slider_;
  QPushButton* reset_button_;

 protected:
  virtual void init( const QString& title );
};
*/

#endif
