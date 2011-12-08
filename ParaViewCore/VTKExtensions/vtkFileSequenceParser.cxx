/*=========================================================================

  Program:   ParaView
  Module:    vtkFileSequenceParser.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkFileSequenceParser.h"

#include "vtkObjectFactory.h"

#include <vtksys/SystemTools.hxx>
#include <vtksys/RegularExpression.hxx>
#include <vtkstd/set>
#include <vtkstd/string>

vtkStandardNewMacro(vtkFileSequenceParser);
//-----------------------------------------------------------------------------
vtkFileSequenceParser::vtkFileSequenceParser() :
  // sequence ending with numbers.
  reg_ex( new vtksys::RegularExpression("^(.*)\\.([0-9.]+)$")),
  // sequence ending with extension.
  reg_ex2( new vtksys::RegularExpression("^(.*)(\\.|_|-)([0-9.]+)\\.(.*)$")),
  // sequence ending with extension, but with no ". or _" before
  // the series number.
  reg_ex3( new vtksys::RegularExpression("^(.*)([a-zA-Z])([0-9.]+)\\.(.*)$")),
  // sequence ending with extension, and starting with series number
  // followed by ". or _".
  reg_ex4( new vtksys::RegularExpression("^([0-9.]+)(\\.|_|-)(.*)\\.(.*)$")),
  // sequence ending with extension, and starting with series number,
  // but not followed by ". or _".
  reg_ex5( new vtksys::RegularExpression("^([0-9.]+)([a-zA-Z])(.*)\\.(.*)$")),
  // fallback: any sequence with a number in the middle (taking the last number
  // if multiple exist).
  reg_ex_last( new vtksys::RegularExpression("^(.*[^0-9])([0-9]+)([^0-9]+)$")),
  SequenceIndex(-1),
  SequenceName(NULL)
{
}

//-----------------------------------------------------------------------------
vtkFileSequenceParser::~vtkFileSequenceParser()
{
  delete this->reg_ex;
  delete this->reg_ex2;
  delete this->reg_ex3;
  delete this->reg_ex4;
  delete this->reg_ex5;
  delete this->reg_ex_last;

  this->SetSequenceName(NULL);
}


//-----------------------------------------------------------------------------
bool vtkFileSequenceParser::ParseFileSequence(char * file)
{
  bool match = false;
  if (this->reg_ex->find(file))
    {
    this->SetSequenceName(this->reg_ex->match(1).c_str());
    this->SequenceIndex = atoi(reg_ex->match(2).c_str());
    match = true;
    }
  else if (this->reg_ex2->find(file))
    {
    this->SetSequenceName(std::string(this->reg_ex2->match(1) + this->reg_ex2->match(2) + ".." + this->reg_ex2->match(4)).c_str());
    this->SequenceIndex = atoi(reg_ex2->match(3).c_str());
    match = true;
    }
  else if (this->reg_ex3->find(file))
    {
    this->SetSequenceName(std::string(this->reg_ex3->match(1) + this->reg_ex3->match(2) + ".." + this->reg_ex3->match(4)).c_str());
    this->SequenceIndex = atoi(reg_ex3->match(3).c_str());
    match = true;
    }
  else if (this->reg_ex4->find(file))
    {
    this->SetSequenceName(std::string(".." + this->reg_ex4->match(2) + this->reg_ex4->match(3) + "." + this->reg_ex4->match(4)).c_str());
    this->SequenceIndex = atoi(reg_ex4->match(1).c_str());
    match = true;
    }
  else if (this->reg_ex5->find(file))
    {
    this->SetSequenceName(std::string(".." + this->reg_ex5->match(2) + this->reg_ex5->match(3) + "." + this->reg_ex5->match(4)).c_str());
    this->SequenceIndex = atoi(reg_ex5->match(1).c_str());
    match = true;
    }
  else if (this->reg_ex_last->find(file))
    {
    this->SetSequenceName(std::string(this->reg_ex_last->match(1) + ".." + this->reg_ex_last->match(3)).c_str());
    this->SequenceIndex = atoi(reg_ex_last->match(2).c_str());
    match = true;
    }
  return match;
}

//-----------------------------------------------------------------------------
void vtkFileSequenceParser::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

}
