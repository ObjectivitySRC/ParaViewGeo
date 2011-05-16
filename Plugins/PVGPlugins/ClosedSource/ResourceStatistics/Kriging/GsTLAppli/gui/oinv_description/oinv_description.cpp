/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "gui" module of the Geostatistical Earth
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

#include <GsTLAppli/gui/oinv_description/oinv_description.h>
#include <GsTLAppli/gui/oinv_description/gstl_SoNode.h>
#include <GsTLAppli/gui/utils/colorscale.h>
#include <GsTLAppli/gui/utils/colormap.h>
#include <GsTLAppli/appli/manager_repository.h>
#include <GsTLAppli/grid/grid_model/geostat_grid.h>
#include <GsTLAppli/grid/grid_model/grid_property.h>

#include <algorithm>
#include <qstring.h>


//TL modified
bool Oinv_description::update_desc(QString & olds, QString & news)
{
  
	CmapMap::iterator found = cmap_map_.find( olds.latin1() );
	if (found == cmap_map_.end())
		return false;

	std::pair<Bounds_pair, Colormap*> p = found->second;
	cmap_map_.erase(found);
	cmap_map_[news.latin1()] = p;
}

Oinv_description::Oinv_description()
  : geostat_grid_( 0 ),
    current_property_( 0 ),
    cmap_( 0 ) {

  oinv_node_ = new GsTL_SoNode;
  oinv_node_->ref();
  property_display_mode_ = Oinv::NOT_PAINTED;
}


Oinv_description::~Oinv_description() {
  oinv_node_->unref();

//  if( cmap_ ) 
//    delete cmap_;
  
  // delete the colormaps
  for( CmapMap::iterator it = cmap_map_.begin(); it != cmap_map_.end(); ++it )
    delete it->second.second;
}


void Oinv_description::set_colormap( const std::string& map_name ) {
  if( current_property_name_.empty() || !current_property_ ) return;
  
  SmartPtr<Named_interface> ni = 
    Root::instance()->interface( colormap_manager + "/" + map_name );
  Color_scale* colors = dynamic_cast<Color_scale*>( ni.raw_ptr() );
  appli_assert( colors );


  CmapMap::iterator found = cmap_map_.find( current_property_name_ );
  
  if( found == cmap_map_.end() ) {
    /* There is no colormap for the current property:
     *   - we compute the min and max of the current property
     *   - we define a new colormap and set its bounds to the min and
     *     max of the property
     *   - we insert all that in the Cmap_map
     */
    Bounds_pair min_max( -9999, -9999 );
    compute_min_max( min_max, current_property_ );
    Colormap* cmap = new Colormap( colors );
    cmap->set_bounds( min_max.first, min_max.second );
    cmap_ = cmap;
    cmap_map_[ current_property_name_ ] = std::make_pair( min_max, cmap );
    
  }
  else {
    /* There is already an entry for the current property.
     * Check that there is an actual colormap (if not, create one using 
     * the already computed min max values). Just change the colorscale
     */
    Colormap* cmap = found->second.second;
    if( cmap ) {
      cmap->color_scale( colors );
      appli_assert( cmap_ == cmap );
    }
    else {
      found->second.second = new Colormap( colors );
      Bounds_pair min_max = found->second.first;
      found->second.second->set_bounds( min_max.first, min_max.second );
      cmap_ = found->second.second;
    }
  }

  refresh();
}

 
 
void Oinv_description::set_colormap( const Colormap& cmap ) {
  
  if( current_property_name_.empty() || !current_property_ ) return;
  
  CmapMap::iterator found = cmap_map_.find( current_property_name_ );
  
  if( found == cmap_map_.end() ) {
    /* There is no colormap for the current property:
     *   - we compute the min and max of the current property
     *   - we insert those values and the new colormap in the Cmap_map
     */
    Bounds_pair min_max( -9999, -9999 );
    compute_min_max( min_max, current_property_ );
    Colormap* new_cmap = new Colormap( cmap );
    cmap_ = new_cmap;
    cmap_map_[ current_property_name_ ] = std::make_pair( min_max, new_cmap );

  }
  else {
    /* There is already an entry for the current property.
     */
    delete found->second.second;
    found->second.second = new Colormap( cmap );
    cmap_ = found->second.second;
  }

  refresh();

}



void Oinv_description::reset_colormap_bounds() {
  if( !cmap_ ) return;
 
  CmapMap::iterator found = cmap_map_.find( current_property_name_ );
  Bounds_pair min_max = found->second.first;
  appli_assert( found->second.second );
  found->second.second->set_bounds( min_max.first, min_max.second );

  refresh();
}




void Oinv_description::set_property( const std::string& property_name ) {
  if( !geostat_grid_->property( property_name ) ) {
    appli_warning( "grid has no property called " << property_name );
    return;
  }

  if( current_property_name_ == property_name ) return;
  
  current_property_ = geostat_grid_->property( property_name );
  current_property_name_ = property_name;

  CmapMap::iterator found = cmap_map_.find( current_property_name_ );
  
  if( found == cmap_map_.end() ) {
    /* There is no entry for property_name
     *   - we compute the min and max of the current property
     *   - we insert those values in the Cmap_map. The colormap inserted is 
     *     a default colormap (or 0 if no default colormap could be created)
     */
    Bounds_pair min_max( -9999, -9999 );
    compute_min_max( min_max, current_property_ );

    // Set up a default colormap for the property (eg "rainbow") 
    Colormap* default_cmap = 0;
    SmartPtr<Named_interface> ni = 
      Root::instance()->interface( colormap_manager + "/rainbow" );
    Color_scale* colors = dynamic_cast<Color_scale*>( ni.raw_ptr() );
    if( colors ) {
      default_cmap = new Colormap( colors, min_max.first, min_max.second );
    }
    cmap_map_[ current_property_name_ ] = std::make_pair( min_max, default_cmap );
    cmap_ = default_cmap;
  }
  else {
    /* There is an entry for property_name
     *   - set cmap_ to the colormap in the cmap_map.
     *   - refresh if cmap_ != 0
     */
    cmap_ = found->second.second;
  }
  

  if( cmap_ )
      refresh();
}
  



const Colormap*
Oinv_description::colormap( const std::string& prop_name ) {

  CmapMap::iterator found = cmap_map_.find( prop_name );
  
  if( found == cmap_map_.end() ) {
    /* There is no entry for property_name
     *   - we compute the min and max of the current property
     *   - we insert those values in the Cmap_map. The colormap inserted is 
     *     a default colormap (or 0 if no default colormap could be created)
     */
    const GsTLGridProperty* property = geostat_grid_->property( prop_name );
    if( !property ) return 0;
  
    Bounds_pair min_max( -9999, -9999 );
    compute_min_max( min_max, property );

    // Set up a default colormap for the property (eg "rainbow") 
    Colormap* default_cmap = 0;
    SmartPtr<Named_interface> ni = 
      Root::instance()->interface( colormap_manager + "/rainbow" );
    Color_scale* colors = dynamic_cast<Color_scale*>( ni.raw_ptr() );
    if( colors ) {
      default_cmap = new Colormap( colors, min_max.first, min_max.second );
    }
    cmap_map_[ prop_name ] = std::make_pair( min_max, default_cmap );
    
    return default_cmap;
  }
  else {
    return found->second.second;
  }
}






bool Oinv_description::compute_min_max( std::pair<float, float>& min_max, 
					const GsTLGridProperty* property ) {
  int start=0;
  while( !property->is_informed( start ) ) {
    start++;
    // If we reach the end of the property 
    if( start >= property->size() ) return false;
  }
    
  float min = property->get_value( start );
  float max = property->get_value( start );
  for( int i=start+1; i < property->size() ; i++ ) {
    if( !property->is_informed(i) ) continue;
      
    float val = property->get_value( i );
    min = std::min( min, val );
    max = std::max( max, val );
  }

  min_max.first = min;
  min_max.second = max;
  
  return true;
}




void Oinv_description::update( Oinv::Update_reason reason ,
                              // const std::string& property_name ) {
                               const std::vector<std::string>* properties ) {

  switch( reason ) { 

  case Oinv::PROPERTY_DELETED:
    if( !properties ) {
      // We don't know which property was removed, so we have to check them all
      appli_assert( geostat_grid_ );
      std::list<std::string> property_list = geostat_grid_->property_list();
      property_list.sort();

      for( CmapMap::iterator it = cmap_map_.begin(); it != cmap_map_.end(); ++it ) {
        bool there = std::binary_search( property_list.begin(), property_list.end(),
				                                 it->first );
      	if( !there ) {
	        property_deleted( it->first );
	      }	
      }
    }      
    else {
      // we know which properties were removed
      std::vector<std::string>::const_iterator it;
      for( it = properties->begin() ; it != properties->end() ; ++it )
        property_deleted( *it );
    }

//    refresh();
    break;
    

  case Oinv::PROPERTY_CHANGED:
    if( !properties ) {
      // we don't know which property was changed, so we update all properties 
      for( CmapMap::iterator it = cmap_map_.begin(); it != cmap_map_.end(); ++it ) {
      	compute_min_max( it->second.first, geostat_grid_->property( it->first ) );
      }
    }
    else {
      std::vector<std::string>::const_iterator prop_it = properties->begin();
      for( ; prop_it != properties->end() ; ++prop_it ) {
        CmapMap::iterator it = cmap_map_.find( *prop_it );
      
        if( it != cmap_map_.end() ) {
        	compute_min_max( it->second.first, geostat_grid_->property( it->first ) );
	        Bounds_pair min_max = it->second.first;
	        it->second.second->set_bounds( min_max.first, min_max.second );
	
//      	  refresh();
        }
      }
    }
    break;
    
  }

  refresh();
}
