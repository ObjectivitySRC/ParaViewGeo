/*=========================================================================

   Program: ParaView
   Module:    pqAlwaysConnectedBehavior.cxx

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
#include "pqAlwaysConnectedBehavior.h"

#include "pqApplicationCore.h"
#include "pqObjectBuilder.h"
#include "pqServer.h"
#include "pqServerManagerModel.h"
#include "vtkNetworkAccessManager.h"
#include "vtkProcessModule.h"

//-----------------------------------------------------------------------------
pqAlwaysConnectedBehavior::pqAlwaysConnectedBehavior(QObject* parentObject)
: Superclass(parentObject),
  DefaultServer("builtin:")
{
  this->Timer.setSingleShot(true);
  this->Timer.setInterval(0);
  QObject::connect(&this->Timer, SIGNAL(timeout()),
    this, SLOT(serverCheck()));

  pqServerManagerModel* smmodel =
    pqApplicationCore::instance()->getServerManagerModel();
  QObject::connect(smmodel, SIGNAL(finishedRemovingServer()),
    this, SLOT(delayedServerCheck()));
  this->serverCheck();
}

//-----------------------------------------------------------------------------
pqAlwaysConnectedBehavior::~pqAlwaysConnectedBehavior()
{
}

//-----------------------------------------------------------------------------
void pqAlwaysConnectedBehavior::delayedServerCheck()
{
  this->Timer.start();
}

//-----------------------------------------------------------------------------
void pqAlwaysConnectedBehavior::serverCheck()
{
  pqApplicationCore* core = pqApplicationCore::instance();
  if (core->getServerManagerModel()->getNumberOfItems<pqServer*>() != 0)
    {
    return;
    }
  if (core->getObjectBuilder()->waitingForConnection())
    {
    // Try again later, we are waiting for server to connect.
    this->delayedServerCheck();
    return;
    }

  core->getObjectBuilder()->createServer(this->DefaultServer);
}
