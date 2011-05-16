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

#ifndef OINV_MGRID_H
#define OINV_MGRID_H

#include <GsTLAppli/gui/oinv_description/oinv_cgrid.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>

class GUI_DECL Oinv_mgrid : public Oinv_cgrid
{
public:
    static Named_interface* create_new_interface( std::string& ); 

	Oinv_mgrid();
	virtual ~Oinv_mgrid();
    virtual void init( const Geostat_grid* grid ); 
    virtual int add_slice( Oinv::Axis axis, int position, bool visible ); 

protected:
    virtual void refresh();  

private:
	const Reduced_grid * grid_;

};

#endif