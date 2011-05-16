#include "pqLineNetworkReader.h"
#include <QtDebug>
#include "vtkSMProperty.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMProxy.h"
#include "vtkSMSourceProxy.h"
#include "vtkEventQtSlotConnect.h"

#include <QTreeWidget>
#include <QComboBox>

#include <QTreeWidgetItem>
#include <QFlags>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QtDebug>

#include "pqPropertyManager.h"
#include "pqProxy.h"
#include <string>




//-------------------------------------------------------------------------------------------------------
pqLineNetworkReader::pqLineNetworkReader(pqProxy* pxy, QWidget* p) :
pqLoadedFormObjectPanel(":/XuzhouGUI/LineNetworkReader/pqLineNetworkReader.ui", pxy, p)
  {

	this->VTKConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
	stv = vtkSMStringVectorProperty::SafeDownCast(
			this->proxy()->GetProperty("Arrays"));


	this->tree = this->findChild<QTreeWidget*>("PropertiesTree");
	this->ArrowProp = this->findChild<QComboBox*>("ArrowProp");

	this->x1 = this->findChild<QComboBox*>("x1");
	this->y1 = this->findChild<QComboBox*>("y1");
	this->z1 = this->findChild<QComboBox*>("z1");
	this->x2 = this->findChild<QComboBox*>("x2");
	this->y2 = this->findChild<QComboBox*>("y2");
	this->z2 = this->findChild<QComboBox*>("z2");

	this->readPropertiesFromFile();

	vtkSMStringVectorProperty* stvp = vtkSMStringVectorProperty::SafeDownCast(
			this->proxy()->GetProperty("x1"));

	if( strlen(stvp->GetElement(0)) )
	{
		this->restoreGuiState();
	}

	this->VTKConnect->Connect(stv, vtkCommand::ModifiedEvent, 
	this, SLOT(onPropertiesModified()));

	this->linkServerManagerProperties();
	}


	//-------------------------------------------------------------------------------------------------------
pqLineNetworkReader::~pqLineNetworkReader()
{
}


//-------------------------------------------------------------------------------------------------------
void pqLineNetworkReader::accept()
{
	// disconnecting the signal before updating the property, to be sure
	// that the SLOT will be called only on internal modification of
	// the property. for example modification caused by loading state 
	// or using SlideShow
	this->VTKConnect->Disconnect();

	stv->SetElements(this->numberOfArrays, this->elements);
	//stv->SetImmediateUpdate(1);

	this->VTKConnect->Connect(stv, vtkCommand::ModifiedEvent, 
		this, SLOT(onPropertiesModified()));

	pqLoadedFormObjectPanel::accept();
}


//-------------------------------------------------------------------------------------------------------
void pqLineNetworkReader::reset()
{
  // reset widgets controlled by the parent class
	pqLoadedFormObjectPanel::reset();
}


//-------------------------------------------------------------------------------------------------------
void pqLineNetworkReader::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
	pqLoadedFormObjectPanel::linkServerManagerProperties();

}



//-------------------------------------------------------------------------------------------------------
void pqLineNetworkReader::readPropertiesFromFile()
{
	vtkSMStringVectorProperty* stvp = vtkSMStringVectorProperty::SafeDownCast(
			this->proxy()->GetProperty("FileName"));

	QString fileName = stvp->GetElement(0);

	QFile file(fileName);
	QString line;
	QStringList lineSplit;

	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Failed to read the file";
		return;
	}

	QTextStream t( &file );
	line = t.readLine();
	lineSplit = line.split(",");

	QFlags<Qt::ItemFlag> flg=Qt::ItemIsUserCheckable |
													Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	this->ArrowProp->addItem("none");

	bool loadStv = true;
	if(this->stv->GetNumberOfElements())
	{
		loadStv = false;
	}
	int counter = 0;
	this->numberOfArrays = lineSplit.size()*2;
	this->elements = new const char*[ this->numberOfArrays ];
	char* elm;
	foreach(QString prop, lineSplit)
	{

		elm = new char[prop.length()+1];
		int i;
		for(i=0; i<prop.length(); i++)
		{
			elm[i] = prop[i].toAscii();
		}
		elm[i] = NULL;
		elements[counter++] = elm;
		if(loadStv)
		{
			elements[counter++] = "0";
		}
		else
		{
			elements[counter++] = this->stv->GetElement(counter);
		}
		
		QTreeWidgetItem *item = new QTreeWidgetItem(QStringList(prop));
		this->tree->addTopLevelItem(item);
		item->setFlags(flg);
		item->setCheckState(0, Qt::Unchecked);

		if(    prop[0].isDigit()   ||   ( atof(prop.toAscii().data()) && (prop[0] == '-') )    )
		{
			this->x1->addItem(prop);
			this->y1->addItem(prop);
			this->z1->addItem(prop);
			this->x2->addItem(prop);
			this->y2->addItem(prop);
			this->z2->addItem(prop);
			this->ArrowProp->addItem(prop);
		}
	}

	QStringList lineSplit2;
	QString prop2;
	line = t.readLine();
	lineSplit2 = line.split(",");
	int i=0;

	foreach(QString prop, lineSplit2)
	{
		if( this->isNumeric(prop.toStdString()) && i < lineSplit.size())
		{
			prop2 = lineSplit[i];
			this->x1->addItem(prop2);
			this->y1->addItem(prop2);
			this->z1->addItem(prop2);
			this->x2->addItem(prop2);
			this->y2->addItem(prop2);
			this->z2->addItem(prop2);
			this->ArrowProp->addItem(prop2);
		}
		i++;
	}

	/*
	if ( lineSplit2.size() != lineSplit.size() )
		{
		qDebug() << "Your file has a mismatch between number of headers, and values per line";
		}
	*/
	this->updateCoordComboBoxes(0);

	QObject::connect(this->tree, SIGNAL(itemChanged ( QTreeWidgetItem*, int)), this,
		SLOT(onTreeSelectionChanged( QTreeWidgetItem*, int)));

	QObject::connect(this->x1, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCoordComboBoxes(int)));

	file.close();
}



//-------------------------------------------------------------------------------------------------------
void pqLineNetworkReader::updateCoordComboBoxes(int index)
{
	this->y1->setCurrentIndex(index+1);
	this->z1->setCurrentIndex(index+2);
	this->x2->setCurrentIndex(index+3);
	this->y2->setCurrentIndex(index+4);
	this->z2->setCurrentIndex(index+5);
}




//-------------------------------------------------------------------------------------------------------
void pqLineNetworkReader::onTreeSelectionChanged(QTreeWidgetItem* item, int column)
{
	column = this->tree->indexOfTopLevelItem(item);
	if(item->checkState(0) == Qt::Checked)
	{
		this->elements[(column*2)+1] = "1";
	}
	else
	{
		this->elements[(column*2)+1] = "0";
	}
	this->referenceProxy()->setModifiedState(pqProxy::MODIFIED);
}


//----------------------------------------------------------------------------
bool pqLineNetworkReader::isNumeric(std::string value)
{
	if(value.length() == 0)
	{
		qDebug() << "the first line is incomplete, the first line," <<endl<<
			"this line is used to determine if the column is numerical or not." <<
			"the column will be considered as text.";
		return false;
	}
	if( (value.at(0)!='-') &! isdigit(value.at(0)) )
	{
		return false;
	}

	for(unsigned int i=1; i<value.length(); i++)
	{
		if( (!isdigit(value[i])) &! (value[i] == '.') )
		{
			return false;
		}
	}
	return true;
}



//-------------------------------------------------------------------------------------------------------
void pqLineNetworkReader::restoreGuiState()
{
	QString text;

	text = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("ArrowProp"))->GetElement(0);
	this->setComboBoxIndex(this->ArrowProp, text);

	text = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("x1"))->GetElement(0);
	this->setComboBoxIndex(this->x1, text);

	text = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("y1"))->GetElement(0);
	this->setComboBoxIndex(this->y1, text);

	text = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("z1"))->GetElement(0);
	this->setComboBoxIndex(this->z1, text);

	text = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("x2"))->GetElement(0);
	this->setComboBoxIndex(this->x2, text);

	text = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("y2"))->GetElement(0);
	this->setComboBoxIndex(this->y2, text);

	text = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("z2"))->GetElement(0);
	this->setComboBoxIndex(this->z2, text);


	for(unsigned int i=0; i<stv->GetNumberOfElements(); i+=2)
	{
		if(strcmp(this->stv->GetElement(i+1), "1")==0)
		{
			this->tree->topLevelItem(i/2)->setCheckState(0, Qt::Checked);
		}
	}

	pqLoadedFormObjectPanel::accept();

}



//-------------------------------------------------------------------------------------------------------
void pqLineNetworkReader::setComboBoxIndex(QComboBox* comboBox, QString &text)
{
	for(int i=0; i<comboBox->count(); i++)
	{
		if(comboBox->itemText(i) == text)
		{
			comboBox->setCurrentIndex(i);
			return;
		}
	}
}

//-------------------------------------------------------------------------------------------------------
void pqLineNetworkReader::onPropertiesModified()
{
	for(unsigned int i=0; i<stv->GetNumberOfElements(); i+=2)
	{
		if(strcmp(this->stv->GetElement(i+1), "1")==0)
		{
			this->tree->topLevelItem(i/2)->setCheckState(0, Qt::Checked);
		}
		else
		{
			this->tree->topLevelItem(i/2)->setCheckState(0, Qt::Unchecked);
		}
	}

	pqLoadedFormObjectPanel::accept();
}