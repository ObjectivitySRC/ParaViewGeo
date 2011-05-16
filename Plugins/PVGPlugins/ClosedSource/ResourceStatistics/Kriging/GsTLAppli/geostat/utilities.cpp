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
**********************************************************************/

#include <GsTLAppli/geostat/utilities.h>
#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/utils/gstl_messages.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/appli/utilities.h>
#include <GsTLAppli/math/angle_convention.h>

#include <GsTL/geometry/covariance.h>
#include <GsTL/geometry/geometry_algorithms.h>
#include <GsTL/kriging/LMC_covariance.h>
#include <GsTL/kriging/MM1_covariance.h>
#include <GsTL/kriging/MM2_covariance.h>

#include <GsTLAppli/grid/grid_model/geostat_grid.h> 

#include <GsTL/utils/smartptr.h>

namespace geostat_utils {



bool initialize_covariance( Covariance<Location>* cov,
			                      const std::string& tag_name,
			                      const Parameters_handler* parameters,
			                      Error_messages_handler* errors ) {
  
  // nugget effect
  cov->nugget( 
	      String_Op::to_number<float>( parameters->value( tag_name + ".nugget" ) )
	      );

  // number of variogram structures
  int struct_count =
    String_Op::to_number<int>( parameters->value( tag_name + ".structures_count" ) );
  
  if( struct_count == 0 && cov->nugget() == 0 ) {
    std::string tag = String_Op::split_string( tag_name, "." ).first;
    errors->report( tag, "You must at least specify a nugget effect" );
    return false;
  }

  // initialize each structure
  for( int i=1; i <= struct_count; i++ ) {

    std::string struct_name = "structure_" + String_Op::to_string( i );
    std::string struct_type = parameters->value( tag_name + "/" + struct_name + ".type" );
    cov->add_structure( struct_type );

    // set the sill
    double sill = String_Op::to_number<double>( parameters->value( tag_name + "/" + struct_name + ".contribution" ));
    if( sill == 0 ) 
      {
      std::string tag = String_Op::split_string( tag_name, "." ).first;
      errors->report( tag, "You must enter a non-zero contribution for each variogram structure" );
      return false;
      }
    cov->sill( i-1, sill );

    // get the ranges
    std::string str1, str2, str3;
    str1 = parameters->value( tag_name + "/" + struct_name + "/ranges.max" );
    str2 = parameters->value( tag_name + "/" + struct_name + "/ranges.medium" );
    str3 = parameters->value( tag_name + "/" + struct_name + "/ranges.min" );
    if( str1.empty() || str2.empty() || str3.empty() ) {
      std::string tag = String_Op::split_string( tag_name, "." ).first;
      errors->report( tag, "You must enter a valid range for each direction" );
      return false;
    }

    double R1,R2,R3;
    R1 = String_Op::to_number<double>( str1 );
    R2 = String_Op::to_number<double>( str2 );
    R3 = String_Op::to_number<double>( str3 );


    // get the angles
    str1 = parameters->value( tag_name + "/" + struct_name + "/angles.x" );
    str2 = parameters->value( tag_name + "/" + struct_name + "/angles.y" );
    str3 = parameters->value( tag_name + "/" + struct_name + "/angles.z" );
    if( str1.empty() || str2.empty() || str3.empty() ) {
      std::string tag = String_Op::split_string( tag_name, "." ).first;
      errors->report( tag, "You must enter a valid angle for each direction" );
      return false;
    }
    
    double a1,a2,a3;
    a1 = degree_to_radian( String_Op::to_number<double>( str1 ) );
    a2 = degree_to_radian( String_Op::to_number<double>( str2 ) );
    a3 = degree_to_radian( String_Op::to_number<double>( str3 ) );
    convert_to_math_standard_angles_rad( a1, a2, a3 );

    // check that the ranges are in decreasing order:
    if( R1 < R2 || R2 < R3 ) {
      std::string tag = String_Op::split_string( tag_name, "." ).first;
      std::ostringstream message;
      message << "The range in the major direction must be greater than "
              << "the ones in the medium and minor directions";
      errors->report( tag, message.str() );
      return false;
    }

    // set the ranges and angles of the current structure
    cov->set_geometry( i-1, R1,R2,R3, a1,a2,a3 );
  }

  return true;
}


bool initialize( Kriging_type ktype,
                 KrigingCombiner*& Kcombiner,
                 KrigingConstraints*& Kconstraints,
                 KrigTagMap& tags_map,
                 const Parameters_handler* parameters,
                 Error_messages_handler* errors,
                 Geostat_grid* simulation_grid,
                 KrigDefaultsMap defaults ) {

  typedef Kriging_constraints_impl<Neighborhood, Location> KrigingConstraintsImpl;
  typedef Kriging_combiner_impl<WeightIterator, Neighborhood> KCombinerImpl;

  switch( ktype ) {
  case geostat_utils::SK :
    {
      appli_message( "doing SK" );

      double skmean;

      KrigDefaultsMap::const_iterator defaults_it = 
        defaults.find( geostat_utils::SK );
      if( defaults_it != defaults.end() ) {
        skmean = String_Op::to_number<double>( defaults_it->second );
      }
      else {
        // Retrieve the SK mean:
        std::string sk_tag = tags_map[SK];
        std::string skmean_str = parameters->value( sk_tag );
        if( skmean_str.empty() ) {
          errors->report( sk_tag, "No SK mean supplied" );
          return false;
        }

        skmean = String_Op::to_number<double>( skmean_str );
      }

      KrigingConstraintsImpl* constraints = 
        new SKConstraints_impl<Neighborhood, Location>;
      Kconstraints = new KrigingConstraints( constraints );

      KCombinerImpl* comb = 
        new SK_combiner<WeightIterator, Neighborhood>( skmean );
      Kcombiner = new KrigingCombiner( comb );

      delete comb;
      delete constraints;
      return true;
    }

  
  case geostat_utils::OK :
    {
      appli_message( "doing OK " );

      KrigingConstraintsImpl* constraints = 
        new OKConstraints_impl<Neighborhood, Location>;
      Kconstraints = new KrigingConstraints( constraints );

      KCombinerImpl* comb = new KCombinerImpl;
      Kcombiner = new KrigingCombiner( comb );

      delete comb;
      delete constraints;
      return true;
    }


  case geostat_utils::KT :
    {
      appli_message( "doing KT " );

      // Retrieve the trend components and store them into a vector<bool>
      std::vector<bool> flags;
      
      std::string kt_tag = tags_map[ geostat_utils::KT ];
      std::string trend = parameters->value( kt_tag );
      if( trend.empty() ) {
        errors->report( kt_tag, "No trend components supplied" );
        return false;
      }

      // convert the trend string into a vector of bool
      std::istringstream input( trend.c_str() );
      std::copy( std::istream_iterator<bool>( input ), std::istream_iterator<bool>(),
	               std::back_inserter( flags ) );

      std::vector<Trend_functor> trend_components;
      trend_components.push_back( Trend_functor( Trend_functions::id ) );
      if( flags[0] ) trend_components.push_back( Trend_functor( Trend_functions::x ) );
      if( flags[1] ) trend_components.push_back( Trend_functor( Trend_functions::y ) );
      if( flags[2] ) trend_components.push_back( Trend_functor( Trend_functions::z ) );
      if( flags[3] ) trend_components.push_back( Trend_functor( Trend_functions::x2 ) );
      if( flags[4] ) trend_components.push_back( Trend_functor( Trend_functions::y2 ) );
      if( flags[5] ) trend_components.push_back( Trend_functor( Trend_functions::z2 ) );
      if( flags[6] ) trend_components.push_back( Trend_functor( Trend_functions::xy ) );
      if( flags[7] ) trend_components.push_back( Trend_functor( Trend_functions::xz ) );
      if( flags[8] ) trend_components.push_back( Trend_functor( Trend_functions::yz ) );

      appli_warning( "number of components: " << trend_components.size() );
      KrigingConstraintsImpl* constraints = 
        new KTConstraints_impl<Trend_functor,Neighborhood, Location>( trend_components );
      Kconstraints = new KrigingConstraints( constraints );

      KCombinerImpl* comb = new KCombinerImpl;
      Kcombiner = new KrigingCombiner( comb );

      delete comb;
      delete constraints;
      return true;
    }


  case geostat_utils::LVM :
    {
      appli_message( "doing LVM" );
      appli_assert( simulation_grid );

      std::string lvm_tag = tags_map[ geostat_utils::LVM ];
      std::string mean_prop = parameters->value( lvm_tag );
    
      if( simulation_grid->property( mean_prop ) == 0 ) {
        errors->report( String_Op::split_string( lvm_tag, "/" ).first, 
                        "No valid property specified" );
        return false;
      }

      typedef SK_local_mean_combiner<WeightIterator, Neighborhood,
                                     Colocated_neighborhood> LVM_combiner;

      Colocated_neighborhood* coloc_neigh = 
        dynamic_cast<Colocated_neighborhood*>( 
	        simulation_grid->colocated_neighborhood( mean_prop )
	      );

      KCombinerImpl* comb = new LVM_combiner( *coloc_neigh );
      Kcombiner = new KrigingCombiner( comb );
    
      KrigingConstraintsImpl* constraints = 
        new SKConstraints_impl<Neighborhood, Location>;
      Kconstraints = new KrigingConstraints( constraints );
    
      delete comb;
      delete constraints;
      return true;
    }
  
  } //end of switch


  // Why did we get here ????
  appli_warning( "No valid kriging type provided" );
  errors->report( "Kriging_Type", "No valid kriging type provided" );
  return false;
}





bool initialize( CoKrigingCombiner*& Kcombiner,
			    CoKrigingConstraints*& Kconstraints,
			    Kriging_type type,
			    const std::string& tag_name,
			    const Parameters_handler* parameters,
			    Error_messages_handler* errors,
          const std::string& defaults ) {
  switch( type ) {

  case geostat_utils::SK :
    {
      typedef CoSK_combiner< WeightIterator, NeighIterator > CoSKCombiner;
      typedef Co_SKConstraints_impl< NeighIterator, Location > CoSKConstraintsImpl;
      
      // the cokriging combiner first
      std::string means_str;
      if( defaults.empty() )
        means_str = parameters->value( tag_name );
      else
        means_str = defaults;
  
      if( means_str.empty() ) {
        std::string sk_tag = String_Op::split_string( tag_name, "." ).first;
        errors->report( sk_tag, "No SK mean supplied" );
        return false;
      }

      std::vector<double> means = String_Op::to_numbers<double>( means_str );
      CoSKCombiner* tmp_comb = new CoSKCombiner( means );
      
      Kcombiner = new CoKrigingCombiner( tmp_comb );
      delete tmp_comb;
      
      // now the kriging constraints
      CoSKConstraintsImpl* tmp_constr = new CoSKConstraintsImpl;
      Kconstraints = new CoKrigingConstraints( tmp_constr );
      delete tmp_constr;
    
      return true; 
      break;
    }

  case geostat_utils::OK :
    {
      typedef CoKriging_combiner_impl< WeightIterator, NeighIterator > CoOKCombiner;
      typedef Co_OKConstraints_impl< NeighIterator, Location > CoOKConstraintsImpl;
      
      // the cokriging combiner first
      CoOKCombiner* tmp_comb = new CoOKCombiner;
      
      Kcombiner = new CoKrigingCombiner( tmp_comb );
      delete tmp_comb;

      // now the kriging constraints
      CoOKConstraintsImpl* tmp_constr = new CoOKConstraintsImpl;
      Kconstraints = new CoKrigingConstraints( tmp_constr );
      delete tmp_constr;
      return true; 

      break;
    }

  default:
    return false;
  }

  return false;
}
	




CovarianceSet* init_covariance_set( Cokriging_type type, 
                                    const Covariance<Location>& C11,
                                    CokrigTagMap& tags_map,
                                    const Parameters_handler* parameters, 
                                    Error_messages_handler* errors,
                                    CokrigDefaultsMap defaults ) {

  CovarianceSet* cov_set = 0;

  switch( type ) {
    
  case geostat_utils::FULL :                  // full cokriging
    {
      std::string tag_string = tags_map[ geostat_utils::FULL ];
      std::vector<std::string> tags = 
        String_Op::decompose_string( tag_string, " ", false );

      // we need 2 tags: one for C12 and one for C22
      if( tags.size() != 2 ) return 0;

      Covariance<Location> C12;
      geostat_utils::initialize_covariance( &C12, tags[0],
    			                                  parameters, errors );
      
      Covariance<Location> C22;
      geostat_utils::initialize_covariance( &C22, tags[1],
    			                                  parameters, errors );
      
      TNT::Matrix< Covariance<Location> > cov_matrix(2,2);
      cov_matrix(1,1) = C11;
      cov_matrix(2,2) = C22;
      cov_matrix(1,2) = C12;
      cov_matrix(2,1) = C12;
      
      typedef LMC_covariance< Covariance<Location> > LmcCovariance;
      LmcCovariance* tmp_covset = new LmcCovariance( cov_matrix, 2 );
      cov_set = new CovarianceSet( tmp_covset );
      delete tmp_covset;
    
      break;
    }



  case geostat_utils::MM1 :        // colocated cokriging with Markov Model 1
    {
      std::string tag_string = tags_map[ geostat_utils::MM1 ];
      std::vector<std::string> tags = 
        String_Op::decompose_string( tag_string, " ", false );

      // we need 2 tags: one for C12(0) and one for C22(0)
      if( tags.size() != 2 ) return 0;

      TNT::Matrix<double> cov_mat(2,2);

      double C22;
      CokrigDefaultsMap::const_iterator defaults_it =
        defaults.find( geostat_utils::MM1 );
      if( defaults_it != defaults.end() ) 
        C22 = String_Op::to_number<double>( defaults_it->second );
      else
        C22 = String_Op::to_number<double>( parameters->value(tags[1]) );

      cov_mat(1,1) = C11.compute( EuclideanVector( 0, 0, 0 ) );
      cov_mat(2,2) = C22;

      double correl = String_Op::to_number<double>( parameters->value(tags[0]) );
      cov_mat(1,2) = correl * std::sqrt( cov_mat(1,1) * cov_mat( 2,2 ) );

      cov_mat(2,1) = cov_mat(1,2);

      typedef MM1_covariance< Covariance<Location> > Mm1Covariance;
      Mm1Covariance* tmp_mm1 = new Mm1Covariance( C11, cov_mat, 2 );
      cov_set = new CovarianceSet( tmp_mm1 );
      delete tmp_mm1;
 
      break;
    }


  case geostat_utils::MM2 :        // colocated cokriging with Markov Model 2
    {
      std::string tag_string = tags_map[ geostat_utils::MM2 ];
      std::vector<std::string> tags = 
        String_Op::decompose_string( tag_string, " ", false );

      // we need 2 tags: one for C12(0) and one for C22(h)
      if( tags.size() != 2 ) return 0;

      Covariance<Location> C22;
      geostat_utils::initialize_covariance( &C22, tags[1],
    			                                  parameters, errors );
 
      TNT::Matrix<double> cov_mat(2,2);
      cov_mat(1,1) = C11.compute( EuclideanVector( 0, 0, 0 ) );
      cov_mat(2,2) = C22.compute( EuclideanVector( 0, 0, 0 ) );

      double correl = String_Op::to_number<double>( parameters->value(tags[0]) );
      cov_mat(1,2) = correl * std::sqrt(cov_mat(1,1) * cov_mat( 2,2 ));
      
      cov_mat(2,1) = cov_mat(1,2);

      std::vector< Covariance<Location> > cov_vector(2);
      cov_vector[0] = C11;
      cov_vector[1] = C22;
      typedef MM2_covariance< Covariance<Location> > Mm2Covariance;
      Mm2Covariance* tmp_mm2 = 
        new Mm2Covariance( cov_vector.begin(), cov_vector.end(), cov_mat, 2 );
      cov_set = new CovarianceSet( tmp_mm2 );
      delete tmp_mm2;
 
      break;
    }

  } // end switch

  return cov_set;
}


NeighborhoodHandle 
init_secondary_neighborhood( Cokriging_type type, 
                             Geostat_grid* hdata_grid,
                             const GsTLGridProperty* secondary_prop,
                             const Parameters_handler* parameters, 
                             Error_messages_handler* errors,
                             const std::string& max_size_tag,
                             const std::string& ellipsoid_tag,
                             const std::string& C22_tag ) {

  SmartPtr<Neighborhood> sec_neighborhood;

  switch( type ) {
    
  case geostat_utils::FULL :                                // full cokriging
    {
      Covariance<Location> C22;
      geostat_utils::initialize_covariance( &C22, C22_tag,
			                                      parameters, errors );
            
      int max_neigh = 
      	String_Op::to_number<int>(parameters->value( max_size_tag ));
  
      GsTLTriplet ranges;
      GsTLTriplet angles;
      bool extract_ok =
        geostat_utils::extract_ellipsoid_definition( ranges, angles, 
    		  			                                     ellipsoid_tag,
                                                     parameters, errors );
      extract_ok = geostat_utils::is_valid_range_triplet( ranges );
      if( !extract_ok ) return NeighborhoodHandle(0);

      extract_ok = geostat_utils::is_valid_range_triplet( ranges );
      errors->report( !extract_ok,
                      String_Op::split_string( ellipsoid_tag, "." ).first,
                      "Ranges must verify: major range >= " 
                      "medium range >= minor range >= 0" );
      if( !extract_ok ) return NeighborhoodHandle(0);


      sec_neighborhood = hdata_grid->neighborhood( ranges, angles, &C22 );
      sec_neighborhood->select_property( secondary_prop->name() );
      sec_neighborhood->max_size( max_neigh );
      sec_neighborhood->includes_center( true );

      break;
    }


  case geostat_utils::MM1 : case geostat_utils::MM2 :       // MM1 or MM2
    {
      sec_neighborhood = 
      	hdata_grid->colocated_neighborhood( secondary_prop->name() );
      
      break;
    }

  } // end switch
  

  return NeighborhoodHandle(sec_neighborhood);
}







bool create( Geostat_grid*& grid,
			const std::string& grid_name,
			const std::string& tag_name,
			Error_messages_handler* errors ) {

  grid = dynamic_cast<Geostat_grid*>( 
		Root::instance()->interface( 
					    gridModels_manager + "/" + grid_name
					    ).raw_ptr()
		);

  if( !grid ) {
    std::ostringstream error_stream;
    error_stream <<  grid_name <<  " is not a valid grid";
    errors->report( tag_name, error_stream.str() );
    return false;
  }

  return true;
}





bool extract_ellipsoid_definition( float* ranges, float* angles,
					      const std::string& tag_name,
					      const Parameters_handler* parameters,
					      Error_messages_handler* errors ) {
  GsTLTriplet range_triplet;
  GsTLTriplet angle_triplet;

  bool ok = extract_ellipsoid_definition( range_triplet, angle_triplet,
					  tag_name, parameters, errors );
  for( int i=0; i < 3; i++ ) {
    ranges[i] =  range_triplet[i];
    angles[i] =  angle_triplet[i];
  }

  return ok;
}
	



				    
bool extract_ellipsoid_definition( GsTLTriplet& ranges, 
					 GsTLTriplet& angles,
					 const std::string& tag_name,
					 const Parameters_handler* parameters,
					 Error_messages_handler* errors ) {

  std::string params = parameters->value( tag_name );
//  String_Op::replace( params, "\n", " " ); 
  std::vector<double> param_vals = String_Op::to_numbers<double>( params );

  if( param_vals.size() != 6 ) {
    std::string tag = String_Op::split_string( tag_name, "." ).first;
    errors->report( tag, "Specify exactly 3 range values and 3 angles" );
    return false;
  } 

  ranges[0] = param_vals[0];
  ranges[1] = param_vals[1];
  ranges[2] = param_vals[2];

  if( ranges[0] < ranges[1] || ranges[1] < ranges[2] ) {
    std::string tag = String_Op::split_string( tag_name, "." ).first;
    std::ostringstream message;
    message << "The range in the major direction must be greater than "
            << "the ones in the medium and minor directions";
    errors->report( tag, message.str() );
    return false;
  }

  angles[0] = degree_to_radian( param_vals[3] );
  angles[1] = degree_to_radian( param_vals[4] );
  angles[2] = degree_to_radian( param_vals[5] );
  convert_to_math_standard_angles_rad( angles[0], angles[1], angles[2] );

  return true;
}



Cokriging_type cokriging_type( const std::string& tag_name,
			   const Parameters_handler* parameters,
			   Error_messages_handler* ) {
  
  // type of cokriging requested
  std::string cokriging_type = parameters->value( tag_name );


  if( String_Op::contains( cokriging_type, "full", false ) ) 
    return geostat_utils::FULL;

  if( String_Op::contains( cokriging_type, "mm1", false ) || 
      String_Op::contains( cokriging_type, "model 1", false ) ) 
    return geostat_utils::MM1;

  if( String_Op::contains( cokriging_type, "mm2", false ) || 
      String_Op::contains( cokriging_type, "model 2", false ) ) 
    return geostat_utils::MM2;
  
  return geostat_utils::UNDEF;
}




Kriging_type kriging_type( const std::string& tag_name,
			 const Parameters_handler* parameters,
			 Error_messages_handler* errors ) {
  
  // type of kriging requested
  std::string kriging_type = parameters->value( tag_name );


  // Simple kriging 
  if( ( String_Op::contains( kriging_type, "simple", false ) || 
	String_Op::contains( kriging_type, "SK", false ) ) &&
      ( !String_Op::contains( kriging_type, "local", false ) ||
	!String_Op::contains( kriging_type, "vary", false ) ) ) {

    return geostat_utils::SK;
  }

  // Ordinary kriging 
  if( String_Op::contains( kriging_type, "ordinary", false ) ||
      String_Op::contains( kriging_type, "OK", false ) ) {
    return  geostat_utils::OK;
  }

  // Kriging with trend
  if( String_Op::contains( kriging_type, "trend", false ) ||
      String_Op::contains( kriging_type, "KT", false ) ) {
    return  geostat_utils::KT;
  }


  // Kriging with locally varying mean 
  if( String_Op::contains( kriging_type, "local", false ) ||
      String_Op::contains( kriging_type, "LVM", false ) ) {
    return  geostat_utils::LVM;
  }

  errors->report( String_Op::split_string( tag_name, "." ).first,
                  "Unknown kriging type" );
  return geostat_utils::ERROR;
}



GsTLGridProperty* add_property_to_grid( Geostat_grid* grid, 
                                       const std::string& prop_name ) {
  std::ostringstream new_prop_name_stream;
  new_prop_name_stream << prop_name;

  GsTLGridProperty* new_prop =
    grid->add_property( prop_name );

  while( !new_prop ) {
    // if the property already exists, try appending "_0" to the name
    new_prop_name_stream << "_0";
    new_prop = grid->add_property( new_prop_name_stream.str() );
  }

  return new_prop;
}




// Set a non parametric cdf from a NonParamCdfInput widget

bool get_non_param_cdf(NonParametricCdfType& cdf_non_param,
					   const Parameters_handler* parameters, 
             Error_messages_handler* errors, std::string tag_name)
{
		std::vector< float > reference;
		bool is_on_file = ( parameters->value( tag_name+".ref_on_file" ) == "1" );

		if( is_on_file )
		{
			std::string filename = parameters->value( tag_name+".filename" );
			errors->report( filename.empty(), 
				"filename", "No property specified" );
			std::ifstream infile(filename.c_str() );

			if( !infile ) {
	  		errors->report( tag_name,std::string( "Can't open file" ) + filename );
		  	return false;
			}

			// try to read a few entries and make sure they are numbers. Otherwise abort
			std::string buf;
			for( int i=0 ; i < 10 ; i++ ) {
  			infile >> buf;
			  if( !String_Op::is_number( buf ) ) {
				  errors->report( "on_file",
								  "Wrong file format. The file must only contain numbers" );
				  return false;
			  }
			}
			infile.seekg( 0 );

			reference.insert( reference.begin(), 
				std::istream_iterator<float>( infile ),std::istream_iterator<float>() );
			//cdf = new Non_param_cdf<>(std::istream_iterator<float>( infile ),std::istream_iterator<float>() );
			infile.close();
		} 
		else
		{
			std::string ref_grid_str = parameters->value( tag_name+".grid" );
			std::string ref_prop_str = parameters->value( tag_name+".property" );
			errors->report( ref_grid_str.empty()|| ref_prop_str.empty(), "tag_name", 
				"No reference distribution specified" );
			Geostat_grid* ref_grid;
			if( !ref_grid_str.empty() ) {
				bool ok = geostat_utils::create( ref_grid, ref_grid_str,
                                         tag_name, errors );
				if( !ok ) return false;
			}
			else 
				return false;
			ref_grid->select_property( ref_prop_str );
			GsTLGridProperty* ref_prop = ref_grid->property(ref_prop_str);

			reference.reserve( ref_grid->size() );

			for(int i=0; i< ref_grid->size(); i++)
				if( ref_prop->is_informed(i) ) reference.push_back( ref_prop->get_value(i) );
		}

    bool include_max = false;
//    if(parameters->value(tag_name+"/UTI_type.function") == "No extrapolation") 
//      include_max = true;
    
    bool break_ties = parameters->value(tag_name+".break_ties") == "1";
 //   std::string ttt = parameters->value(tag_name+"/break_ties.value");
  //  if(ttt == "1") break_ties = true;

    if(break_ties)
      build_cdf_tie_break(reference.begin(), reference.end(), 
                          cdf_non_param, include_max );
    else
		  build_cdf(reference.begin(), reference.end(), 
                cdf_non_param, include_max );
    
		bool ok = set_cdf_extrapolation_tail( parameters, errors, 
                                          cdf_non_param, tag_name+"/LTI_type",
                                          tag_name+"/UTI_type" );
		return ok;
}


// Set the extrapolation tail from the TailCdfInput widget

bool set_cdf_extrapolation_tail( const Parameters_handler* parameters,
	                               Error_messages_handler* errors, Non_param_cdf<>& nparam_cdf,
	                               const std::string& LTI_str, const std::string& UTI_str )
{
  String_Op::string_pair base_name = String_Op::split_string(LTI_str,"/");
  std::string LTI_param_name = base_name.first;
  base_name = String_Op::split_string(UTI_str,"/");
  std::string UTI_param_name = base_name.first;

	//Set the tail extrapolation functions
	std::string LTI_function_str = parameters->value( LTI_str+".function" );
	std::string UTI_function_str = parameters->value( UTI_str+".function" );

	// Lower tail setting,
  // If the LTI is exponential, do nothing, it is so per default.
	if(LTI_function_str == "Power") {

    double z_min = *(nparam_cdf.z_begin());
  	std::string power_min = parameters->value( LTI_str+".extreme" );
    if( !power_min.empty() ) z_min = String_Op::to_number< double >( power_min );
		//errors->report( power_min.empty() , LTI_param_name, 
		//"A minimum value must be specified for the Power extrapolation function" );
		//double z_min = String_Op::to_number< double >( power_min );
    double prop_min = *(nparam_cdf.z_begin());
    if( prop_min < z_min ) {
      std::ostringstream message;
      message << "The min value must be lesser than " << prop_min;
      errors->report( LTI_param_name, message.str() );
      return false;
    }
	
		std::string power_omega = parameters->value( LTI_str+".omega" );
    if( power_omega.empty() ) {
  		errors->report( LTI_param_name, "An omega value must be specified for "
                                      "the Power extrapolation function" );
      return false;
    }
		double omega = String_Op::to_number< double >( power_omega );

		nparam_cdf.lower_tail_interpolator( Tail_interpolator( new Power_LTI(z_min,omega) ) );
	}
  else if(LTI_function_str == "No extrapolation") {
    nparam_cdf.lower_tail_interpolator( Tail_interpolator( new No_TI() ) );
  }

	// Set up the upper tail

  if(UTI_function_str == "No extrapolation") {
    nparam_cdf.upper_tail_interpolator( Tail_interpolator( new No_TI() ) );
  }
  else 
  {
	  std::string uti_omega_str = parameters->value( UTI_str+".omega" );
    if( uti_omega_str.empty() ) {
  	  errors->report( UTI_param_name, "An omega value must be specified for "
                                      "the upper tail extrapolation functions" );
      return false;
    }
	  double uti_omega = String_Op::to_number< double >( uti_omega_str );
  	
	  if(UTI_function_str == "Hyperbolic" )
		  nparam_cdf.upper_tail_interpolator( Tail_interpolator( new Hyperbolic_UTI(uti_omega) ) );
	  else if(UTI_function_str == "Power") 
	  {
      double z_max = *(nparam_cdf.z_end()-1);
		  std::string power_max = parameters->value( UTI_str+".extreme" );
      if( !power_max.empty() ) z_max = String_Op::to_number< double >( power_max );
  /*    if( power_max.empty() ) {
  		  errors->report( UTI_param_name, "A maximum value must be specified for "
                                        "the Power extrapolation function" );
        return false;
      }*/
		  //double z_max = String_Op::to_number< double >( power_max );
      double prop_max = *(nparam_cdf.z_end()-1);
      if( prop_max > z_max ) {
        std::ostringstream message;
        message << "The max value must be greater than " << prop_max;
        errors->report( UTI_param_name, message.str() );
        return false;
      }
		  nparam_cdf.upper_tail_interpolator( Tail_interpolator( new Power_UTI(z_max,uti_omega) ) );
	  }
  }

  return true;
}



void setup_cdf_tails( NonParametricCdfType* cdf,
                      const std::string& min_tag, 
                      const std::string& max_tag,
                      const Parameters_handler* parameters, 
				              Error_messages_handler* errors ) {

  if( cdf->z_begin() == cdf->z_end() ) return;

  double min = 
    String_Op::to_number<double>( parameters->value( min_tag + ".value" ) );
  double max = 
    String_Op::to_number<double>( parameters->value( max_tag + ".value" ) );

  double prop_min = *(cdf->z_begin());
  if( prop_min <= min ) {
    std::ostringstream message;
    message << "The min value must be lesser than " << prop_min;
    errors->report( min_tag, message.str() );
  }

  double prop_max = *( cdf->z_end() - 1 );
  if( prop_max >= max ) {
    std::ostringstream message;
    message << "The max value must be greater than " << prop_max;
    errors->report( max_tag, message.str() );
  }

  Power_UTI upper_tail( max );
  Power_LTI lower_tail( min );
  cdf->upper_tail_interpolator( Tail_interpolator( &upper_tail ) );
  cdf->lower_tail_interpolator( Tail_interpolator( &lower_tail ) );  
}



}  // namespace

