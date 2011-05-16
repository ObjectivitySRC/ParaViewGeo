#include <GsTLAppli/utils/gstl_data_array.h>

GSTLDATA_ARRAY_IMPLEMENT(GsTLChar)
GSTLDATA_ARRAY_IMPLEMENT(GsTLUChar)
GSTLDATA_ARRAY_IMPLEMENT(GsTLBool)
GSTLDATA_ARRAY_IMPLEMENT(GsTLShort)
GSTLDATA_ARRAY_IMPLEMENT(GsTLInt)
GSTLDATA_ARRAY_IMPLEMENT(GsTLLong)
GSTLDATA_ARRAY_IMPLEMENT(GsTLUShort)
GSTLDATA_ARRAY_IMPLEMENT(GsTLUInt)
GSTLDATA_ARRAY_IMPLEMENT(GsTLULong)
GSTLDATA_ARRAY_IMPLEMENT(GsTLFloat)
GSTLDATA_ARRAY_IMPLEMENT(GsTLDouble)


template < class R >
GsTLDataArray<R>::const_iterator 
GsTLDataArray<R>::begin() {
  return DataArrayIterator<R>( this, 0 ) ;
}

template < class R >
GsTLDataArray<R>::const_iterator 
GsTLDataArray<R>::end() {
  return DataArrayIterator<R>( this, size_ ) ;
}



//=======================================

template < class R >
DataArrayIterator<R>::DataArrayIterator( GsTLDataArray<R>* array, GsTLInt pos ) 
  : array_(array), pos_(pos) {
}

template < class R >
DataArrayIterator<R>::DataArrayIterator( const DataArrayIterator<R>& rhs )
  : array_(rhs.array_), pos_(rhs.pos_) {
}

template < class R >
DataArrayIterator<R>& 
DataArrayIterator<R>::operator = ( const DataArrayIterator<R>& rhs ) {
  array_ = rhs.array_;
  pos_=rhs.pos_;
  return *this;
}

template < class R >
bool 
DataArrayIterator<R>::operator == ( const DataArrayIterator<R>& rhs ) {
  return ( pos_ == rhs.pos_ ) && ( array_ == rhs.array_ ); 
}

template < class R >
bool 
DataArrayIterator<R>::operator != ( const DataArrayIterator<R>& rhs ) {
  return ( pos_ != rhs.pos_ ) || ( array_ != rhs.array_ ); 
}
