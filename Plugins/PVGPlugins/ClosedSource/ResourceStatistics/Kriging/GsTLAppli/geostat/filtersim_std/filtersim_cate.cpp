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


#include <GsTLAppli/grid/grid_model/strati_grid.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/rgrid_neighborhood.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/grid/grid_model/grid_initializer.h>
#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/geostat/utilities.h>
#include <GsTLAppli/appli/utilities.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/utils/gstl_plugins.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/progress_notifier.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/math/random_numbers.h>

#include <sstream>
#include <fstream>
#include <numeric>
#include <cmath>
#include <vector>
#include <algorithm>
#include <ctime>

#include "filtersim_cate.h"
#include "filtersim_tau_updating.h"
//#include "filtersim_nu_updating.h"
#include "filtersim_servo_system.h"
#include "sequential_patch_simulation.h"

/*
 * function to create a SGEMS interface for Filtersim_Cate
 */
Named_interface* Filtersim_Cate::create_new_interface( std::string& ) 
{
	return new Filtersim_Cate;
}


/*
 * constructor function for Filtersim_Cate
 */
Filtersim_Cate::Filtersim_Cate() 
{
	window_geom_ = 0;
	simul_grid_ = 0;
	multireal_property_ = 0;

    my_filters_ = 0;

	training_image_ = 0;
	
    harddata_grid_ = 0;
	harddata_property_ = 0;

	is_viewscore_ = 0;
    is_view_intermediate_ = 0;
    is_view_indicator_ = 0;

    is_dist_from_pixel_ = 0;
    use_default_filter_ = 0;
	is_categorical_ = 0;
    treat_cate_as_cont_ = 1;    // it is true for continuous variable
    transcon_data_ = 0;           // by default, there is no post-processing
	use_soft_field_ = 0;
}


/*
 * destructor function to release memory
 */
Filtersim_Cate::~Filtersim_Cate() 
{
    if ( window_geom_ )
        delete window_geom_;

    delete my_filters_;
}


/*
 * function to read parameters file .ui interface
 */
bool Filtersim_Cate::initialize( const Parameters_handler* parameters,
						   Error_messages_handler* error_mesgs ) 
{
	nb_reals_ = String_Op::to_number<int>( parameters->value( "Nb_Realizations.value" ) );
	nb_multigrids_ = String_Op::to_number<int>( parameters->value( "Nb_Multigrids_ADVANCED.value" ) );

    is_dist_from_pixel_ = String_Op::to_number<int>(parameters->value( "Use_Normal_Dist.value" ));
    use_score_dist_ = (is_dist_from_pixel_==0);        // use score to find the closest prototype

    transcon_data_ = String_Op::to_number<int>(parameters->value( "Trans_Result.value" ));

    seed_ = String_Op::to_number<int>( parameters->value( "Seed.value" ) );
    if ( seed_ == 0 )
    {
        error_mesgs->report( "Seed", "The seed number cannot be zero" );
        return false;
    }

    get_debug_level( parameters );
    get_nb_bins( parameters );

    if ( !get_marginal_data( parameters, error_mesgs ) )
        return false;
    
    if ( !get_servo_system( parameters, error_mesgs ) )
        return false;

    if ( !get_simul_grid( parameters, error_mesgs ) )
        return false;
	
    if ( !get_training_image( parameters, error_mesgs ) )
        return false;
	
    if ( !get_hard_data( parameters, error_mesgs ) )
        return false;

    if ( !get_soft_data( parameters, error_mesgs ) )
        return false;
   
    if ( !get_region_data( parameters, error_mesgs ) )
        return false;

    if ( !get_TI_scan_template( parameters, error_mesgs ) )
        return false;
    
    if ( !get_patch_template( parameters, error_mesgs ) )
        return false;

    if ( !calculate_max_mgrid( error_mesgs ) )
        return false;

    if ( !get_cmin( parameters, error_mesgs ) )
        return false;
    
    if ( !get_data_weight( parameters, error_mesgs ) )
        return false;

    if ( !get_filters( parameters, error_mesgs ) )
        return false;
    
    // create searching template
    window_geom_ = my_filters_->get_window_geometry();

    print_parameters();	
	
	// Initialize the global random number generator
	Global_random_number_generator::instance()->seed( seed_ );
	
	if( !error_mesgs->empty() )
		return false;
	
	return true;
}


/*
 * main executable function
 */
int Filtersim_Cate::execute( GsTL_project* proj ) 
{
    std::cout << "\n\nRunning algo Filtersim_Cate... \n"; 
    std::cout << "-- Score calculation and pattern partition are only performed once --\n\n"; 

	// Set up a progress notifier	
	int total_steps, frequency;
    pre_progress_notifer(total_steps, frequency);

	SmartPtr<Progress_notifier> progress_notifier = 
		        utils::create_notifier( "Running Filtersim_Cate", total_steps, frequency );

    // the size of filter_weight is
    //      (nb_facies-1)*filter :  if nb_facies > 1 && treat_cate_as_cont_=0
    //      filter               :  if nb_facies < 2 or treat_cate_as_cont_=1
    get_filter_score_weight();

    pattern_mapping_.set_grid( simul_grid_, training_image_ );

    // loop on all realizations
	for( int nreal = 1; nreal <= nb_reals_; nreal++ ) 
	{
        //cout << "\nWorking on realization " << nreal << endl;
		progress_notifier->message() << "\n" << "Working on realization " << nreal 
                                                     << " ... please be patient\n" << gstlIO::end;
		if( ! progress_notifier->notify() ) return 1;
		
        // add new property
		GsTLGridProperty* prop = multireal_property_->new_realization();

        bool not_success = simulate_one_realization( progress_notifier, prop, nreal );
        if ( not_success )    return not_success;

	}	// end of multi-realizations

    // more property clean???

    appli_message("... done" << std::endl );

	return 0;		
}


//-- starting utility functions for initialization --

/*
 * function to get the debug level
 */
void Filtersim_Cate::get_debug_level( const Parameters_handler* parameters )
{
	int level = String_Op::to_number<int>(parameters->value( "Debug_Level.value" ));

    switch(level)
    {
        case 3:
            is_view_indicator_ = 1;
        case 2:
            is_view_intermediate_ = 1;
        case 1:
            is_viewscore_ = 1;
        default:
            break;
    }
}

/*
 * function to get marginal data
 */
bool Filtersim_Cate::get_marginal_data( const Parameters_handler* parameters,
                                  Error_messages_handler* error_mesgs )
{
	nb_facies_ = 1;

    //is_categorical_ = String_Op::to_number<int>(parameters->value("Is_Catv.value"));
    is_categorical_ = 1;

    // get variable type
    nb_facies_ = String_Op::to_number<unsigned int>( parameters->value( "Nb_Facies.value" ) );

    // for categorical variable
    /*
    std::string marginal_cpdf_str = parameters->value( "Marginal_Cpdf.value" );
    if ( marginal_cpdf_str.empty() )
    {
        error_mesgs->report( "Marginal_Cpdf", "No cpdf distribution specified" );
        return false;
    }
    */
    //std::string marginal_cpdf_str = "1 0";

    treat_cate_as_cont_ = String_Op::to_number<int>(parameters->value("Treat_Cate_As_Cont.value"));

    // two facies categories is equivalent to the continuous variable
    if ( nb_facies_ == 2 )
        treat_cate_as_cont_ = 1;

    //return get_marginal_cdf(marginal_cpdf_str, error_mesgs);
    return true;
}


/*
 * function to get servosystem
 */
bool Filtersim_Cate::get_servo_system( const Parameters_handler* parameters,
                                  Error_messages_handler* error_mesgs )
{
    /*
    serv_ = String_Op::to_number<double>( parameters->value( "Constraint_Marginal_ADVANCED.value" ) );

    if ( serv_ < 0 ||  serv_ > 1 )
    {
        error_mesgs->report( "Constraint_Marginal_ADVANCED", "Input a number between 0 and 1" );
        return false;
    }
    */

    serv_ = 0.0;

    return true;
}


/*
 * function to get target cdf (for categorical variable)
 */
bool Filtersim_Cate::get_marginal_cdf(std::string marginal_cpdf_str, 
                                 Error_messages_handler* error_mesgs)
{
    /*
	std::istringstream marginal_cpdf_stream( marginal_cpdf_str );

	float prob;
	while( marginal_cpdf_stream >> prob )
		target_cpdf_.push_back( prob );
	
    // check pdf size
    if ( target_cpdf_.size() <  nb_facies_ )
    {
        error_mesgs->report( "Marginal_Cpdf", "Too few values provided" );
        return false;
    }
    
    if ( target_cpdf_.size() >  nb_facies_ )
    {
        error_mesgs->report( "Marginal_Cpdf", "Too many values provided" );
        return false;
    }
    
    float cumul = std::accumulate( target_cpdf_.begin(), target_cpdf_.end(), 0.0 );
    if ( !GsTL::equals( cumul, float(1.0), float(0.00001) ) ) 
    {
        error_mesgs->report( "Marginal_Cpdf", "Values must sum up to 1" );
        return false;
    }
    */

    target_cpdf_.resize(nb_facies_, 1.0/nb_facies_);
    //target_cpdf_[0]=0.5;
    //target_cpdf_[1]=0.5;

    return true;
}



/*
 * function to get marginal proportions from TI
 */
bool Filtersim_Cate::get_marginal_cdf()
{
    target_cpdf_.resize(nb_facies_, 0.);
    GsTLGridProperty*prop = training_image_->select_property( training_property_name_ );
    int nsize = 0;

    for (int i=0; i<prop->size(); i++)
    {
        if ( prop->is_informed(i) )
        {
            target_cpdf_[ (int) prop->get_value(i) ] ++;
            nsize++;
        }
    }

    if ( nsize>0 )
    {
        for (int j=0; j<nb_facies_; j++)
            target_cpdf_[j] /= nsize;

        return true;
    }
    else
        return false;
}

/*
 * function to get marginal proportions from any property
 */
bool Filtersim_Cate::get_current_cdf( GsTLGridProperty*prop )
{
    current_prop_.resize(nb_facies_, 0.);

    int nsize=0;
    for (int i=0; i<prop->size(); i++)
    {
        if ( region_[i] && prop->is_informed(i) )
        {
            nsize ++;
            current_prop_[ (int) prop->get_value(i) ] ++;
        }
    }

    if ( nsize>0 )
    {
        for (int j=0; j<nb_facies_; j++)
            current_prop_[j] /= nsize;

        return true;
    }
    else
        return false;
}


/*
 * function to get simulation grid, simulation property name
 */
bool Filtersim_Cate::get_simul_grid( const Parameters_handler* parameters,
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

        //if ( simul_grid_->classname() == "Masked_grid" )
        //{
        //    error_mesgs->report( "GridSelector_Sim", "simulation grid cannot be a Masked Grid" );
        //    return false;
        //}
        
        multireal_property_ =
            simul_grid_->add_multi_realization_property( property_name_prefix_ );
    }
    
    // the simulation grid cannot be 0D or 1D
    int nx = simul_grid_->nx();	
    int ny = simul_grid_->ny();	
    int nz = simul_grid_->nz();	

    if ( (nx*ny == 1) || (ny*nz == 1) || (nz*nx == 1) )
    {
        error_mesgs->report( "GridSelector_Sim", "Simulation grid cannot be in 1D" );
        return false;
    }
    
    return true;
}


/*
 * function to get training image grid, training image property
 */
bool Filtersim_Cate::get_training_image( const Parameters_handler* parameters,
		    Error_messages_handler* error_mesgs )
{
	training_image_name_ = parameters->value( "PropertySelector_Training.grid" );
	error_mesgs->report( training_image_name_.empty(), 
		        "PropertySelector_Training", "No training image selected" );
	
	training_property_name_ = parameters->value( "PropertySelector_Training.property" );
    if ( training_property_name_.empty() )
    {
        error_mesgs->report( "PropertySelector_Training", "No training property selected" );
        return false;
    }

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

        // forbid training image grid to be same as simulation grid
        if ( training_image_==simul_grid_ )
        {
			error_mesgs->report( "PropertySelector_Training", "Training image grid must be different from the simulation grid" );
			return false;
        }
		
		training_image_->select_property( training_property_name_ );

        // check the indicator coding
        if ( is_categorical_==1 )
        {
            int nb_cat;
            if ( !is_indicator_prop(training_image_->selected_property(), nb_cat) )
            {
                error_mesgs->report( "PropertySelector_Training", "Training image must be indicators and start from 0" );
                return false;
            }
            
            if ( nb_cat != nb_facies_ )
            {
                error_mesgs->report( "PropertySelector_Training", "Total # of facies is inconsistent betwen TI and Marginal" );
                return false;
            }
        }

        // training image cannot be in 1D
        int nx = training_image_->nx();	
        int ny = training_image_->ny();	
        int nz = training_image_->nz();	
        
        if ( (nx*ny == 1) || (nx*nz == 1) || (ny*nz == 1) )
        {
            error_mesgs->report( "PropertySelector_Training", "Training image cannot be in 1D grid" );
            return false;
        }
        
        return get_marginal_cdf();

        //return true;
    }
    else 
		return false;
}


/*
 * function to get hard data
 */
bool Filtersim_Cate::get_hard_data( const Parameters_handler* parameters,
		                        Error_messages_handler* error_mesgs )
{
    std::string harddata_grid_name = parameters->value( "Hard_Data.grid" );
    
    if( !harddata_grid_name.empty() ) 
    {
        // Get the harddata grid from the grid manager
        bool ok = geostat_utils::create( harddata_grid_, harddata_grid_name, 
                                        "Hard_Data", error_mesgs );
        if( !ok )     return false;

        if ( harddata_grid_->classname() != "Point_set" )
        {
            error_mesgs->report( "Hard_Data", "hard data must be given in Point Set" );
            return false;
        }

        std::string hdata_prop_name = parameters->value( "Hard_Data.property" );
        if ( hdata_prop_name.empty() )
        {
            error_mesgs->report( "Hard_Data", "No property name specified" );
            return false;
        }
        
        // create hard data property copy
        harddata_property_ = harddata_grid_->property( hdata_prop_name );
        if( harddata_property_ ) 
        {
            property_copier_ = 
                    Property_copier_factory::get_copier( harddata_grid_, simul_grid_ );

            if( !property_copier_ ) 
            {
                std::ostringstream message;
                message << "It is currently not possible to copy a property from a "
                        << harddata_grid_->classname() << " to a " 
                        << simul_grid_->classname() ;
                error_mesgs->report( !property_copier_, "Hard_Data", message.str() );
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


/*
 * function to get soft data, and tau updating values
 */
bool Filtersim_Cate::get_soft_data( const Parameters_handler* parameters,
                              Error_messages_handler* error_mesgs )
{
    use_soft_field_ = String_Op::to_number<int>( parameters->value( "Use_SoftField.value" ) );

    //  std::string probfield_grid_name;
    if( use_soft_field_==1 )
    {
        // get soft property
        softdata_property_names_ = parameters->value( "SoftData_properties.value" );

        if ( softdata_property_names_.empty() )
        {
            error_mesgs->report( "SoftData_properties", "No prob. field property selected" );
            return false;
        }
        
        vector< string > prop_names =
                String_Op::decompose_string( softdata_property_names_, ";", false );

        // check the property size
        if (prop_names.size() != nb_facies_)
        {
            error_mesgs->report( "SoftData_properties", "Supply one and only one probability field per facies" );
            return false;
        }
        
        for( vector< string >::const_iterator it = prop_names.begin();
                    it != prop_names.end(); ++it ) 
        {
            GsTLGridProperty* prop = simul_grid_->property( *it );
            if( !prop ) 
            {
                error_mesgs->report( "SoftData_properties", "The given soft does not exist" );
                return false;
            }
            softDataProps_.push_back( prop );
        }

        // get Tau parameters
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
            
            /*  // use only one Tau value for soft data
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
    }
    
    return true;
}


/*
 * function to get region data
 */
bool Filtersim_Cate::get_region_data( const Parameters_handler* parameters,
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

        // check the property type: categroical or continuous
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


/*
 * function to get search template size
 */
bool Filtersim_Cate::get_TI_scan_template(const Parameters_handler* parameters, 
                                     Error_messages_handler* error_mesgs)
{
	std::string scan_template_str = parameters->value( "Scan_Template.value" );

    if ( scan_template_str.empty() )
    {
        error_mesgs->report( "Scan_Template", "No scan template size specified" );
        return false;
    }

	std::istringstream scan_template_stream( scan_template_str );
	int sizev;
	std::vector<int> template_window;
	while( scan_template_stream >> sizev ) 
		template_window.push_back( sizev );
	
    if ( template_window.size() < 3 )
    {
        error_mesgs->report( "Scan_Template", "Too few values provided" );
        return false;
    }
	
    if ( template_window.size() > 3 )
    {
        error_mesgs->report( "Scan_Template", "Too many values provided" );
        return false;
    }

    nxdt_ = template_window[0];
	nydt_ = template_window[1];
    nzdt_ = template_window[2];

    nxyzdt_ = nxdt_*nydt_*nzdt_;

    if ( nxyzdt_%2 == 0)
    {
        error_mesgs->report( "Scan_Template", "Template size must be odd" );
        return false;
    }

    // check the tempate size
    int nx = std::min( training_image_->nx(), simul_grid_->nx() );
    int ny = std::min( training_image_->ny(), simul_grid_->ny() );
    int nz = std::min( training_image_->nz(), simul_grid_->nz() );

    if( nx==1 ) nxdt_ = 1;	
	if( ny==1 ) nydt_ = 1;	
	if( nz==1 ) nzdt_ = 1;	

    error_mesgs->report( nxdt_<1 || nxdt_>nx, 
                "Scan_Template","X search size must be >=1 and <=nx" );
    error_mesgs->report( nydt_<1 || nydt_>ny, 
                "Scan_Template","Y search size must be >=1 and <=ny" );
    error_mesgs->report( nzdt_<1 || nzdt_>nz, 
                "Scan_Template","Z search size must be >=1 and <=nz" );
	
    if ( nxdt_ != 1 && nydt_ != 1 && nzdt_ != 1 )
        nfilter_ = 9;
    else
        nfilter_ = 6;

	if( !error_mesgs->empty() )
		return false;
	else
		return true;
}


/*
 * function to get patch template size
 */
bool Filtersim_Cate::get_patch_template(const Parameters_handler* parameters, 
                                   Error_messages_handler* error_mesgs)
{
	std::vector<int> template_window;
	
	std::string patch_template_str = parameters->value( "Patch_Template_ADVANCED.value" );
    if ( patch_template_str.empty() )
    {
        error_mesgs->report( "Patch_Template_ADVANCED", "No patch template size specified" );
        return false;
    }
	
	int sizev;
	std::istringstream patch_template_stream( patch_template_str );
	while( patch_template_stream >> sizev ) 
		template_window.push_back( sizev );
	
    if ( template_window.size() < 3 )
    {
        error_mesgs->report( "Patch_Template_ADVANCED", "Too few values provided" );
        return false;
    }
	
    if ( template_window.size() > 3 )
    {
        error_mesgs->report( "Patch_Template_ADVANCED", "Too many values provided" );
        return false;
    }
	
	patch_nxdt_ = template_window[0];
	patch_nydt_ = template_window[1];
    patch_nzdt_ = template_window[2];

    patch_nxyzdt_ = patch_nxdt_*patch_nydt_*patch_nzdt_;

    if ( patch_nxyzdt_%2 == 0)
    {
        error_mesgs->report( "Patch_Template_ADVANCED", "Template size must be odd" );
        return false;
    }

    // check the template size
	if( nxdt_==1 ) patch_nxdt_ = 1;	
	if( nydt_==1 ) patch_nydt_ = 1;	
	if( nzdt_==1 ) patch_nzdt_ = 1;	
	
	error_mesgs->report(patch_nxdt_<1 || patch_nxdt_>nxdt_, 
			    "Patch_Template_ADVANCED","X patch size must be >=1 and <=nxdt");
	error_mesgs->report(patch_nydt_<1 || patch_nydt_>nydt_, 
			    "Patch_Template_ADVANCED","Y patch size must be >=1 and <=nydt");
    error_mesgs->report(patch_nzdt_<1 || patch_nzdt_>nzdt_, 
                "Patch_Template_ADVANCED","Z patch size must be >=1 and <=nzdt");
	
	if( !error_mesgs->empty() ) return false;

    return true;
}


/*
 * function to get number of bins/clusters for first and second partition
 */
void Filtersim_Cate::get_nb_bins(const Parameters_handler* parameters)
{
    is_cross_partition_ = String_Op::to_number<int>(parameters->value("CrossPartition.value"));

    if ( is_cross_partition_== 1 )  // for cross partition
    {
        nb_bins_ = String_Op::to_number<int>( parameters->value( "Nb_Bins_ADVANCED.value" ) );
        nb_bins_2nd_ = String_Op::to_number<int>( parameters->value( "Nb_Bins_ADVANCED2.value" ) );
    }
    else    // for k-mean cluster partition
    {
        nb_bins_ = String_Op::to_number<int>( parameters->value( "Nb_Clusters_ADVANCED.value" ) );
        nb_bins_2nd_ = String_Op::to_number<int>( parameters->value( "Nb_Clusters_ADVANCED2.value" ) );
    }
}


/*
 * function to get cmins which is critera for splitting
 */
bool Filtersim_Cate::get_cmin(const Parameters_handler* parameters, Error_messages_handler* error_mesgs)
{
	std::string cmin_replicates_str = parameters->value( "Cmin_Replicates.value" );
	
    if ( cmin_replicates_str.empty() )
    {
        error_mesgs->report( "Cmin_Replicates", "No cmin replicates specified" );
        return false;
    }
	
	std::istringstream cmin_replicates_stream( cmin_replicates_str );
	int cmin;
	while( cmin_replicates_stream >> cmin ) 
		cmin_replicates_.push_back( cmin );

    if ( cmin_replicates_.size() < nb_multigrids_ )
    {
        error_mesgs->report( "Cmin_Replicates", "Too few values provided" );
        return false;
    }

    // check the input data size
    if ( cmin_replicates_.size() > nb_multigrids_ )
    {
        vector< int >::iterator iter=cmin_replicates_.begin();
        for (int i=cmin_replicates_.size()-1; i>nb_multigrids_-1; i--)
            cmin_replicates_.erase( iter+i );
    }

    for (int i=0; i<cmin_replicates_.size(); i++)
    {
        if ( cmin_replicates_[i]<1 )
        {
            error_mesgs->report( "Cmin_Replicates", "Minimum # of replicates can NOT be less than 1" );
             return false;
        }
    }

    //reverse(cmin_replicates_.begin(), cmin_replicates_.end());
    return true;
}


/*
 * function to get data weight assign to different data type
 */
bool Filtersim_Cate::get_data_weight(const Parameters_handler* parameters, Error_messages_handler* error_mesgs)
{
	// get the weights given to three types of data
	std::string data_weights_str = parameters->value( "Data_Weights.value" );
    if ( data_weights_str.empty() )
    {
        error_mesgs->report( "Data_Weights", "No weights specified" );
        return false;
    }

	std::istringstream data_weights_stream( data_weights_str );
	float weight;

    while( data_weights_stream >> weight ) 
		    data_weight_.push_back( weight );

    if ( data_weight_.size() < 3 )
    {
        error_mesgs->report( "Data_Weights", "Too few values provided" );
        return false;
    }
    if ( data_weight_.size() > 3 )
    { 
        error_mesgs->report( "Data_Weights", "Too many values provided" );
        return false;
    }

	float cumul = std::accumulate( data_weight_.begin(), data_weight_.end(), (float) 0.0 );

    if ( fabs(cumul-1.0) > 0.0000001 )
    {
        error_mesgs->report( "Data_Weights", "Values must sum up to 1" );
        return false;
    }
	
    if ( data_weight_[1] < 0.0000001 )
    {
        error_mesgs->report( "Data_Weights", "Weight for patched data must > 0" );
        return false;
    }

    if ( data_weight_[1] < data_weight_[2]  )
    {
        error_mesgs->report( "Data_Weights", "Weight for patched data must > weight for other data" );
        return false;
    }

    return true;
}


/*
 * function to calculate the maximally allowed multi-grids # according to search template
 */
bool Filtersim_Cate::calculate_max_mgrid( Error_messages_handler* error_mesgs )
{
    // minimum half size of the training image and the simulation grid
    /*
    int half_nx = ( min( training_image_->nx(), simul_grid_->nx() ) - 1 )/2;
    int half_ny = ( min( training_image_->ny(), simul_grid_->ny() ) - 1 )/2;
    int half_nz = ( min( training_image_->nz(), simul_grid_->nz() ) - 1 )/2;
    */

    int half_nx = ( training_image_->nx() - 1 )/2;
    int half_ny = ( training_image_->ny() - 1 )/2;
    int half_nz = ( training_image_->nz() - 1 )/2;

    // half size of the searching template
	int half_nxdt = (nxdt_-1)/2;  
	int half_nydt = (nydt_-1)/2;  
    int half_nzdt = (nzdt_-1)/2;

    // maximally allowed jump step
    int x_step = (half_nxdt!=0) ? (half_nx/half_nxdt) : 99999;
    int y_step = (half_nydt!=0) ? (half_ny/half_nydt) : 99999;
    int z_step = (half_nzdt!=0) ? (half_nz/half_nzdt) : 99999;

    int nb_allowed; // test the maximally allowed # of multigrids
    for (nb_allowed=1; nb_allowed<=nb_multigrids_; nb_allowed++)
    {
        // jumping step for current multigrid
        int step = pow( 2.0, nb_allowed-1 );

        if ( step>x_step || step>y_step || step>z_step )
            break;
    }

    nb_allowed --;

    if ( nb_multigrids_ > nb_allowed )
    {
        GsTLcout<<"\nWarning: # of multi-grids is too large and is reset as " << nb_allowed << " !"  << gstlIO::end;
        nb_multigrids_ = nb_allowed;
    }

    if ( use_score_dist_ && nb_multigrids_ <= 1)
    {
        error_mesgs->report( "Scan_Template", "Search template is too large to use at least 2 multiple grids" );
        error_mesgs->report( "Nb_Multigrids_ADVANCED", "# of multiple grids must be at least 2 for using score-based distance function" );
        return false;
    }

    if ( nb_multigrids_ < 1 )
    {
        error_mesgs->report( "Scan_Template", "Input search template is too large (larger than the grid size)" );
        return false;
    }

    return true;
}


/*
 * function to get filter settings
 */
bool Filtersim_Cate::get_filters(const Parameters_handler* parameters, 
                                Error_messages_handler* error_mesgs)
{
    // get filter type
    use_default_filter_ = String_Op::to_number<int>(parameters->value( "Filter_Default.value" ));

    if ( use_default_filter_==1 )
        create_default_filter();
    else
    {   // use user defined filter
        if ( !create_user_define_filter(parameters, error_mesgs) )
            return false;
    }

    // generate score names if user want to view them in the training image grid
    if ( is_viewscore_==1 )
    {
        for(int i=0; i<nfilter_; i++) 
        {
            int total_nb_variables = ( treat_cate_as_cont_==1 ? 1 : nb_facies_ ); 

            for(int j=0; j<total_nb_variables; j++) 
            {
                string fullname= my_filters_->get_filter_name(i)+"_"+String_Op::to_string(j);
                scoreProps_.push_back(geostat_utils::add_property_to_grid(training_image_,fullname));
            }
        }
    }
    
    return true;
}


/*
 * function to create default filter
 */
void Filtersim_Cate::create_default_filter()
{
    // half template size
	int half_nxdt = (nxdt_-1)/2;  
	int half_nydt = (nydt_-1)/2;  
    int half_nzdt = (nzdt_-1)/2;

    my_filters_ = new Filters_default(half_nxdt, half_nydt, half_nzdt, nfilter_);
}


/*
 * function to create user define filter
 */
bool Filtersim_Cate::create_user_define_filter(const Parameters_handler* parameters, 
                                          Error_messages_handler* error_mesgs)
{
    std::string filter_file_name = parameters->value( "User_Def_Filter_File.value" );

    if( filter_file_name.empty() ) 
    {
        error_mesgs->report( "User_Def_Filter_File", "No data file specified" );
        return false;
    }
    
    ifstream infile( filter_file_name.c_str() );
    if( !infile ) 
    {
        error_mesgs->report( "User_Def_Filter_File", "Cannot open the file contains filter defintion" );
        return false;
    }

    // half filter template size
	int half_nxdt = (nxdt_-1)/2;  
	int half_nydt = (nydt_-1)/2;  
    int half_nzdt = (nzdt_-1)/2;

    if ( !valid_filter_file( half_nxdt, half_nydt, half_nzdt, filter_file_name, error_mesgs ) )
    {
        //error_mesgs->report( "User_Def_Filter_File", "Not a valid filter file" );
        return false;
    }
    
    // create filters
    my_filters_ = new Filters_user_define( half_nxdt, half_nydt, half_nzdt, filter_file_name );

    nfilter_ = my_filters_->get_total_filter_number();

    return true;
}


/*
 * function to validate the input filter file
 */
bool Filtersim_Cate::valid_filter_file( int nxdt, int nydt, int nzdt, string filter_file_name,
                                                       Error_messages_handler* error_mesgs )
{   // nxdt, nydt, nzdt specify the half size of the searching template
    ifstream infile( filter_file_name.c_str() );

    string line;
    int line_id=1;

    // number of filters
    getline(infile,line); 

    if ( !is_number( String_Op::simplify_white_space(line) ) )
    {
        std::ostringstream error_stream;
        error_stream <<  filter_file_name <<  " line " << line_id << " is not # of filters";
        error_mesgs->report( "User_Def_Filter_File", error_stream.str() );
        return false;
    }

    int total_nfilter = String_Op::to_number<int>(line);

    int cur_filter=-1;       // the cur_filter^{th} filter
    int total_weight_num=99; // the total_weight_num^{th} weight for current filter

    while( getline(infile,line) )
    {
        line_id++;

        string cur_str;
        vector< string > str; 
        istringstream scan_stream( String_Op::simplify_white_space(line) );

        while( scan_stream >> cur_str ) 
            str.push_back( cur_str );
        
        // filter name and its weight
        if ( str.size() == 2 )
        {
            cur_filter++;
            
            std::ostringstream error_stream;

            if ( cur_filter >= total_nfilter)   
                error_stream <<  filter_file_name <<  " line " << line_id << " starts too many # of filters";
            if ( total_weight_num == 0 )    
                error_stream <<  filter_file_name <<  " line " << line_id << " must contain filter definitions";   // no weight specified
            if ( !is_number( str[1] ) )   
                error_stream <<  filter_file_name <<  " line " << line_id << " has no score weight";

            if ( error_stream.str().size()>0 )    
            {
                error_mesgs->report( "User_Def_Filter_File", error_stream.str() );
                return false;
            }
            
            total_weight_num = 0;
        }
        else if ( str.size() == 4 ) // filter offset and its loading weight
        {
            for (int i=0; i<4; i++)
            {
                if ( !is_number( str[i] ) )    return false;
            }
            
            // the node must be inside searching template
            if ( abs( String_Op::to_number<int>(str[0]) )<=nxdt &&
                        abs( String_Op::to_number<int>(str[1]) )<=nydt &&
                        abs( String_Op::to_number<int>(str[2]) )<=nzdt ) 
                total_weight_num++;
        }
        else
        {
            std::ostringstream error_stream;
            error_stream <<  filter_file_name <<  " line " << line_id << " is wrong for filter defintions";
            error_mesgs->report( "User_Def_Filter_File", error_stream.str() );
            return false;
        }
    }   // end while( getline(infile,line) )

    if ( cur_filter != total_nfilter-1)   
    {
        error_mesgs->report( "User_Def_Filter_File", "# of filters in data file is too few" );
        return false;   // too few filters
    }
    else    
        return true;
}


/*
 * function to print some of the input parameters
 */
void Filtersim_Cate::print_parameters()
{
	std::cout << "\n---Parameters read: " << std::endl;
	std::cout << "Simul grid: " << simul_grid_name_ << std::endl
		<< "Property prefix: " << property_name_prefix_ << std::endl
		<< "Training image: " << training_image_name_ << std::endl
		<< "Train. property: " << training_property_name_ << std::endl
		<< "Nb of real: " << nb_reals_ << std::endl
		<< "Seed: " << seed_ << std::endl
		<< "Nb facies: " << nb_facies_ << std::endl
		<< "Multigrids: " << nb_multigrids_ << std::endl
		<< "Template size: " << nxdt_ << " " << nydt_ << " " << nzdt_ << std::endl 
		<< "Patch size: " << patch_nxdt_ << " " << patch_nydt_ << " " << patch_nzdt_ << std::endl
		<< "Data weights: " << " " << data_weight_[0] <<" "<< data_weight_[1] << " " 
					<< data_weight_[2] << std::endl
		<< "Servosystem: " << serv_ << std::endl
        << "Partition method: ";
    if ( is_cross_partition_ == 1 )
        std::cout << "Cross Partition (" << nb_bins_ << ", " << nb_bins_2nd_ << ")" << std::endl;
    else
        std::cout << "Partition with K-mean (" << nb_bins_ << ", " << nb_bins_2nd_ << ")" << std::endl;
}


// --  start utility functions for execution --

/*
 * function to calculate total steps and frequency
 */
void Filtersim_Cate::pre_progress_notifer(int& total_steps, int& frequency)
{
	float slow_factor=0;

	for( int ncoarse = nb_multigrids_ ; ncoarse >= 1 ; ncoarse--)
	{
		int one_step = pow( 2.0, ncoarse-1 );

		if( (nxdt_ == 1) || (nydt_ == 1) || (nzdt_ == 1) )  // 2D case
		  slow_factor += 1.0/pow( (float)one_step, 2 );
		else              // 3D case
		  slow_factor += 1.0/pow( (float)one_step, 3 );
	}

    if ( use_score_dist_ )
        slow_factor += 1.0;

    total_steps = floor(simul_grid_->size() * slow_factor * nb_reals_/patch_nxyzdt_*2) ;

    frequency = std::max( total_steps / 50, 1 );
}


/*
 * function to initialize random path
 */
void Filtersim_Cate::init_random_path() 
{
    const SGrid_cursor* sg_cursor = simul_grid_->cursor() ;
    grid_paths_.clear();

    if (use_region_ == 1)   // use regions
    {
        simul_grid_->select_property( region_property_->name() );
        for( int i=0; i < sg_cursor->max_index(); i++ )
        {
            int region_value = (simul_grid_->geovalue(sg_cursor->node_id(i))).property_value();

            if ( find(active_region_.begin(), active_region_.end(), region_value) != active_region_.end() )    
                grid_paths_.push_back(i);
        }
    }
    else    // without regions
    {
        for( int i=0; i < sg_cursor->max_index(); i++ )
            grid_paths_.push_back(i);
    }

    STL_generator gen;
    std::random_shuffle( grid_paths_.begin(), grid_paths_.end(), gen );
}


/*
 * function to copy the previously simulated data to 
 * current simulation property
 */
void Filtersim_Cate::copy_pre_simulation_data()
{
    GsTLGridProperty* prop = simul_grid_->selected_property();
    
    //for (int i=0; i<simul_grid_->nxyz(); i++) 
    for (int i=0; i<simul_grid_->size(); i++) 
    {
        if ( region_simulated_->is_informed(i) )
        {
            prop->set_value(region_simulated_->get_value(i), i);

            //TODO
            // should mark the pre-simulated data as hard data??
            prop->set_harddata(true, i);
        }
    }
}


/*
 * function to set the simulation region
 */
void Filtersim_Cate::set_region_indicator()
{
    nb_region_node_ = 0;

    //region_.resize(simul_grid_->nxyz(), false);
    region_.resize(simul_grid_->size(), false);

    //for (int i=0; i<simul_grid_->nxyz(); i++) 
    for (int i=0; i<simul_grid_->size(); i++) 
    {
        int region_value = region_property_->get_value(i);
        
        if ( find(active_region_.begin(), active_region_.end(), region_value) != active_region_.end() )   
        {
            region_[i] = true;
            nb_region_node_ ++;   // record the # of nodes in current region(s)
        }
    }
}


/*
 * function to remove the simulation property when
 * this program abort
 */
void Filtersim_Cate::clean( GsTLGridProperty* prop ) 
{
	if( prop ) 
		simul_grid_->remove_property( prop->name() );
}



/*
 * function to set the filter score weight
 */
void Filtersim_Cate::get_filter_score_weight()
{
    if ( treat_cate_as_cont_ == 1 )
        filter_weight_ = my_filters_->get_filter_weights();
    else
    {
        vector<float> weight = my_filters_->get_filter_weights();
        
        for (int i=0; i<nfilter_; i++)  {
            for (int j=0; j<nb_facies_-1; j++)   {
                filter_weight_.push_back( weight[i]/(nb_facies_-1) );
            }
        }
    }
}


/*
 * main function to perform simulation for one realization
 */
bool Filtersim_Cate::simulate_one_realization( SmartPtr<Progress_notifier>& progress_notifier, 
                                             GsTLGridProperty* prop, int nreal )
{
    simul_grid_->select_property( prop->name() );
    simul_grid_->set_level(1);	

    if( property_copier_ ) 
        property_copier_->copy( harddata_grid_, harddata_property_, simul_grid_, prop );

    // create servosystem
    Filtersim_Servosystem_Cate< Random_number_generator >* categorical_sampler =
                        new Filtersim_Servosystem_Cate< Random_number_generator >( 
                                                    target_cpdf_, serv_, 
                                                    simul_grid_->begin(), simul_grid_->end(),
                                                    Random_number_generator(), patch_nxyzdt_ );

    if( property_copier_ ) 
        property_copier_->undo_copy();

    // set up region information
    if ( use_region_ == 1 )
    {
        set_region_indicator();

        if ( nb_region_node_==0 )    return 0;

        if ( use_pre_region_ == 1)
            copy_pre_simulation_data();
    }
    else
        region_.resize(simul_grid_->size(), true);
        //region_.resize(simul_grid_->nxyz(), true);

    int proto_loc;    // record the working prototype location in the prototype list
    int start_score_based_dist = -1;    // the starting coarse grid to be filled in using dual template
    int finest_grid_nb=1;     // mark the finest multiple grid nb: 1 for pixel-based distance; 0 for score-based distance  

    if ( use_score_dist_ )    
    {
        start_score_based_dist = 1;

        // *************************************************** //
        // un-comment this line to run simulation twice on the finest grid //
        // *************************************************** //
        //finest_grid_nb = 0;  
    }

    bool first_run=true;

    // will be removed later
    int loop_count=0;
    int total_run = 10;

    // loop on all coarse grids
    for( int ncoarse = nb_multigrids_ ; ncoarse >= finest_grid_nb ; ncoarse-- ) 
    {
        GsTLcout << "Working on realization " << nreal << ", coarse grid " << ncoarse << gstlIO::end;

        if ( use_score_dist_ && ncoarse==0 )   
        {  // one additional simulation run on the finest grid
            ncoarse=1;
            first_run = false;
            loop_count ++;
            //patch_nxdt_ = 1;
            //patch_nydt_ = 1;
            //patch_nzdt_ = 1;
        }

        proto_loc =  nb_multigrids_-ncoarse;

        progress_notifier->message() << "\n working on realization " << nreal
                                                     << "\n coarse grid " << ncoarse << gstlIO::end;

        if( !progress_notifier->notify() ) 
        {
            // the execution was aborted. We could not finish the simulation of the 
            // current property, so delete the current property
            appli_warning( "deleting property " << prop->name() );
            clean( prop );
            return 1;
        }

        // initialize random path
        simul_grid_->set_level( ncoarse );
        init_random_path();
        simul_grid_->select_property( prop->name() );

        if( property_copier_ ) 
            property_copier_->copy( harddata_grid_, harddata_property_, simul_grid_, prop );

        // Because the scores/prototypes are calculated 
        // in the finest grid of the training image,
        // the level of training image should always be 1
        training_image_->set_level( 1 );

        ScoresType cur_score;

        GsTLcout << "Creating filter scores ..."  << gstlIO::end;

        const std::clock_t timer_start = std::clock();

        create_filter_scores( training_image_, cur_score, 
                                        my_filters_, ncoarse, is_viewscore_,
                                        treat_cate_as_cont_, nb_facies_, 
                                        training_property_name_, scoreProps_, nreal,
                                        max_value_, min_value_ ); 

        if ( cur_score.size() == 0 )     continue;          // the TI is too small, hence no score is calculated

        const std::clock_t timer_end = std::clock();
        const float run_time = ( timer_end - timer_start ) / CLOCKS_PER_SEC;

        GsTLcout << "Creating filter scores ... done!  CPU time: " << run_time << " seconds " << gstlIO::end;

        // set up search template
        Window_neighborhood* neighbors = simul_grid_->window_neighborhood( *window_geom_ );
        neighbors->select_property( prop->name() );

        // search neighborhood in TI
        Grid_template* TI_window_geom = my_filters_->get_window_geometry( ncoarse );
        Window_neighborhood* TI_neighbors = training_image_->window_neighborhood( *TI_window_geom );
        TI_neighbors->select_property( training_property_name_ );

        // patch neighborhood
        Grid_template* patch_window_geom = my_filters_->get_window_geometry( ncoarse, 
                                                                                                                            (int)((patch_nxdt_-1)/2), 
                                                                                                                            (int)((patch_nydt_-1)/2), 
                                                                                                                            (int)((patch_nzdt_-1)/2) );

        Window_neighborhood* patch_neighbors = training_image_->window_neighborhood( *patch_window_geom );
        patch_neighbors->select_property( training_property_name_ );

        simul_grid_->select_property( prop->name() );

        //*
        RGrid::random_path_iterator path_begin;
        RGrid::random_path_iterator path_end;

        path_begin = RGrid::random_path_iterator( simul_grid_,simul_grid_->select_property( prop->name() ),
                                        0, int(grid_paths_.size()), TabularMapIndex(&grid_paths_) );
        path_end = RGrid::random_path_iterator( simul_grid_,simul_grid_->select_property( prop->name() ),
                                        int(grid_paths_.size()), int(grid_paths_.size()), TabularMapIndex(&grid_paths_) );
        //*/

        /* // alternative random path definitions
        RGrid::random_path_iterator  
        path_begin( simul_grid_,simul_grid_->select_property( prop->name() ),
        0, int(grid_paths_.size()), TabularMapIndex(&grid_paths_) );

        RGrid::random_path_iterator  
        path_end( simul_grid_,simul_grid_->select_property( prop->name() ),
        int(grid_paths_.size()), int(grid_paths_.size()), TabularMapIndex(&grid_paths_) );
        */

        // set up patch help to freeze the inner part of the pasted patterns
        Patch_Helper* patcher; 
        if ( use_region_==1 )
            patcher = new Patch_Helper( simul_grid_, region_, harddata_property_!=0 );
        else
            patcher = new Patch_Helper( simul_grid_, harddata_property_!=0 );

        int status;        // return value of the running status

        // same pattern paster for all categorical simulation situations
        Pattern_Paster_Categorical pattern_paster( simul_grid_, neighbors, training_image_, TI_neighbors, 
                                                                         patcher, patch_nxdt_, patch_nydt_, patch_nzdt_ );

        if ( is_cross_partition_ == 1 ) // use cross partition
        {
            PrototypeListType1 proto_list1_;
            create_prototypelist( proto_list1_, nreal, ncoarse, nb_facies_, TI_neighbors, patch_neighbors, 
                                                                    cur_score, cmin_replicates_[ncoarse-1]);

            appli_message("Simulating patterns ... ");

            if ( ncoarse>start_score_based_dist )   // use pixel-based distance calculation
            {
                // same hard data event finder w/o soft data
                HD_DEV_Finder_Categorical hd_dev_finder( simul_grid_, neighbors, patcher, nb_facies_, data_weight_ );

                if ( use_soft_field_ )
                {
                    Window_neighborhood* st_neighbors = simul_grid_->window_neighborhood( *window_geom_ );

                    ST_DEV_Finder_Categorical st_dev_finder( simul_grid_, st_neighbors, softDataProps_, nb_facies_, data_weight_ );

                    Filtersim_Tau_updating updater(target_cpdf_, tau1_, tau2_, UNINFORMED);
                    //Filtersim_Nu_updating updater(target_cpdf_, nu_, UNINFORMED);

                    status = sequential_patch_simulation_categorical_2(
                                                            path_begin, 
                                                            path_end,
                                                            proto_list1_,        
                                                            *categorical_sampler, 
                                                            updater,
                                                            hd_dev_finder,  
                                                            st_dev_finder,
                                                            patcher,
                                                            pattern_paster,   
                                                            pattern_mapping_,
                                                            progress_notifier.raw_ptr() );
                }
                else    // no soft data available
                {
                    status = sequential_patch_simulation_categorical(
                                                            path_begin, 
                                                            path_end,
                                                            proto_list1_,        
                                                            *categorical_sampler, 
                                                            hd_dev_finder,  
                                                            patcher,
                                                            pattern_paster,   
                                                            pattern_mapping_,
                                                            progress_notifier.raw_ptr() );
                }
            }
            else   // use score-based distance calculation
            {
                DEV_Finder_Categorical_Score_based  hd_dev_score_finder( simul_grid_, neighbors, my_filters_,  
                                                        pattern_mapping_, nb_facies_, max_value_, min_value_, treat_cate_as_cont_ );

                status = sequential_patch_simulation_categorical_score_based(
                                                        path_begin, 
                                                        path_end,
                                                        proto_list1_,     
                                                        *categorical_sampler, 
                                                        hd_dev_score_finder,  
                                                        patcher,
                                                        pattern_paster,   
                                                        pattern_mapping_,
                                                        progress_notifier.raw_ptr() );
            }
        }
        else    // partition with k-mean
        {
            PrototypeListType2 proto_list2_;
            create_prototypelist( proto_list2_, nreal, ncoarse, nb_facies_, TI_neighbors, patch_neighbors, 
                                                                    cur_score, cmin_replicates_[ncoarse-1]);

            appli_message("Simulating patterns ... ");

            if ( ncoarse>start_score_based_dist )   // use pixel-based distance calculation
            {
                HD_DEV_Finder_Categorical hd_dev_finder( simul_grid_, neighbors, patcher, nb_facies_, data_weight_ );

                if ( use_soft_field_ )
                {
                    Window_neighborhood* st_neighbors = simul_grid_->window_neighborhood( *window_geom_ );

                    ST_DEV_Finder_Categorical st_dev_finder( simul_grid_, st_neighbors, softDataProps_, nb_facies_, data_weight_ );

                    Filtersim_Tau_updating updater(target_cpdf_, tau1_, tau2_, UNINFORMED);
                    //Filtersim_Nu_updating updater(target_cpdf_, nu_, UNINFORMED);

                    status = sequential_patch_simulation_categorical_2(
                                                            path_begin, 
                                                            path_end,
                                                            proto_list2_,     
                                                            *categorical_sampler, 
                                                            updater,
                                                            hd_dev_finder,  
                                                            st_dev_finder,
                                                            patcher,
                                                            pattern_paster,   
                                                            pattern_mapping_,
                                                            progress_notifier.raw_ptr() );
                }
                else    // no soft data available
                {
                    status = sequential_patch_simulation_categorical(
                                                            path_begin, 
                                                            path_end,
                                                            proto_list2_,       
                                                            *categorical_sampler, 
                                                            hd_dev_finder,  
                                                            patcher,
                                                            pattern_paster,   
                                                            pattern_mapping_,
                                                            progress_notifier.raw_ptr() );
                }
            }
            else   // use score-based distance calculation
            {
                DEV_Finder_Categorical_Score_based  hd_dev_score_finder( simul_grid_, neighbors, my_filters_,  
                                                 pattern_mapping_, nb_facies_, max_value_, min_value_, treat_cate_as_cont_ );

                status = sequential_patch_simulation_categorical_score_based(
                                                        path_begin, 
                                                        path_end,
                                                        proto_list2_, 
                                                        *categorical_sampler, 
                                                        hd_dev_score_finder,  
                                                        patcher,
                                                        pattern_paster,   
                                                        pattern_mapping_,
                                                        progress_notifier.raw_ptr() );
            }
        }

        // view all intermediate simulations, except dual results
        //if ( is_view_intermediate_ && loop_count<total_run )
        if ( is_view_intermediate_ && first_run )
        {
            if (  ncoarse>1 )
                record_intermediate_sim( prop, ncoarse );
            else if ( use_score_dist_ )  // score-based distance calculations, for 1st run
            {
                /*
                ostringstream ostr; 
                ostr << loop_count;
                string IM_name = "_FR_"+ostr.str();
                record_intermediate_sim( prop, ncoarse, IM_name );
                */
                //record_intermediate_sim( prop, ncoarse,  "_FR" );
            }
        }

        // add dual pattern paster here
        if ( use_score_dist_  )
        {
            /*  TODO in future
            if ( is_categorical_ && use_soft_field_ )
            update_simulation_with_soft();
            */

            // processing the pattern mapping data
            if ( ncoarse == 2 )
            {                
                pattern_mapping_.set_TI_neighborhood( TI_neighbors );
                fillin_with_dual_template( prop, pattern_mapping_, ncoarse );
                pattern_mapping_.record_node_location();

                if ( is_view_intermediate_ )    // record the result pasted with dual template
                    record_intermediate_sim( prop, ncoarse, "_dual" );
            }
            else if ( ncoarse == 1 && !first_run )
            {
                pattern_mapping_.remove_previous_recording();
                pattern_mapping_.record_node_location();
            }
        }        

        if ( transcon_data_==1 && ncoarse ==  2 )
        {
            GsTLcout << "Post-processing simulation on the penultimate coarse grid " << gstlIO::end;
            post_processing_realization( prop );

            if ( is_view_intermediate_ )    // record the result pasted with dual template
            {
                if ( use_score_dist_ )
                    record_intermediate_sim( prop, ncoarse, "_dual_tr" );
                else
                    record_intermediate_sim( prop, ncoarse, "_tr" );
            }
        }

        // remove pointer data
        delete patcher;
        delete TI_window_geom;
        delete patch_window_geom;

        // clean the property if snesim is aborted
        if( status == -1 ) 
        {
            clean( prop );
            return 1;
        }

        // undo hard data relocation
        if( property_copier_ && ncoarse != 1 )
            property_copier_->undo_copy();

        // reset ncoarse value to stop simulation
        //if ( !first_run && ncoarse==1 && loop_count==total_run )     ncoarse=-1;
        if ( !first_run && ncoarse==1 )     ncoarse=-1;

        appli_message("Finish simulation on grid ..." << ncoarse << std::endl );
    }	// end of multi-grid

    // clean servosystem sample when current realization is finished.
    if ( categorical_sampler != 0 ) delete categorical_sampler; 

    return 0;
}


/*
 * function to record the intermediate result in the corresponding coarse grid
 */
void Filtersim_Cate::record_intermediate_sim( GsTLGridProperty* prop, int ncoarse, string dual )
{
    // initialize the property name to be recorded
    string  previous_prop;
    ostringstream ostr; 
    ostr << ncoarse;

    previous_prop = prop->name() + "_mg_" + ostr.str();

    if ( dual.length()>0 )
        previous_prop  += dual;

    // select the property to be saved, 
    // if the property does not exit, add the property to current object
    GsTLGridProperty* prop2 = simul_grid_->select_property( previous_prop );
    if ( !prop2 )   prop2 = geostat_utils::add_property_to_grid(simul_grid_,previous_prop);

    // record the data property
    //for (int ik=0; ik<simul_grid_->nxyz(); ik++) 
    for (int ik=0; ik<simul_grid_->size(); ik++) 
    {
        if ( prop->is_informed(ik) && region_[ik] )
            prop2->set_value(prop->get_value(ik), ik);
    }

    simul_grid_->select_property( prop->name() );
}


/*
 * function to paste the 2nd to finest grid with dual template
 */
void Filtersim_Cate::fillin_with_dual_template( GsTLGridProperty* prop,
                                                                      Pattern_Node_Map& mapping, int ncoarse)
{
    simul_grid_->set_level(1);
    GsTLcout << "Fill-in the 2nd multiple grid with dual-template !" << gstlIO::end;

    // set up a dual search template geometry
    Grid_template* dual_window_geom = my_filters_->get_dual_window_geometry( ncoarse,
                                                                                                                               (int)((nxdt_-1)/2), 
                                                                                                                               (int)((nydt_-1)/2),  
                                                                                                                               (int)((nzdt_-1)/2) );

    // set up a dual search template
    Window_neighborhood* dual_neighbors = simul_grid_->window_neighborhood( *dual_window_geom );
    dual_neighbors->select_property( prop->name() );

    Window_neighborhood* dual_TI_neighbors = training_image_->window_neighborhood( *dual_window_geom );
    dual_TI_neighbors->select_property( training_property_name_ );

    // the patcher is used to mimic the simulation process to freeze the inner part of the training patterns
    Patch_Helper* dual_patcher; 

    if ( use_region_==1 )
        dual_patcher = new Patch_Helper( simul_grid_, region_, harddata_property_!=0 );
    else
        dual_patcher = new Patch_Helper( simul_grid_, harddata_property_!=0 );

    // set up a dual pattern paster to control the actual dual template pasting
    Pattern_Paster dual_pattern_paster(simul_grid_, dual_neighbors,  training_image_, dual_TI_neighbors, 
                                                 dual_patcher, 2*patch_nxdt_-1, 2*patch_nydt_-1, 2*patch_nzdt_-1 );

    // paste the patterns exactly along the random path with the dual_paster
    for (int i=0; i<mapping.total_mapping_pair(); i++)
        dual_pattern_paster( mapping.get_node_id(i), mapping.get_pattern_id(i) );

    delete dual_window_geom;
}


/*
 * template function to create prototype list, the prototype type
 * is given by class CPrototype
 */
template <class CPrototype > 
void 
Filtersim_Cate::create_prototypelist( CPrototype& proto_list,
                                                     int nreal, int ncoarse, int nb_facies,
                                                     Window_neighborhood* TI_neighbors,
                                                     Window_neighborhood* patch_neighbors, 
                                                     ScoresType& cur_score, 
                                                     int cmin_replicates_ )
{
    appli_message("Creating prototype list ... ");

    //GsTLcout << "Creating prototype list ..."  << gstlIO::end;
    proto_list = CPrototype( training_image_, TI_neighbors, patch_neighbors, cur_score, 
                                            filter_weight_,  nb_facies, cmin_replicates_, nb_bins_, nb_bins_2nd_ );

    // create the whole pattern prototype list
    proto_list.create_prototype_list();

    // record the prototype indicator if needed
    if ( is_view_indicator_==1 && nreal==1 && ncoarse==1 )
        proto_list.create_prototype_indicator();
}


void Filtersim_Cate::get_target_pdf_factor( vector<float>& current_prop_factor )
{
    float omega=15.0;
    float val;

    for ( int i=0; i<nb_facies_; i++)
    {
        if ( current_prop_[i] < 0.0001 )    current_prop_[i]  = 0.0001;

        val = std::abs(target_cpdf_[i]-current_prop_[i])/std::max(target_cpdf_[i],current_prop_[i])*omega + 1.0;
        current_prop_factor.push_back( std::pow( target_cpdf_[i]/current_prop_[i], val) );
    }
}


void Filtersim_Cate::post_processing_realization( GsTLGridProperty*prop )
{
    if ( !get_current_cdf( prop ) )    return;

    vector<float> current_prop_factor;
    get_target_pdf_factor( current_prop_factor );
    
    // set up conditioning property
    int locl_cond_factor = 10;
    string cond_prop_name = prop->name() + "_cond";
    GsTLGridProperty* cond_prop = geostat_utils::add_property_to_grid(simul_grid_,cond_prop_name );
    cond_prop_name = cond_prop->name();

    simul_grid_->set_level(1);

    if( property_copier_ ) 
        property_copier_->copy( harddata_grid_, harddata_property_, simul_grid_, cond_prop );

    // create working property
    pp_tmp_prop_ = prop->name() + "bak";
    GsTLGridProperty* working_prop = geostat_utils::add_property_to_grid(simul_grid_,pp_tmp_prop_);
    pp_tmp_prop_ = working_prop->name();

    //for (int i=0; i<simul_grid_->nxyz(); i++) 
    for (int i=0; i<simul_grid_->size(); i++) 
        working_prop->set_value( prop->get_value(i), i );

    // create filter
	int half_w_nx; 
	int half_w_ny;
    int half_w_nz; 

    if (use_score_dist_)
    {
        half_w_nx = (5-1)/2;  
        half_w_ny = (5-1)/2;  
        half_w_nz = (3-1)/2;
    }
    else
    {
        half_w_nx = (9-1)/2;  
        half_w_ny = (9-1)/2;  
        half_w_nz = (5-1)/2;
    }

    if (simul_grid_->nx() == 1)        half_w_nx = 0;
    if (simul_grid_->ny() == 1)        half_w_ny = 0;
    if (simul_grid_->nz() == 1)        half_w_nz = 0;

    Filter* my_filter = new Transcat_Filter_default(half_w_nx, half_w_ny, half_w_nz, 1);
    Grid_template* window_geom = my_filter->get_window_geometry();
    vector<float> filter_weight = my_filter->get_weights(0);

    //simul_grid_->select_property( pp_tmp_prop_ );
    SmartPtr<Neighborhood> neighborhood = simul_grid_->window_neighborhood( *window_geom );
    neighborhood->select_property( pp_tmp_prop_ );

    int loc = 0;
    // moving average over each grid node
    for( Geostat_grid::iterator node_iter = simul_grid_->begin(); 
                node_iter != simul_grid_->end();  node_iter++, loc++ )  
    {
        if ( region_[loc] && node_iter->is_informed() && (!cond_prop->is_informed(loc)) )   
        {  // if not a hard data and in the current working region, then do following
            neighborhood->find_neighbors( *node_iter );
            vector<float>::const_iterator weight_iter = filter_weight.begin();
            vector<float> nb_prop(nb_facies_, 0.0);

            // get the category proportion within the moving window
            for( Neighborhood::iterator neighbor_iter = neighborhood->begin();
                neighbor_iter != neighborhood->end() ; neighbor_iter++, weight_iter++)	
            {
                // current neighborhood node is valid
                int nb_node_id = neighbor_iter->node_id();
                //if ( (nb_node_id > -1) && (nb_node_id < nb_of_nodes) )
                if ( neighbor_iter->is_informed() )
                {
                    if ( cond_prop->is_informed(nb_node_id) )
                        nb_prop[ neighbor_iter->property_value() ] += (*weight_iter)*locl_cond_factor;
                    else
                        nb_prop[ neighbor_iter->property_value() ] += *weight_iter;
                }
            }

            // calculate the new category value
            for (int i=0; i<nb_facies_; i++)
                nb_prop[i] *= current_prop_factor[i];

            prop->set_value( std::max_element(nb_prop.begin(), nb_prop.end())-nb_prop.begin(), loc );
        }
    }

    delete my_filter;
    delete window_geom;
    clean(cond_prop);
    clean(working_prop);
}
