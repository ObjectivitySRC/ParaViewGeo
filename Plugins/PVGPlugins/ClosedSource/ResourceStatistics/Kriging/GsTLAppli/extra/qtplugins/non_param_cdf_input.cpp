#include <GsTLAppli/extra/qtplugins/non_param_cdf_input.h>
#include <GsTLAppli/extra/qtplugins/selectors.h>
#include <GsTLAppli/extra/qtplugins/filechooser.h>
#include <GsTLAppli/extra/qtplugins/variogram_input.h>


#include <qcombobox.h>
#include <qlineedit.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>


TailCdfInput::TailCdfInput(QWidget* parent, const char* name)
: QWidget(parent, name ) {

  QGridLayout* main_layout = new QGridLayout( this, 2, 5 );
  main_layout->setRowStretch( 0, 0 );
  main_layout->setRowStretch( 1, 0 );
  main_layout->setRowStretch( 2, 1 );

  QLabel* function_label = new QLabel( "Function", this, "func_label" );
  function_type = new QComboBox( FALSE, this, "box" );
  main_layout->addWidget( function_label, 0, 0 );
  main_layout->addWidget( function_type, 1, 0 );
  main_layout->setColSpacing( 1, 6 );

  min_max_label_ = new QLabel( this, "min_max_label" );
  extremeValue = new QLineEdit( this, "extreme_value" );
  main_layout->addWidget( min_max_label_, 0, 2 );
  main_layout->addWidget( extremeValue, 1, 2 );
  main_layout->setColSpacing( 3, 6 );

  omega_label = new QLabel( "omega", this, "omega_label" );
  omega = new QLineEdit( this, "omega" );
  main_layout->addWidget( omega_label, 0, 4 );
  main_layout->addWidget( omega, 1, 4 );


	QObject::connect( function_type, SIGNAL( activated( int ) ),
		    this, SLOT( setExtremeBoxEnabbled( int ) ) );
	QObject::connect( function_type, SIGNAL( activated( int ) ),
		    this, SLOT( setOmegaBoxEnabbled( int ) ) );
}

QString TailCdfInput::getFunctionType() const {
	return function_type->currentText();
}

float TailCdfInput::getExtremeValue() const {
	return extremeValue->text().toFloat();
}

float TailCdfInput::getOmega() const {
	return omega->text().toFloat();
}

void TailCdfInput::setFunctionType(QString text ) {
	function_type->setCurrentText( text );
}

void TailCdfInput::setExtremeValue( QString extremeStr ) {
	extremeValue->setText(extremeStr);
}

void TailCdfInput::setOmega( QString omegaStr ) {
	omega->setText(omegaStr);
}


void TailCdfInput::setExtremeBoxEnabbled(int index){
//	bool on = function_type->currentText() == "Power";
  bool on = function_type_index[index] == "Power";
	min_max_label_->setEnabled( on );
  extremeValue->setEnabled( on );
}

void TailCdfInput::setOmegaBoxEnabbled(int index){
	//bool on = function_type->currentText() != "Exponential";
  std::string function_type = function_type_index[index];
  bool on = (function_type != "Exponential") && (function_type != "No extrapolation");
	omega_label->setEnabled( on );
	omega->setEnabled( on );
}

LowerTailCdfInput::LowerTailCdfInput(QWidget* parent, const char* name)
: TailCdfInput(parent, name) {
	function_type->clear();
  function_type->insertItem( tr( "Power" ) );
  function_type->insertItem( tr( "Exponential" ) );
  function_type->insertItem( tr( "No extrapolation" ) );
  omega->setText( "3");

  function_type_index.push_back("Power");
  function_type_index.push_back("Exponential");
  function_type_index.push_back("No extrapolation");

	min_max_label_->setText( "Min" );
}

UpperTailCdfInput::UpperTailCdfInput(QWidget* parent, const char* name) 
: TailCdfInput(parent, name) {
	function_type->clear();
  function_type->insertItem( tr( "Power" ) );
  function_type->insertItem( tr( "Hyperbolic" ) );
  function_type->insertItem( tr( "No extrapolation" ) );
  omega->setText("0.333");

  function_type_index.push_back("Power");
  function_type_index.push_back("Hyperbolic");
  function_type_index.push_back("No extrapolation");

	min_max_label_->setText( "Max" );

}


//=====================================

NonParamCdfInput::NonParamCdfInput( QWidget* parent, const char* name) 
: QWidget(parent,name) {

  QVBoxLayout* main_layout = new QVBoxLayout( this );

  QButtonGroup* options_group = 
    new QButtonGroup( 2, Qt::Horizontal, "Reference distribution from:",
                      this, "options" );

  from_grid = 
    new QRadioButton( "From Object", options_group, "fromobject" );
  from_file = 
    new QRadioButton( "From File", options_group, "fromfile" );

  break_ties = 
    new QRadioButton( "Break ties", this, "break_ties" );

  Line_separator* file_sep = 
    new Line_separator( "File with ref. distribution", this, "filesep" );
	refFileChooser = new FileChooser( this, "refFilename" );

  Line_separator* prop_sep = 
    new Line_separator( "Property with ref. distribution", this, "propsep" );  
  refPropSelector = new PropertySelector( this, "refGrid" );
  refPropSelector->setMinimumHeight( 95 );

  Line_separator* lti_sep = 
    new Line_separator( "Lower Tail Extrapolation", this, "ltisep" );  
	LTI = new LowerTailCdfInput( this, "LTI_type" );

  Line_separator* uti_sep = 
    new Line_separator( "Upper Tail Extrapolation", this, "utisep" );  
	UTI = new UpperTailCdfInput( this, "UTI_type" );

  main_layout->addWidget( options_group );
  main_layout->addSpacing( 10 );
  main_layout->addWidget( break_ties );
  main_layout->addWidget( file_sep );
  main_layout->addWidget( refFileChooser );

  main_layout->addWidget( prop_sep );
  main_layout->addWidget( refPropSelector);
  main_layout->addSpacing( 8 );

  main_layout->addWidget( lti_sep );
  main_layout->addSpacing( 6 );
  main_layout->addWidget( LTI );
  main_layout->addSpacing( 10 );

  main_layout->addWidget( uti_sep );
  main_layout->addSpacing( 6 );
  main_layout->addWidget( UTI );

  main_layout->setStretchFactor( LTI, 0 );
  main_layout->setStretchFactor( UTI, 0 );

  QObject::connect( from_file, SIGNAL( toggled(bool) ),
                    file_sep, SLOT( setShown(bool) ) );
  QObject::connect( from_file, SIGNAL( toggled(bool) ),
                    refFileChooser, SLOT( setShown(bool) ) );
  QObject::connect( from_grid, SIGNAL( toggled(bool) ),
                    prop_sep, SLOT( setShown(bool) ) );
  QObject::connect( from_grid, SIGNAL( toggled(bool) ),
                    refPropSelector, SLOT( setShown(bool) ) );

  from_grid->toggle();
  file_sep->setShown( false );
  refFileChooser->setShown( false );
}

void NonParamCdfInput::setReferenceGrid(const QString& grid_name){
  refPropSelector->setSelectedGrid( grid_name );
}
void NonParamCdfInput::setReferenceProperty(const QString& prop_name){
  refPropSelector->setSelectedProperty( prop_name );
}

bool NonParamCdfInput::isRefOnFile() { return from_file->isChecked(); }
bool NonParamCdfInput::isRefOnGrid(){ return from_grid->isChecked(); }

QString NonParamCdfInput::getRefGridName() const{return refPropSelector->selectedGrid();}
QString NonParamCdfInput::getRefPropName() const{ return refPropSelector->selectedProperty();}
QString NonParamCdfInput::getRefFileName() const{return refFileChooser->fileName();}
void NonParamCdfInput::setRefGridName(QString gridName){
  refPropSelector->setSelectedGrid(gridName);
}
void NonParamCdfInput::setRefPropName(QString propName){
  refPropSelector->setSelectedProperty(propName);
}
void NonParamCdfInput::setRefFileName(QString fileName){
  refFileChooser->setFileName(fileName);
}
void NonParamCdfInput::setRefOnFile(bool on ){ 
  from_file->setChecked(on);
  from_grid->setChecked(!on);
}

void NonParamCdfInput::setRefOnGrid(bool on ){ 
  from_file->setChecked(!on);
  from_grid->setChecked(on);
}

bool NonParamCdfInput::isTieBreaking() { return break_ties->isChecked(); }

void  NonParamCdfInput::setTieBreaking(bool on ){
  break_ties->setChecked(on);
}


