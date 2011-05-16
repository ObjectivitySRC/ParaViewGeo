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

#ifndef __GSTLAPPLI_GEOSTAT_PSET_VARIOG_COMPUTER_H__
#define __GSTLAPPLI_GEOSTAT_PSET_VARIOG_COMPUTER_H__

#include <GsTLAppli/geostat/common.h>

#include <vector>
#include <map>
#include <string>


class Point_set;
class GsTLGridProperty;
class Discrete_function;
class Direction_3d;
class Progress_notifier;
class Correlation_measure;


class GEOSTAT_DECL Pset_variog_computer {
 public:
  Pset_variog_computer();
  Pset_variog_computer( Point_set *pset, 
                        GsTLGridProperty* head_prop = 0, 
                        GsTLGridProperty* tail_prop = 0 );
    
  bool standardize() const { return standardize_; }
  void standardize( bool f ) { standardize_ = f; }

  std::vector<int> compute_variogram_values( Discrete_function& experim_variog,
                                    				 const std::vector<double>& lag_tol,
  		                                       const Direction_3d& direction,
                                             Correlation_measure* correl_measure,
                                             Progress_notifier* progress = 0 );
  
 private:
  Point_set *pset_;
  GsTLGridProperty* head_prop_;    
  GsTLGridProperty* tail_prop_;    
  bool standardize_;
};




#endif
