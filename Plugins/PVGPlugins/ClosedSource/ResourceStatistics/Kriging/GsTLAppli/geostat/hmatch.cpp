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

// $Log: hmatch.cpp,v $
// Revision 4.0  2008/04/23 02:42:02  nico97492
// BOOK RELEASE
//
// Revision 3.0  2007/06/25 01:11:59  nico97492
// NEW RELEASE
//
// Revision 1.2  2007/04/25 17:19:34  lting2001
// merged with branch testing_030707
//
// Revision 1.1.2.3  2007/04/20 00:46:56  lting2001
// ppm is removed from sgems.
//
// Revision 1.1.2.2  2007/04/19 19:21:50  lting2001
// *** empty log message ***
//
// Revision 1.1.2.1  2007/04/09 21:10:10  lting2001
// *** empty log message ***
//
// Revision 1.1  2006/12/06 22:06:02  lting2001
// History match plugin
//

#include <GsTLAppli/geostat/hmatch.h>
#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/geostat/parameters_handler_impl.h> 
#include <GsTLAppli/utils/gstl_messages_private.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/gstl_plugins.h>
#include <GsTLAppli/actions/defines.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/grid/grid_model/reduced_grid.h>
 
#include <string>
#include <math.h>   // for pow function
#include <stdio.h>  // for sprintf
#include <fstream>
#include <memory.h> //for memset
#include <stdio.h>
#include <map>

#define INDPREFIX "__INDICATOR7923__"
#define VALIDATE_FUNC(func) \
	if (!func || !PyCallable_Check(func)) {    \
		appli_warning("Not a valid function"); \
		return false;						   \
	}

Named_interface* HMatch::create_new_interface( std::string& ) {
  return new HMatch;
}

// used in Brent's method
inline float SIGN(float a, float b)
{
	if (b >= 0 ) {
		if (a >= 0)
			b=a;
		else
			b = -a;
	}
	else {
		if (a >= 0)
			b = -a;
		else
			b = a;
	}
	return b;
}

map<std::string, double> M;

//-------------------------------------------

bool HMatch::initialize( const Parameters_handler* parameters,
			  Error_messages_handler* errors ) 
{
	Py_Initialize();
	PyRun_SimpleString(""
		"import redirect\n"
		"class CoutLogger:\n"
		"    def __init__(self):\n"
		"        self.buf = []\n"
		"    def write(self, data):\n"
		"        self.buf.append(data)\n"
		"        if data.endswith('\\n'):\n"
		"            redirect.sgems_cout(''.join(self.buf))\n"
		"            self.buf = []\n"
		"\n"
		"class CerrLogger:\n"
		"    def __init__(self):\n"
		"        self.buf = []\n"
		"    def write(self, data):\n"
		"        self.buf.append(data)\n"
		"        if data.endswith('\\n'):\n"
		"            redirect.sgems_cerr(''.join(self.buf))\n"
		"            self.buf = []\n"
		"\n"
		"import sys\n"
		"sys.stdout = CoutLogger()\n"
		"sys.stderr = CerrLogger()\n"
		"");

	if (!readMemberVars(parameters)) {
		return false;
	}
	
	_useBrent = String_Op::to_number<bool>(parameters->value("_useBrent.value"));

	if (!_useBrent) {
		_pOpFunc = PyObject_GetAttrString(_pinst,
										  (char*)parameters->value("_optimization.value").c_str());
		VALIDATE_FUNC(_pOpFunc);
	}

	return true;
}

int HMatch::execute(GsTL_project * proj) 
{	
	int  i;
	std::string bestReal;
	double bestObjFunc;
	vector<double> Rds, objs;

	objs.reserve(_numRegions);
	Rds.reserve(_numRegions);

	for ( i = 0; i < _numRegions; ++i)
		Rds[i] = _initRd;

	bestReal = mainFunc(true, "", Rds, randomNumGenerator(&_initRandom), objs, true); 
	bestObjFunc = overallObjFunc(objs);

	if (bestObjFunc < _tolerance) {
		appli_message("History match before the loop");
		return 0;
	}

	M[bestReal] = bestObjFunc;


	// PPM outer loop
	for (i = 0; i < _maxOuterLoop; ++i) {

		
		bestObjFunc = innerLoop(bestReal, bestObjFunc, 
								randomNumGenerator(&_initRandom));
		if (bestObjFunc < _tolerance)
			break;
	}

	/*
	GsTLcout << "best real is " << bestReal << ": " << bestObjFunc << gstlIO::end;	
	for (map<std::string,double>::iterator itr = M.begin(); itr != M.end(); ++itr)
		GsTLcout << itr->first << "," << itr->second << gstlIO::end;
	*/
	return 0;
}

std::string HMatch::mainFunc(bool isFirstRun, std::string bestReal, vector<double> & Rds,
					  std::string seed, vector<double> & objs, bool outerLoopStart)
{
	PyObject * args, * arg1, * ret, *p1;
	int index = 0;
	char * result;
	double objVal;

	args = PyTuple_New(5);

	//arg 1 : is first run
	PyTuple_SetItem(args, 0, PyBool_FromLong((long)isFirstRun));

	// arg 2: best realization name
	PyTuple_SetItem(args, 1, Py_BuildValue("s", bestReal.c_str()));

	// arg 3: list of Rd
	arg1 = PyList_New(Rds.size());
	for (vector<double>::const_iterator it = Rds.begin(); it != Rds.end(); ++it,++index) {
		PyList_SetItem(arg1, index, PyFloat_FromDouble(*it));
	}
	PyTuple_SetItem(args, 2, arg1);

	// arg 4: seed
	PyTuple_SetItem(args, 3, Py_BuildValue("s", seed.c_str()));

	// arg 5: first outer loop
	PyTuple_SetItem(args, 4, PyBool_FromLong((long)outerLoopStart));

	if (!(ret = PyObject_CallObject(_pMainFunc, args))) {
		PyErr_Print();
		return "";
	}

	PyArg_Parse(PyList_GetItem(ret, 0), "s", &result);

	p1 = PyList_GetItem(ret, 1);
	for (int i = 0; i < _numRegions; ++i) {
		PyArg_Parse(PyList_GetItem(p1,i), "d", &objVal);
		objs[i] = objVal;
	}

	M[result] = overallObjFunc(objs);
	return result;
}

double HMatch::innerLoop(std::string &R, double best, std::string seed)
{
	PyObject *presult, *p1, *p2;
	char * retReal;
	double ret;

	if (_useBrent)
		return brent(0.0, _initRd, 1.0, _brentTol, R, best, seed);
	
	presult = PyObject_CallObject(_pOpFunc, 
		Py_BuildValue("(ffffs)", 0.0, _initRd, 1.0, _brentTol, R.c_str()));
	appli_assert(presult != NULL);

	p1 = PyList_GetItem(presult, 0);  // best object function value
	p2 = PyList_GetItem(presult, 1);  // best Rd

	PyArg_Parse(p1, "d", &ret);
	PyArg_Parse(p2, "s", &retReal);
	R = retReal;

	return ret;
}


bool HMatch::readMemberVars(const Parameters_handler* parameters)
{
	PyObject * pclass, * parg;
	PyObject * fromList = Py_BuildValue("[s]", "*");

	_maxOuterLoop = String_Op::to_number<int>(parameters->value("_maxOuterLoop.value"));
	_innerLoop = String_Op::to_number<int>(parameters->value("_innerLoop.value"));
	_tolerance = String_Op::to_number<float>(parameters->value("_tolerance.value"));
	_initRd = String_Op::to_number<double>(parameters->value("_initRd.value"));
	_numFacies = String_Op::to_number<int>(parameters->value("_numFacies.value"));
	_brentTol = String_Op::to_number<float>(parameters->value("_brentTol.value"));
	_numRegions = String_Op::to_number<int>(parameters->value("_numRegions.value"));
	_useSgemsAlgo = String_Op::to_number<bool>(parameters->value("_useSgemsAlgo.value"));

	_baseMod = PyImport_ImportModule((char*)parameters->value("_script.value").c_str());

	_pMod = PyImport_ImportModuleEx(
			(char *)parameters->value("_script.value").c_str(), NULL, NULL, fromList);

	if (!_pMod){
		appli_warning("Invalid python script");
		PyErr_Print();
		return false;
	}

	pclass = PyObject_GetAttrString(_pMod,(char *)parameters->value("_hmClass.value").c_str());

	// parameters for the constructor
	if (_useSgemsAlgo)
		parg = Py_BuildValue("(ss)", 
			(dynamic_cast<const Parameters_handler_xml*>(parameters)->getDoc()).toString().toLatin1(),
			parameters->value("_sgemsAlgo.value").c_str());
	else
		parg = Py_BuildValue("(ss)", 
			(dynamic_cast<const Parameters_handler_xml*>(parameters)->getDoc()).toString().toLatin1(),
			"");

	// get an instance of the class
	if (!(_pinst = PyEval_CallObject(pclass, parg))) {
		PyErr_Print();
		return false;
	}

	PyObject * r = PyObject_GetAttrString(_pinst, "_initRandom");
	if (r)
		PyArg_Parse(r, "l", &_initRandom);

	_pMainFunc = PyObject_GetAttrString(_pinst, "onePass");

	VALIDATE_FUNC(_pMainFunc);
	return true;
}

double HMatch::overallObjFunc(vector<double> & v)
{
	double ret = 0.0;
	for (int i = 0; i < _numRegions; ++i) {
		ret += v[i];
	}
	return ret;
}

double HMatch::brent(float ax, float bx, float cx, float tol, std::string & bestReal, 
					 double best, std::string seed)

{
	std::string bestSoFar, temp;
	vector<double> a,b,e,fu,fv,fw,fx,tol1,tol2,u,v,w,x,xm;
	double overallObj,p,q,r,etemp,d;

	// initialization
	for (int i = 0; i < _numRegions; ++i) {
		a.push_back(ax < cx ? ax : cx);
		b.push_back(ax > cx ? ax : cx);
		v.push_back(bx);
		w.push_back(bx);
		x.push_back(bx);

		e.push_back(0.0);
		fu.push_back(0.0);
		fv.push_back(0.0);
		fw.push_back(0.0);
		fx.push_back(0.0);
		u.push_back(0.0);
		xm.push_back(0.0);
		tol1.push_back(0.0);
		tol2.push_back(0.0);
	}

	bestSoFar = mainFunc(false, bestReal, x, seed, fw, true);
	fv = fx = fw;
	overallObj = overallObjFunc(fw);

	for (int iter=1;iter<=_innerLoop;iter++) { 
		int done = 0;
		for (int i = 0; i < _numRegions; ++i) {
			xm[i] = 0.5*(a[i]+b[i]);
			tol2[i]=2.0*(tol1[i]=tol*fabs(x[i])+ZEPS);
			if (fabs(x[i]-xm[i]) <= (tol2[i]-0.5*(b[i]-a[i])))  
				done++;
		}

		if (done == _numRegions) {
			if (best <= overallObj) 
				return best;
			
			bestReal = bestSoFar;
			return overallObj;
		}
		for (int i = 0; i < _numRegions; ++i) {
			if (fabs(e[i]) > tol1[i]) { 
				r=(x[i]-w[i])*(fx[i]-fv[i]);
				q=(x[i]-v[i])*(fx[i]-fw[i]);
				p=(x[i]-v[i])*q-(x[i]-w[i])*r;
				q=2.0*(q-r);
				if (q > 0.0) p = -p;
				q=fabs(q);
				etemp=e[i];
				e[i]=d;
				if (fabs(p) >= fabs(0.5*q*etemp) || p <= q*(a[i]-x[i]) || p >= q*(b[i]-x[i]))
					d=CGOLD*(e[i]=(x[i] >= xm[i] ? a[i]-x[i] : b[i]-x[i]));

				else {
					d=p/q; 
					u[i]=x[i]+d;
					if (u[i]-a[i] < tol2[i] || b[i]-u[i] < tol2[i])
						d=SIGN(tol1[i],xm[i]-x[i]);
				}
			} else {
				d=CGOLD*(e[i]=(x[i] >= xm[i] ? a[i]-x[i] : b[i]-x[i]));
			}
			u[i]=(fabs(d) >= tol1[i] ? x[i]+d : x[i]+SIGN(tol1[i],d));
		}

		temp = mainFunc(false, bestReal, u, seed, fu, false);

		for (int i = 0; i < _numRegions; ++i ) {
			if (fu[i] <= fx[i]) { 
				if (u[i] >= x[i]) a[i]=x[i]; else b[i]=x[i]; 
				SHFT(v[i],w[i],x[i],u[i]) 
				SHFT(fv[i],fw[i],fx[i],fu[i])
			} else {
				if (u[i] < x[i]) a[i]=u[i]; else b[i]=u[i];
				if (fu[i] <= fw[i] || w[i] == x[i]) {
					v[i]=w[i];
					w[i]=u[i];
					fv[i]=fw[i];
					fw[i]=fu[i];
				} else if (fu[i] <= fv[i] || v[i] == x[i] || v[i] == w[i]) {
					v[i]=u[i];
					fv[i]=fu[i];
				}
			} 
		}

		if (overallObjFunc(fu) < overallObj) {
			overallObj = overallObjFunc(fu);
			bestSoFar = temp;
		}
	} 
	if (best <= overallObj) 
		return best;
	
	bestReal = bestSoFar;
	return overallObj;
}


std::string HMatch::randomNumGenerator(long *idum)

{
	int j;
	long k;
	std::string val;
	char buf[128];
	static long iy=0;
	static long iv[NTAB];
	float temp, ret;
	if (*idum <= 0 || !iy) { 
		if (-(*idum) < 1) *idum=1; 
		else *idum = -(*idum);
		for (j=NTAB+7;j>=0;j--) { 
			k=(*idum)/IQ;
			*idum=IA*(*idum-k*IQ)-IR*k;
			if (*idum < 0) *idum += IM;
			if (j < NTAB) iv[j] = *idum;
		}
		iy=iv[0];
	}
	k=(*idum)/IQ; 
	*idum=IA*(*idum-k*IQ)-IR*k; 
	if (*idum < 0) *idum += IM; 
	j=iy/NDIV; 
	iy=iv[j]; 
	iv[j] = *idum; 
	if ((temp=AM*iy) > RNMX) ret = RNMX; 
	else ret = temp;

	*idum = -1*(*idum);

	ret *= 1e20;
	sprintf(buf, "%f", ret);
	val = buf;
	return val.substr(0,6);
}


//GEOSTAT_PLUGIN(HMatch);
