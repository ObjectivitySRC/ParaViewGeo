#include "pqPointSetReader.h"
#include <QtDebug>
#include "vtkSMProperty.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMProxy.h"
#include "vtkSMSourceProxy.h"
#include <QTreeWidget>
#include <QComboBox>

#include <QTreeWidgetItem>
#include <QFlags>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QtDebug>
#include <QLineEdit>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>

#include"pqPropertyManager.h"
#include "pqProxy.h"
#include <string>
#include "vtkEventQtSlotConnect.h"

//-------------------------------------------------------------------------------------------------------
pqPointSetReader::pqPointSetReader(pqProxy* pxy, QWidget* p) :
pqLoadedFormObjectPanel(":/StableReaders/PointSetReader/pqPointSetReader.ui", pxy, p)
  {

	this->VTKConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();

  stv = vtkSMStringVectorProperty::SafeDownCast(
			this->proxy()->GetProperty("Arrays"));
	this->tree = this->findChild<QTreeWidget*>("PropertiesTree");
	this->separator = this->findChild<QLineEdit*>("Separator");
	// Set default value for separator
	this->separator->setText(",");
	this->noDataValue = this->findChild<QLineEdit*>("NoDataValue");
	// Set default value for noDataValue
	this->noDataValue->setText("99999");
	QObject::connect(this->noDataValue, SIGNAL(textEdited ( const QString & )), this,
		SLOT(updatePreviewTable()));

	this->x1 = this->findChild<QComboBox*>("x1");
	this->y1 = this->findChild<QComboBox*>("y1");
	this->z1 = this->findChild<QComboBox*>("z1");

	// Set row & col numbers
	this->previewRows = 5;
	this->previewCols = 6;

	this->previewTable = this->findChild<QTableWidget*>("previewTable");
	this->previewTable->setRowCount( this->previewRows );
	this->previewTable->setColumnCount( this->previewCols );
	this->previewTable->setVisible(false);

	this->previewButton = this->findChild<QPushButton*>("previewButton");

	this->readPropertiesFromFile();

	vtkSMStringVectorProperty* stvx1 = vtkSMStringVectorProperty::SafeDownCast(
			this->proxy()->GetProperty("x1"));

	if( strlen(stvx1->GetElement(0)) )
	{
		this->restoreGuiState();
	}

	this->linkServerManagerProperties();

	// Catch signals here. If done in readPropertiesFromFile, onSeparatorChanged causes
	//		the signals to re-created over and over, which eventually crashes Paraview
	QObject::connect(this->tree, SIGNAL(itemChanged ( QTreeWidgetItem*, int)),
		SLOT(onTreeSelectionChanged( QTreeWidgetItem*, int)));

	QObject::connect(this->x1, SIGNAL(currentIndexChanged(int)), this, SLOT(updateCoordComboBoxes(int)));

	QObject::connect(this->separator, SIGNAL(textEdited ( const QString & )), this,
		SLOT(onSepartorChanged( const QString &)));

	QObject::connect(this->previewButton, SIGNAL(clicked ()), this, SLOT(togglePreviewTable()));

	this->VTKConnect->Connect(stv, vtkCommand::ModifiedEvent, 
	this, SLOT(onPropertiesModified()));

	}


	//-------------------------------------------------------------------------------------------------------
pqPointSetReader::~pqPointSetReader()
{
}


//-------------------------------------------------------------------------------------------------------
void pqPointSetReader::onPropertiesModified()
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

//-------------------------------------------------------------------------------------------------------
void pqPointSetReader::accept()
{
	this->VTKConnect->Disconnect();

	stv->SetElements(this->numberOfArrays, this->elements);

	this->VTKConnect->Connect(stv, vtkCommand::ModifiedEvent, 
		this, SLOT(onPropertiesModified()));

	pqLoadedFormObjectPanel::accept();
}


//-------------------------------------------------------------------------------------------------------
void pqPointSetReader::reset()
{
  // reset widgets controlled by the parent class
	pqLoadedFormObjectPanel::reset();
}


//-------------------------------------------------------------------------------------------------------
void pqPointSetReader::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
	pqLoadedFormObjectPanel::linkServerManagerProperties();
}



//-------------------------------------------------------------------------------------------------------
void pqPointSetReader::readPropertiesFromFile()
{
	vtkSMStringVectorProperty* stv = vtkSMStringVectorProperty::SafeDownCast(
			this->proxy()->GetProperty("FileName"));

	QString fileName = stv->GetElement(0);

	QFile file(fileName);
	QString line;
	QStringList lineSplit;

	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Failed to read the file";
		return;
	}

	// Replace \t with a TAB character
	const QString tmp = "\\t";
	const QString orgVal = this->separator->text();

	if(this->separator->text().compare(tmp) == 0)
	{
		const QString tmp2 = "\\t"; //'\t';
		this->separator->setText(tmp2);
	}

	QTextStream t( &file );
	line = t.readLine();
	lineSplit = line.split(this->separator->text());
	unsigned int numberOfHeaders = lineSplit.size();

	// Ignoring empty strings and forward slashes
	if(lineSplit.isEmpty() || this->separator->text() == "" || this->separator->text() == "/")
	{
		file.close();
		return;
	}
		
	// Clear the widgets so that they do not contain duplicates
	this->x1->clear();
	this->y1->clear();
	this->z1->clear();
	this->tree->clear();
	this->items.clear();

	QFlags<Qt::ItemFlag> flg=Qt::ItemIsUserCheckable |
													Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	bool loadStv = true;
	if(this->stv->GetNumberOfElements())
	{http://linux.about.com/library/cmd/blcmdln_foreach.htm
		loadStv = false;
	}

	int counter = 0;
	this->numberOfArrays = lineSplit.size()*2;
	this->elements = new const char*[this->numberOfArrays];
	char *elm;
	foreach(QString prop, lineSplit)
	{
		elm = new char[prop.length()+1];
		int i;
		for(i=0 ; i<prop.length() ; i++)
		{
			elm[i] = prop[i].toAscii();
		}
		elm[i] = '\0';
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
		}
	}

	QStringList lineSplit2;
	QString prop2;
	line = t.readLine();
	lineSplit2 = line.split(this->separator->text());
	int i=0;

	if(lineSplit2.size() != numberOfHeaders)
	{
		return;
	}

	foreach(QString prop, lineSplit2)
	{
		if( this->isNumeric(prop.toStdString()) )
		{
			prop2 = lineSplit[i];
			this->x1->addItem(prop2);
			this->y1->addItem(prop2);
			this->z1->addItem(prop2);
		}
		i++;
	}

	this->updateCoordComboBoxes(0);

	file.close();

	if(this->previewTable->isVisible())
	{
		this->readPreviewData();
	}
}


//-------------------------------------------------------------------------------------------------------
void pqPointSetReader::readPreviewData()
{
	vtkSMStringVectorProperty* stv = vtkSMStringVectorProperty::SafeDownCast(
			this->proxy()->GetProperty("FileName"));

	QString fileName = stv->GetElement(0);

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
	lineSplit = line.split(this->separator->text());
	unsigned int numberOfHeaders = lineSplit.size();

	// Ignoring empty strings and forward slashes
	if(lineSplit.isEmpty() || this->separator->text() == "" || this->separator->text() == "/")
	{
		file.close();
		return;
	}
		
	// Clear the widgets so that they do not contain duplicates
	this->previewTable->clear();

	QFlags<Qt::ItemFlag> flg=Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	int row = 0;
	int col = 0;

	foreach(QString prop, lineSplit)
	{
		if(col == this->previewCols)
		{
			break;
		}
		QTableWidgetItem *item = new QTableWidgetItem(prop);
		item->setFlags(flg);
		this->previewTable->setItem(row, col, item);
		col++;
	}
	col = 0;
	row++;

	QStringList lineSplit2;
	QString prop2;
	line = t.readLine();
	lineSplit2 = line.split(this->separator->text());
	bool readData = true;

	while(readData && lineSplit2.size() == numberOfHeaders)
	{
		foreach(QString prop, lineSplit2)
		{
			if(prop.toStdString().length() == 0)
			{
				prop = this->noDataValue->text();
			}
			if(col == this->previewCols)
			{
				col = 0;
				row++;
				break;
			}
			if(row == this->previewRows)
			{
				readData = false;
				break;
			}
			QTableWidgetItem *item = new QTableWidgetItem(prop);
			item->setFlags(flg);
			this->previewTable->setItem(row, col, item);
			col++;
		}
		line = t.readLine();
		lineSplit2 = line.split(this->separator->text());
	}

	file.close();
}


//-------------------------------------------------------------------------------------------------------
void pqPointSetReader::updateCoordComboBoxes(int index)
{
	this->y1->setCurrentIndex(index+1);
	this->z1->setCurrentIndex(index+2);
}

//-------------------------------------------------------------------------------------------------------
void pqPointSetReader::updatePreviewTable()
{
	if(this->previewTable->isVisible())
	{
		this->readPreviewData();
	}
}

//-------------------------------------------------------------------------------------------------------
void pqPointSetReader::togglePreviewTable()
{
	if(this->previewTable->isVisible())
	{
		this->previewTable->setVisible(false);
	}
	else
	{
		this->previewTable->setVisible(true);
		this->readPreviewData();
	}
}

//-------------------------------------------------------------------------------------------------------
void pqPointSetReader::onSepartorChanged(const QString & text)
{
	this->readPropertiesFromFile();
}


//-------------------------------------------------------------------------------------------------------
void pqPointSetReader::onTreeSelectionChanged(QTreeWidgetItem* item, int column)
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
bool pqPointSetReader::isNumeric(std::string value)
{
	if(value.length() == 0)
	{
		//qDebug() << "the first line is incomplete, the first line," <<endl<<
		//	"this line is used to determine if the column is numerical or not." <<
		//	"the column will be considered as text.";
		return true;
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
void pqPointSetReader::restoreGuiState()
{
	QString text;

	text = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("Separator"))->GetElement(0);
	this->separator->setText(text);

	text = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("NoDataValue"))->GetElement(0);
	this->noDataValue->setText(text);

	text = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("x1"))->GetElement(0);
	this->setComboBoxIndex(this->x1, text);

	text = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("y1"))->GetElement(0);
	this->setComboBoxIndex(this->y1, text);

	text = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("z1"))->GetElement(0);
	this->setComboBoxIndex(this->z1, text);

	QString sep = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("Separator"))->GetElement(0);

	// QStringList items = text.split(sep);

	// for(int i=0; i<this->tree->topLevelItemCount(); i++)
	// {
		// if(items.contains(this->tree->topLevelItem(i)->text(0)))
		// {
			// this->tree->topLevelItem(i)->setCheckState(0,Qt::Checked);
		// }
	// }
	
	for(unsigned int i=0 ; i<stv->GetNumberOfElements() ; i+=2)
	{
		if(strcmp(this->stv->GetElement(i+1),"1") == 0)
			this->tree->topLevelItem(i/2)->setCheckState(0,Qt::Checked);
	}

	pqLoadedFormObjectPanel::accept();
}

//-------------------------------------------------------------------------------------------------------
void pqPointSetReader::setComboBoxIndex(QComboBox* comboBox, QString &text)
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


