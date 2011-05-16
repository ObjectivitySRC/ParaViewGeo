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

#include <GsTLAppli/filters/eclipse/EclOutDlg.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/appli/manager_repository.h>

#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qtextbrowser.h>
#include <qcheckbox.h>
#include <qmessagebox.h>

#include <iostream>

EclOutDlg::EclOutDlg( QWidget* parent, const char* name ) :
	EclOutDlgBase(parent, name)
{
	QObject::connect( done, SIGNAL(clicked()), 
		this, SLOT( accept() ) );

	QObject::connect( cancel, SIGNAL(clicked()), 
		this, SLOT( reject() ) );
	
	QObject::connect(assoc, SIGNAL(clicked()), this, SLOT(associate()));
}


EclOutDlg::~EclOutDlg(){
}


void EclOutDlg::accept() 
{
	if (_pairs.empty())
		QMessageBox::warning( this, "Missing parameters", 
		"You must select at least one property to save",
		QMessageBox::Ok, QMessageBox::NoButton,
		QMessageBox::NoButton );
	else
		QDialog::accept();
}


void EclOutDlg::associate()
{
	QString S = listSrc->currentText();
	QString C = combo->currentText();
	QString result;

	if (S.isEmpty())
		return;

	if (C.isEmpty()){
		QMessageBox::warning( this, "Missing parameters", 
			"Please select an Eclipse keyword",
			QMessageBox::Ok, QMessageBox::NoButton,
			QMessageBox::NoButton );
		return;
	}

	result = S+"-->"+C;
	listSrc->removeItem(listSrc->currentItem());
	listDest->insertItem(result);
	_pairs[S.ascii()] = C.ascii();
}


