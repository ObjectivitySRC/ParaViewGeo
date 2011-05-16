/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "math" module of the Geostatistical Earth
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

#include <GsTLAppli/math/correlation_measure_computer.h>

#include <cmath>

const double Correlation_measure_computer::NaN = -9999;

Correlation_measure_computer::Correlation_measure_computer() {
  model_map_["semivariogram"]=semivariogram;
  model_map_["covariance"]=covariance;
  model_map_["correlogram"]=correlogram;
  model_map_["general_relative_semivariogram"]=general_relative_semivariogram;
  model_map_["semimadogram"]=semimadogram;
  model_map_["logarithmic_semivariogram"]=logarithmic_semivariogram;
  model_map_["indicator"]=indicator;
}

std::vector<double> Correlation_measure_computer::
operator() ( const PairMatrix& matrix, 
             const std::string& measure_type, double param ) {

  std::vector<double> result( matrix.size(), 0.0 );

  std::map<std::string, MeasureType>::iterator it = 
    model_map_.find( measure_type );    
  if( it == model_map_.end() ) return result;
       
  MeasureType type=(*it).second;

  switch( type ) {
    case semivariogram :
      compute_variogram( result, matrix );
      break; 

	  case( covariance ):
      compute_covariance( result, matrix ); 
      break;

	  case( correlogram ):
      compute_correlogram( result, matrix );
      break;

	   case( general_relative_semivariogram ):
	     compute_general_variogram( result, matrix );
       break;

	   case( logarithmic_semivariogram ):
	     compute_log_variogram( result, matrix );
       break;
      
	   case( semimadogram ):
      compute_madogram( result, matrix );
	    break;

     case( indicator ):
	     compute_indicator_variogram( result, matrix, param );  
       break;
	   

  }
  return result;
}



void Correlation_measure_computer::
compute_variogram( std::vector<double>& result,
                  const PairMatrix& matrix ) {
  for( unsigned int i = 0 ; i < matrix.size() ; i++ ) {
    double res = 0;
    const std::vector< std::pair<float,float> >& candidates = matrix[i];

    if( candidates.empty() ) {
      result[i] = Correlation_measure_computer::NaN;
      continue;
    }

    for( unsigned int j=0 ; j < candidates.size() ; j++) {
      double prop1 = candidates[j].first;
      double prop2 = candidates[j].second;
      res += (prop1-prop2)*(prop1-prop2);
    }
    res /= 2 * candidates.size();
    result[i] = res;
  }	   
}


void Correlation_measure_computer::
compute_covariance( std::vector<double>& result,
                    const PairMatrix& matrix ) {
  for( unsigned int k = 0 ; k < matrix.size() ; k++ ) {

    std::pair<float,float> means = this->mean( matrix[k] );
    double res = 0;
    const std::vector< std::pair<float,float> >& candidates = matrix[k];

    if( candidates.empty() ) {
      result[k] = Correlation_measure_computer::NaN;
      continue;
    }

    for( unsigned int j=0 ; j < candidates.size() ; j++) {
      double prop1 = candidates[j].first;
		  double prop2=candidates[j].second;
		  res += (prop1*prop2);
	  }
	  res /= double( candidates.size() );
	  res -= means.first * means.second;

    result[k] = res;
  }
}


void Correlation_measure_computer::
compute_correlogram( std::vector<double>& result,
                     const PairMatrix& matrix ) {
  for( unsigned int k = 0 ; k < matrix.size() ; k++ ) {

    std::pair<float,float> means = this->mean( matrix[k] );
    double res=0, temp1=0, temp2=0, temp3=0;
    const std::vector< std::pair<float,float> >& candidates = matrix[k];

    if( candidates.empty() ) {
      result[k] = Correlation_measure_computer::NaN;
      continue;
    }    

    for(int j2=0 ; j2 < static_cast<signed>(candidates.size()) ; j2++) {
      temp1=temp1+(candidates[j2].first*candidates[j2].second);
		  temp2=temp2+(candidates[j2].first*candidates[j2].first);
		  temp3=temp3+(candidates[j2].second*candidates[j2].second);
	  }

    double means_prod = means.first * means.second;
	  temp1 /= float( candidates.size() );
	  temp1 -= means_prod;

	  temp2 /= float( candidates.size() );
	  temp2 -= means_prod;
	       
	  temp3 /= float( candidates.size() );
	  temp3 -= means.second * means.second ;

    res = temp1 / std::sqrt(temp2*temp3);
    result[k] = res;
  }
}


void Correlation_measure_computer::
compute_general_variogram( std::vector<double>& result,
                           const PairMatrix& matrix ) {

  for( unsigned int k = 0 ; k < matrix.size() ; k++ ) {
    const std::vector< std::pair<float,float> >& candidates = matrix[k];

    if( candidates.empty() ) {
      result[k] = Correlation_measure_computer::NaN;
      continue;
    }

    double yval=0, m1=0, m2=0;
    for(int i3=0 ; i3 < static_cast<signed>(candidates.size()) ; i3++) {
      m1 = m1 + candidates[i3].first;
	    m2 = m2 + candidates[i3].second;
		  double prop1=candidates[i3].first;
		  double prop2=candidates[i3].second;
		  yval += (prop1-prop2)*(prop1-prop2);
	  }
	  m1 /= double( candidates.size() );
	  m2 /= double( candidates.size() );
    yval /= 2 * double( candidates.size() );
	  yval /= (m1+m2)/2 * (m1+m2)/2;

    result[k] = yval;
  }
}


void Correlation_measure_computer::
compute_log_variogram( std::vector<double>& result,
                       const PairMatrix& matrix ) {
  
  for( unsigned int k = 0 ; k < matrix.size() ; k++ ) {
    const std::vector< std::pair<float,float> >& candidates = matrix[k];

    if( candidates.empty() ) {
      result[k] = Correlation_measure_computer::NaN;
      continue;
    }
    
    double yval=0;
    for( int i=0 ; i < static_cast<signed>(candidates.size()) ; i++ ) {
		  yval += (log(candidates[i].first)-log(candidates[i].second))*
		          (log(candidates[i].first)-log(candidates[i].second));
	    yval /= 2.0 * double(candidates.size());
    }
    
    result[k] = yval;
  }
}


void Correlation_measure_computer::
compute_madogram( std::vector<double>& result,
                  const PairMatrix& matrix ) {
  for( unsigned int k = 0 ; k < matrix.size() ; k++ ) {
    const std::vector< std::pair<float,float> >& candidates = matrix[k];

    if( candidates.empty() ) {
      result[k] = Correlation_measure_computer::NaN;
      continue;
    }
    
    double yval=0;
    for(int i=0 ; i < static_cast<signed>(candidates.size()) ; i++) {
		  yval=yval+fabs(candidates[i].first - candidates[i].second);
	    yval /= 2*double(candidates.size());
    }

    result[k] = yval;
  }
}


void Correlation_measure_computer::
compute_indicator_variogram( std::vector<double>& result,
                             const PairMatrix& matrix,
                             double param ) {
  for( unsigned int k = 0 ; k < matrix.size() ; k++ ) {
    const std::vector< std::pair<float,float> >& candidates = matrix[k];
    double yval=0;
	       
    if( candidates.empty() ) {
      result[k] = Correlation_measure_computer::NaN;
      continue;
    }

    for( int j6=0 ; j6 < static_cast<signed>(candidates.size()) ; j6++) {
		  double prop1;
		  double prop2;
		   
		  if(candidates[j6].first <= param)
		    prop1 =1;
		  else 
        prop1=0;

		  if(candidates[j6].second <= param)
			  prop2 =1;
		  else
        prop2=0;
		   
		  yval=yval+(prop1-prop2)*(prop1-prop2);
    }
	          
    yval /= 2.0 * double(candidates.size());
    result[k] = yval;  
  }
}




std::pair<float,float> Correlation_measure_computer::
mean( const std::vector< std::pair<float,float> >& array ) {
  float mean1 = 0, mean2 = 0;
  for( unsigned int i = 0; i < array.size() ; i++ ) {
    mean1 += array[i].first;
    mean2 += array[i].second;
  }
  mean1 /= float( array.size() );
  mean2 /= float( array.size() );

  return std::make_pair( mean1, mean2 );
}







double Correlation_measure_computer::
operator() ( const PairVector& pairs, 
             const std::string& measure_type, double param ) {

  std::map<std::string, MeasureType>::iterator it = 
    model_map_.find( measure_type );    
  if( it == model_map_.end() ) return 0;
       
  MeasureType type=(*it).second;

  switch( type ) {
    case semivariogram :
      return compute_variogram( pairs );
      break; 

	  case( covariance ):
      return compute_covariance( pairs ); 
      break;

	  case( correlogram ):
      return compute_correlogram( pairs );
      break;

	   case( general_relative_semivariogram ):
	     return compute_general_variogram( pairs );
       break;

	   case( logarithmic_semivariogram ):
	     return compute_log_variogram( pairs );
       break;
      
	   case( semimadogram ):
      return compute_madogram( pairs );
	    break;

     case( indicator ):
	     return compute_indicator_variogram( pairs, param );  
       break;
	   

  }
  return 0;
}



double Correlation_measure_computer::
compute_variogram( const PairVector& pairs ) {
    if( pairs.empty() ) {
      return Correlation_measure_computer::NaN;
    }

    double res = 0;
    for( unsigned int j=0 ; j < pairs.size() ; j++) {
      double prop1 = pairs[j].first;
      double prop2 = pairs[j].second;
      res += (prop1-prop2)*(prop1-prop2);
    }

    res /= 2 * pairs.size();
    return res;	   
}


double Correlation_measure_computer::
compute_covariance( const PairVector& pairs ) {
    std::pair<float,float> means = this->mean( pairs );
    double res = 0;

    if( pairs.empty() ) {
      return Correlation_measure_computer::NaN;
    }

    for( unsigned int j=0 ; j < pairs.size() ; j++) {
      double prop1 = pairs[j].first;
		  double prop2 = pairs[j].second;
		  res += (prop1*prop2);
	  }
	  res /= double( pairs.size() );
	  res -= means.first * means.second;

    return res;
}


double Correlation_measure_computer::
compute_correlogram( const PairVector& pairs ) {
    if( pairs.empty() ) {
      return Correlation_measure_computer::NaN;
    }    

    std::pair<float,float> means = this->mean( pairs );
    double res=0, temp1=0, temp2=0, temp3=0;

    for( unsigned int j2=0 ; j2 < pairs.size() ; j2++ ) {
      temp1 += pairs[j2].first * pairs[j2].second ;
		  temp2 += pairs[j2].first * pairs[j2].first;
		  temp3 += pairs[j2].second * pairs[j2].second;
	  }

    double means_prod = means.first * means.second;
	  temp1 /= float( pairs.size() );
	  temp1 -= means_prod;

	  temp2 /= float( pairs.size() );
	  temp2 -= means_prod;
	       
	  temp3 /= float( pairs.size() );
	  temp3 -= means.second * means.second ;

    res = temp1 / std::sqrt(temp2*temp3);
    return res;
}


double Correlation_measure_computer::
compute_general_variogram( const PairVector& pairs ) {
  if( pairs.empty() ) {
    return Correlation_measure_computer::NaN;
  }    

  double yval=0, m1=0, m2=0;
  for( unsigned int i3=0 ; i3 < pairs.size() ; i3++) {
      m1 = m1 + pairs[i3].first;
	    m2 = m2 + pairs[i3].second;
		  double prop1=pairs[i3].first;
		  double prop2=pairs[i3].second;
		  yval += (prop1-prop2)*(prop1-prop2);
	  }
	  m1 /= double( pairs.size() );
	  m2 /= double( pairs.size() );
    yval /= 2 * double( pairs.size() );
	  yval /= (m1+m2)/2 * (m1+m2)/2;

    return yval;
}


double Correlation_measure_computer::
compute_log_variogram( const PairVector& pairs ) {
  if( pairs.empty() ) {
    return Correlation_measure_computer::NaN;
  }    
    
  double yval=0;
  for( unsigned int i=0 ; i < pairs.size() ; i++ ) {
    yval += (log(pairs[i].first)-log(pairs[i].second))*
		          (log(pairs[i].first)-log(pairs[i].second));
	    yval /= 2.0 * double(pairs.size());
    }
    
  return yval;
}


double Correlation_measure_computer::
compute_madogram( const PairVector& pairs ) {
  if( pairs.empty() ) {
    return Correlation_measure_computer::NaN;
  }    
    
  double yval=0;
  for( unsigned int i=0 ; i < pairs.size() ; i++) {
	  yval += fabs( pairs[i].first - pairs[i].second);
	  yval /= 2*double(pairs.size());
  }

  return yval;
}


double Correlation_measure_computer::
compute_indicator_variogram( const PairVector& pairs,
                             double param ) {
  if( pairs.empty() ) {
    return Correlation_measure_computer::NaN;
  }    

  double yval=0;
  for( unsigned int j=0 ; j < pairs.size() ; j++) {
		  double prop1;
		  double prop2;
		   
		  if(pairs[j].first <= param)
		    prop1 =1;
		  else 
        prop1=0;

		  if(pairs[j].second <= param)
			  prop2 =1;
		  else
        prop2=0;
		   
		  yval += (prop1-prop2)*(prop1-prop2);
    }
	          
    yval /= 2.0 * double(pairs.size());
    return  yval;  
}

