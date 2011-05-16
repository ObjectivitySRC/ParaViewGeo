/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "grid" module of the Geostatistical Earth
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

#include <GsTLAppli/grid/grid_model/rgrid.h>
#include <GsTLAppli/grid/grid_model/sgrid_cursor.h>
#include <GsTLAppli/grid/grid_model/cross_variog_computer.h>
#include <GsTLAppli/grid/grid_model/property_copier.h>
#include <GsTLAppli/math/discrete_function.h>

#include <GsTL/geometry/geometry_algorithms.h>
#include <GsTL/univariate_stats/utils.h>

#include <algorithm>
#include <math.h>
#include <numeric>
#include <iterator>


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


Cross_variog_computer::Cross_variog_computer(Geostat_grid *pset,Geostat_grid *grid):pset_(pset),grid_(grid)
{



    models_["semivariogram"]=semivariogram;
    models_["covariance"]=covariance;
    models_["correlogram"]=correlogram;
    models_["general_relative_semivariogram"]=general_relative_semivariogram;
    models_["semimadogram"]=semimadogram;
    models_["logarithmic_semivariogram"]=logarithmic_semivariogram;
    models_["indicator"]=indicator;



    
     SmartPtr<Property_copier> ptr= Property_copier_factory:: get_copier( pset_, grid_);
    GsTLGridProperty *new_prop = add_property_to_grid((Geostat_grid *)grid_,"temp");
    temp_prop_name=new_prop->name(); 
    ptr->copy(pset_,pset_->selected_property(),(Geostat_grid *)grid_,new_prop);

    double mean;
    const GsTLGridProperty *prop1=pset_->selected_property();
    prim_var_=GsTL::variance(prop1->begin(true),prop1->end(),&mean);

    const GsTLGridProperty *prop2=grid_->selected_property();
    sec_var_=GsTL::variance(prop2->begin(true),prop2->end(),&mean);
    
}


std::vector<int> Cross_variog_computer::compute_variogram_values(Discrete_function &f,
								GsTLVector<int> v,int num,
								float var,std::string& model_type,
								double mod_param)
{

    const RGrid *tempGrid=dynamic_cast<const RGrid*>(grid_);
    SGrid_cursor cursor=*(tempGrid->cursor());
    
    cursor.set_multigrid_level(1);
    std::vector<std::vector<std::pair<float,float> > >table;
    int i,j;

    for (i=0;i<num;i++)
    {
	std::vector<std::pair<float,float> > temp;
	table.push_back(temp);
	for(j=0;j<tempGrid->size();j++)
	{
	    int x,y,z;
	    cursor.coords(j,x,y,z);
	    GsTLGridNode point(x,y,z);	
	    GsTLVector<int> vect(v.x()*(i+1),v.y()*(i+1),v.z()*(i+1));
   GsTLGridNode candidate(point.x()+vect.x(),point.y()+vect.y(),point.z()+vect.z());

	    
	    int node_id=cursor.node_id((int)candidate.x(),(int)candidate.y(),
				   (int)candidate.z());
	   
	    if((node_id!=-1)&&(cursor.check_triplet((int)candidate.x(),(int)candidate.y(),
						    (int)candidate.z())))
	    {
		
		std::pair<float,float> temp;
		const GsTLGridProperty *prop1=tempGrid->selected_property();
                temp.first=prop1->get_value(j);
		const GsTLGridProperty *prop2=tempGrid->property(temp_prop_name);
		if (prop2->is_informed(node_id))
		{
		    temp.second=prop2->get_value(node_id);
		    table[i].push_back(temp);
		}
		
	
       	    }
	}
    }
 
    
    GsTLInt nx=tempGrid->nx();
    GsTLInt ny=tempGrid->ny();
    GsTLInt nz=tempGrid->nz();
    std::vector<double> x_values;
    std::vector<double> y_values;
    float temp;
    for(i=0;i<static_cast<signed>(table.size());i++)
    {
	temp=compute_single_value(table[i],model_type,mod_param);
	
	if((prim_var_*sec_var_)!=0)
	    y_values.push_back(temp/sqrt(prim_var_*sec_var_));
	else  y_values.push_back(temp);
	
        //y_values.push_back(temp/var);
	x_values.push_back( (i+1) * sqrt( double( v.x()*v.x() + v.y()*v.y() + v.z()*v.z() ) ) );
    }

    //Set y_values and x_values of discrete function object 
    f.set_y_values(y_values);
    f.set_x_values(x_values);

    std::vector<int> num_pairs;
    for(int j2=0;j2<static_cast<signed>(table.size());j2++)
    	num_pairs.push_back(table[j2].size());
    

    grid_->remove_property(temp_prop_name);
    return num_pairs;

}





float Cross_variog_computer:: compute_variance()
{
    const RGrid *tempGrid=dynamic_cast<const RGrid*>(grid_);
    const GsTLGridProperty *prop=tempGrid->selected_property();

    
    
    float square_sum=std::inner_product(prop->begin(),prop->end(),
					prop->begin(),0.0);
    
    float sum=std::accumulate(prop->begin(),prop->end(),
					0.0);
    
    int n=0;
#if defined(_MSC_VER) && _MSC_VER <= 1300   // 1300 == VC++ 7.0
    std::distance(prop->begin(),prop->end(), n);
#else
    n = std::distance(prop->begin(),prop->end());
#endif

    return((square_sum/n)-(sum/n)*(sum/n));
}





float Cross_variog_computer:: compute_single_value(
    const std::vector< std::pair<float,float> >& candidates, 
    std::string&  model_type,double mod_param ) 

{

    
    std::map<std::string,MeasureType>::iterator it=
    	models_.find(model_type);
    float yval=0;
    double m1=0,m2=0,temp1=0,temp2=0,temp3=0;
    
    if(it!=models_.end())
       {
	   int type=(*it).second;
	   switch(type)
	   {
	   case(0):
       {
	       for(int j=0;j<static_cast<signed>(candidates.size());j++)
	       {
		       double prop1=candidates[j].first;
    		   double prop2=candidates[j].second;
		       yval=yval+(prop1-prop2)*(prop1-prop2);
	       }
	       yval=yval/(2*(candidates.size()));
	   
	       break;
       }

	   case(1):
       {
	       m1=0,m2=0;
	       for(int i=0;i<static_cast<signed>(candidates.size());i++)
	       {
		       m1 = m1 + candidates[i].first;
		       m2 = m2 + candidates[i].second;
	       }
	       m1=m1/candidates.size();
	       m2=m2/candidates.size();
	       double temp = 0;
	       for(int j2=0;j2<static_cast<signed>(candidates.size());j2++)
	       {
    		   double prop1=candidates[j2].first;
		       double prop2=candidates[j2].second;
		       temp=temp+(prop1*prop2);
	       }
	       temp=temp/candidates.size();
	       yval=temp-(m1*m2);
	       break;
       }

	   case(2):
       {
	       m1=0,m2=0;
	       for(int i2=0;i2<static_cast<signed>(candidates.size());i2++)
	       {
		      m1 = m1 + candidates[i2].first;
		      m2 = m2 + candidates[i2].second;
	       }
	       m1=m1/candidates.size();
	       m2=m2/candidates.size();
	       for(int j1=0;j1<static_cast<signed>(candidates.size());j1++)
	       {
		   temp1=temp1+(candidates[j1].first*candidates[j1].second);
		   temp2=temp2+(candidates[j1].first*candidates[j1].first);
		   temp3=temp3+(candidates[j1].second*candidates[j1].second);
	       }
	       temp1=temp1/candidates.size();
	       temp1=temp1-(m1*m2);

	       temp2=temp2/candidates.size();
	       temp2=temp2-m1*m1;
	       
	       temp3=temp3/candidates.size();
	       temp3=temp3-m2*m2;

	       yval=temp1/sqrt(temp2*temp3);
	       break;
       }

	   case(3):
       {
	       m1=0,m2=0;
	       for(int i3=0;i3<static_cast<signed>(candidates.size());i3++)
	       {
		   m1 = m1 + candidates[i3].first;
		   m2 = m2 + candidates[i3].second;
		   double prop1=candidates[i3].first;
		   double prop2=candidates[i3].second;
		   yval=yval+(prop1-prop2)*(prop1-prop2);
	       }
	       m1=m1/candidates.size();
	       m2=m2/candidates.size();
	       yval=yval/(2*(candidates.size()));
	       yval=yval/(((m1+m2)/2)*((m1+m2)/2));
	       break;
       }

	   case(4):
       {
	       for(int i4=0;i4<static_cast<signed>(candidates.size());i4++)
		   yval=yval+(log(candidates[i4].first)-log(candidates[i4].second))*
		             (log(candidates[i4].first)-log(candidates[i4].second));
	       yval=yval/(2*(candidates.size()));
	       break;
       }
	       
	   case(5):
       {
	       for(int i5=0;i5<static_cast<signed>(candidates.size());i5++)
		   yval = yval + fabs( candidates[i5].first - candidates[i5].second );
	       yval=yval/(2*(candidates.size()));
	       break;
       }

	   case(6):
       {  
	       for(int j6=0;j6<static_cast<signed>(candidates.size());j6++)
	       {
		   double prop1;
		   double prop2;
		   
		   if(candidates[j6].first < mod_param)
			   prop1 =1;
		   else prop1=0;

		   if(candidates[j6].second < mod_param)
			   prop2 =1;
		   else prop2=0;
		   
		   yval=yval+(prop1-prop2)*(prop1-prop2);
	       }
	       yval=yval/(2*(candidates.size()));
	   
	       break;
       }   

	       
	   }
	   return yval;   
       }
       return -1;
}



	
	
