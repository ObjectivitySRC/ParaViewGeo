#include "vtkSMNewWidgetRepresentationProxy.h"
#include "pq3DWidgetFactory.h"
#include "pq3DWidget.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMRenderViewProxy.h"
#include "vtkPVXMLElement.h"
#include "vtkSmartPointer.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkCommand.h"
#include "pqApplicationCore.h"
#include "pqDOTReaderPointWidget.h"
#include "pqRenderView.h"
#include "pqActiveView.h"
#include "pqServer.h"
#include "vtkCommand.h"
#include <QtDebug>
#include <QShortcut>
#include "pqSMAdaptor.h"
#include <QMap>






//----------------------------------------------------------------------
pqDOTReaderPointWidget::pqDOTReaderPointWidget(pqServer *server)
{
	this->activeServer = server;
}

//----------------------------------------------------------------------
pqDOTReaderPointWidget::~pqDOTReaderPointWidget()
{
  pqApplicationCore::instance()->get3DWidgetFactory()->
    free3DWidget(this->widget);
	//this->widget->Delete();
}


//------------------------------------------------------------------------
pqDOTReaderPointWidget::pqDOTReaderPointWidget(pqServer *server, 
																							 double x, double y, double z)
{
	this->activeServer = server;
	this->addWidget(x, y, z);
}


//------------------------------------------------------------------------
void pqDOTReaderPointWidget::addWidget(double x, double y, double z)
{
	this->widget = 
		pqApplicationCore::instance()->get3DWidgetFactory()->
		get3DWidget("PointSourceWidgetRepresentation", this->activeServer);
	this->widget->UpdateVTKObjects();
	this->widget->UpdatePropertyInformation();

	this->setVisible(true);
	pqRenderView *rview = qobject_cast<pqRenderView*>(pqActiveView::instance().current());
	if(rview)
	{
		this->addRepresentation(rview);
	}

	this->move(x, y, z);
}



//------------------------------------------------------------------------
void pqDOTReaderPointWidget::setVisible(bool visible)
{
	 if(vtkSMIntVectorProperty* const visibility =
			vtkSMIntVectorProperty::SafeDownCast(
				this->widget->GetProperty("Visibility")))
	 {
		 visibility->SetElement(0, visible);
	 }
	 else
	 {
		 qDebug() << "Visibility not found";
	 }
	 this->widget->UpdateVTKObjects();
}


//----------------------------------------------------------------------
void pqDOTReaderPointWidget::addRepresentation(pqRenderView *rview)
{
	//rview->getRenderViewProxy()->RemoveRepresentation(this->widget);
	rview->getRenderViewProxy()->AddRepresentation(this->widget);
	rview->render();

}


//----------------------------------------------------------------------
void pqDOTReaderPointWidget::removeRepresentation(pqRenderView *rview)
{
	rview->getRenderViewProxy()->RemoveRepresentation(this->widget);
	rview->render();
}


//----------------------------------------------------------------------
void pqDOTReaderPointWidget::render()
{
	pqRenderView *rview = qobject_cast<pqRenderView*>(pqActiveView::instance().current());
	if(rview)
	{
		rview->render();
	}
}


//----------------------------------------------------------------------
void pqDOTReaderPointWidget::getPosition(double *pos)
{
	bool *ok = new bool();
	QList<QVariant> value;
	value = pqSMAdaptor::getMultipleElementProperty(
							this->widget->GetProperty("WorldPosition"));
	pos[0] = value[0].toDouble(ok);
	pos[1] = value[1].toDouble(ok);
	pos[2] = value[2].toDouble(ok);

	delete ok;
}



//------------------------------------------------------------------------------
void pqDOTReaderPointWidget::move(double x, double y, double z)
{
	QList<QVariant> value;
	value << x << y << z;
	pqSMAdaptor::setMultipleElementProperty(
			this->widget->GetProperty("WorldPosition"),value);
	this->widget->UpdateVTKObjects();
	//this->setModified();
	this->render();
}













