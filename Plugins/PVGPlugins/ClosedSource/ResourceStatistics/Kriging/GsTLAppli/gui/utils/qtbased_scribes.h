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

#ifndef __GSTLAPPLI_GUI_UTILS_QTBASED_SCRIBES_H__
#define __GSTLAPPLI_GUI_UTILS_QTBASED_SCRIBES_H__

#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <qstring.h>

class QTextEdit;


/** QTextedit_scribe is a Scribe (ie an object that listens to a Channel) that
* outputs the messages into a QTextEdit, possibly using a richtext format. 
* The color to use to display the messages can be specified.
*/
class QTextedit_scribe : public Scribe {
public:

  /** Constructs a scribe which will write to QTextEdit \c message_box.
  * Text will be written in the color specified by \c color
  */
  QTextedit_scribe( QTextEdit* message_box = 0, 
                    const QString& color = QString::null );
  virtual void write( const std::string& str, const Channel* );

private:
  QTextEdit* message_box_;
  QString opening_tag_;
  QString closing_tag_;
};


#endif
