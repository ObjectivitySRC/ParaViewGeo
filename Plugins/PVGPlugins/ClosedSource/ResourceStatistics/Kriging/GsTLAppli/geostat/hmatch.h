/**********************************************************************
** Author: Ting Li
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "geostat" module of the Geostatistical Earth
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

// $Log: hmatch.h,v $
// Revision 4.0  2008/04/23 02:42:02  nico97492
// BOOK RELEASE
//
// Revision 3.0  2007/06/25 01:11:59  nico97492
// NEW RELEASE
//
// Revision 1.3  2007/06/04 06:15:27  nico97492
// Fixes to compile on Ubuntu 7 - g++ 4.1.2
//
// Revision 1.2  2007/02/13 00:23:57  nico97492
// corrected the way Python.h is included (_DEBUG should not be re-defined if
// it wasn't defined in the first place)
//
// Revision 1.1  2006/12/06 22:06:03  lting2001
// History match plugin
//

#ifndef HMATCH_H
#define HMATCH_H

//#ifdef _DEBUG
//#undef _DEBUG
//#include <Python.h>
//#define _DEBUG
//#else
//#include <Python.h>
//#endif
#include <Python.h>

#include <GsTLAppli/geostat/geostat_algo.h> 
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/appli/project.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <string>
#include <set>

// for Brent's optimization method
#define CGOLD 0.3819660
#define ZEPS 1.0e-10
#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);

//for random number generator
#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)



struct ltstr
{
	bool operator()(const std::string & s1, const std::string & s2) const
	{
		return s1 < s2;
	}
};

class HMatch : public Geostat_algo 
{
public:

	HMatch() 
	{
		_initRandom = -43;
	}

	~HMatch() {}
	
			
	virtual bool initialize( const Parameters_handler* parameters, 
		Error_messages_handler* errors ); 

	virtual int execute(GsTL_project * p = 0);
	virtual std::string name() const { return "PPM"; } 

public: 
	static Named_interface* create_new_interface( std::string& ); 

private:

	bool _useSgemsAlgo;

	//bool _hmFirstRun;

	// number of regions we perturb separately
	int _numRegions;

	// max number of inner loops
	int _innerLoop;

	int _maxOuterLoop;

	// use default optimization routine(Brent's method)
	bool _useBrent;

	// user supplied objective function
	PyObject * _pMainFunc;

	// for importing from Python scripts
	PyObject *_pMod, *_pinst, *_baseMod;

	PyObject * _pOpFunc;

	// tolerance for Brent's method
	float _brentTol;

	// facies in input
	int _numFacies;

	// termination condition for history matching loop
	float _tolerance;

	// initial Rd
	double _initRd;

	// initializes the random number generator
	long _initRandom;  

	//GsTL_project* _proj; 

	std::string mainFunc(bool isFirstRun, std::string bestReal, 
		vector<double> & Rds, std::string seed, vector<double> & objs, bool);

	/* this code is borrowed with minor changes from class 
	   parameters_handler_impl.
	   The empty string "", when used for the 3rd argument, is a special code.
	   It means the attribute specified in the 2nd argument is to be retrieved.
	   Otherwise, it is to be replaced by the new value.
	*/
	//std::string access_value( QDomElement,const std::string&, const std::string& ) ;

	double brent(float , float , float , float , std::string &, double , std::string );

	double innerLoop(std::string &R, double best, std::string seed);

	std::string randomNumGenerator(long *);

	bool readMemberVars(const Parameters_handler* );

	double overallObjFunc(vector<double> & v);


};

#endif
