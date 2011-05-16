/**********************************************************************
** Author: Nicolas Remy
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
**
** Modified by Jianbing Wu, SCRF, Stanford University, June 2004
**********************************************************************/

#include <GsTLAppli/grid/grid_model/strati_grid.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/grid/grid_model/rgrid_neighborhood.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/math/random_numbers.h>
#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/grid/grid_model/grid_initializer.h>
#include <GsTLAppli/geostat/utilities.h>
#include <GsTLAppli/appli/utilities.h>
#include <GsTLAppli/utils/gstl_plugins.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/progress_notifier.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/geostat/filtersim_std/is_categorical.h>

#include <GsTL/cdf/non_param_pdf.h>
#include <GsTL/univariate_stats/build_cdf.h>
#include <GsTL/sampler/monte_carlo_sampler.h>
#include <GsTL/cdf_estimator/search_tree.h>
#include <GsTL/simulation/sequential_simulation.h>
#include <GsTL/math/math_functions.h>
#include <GsTL/sampler/updater_sampler.h>

#include <sstream>
#include <fstream>
#include <numeric>
#include <cmath>
#include <vector>
#include <set>
#include <iomanip>

#include "snesim_std.h"
#include "tree_list.h"
#include "tau_updating.h"
//#include "nu_updating.h"
#include "layer_sequential_simulation.h"
#include "../filtersim_std/is_categorical.h"
#include "NodeDropped.h"

using namespace std;

typedef Tree_list<std::pair<Colocated_value*, Colocated_value*> > TreeList;

float Property_map::operator() ( const Geovalue& g ) const 
{
	int id = g.node_id();
	appli_assert( prop_->is_informed( id) );
	return prop_->get_value( id );
}


//-----------------------------------------------------
//-----------------------------------------------------

Named_interface* Snesim_Std::create_new_interface( std::string& ) 
{
	return new Snesim_Std;
}
 

Snesim_Std::Snesim_Std() 
{
	tau1_ = 1;
	tau2_ = 1;
	constraint_to_target_cdf_ = 0.5;
	seed_ = 21111975;
	nb_multigrids_ = 3;

    revisitNodesProp_ = 0.0;
    revisit_criterion_ = -1;
    revisit_iter_nb_ = 1;

    is_power_factor_ = true;
    iso_expansion_ == 1;

    is_view_intermediate_ = 0;
    is_view_node_drop_ = 0;

    for(int nw = 0; nw<=3; nw++)
		window_geom_sg_[nw] = 0;
	
    // grid or property pointers
	simul_grid_ = 0;
	multireal_property_ = 0;
	training_image_ = 0;
	rot_property_ = 0;
	aff_property_ = 0;
	harddata_grid_ = 0;
	harddata_property_ = 0;
    region_property_ = 0;
    region_simulated_ = 0;

	use_vertical_ = false;
	use_soft_cube_ = false;

    sampler1_ = 0;
    sampler2_ = 0;

	// un-used parameters
    /*
	scanchoice_ = 0;
	nbins_ = 0;
	maxdiff_ = 0;
	hardno_ = 0;
	resim_sg_ = 0;
	resim_choice_ = 0;
    */
}

Snesim_Std::~Snesim_Std() 
{
	for(int nw = 0;nw<=NUM_SG;nw++)
    {
		if( window_geom_sg_[nw] )
			delete window_geom_sg_[nw];
    }
}


/* 
* Its aim is to extract the information input by the user and
* initialize the data members of the class, so that the class 
* is ready for use. 
* The function also checks if the parameters are valid and reports
* any error. The function is not interupted if an error is encountered
* (ie a supplied parameter is not valid) because it reports all the
* parameter errors, not just the first encountered.
*/
bool Snesim_Std::initialize( const Parameters_handler* parameters,
							Error_messages_handler* error_mesgs ) 
{	
	// Extract the parameters input by the user from the parameter handler
 	cmin_ = String_Op::to_number<int>( parameters->value( "Cmin.value" ) );
	nb_reals_ = String_Op::to_number<int>( parameters->value( "Nb_Realizations.value" ) );
	nb_facies_ = String_Op::to_number<unsigned int>( parameters->value( "Nb_Facies.value" ) );
	num_close_max_ = String_Op::to_number<int>( parameters->value( "Previously_simulated.value" ) );
	max_prevcond_ = String_Op::to_number<int>( parameters->value( "Max_Cond.value" ) );
	subgrid_choice_ = String_Op::to_number<int>( parameters->value( "Subgrid_choice.value" ) );

	seed_ = String_Op::to_number<int>( parameters->value( "Seed.value" ) );
    if ( seed_ == 0 )
    {
        error_mesgs->report( "Seed", "The seed number cannot be zero" );
        return false;
    }

    revisit_criterion_ = String_Op::to_number<int>( parameters->value( "resimulation_criterion.value" ) );
    revisit_iter_nb_ = String_Op::to_number<int>( parameters->value( "resimulation_iteration_nb.value" ) );
    if ( revisit_criterion_ < 0 )       revisit_iter_nb_ = 0;
    if ( revisit_criterion_ > -1 )      revisitNodesProp_ = revisit_criterion_/1000.0;

    // get the servo-system
	constraint_to_target_cdf_ = String_Op::to_number<float>( 
		        parameters->value( "Constraint_Marginal_ADVANCED.value" ) );
	error_mesgs->report(  constraint_to_target_cdf_ < 0 ||  constraint_to_target_cdf_ > 1,
			    "Constraint_Marginal_ADVANCED", "Input a number between 0 and 1"  );

	int input_nb_multigrids = String_Op::to_number<int>( 
						parameters->value( "Nb_Multigrids_ADVANCED.value" ) );

    // no subgrid is allowed for only 1 grid simulation
    //if ( input_nb_multigrids==1 )   subgrid_choice_ = 0;

    get_debug_level( parameters );

    if ( !get_marginal_cdf( parameters, error_mesgs ) )
        return false;

    if ( !get_simul_grid( parameters, error_mesgs ) )
        return false;

    if ( !get_training_image( parameters, error_mesgs ) )
        return false;

    if ( !get_hard_data( parameters, error_mesgs ) )
        return false;

    local_rot_ = local_aff_ = prob_field_ = use_region_ = use_pre_region_ = 0;
	
    if ( !get_rotation_data( parameters, error_mesgs ) )
        return false;

    if ( !get_affinity_data( parameters, error_mesgs ) )
        return false;
	
    if ( !get_region_data( parameters, error_mesgs ) )
        return false;

    if ( !get_soft_prob_data( parameters, error_mesgs ) )
        return false;
	
    if ( !get_vert_prob_data( parameters, error_mesgs ) )
        return false;
	
    if ( !get_search_ellipsoid( parameters, error_mesgs ) )
        return false;
	
	//calculate the max. # of multi-grid
	cal_max_multi_grid_number( input_nb_multigrids );

    if ( !get_template_expansion_factor( parameters, error_mesgs ) )
        return false;

	// We're finally done with members initialization 
	
	// Do some parameter checking (for debugging) :
    check_prob_consistency( parameters, error_mesgs );
	print_parameters();	

	if( !error_mesgs->empty() )
		return false;
	
	return true;
}

/*
* perform the main snesim algorithm
*/
int Snesim_Std::execute( GsTL_project* proj ) 
{
    // Initialize the global random number generator
    Global_random_number_generator::instance()->seed( seed_ );

    // Set up a progress notifier	
    int total_steps = simul_grid_->size() * nb_reals_;
    if ( revisit_criterion_>-1 ) 
        total_steps = int( total_steps * (1.001+revisitNodesProp_*revisit_iter_nb_) );

    int frequency = std::max( total_steps / 20, 1 );
    SmartPtr<Progress_notifier> progress_notifier = 
        utils::create_notifier( "Running Snesim_Std", total_steps, frequency );

    simul_grid_->set_level(1);	
    training_image_->set_level(1);

    // used to get the vertical coordinate for each node
    get_vertical_index_ = ComputeLayerIndex( simul_grid_->cursor() );

    // anisotropic template expansion
    if ( iso_expansion_==0 )    
    {
        // set anisotropic expansion in simulation grid
        simul_grid_->cursor()->set_anistropic_expansion( expansion_factor_ );

        // set anisotropic expansion in training image grid
        training_image_->cursor()->set_anistropic_expansion( expansion_factor_ );
    }

    bool not_success;

    // loop on all realizations
    for( int nreal = 1; nreal <= nb_reals_; nreal++ ) 
    {
        progress_notifier->message() << "working on realization " << nreal << gstlIO::end;
        if( ! progress_notifier->notify() ) return 1;

        appli_message("realization " << nreal );

        GsTLGridProperty* prop = multireal_property_->new_realization();
        simul_grid_->select_property( prop->name() );

        simul_grid_->set_level(1);	

        if( property_copier_ ) 
            property_copier_->copy( harddata_grid_, harddata_property_, simul_grid_, prop );

        if ( !use_vertical_ )
        {  // use the basic servo-system correction
            sampler1_ =  new ServoSystem( marginal_, double(constraint_to_target_cdf_),
                simul_grid_->begin(), simul_grid_->end(), 
                Random_number_generator() );
        }
        else
        {  // use the modifed servo-system to account the vertical effect            
            sampler2_ =  new LayerServoSystem( vert_marginal_, double(constraint_to_target_cdf_),
                simul_grid_->begin(), simul_grid_->end(), 
                Random_number_generator(), get_vertical_index_ );
        }


        simul_grid_->set_level(1);	

        appli_message("creating servosystem... " );

        if( property_copier_ ) 
            property_copier_->copy( harddata_grid_, harddata_property_, simul_grid_, prop );

        if ( !use_vertical_ )
        {
            // use the basic servo-system correction
            // Monte_carlo_sampler_t monte_sampler;
            sampler1_ =  new ServoSystem( marginal_, double(constraint_to_target_cdf_),
                simul_grid_->begin(), simul_grid_->end(),
                Random_number_generator() );
        }
        else
        {
            // use the modifed servo-system to account the vertical effect
            sampler2_ =  new LayerServoSystem( vert_marginal_, double(constraint_to_target_cdf_),
                simul_grid_->begin(), simul_grid_->end(), 
                Random_number_generator(), get_vertical_index_ );
        }

        if( property_copier_ ) 
            property_copier_->undo_copy();

        if ( is_view_node_drop_ )
        {
            std::string data_drop_prop_name_ = prop->name() + "_nd";
            data_drop_prop_ = geostat_utils::add_property_to_grid(simul_grid_, data_drop_prop_name_);
        }

        not_success = simulate_one_realization( progress_notifier, prop, nreal );
        if ( not_success )    return not_success;

        if ( sampler1_ != 0 ) delete sampler1_; 
        if ( sampler2_ != 0 ) delete sampler2_;
    }   // end of simulation for each realization

    if ( iso_expansion_==0 )   
    {
        simul_grid_->cursor()->set_istropic_expansion();
        training_image_->cursor()->set_istropic_expansion();
    }

    appli_message("finished simulating all realizations " << std::endl );
    return 0;
}


void Snesim_Std::clean( GsTLGridProperty* prop ) 
{
	if( prop ) 
		simul_grid_->remove_property( prop->name() );
}


/*
 * Calculate the maximum multi-grid number "Max_allowed"
 * the licit mutli-grid number is the minimum of Max_allowed and
 * the user's input "input_nb_multigrids"
 */
void Snesim_Std::cal_max_multi_grid_number(int input_nb_multigrids)
{
	vector<int> vec_x;
	vector<int> vec_y;
	
	for( Grid_template::iterator it = window_geom_sg_[NUM_SG] ->begin(); 
			it != window_geom_sg_[NUM_SG] ->end(); ++it) 
	{
		vec_x.push_back( (*it).x() );
		vec_y.push_back( (*it).y() );
	}
	
	double cell_range_x = *( max_element( vec_x.begin(), vec_x.end() ) )
						- *( min_element( vec_x.begin(), vec_x.end() ) ) + 1;
	double cell_range_y = *( max_element( vec_y.begin(), vec_y.end() ) )
						- *( min_element( vec_y.begin(), vec_y.end() ) ) + 1;
	
	int nx_min = (int) (log( 2*training_image_->nx()/cell_range_x )/log(2.0) + 1);
	int ny_min = (int) (log( 2*training_image_->ny()/cell_range_y )/log(2.0) + 1);
	
	nb_multigrids_ = min(input_nb_multigrids, int(min(nx_min, ny_min)));
	
	if ( nb_multigrids_ != input_nb_multigrids )
	{
		cout << "\nThe input multigrid number is larger than the allowed, \n" 
			<< "The number of multigrid is reset as: " << nb_multigrids_ 
			<< std::endl << endl;
	}
}


/*
 * get the template expansion factors in X/Y/Z direction for each multi-grid
 */
/*  reading expansion factor data from a file
bool Snesim_Std::get_template_expansion_factor(const Parameters_handler* parameters,
							Error_messages_handler* error_mesgs)
{
    iso_expansion_ = String_Op::to_number<int>( parameters->value( "expand_isotropic.value" ) );
    
    if ( iso_expansion_==0 )
    {
        string datafile = parameters->value( "anisotropic_file.value" );

        if( datafile.empty() ) 
        {
            error_mesgs->report( "anisotropic_file", "No data file specified" );
            return false;
        }

        ifstream infile( datafile.c_str() );
        if( !infile ) 
        {
            error_mesgs->report( "anisotropic_file", "Cannot open the file contains expansion factors" );
            return false;
        }

        if ( !read_expansion_datafile( datafile ) )
        {
            error_mesgs->report( "anisotropic_file", "Not a valid template expansion data file" );
            return false;
        }

        switch( check_expansion_factor() )
        {
        case 1:
            error_mesgs->report( "anisotropic_file", "Input data file does not contain enough expansion factors" );
            return false;
        case 2:
            error_mesgs->report( "anisotropic_file", "All expansion factors in the finest grid must be 1" );
            return false;
        case 3:
            error_mesgs->report( "anisotropic_file", "Expansion factor in coarse grid must be larger than in finer grid" );
            return false;
        case 4:
            error_mesgs->report( "anisotropic_file", "Coarser grid nodes must be full contained in the finer grid" );
            return false;
        default:
            break;
        }
    }

    return true;
}
*/

// read expansion factor from a widget table
bool Snesim_Std::get_template_expansion_factor(const Parameters_handler* parameters,
							Error_messages_handler* error_mesgs)
{
    iso_expansion_ = String_Op::to_number<int>( parameters->value( "expand_isotropic.value" ) );
    
    if ( iso_expansion_==0 )
    {
        std::string factor_list_str = parameters->value( "aniso_factor.value" );
        if ( factor_list_str.empty() )  
        {
            error_mesgs->report( "aniso_factor_box", "No expansion factor list specified" );
            return false;
        }

        std::vector<int> factor_vals = String_Op::to_numbers<int>( factor_list_str );
        int factor_ng = factor_vals.size() / 3;

        if ( (factor_vals.size() %3 )!=0 )
        {
            error_mesgs->report("aniso_factor_box", "Total number of expansion factors is incorrect" );
            return false;
        }

        for (int i=0; i<factor_ng; i++)
        {
            vector<int> factor;
            for (int j=0; j<3; j++)
            {
                int loc = i*3 + j;
                if (factor_vals[loc]>0)
                    factor.push_back( factor_vals[loc] );
                else
                    factor.push_back( 1 );
            }

            expansion_factor_.push_back( factor );
        }
        
        if ( factor_ng>nb_multigrids_ )
            expansion_factor_.erase( expansion_factor_.begin()+nb_multigrids_, expansion_factor_.end() );

        switch( check_expansion_factor() )
        {
        case 1:
            error_mesgs->report( "aniso_factor_box", "Input data file does not contain enough expansion factors" );
            return false;
        case 2:
            error_mesgs->report( "aniso_factor_box", "All expansion factors in the finest grid must be 1" );
            return false;
        case 3:
            error_mesgs->report( "aniso_factor_box", "Expansion factor in coarse grid must be larger than in finer grid" );
            return false;
        case 4:
            error_mesgs->report( "aniso_factor_box", "Coarser grid nodes must be full contained in the finer grid" );
            return false;
        default:
            break;
        }

        is_power_factor_ = is_power2_factor( expansion_factor_ );
    }
    else if ( ( training_image_->nz()>1 || simul_grid_->nz()>1 ) && tmpl_nz_==1 && nb_multigrids_>1 )
    {  // added to use 2D template for 3D TI
        GsTLcout << "Use 2D template expansion instead of 3D"  << gstlIO::end;
        iso_expansion_=0;
        is_power_factor_ = 1;

        for (int mi=0; mi<nb_multigrids_; mi++)
        {
            vector<int> factor(3, 1.0);
            factor[0] = pow(2.0, mi);
            factor[1] = pow(2.0, mi);
            expansion_factor_.push_back( factor );
        }
    }

    return true;
}

// not used 
bool Snesim_Std::read_expansion_datafile( string filename )
{
    string line;
    ifstream infile( filename.c_str() );

    int count = 0;

    while( getline(infile,line) )
    {
        int loc_factor;
        string cur_str;
        vector< string > str; 
        istringstream scan_stream( line );

        while( scan_stream >> cur_str ) 
            str.push_back( cur_str );

        if ( str.size() != 3 )  return false;

        vector<int> factor;
        for (int i=0; i<3; i++)
        {
            if ( !is_number( str[i] ) )    return false;

            loc_factor = std::max( String_Op::to_number<int>(str[i]), 1 );
            factor.push_back( loc_factor );
        }

        expansion_factor_.push_back( factor );

        count ++;
    }

    // too many multigrids are provided, only retain the first "nb_multigrids_" multiple grids
    if ( count>nb_multigrids_ )
        expansion_factor_.erase( expansion_factor_.begin()+nb_multigrids_, expansion_factor_.end() );

    is_power_factor_ = is_power2_factor( expansion_factor_ );

    return true;
}

int Snesim_Std::check_expansion_factor()
{
    if ( expansion_factor_.size()  <  nb_multigrids_ )
        return 1;
    else 
    {
        vector<int>& factor = expansion_factor_[0];

        if ( simul_grid_->nz()<=1 )     // all expansion factors in finest grid must be 1
        {
            //factor[2] = 1;        // reset expansion factor in Z as 1
            if ( factor[0]*factor[1] != 1 )    
                return 2;
        }
        else
        {
            if ( factor[0]*factor[1]*factor[2] != 1 )    
                return 2;
        }

        if ( nb_multigrids_  >  1 )
        {
            for (int j=0; j<nb_multigrids_-1; j++)
            {
                for (int i=0; i<3; i++)
                {
                    if ( i==2 && simul_grid_->nz()<=1 ) continue;

                    int f1 = expansion_factor_[j][i];
                    int f2 = expansion_factor_[j+1][i];

                    if ( f1>f2)     return 3;      // expansion factor in coarse grid must be larger than in finer grid
                    if ( f2%f1 != 0 ) return 4; // coarser grid nodes must be full contained in the finer grid
                }
            }
        }

        return 0;
    }
}

bool Snesim_Std::is_power_N_factor( int factor, int N )
{
    if ( factor==1 )      return true;

    while( factor>N)
    {
        if ( factor%N != 0 )
            return false;

        factor /= N;
    }

    if ( factor==N )     
        return true;
    else 
        return false;
}

bool Snesim_Std::is_power2_factor( vector< vector<int> >& factor )
{
    int base=2;

    int dim=3;
    if ( simul_grid_->nz()<=1 )   dim=2;

    for (int i=0; i<factor.size(); i++)
    {
        vector<int>& one_factor = factor[i];
   
        for (int j=0; j<dim; j++)
        {
            if ( !is_power_N_factor(one_factor[j], base) )        return false;
        }
    }

    return true;
}


//bool Snesim_Std::is_power2_factor( vector< vector<int> >& factor )
//{
//    int ng = factor.size();
//
//    for (int i=0; i<ng; i++)
//    {
//        int iso_exp = (int) pow(2.0, i); 
//        vector<int>& one_factor = factor[i];
//
//        if ( simul_grid_->nz()<=1 )
//        {
//            if ( one_factor[0]!=iso_exp || one_factor[1]!=iso_exp )
//                return false;
//        }
//        else
//        {
//            if ( one_factor[0]!=iso_exp || one_factor[1]!=iso_exp || one_factor[2]!=iso_exp )
//                return false;
//        }
//    }
//
//    return true;
//}


void Snesim_Std::check_vertical_prop(const Parameters_handler* parameters,
							Error_messages_handler* error_mesgs)
{
	float cur_value;
	int nz=vertical_curve_grid_->nz();
	float precision=0.00001;
	vector<float> vertical_sum;
	
	cout << "\nCheck the validation of the vertical proportion curve " << endl;
	
	if ( simul_grid_->nz() != nz ) 
	{
		error_mesgs->report( simul_grid_->nz() != vertical_curve_grid_->nz(),
			"VerticalPropObject", 
			"# of vertical curve layers is not equal to # of simulation layers" );
	}
				
    bool has_non_data_value_ = false;
	for (int i=0; i<nz; i++) 
	{
		float sum=0;
		int j;
		std::vector<double> prob_values;
		CdfType ver_cdf;
		
		for (j=0; j<nb_facies_; j++)
			vertical_sum.push_back(0);
		
		for (j=0; j<nb_facies_; j++) 
		{
			if ( vertical_properties_[j]->is_informed(i) )
                cur_value = vertical_properties_[j]->get_value(i);
            else  
            {  // if not informed, replace it by its marginal
                cur_value = marginal_.prob(j);
                has_non_data_value_ = true;
            }

			sum += cur_value;
			vertical_sum[j] += cur_value; 
			
			if ( use_vertical_ && use_soft_cube_ )
				vert_prop_vector_[i][j] = cur_value;
			
			//set vertical target at each layer
			prob_values.push_back(cur_value);
		}
		
		//error_mesgs->report( !GsTL::equals( sum, float(1.0), precision ),
		//      	"VerticalPropObject", "Values must sum up to 1 in each horizontal section" );

		make_pdf_valid( prob_values.begin(), prob_values.end() );     // validate a cdf 
		ver_cdf.z_set( nb_facies_ );
		ver_cdf.p_set( prob_values.begin(), prob_values.end() );
        //ver_cdf.make_valid();   // validate a cdf 
		
		vert_marginal_.push_back(ver_cdf);
	}
	
	vert_ccdf_ = vert_marginal_;

    if ( has_non_data_value_ )
        GsTLcout << "The missing value in the vertical proportion curve is reset as the marginal proportion"  << gstlIO::end;
	
	float consistent_precision=0.1;
	bool vert_consistency = true;
	
	for (int j=0; j<nb_facies_; j++) 
	{
		if ( !GsTL::equals( vertical_sum[j]/nz, float(marginal_.prob(j)), consistent_precision ) )
			vert_consistency = false;
	}
	
	if ( !vert_consistency )
	{
        GsTLcout << "The vertical proportion is not consistent with the target proportion"  << gstlIO::end;

		cout << "Warning:" << endl
			<< "     The vertical proportion is not consistent with the target proportion" << endl
			<< "     The global target from the vertical proportion curve is :" << endl
			<< "     " ;
		for (int j=0; j<nb_facies_; j++)
			cout << "(" << j << ", " << setw(4) << setprecision(3) << vertical_sum[j]/nz << ")    ";
		cout << endl;
	}
	else
		cout << "The vertical proportion is valid" << endl;
}



void Snesim_Std::check_soft_global()
{
	float cur_value;
	const GsTLGridProperty* prop;
	vector<float> soft_sum;
	int i,j;
	int x,y,z;
	
	int grid_size=simul_grid_->size();
	const SGrid_cursor* const_cursor;
	
	cout << "\nCheck the soft probability consistency " << endl;
	
	for ( j=0; j<nb_facies_; j++ )  
		soft_sum.push_back(0);
	
    vector<int> node_size(nb_facies_, 0);
	for ( j=0; j<nb_facies_; j++ )  
	{
		prop = probfield_properties_[j].property();
		const_cursor = simul_grid_->cursor();
		
		for (i=0; i<grid_size; i++) 
		{
			cur_value = prop->get_value(i);
            if ( cur_value>=0 & cur_value<=1)
            {
                node_size[j] ++;

                const_cursor->coords(i, x, y, z);
                soft_sum[j] += cur_value;

                if ( use_vertical_ && use_soft_cube_ )
                    soft_prop_vector_[z][j] += cur_value;
            }
		}

        if ( node_size[j]==0 )    node_size[j]=1;
	}
	
	if ( use_vertical_ && use_soft_cube_ )
	{
		int layer_size = simul_grid_->nx() * simul_grid_->ny();
		for (i=0; i<simul_grid_->nz(); i++)
		{
			for (j=0; j<nb_facies_; j++)
                soft_prop_vector_[i][j] /= node_size[j];
				//soft_prop_vector_[i][j] /= layer_size;
		}
	}
		
	float consistent_precision=0.1;
	bool soft_consistency = true;

	for (j=0; j<nb_facies_; j++) 
	{
		//if ( !GsTL::equals( soft_sum[j]/grid_size, float(marginal_.prob(j)), consistent_precision ) )
		if ( !GsTL::equals( soft_sum[j]/node_size[j], float(marginal_.prob(j)), consistent_precision ) )
			soft_consistency = false;
	}
	
	if ( !soft_consistency )
	{
        GsTLcout << "The soft probability is not consistent with the target proportion"  << gstlIO::end;
		cout << "Warning:" << endl
			<< "     The soft probability cube is not consistent with the target proportion" << endl
			<< "     The global target from the soft proportion curve is :" << endl
			<< "     " ;
		for (int j=0; j<nb_facies_; j++)
			cout << "(" << j << ", " << setw(4) << setprecision(3) 
				 << soft_sum[j]/node_size[j] << ")    ";
				 //<< soft_sum[j]/grid_size << ")    ";
		cout << endl;
	}
	else
		cout << "The soft probability is consistent with global proportion" << endl;
}


void Snesim_Std::check_vertical_soft_consistency()
{
	int i,j;
	bool consistency = true;
	float consistent_precision=0.1;

	cout << "\nCheck the consistency between soft prob. cube and vert. prop.n curve " << endl;

	for ( i=0; i<vert_prop_vector_.size(); i++ )
	{
		for ( j=0; j<vert_prop_vector_[0].size(); j++ )
		{
			if ( !GsTL::equals( vert_prop_vector_[i][j], soft_prop_vector_[i][j], consistent_precision ) )
				consistency = false;
		}
	}

	if ( !consistency )
	{
        GsTLcout << "The soft probability is not consistent with the vertical  proportion curves"  << gstlIO::end;
		cout << "Warning:" << endl
			<< "     The soft prob. cube is not consistent with the vert. prop. curve" << endl
			<< "     The proportion at each layer from the soft prop. curve are :" << endl;
		for ( i=0; i<vert_prop_vector_.size(); i++ )
		{
			cout << "     Layer " << setw(2) << i << " : " ;
			for ( j=0; j<vert_prop_vector_[0].size(); j++ )
				cout << " (" << j << ", " << setw(4) 
					 << setprecision(3) << soft_prop_vector_[i][j] << ")    ";
			cout << endl;
		}
	}
	else
		cout << "The soft probability is consistent with vertical proportion curve" << endl;
}


void Snesim_Std::print_parameters()
{
	cout << "\nParameters read: " << endl;
	cout << "Simul grid: " << simul_grid_name_ << endl
		<< "Property prefix: " << property_name_prefix_ << endl
		<< "Training image: " << training_image_name_ << endl
		<< "Train. prop: " << training_property_name_ << endl
		<< "Nb of real: " << nb_reals_ << endl
		<< "Seed: " << seed_ << endl
		<< "Nb facies: " << nb_facies_ << endl
		<< "Multigrids: " << nb_multigrids_ << endl
		<< "Constraint: " << constraint_to_target_cdf_ << endl
        << "Re-simulate nodes proportion: " << revisitNodesProp_ << endl
		<< "Nu value: " << nu_ << endl << endl;

    if ( iso_expansion_==0 )
    {
        cout << "User input template expansion factors:" << endl;
        for (int i=0; i<expansion_factor_.size(); i++)
        {
            vector<int>& factor = expansion_factor_[i];
            cout << "      ( " << factor[0] << " , " << factor[1] << " , " << factor[2] << " )\n";
        }
        cout << endl;
    }
	
	cout << "Window geometry: " << endl;
	for(Grid_template::iterator it = window_geom_sg_[NUM_SG] ->begin(); 
				it != window_geom_sg_[NUM_SG] ->end(); ++it)
	{
		cout << *it << endl;
	}
    cout << endl;
    /*
    for (int j=0; j<=NUM_SG; j++)
    {
        cout << "subgrid " << j << endl;
        for(Grid_template::iterator it = window_geom_sg_[j] ->begin(); 
            it != window_geom_sg_[j] ->end(); ++it)
        {
            cout << *it << endl;
        }
    cout << endl;
    }
    */
}

/*
 * function to get the debug level
 */
void Snesim_Std::get_debug_level( const Parameters_handler* parameters )
{
	int level = String_Op::to_number<int>(parameters->value( "Debug_Level.value" ));

    switch(level)
    {
        case 2:
            is_view_intermediate_ = 1;
        case 1:
            is_view_node_drop_ = 1;
        default:
            break;
    }
}

/*
 * Initialize a random path
 * create 3 vectors first, then shuffle each one of them
 */
void Snesim_Std::init_random_path(int level) 
{
    if (use_region_ == 1)
        init_random_path_use_region( level );
    else
        init_random_path_normal( level );

}

void Snesim_Std::init_random_path_use_region(int level) 
{
    simul_grid_->select_property( region_property_->name() );

	if( (level>0)&&(subgrid_choice_ == 1)&&is_expansion_factor_dividable(nb_multigrids_ - level) )
	{
		grid_paths_[0].clear();
		grid_paths_[1].clear();
		grid_paths_[2].clear();
		const SGrid_cursor* sg_cursor = simul_grid_->cursor() ;
		
		for(int gi = 0;gi < sg_cursor->max_index() ;gi++)
		{
            bool skip = ( count(active_region_.begin(), active_region_.end(), 
                        (simul_grid_->geovalue(sg_cursor->node_id(gi))).property_value() )==0 );

            if ( !skip )
            {
                // check for grid size xsize etc here.
                int jx,jy,jz;
                sg_cursor->coords(sg_cursor->node_id(gi),jx,jy,jz);
                int loctest = abs(jx)%2+abs(jy)%2*2+abs(jz)%2*4+1;
                
                if((loctest ==1)||(loctest==8))
                    grid_paths_[0].push_back(gi);
                else if((loctest ==4)||(loctest==5))
                    grid_paths_[1].push_back(gi); 
                else
                    grid_paths_[2].push_back(gi);
            }
		}
		
		STL_generator gen;
		if(!grid_paths_[0].empty())  //in 3D
			std::random_shuffle( grid_paths_[0].begin(), grid_paths_[0].end(), gen );
		std::random_shuffle( grid_paths_[1].begin(), grid_paths_[1].end(), gen );
		std::random_shuffle( grid_paths_[2].begin(), grid_paths_[2].end(), gen ); 
	}       
	else{
		const SGrid_cursor* sg_cursor = simul_grid_->cursor() ;
		grid_paths_[3].clear();

		for( int i=0; i < sg_cursor->max_index(); i++ )
		{
            bool skip = ( count(active_region_.begin(), active_region_.end(), 
                    (simul_grid_->geovalue(sg_cursor->node_id(i))).property_value() )==0 );

            if ( !skip )
                grid_paths_[3].push_back(i);
		}
		
		STL_generator gen;
		std::random_shuffle( grid_paths_[3].begin(), grid_paths_[3].end(), gen );
	}
}

void Snesim_Std::init_random_path_normal(int level) 
{
	if( (level>0)&&(subgrid_choice_ == 1)&&is_expansion_factor_dividable(nb_multigrids_ - level) )
	{
		grid_paths_[0].clear();
		grid_paths_[1].clear();
		grid_paths_[2].clear();
		const SGrid_cursor* sg_cursor = simul_grid_->cursor() ;
		
		for(int gi = 0;gi < sg_cursor->max_index(); gi++)
		{
			// check for grid size xsize etc here.
			int jx,jy,jz;
			sg_cursor->coords(sg_cursor->node_id(gi),jx,jy,jz);
			int loctest = abs(jx)%2+abs(jy)%2*2+abs(jz)%2*4+1;

			if((loctest ==1)||(loctest==8))
				grid_paths_[0].push_back(gi);
			else if((loctest ==4)||(loctest==5))
				grid_paths_[1].push_back(gi); 
			else
				grid_paths_[2].push_back(gi);
		}
		
		STL_generator gen;
		if(!grid_paths_[0].empty())  //in 3D
			std::random_shuffle( grid_paths_[0].begin(), grid_paths_[0].end(), gen );
		std::random_shuffle( grid_paths_[1].begin(), grid_paths_[1].end(), gen );
		std::random_shuffle( grid_paths_[2].begin(), grid_paths_[2].end(), gen ); 
	}       
	else{
		const SGrid_cursor* sg_cursor = simul_grid_->cursor() ;

        grid_paths_[3].clear();

		for( int i=0; i < sg_cursor->max_index(); i++ )
			grid_paths_[3].push_back(i);

        //GsTLcout << "nb nodes is "  << sg_cursor->max_index() << gstlIO::end;
		
		STL_generator gen;
		std::random_shuffle( grid_paths_[3].begin(), grid_paths_[3].end(), gen );
	}
}

void Snesim_Std::create_window_geom( int rdmax1,int rdmax2,int 
									rdmax3,double ang1,double ang2,double ang3,
									int max_prevcond ) 
{
	training_image_->select_property( training_property_name_ );
	Ellipsoid_rasterizer 
		ell_ras(training_image_->nx(),training_image_->ny(),training_image_->nz(),
		rdmax1,rdmax2,rdmax3,ang1,ang2,ang3);
	std::vector< Ellipsoid_rasterizer::EuclideanVector >& templ =
		ell_ras.rasterize();
	Covariance<GsTLPoint> covar;
	int id = covar.add_structure( "Spherical" );
	
	covar.set_geometry( id,rdmax1,rdmax2,rdmax3,ang1,ang2,ang3);
	std::sort( templ.begin(), templ.end(), Evector_greater_than( covar ) );
	
	int mgo[8][NUM_SG] = {{1,1,1},{0,0,1},{0,0,1},{0,1,1},{0,1,1},{0,0,1},{0,0,1},{1,1,1}};
	int pgo[8][NUM_SG] = {{1,1,1},{0,0,0},{0,0,0},{0,0,1},{0,0,1},{0,0,0},{0,0,0},{0,1,1}};
	
	for(int sg_no = 0;sg_no<NUM_SG;sg_no++)
    {
		window_geom_sg_[sg_no] = new Grid_template;
		int numclose = 0,numtemp = 0;
		for(int iw =0;iw <int(templ.size());iw++)
		{
			int x,y,z;
			x = templ[iw].x();
			y = templ[iw].y();      
			z = templ[iw].z();	     
			int loctest = abs(x)%2 + abs(y)%2*2 + abs(z)%2*4 + 1;
			if(mgo[loctest-1][sg_no]==1)
			{
				if((pgo[loctest-1][sg_no]==1)&&(numclose<num_close_max_))
				{
					window_geom_sg_[sg_no]->add_vector( x, y, z );
					numclose++;
				}
				else if((pgo[loctest-1][sg_no]==0)&&(numtemp<max_prevcond))
				{
					window_geom_sg_[sg_no]->add_vector( x, y, z );
					numtemp++;
				}
			}
		}
		//cout << "Size of template = " << window_geom_sg_[0]->size() << endl;
		//cout << "Numclose = " << numclose << endl;
    }
	// also initializing coarse grid template
	window_geom_sg_[NUM_SG] = new Grid_template;

	for(int iw =0;iw <max_prevcond;iw++)
		window_geom_sg_[NUM_SG]->add_vector( templ[iw] );

}

Grid_template* Snesim_Std::multgrid_template(Grid_template* base_template)
{
	Grid_template* mg_template = new Grid_template;
	Grid_template::iterator iter;

	for(iter = base_template->begin();iter!=base_template->end();iter++) 
		mg_template->add_vector(*iter);

    return mg_template;
}


bool Snesim_Std::get_rotation_data( const Parameters_handler* parameters,
                                   Error_messages_handler* error_mesgs )
{	
    bool use_rot = String_Op::to_number<bool>( parameters->value( "Use_Rotation.value" ) );

    if (use_rot)
        local_rot_ = String_Op::to_number<int>( parameters->value( "Use_Local_Rotation.value") );
    
    if(local_rot_ == 1)
    {
        int nb_cat = 0; // total number of categories

        rot_property_name_ = parameters->value( "Rotation_property.value" );
        if ( rot_property_name_.empty() )
        {
            error_mesgs->report( "Rotation_property", "No rotation property selected" );
            return false;
        }
                
        rot_property_ = simul_grid_->property( rot_property_name_ );
        if( !rot_property_ ) return false;

        if ( !is_indicator_prop(rot_property_, nb_cat) )
        {
            error_mesgs->report( "Rotation_property", "Rotation indicator must be integer and start from 0" );
            return false;
        }
       
        std::string angle_str = parameters->value( "Rotation_categories.value" );
        if ( angle_str.empty() )
        {
            error_mesgs->report( "Rotation_categories", "No angles specified" );
            return false;
        }

        angles_ = String_Op::to_numbers<double>( angle_str );

        if ( angles_.size() != nb_cat )
        {
            std::ostringstream error_stream;
            error_stream << "total # of angle categories is " << nb_cat 
                         << " is not equal to the # of angles specified";
            error_mesgs->report( "Rotation_categories", error_stream.str() );
            return false;
        }
        
        // Those values are azimuth angles, ie angles measured from the y axis
        // turn them in angles measured from the x axis
        //std::transform( angles_.begin(), angles_.end(), angles_.begin(), 
        //                std::bind1st( std::minus<double>(), 90.0 ) );
        std::transform( angles_.begin(), angles_.end(), angles_.begin(), 
                                std::bind1st( std::multiplies<double>(), -1.0 ) );
        
        angles_ncat_ = angles_.size();
    }
    else
    {
		int global_rot = 0;
		if( use_rot )
			global_rot = String_Op::to_number<int>( parameters->value( "Use_Global_Rotation.value") );

        float angle = 0;
		if( global_rot == 1 )
            angle = String_Op::to_number<float>( parameters->value( "Global_Angle.value") );

        angles_ncat_ = 1;
        angles_.push_back(-angle);
    }	
    
    return true;
}


bool Snesim_Std::get_region_data( const Parameters_handler* parameters,
                                   Error_messages_handler* error_mesgs )
{	
    use_region_ = String_Op::to_number<int>( parameters->value( "Use_Region.value") );
    
    if(use_region_ == 1)
    {
        int nb_cat = 0;
        // get region indicator
        region_property_name_ = parameters->value( "Region_Indicator_Prop.value" );
        if ( region_property_name_.empty() )
        {
            error_mesgs->report( "Region_Indicator_Prop", "No region property selected" );
            return false;
        }
                
        region_property_ = simul_grid_->property( region_property_name_ );
        if( !region_property_ ) return false;

        
        if ( !is_integer_prop(region_property_, nb_cat) )
        {
            error_mesgs->report( "Region_Indicator_Prop", "Region indicator must be integer" );
            return false;
        }
        
        // get active region code
        std::string region_code_str = parameters->value( "Active_Region_Code.value" );
        if ( region_code_str.empty() )  
        {
            error_mesgs->report( "Active_Region_Code", "No active region code(s) specified" );
            return false;
        }

        active_region_ = String_Op::to_numbers<int>( region_code_str );

        // get previously simulated property
        use_pre_region_ = String_Op::to_number<int>( parameters->value( "Use_Previous_Simulation.value") );

        if ( use_pre_region_ == 1 )
        {
            pre_region_property_name_ = parameters->value( "Previous_Simulation_Prop.value" );
            if ( pre_region_property_name_.empty() )
            {
                error_mesgs->report( "Previous_Simulation_Prop", "No previously simulated property selected" );
                return false;
            }
            
            region_simulated_ = simul_grid_->property( pre_region_property_name_ );
            if( !region_simulated_ ) return false;

            int nb_faceis_simulated;
            if ( !is_integer_prop(region_simulated_, nb_faceis_simulated) )
            {
                error_mesgs->report( "Previous_Simulation_Prop", "Previously simulation value must be integer" );
                return false;
            }

            /*
            if ( nb_faceis_simulated > nb_facies_)
            {
                error_mesgs->report( "Previous_Simulation_Prop", "Previously simulated region contains more facies" );
                return false;
            }
            */
        }
    }
    return true;
}


bool Snesim_Std::get_affinity_data( const Parameters_handler* parameters,
                                   Error_messages_handler* error_mesgs )
{
    bool use_aff = String_Op::to_number<bool>( parameters->value( "Use_Affinity.value" ) );

    if (use_aff)
        local_aff_ = String_Op::to_number<int>( parameters->value( "Use_Local_Affinity.value") );
    
    //  std::string aff_grid_name;
    if(local_aff_ == 1)
    {
        int nb_cat = 0;

        aff_property_name_ = parameters->value( "Affinity_property.value" );
        if ( aff_property_name_.empty() )  
        {
            error_mesgs->report( "Affinity_property", "No affinity property selected" );
            return false;
        }
        
        aff_property_ = simul_grid_->property( aff_property_name_ );
        if( !aff_property_ ) return false;

        
        if ( !is_indicator_prop(aff_property_, nb_cat) )
        {
            error_mesgs->report( "Affinity_property", "Affinity indicator must be integer and start from 0" );
            return false;
        }
        

        std::string aff_list_str = parameters->value( "Affinity_categories.value" );
        std::cout << "Affinity ratios: " << aff_list_str << std::endl;
        if ( aff_list_str.empty() )  
        {
            error_mesgs->report( "Affinity_categories", "No affinity list specified" );
            return false;
        }

        std::vector<float> aff_vals = String_Op::to_numbers<float>( aff_list_str );
        aff_ncat_ = aff_vals.size() / 3;


        if ( (aff_vals.size() - aff_ncat_*3)!=0 )
        {
            error_mesgs->report("Affinity_categories", "Total number of affinities is incorrect" );
            return false;
        }
        
        
        if ( aff_ncat_ != nb_cat )
        {
            std::ostringstream error_stream;
            error_stream << "total affinity categories is " << nb_cat 
                         << " is not equal to the # of affinities specified";
            error_mesgs->report( "Affinity_categories", error_stream.str() );
            return false;
        }
        

        std::istringstream aff_list_stream(aff_list_str );
        float ratio;
        for(int na =0;na<aff_ncat_;na++)
        {
            for(int idir =0;idir < 3;idir++)
            {
                aff_list_stream >> ratio ;
                if ( ratio < 0.00001 )
                {
                    error_mesgs->report( "Affinity_categories", "Affinity factor must be > 0" );
                    return false;
                }
                aff_[idir].push_back( 1.0/ratio );
            }
        }       
    }
    else
    {       
        int global_aff = 0;
        if( use_aff )
            global_aff = String_Op::to_number<int>( parameters->value( "Use_Global_Affinity.value") );

        float ratio[3] = {1.0, 1.0 , 1.0};

        if(global_aff ==1)
        {            
            std::string aff_str = parameters->value( "Global_Affinity.value" );
            if ( aff_str.empty() )  
            {
                error_mesgs->report( "Global_Affinity", "No affinity list specified" );
                return false;
            }

            std::vector<float> aff_vals = String_Op::to_numbers<float>( aff_str );

            if (aff_vals.size() != 3)
            {
                error_mesgs->report( "Global_Affinity", "The # of affinity list must be 3" );
                return false;
            }

            ratio[0] = aff_vals[0];
            ratio[1] = aff_vals[1];
            ratio[2] = aff_vals[2];
        }
        
        aff_ncat_ = 1;
        if ( ratio[0] < 0.00001 || ratio[1] < 0.00001 || ratio[2] < 0.00001)
        {
            error_mesgs->report( "Affinity_categories", "Affinity factor must be > 0" );
            return false;
        }
        aff_[0].push_back(1./ratio[0]);
        aff_[1].push_back(1./ratio[1]);
        aff_[2].push_back(1./ratio[2]);
    }
    
    return true;
}


bool Snesim_Std::get_soft_prob_data( const Parameters_handler* parameters,
                                    Error_messages_handler* error_mesgs )
{
    bool use_prob = String_Op::to_number<bool>( parameters->value( "Use_ProbField.value" ) );

    if (use_prob)
        prob_field_ = String_Op::to_number<int>( parameters->value( "Use_ProbField.value") );
    
    //  std::string probfield_grid_name;
    if( prob_field_ == 1 )
    {
        probfield_property_names_ = parameters->value( "ProbField_properties.value" );

        if ( probfield_property_names_.empty() )
        {
            error_mesgs->report( "ProbField_properties", "No prob. field property selected" );
            return false;
        }
        
        std::vector< std::string > prop_names =
            String_Op::decompose_string( probfield_property_names_, ";", false );

        if (prop_names.size() != nb_facies_)
        {
            error_mesgs->report( "ProbField_properties", "Supply one and only one probability field per facies" );
            return false;
        }
        
        for( std::vector< std::string >::const_iterator it = prop_names.begin();
        it != prop_names.end(); ++it ) 
        {
            GsTLGridProperty* prop = simul_grid_->property( *it );
            if( !prop ) 
            {
                error_mesgs->report( "ProbField_properties", "The given probability field does not exist" );
                return false;
            }
            probfield_properties_.push_back( Property_map(prop) );
        }
        
        std::string tau_updating_str = parameters->value( "TauModelObject.value" );
        
        if( !tau_updating_str.empty() )
        {
            std::istringstream tau_stream( tau_updating_str );
            std::vector<float> tau_values;
            float tau;
            
            while( tau_stream >> tau )
                tau_values.push_back( tau );
            
            if ( tau_values.size()!=2 )
            {
                error_mesgs->report( "TauModelObject", "two Tau's must be specified" );
                return false;
            }
            else
            {
                tau1_ = tau_values[0];
                tau2_ = tau_values[1];
            }

            /*     // use only one Tau value for the soft data
            tau1_ = 1;
            if ( tau_values.size()==1 )
                tau2_ = tau_values[0];
            else
            {
                error_mesgs->report( "TauModelObject", "Only one Nu value is allowed" );
                return false;
            }
            */
        }
        else
        {
            error_mesgs->report( "TauModelObject", "No Tau's specified" );
            return false;
        }
        
        use_soft_cube_ = true;
    }
    
    return true;
}


bool Snesim_Std::get_simul_grid( const Parameters_handler* parameters,
		Error_messages_handler* error_mesgs )
{
    simul_grid_name_ = parameters->value( "GridSelector_Sim.value" );
    error_mesgs->report( simul_grid_name_.empty(), 
        "GridSelector_Sim", "No simulation grid selected" );
    
    property_name_prefix_ = parameters->value( "Property_Name_Sim.value" );

    if ( property_name_prefix_.empty() )
    {
        error_mesgs->report( "Property_Name_Sim", "No Property prefix specified" );
        return false;
    }
    
    // Get the simulation grid from the grid manager
    if( !simul_grid_name_.empty() ) 
    {
        simul_grid_ = dynamic_cast<RGrid*>( 
            Root::instance()->interface( 
            gridModels_manager + "/" + simul_grid_name_).raw_ptr() );
        
        if( !simul_grid_ ) 
        {
            std::ostringstream error_stream;
            error_stream <<  simul_grid_name_ <<  " is not a valid simulation grid";
            error_mesgs->report( "GridSelector_Sim", error_stream.str() );
            return false;
        }
        
        multireal_property_ =
            simul_grid_->add_multi_realization_property( property_name_prefix_ );
    }
        
    return true;
}


bool Snesim_Std::get_training_image( const Parameters_handler* parameters,
		Error_messages_handler* error_mesgs )
{
	training_image_name_ = parameters->value( "PropertySelector_Training.grid" );
	error_mesgs->report( training_image_name_.empty(), 
		"PropertySelector_Training", "No training image selected" );
	
	training_property_name_ = parameters->value( "PropertySelector_Training.property" );
	error_mesgs->report( training_property_name_.empty(), 
		"PropertySelector_Training", "No training property selected" );

	// Get the training image from the grid manager
	// and select the training property
	if( !training_image_name_.empty() ) 
	{
		training_image_ = dynamic_cast<RGrid*>( 
			    Root::instance()->interface( 
			    gridModels_manager + "/" + training_image_name_).raw_ptr() );
		
		if( !training_image_ ) 
		{
			std::ostringstream error_stream;
			error_stream <<  training_image_name_ <<  " is not a valid training image";
			error_mesgs->report( "PropertySelector_Training", error_stream.str() );
			return false;
		}
		
		training_image_->select_property( training_property_name_ );

        int nb_cat;
        if ( !is_indicator_prop(training_image_->selected_property(), nb_cat) )
        {
            error_mesgs->report( "PropertySelector_Training", "Training image must be integer and start from 0" );
            return false;
        }

        if ( nb_cat != nb_facies_ )
        {
            error_mesgs->report( "PropertySelector_Training", "Total # of facies is inconsistent betwen TI and Marginal" );
            return false;
        }

        return true;
	}
	else 
		return false;
}

bool Snesim_Std::get_marginal_cdf( const Parameters_handler* parameters,
                                  Error_messages_handler* error_mesgs )
{
    std::string marginal_cdf_str = parameters->value( "Marginal_Cdf.value" );
    
    // Initialize the marginal distribution
    // First extract the pdf input by the user then convert it to 
    // a cdf. Don't forget to check if the pdf values sum up to 1.
    if ( !marginal_cdf_str.empty() )
    {
        std::istringstream marginal_cdf_stream( marginal_cdf_str );
        std::vector<float> prob_values;
        float prob;
        while( marginal_cdf_stream >> prob )
            prob_values.push_back( prob );
        
        if ( prob_values.size() <  nb_facies_ )
        {
            error_mesgs->report( "Marginal_Cdf", "Too few values provided" );
            return false;
        }
        
        if ( prob_values.size() >  nb_facies_ )
        {
            error_mesgs->report( "Marginal_Cdf", "Too many values provided" );
            return false;
        }
        
        const float precision=0.00001;
        float cumul = std::accumulate( prob_values.begin(), prob_values.end(), 0.0 );

        if ( !GsTL::equals( cumul, float(1.0), precision ) ) 
        {
            error_mesgs->report( "Marginal_Cdf", "Values must sum up to 1" );
            return false;
        }
        else
        {
            marginal_.z_set( nb_facies_ );
            marginal_.p_set( prob_values.begin(), prob_values.end() );
        }
    }
    else	//get global proportion from training image
    {
        cout << "\nGet global proportion from the training image " << endl;
        
        int i;
        //int nxyz=training_image_->nxyz();
        int nxyz=training_image_->size();
        vector<float> cur_class_sum;
        
        for (i=0; i<nb_facies_; i++)
            cur_class_sum.push_back(0);
        
        const GsTLGridProperty* prop = training_image_->property(training_property_name_);
        
        for (i=0; i<nxyz; i++) 
            cur_class_sum[prop->get_value(i)] ++;
        
        for (i=0; i<nb_facies_; i++)
            cur_class_sum[i] /= nxyz;
        
        marginal_.z_set( nb_facies_ );
        marginal_.p_set( cur_class_sum.begin(), cur_class_sum.end() );
    }
    
    // initialize ccdf_
    ccdf_ = marginal_;
    
    cout << "\nTarget Marginal distribution:" << endl;
    CdfType::z_iterator z_it = marginal_.z_begin();
    CdfType::p_iterator p_it = marginal_.p_begin();
    for( ; z_it != marginal_.z_end() ; ++z_it, ++p_it )
    {
        cout << "(" << *z_it << ", " << *p_it << ")   " ;
    }
    cout << endl;

    return true;
}

void Snesim_Std::update_ccdf_from_available(CdfType& ccdf)
{
    int i;
    //int nxyz=simul_grid_->nxyz();
    int nxyz=simul_grid_->size();
    GsTLGridProperty* prop = simul_grid_->selected_property();
    vector<float> cur_class_sum(nb_facies_, 0);
    
    for (i=0; i<nxyz; i++) 
    {
        if ( prop->is_informed(i) )
            cur_class_sum[ prop->get_value(i) ] ++;
    }
    
    for (i=0; i<nb_facies_; i++)
        cur_class_sum[i] /= nxyz;
        //cur_class_sum[i] = cur_class_sum[i]/nxyz + ccdf.prob(i);
    
    ccdf.p_set( cur_class_sum.begin(), cur_class_sum.end() );
}

void Snesim_Std::copy_pre_simulation_data()
{
    GsTLGridProperty* prop = simul_grid_->selected_property();
    
    //for (int i=0; i<simul_grid_->nxyz(); i++) 
    for (int i=0; i<simul_grid_->size(); i++) 
    {
        if ( region_simulated_->is_informed(i) )
            prop->set_value(region_simulated_->get_value(i), i);
    }
}

bool Snesim_Std::get_hard_data( const Parameters_handler* parameters,
		Error_messages_handler* error_mesgs )
{
    std::string harddata_grid_name = parameters->value( "Hard_Data.grid" );
    
    if( !harddata_grid_name.empty() ) 
    {
        std::string hdata_prop_name = parameters->value( "Hard_Data.property" );
        error_mesgs->report( hdata_prop_name.empty(), 
                                         "Hard_Data", "No property name specified" );
        
        // Get the harddata grid from the grid manager
        bool ok = geostat_utils::create( harddata_grid_, harddata_grid_name, 
                                        "Hard_Data", error_mesgs );

        if( !ok )     return false;
        
        if ( harddata_grid_->classname() != "Point_set" )
        {
            error_mesgs->report( "Hard_Data", "hard data must be given in Point Set" );
            return false;
        }

        harddata_property_ = harddata_grid_->property( hdata_prop_name );
        if( harddata_property_ ) 
        {
            property_copier_ = 
                    Property_copier_factory::get_copier( harddata_grid_, simul_grid_ );

            if( !property_copier_ ) 
            {
                std::ostringstream message;
				appli_message("Ting: cannot find copier");
                message << "It is currently not possible to copy a property from a "
                        << harddata_grid_->classname() << " to a " 
                        << simul_grid_->classname() ;
                error_mesgs->report( !property_copier_, "Assign_Hard_Data", message.str() );
                return false;
            }
        }
        else
        {
            std::ostringstream error_stream;
            error_stream <<  harddata_grid_name 
                                <<  " does not have a property called " 
                                << hdata_prop_name;
            error_mesgs->report( "Hard_Data", error_stream.str() );
            return false;
        }
    }
    return true;
}

bool Snesim_Std::get_vert_prob_data( const Parameters_handler* parameters,
                                    Error_messages_handler* error_mesgs )
{
    string vertical_grid_name = parameters->value( "VerticalPropObject.value" );
    
    if( !vertical_grid_name.empty() )
    {
        vertical_curve_grid_ = dynamic_cast<Strati_grid*>( 
                                Root::instance()->interface( gridModels_manager + "/" + 
                                vertical_grid_name).raw_ptr() );
        
        if( !vertical_curve_grid_ ) 
        {
            std::ostringstream error_stream;
            error_stream <<  vertical_grid_name 
                        <<  " is not a valid vertical proportion grid";
            error_mesgs->report( "VerticalPropObject", error_stream.str() );
            return false;
        }
        
        vertical_properties_names_ = parameters->value( "VerticalProperties.value" );

        if ( vertical_properties_names_.empty() )
        {
            error_mesgs->report( "VerticalProperties", "No prob. field property selected" );
            return false;
        }
        
        std::vector< std::string > vert_prop_names =
                String_Op::decompose_string( vertical_properties_names_, ";", false );
        
        if ( vert_prop_names.size() != nb_facies_ )
        {
            error_mesgs->report( "VerticalProperties", "Supply one probability field per facies" );
            return false;
        }
        
        for (int i=0; i<nb_facies_; i++)
        {
            GsTLGridProperty* prop = vertical_curve_grid_->property( vert_prop_names[i] );
            if( !prop ) 
            {
                error_mesgs->report( "VerticalProperties", "Vertical property does not exist" );
                return false;
            }
            vertical_properties_.push_back( prop );
        }
        
        use_vertical_ = true;
    }

    return true;
}

bool Snesim_Std::get_search_ellipsoid( const Parameters_handler* parameters,
		                                Error_messages_handler* error_mesgs )
{
	GsTLTriplet ranges, angles;
	bool extract_ok = geostat_utils::extract_ellipsoid_definition( ranges, angles, 
							"Search_Ellipsoid.value", parameters, error_mesgs );
	if( !extract_ok ) return false;

	extract_ok = geostat_utils::is_valid_range_triplet( ranges );
	error_mesgs->report( !extract_ok,  "Search_Ellipsoid",
						"Ranges must verify: major range >= " 
						"medium range >= minor range >= 0" );
	if( !extract_ok ) return false;
	
	const RGrid_geometry* geometry = simul_grid_->geometry();
	const GsTLCoordVector& cell_dims = geometry->cell_dims();
	
	// check that the dimensions of the search radius expressed in # of cells 
	// is not 0 (i.e. that ranges[i] > cell_dims[i]
	GsTLTripletTmpl<int> cell_ranges;
	cell_ranges[0] = ranges[0] / cell_dims[0];
	cell_ranges[1] = ranges[1] / cell_dims[1];
	cell_ranges[2] = ranges[2] / cell_dims[2];

	if( cell_ranges[0] == 0 || cell_ranges[1] == 0 || cell_ranges[2] == 0 ) 
	{
		error_mesgs->report( "Search_Ellipsoid",
			"Some of the ranges of the search ellipsoid are \n"
			"smaller than a single grid cell. Increase the \n"
			"dimensions of the search ellipsoid (they are expressed\n"
			"in actual unit, not in # of cells" );
		return false;
	}

    float factor = cell_ranges[0]*cell_ranges[1]*cell_ranges[2]/(float)max_prevcond_;
    if ( factor < 0.8 )
    {
        factor = std::sqrt( factor );
        cell_ranges[0] = floor( cell_ranges[0]/factor );
        cell_ranges[1] = floor( cell_ranges[1]/factor );
        cell_ranges[2] = floor( cell_ranges[2]/factor );
    }
	
	create_window_geom( cell_ranges[0], cell_ranges[1], cell_ranges[2],
		                angles[0], angles[1], angles[2], max_prevcond_ );

    tmpl_nz_ = cell_ranges[2] ;   // added to use 2D template for 3D TI

    return true;
}


// save the probability at each horizontal layer
// for the purpose of checking consistency between
// the vertical proportion curve and soft cube
void Snesim_Std::check_prob_consistency( const Parameters_handler* parameters, 
                                        Error_messages_handler* error_mesgs )
{
    // initialize probability arrays at each horizontal layer
    if ( use_vertical_ && use_soft_cube_ )
    {
        for (int i=0; i<simul_grid_->nz(); i++ )
        {
            vector<float> layer_prop(nb_facies_, 0);
            //for (int j=0; j<nb_facies_; j++)
            //	layer_prop.push_back(0);
            
            vert_prop_vector_.push_back(layer_prop);
            soft_prop_vector_.push_back(layer_prop);
        }
    }
    
    // check vertical proporiton consistency
    if( use_vertical_ ) 
        check_vertical_prop( parameters, error_mesgs );
    
    // check soft probability cube with global target proportion
    if( prob_field_ == 1 )
        check_soft_global();
    
    if ( use_vertical_ && use_soft_cube_ )
        check_vertical_soft_consistency();
    
    if ( use_vertical_ )
        cout << endl << "Because you are using vertical proportion curve" 
             << endl << "The global target proportion will be neglected " << endl;
}


bool Snesim_Std::is_expansion_factor_dividable(int ncoarse)
{
    if ( iso_expansion_ ) return true;

    if ( ncoarse == nb_multigrids_ ) return false;

    for (int i=0; i<3; i++)
    {
        if ( i==2 && simul_grid_->nz()<=1 ) continue;

        int f1 = expansion_factor_[ncoarse-1][i];
        int f2 = expansion_factor_[ncoarse][i];

        if ( f1==1 && f2>2 )  return false;
        if ( f2/f1 > 2 ) return false;
    }

    return true;
}

//bool Snesim_Std::is_expansion_factor_dividable(int ncoarse)
//{
//    if ( iso_expansion_ ) return true;
//
//    if ( ncoarse == nb_multigrids_ ) return false;
//
//    for (int i=0; i<3; i++)
//    {
//        if ( i==2 && simul_grid_->nz()<=1 ) continue;
//
//        int f1 = expansion_factor_[ncoarse-1][i];
//        int f2 = expansion_factor_[ncoarse][i];
//
//        if ( f1==1 && f2%2!=0 )  return false;
//        if ( (f2/f1) % 2 != 0) return false;
//    }
//
//    return true;
//}

bool Snesim_Std::get_simulation_choice(int sg_no, int ncoarse)
{	
    if( subgrid_choice_==0 )    // no subgrid
        return (sg_no==NUM_SG); 
    else if( ncoarse == nb_multigrids_ ) 
        return (sg_no ==NUM_SG);       // no subgrid for the coursest grid
    else if( iso_expansion_==0 && !is_expansion_factor_dividable(ncoarse) )
        return (sg_no==NUM_SG); 
    else  // with subgrid
    {
        bool sg_avoid  = (sg_no==0)&&(simul_grid_->nz()<=1) && !property_copier_
                                    && is_expansion_factor_dividable(ncoarse);

        bool sg_coarse  = (sg_no <NUM_SG);  

        return (!sg_avoid)&&(sg_coarse);
        //return sg_coarse;
    }


    /*
    if( subgrid_choice_==0 )    // no subgrid
        return (sg_no==NUM_SG); 
    else if( iso_expansion_==0 && ncoarse == 1 )
        return (sg_no==NUM_SG); 
    else  // use subgrid option
    {
        if( ncoarse == nb_multigrids_ )  // no subgrid in the coarsest grid
            return (sg_no ==NUM_SG);  
        else if( ncoarse == 1 )
            return sg_no <NUM_SG;
        else if ( !is_expansion_factor_dividable(ncoarse) ) // no subgrid for non-power factors
            return (sg_no ==NUM_SG);  
        else
        {
            bool sg_avoid  = (sg_no==0)&&(simul_grid_->nz()<=1) && is_power_factor_;	  
            bool sg_coarse = (sg_no <NUM_SG);
            return (!sg_avoid)&&(sg_coarse);
        }
    }
    */

    // if want to use same expansion factor for two multi-grids, then comment the upper part
    // and use the following simulation grid selection.
    /* 
    bool sg_avoid  = (sg_no==0)&&(simul_grid_->nz()<=1) && is_power_factor_;	   
    
    bool sg_coarse;
    if( ncoarse == nb_multigrids_ ) 
        sg_coarse = (sg_no ==NUM_SG); 
    else
        sg_coarse = (sg_no <NUM_SG);  
    
    if(subgrid_choice_==0)
        return (sg_no==NUM_SG); 
    else
        return (!sg_avoid)&&(sg_coarse);
        //return sg_coarse;
   */
}


int Snesim_Std::Nint(double x)
{
	return x>=0 ? static_cast<int> (x + 0.5) : static_cast<int> (x - 0.5) ;
}


bool Snesim_Std::simulate_one_realization( SmartPtr<Progress_notifier>& progress_notifier, 
                                          GsTLGridProperty* prop, int nreal )
{
    CdfType ccdf = ccdf_;     // copy of current ccdf
    std::vector<CdfType> vert_ccdf = vert_ccdf_;

    // having property with previous region simulatation + hard data
    if ( use_pre_region_ == 1)
        copy_pre_simulation_data();

    // loop on all coarse grids
    for( int ncoarse = nb_multigrids_ ; ncoarse >=1 ; ncoarse-- ) 
    {
        //if ( ncoarse == nb_multigrids_-1)  return 0;

        appli_message("working on coarse grid " << ncoarse );
        progress_notifier->message() << "working on realization " << nreal << "\n"
            << "coarse grid " << ncoarse << gstlIO::end;
        if( !progress_notifier->notify() ) 
        {
            // the execution was aborted. We could not finish the simulation of the 
            // current property, so delete the current property
            appli_warning( "deleting property " << prop->name() );
            simul_grid_->remove_property( prop->name() );
            return 1;
        }

        if ( is_view_intermediate_ && ncoarse < nb_multigrids_ )
        {
            ostringstream ostr; 
            ostr << ncoarse+1;
            string previous_prop = prop->name() + "_mg_" + ostr.str();
            GsTLGridProperty* prop2;
            prop2 = simul_grid_->select_property(previous_prop);
            if ( !prop2)
                prop2 = geostat_utils::add_property_to_grid(simul_grid_,previous_prop);

            GsTLGridProperty* prop1 = simul_grid_->select_property(prop->name());

            //for (int ik=0; ik<simul_grid_->nxyz(); ik++) 
            for (int ik=0; ik<simul_grid_->size(); ik++) 
            {
                if ( prop1->is_informed(ik) )
                    prop2->set_value(prop1->get_value(ik), ik);
            }
        }

        if ( use_vertical_ )      sampler2_->set_grid_level( ncoarse );

        simul_grid_->select_property( prop->name() );
        simul_grid_->set_level( ncoarse );

        if( property_copier_ ) 
            property_copier_->copy( harddata_grid_, harddata_property_, simul_grid_, prop );

        init_random_path(nb_multigrids_ - ncoarse);

        simul_grid_->select_property( prop->name() );

        // Simulate the coarse grid			
        appli_message("simulating the coarse grid..." );

        for(int sg_no =0;sg_no<=NUM_SG;sg_no++)
        {
            if( get_simulation_choice(sg_no, ncoarse) )
            {
                appli_message("working on subgrid number: " << sg_no);

                if ( grid_paths_[sg_no].size()==0 )    continue;

                Colocated_value* coloc_rot = NULL;
                if(local_rot_ ==1)  
                    coloc_rot = new Colocated_value( rot_property_ );

                Colocated_value* coloc_aff = NULL;
                if(local_aff_ ==1)
                    coloc_aff = new Colocated_value( aff_property_ );				

                simul_grid_->select_property( prop->name() );

                // rotation + affinity colocated function pair    
                std::pair<Colocated_value*,Colocated_value*> coloc_pair(coloc_rot,coloc_aff);

                // first initial grids do normal snesim.
                training_image_->select_property( training_property_name_ );

                // setting training image to finest grid level
                training_image_->set_level(1);	    	    

                Grid_template* mg_template = multgrid_template(window_geom_sg_[sg_no]);
                Window_neighborhood* training_nbd =
                    //SmartPtr<Neighborhood> training_nbd =
                    training_image_->window_neighborhood(*mg_template);

                cout << "template size = " << int(mg_template->size()) << endl;
                training_nbd->select_property( training_property_name_);

                progress_notifier->notify();

                //typedef Tree_list<std::pair<Colocated_value*, Colocated_value*> > TreeList;
                TreeList mptree( training_image_->begin(), training_image_->end(),
                    training_nbd,
                    mg_template->begin(), mg_template->end(),
                    &coloc_pair, angles_,
                    aff_[0],aff_[1],aff_[2],
                    ncoarse, nb_facies_, 
                    mg_template->size(), cmin_,
                    expansion_factor_ );

                //if ( iso_expansion_==0 )  
                //    mptree.set_anisotropic_expansion_factor( expansion_factor_[ncoarse-1] );

                progress_notifier->notify();

                simul_grid_->select_property( prop->name() );

                // setting multiple grid level for simulation grid
                simul_grid_->set_level( ncoarse);

                Window_neighborhood* neighbors =
                    //SmartPtr<Neighborhood> neighbors =
                    simul_grid_->window_neighborhood( *window_geom_sg_[sg_no] );

                RGrid::random_path_iterator  
                    path_begin( simul_grid_,simul_grid_->select_property( prop->name() ),
                    0, int(grid_paths_[sg_no].size()),
                    TabularMapIndex(&grid_paths_[sg_no]) );

                RGrid::random_path_iterator  
                    path_end( simul_grid_,simul_grid_->select_property( prop->name() ),
                    int(grid_paths_[sg_no].size()), int(grid_paths_[sg_no].size()),
                    TabularMapIndex(&grid_paths_[sg_no]) );

                NodeDropped dropped_nodes( grid_paths_[sg_no], revisit_criterion_ );
                vector<int> grid_path_new;

                if( use_soft_cube_ && (!use_vertical_) ) 
                {
                    //*
                    typedef Tau_updating<CdfType, Property_map> TauUpdater;
                    TauUpdater cdf_updater( probfield_properties_.begin(),
                        probfield_properties_.end(), marginal_, tau1_, tau2_ );
                    Updater_sampler<TauUpdater,ServoSystem> updater_sampler( cdf_updater, *sampler1_ );
                    //*/
                    /*
                    typedef Nu_updating_Val<CdfType, Property_map> NuUpdater;
                    NuUpdater cdf_updater( probfield_properties_.begin(),
                    probfield_properties_.end(), marginal_, nu_ );
                    Updater_sampler<NuUpdater,ServoSystem> updater_sampler( cdf_updater, *sampler1 );
                    //*/

                    int status = sequential_simulation( path_begin, path_end,
                        *neighbors, ccdf,
                        mptree, marginal_, 
                        updater_sampler, 
                        progress_notifier.raw_ptr(),
                        dropped_nodes );	
                    if( status == -1 ) 
                    {
                        clean( prop );
                        return 1;
                    }

                    if ( is_view_node_drop_ )   dropped_nodes.setDNProp(simul_grid_, data_drop_prop_);

                    for (int iter=0; iter<revisit_iter_nb_; iter++)
                    {
                        dropped_nodes.GetRevisitNodes( grid_path_new );

                        if (  grid_path_new.size()>0 )
                        {
                            sampler1_->removeSimulatedNode( simul_grid_, prop, grid_path_new );
                            dropped_nodes.ResetPath( grid_path_new );

                            RGrid::random_path_iterator  
                                path_begin2( simul_grid_,simul_grid_->select_property( prop->name() ),
                                0, int(grid_path_new.size()), 
                                TabularMapIndex(&grid_path_new) );

                            RGrid::random_path_iterator  
                                path_end2( simul_grid_,simul_grid_->select_property( prop->name() ),
                                int(grid_path_new.size()), int(grid_path_new.size()),
                                TabularMapIndex(&grid_path_new) );

                            status = sequential_simulation( path_begin2, path_end2,
                                *neighbors, ccdf,
                                mptree, marginal_, 
                                updater_sampler, 
                                progress_notifier.raw_ptr(),
                                dropped_nodes );	

                            if ( is_view_node_drop_ )     dropped_nodes.setDNProp(simul_grid_, data_drop_prop_);
                        }
                    }
                }
                else if ( use_soft_cube_ && use_vertical_ )
                {
                    //*
                    typedef Tau_updating<CdfType, Property_map> TauUpdater;
                    TauUpdater cdf_updater( probfield_properties_.begin(),
                        probfield_properties_.end(), marginal_, tau1_, tau2_ );
                    Updater_sampler<TauUpdater,LayerServoSystem> updater_sampler( cdf_updater, *sampler2_ );
                    //*/
                    /*
                    typedef Nu_updating_Val<CdfType, Property_map> NuUpdater;
                    NuUpdater cdf_updater( probfield_properties_.begin(),
                    probfield_properties_.end(), marginal_, nu_ );
                    Updater_sampler<NuUpdater,LayerServoSystem> updater_sampler( cdf_updater, *sampler2 );
                    //*/

                    int status = layer_sequential_simulation( path_begin, path_end,
                        *neighbors, vert_ccdf,
                        mptree, vert_marginal_, 
                        updater_sampler, 
                        progress_notifier.raw_ptr(),
                        get_vertical_index_,
                        dropped_nodes );	
                    if( status == -1 ) 
                    {
                        clean( prop );
                        return 1;
                    }

                    if ( is_view_node_drop_ )     dropped_nodes.setDNProp(simul_grid_, data_drop_prop_);

                    for (int iter=0; iter<revisit_iter_nb_; iter++)
                    {
                        dropped_nodes.GetRevisitNodes( grid_path_new );

                        if (  grid_path_new.size()>0 )
                        {
                            sampler2_->removeSimulatedNode( simul_grid_, prop, grid_path_new );
                            dropped_nodes.ResetPath( grid_path_new );

                            RGrid::random_path_iterator  
                                path_begin2( simul_grid_,simul_grid_->select_property( prop->name() ),
                                0, int(grid_path_new.size()),
                                TabularMapIndex(&grid_path_new) );

                            RGrid::random_path_iterator  
                                path_end2( simul_grid_,simul_grid_->select_property( prop->name() ),
                                int(grid_path_new.size()), int(grid_path_new.size()),
                                TabularMapIndex(&grid_path_new) );

                            status = layer_sequential_simulation( path_begin2, path_end2,
                                *neighbors, vert_ccdf,
                                mptree, vert_marginal_, 
                                updater_sampler, 
                                progress_notifier.raw_ptr(),
                                get_vertical_index_,
                                dropped_nodes );	

                            if ( is_view_node_drop_ )     dropped_nodes.setDNProp(simul_grid_, data_drop_prop_);
                        }
                    }
                }
                else if ( (!use_soft_cube_) && use_vertical_ )
                {
                    int status = layer_sequential_simulation( path_begin, path_end,
                        *neighbors, vert_ccdf,
                        mptree, vert_marginal_, *sampler2_, 
                        progress_notifier.raw_ptr(),
                        get_vertical_index_,
                        dropped_nodes );	
                    if( status == -1 ) 
                    {
                        clean( prop );
                        return 1;
                    }

                    if ( is_view_node_drop_ )     dropped_nodes.setDNProp(simul_grid_, data_drop_prop_);

                    for (int iter=0; iter<revisit_iter_nb_; iter++)
                    {
                        dropped_nodes.GetRevisitNodes( grid_path_new );

                        if (  grid_path_new.size()>0 )
                        {
                            sampler2_->removeSimulatedNode( simul_grid_, prop, grid_path_new );
                            dropped_nodes.ResetPath( grid_path_new );

                            RGrid::random_path_iterator  
                                path_begin2( simul_grid_,simul_grid_->select_property( prop->name() ),
                                0, int(grid_path_new.size()),
                                TabularMapIndex(&grid_path_new) );

                            RGrid::random_path_iterator  
                                path_end2( simul_grid_,simul_grid_->select_property( prop->name() ),
                                int(grid_path_new.size()), int(grid_path_new.size()),
                                TabularMapIndex(&grid_path_new) );

                            status = layer_sequential_simulation( path_begin2, path_end2,
                                *neighbors, vert_ccdf,
                                mptree, vert_marginal_, *sampler2_, 
                                progress_notifier.raw_ptr(),
                                get_vertical_index_,
                                dropped_nodes );		

                            if ( is_view_node_drop_ )     dropped_nodes.setDNProp(simul_grid_, data_drop_prop_);
                        }
                    }
                }
                else  // no soft, no vertical
                {
                    int status = sequential_simulation( path_begin, path_end,
                        *neighbors, ccdf,
                        mptree, marginal_, *sampler1_, 
                        progress_notifier.raw_ptr(),
                        dropped_nodes );	
                    if( status == -1 ) 
                    {
                        clean( prop );
                        return 1;
                    }

                    if ( is_view_node_drop_ )   dropped_nodes.setDNProp(simul_grid_, data_drop_prop_);

                    for (int iter=0; iter<revisit_iter_nb_; iter++)
                    {
                        dropped_nodes.GetRevisitNodes( grid_path_new );

                        if (  grid_path_new.size()>0 )
                        {
                            sampler1_->removeSimulatedNode( simul_grid_, prop, grid_path_new );
                            dropped_nodes.ResetPath( grid_path_new );

                            RGrid::random_path_iterator  
                                path_begin2( simul_grid_,simul_grid_->select_property( prop->name() ),
                                0, int(grid_path_new.size()),
                                TabularMapIndex(&grid_path_new) );

                            RGrid::random_path_iterator  
                                path_end2( simul_grid_,simul_grid_->select_property( prop->name() ),
                                int(grid_path_new.size()), int(grid_path_new.size()),
                                TabularMapIndex(&grid_path_new) );

                            status = sequential_simulation( path_begin2, path_end2,
                                *neighbors, ccdf,
                                mptree, marginal_, *sampler1_, 
                                progress_notifier.raw_ptr(),
                                dropped_nodes );	

                            if ( is_view_node_drop_ )     dropped_nodes.setDNProp(simul_grid_, data_drop_prop_);
                        } 
                    }
                }

                appli_message("Finished simulating sub-grid " << sg_no << std::endl );
            }
        } 
        // Uncomment the 2 following lines to undo the data assignment
        // But then there will be a problem with subgrids and conditioning...
        //if( property_copier_ && ncoarse != 1 )
        //    property_copier_->undo_copy();

        appli_message("Finished simulating multiple-grid " << ncoarse << std::endl );    
    }

    return 0;
}


// Connect to the plugin interface
//GEOSTAT_PLUGIN(Snesim_Std);
/*
extern "C" __declspec(dllexport) int Snesim_Std_init() 
{
	GsTLlog << "\n\n registering plugin snesim_std" << "\n"; 
	SmartPtr<Named_interface> ni = 
		Root::instance()->interface( geostatAlgo_manager ); 
	Manager* dir = dynamic_cast<Manager*>( ni.raw_ptr() ); 
	if( !dir ) 
	{ 
		GsTLlog << "Directory " << geostatAlgo_manager << " does not exist \n"; 
		return 1; 
	} 
	Snesim_Std toto; 
	dir->factory( toto.name(), Snesim_Std::create_new_interface ); 
	return 0; 
}
*/

