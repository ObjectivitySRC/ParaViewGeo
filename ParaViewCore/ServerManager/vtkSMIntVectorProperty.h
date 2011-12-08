/*=========================================================================

  Program:   ParaView
  Module:    vtkSMIntVectorProperty.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSMIntVectorProperty - property representing a vector of integers
// .SECTION Description
// vtkSMIntVectorProperty is a concrete sub-class of vtkSMVectorProperty
// representing a vector of integers.
// .SECTION See Also
// vtkSMVectorProperty vtkSMDoubleVectorProperty vtkSMStringVectorProperty

#ifndef __vtkSMIntVectorProperty_h
#define __vtkSMIntVectorProperty_h

#include "vtkSMVectorProperty.h"

class vtkSMStateLocator;

class VTK_EXPORT vtkSMIntVectorProperty : public vtkSMVectorProperty
{
public:
  static vtkSMIntVectorProperty* New();
  vtkTypeMacro(vtkSMIntVectorProperty, vtkSMVectorProperty);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Returns the size of the vector.
  virtual unsigned int GetNumberOfElements();

  // Description:
  // Sets the size of the vector. If num is larger than the current
  // number of elements, this may cause reallocation and copying.
  virtual void SetNumberOfElements(unsigned int num);

  // Description:
  // Set the value of 1 element. The vector is resized as necessary.
  // Returns 0 if Set fails either because the property is read only
  // or the value is not in all domains. Returns 1 otherwise.
  int SetElement(unsigned int idx, int value);

  // Description:
  // Set the values of all elements. The size of the values array
  // has to be equal or larger to the size of the vector.
  // Returns 0 if Set fails either because the property is read only
  // or one or more of the values is not in all domains.
  // Returns 1 otherwise.
  int SetElements(const int* values);
  int SetElements(const int* values, unsigned int numElems);
  int *GetElements();

  // Description:
  // Set the value of 1st element. The vector is resized as necessary.
  // Returns 0 if Set fails either because the property is read only
  // or one or more of the values is not in all domains.
  // Returns 1 otherwise.
  int SetElements1(int value0);

  // Description:
  // Set the values of the first 2 elements. The vector is resized as necessary.
  // Returns 0 if Set fails either because the property is read only
  // or one or more of the values is not in all domains.
  // Returns 1 otherwise.
  int SetElements2(int value0, int value1);

  // Description:
  // Set the values of the first 3 elements. The vector is resized as necessary.
  // Returns 0 if Set fails either because the property is read only
  // or one or more of the values is not in all domains.
  // Returns 1 otherwise.
  int SetElements3(int value0, int value1, int value2);

  // Description:
  // Returns the value of 1 element.
  int GetElement(unsigned int idx);

  // Description:
  // Returns the value of 1 unchecked element. These are used by
  // domains. SetElement() first sets the value of 1 unchecked
  // element and then calls IsInDomain and updates the value of
  // the corresponding element only if IsInDomain passes.
  int GetUncheckedElement(unsigned int idx);

  // Description:
  // Set the value of 1 unchecked element. This can be used to
  // check if a value is in all domains of the property. Call
  // this and call IsInDomains().
  void SetUncheckedElement(unsigned int idx, int value);

  // Description:
  // Returns the size of unchecked elements. Usually this is
  // the same as the number of elements but can be different
  // before a domain check is performed.
  virtual unsigned int GetNumberOfUncheckedElements();

  // Description:
  // If ArgumentIsArray is true, multiple elements are passed in as
  // array arguments. For example, For example, if
  // RepeatCommand is true, NumberOfElementsPerCommand is 2, the
  // command is SetFoo and the values are 1 2 3 4 5 6, the resulting
  // stream will have:
  // @verbatim
  // * Invoke obj SetFoo array(1, 2)
  // * Invoke obj SetFoo array(3, 4)
  // * Invoke obj SetFoo array(5, 6)
  // @endverbatim
  vtkGetMacro(ArgumentIsArray, int);
  vtkSetMacro(ArgumentIsArray, int);
  vtkBooleanMacro(ArgumentIsArray, int);

  // Description: 
  // Copy all property values.
  virtual void Copy(vtkSMProperty* src);

  // Description:
  // Returns the default value, if any, specified in the XML.
  int GetDefaultValue(int idx);

//BTX
protected:
  vtkSMIntVectorProperty();
  ~vtkSMIntVectorProperty();

  // Description:
  // Let the property write its content into the stream
  virtual void WriteTo(vtkSMMessage*);

  // Description:
  // Let the property read and set its content from the stream
  virtual void ReadFrom(const vtkSMMessage*, int message_offset);


  // Handle XML with int type for default values.
  virtual int ReadXMLAttributes(vtkSMProxy* parent, 
                                vtkPVXMLElement* element);

  class vtkInternals;
  vtkInternals* Internals;

  int ArgumentIsArray;

  // Description:
  // Sets the size of unchecked elements. Usually this is
  // the same as the number of elements but can be different
  // before a domain check is performed.
  virtual void SetNumberOfUncheckedElements(unsigned int num);

  // Subclass may override this if ResetToDefault can reset to default
  // value specified in the configuration file.
  virtual void ResetToDefaultInternal();

  // Save concrete property values into the XML state property declaration
  virtual void SaveStateValues(vtkPVXMLElement* propElement);

  // Used in the LoadState method of VectorProperty
  virtual int SetElementAsString(int idx, const char* value);

private:
  vtkSMIntVectorProperty(const vtkSMIntVectorProperty&); // Not implemented
  void operator=(const vtkSMIntVectorProperty&); // Not implemented
//ETX
};

#endif
