/*
Nehme Bilal
TNT new class:
Coord3 is an Array1D with 3 component (x, y, z)
*/

#include <tnt_array1d.h>

#ifndef _TNT_TUPLE3_H
#define _TNT_TUPLE3_H



namespace TNT
{

template <class T>
class Coord3 : public Array1D<T>
{
	Coord3();
	Coord3(T x, T y, T z);
	Coord3(const T& v);
	Coord3(const T* v);
	Coord3(const Coord3& v);
	//virtual ~Coord3();

	// get the x coordinate
  inline T x () const;

	// set the x coordinate
	inline void setX(T x);

	// get the y coordinate
  inline T y () const;

	// set the y coordinate
	inline void setY(T y);

	// get the z coordinate
  inline T z () const;

	// set the z coordinate
	inline void setZ(T Z);


  inline bool operator== (const Coord3& v) const;
  inline bool operator!= (const Coord3& v) const;

  inline Coord3 operator+ (const Coord3& v) const;
  inline Coord3 operator- (const Coord3& v) const;
  inline Coord3& operator+= (const Coord3& v);
  inline Coord3& operator-= (const Coord3& v);
  inline Coord3 operator* (T f) const;
  inline Coord3 operator/ (T f) const;
  inline Coord3& operator*= (T f);
  inline Coord3& operator/= (T f);
};



// Implementation

//------------------------------------------------
template <class T>
Coord3<T>::Coord3() : Array1D(3){}

//------------------------------------------------
template <class T>
Coord3<T>::Coord3(T x, T y, T z) : Array1D(3)
{
	this->data_[0] = x;
	this->data_[1] = y;
	this->data_[2] = z;
}

//------------------------------------------------
template <class T>
Coord3<T>::Coord3(const T& v) : Array1D(3, v){}

//------------------------------------------------
template <class T>
Coord3<T>::Coord3(const T* v) : Array1D(3, v){}

//------------------------------------------------
template <class T>
Coord3<T>::Coord3(const Coord3& v) : Array1D(3, v.data_){}

//------------------------------------------------
template <class T>
inline T Coord3<T>::x() const
{
	return this->data_[0];
}

//------------------------------------------------
template <class T>
inline T Coord3<T>::y() const
{
	return this->data_[1];
}

//------------------------------------------------
template <class T>
inline T Coord3<T>::z() const
{
	return this->data_[2];
}

//------------------------------------------------
template <class T>
inline void Coord3<T>::setX(T x)
{
	this->data_[0] = x;
}

//------------------------------------------------
template <class T>
inline void Coord3<T>::setY(T y)
{
	this->data_[1] = y;
}

//------------------------------------------------
template <class T>
inline void Coord3<T>::setZ(T Z)
{
	this->data_[3] = z;
}



//------------------------------------------------
template <class T>
inline bool Coord3<T>::operator ==(const TNT::Coord3<T> &v) const
{
	return 
		(this->data_[0] == v.data_[0] && 
		 this->data_[1] == v.data_[1] &&
	 	 this->data_[2] == v.data_[2]);
}

//------------------------------------------------
template <class T>
inline bool Coord3<T>::operator !=(const TNT::Coord3<T> &v) const
{
	return !( (*this) == v);
}

//------------------------------------------------
template <class T>
inline Coord3<T> Coord3<T>::operator +(const Coord3& v) const
{
	return Coord3(
		this->data_[0] + v.data_[0],
		this->data_[1] + v.data_[1],
		this->data_[2] + v.data_[2]);
}

//------------------------------------------------
template <class T>
inline Coord3<T> Coord3<T>::operator -(const Coord3& v) const
{
	return Coord3(
		this->data_[0] - v.data_[0],
		this->data_[1] - v.data_[1],
		this->data_[2] - v.data_[2]);
}

//------------------------------------------------
template <class T>
inline Coord3<T>& Coord3<T>::operator +=(const Coord3& v)
{
	this->data_[0] += v.data_[0];
	this->data_[1] += v.data_[1];
	this->data_[2] += v.data_[2];

	return (*this);
}

//------------------------------------------------
template <class T>
inline Coord3<T>& Coord3<T>::operator -=(const Coord3& v)
{
	this->data_[0] -= v.data_[0];
	this->data_[1] -= v.data_[1];
	this->data_[2] -= v.data_[2];

	return (*this);
}

//------------------------------------------------
template <class T>
inline Coord3<T> Coord3<T>::operator *(T f) const
{
	return Coord3(
		this->data_[0]*f,
		this->data_[1]*f,
		this->data_[2]*f);
}

//------------------------------------------------
template <class T>
inline Coord3<T> Coord3<T>::operator /(T f) const
{
	if(f == 0.0)
	{
		throw("Error: Division by zero");
	}
	return Coord3(
		this->data_[0]/f,
		this->data_[1]/f,
		this->data_[2]/f);
}

//------------------------------------------------
template <class T>
inline Coord3<T>& Coord3<T>::operator *=(T f) 
{
	this->data_[0] *= f;
	this->data_[1] *= f;
	this->data_[2] *= f;

	return (*this);
}

//------------------------------------------------
template <class T>
inline Coord3<T>& Coord3<T>::operator /=(T f) 
{
	if(f == 0.0)
	{
		throw("Error: Division by zero");
	}

	this->data_[0] /= f;
	this->data_[1] /= f;
	this->data_[2] /= f;

	return (*this);
}

}

#endif