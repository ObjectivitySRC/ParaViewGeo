#include "pqGeoReferencedImageReader.h"

#include <QCheckBox>
#include <QFileInfo>

#include "vtkPVDataInformation.h"
#include "vtkProcessModule.h"
#include "vtkSMSourceProxy.h"
#include "vtkSMProxy.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMProxyManager.h"

#include "pqDataRepresentation.h"
#include "pqOutputPort.h"
#include "pqRepresentation.h"
#include "pqPipelineSource.h"
#include "pqPropertyManager.h"
#include "pqSMAdaptor.h"
#include "pqView.h"

#include <vtksys/SystemTools.hxx>

#define TEXTURESGROUP "textures"

pqGeoReferencedImageReader::pqGeoReferencedImageReader(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/WinStableReaders/GeoReferencedImage/pqGeoReferencedImageReader.ui", pxy, p)
{	
	this->linkServerManagerProperties();

	//get the file name we are using.
	vtkSMSourceProxy* sp = vtkSMSourceProxy::SafeDownCast(this->proxy());
	vtkSMStringVectorProperty *name = vtkSMStringVectorProperty::SafeDownCast( sp->GetProperty("FileName") );
	this->filename = name->GetElement(0);

	//setup up a connection between the the accept is finished and loading a texture
	QObject::connect(pxy, SIGNAL(dataUpdated(pqPipelineSource*)),
  this, SLOT(loadTexture()));
}

pqGeoReferencedImageReader::~pqGeoReferencedImageReader()
{
}

void pqGeoReferencedImageReader::accept()
{
  // accept widgets controlled by the parent class
  pqLoadedFormObjectPanel::accept();	
}


void pqGeoReferencedImageReader::reset()
{
  // reset widgets controlled by the parent class
  pqLoadedFormObjectPanel::reset();
}

void pqGeoReferencedImageReader::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
  pqLoadedFormObjectPanel::linkServerManagerProperties();
}

//-----------------------------------------------------------------------------
bool pqGeoReferencedImageReader::loadTexture()
{  

	QFileInfo finfo(this->filename);
  if (!finfo.isReadable() )
    {
    return false;
    }

	vtkSMSourceProxy* sp = vtkSMSourceProxy::SafeDownCast(this->proxy());
	vtkSMStringVectorProperty *name = vtkSMStringVectorProperty::SafeDownCast( sp->GetProperty("FileName") );
	this->filename = name->GetElement(0);

	pqPipelineSource* ps = qobject_cast<pqPipelineSource*>(this->referenceProxy());
	
	//we should never have more than 1 view for this, so we don't need to worry about this
	pqDataRepresentation *rep = ps->getRepresentation(  this->view() );
	if ( rep )
		{
		vtkSMProxyManager* pxm = vtkSMProxyManager::GetProxyManager();
		vtkSMProxy* texture = pxm->NewProxy("textures", "ImageTexture");
		
		texture->SetConnectionID( this->proxy()->GetConnectionID() );
		texture->SetServers(vtkProcessModule::CLIENT|vtkProcessModule::RENDER_SERVER);

		pqSMAdaptor::setElementProperty(texture->GetProperty("FileName"), filename);
		pqSMAdaptor::setEnumerationProperty(texture->GetProperty("SourceProcess"), "Client");
		texture->UpdateVTKObjects();

		pxm->RegisterProxy(TEXTURESGROUP,
			vtksys::SystemTools::GetFilenameName(filename.toAscii().data()).c_str(),
			texture);

		vtkSMProperty* textureProperty = rep->getProxy()->GetProperty("Texture");	
		if ( textureProperty )
			{
			pqSMAdaptor::setProxyProperty(textureProperty, texture);		
			texture->UpdateVTKObjects();
			}
		texture->Delete();
		}
  return true;
}