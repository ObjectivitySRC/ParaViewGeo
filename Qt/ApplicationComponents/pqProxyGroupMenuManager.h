/*=========================================================================

   Program: ParaView
   Module:    pqProxyGroupMenuManager.h

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
#ifndef __pqProxyGroupMenuManager_h 
#define __pqProxyGroupMenuManager_h

#include <QMenu>
#include "pqApplicationComponentsExport.h"

class vtkPVXMLElement;
class vtkSMProxy;

/// pqProxyGroupMenuManager is a menu-populator that fills up a menu with
/// proxies defined in an XML configuration file. This is use to automatically
/// build the sources and filters menu in ParaView.
class PQAPPLICATIONCOMPONENTS_EXPORT pqProxyGroupMenuManager : public QObject
{
  Q_OBJECT
  typedef QObject Superclass;
public:
  /// Constructor.
  /// \c menu is the Menu to be populated.
  /// \c resourceTagName is the tag name eg. "ParaViewSources" in the client
  /// configuration files which contains lists the items shown by this menu.
  pqProxyGroupMenuManager(QMenu* menu, const QString& resourceTagName);
  ~pqProxyGroupMenuManager();

  /// Access the menu.
  QMenu* menu() const
    { return static_cast<QMenu*>(this->parent()); }

  /// When size>0 a recently used category will be added to the menu.
  /// One must call update() or initialize() after changing this value.
  void setRecentlyUsedMenuSize(unsigned int val)
    { this->RecentlyUsedMenuSize = val; }

  unsigned int recentlyUsedMenuSize() const
    { return this->RecentlyUsedMenuSize; }

  /// returns the actions created by this menu manager.
  QList<QAction*> actions() const;

  /// Returns the prototype proxy for the action.
  vtkSMProxy* getPrototype(QAction* action) const;

  /// Provides mechanism to explicitly add a proxy to the menu.
  void addProxy(const QString& xmlgroup, const QString& xmlname);

  /// Provides mechanism to explicitly remove a proxy to the menu.
  void removeProxy(const QString& xmlgroup, const QString& xmlname);

  /// Returns a list of categories that have the "show_in_toolbar" attribute set
  /// to 1.
  QStringList getToolbarCategories() const;

  /// Returns the list of actions in a category.
  QList<QAction*> actions(const QString& category);

  /// Attach an observer to proxy manager to monitor any proxy definition update
  /// The detected proxy have to own a hint
  ///     <ShowInMenu category=""/>
  /// where those attribute are fully optional
  void addProxyDefinitionUpdateListener(const QString& proxyGroupName);
  void removeProxyDefinitionUpdateListener(const QString& proxyGroupName);

public slots:
  /// Load a configuration XML. It will find the elements with resourceTagName
  /// in the XML and populate the menu accordingly. Applications do not need to
  /// call this method directly, it's by default connected to
  /// pqApplicationCore::loadXML()
  void loadConfiguration(vtkPVXMLElement*);

  /// Look for new proxy definition to add inside the menu
  void lookForNewDefinitions();

  /// Remove all ProxyDefinitionUpdate observers to active server
  void removeProxyDefinitionUpdateObservers();

  /// Update the list of ProxyDefinitionUpdate observers to  server
  void addProxyDefinitionUpdateObservers();

  /// Enable/disable the menu and the actions.
  void setEnabled(bool enable);

  /// Forces a re-population of the menu. Any need to call this only after
  /// addProxy() has been used to explicitly add entries.
  void populateMenu();

signals:
  void triggered(const QString& group, const QString& name);
  
  /// fired when the menu gets repopulated,typically means that the actions have
  /// been updated.
  void menuPopulated();

protected slots:
  void triggered();
  void quickLaunch();

protected:
  QString ResourceTagName;
  vtkPVXMLElement* MenuRoot;
  int RecentlyUsedMenuSize;
  bool Enabled;

  void loadRecentlyUsedItems();
  void saveRecentlyUsedItems();
  void populateRecentlyUsedMenu(QMenu*);

  /// Returns the action for a given proxy.
  QAction* getAction(const QString& pgroup, const QString& proxyname);

private:
  Q_DISABLE_COPY(pqProxyGroupMenuManager)
  
  class pqInternal;
  pqInternal* Internal;
};

#endif


