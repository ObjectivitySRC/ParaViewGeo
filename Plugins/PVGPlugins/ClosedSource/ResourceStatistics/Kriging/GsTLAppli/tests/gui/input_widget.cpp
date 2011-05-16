#include "input_widget.h"
#include <GsTLAppli/gui/utils/qwidget_value_collector.h>
#include <GsTLAppli/appli/manager_repository.h>

#include <qscrollview.h>
#include <qapplication.h>
#include <qvbox.h>
#include <qlabel.h>

#include <qwidgetfactory.h>

#include <iostream>
#include <string>

Input_widget::Input_widget( const QString& uifile, 
                            QWidget *parent, const char *name ) 
  : QWidget(parent, name) {

  QBoxLayout* layout_ = new QVBoxLayout(this);

  scrollview_ = new QScrollView( this, "view", WRepaintNoErase );
  
  dynamic_widget_ = QWidgetFactory::create( uifile, 0, 
                                            this,
					    "dynamic" );
  
  scrollview_->setMaximumHeight( dynamic_widget_->maximumHeight() );
  run_ = new QPushButton("Run", this, "runbutton" );
  scrollview_->addChild(dynamic_widget_);

  layout_->addWidget(scrollview_);
  layout_->addWidget(run_);
  layout_->activate();
  
  QObject::connect( run_, SIGNAL(clicked()), this, SLOT(output_content()) );
}

Input_widget::~Input_widget() {

}

void Input_widget::output_content() {
  QWidgets_values_collector_xml collector;
  std::string parameters = collector.widgets_values(dynamic_widget_, "");
  std::cout << parameters << std::endl;

  std::cout << std::endl << std::endl
	    << "List of managers " << std::endl;
  Root::instance()->list_all( std::cout );
}
