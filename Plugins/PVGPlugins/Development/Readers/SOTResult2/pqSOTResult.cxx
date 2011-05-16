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
  pqLoadedFormObjectPanel(":/CustomWidget/pqSOTResult.ui", pxy, p)
{
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
  this->planeWidget = 0;

	// Hide widgets used to send data to server
	this->planeDistanceVariable = this->findChild<QLineEdit*>("planeDistanceVariable");
	this->planeDistanceVariable->hide();
	this->findChild<QLineEdit*>("bTransXVariable")->hide();
	this->findChild<QLineEdit*>("bTransYVariable")->hide();
	this->findChild<QLineEdit*>("bTransZVariable")->hide();
	this->findChild<QLineEdit*>("bScaleXVariable")->hide();
	this->findChild<QLineEdit*>("bScaleYVariable")->hide();
	this->findChild<QLineEdit*>("bScaleZVariable")->hide();
	this->findChild<QLineEdit*>("clipType")->hide();

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

	vtkSMStringVectorProperty *headers = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("Headers"));

	vtkSMStringVectorProperty *stringHeaders = vtkSMStringVectorProperty::SafeDownCast(
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
  this->sliceType =  this->findChild<QComboBox*>("sliceType");
  this->sliceFrame = this->findChild<QStackedWidget*>("sliceFrame");

	// remove all the widgets from the frame, we are going to add
	// the plan and box widgets to it
  for (int cc=this->sliceFrame->count(); cc >= 0; cc--)
    {
    QWidget *curWidget = this->sliceFrame->widget(cc);
    this->sliceFrame->removeWidget(curWidget);
    delete curWidget;
    }

  new pqComboBoxDomain(this->sliceType,
    pxy->getProxy()->GetProperty("CutFunction"),
    "proxy_list");

  QObject::connect(this->sliceType, SIGNAL(currentIndexChanged(int)), 
    this, SLOT(onCutFunctionChanged()));
  QObject::connect(this->sliceType, SIGNAL(currentIndexChanged(int)), 
    this, SIGNAL(cutFunctionChanged()));

	// Add 3DWidgets
  this->setup3DWidgetsIfNeeded();

  QObject::connect(pxy, SIGNAL(dataUpdated(pqPipelineSource*)),
    this, SLOT(setup3DWidgetsIfNeeded()));
  this->linkServerManagerProperties();

  this->propertyManager()->registerLink(
    this, "cutFunction", SIGNAL(cutFunctionChanged()),
    pxy->getProxy(),
    pxy->getProxy()->GetProperty("CutFunction"));
}

pqSOTResult::~pqSOTResult()
{
	delete this->bounds;
}

void pqSOTResult::accept()
{
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
	
  if (this->planeWidget && this->boxWidget)
    {
    return;
    }

  pqProxy* pqproxy = this->referenceProxy();
  double Bounds[6];
  qobject_cast<pqPipelineSource*>(pqproxy)
    ->getOutputPort(0)->getDataInformation(0)->GetBounds(Bounds);
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
        if (qobject_cast<pqImplicitPlaneWidget*>(widgets[0]))
          {		
          this->planeWidget = qobject_cast<pqImplicitPlaneWidget*>(widgets[0]);
          widget = this->planeWidget;
					this->sliceFrame->addWidget(widget);

					QPushButton *resetBounds = this->findChild<QPushButton*>("resetBounds");
					if(resetBounds)
						{
						resetBounds->disconnect();
						QObject::connect(resetBounds, SIGNAL(released()), this, SLOT(onResetBounds()));
						}

					this->originX = this->findChild<QLineEdit*>("originX");
					this->originX->hide();
					this->originY = this->findChild<QLineEdit*>("originY");
					this->originY->hide();
					this->originZ = this->findChild<QLineEdit*>("originZ");
					this->originZ->hide();

					// Catch movement of the plane (on Z Axis)
					QObject::connect(originZ, SIGNAL(textChanged(const QString&)), 
						this, SLOT(onOriginChanged(const QString&)));

					QPushButton *useXNormal = this->findChild<QPushButton*>("useXNormal");
					useXNormal->hide();
					QPushButton *useYNormal = this->findChild<QPushButton*>("useYNormal");
					useYNormal->hide();
					QPushButton *useCameraNormal = this->findChild<QPushButton*>("useCameraNormal");
					useCameraNormal->hide();

					QLabel *labelOrigin = this->findChild<QLabel*>("labelOrigin");
					labelOrigin->hide();
					
					labelOrigin = this->findChild<QLabel*>("labelNormal");
					labelOrigin->setText("Distance Value");

					QGridLayout *grid = new QGridLayout();

					grid->addWidget(labelOrigin,0,0);

					QLineEdit *distanceLineEdit = new QLineEdit();
					distanceLineEdit->setObjectName("distanceLineEdit");
					this->clipWidgetDistance = distanceLineEdit;
					widget->layout()->addWidget(distanceLineEdit);
					grid->addWidget(distanceLineEdit,0,1);

					this->useZNormal = this->findChild<QPushButton*>("useZNormal");
					grid->addWidget(useZNormal,1,0);

					grid->addWidget(resetBounds,1,1);

					QPushButton *useCenterBounds = this->findChild<QPushButton*>("useCenterBounds");
					grid->addWidget(useCenterBounds,2,1);

					widget->layout()->addItem(grid);

					}
					else if (qobject_cast<pqBoxWidget*>(widgets[0]))
					{
					
					this->boxWidget = qobject_cast<pqBoxWidget*>(widgets[0]);
					this->boxWidget->setRotatable(false);
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

          }

					// Set current slice type, and watch for changes
					this->clipType = this->findChild<QLineEdit*>("clipType");
					this->clipType->setText("Plane");
					QObject::connect(sliceType, SIGNAL(currentIndexChanged(const QString&)), 
						this, SLOT(onSliceTypeChanged(const QString&)));

					// Get pointer to "show" checkbox for widget
					this->showWidgetCB = this->findChild<QCheckBox*>("show3DWidget");

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
	
}

//-------------------------------------------------------------------------------------------------------
void pqSOTResult::onCutFunctionChanged()
{
  this->sliceFrame->setCurrentIndex(this->sliceType->currentIndex());
  if (this->planeWidget && this->boxWidget)
    {
    this->planeWidget->deselect();
    this->boxWidget->deselect();
    if (this->selected())
      {
      if (this->sliceType->currentText() == "Plane")
        {
        this->planeWidget->select();
        }
      else
        {
        this->boxWidget->select();
        }
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
  if (index != -1)
    {
    this->sliceType->setCurrentIndex(index);
    }
}

//-------------------------------------------------------------------------------------------------------
pqSMProxy pqSOTResult::cutFunction() const
{
  QList<pqSMProxy> proxies = pqSMAdaptor::getProxyPropertyDomain(
    this->proxy()->GetProperty("CutFunction"));  
  return proxies[this->sliceType->currentIndex()];
}


//-------------------------------------------------------------------------------------------------------
void pqSOTResult::onResetBounds()
{
	if(!this->planeWidget || !this->boxWidget)
	{
		return;
	}
	if (this->sliceType->currentText() == "Plane")
	{
		this->planeWidget->resetBounds(this->bounds);

		QLineEdit *currNorm = this->findChild<QLineEdit*>("normalZ");
		// Use value that will not be set by user. Allows widget normal to be "reset"
		currNorm->setText(".123456");
		currNorm->setText("1");
		currNorm = this->findChild<QLineEdit*>("normalX");
		currNorm->setText("0");
	}
	else
	{
		this->boxWidget->resetBounds(this->bounds);
	}
}

void pqSOTResult::onSliceTypeChanged(const QString& text)
{
	this->clipType->setText(text);
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