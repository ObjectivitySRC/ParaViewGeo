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

#ifndef __kernelized_kmeans_H__
#define __kernelized_kmeans_H__

#include <GsTLAppli/geostat/common.h>
#include <GsTL/math/random_number_generators.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include "distance_kernels.h"

#define TOL 1e-7
#define MAX_ITER 500



template< class Vec >
bool is_cluster_empty(Vec vec){
	return vec.empty();
}


template< class Scores >
bool less_than_score(Scores s1, Scores s2){
	//gstl_assert(s1.second.size() == s2.second.size() );
	for(int i=0; i<s1.second.size(); ++i )
		if(s1.second[i] > s2.second[i] ) return false;
	return true;
}

template< class Scores >
bool equal_score(Scores s1, Scores s2){
	//gstl_assert(s1.second.size() == s2.second.size() );
	for(int i=0; i<s1.second.size(); i++ )
		if(s1.second[i] != s2.second[i] ) return false;
	return true;
}

/*
 * class Kmeans
 * the based class perform K-Mean clusters classification on score vector
 */
template < class XResponse >
class GEOSTAT_DECL Kmeans 
{
public:
	typedef std::vector<XResponse> clusterT;
	typedef std::vector< float > meansType;
	typedef std::vector< float > meansT;

public:
	Kmeans(){};
	~Kmeans(){};

	bool operator()(clusterT& x,  std::vector<clusterT>& new_cluster,int n_clusters );

protected :
	std::vector< std::vector<float> > initialize_cluster(clusterT& x,int& n_clusters);
};



/*
 * function to initialize the centroid location for kmean clusters
 */
template< class XResponse >
std::vector< std::vector<float> > Kmeans<XResponse>::
initialize_cluster(clusterT& x, int& n_clusters)
{
	clusterT x_copy(x);

    // sort the input scores, and only keep the unique ones
    std::stable_sort(x_copy.begin(), x_copy.end(), less_than_score<typename clusterT::value_type> );
    typename clusterT::iterator it_unique_last = std::unique(x_copy.begin(), x_copy.end(), equal_score< typename clusterT::value_type> );
    
    STL_generator gen;
    std::random_shuffle(x_copy.begin(), it_unique_last, gen );

    // reset the number of clusters if necessary
	if( std::distance(x_copy.begin(),it_unique_last) < n_clusters )  
		n_clusters = std::distance(x_copy.begin(),it_unique_last)/2;

    n_clusters = max(2, n_clusters);    // at least has two clusters

    // calculate the centroid locations
	std::vector<meansT> means_groups(n_clusters);
 
	for(int i=0; i< n_clusters; i++ )
		means_groups[i] = x_copy[i].second;

	return means_groups;
}



/*
 * main operator function of general kmean cluster algorithm
 */
template< class XResponse>
bool Kmeans<XResponse>::
operator ()(clusterT& x, std::vector<clusterT>& kmeans_clusters, int nb_clusters )
{
    int n_clusters = nb_clusters;
    int nb_filter = x[0].second.size();
    typedef typename clusterT::iterator it_xT;

    // initialize the centroid locations
	std::vector<meansT> means_clusters = initialize_cluster(x,n_clusters);
	kmeans_clusters.resize(n_clusters);
	int id_cl;
    int repl;

    // declare an indicator to record the cluster index
    int n_replicates = x.size();
    std::vector<int> cluster_indicator(n_replicates,-1);
    bool no_more_switch;

	for (int iter=0; iter<MAX_ITER; iter++)
	{
        repl=0;
        no_more_switch = true;

		std::vector<float> dists(n_clusters);
		meansT mean( nb_filter );

        // the new centroid location for current classification
		std::vector<meansT> new_means_cl(n_clusters,mean);

		std::vector<int> n_element_cluster(n_clusters);
		for(it_xT it_x=x.begin(); it_x != x.end(); ++it_x, repl++)
		{
            // calculate the distance between each score and the centroid 
            for(int i=0; i<n_clusters; i++ )
            {
				dists[i] = std::inner_product( it_x->second.begin(),it_x->second.end(),
				                               means_clusters[i].begin(),0.0,
                                               std::plus<float>(), square_diff<float> );
            }

            // find the opitmal cluster index
			id_cl = std::distance(dists.begin(), std::min_element(dists.begin(),dists.end()) );

            // check the stop criteria
            if ( no_more_switch && cluster_indicator[repl] != id_cl )
                no_more_switch = false;

            // assign the cluster index to the score
            cluster_indicator[repl] = id_cl;

            // update the centroid
			std::transform( it_x->second.begin(),it_x->second.end(), new_means_cl[id_cl].begin(), 
					        new_means_cl[id_cl].begin(), plus<float>() );

            // record the number of scores in current cluster
			n_element_cluster[id_cl]++;
		}   // end for(it_xT it_x=x.begin(); ...

        // finish classification
        if ( no_more_switch )
            break;

        // re-calculate the centroid locations
		for( int i=0; i<n_clusters; i++ ) 
        {
			if(n_element_cluster[i]>0) 
            {
				for(meansT::iterator it = new_means_cl[i].begin(); it != new_means_cl[i].end(); ++it )
					*it /= n_element_cluster[i];

                // assign the updated centroid location
                new_means_cl[i].swap(means_clusters[i]);
			}
		}
	}   // end for (int iter=0; iter<MAX_ITER; iter++)

    // initialize the output clusters
    repl = 0;
    for(it_xT it_x=x.begin(); it_x != x.end(); ++it_x, repl++)
        kmeans_clusters[ cluster_indicator[repl] ].push_back(*it_x);
    
    // Remove the empty clusters 
    while(true)
    {
      typename std::vector<clusterT>::iterator it = 
	std::find_if( kmeans_clusters.begin(),
		      kmeans_clusters.end(),is_cluster_empty<clusterT>);
        
        if(it == kmeans_clusters.end())     break;
        kmeans_clusters.erase(it);
    }

    if ( no_more_switch )
    {
#if defined ( _DEBUG )
        cout << " # of iterations for k-mean partition is "<< iter+1 << endl;
#endif
        return true;
    }
    else
        return false;
}


// the following has not been implemented yet
/*
template< class XResponse >
class GEOSTAT_DECL Kernelized_kmeans 
{
public:
	typedef std::vector<XResponse> clusterT;
	typedef std::vector<XResponse*> ptr_clusterT;

public:
	Kernelized_kmeans(){};
	~Kernelized_kmeans(){};

	template< class KernelDistanceT >
	bool operator()(clusterT& x, std::vector<clusterT>& new_cluster, int n_clusters, KernelDistanceT K );

protected :
	bool initialize_cluster(clusterT& x, std::vector<ptr_clusterT>& ptr_clusters,int n_clusters);
};


template< class XResponse >
bool Kernelized_kmeans<XResponse>::
initialize_cluster(clusterT& x , std::vector<ptr_clusterT>& ptr_clusters , int n_clusters)
{
	Rand48_generator gen;

	for(int i=0; i< n_clusters; i++ ) {
		ptr_clusterT ptr_cluster;
		ptr_clusters.push_back(ptr_cluster);
		ptr_clusters[i].reserve( static_cast<int>(x.size()/n_clusters) );
	}
	
	std::vector< int > init;
	init.reserve(x.size());
	for(int i=0; i<x.size(); i++ ) init[i] = i;
//	std::iota(init.begin(),init.end(),0);
	std::random_shuffle(init.begin(), init.end()  );

	for(int i=0; i< n_clusters; i++ ) {
		for( int j=i*(x.size()/n_clusters); j<(i+1)*(x.size()/n_clusters); j++ )
		{
			ptr_clusters[i].push_back( &(x[init[j]]) );
		}
	}

	return true;
}

template <class XResponse>
template <class KernelDistanceT>
bool Kernelized_kmeans<XResponse>::operator ()
(clusterT& x, std::vector<clusterT>& kmeans_clusters, int n_clusters, KernelDistanceT K)
{
	std::vector<ptr_clusterT> ptr_clusters;
	initialize_cluster(x,ptr_clusters,n_clusters);
	typedef clusterT::iterator it_xT;
	typedef ptr_clusterT::iterator it_ptr_xT;

	std::vector<float> distK(n_clusters);

	std::vector<ptr_clusterT> new_clusters(n_clusters);
	for(int i=0; i< n_clusters; i++ ) {
//		ptr_clusterT ptr_cluster;
//		ptr_clusters.push_back(ptr_cluster);
		new_clusters[i].reserve( static_cast<int>(x.size()/n_clusters) );
	}
	
	int n_iterations = 0;
	while(true) 
	{
		for(int i=0; i< n_clusters; i++ ) {
			new_clusters[i].clear();
			new_clusters[i].reserve( ptr_clusters[i].size() );
		}
		n_iterations++;
		std::vector<float>  K_lp(n_clusters);
		for(int i=0; i<n_clusters; i++)
		for(it_ptr_xT it_l=ptr_clusters[i].begin(); it_l!= ptr_clusters[i].end(); it_l++)
		for(it_ptr_xT it_p=ptr_clusters[i].begin(); it_p!= ptr_clusters[i].end(); it_p++)
		{
			K_lp[i] += K((*it_l)->second,(*it_p)->second);
		}
			
		for(it_xT it_i=x.begin(); it_i != x.end(); ++it_i)
		{
			float K_ii = K(it_i->second,it_i->second);
			//std::vector<float>  K_il(n_clusters);
			float K_il = 0.0;
			for(int i=0; i<n_clusters; i++) {
                for(it_ptr_xT it_l=ptr_clusters[i].begin(); it_l!= ptr_clusters[i].end(); it_l++)
				{
					K_il += K(it_i->second,(*it_l)->second);
				}
				float size = ptr_clusters[i].size();
				distK[i] = K_ii -2.0/size*K_il + K_lp[i]/size/size;
			}
			int argmin = distance(distK.begin(), std::min_element(distK.begin(),distK.end()));
			new_clusters[argmin].push_back( &(*it_i) );
		}
		ptr_clusters.swap(new_clusters);
		//Test for convergeance, to be done properly with 
		if( ptr_clusters == new_clusters || n_iterations > 100 ) break;
	}
	kmeans_clusters.clear;
	kmeans_clusters.reserve(ptr_clusters.size() );
	for( int i=0; i<ptr_clusters.size() ; i++ ){
		clusterT  kmean_cluster;
		kmean_cluster.reserve( ptr_clusters[i].size() );
		kmeans_clusters.push_back(kmean_cluster);
		for( it_ptr_xT it=ptr_clusters[i].begin(); it!= ptr_clusters[i].end(); it++ )
			kmeans_clusters[i].push_back( *(*it) );
	}

	return true;
}
*/



#endif
