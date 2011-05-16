#include "pqDOTReader.h"

#include "vtkSMProperty.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMProxy.h"
#include "vtkSMSourceProxy.h"
#include "vtkSMDoubleVectorProperty.h"
#include "vtkSMProxyManager.h"
#include "vtkMath.h"

// 3DWidgets include:
#include "vtkSMNewWidgetRepresentationProxy.h"
#include "pq3DWidgetFactory.h"
#include "pq3DWidget.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMRenderViewProxy.h"
#include "vtkPVXMLElement.h"
#include "vtkSmartPointer.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkCommand.h"
#include "pqDOTReaderPointWidget.h"

#include <QString>
#include <QtDebug>
#include <QLineEdit>
#include <QWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QShortcut>
#include <QVariant>


#include "pqProxy.h"
#include "pqApplicationCore.h"
#include "pqObjectBuilder.h"
#include "pqServer.h"
#include "pqServerManagerModel.h"
#include "pqServerManagerSelectionModel.h"
#include "pqServerManagerModelItem.h"
#include "pqPipelineSource.h"
#include "pqView.h"
#include "pqRenderView.h"
#include "pqActiveView.h"
#include "pqDataRepresentation.h"

#include <string>




//-------------------------------------------------------------------------------------------------------
class pqDOTWidgetInternal
{
public:

	pqDOTWidgetInternal(pqServer *server, int r, double x, double y, double z)
	{
		this->widget = new pqDOTReaderPointWidget(server, x, y, z);
		this->row = r;
	}

	~pqDOTWidgetInternal()
	{
		delete this->widget;
	}

	int row;
	pqDOTReaderPointWidget *widget;
};





//-------------------------------------------------------------------------------------------------------
pqDOTReader::pqDOTReader(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/CustomWidget/pqDOTReader.ui", pxy, p)
{
	this->FromServer = this->findChild<QLineEdit*>("FromServer");
	this->ToServer = this->findChild<QLineEdit*>("ToServer");

	QObject::connect(this->FromServer, SIGNAL(textChanged(const QString&)),
		this, SLOT(createPointsWidgets(const QString&)),Qt::QueuedConnection);

	this->table = this->findChild<QTableWidget*>("tableWidget");

	this->AddWidget = this->findChild<QPushButton*>("AddWidget");
	this->RemoveWidget = this->findChild<QPushButton*>("RemoveWidget");

	this->ToServer->setVisible(false);
	this->FromServer->setVisible(false);

	QObject::connect(this->AddWidget, SIGNAL(released()), this, SLOT(onAddWidget()));
	QObject::connect(this->RemoveWidget, SIGNAL(released()), this, SLOT(onRemoveWidget()));

	//this->FromServer->setVisible(false);


	//vtkSMDoubleVectorProperty *center = vtkSMDoubleVectorProperty::SafeDownCast(
		//this->proxy()->GetProperty("Center1"));
	//center->SetNumberOfElements(4);
	//center->SetElement(0,5);
	//center->SetElement(1,5);
	//center->SetElement(2,5);
	//center->SetElement(3,5);
	//this->proxy()->UpdateVTKObjects();



  pqApplicationCore* core = pqApplicationCore::instance();
  pqServerManagerModel* sm = core->getServerManagerModel();
	this->activeServer = sm->getItemAtIndex<pqServer*>(0);

	this->pickHelper.setView(pqActiveView::instance().current());
	this->moveHelper.setView(pqActiveView::instance().current());

  this->pickShortcut = new QShortcut(Qt::Key_P
    , pqActiveView::instance().current()->getWidget());
    QObject::connect(this->pickShortcut, SIGNAL(activated()),
      &this->pickHelper, SLOT(pick()));

	QObject::connect(&this->pickHelper,
	SIGNAL(pickFinished(double, double, double)),
	this, SLOT(pick(double, double, double)));

    this->moveShortcut = new QShortcut(Qt::Key_M
    , pqActiveView::instance().current()->getWidget());
    QObject::connect(this->moveShortcut, SIGNAL(activated()),
      &this->moveHelper, SLOT(pick()));

	QObject::connect(&this->moveHelper,
	SIGNAL(pickFinished(double, double, double)),
	this, SLOT(moveWidget(double, double, double)));

	QObject::connect(&(pqActiveView::instance()), SIGNAL(changed(pqView*)), this, 
		SLOT(onActiveViewchanged(pqView*)));



	pqPipelineSource *src = dynamic_cast<pqPipelineSource*>(this->referenceProxy());

	QObject::connect(src, SIGNAL(representationAdded(pqPipelineSource*, 
    pqDataRepresentation*, int)),this, SLOT(addWidgetsRepresentations(
		pqPipelineSource*, pqDataRepresentation*, int)));

	QObject::connect(src, SIGNAL(representationRemoved(pqPipelineSource*, 
    pqDataRepresentation*, int)),this, SLOT(removeWidgetsRepresentations(
		pqPipelineSource*, pqDataRepresentation*, int)));




	//pqServer *activeServer = smmodel->findServer(_smproxy->GetConnectionID())

/*
	this->widgets = new pqDOTReaderPointWidget(activeServer);

	QObject::connect(this->widgets, SIGNAL(widgetMoved(QString&, double, double, double)), this,
									SLOT(onWidgetMoved(QString&, double, double, double)) );

*/

	this->linkServerManagerProperties();
}


//-------------------------------------------------------------------------------------------------------
void pqDOTReader::onAddWidget()
{
	this->table->setRowCount(this->table->rowCount() + 1);
}

//-------------------------------------------------------------------------------------------------------
void pqDOTReader::onRemoveWidget()
{
}


//-------------------------------------------------------------------------------------------------------
void pqDOTReader::showWidgets(bool visible)
{
	for(QMap< QString, pqDOTWidgetInternal* >::iterator it = 
		this->Widgets.begin(); it != this->Widgets.end(); it++)
	{
		it.value()->widget->setVisible(visible);
	}
}


//-------------------------------------------------------------------------------------------------------
void pqDOTReader::onActiveViewchanged(pqView* view)
{
	this->pickHelper.setView(view);
	this->moveHelper.setView(view);
}


//-------------------------------------------------------------------------------------------------------
void pqDOTReader::sendPointsToServer()
{
	//this->proxy()->AddProperty(pointi, myPoint);
	//this->proxy()->SetHints();
  pqApplicationCore* core = pqApplicationCore::instance();
  pqObjectBuilder* builder = core->getObjectBuilder();
  pqServerManagerModel* sm = core->getServerManagerModel();
  pqServerManagerSelectionModel* sms = core->getSelectionModel();
	pqServer *activeServer = sm->getItemAtIndex<pqServer*>(0);

	QList<pqPipelineSource*> pointsList = sm->findItems<pqPipelineSource*>(activeServer);
	QString text;
	vtkSMDoubleVectorProperty *center;

	foreach(pqPipelineSource *src, pointsList)
	{
		if(src->getProxy()->GetXMLName() == "PointSource")
		{
			center = vtkSMDoubleVectorProperty::SafeDownCast(
													src->getProxy()->GetProperty("Center"));
			text = QString::number(center->GetElement(0)) + "," + QString::number(center->GetElement(1)) + "," +
				QString::number(center->GetElement(2))+ "," + QString(src->getSMName());
		}
	}
}


//-------------------------------------------------------------------------------------------------------
void pqDOTReader::addWidgetsRepresentations(pqPipelineSource* source, 
									pqDataRepresentation* repr, int srcOutputPort)
{
	pqRenderView *rview = qobject_cast<pqRenderView*>(pqActiveView::instance().current());

	if(rview)
	{
		for(QMap< QString, pqDOTWidgetInternal* >::iterator it = 
			this->Widgets.begin(); it != this->Widgets.end(); it++)
		{
			it.value()->widget->addRepresentation(rview);
		}
	}
	
	QObject::connect(repr, SIGNAL(visibilityChanged(bool)), this, SLOT(showWidgets(bool)));
}


//-------------------------------------------------------------------------------------------------------
void pqDOTReader::removeWidgetsRepresentations(pqPipelineSource* source, 
									pqDataRepresentation* repr, int srcOutputPort)
{
	pqRenderView *rview = qobject_cast<pqRenderView*>(repr->getView());
	if(rview)
	{
		for(QMap< QString, pqDOTWidgetInternal* >::iterator it = 
			this->Widgets.begin(); it != this->Widgets.end(); it++)
		{
			it.value()->widget->removeRepresentation(rview);
		}
	}
}




//-------------------------------------------------------------------------------------------------------
void pqDOTReader::createPointsWidgets(const QString &points)
{
	QStringList pointList = this->FromServer->text().split("|",QString::SkipEmptyParts);
	QStringList pointCoords;
	double x,y,z;
	bool *ok = new bool;
	QString group;
	QSet<QString> names;
	int counter = 0;
	pqDOTWidgetInternal *widget;

	this->table->setRowCount(pointList.size());
	this->table->setColumnCount(3);

	QTableWidgetItem *item = new QTableWidgetItem("X");
	
	this->table->setHorizontalHeaderItem(0, item);

	item = new QTableWidgetItem("Y");
	this->table->setHorizontalHeaderItem(1, item);

	item = new QTableWidgetItem("Z");
	this->table->setHorizontalHeaderItem(2, item);

	foreach(QString point, pointList)
	{
		pointCoords = point.split(",");
		x = pointCoords[1].toDouble(ok);
		if(!ok)
		{
			continue;
		}
		y = pointCoords[2].toDouble(ok);
		if(!ok)
		{
			continue;
		}
		z = pointCoords[3].toDouble(ok);
		if(!(*ok))
		{
			continue;
		}

		group = pointCoords[4];
		if(!names.contains(group))
		{
			widget = new pqDOTWidgetInternal(this->activeServer, counter, x, y, z);
			this->Widgets[group] = widget;

			item = new QTableWidgetItem(group);
			this->table->setVerticalHeaderItem(counter, item);

			item = new QTableWidgetItem(QString::number(x));
			item->setTextAlignment(Qt::AlignCenter);
			this->table->setItem(counter, 0, item);

			item = new QTableWidgetItem(QString::number(y));
			item->setTextAlignment(Qt::AlignCenter);
			this->table->setItem(counter, 1, item);

			item = new QTableWidgetItem(QString::number(z));
			item->setTextAlignment(Qt::AlignCenter);
			this->table->setItem(counter, 2, item);

			counter++;
		}
	}
	this->table->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

	pqPipelineSource *src = dynamic_cast<pqPipelineSource*>(this->referenceProxy());
	pqDataRepresentation *repr = src->getRepresentation(pqActiveView::instance().current());
	
	QObject::connect(repr, SIGNAL(visibilityChanged(bool)), this, SLOT(showWidgets(bool)));

	
}


//-------------------------------------------------------------------------------------------------------
void pqDOTReader::onWidgetMoved(QString& group, double x, double y, double z)
{
	
	for(int i=0; i<this->table->rowCount(); i++)
	{
		if(table->verticalHeaderItem(i)->text() == group)
		{
			table->item(i,0)->setText(QString::number(x));
			table->item(i,1)->setText(QString::number(y));
			table->item(i,2)->setText(QString::number(z));
		}
	}
}


//----------------------------------------------------------------------
void pqDOTReader::pick(double x, double y, double z)
{

	double distance = 0;
	double min = VTK_DOUBLE_MAX;
	double pos[3];
	bool found = false;
	//int index = 0;

	double mouse[3];
	mouse[0] = x;
	mouse[1] = y;
	mouse[2] = z;

	for(QMap< QString, pqDOTWidgetInternal* >::iterator it = 
		this->Widgets.begin(); it != this->Widgets.end(); it++)
	{
		it.value()->widget->getPosition(pos);

		distance = vtkMath::Distance2BetweenPoints(pos, mouse);
		if(distance < min)
		{
			min = distance;
			this->target = it.value();
			table->selectRow(this->target->row);
			found = true;
		}
	}

	if(!found)
	{
		this->target = NULL;
		qDebug() << "null";
	}

}

//-------------------------------------------------------------------------------------------------------
void pqDOTReader::moveWidget(double x, double y, double z)
{
	this->target->widget->move(x, y, z);

	int i = target->row;
	
	table->item(i,0)->setText(QString::number(x));
	table->item(i,1)->setText(QString::number(y));
	table->item(i,2)->setText(QString::number(z));
}

//-------------------------------------------------------------------------------------------------------
pqDOTReader::~pqDOTReader()
{
	for(QMap< QString, pqDOTWidgetInternal* >::iterator it = 
		this->Widgets.begin(); it != this->Widgets.end(); it++)
	{
		delete it.value();
	}
}


//-------------------------------------------------------------------------------------------------------
void pqDOTReader::accept()
{
	pqLoadedFormObjectPanel::accept();
}


//-------------------------------------------------------------------------------------------------------
void pqDOTReader::reset()
{
  // reset widgets controlled by the parent class
	pqLoadedFormObjectPanel::reset();
}


//-------------------------------------------------------------------------------------------------------
void pqDOTReader::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
	pqLoadedFormObjectPanel::linkServerManagerProperties();

}

