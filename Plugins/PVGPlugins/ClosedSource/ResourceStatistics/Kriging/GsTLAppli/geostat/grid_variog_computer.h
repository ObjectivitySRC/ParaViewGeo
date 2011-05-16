/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "geostat" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_GEOSTAT_GRID_VARIOG_COMPUTER_H__
#define __GSTLAPPLI_GEOSTAT_GRID_VARIOG_COMPUTER_H__

#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/math/gstlvector.h>

#include <vector>
#include <map>
#include <string>

class Discrete_function;
class Strati_grid;
class GsTLGridProperty;
class Progress_notifier;
class Correlation_measure;


class GEOSTAT_DECL Grid_variog_computer {
public:
  Grid_variog_computer();
  Grid_variog_computer( Strati_grid *grid, 
                        GsTLGridProperty* head_prop, 
                        GsTLGridProperty* tail_prop );
  ~Grid_variog_computer(){}    

  bool standardize() const { return standardize_; }
  void standardize( bool f ) { standardize_ = f; }

  std::vector<int> compute_variogram_values( Discrete_function &f,
                                             GsTLVector<double> direction,
                                             int lags_count,
                                             Correlation_measure* correl_measure,
                                             Progress_notifier* progress = 0 );

protected:
  double compute_covariance() const;

protected: 
  const Strati_grid *grid_;
  const GsTLGridProperty* head_prop_;    
  const GsTLGridProperty* tail_prop_;    
  bool standardize_;

};
	
#endif
