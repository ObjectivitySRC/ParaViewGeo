#include "pqSOTResult.h"
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QStackedWidget>
#include <QString>
#include <QStringList>
#include <QtDebug>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QGridLayout>
#include <QCheckBox>

#include "vtkSMProxy.h"
#include "vtkSMProperty.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMDoubleVectorProperty.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMRenderViewProxy.h"
#include "vtkPVDataInformation.h"
#include "vtkSMNewWidgetRepresentationProxy.h"
#include "vtkSMPropertyHelper.h"
#include "vtkSMSourceProxy.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkEventQtSlotConnect.h"

#include "pqBoxWidget.h"
#include "pqComboBoxDomain.h"
#include "pqImplicitPlaneWidget.h"
#include "pqOutputPort.h"
#include "pqPipelineSource.h"
#include "pqPropertyManager.h"
#include "pqProxySelectionWidget.h"
#include "pqRenderView.h"
#include "pqSMAdaptor.h"
#include "pqSMProxy.h"
#include "pqView.h"

pqSOTResult::pqSOTResult(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/StableReaders/SOTResult/pqSOTResult.ui", pxy, p)
{
	//adding for selection properties table
	this->QVTKConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
	this->PArraySelection = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("ArraySelection"));
	if(!this->PArraySelection)
		{
			qDebug() << "ArraySelection property not found";
		}

	this->Arrays= NULL;
	this->DataTable = this->findChild<QTableWidget*>("DataWidget");
	QObject::connect(this->DataTable, SIGNAL(itemChanged ( QTableWidgetItem*)),
									SLOT(onDataTableSelectionChanged( QTableWidgetItem*)));

	this->referenceCirclesSize = this->findChild<QSpinBox*>("referenceCirclesSize");
	this->distanceBetween = this->findChild<QSpinBox*>("distanceBetween");
	this->bounds = new double[6];
	double r = this->referenceCirclesSize->value();
	double d = this->distanceBetween->value();
	this->bounds[0] = -r;
	this->bounds[1] = r;
	this->bounds[2] = -r;
	this->bounds[3] = r;
	this->bounds[4] = 0;
	this->bounds[5] = -d;

	this->radiusVariable = this->findChild<QComboBox*>("radiusVariable");
	this->distanceVariable = this->findChild<QComboBox*>("distanceVariable");
	this->angleVariable = this->findChild<QComboBox*>("angleVariable");
	this->thVariable = this->findChild<QComboBox*>("thVariable");
  this->boxWidget = 0;

	// Hide widgets used to send data to server
	this->planeDistanceVariable = this->findChild<QLineEdit*>("planeDistanceVariable");
	this->planeDistanceVariable->hide();
	this->findChild<QLineEdit*>("bTransXVariable")->hide();
	this->findChild<QLineEdit*>("bTransYVariable")->hide();
	this->findChild<QLineEdit*>("bTransZVariable")->hide();
	this->findChild<QLineEdit*>("bScaleXVariable")->hide();
	this->findChild<QLineEdit*>("bScaleYVariable")->hide();
	this->findChild<QLineEdit*>("bScaleZVariable")->hide();

	this->distMin = vtkSMDoubleVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("DistMin"));

	this->distMax = vtkSMDoubleVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("DistMax"));

	this->thVariable->setEnabled(false);

	QCheckBox *box = this->findChild<QCheckBox*>("alignCircles");
	QObject::connect(box, SIGNAL(stateChanged(int)), this, SLOT(toogleParallelProjection(int)));

	QLineEdit *applyPressed = this->findChild<QLineEdit*>("ApplyPressed");
	applyPressed->setVisible(false);
	QObject::connect(applyPressed, SIGNAL(textChanged(const QString&)), 
		this, SLOT(onApplyPressed(const QString&)), Qt::QueuedConnection );

	headers = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("Headers"));

	stringHeaders = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("StringHeaders"));

	// Populate numberic property combo boxes
	QString line(headers->GetElement(0));
	QStringList lineSplit;

	lineSplit = line.split("|", QString::SkipEmptyParts);

	foreach(QString prop, lineSplit)
	{
		this->radiusVariable->addItem(prop);
		this->distanceVariable->addItem(prop);
		this->angleVariable->addItem(prop);
		this->thVariable->addItem(prop);
	}

	// Populate string property combo boxes
	QString stringLine(stringHeaders->GetElement(0));
	lineSplit = stringLine.split("|", QString::SkipEmptyParts);

	foreach(QString prop, lineSplit)
	{
		this->thVariable->addItem(prop);
	}

	vtkSMStringVectorProperty* stv = vtkSMStringVectorProperty::SafeDownCast(
			this->proxy()->GetProperty("radiusVariable"));

	if( strlen(stv->GetElement(0)) )
	{
		this->restoreGuiState();
	}

  // Create the selection widget for the proxies for the CutFunction.
  this->sliceTypeGroup = this->findChild<QGroupBox*>("sliceTypeGroup");
  this->sliceTypeGroup->setEnabled(false);
  this->sliceFrame = this->findChild<QStackedWidget*>("sliceFrame");

	// remove all the widgets from the frame, we are going to add
	// the plan and box widgets to it
  for (int cc=this->sliceFrame->count(); cc >= 0; cc--)
    {
    QWidget *curWidget = this->sliceFrame->widget(cc);
    this->sliceFrame->removeWidget(curWidget);
    delete curWidget;
    }

	// Add 3DWidgets
  this->setup3DWidgetsIfNeeded();

  QObject::connect(pxy, SIGNAL(dataUpdated(pqPipelineSource*)),
    this, SLOT(setup3DWidgetsIfNeeded()));
  

  this->propertyManager()->registerLink(
    this, "cutFunction", SIGNAL(cutFunctionChanged()),
    pxy->getProxy(),
    pxy->getProxy()->GetProperty("CutFunction"));

	//keep the properties synced
	this->QVTKConnect->Connect(this->PArraySelection, vtkCommand::ModifiedEvent,
		this, SLOT(updateDataTable()));

	this->linkServerManagerProperties();
}

pqSOTResult::~pqSOTResult()
{
	delete this->bounds;
	if (this->Arrays!=NULL)
		delete[] this->Arrays;
}

void pqSOTResult::accept()
{
	this->QVTKConnect->Disconnect();
	this->PArraySelection->SetElements(this->NbArrays, this->Arrays);
	this->QVTKConnect->Connect(this->PArraySelection, vtkCommand::ModifiedEvent,
		this, SLOT(updateDataTable()));

  // accept widgets controlled by the parent class
  pqLoadedFormObjectPanel::accept();
}

void pqSOTResult::reset()
{
  // reset widgets controlled by the parent class
  pqLoadedFormObjectPanel::reset();
}

void pqSOTResult::deselect()
{
  this->Superclass::deselect();
  this->onCutFunctionChanged();
}

void pqSOTResult::select()
{
  this->Superclass::select();
  this->onCutFunctionChanged();
}

void pqSOTResult::linkServerManagerProperties()
{
  // parent class hooks up some of our widgets in the ui
  pqLoadedFormObjectPanel::linkServerManagerProperties();

	QStringList list;
	QFlags<Qt::ItemFlag> flg=Qt::ItemIsUserCheckable | 
													Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	QStringList lineSplit;
	QString stringLine(this->headers->GetElement(0));
	lineSplit = stringLine.split("|", QString::SkipEmptyParts);
	foreach(QString prop, lineSplit)
		{
		list.push_back(prop);
		}

	QStringList lineSplit1;
	QString stringLine1(this->stringHeaders->GetElement(0));
	lineSplit1 = stringLine1.split("|", QString::SkipEmptyParts);
	foreach(QString prop, lineSplit1)
		{
		list.push_back(prop);
		}

	this->DataTable->setRowCount(list.size());
	this->DataTable->setColumnCount(1);
	QTableWidgetItem *item = new QTableWidgetItem("");	
	this->DataTable->setHorizontalHeaderItem(0, item);
	this->DataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	this->NbArrays= list.size()*2;
	this->Arrays = new const char*[this->NbArrays];

	int i=0, count=0, counter=0;
	char* elem;
	QString temp;
	foreach(QString prop, list)
		{
		elem = new char[prop.length()+1];
		for(count=0; count<prop.length(); count++)
		{
			elem[count] = prop[count].toAscii();
		}
		elem[count] = '\0';
		this->Arrays[counter] = elem;	
		this->Arrays[counter+1] = "0";

		temp= QString::fromAscii(elem); 
		item = new QTableWidgetItem(temp);
		this->DataTable->setVerticalHeaderItem(i, item);

		item =  new QTableWidgetItem("");
		item->setTextAlignment(Qt::AlignCenter);
		item->setFlags(flg);
		item->setCheckState(Qt::Unchecked);
		this->DataTable->setItem(i, 0, item);
		i++;
		counter+=2;
		}
	this->DataTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
 	this->referenceProxy()->setModifiedState(pqProxy::MODIFIED);
}


//-------------------------------------------------------------------------------------------------------
void pqSOTResult::restoreGuiState()
{
	QString text;

	text = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("radiusVariable"))->GetElement(0);
	this->setComboBoxIndex(this->radiusVariable, text);

	text = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("distanceVariable"))->GetElement(0);
	this->setComboBoxIndex(this->distanceVariable, text);

	text = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("angleVariable"))->GetElement(0);
	this->setComboBoxIndex(this->angleVariable, text);

	text = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("thVariable"))->GetElement(0);
	this->setComboBoxIndex(this->thVariable, text);

}


//-------------------------------------------------------------------------------------------------------
void pqSOTResult::setComboBoxIndex(QComboBox* comboBox, QString &text)
{
	for(int i=0; i<comboBox->count(); i++)
	{
		if(comboBox->itemText(i) == text)
		{
			comboBox->setCurrentIndex(i);
			return;
		}
	}
}

//-------------------------------------------------------------------------------------------------------
void pqSOTResult::toogleParallelProjection(int state)
{
	pqRenderView* ren = qobject_cast<pqRenderView*>(this->view());
	if(!ren)
	{
		return;
	}
	vtkSMRenderViewProxy *renProxy = ren->getRenderViewProxy();
	if(renProxy)
	{
		vtkSMIntVectorProperty* ivp = vtkSMIntVectorProperty::SafeDownCast(
			renProxy->GetProperty("CameraParallelProjection"));
		if(state == Qt::Checked)
		{
			pqSMAdaptor::setMultipleElementProperty(ivp,0,1);
			this->thVariable->setEnabled(true);
		}
		else
		{
			pqSMAdaptor::setMultipleElementProperty(ivp,0,0);
			this->thVariable->setEnabled(false);
		}
		renProxy->UpdateVTKObjects();

		// if you want to turn off/on parallel projection only after apply,
		// remove the following line
		ren->render();
	}
}

//-------------------------------------------------------------------------------------------------------
void pqSOTResult::onApplyPressed(const QString& text)
{
	pqRenderView* ren = qobject_cast<pqRenderView*>(this->view());
	if(!ren)
	{
		return;
	}
	//int state = this->showWidgetCB->checkState();
	if(text == "1")
	{
		this->toogleParallelProjection(Qt::Checked);
		ren->resetViewDirection(0, 0, -1, 0, 1, 0);
		//this->showWidgetCB->setEnabled(false);
		//int stop = 1;
	}
	else
	{
		this->toogleParallelProjection(Qt::Unchecked);
		//this->showWidgetCB->setEnabled(true);
	}
}

//-------------------------------------------------------------------------------------------------------
void pqSOTResult::onOriginChanged(const QString& text)
{

	if(this->distMin->GetNumberOfElements() > 0 && this->distMax->GetNumberOfElements() > 0)
	{
		double distScaled = (text.toDouble()/this->distanceBetween->value())*( this->distMax->GetElement(0) -
			this->distMin->GetElement(0) );

		this->clipWidgetDistance->setText(QString::number(distScaled*(-1)));

		this->planeDistanceVariable->setText(text);
	}
}

//-------------------------------------------------------------------------------------------------------
void pqSOTResult::setup3DWidgetsIfNeeded()
{
	double r = this->referenceCirclesSize->value();
	double d = this->distanceBetween->value();
	this->bounds[0] = -r;
	this->bounds[1] = r;
	this->bounds[2] = -r;
	this->bounds[3] = r;
	this->bounds[4] = 0;
	this->bounds[5] = -d;
	
  if (this->boxWidget)
    {
    return;
    }

  pqProxy* pqproxy = this->referenceProxy();
  double Bounds[6];
  qobject_cast<pqPipelineSource*>(pqproxy)
    ->getOutputPort(0)->getDataInformation()->GetBounds(Bounds);
  if (Bounds[1] >= Bounds[0] && Bounds[3] >= Bounds[2] &&
    Bounds[5] >= Bounds[4])
    {
    this->sliceTypeGroup->setEnabled(true);
    QList<pqSMProxy> proxies = pqSMAdaptor::getProxyPropertyDomain(
      pqproxy->getProxy()->GetProperty("CutFunction"));
    foreach (vtkSMProxy* _proxy, proxies)
      {
      QList<pq3DWidget*> widgets = 
        pq3DWidget::createWidgets(pqproxy->getProxy(), _proxy);
      pq3DWidget* widget = 0;
      if (widgets.size() == 1)
        {
        	this->boxWidget = qobject_cast<pqBoxWidget*>(widgets[0]);
					
					//TODO: nbilal - set rotatable must be added back
					//this->boxWidget->setRotatable(false);
					widget = this->boxWidget;

					this->sliceFrame->addWidget(widget);

					QPushButton *resetBounds2 = this->findChild<QPushButton*>("resetBounds");
					if(resetBounds2)
						{
						resetBounds2->disconnect();
						QObject::connect(resetBounds2, SIGNAL(released()), this, SLOT(onResetBounds()));
					}

					// Grab Transform LineEdits
					this->boxTransX = this->findChild<QLineEdit*>("positionX");
					this->boxTransY = this->findChild<QLineEdit*>("positionY");
					this->boxTransZ = this->findChild<QLineEdit*>("positionZ");

					// Grab Scale LineEdits
					this->boxScaleX = this->findChild<QLineEdit*>("scaleX");
					this->boxScaleY = this->findChild<QLineEdit*>("scaleY");
					this->boxScaleZ = this->findChild<QLineEdit*>("scaleZ");

					// Hide Rotation LineEdits and Label
					this->findChild<QLineEdit*>("rotationX")->hide();
					this->findChild<QLineEdit*>("rotationY")->hide();
					this->findChild<QLineEdit*>("rotationZ")->hide();
					this->findChild<QLabel*>("label_2")->hide();

					// Attach all signals to box data
					QObject::connect(boxTransX, SIGNAL(textChanged(const QString&)), 
						this, SLOT(onTransXChanged(const QString&)));
					QObject::connect(boxTransY, SIGNAL(textChanged(const QString&)), 
						this, SLOT(onTransYChanged(const QString&)));
					QObject::connect(boxTransZ, SIGNAL(textChanged(const QString&)), 
						this, SLOT(onTransZChanged(const QString&)));
					QObject::connect(boxScaleX, SIGNAL(textChanged(const QString&)), 
						this, SLOT(onScaleXChanged(const QString&)));
					QObject::connect(boxScaleY, SIGNAL(textChanged(const QString&)), 
						this, SLOT(onScaleYChanged(const QString&)));
					QObject::connect(boxScaleZ, SIGNAL(textChanged(const QString&)), 
						this, SLOT(onScaleZChanged(const QString&)));

					// Grab pointers to LineEdits that will send data to server
					this->bTransXVariable = this->findChild<QLineEdit*>("bTransXVariable");
					this->bTransYVariable = this->findChild<QLineEdit*>("bTransYVariable");
					this->bTransZVariable = this->findChild<QLineEdit*>("bTransZVariable");
					this->bScaleXVariable = this->findChild<QLineEdit*>("bScaleXVariable");
					this->bScaleYVariable = this->findChild<QLineEdit*>("bScaleYVariable");
					this->bScaleZVariable = this->findChild<QLineEdit*>("bScaleZVariable");

					// Get pointer to "show" checkbox for widget
					this->showWidgetCB = this->findChild<QCheckBox*>("show3DWidget");
					this->showWidgetCB->setChecked(false);
					
        if (widget)
          {				
					widget->resetBounds(this->bounds);
          QObject::connect(this, SIGNAL(onaccept()),
            widget, SLOT(accept()));
          QObject::connect(this, SIGNAL(onreset()),
            widget, SLOT(reset()));
          QObject::connect(widget, SIGNAL(modified()), 
            this, SLOT(setModified()));
          QObject::connect(this, SIGNAL(viewChanged(pqView*)),
            widget, SLOT(setView(pqView*)));
          widget->setView(this->view());
          }
        }
      }
    }
  this->onCutFunctionChanged();
		pqLoadedFormObjectPanel::accept();
}

//-------------------------------------------------------------------------------------------------------
void pqSOTResult::onCutFunctionChanged()
{
  this->sliceFrame->setCurrentIndex( 1);
  if (this->boxWidget)
    {
    this->boxWidget->deselect();
    if (this->selected())
      {
        this->boxWidget->select();
				this->onResetBounds();
      }
    }
}
//-------------------------------------------------------------------------------------------------------
void pqSOTResult::setCutFunction(pqSMProxy proxy)
{
  QList<pqSMProxy> proxies = pqSMAdaptor::getProxyPropertyDomain(
    this->proxy()->GetProperty("CutFunction"));  
  int index = proxies.indexOf(proxy);
  /*if (index != -1)
    {
    this->sliceType->setCurrentIndex(index);
    }*/
}

//-------------------------------------------------------------------------------------------------------
pqSMProxy pqSOTResult::cutFunction() const
{
  QList<pqSMProxy> proxies = pqSMAdaptor::getProxyPropertyDomain(
    this->proxy()->GetProperty("CutFunction"));  
  return proxies[ 0];
}


//-------------------------------------------------------------------------------------------------------
void pqSOTResult::onResetBounds()
{
	if(!this->boxWidget)
	{
		return;
	}

	this->boxWidget->resetBounds(this->bounds);
}


void pqSOTResult::onSliceTypeChanged(const QString& text)
{
	//this->clipType->setText(text);
}

//-------------------------------------------------------------------------------------------------------
void pqSOTResult::onTransXChanged(const QString& text)
{
	this->bTransXVariable->setText(text);
}

//-------------------------------------------------------------------------------------------------------
void pqSOTResult::onTransYChanged(const QString& text)
{
	this->bTransYVariable->setText(text);
}

//-------------------------------------------------------------------------------------------------------
void pqSOTResult::onTransZChanged(const QString& text)
{
	this->bTransZVariable->setText(text);
}

//-------------------------------------------------------------------------------------------------------
void pqSOTResult::onScaleXChanged(const QString& text)
{
	this->bScaleXVariable->setText(text);
}

//-------------------------------------------------------------------------------------------------------
void pqSOTResult::onScaleYChanged(const QString& text)
{
	this->bScaleYVariable->setText(text);
}

//-------------------------------------------------------------------------------------------------------
void pqSOTResult::onScaleZChanged(const QString& text)
{
	this->bScaleZVariable->setText(text);
}

void pqSOTResult::onDataTableSelectionChanged(QTableWidgetItem * item)
{
	bool checked = item->checkState() == Qt::Checked;
	if(checked)
		{
			this->DataTable->item(item->row(), 0)->setCheckState(Qt::Checked);
			this->Arrays[item->row()*2+1]= "1";	
		}
	else
		{
			this->DataTable->item(item->row(), 0)->setCheckState(Qt::Unchecked);
			this->Arrays[item->row()*2+1]= "0";	
		}
	
	this->referenceProxy()->setModifiedState(pqProxy::MODIFIED);
		
}

void pqSOTResult::updateDataTable()
{
	for( unsigned int i=0; i<this->PArraySelection->GetNumberOfElements(); i+=2 )
	{	
		if( strcmp(this->PArraySelection->GetElement(i+1), "1") == 0 )
			this->DataTable->item(i/2, 0)->setCheckState(Qt::Checked);
		else this->DataTable->item(i/2, 0)->setCheckState(Qt::Unchecked);
	}
	pqLoadedFormObjectPanel::accept();
}
