/*=========================================================================

  Program:   ParaView
  Module:    vtkFileSequenceParser.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkFileSequenceParser - Parses out the base file name of a file
// sequence and also the specific index of the given file.
// .SECTION Description:
// Given a file name (without path). I will
// extract the base portion of the file name that is common to all the files
// in the sequence. It will also provide the current sequence index of the
// provided file name.
// by several vtkPVUpdateSuppressor objects.

#ifndef __vtkFileSequenceParser_h
#define __vtkFileSequenceParser_h

#include "vtkObject.h"

namespace vtksys {
  class RegularExpression;
}


class VTK_EXPORT vtkFileSequenceParser : public vtkObject
{
public:
  static vtkFileSequenceParser* New();
  vtkTypeMacro(vtkFileSequenceParser, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Extract base file name sequence from the file.
  // Returns true if a sequence is detected and
  // sets SequenceName and SequenceIndex.
  bool ParseFileSequence(char * file);

  vtkGetStringMacro(SequenceName);
  vtkGetMacro(SequenceIndex, int);

protected:
  vtkFileSequenceParser();
  ~vtkFileSequenceParser();

  vtksys::RegularExpression * reg_ex;
  vtksys::RegularExpression * reg_ex2;
  vtksys::RegularExpression * reg_ex3;
  vtksys::RegularExpression * reg_ex4;
  vtksys::RegularExpression * reg_ex5;
  vtksys::RegularExpression * reg_ex_last;

  // Used internall so char * allocations are done automatically.
  vtkSetStringMacro(SequenceName);

  int SequenceIndex;
  char * SequenceName;
private:
  vtkFileSequenceParser(const vtkFileSequenceParser&); // Not implemented.
  void operator=(const vtkFileSequenceParser&); // Not implemented.
};

#endif
