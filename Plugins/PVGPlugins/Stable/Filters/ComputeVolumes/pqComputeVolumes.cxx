#include "pqComputeVolumes.h"
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QWidget>
#include <QDoubleSpinBox>
#include "vtkPoints.h"
#include <QtDebug>
//#include <QApplication>
//#include <QtGui>
#include <QAction>

#include "vtkSMProxy.h"
#include "pqProxy.h"
#include "vtkSMDoubleVectorProperty.h"
#include "vtkDoubleArray.h"

#include <QTableWidgetItem>
#include <QHeaderView>
#include <QClipboard>
#include <QApplication>



pqComputeVolumes::pqComputeVolumes(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/StableFilters/ComputeVolumes/pqComputeVolumes.ui", pxy, p)
  {

		this->volumes = this->findChild<QLineEdit*>("VolumesArray");
		this->volumesList = this->findChild<QTableWidget*>("volumesList");

		this->volumes->setVisible(false);

		QObject::connect(this->volumes, SIGNAL(textChanged(const QString&)),
			this, SLOT(onShowVolumesList(const QString&)),Qt::QueuedConnection);

		this->copyAction = new QAction("copy", this->volumesList);
		this->volumesList->addAction(copyAction);

		QObject::connect(this->copyAction, SIGNAL(triggered()), this, SLOT(onCopySelection()));

	  this->linkServerManagerProperties();
	}

pqComputeVolumes::~pqComputeVolumes()
{
}

void pqComputeVolumes::accept()
{
	pqLoadedFormObjectPanel::accept();
}

void pqComputeVolumes::reset()
{
  // reset widgets controlled by the parent class
	pqLoadedFormObjectPanel::reset();
}

void pqComputeVolumes::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
	pqLoadedFormObjectPanel::linkServerManagerProperties();

}



void pqComputeVolumes::onShowVolumesList(const QString& text)
{
	int i=0;
	double totalVolume = 0;
	QStringList list = text.split("|",QString::SkipEmptyParts);


	this->volumesList->setRowCount(list.size()+1);
	QTableWidgetItem *item2 = new QTableWidgetItem("");

	this->volumesList->verticalHeader()->hide();
	this->volumesList->setEditTriggers(QAbstractItemView::NoEditTriggers);


	foreach( QString v, list)
	{
		QTableWidgetItem *item = new QTableWidgetItem(QString::number(i+1));
		item->setTextAlignment(Qt::AlignCenter);
		this->volumesList->setItem(i,0,item);
		item = new QTableWidgetItem(v);
		item->setTextAlignment(Qt::AlignCenter);
		this->volumesList->setItem(i,1,item);
		i++;
		totalVolume += v.toDouble();
	}

	item2 = new QTableWidgetItem("Sum");
	item2->setTextAlignment(Qt::AlignCenter);
	this->volumesList->setItem(i,0,item2);
	item2 = new QTableWidgetItem(QString::number(totalVolume));
	item2->setTextAlignment(Qt::AlignCenter);
	this->volumesList->setItem(i,1,item2);

}



void pqComputeVolumes::onCopySelection()
{
	QString copied = "";
	for(int i=0; i<this->volumesList->rowCount(); i++)
	{
		QTableWidgetItem *item = this->volumesList->item(i,0);
		if(item->isSelected())
		{
			copied += item->text() + ",";
		}

		item = this->volumesList->item(i,1);
		if(item->isSelected())
		{
			copied += item->text() ;
		}

		copied += "\n";
	}

	QApplication::clipboard()->setText(copied);

}


/*void pqComputeVolumes::updateInformationAndDomains()
{
	vtkSMProperty* smProperty = this->proxy()->GetProperty("VolumesArray");
  if (NULL == smProperty)
  {
		qDebug() << "Volumes Array is NULL";
  }

	vtkSMDoubleVectorProperty* dvp = vtkSMDoubleVectorProperty::SafeDownCast(smProperty);
	qDebug()<<"number of elements: "<<dvp->GetNumberOfElements();
}*/
