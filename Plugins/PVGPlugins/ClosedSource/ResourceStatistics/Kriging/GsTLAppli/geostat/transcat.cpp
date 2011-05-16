/**********************************************************************
** Author: Jianbing Wu
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


#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/gstl_plugins.h>
#include <GsTLAppli/geostat/utilities.h>
#include <GsTLAppli/appli/utilities.h>
#include <GsTLAppli/grid/grid_model/grid_initializer.h>
#include <GsTLAppli/grid/grid_model/sgrid_cursor.h>
#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTL/math/math_functions.h>

#include <cmath>

#include "transcat.h"


transcat::transcat()
{
    source_grid_ = 0;
    source_property_ = 0;
    harddata_grid_ = 0;
    harddata_property_ = 0;
    my_filter_ = 0;
    window_geom_ = 0;
}

transcat::~transcat()
{
    if ( window_geom_ )
        delete window_geom_;

    delete my_filter_;
}


bool transcat::initialize( const Parameters_handler* parameters,
							   Error_messages_handler* error_mesgs ) 
{
    nb_facies_ =  String_Op::to_number<int>( parameters->value( "Nb_Categories.value" ) );
    nb_iterations_ =  String_Op::to_number<int>( parameters->value( "Nb_Iterations.value" ) );

    if ( !get_source_data(parameters, error_mesgs) )
        return false;

    if ( !get_condition_data(parameters, error_mesgs) )
        return false;

    if ( !get_target_prop(parameters, error_mesgs) )
        return false;

    if ( !get_moving_window(parameters, error_mesgs) )
        return false;

    if ( !get_filters(parameters, error_mesgs) )
        return false;

    window_geom_ = my_filter_->get_window_geometry();
    filter_weight_ = my_filter_->get_weights(0);

	return true;
}


int transcat::execute(  GsTL_project* proj )
{
    // set up conditioning property
    cond_prop_name_ = source_property_name_ + "_cond";
    GsTLGridProperty* cond_prop = geostat_utils::add_property_to_grid(source_grid_,cond_prop_name_);
    cond_prop_name_ = cond_prop->name();

    if( property_copier_ ) 
    {
        source_grid_->set_level(1);
        property_copier_->copy( harddata_grid_, harddata_property_, source_grid_, cond_prop );
    }

    // set up output property
    string output_prop_name = source_property_name_ + output_suffix_;
    GsTLGridProperty* output_prop = geostat_utils::add_property_to_grid(source_grid_,output_prop_name);
    output_prop_name = output_prop->name();

    //for (int ik=0; ik<source_grid_->nxyz(); ik++) 
    for (int ik=0; ik<source_grid_->size(); ik++) 
        output_prop->set_value( source_property_->get_value(ik), ik );

    // set up working property
    string working_prop_name = output_prop_name + "bak";
    GsTLGridProperty* working_prop = geostat_utils::add_property_to_grid(source_grid_,working_prop_name);
    working_prop_name = working_prop->name();

    // initialize a search neighborhood with the moving window template geometry
    SmartPtr<Neighborhood> neighborhood = source_grid_->window_neighborhood( *window_geom_ );

    //int nb_of_nodes = source_grid_->nxyz();
    int nb_of_nodes = source_grid_->size();

    //GsTLcout << "starting processing " << gstlIO::end;

    for (int iter=0; iter<nb_iterations_; iter++)
    {
        // copy working property from the current output property which is initialized by the original property
        for (int ik=0; ik<nb_of_nodes; ik++) 
            working_prop->set_value( output_prop->get_value(ik), ik );

        calculate_category_proportions( working_prop );
        neighborhood->select_property( working_prop_name );

        int loc = 0;
        // moving average over each grid node
        for( Geostat_grid::iterator node_iter = source_grid_->begin(); 
                node_iter != source_grid_->end();  node_iter++, loc++ )  
        {
            //GsTLcout << "      node id:  " << loc;

            if ( node_iter->is_informed() && (!cond_prop->is_informed(loc)) )   // if not a hard data, then do following
            { 
                //GsTLcout << " is valid";
                neighborhood->find_neighbors( *node_iter );
                vector<float>::const_iterator weight_iter = filter_weight_.begin();
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
                            nb_prop[ neighbor_iter->property_value() ] += (*weight_iter)*locl_cond_factor_;
                        else
                            nb_prop[ neighbor_iter->property_value() ] += *weight_iter;
                    }
               }

                // calculate the new category value
                for (int i=0; i<nb_facies_; i++)
                    nb_prop[i] *= current_prop_factor_[i];

                output_prop->set_value( std::max_element(nb_prop.begin(), nb_prop.end())-nb_prop.begin(), loc );
            }
            //else
                //GsTLcout << " is note valid";

            //GsTLcout << gstlIO::end;
        }
    }

    // clean unnecessary properties
    source_grid_->remove_property( working_prop_name );
    source_grid_->remove_property( cond_prop_name_ );

    return 0;
}


void transcat::calculate_category_proportions( GsTLGridProperty* prop )
{
    current_prop_.clear();
    current_prop_factor_.clear();
    vector<int> icount(nb_facies_, 0);

    int nsize=0;

    for ( int i=0; i<prop->size(); i++ )
    {
        if ( prop->is_informed(i) )
        {
            nsize ++;
            icount[ (int)(prop->get_value(i)) ] ++;
        }
    }

    //float omega=target_pdf_factor_[0];
    for ( int i=0; i<nb_facies_; i++)
    {
        float val = (float)icount[i]/nsize;
        if ( val < 0.0001 )    val = 0.0001;
        current_prop_.push_back( val );
        //current_prop_factor_.push_back( target_pdf_[i]*target_pdf_factor_[i]/val );
        current_prop_factor_.push_back( std::pow( target_pdf_[i]/val, target_pdf_factor_[i]) );

        //val = std::abs(target_pdf_[i]-current_prop_[i])/std::max(target_pdf_[i],current_prop_[i])*omega + 1.0;
        //current_prop_factor_.push_back( std::pow( target_pdf_[i]/current_prop_[i], val) );
    }
}

bool transcat::get_source_data( const Parameters_handler* parameters,
							                     Error_messages_handler* error_mesgs ) 
{
    std::string source_grid_name = parameters->value( "Working_Grid.grid" );
    
    if( !source_grid_name.empty() ) 
    {
        // initialize the working grid
        source_grid_ = dynamic_cast<RGrid*>(  
                                                Root::instance()->interface( 
			                                    gridModels_manager + "/" + source_grid_name).raw_ptr() );

        if( !source_grid_ ) 
        {
			std::ostringstream error_stream;
			error_stream <<  source_grid_name <<  " is not a valid working grid";
			error_mesgs->report( "Working_Grid", error_stream.str() );
            return false;
        }

        // get the current working property
        source_property_name_ = parameters->value( "Working_Grid.property" );
        if ( source_property_name_.empty() )
        {
            error_mesgs->report( "Working_Grid", "No property name specified" );
            return false;
        }
        source_property_ = source_grid_->select_property( source_property_name_ );

        // check the consistency of the categories #
        int nb_cat;
        if ( !is_indicator_prop(source_property_, nb_cat) )
        {
            error_mesgs->report( "Working_Grid", "Working property must be indicators and start from 0" );
            return false;
        }

        if ( nb_cat != nb_facies_ )
        {
            error_mesgs->report( "Working_Grid", "Total # of categories is inconsistent with the input # of categories " );
            return false;
        }

        // get the output suffix name
        output_suffix_ = parameters->value( "Out_Suffix.value" );
        if ( output_suffix_.empty() )
        {
            error_mesgs->report( "Out_Suffix", "No output suffix name specified" );
            return false;
        }

        return true;
    }

    return false;
}


bool transcat::get_condition_data( const Parameters_handler* parameters,
		                                             Error_messages_handler* error_mesgs )
{
    use_local_condition_ =  String_Op::to_number<int>( parameters->value( "Is_Cond.value" ) );

    if ( use_local_condition_== 1) 
    {
        std::string harddata_grid_name = parameters->value( "Cond_Data.grid" );
        if( !harddata_grid_name.empty() ) 
        {
            // Get the harddata grid from the grid manager
            bool ok = geostat_utils::create( harddata_grid_, harddata_grid_name, "Cond_Data", error_mesgs );
            if( !ok ) 
            {
                error_mesgs->report( "Cond_Data", "Not a valid conditioning data grid" );
                return false;
            }

            std::string hdata_prop_name = parameters->value( "Cond_Data.property" );
            if ( hdata_prop_name.empty() )
            {
                error_mesgs->report( "Cond_Data", "No property name specified" );
                return false;
            }

            // create hard data property copy
            harddata_property_ = harddata_grid_->property( hdata_prop_name );
            if( harddata_property_ ) 
            {
                property_copier_ = Property_copier_factory::get_copier( harddata_grid_, source_grid_ );

                if( !property_copier_ ) 
                {
                    std::ostringstream message;
                    message << "It is currently not possible to copy a property from a "
                                    << harddata_grid_->classname() << " to a " 
                                    << source_grid_->classname() ;
                    error_mesgs->report( !property_copier_, "Cond_Data", message.str() );
                    return false;
                }
            }
            else
            {
                std::ostringstream error_stream;
                error_stream <<  harddata_grid_name 
                                    <<  " does not have a property called " 
                                    << hdata_prop_name;
                error_mesgs->report( "Cond_Data", error_stream.str() );
                return false;
            }

            locl_cond_factor_ = String_Op::to_number<float>( parameters->value( "Weight_Factor.value" ) );
        }
    }

    return true;
}

bool transcat::get_target_prop( const Parameters_handler* parameters,
		                                        Error_messages_handler* error_mesgs )
{
    // get target marginal proportions
    std::string marginal_pdf_str = parameters->value( "Marginal_Pdf.value" );
	std::istringstream marginal_pdf_stream( marginal_pdf_str );

	float prob;
	while( marginal_pdf_stream >> prob )
		target_pdf_.push_back( prob );
	
    // check pdf size
    if ( target_pdf_.size() <  nb_facies_ )
    {
        error_mesgs->report( "Marginal_Pdf", "Too few values provided" );
        return false;
    }
    
    if ( target_pdf_.size() >  nb_facies_ )
    {
        error_mesgs->report( "Marginal_Pdf", "Too many values provided" );
        return false;
    }
    
    float cumul = std::accumulate( target_pdf_.begin(), target_pdf_.end(), 0.0f );
    if ( !GsTL::equals( cumul, float(1.0), float(0.00001) ) ) 
    {
        error_mesgs->report( "Marginal_Pdf", "Values must sum up to 1" );
        return false;
    }

    // get target marginal proportion factors
    std::string marginal_pdf_factor_str = parameters->value( "Marginal_Pdf_Factor.value" );
	std::istringstream marginal_pdf_factor_stream( marginal_pdf_factor_str );

	float factor;
	while( marginal_pdf_factor_stream >> factor )
		target_pdf_factor_.push_back( factor );
	
    // check size
    if ( target_pdf_factor_.size() <  nb_facies_ )
    {
        error_mesgs->report( "Marginal_Pdf_Factor", "Too few values provided" );
        return false;
    }
    
    if ( target_pdf_factor_.size() >  nb_facies_ )
    {
        error_mesgs->report( "Marginal_Pdf_Factor", "Too many values provided" );
        return false;
    }

    return true;
}


bool transcat::get_moving_window( const Parameters_handler* parameters, 
                                                        Error_messages_handler* error_mesgs )
{
	std::string scan_template_str = parameters->value( "Moving_Window.value" );

    if ( scan_template_str.empty() )
    {
        error_mesgs->report( "Moving_Window", "No scan template size specified" );
        return false;
    }

	std::istringstream scan_template_stream( scan_template_str );
	int sizev;
	std::vector<int> template_window;
	while( scan_template_stream >> sizev ) 
		template_window.push_back( sizev );
	
    if ( template_window.size() < 3 )
    {
        error_mesgs->report( "Moving_Window", "Too few values provided" );
        return false;
    }
	
    if ( template_window.size() > 3 )
    {
        error_mesgs->report( "Moving_Window", "Too many values provided" );
        return false;
    }

    nxdt_ = template_window[0];
	nydt_ = template_window[1];
    nzdt_ = template_window[2];
    nxyzdt_ = nxdt_*nydt_*nzdt_;

    // make sure that the moving window size is odd
    if ( nxyzdt_%2 == 0)
    {
        error_mesgs->report( "Moving_Window", "Template size must be odd" );
        return false;
    }

    // check the tempate size
    int nx = source_grid_->nx();
    int ny = source_grid_->ny();
    int nz = source_grid_->nz();

    if( nx==1 ) nxdt_ = 1;	
	if( ny==1 ) nydt_ = 1;	
	if( nz==1 ) nzdt_ = 1;	

    error_mesgs->report( nxdt_<1 || nxdt_>nx, 
                "Moving_Window","X dimension size must be >=1 and <= grid.nx" );
    error_mesgs->report( nydt_<1 || nydt_>ny, 
                "Moving_Window","Y dimension size must be >=1 and <=grid.ny" );
    error_mesgs->report( nzdt_<1 || nzdt_>nz, 
                "Moving_Window","Z dimension size must be >=1 and <=grid.nz" );
	
	if( !error_mesgs->empty() )
		return false;
	else
		return true;
}


bool transcat::get_filters(const Parameters_handler* parameters, 
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
    
    return true;
}


/*
 * function to create default filter
 */
void transcat::create_default_filter()
{
    // half template size
	int half_nxdt = (nxdt_-1)/2;  
	int half_nydt = (nydt_-1)/2;  
    int half_nzdt = (nzdt_-1)/2;

    my_filter_ = new Transcat_Filter_default(half_nxdt, half_nydt, half_nzdt, 1);
}


/*
 * function to create user define filter
 */
bool transcat::create_user_define_filter( const Parameters_handler* parameters, 
                                                             Error_messages_handler* error_mesgs )
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

    if ( !valid_filter_file( parameters, error_mesgs, half_nxdt, half_nydt, half_nzdt, filter_file_name ) )
        return false;
    
    // create user-defined filter
    my_filter_ = new Filters_user_define( half_nxdt, half_nydt, half_nzdt, filter_file_name );

    return true;
}


/*
 * function to validate the input filter file
 */
bool transcat::valid_filter_file( const Parameters_handler* parameters, 
                                               Error_messages_handler* error_mesgs, 
                                               int nxdt, int nydt, int nzdt, string filter_file_name )
{   // nxdt, nydt, nzdt specify the half size of the searching template
    string line;
    ifstream infile( filter_file_name.c_str() );

    // number of filters
    getline(infile,line); 
    if ( !is_number( String_Op::simplify_white_space(line) ) )
    {
        error_mesgs->report( "User_Def_Filter_File", "line 1 is not the # of filters" );
        return false;
    }

    int total_nfilter = String_Op::to_number<int>(line);

    if ( total_nfilter>1 )
    {
        error_mesgs->report( "User_Def_Filter_File", "Filter data file can only have ONE filter defintion" );
        return false;
    }

    // get the filter name 
   getline(infile,line);

   int total_weight_num=0;

    while( getline(infile,line) )
    {
        string cur_str;
        vector< string > str; 
        istringstream scan_stream( String_Op::simplify_white_space(line) );

        while( scan_stream >> cur_str ) 
            str.push_back( cur_str );
        
        if ( str.size() == 4 ) // filter offset and its loading weight
        {
            for (int i=0; i<4; i++)
            {
                if ( !is_number( str[i] ) )    
                {
                    error_mesgs->report( "User_Def_Filter_File", "Filter data file contains non-digital letters" );
                    return false;
                }
            }
            
            // the node must be inside searching template
            if ( abs( String_Op::to_number<int>(str[0]) )<=nxdt &&
                        abs( String_Op::to_number<int>(str[1]) )<=nydt &&
                        abs( String_Op::to_number<int>(str[2]) )<=nzdt ) 
                total_weight_num++;
        }
        else
        {
            error_mesgs->report( "User_Def_Filter_File", "Filter data does not contain four columns" );
            return false;
        }
    }   // end while( getline(infile,line) )

     if ( total_weight_num == 0 )    // no weight specified  
     {
         error_mesgs->report( "User_Def_Filter_File", "No filter data within the moving window" );
        return false; 
     }
    else    
        return true;
}


//GEOSTAT_PLUGIN(transcat);
