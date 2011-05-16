/**********************************************************************
** Author: Ting Li
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "filters" module of the Geostatistical Earth
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

/* -*- c++ -*- */

#ifndef __GSTLAPPLI_ECLIPSE_OUT_FILTERS_H__ 
#define __GSTLAPPLI_ECLIPSE_OUT_FILTERS_H__ 
 
 
#include <GsTLAppli/filters/common.h>
#include <GsTLAppli/filters/filter.h> 
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/filters/eclipse/EclOutDlg.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>

#include <vector>

class GsTLGridProperty;
class QWidget; 
 
typedef GsTLGridProperty::property_type Type ;

/** This class defines a filter capable of writing a geostat_grid in Eclipse  
 * format. 
 */ 
class FILTERS_DECL Ecl_outfilter : public Output_filter { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 

 public: 
  Ecl_outfilter(); 
  virtual ~Ecl_outfilter(); 

  std::string filter_name() const { return "eclipse" ; }

  // file extension = "*.*" by default
  
  virtual bool write( std::string , const Geostat_grid* ,
                      std::string* errors = 0 ); 


 protected:
	 virtual bool write( std::ofstream& , const Geostat_grid*, 
		 std::map<std::string,std::string> &);
  void transform(const GsTLGridProperty*, int, int, int, Type *) ;
  void transformReduced(const GsTLGridProperty*, int, int, int, Type *, const Reduced_grid *) ;
  
  EclOutDlg * _dlg;
}; 
 
 
#endif 
