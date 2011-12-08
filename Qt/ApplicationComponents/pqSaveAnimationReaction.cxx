/*=========================================================================

   Program: ParaView
   Module:    pqSaveAnimationReaction.cxx

   Copyright (c) 2005,2006 Sandia Corporation, Kitware Inc.
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

========================================================================*/
#include "pqSaveAnimationReaction.h"

#include "pqActiveObjects.h"
#include "pqAnimationManager.h"
#include "pqPVApplicationCore.h"

#include "vtkPVConfig.h"
#ifdef PARAVIEW_ENABLE_PYTHON
#include "pqPythonManager.h"
#include "pqPythonDialog.h"
#include "pqPythonShell.h"
#endif

#include <QDebug>

//-----------------------------------------------------------------------------
pqSaveAnimationReaction::pqSaveAnimationReaction(QAction* parentObject)
  : Superclass(parentObject)
{
  // load state enable state depends on whether we are connected to an active
  // server or not and whether
  pqActiveObjects* activeObjects = &pqActiveObjects::instance();
  QObject::connect(activeObjects, SIGNAL(serverChanged(pqServer*)),
    this, SLOT(updateEnableState()));
  this->updateEnableState();
}

//-----------------------------------------------------------------------------
void pqSaveAnimationReaction::updateEnableState()
{
  pqActiveObjects* activeObjects = &pqActiveObjects::instance();
  bool is_enabled = (activeObjects->activeServer() != NULL);
  this->parentAction()->setEnabled(is_enabled);
}

//-----------------------------------------------------------------------------
void pqSaveAnimationReaction::saveAnimation()
{
  pqAnimationManager* mgr = pqPVApplicationCore::instance()->animationManager();
  if (!mgr || !mgr->getActiveScene())
    {
    qDebug() << "Cannot save animation since no active scene is present.";
    return;
    }

  QObject::connect(mgr, SIGNAL(writeAnimation(const QString&, int, double)),
    this, SLOT(onWriteAnimation(const QString&, int, double)));
  mgr->saveAnimation();
  QObject::disconnect(mgr, SIGNAL(writeAnimation(const QString&, int, double)),
    this, SLOT(onWriteAnimation(const QString&, int, double)));
#ifdef PARAVIEW_ENABLE_PYTHON
  pqPythonManager* manager = pqPVApplicationCore::instance()->pythonManager();
  if (manager && manager->interpreterIsInitialized())
    {
    QString script =
    "try:\n"
    "  paraview.smtrace\n"
    "  paraview.smtrace.trace_save_animation_end()\n"
    "except AttributeError: pass\n";
    pqPythonShell* shell = manager->pythonShellDialog()->shell();
    shell->executeScript(script);
    }
#endif
}

//-----------------------------------------------------------------------------
void pqSaveAnimationReaction::onWriteAnimation(
  const QString& filename, int magnification, double framerate)
{
  (void)filename;
  (void)magnification;
  (void)framerate;
#ifdef PARAVIEW_ENABLE_PYTHON
  pqPythonManager* manager = pqPVApplicationCore::instance()->pythonManager();
  if (manager && manager->interpreterIsInitialized())
    {
    QString script =
    "try:\n"
    "  paraview.smtrace\n"
    "  paraview.smtrace.trace_save_animation('%1', %2, %3, %4)\n"
    "except AttributeError: pass\n";
    script = script.arg(filename).arg(magnification).arg(2).arg(framerate);
    pqPythonShell* shell = manager->pythonShellDialog()->shell();
    shell->executeScript(script);
    }
#endif
}
