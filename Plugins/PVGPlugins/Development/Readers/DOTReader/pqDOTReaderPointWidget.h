/*
   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*=========================================================================
MIRARCO MINING INNOVATION
Author: Nehme Bilal (nehmebilal@gmail.com)
===========================================================================*/

#ifndef __pqDOTReaderPointWidget_h
#define __pqDOTReaderPointWidget_h

/*
#include "vtkSMNewWidgetRepresentationProxy.h"
#include "pq3DWidgetFactory.h"
#include "pq3DWidget.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMRenderViewProxy.h"
#include "vtkPVXMLElement.h"
#include "vtkSmartPointer.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkCommand.h"
#include "pqServer.h"
*/

#include "vtkSmartPointer.h"
#include "pqPickHelper.h"
#include <QMap>

class vtkSMNewWidgetRepresentationProxy;

class vtkEventQtSlotConnect;
class pqServer;
class vtkCommand;
class QShortcut;






#include <QObject>

class pqDOTReaderPointWidget : public QObject
{
	Q_OBJECT

public:


	pqDOTReaderPointWidget(pqServer *server);
	pqDOTReaderPointWidget(pqServer *server, double x, double y, double z);


	void setVisible(bool visible);

	// add widget representation to the render view
	void addRepresentation(pqRenderView *rview);

	void removeRepresentation(pqRenderView *rview);

	void render();

	void addWidget(double x, double y, double z);

	void move(double x, double y, double z);

	void getPosition(double *pos);

	virtual ~pqDOTReaderPointWidget();

signals:

	void widgetMoved(double x, double y, double z);

public slots:

	// update the widget position to the cursor position
	//void pick(double x, double y, double z);

	// pick the nearest widget to the mouse cursor
	//void pickWidget(double x, double y, double z);

protected:

	vtkSmartPointer<vtkSMNewWidgetRepresentationProxy> widget;
	//vtkSmartPointer<vtkEventQtSlotConnect> VTKConnect;
	//vtkSmartPointer<vtkCommand> ControlledPropertiesObserver;
	pqServer *activeServer;


};





#endif
