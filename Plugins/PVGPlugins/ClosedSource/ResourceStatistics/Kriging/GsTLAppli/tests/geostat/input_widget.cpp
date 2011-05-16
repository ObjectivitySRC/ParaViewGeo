#include "input_widget.h"
#include <GsTLAppli/gui/utils/qwidget_value_collector.h>
#include <GsTLAppli/geostat/snesim.h>

#include <qscrollview.h>
#include <qapplication.h>

#include <qwidgetfactory.h>

#include <iostream>
#include <string>

Input_widget::Input_widget( const QString& uifile, 
                            QWidget *parent, const char *name ) 
  : QWidget(parent, name) {

  QBoxLayout* layout_ = new QVBoxLayout(this);

  scrollview_ = new QScrollView( this, "view", WRepaintNoErase );
  //scrollview_->setMinimumWidth(210);
  //scrollview_->setMinimumHeight(400);  

  dynamic_widget_ = QWidgetFactory::create( uifile, 0, 
                                            this,
					    "dynamic" );

  //dynamic_widget_->show();

  run_ = new QPushButton("Run", this, "runbutton" );
  scrollview_->addChild(dynamic_widget_);

  layout_->addWidget(scrollview_);
  layout_->addWidget(run_);
  layout_->activate();
  
  QObject::connect( run_, SIGNAL(clicked()), this, SLOT(run_algo()) );
}

Input_widget::~Input_widget() {
  delete scrollview_;
  delete run_;
}

void Input_widget::run_algo() {
  std::string parameters;
  QWidget_value_collector::widgets_value(dynamic_widget_, parameters);
  parameters = "<param>" + parameters + "</param>";
  
  std::cout << "generated XML file: " << endl;
  std::cout << parameters << std::endl << std::endl;
  
  std::cout << endl 
	    <<"===================================" << endl
	    << "   starting snesim" << endl;

  snesim_run( parameters );
  cout << "Over" << endl;
}


void Input_widget::snesim_run( std::string& parameters ) {

  Snesim_generic algo;
  algo.initialize( parameters, 0 );

  algo.execute();

}
