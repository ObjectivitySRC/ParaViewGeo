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


#ifndef __filtersim_prototype_H__
#define __filtersim_prototype_H__

#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/rgrid_neighborhood.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/math/random_numbers.h>
#include <GsTLAppli/utils/string_manipulation.h>

#include <algorithm>
#include <iomanip>

#include "distance.h"
#include "pattern.h"

/*
 * template class Prototype_Base
 * the base class for prototype
 */
template< class Distance_ >
class GEOSTAT_DECL Prototype_Base
{
public:
    Prototype_Base( RGrid* TI_grid, Window_neighborhood* neighbors, 
                    Window_neighborhood* patch_neighbors, ScoresType pattern_score, 
                    vector<float>& score_weight, int nfacies=1 );
    
    ~Prototype_Base(){}

    void calculate_score_variance();  

    RGrid* get_grid() { return TI_grid_; }
    Window_neighborhood* get_neighbor() { return neighbors_; }
    Window_neighborhood* get_patch_neighbor() { return patch_neighbors_; }
    ScoresType& get_score() { return pattern_score_; }

    int get_nb_facies() { return nfacies_; }
    int get_replicates() { return replicate_; }

    float get_prototype_variance(){ return pt_variance_; }
    float get_prototype_sharpness(){ return sharpness_; }
    //float get_prototype_sharpness(){ return pt_variance_; }
    vector<float>& get_filter_variance() { return variance_; }    

    vector<float>& get_prototype_score() { return m_value_; }    
    vector<float>& get_prototype_weight() { return score_weight_; }

    float get_score_distance( vector<float>& dev_score );

    void remove_score();

protected:
    int nfacies_;               // # of facies
    int nscore_;                // # of total filter score

    int nb_neighbors_;
    int pt_nb_neighbors_;

    int replicate_;             // nl
    float pt_variance_;         // Vl
    float sharpness_;       //sharpness index

    vector<float> m_value_;     // ml for each i^{th} filter, also the filter score of each prototype
    vector<float> variance_;    // sigma^2 for each i^{th} filter
    ScoresType pattern_score_;

    vector<float> score_weight_;    // weight assigned to each filter score

    RGrid* TI_grid_;
    Window_neighborhood* neighbors_;
    Window_neighborhood* patch_neighbors_;

protected:
    Distance_ distance_;
};


/*
 * template class Prototype_Continuous for continuous variable
 */
template< class Distance >
class GEOSTAT_DECL Prototype_Continuous: public Prototype_Base< Distance >
{
public:
    typedef vector<float> pixel_type;
    typedef vector<float> proportion_type;
    typedef float mean_prop_type;

public:
    Prototype_Continuous( RGrid* TI_grid, Window_neighborhood* neighbors, 
                          Window_neighborhood* patch_neighbors, ScoresType pattern_score, 
                          vector<float>& score_weight, int nfacies=1 )
             : Prototype_Base< Distance >( TI_grid, neighbors, patch_neighbors, 
                                           pattern_score, score_weight, nfacies ){}
    ~Prototype_Continuous(){}

    void calculate_pattern_prototype();
    void clean_prototype_pattern();
    float get_distance(pixel_type& pattern, vector<float>& weight);

    pixel_type& get_prototype_pattern() { return prototype_pixel_; }
    proportion_type& each_pattern_prop() {return each_pattern_prop_;}
    mean_prop_type& pattern_prop() { return pattern_mean_; }
    void calculate_prototype_sharpness();

private:
    typedef Prototype_Base< Distance > Base_;
    pixel_type prototype_pixel_;   // pixel value on each template node
    proportion_type each_pattern_prop_;  
    mean_prop_type pattern_mean_;
};



/*
 * template class Prototype_Categorical for categorical variable
 */
template< class Distance >
class GEOSTAT_DECL Prototype_Categorical: public Prototype_Base< Distance >
{
public:
    typedef vector< vector<float> > pixel_type;
    typedef vector< vector<float> > proportion_type;
    typedef vector<float> mean_prop_type;

public:
    Prototype_Categorical( RGrid* TI_grid, Window_neighborhood* neighbors, 
                           Window_neighborhood* patch_neighbors, ScoresType pattern_score, 
                           vector<float>& score_weight, int nfacies=2 )
             : Prototype_Base< Distance >( TI_grid, neighbors, patch_neighbors, 
                                           pattern_score, score_weight, nfacies ) {}

    ~Prototype_Categorical(){}

    void calculate_pattern_prototype();
    void clean_prototype_pattern();
    float get_distance(pixel_type& pattern, vector<float>& weight);

    pixel_type& get_prototype_pattern() { return prototype_pixel_; }
    proportion_type& each_pattern_prop() {return each_pattern_prop_;}
    mean_prop_type& pattern_prop() { return pattern_mean_prop_; }
    void calculate_prototype_sharpness();

private:
    typedef Prototype_Base< Distance > Base_;
    pixel_type prototype_pixel_;   // pixel value on each template node
    proportion_type each_pattern_prop_;
    mean_prop_type pattern_mean_prop_;
};



/*
 * constructor for template Prototype_Base
 */
template< class Distance >
Prototype_Base< Distance >::
Prototype_Base( RGrid* TI_grid, Window_neighborhood* neighbors, 
                Window_neighborhood* patch_neighbors, ScoresType pattern_score, 
                vector<float>& score_weight, int nfacies )
{
    TI_grid_ = TI_grid;
    neighbors_ = neighbors;
    patch_neighbors_ = patch_neighbors;
    pattern_score_ = pattern_score;
    nfacies_ = nfacies;
    score_weight_ = score_weight;

    replicate_ = pattern_score_.size();
    nscore_ = pattern_score_[0].second.size();

    nb_neighbors_ = neighbors_->max_size();
    pt_nb_neighbors_ = patch_neighbors_->max_size();

    m_value_.resize(nscore_, 0.);
    variance_.resize(nscore_, 0.);

    //calculate_score_variance();
}


/*
 * function to calculate the mean value and the variance
 * of each filter score
 */
template< class Distance >
void 
Prototype_Base< Distance >::
calculate_score_variance()
{
    int i,j;

    // loop over each pattern
    for (i=0; i<replicate_; i++)
    {
        vector<float>& score = pattern_score_[i].second;
        for (j=0; j<nscore_; j++)
        {
            m_value_[j] += score[j];
            variance_[j] += (score[j] * score[j]);
        }
    }

    if ( replicate_ == 1 )
    {
        for (j=0; j<nscore_; j++)
            variance_[j] = 0;
    }
    else
    {
        for (j=0; j<nscore_; j++)
        {
            m_value_[j] /= replicate_;
            //variance_[j] = variance_[j]/replicate_ - m_value_[j]*m_value_[j];
            variance_[j] = variance_[j]/(replicate_-1) 
                           - m_value_[j]*m_value_[j]*replicate_/(replicate_-1) ;

            // correct float inaccuracy (very small values close to 0)
            if ( variance_[j] < EPSILON )    variance_[j]=0;
        }
    }

    // the length of "weight" is same as nscore_
    // summation of "weight" is "1"
    pt_variance_ = std::inner_product(variance_.begin(), variance_.end(), score_weight_.begin(), 0.);

    if ( pt_variance_< EPSILON )    pt_variance_ = 0;
}


/*
 * function to calculate the distance between DEV score and prototype score
 */
template< class Distance >
float 
Prototype_Base< Distance >::
get_score_distance( vector<float>& dev_score)
{
    return distance_( m_value_.begin(), m_value_.end(), dev_score.begin() );
}


/*
 * function to calculate the mean value and the variance
 * of each filter score
 */
template< class Distance >
void 
Prototype_Base< Distance >::
remove_score()
{
    //m_value_.clear();
    variance_.clear();
    score_weight_.clear();

    for (int i=0; i<pattern_score_.size(); i++)
        pattern_score_[i].second.clear();
}


/*
 * function to calculate the sharpness index
 */
template< class Distance >
void 
Prototype_Continuous< Distance >::
calculate_prototype_sharpness()
{
    int i,j;
    //vector<float> m(Base_::nscore_, 0.);    // mean for each pattern location
    //vector<float> v(Base_::nscore_, 0.);     // variance for each pattern location
    vector<float>& m = Base_::m_value_;
    vector<float>& v = Base_::variance_;

    // loop over each pattern
    for (i=0; i<Base_::replicate_; i++)
    {
        vector<float>& score = Base_::pattern_score_[i].second;
        for (j=0; j<Base_::nscore_; j++)
        {
            m[j] += score[j];
            v[j] += (score[j] * score[j]);
        }
    }

    if ( Base_::replicate_ == 1 )
    {
        Base_::sharpness_ = 1.0;

        for (j=0; j<Base_::nscore_; j++)
            v[j] = 0.0;

        return;
    }

    // calculate the overall mean and variance
    float mean = std::accumulate(m.begin(), m.end(), 0.0) /(Base_::nscore_)/(Base_::replicate_);
    float variance = std::accumulate(v.begin(), v.end(), 0.0);
    variance = variance/(Base_::nscore_)/(Base_::replicate_) - mean*mean;

    for (j=0; j<Base_::nscore_; j++)
    {
        m[j] /= Base_:: replicate_;
        v[j] = v[j]/Base_::replicate_ - m[j]*m[j];
        
        // correct float inaccuracy (very small values close to 0)
        if ( v[j] < EPSILON )    v[j]=0;
    }

    Base_::sharpness_ = 1- std::accumulate(v.begin(), v.end(), 0.0)/variance/(Base_::nscore_);
}


/*
 * function to remove the prototype pattern for memory issue
 */
template< class Distance >
void 
Prototype_Continuous< Distance >::
clean_prototype_pattern()
{
    each_pattern_prop_.clear();
    prototype_pixel_.clear();
}


/*
 * function to calculate the prototype which is 
 * the mean value over each template node
 */
template< class Distance >
void 
Prototype_Continuous< Distance >::
calculate_pattern_prototype()
{
    // because searching template in TI_grid is always full, 
    // number of found neighbors is the maximal size of geometry
  each_pattern_prop_.resize( Base_::replicate_, 0.0);
  prototype_pixel_.resize( Base_::nb_neighbors_, 0.0);
    
  for (int i=0; i < Base_::replicate_; i++)
    {
      int node_id = (Base_::pattern_score_)[i].first;
      (Base_::neighbors_)->find_neighbors( (Base_::TI_grid_)->geovalue(node_id) );
        
        int j = 0;
        // the summation over all the template nodes
        for( Neighborhood::iterator nb_iter = (Base_::neighbors_)->begin();
	            nb_iter != (Base_::neighbors_)->end() ; nb_iter++, j++ )	
        {
            prototype_pixel_[j] += nb_iter->property_value();
        }

        // only use the nodes within patch template to calculate pattern proportion
        Base_::patch_neighbors_->find_neighbors( Base_::TI_grid_->geovalue(node_id) );
        for( Neighborhood::iterator pt_nb_iter = Base_::patch_neighbors_->begin();
	              pt_nb_iter != Base_::patch_neighbors_->end() ; pt_nb_iter++ )	
        {
            each_pattern_prop_[i] += pt_nb_iter->property_value();
        }
                
        each_pattern_prop_[i] /= static_cast<float>( Base_::pt_nb_neighbors_ );
    }

    pattern_mean_ = std::accumulate( each_pattern_prop_.begin(), 
                                     each_pattern_prop_.end(), 0. ) / Base_::replicate_;

    std::transform( prototype_pixel_.begin(), prototype_pixel_.end(), 
                    prototype_pixel_.begin(),
                    bind2nd( std::divides<float>(), Base_::replicate_ ) );
}


/*
 * function to calculate the distance between DEV and prototype
 */
template< class Distance >
float 
Prototype_Continuous< Distance >::
get_distance(pixel_type& pattern, vector<float>& weight)
{
    return Prototype_Continuous<Distance>::
      distance_(pattern.begin(), pattern.end(), 
		prototype_pixel_.begin(), weight.begin());
}

/*
 * function to calculate the sharpness index
 */
template< class Distance >
void 
Prototype_Categorical< Distance >::
calculate_prototype_sharpness()
{
    int i, j;

    float K1 = 1.0/(Base_::nfacies_);

    // loop over each pattern
    for (i=0; i<Base_::nfacies_; i++)
    {
        vector<float>& proto = prototype_pixel_[i];
        for (j=0; j<Base_::nscore_; j++)
            Base_::sharpness_ += sqr( proto[j] - K1 );
    }

    if ( Base_::replicate_ == 1 )
        Base_::sharpness_ = 1.0;
    else
        Base_::sharpness_  *= (Base_::nfacies_)/(Base_::nfacies_-1)/(Base_::nscore_);

    // calculate the variance for each filters
    vector<float>& m = Base_::m_value_;
    vector<float>& v = Base_::variance_;

    // loop over each pattern
    for (i=0; i<Base_::replicate_; i++)
    {
        vector<float>& score = Base_::pattern_score_[i].second;
        for (j=0; j<Base_::nscore_; j++)
        {
            m[j] += score[j];
            v[j] += (score[j] * score[j]);
        }
    }

    if ( Base_::replicate_ == 1 )
    {
        for (j=0; j<Base_::nscore_; j++)
            v[j] = 0;
    }
    else
    {
        for (j=0; j<Base_::nscore_; j++)
        {
            m[j] /= Base_::replicate_;
            v[j] = v[j]/(Base_::replicate_) - m[j]*m[j] ;

            // correct float inaccuracy (very small values close to 0)
            if ( v[j] < EPSILON )    v[j]=0;
        }
    }
}


/*
 * function to remove the prototype pattern for memory issue
 */
template< class Distance >
void 
Prototype_Categorical< Distance >::
clean_prototype_pattern()
{
    for (int i=0; i<Base_::nfacies_; i++)
        prototype_pixel_[i].clear();

    for (int j=0; j<Base_::replicate_; j++)
        each_pattern_prop_[j].clear();

    prototype_pixel_.clear();
    each_pattern_prop_.clear();
    pattern_mean_prop_.clear();
}


/*
 * function to calculate the prototype which is 
 * the mean value per facies over each template node
 */
template < class Distance >
void Prototype_Categorical< Distance >::
calculate_pattern_prototype()
{
    int i, j;
    PatternType oneFaciesPattern( Base_::nb_neighbors_, 0. );

    for (j=0; j < Base_::nfacies_; j++)
        prototype_pixel_.push_back( oneFaciesPattern );

    pattern_mean_prop_.resize(Base_::nfacies_, 0.0);

    for (i=0; i < Base_::replicate_; i++)
    {
        int node_id = Base_::pattern_score_[i].first;
        Base_::neighbors_->find_neighbors( Base_::TI_grid_->geovalue(node_id) );

        j = 0;
        // summation per facies over all template node
        for( Neighborhood::iterator nb_iter = Base_::neighbors_->begin();
	     nb_iter != Base_::neighbors_->end() ; nb_iter++, j++ )	
        {
            prototype_pixel_[ nb_iter->property_value() ][j] += 1.0 ;
        }
                
        Base_::patch_neighbors_->find_neighbors( Base_::TI_grid_->geovalue(node_id) );
        vector<float> temp_prop( Base_::nfacies_, 0.0);
        
        // only use the nodes within patch template to calculate pattern proportion
        for( Neighborhood::iterator pt_nb_iter = Base_::patch_neighbors_->begin();
	     pt_nb_iter != Base_::patch_neighbors_->end() ; pt_nb_iter++ )	
        {
            temp_prop[ pt_nb_iter->property_value() ] += 1.0;
        }

        // mean value for each facies
        for (j=0; j < Base_::nfacies_; j++)
        {
            temp_prop[j] /= static_cast<float>( Base_::pt_nb_neighbors_ );
            pattern_mean_prop_[j] += temp_prop[j];
        }

        each_pattern_prop_.push_back( temp_prop );
    }
    
    for (j=0; j < Base_::nfacies_; j++)
    {
        std::transform( prototype_pixel_[j].begin(), prototype_pixel_[j].end(), 
                        prototype_pixel_[j].begin(),
                        bind2nd( std::divides<float>(), Base_::replicate_ ) );
    }

    std::transform( pattern_mean_prop_.begin(), pattern_mean_prop_.end(), 
                    pattern_mean_prop_.begin(),
                    bind2nd( std::divides<float>(), Base_::replicate_ ) );
}


/*
 * function to calculate the distance DEV and the prototype
 */
template< class Distance >
float 
Prototype_Categorical< Distance >::
get_distance(pixel_type& pattern, vector<float>& weight)
{
    float dist = 0.0;

    for (int j=0; j < Base_::nfacies_; j++)
        dist += 
	  Prototype_Categorical<Distance>::
	  distance_( 
		    pattern[j].begin(), pattern[j].end(), 
		    prototype_pixel_[j].begin(), weight.begin() 
		    );
    
    return dist;
}

#endif // __filtersim_prototype_H__
