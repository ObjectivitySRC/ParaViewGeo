/**********************************************************************
** Author: Ting Li
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "actions" module of the Geostatistical Earth
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

//$Log: lineeditkey.h,v $
//Revision 4.0  2008/04/23 02:42:17  nico97492
//BOOK RELEASE
//
//Revision 3.0  2007/06/25 01:12:21  nico97492
//NEW RELEASE
//
//Revision 2.3  2007/06/04 06:15:27  nico97492
//Fixes to compile on Ubuntu 7 - g++ 4.1.2
//
//Revision 2.2  2007/04/25 17:19:35  lting2001
//merged with branch testing_030707
//
//Revision 2.1.2.1  2007/04/09 21:10:14  lting2001
//*** empty log message ***
//
//Revision 2.1  2007/02/27 19:36:36  lting2001
//*** empty log message ***
//
//Revision 1.1  2006/12/07 01:15:52  lting2001
//*** empty log message ***
//

#ifndef LINEEDITKEY_H
#define LINEEDITKEY_H

#include <GsTLAppli/utils/manager.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/utils/gstl_messages_private.h>

#include <qevent.h>
#include <qlineedit.h>
#include <qnamespace.h>  // for key defs
#include <vector>

// We want to capture certain key combos in the line edit
class LineEditKey : public QLineEdit
{
public:
	LineEditKey(QWidget * parent, const char * name = 0) : QLineEdit(parent, name) {
		_v = NULL;
		_curCommand = "";
		_needSave = true;
	}
	void setHistory(const std::vector<std::string> * p) { _v = p; }
	void reset() {  _itr = _v->end(); _needSave = true;}

protected:
	void keyPressEvent ( QKeyEvent * e ) {
		QString command;

		// auto finish
		if (e->state() == Qt::ControlButton && e->key() == Qt::Key_Tab) {
			std::vector<QString> match;
			Manager* mng;
			SmartPtr<Named_interface> ni;
			std::vector<std::string>::iterator itr;

			ni = Root::instance()->interface( actions_manager );
			mng = dynamic_cast<Manager*>( ni.raw_ptr() );
			if( !mng ) {
				appli_message("Cannot cast to manager type");
				return;
			}

			command = text();
			std::string sCommand = command.latin1();
			sCommand = mng->finish(sCommand);
			command = sCommand.c_str();
			
			for (Manager::type_iterator titr = mng->begin(); titr != mng->end(); ++titr) {
				QString temp = (*titr).c_str();
				if (temp.startsWith(command, false))
					match.push_back(temp);
			}

			if (match.size() == 1) {
				setText(match[0]);
			}
			else if (match.size() == 0) {
				GsTLcout << "***No match found***" << gstlIO::end;
			}
			else {
				int count = 1;
				setText(command);
				GsTLcout << "Possible completions:" << gstlIO::end;				
				for (std::vector<QString>::iterator itr = match.begin();
						itr != match.end(); ++itr,++count)
					GsTLcout << "**" << count << " - " << itr->latin1() << gstlIO::end;
			}
			return;
		}
		
		switch (e->key()) {
		case Qt::Key_Up :  // shows command history
			if (_v->empty())
				break;
			if (_needSave) {
				_curCommand = text();
				_needSave = false;
			}
			if (_itr != _v->begin()) {
				--_itr;
				command = (*_itr).c_str();
				setText(command);
			}
			break;
		case Qt::Key_Down :
			if (_itr == _v->end())
				break;
			else if (_itr == (_v->end()-1)) {
				command = _curCommand;
				++_itr;
			}
			else {
				++_itr;
				command = (*_itr).c_str();
			}
			setText(command);
			break;
		case Qt::Key_Tab:
			break;
		default :
			QLineEdit::keyPressEvent(e); 
			break;
		}
	}

	const std::vector<std::string> * _v;
	QString _curCommand;
	std::vector<std::string>::const_iterator _itr;
	bool _needSave;
};

#endif
