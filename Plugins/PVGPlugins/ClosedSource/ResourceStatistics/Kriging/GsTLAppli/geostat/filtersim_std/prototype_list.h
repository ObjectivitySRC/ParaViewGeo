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


#ifndef __filtersim_prototype_list_H__
#define __filtersim_prototype_list_H__

#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/rgrid_neighborhood.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>
#include <GsTLAppli/math/random_numbers.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/geostat/utilities.h>

#include "filters.h"
#include "distance.h"
#include "pattern.h"
#include "prototype.h"
#include "prototype_help.h"

#include <list>
#include <sstream> 


/*
 * template class PrototypeList
 * its main function is to find the best prototype
 * which has the smallest distance to the data event
 */
template 
< 
    class Prototype,    // prototype type
    class InitializePrototypeList,  // initialization type
    class SplitPrototype,   // split type
    class Distance      // distance function
>
class GEOSTAT_DECL PrototypeList
{
public:
    typedef typename list< Prototype >::iterator ListItr;
    typedef typename Prototype::pixel_type pixel_pattern_type;
    typedef typename Prototype::proportion_type proportion_type;
    typedef typename Prototype::mean_prop_type mean_prop_type;

public:
    // constructor
    // filter_weight must be created in filtersim, its size is
    //      (nb_facies-1)*filter :  if nb_facies > 2 && treat_cate_as_cont_=0
    //      filter               :  if nb_facies > 2 or treat_cate_as_cont_=1
    PrototypeList( RGrid* TI_grid, Window_neighborhood* neighbors, 
                   Window_neighborhood* patch_neighbors, ScoresType& score,
                   vector<float>& filter_weight, int nfacies, int cmin, int nbins, int nbins_2nd );
    PrototypeList(){}

    ~PrototypeList(){};

    void create_prototype_list();
    void create_prototype_indicator();

    list< Prototype >& get_prototype_list() { return prototypes_; }
    int get_prototype_bins() { return nbins_; }
    int get_prototype_bins_second() { return nbins_2nd_; }
    int get_nb_facies() { return nfacies_; }

    // find the prototype which is closest to the data event
    Prototype& find_closet_prototype( vector<float>& dev_score);    // from DEV score
    Prototype& find_closet_prototype( pixel_pattern_type& pattern, vector<float>& weight);  // from patten pixel

    void clean_prototype_pattern();
    void create_prototype_pattern();

private:
    void clean_score_value();

private:
    RGrid* TI_grid_;
    Window_neighborhood* neighbors_;
    Window_neighborhood* patch_neighbors_;
    vector<float> filter_weight_;

    int nbins_;
    int nbins_2nd_;
    int nb_parent_proto_;
    int nfacies_;
    int cmin_;
    int nb_templ_;

    // parent prototype list
    list< Prototype > prototypes_;
    // for the purpose of secondary prototype searching
    vector< list< Prototype > > child_prototypes_;

private:
    InitializePrototypeList initialization_;
    SplitPrototype split_;
    Distance distance_;

    Rand48_generator gen_;
};


/*
 * constructor function to initialize prototype list
 */
template 
< 
    class Prototype,
    class InitializePrototypeList, 
    class SplitPrototype, 
    class Distance
>
PrototypeList<Prototype, InitializePrototypeList, SplitPrototype, Distance>::
PrototypeList( RGrid* TI_grid, Window_neighborhood* neighbors, 
               Window_neighborhood* patch_neighbors, ScoresType& score,
               vector<float>& filter_weight, int nfacies, int cmin, int nbins, int nbins_2nd )
    : TI_grid_(TI_grid), neighbors_(neighbors), 
      patch_neighbors_(patch_neighbors), filter_weight_(filter_weight) 
{
    nfacies_ = nfacies;
    cmin_ = cmin;
    nbins_ = nbins; // # of bins for first partition
    nbins_2nd_ = nbins_2nd; // # of bins for the second partition

    // initialize the root prototype
    prototypes_.push_back( Prototype(TI_grid, neighbors, patch_neighbors, score, filter_weight, nfacies) );

    nb_templ_ = neighbors->max_size();

    initialization_ = InitializePrototypeList( nbins_ );
    split_ = SplitPrototype( cmin_, nbins_2nd_ );
}


/*
 * main function to construct one parent-prototype-list
 * and a set of child-prototype-lists
 */
template 
< 
    class Prototype,
    class InitializePrototypeList, 
    class SplitPrototype, 
    class Distance
>
void PrototypeList<Prototype, InitializePrototypeList, SplitPrototype, Distance>::
create_prototype_list()
{
    const std::clock_t timer_start = std::clock();

    // create parent-prototype-list
    initialization_.execute( prototypes_);
    nb_parent_proto_ = prototypes_.size();
    GsTLcout << "Total number of parent prototypes is " << nb_parent_proto_;

    int nb_children = 0;

    // initialize child-prototype-lists
    list< Prototype > child_prototype;
    for (int i=0; i<nb_parent_proto_; i++)
        child_prototypes_.push_back( child_prototype );

    // create child-prototype-lists
    split_.execute( prototypes_, child_prototypes_ );

    int i=0;
    // calculate the prototype pattern
    for ( ListItr itr = prototypes_.begin(); itr != prototypes_.end(); itr++, i++ )
    {
        //itr->calculate_pattern_prototype();
        nb_children += child_prototypes_[i].size();
    }

    const std::clock_t timer_end = std::clock();
    const float run_time = ( timer_end - timer_start ) / CLOCKS_PER_SEC;

    GsTLcout << ", total number of children prototypes is " << nb_children 
                    << ".  CPU time: " << run_time << " seconds" << gstlIO::end;

    // remove the score values which will not be used in the future
    clean_score_value();
}


/*
 * function to find the best prototype from score
 */
template 
< 
    class Prototype,
    class InitializePrototypeList, 
    class SplitPrototype, 
    class Distance
>
Prototype& PrototypeList<Prototype, InitializePrototypeList, SplitPrototype, Distance>::
find_closet_prototype( vector<float>& dev_score )
{
    // initialize distance vector for parent-prototype-list
    vector<float> dist( nb_parent_proto_, 0.);
    int i=0;

    ListItr itr = prototypes_.begin();
    for (; itr != prototypes_.end(); itr++, i++)
        dist[i] = itr->get_score_distance( dev_score );

    // the best parent-prototype id
    int prototype_id = std::min_element( dist.begin(), dist.end() ) - dist.begin();
    /*
    std::stable_sort( dist.begin(), dist.end() );
    int icountP=std::count( dist.begin(), dist.end(), *(dist.begin()) );
    Rand48_generator gen;
    int prototype_id = floor( gen()*(icountP-1) ); 

    GsTLcout << "selected prototype id: " << prototype_id 
                    << " out of " << nb_parent_proto_
                    << " prototypes." << gstlIO::end;
    */

    // has child prototype list
    if ( child_prototypes_[ prototype_id ].size()>0 )
    {
        list< Prototype >& child = child_prototypes_[ prototype_id ];
        vector<float> child_dist( child.size(), 0.);
        i=0;

        ListItr child_itr = child.begin();
        for ( ; child_itr != child.end(); child_itr++, i++)
              child_dist[i] = child_itr->get_score_distance( dev_score );

        // the best child-prototype id
        prototype_id = std::min_element( child_dist.begin(), child_dist.end() ) - child_dist.begin();
        /*
        std::stable_sort( child_dist.begin(), child_dist.end() );
        int icountC=std::count( child_dist.begin(), child_dist.end(), *(child_dist.begin()) );
        Rand48_generator gen;
        prototype_id = floor( gen()*(icountC-1) ); 
        */

        child_itr = child.begin();
        for (i=0; i<prototype_id; i++)
            child_itr++;

        return *child_itr;
    }
    else    // no child prototype list
    {
        itr = prototypes_.begin();
        for (i=0; i<prototype_id; i++)
            itr++;

        return *itr;
    }
}


/*
 * function to find the best prototype from data event
 */
template 
< 
    class Prototype,
    class InitializePrototypeList, 
    class SplitPrototype, 
    class Distance
>
Prototype& PrototypeList<Prototype, InitializePrototypeList, SplitPrototype, Distance>::
find_closet_prototype( pixel_pattern_type& pattern, vector<float>& weight)
{
    // initialize distance vector for parent-prototype-list
    vector<float> dist( nb_parent_proto_, 0.);
    int i=0;

    ListItr itr = prototypes_.begin();
    for (; itr != prototypes_.end(); itr++, i++)
        dist[i] = itr->get_distance(pattern, weight);

    // the best parent-prototype id
    int prototype_id = std::min_element( dist.begin(), dist.end() ) - dist.begin();
    /*
    std::stable_sort( dist.begin(), dist.end() );
    int icountP=std::count( dist.begin(), dist.end(), *(dist.begin()) );
    Rand48_generator gen;
    int prototype_id = floor( gen()*icountP ); 
    */

    // has child prototype list
    if ( child_prototypes_[ prototype_id ].size()>0 )
    {
        list< Prototype >& child = child_prototypes_[ prototype_id ];
        vector<float> child_dist( child.size(), 0.);
        i=0;

        ListItr child_itr = child.begin();
        for ( ; child_itr != child.end(); child_itr++, i++)
              child_dist[i] = child_itr->get_distance(pattern, weight);

        // the best child-prototype id
        prototype_id = std::min_element( child_dist.begin(), child_dist.end() ) - child_dist.begin();
        /*
        std::stable_sort( child_dist.begin(), child_dist.end() );
        int icountC=std::count( child_dist.begin(), child_dist.end(), *(child_dist.begin()) );
        Rand48_generator gen;
        prototype_id = floor( gen()*icountC ); 
        */

        child_itr = child.begin();
        for (i=0; i<prototype_id; i++)
            child_itr++;

        return *child_itr;
    }
    else    // no child prototype list
    {
        itr = prototypes_.begin();
        for (i=0; i<prototype_id; i++)
            itr++;

        return *itr;
    }
}


/*
 * function to create the parent-prototype-indicator map
 * the patterns within a parent prototype are assinged with
 * a same unique integer number (which is prototype location 
 * in the prototype list).
 */
template 
< 
    class Prototype,
    class InitializePrototypeList, 
    class SplitPrototype, 
    class Distance
>
void PrototypeList<Prototype, InitializePrototypeList, SplitPrototype, Distance>::
create_prototype_indicator()
{
    // create property name
    int level = TI_grid_->current_level();
    ostringstream ostr;  
    ostr << level;
    string prop_name = "prototype_#_mg_" + ostr.str();

    // add a new property
    GsTLGridProperty* prop = geostat_utils::add_property_to_grid(TI_grid_,prop_name);
    
    int prototype_id=0;
    for (ListItr itr = prototypes_.begin(); itr != prototypes_.end(); itr++, prototype_id++)
    {
        ScoresType cur_score = itr->get_score();
        for (int i=0; i<itr->get_replicates(); i++)
        {
            int node_id = cur_score[i].first;
            prop->set_value(prototype_id, node_id);
        }
    }
}

/*
 * main function to clean the score values which will not be used in the future
 */
template 
< 
    class Prototype,
    class InitializePrototypeList, 
    class SplitPrototype, 
    class Distance
>
void PrototypeList<Prototype, InitializePrototypeList, SplitPrototype, Distance>::
clean_score_value()
{
    int i=0;
    for (ListItr itr = prototypes_.begin(); itr != prototypes_.end(); itr++, i++)
    {
        itr->remove_score();

        if ( child_prototypes_[i].size() > 0 )
        {
            for (ListItr child_itr = child_prototypes_[i].begin(); 
                 child_itr != child_prototypes_[i].end(); child_itr++)
            {
                child_itr->remove_score();
            }
        }
    }
}


/*
 * main function to clean the prototype patterns to save memory
 */
template 
< 
    class Prototype,
    class InitializePrototypeList, 
    class SplitPrototype, 
    class Distance
>
void PrototypeList<Prototype, InitializePrototypeList, SplitPrototype, Distance>::
clean_prototype_pattern()
{
    int i=0;
    for (ListItr itr = prototypes_.begin(); itr != prototypes_.end(); itr++, i++)
    {
        itr->clean_prototype_pattern();

        if ( child_prototypes_[i].size() > 0 )
        {
            for (ListItr child_itr = child_prototypes_[i].begin(); 
                 child_itr != child_prototypes_[i].end(); child_itr++)
            {
                child_itr->clean_prototype_pattern();
            }
        }
    }
}


/*
 * main function to create prototype patterns from pattern ids
 */
template 
< 
    class Prototype,
    class InitializePrototypeList, 
    class SplitPrototype, 
    class Distance
>
void PrototypeList<Prototype, InitializePrototypeList, SplitPrototype, Distance>::
create_prototype_pattern()
{
    int i=0;
    for (ListItr itr = prototypes_.begin(); itr != prototypes_.end(); itr++, i++)
    {
        itr->calculate_pattern_prototype();

        if ( child_prototypes_[i].size() > 0 )
        {
            for (ListItr child_itr = child_prototypes_[i].begin(); 
                 child_itr != child_prototypes_[i].end(); child_itr++)
            {
                child_itr->calculate_pattern_prototype();
            }
        }
    }
}


#endif // __filtersim_prototype_list_H__
