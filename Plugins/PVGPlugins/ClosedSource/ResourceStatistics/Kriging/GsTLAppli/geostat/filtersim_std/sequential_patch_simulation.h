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


#ifndef __filtersim_sequential_patch_simulation__
#define __filtersim_sequential_patch_simulation__

#include <vector>

/*
 * Utility function
 * replace prototype with data event at the nodes which is 
 * indicated as hard data or frozen data
 */
inline 
void MergeDataToPrototype( vector< vector<float> >& dev, 
                           vector< vector<float> >& new_dev, vector<int>& data_type )
{
    for (int i=0; i<dev.size(); i++)
    {
        for (int j=0; j<dev[0].size(); j++)
        {
            if ( data_type[j]==1 )
                new_dev[i][j] = dev[i][j];
        }
    }
}


/*
 * sequential patch simulatoin function for 
 * categorical variable without soft data
 */
template
<
  class GeovalueIterator,
  class PrototypeList,     // prototype_list
  class Patcher,              // patch
  class DEV_Finder,      // DEV
  class Servosystem,     // servosystem
  class Paster,               // paste pattern
  class PMapping,         // pattern id mapping
  class Notifier              // progress notifier
>
inline int 
sequential_patch_simulation_categorical(
		      GeovalueIterator begin, GeovalueIterator end,
		      PrototypeList& estim,                         // prototype_list
		      Servosystem& samp,                          // servosystem
              DEV_Finder& find_data_event,          // dev
              Patcher* patcher,                                // patch
              Paster& pattern_paster,                       // paster
              PMapping& mapping,                         // pattern_id mapping
              Notifier* notifier                                  // progress notifier
		      ) 
{
    int ok = 0;
    int nb_facies = estim.get_nb_facies();

    mapping.reset_mapping_table();

    // loop over all the nodes to be simulated
    for(; begin != end; begin++) 
    {
        if( patcher->is_node_patched( *begin ) ) continue;

        vector< vector<float> > dev;
        vector<float> dev_weight;
        int pattern_id;

        // find data event, the best prototype and the best pattern
        if ( find_data_event( begin, dev, dev_weight ) )
            pattern_id = samp( estim.find_closet_prototype(dev, dev_weight) );
        else
            pattern_id = samp( estim.get_prototype_list() );

        // paste the found pattern into simulation grid
        vector<float> histogram( nb_facies, 0. );
        int nb_patched;
        pattern_paster( begin, pattern_id, histogram, nb_patched );

        // update the servosystem
        // no more servosystem
        //samp.update_current_histogram( histogram, nb_patched );

        // add the mapping between simulation node_id and the pattern_id
        mapping.add_mapping( begin->node_id(),  pattern_id );
        //std::cout << "     node_id: " << begin->node_id() << ", pattern_id: " << pattern_id << std::endl;

        ok++;

        if( !notifier->notify() ) return -1;
    }
    
    return ok;
}


/*
 * sequential patch simulatoin function for 
 * continuous variable without soft data
 */
template
<
  class GeovalueIterator,
  class PrototypeList,         // prototype_list
  class Patcher,                  // patch
  class DEV_Finder,          // DEV
  class Servosystem,          // servosystem
  class Paster,                    // paste pattern
  class PMapping,              // pattern id mapping
  class Notifier                   // progress notifier
>
inline int 
sequential_patch_simulation_continuous(
		      GeovalueIterator begin, GeovalueIterator end,
		      PrototypeList& estim,                                 // prototype_list
		      Servosystem& samp,                                  // servosystem
              DEV_Finder& find_data_event,                  // dev
              Patcher* patcher,                                        // patch
              Paster& pattern_paster,                              // paster
              PMapping& mapping,                                // pattern_id mapping
              Notifier* notifier                                         // progress notifier
		      ) 
{
    int ok = 0;

    mapping.reset_mapping_table();

    // loop over all the nodes to be simulated
    for(; begin != end; begin++) 
    {
        if( patcher->is_node_patched( *begin ) ) continue;

        vector<float> dev;
        vector<float> dev_weight;
        int pattern_id;

        // find data event, the best prototype and the best pattern
        if ( find_data_event( begin, dev, dev_weight ) )
            pattern_id = samp( estim.find_closet_prototype(dev, dev_weight) );
        else
            pattern_id = samp( estim.get_prototype_list() );

        // paste the found pattern into simulation grid
        float pattern_mean( 0. );
        int nb_patched;
        pattern_paster( begin, pattern_id, pattern_mean, nb_patched );

        // update the servosystem
        // no more servosystem
        //samp.update_current_histogram( pattern_mean, nb_patched );

        // add the mapping between simulation node_id and the pattern_id
        mapping.add_mapping( begin->node_id(),  pattern_id );
        //std::cout << "     node_id: " << begin->node_id() << ", pattern_id: " << pattern_id << std::endl;

        ok++;

        if( !notifier->notify() ) return -1;
    }
    
    return ok;
}

// -------------------------------------------------


/*
 * sequential patch simulatoin function for 
 * categorical variable with soft data
 */
template
<
  class GeovalueIterator,
  class PrototypeList,             // prototype_list
  class Patcher,                      // patch
  class DEV_Finder,              // DEV
  class Servosystem,             // servosystem
  class Soft_DEV_Finder,     // soft DEV
  class Updater,                     // soft data
  class Paster,                       // paste pattern
  class PMapping,                 // pattern id mapping
  class Notifier                      // progress notifier
>
inline int 
sequential_patch_simulation_categorical_2(
		      GeovalueIterator begin, GeovalueIterator end,
		      PrototypeList& estim,                                     // prototype_list
		      Servosystem& samp,                                      // servosystem
              Updater& updater,                                          // soft data
              DEV_Finder& find_data_event,                      // dev
              Soft_DEV_Finder& find_soft_data_event,      // soft dev
              Patcher* patcher,                                            // patch
              Paster& pattern_paster,                                   // paster
              PMapping& mapping,                                     // pattern_id mapping
              Notifier* notifier                                              // progress notifier
		      ) 
{
    int ok = 0;
    int nb_facies = estim.get_nb_facies();

    mapping.reset_mapping_table();

    for(; begin != end; begin++) 
    {
        if( patcher->is_node_patched( *begin ) ) continue;

        vector< vector<float> > dev;
        vector< vector<float> > soft_dev;
        vector<float> dev_weight;
        vector<int> data_type;
        int pattern_id;

        find_soft_data_event( begin, soft_dev);
 
        // find data event, the best prototype and the best pattern
        //if ( find_data_event( begin, dev, dev_weight, data_type ) )
        if ( find_data_event( begin, dev, dev_weight ) )
        {
            // first search
            vector< vector<float> > new_dev = 
                ( estim.find_closet_prototype(dev, dev_weight) ).get_prototype_pattern();

            // merge dev with the found prototype
            //MergeDataToPrototype( dev, new_dev, data_type );

            // use Tau model to update data event
            updater(new_dev, soft_dev);
            // second search
            pattern_id = samp( estim.find_closet_prototype(new_dev, dev_weight) );
        }
        else
            pattern_id = samp( estim.find_closet_prototype(soft_dev, dev_weight) );

        // paste the found pattern into simulation grid
        vector<float> histogram( nb_facies, 0. );
        int nb_patched;
        pattern_paster( begin, pattern_id, histogram, nb_patched );

        // update the servosystem
        // no more servosystem
        //samp.update_current_histogram( histogram, nb_patched );

        // add the mapping between simulation node_id and the pattern_id
        mapping.add_mapping( begin->node_id(),  pattern_id );

        ok++;

        if( !notifier->notify() ) return -1;
    }
    
    return ok;
}


/*
 * sequential patch simulatoin function for 
 * continuous variable with soft data
 */
template
<
  class GeovalueIterator,
  class PrototypeList,                        // prototype_list
  class Patcher,                                 // patch
  class DEV_Finder,                         // DEV
  class Servosystem,                         // servosystem
  class Soft_DEV_Finder,                 // soft DEV
  class Paster,                                   // paste pattern
  class PMapping,                             // pattern id mapping
  class Notifier                                  // progress notifier
>
inline int 
sequential_patch_simulation_continuous_2(
		      GeovalueIterator begin, GeovalueIterator end,
		      PrototypeList& estim,                                                 // prototype_list
		      Servosystem& samp,                                                  // servosystem
              DEV_Finder& find_data_event,                                  // dev
              Soft_DEV_Finder& find_soft_data_event,                  // soft dev
              Patcher* patcher,                                                        // patch
              Paster& pattern_paster,                                              // paster
              PMapping& mapping,                                                // pattern_id mapping
              Notifier* notifier                                                         // progress notifier
		      ) 
{
    int ok = 0;

    mapping.reset_mapping_table();

    for(; begin != end; begin++) 
    {
        if( patcher->is_node_patched( *begin ) ) continue;

        vector< float > soft_dev;
        vector<float> dev_weight;

        // find data event, the best prototype and the best pattern
        find_soft_data_event( begin, soft_dev);
        find_data_event( begin, soft_dev, dev_weight );

        int pattern_id = samp( estim.find_closet_prototype(soft_dev, dev_weight) );

        // paste the found pattern into simulation grid
        float pattern_mean( 0. );
        int nb_patched;
        pattern_paster( begin, pattern_id, pattern_mean, nb_patched );

        // update the servosystem
        // no more servosystem
        //samp.update_current_histogram( pattern_mean, nb_patched );

        // add the mapping between simulation node_id and the pattern_id
        mapping.add_mapping( begin->node_id(),  pattern_id );

        ok++;

        if( !notifier->notify() ) return -1;
    }
    
    return ok;
}


// ---------------------------------------------------
/*
 * sequential patch simulatoin function 
 * when perform simulation using filter scores
 */
template
<
  class GeovalueIterator,
  class PrototypeList,                         // prototype_list
  class Patcher,                                  // patch
  class DEV_score_Finder,                // DEV
  class Servosystem,                          // servosystem
  class Paster,                                    // paste pattern
  class PMapping,                              // pattern id mapping
  class Notifier                                   // progress notifier
>
inline int 
sequential_patch_simulation_categorical_score_based(
		      GeovalueIterator begin, GeovalueIterator end,
		      PrototypeList& estim,                                         // prototype_list
		      Servosystem& samp,                                          // servosystem
              DEV_score_Finder& find_data_event_score,      // dev_score
              Patcher* patcher,                                                // patch
              Paster& pattern_paster,                                       // paster
              PMapping& mapping,                                         // pattern_id mapping
              Notifier* notifier                                                  // progress notifier
		      ) 
{
    int ok = 0;
    int pattern_id;
    int nb_facies = estim.get_nb_facies();

    // loop over all the nodes to be simulated
    for(; begin != end; begin++) 
    {
        if( patcher->is_node_patched( *begin ) ) continue;

        // find data event score, the best prototype and the best pattern
        vector<float> dev_score;
        if ( find_data_event_score( begin, dev_score, pattern_id ) )
            pattern_id = samp( estim.find_closet_prototype( dev_score ) );

        if ( pattern_id < 0 )   continue;

        // paste the found pattern into simulation grid
        vector<float> histogram( nb_facies, 0. );
        int nb_patched;
        pattern_paster( begin, pattern_id, histogram, nb_patched );

        // update the servosystem
        // no more servosystem
        //samp.update_current_histogram( histogram, nb_patched );

        // add the mapping between simulation node_id and the pattern_id
        mapping.add_mapping( begin->node_id(),  pattern_id );

        ok++;

        if( !notifier->notify() ) return -1;
    }
    
    return ok;
}


template
<
  class GeovalueIterator,
  class PrototypeList,                         // prototype_list
  class Patcher,                                  // patch
  class DEV_score_Finder,                // DEV
  class Servosystem, 
  class Paster,                                    // paste pattern
  class PMapping,                              // pattern id mapping
  class Notifier                                   // progress notifier
>
inline int 
sequential_patch_simulation_continuous_score_based(
		      GeovalueIterator begin, GeovalueIterator end,
		      PrototypeList& estim,                                             // prototype_list
              Servosystem& samp,                                              // servosystem
              DEV_score_Finder& find_data_event_score,          // dev_score
              Patcher* patcher,                                                    // patch
              Paster& pattern_paster,                                           // paster
              PMapping& mapping,                                             // pattern_id mapping
              Notifier* notifier                                                     // progress notifier
		      ) 
{
    int ok = 0;
    int pattern_id;

    for(; begin != end; begin++) 
    {
        if( patcher->is_node_patched( *begin ) ) continue;

        // find data event score, the best prototype and the best pattern
        vector< float > dev_score;
        if ( find_data_event_score( begin, dev_score, pattern_id ) )
            pattern_id = samp( estim.find_closet_prototype( dev_score ) );

        if ( pattern_id < 0 )   continue;
        
        float pattern_mean( 0. );
        int nb_patched;
        pattern_paster( begin, pattern_id, pattern_mean, nb_patched );

        // update the servosystem
        // no more servosystem
        //samp.update_current_histogram( pattern_mean, nb_patched );

        // add the mapping between simulation node_id and the pattern_id
        mapping.add_mapping( begin->node_id(),  pattern_id );

        ok++;

        if( !notifier->notify() ) return -1;
    }
    
    return ok;
}


#endif  // __filtersim_sequential_patch_simulation__
