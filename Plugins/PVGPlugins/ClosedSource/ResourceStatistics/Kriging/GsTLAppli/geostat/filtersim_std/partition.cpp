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


#include "partition.h"

/*
 * function to find the bin threshold given the score value and number of bins
 */
void calculateScoreBinThreshold( vector<float>& score_value, vector<float>& threshold, int nbins )
{
    // remove the identical values
    vector<float>::iterator new_end=unique( score_value.begin(), score_value.end() );
    score_value.erase( new_end, score_value.end() );
    sort( score_value.begin(), score_value.end() );
    
    // number of unique score
    int nscore = score_value.size();
    vector<int> cut_location(nbins, nscore-1);

    if ( nscore >= nbins )  // have enough replicates
    {
        for (int i=1; i<=nbins; i++)
        {
            int loc = (int) std::ceil(i*float(nscore)/nbins);
            cut_location[i-1] = loc-1;
        }
    }
    else
    {   // put each unique score location into threshold location
        for (int i=0; i<nscore; i++)
            cut_location[i] = i;
    }

    for (int j=0; j<nbins; j++)
        threshold.push_back( score_value[ cut_location[j] ] );
}

/*
 * function to find the bin threshold for all filter scores
 */
void CrossPartition::
calculateBinThreshold( const ScoresType& score, vector< vector<float> >& threshold )
{
    int i, j;
    
    int nscore = score.size();
    int nfilter = score[0].second.size();
    
    // loop over all filters
    for (i=0; i<nfilter; i++)
    {
        vector<float> oneFilterScore;
        
        // save current filter scores into a vector
        for (j=0; j<nscore; j++)
            oneFilterScore.push_back( score[j].second.operator[](i) );
        
        // find the threshold of current score vector
        vector<float> cutoff;
        calculateScoreBinThreshold( oneFilterScore, cutoff, bins_ );        

        threshold.push_back( cutoff );
    }
}
