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

#include <GsTLAppli/grid/common.h>
#include <GsTLAppli/grid/grid_model/gval_iterator.h> 
#include <GsTLAppli/grid/grid_model/rgrid_gval_accessor.h> 
#include <GsTLAppli/grid/grid_model/grid_template.h> 
#include <GsTLAppli/grid/grid_model/sgrid_cursor.h> 
#include <GsTLAppli/utils/gstl_types.h> 
 
 
 
//================================================= 
// Implementation for GsTLRgrid 
 
class GRID_DECL LinearMapIndex { 
public: 
    LinearMapIndex(){} 
    ~LinearMapIndex(){} 
    GsTLInt map( GsTLInt i) { 
        return i; 
    } 
}; 
 
/* Should be very careful when using this class. 
 * vec_ is a vector*, not a vector. The vector vec_ points to 
 * is not owned by TabularMapIndex, and if it disappears before 
 * TabularMapIndex, we have a big problem.  
 *  
 */ 
class GRID_DECL TabularMapIndex { 
public: 
    TabularMapIndex() : vec_(0) {} 
    TabularMapIndex(const std::vector<GsTLInt>* vec) : vec_(vec){} 
    TabularMapIndex(const TabularMapIndex& t ) : vec_(t.vec_){} 
    ~TabularMapIndex() {} 
    GsTLInt map( GsTLInt i) { 
        return (*vec_)[i]; 
    } 
private: 
    const std::vector<GsTLInt>* vec_; 
}; 
 
 
 
 
template <class GsTLGridInfoAccessor, class MapIndex> 
class GRID_DECL GsTLGridIterator : public Gval_iterator_impl { 
   
 public: 
   
  typedef typename GsTLGridInfoAccessor::geovalue_type  GsTLNodeInfo; 
 
    GsTLGridIterator() : acount_(0) { 
    } 
     
    GsTLGridIterator( 
        const GsTLGridIterator& it 
    ) : Gval_iterator_impl(), 
        acount_(it.acount_), 
        grid_cursor_(it.grid_cursor_), 
        accessor_(it.accessor_), 
        map_index_(it.map_index_) { 
        grid_cursor_.index(rindex()); 
        check_bounds(); 
    } 
     
    GsTLGridIterator( 
        GsTLInt start, 
        const GsTLGridCursor& gc, 
        const GsTLGridInfoAccessor& accessor, 
        MapIndex midx 
    ) : Gval_iterator_impl(), 
        acount_(start), 
        grid_cursor_(gc), 
        accessor_(accessor), 
        map_index_(midx) { 
        check_bounds(); 
        grid_cursor_.index(rindex()); 
    } 
     
    virtual ~GsTLGridIterator() { 
    } 
    
    GsTLGridIterator & operator = ( const GsTLGridIterator& it ) { 
        if( this != &it ) { 
            acount_ = it.acount_; 
            grid_cursor_ = it.grid_cursor_; 
            accessor_ = it.accessor_; 
            map_index_ = it.map_index_; 
            check_bounds(); 
        } 
        return *this; 
    } 
     
    virtual Gval_iterator_impl* clone() const { 
      return new GsTLGridIterator(*this); 
    } 
 
    virtual GsTLNodeInfo& operator * () { 
        return(*accessor_.node()); 
    } 
     
    virtual GsTLNodeInfo* operator -> () { 
        return(accessor_.node()); 
    } 
     
    virtual GsTLGridIterator& operator ++ () { 
        acount_++; 
        if (acount_ < grid_cursor_.max_index()) { 
            grid_cursor_.index(rindex()); 
            accessor_.set_cursor(grid_cursor_); 
        } 
        return *this; 
    } 
     
    virtual GsTLGridIterator& operator ++ ( int ) { 
        acount_++; 
        if (acount_ < grid_cursor_.max_index()) { 
            grid_cursor_.index(rindex()); 
            accessor_.set_cursor(grid_cursor_); 
        } 
        return *this; 
    } 
 
 
    virtual int operator - ( const Gval_iterator_impl& it) const { 
      const GsTLGridIterator* it_ptr = dynamic_cast<const GsTLGridIterator*>(&it); 
      appli_assert( it_ptr ); 
        return (acount_ - it_ptr->acount_); 
    } 
     
    virtual bool operator != ( const Gval_iterator_impl& it ) const { 
      const GsTLGridIterator* it_ptr = dynamic_cast<const GsTLGridIterator*>(&it); 
      appli_assert( it_ptr ); 
      return (acount_ != it_ptr->acount_); 
    } 
     
    virtual bool operator == ( const Gval_iterator_impl& it ) const { 
      const GsTLGridIterator* it_ptr = dynamic_cast<const GsTLGridIterator*>(&it); 
      appli_assert( it_ptr ); 
      return (acount_ == it_ptr->acount_); 
    } 
     
protected: 
    GsTLInt rindex() { 
        return map_index_.map(acount_); 
    } 
    void check_bounds() { 
        if (acount_ < 0) acount_ = 0; 
        if (acount_ >= grid_cursor_.max_index() ) acount_ = grid_cursor_.max_index() ; 
    } 
protected: 
    GsTLInt acount_; 
    GsTLGridCursor grid_cursor_; 
    GsTLGridInfoAccessor accessor_; 
    MapIndex map_index_; 
}; 
