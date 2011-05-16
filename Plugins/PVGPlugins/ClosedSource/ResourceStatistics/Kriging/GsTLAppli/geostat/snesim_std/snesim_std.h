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
**
**********************************************************************/

#ifndef __GSTLAPPLI_Snesim_Std_H__
#define __GSTLAPPLI_Snesim_Std_H__

#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/geostat/geostat_algo.h>
#include <GsTLAppli/utils/gstl_types.h>
#include <GsTLAppli/grid/grid_model/geovalue.h>
#include <GsTLAppli/grid/grid_model/property_copier.h> 
#include <GsTLAppli/grid/grid_model/sgrid_cursor.h> 

#include <GsTL/cdf/categ_non_param_cdf.h>
#include <GsTL/cdf/non_param_cdf.h>
#include <GsTL/sampler/servo_system.h>

#include <vector>
#include <string>

#include "layer_servo_system_sampler.h"

using namespace std;

#define NUM_SG  3

class Strati_grid;
class Grid_template;
class RGrid;
class Geostat_grid;
class Colocated_neighborhood;
class ComputeLayerIndex;

typedef Categ_non_param_cdf<int> CdfType;
typedef Servo_system_sampler< Random_number_generator > ServoSystem;
typedef Layer_servo_system_sampler< Random_number_generator,ComputeLayerIndex > LayerServoSystem;

class GEOSTAT_DECL  Property_map 
{
public:
	typedef Geovalue key_type;
	typedef float value_type;
	
public:
	Property_map( const GsTLGridProperty* prop ) : prop_(prop) {}
	value_type operator() ( const key_type& g ) const;
	const GsTLGridProperty* property() { return prop_; }
	
private:
	const GsTLGridProperty* prop_;
};


inline float get( const Property_map& map, const Geovalue& g ) 
{
	return map(g);
}


class GEOSTAT_DECL Colocated_value
{
public:
	Colocated_value( const GsTLGridProperty* prop )	
	{
		property_ = prop;
	}
	
	~Colocated_value() {}
	
	template< class Geovalue_ > float get( const Geovalue_& u )  
	{
		return property_->get_value( u.node_id() );
	}
	
private:
	const GsTLGridProperty* property_;
};


class GEOSTAT_DECL ComputeLayerIndex 
{
public:
	typedef Geovalue key_type;
	typedef int value_type;
	
public:
    ComputeLayerIndex(){}
	ComputeLayerIndex( SGrid_cursor* cursor ) : cursor_(cursor) {}

	value_type operator() ( const key_type g )
	{
		int x,y,z;
		cursor_->coords( g.node_id() , x, y, z );
		return z;
	}
	
	value_type operator() ( const int node_id )
	{
		int x,y,z;
		cursor_->coords( node_id, x, y, z );
		return z;
	}

    void set_grid_level( int level )
    {
        cursor_->set_multigrid_level( level );
    }

private:
	SGrid_cursor* cursor_;
};


//=================================
/** Snesim specialized for RGrid's.
*/

class GEOSTAT_DECL Snesim_Std : public Geostat_algo 
{
public:
	static Named_interface* create_new_interface( std::string& );
	
public:
	Snesim_Std() ;
	~Snesim_Std();
	
	virtual bool initialize( const Parameters_handler* parameters, Error_messages_handler* errors );
	virtual int execute( GsTL_project* proj=0 );
	virtual std::string name() const { return "snesim_std"; }
	
private:
    // for execute() function;
    bool simulate_one_realization( SmartPtr<Progress_notifier>& progress_notifier, GsTLGridProperty* prop, int nreal );
    bool get_simulation_choice(int sg_no, int ncoarse);
	void init_random_path(int level);
    void init_random_path_normal(int level);
    void init_random_path_use_region(int level);
	void create_window_geom(int rdmax1,int rdmax2,int rdmax3,double ang1,
		                                     double ang2,double ang3,int max_prevcond);
	void rotate_templates(double theta);
	int Nint(double x);
	Grid_template* multgrid_template(Grid_template* base_template);
    void update_ccdf_from_available(CdfType& ccdf);
    void copy_pre_simulation_data();
    void clean( GsTLGridProperty* prop = 0 );

    // for initialize() function
    bool get_rotation_data( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool get_affinity_data( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool get_region_data( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool get_soft_prob_data( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool get_vert_prob_data( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool get_simul_grid( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool get_training_image( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool get_hard_data( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool get_search_ellipsoid( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool get_marginal_cdf( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    void get_debug_level( const Parameters_handler* parameters );

    bool get_template_expansion_factor( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool read_expansion_datafile( string filename );
    int check_expansion_factor();
    bool is_power_N_factor( int factor, int N );
    bool is_power2_factor( vector< vector<int> >& factor );
    bool is_expansion_factor_dividable(int ncoarse);

    void cal_max_multi_grid_number(int input_nb_multigrids);
	void check_vertical_soft_consistency();
	void check_soft_global();
	void check_vertical_prop( const Parameters_handler* parameters, Error_messages_handler* errors );
    void check_prob_consistency( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
	void print_parameters();
	
private:
   // general parameters
	int nb_reals_;
	long int seed_;
	int nb_facies_;
	int max_prevcond_;      // # of conditioning data
	int cmin_;
	int nb_multigrids_;

    // on simulation grid
    std::string simul_grid_name_;
	RGrid* simul_grid_;
	std::string property_name_prefix_;
	MultiRealization_property* multireal_property_; 

    CdfType marginal_;
	CdfType ccdf_;
	
    // on training image grid
    std::string training_image_name_;
	RGrid* training_image_;
	std::string training_property_name_;
	
	//  for local rotation
	GsTLGridProperty* rot_property_;
	std::string rot_property_name_;

    int local_rot_;
    int angles_ncat_;
	std::vector<double> angles_;

    //  for local scaling
	GsTLGridProperty* aff_property_;
	std::string aff_property_name_;

	int local_aff_;
	int aff_ncat_;
	std::vector<double> aff_[3];
	
	//  for simulation with regions
	GsTLGridProperty* region_property_;
	GsTLGridProperty* region_simulated_;
	std::string region_property_name_;
	std::string pre_region_property_name_;

	int use_region_;
	int use_pre_region_;
    std::vector<int> active_region_;

    //  for soft conditioning data
	std::vector<Property_map> probfield_properties_;
	std::string probfield_property_names_;
	vector< vector<float> > soft_prop_vector_;

	int prob_field_;
	bool use_soft_cube_;
	float tau1_;
	float tau2_;

    // for hard conditioning data
	Geostat_grid* harddata_grid_;
	GsTLGridProperty* harddata_property_;
	SmartPtr<Property_copier> property_copier_;
	
	//  vertical proportion curve
	Strati_grid* vertical_curve_grid_;
	std::vector<GsTLGridProperty*> vertical_properties_;
	std::string vertical_properties_names_;

    bool use_vertical_;
	std::vector<CdfType> vert_marginal_;	//for the vertical proportion curve
	std::vector<CdfType> vert_ccdf_;	//for the vertical proportion curve
    ComputeLayerIndex  get_vertical_index_;	
	vector< vector<float> > vert_prop_vector_;

    // the property to record: 
    //      (1) # of conditioning nodes when use less than 10 conditioning data (or tree level)
    //      (2)  # of conditioning nodes dropped, otherwise
    GsTLGridProperty* data_drop_prop_;
	
    // search template geometry
	Grid_template* window_geom_sg_[NUM_SG+1];
    bool is_power_factor_;
    int iso_expansion_;
    vector< vector<int> > expansion_factor_;
    int tmpl_nz_;     // added to use 2D template for 3D TI
	
    // servo-system
    ServoSystem* sampler1_;
    LayerServoSystem* sampler2_;
	float constraint_to_target_cdf_;
    float nu_;

    // debug level
    int is_view_intermediate_;
    int is_view_node_drop_;

    // for re-simulated nodes
    float revisitNodesProp_;
    int revisit_iter_nb_;
    int revisit_criterion_;

	// vector added for the random subgrid paths
	std::vector<GsTLInt> grid_paths_[NUM_SG+1];

     // for subgrid option
	int subgrid_choice_;
	int num_close_max_;

    /*
	int scanchoice_;
	int nbins_;
	double maxdiff_;
	int hardno_;
	int resim_sg_;  
	int max_scan_;
	
	int resim_choice_;
	int sim_prev_;

	int rdmax1_;
	int rdmax2_;
	int rdmax3_;
	double ang1_;
	double ang2_;
	double ang3_;
	double affx_;
	double affy_;
	double affz_;
    */
};

#endif
