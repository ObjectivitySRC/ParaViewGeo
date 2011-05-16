/**********************************************************************
** Author: Ting Li
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "filters" module of the Geostatistical Earth
** Modeling Software (GEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/

/* -*- c++ -*- */

#include "pprt.h"

#include <stdio.h>  /* for popen */
#include <iostream>

#ifdef WIN32
#define popen  _popen
#define pclose _pclose
#endif

using namespace std;

pprt::pprt() : _pipe(NULL), _package(NULL)
{
  //if (!(_pipe = popen(buf, "rb"))) /* should raise an exception here */
  if (!(_pipe = fopen(output, "rb")))
    return;
}

/* Does very little value validation. Assumes script
   is spitting out the correct values */
Prt * pprt::fill()
{
  int num[5];
  int i, len, total, temp = 0;

  if (!_pipe)
    return NULL;

  if (_package)
    return _package;

  _package = new Prt;
  
  /* ----------------- read header --------------*/
  fread(num, sizeof(int), 5, _pipe);
  
  /* number of values to be read */
  total = num[0]*num[1]*num[2]*num[3]*num[4];
  
  _package->X = num[0]; _package->Y = num[1]; _package->Z = num[2];
  _package->num_dates = num[3];
  _package->num_properties = num[4];

  _package->dates = new char *[_package->num_dates];
  _package->properties = new char *[_package->num_properties];
  
  for (i = 0; i < _package->num_dates; ++i) {
    fread(&len, sizeof(int), 1, _pipe);
    _package->dates[i] = new char[len+1];
    fread(_package->dates[i], sizeof(char), len, _pipe);
    _package->dates[i][len] = '\0';
  }

  for (i = 0; i < _package->num_properties; ++i) {
    fread(&len, sizeof(int), 1, _pipe);
    _package->properties[i] = new char[len+1];
    fread(_package->properties[i], sizeof(char), len, _pipe);
    _package->properties[i][len] = '\0';
  }
  /* -------------------------------------------*/


  _package->data = new float[total];

  if ((temp=fread(_package->data, sizeof(float), total, _pipe)) < total) {
    cerr << "Error reading " << total << "," << temp << endl;
    return NULL;
  }

  /*
  cout << _package->X << "," << _package->Y << "," << _package->Z << endl;

  for (i = 0; i < _package->num_dates; ++i)
    cout << _package->dates[i] << endl;
  for (i = 0; i < _package->num_properties; ++i)
    cout << _package->properties[i] << endl;

  return _package;
  */
  return _package;

}

pprt::~pprt()
{
  int i;
  if (!_package)
	  return;

  for (i = 0; i < _package->num_dates; ++i)
    delete _package->dates[i];
  delete []_package->dates;

  for (i = 0; i < _package->num_properties; ++i)
    delete _package->properties[i];
  delete []_package->properties;

  delete []_package->data;
  delete _package;

  if (_pipe)
    fclose(_pipe);


}
