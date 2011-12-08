// -*- c++ -*-
/*=========================================================================

   Program: ParaView
   Module:    pqTimerLogDisplay.h

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

#ifndef _pqTimerLogDisplay_h
#define _pqTimerLogDisplay_h

#include <QDialog>
#include "pqComponentsExport.h"

class pqTimerLogDisplayUi;

class vtkPVTimerInformation;

class PQCOMPONENTS_EXPORT pqTimerLogDisplay : public QDialog
{
  Q_OBJECT

public:
  pqTimerLogDisplay(QWidget *p = NULL);
  ~pqTimerLogDisplay();
  typedef QDialog Superclass;

  float timeThreshold() const;
  int bufferLength() const;
  bool isEnabled() const;

public slots:
  void refresh();
  void clear();
  void setTimeThreshold(float value);
  void setBufferLength(int value);
  void setEnable(bool state);
  void save();
  void save(const QString &filename);
  void save(const QStringList &files);

  void saveState();
  void restoreState();

protected:
  virtual void addToLog(const QString &source,
                        vtkPVTimerInformation *timerInfo);

  virtual void showEvent(QShowEvent*);
  virtual void hideEvent(QHideEvent*);

protected slots:
  void setTimeThresholdById(int id);
  void setBufferLengthById(int id);

private:
  pqTimerLogDisplay(const pqTimerLogDisplay &); // Not implemented
  void operator=(const pqTimerLogDisplay &);    // Not implemented

  double LogThreshold;
  pqTimerLogDisplayUi *ui;
};

#endif //_pqTimerLogDisplay_h
