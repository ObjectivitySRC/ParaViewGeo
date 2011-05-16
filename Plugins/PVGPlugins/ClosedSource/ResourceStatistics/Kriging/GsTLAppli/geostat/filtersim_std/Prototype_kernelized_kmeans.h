/**********************************************************************
** Author: Jianbing Wu, Alexandre Boucher
** Contributor: Tuanfeng Zhang
**
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "geostat" module of the Stanford Geostatistical 
** Earth Modeling Software (SGEMS)
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

#ifndef __prototype_kernelized_kmeans_H__
#define __prototype_kernelized_kmeans_H__

#include <GsTLAppli/geostat/common.h>
#include <vector>
#include <list>
#include <algorithm>

#include "kernelized_kmeans.h"
#include "distance_kernels.h"


/*
 * class Prototype_kernelized_kmeans
 * Not used in current filtersim version
 */
/*
template< class DistanceKernelT = radial_kernel >
class GEOSTAT_DECL Prototype_kernelized_kmeans
{
public:
	typedef std::pair< int, std::vector<float> > OneScoreType;
	typedef std::vector< OneScoreType > ScoresType; 

public:
	Prototype_kernelized_kmeans(){};	
	Prototype_kernelized_kmeans(DistanceKernelT K) : K_(K){};
	Prototype_kernelized_kmeans(int k_groups) : k_(k_groups){ K_=DistanceKernelT(); }
	~Prototype_kernelized_kmeans(){};

	template< class PrototypeType > 
	bool execute( PrototypeType& proto, std::vector< ScoresType >& grouped_score  ); 

private :
	DistanceKernelT K_;
	int k_;
};
*/


/*
 * operator function of kernelized kmean classification
 */
/*
template< class DistanceKernelT >
template< class PrototypeType >
bool Prototype_kernelized_kmeans<DistanceKernelT>::
execute( PrototypeType& proto, std::vector< ScoresType >& grouped_score  )
{
	Kernelized_kmeans< OneScoreType > kernel_kmeans;

	kernel_kmeans(proto.get_score(),grouped_score,k_,K_);

	return true;
}
*/

/*
 * class Prototype_kmeans
 * to divide one prototype into a set of sub-prototypes
 */
class GEOSTAT_DECL Prototype_kmeans
{
public:
	typedef std::pair< int, std::vector<float> > OneScoreType;
	typedef std::vector< OneScoreType > ScoresType; 

public:
	Prototype_kmeans( ){};
	Prototype_kmeans(int k_groups):k_(k_groups){};
	~Prototype_kmeans(){};

	template< class PrototypeType > 
	bool execute( PrototypeType& proto, std::vector< ScoresType >& grouped_score  ); 

private :
	int k_;
};


/*
 * operator function of classification
 */
template< class PrototypeType >
bool Prototype_kmeans::
execute( PrototypeType& proto, std::vector< ScoresType >& grouped_score  )
{
	Kmeans< OneScoreType > kmeans;

	kmeans(proto.get_score(),grouped_score,k_);

	return true;
}


#endif


	












