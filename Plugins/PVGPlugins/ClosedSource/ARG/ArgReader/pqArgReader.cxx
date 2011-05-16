#include "pqArgReader.h"
#include "pqProxy.h"
#include <QtDebug>
#include "vtkSMProperty.h"
#include "vtkSMProxy.h"
#include "vtkSMSourceProxy.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkEventQtSlotConnect.h"

pqArgReader::pqArgReader(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/ArgReader/pqArgReader.ui", pxy, p)
{   
	this->QVTKConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
	this->PArraySelection = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("ArraySelection"));
	if(!this->PArraySelection)
		{
			qDebug() << "ArraySelection property not found";
		}
	this->Arrays= NULL;
	this->DMTable = this->findChild<QTableWidget*>("DMWArraysWidget");
	this->PUNOTable= this->findChild<QTableWidget*>("PunoArraysWidget");
	this->DeclineTable= this->findChild<QTableWidget*>("DeclineArraysWidget");	

	QObject::connect(this->DMTable, SIGNAL(itemChanged ( QTableWidgetItem*)),
			SLOT(onDMTableSelectionChanged( QTableWidgetItem*)));

	QObject::connect(this->PUNOTable, SIGNAL(itemChanged ( QTableWidgetItem*)),
			SLOT(onPUNOTableSelectionChanged( QTableWidgetItem*)));

	QObject::connect(this->DeclineTable, SIGNAL(itemChanged ( QTableWidgetItem*)),
			SLOT(onDeclineTableSelectionChanged( QTableWidgetItem*)));
	
	//keep the properties synced
	this->QVTKConnect->Connect(this->PArraySelection, vtkCommand::ModifiedEvent,
			this, SLOT(updateTable()));

  this->linkServerManagerProperties();

}

pqArgReader::~pqArgReader()
{
	if (this->Arrays!=NULL)
		delete[] this->Arrays;
}

void pqArgReader::accept()
{	
	this->QVTKConnect->Disconnect();
	this->PArraySelection->SetElements(this->nArrays, this->Arrays);
	this->QVTKConnect->Connect(this->PArraySelection, vtkCommand::ModifiedEvent,
		this, SLOT(updateTable()));

  // accept widgets controlled by the parent class
  pqLoadedFormObjectPanel::accept();
}

void pqArgReader::reset()
{
  // reset widgets controlled by the parent class
  pqLoadedFormObjectPanel::reset();
}

void pqArgReader::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
  pqLoadedFormObjectPanel::linkServerManagerProperties();

	//populate the model with data from the server	
	vtkSMStringVectorProperty *stringHeaders = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("ArraysInfo"));

	
	QStringList list;
	QStringList punoList;
	QStringList DeclineList;
	QFlags<Qt::ItemFlag> flg=Qt::ItemIsUserCheckable | 
													Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	
	QStringList lineSplit;
 	QString stringLine(stringHeaders->GetElement(0));
	lineSplit = stringLine.split("|", QString::SkipEmptyParts);
	char* first= new char[2];
	foreach(QString prop, lineSplit)
		{
		first[0]= prop[0].toAscii();
		first[1]='\0';
		if(strcmp(first,"0")==0)
		list.push_back(prop);
		else if(strcmp(first,"1")==0)
		punoList.push_back(prop);
		else if(strcmp(first,"2")==0)
		DeclineList.push_back(prop);
		}

	this->DMTable->setRowCount(list.size());
	this->DMTable->setColumnCount(2);

	QTableWidgetItem *item = new QTableWidgetItem("Uniform");	
	this->DMTable->setHorizontalHeaderItem(0, item);
	item = new QTableWidgetItem("Segmentable");
	this->DMTable->setHorizontalHeaderItem(1, item);

	this->DMTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

	this->DMNumberOfArrays = list.size()*2;
	this->PUNONumberOfArrays = punoList.size()*2;
	this->DeclineNumberOfArrays = DeclineList.size()*2;
	this->nArrays= this->DMNumberOfArrays+this->PUNONumberOfArrays+this->DeclineNumberOfArrays;
	this->Arrays = new const char*[this->nArrays];

  int i=0, count=0, counter=0;
	char* elem;
	QString temp;
	foreach(QString prop, list)
		{
		elem = new char[prop.length()];
		for(count=0; count<(prop.length()-1); count++)
		{
			elem[count] = prop[count+1].toAscii();
		}
		elem[count] = '\0';
		this->Arrays[counter] = elem;	
		this->Arrays[counter+1] = "0";

		temp= QString::fromAscii(elem); 
		item = new QTableWidgetItem(temp);
		this->DMTable->setVerticalHeaderItem(i, item);

		item =  new QTableWidgetItem("");
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(flg);
		item->setCheckState(Qt::Unchecked);
		this->DMTable->setItem(i, 0, item);

		item =  new QTableWidgetItem("");
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(flg);
		item->setCheckState(Qt::Unchecked);
		this->DMTable->setItem(i, 1, item);

		i++;
		counter+=2;
		}
	this->DMTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

	item = new QTableWidgetItem("Status");	
	this->PUNOTable->setHorizontalHeaderItem(0, item);

	this->PUNOTable->setRowCount(punoList.size());
	this->PUNOTable->setColumnCount(1);
	this->PUNOTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

  i=0;
	count=0;
	
	foreach(QString prop, punoList)
		{
		elem = new char[prop.length()];
		for(count=0; count<(prop.length()-1); count++)
		{
			elem[count] = prop[count+1].toAscii();
		}
		elem[count] = '\0';
		this->Arrays[counter] = elem;	
		this->Arrays[counter+1] = "3";

		temp= QString::fromAscii(elem); 
		item = new QTableWidgetItem(temp);
		this->PUNOTable->setVerticalHeaderItem(i, item);

		item =  new QTableWidgetItem("");
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(flg);
		item->setCheckState(Qt::Unchecked);
		this->PUNOTable->setItem(i, 0, item);

		i++;
		counter+=2;
		}
	this->PUNOTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);


	item = new QTableWidgetItem("Status");	
	this->DeclineTable->setHorizontalHeaderItem(0, item);

	this->DeclineTable->setRowCount(DeclineList.size());
	this->DeclineTable->setColumnCount(1);
	this->DeclineTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    i=0;
	count=0; 
	
	foreach(QString prop, DeclineList)
		{
		elem = new char[prop.length()];
		for(count=0; count<(prop.length()-1); count++)
		{
			elem[count] = prop[count+1].toAscii();
		}
		elem[count] = '\0';
		this->Arrays[counter] = elem;	
		this->Arrays[counter+1] = "5";

		temp= QString::fromAscii(elem); 
		item = new QTableWidgetItem(temp);
		this->DeclineTable->setVerticalHeaderItem(i, item);

		item =  new QTableWidgetItem("");
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(flg);
		item->setCheckState(Qt::Unchecked);
		this->DeclineTable->setItem(i, 0, item);

		i++;
		counter+=2;
		}
	this->DeclineTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	//this->updateTable();	
	this->referenceProxy()->setModifiedState(pqProxy::MODIFIED);
}


//--------------------------------------------------------------------------------
void pqArgReader::onDMTableSelectionChanged(QTableWidgetItem * item)
{
	bool checked = item->checkState() == Qt::Checked;
	
	if(checked)
		{
			// only one column in each line can be selected
			for( unsigned int i=0 ; i<2; i++)
			{
				if( i != item->column() )
					this->DMTable->item(item->row(), i)->setCheckState(Qt::Unchecked);				
				else this->DMTable->item(item->row(), i)->setCheckState(Qt::Checked);
			}			
			if(item->column()==0)
				this->Arrays[item->row()*2 +1] = "1";
			else if(item->column()==1)
				this->Arrays[item->row()*2 +1] = "2";
		}
	else
	 this->Arrays[item->row()*2 +1] = "0";
	 	
	this->referenceProxy()->setModifiedState(pqProxy::MODIFIED);
}

//--------------------------------------------------------------------------------
void pqArgReader::onPUNOTableSelectionChanged(QTableWidgetItem * item)
{
	bool checked = item->checkState() == Qt::Checked;
	
	int n= (this->DMNumberOfArrays/2)+item->row();
	if(checked)
		{
		this->PUNOTable->item(item->row(), 0)->setCheckState(Qt::Checked);
		this->Arrays[n*2+1]= "4";	
		}
	else
		{
		this->PUNOTable->item(item->row(), 0)->setCheckState(Qt::Unchecked);
		this->Arrays[n*2+1]= "3";	
		}
	
	this->referenceProxy()->setModifiedState(pqProxy::MODIFIED);
}

//--------------------------------------------------------------------------------
void pqArgReader::onDeclineTableSelectionChanged(QTableWidgetItem * item)
{
	bool checked = item->checkState() == Qt::Checked;
  
	int n= (this->DMNumberOfArrays/2)+(this->PUNONumberOfArrays/2)+item->row();
	if(checked)
		{
		this->DeclineTable->item(item->row(), 0)->setCheckState(Qt::Checked);
		this->Arrays[n*2+1]= "6";		
		}
	else
		{
		this->DeclineTable->item(item->row(), 0)->setCheckState(Qt::Unchecked);
		this->Arrays[n*2+1]= "5";		
		}
	
	this->referenceProxy()->setModifiedState(pqProxy::MODIFIED);
}

//--------------------------------------------------------------------------------
void pqArgReader::updateTable()
{
	for( unsigned int i=0; i<this->PArraySelection->GetNumberOfElements(); i+=2 )
	{
		if(i<this->DMNumberOfArrays)
		{
			if( strcmp(this->PArraySelection->GetElement(i+1), "1") == 0 )
				{
				this->DMTable->item(i/2, 0)->setCheckState(Qt::Checked);
				this->DMTable->item(i/2, 1)->setCheckState(Qt::Unchecked);
				}
			else if( strcmp(this->PArraySelection->GetElement(i+1), "2") == 0 )
				{
				this->DMTable->item(i/2, 0)->setCheckState(Qt::Unchecked);
				this->DMTable->item(i/2, 1)->setCheckState(Qt::Checked);
				}			
			else if( strcmp(this->PArraySelection->GetElement(i+1), "0") == 0 )
			{
			this->DMTable->item(i/2, 0)->setCheckState(Qt::Unchecked);
			this->DMTable->item(i/2, 1)->setCheckState(Qt::Unchecked);
			}
		}
		else if((i>=this->DMNumberOfArrays)&&(i<(this->DMNumberOfArrays+this->PUNONumberOfArrays)) )
		{
			if( strcmp(this->PArraySelection->GetElement(i+1), "4") == 0 )
				this->PUNOTable->item((i-this->DMNumberOfArrays)/2, 0)->setCheckState(Qt::Checked);
			else this->PUNOTable->item((i-this->DMNumberOfArrays)/2, 0)->setCheckState(Qt::Unchecked);
		}
		else if((i>=(this->DMNumberOfArrays+this->PUNONumberOfArrays))
						&&(i<(this->DMNumberOfArrays+this->PUNONumberOfArrays+ this->DeclineNumberOfArrays)) )
		{
			if( strcmp(this->PArraySelection->GetElement(i+1), "6") == 0 )
				this->DeclineTable->item((i-(this->DMNumberOfArrays+this->PUNONumberOfArrays))/2, 0)->setCheckState(Qt::Checked);
			else 	this->DeclineTable->item((i-(this->DMNumberOfArrays+this->PUNONumberOfArrays))/2, 0)->setCheckState(Qt::Unchecked);
		}
	}

	pqLoadedFormObjectPanel::accept();
}



