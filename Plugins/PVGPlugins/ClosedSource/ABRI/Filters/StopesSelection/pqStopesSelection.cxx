#include "pqStopesSelection.h"
#include <pqFileDialog.h>
#include <QtDebug>
#include <QToolButton>

// VTK includes

// ParaView Server Manager includes
#include "vtkSMProperty.h"
#include "vtkSMSourceProxy.h"
#include "vtkPVDataSetAttributesInformation.h"
#include "vtkPVDataInformation.h"
#include "vtkPVArrayInformation.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMInputProperty.h"
#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"

// ParaView includes
#include "pqPipelineFilter.h"
#include "pqSMAdaptor.h"
#include "pqScalarBarRepresentation.h"


pqStopesSelection::pqStopesSelection(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/ABRIFilters/StopesSelection/pqStopesSelection.ui", pxy, p)
  {
		this->path= NULL;
		this->fileNameLine = this->findChild<QLineEdit*>("fileName");
		this->fileButton = this->findChild<QToolButton*>("browseButton");
		QObject::connect(this->fileButton, SIGNAL(released()), this, SLOT(onBrowse()));
		
		this->fileRadioButton = this->findChild<QRadioButton*>("fileButton");
		this->manualButton = this->findChild<QRadioButton*>("manualButton");
		QObject::connect(fileRadioButton, SIGNAL(clicked()), this, SLOT(onRadioButtonClicked()));
		QObject::connect(manualButton, SIGNAL(clicked()), this, SLOT(onRadioButtonClicked()));
		this->fileLabel = this->findChild<QLabel*>("configLabel");
		this->manualGroup = this->findChild<QGroupBox*>("manualGroup");
		this->densityInput=  this->findChild<QComboBox*>("densityInput");
		this->stopeWidth= this->findChild<QDoubleSpinBox*>("stopeWidth");
		this->stopeDepth= this->findChild<QDoubleSpinBox*>("stopeDepth");
		this->SaveAs = this->findChild<QToolButton*>("SaveAs");
		QObject::connect(this->SaveAs, SIGNAL(released()), this,
                 SLOT(SaveAsSlot()));
		this->Save = this->findChild<QToolButton*>("Save");
		QObject::connect(this->Save, SIGNAL(released()), this,
                 SLOT(SaveSlot()));

		//connect calculator widget tab
		this->Vectors = this->findChild<QPushButton*>("Vectors");
		this->Vectors->setMenu(&this->VectorsMenu);
	  QObject::connect(&this->VectorsMenu,
                 SIGNAL(triggered(QAction*)),
                 this,
                 SLOT(variableChosen(QAction*)));

    this->Scalars = this->findChild<QPushButton*>("Scalars");
		this->Scalars->setMenu(&this->ScalarsMenu);
    QObject::connect(&this->ScalarsMenu,
                   SIGNAL(triggered(QAction*)),
                   this,
                   SLOT(variableChosen(QAction*)));

		// clicking on any button or any part of the panel where another button
		// doesn't take focus will cause the line edit to have focus 
		this->Function = this->findChild<QLineEdit*>("Function");
		this->setFocusProxy(this->Function);

		this->xy = this->findChild<QToolButton*>("xy");		
		// connect all buttons for which the text of the button 
		// is the same as what goes into the function
		QRegExp regexp("^([ijk]Hat|n[0-9]|ln|log10|sin|cos|"
									 "tan|asin|acos|atan|sinh|cosh|tanh|"
									 "sqrt|exp|ceil|floor|abs|norm|mag|"
									 "LeftParentheses|RightParentheses|"
									 "Divide|Multiply|Minus|Plus|Dot)$");
  
		QList<QToolButton*> buttons;
		buttons = this->findChildren<QToolButton*>(regexp);
		foreach(QToolButton* tb, buttons)
			{
			QSignalMapper* mapper = new QSignalMapper(tb);
			QObject::connect(tb,
											 SIGNAL(pressed()),
											 mapper,
											 SLOT(map()));
			mapper->setMapping(tb, tb->text());
			QObject::connect(mapper,
											 SIGNAL(mapped(const QString&)),
											 this,
											 SLOT(buttonPressed(const QString&)));
			}
    
		QToolButton* tb = this->xy;
		QSignalMapper* mapper = new QSignalMapper(tb);
		QObject::connect(tb,
										 SIGNAL(pressed()),
										 mapper,
										 SLOT(map()));
		mapper->setMapping(tb, "^");
		QObject::connect(mapper,
										 SIGNAL(mapped(const QString&)),
										 this,
										 SLOT(buttonPressed(const QString&)));

	  this->v1v2 = this->findChild<QToolButton*>("v1v2");		
		tb = this->v1v2;
		mapper = new QSignalMapper(tb);
		QObject::connect(tb,
										 SIGNAL(pressed()),
										 mapper,
										 SLOT(map()));
		mapper->setMapping(tb, ".");
		QObject::connect(mapper,
										 SIGNAL(mapped(const QString&)),
										 this,
										 SLOT(buttonPressed(const QString&)));

		this->Clear = this->findChild<QToolButton*>("Clear");		
		QObject::connect(this->Clear,
										 SIGNAL(pressed()),
										 this->Function,
										 SLOT(clear()));

		// mark panel modified if the following are changed 
		QObject::connect(this->Function,
										 SIGNAL(textEdited(const QString&)),
										 this,
										 SLOT(setModified()));

		this->FitnessArrayName = this->findChild<QLineEdit*>("FitnessArrayName");		
		QObject::connect(this->FitnessArrayName,
										 SIGNAL(textEdited(const QString&)),
										 this,
										 SLOT(setModified()));

		this->ReplaceInvalidResult = this->findChild<QCheckBox*>("ReplaceInvalidResult");	
		QObject::connect(this->ReplaceInvalidResult,
										 SIGNAL(stateChanged(int)),
										 this,
										 SLOT(setModified()));

		this->ReplacementValue = this->findChild<QLineEdit*>("ReplacementValue");	
		QObject::connect(this->ReplacementValue,
										 SIGNAL(textChanged(const QString&)),
										 this,
										 SLOT(setModified()));

		this->XINC= this->findChild<QComboBox*>("XINC");
		this->YINC= this->findChild<QComboBox*>("YINC");
		this->ZINC= this->findChild<QComboBox*>("ZINC");

		QObject::connect(this->XINC, SIGNAL(currentIndexChanged(int)), this,
                 SLOT(XINC_Click(int)));


		this->updateVariables();
		this->reset();
		
		//search the index of the array in the XINC combox and use that to set the others
		if(this->XINC->count()>1)
		{
			this->YINC->setCurrentIndex(this->XINC->currentIndex()+1);
			this->ZINC->setCurrentIndex(this->XINC->currentIndex()+2);
		}
		this->linkServerManagerProperties();
}

pqStopesSelection::~pqStopesSelection()
{
	if(this->path!=NULL)
		delete this->path;
}

void pqStopesSelection::accept()
{
	pqLoadedFormObjectPanel::accept();

	 if(!this->proxy())
    {
    return;
    }

	 // put in new variables
  QList<QAction*> scalarActions = this->ScalarsMenu.actions();
  QList<QAction*> vectorActions = this->VectorsMenu.actions();
 
}

void pqStopesSelection::onBrowse()
{
	currentFile = QFileDialog::getOpenFileName(this,"Select configuration file",currentFile, QString("(*.xml)"));
	if(currentFile.isEmpty())
		return;
	this->fileNameLine->setText(currentFile);
	
	//convert QString to char* to have the path of the file 
	char* text= new char[currentFile.length()+1];
	int j;
	for(j=0 ; j<currentFile.length() ; j++)
		text[j] = currentFile[j].toAscii();
	text[j] = '\0';
	this->path= new char[currentFile.length()+1];
	strcpy(this->path, text);
	this->XML_browseButtonClicked(text); //update all the Gui parameters
	//delete text;	
}

void pqStopesSelection::XML_browseButtonClicked(const char* name)
{
	int index=0;
	vtkXMLDataElement *configElement = vtkXMLUtilities::ReadElementFromFile(name);
	vtkXMLDataElement *element;
		
	//---------------------------
	element = configElement->LookupElementWithName("Density");
	if(element != NULL)
	{
		const char* value = element->GetAttributeValue(0);
		QString Den= QString::fromAscii(value); 
		index=0;
		for(int i=0; i< this->densityInput->count(); i++)
			if(this->densityInput->itemText(i).compare(Den)==0)
				index=i;
		this->densityInput->setCurrentIndex(index);
	}
	else	qDebug() <<"cannot find densityArray property...Aborting";
		
	
	//---------------------------
	element = configElement->LookupElementWithName("stopeDimension");
	if(element != NULL)
	{
	  vtkXMLDataElement*	element2 = element->LookupElementWithName("stopeWidth");
		if(element2 != NULL)
		{
			double value = 0.0;			
			int result = element2->GetScalarAttribute("value",value);
			if(result ==	1)
				this->stopeWidth->setValue(value);
			else	qDebug() <<"Error while reading the stopeWidth. Make sure the value is valid";		
		}		
		else	qDebug() <<"cannot find stopeWidth property...Aborting";
		
	  element2 = element->LookupElementWithName("stopeDepth");
		if(element2 != NULL)
		{
			double value = 0.0;
			int result = element2->GetScalarAttribute("value",value);
			if(result == 1)
				this->stopeDepth->setValue(value);
			else	qDebug() <<"Error while reading the stopeDepth. Make sure the value is valid";			
		}		
		else	qDebug() <<"cannot find stopeDepth property...Aborting";	

	}
	else	qDebug() <<"cannot find stopeDimension property...Aborting";

	//---------------------------

	element = configElement->LookupElementWithName("FitnessEquation");

	if(element != NULL)
	{
		const char* value = element->GetAttributeValue(0);
		QString Equation= QString::fromAscii(value);
		this->Function->setText(Equation);	
	}
	else	qDebug() <<"cannot find FitnessEquation property...Aborting";
}
void pqStopesSelection::onRadioButtonClicked()
{
	if(this->fileRadioButton->isChecked())
	{
		this->fileLabel->setEnabled(true);
		this->fileNameLine->setEnabled(true);
		this->fileButton->setEnabled(true);
		this->manualGroup->setDisabled(true);
	}
	else
	{
		this->fileLabel->setDisabled(true);
		this->fileNameLine->setDisabled(true);
		this->fileButton->setDisabled(true);
		this->manualGroup->setEnabled(true);
	}
}

void pqStopesSelection::reset()
{
  // reset widgets controlled by the parent class
	pqLoadedFormObjectPanel::reset();
}

void pqStopesSelection::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
	pqLoadedFormObjectPanel::linkServerManagerProperties();
}

void pqStopesSelection::buttonPressed(const QString& buttonText)
{
  this->Function->insert(buttonText);
}

void pqStopesSelection::updateVariables()
{
  this->VectorsMenu.clear();
  this->ScalarsMenu.clear();

  this->VectorsMenu.addAction("coords");
  this->ScalarsMenu.addAction("coordsX");
  this->ScalarsMenu.addAction("coordsY");
  this->ScalarsMenu.addAction("coordsZ");    

  vtkPVDataSetAttributesInformation* fdi = NULL;
  pqPipelineFilter* f = qobject_cast<pqPipelineFilter*>(this->referenceProxy());
  if(!f)
    {
    return;
    }
		vtkSMInputProperty* ivp = vtkSMInputProperty::SafeDownCast(
		this->proxy()->GetProperty("Input"));
		int properPort = ivp->GetOutputPortForConnection( 0 );
 
		fdi = vtkSMSourceProxy::SafeDownCast(f->getInput(0)->getProxy())->
      GetDataInformation(properPort)->GetPointDataInformation();
    
  if(!fdi)
    {
    return;
    }

  for(int i=0; i<fdi->GetNumberOfArrays(); i++)
    {
    vtkPVArrayInformation* arrayInfo = fdi->GetArrayInformation(i);
    if (arrayInfo->GetDataType() == VTK_STRING
        || arrayInfo->GetDataType() == VTK_VARIANT )
      {
      continue;
      }

    int numComponents = arrayInfo->GetNumberOfComponents();
    QString name = arrayInfo->GetName();

    for(int j=0; j<numComponents; j++)
      {
      if(numComponents == 1)
        {
        this->ScalarsMenu.addAction(name);
        }
      else
        {
        QString n = name + QString("_%1").arg(
          pqScalarBarRepresentation::getDefaultComponentLabel(j, numComponents));
        QStringList d;
        d.append(name);
        d.append(QString("%1").arg(j));
        QAction* a = new QAction(n, &this->ScalarsMenu);
        a->setData(d);
        this->ScalarsMenu.addAction(a);
        }
      }

    if(numComponents == 3)
      {
      this->VectorsMenu.addAction(name);
      }
    }
}

void pqStopesSelection::variableChosen(QAction* a)
{
  if(a)
    {
    QString text = a->text();
    this->Function->insert(text);
    }
}

void pqStopesSelection::disableResults(bool e)
{
  this->FitnessArrayName->setEnabled(!e);
}

char* convertQStringToChar(QString current)
{
	char* text= new char[current.length()+1];
	int j;
	for(j=0 ; j<current.length() ; j++)
		text[j] = current[j].toAscii();
	text[j] = '\0';
	return text;
}

void pqStopesSelection::SaveAsSlot()
{
	currentFile = QFileDialog::getSaveFileName(this,"Save configuration file As",
																							currentFile, QString("(*.xml)"));
	if(currentFile.isEmpty())
		return;
	
	//convert QString to char* to have the path of the file 
	char* text= new char[currentFile.length()+1];
	int j;
	for(j=0 ; j<currentFile.length() ; j++)
		text[j] = currentFile[j].toAscii();
	text[j] = '\0';

	//-----------Write a new XML file if is not existed or replace just the changes----------
	this->path=  new char[currentFile.length()+1];
	strcpy(this->path, text);
	this->writeFile(this->path);	
}

void pqStopesSelection::SaveSlot()
{
	if(this->path!=NULL)
		this->writeFile(this->path);	
	else 	this->SaveAsSlot();
}

void pqStopesSelection::writeFile(const char* name)
{
	ofstream myFile;
	myFile.open(name);
	if(!myFile)
		qDebug() <<"Error writing to the save configuration file";
	else{
		myFile<<"<?xml version="<<'"'<<"1.0"<<'"'<<"?>\n";
		myFile<< "<StopeLocatorParameter>"<<endl;	

		myFile<<"\t"<<"<Density value="<<'"'<<convertQStringToChar(this->densityInput->currentText())<<'"'
			<<" description="<<'"'<<"specifies the Density array. The name has to match an input array"<<'"'<<"/>"<<endl;
	
		myFile<<"\t"<<"<stopeDimension>"<<endl;
		myFile<<"\t"<<"\t"<<"<stopeWidth value="<<'"'<<this->stopeWidth->value()<<'"'<<"/>"<<endl;
		myFile<<"\t"<<"\t"<<"<stopeDepth value="<<'"'<<this->stopeDepth->value()<<'"'<<"/>"<<endl;
		myFile<<"\t"<<"</stopeDimension>"<<endl;

		myFile<<"\t"<<"<FitnessEquation equation="<<'"'<<convertQStringToChar(this->Function->text())<<'"'<<"/>"<<endl;

		myFile<< "</StopeLocatorParameter>"<<endl;	

  	myFile.close();

	}
}

void pqStopesSelection::XINC_Click(int index)
{
	if(this->XINC->count()>1)
		{
			this->YINC->setCurrentIndex(index+1);
			this->ZINC->setCurrentIndex(index+2);
		}
}