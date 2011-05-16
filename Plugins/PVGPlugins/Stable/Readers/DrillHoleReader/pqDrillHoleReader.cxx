#include "pqDrillHoleReader.h"

#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QListWidget>

#include "pqSMAdaptor.h"
#include "vtkSMSourceProxy.h"
#include "vtkSMProperty.h"
#include "vtkSMProperty.h"
#include "vtkSMProxyManager.h"
#include "vtkStdString.h"
#include "vtkSMStringVectorProperty.h"
#include "pqPropertyManager.h"
#include "pqProxy.h"
#include "pqListWidgetItemObject.h"
#include "pqPropertyLinks.h"
#include "pqSMSignalAdaptors.h"

#include "vtkEventQtSlotConnect.h"

//#include <time.h>

pqDrillHoleReader::pqDrillHoleReader(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/StableReaders/DrillHoleReader/pqDrillHoleReader.ui", pxy, p)
{
	this->QVTKConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
	this->drillHoles = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("DrillHoles"));
	this->QVTKConnect->Connect(this->drillHoles, vtkCommand::ModifiedEvent, 
	this, SLOT(UpdateGUI()));

	this->view = this->findChild<QListWidget*>("DrillHoleWidget");	
	this->view->setSelectionMode( QAbstractItemView::ExtendedSelection );
	this->view->sortItems();
	if (!this->view)
		{
		return;
		}
			
	//setup the filter input box
	//so that the user can actualy filter the drill hole list
	this->filter = this->findChild<QLineEdit*>("Filter");
	
	//make the signal connection of the lineEdit to the FilterModel
	QObject::connect(this->filter,
      SIGNAL(textChanged(const QString&)),
      this, SLOT(updateFilter(const QString&)));	


	this->CreatePointProp = this->findChild<QCheckBox*>("CreatePointProp");
	this->KeepCellProp = this->findChild<QCheckBox*>("KeepCellProp");
	this->KeepCellProp->setEnabled(false);
	
	QObject::connect(this->CreatePointProp, SIGNAL(stateChanged(int)),
      this, SLOT(EnableCellProp( )) );	
	QObject::connect(this->KeepCellProp, SIGNAL(stateChanged(int)),
      this, SLOT(updateAccept( )));	
		
	//link the rest of the objects in the xml
	this->linkServerManagerProperties();
}

pqDrillHoleReader::~pqDrillHoleReader()
{
}

void pqDrillHoleReader::updateFilter(const QString& pattern)
{		
		
	QListWidgetItem *item;
	QRegExp re(pattern);
	re.setPatternSyntax(QRegExp::FixedString);
	for ( int i=0; i < this->view->count() ; i++)
		{		
		item = this->view->item( i );
		bool hide = ( re.indexIn( item->text() ) == -1 ); // see if the item matches the string
		item->setHidden( hide );		//hide the object

		//if it is hidden, make sure it is unchecked
		Qt::CheckState checked = ( hide==true ) ? Qt::Unchecked : Qt::Checked; 		
		item->setCheckState( checked );
		}		
}

void pqDrillHoleReader::updateAccept()
	{
	this->referenceProxy()->setModifiedState(pqProxy::MODIFIED);	
	}
void pqDrillHoleReader::accept()
{  
	//wow, this a work around to the standard way to use pqListWidgetItemObject
	//we only want to push the status of all the pqListWidgetItemObject at accept time
	//rather than when each item is checked/unchecked.

	QListWidgetItem *item;
	pqLoadedFormObjectPanel::accept();	
	vtkSMSourceProxy* sp = vtkSMSourceProxy::SafeDownCast(this->proxy());

	this->drillHoles->SetImmediateUpdate( 0 );


	//we push the set as an array for speed reasons
	int size = this->view->count() * 2;
	const char** values = new const char*[ size ];
	const char* checked;
	int position = 0;
	for ( int i=0; i < this->view->count() ; i++)
		{		
		item = this->view->item( i );		
		checked = (item->checkState() == Qt::Checked ) ? "1" : "0"; //is the item checked or not				
		values[position] = this->drillHoles->GetElement( position );
		values[position + 1 ] = checked;	
		position+=2;
		}

	this->QVTKConnect->Disconnect();

	this->drillHoles->SetElements( size , values ); //push the values to the server
	this->drillHoles->UpdateDependentDomains();
	sp->UpdateVTKObjects();

	this->QVTKConnect->Connect(this->drillHoles, vtkCommand::ModifiedEvent, 
	this, SLOT(UpdateGUI()));

	// accept widgets controlled by the parent class	
	pqLoadedFormObjectPanel::accept();
	
}

void pqDrillHoleReader::reset()
{
  // reset widgets controlled by the parent class
  pqLoadedFormObjectPanel::reset();
}

void pqDrillHoleReader::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
  pqLoadedFormObjectPanel::linkServerManagerProperties();

	
	QPixmap cellPixmap(":/DrillHole/pqDrillHole16.PNG");
	//get the data from the server
	vtkSMSourceProxy* sp = vtkSMSourceProxy::SafeDownCast(this->proxy());

	QList<QVariant> sel_domain = pqSMAdaptor::getSelectionPropertyDomain(this->drillHoles);
		
	//populate the model with data from the server	
	for(int j=0; j<sel_domain.size(); j++)
    {
    pqListWidgetItemObject* item = 
			new pqListWidgetItemObject(sel_domain[j].toString(), this->view);		
		item->setChecked( true );
		item->setData(Qt::DecorationRole,cellPixmap);					
		QObject::connect(item, SIGNAL(checkedStateChanged(bool)),
      this, SLOT(updateAccept( )));			
    }
}



void pqDrillHoleReader::UpdateGUI()
{
	for(unsigned int i=0; i<this->drillHoles->GetNumberOfElements(); i+=2)
	{
		if(strcmp(this->drillHoles->GetElement(i+1), "1") == 0)
		{
			this->view->item(i/2)->setCheckState(Qt::Checked);
		}
		else
		{
			this->view->item(i/2)->setCheckState(Qt::Unchecked);	
		}
	}
	pqLoadedFormObjectPanel::accept();
}

void pqDrillHoleReader::EnableCellProp()
{
	this->KeepCellProp->setEnabled(!this->KeepCellProp->isEnabled());
	this->updateAccept();
}