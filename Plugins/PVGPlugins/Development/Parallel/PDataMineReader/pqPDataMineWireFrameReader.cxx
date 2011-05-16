#include "pqPDataMineWireFrameReader.h"

//QT Includes
#include <QByteArray>
#include <QDir>
#include <QFileInfo>
#include <QtGui>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTableView>


//ParaView includes
#include "vtkStdString.h"
#include "vtkEventQtSlotConnect.h"

// ParaView Server Manager includes
#include "vtkSMSourceProxy.h"
#include "vtkSMProperty.h"
#include "vtkSMProperty.h"
#include "vtkSMProxyManager.h"
#include "vtkSMStringVectorProperty.h"

//ParaView GUI
#include "pqFileDialog.h"
#include "pqListWidgetItemObject.h"
#include "pqPropertyManager.h"
#include "pqProxy.h"
#include "pqPropertyLinks.h"
#include "pqSMAdaptor.h"
#include "pqSMSignalAdaptors.h"



#define NUM_COLS 3

InternalCheckableItem::InternalCheckableItem() : QObject(), QStandardItem()
	{
	this->setCheckable( true );
	this->setEditable( false );

	}
InternalCheckableItem::~InternalCheckableItem()
	{

	}

void InternalCheckableItem::setData(const QVariant &value, int role)
	{
	if ( role == Qt::CheckStateRole)
		{
		if ( value == Qt::Checked )
			{
			emit(checked());
			}
		emit modified();
		}	
	QStandardItem::setData( value, role );
	}

void InternalCheckableItem::unChecked( )
	{
	this->setData( Qt::Unchecked, Qt::CheckStateRole );
	}

pqPDataMineWireFrameReader::pqPDataMineWireFrameReader(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/CustomWidget/pqPDMWireFrameReader.ui", pxy, p)
{      
		
	//create the view/model
	this->Model = new QStandardItemModel( );
	this->View = this->findChild<QTableView *>("Properties");
	this->View->setModel( this->Model );


	//link to object created in the UI
	this->PointFileName = this->findChild<QLineEdit *>("PointFileName");
	this->TopoFileName = this->findChild<QLineEdit *>("TopoFileName");	
	
	this->FileName = this->findChild<QLineEdit *>("FileName");	
	
	//setup up the links, has to be done after we have set what table is
	this->linkServerManagerProperties();
	
	//set up the look
	this->FileName->setVisible( false ); //hide this always	
	
	//update the current path qstring
  QFileInfo pathInfo(this->FileName->text());
  this->CurrentDirectory = pathInfo.absoluteDir().absolutePath();
					
	//file loading connections
	QObject::connect(this->findChild<QPushButton *>("btnPoints"),
      SIGNAL(clicked()),
      this, SLOT(pointFileLoad()) );

	QObject::connect(this->findChild<QPushButton *>("btnTopo"),
      SIGNAL(clicked()),
      this, SLOT(topoFileLoad()) );      

	//keep the properties synced
	this->QVTKConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
	vtkSMStringVectorProperty *stringVector = vtkSMStringVectorProperty::SafeDownCast(
							this->proxy()->GetProperty("CellArrayStatus"));	
	this->QVTKConnect->Connect(stringVector, vtkCommand::ModifiedEvent, this, SLOT(updateProperties()));

	//if loading from a state, restore the property model/view
	if ( this->FileName->text().length() != 0 )
		{
		this->updateProperties();		
		this->setModified();
		}			    		
	
}

pqPDataMineWireFrameReader::~pqPDataMineWireFrameReader()
{
	delete this->PointFileName;
	delete this->TopoFileName;
	delete this->FileName;

	delete this->View;
	delete this->Model;
}


void pqPDataMineWireFrameReader::pointFileLoad()
{
	QString name = "Point WireFrame";
	QString text = "DataMine Point WireFrame";	
	this->FileLoad(name,text,this->PointFileName);	
}

void pqPDataMineWireFrameReader::topoFileLoad()
{
	QString name = "Triangle WireFrame";
	QString text = "DataMine Triangle WireFrame";	
	bool changed = this->FileLoad(name,text,this->TopoFileName);		
	if ( changed )
		{		
		//reset the view/model
		//this->List->clear();		//clear the visual List
		this->proxy()->UpdateVTKObjects();
		}		
}

bool pqPDataMineWireFrameReader::FileLoad( QString &name, QString &filterText, QLineEdit *lineEdit )
{
	QString filters = filterText + " File: (*.DM *.dm *.Dm);;All files (*)";
	
	pqFileDialog dialog(0, this, tr("Open " + name.toAscii() + " File:"), this->CurrentDirectory, filters);
  dialog.setObjectName(QString("Load")+name);
  dialog.setFileMode(pqFileDialog::ExistingFile);
  if (dialog.exec())
    {
    QStringList files = dialog.getSelectedFiles();
    
    if (files.size() > 0)
      {
      //grab the path
      QString path = files.at(0);      
      
			if ( path != lineEdit->text() )
				{
				//need to convert from QString to char*
				QByteArray tempPath = path.toAscii();
				char* name = tempPath.data();         
				
					
				lineEdit->setText( name );            
	      
				//update the current path qstring
				QFileInfo pathInfo(path);
				this->CurrentDirectory = pathInfo.absoluteDir().absolutePath();

				//path changed
				return true;
				}
      }
    }
	return false;
}

void pqPDataMineWireFrameReader::accept()
{
	//wow this is a hack of a work around since it seems the DrillHoles
	//we also use this to actually update the server with what values have been changes
	//we do it hear so the UI stays responsive while the user filters the list
	
	pqLoadedFormObjectPanel::accept();	
	vtkSMSourceProxy* sp = vtkSMSourceProxy::SafeDownCast(this->proxy());

	vtkSMStringVectorProperty *stringVector = vtkSMStringVectorProperty::SafeDownCast( this->proxy()->GetProperty("CellArrayStatus"));
	stringVector->SetImmediateUpdate( 0 );

	//we push the set as an array for speed reasons
	int size = stringVector->GetNumberOfElements();	
	
	const char** values = new const char*[ size ];
	const char* checked;

	int position = 0;
	QModelIndex index;
	QStandardItem *item;
	for ( int i=0; i < this->Model->rowCount() ; i++)
		{		
		index = this->Model->index(i,1);
		item = this->Model->itemFromIndex( index );

		checked = (item->checkState() == Qt::Checked ) ? "1" : "0"; //is the item checked or not				
		
		index = this->Model->index(i,2);
		item = this->Model->itemFromIndex( index );

		checked = (item->checkState() == Qt::Checked ) ? "2" : checked; //is the item checked or not							

		values[position] = stringVector->GetElement( position );
		values[position + 1 ] = checked;			
		
		position+=2;
		}

	stringVector->SetElements( size , values ); //push the values to the server
	stringVector->UpdateDependentDomains();
	sp->UpdateVTKObjects();
	
  // accept widgets controlled by the parent class
  pqLoadedFormObjectPanel::accept();
}

void pqPDataMineWireFrameReader::reset()
{
  // reset widgets controlled by the parent class
  pqLoadedFormObjectPanel::reset();
}

void pqPDataMineWireFrameReader::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
  pqLoadedFormObjectPanel::linkServerManagerProperties();

	//grab the domain to fill the table with
	vtkSMStringVectorProperty *stringVector = vtkSMStringVectorProperty::SafeDownCast(
							this->proxy()->GetProperty("CellArrayStatus"));
	QList<QVariant> sel_domain = pqSMAdaptor::getSelectionPropertyDomain(stringVector);

	int len = stringVector->GetNumberOfElements();
	int size = sel_domain.size();
			
	//populate the model with data from the server
	QList<QStandardItem*> items;
	for(int i=0; i< size; i++)
    {						
		QStandardItem *name = new QStandardItem( sel_domain[i].toString() );
		InternalCheckableItem *uni = new InternalCheckableItem( );
		InternalCheckableItem *seg = new InternalCheckableItem( );

		name->setEditable( false );
		
		items.append( name );
		items.append( uni );
		items.append( seg );

		//connect the two checkboxes, so they are like a radio box group
		QObject::connect( uni, SIGNAL( checked() ), seg, SLOT(unChecked() ) );
		QObject::connect( seg, SIGNAL( checked() ), uni, SLOT(unChecked() ) );

		//anyting a checkbox is used, throw a modified flag
		QObject::connect(uni, SIGNAL(modified( )), this, SLOT(setModified( )));			
		QObject::connect(seg, SIGNAL(modified( )), this, SLOT(setModified( )));			

		//have to connect to uncheck too!

		this->Model->insertRow(i, items );		
		items.clear();
    }

	//set up the headers	
	this->Model->setHeaderData(0,Qt::Horizontal,"Name");
	this->Model->setHeaderData(1,Qt::Horizontal,"Uniform");
	this->Model->setHeaderData(2,Qt::Horizontal,"Segmentable");
	this->View->verticalHeader()->hide();

	//other tweaks
	this->View->adjustSize();	
}

void pqPDataMineWireFrameReader::updateProperties()
{
	vtkSMStringVectorProperty *stringVector = vtkSMStringVectorProperty::SafeDownCast( this->proxy()->GetProperty("CellArrayStatus"));
	
	int size = this->Model->rowCount();
	int pos = 0, offset = 0;
	QModelIndex index;
	QStandardItem *item;
	
	for(int i=0; i< size ; i++)
		{		
		offset = QVariant(stringVector->GetElement(pos+1)).toInt();
		if ( offset > 0 )
			{		
			index = this->Model->index(i,offset);
			item = this->Model->itemFromIndex( index );
			item->setCheckState( Qt::Checked );
			}	
		pos+=2;
		}	
	//pqLoadedFormObjectPanel::accept();
}


#undef NUM_COLS