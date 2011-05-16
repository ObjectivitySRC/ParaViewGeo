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

#ifndef __GSTLAPPLI_GUI_RGRID_PARAMS_H__
#define __GSTLAPPLI_GUI_RGRID_PARAMS_H__

#include <GsTLAppli/gui/variogram2/rgrid_base.h>
#include <GsTLAppli/math/gstlvector.h>

#include <qstring.h>
#include <qtable.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qbutton.h>

#include <vector>
#include <string>

class GsTLTable;


class RGrid_params : public RGrid_base
{
    Q_OBJECT

public:

  RGrid_params(QWidget* parent = 0, const char* name = 0 );

  const QTable * vector_table();
  int num_lags();

  void all_parameters( std::vector<GsTLVector<double> > & d,
		       std::vector<std::string >& mod_type,
		       std::vector<std::pair<double, double> >& mod_param);

  bool scale_with_variance();
  
    
public slots:
  void load_parameters();
  void save_parameters();
  void set_directions_count( int );
  

signals:
  void parameterChanged();


private:
  QStringList methods_list_;
  GsTLTable* vector_table_;

};




#endif
