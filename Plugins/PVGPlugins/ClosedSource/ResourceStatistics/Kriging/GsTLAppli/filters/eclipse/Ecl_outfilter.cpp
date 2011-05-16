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

#include <GsTLAppli/filters/eclipse/Ecl_outfilter.h>
#include <GsTLAppli/filters/eclipse/EclOutDlg.h>
#include <GsTLAppli/utils/gstl_messages.h>

#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/cartesian_grid.h>
#include <GsTLAppli/grid/grid_model/point_set.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/actions/obj_manag_actions.h>

#include <GsTLAppli/actions/defines.h>

#include <qdialog.h>
#include <qapplication.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qstringlist.h>
#include <qcursor.h>

#include <string>
#include <cctype>
#include <map>
#include <list>

//temporary
#include <algorithm>


Named_interface* Ecl_outfilter::create_new_interface( std::string& ) {
  return new Ecl_outfilter();
}

Ecl_outfilter::Ecl_outfilter() {
	_dlg = new EclOutDlg(qApp->mainWidget());
}

Ecl_outfilter::~Ecl_outfilter() {
}


void Ecl_outfilter::transformReduced(const GsTLGridProperty* src, int x, int y,
			      int z, Type * dest, const Reduced_grid * grid)
{
	int i,j,k,cur,index = 0;

	// flip slices about the x-axis 
	for (i = z-1; i >= 0; --i)   {
		for (j = y-1; j >= 0; --j) {
			cur = j*x+i*x*y;
			for (k = 0; k < x; ++k, ++index) {
				if (grid->isActive(index))
					*(dest+cur+k) = src->get_value(grid->full2reduced(index));
				else
					*(dest+cur+k) = 0;
			}
		}
	}
}

void Ecl_outfilter::transform(const GsTLGridProperty* src, int x, int y,
			      int z, Type * dest)
{
  int i,j,k,cur,index = 0;

  // flip slices about the x-axis 
  for (i = z-1; i >= 0; --i)   {
	  for (j = y-1; j >= 0; --j) {
		  cur = j*x+i*x*y;
		  for (k = 0; k < x; ++k, ++index) {
			  *(dest+cur+k) = src->get_value(index);
		  }
	  }
  }
}

bool Ecl_outfilter::write( std::string outfile, const Geostat_grid* grid,
                            std::string* errors ) 
{
	typedef std::list<std::string>::const_iterator citr; 
    std::vector< std::string > params;
	bool flag = false;

	if (errors->size()) {
		flag = true;
		params = String_Op::decompose_string( *errors, Actions::separator,
					Actions::unique );
		errors->clear();
	}
	else {

		std::list<std::string> plist = grid->property_list();

		for (citr itr = plist.begin(); itr != plist.end(); ++itr)
			_dlg->insertSrcItem( *itr );

		/* pointset is not supported by eclipse */
		if( dynamic_cast<const Point_set*>( grid ) ) {
			errors->append("Point set dataset cannot be exported to eclipse");
			return false;
		}

		QApplication::setOverrideCursor( QCursor(Qt::ArrowCursor) );
		if (_dlg->exec() == QDialog::Rejected){
			QApplication::restoreOverrideCursor();
			return true;
		}
		QApplication::restoreOverrideCursor();
	}

	std::ofstream out( outfile.c_str() );

	if( !out ) {
		if( errors )
			errors->append( "can't write to file: " + outfile );
		return false;
	}

	if (flag) {
		std::map<std::string,std::string> assoc;
		for (int i = 3; i < params.size(); i+=2)
			assoc[params[i]] = params[i+1];

		return this->write(out,grid,assoc);
	}

	return this->write(out,grid, _dlg->getPairs());
}


bool Ecl_outfilter::write( std::ofstream& outfile, const Geostat_grid* grid,
						  map<std::string,std::string> & assoc)
{	
	const Reduced_grid * redgrid;
	int trueSize;
	Type * buf;
	std::map<std::string,std::string>::iterator itr;
	const RGrid * rgrid = dynamic_cast<const RGrid*>(grid);

	if (!rgrid) {
		appli_message("Wrong grid type");
		return false;
	}

	if (redgrid=dynamic_cast<const Reduced_grid*>(grid))
		trueSize = redgrid->trueSize();
	else 
		trueSize = rgrid->size();

	buf = new Type[rgrid->nx()*rgrid->ny()*rgrid->nz()];


	for (itr = assoc.begin(); itr != assoc.end(); ++itr) {
		const GsTLGridProperty* p = grid->property(itr->first);
		if (!p) {
			appli_message("Non existing property " << itr->first );
			return false;
		}
		if (redgrid)
			transformReduced(p,rgrid->nx(), rgrid->ny(), rgrid->nz(), buf, redgrid);
		else
			transform(p, rgrid->nx(), rgrid->ny(), rgrid->nz(), buf);
		appli_message("Writing property " << itr->first << "-->" << itr->second);

		outfile << itr->second << std::endl;
		for (int i = 0; i < trueSize; ++i)
			outfile << buf[i] << std::endl;
		outfile << "/" << std::endl << std::endl;
	}

	delete []buf;
	return true;
}

