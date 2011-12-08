/*=========================================================================

   Program: ParaView
   Module:    pqStatusBar.cxx

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
#include "pqStatusBar.h"

#include "pqApplicationCore.h"
#include "pqProgressManager.h"
#include "pqProgressWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPixmap>
#include <QToolButton>

#include <iostream>

//-----------------------------------------------------------------------------
pqStatusBar::pqStatusBar(QWidget* parentObject)
  : Superclass(parentObject)
{
  QFrame* widget = new QFrame(this);
  widget->setFrameShape(QFrame::NoFrame);
  QHBoxLayout* hbox = new QHBoxLayout(widget);
  hbox->setMargin(0);
  hbox->setSpacing(0);
  pqProgressManager* progress_manager =
    pqApplicationCore::instance()->getProgressManager();

  // Progress bar/button management
  pqProgressWidget* const progress_bar = new pqProgressWidget(widget);
  progress_manager->addNonBlockableObject(progress_bar);
  progress_manager->addNonBlockableObject(progress_bar->getAbortButton());

  QObject::connect( progress_manager, SIGNAL(enableProgress(bool)),
                    progress_bar,     SLOT(enableProgress(bool)));

  QObject::connect( progress_manager, SIGNAL(progress(const QString&, int)),
                    progress_bar,     SLOT(setProgress(const QString&, int)));

  QObject::connect( progress_manager, SIGNAL(enableAbort(bool)),
                    progress_bar,     SLOT(enableAbort(bool)));

  QObject::connect( progress_bar,     SIGNAL(abortPressed()),
                    progress_manager, SLOT(triggerAbort()));

  // Final ui setup
  hbox->addWidget(progress_bar);
  this->addPermanentWidget(widget);
  widget->setEnabled(true);
}

//-----------------------------------------------------------------------------
pqStatusBar::~pqStatusBar()
{
}
