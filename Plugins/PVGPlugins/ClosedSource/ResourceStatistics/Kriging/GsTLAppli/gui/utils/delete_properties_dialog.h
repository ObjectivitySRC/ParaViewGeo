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

#ifndef __GSTLAPPLI_GUI_UTILS_DELETE_PROPERTIES_DIALOG_H__
#define __GSTLAPPLI_GUI_UTILS_DELETE_PROPERTIES_DIALOG_H__

#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/gui/utils/multichoice_dialog.h>

#include <qdialog.h>

class MultiPropertySelector;
class GridSelector;


class GUI_DECL Delete_properties_dialog : public Multichoice_dialog {

  Q_OBJECT

public:
  Delete_properties_dialog( GsTL_project* proj,
                            QWidget* parent = 0, const char* name = 0 );
  virtual ~Delete_properties_dialog() {}
  QString selected_grid() const;

protected:
  GridSelector* grid_selector_;
};

#endif

