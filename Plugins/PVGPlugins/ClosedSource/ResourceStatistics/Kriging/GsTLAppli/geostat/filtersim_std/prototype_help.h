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



#ifndef __filtersim_prototype_helper_H__
#define __filtersim_prototype_helper_H__

#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/rgrid_neighborhood.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/math/random_numbers.h>
#include <GsTLAppli/utils/string_manipulation.h>

#include <algorithm>

#include "filters.h"
#include "pattern.h"
#include "prototype.h"
#include "partition.h"


/*
 * class InitializePrototypeList
 * to initialze a prorotype list from a root prototype
 */
template< class Splitter, class Prototype >
class GEOSTAT_DECL InitializePrototypeList
{
public:
    typedef typename list< Prototype >::iterator ListItr;

public:
    InitializePrototypeList( int bins=2 ){ splitter_ = Splitter(bins); }
    ~InitializePrototypeList(){}

    void execute( list<Prototype>& prototype_list );

private:
    Splitter splitter_;

    vector< vector<float> > threshold_;
};


/*
 * class SplitPrototype
 * to split a parent prototype into children prototypes
 */
template< class Splitter, class Prototype >
class GEOSTAT_DECL SplitPrototype
{
public:
    typedef typename list< Prototype >::iterator ListItr;

public:
    SplitPrototype( int cmin=10, int nbin=2 ) 
    { 
        optReplicateCutoff = cmin; 
        optVarianceCutoff = 0.95;
        splitter_ = Splitter(nbin); 
    }

    ~SplitPrototype(){};

    void execute( list<Prototype>& prototype_list );
    void execute( list<Prototype>& prototype_list, 
                  vector< list< Prototype > >& child_prototype_list );

private:
    void getCutoffValueFromPrototypes(list<Prototype>& prototype_list);
    bool findPrototypeToSplit( list<Prototype>& prototype_list, 
                               vector< ListItr >& splitLocItr, vector< int >& splitLoc );
    void splitOneParentPrototype( list<Prototype>& prototype_list, const ListItr split_itr);

private:
    Splitter splitter_;

    int optReplicateCutoff;     // nb_replicates>optReplicateCutoff will be split
    float optVarianceCutoff;    // Variance>optVarianceCutoff will be split
};


/*
 * function to split one prototype into child prototypes
 */
template< class Splitter, class Prototype > 
void InitializePrototypeList<Splitter, Prototype>::
execute( list<Prototype>& prototype_list ) 
{
    // for the purpose of creating prototypes
    ListItr itr = prototype_list.begin();

    int nb_facies = itr->get_nb_facies();
    RGrid*  TI_grid = itr->get_grid();
    vector<float> prototype_weight = itr->get_prototype_weight();
    Window_neighborhood* neighbors = itr->get_neighbor();
    Window_neighborhood* patch_neighbors = itr->get_patch_neighbor();

    vector< ScoresType > grouped_score;
    splitter_.execute( *itr, grouped_score );

    prototype_list.clear();
    for ( int i=0; i<grouped_score.size(); i++ )
    {
        Prototype proto( TI_grid, neighbors, patch_neighbors, grouped_score[i], prototype_weight, nb_facies);
        prototype_list.push_back( proto );
    }

    for (ListItr itr = prototype_list.begin(); itr != prototype_list.end(); itr++)
    {
        itr->calculate_pattern_prototype();
        itr->calculate_prototype_sharpness();
        //itr->calculate_score_variance();
    }
}


/*
 * Find the variance cutoff.
 * The prototypes with variance higher than 'optVarianceCutoff' might be split
 *
 *                NOT used when use sharpness as split criteria
 */
template< class Splitter, class Prototype >
void SplitPrototype< Splitter, Prototype >::
getCutoffValueFromPrototypes(list<Prototype>& prototype_list)
{
    vector<float> vVariance;

    int nb_replicates = prototype_list.size();

    for ( ListItr itr = prototype_list.begin(); itr != prototype_list.end(); itr++ )
    {
        vVariance.push_back( itr->get_prototype_variance() );
    }

    sort( vVariance.begin(), vVariance.end() );

    // variance higher than 10 quartile might be split
    optVarianceCutoff = ( vVariance[nb_replicates-1] -vVariance[0] ) * 0.1 + vVariance[0];
}


/*
 * function to find prototypes which need to be split
 */
template< class Splitter, class Prototype >
bool SplitPrototype< Splitter, Prototype >::
findPrototypeToSplit( list<Prototype>& prototype_list, 
                      vector< ListItr >& splitLocItr, vector< int >& splitLoc )
{
    splitLocItr.clear();    // erase all privious findings
    splitLoc.clear();       // split location in the prototype list

    int i=0;
    for ( ListItr itr = prototype_list.begin(); itr != prototype_list.end(); itr++, i++ )
    {   
        if ( itr->get_replicates() > optReplicateCutoff &&     
             itr->get_prototype_sharpness() < optVarianceCutoff )
        {
             splitLocItr.push_back(itr);
             splitLoc.push_back(i);
        }
    }

    if ( splitLocItr.size() == 0 )
        return false;
    else
        return true;
}


/*
 * function to split a prototype list into a new prototype list,
 * in which all the prototypes satisfying the conditions:
 *      (1) nb_replicates<=optReplicateCutoff 
 *      (2) Variance<=optVarianceCutoff 
 */     
template< class Splitter, class Prototype >
void SplitPrototype< Splitter, Prototype >::
execute( list<Prototype>& prototype_list )
{
    //getCutoffValueFromPrototypes( prototype_list );

    vector< ListItr > splitLocItr;
    vector< int > splitLoc;
    if( !findPrototypeToSplit(prototype_list, splitLocItr, splitLoc) )   return;

    // for the purpose of creating prototypes
    ListItr itr = prototype_list.begin();
    ListItr end_itr = prototype_list.end()-1;

    int nb_facies = itr->get_nb_facies();
    RGrid*  TI_grid = itr->get_grid();
    vector<float> prototype_weight = itr->get_prototype_weight();
    Window_neighborhood* neighbors = itr->get_neighbor();
    Window_neighborhood* patch_neighbors = itr->get_patch_neighbor();

    while( splitLocItr.size()>0 )
    {
        vector< ScoresType > grouped_score;
        splitter_.execute( *(splitLocItr[0]), grouped_score );
        
        // create new prototypes
        for (int j=0; j<grouped_score.size(); j++)
        {
            Prototype proto( TI_grid, neighbors, patch_neighbors, grouped_score[j], prototype_weight, nb_facies);
            prototype_list.push_back( proto );

            end_itr++;
            end_itr->calculate_pattern_prototype();
            end_itr->calculate_prototype_sharpness();
            //end_itr->calculate_score_variance();

            // for further splitting
            if ( proto.get_replicates() > optReplicateCutoff &&     
                 proto.get_prototype_sharpness() < optVarianceCutoff )
            {
                splitLocItr.push_back( end_itr );
            }
        }

        // remove the prototypes which have been split
        prototype_list.erase( splitLocItr[0] );
        splitLocItr.erase( splitLocItr.begin() );
    }
}


/*
 * function to split a prototype list into child prototype lists,
 * in which all the prototypes satisfying the conditions:
 *      (1) nb_replicates<=optReplicateCutoff 
 *      (2) Variance<=optVarianceCutoff 
 */     
template< class Splitter, class Prototype >
void SplitPrototype< Splitter, Prototype >::
execute( list<Prototype>& prototype_list, 
         vector< list< Prototype > >& child_prototype_list )
{
    //getCutoffValueFromPrototypes( prototype_list );

    vector< ListItr > splitLocItr;
    vector< int > splitLoc;
    if( !findPrototypeToSplit(prototype_list, splitLocItr, splitLoc) )   return;

    // divide parent prototype list into children prototype lists
    for (int i=0; i<splitLocItr.size(); i++)
    {
        list<Prototype>& child_list = child_prototype_list[ splitLoc[i] ];
        splitOneParentPrototype( child_list, splitLocItr[i] );
    }
}


/*
 * function to split one prototype into a prototype list
 * the input prototype is saved in the first position of list
 * the new prototypes are appended to the prototype list
 * after split, the first one in the list will be removed
 */     
template< class Splitter, class Prototype >
void SplitPrototype< Splitter, Prototype >::
splitOneParentPrototype( list<Prototype>& prototype_list, const ListItr split_itr )
{
    // for the purpose of creating new prototypes
    prototype_list.push_back( *split_itr );
    ListItr end_itr = prototype_list.begin();
    vector< ListItr > splitLocItr;
    splitLocItr.push_back( end_itr );

    int nb_facies = split_itr->get_nb_facies();
    RGrid*  TI_grid = split_itr->get_grid();
    vector<float> prototype_weight = split_itr->get_prototype_weight();
    Window_neighborhood* neighbors = split_itr->get_neighbor();
    Window_neighborhood* patch_neighbors = split_itr->get_patch_neighbor();

    // loop until no more split
    while( splitLocItr.size()>0 )
    {
        vector< ScoresType > grouped_score;
        splitter_.execute( *(splitLocItr[0]), grouped_score );
        
        for (int j=0; j<grouped_score.size(); j++)
        {
            Prototype proto( TI_grid, neighbors, patch_neighbors, grouped_score[j], prototype_weight, nb_facies);
            prototype_list.push_back( proto );

            end_itr++;
            end_itr->calculate_pattern_prototype();
            end_itr->calculate_prototype_sharpness();
            //end_itr->calculate_score_variance();

            // for further splitting
            if ( proto.get_replicates() > optReplicateCutoff &&    
                 proto.get_prototype_sharpness() < optVarianceCutoff &&
                 grouped_score.size() > 1 )
            {
                splitLocItr.push_back( end_itr );
            }
        }

        // remove the prototypes which have been split
        prototype_list.erase( splitLocItr[0] );
        splitLocItr.erase( splitLocItr.begin() );
    }
}


#endif // __filtersim_prototype_helper_H__
