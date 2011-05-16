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

#ifndef __GSTLAPPLI_FILTERS_GSLIB_QT_DIALOGS_H__ 
#define __GSTLAPPLI_FILTERS_GSLIB_QT_DIALOGS_H__ 

#include <GsTLAppli/filters/common.h>
#include <GsTLAppli/filters/gslib/gslibimportwizard.h> // "qt-designer" generated file 
#include <GsTLAppli/filters/gslib/gslibgridimport.h> // "qt-designer" generated file 
#include <GsTLAppli/filters/gslib/gslib_pointset_import.h> // "qt-designer" generated file 
#include <GsTLAppli/filters/gslib/gslibmaskedimport.h> // "qt-designer" generated file 
 
#include <qtextedit.h> 
#include <qcombobox.h> 
#include <qstring.h> 
#include <qwidget.h> 
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qradiobutton.h>

#include <fstream> 
 
 
class Gslib_specialized_infilter; 
 
 
/** Gslib_infilter_dialog is the GUI part of the import filter. 
 * This wizard first determines which specialized filter to use (eg a grid 
 * import filter, or a pointset import filter), and then prompts for 
 * additional information, depending on the selected specialized filter. 
 */ 
class FILTERS_DECL Gslib_infilter_dialog : public GslibImportWizard { 
   
  Q_OBJECT 
 
 public: 
  Gslib_infilter_dialog( QWidget* parent , const char* name = 0 ); 
  virtual ~Gslib_infilter_dialog(); 
 
  void set_file( std::ifstream& infile ); 
  Gslib_specialized_infilter* filter() { return filter_; } 
 
 public slots: 
  void create_specialized_filter( const QString& ); 
 
 protected slots:
   virtual void accept();

 protected: 
  Gslib_specialized_infilter* filter_; 
  std::ifstream* file_stream_; 
}; 
 
//TL modified
class FILTERS_DECL Gslib_input_mgrid_dialog : public GslibMaskedImport
{
	Q_OBJECT
public:
    Gslib_input_mgrid_dialog( QWidget* parent , const char* name = 0 ); 
	bool isregular() { return _isregular->isChecked(); }
	/// Grid dimensions 
	int nx() const; 
	int ny() const; 
	int nz() const; 

	/// Cell dimensions 
	float x_size() const; 
	float y_size() const; 
	float z_size() const; 

	/// Coordinates of the origin 
	float Ox() const; 
	float Oy() const; 
	float Oz() const; 

	/// name of the grid 
	QString name() const; 

	int X_column() const; 
	int Y_column() const; 
	int Z_column() const; 


	int maskColumnNum() { return _maskcol->value(); }

	// no data value
	bool use_no_data_value() const;
	float no_data_value() const;
};

 
/** This class is the GUI part of the grid import filter. It prompts for 
 * information concerning the grid to import: 
 * what name should the imported grid have, what are its dimensions, etc.  
 */ 
class FILTERS_DECL Gslib_input_grid_dialog : public GslibGridImport { 
 
  Q_OBJECT 
 
 public: 
  Gslib_input_grid_dialog( QWidget* parent , const char* name = 0 ); 
 
  /// Grid dimensions 
  int nx() const; 
  int ny() const; 
  int nz() const; 
 
  /// Cell dimensions 
  float x_size() const; 
  float y_size() const; 
  float z_size() const; 
 
  /// Coordinates of the origin 
  float Ox() const; 
  float Oy() const; 
  float Oz() const; 
   
  /// name of the grid 
  QString name() const; 

  // no data value
  bool use_no_data_value() const;
  float no_data_value() const;
}; 
 
 
 
 
/** This class is the GUI part of the pointset import filter. It prompts for 
 * information concerning the point-set to import: 
 * what name the imported pointset should have, etc.  
 */ 
class FILTERS_DECL Gslib_input_pointset_dialog : public GslibPointsetImport { 
Q_OBJECT

 public: 
  Gslib_input_pointset_dialog( QWidget* parent , const char* name = 0 ); 
  Gslib_input_pointset_dialog( std::ifstream& infile, 
			       QWidget* parent , const char* name = 0 ); 
   
  QString name() const; 
  int X_column() const; 
  int Y_column() const; 
  int Z_column() const; 

  // no data value
  bool use_no_data_value() const;
  float no_data_value() const;

}; 
 
 
#endif 
