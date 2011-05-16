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


#ifndef __filtersim_partition_H__
#define __filtersim_partition_H__

#include <GsTLAppli/geostat/common.h>
#include <algorithm>
#include <vector>
#include <cmath>

#include "filters.h"
#include "pattern.h"

GEOSTAT_DECL 
void calculateScoreBinThreshold( vector<float>& score_value, vector<float>& threshold, int nbins );

/*
 * CrossPartition divide the patterns into equal bins 
 * in each filter space. Maximal size: (bins)^(filter_#)
 */
class GEOSTAT_DECL CrossPartition
{
public:
    CrossPartition( int bins=2 ):bins_(bins) {}
    ~CrossPartition(){}

    // the return vector grouped_score will be used to create new prototypes
    template< class Prototype > 
        void execute( Prototype& proto, vector< ScoresType >& grouped_score );

private:
    void calculateBinThreshold ( const ScoresType& score, vector< vector<float> >& threshold );

private:
    int bins_;
};


/*
 * DefaultSplitter divide a prototype into equal bins,
 * designed as the default splitter for second partition
 */
class GEOSTAT_DECL DefaultSplitter
{
public:
    DefaultSplitter( int bins=2 ):bins_(bins) {}
    ~DefaultSplitter(){}

    // the return vector grouped_score will be used to create new prototypes
    template< class Prototype > 
        void execute( Prototype& proto, vector< ScoresType >& grouped_score );

private:
    template< class Prototype > int findFilterToSplit( Prototype& proto );
    //void calculateBinThreshold ( vector<float>& score_value, vector<float>& threshold );

private:
    int bins_;
};

///---------------------------------------
/*
 * function to divide the current prototype into several sub_prototype
 * the return value is a vector of score which will be used to create
 * child-prorotypes
 */
template< class Prototype > 
void CrossPartition::
execute( Prototype& proto, vector< ScoresType >& grouped_score )
{   
    // get the score from the parent prototype
    ScoresType parent_score = proto.get_score();
    int nscore = proto.get_replicates();
    int nfilter = parent_score[0].second.size();

    // divide into equal bins per filter score
    vector< vector<float> > threshold;
    calculateBinThreshold( parent_score, threshold );    

    vector< vector<int> > grouped_index;  // save score group indicator

    for (int i=0; i<nscore; i++)
    {
        vector<int> tmp_ind( nfilter );
        vector<float>& score_value = parent_score[i].second;

        // find the bin number for current score
        for (int j=0; j<nfilter; j++)
        {
            for (int k=0; k<bins_; k++)
            {
                if( score_value[j]<=threshold[j][k])
                {
                    tmp_ind[j] = k; // bin number
                    break;
                }
            }
        }

        vector< vector<int> >::iterator resultIter;
        resultIter=find(grouped_index.begin(), grouped_index.end(), tmp_ind);
        
        // assign current score a group indicator
        if(resultIter != grouped_index.end())
        {   // this group indicator exists
            int index = resultIter - grouped_index.begin();
            grouped_score[index].push_back( parent_score[i] );
        }
        else
        {   // this group indicator need to created
            grouped_index.push_back(tmp_ind);
            ScoresType one_score;
            one_score.push_back( parent_score[i] );
            grouped_score.push_back(one_score);
        }
    }
}


/*
 * function to find which filter need to split
 */
template< class Prototype >
int DefaultSplitter::findFilterToSplit( Prototype& proto )
{
    vector<float> variance = proto.get_filter_variance();
    return std::max_element(variance.begin(), variance.end()) - variance.begin();
}


/*
 * function to divide the current prototype into several sub_prototype
 * the return value is a vector of score which will be used to create
 * child-prorotypes
 */
template< class Prototype > 
void DefaultSplitter::
execute( Prototype& proto, vector< ScoresType >& grouped_score )
{   
    // get the score from parent prototype
    ScoresType parent_score = proto.get_score();
    int nscore = proto.get_replicates();
    int nfilter = parent_score[0].second.size();

    // find the filter to be split
    int split_filter = findFilterToSplit( proto );

    // save the selected filter score into a vector
    vector<float> oneFilterScore;
    for (int i=0; i<nscore; i++)
        oneFilterScore.push_back( parent_score[i].second.operator[](split_filter) );
        
    // divide score vector into equal bins 
    vector<float> threshold;
    calculateScoreBinThreshold( oneFilterScore, threshold, bins_ );  
    
    ScoresType one_score;
    for (int j=0; j<bins_; j++)
        grouped_score.push_back( one_score );

    // divide current score into different score group
    for ( int k=0; k < nscore; k++)
    {
        float& cur_score = parent_score[k].second.operator[](split_filter);

        for (int j=0; j<bins_; j++)
        {
            if( cur_score<=threshold[j])
            {
                grouped_score[j].push_back( parent_score[k] );
                break;
            }
        }
    }

    // remove the empty groups
    for ( int j=bins_-1; j>-1; j--)
    {
        if ( grouped_score[j].size() == 0 )
            grouped_score.erase( grouped_score.begin()+j );
    }
}


#endif // __filtersim_partition_H__
