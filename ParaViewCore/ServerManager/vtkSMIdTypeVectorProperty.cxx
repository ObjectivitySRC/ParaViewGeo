/*=========================================================================

  Program:   ParaView
  Module:    vtkSMIdTypeVectorProperty.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMIdTypeVectorProperty.h"

#include "vtkClientServerStream.h"
#include "vtkObjectFactory.h"
#include "vtkPVXMLElement.h"
#include "vtkSMMessage.h"
#include "vtkSMVectorPropertyTemplate.h"
#include "vtkSMStateLocator.h"

vtkStandardNewMacro(vtkSMIdTypeVectorProperty);

class vtkSMIdTypeVectorProperty::vtkInternals :
  public vtkSMVectorPropertyTemplate<vtkIdType>
{
public:
  vtkInternals(vtkSMIdTypeVectorProperty* ivp) :
    vtkSMVectorPropertyTemplate<vtkIdType>(ivp)
  {
  }
};

//---------------------------------------------------------------------------
vtkSMIdTypeVectorProperty::vtkSMIdTypeVectorProperty()
{
  this->Internals = new vtkInternals(this);
  this->ArgumentIsArray = 0;
}

//---------------------------------------------------------------------------
vtkSMIdTypeVectorProperty::~vtkSMIdTypeVectorProperty()
{
  delete this->Internals;
}

//---------------------------------------------------------------------------
void vtkSMIdTypeVectorProperty::WriteTo(vtkSMMessage* msg)
{
  ProxyState_Property *prop = msg->AddExtension(ProxyState::property);
  prop->set_name(this->GetXMLName());
  Variant *variant = prop->mutable_value();
  variant->set_type(Variant::IDTYPE);
  vtkstd::vector<vtkIdType>::iterator iter;
  for (iter = this->Internals->Values.begin(); iter!=
    this->Internals->Values.end(); ++iter)
    {
    variant->add_idtype(*iter);
    }
}

//---------------------------------------------------------------------------
void vtkSMIdTypeVectorProperty::ReadFrom(const vtkSMMessage* msg, int offset)
{
  assert(msg->ExtensionSize(ProxyState::property) > offset);

  const ProxyState_Property *prop = &msg->GetExtension(ProxyState::property,
    offset);
  assert(strcmp(prop->name().c_str(), this->GetXMLName()) == 0);

  const Variant *variant = &prop->value();

  int num_elems = variant->idtype_size();
  vtkIdType *values = new vtkIdType[num_elems+1];
  for (int cc=0; cc < num_elems; cc++)
    {
    values[cc] = variant->idtype(cc);
    }
  this->SetElements(values, num_elems);
  delete[] values;
}

//---------------------------------------------------------------------------
void vtkSMIdTypeVectorProperty::SetNumberOfUncheckedElements(unsigned int num)
{
  this->Internals->SetNumberOfUncheckedElements(num);
}

//---------------------------------------------------------------------------
void vtkSMIdTypeVectorProperty::SetNumberOfElements(unsigned int num)
{
  this->Internals->SetNumberOfElements(num);
}

//---------------------------------------------------------------------------
unsigned int vtkSMIdTypeVectorProperty::GetNumberOfUncheckedElements()
{
  return this->Internals->GetNumberOfUncheckedElements();
}

//---------------------------------------------------------------------------
unsigned int vtkSMIdTypeVectorProperty::GetNumberOfElements()
{
  return this->Internals->GetNumberOfElements();
}

//---------------------------------------------------------------------------
vtkIdType vtkSMIdTypeVectorProperty::GetElement(unsigned int idx)
{
  return this->Internals->GetElement(idx);
}

//---------------------------------------------------------------------------
vtkIdType vtkSMIdTypeVectorProperty::GetUncheckedElement(unsigned int idx)
{
  return this->Internals->GetUncheckedElement(idx);
}

//---------------------------------------------------------------------------
void vtkSMIdTypeVectorProperty::SetUncheckedElement(
  unsigned int idx, vtkIdType value)
{
  this->Internals->SetUncheckedElement(idx, value);
}

//---------------------------------------------------------------------------
int vtkSMIdTypeVectorProperty::SetElement(unsigned int idx, vtkIdType value)
{
  return this->Internals->SetElement(idx, value);
}

//---------------------------------------------------------------------------
int vtkSMIdTypeVectorProperty::SetElements1(vtkIdType value0)
{
  return this->SetElement(0, value0);
}

//---------------------------------------------------------------------------
int vtkSMIdTypeVectorProperty::SetElements2(vtkIdType value0, vtkIdType value1)
{
  int retVal1 = this->SetElement(0, value0);
  int retVal2 = this->SetElement(1, value1);
  return (retVal1 && retVal2);
}

//---------------------------------------------------------------------------
int vtkSMIdTypeVectorProperty::SetElements3(vtkIdType value0, 
                                            vtkIdType value1, 
                                            vtkIdType value2)
{
  int retVal1 = this->SetElement(0, value0);
  int retVal2 = this->SetElement(1, value1);
  int retVal3 = this->SetElement(2, value2);
  return (retVal1 && retVal2 && retVal3);
}

//---------------------------------------------------------------------------
int vtkSMIdTypeVectorProperty::SetElements(const vtkIdType* values)
{
  return this->Internals->SetElements(values);
}

//---------------------------------------------------------------------------
int vtkSMIdTypeVectorProperty::SetElements(const vtkIdType* values,
  unsigned int num)
{
  return this->Internals->SetElements(values, num);
}

//---------------------------------------------------------------------------
int vtkSMIdTypeVectorProperty::ReadXMLAttributes(vtkSMProxy* parent,
                                                 vtkPVXMLElement* element)
{
  int retVal;

  retVal = this->Superclass::ReadXMLAttributes(parent, element);
  if (!retVal)
    {
    return retVal;
    }

  int arg_is_array;
  retVal = element->GetScalarAttribute("argument_is_array", &arg_is_array);
  if(retVal) 
    { 
    this->SetArgumentIsArray(arg_is_array); 
    }

  int numElems = this->GetNumberOfElements();
  if (numElems > 0)
    {
    if (element->GetAttribute("default_values") &&
        strcmp("none", element->GetAttribute("default_values")) == 0 )
      {
      this->Internals->Initialized = false;
      }
    else
      {
      int* initVal = new int[numElems];
      int numRead = element->GetVectorAttribute("default_values",
                                                numElems,
                                                initVal);
      
      if (numRead > 0)
        {
        if (numRead != numElems)
          {
          vtkErrorMacro("The number of default values does not match the "
                        "number of elements. Initialization failed.");
          delete[] initVal;
          return 0;
          }
        vtkIdType *idtype_values = new vtkIdType[numElems];
        vtkstd::copy(initVal, initVal+numElems, idtype_values);
        this->SetElements(idtype_values);
        delete[] idtype_values;
        this->Internals->UpdateDefaultValues();
        }
      else if (!this->Internals->Initialized)
        {
        vtkErrorMacro("No default value is specified for property: "
                      << this->GetXMLName()
                      << ". This might lead to stability problems");
        }
      delete[] initVal;
      }
    }

  return 1;
}

//---------------------------------------------------------------------------
void vtkSMIdTypeVectorProperty::Copy(vtkSMProperty* src)
{
  this->Superclass::Copy(src);

  vtkSMIdTypeVectorProperty* dsrc = vtkSMIdTypeVectorProperty::SafeDownCast(
    src);
  if (dsrc)
    {
    this->Internals->Copy(dsrc->Internals);
    }
}

//---------------------------------------------------------------------------
void vtkSMIdTypeVectorProperty::ResetToDefaultInternal()
{
  this->Internals->ResetToDefaultInternal();
}

//---------------------------------------------------------------------------
void vtkSMIdTypeVectorProperty::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "ArgumentIsArray: " << this->ArgumentIsArray << endl;
  os << indent << "Values: ";
  for (unsigned int i=0; i<this->GetNumberOfElements(); i++)
    {
    os << this->GetElement(i) << " ";
    }
  os << endl;
}
//---------------------------------------------------------------------------
void vtkSMIdTypeVectorProperty::SaveStateValues(vtkPVXMLElement* propElement)
{
  this->Internals->SaveStateValues(propElement);
}
//---------------------------------------------------------------------------
int vtkSMIdTypeVectorProperty::SetElementAsString(int idx, const char* value)
{
  return this->Internals->SetElementAsString(idx, value);
}
