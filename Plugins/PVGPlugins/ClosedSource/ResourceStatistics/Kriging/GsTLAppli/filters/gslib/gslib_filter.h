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

#ifndef __GSTLAPPLI_GSLIB_FILTERS_H__ 
#define __GSTLAPPLI_GSLIB_FILTERS_H__ 
 
 
#include <GsTLAppli/filters/common.h>
#include <GsTLAppli/filters/filter.h> 
#include <GsTLAppli/filters/gslib/filter_qt_dialogs.h>
 
#include <vector>

class GsTLGridProperty;
class Gslib_infilter_dialog; 
class Gslib_input_grid_dialog; 
class Gslib_input_pointset_dialog; 
class QWidget; 
class Reduced_grid;
 

/** This class defines a general purpose filter for reading gslib files. 
 * There are two types of gslib files: point-set files or grid files, and 
 * parsing these 2 two types of files requires different kind of information. 
 * This class doesn't parse the gslib file by itself. 
 * Instead, it resorts on a specialized filter to read the file and create  
 * the corresponding geostat_grid. 
 * The only task of Gslib_filter is to determine what type of filter it should 
 * use to parse the gslib file. 
 */ 
class FILTERS_DECL Gslib_infilter : public Input_filter { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
   
 public: 
  Gslib_infilter(); 
  virtual ~Gslib_infilter(); 
 
  virtual std::string filter_name() const { return "gslib" ; }
  virtual std::string file_extensions() const { return "*.dat  *.out"; }

  virtual bool can_handle( const std::string& filename );
  virtual Geostat_grid* read( const std::string& filename, 
                              std::string* errors = 0 ); 
//  virtual Geostat_grid* read( std::ifstream& infile ); 
//  virtual Geostat_grid* read( std::istringstream& instr ); 
 
 protected: 
  Gslib_infilter_dialog* wizard_; 
 
 private: 
  Gslib_infilter( const Gslib_infilter& ); 
  Gslib_infilter& operator=( const Gslib_infilter& ); 
}; 
 
 
 
 
/** This class defines the common interface of all gslib filters capable 
 * or parsing a given type of file. 
 * The "file" can be either read from an actual file or a string 
 */ 
class FILTERS_DECL Gslib_specialized_infilter : public Input_filter { 
 public: 
  virtual ~Gslib_specialized_infilter() {}; 
 
  virtual QWidget* init_dialog( std::ifstream& infile ) = 0; 
  virtual std::string filter_name() const { return "gslib specialized" ; } 
  virtual Geostat_grid* read( const std::string& filename, 
                              std::string* errors = 0 ); 
  virtual Geostat_grid* read( std::ifstream& infile ) = 0; 

  virtual std::string object_filtered() { return ""; } 
 
  virtual bool has_valid_parameters() const = 0;
}; 
 
 
/** This class defines a filter capable of parsing a gslib grid file. 
 * The grid it will create from the file will be a cartesian grid.  
 */ 
class FILTERS_DECL Gslib_grid_infilter : public Gslib_specialized_infilter { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
   
 public: 
  Gslib_grid_infilter(); 
  virtual ~Gslib_grid_infilter(); 
   
  virtual QWidget* init_dialog( std::ifstream& infile ); 
  virtual std::string object_filtered() { return "cartesian grid" ; } 
  virtual Geostat_grid* read( std::ifstream& infile ); 
  virtual bool has_valid_parameters() const;


 protected: 
  Gslib_input_grid_dialog* dialog_; 

 private: 
  Gslib_grid_infilter( const Gslib_grid_infilter& ); 
  Gslib_grid_infilter& operator=( const Gslib_grid_infilter& ); 

  bool read_one_realization( std::ifstream& infile, 
                             const std::vector<GsTLGridProperty*>& props,
                             long int grid_size);
}; 
 
class FILTERS_DECL Gslib_mgrid_infilter : public Gslib_specialized_infilter { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
   
 public: 
  Gslib_mgrid_infilter(); 
  virtual ~Gslib_mgrid_infilter(); 
   
  virtual QWidget* init_dialog( std::ifstream& infile ); 
  virtual std::string object_filtered() { return "masked grid" ; } 
  virtual Geostat_grid* read( std::ifstream& infile ); 
  virtual bool has_valid_parameters() const;

 protected: 
  Gslib_input_mgrid_dialog* dialog_; 

 private: 
  Gslib_mgrid_infilter( const Gslib_mgrid_infilter& ); 
  Gslib_mgrid_infilter& operator=( const Gslib_mgrid_infilter& ); 

  Geostat_grid * readRegular(std::ifstream& infile,Reduced_grid *);
  Geostat_grid * readPointset(std::ifstream& infile,Reduced_grid *);
  bool read_one_realization( std::ifstream& infile, 
                             const std::vector<GsTLGridProperty*>& props,
                             Reduced_grid * grid);
};  
 
/** This class defines a filter capable of parsing a gslib point-set file. 
 */ 

class FILTERS_DECL Gslib_poinset_infilter : public Gslib_specialized_infilter { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
   
 public: 
  Gslib_poinset_infilter(); 
  virtual ~Gslib_poinset_infilter(); 
   
  virtual QWidget* init_dialog( std::ifstream& infile ); 
  virtual std::string object_filtered() { return "point set" ; } 
  virtual Geostat_grid* read( std::ifstream& infile ); 
 
  virtual bool has_valid_parameters() const;

 protected: 
  Gslib_input_pointset_dialog* dialog_; 

 private: 
  Gslib_poinset_infilter( const Gslib_poinset_infilter& ); 
  Gslib_poinset_infilter& operator=( const Gslib_poinset_infilter& ); 
}; 
 
 
 
/** This class defines a filter capable of writing a geostat_grid in gslib  
 * format. 
 */ 
class FILTERS_DECL Gslib_outfilter : public Output_filter { 
 public: 
  static Named_interface* create_new_interface( std::string& ); 
   
 public: 
  Gslib_outfilter(); 
  virtual ~Gslib_outfilter(); 
   
  virtual std::string filter_name() const { return "gslib" ; }  
  virtual bool write( std::string outfile, const Geostat_grid* grid, 
                      std::string* errors = 0 ); 

 protected:
  virtual bool write( std::ofstream& outfile, const Geostat_grid* grid ); 
  virtual bool writeReduced2Cartesian(std::ofstream& outfile, const Geostat_grid* grid);


   
}; 
 
 
#endif 
