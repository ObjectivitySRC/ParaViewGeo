#include "pqGocadReader.h"

#include <QList>
#include <QVariant>

#include "vtkPVDataInformation.h"
#include "vtkProcessModule.h"
#include "vtkSMSourceProxy.h"
#include "vtkSMProxy.h"
#include "vtkSMDoubleVectorProperty.h"
#include "vtkSMProxyManager.h"
#include "vtkEventQtSlotConnect.h"

#include "pqDataRepresentation.h"
#include "pqOutputPort.h"
#include "pqRepresentation.h"
#include "pqPipelineSource.h"
#include "pqPropertyManager.h"
#include "pqSMAdaptor.h"
#include "pqView.h"
#include "pqPipelineRepresentation.h"


pqGocadReader::pqGocadReader(pqProxy* pxy, QWidget* p) :
  pqNamedObjectPanel(pxy, p)
{			
	//link the rest of the objects in the xml
	this->linkServerManagerProperties();
	
	this->QVTKConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
	
	this->RGB = vtkSMDoubleVectorProperty::SafeDownCast(this->proxy()->GetProperty("RGB"));
	
	this->QVTKConnect->Connect(this->RGB, vtkCommand::ModifiedEvent, 
			this, SLOT(UpdateRGB()));	
		
	QObject::connect(pxy, SIGNAL(dataUpdated(pqPipelineSource*)),
  this, SLOT(UpdateRGB()));
	
}

pqGocadReader::~pqGocadReader()
{
}

void pqGocadReader::accept()
{  	
	pqNamedObjectPanel::accept();		
}

void pqGocadReader::reset()
{
  // reset widgets controlled by the parent class
  pqNamedObjectPanel::reset();
}

void pqGocadReader::linkServerManagerProperties()
{		
  // parent class hooks up some of our widgets in the ui
  pqNamedObjectPanel::linkServerManagerProperties();	
}

void pqGocadReader::UpdateRGB()
{
	pqPipelineSource* ps = qobject_cast<pqPipelineSource*>(this->referenceProxy());
	
	pqPipelineRepresentation *pr = qobject_cast<pqPipelineRepresentation*> ( ps->getRepresentation(this->view()) );
	
	if ( pr != NULL )
		{		
		QList<QVariant> rgb;
		rgb.push_back( this->RGB->GetElement(0) );
		rgb.push_back( this->RGB->GetElement(1) );
		rgb.push_back( this->RGB->GetElement(2) );

		pqSMAdaptor::setMultipleElementProperty(
			pr->getProxy()->GetProperty("AmbientColor"), rgb);
		pqSMAdaptor::setMultipleElementProperty(
			pr->getProxy()->GetProperty("DiffuseColor"), rgb);
		
		pr->getProxy()->UpdateVTKObjects();
		this->updateInformationAndDomains();
		}
	
	
}