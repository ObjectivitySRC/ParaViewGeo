/*=========================================================================

   Program: ParaView
   Module:    pqAnimationViewWidget.cxx

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

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

=========================================================================*/

#include "pqAnimationViewWidget.h"
#include "ui_pqPythonAnimationCue.h"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPointer>
#include <QPushButton>
#include <QSignalMapper>
#include <QSpinBox>
#include <QtDebug>
#include <QVBoxLayout>

#include "pqActiveView.h"
#include "pqAnimatablePropertiesComboBox.h"
#include "pqAnimatableProxyComboBox.h"
#include "pqAnimationCue.h"
#include "pqAnimationKeyFrame.h"
#include "pqAnimationModel.h"
#include "pqAnimationScene.h"
#include "pqAnimationTrack.h"
#include "pqAnimationWidget.h"
#include "pqApplicationCore.h"
#include "pqComboBoxDomain.h"
#include "pqKeyFrameEditor.h"
#include "pqOrbitCreatorDialog.h"
#include "pqPipelineTimeKeyFrameEditor.h"
#include "pqPropertyLinks.h"
#include "pqRenderView.h"
#include "pqServer.h"
#include "pqServerManagerModel.h"
#include "pqServerManagerSelectionModel.h"
#include "pqSetName.h"
#include "pqSignalAdaptors.h"
#include "pqSMAdaptor.h"
#include "pqUndoStack.h"
#include "vtkCamera.h"
#include "vtkPVConfig.h"
#include "vtkSMProperty.h"
#include "vtkSMPropertyHelper.h"
#include "vtkSMProxy.h"
#include "vtkSMRenderViewProxy.h"

//-----------------------------------------------------------------------------
class pqAnimationViewWidget::pqInternal
{
public:
  pqInternal()
    {
    }
  ~pqInternal()
    {
    }

  QPointer<pqAnimationScene> Scene;
  pqAnimationWidget* AnimationWidget;
  QSignalMapper KeyFramesChanged;
  typedef QMap<QPointer<pqAnimationCue>, pqAnimationTrack*> TrackMapType;
  TrackMapType TrackMap;
  QPointer<QDialog> Editor;
  QComboBox* PlayMode;
  QLineEdit* Time;
  QLineEdit* StartTime;
  QLineEdit* EndTime;
  QLabel* DurationLabel;
  QSpinBox* Duration;
  pqPropertyLinks Links;
  pqPropertyLinks DurationLink;
  pqAnimatableProxyComboBox* CreateSource;
  pqAnimatablePropertiesComboBox* CreateProperty;
  QPushButton* LockEndTime;
  QPushButton* LockStartTime;

  pqAnimationTrack* findTrack(pqAnimationCue* cue)
    {
    TrackMapType::iterator iter;
    iter = this->TrackMap.find(cue);
    if(iter != this->TrackMap.end())
      {
      return iter.value();
      }
    return NULL;
    }
  pqAnimationCue* findCue(pqAnimationTrack* track)
    {
    TrackMapType::iterator iter;
    for(iter = this->TrackMap.begin(); iter != this->TrackMap.end(); ++iter)
      {
      if(iter.value() == track)
        {
        return iter.key();
        }
      }
    return NULL;
    }
  QString cueName(pqAnimationCue* cue)
    {
    QString name;
    if(this->cameraCue(cue))
      {
      name = "Camera";
      }
    else if (this->pythonCue(cue))
      {
      name = "Python";
      }
    else
      {
      pqServerManagerModel* model = 
        pqApplicationCore::instance()-> getServerManagerModel();
      
      vtkSMProxy* pxy = cue->getAnimatedProxy();
      vtkSMProperty* pty = cue->getAnimatedProperty();
      QString p = pty->GetXMLLabel();
      if(pqSMAdaptor::getPropertyType(pty) == pqSMAdaptor::MULTIPLE_ELEMENTS)
        {
        p = QString("%1 (%2)").arg(p).arg(cue->getAnimatedPropertyIndex());
        }
      
      QList<pqProxy*> pxys = model->findItems<pqProxy*>();
      for(int i=0; i<pxys.size(); i++)
        {
        if(pxys[i]->getProxy() == pxy)
          {
          QString n = pxys[i]->getSMName();
          name = QString("%1 - %2").arg(n).arg(p);
          }
        }
      
      // could be a helper proxy
      for(int i=0; i<pxys.size(); i++)
        {
        pqProxy* pqproxy = pxys[i];
        QList<QString> keys = pqproxy->getHelperKeys();
        for(int j=0; j<keys.size(); j++)
          {
          QString key = keys[j];
          QList<vtkSMProxy*> helpers = pqproxy->getHelperProxies(keys[j]);
          int idx = helpers.indexOf(pxy);
          if(idx != -1)
            {
            vtkSMProperty* prop =
              pqproxy->getProxy()->GetProperty(key.toAscii().data());
            QString n = pqproxy->getSMName();
            if (prop)
              {
              QString pp = prop->GetXMLLabel();
              name = QString("%1 - %2 - %3").arg(n).arg(pp).arg(p);
              }
            else
              {
              name = QString("%1 - %2").arg(n).arg(p);
              if (helpers.size() > 0)
                {
                name = QString("%1 [%2]").arg(name).arg(idx);
                }
              }
            }
          }
        }
      }
    return name;
    }
  // returns if this is a cue for animating a camera
  bool cameraCue(pqAnimationCue* cue)
    {
    if(QString("CameraAnimationCue") == cue->getProxy()->GetXMLName())
      {
      return true;
      }
    return false;
    }

  /// returns true if the cue is a python cue.
  bool pythonCue(pqAnimationCue* cue)
    {
    if(QString("PythonAnimationCue") == cue->getProxy()->GetXMLName())
      {
      return true;
      }
    return false;
    }

  int numberOfTicks()
    {
    vtkSMProxy* pxy = this->Scene->getProxy();
    QString mode =
      pqSMAdaptor::getEnumerationProperty(pxy->GetProperty("PlayMode")).toString();

    int num = 0;
    
    if(mode == "Sequence")
      {
      num = 
        pqSMAdaptor::getElementProperty(
          pxy->GetProperty("NumberOfFrames")).toInt();
      }
    else if(mode == "Snap To TimeSteps")
      {
      num = this->Scene->getTimeSteps().size(); 
      }
    return num;
    }

  QList<double> ticks()
    {
    vtkSMProxy* pxy = this->Scene->getProxy();
    QString mode =
      pqSMAdaptor::getEnumerationProperty(pxy->GetProperty("PlayMode")).toString();
    if(mode == "Snap To TimeSteps")
      {
      return this->Scene->getTimeSteps(); 
      }
    return QList<double>();
    }
};

//-----------------------------------------------------------------------------
pqAnimationViewWidget::pqAnimationViewWidget(QWidget* _parent) : QWidget(_parent)
{
  this->Internal = new pqAnimationViewWidget::pqInternal();
  QVBoxLayout* vboxlayout = new QVBoxLayout(this);
  QHBoxLayout* hboxlayout = new QHBoxLayout;
  vboxlayout->addLayout(hboxlayout);
  hboxlayout->setMargin(0);

  hboxlayout->addWidget(new QLabel("Mode:", this));
  this->Internal->PlayMode = new QComboBox(this) << pqSetName("PlayMode");
  this->Internal->PlayMode->addItem("Snap to Timesteps");
  hboxlayout->addWidget(this->Internal->PlayMode);
  hboxlayout->addWidget(new QLabel("Time:", this));
  this->Internal->Time = new QLineEdit(this) << pqSetName("Time");
  this->Internal->Time->setValidator(
    new QDoubleValidator(this->Internal->Time));
  hboxlayout->addWidget(this->Internal->Time);
  hboxlayout->addWidget(new QLabel("Start Time:", this));
  this->Internal->StartTime = new QLineEdit(this) << pqSetName("StartTime");
  this->Internal->StartTime->setValidator(
    new QDoubleValidator(this->Internal->StartTime));
  hboxlayout->addWidget(this->Internal->StartTime);
  this->Internal->LockStartTime = new QPushButton(this) <<
    pqSetName("LockStartTime");
  this->Internal->LockStartTime->setIcon(QIcon(":pqWidgets/Icons/pqLock24.png"));
  this->Internal->LockStartTime->setToolTip(
    "<html>Lock the start time to keep ParaView from changing it "
    "as available data times change</html>");
  this->Internal->LockStartTime->setStatusTip(
    "<html>Lock the start time to keep ParaView from changing it "
    "as available data times change</html>");
  this->Internal->LockStartTime->setCheckable(true);
  hboxlayout->addWidget(this->Internal->LockStartTime);
  hboxlayout->addWidget(new QLabel("End Time:", this));
  this->Internal->EndTime = new QLineEdit(this) << pqSetName("EndTime");
  this->Internal->EndTime->setValidator(
    new QDoubleValidator(this->Internal->EndTime));
  hboxlayout->addWidget(this->Internal->EndTime);
  this->Internal->LockEndTime = new QPushButton(this) <<
    pqSetName("LockEndTime");
  this->Internal->LockEndTime->setIcon(QIcon(":pqWidgets/Icons/pqLock24.png"));
  this->Internal->LockEndTime->setToolTip(
    "<html>Lock the end time to keep ParaView from changing it"
    " as available data times change</html>");
  this->Internal->LockEndTime->setStatusTip(
    "<html>Lock the end time to keep ParaView from changing it"
    " as available data times change</html>");
  this->Internal->LockEndTime->setCheckable(true);
  hboxlayout->addWidget(this->Internal->LockEndTime);
  this->Internal->DurationLabel = new QLabel(this);
  hboxlayout->addWidget(this->Internal->DurationLabel);
  this->Internal->Duration = new QSpinBox(this) << pqSetName("Duration");
  this->Internal->Duration->setRange(1, (int)(~0u >> 1));
  hboxlayout->addWidget(this->Internal->Duration);
  hboxlayout->addStretch();

  this->Internal->AnimationWidget = new pqAnimationWidget(this)
    << pqSetName("pqAnimationWidget");
  this->Internal->AnimationWidget->animationModel()->setInteractive(true);
  QWidget* w = this->Internal->AnimationWidget->createDeleteWidget();

  this->Internal->CreateSource = new pqAnimatableProxyComboBox(w)
    << pqSetName("ProxyCombo");
#ifdef PARAVIEW_ENABLE_PYTHON
  this->Internal->CreateSource->addProxy(0, "Python", NULL);
#endif
  this->Internal->CreateProperty = new pqAnimatablePropertiesComboBox(w)
    << pqSetName("PropertyCombo");
  this->Internal->CreateSource->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  this->Internal->CreateProperty->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  QHBoxLayout* l = new QHBoxLayout(w);
  l->setMargin(0);
  l->addSpacing(6);
  l->addWidget(this->Internal->CreateSource);
  l->addWidget(this->Internal->CreateProperty);
  l->addStretch();

  QObject::connect(&this->Internal->KeyFramesChanged, SIGNAL(mapped(QObject*)),
                   this, SLOT(keyFramesChanged(QObject*)));
  QObject::connect(this->Internal->AnimationWidget,
                   SIGNAL(trackSelected(pqAnimationTrack*)),
                   this, SLOT(trackSelected(pqAnimationTrack*)));
  QObject::connect(this->Internal->AnimationWidget,
                   SIGNAL(deleteTrackClicked(pqAnimationTrack*)),
                   this, SLOT(deleteTrack(pqAnimationTrack*)));
  QObject::connect(this->Internal->AnimationWidget,
                   SIGNAL(enableTrackClicked(pqAnimationTrack*)),
                   this, SLOT(toggleTrackEnabled(pqAnimationTrack*)));
  QObject::connect(this->Internal->AnimationWidget,
                   SIGNAL(createTrackClicked()),
                   this, SLOT(createTrack()));
  
  QObject::connect(this->Internal->AnimationWidget->animationModel(),
                   SIGNAL(currentTimeSet(double)),
                   this, SLOT(setCurrentTime(double)));
  QObject::connect(this->Internal->AnimationWidget->animationModel(),
                   SIGNAL(keyFrameTimeChanged(pqAnimationTrack*, pqAnimationKeyFrame*, int, double)),
                   this, SLOT(setKeyFrameTime(pqAnimationTrack*, pqAnimationKeyFrame*, int, double)));
  
  QObject::connect(&pqActiveView::instance(),
    SIGNAL(changed(pqView*)),
    this, SLOT(setActiveView(pqView*)));
  
  QObject::connect(pqApplicationCore::instance()->getSelectionModel(),
    SIGNAL(currentChanged(pqServerManagerModelItem*)),
    this, SLOT(setCurrentSelection(pqServerManagerModelItem*)));
  
  QObject::connect(this->Internal->CreateSource,
    SIGNAL(currentProxyChanged(vtkSMProxy*)),
    this, SLOT(setCurrentProxy(vtkSMProxy*)));

  vboxlayout->addWidget(this->Internal->AnimationWidget);
}

//-----------------------------------------------------------------------------
pqAnimationViewWidget::~pqAnimationViewWidget()
{
  delete this->Internal;
}

//-----------------------------------------------------------------------------
void pqAnimationViewWidget::setScene(pqAnimationScene* scene)
{
  if(this->Internal->Scene)
    {
    this->Internal->Links.removeAllPropertyLinks();
    QObject::disconnect(this->Internal->Scene, 0, this, 0);
    
    pqComboBoxDomain* d0 =
      this->Internal->PlayMode->findChild<pqComboBoxDomain*>("ComboBoxDomain");
    if(d0)
      {
      delete d0;
      }
    pqSignalAdaptorComboBox* adaptor = 
      this->Internal->PlayMode->findChild<pqSignalAdaptorComboBox*>("ComboBoxAdaptor");
    if(adaptor)
      {
      delete adaptor;
      }
    }
  this->Internal->Scene = scene;
  if(this->Internal->Scene)
    {
    pqComboBoxDomain* d0 = new pqComboBoxDomain(this->Internal->PlayMode,
      scene->getProxy()->GetProperty("PlayMode"));
    d0->setObjectName("ComboBoxDomain");
    pqSignalAdaptorComboBox* adaptor = 
      new pqSignalAdaptorComboBox(this->Internal->PlayMode);
    adaptor->setObjectName("ComboBoxAdaptor");
    this->Internal->Links.addPropertyLink(adaptor, "currentText",
      SIGNAL(currentTextChanged(const QString&)), scene->getProxy(),
      scene->getProxy()->GetProperty("PlayMode"));
   
    // connect time 
    this->Internal->Links.addPropertyLink(this->Internal->Time, "text",
      SIGNAL(editingFinished()), scene->getProxy(),
      scene->getProxy()->GetProperty("AnimationTime"));
    // connect start time 
    this->Internal->Links.addPropertyLink(this->Internal->StartTime, "text",
      SIGNAL(editingFinished()), scene->getProxy(),
      scene->getProxy()->GetProperty("StartTime"));
    // connect end time 
    this->Internal->Links.addPropertyLink(this->Internal->EndTime, "text",
      SIGNAL(editingFinished()), scene->getProxy(),
      scene->getProxy()->GetProperty("EndTime"));
    // connect lock start time.
    this->Internal->Links.addPropertyLink(this->Internal->LockStartTime,
      "checked", SIGNAL(toggled(bool)), scene->getProxy(),
      scene->getProxy()->GetProperty("LockStartTime"));
    this->Internal->Links.addPropertyLink(this->Internal->LockEndTime,
      "checked", SIGNAL(toggled(bool)), scene->getProxy(),
      scene->getProxy()->GetProperty("LockEndTime"));

    QObject::connect(scene, SIGNAL(cuesChanged()), 
      this, SLOT(onSceneCuesChanged()));
    QObject::connect(scene, SIGNAL(clockTimeRangesChanged()),
            this, SLOT(updateSceneTimeRange()));
    QObject::connect(scene, SIGNAL(timeStepsChanged()),
            this, SLOT(updateTicks()));
    QObject::connect(scene, SIGNAL(frameCountChanged()),
            this, SLOT(updateTicks()));
    QObject::connect(scene, SIGNAL(animationTime(double)),
            this, SLOT(updateSceneTime()));
    QObject::connect(scene, SIGNAL(playModeChanged()), 
      this, SLOT(updatePlayMode()));
    QObject::connect(scene, SIGNAL(playModeChanged()), 
      this, SLOT(updateTicks()));
    QObject::connect(scene, SIGNAL(playModeChanged()), 
      this, SLOT(updateSceneTime()));
    this->updateSceneTimeRange();
    this->updateSceneTime();
    this->updatePlayMode();
    this->updateTicks();
    }
}

//-----------------------------------------------------------------------------
void pqAnimationViewWidget::onSceneCuesChanged()
{
  QSet<pqAnimationCue*> cues = this->Internal->Scene->getCues();
  pqAnimationModel* animModel =
    this->Internal->AnimationWidget->animationModel();
    
  pqInternal::TrackMapType oldCues = this->Internal->TrackMap;
  pqInternal::TrackMapType::iterator iter;

  // add new tracks
  foreach(pqAnimationCue* cue, cues)
    {
    QString completeName = this->Internal->cueName(cue);

    iter = this->Internal->TrackMap.find(cue);

    if(iter == this->Internal->TrackMap.end())
      {
      pqAnimationTrack* track = animModel->addTrack();
      if(completeName.startsWith("TimeKeeper"))
        {
        track->setDeletable(false);
        }
      this->Internal->TrackMap.insert(cue, track);
      track->setProperty(completeName);
      this->Internal->KeyFramesChanged.setMapping(cue, cue);
      QObject::connect(cue, SIGNAL(keyframesModified()),
        &this->Internal->KeyFramesChanged,
        SLOT(map()));
      QObject::connect(cue, SIGNAL(enabled(bool)),
        track, SLOT(setEnabled(bool)));
      track->setEnabled(cue->isEnabled());

      // this ensures that the already present keyframes are loaded currently
      // (which happens when loading state files).
      this->keyFramesChanged(cue);
      }
    else
      {
      oldCues.remove(cue);
      }
    }

  // remove old tracks
  for(iter = oldCues.begin(); iter != oldCues.end(); iter++)
    {
    animModel->removeTrack(iter.value());
    this->Internal->TrackMap.remove(iter.key());
    if(iter.key())
      {
      QObject::disconnect(iter.key(), SIGNAL(keyframesModified()),
        &this->Internal->KeyFramesChanged,
        SLOT(map()));
      }
    }

}
  
//-----------------------------------------------------------------------------
void pqAnimationViewWidget::keyFramesChanged(QObject* cueObject)
{
  pqAnimationCue* cue = qobject_cast<pqAnimationCue*>(cueObject);
  pqAnimationTrack* track = this->Internal->findTrack(cue);

  QList<vtkSMProxy*> keyFrames = cue->getKeyFrames();

  bool camera = this->Internal->cameraCue(cue);

  // clean out old ones
  while(track->count())
    {
    track->removeKeyFrame(track->keyFrame(0));
    }

  for(int j=0; j<keyFrames.count()-1; j++)
    {
    QIcon icon;
    QVariant startValue;
    QVariant endValue;
      
    double startTime =
      pqSMAdaptor::getElementProperty(keyFrames[j]->GetProperty("KeyTime")).toDouble();
    double endTime =
      pqSMAdaptor::getElementProperty(keyFrames[j+1]->GetProperty("KeyTime")).toDouble();

    if(!camera)
      {
      QVariant interpolation =
        pqSMAdaptor::getEnumerationProperty(keyFrames[j]->GetProperty("Type"));
      if(interpolation == "Boolean")
        interpolation = "Step";
      else if(interpolation == "Sinusoid")
        interpolation = "Sinusoidal";
      QString iconstr =
        QString(":pqWidgets/Icons/pq%1%2.png").arg(interpolation.toString()).arg(16);
      icon = QIcon(iconstr);
      
      startValue =
        pqSMAdaptor::getElementProperty(keyFrames[j]->GetProperty("KeyValues"));
      endValue =
        pqSMAdaptor::getElementProperty(keyFrames[j+1]->GetProperty("KeyValues"));
      }

    pqAnimationKeyFrame* newFrame = track->addKeyFrame();
    newFrame->setNormalizedStartTime(startTime);
    newFrame->setNormalizedEndTime(endTime);
    newFrame->setStartValue(startValue);
    newFrame->setEndValue(endValue);
    newFrame->setIcon(QIcon(icon));
    }
}

//-----------------------------------------------------------------------------
void pqAnimationViewWidget::updateSceneTimeRange()
{
  pqAnimationModel* animModel =
    this->Internal->AnimationWidget->animationModel();
  QPair<double, double> timeRange = this->Internal->Scene->getClockTimeRange();
  animModel->setStartTime(timeRange.first);
  animModel->setEndTime(timeRange.second);
}

//-----------------------------------------------------------------------------
void pqAnimationViewWidget::updateSceneTime()
{
  double time =
    this->Internal->Scene->getAnimationTime();

  pqAnimationModel* animModel =
    this->Internal->AnimationWidget->animationModel();
  animModel->setCurrentTime(time);
}

//-----------------------------------------------------------------------------
void pqAnimationViewWidget::setCurrentTime(double t)
{
  vtkSMPropertyHelper(
    this->Internal->Scene->getProxy(), "AnimationTime").Set(t);
  this->Internal->Scene->getProxy()->UpdateVTKObjects();
}
  
//-----------------------------------------------------------------------------
void pqAnimationViewWidget::setKeyFrameTime(pqAnimationTrack* track, 
                                            pqAnimationKeyFrame* kf, 
                                            int edge, double time)
{
  pqAnimationCue* cue = this->Internal->findCue(track);
  if(!cue)
    {
    return;
    }
  QList<vtkSMProxy*> keyFrames = cue->getKeyFrames();
  int i = 0;
  for(i=0; i<track->count(); i++)
    {
    if(track->keyFrame(i) == kf)
      {
      break;
      }
    }
  if(edge)
    {
    i++;
    }

  if(i < keyFrames.size())
    {
    QPair<double, double> timeRange = this->Internal->Scene->getClockTimeRange();
    double normTime = (time - timeRange.first) / (timeRange.second - timeRange.first);
    pqSMAdaptor::setElementProperty(keyFrames[i]->GetProperty("KeyTime"), 
                                    normTime);
    keyFrames[i]->UpdateVTKObjects();
    }
}

//-----------------------------------------------------------------------------
void pqAnimationViewWidget::trackSelected(pqAnimationTrack* track)
{
  pqAnimationCue* cue = this->Internal->findCue(track);
  if(!cue)
    {
    return;
    }

  if(this->Internal->Editor)
    {
    this->Internal->Editor->raise();
    return;
    }

  if(track->property().toString().startsWith("TimeKeeper"))
    {
    this->Internal->Editor = 
      new pqPipelineTimeKeyFrameEditor(this->Internal->Scene, cue, NULL);
    }
  else if (this->Internal->pythonCue(cue))
    {
    QDialog dialog(this);
    Ui::PythonAnimationCue ui;
    ui.setupUi(&dialog);
    ui.script->setPlainText(
      vtkSMPropertyHelper(cue->getProxy(), "Script").GetAsString());
    if (dialog.exec() == QDialog::Accepted)
      {
      vtkSMPropertyHelper(cue->getProxy(), "Script").Set(
        ui.script->toPlainText().toAscii().data());
      cue->getProxy()->UpdateVTKObjects();
      }
    return;
    }
  else
    {
    this->Internal->Editor = new QDialog;
    QVBoxLayout* l = new QVBoxLayout(this->Internal->Editor);
    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok
                                                | QDialogButtonBox::Cancel);
    pqKeyFrameEditor* editor = new pqKeyFrameEditor(this->Internal->Scene, cue,
                        QString("Editing ") + this->Internal->cueName(cue),
                        this->Internal->Editor);

    l->addWidget(editor);
    l->addWidget(buttons);
    
    connect(buttons, SIGNAL(accepted()), 
            this->Internal->Editor, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), 
            this->Internal->Editor, SLOT(reject()));
    connect(this->Internal->Editor, SIGNAL(accepted()), 
            editor, SLOT(writeKeyFrameData()));
    }
  
  this->Internal->Editor->setWindowTitle(tr("Animation Keyframes"));
  this->Internal->Editor->setAttribute(Qt::WA_QuitOnClose, false);
  this->Internal->Editor->setAttribute(Qt::WA_DeleteOnClose);
  

  this->Internal->Editor->resize(600, 400);
  this->Internal->Editor->show();
}
  
//-----------------------------------------------------------------------------
void pqAnimationViewWidget::updatePlayMode()
{
  pqAnimationModel* animModel =
    this->Internal->AnimationWidget->animationModel();
  vtkSMProxy* pxy = this->Internal->Scene->getProxy();

  QString mode = pqSMAdaptor::getEnumerationProperty(
    pxy->GetProperty("PlayMode")).toString();
    
  this->Internal->DurationLink.removeAllPropertyLinks();

  if(mode == "Real Time")
    {
    animModel->setMode(pqAnimationModel::Real);
    this->Internal->StartTime->setEnabled(true);
    this->Internal->EndTime->setEnabled(true);
    this->Internal->Time->setEnabled(true);
    this->Internal->Duration->setEnabled(true);
    this->Internal->DurationLabel->setEnabled(true);
    this->Internal->DurationLabel->setText("Duration:");
    this->Internal->DurationLink.addPropertyLink(
      this->Internal->Duration, "value",
      SIGNAL(valueChanged(int)), this->Internal->Scene->getProxy(),
      this->Internal->Scene->getProxy()->GetProperty("Duration"));
    }
  else if(mode == "Sequence")
    {
    animModel->setMode(pqAnimationModel::Sequence);
    this->Internal->StartTime->setEnabled(true);
    this->Internal->EndTime->setEnabled(true);
    this->Internal->Time->setEnabled(true);
    this->Internal->Duration->setEnabled(true);
    this->Internal->DurationLabel->setEnabled(true);
    this->Internal->DurationLabel->setText("No. Frames:");
    this->Internal->DurationLink.addPropertyLink(
      this->Internal->Duration, "value",
      SIGNAL(valueChanged(int)), this->Internal->Scene->getProxy(),
      this->Internal->Scene->getProxy()->GetProperty("NumberOfFrames"));
    }
  else if(mode == "Snap To TimeSteps")
    {
    animModel->setMode(pqAnimationModel::Custom);
    this->Internal->Duration->setEnabled(false);
    this->Internal->DurationLabel->setEnabled(false);
    this->Internal->StartTime->setEnabled(false);
    this->Internal->EndTime->setEnabled(false);
    this->Internal->Time->setEnabled(false);
    }
  else
    {
    qWarning("Unrecognized play mode");
    }

}
  
//-----------------------------------------------------------------------------
void pqAnimationViewWidget::updateTicks()
{
  pqAnimationModel* animModel =
    this->Internal->AnimationWidget->animationModel();
  if (animModel->mode() == pqAnimationModel::Custom)
    {
    QList<double> ticks = this->Internal->ticks();
    double *dticks = new double[ticks.size()+1];
    for (int cc=0; cc < ticks.size(); cc++)
      {
      dticks[cc] = ticks[cc];
      }
    animModel->setTickMarks(ticks.size(), dticks);
    delete [] dticks;
    }
  else
    {
    animModel->setTicks(this->Internal->numberOfTicks());
    }
}

//-----------------------------------------------------------------------------
void pqAnimationViewWidget::toggleTrackEnabled(pqAnimationTrack* track)
{
  pqAnimationCue* cue = this->Internal->findCue(track);
  if(!cue)
    {
    return;
    }
  BEGIN_UNDO_SET("Toggle Animation Track");
  cue->setEnabled(!track->isEnabled());
  END_UNDO_SET();
}

//-----------------------------------------------------------------------------
void pqAnimationViewWidget::deleteTrack(pqAnimationTrack* track)
{
  pqAnimationCue* cue = this->Internal->findCue(track);
  if(!cue)
    {
    return;
    }
  BEGIN_UNDO_SET("Remove Animation Track");
  this->Internal->Scene->removeCue(cue);
  END_UNDO_SET();
}

//-----------------------------------------------------------------------------
void pqAnimationViewWidget::setActiveView(pqView* view)
{
  pqRenderView* rview = qobject_cast<pqRenderView*>(view);
  this->Internal->CreateSource->removeProxy("Camera");
  if (rview && this->Internal->CreateSource->findText("Camera") == -1)
    {
    this->Internal->CreateSource->addProxy(0, "Camera", rview->getProxy());
    }
}

//-----------------------------------------------------------------------------
void pqAnimationViewWidget::setCurrentSelection(pqServerManagerModelItem* item)
{
  pqProxy* pxy = qobject_cast<pqProxy*>(item);
  if(pxy)
    {
    int idx = this->Internal->CreateSource->findProxy(pxy->getProxy());
    if(idx != -1)
      {
      this->Internal->CreateSource->setCurrentIndex(idx);
      }
    }
}

//-----------------------------------------------------------------------------
void pqAnimationViewWidget::setCurrentProxy(vtkSMProxy* pxy)
{
  if(vtkSMRenderViewProxy::SafeDownCast(pxy))
    {
    this->Internal->CreateProperty->setSourceWithoutProperties(pxy);
    // add camera animation modes as properties for creating the camera
    // animation track.
    this->Internal->CreateProperty->addSMProperty(
      "Orbit", "orbit", 0);
    this->Internal->CreateProperty->addSMProperty(
      "Follow Path", "path", 0);
    this->Internal->CreateProperty->addSMProperty(
      "Interpolate camera locations", "camera", 0);
    }
  else
    {
    this->Internal->CreateProperty->setSource(pxy);
    }
}

//-----------------------------------------------------------------------------
void pqAnimationViewWidget::createTrack()
{
  vtkSMRenderViewProxy* ren =
    vtkSMRenderViewProxy::SafeDownCast(this->Internal->CreateSource->getCurrentProxy());
  // Need to create new cue for this property.
  vtkSMProxy* curProxy = this->Internal->CreateProperty->getCurrentProxy();
  QString pname = this->Internal->CreateProperty->getCurrentPropertyName();
  int pindex = this->Internal->CreateProperty->getCurrentIndex();

  // used for camera tracks.
  QString mode = this->Internal->CreateProperty->getCurrentPropertyName();

  if (ren)
    {
    curProxy = ren;
    pname = QString();
    pindex = 0;
    }

  if(!curProxy)
    {
    // curProxy == NULL is only used for "Python" track for now. Of course,
    // we only support that when python is enabled.
    // we allow creating as many python tracks as needed, hence we don't check
    // if there exists a track already (which is the case with others).
#ifdef PARAVIEW_ENABLE_PYTHON
    this->createPythonTrack();
#endif
    return;
    }

  // check that we don't already have one
  foreach(pqAnimationCue* cue, this->Internal->TrackMap.keys())
    {
    if (cue->getAnimatedProxy() == NULL)
      {
      continue; // skip Python tracks.
      }
    if(cue->getAnimatedProxy() == curProxy &&
       cue->getAnimatedProxy()->GetPropertyName(cue->getAnimatedProperty()) == pname &&
       cue->getAnimatedPropertyIndex() == pindex)
      {
      return;
      }
    }
  
  pqOrbitCreatorDialog creator(this);

  // if mode=="orbit" show up a dialog allowing the user to customize the
  // orbit.
  if (ren && mode == "orbit")
    {
    creator.setNormal( ren->GetActiveCamera()->GetViewUp() );
    creator.setOrigin( ren->GetActiveCamera()->GetPosition() );
    if (creator.exec() != QDialog::Accepted)
      {
      return;
      }
    }

  BEGIN_UNDO_SET("Add Animation Track");

  // This will create the cue and initialize it with default keyframes.
  pqAnimationCue* cue = this->Internal->Scene->createCue(curProxy,
    pname.toAscii().data(), pindex, ren? "CameraAnimationCue" :
    "KeyFrameAnimationCue");

  if (ren)
    {
    if (mode=="path" || mode =="orbit")
      {
      // Setup default animation to revolve around the selected objects (if any)
      // in a plane normal to the current view-up vector.
      pqSMAdaptor::setElementProperty(
        cue->getProxy()->GetProperty("Mode"), 1); // PATH-based animation.
      }
    else
      {
      pqSMAdaptor::setElementProperty(
        cue->getProxy()->GetProperty("Mode"), 0); // non-PATH-based animation.
      }
    cue->getProxy()->UpdateVTKObjects();

    if (mode == "orbit")
      {
      // update key frame paramters based on the orbit points.
      vtkSMProxy* kf = cue->getKeyFrame(0);
      pqSMAdaptor::setMultipleElementProperty(
        kf->GetProperty("PositionPathPoints"),
        creator.orbitPoints(7));
      pqSMAdaptor::setMultipleElementProperty(
        kf->GetProperty("FocalPathPoints"),
        creator.center());
      pqSMAdaptor::setElementProperty(
        kf->GetProperty("ClosedPositionPath"), 1);
      kf->UpdateVTKObjects();
      }
    }

  END_UNDO_SET();
}

//-----------------------------------------------------------------------------
void pqAnimationViewWidget::createPythonTrack()
{
#ifdef PARAVIEW_ENABLE_PYTHON
  BEGIN_UNDO_SET("Add Animation Track");

  pqAnimationCue* cue = this->Internal->Scene->createCue("PythonAnimationCue");
  Q_ASSERT(cue != NULL);
  (void)cue;
  END_UNDO_SET();
#else
  qCritical() << "Python support not enabled. Please recompile ParaView "
    "with Python enabled.";
#endif
}
