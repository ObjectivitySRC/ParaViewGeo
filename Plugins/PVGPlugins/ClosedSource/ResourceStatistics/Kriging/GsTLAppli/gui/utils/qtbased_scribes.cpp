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

#include <GsTLAppli/gui/utils/qtbased_scribes.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qtextedit.h>
#include <qregexp.h>


QTextedit_scribe::QTextedit_scribe( QTextEdit* message_box, 
                                    const QString& color ) {
  message_box_ = message_box;
  opening_tag_ = "";
  closing_tag_ = "";

  if( !color.isEmpty() ) {
    opening_tag_ = QString( "<font color=" ) + color + QString( ">" );
    closing_tag_ = "</font>";
  }
}


void QTextedit_scribe::write( const std::string& str, const Channel* ch ) {
  if( !message_box_ || str.empty() ) return ;

  QString qstr( str.c_str() );

  if( dynamic_cast<const Error_channel*>( ch ) )
    qstr.prepend( "<font color=red>error</font>: " );

  if( !opening_tag_.isEmpty() ) {
    QStringList list = QStringList::split( "\n", qstr );
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
       (*it).prepend( opening_tag_ );
       (*it).append( closing_tag_ );
       message_box_->append( *it );
    }
  }
  else {
    message_box_->append( qstr  );
  }
}
