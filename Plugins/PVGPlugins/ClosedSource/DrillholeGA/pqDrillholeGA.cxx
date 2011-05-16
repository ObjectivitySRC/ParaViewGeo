#include "pqDrillholeGA.h"
#include <QComboBox>
#include <QLineEdit>
#include <QString>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "vtkSMStringVectorProperty.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMProxy.h"
#include "vtkSMSourceProxy.h"
#include "pqApplicationCore.h"


pqDrillholeGA::pqDrillholeGA(pqProxy* pxy, QWidget* p) :
  pqLoadedFormObjectPanel(":/CustomWidget/pqDrillholeGA.ui", pxy, p)
{
  //Obtain references to involved widgets (store them as member variables if they are involved
  //in signal slot connections
  this->ResumeFrom = this->findChild<QComboBox*>("ResumeFrom");
  this->ResumeFromC2S = this->findChild<QLineEdit*>("ResumeFromC2S");
  this->ResumeFromC2S->setVisible(false);
  this->ResumePreviousRun = this->findChild<QCheckBox*>("Resume");
  this->CollarLocation = this->findChild<QComboBox*>("CollarLocation");

  this->HCLength = this->findChild<QCheckBox*>("DoHillClimbLength");
  this->HCDirection = this->findChild<QCheckBox*>("DoHillClimbDirection");
  this->HCLengthAmount = this->findChild<QDoubleSpinBox*>("HillClimbLength");
  this->HCDirectionAmountAz = this->findChild<QDoubleSpinBox*>("HillClimbAzimuthWindow");
  this->HCDirectionAmountDip = this->findChild<QDoubleSpinBox*>("HillClimbDipWindow");

  this->RefreshButton = this->findChild<QPushButton*>("RefreshButton");
  this->RefreshPlease = this->findChild<QCheckBox*>("RefreshPlease");
  this->RefreshPlease->setVisible(false);
  this->RefreshPlease->setChecked(false);
  
  QString top = this->FillComboBox();
  this->ResumeFromC2S->setText(top); // I also make sure that my client-to-server property has a value

  //Make a signal-slot connection so that everytime the user changes the combo box 
  //index, the client-to-server property is updated with the new text value
  QObject::connect(this->ResumeFrom, SIGNAL(currentIndexChanged(const QString&)), 
    this->ResumeFromC2S, SLOT(setText(const QString&)), Qt::QueuedConnection);
  QObject::connect(this->CollarLocation, SIGNAL(currentIndexChanged(const int)), 
    this, SLOT(CollarLocationChanged(const int)), Qt::QueuedConnection);

  //make sure the CollarFile box is in the right state
  this->CollarLocationChanged(this->CollarLocation->currentIndex());

  //make sure the ResumeFrom combo box is enabled/disabled correctly
  QObject::connect(this->ResumePreviousRun, SIGNAL(toggled(bool)),
    this->ResumeFrom, SLOT(setEnabled(bool)), Qt::QueuedConnection);
  this->ResumeFrom->setEnabled(this->ResumePreviousRun->isChecked());

  //make sure the hill climbing boxes are enabled/disabled correctly
  QObject::connect(this->HCLength, SIGNAL(toggled(bool)),
    this->HCLengthAmount, SLOT(setEnabled(bool)), Qt::QueuedConnection);
  this->HCLengthAmount->setEnabled(this->HCLength->isChecked());

  QObject::connect(this->HCDirection, SIGNAL(toggled(bool)),
    this->HCDirectionAmountAz, SLOT(setEnabled(bool)), Qt::QueuedConnection);
  this->HCDirectionAmountAz->setEnabled(this->HCDirection->isChecked());

  QObject::connect(this->HCDirection, SIGNAL(toggled(bool)),
    this->HCDirectionAmountDip, SLOT(setEnabled(bool)), Qt::QueuedConnection);
  this->HCDirectionAmountDip->setEnabled(this->HCDirection->isChecked());

  //connect the refresh button to the handler slot
  QObject::connect(this->RefreshButton, SIGNAL(clicked(bool)),
    this, SLOT(OnRefreshButtonClicked(bool)), Qt::QueuedConnection);

  //You have to call this, I assume it does what it says
  this->linkServerManagerProperties();
}

void pqDrillholeGA::accept()
{
  //if they click accept, uncheck the RefreshPlease box
  this->RefreshPlease->setChecked(false);
	pqLoadedFormObjectPanel::accept();
}


void pqDrillholeGA::CollarLocationChanged(const int index)
  {
  this->findChild<QLineEdit*>("CollarFile")->setEnabled(index == 0);
  }

void pqDrillholeGA::OnRefreshButtonClicked(bool)
  {
  // Leave the triple slashes (///) below so they show up in doxygen
  /// Important Implementation Details:
  /// Set the <code>RefreshPlease</code> checkbox to 'checked' to modify the <code>RefreshPlease</code> server property
  this->RefreshPlease->setChecked(true);
  /// Force the <code>Refresh</code> server property to be modified, which will cause <code>vtkDrillholeGA::DoSomething()</code> to be called on the server side.
  vtkSMSourceProxy* sp = vtkSMSourceProxy::SafeDownCast(this->proxy());
  vtkSMProperty *prop = sp->GetProperty("Refresh");
  prop->Modified();
  sp->UpdatePipeline();
  /// Call <code>pqLoadedFormObjectPanel::accept()</code> directly, not <code>this->accept()</code>
  pqLoadedFormObjectPanel::accept();
  /// Force the View Manager to <code>render()</code> so there are no delayed updates.
  pqApplicationCore::instance()->render();
  /// Fill the <code>ResumeFrom</code> <code>QComboBox</code>. 
  /// Note: This is going to cause the Apply button to be green again because the currentIndex of the combo box will change.
  this->FillComboBox();
  }

QString pqDrillholeGA::FillComboBox()
  {
  //Retrieve information FROM the server
  vtkSMStringVectorProperty* ResumeFromS2C = vtkSMStringVectorProperty::SafeDownCast(
		this->proxy()->GetProperty("ResumeFromS2C"));
  QString line(ResumeFromS2C->GetElement(0));

  //Act on the information received FROM the server. in my case I am filling a combo box 
  //with values obtained by splitting the server property with the character |
  //e.g. value1|value2|value3|...
  this->ResumeFrom->clear();
  QStringList list = line.split("|", QString::SkipEmptyParts);
  foreach( QString s, list)
    {
    this->ResumeFrom->addItem(s);
    }
  if (list.size() > 0)
    {
    return list.at(0);
    }
  else
    {
    return QString("");
    }
  }