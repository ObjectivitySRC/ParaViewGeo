#include "pqApplyScheduleToBlockModel.h"
#include <pqFileDialog.h>
#include <QtDebug>
#include "vtkSMProperty.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMProxy.h"
#include "vtkSMSourceProxy.h"
#include "vtkEventQtSlotConnect.h"

#include "pqProxy.h"


pqApplyScheduleToBlockModel::pqApplyScheduleToBlockModel(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/ABRIFilters/ApplyScheduleToBlockModel/pqApplyScheduleToBlockModel.ui", pxy, p)
  {
		this->QVTKConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
		this->ArraySelection = vtkSMStringVectorProperty::SafeDownCast(
			this->proxy()->GetProperty("ArraySelection"));
		if(!ArraySelection)
		{
			qDebug() << "ArraySelection property not found";
		}

		this->BrowseButton = this->findChild<QPushButton*>("BrowseButton");
		this->fileName = this->findChild<QLineEdit*>("fileName");
		this->table = this->findChild<QTableWidget*>("propertiesTableWidget");
		this->table->setObjectName("tableWidget");

		this->ScheduleSegID = this->findChild<QComboBox*>("ScheduleSegName");
		this->ScheduleSegID->setObjectName("ScheduleSegID");
		this->finishPropertyName = this->findChild<QComboBox*>("finishDatePropertyName");
		this->finishPropertyName->setObjectName("finishDatePropertyName");

		this->fileName->setReadOnly(true);

		loading = true;
		if(this->fileName->text().length() != 0)
		{
			this->onFileNameChanged();
			this->onStateReloaded();
		}

		QObject::connect(this->BrowseButton, SIGNAL(clicked()), this, SLOT(onBrowse()));
		//QObject::connect(this->fileName, SIGNAL( editingFinished() ), this,
			//SLOT( onFileNameChanged() ));
		QObject::connect(this->table, SIGNAL(itemChanged ( QTableWidgetItem*)),
			SLOT(onTableSelectionChanged( QTableWidgetItem*)));
		
		this->QVTKConnect->Connect(this->ArraySelection, vtkCommand::ModifiedEvent,
			this, SLOT(updateTable()));

	  this->linkServerManagerProperties();	
	}



//--------------------------------------------------------------------------------
pqApplyScheduleToBlockModel::~pqApplyScheduleToBlockModel()
{
}



//--------------------------------------------------------------------------------
void pqApplyScheduleToBlockModel::accept()
{
	this->QVTKConnect->Disconnect();
	this->ArraySelection->SetElements(this->numberOfArrays, this->arrays);
	this->QVTKConnect->Connect(this->ArraySelection, vtkCommand::ModifiedEvent,
		this, SLOT(updateTable()));

	pqLoadedFormObjectPanel::accept();
}


//--------------------------------------------------------------------------------
void pqApplyScheduleToBlockModel::reset()
{
  // reset widgets controlled by the parent class
	pqLoadedFormObjectPanel::reset();
}



//--------------------------------------------------------------------------------
void pqApplyScheduleToBlockModel::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
	pqLoadedFormObjectPanel::linkServerManagerProperties();

}



//--------------------------------------------------------------------------------
void pqApplyScheduleToBlockModel::clearTimeColumn(int id)
{
	for (int i=0 ; i < this->table->rowCount(); i++)
	{
		this->table->item(i, 1)->setCheckState(Qt::Unchecked);
	}
}

//--------------------------------------------------------------------------------
void pqApplyScheduleToBlockModel::onTableSelectionChanged(QTableWidgetItem * item)
{
	this->finishRow = this->finishPropertyName->currentIndex();
	bool start = false;
	bool checked = item->checkState() == Qt::Checked;
	QString text = this->table->verticalHeaderItem(item->row())->text();
	text += QString::number(item->column());

	if(!loading)
	{
		if(checked)
		{
			// only one column in each line can be selected
			for( unsigned int i=0 ; i<3 ; i++)
			{
				if( i != item->column() )
				{
					this->table->item(item->row(), i)->setCheckState(Qt::Unchecked);
				}
			}

			// if any item in the second row is selected, on item in the finish
			// line must be selected
			if(item->column() == 1)
			{
				bool addFinish = true;
				// if finish is already selected, we don't have to select it again
				for(int i=0 ; i<3; i++)
				{
					if( this->table->item(this->finishRow, i)->checkState() == Qt::Checked )
					{
						addFinish = false;
					}
				}
				if(addFinish)
					this->table->item(this->finishRow, 2)->setCheckState(Qt::Checked);
			}
			
			this->arrays[item->row()*4 + item->column() + 1] = "1";
		}
		else
		{
			if(item->row() == this->finishRow)
			{
				bool removeStart = true; // must be renamed removeTime

				// if finish item is unchecked, all items in time column
				// must be unchecked if one or more are checked
				for (int i=0 ; i < this->table->rowCount(); i++)
				{
					if( this->table->item(i, 1)->checkState() == Qt::Checked )
					{
						removeStart = true;
					}
				}

				for(int i=0 ; i<3; i++)
				{
					if( this->table->item(this->finishRow, i)->checkState() == Qt::Checked )
					{
						removeStart = false;
					}
				}
				for (int i=0 ; i < this->table->rowCount(); i++)
				{
					if(removeStart)
					{
						this->table->item(i, 1)->setCheckState(Qt::Unchecked);
					}
				}
			}
			this->arrays[item->row()*4 + item->column() + 1] = "0";
		}
	}

	this->referenceProxy()->setModifiedState(pqProxy::MODIFIED);
}




//--------------------------------------------------------------------------------
void pqApplyScheduleToBlockModel::updateTable()
{
	for( unsigned int i=0; i<this->ArraySelection->GetNumberOfElements(); i+=4 )
	{
		if( strcmp(this->ArraySelection->GetElement(i+1), "1") == 0 )
		{
			this->table->item(i/4, 0)->setCheckState(Qt::Checked);
			//this->table->item(i/4, 1)->setCheckState(Qt::Unchecked);
			//this->table->item(i/4, 2)->setCheckState(Qt::Unchecked);

		}
		else
		{
			this->table->item(i/4, 0)->setCheckState(Qt::Unchecked);
			if( strcmp(this->ArraySelection->GetElement(i+2), "1") == 0 )
			{
				this->table->item(i/4, 1)->setCheckState(Qt::Checked);
				//this->table->item(i/4, 2)->setCheckState(Qt::Unchecked);
			}
			else
			{
				this->table->item(i/4, 1)->setCheckState(Qt::Unchecked);
				if( strcmp(this->ArraySelection->GetElement(i+3), "1") == 0 )
				{
					this->table->item(i/4, 2)->setCheckState(Qt::Checked);
				}
				else
				{
					this->table->item(i/4, 2)->setCheckState(Qt::Unchecked);
				}
			}
		}
	}

	
	pqLoadedFormObjectPanel::accept();
	//this->referenceProxy()->setModifiedState(pqProxy::UNMODIFIED);
}

//--------------------------------------------------------------------------------
void pqApplyScheduleToBlockModel::restoreRowState(QString &value)
{
	int j = value[value.length()-1].digitValue();
	for(int i=0; i<table->rowCount(); i++)
	{
		if(value.startsWith(table->verticalHeaderItem(i)->text()))
		{
			table->item(i,j)->setCheckState(Qt::Checked);
		}
	}
}


//--------------------------------------------------------------------------------
void pqApplyScheduleToBlockModel::onStateReloaded()
{

	vtkSMSourceProxy* sp = vtkSMSourceProxy::SafeDownCast(this->proxy());
	//vtkErrorMacro(""<<sp->GetXMLLabel());
	vtkSMStringVectorProperty *finishPropVector = 
		vtkSMStringVectorProperty::SafeDownCast(sp->GetProperty("finishDatePropertyName"));
	QString finish = finishPropVector->GetElement(0);

	if(!finishPropVector)
	{
		qDebug() << "finish properties not found in the xml file";
	}

	for(int i=0; i<this->finishPropertyName->count(); i++)
	{
		if(this->finishPropertyName->itemText(i) == finish)
		{
			this->finishPropertyName->setCurrentIndex(i);
			break;
		}
	}

	vtkSMStringVectorProperty *ScheduleSegIdVector = 
		vtkSMStringVectorProperty::SafeDownCast(sp->GetProperty("ScheduleSegName"));
	QString seg = ScheduleSegIdVector->GetElement(0);

	if(!ScheduleSegIdVector)
	{
		qDebug() << "ScheduleSegName properties not found in the xml file";
	}

	for(int i=0; i<this->ScheduleSegID->count(); i++)
	{
		if(this->ScheduleSegID->itemText(i) == seg)
		{
			this->ScheduleSegID->setCurrentIndex(i);
			break;
		}
	}

	for( unsigned int i=0; i<this->ArraySelection->GetNumberOfElements(); i+=4 )
	{
		if( strcmp(this->ArraySelection->GetElement(i+1), "1") == 0 )
		{
			this->table->item(i/4, 0)->setCheckState(Qt::Checked);
		}
		else if( strcmp(this->ArraySelection->GetElement(i+2), "1") == 0 )
		{
			this->table->item(i/4, 1)->setCheckState(Qt::Checked);
		}
		else if( strcmp(this->ArraySelection->GetElement(i+3), "1") == 0 )
		{
			this->table->item(i/4, 2)->setCheckState(Qt::Checked);
		}
	}

	pqLoadedFormObjectPanel::accept();

}





//--------------------------------------------------------------------------------
void pqApplyScheduleToBlockModel::onBrowse()
{
  QString filters;
  filters += "ParaView state file (*.csv)";
  filters += ";;All files (*)";

  QString path;

	pqFileDialog *fileDialog = new pqFileDialog(NULL,
      this, tr("Open Schedule File:"), QString(), filters);
  //fileDialog->setAttribute(Qt::WA_DeleteOnClose);
  fileDialog->setObjectName("FileOpenSchedule");
  fileDialog->setFileMode(pqFileDialog::ExistingFile);

  if (fileDialog->exec() == QDialog::Accepted)
	{
    path = fileDialog->getSelectedFiles()[0];
	}

	if(path == QString())
		return;

	this->fileName->setText(path);

	this->onFileNameChanged();

}



//--------------------------------------------------------------------------------
void pqApplyScheduleToBlockModel::onFileNameChanged( )
{
	if(this->fileName->text().size() == 0)
		return;

	QString path = this->fileName->text();

	

	QObject::disconnect(this->finishPropertyName, SIGNAL( currentIndexChanged(int) ),
		this, SLOT( clearTimeColumn(int)) );

	if(!loading)
	{
		//this->DurationPropertyName->clear();
		this->ScheduleSegID->clear();
		this->table->clear();
	}

	QFlags<Qt::ItemFlag> flg=Qt::ItemIsUserCheckable | 
													Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	QFile file(path);
	QString line;

	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Failed to read the file";
		return;
	}

	QStringList list;
	QStringList list2;
	QStringList list3;

	QStringList wanted;
	//wanted.push_back("start");
	//wanted.push_back("finish");
	//wanted.push_back("finish");
	//wanted.push_back("rate");
	//wanted.push_back("constraint date");

	QTextStream t( &file );

	line = t.readLine();
	list = line.split(",");

	line = t.readLine();
	list2 = line.split(",");
	
	list.removeAt(0);
	list2.removeAt(0);

	int i=0;
	line.clear();


	foreach(QString prop, list)
	{
		this->ScheduleSegID->addItem(prop);

		//list2[i].toDouble(&ok);
		if( list2[i][0].isDigit() || atof(list2[i].toAscii().data()) )//ok || wanted.contains(prop.toLower()))
		{
			list3.push_back(prop);
			this->finishPropertyName->addItem(prop);
		}
		i++;
	}


	this->table->setRowCount(list3.size());
	this->table->setColumnCount(3);


	QTableWidgetItem *item = new QTableWidgetItem("Uniform");
	
	this->table->setHorizontalHeaderItem(0, item);

	item = new QTableWidgetItem("Time");
	this->table->setHorizontalHeaderItem(1, item);

	item = new QTableWidgetItem("Clone");
	this->table->setHorizontalHeaderItem(2, item);

	this->table->setEditTriggers(QAbstractItemView::NoEditTriggers);

	bool loadArrays = true;
	if(this->ArraySelection->GetNumberOfElements())
	{
		// if ArraySelection is not empty, that's mean a 
		// state is being loaded and we just need to copy
		// the existing arrays from the state
		loadArrays = false;
	}

	// in the table widget, we have 4 element per line,
	// so the number of element in the stringVectorProperty 
	// will be equal to 4 * the number of lines 
	this->numberOfArrays = list3.size()*4;
	this->arrays = new const char*[this->numberOfArrays];
	char* elem;

	i=0;
	int counter = 0;
	foreach(QString prop, list3)
	{
		elem = new char[prop.length() + 1];
		int count;
		for(count=0; count<prop.length(); count++)
		{
			elem[count] = prop[count].toAscii();
		}
		elem[count] = '\0';
		this->arrays[counter++] = elem;

		if(loadArrays)
		{
			
			this->arrays[counter++] = "0";
			this->arrays[counter++] = "0";
			this->arrays[counter++] = "0";
		}
		else
		{
			this->arrays[counter++] = this->ArraySelection->GetElement(counter);
			this->arrays[counter++] = this->ArraySelection->GetElement(counter);
			this->arrays[counter++] = this->ArraySelection->GetElement(counter);
		}
		

		item = new QTableWidgetItem(prop);
		this->table->setVerticalHeaderItem(i, item);

		item =  new QTableWidgetItem("");
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(flg);
		item->setCheckState(Qt::Unchecked);
		this->table->setItem(i, 0, item);

		item =  new QTableWidgetItem("");
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(flg);
		item->setCheckState(Qt::Unchecked);
		this->table->setItem(i, 1, item);

		item =  new QTableWidgetItem("");
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(flg);
		item->setCheckState(Qt::Unchecked);
		this->table->setItem(i, 2, item);
		
		i++;
	}

	table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

	loading = false;

	QObject::connect(this->finishPropertyName, SIGNAL( currentIndexChanged(int) ),
		this, SLOT( clearTimeColumn(int)) );

	file.close();
	
}
