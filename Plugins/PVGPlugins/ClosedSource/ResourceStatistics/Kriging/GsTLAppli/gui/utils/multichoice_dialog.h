/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "gui" module of the Geostatistical Earth
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

#ifndef __GSTLAPPLI_GUI_UTILS_MULTICHOICE_DIALOG_H__
#define __GSTLAPPLI_GUI_UTILS_MULTICHOICE_DIALOG_H__

#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/utils/multichoice_dialog_base.h>

#include <qdialog.h>

class GsTL_project;
class MultiPropertySelector;

class GUI_DECL Multichoice_dialog : public Multichoice_dialog_base {

  Q_OBJECT

public:
  Multichoice_dialog( QWidget* parent = 0, const char* name = 0 );
  void set_label( const QString& label );
  void set_okbutton_caption( const QString& label );

  void set_choice( const QStringList& list );
  void add_choice_item( const QString& item );
  QStringList selected_items();

protected:
  MultiPropertySelector* choice_list_;
};

#endif