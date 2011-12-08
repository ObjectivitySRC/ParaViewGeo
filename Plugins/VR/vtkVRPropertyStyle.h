/*=========================================================================

   Program: ParaView
   Module:    $RCSfile$

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
#ifndef __vtkVRPropertyStyle_h
#define __vtkVRPropertyStyle_h

#include "vtkVRInteractorStyle.h"
#include "vtkSmartPointer.h"
#include <vector>
#include <map>

class vtkSMProperty;
class vtkSMProxy;

/// vtkVRPropertyStyle is superclass for interactor styles that change any
/// property on some proxy. Which property/proxy is being controlled can be
/// setup (in future) by means, reading configuration values from the state file
/// or via some user-interface panel.
class vtkVRPropertyStyle : public vtkVRInteractorStyle
{
  Q_OBJECT
  typedef vtkVRInteractorStyle Superclass;
public:
  vtkVRPropertyStyle(QObject* parent=0);
  virtual ~vtkVRPropertyStyle();

  /// called to handle an event. If the style does not handle this event or
  /// handles it but does not want to stop any other handlers from handling it
  /// as well, they should return false. Other return true. Returning true
  /// indicates that vtkVRQueueHandler the event has been "consumed".
  virtual bool handleEvent(const vtkVREventData& data);

  /// called to update all the remote vtkObjects and perhaps even to render.
  /// Typically processing intensive operations go here. The method should not
  /// be called from within the handler and is reserved to be called from an
  /// external interaction style manager.
  virtual bool update();

  /// configure the style using the xml configuration.
  virtual bool configure(vtkPVXMLElement* child, vtkSMProxyLocator*);

  /// save the xml configuration.
  virtual vtkPVXMLElement* saveConfiguration() const;

  /// Get/set the property being controlled by this style.
  virtual vtkSMProperty* getSMProperty() const;
  virtual vtkSMProxy* getSMProxy() const;
  virtual const QString& getSMPropertyName() const
    {return this->PropertyName; }
  void setSMProperty(vtkSMProxy*, const QString& property_name);

protected:
  void HandleButton ( const vtkVREventData& data );
  void HandleAnalog ( const vtkVREventData& data );
  void HandleTracker( const vtkVREventData& data );
  void SetButtonValue( std::string dest, int value );
  void SetAnalogValue( std::string dest, double value );
  void SetTrackerValue( std::string dest, double value );
  vtkSmartPointer<vtkSMProxy> Proxy;
  QString PropertyName;
  std::vector<std::string> tokenize( std::string input);
  std::map<std::string,std::string> Map;

private:
  Q_DISABLE_COPY(vtkVRPropertyStyle)
};

#endif
