#include "gridSampling.h"
#include <GsTLAppli/geostat/parameters_handler.h>
#include <GsTLAppli/utils/gstl_plugins.h>
#include <GsTLAppli/utils/error_messages_handler.h>
#include <GsTLAppli/utils/string_manipulation.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/neighborhood.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h>
#include <GsTLAppli/math/gstlpoint.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

/*
Initialize the parameters
*/
bool gridSampling::initialize( const Parameters_handler* parameters,
			Error_messages_handler* errors ) {
    param_global=parameters;
	std::cout << "initializing algorithm Grid Sampling \n";

	std::string harddata_grid_name = parameters->value( "Hard_Data.grid" );
	errors->report( harddata_grid_name.empty(), 
			"Hard_Data", "No hard data specified" );
	std::string harddata_prop_name_ = parameters->value( "Hard_Data.property" );
	errors->report( harddata_prop_name_.empty(), 
			"Hard_Data", "No property name specified" );
	
	// Get the simulation grid from the grid manager
	if( !harddata_grid_name.empty() ) {
		bool ok = geostat_utils::create( grid_, harddata_grid_name,
				 "Grid_Name", errors );
		if( !ok ) return false;
	}
	else 
		return false;

	samplingType = parameters->value( "Sampling_type.value" );	

	std::string samp_prop_name_ = parameters->value( "Samp_prop_name.value" );
	errors->report( samp_prop_name_.empty(), 
			  "Sampling", "No property name specified" );
	propSamp  = geostat_utils::add_property_to_grid( grid_, samp_prop_name_);

	propHard = grid_->property( harddata_prop_name_ );

	do_output = false;
	if(parameters->value("writeSample.value") != "0")
	{
		do_output = true;
		outFileName =	parameters->value("outfileName.value");
	}

	return true;
}

int gridSampling::execute( GsTL_project* proj ) { 
//int gridSampling::execute( ) { 
	
	if(	samplingType == "Regular Grid Sampling" ) gridSampling::regularSampling();
	else if ( samplingType == "Stratified Grid Sampling") gridSampling::stratRandomSampling();
	else if ( samplingType == "Random Sampling") gridSampling::randomSampling();
	else if ( samplingType == "Histogram Sampling") gridSampling::histogramSampling();  
	else if ( samplingType == "Extreme Sampling") gridSampling::extremeSampling();  
	else return 1;
	if(do_output) gridSampling::output();	
	return 0;
}

int gridSampling::extremeSampling() 
{
    int i,j;
    int nsize = grid_->size();

	nSamples = String_Op::to_number<int>( param_global->value( "num_of_samples_extreme.value" ) );
	int seed_ = String_Op::to_number<int>( param_global->value( "seed_extreme.value" ) );
	Global_random_number_generator::instance()->seed( seed_ );
    STL_generator gen;

    if (nSamples>=nsize)
    {
        for (i=0; i<nsize; i++)
            propSamp->set_value( propHard->get_value(i), i );

        return 0;
    }

    std::vector< std::pair<int, double> > vp;
    for (i=0; i<nsize; i++ )
        vp.push_back( std::make_pair(i, propHard->get_value(i)) );

    // sort the property values in ascending order
    std::stable_sort( vp.begin(), vp.end(), prop_greater );

    // set up the number of sampling regions
    int ncut=10;
    if (ncut>nSamples)    ncut=nSamples;
    int nb_intervals=std::floor((float)nsize/ncut);
    int nb_samples=std::floor((float)nSamples/3); 

    while ( nb_intervals<nb_samples )
    {
        ncut--;
        nb_intervals=std::floor((float)nsize/ncut);
    }

    int istart,  iend;
    int nb_select=nb_samples; 

    for (i=0; i<3; i++)
    {
        if (i==0)
        {
            istart=0;
            iend=nb_intervals;
        }
        else if (i==1)
        {
            istart=nb_intervals;
            iend=(ncut-1)*nb_intervals;
        }
        else   
        {
            istart=(ncut-1)*nb_intervals;
            iend=nsize;
            nb_select = nSamples-nb_samples*2;
        }

        std::random_shuffle(vp.begin()+istart, vp.begin()+iend, gen );

        for (j=0; j<nb_select; j++)
        {
            int node_id = vp[istart+j].first;
            if (propHard->is_informed(node_id))
                propSamp->set_value( propHard->get_value(node_id), node_id );
        }
    }

	return 0;
}

int gridSampling::histogramSampling() 
{
    int i,j;
    int nsize = grid_->size();

	nSamples = String_Op::to_number<int>( param_global->value( "num_of_samples_hist.value" ) );
	int seed_ = String_Op::to_number<int>( param_global->value( "seed_hist.value" ) );
	Global_random_number_generator::instance()->seed( seed_ );
    STL_generator gen;

    std::vector< std::pair<int, double> > vp;
    for (i=0; i<nsize; i++ )
        vp.push_back( std::make_pair(i, propHard->get_value(i)) );

    // sort the property values in ascending order
    std::stable_sort( vp.begin(), vp.end(), prop_greater );

    // set up the number of sampling regions
    int ncut=String_Op::to_number<int>( param_global->value( "num_of_intervals.value" ) );
    if (ncut>nSamples)    ncut=nSamples;
    int nb_intervals=std::floor((float)nsize/ncut);
    int nb_samples=std::floor((float)nSamples/ncut); 

    int istart, iend; 
    for (i=0; i<=ncut; i++)
    {
        istart=(i-0.5)*nb_intervals;
        iend=(i+0.5)*nb_intervals;
        if (istart<0) istart=0;
        if (iend>nsize)   iend=nsize;
        /*
        if (iend==ncut-1)   
        {
            iend=nsize;
            nb_samples = nSamples-nb_samples*ncut;
        }
        */

        std::random_shuffle(vp.begin()+istart, vp.begin()+iend, gen );
        int nb_select = (i==0 )? nb_samples/2 : nb_samples;
        if (i==ncut)   nb_select = nSamples-nb_samples*(ncut-0.5);

        for (j=0; j<nb_select; j++)
        {
            int node_id = vp[istart+j].first;
            if (propHard->is_informed(node_id))
                propSamp->set_value( propHard->get_value(node_id), node_id );
        }
    }

	return 0;
}

int gridSampling::randomSampling() {

	nSamples = String_Op::to_number<int>( param_global->value( "num_of_samples.value" ) );
	int seed_ = String_Op::to_number<int>( param_global->value( "seed_rand.value" ) );
	Global_random_number_generator::instance()->seed( seed_ );

	grid_->init_random_path();
	Geostat_grid::random_path_iterator it = grid_->random_path_begin(propHard);
	Geostat_grid::random_path_iterator it_end = grid_->random_path_end(propHard);
	for( int i=0; i<nSamples;i++)  {
		if( it->is_informed() )
			propSamp->set_value(it->property_value(),it->node_id());
		else i--;
		it++;
		if(it == it_end ) break;
	}
	return 0;
}


int gridSampling::stratRandomSampling() {

	RGrid *regGrid=dynamic_cast< RGrid*>(grid_);
	if( !regGrid ) {
		GsTLcerr << "The object is not a regular grid\n" << gstlIO::end;
		return 0;
	}

	int seed_ = String_Op::to_number<int>( param_global->value( "seed_strat.value" ) );
	Global_random_number_generator::instance()->seed( seed_ );	
	STL_generator gen;
    //Random_number_generator gen;

	int nWin[3];
	nWin[0] = std::min(regGrid->nx(), String_Op::to_number<int>
		( param_global->value( "num_of_X_window.value" ) ) );
	nWin[1] = std::min(regGrid->ny(), String_Op::to_number<int>
		( param_global->value( "num_of_Y_window.value" ) ) );
	nWin[2] = std::min(regGrid->nz(), String_Op::to_number<int>
		( param_global->value( "num_of_Z_window.value" ) ));

	int sizeWin[3] = { (regGrid->nx())/nWin[0],
		(regGrid->ny())/nWin[1],(regGrid->nz())/nWin[2] };
	
	int nSampWin = std::min(String_Op::to_number<int>
		( param_global->value( "num_of_samp_per_w.value" ) ), 
		sizeWin[0]*sizeWin[1]*sizeWin[2]);
	
	const Grid_template* stratTpl = fillTemplate( sizeWin );
	//std::vector< Geovalue > gvalVec(nSampWin);
	Window_neighborhood* stratNeigh = regGrid->window_neighborhood(*stratTpl);
	stratNeigh->select_property( propHard->name() );
	std::vector< Geovalue > winNeigh;

    int node_id;

    //SGrid_cursor* cursor = regGrid->cursor();
	Geovalue geoval(regGrid, propHard, 0);
	for( int ix=0; ix<nWin[0]; ix++)
	for( int iy=0; iy<nWin[1]; iy++)
	for( int iz=0; iz<nWin[2]; iz++){
        node_id = regGrid->cursor()->node_id(ix*sizeWin[0], iy*sizeWin[1], iz*sizeWin[2]);
        if ( regGrid->is_valid(node_id) )
        {
            geoval.set_node_id( node_id );
            stratNeigh->find_neighbors( geoval );
            std::random_shuffle(stratNeigh->begin(), stratNeigh->end(), gen );
            Window_neighborhood::const_iterator it=stratNeigh->begin();
            for( int i=0; (i < nSampWin && it != stratNeigh->end() ); ++i,++it )
                if(it->is_informed() ) propSamp->set_value(it->property_value(),it->node_id());
        }
	}

	return 0;
}


int gridSampling::regularSampling() {

	RGrid *regGrid=dynamic_cast< RGrid* >(grid_);
	if( !regGrid ) {
		GsTLcerr << "The object is not a regular grid\n" << gstlIO::end;
		return 0;
	}

	int stepSize[3] = {String_Op::to_number<int>
		( param_global->value( "step_size_X.value" ) ),
		String_Op::to_number<int>
		( param_global->value( "step_size_Y.value" ) ),
		String_Op::to_number<int>
		( param_global->value( "step_size_Z.value" ) )};


	int id;
	for( int iz = floor( (double)stepSize[2]/2 ); iz < regGrid->nz() ; iz=iz+stepSize[2] )
	for( int iy = floor( (double)stepSize[1]/2 ); iy < regGrid->ny() ; iy=iy+stepSize[1] )
	for( int ix = floor( (double)stepSize[0]/2 ); ix < regGrid->nx() ; ix=ix+stepSize[0] ) {
		id = regGrid->cursor()->node_id(ix,iy,iz);
		if( id<0 ) {
			//GsTLcerr << "Error in cursor" << gstlIO::end;
			//return 0;
		}
		else if(propHard->is_informed(id) )
			propSamp->set_value(propHard->get_value(id),id);
	}
	return 0;
}


const Grid_template* gridSampling::fillTemplate(int sizeWin[3])
{
	Grid_template* stratTpl = new Grid_template();
	std::vector< Geovalue > gvalVec;
	for( GsTLInt i=0; i < sizeWin[0]; i++)
		for( GsTLInt j=0; j < sizeWin[1]; j++)
			for( GsTLInt k=0; k < sizeWin[2]; k++)
				stratTpl->add_vector(i,j,k);
	return stratTpl;
}


int gridSampling::output() {

	std::ofstream ofile( outFileName.c_str() );
  if( !ofile ) {
	  std::cerr << "can't create output file" << std::endl;
    return 1;
  }

	Geostat_grid::iterator it = grid_->begin(propSamp);

  ofile << "gridSampling" << std::endl << "4" << std::endl << "X" <<std::endl<<"Y"<<std::endl\
	  <<"Z"<<std::endl<<propSamp->name() << std::endl ;
	int id;
  while( it!=grid_->end(propSamp) ) {
	  id = it->node_id();
	  if( it->is_informed() ) ofile<< grid_->location(id)[0]<<" "<<grid_->location(id)[1]
		  <<" "<<grid_->location(id)[2]<<" "<<it->property_value()<<std::endl;
	  it++;
  }

	return 0;
}


Named_interface* gridSampling::create_new_interface( std::string& ) {
  return new gridSampling;
}

//GEOSTAT_PLUGIN(gridSampling);
