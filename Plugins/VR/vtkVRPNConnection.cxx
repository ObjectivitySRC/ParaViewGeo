/*=========================================================================

   Program: ParaView
   Module:    vtkVRPNConnection.cxx

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
#include "vtkVRPNConnection.h"

#include <vrpn_Tracker.h>
#include <vrpn_Button.h>
#include <vrpn_Analog.h>
#include <vrpn_Dial.h>
#include <vrpn_Text.h>
#include "vtkMath.h"
#include "pqActiveObjects.h"
#include "pqView.h"
#include <pqDataRepresentation.h>
#include "vtkSMRenderViewProxy.h"
#include "vtkSMDoubleVectorProperty.h"
#include "vtkSMRepresentationProxy.h"
#include "vtkSMPropertyHelper.h"
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "vtkVRPNCallBackHandlers.h"
#include <QDateTime>
#include <QDebug>
#include <vtkstd/vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "vtkObjectFactory.h"
#include "vtkPVXMLElement.h"
#include "vtkMath.h"

class vtkVRPNConnection::pqInternals
{
public:
  pqInternals()
    {
      this->Tracker=0;
      this->Button=0;
      this->Analog=0;
      this->Dial=0;
      this->Text=0;
    }

  ~pqInternals()
    {
      if(this->Tracker!=0)
        {
        delete this->Tracker;
        }
      if(this->Button!=0)
        {
        delete this->Button;
        }
      if(this->Analog!=0)
        {
        delete this->Analog;
        }
      if(this->Dial!=0)
        {
        delete this->Dial;
        }
      if(this->Text!=0)
        {
        delete this->Text;
        }
    }

  vrpn_Tracker_Remote *Tracker;
  vrpn_Button_Remote  *Button;
  vrpn_Analog_Remote  *Analog;
  vrpn_Dial_Remote    *Dial;
  vrpn_Text_Receiver  *Text;
};


// -----------------------------------------------------------------------cnstr
vtkVRPNConnection::vtkVRPNConnection(QObject* parentObject)
  :Superclass( parentObject )
{
  this->Internals=new pqInternals();
  this->Initialized=false;
  this->_Stop = false;
  this->Address = "";
  this->Name = "";
  this->Type = "VRPN";
  this->TrackerPresent = false;
  this->AnalogPresent = false;
  this->ButtonPresent = false;
  this->TrackerTransformPresent = false;
  this->Transformation = vtkMatrix4x4::New();

  // this->Mapping["Tracker0@192.168.1.126"]="kinect";
  // this->Mapping["device0@localhost"]="space-navigator";
  // this->Mapping["TRACKER.0"] = "head";
  // this->Mapping["TRACKER.13"] = "right-hand";
}

// -----------------------------------------------------------------------destr
vtkVRPNConnection::~vtkVRPNConnection()
{
  delete this->Internals;
  this->Transformation->Delete();
}

// ----------------------------------------------------------------------public
void vtkVRPNConnection::AddButton(std::string id, std::string name)
{
  std::stringstream returnStr;
  returnStr << "button." << id;
  this->ButtonMapping[returnStr.str()] = name;
  this->ButtonPresent = true;
}

// ----------------------------------------------------------------------public
void vtkVRPNConnection::AddAnalog(std::string id, std::string name )
{
  std::stringstream returnStr;
  returnStr << "analog." << id;
  this->AnalogMapping[returnStr.str()] = name;
  this->AnalogPresent = true;
}

// ----------------------------------------------------------------------public
void vtkVRPNConnection::AddTracking( std::string id, std::string name)
{
  std::stringstream returnStr;
  returnStr << "tracker." << id;
  this->TrackerMapping[returnStr.str()] = name;
  this->TrackerPresent = true;
}

// ----------------------------------------------------------------------public
void vtkVRPNConnection::SetName(std::string name)
{
  this->Name = name;
}

// ----------------------------------------------------------------------public
void vtkVRPNConnection::SetAddress(std::string name)
{
  this->Address = name;
}

// ----------------------------------------------------------------------public
void vtkVRPNConnection::SetQueue( vtkVRQueue* queue )
{
  this->EventQueue = queue;
}

// ----------------------------------------------------------------------------
bool vtkVRPNConnection::Init()
{
  this->Internals->Tracker = new vrpn_Tracker_Remote(this->Address.c_str());
  this->Internals->Analog = new vrpn_Analog_Remote(this->Address.c_str());
  this->Internals->Button = new vrpn_Button_Remote(this->Address.c_str());
  this->Internals->Dial = new vrpn_Dial_Remote(this->Address.c_str());
  this->Internals->Text = new vrpn_Text_Receiver(this->Address.c_str());

  this->Initialized= ( this->Internals->Tracker!=0
                       && this->Internals->Analog!=0
                       && this->Internals->Button!=0
                       && this->Internals->Dial!=0
                       && this->Internals->Text!=0 );

  if(this->Initialized)
    {
    this->Internals->Tracker->register_change_handler(static_cast<void*>( this ),
                                                      handleTrackerChange );
    this->Internals->Analog->register_change_handler(static_cast<void*>( this ),
                                                     handleAnalogChange );
    this->Internals->Button->register_change_handler( static_cast<void*>( this ),
                                                      handleButtonChange );
    }
  return this->Initialized;
}

// ----------------------------------------------------------------private-slot
void vtkVRPNConnection::run()
{
  while ( !this->_Stop )
    {
    if(this->Initialized)
      {
      //    std::cout << "callback()" << std::endl;
      this->Internals->Tracker->mainloop();
      this->Internals->Button->mainloop();
      this->Internals->Analog->mainloop();
      this->Internals->Dial->mainloop();
      this->Internals->Text->mainloop();
      //msleep( 40 );
      }
    }
}

// ---------------------------------------------------------------------private
void vtkVRPNConnection::Stop()
{
  this->_Stop = true;
  QThread::terminate();
}

// ---------------------------------------------------------------------private
void vtkVRPNConnection::NewAnalogValue(vrpn_ANALOGCB data)
{
  vtkVREventData temp;
  temp.connId = this->Address;
  temp.name = GetName( ANALOG_EVENT);
  temp.eventType = ANALOG_EVENT;
  temp.timeStamp =   QDateTime::currentDateTime().toTime_t();
  temp.data.analog.num_channel = data.num_channel;
  for ( int i=0 ; i<data.num_channel ;++i )
    {
    temp.data.analog.channel[i] = data.channel[i];
    }
  this->EventQueue->enqueue( temp );
}

// ---------------------------------------------------------------------private
void vtkVRPNConnection::NewButtonValue(vrpn_BUTTONCB data)
{
  vtkVREventData temp;
  temp.connId = this->Address;
  temp.name = this->GetName( BUTTON_EVENT, data.button );
  temp.eventType = BUTTON_EVENT;
  temp.timeStamp =   QDateTime::currentDateTime().toTime_t();
  temp.data.button.button = data.button;
  temp.data.button.state = data.state;
  this->EventQueue->enqueue( temp );
}

// ---------------------------------------------------------------------private
void vtkVRPNConnection::NewTrackerValue(vrpn_TRACKERCB data )
{
  vtkVREventData temp;
  temp.connId = this->Address;
  temp.name = GetName( TRACKER_EVENT, data.sensor );
  temp.eventType = TRACKER_EVENT;
  temp.timeStamp =   QDateTime::currentDateTime().toTime_t();
  temp.data.tracker.sensor = data.sensor;
  double rotMatrix[3][3];
  vtkMath::QuaternionToMatrix3x3( data.quat, rotMatrix );
  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();

  matrix->Element[0][0] = rotMatrix[0][0];
  matrix->Element[0][1] = rotMatrix[0][1];
  matrix->Element[0][2] = rotMatrix[0][2];
  matrix->Element[0][3] = data .pos[0]*1;

  matrix->Element[1][0] = rotMatrix[1][0];
  matrix->Element[1][1] = 1*rotMatrix[1][1];
  matrix->Element[1][2] = rotMatrix[1][2];
  matrix->Element[1][3] = data.pos[1];

  matrix->Element[2][0] = rotMatrix[2][0];
  matrix->Element[2][1] = rotMatrix[2][1];
  matrix->Element[2][2] = rotMatrix[2][2];
  matrix->Element[2][3] = data.pos[2];

  matrix->Element[3][0] = 0.0f;
  matrix->Element[3][1] = 0.0f;
  matrix->Element[3][2] = 0.0f;
  matrix->Element[3][3] = 1.0f;

  vtkMatrix4x4::Multiply4x4( this->Transformation, matrix, matrix );

  temp.data.tracker.matrix[0] = matrix->Element[0][0];
  temp.data.tracker.matrix[1] = matrix->Element[0][1];
  temp.data.tracker.matrix[2] = matrix->Element[0][2];
  temp.data.tracker.matrix[3] = matrix->Element[0][3];

  temp.data.tracker.matrix[4] = matrix->Element[1][0];
  temp.data.tracker.matrix[5] = matrix->Element[1][1];
  temp.data.tracker.matrix[6] = matrix->Element[1][2];
  temp.data.tracker.matrix[7] = matrix->Element[1][3];

  temp.data.tracker.matrix[8] = matrix->Element[2][0];
  temp.data.tracker.matrix[9] = matrix->Element[2][1];
  temp.data.tracker.matrix[10] = matrix->Element[2][2];
  temp.data.tracker.matrix[11] = matrix->Element[2][3];

  temp.data.tracker.matrix[12] = matrix->Element[3][0];
  temp.data.tracker.matrix[13] = matrix->Element[3][1];
  temp.data.tracker.matrix[14] = matrix->Element[3][2];
  temp.data.tracker.matrix[15] = matrix->Element[3][3];

  matrix->Delete();
  this->EventQueue->enqueue( temp );
}

// ---------------------------------------------------------------------private
std::string vtkVRPNConnection::GetName( int eventType, int id )
{
  std::stringstream returnStr,connection,event;
  if(this->Name.size())
    returnStr << this->Name << ".";
  else
    returnStr << this->Address << ".";
  switch (eventType )
    {
    case ANALOG_EVENT:
      event << "analog." << id;
      if( this->AnalogMapping.find( event.str())!= this->ButtonMapping.end())
        returnStr << this->AnalogMapping[event.str()];
      else
        returnStr << event.str();
      break;
    case BUTTON_EVENT:
      event << "button." << id;
      if( this->ButtonMapping.find( event.str())!= this->ButtonMapping.end())
        returnStr << this->ButtonMapping[event.str()];
      else
        returnStr << event.str();
      break;
    case TRACKER_EVENT:
      event << "tracker."<< id;
      if( this->TrackerMapping.find( event.str())!=this->TrackerMapping.end())
        returnStr << this->TrackerMapping[event.str()];
      else
        returnStr << event.str();
      break;
    }
  return returnStr.str();
}

// ---------------------------------------------------------------------private
void vtkVRPNConnection::verifyConfig( const char* id,
                                      const char* name )
{
  if ( !id )
    {
    qWarning() << "\"id\" should be specified";
    }
  if ( !name )
    {
    qWarning() << "\"name\" should be specified";
    }
}

// ----------------------------------------------------------------------public
bool vtkVRPNConnection::configure(vtkPVXMLElement* child, vtkSMProxyLocator*)
{
  bool returnVal = false;
  if (child->GetName() && strcmp(child->GetName(),"VRPNConnection") == 0 )
    {
    for ( unsigned cc=0; cc < child->GetNumberOfNestedElements();++cc )
      {
      vtkPVXMLElement* event = child->GetNestedElement(cc);
      if ( event && event->GetName() )
        {
        const char* id = event->GetAttributeOrEmpty( "id" );
        const char* name = event->GetAttributeOrEmpty( "name" );
        this->verifyConfig(id, name);

        if ( strcmp( event->GetName(), "Button" )==0 )
          {
          this->AddButton( id, name );
          }
        else if ( strcmp( event->GetName(), "Analog" )==0 )
          {
          this->AddAnalog( id, name );
          }
        else if ( strcmp ( event->GetName(),  "Tracker" ) ==0 )
          {
          this->AddTracking( id, name );
          }
        else if ( strcmp ( event->GetName(),  "TrackerTransform" ) ==0 )
          {
          this->configureTransform( event );
          }

        else
          {
          qWarning() << "Unknown Device type: \"" << event->GetName() <<"\"";
          }
        returnVal = true;
        }
      }
    }
  return returnVal;
}

// ---------------------------------------------------------------------private
void vtkVRPNConnection::configureTransform( vtkPVXMLElement* child )
{
  if (child->GetName() && strcmp( child->GetName(), "TrackerTransform" )==0 )
    {
    child->GetVectorAttribute( "value",
                               16,
                               ( double* ) this->Transformation->Element );
    this->TrackerTransformPresent = true;
    }
}

// ----------------------------------------------------------------------public
vtkPVXMLElement* vtkVRPNConnection::saveConfiguration() const
{
  vtkPVXMLElement* child = vtkPVXMLElement::New();
  child->SetName("VRPNConnection");
  child->AddAttribute( "name", this->Name.c_str() );
  child->AddAttribute( "address",  this->Address.c_str() );
  saveButtonEventConfig( child );
  saveAnalogEventConfig( child );
  saveTrackerEventConfig( child );
  saveTrackerTransformationConfig( child );
  return child;
}

// ----------------------------------------------------------------------public
void vtkVRPNConnection::saveButtonEventConfig( vtkPVXMLElement* child )const
{
  if(!this->ButtonPresent) return;
  for ( std::map<std::string,std::string>::const_iterator it = this->ButtonMapping.begin();
        it!=this->ButtonMapping.end();
        ++it )
    {
    std::string key = it->first;
    std::string value = it->second;
    std::replace(key.begin(), key.end(), '.', ' ');
    std::istringstream stm(key);
    std::vector<std::string> token;
    for (;;)
      {
      std::string word;
      if (!(stm >> word)) break;
      token.push_back(word);
      }
    vtkPVXMLElement* event = vtkPVXMLElement::New();
    if ( strcmp( token[0].c_str(), "button" )==0 )
      {
      event->SetName("Button");
      event->AddAttribute("id", token[1].c_str() );
      event->AddAttribute("name",value.c_str());
      }
    child->AddNestedElement(event);
    event->FastDelete();
    }
}

// ----------------------------------------------------------------------public
void vtkVRPNConnection::saveAnalogEventConfig( vtkPVXMLElement* child ) const
{
  if(!this->AnalogPresent) return;
  for ( std::map<std::string,std::string>::const_iterator it = this->AnalogMapping.begin();
        it!=this->AnalogMapping.end();
        ++it )
    {
    std::string key = it->first;
    std::string value = it->second;
    std::replace(key.begin(), key.end(), '.', ' ');
    std::istringstream stm(key);
    std::vector<std::string> token;
    for (;;)
      {
      std::string word;
      if (!(stm >> word)) break;
      token.push_back(word);
      }
    vtkPVXMLElement* event = vtkPVXMLElement::New();
    if ( strcmp( token[0].c_str(), "analog" )==0 )
      {
      event->SetName("Analog");
      event->AddAttribute("id", token[1].c_str() );
      event->AddAttribute("name",value.c_str());
      }
    child->AddNestedElement(event);
    event->FastDelete();
    }
}

// ----------------------------------------------------------------------public
void vtkVRPNConnection::saveTrackerEventConfig( vtkPVXMLElement* child ) const
{
  if(!this->TrackerPresent) return;
  for ( std::map<std::string,std::string>::const_iterator it = this->TrackerMapping.begin();
        it!=this->TrackerMapping.end();
        ++it )
    {
    std::string key = it->first;
    std::string value = it->second;
    std::replace(key.begin(), key.end(), '.', ' ');
    std::istringstream stm(key);
    std::vector<std::string> token;
    for (;;)
      {
      std::string word;
      if (!(stm >> word)) break;
      token.push_back(word);
      }
    vtkPVXMLElement* event = vtkPVXMLElement::New();
    if ( strcmp( token[0].c_str(), "tracker" )==0 )
      {
      event->SetName("Tracker");
      event->AddAttribute("id", token[1].c_str() );
      event->AddAttribute("name",value.c_str());
      }
    child->AddNestedElement(event);
    event->FastDelete();
    }
}

// ---------------------------------------------------------------------private
void vtkVRPNConnection::saveTrackerTransformationConfig( vtkPVXMLElement* child ) const
{
  if(!this->TrackerTransformPresent) return;
  vtkPVXMLElement* transformationMatrix = vtkPVXMLElement::New();
  transformationMatrix->SetName("TrackerTransform");
  std::stringstream matrix;
  for (int i = 0; i < 16; ++i)
    {
    matrix <<  double( *( ( double* )this->Transformation->Element +i ) ) << " ";
    }
  transformationMatrix->AddAttribute( "value",  matrix.str().c_str() );
  child->AddNestedElement(transformationMatrix);
  transformationMatrix->FastDelete();
}

// ----------------------------------------------------------------------public
void vtkVRPNConnection::SetTransformation( vtkMatrix4x4* matrix )
{
  for (int i = 0; i < 4; ++i)
    {
    for (int j = 0; j < 4; ++j)
      {
      this->Transformation->SetElement(i,j, matrix->GetElement( i,j ) );
      }
    }
  this->TrackerTransformPresent = true;
}
