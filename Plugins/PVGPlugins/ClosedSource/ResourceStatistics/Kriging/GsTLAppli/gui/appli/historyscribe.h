/**********************************************************************
** Author: Ting Li
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

// $Log: historyscribe.h,v $
// Revision 4.0  2008/04/23 02:42:06  nico97492
// BOOK RELEASE
//
// Revision 3.0  2007/06/25 01:12:06  nico97492
// NEW RELEASE
//
// Revision 1.2  2007/02/28 18:56:37  lting2001
// changed include path of derived lineedit
//
// Revision 1.1  2006/12/07 01:15:52  lting2001
// *** empty log message ***
//

#ifndef HISTORY_SCRIBE_H
#define HISTORY_SCRIBE_H

#include <GsTLAppli/gui/common.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <vector>

#include <GsTLAppli/utils/lineeditkey.h>

// saves the gstl log in a buffer
class HistoryScribe : public Scribe
{
public:
	HistoryScribe(LineEditKey * p) : _edit(p) {}
	virtual ~HistoryScribe() {}
	virtual void write( const std::string& str, const Channel* ch) {
		_history.push_back(str);
		_edit->reset();
	}

	const std::vector<std::string> & history() const { return _history; }

private:
	std::vector<std::string> _history;
	LineEditKey * _edit;
};

#endif
