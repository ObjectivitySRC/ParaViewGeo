/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "grid" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_GRIDMODEL_GRID_INITIALIZER_H__ 
#define __GSTLAPPLI_GRIDMODEL_GRID_INITIALIZER_H__ 
 
#include <GsTLAppli/grid/common.h>
#include <GsTLAppli/utils/gstl_types.h> 
 
#include <string> 
#include <vector> 
#include <utility> 
 
class Cartesian_grid; 
class Geostat_grid; 
class GsTLGridProperty; 
 

//class GRID_DECL Grid_initializer { 
// public: 

  /** Create a Grid_initializer that will assign data to grid \c target.
   * if from_scratch is false, the initializer checks if the last 
   * call to assign was made with the same source and source property 
   * arguments. If that is the case, it re-uses what it did before. Be careful:
   * Grid_initializer does not check if the source or target grids changed. 
   * For example, if the multigrid level (in case of a Strati_grid) has been
   * changed on \c target, and from_scratch is false, Grid_initializer will redo
   * what it had done before, ignoring the change made on \c target. It is safer
   * to let \c from_scratch equal to true, unless you know what you are doing.
   * That assumes that the source property did not change in the meantime.  
   * @param if overwrite is true, if a datum from source should go to a cell 
   * which already contains a datum (before function "assign" was called), that 
   * datum is discarded. 
   * @param if mark_as_hard is true, each assigned value is flagged as being  
   * a hard datum.  
  */
/*
  Grid_initializer( Geostat_grid* target = 0, 
                    bool from_scratch = true, bool overwrite = false,
                    bool mark_as_hard = true ); 

  bool starts_from_scratch() const { return from_scratch_; }
  void starts_from_scratch( bool on ) { from_scratch_ = on; }

  bool does_overwrite() const { return overwrite_; }
  void does_overwrite( bool on ) { overwrite_ = on; }
  
  bool marks_as_hard() const { return mark_as_hard_; }
  void marks_as_hard( bool on ) { mark_as_hard_ = on; }

  Geostat_grid* target_grid() const { return target_; }
  void target_grid( Geostat_grid* target );
*/
  /** Copy property \c source_property of \c source to \c target_property. 
   */ 
/*
  bool assign( GsTLGridProperty* target_property, 
	             const Geostat_grid* source, 
	             const std::string& source_property ); 
 */

  /** Undo the last assignement made on "target" 
   */ 
 // bool undo_assign(); 
 /*

 private: 
  //  Geostat_grid* target_; 
  Cartesian_grid* target_; 
  const Geostat_grid* source_; 
  std::string source_property_name_; 
 
  std::vector< std::pair<GsTLInt,GsTLInt> > last_assignements_; 

  bool from_scratch_;
  bool overwrite_;
  bool mark_as_hard_;
}; 
 */
#endif 
