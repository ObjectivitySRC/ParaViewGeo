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

#include "ImageProcess.h"

ImageProcess::ImageProcess()
{
    source_grid_ = 0;
    source_property_ = 0;
    target_grid_ = 0;
    target_property_ = 0;
}


bool ImageProcess::initialize( const Parameters_handler* parameters,
							   Error_messages_handler* error_mesgs ) 
{
    action_type_ =0;   // no action as initial

    if ( !get_Source_Data(parameters, error_mesgs) )
        return false;

    if ( !get_Target_Data(parameters, error_mesgs) )
        return false;

    if ( !get_Rotation_Data(parameters, error_mesgs) )
        return false;

    if ( !get_Scaling_Data(parameters, error_mesgs) )
        return false;

	if( !error_mesgs->empty() )
		return false;

    // initialize the TI centers
    source_center_.resize(3);
    source_center_[0] = ( source_grid_->nx() -1. )/2.0;
    source_center_[1] = ( source_grid_->ny() -1. )/2.0;
    source_center_[2] = ( source_grid_->nz() -1. )/2.0;

    target_center_.resize(3);
    target_center_[0] = ( target_grid_->nx() -1. )/2.0;
    target_center_[1] = ( target_grid_->ny() -1. )/2.0;
    target_center_[2] = ( target_grid_->nz() -1. )/2.0;

    target_property_ = geostat_utils::add_property_to_grid(target_grid_, target_prop_name_);
	
	return true;
}

int ImageProcess::execute( GsTL_project* proj )
{
    source_grid_->set_level(1);
    target_grid_->set_level(1);

    switch( action_type_ )
    {
    case No_Action:
        perform_no_action();
        return 0;
    case ROT_Only:
        perform_rotation();
        return 0;
    case SCA_Only:
        perform_scaling();
        return 0;
    case FULL_Action:
        perform_full_action();
        return 0;
    default:
        return 1;
    }

    return 0;
}

bool ImageProcess::get_Source_Data(const Parameters_handler* parameters,
							       Error_messages_handler* error_mesgs ) 
{
    string grid_name = parameters->value( "Source_Grid.value" );

    if ( grid_name.empty() )
    {
        error_mesgs->report( "Source_Grid", "No source grid selected" );
        return false;
    }

    // Get the source grid from the grid manager
    source_grid_ = dynamic_cast<RGrid*>( Root::instance()->interface( 
                            gridModels_manager + "/" + grid_name).raw_ptr() );
    
    if( !source_grid_ ) 
    {
        std::ostringstream error_stream;
        error_stream <<  grid_name <<  " is not a valid (Regular) object grid";
        error_mesgs->report( "Source_Grid", error_stream.str() );
        return false;
    }

    //if ( source_grid_->classname() == "Masked_grid" )
    //{
    //    error_mesgs->report( "Source_Grid", "source grid MUST be a Regular Grid" );
    //    return false;
    //}
    
    string prop_name = parameters->value( "Source_Property.value" );

    if ( prop_name.empty() )
    {
        error_mesgs->report( "Source_Property", "No source property specified" );
        return false;
    }
    
    source_property_ = source_grid_->select_property(prop_name);

    return true;
}


bool ImageProcess::get_Target_Data(const Parameters_handler* parameters,
							         Error_messages_handler* error_mesgs ) 
{
    //get target grid
    string grid_name = parameters->value( "Target_Grid.value" );

    if ( grid_name.empty() )
    {
        error_mesgs->report( "Target_Grid", "No rotation grid selected" );
        return false;
    }
    
    // Get the simulation grid from the grid manager
    target_grid_ = dynamic_cast<RGrid*>( Root::instance()->interface( 
                            gridModels_manager + "/" + grid_name).raw_ptr() );
    
    if( !target_grid_ ) 
    {
        std::ostringstream error_stream;
        error_stream <<  grid_name <<  " is not a valid (Regular) object grid";
        error_mesgs->report( "Target_Grid", error_stream.str() );
        return false;
    }

    //if ( target_grid_->classname() == "Masked_grid" )
    //{
    //    error_mesgs->report( "Target_Grid", "source grid MUST be a Regular Grid" );
    //    return false;
    //}
    
    // get target property name
    target_prop_name_ = parameters->value( "Target_Property.value" );

    return true;
}


bool ImageProcess::get_Rotation_Data(const Parameters_handler* parameters,
							         Error_messages_handler* error_mesgs ) 
{
    // get rotation angles
    angles_.resize(3, 0.0);

    string angle_str0 = parameters->value( "Angle0.value" );
    if ( angle_str0.empty() )
    {
        error_mesgs->report( "Angle0", "No rotation angle specified" );
        return false;
    }
    else
        angles_[0] = String_Op::to_number<float>( angle_str0 );
    
    string angle_str1 = parameters->value( "Angle1.value" );
    if ( angle_str1.empty() )
    {
        error_mesgs->report( "Angle1", "No rotation angle specified" );
        return false;
    }
    else
        angles_[1] = String_Op::to_number<float>( angle_str1 );

    string angle_str2 = parameters->value( "Angle2.value" );
    if ( angle_str2.empty() )
    {
        error_mesgs->report( "Angle2", "No rotation angle specified" );
        return false;
    }
    else
        angles_[2] = String_Op::to_number<float>( angle_str2 );

    // set action type
    if ( !GsTL::equals( angles_[0], float(0.0), float(0.00001) ) ||
         !GsTL::equals( angles_[1], float(0.0), float(0.00001) ) ||
         !GsTL::equals( angles_[2], float(0.0), float(0.00001) ) )
        action_type_ = ROT_Only;

    // validate rotation angles 
    float factor = -3.14159265/180;

    for ( int i=0; i<3; i++ )
    {
        while(angles_[i]>180)    angles_[i] -= 360;
        while(angles_[i]<-180)    angles_[i] += 360;

        angles_[i] *= factor;
    }

    angles_[0] = 0-angles_[0];

    return true;
}


bool ImageProcess::get_Scaling_Data(const Parameters_handler* parameters,
							        Error_messages_handler* error_mesgs ) 
{
    // get scaling factor
    factors_.resize(3, 0.0);

    string factor_str0 = parameters->value( "Factor0.value" );
    if ( factor_str0.empty() )
    {
        error_mesgs->report( "Factor0", "No rotation angle specified" );
        return false;
    }
    else
        factors_[0] = String_Op::to_number<float>( factor_str0 );
    
    string factor_str1 = parameters->value( "Factor1.value" );
    if ( factor_str1.empty() )
    {
        error_mesgs->report( "Factor1", "No rotation angle specified" );
        return false;
    }
    else
        factors_[1] = String_Op::to_number<float>( factor_str1 );

    string factor_str2 = parameters->value( "Factor2.value" );
    if ( factor_str2.empty() )
    {
        error_mesgs->report( "Factor2", "No rotation angle specified" );
        return false;
    }
    else
        factors_[2] = String_Op::to_number<float>( factor_str2 );

    // set action type
    if ( !GsTL::equals( factors_[0], float(1.0), float(0.00001) ) ||
         !GsTL::equals( factors_[1], float(1.0), float(0.00001) ) ||
         !GsTL::equals( factors_[2], float(1.0), float(0.00001) ) )
    {
        if ( action_type_ == No_Action ) 
            action_type_ = SCA_Only;
        else
            action_type_ = FULL_Action;
    }

    // change affinity convention: the larger the factor, the larger the pattern size
    for (int i=0; i<3; i++)
    {
        if ( factors_[i] < 0.00001 )
        {
            error_mesgs->report( "Affinity", "Affinity factor must be > 0" );
            return false;
        }
        factors_[i] = 1.0/factors_[i];
    }

    return true;
}


void ImageProcess::perform_no_action()
{
    // simply copy the source property to target property
    for( int i=0; i<source_grid_->nxyz(); i++ )
        target_property_->set_value( source_property_->get_value(i), i);
}


void ImageProcess::perform_rotation()
{
    // perform rotation only
    Rotation rotator(  target_center_, source_center_, angles_ );

    SGrid_cursor* s_cursor = source_grid_->cursor();
    SGrid_cursor* t_cursor = target_grid_->cursor();

    //int nxyz = source_grid_->nxyz();
    int nxyz = source_grid_->size();

    int si, sj, sk;     // source
    int ti, tj, tk;     // target
    int node_id;

    //for( int i=0; i<target_grid_->nxyz(); i++ )
    for( int i=0; i<target_grid_->size(); i++ )
    {
        t_cursor->coords( i, ti, tj, tk ); 
        rotator(ti, tj, tk, si, sj, sk);

        node_id = s_cursor->node_id(si, sj, sk);
        if ( node_id>-1 && node_id<nxyz )
            target_property_->set_value( source_property_->get_value(node_id), i);
    }
}


void ImageProcess::perform_scaling()
{
    // perform scaling only
    Scaling scaler( target_center_, source_center_, factors_ );

    SGrid_cursor* s_cursor = source_grid_->cursor();
    SGrid_cursor* t_cursor = target_grid_->cursor();

    //int nxyz = source_grid_->nxyz();
    int nxyz = source_grid_->size();

    int si, sj, sk;     // source
    int ti, tj, tk;     // target
    int node_id;

    //for( int i=0; i<target_grid_->nxyz(); i++ )
    for( int i=0; i<target_grid_->size(); i++ )
    {
        t_cursor->coords( i, ti, tj, tk ); 
        scaler(ti, tj, tk, si, sj, sk);

        node_id = s_cursor->node_id(si, sj, sk);
        if ( node_id>-1 && node_id<nxyz )
            target_property_->set_value( source_property_->get_value(node_id), i);
    }
}


void ImageProcess::perform_full_action()
{
    // perform rotation and affinity
    //SRotation  modifier( target_center_, source_center_, angles_, factors_ );
    RScaling  modifier( target_center_, source_center_, angles_, factors_ );

    SGrid_cursor* s_cursor = source_grid_->cursor();
    SGrid_cursor* t_cursor = target_grid_->cursor();

    //int nxyz = source_grid_->nxyz();
    int nxyz = source_grid_->size();

    int si, sj, sk;     // source
    int ti, tj, tk;     // target
    int node_id;

    //for( int i=0; i<target_grid_->nxyz(); i++ )
    for( int i=0; i<target_grid_->size(); i++ )
    {
        t_cursor->coords( i, ti, tj, tk ); 
        modifier(ti, tj, tk, si, sj, sk);

        node_id = s_cursor->node_id(si, sj, sk);
        if ( node_id>-1 && node_id<nxyz )
            target_property_->set_value( source_property_->get_value(node_id), i);
    }
}


//GEOSTAT_PLUGIN(ImageProcess);
