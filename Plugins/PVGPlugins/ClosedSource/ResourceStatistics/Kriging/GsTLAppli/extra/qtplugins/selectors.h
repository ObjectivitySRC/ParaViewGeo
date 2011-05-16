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

#ifndef __GSTLAPPLI_QTPLUGINS_SELECTORS_H__
#define __GSTLAPPLI_QTPLUGINS_SELECTORS_H__


#include <GsTLAppli/extra/qtplugins/common.h>
#include <GsTLAppli/appli/project.h>

#include <qcombobox.h>
#include <qlistbox.h>
#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qtable.h>
#include <qhbox.h>
#include <qdialog.h>

#include <vector>
#include <set>

class QString;
class QGroupBox;
class QHBoxLayout;
class QVBoxLayout;
class QLabel;
class QKeyEvent;

class QTPLUGINS_DECL GridSelector : public QComboBox, public Project_view {

  Q_OBJECT

 public:
  GridSelector( QWidget* parent = 0, const char* name = 0,
		GsTL_project* project = 0 );
  virtual ~GridSelector();
  virtual void setCurrentText( const QString& );

  virtual void init( GsTL_project* project );
  virtual void update( std::string obj );
  virtual void new_object( std::string obj );
  virtual void deleted_object( std::string obj );

 public:
  static const QString no_selection;
 signals:
  /** Tells when a new object is available, ie has been added to 
   * the project.
   */
  void new_object_available( const QString& obj );
};


/** Enables to choose a single property from a grid
 */ 
class QTPLUGINS_DECL SinglePropertySelector : public QComboBox {

  Q_OBJECT

 public:
  SinglePropertySelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~SinglePropertySelector() {}
  
  public slots:
    void show_properties( const QString& grid_name );

  protected:
    QString current_grid_;
};



/** A widget to choose multiple properties from a grid
 */ 
class QTPLUGINS_DECL MultiPropertySelector : public QListBox {

  Q_OBJECT

 public:
  MultiPropertySelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~MultiPropertySelector() {}
  
  public slots:
    void show_properties( const QString& grid_name );

  protected slots:
    void selection_size();

 signals:
  void selected_count( int );
};





class QTPLUGINS_DECL PropertySelector : public QWidget {

  Q_OBJECT

 public:
  PropertySelector( QWidget* parent = 0, const char* name = 0,
		    GsTL_project* project = 0 );
  virtual ~PropertySelector() {};

  virtual void init( GsTL_project* project ) { object_selector_->init( project ); }

  QString selectedGrid() const;
  void setSelectedGrid( const QString& name );
  QString selectedProperty() const;
  void setSelectedProperty( const QString& name );
  virtual QSize sizeHint() const { return QSize( 180, 40 ); }

// public slots:
//  void update_properties( const QString& obj );  

 protected slots:
   void forward_property_changed();
   
 signals:
  void property_changed( const QString& string );
  void object_changed( const QString& string );
  void new_object_available( const QString& obj );


 protected:
  QGroupBox* GroupBox4;
  GridSelector* object_selector_;
  QGroupBox* GroupBox5;
  SinglePropertySelector* property_selector_;
//  QComboBox* property_selector_;


 protected:
  QHBoxLayout* Selector_uiLayout;
  QVBoxLayout* Layout2;
  QHBoxLayout* GroupBox4Layout;
  QHBoxLayout* GroupBox5Layout;
};



/*
class QTPLUGINS_DECL Ordered_property_selector_dialog
    : public QDialog {

  Q_OBJECT

public:
  Ordered_property_selector_dialog( QWidget* parent = 0, const char* name = 0 );
  virtual ~Ordered_property_selector_dialog() {}
  
  QStringList selection() const;

public slots:
  void show_properties( const QString& grid_name );
  void set_current_selection( const QStringList& list );
 
protected slots:
  void move_selected_item_up();
  void move_selected_item_down();
  void left2right();
  void right2left();
  void remove_selected_item();

protected:
  MultiPropertySelector* property_selector_;
  QListBox* selected_properties_;
};
*/

#include <GsTLAppli/extra/qtplugins/order_properties_dialog.h>

class QTPLUGINS_DECL Ordered_property_selector_dialog
    : public Ordered_property_selector_dialog_base {

  Q_OBJECT

public:
  Ordered_property_selector_dialog( QWidget* parent = 0, const char* name = 0 );
  virtual ~Ordered_property_selector_dialog() {}
  
  QStringList selection() const;

public slots:
  void show_properties( const QString& grid_name );
  void set_current_selection( const QStringList& list );
 
protected slots:
  void move_selected_item_up();
  void move_selected_item_down();
  void left2right();
  void right2left();
  void remove_selected_item();

protected:
  MultiPropertySelector* property_selector_;
};


class QTPLUGINS_DECL GsTLGroupBox : public QGroupBox {

  Q_OBJECT
  Q_PROPERTY( QString keyword READ keyword WRITE set_keyword DESIGNABLE true STORED true )
  Q_PROPERTY( bool displayed READ is_displayed WRITE set_displayed DESIGNABLE true STORED true )
  Q_PROPERTY( bool activated READ is_activated WRITE set_activated DESIGNABLE true STORED true )

 public:
  GsTLGroupBox( QWidget* parent = 0, const char* name = 0,
		const QString& keyword = "Abracadabra",
		bool displayed = true, bool activated = true );
  GsTLGroupBox( int strips, Orientation orientation,
		QWidget* parent = 0, const char* name = 0,
		const QString& keyword = "Abracadabra",
		bool displayed = true, bool activated = true  );
  
  QString keyword() const;
  void set_keyword( const QString& keyword );

  bool is_displayed() const;
  void set_displayed( bool on );
  bool is_activated() const { return activated_; }
  void set_activated( bool on ) { activated_ = on; }

 public slots:
  virtual void setHidden( bool on );
  virtual void setHidden( const QString& keyword );
  virtual void setShown( bool on );
  virtual void setShown( const QString& keyword );

  virtual void setDeActivated( bool on );
  virtual void setDeActivated( const QString& keyword );
  virtual void setActivated( bool on );
  virtual void setActivated( const QString& keyword );

  virtual void setEnabled( bool on );
  virtual void setEnabled( const QString& keyword );
  virtual void setDisabled( bool on );
  virtual void setDisabled( const QString& keyword );

 protected:
  QString keyword_;
  std::set< QString > keywords_map_;
  bool displayed_;
  bool activated_;
};



class QTPLUGINS_DECL GsTLButtonGroup : public QButtonGroup {

  Q_OBJECT
  Q_PROPERTY( QString keyword READ keyword WRITE set_keyword DESIGNABLE true STORED true )
  Q_PROPERTY( bool displayed READ is_displayed WRITE set_displayed DESIGNABLE true STORED true )
  Q_PROPERTY( bool activated READ is_activated WRITE set_activated DESIGNABLE true STORED true )

 public:
  GsTLButtonGroup( QWidget* parent = 0, const char* name = 0,
		               const QString& keyword = "Abracadabra",
		               bool displayed = true, bool activated = true  );
  QString keyword() const;
  void set_keyword( const QString& keyword );

  bool is_displayed() const { return displayed_; }
  void set_displayed( bool on ) { displayed_ = on; }
  bool is_activated() const { return activated_; }
  void set_activated( bool on ) { activated_ = on; }

 public slots:
  virtual void setHidden( bool on );
  virtual void setHidden( const QString& keyword );
  virtual void setShown( bool on );
  virtual void setShown( const QString& keyword );
  
  virtual void setDeActivated( bool on );
  virtual void setDeActivated( const QString& keyword );
  virtual void setActivated( bool on );
  virtual void setActivated( const QString& keyword );

 protected:
  QString keyword_;
  bool displayed_;
  bool activated_;

};


class QTPLUGINS_DECL CloneGroupBox : public GsTLGroupBox {

  Q_OBJECT

 public:
  CloneGroupBox( QWidget* parent = 0, const char* name = 0 );
  int count() const { return count_; }
  void init_cloning();

  static QWidget* clone_widget( QWidget* widget, QWidget* parent = 0,
                                const char* name = 0 );

 public slots:
  void set_count( int n );

 protected:
  int count_;
  std::vector< QWidget* > cloned_widgets_;
  std::vector< QWidget* > labels_;
  QWidget* main_widget_;
  QLabel* main_label_;
  int adjust_;
};



class QTPLUGINS_DECL OrderedPropertySelector 
    : public GsTLGroupBox {

  Q_OBJECT

public:
  OrderedPropertySelector( QWidget* parent = 0, const char* name = 0 );
  virtual ~OrderedPropertySelector() {}
  QStringList selected_properties() const;
  void set_selected_properties( QStringList list );

public slots:
  void show_properties( const QString& grid_name );
  void show_selection_dialog();

signals:
  void selected_count( int );

protected:
  MultiPropertySelector* selected_properties_;
  QString grid_name_;  
};


#endif
