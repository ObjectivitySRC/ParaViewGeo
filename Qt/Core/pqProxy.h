/*=========================================================================

   Program: ParaView
   Module:    pqProxy.h

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

/// \file pqProxy.h
///
/// \date 11/16/2005

#ifndef _pqPipelineObject_h
#define _pqPipelineObject_h

#include "pqServerManagerModelItem.h"
class pqProxyInternal;
class pqServer;
class vtkPVXMLElement;
class vtkSMProxy;
class vtkSMProxyManager;

/// This class represents any registered Server Manager proxy.
/// It keeps essential information to locate the proxy as well as
/// additional metadata such as user-specified label.
class PQCORE_EXPORT pqProxy : public pqServerManagerModelItem
{
  Q_OBJECT
public:

  /// The modification state of this proxy
  enum ModifiedState { UNINITIALIZED, MODIFIED, UNMODIFIED };

  pqProxy(const QString& group, const QString& name,
    vtkSMProxy* proxy, pqServer* server, QObject* parent=NULL);
  virtual ~pqProxy();

  /// Get the server on which this proxy exists.
  pqServer *getServer() const
    { return this->Server; }

  /// This is a convenience method. It re-registers the underlying proxy
  /// with the requested new name under the same group. Then it unregisters
  /// the proxy from the group with the old name. This operation is
  /// understood as renaming the proxy, since as a consequence, this
  /// pqProxy's \c SMName changes.
  void rename(const QString& newname);

  /// Get the name with which this proxy is registered on the
  /// server manager. A proxy can be registered with more than
  /// one name on the Server Manager. This is the name/group which
  /// this pqProxy stands for. 
  const QString& getSMName();
  const QString& getSMGroup();

  /// Get the vtkSMProxy this object stands for.
  /// This can never be null. A pqProxy always represents
  /// one and only one Server Manager proxy.
  vtkSMProxy* getProxy() const;
  
  /// \brief
  ///   Gets whether or not the source has been modified.
  /// \return
  ///   True if the source has been modified.
  ModifiedState modifiedState() const {return this->Modified;}

  /// \brief
  ///   Sets whether or not the source has been modified.
  /// \param modified True if the source has been modified.
  void setModifiedState(ModifiedState modified);

  /// Returns the hints for this proxy, if any. May returns NULL
  /// if no hints are defined.
  vtkPVXMLElement* getHints() const;

  /// Sets default values for the underlying proxy. 
  /// This is during the initialization stage of the pqProxy 
  /// for proxies created by the GUI itself i.e.
  /// for proxies loaded through state or created by python client
  /// this method won't be called. 
  /// The default implementation iterates over all properties
  /// of the proxy and sets them to default values. 
  virtual void setDefaultPropertyValues();

  /// Returns a list of all helper proxies.
  QList<vtkSMProxy*> getHelperProxies() const;

  /// Returns a list of all the helper proxies added with a given key.
  QList<vtkSMProxy*> getHelperProxies(const QString& key) const;

  /// Returns the keys for helper proxies.
  QList<QString> getHelperKeys() const;

  /// Concept of helper proxies:
  /// A pqProxy is created for every important vtkSMProxy registered. Many a times, 
  /// there may be other proxies associated with that proxy, eg. lookup table proxies,
  /// implicit function proxies may be associated with a filter/source proxy. 
  /// The GUI can create "associated" proxies and add them as helper proxies.
  /// Helper proxies get registered under special groups, so that they are 
  /// undo/redo-able, and state save-restore-able. The pqProxy makes sure that 
  /// the helper proxies are unregistered when the main proxy is unregistered.
  virtual void addHelperProxy(const QString& key, vtkSMProxy*);
  void removeHelperProxy(const QString& key, vtkSMProxy*);

  /// Unregisters all helper proxies.
  void clearHelperProxies();

  /// Updates the internal datastructures using the proxies currently registered
  /// under the group that would be used for helper proxies. This makes it
  /// possible to locate helper proxies created from Python.
  void updateHelperProxies() const;

signals:
  /// Fired when the name of the proxy is changed.
  void nameChanged(pqServerManagerModelItem*);

  /// Fired when the modified status changes for the proxy.
  void modifiedStateChanged(pqServerManagerModelItem*);

protected:
  friend class pqServerManagerModel;

  /// Make this pqProxy take on a new identity. This is following case:
  /// Proxy A registered as (gA, nA), then is again registered as (gA, nA2).
  /// pqServerManagerModel does not create a new pqProxy for (gA, nA2). 
  /// However, if (gA, nA) is now unregistered, the same old instace of pqProxy
  /// which represented (gA, nA) will now "take on a new identity" and 
  /// represent proxy (gA, nA2). This method will trigger the
  /// nameChanged() signal.
  void setSMName(const QString& new_name);

  // Use this method to initialize the pqObject state using the
  // underlying vtkSMProxy. This needs to be done only once,
  // after the object has been created. 
  virtual void initialize() { };

  /// Returns the proxy manager by calling this->getProxy()->GetProxyManager();
  vtkSMProxyManager* proxyManager() const;

private:
  pqServer *Server;           ///< Stores the parent server.
  QString SMName;
  QString SMGroup;
  pqProxyInternal* Internal;
  ModifiedState Modified;
};

#endif
