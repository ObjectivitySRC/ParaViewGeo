/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "extra" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_MISC_QT_ACCESSORS_H__
#define __GSTLAPPLI_MISC_QT_ACCESSORS_H__

#include <GsTLAppli/extra/gui/common.h>
#include <GsTLAppli/gui/utils/qwidget_value_accessor.h>

#include <string>

class GridSelector;
class PropertySelector;
class SinglePropertySelector;
class MultiPropertySelector;
class OrderedPropertySelector;
class FileChooser;
class VariogramInput;
class KrigingTypeSelector;
class TrendComponents;
class EllipsoidInput;
class NonParamCdfInput;
class TailCdfInput;


class EXTRAGUI_DECL GridSelector_accessor : public QWidget_value_accessor {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  GridSelector_accessor( QWidget* widget = 0 );
  virtual ~GridSelector_accessor() {}

  virtual bool initialize( QWidget* widget = 0 );
  virtual bool is_valid() const { return selector_ != 0; } 
  virtual std::string value() const;
  virtual bool set_value( const std::string& str );
  virtual void clear() {}
    
 private:
  GridSelector* selector_;

};



class EXTRAGUI_DECL PropertySelector_accessor : public QWidget_value_accessor {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  PropertySelector_accessor( QWidget* widget = 0 );
  virtual ~PropertySelector_accessor() {}
  
  virtual bool initialize( QWidget* widget = 0 );
  virtual bool is_valid() const { return selector_ != 0; } 
  virtual std::string value() const;
  virtual bool set_value( const std::string& str );
  virtual void clear() {}
    
 private:
  PropertySelector* selector_;

};

class EXTRAGUI_DECL SinglePropertySelector_accessor : public QWidget_value_accessor {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  SinglePropertySelector_accessor( QWidget* widget = 0 );
  virtual ~SinglePropertySelector_accessor() {}
  
  virtual bool initialize( QWidget* widget = 0 );
  virtual bool is_valid() const { return selector_ != 0; } 
  virtual std::string value() const;
  virtual bool set_value( const std::string& str );
  virtual void clear() {}
    
 private:
  SinglePropertySelector* selector_;

};


class EXTRAGUI_DECL MultiPropertySelector_accessor : public QWidget_value_accessor {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  MultiPropertySelector_accessor( QWidget* widget = 0 );
  virtual ~MultiPropertySelector_accessor() {}
  
  virtual bool initialize( QWidget* widget = 0 );
  virtual bool is_valid() const { return selector_ != 0; } 
  virtual std::string value() const;
  virtual bool set_value( const std::string& str );
  virtual void clear() {}
    
 private:
  MultiPropertySelector* selector_;

};


class EXTRAGUI_DECL OrderedPropertySelector_accessor : public QWidget_value_accessor {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  OrderedPropertySelector_accessor( QWidget* widget = 0 );
  virtual ~OrderedPropertySelector_accessor() {}
  
  virtual bool initialize( QWidget* widget = 0 );
  virtual bool is_valid() const { return selector_ != 0; } 
  virtual std::string value() const;
  virtual bool set_value( const std::string& str );
  virtual void clear() {}
    
 private:
  OrderedPropertySelector* selector_;

};


class EXTRAGUI_DECL FileChooser_accessor : public QWidget_value_accessor {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  FileChooser_accessor( QWidget* widget = 0 );
  virtual ~FileChooser_accessor() {}

  virtual bool initialize( QWidget* widget = 0 );
  virtual bool is_valid() const { return file_chooser_ != 0; }
  virtual std::string value() const;
  virtual bool set_value( const std::string& str );
  virtual void clear();
  
 private:
  FileChooser* file_chooser_;

};



class EXTRAGUI_DECL VariogramInput_accessor : public QWidget_value_accessor {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  VariogramInput_accessor( QWidget* widget = 0 );
  virtual ~VariogramInput_accessor() {}

  virtual bool initialize( QWidget* widget = 0 );
  virtual bool is_valid() const { return varg_input_ != 0; }
  virtual std::string value() const;
  virtual bool set_value( const std::string& str );
  virtual void clear();
  
 private:
  VariogramInput* varg_input_;

};



class EXTRAGUI_DECL KrigingTypeSelector_accessor : public QWidget_value_accessor {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  KrigingTypeSelector_accessor( QWidget* widget = 0 );
  virtual ~KrigingTypeSelector_accessor() {}

  virtual bool initialize( QWidget* widget = 0 );
  virtual bool is_valid() const { return selector_ != 0; }
  virtual std::string value() const;
  virtual bool set_value( const std::string& str );
  virtual void clear();
  
 private:
  KrigingTypeSelector* selector_;

};


class EXTRAGUI_DECL TrendComponents_accessor : public QWidget_value_accessor {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  TrendComponents_accessor( QWidget* widget = 0 );
  virtual ~TrendComponents_accessor() {}

  virtual bool initialize( QWidget* widget = 0 );
  virtual bool is_valid() const { return selector_ != 0; }
  virtual std::string value() const;
  virtual bool set_value( const std::string& str );
  virtual void clear();
  
 private:
  TrendComponents* selector_;

};



class EXTRAGUI_DECL EllipsoidInput_accessor : public QWidget_value_accessor {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  EllipsoidInput_accessor( QWidget* widget = 0 );
  virtual ~EllipsoidInput_accessor() {}

  virtual bool initialize( QWidget* widget = 0 );
  virtual bool is_valid() const { return input_ != 0; }
  virtual std::string value() const;
  virtual bool set_value( const std::string& str );
  virtual void clear();
  
 private:
  EllipsoidInput* input_;

};


class EXTRAGUI_DECL nonParamCdfInput_accessor : public QWidget_value_accessor {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  nonParamCdfInput_accessor( QWidget* widget = 0 );
  virtual ~nonParamCdfInput_accessor() {}

  virtual bool initialize( QWidget* widget = 0 );
  virtual bool is_valid() const { return input_ != 0; }
  virtual std::string value() const;
  virtual bool set_value( const std::string& str );
  virtual void clear();
  
 private:
  NonParamCdfInput* input_;

};

class EXTRAGUI_DECL tailCdfInput_accessor : public QWidget_value_accessor {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  tailCdfInput_accessor( QWidget* widget = 0 );
  virtual ~tailCdfInput_accessor() {}

  virtual bool initialize( QWidget* widget = 0 );
  virtual bool is_valid() const { return input_ != 0; }
  virtual std::string value() const;
  virtual bool set_value( const std::string& str );
  virtual void clear();
  
 private:
  TailCdfInput* input_;
};


class EXTRAGUI_DECL lowerTailCdfInput_accessor : public tailCdfInput_accessor {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  lowerTailCdfInput_accessor( QWidget* widget = 0 );
  virtual ~lowerTailCdfInput_accessor() {}

//  virtual bool initialize( QWidget* widget = 0 );
//  virtual bool is_valid() const { return input_ != 0; }
//  virtual std::string value() const;
//  virtual bool set_value( const std::string& str );
//  virtual void clear();
  
// protected:
//  TailCdfInput* input_;
};


class EXTRAGUI_DECL upperTailCdfInput_accessor : public tailCdfInput_accessor {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  upperTailCdfInput_accessor( QWidget* widget = 0 );
  virtual ~upperTailCdfInput_accessor() {}
  
};
/*
class EXTRAGUI_DECL lowerTailCdfInput_accessor : public QWidget_value_accessor {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  lowerTailCdfInput_accessor( QWidget* widget = 0 );
  virtual ~lowerTailCdfInput_accessor() {}

  virtual bool initialize( QWidget* widget = 0 );
  virtual bool is_valid() const { return input_ != 0; }
  virtual std::string value() const;
  virtual bool set_value( const std::string& str );
  virtual void clear();
  
 private:
  TailCdfInput* input_;
};


class EXTRAGUI_DECL upperTailCdfInput_accessor : public QWidget_value_accessor {
 public:
  static Named_interface* create_new_interface(std::string&);

 public:
  upperTailCdfInput_accessor( QWidget* widget = 0 );
  virtual ~upperTailCdfInput_accessor() {}

  virtual bool initialize( QWidget* widget = 0 );
  virtual bool is_valid() const { return input_ != 0; }
  virtual std::string value() const;
  virtual bool set_value( const std::string& str );
  virtual void clear();
  
 private:
  TailCdfInput* input_;
};
*/

#endif 
