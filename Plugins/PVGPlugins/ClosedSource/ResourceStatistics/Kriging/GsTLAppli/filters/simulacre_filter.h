/**********************************************************************
** Author: Nicolas Remy
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

#ifndef __GSTLAPPLI_FILTERS_SIMULACRE_FILTER_H__
#define __GSTLAPPLI_FILTERS_SIMULACRE_FILTER_H__

#include <GsTLAppli/filters/common.h>
#include <GsTLAppli/filters/filter.h>

class QDataStream;
class Geostat_grid;

/** The Simulacre_input_filter class is a filter that can read the default file
* format of GsTLAppli. The format is a binary format, with big endian byte 
* order. Following are a description of the file formats for the pointset and
* the cartesian grid objects. All file formats begin with magic number 
* 0xB211175D, a string indicating the type of object stored in the file, the 
* name of the object, and a version number (Q_INT32). The rest is specific 
* to the object stored:
* 
*  - point-set:
* a Q_UINT32 indicating the number of points in the object.
* a Q_UINT32 indicating the number of properties in the object
* strings containing the names of the properties
* the x,y,z coordinates of each point, as floats
* all the property values, one property at a time, in the order specified
* by the strings of names, as floats. For each property there are as many
* values as points in the point-set.
*
*  - cartesian grid:
* 3 Q_UINT32 indicating the number of cells in the x,y,z directions
* 3 floats for the dimensions of a single cell
* 3 floats for the origin of the grid
* a Q_UINT32 indicating the number of properties
* all the property values, one property at a time, in the order specified
* by the strings of names, as floats. For each property, there are nx*ny*nz 
* values (nx,ny,nz are the number of cells in the x,y,z directions).   
*/

class FILTERS_DECL Simulacre_input_filter : public Input_filter {
public:
  static Named_interface* create_new_interface( std::string& );

public: 
  Simulacre_input_filter();
  virtual ~Simulacre_input_filter(); 
  virtual std::string filter_name() const { return "s-gems" ; }  
  virtual std::string file_extensions() const { return "*.sgems"; }

  virtual bool can_handle( const std::string& filename );
  virtual Geostat_grid* read( const std::string& filename, 
                              std::string* errors = 0 );

protected:
  //TL modified
  virtual Geostat_grid* read_reduced_grid( QDataStream& , std::string* errors );

  virtual Geostat_grid* read_cartesian_grid( QDataStream& stream, 
                                             std::string* errors );
  virtual Geostat_grid* read_pointset( QDataStream& stream, 
                                       std::string* errors );
};



class FILTERS_DECL Simulacre_output_filter : public Output_filter {
public:
  static Named_interface* create_new_interface( std::string& );

public: 
  Simulacre_output_filter();
  virtual ~Simulacre_output_filter(); 
  virtual std::string filter_name() const { return "s-gems" ; }  
  virtual std::string file_extensions() const { return "*.sgems"; }

  virtual bool write( std::string outfile, const Geostat_grid* grid, 
                      std::string* errors = 0 ); 

protected:
  //TL modified
  virtual bool write_reduced_grid( QDataStream& , const Geostat_grid* );

  virtual bool write_cartesian_grid( QDataStream& stream, const Geostat_grid* grid );
  virtual bool write_pointset( QDataStream& stream, const Geostat_grid* grid );
};


#endif
