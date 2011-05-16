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

#include "filters.h"

/// -----------------------------------------------
// for the base filter class "Filter"

/*
 * destructor, delete the offset geometrhy
 */
Filter::~Filter()
{
    if ( offset ) 
        delete offset;
}


/*
 * this function is used to get the tempate geometry at a given grid 
 */
Grid_template* Filter::get_window_geometry(int ncoarse)
{
    Grid_template* one_template = new Grid_template();

    // calculate the jump step
    int expansion_factor = pow(2.0, ncoarse-1);
    
    for(int k=-nzdt_; k<=nzdt_; k++) {
        for(int j=-nydt_; j<=nydt_; j++) {
            for(int i=-nxdt_; i<=nxdt_; i++) {					
                one_template->add_vector( expansion_factor * i, 
                                          expansion_factor * j, 
                                          expansion_factor * k, -1);	
            }
        }
    }
	
	return one_template;
}


/*
 * this function is used to get a general rectangular tempate geometry 
 * at a given grid and the given half template size (nx, ny, nz)
 */
Grid_template* Filter::get_window_geometry(int ncoarse, int nx, int ny, int nz)
{
    Grid_template* one_template = new Grid_template();

    // calculate the jump step
    int expansion_factor = pow(2.0, ncoarse-1);
    
    for(int k=-nz; k<=nz; k++) {
        for(int j=-ny; j<=ny; j++) {
            for(int i=-nx; i<=nx; i++) {					
                one_template->add_vector( expansion_factor * i, 
                                          expansion_factor * j, 
                                          expansion_factor * k, -1);	
            }
        }
    }
	
	return one_template;
}


/*
 * this function is used to get a general rectangular dual-tempate geometry 
 * at a given grid and the given half template size (nx, ny, nz)*pow(2,ncoarse-1)
 */
Grid_template* Filter::get_dual_window_geometry(int ncoarse, int nx, int ny, int nz)
{
    Grid_template* one_template = new Grid_template();

    // calculate the jump step
    int expansion_factor = pow(2.0, ncoarse-1);
    int dual_nx = nx*expansion_factor;
    int dual_ny = ny*expansion_factor;
    int dual_nz = nz*expansion_factor;
    
    for(int k=-dual_nz; k<=dual_nz; k++) {
        for(int j=-dual_ny; j<=dual_ny; j++) {
            for(int i=-dual_nx; i<=dual_nx; i++) {					
                one_template->add_vector( i, j, k, -1);	
            }
        }
    }
	
	return one_template;
}

/// -----------------------------------------------
// for the default filter class "Filters_default"

/*
 * constructor function 
 *      nxdt, nydt, nzdt is the half size of the search template
 */
Filters_default::Filters_default(int nxdt, int nydt, int nzdt, int nfilter)
                        : Filter( nxdt, nydt, nzdt ) 
{ 
    nfilter_ = nfilter; 

    save_pixel_weight();    // score loading weight
    save_filter_name();     // filter name
    save_filter_weight();   // weight for each filter

    nfilter_ = filter_name.size();   // in case, reset # of filters

    offset = get_window_geometry();
}


/*
 * calculate the E-W average filter loading weight
 */
vector<float> Filters_default::filter_average_EW() 
{
	vector<float> weights;
	float local_weight;
	float denominator = std::max( (float)nxdt_, 0.1f );
	
    for(int k=-nzdt_; k<=nzdt_; k++) { 
        for(int j=-nydt_; j<=nydt_; j++) {
            for(int i=-nxdt_; i<=nxdt_; i++) {
                local_weight=1-abs(i)/denominator;
                weights.push_back(local_weight);
            }
        }
    }

	return weights;
}   

/*
 * calculate the N-S average filter loading weight
 */
vector<float> Filters_default::filter_average_NS() 
{
	vector<float> weights;
	float local_weight;
    float denominator = std::max( (float)nydt_, 0.1f );
	
	for(int k=-nzdt_; k<=nzdt_; k++) {
		for(int j=-nydt_; j<=nydt_; j++) {
			for(int i=-nxdt_; i<=nxdt_; i++) {
				local_weight=1-abs(j)/denominator;
				weights.push_back(local_weight);
			}
		}
	}

	return weights;
}   


/*
 * calculate the T-B average filter loading weight
 */
vector<float> Filters_default::filter_average_TB() 
{
	vector<float> weights;
	float local_weight;
    float denominator = std::max( (float)nzdt_, 0.1f );
	
	for(int k=-nzdt_; k<=nzdt_; k++) {
		for(int j=-nydt_; j<=nydt_; j++) {
			for(int i=-nxdt_; i<=nxdt_; i++) {
				local_weight=1-abs(k)/denominator;
				weights.push_back(local_weight);
			}
		}
	}

	return weights;
}  

/*
 * calculate the E-W gradient filter loading weight
 */
vector<float> Filters_default::filter_gradient_EW() 
{
	vector<float> weights;
	float local_weight;
    float denominator = std::max( (float)nxdt_, 0.1f );
	
	for(int k=-nzdt_; k<=nzdt_; k++) {
		for(int j=-nydt_; j<=nydt_; j++) {
			for(int i=-nxdt_; i<=nxdt_; i++) {
				local_weight=(float)i/denominator;
				weights.push_back(local_weight);
			}
		}
	}

	return weights;
}   


/*
 * calculate the N-S gradient filter loading weight
 */
vector<float> Filters_default::filter_gradient_NS() 
{
	vector<float> weights;
	float local_weight;
    float denominator = std::max( (float)nydt_, 0.1f);
	
	for(int k=-nzdt_; k<=nzdt_; k++) {		 
		for(int j=-nydt_; j<=nydt_; j++) {
			for(int i=-nxdt_; i<=nxdt_; i++) {
				local_weight=(float)j/denominator;
				weights.push_back(local_weight);
			}
		}
	}

	return weights;
}   


/*
 * calculate the T-B gradient filter loading weight
 */
vector<float> Filters_default::filter_gradient_TB() 
{
	vector<float> weights;
	float local_weight;
    float denominator = std::max( (float)nzdt_, 0.1f );
	
	for(int k=-nzdt_; k<=nzdt_; k++) {		 
		for(int j=-nydt_; j<=nydt_; j++) {
			for(int i=-nxdt_; i<=nxdt_; i++) {
				local_weight=(float)k/denominator;
				weights.push_back(local_weight);
			}
		}
	}

	return weights;
}   

/*
 * calculate the E-W curvature filter loading weight
 */
vector<float> Filters_default::filter_curvature_EW() 
{
	vector<float> weights;
	float local_weight;
    float denominator = std::max( (float)nxdt_, 0.1f);
	
	for(int k=-nzdt_; k<=nzdt_; k++) {
		for(int j=-nydt_; j<=nydt_; j++) {
			for(int i=-nxdt_; i<=nxdt_; i++) {
				local_weight=2.0*abs(i)/denominator-1;
				weights.push_back(local_weight);
			}
		}
	}

	return weights;
}   


/*
 * calculate the N-S curvature filter loading weight
 */
vector<float> Filters_default::filter_curvature_NS() 
{
	vector<float> weights;
	float local_weight;
    float denominator = std::max( (float)nydt_, 0.1f);
	
	for(int k=-nzdt_; k<=nzdt_; k++) {
		for(int j=-nydt_; j<=nydt_; j++) {
			for(int i=-nxdt_; i<=nxdt_; i++) {
				local_weight=2.0*abs(j)/denominator-1;
				weights.push_back(local_weight);
			}
		}
	}

	return weights;
}   


/*
 * calculate the T-B curvature filter loading weight
 */
vector<float> Filters_default::filter_curvature_TB() 
{
	vector<float> weights;
	float local_weight;
    float denominator = std::max( (float)nzdt_, 0.1f);
	
	for(int k=-nzdt_; k<=nzdt_; k++) {
		for(int j=-nydt_; j<=nydt_; j++) {
			for(int i=-nxdt_; i<=nxdt_; i++) {
				local_weight=2.0*abs(k)/denominator-1;
				weights.push_back(local_weight);
			}
		}
	}

	return weights;
}   

/*
 * save all the filter score loading weights in a certain sequence
 * depending on the 2D or 3D grid
 */
void Filters_default::save_pixel_weight()
{
    // save all weights of 3D template
	weight.push_back(filter_average_EW());      // 0
	weight.push_back(filter_average_NS());      // 1
    weight.push_back(filter_average_TB());      // 2

	weight.push_back(filter_gradient_EW());     // 3
	weight.push_back(filter_gradient_NS());     // 4
	weight.push_back(filter_gradient_TB());     // 5

	weight.push_back(filter_curvature_EW());    // 6
	weight.push_back(filter_curvature_NS());    // 7
	weight.push_back(filter_curvature_TB());    // 8

    vector< vector<float> >::iterator iter=weight.begin();

	if( nxdt_ == 0 )      // nx=1;
	{
		weight.erase(iter+6);
		weight.erase(iter+3);
		weight.erase(iter);
	}
	else if( nydt_ == 0)  // ny=1;
	{
		weight.erase(iter+7);
		weight.erase(iter+4);
		weight.erase(iter+1);
	}
	else if( nzdt_ == 0 )  // nz=1;
	{
		weight.erase(iter+8);
		weight.erase(iter+5);
		weight.erase(iter+2);
	}
}


/*
 * save all the filter name in a certain sequence
 * depending on the 2D or 3D grid
 */
void Filters_default::save_filter_name()
{
    // save filter names
    filter_name.push_back("EW_average");
    filter_name.push_back("NS_average");
    filter_name.push_back("TB_average");

    filter_name.push_back("EW_gradient");
    filter_name.push_back("NS_gradient");
    filter_name.push_back("TB_gradient");

    filter_name.push_back("EW_curvature");
    filter_name.push_back("NS_curvature");
    filter_name.push_back("TB_curvature");

    vector< string >::iterator siter=filter_name.begin();

	if( nxdt_ == 0 )      // nx=1;
	{
		filter_name.erase(siter+6);
		filter_name.erase(siter+3);
		filter_name.erase(siter);
	}
	else if( nydt_ == 0)  // ny=1;
	{
		filter_name.erase(siter+7);
		filter_name.erase(siter+4);
		filter_name.erase(siter+1);
	}
	else if( nzdt_ == 0 )  // nz=1;
	{
		filter_name.erase(siter+8);
		filter_name.erase(siter+5);
		filter_name.erase(siter+2);
	}
}

/*
 * save all the filter  weights in a certain sequence
 * depending on the 2D or 3D grid
 */
void Filters_default::save_filter_weight()
{
    // save all weights of 3D template
    float average = 3.0;
	filter_weight.push_back(average);       //filter_average_EW
	filter_weight.push_back(average);       //filter_average_NS
    filter_weight.push_back(average);       //filter_average_TB

    float gradient = 2.0;
	filter_weight.push_back(gradient);      //filter_gradient_EW
	filter_weight.push_back(gradient);      //filter_gradient_NS
	filter_weight.push_back(gradient);      //filter_gradient_TB

    float curvature = 1.0;
	filter_weight.push_back(curvature);     //filter_curvature_EW
	filter_weight.push_back(curvature);     //filter_curvature_NS
	filter_weight.push_back(curvature);     //filter_curvature_TB

    vector<float>::iterator iter=filter_weight.begin();

	if( nxdt_ == 0 )      // nx=1;
	{
		filter_weight.erase(iter+6);
		filter_weight.erase(iter+3);
		filter_weight.erase(iter);
	}
	else if( nydt_ == 0)  // ny=1;
	{
		filter_weight.erase(iter+7);
		filter_weight.erase(iter+4);
		filter_weight.erase(iter+1);
	}
	else if( nzdt_ == 0 )  // nz=1;
	{
		filter_weight.erase(iter+8);
		filter_weight.erase(iter+5);
		filter_weight.erase(iter+2);
	}

    // standardize the weight assgin to each filter
    standardize_weight( filter_weight );
}


/// --------------------------------------------------------
// for the user defined filter class "Filters_user_define"

/*
 * constructor function 
 *      nxdt, nydt, nzdt is the half size of the search template
 *      filename is the data file contains the user defined filter information
 * for the purpose of simulation, the score filter template must be rectangular,
 * and only the nodes within the search template are counted. For those nodes not
 * specified in the filter data file are assigned 0 weight 
 */
Filters_user_define::Filters_user_define(int nxdt, int nydt, int nzdt, string filename)
                        : Filter( nxdt, nydt, nzdt)
{
    int i;
    string line;

    // open the data file
    ifstream infile( filename.c_str() );

    // get total number of filters
    getline(infile,line);
    nfilter_ = String_Op::to_number<int>(line); 

    // # of nodes in searching template X direction
    int size_x = 2*nxdt+1;  
    // # of nodes in search template XY plane
    int size_xy = (2*nxdt+1)*(2*nydt+1); 
    // # of nodes in search template
    int nb_templ = (2*nxdt+1)*(2*nydt+1)*(2*nzdt+1);
    // initialize the score loading weight as 0 for each offset
    vector<float> one_weight(nb_templ, 0.);

    // set the same geometry of the whole searching template for each filter
    offset = get_window_geometry();

    for ( i=0; i<nfilter_; i++ )
        weight.push_back( one_weight );

    // get the filter weights
    int dx, dy, dz;
    float weight_value;
    int current_loc;            // node_id within the searching template
    int cur_filter = -1;        // the cur_filter^{th} filter

    while( getline(infile,line) )
    {
        string cur_str;
        vector< string > str; 
        istringstream scan_stream( line );

        while( scan_stream >> cur_str ) 
            str.push_back( cur_str );

        if ( str.size() == 4 )  // contains offset and loading weight
        {
            // offset in X, Y, Z direction
            dx = String_Op::to_number<int>(str[0]);
            dy = String_Op::to_number<int>(str[1]);
            dz = String_Op::to_number<int>(str[2]);
            // score loading weight
            weight_value = String_Op::to_number<float>(str[3]);

            // current node location in the template
            current_loc = size_xy*(dz+nzdt) + size_x*(dy+nydt) + (dx+nxdt);

            // only keep the nodes within the searching template
            if ( current_loc>-1 && current_loc<nb_templ )
                weight[cur_filter].operator[](current_loc) = weight_value;
        }
        else    // contains filter name and filter weigth
        {
            cur_filter++;           
            filter_name.push_back( str[0] );    // filter name
            filter_weight.push_back( String_Op::to_number<float>(str[1]) ); // filter weight
        }
    }   // end while( getline(infile,line) )

    // standardize the weight assgin to each filter
    standardize_weight( filter_weight );
}


// ------------------------
// for class "Transcat_Filter_default"

Transcat_Filter_default::Transcat_Filter_default(int nxdt, int nydt, int nzdt, int nfilter) : Filter( nxdt, nydt, nzdt ) 
{ 
    nfilter_ = nfilter; 
    create_one_filter_weight();   // weight for each filter node
    offset = get_window_geometry();
}


void Transcat_Filter_default::create_one_filter_weight()
{
    int size_x = 2*nxdt_+1;  
    int size_xy = (2*nxdt_+1)*(2*nydt_+1); 
    int nb_templ = (2*nxdt_+1)*(2*nydt_+1)*(2*nzdt_+1);
    vector<float> one_weight(nb_templ, 1.);

    /*  for test only
    one_weight[ size_xy*(0+nzdt_) + size_x*(-1+nydt_) + (-1+nxdt_) ] = 2.;
    one_weight[ size_xy*(0+nzdt_) + size_x*(-1+nydt_) + (1+nxdt_) ] = 2.;
    one_weight[ size_xy*(0+nzdt_) + size_x*(1+nydt_) + (-1+nxdt_) ] = 2.;
    one_weight[ size_xy*(0+nzdt_) + size_x*(1+nydt_) + (1+nxdt_) ] = 2.;

    one_weight[ size_xy*(0+nzdt_) + size_x*(0+nydt_) + (1+nxdt_) ] = 3.;
    one_weight[ size_xy*(0+nzdt_) + size_x*(0+nydt_) + (-1+nxdt_) ] = 3.;
    one_weight[ size_xy*(0+nzdt_) + size_x*(1+nydt_) + (0+nxdt_) ] = 3.;
    one_weight[ size_xy*(0+nzdt_) + size_x*(-1+nydt_) + (0+nxdt_) ] = 3.;

    one_weight[ size_xy*(0+nzdt_) + size_x*(0+nydt_) + (0+nxdt_) ] = 5.;
    */

    // the filter weight for the central node
    float nx = float(2*nxdt_+1);
    float ny = float(2*nydt_+1);
    float nz = float(2*nzdt_+1);
    float center_value = max( nx, max( ny, nx ) );

    for (int k=-nzdt_; k<=nzdt_; k++)
    {
        for (int j=-nydt_; j<=nydt_; j++)
        {
            for (int i=-nxdt_; i<=nxdt_; i++)
            {
                if ( ( nx!=1 && ny!=1 && nz!=1 && std::abs(k) != nzdt_ && std::abs(j) != nydt_ && std::abs(i) != nxdt_ ) ||
                      ( nx==1 && std::abs(k) != nzdt_ && std::abs(j) != nydt_ ) ||
                      ( ny==1 && std::abs(k) != nzdt_ && std::abs(i) != nxdt_ ) ||
                      ( nz==1 && std::abs(j) != nydt_ && std::abs(i) != nxdt_ ) )
                {
                    // calculate the geometric distance
                    float dist = std::sqrt( pow(i*center_value/nx, 2) + pow(j*center_value/ny, 2) + pow(k*center_value/nz, 2) );
                    one_weight[ size_xy*(k+nzdt_) + size_x*(j+nydt_) + (i+nxdt_) ] = round(center_value/(1.0+dist));
                }
            }
        }
    }

    // reset the nodes along the X axis
    for (int ik=0; ik<nxdt_; ik++)
    {
        int val = round(center_value-2.0*ik/nx*center_value);
        one_weight[ size_xy*(nzdt_) + size_x*(nydt_) + (nxdt_+ik) ] = val;
        one_weight[ size_xy*(nzdt_) + size_x*(nydt_) + (nxdt_-ik) ] = val;
    }
    // reset the nodes along the Y axis
    for (int jk=0; jk<nydt_; jk++)
    {
        int val = round(center_value-2.0*jk/ny*center_value);
        one_weight[ size_xy*(nzdt_) + size_x*(nydt_+jk) + (nxdt_) ] = val;
        one_weight[ size_xy*(nzdt_) + size_x*(nydt_-jk) + (nxdt_) ] = val;
    }
    // reset the nodes along the Z axis
    for (int kk=0; kk<nzdt_; kk++)
    {
        int val = round(center_value-2.0*kk/nz*center_value);
        one_weight[ size_xy*(nzdt_+kk) + size_x*(nydt_) + (nxdt_) ] = val;
        one_weight[ size_xy*(nzdt_-kk) + size_x*(nydt_) + (nxdt_) ] = val;
    }

#if defined ( _DEBUG )
    cout << "-------- the filter weights are --------\n";
    for (int k=-nzdt_; k<=nzdt_; k++)
    {
        cout << "Layer " << k << ":\n"; 
        for (int j=-nydt_; j<=nydt_; j++)
        {
            for (int i=-nxdt_; i<=nxdt_; i++)
            {
                cout << one_weight[ size_xy*(k+nzdt_) + size_x*(j+nydt_) + (i+nxdt_) ] << "    ";
            }
            cout << endl;
        }
        cout << endl;
    }
#endif

    weight.push_back( one_weight  );
}
