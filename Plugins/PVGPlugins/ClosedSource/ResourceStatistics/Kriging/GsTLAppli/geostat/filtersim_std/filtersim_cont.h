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


#ifndef _Filtersim_Cont_H__
#define _Filtersim_Cont_H__

#include <GsTLAppli/geostat/common.h>
#include <GsTLAppli/geostat/geostat_algo.h>
#include <GsTLAppli/utils/gstl_types.h>
#include <GsTLAppli/grid/grid_model/geovalue.h>
#include <GsTLAppli/grid/grid_model/property_copier.h>

#include <GsTL/utils/smartptr.h>

#include <stdlib.h>
#include <functional>
#include <algorithm>
#include <vector>
#include <string>
#include <set>
#include <map>

#include "filters.h"
#include "prototype.h"
#include "pattern.h"
#include "patch_helper.h"
#include "is_categorical.h"
#include "patch_helper.h"
#include "prototype_list.h"
#include "dev_finder.h"
#include "partition.h"
#include "pattern_paster.h"
#include "Prototype_kernelized_kmeans.h"
#include "distance_kernels.h"
#include "mapping_pattern_id.h"

using namespace std;
using namespace geostat_utils;

class Strati_grid;
class Grid_template;
class Geostat_grid;
class RGrid;
class Neighborhood;
class GsTLGridProperty;

typedef  int  GsTLInt;
typedef  GsTLTripletTmpl<GsTLInt> GsTLGridNode;
typedef Manhattan_Distance< vector<float>::iterator > Distance;
//typedef Euclidean_Distance< vector<float>::iterator > Distance;
typedef Prototype_Categorical< Distance > Cate_Prototype;
typedef Prototype_Continuous< Distance > Cont_Prototype;

// proptotypes definitions
// continuous+cross
typedef InitializePrototypeList<CrossPartition, Cont_Prototype> Initializer3;
typedef SplitPrototype<DefaultSplitter, Cont_Prototype> Splitter3;
typedef PrototypeList< Cont_Prototype, Initializer3, Splitter3, Distance >  PrototypeListType3;

// continuous+kmean
typedef InitializePrototypeList<Prototype_kmeans, Cont_Prototype > Initializer4;
typedef SplitPrototype<Prototype_kmeans, Cont_Prototype> Splitter4;
typedef PrototypeList< Cont_Prototype, Initializer4, Splitter4, Distance >  PrototypeListType4;


// filtersim class
class GEOSTAT_DECL Filtersim_Cont: public Geostat_algo 
{
public:
	Filtersim_Cont();
	~Filtersim_Cont();
	
	virtual bool initialize( const Parameters_handler* parameters, Error_messages_handler* errors );
	virtual int execute( GsTL_project* proj=0 ) ;
	virtual std::string name() const { return "filtersim_cont"; }
	static Named_interface* create_new_interface(std::string&);

private:
	// for simulation
    bool simulate_one_realization( SmartPtr<Progress_notifier>& progress_notifier, GsTLGridProperty* prop, int nreal);

	void init_random_path();
    void copy_pre_simulation_data();
    void set_region_indicator();
    void pre_progress_notifer(int& total_steps, int& frequency);

	void clean( GsTLGridProperty* prop = 0 );
    void get_filter_score_weight();

    // for pasting with dual tempate
    void fillin_with_dual_template( GsTLGridProperty* prop, Pattern_Node_Map& mapping, int ncoarse );
    void record_intermediate_sim( GsTLGridProperty* prop, int ncoarse, string dual="" );

    template <class CPrototype > void  create_prototypelist( vector<CPrototype>& proto_list, int nreal, int ncoarse, int nb_facies,
                                                    Window_neighborhood* TI_neighbors, Window_neighborhood* patch_neighbors, 
                                                    ScoresType& cur_score, int cmin_replicates_ );

    // for initialize() function
    void get_debug_level( const Parameters_handler* parameters );
    void get_nb_bins(const Parameters_handler* parameters);
    bool get_servo_system( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool get_marginal_data( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool get_simul_grid( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool get_training_image( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool get_hard_data( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool get_soft_data( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool get_region_data( const Parameters_handler* parameters, Error_messages_handler* error_mesgs );
    bool get_TI_scan_template(const Parameters_handler* parameters, Error_messages_handler* error_mesgs);
	bool get_patch_template(const Parameters_handler* parameters, Error_messages_handler* error_mesgs);
    bool get_cmin(const Parameters_handler* parameters, Error_messages_handler* error_mesgs);
    bool get_data_weight(const Parameters_handler* parameters, Error_messages_handler* error_mesgs);
    bool get_filters(const Parameters_handler* parameters, Error_messages_handler* error_mesgs);
    bool create_user_define_filter(const Parameters_handler* parameters, Error_messages_handler* error_mesgs);
    bool valid_filter_file( int nxdt, int nydt, int nzdt, string filter_file_name, Error_messages_handler* error_mesgs);
    bool calculate_max_mgrid( Error_messages_handler* error_mesgs );

    void create_default_filter();
    void print_parameters();

    // for histogram transformation
    void get_property_vector( GsTLGridProperty* prop, vector<float>& data, bool use_region=false );
    void trans_prop_cdf( GsTLGridProperty* prop, Non_param_cdf<>& source_cdf );

private:
    // on the simulation grid
    string simul_grid_name_;
	RGrid* simul_grid_;
	std::string property_name_prefix_;
	MultiRealization_property* multireal_property_;

    // on the TI grid
    string training_image_name_;
	RGrid* training_image_;
	std::string training_property_name_;

    // honor TI histogram on the penultimate grid
    int transcon_data_;
    Non_param_cdf<> target_cdf_;
    bool includes_max_;
    float z_min_;
    float z_max_;
	
    // for soft conditioning
	std::vector<GsTLGridProperty*> softDataProps_;
	std::string softdata_property_names_;
	int use_soft_field_;
		
    // for hard conditioning
	Geostat_grid* harddata_grid_;
	GsTLGridProperty* harddata_property_;
	SmartPtr<Property_copier> property_copier_;
	
    // simulation with regions
	GsTLGridProperty* region_property_;
	GsTLGridProperty* region_simulated_;
	std::string region_property_name_;
	std::string pre_region_property_name_;

	int use_region_;
	int use_pre_region_;
    std::vector<int> active_region_;
    std::vector<bool> region_;
    int nb_region_node_;

    // for filters and scores
	int nfilter_;
	Filter* my_filters_;
    int use_default_filter_;
    vector<float> filter_weight_;
	std::vector<GsTLGridProperty*> scoreProps_;
    Grid_template* window_geom_;
	
    // for debug output
	int is_viewscore_;
    int is_view_intermediate_;
    int is_view_indicator_;

    // for classification
    int is_cross_partition_;
	int nb_bins_;
    int nb_bins_2nd_;
	std::vector<int> cmin_replicates_;
    int treat_cate_as_cont_;

    // for target control
    vector<float> target_cpdf_;
    vector<float> current_prop_;
	float         target_mean_;
    float serv_;
	
	// define windows size to scan TI
	int nxdt_;
	int nydt_;
	int nzdt_;
    int nxyzdt_;
	
	// define patch size for simulation
	int patch_nxdt_;
	int patch_nydt_;
	int patch_nzdt_;
    int patch_nxyzdt_;
	
	// define wights given to three types of data
    vector< float > data_weight_;
	// [0]   weight_hard_data_;
	// [1]   weight_patch_data_;
	// [2]   weight_soft_data_;

	// for random path, and sampler
	long int seed_;
    std::vector<GsTLInt> grid_paths_;

    // prototype bakup data 
    vector< PrototypeListType3 > proto_list3_;
    vector< PrototypeListType4 > proto_list4_;

    // use score based distance or pixel based distance
    int is_dist_from_pixel_;
    bool use_score_dist_;
    vector<float> max_value_;        // the max. value of the filter score
    vector<float> min_value_;
    Pattern_Node_Map pattern_mapping_;

    // other parameters
	int nb_reals_;
	
	int is_categorical_;
	int nb_facies_;
	int nb_multigrids_;
};

#endif  // _Filtersim_Cont_H__

