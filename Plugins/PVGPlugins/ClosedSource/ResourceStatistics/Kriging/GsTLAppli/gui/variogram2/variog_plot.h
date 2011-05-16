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

#ifndef __GSTLAPPLI_VARIOG_PLOT_H__
#define __GSTLAPPLI_VARIOG_PLOT_H__

#include <GsTLAppli/math/gstlvector.h>
#include <GsTLAppli/math/gstlpoint.h>
#include <GsTLAppli/extra/qwt/qwt_plot.h>

#include <GsTL/geometry/covariance.h>

#include <qapplication.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qevent.h>
#include <vector>



const int margin =40;

class Model_manager;
class Discrete_function;


class Variog_plot : public QwtPlot {
    
 Q_OBJECT
     
public:
  Variog_plot(){}

  Variog_plot( const Discrete_function& df,
               const Variogram_function_adaptor<Covariance<GsTLPoint> > *model,
               GsTLVector<double> angle,
               const std::vector<int>& pairs_count,
               const QString& title = QString::null,
               bool y_starts_at_zero = false,
               QWidget *parent=0, const char *name=0);
 
public slots:
  void refresh();
  void show_pairs_count( const QMouseEvent& );
    
private:
  std::vector<int> pairs_;
  std::vector<double> pairs_coord_x_, pairs_coord_y_;
  bool pairs_shown_;

  GsTLVector<double> angle_;
  const Variogram_function_adaptor<Covariance<GsTLPoint> > *model1_;
  const Variogram_function_adaptor<Covariance<GsTLPoint> > *model2_;

  long curve1_,curve2_;
 
  double max_x_;
};
	
    
#endif
