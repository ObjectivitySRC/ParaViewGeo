/*
Nehme Bilal
TNT new class:
Coord3 is an Array1D with 3 component (x, y, z)
*/

#include <tnt_tuple3.h>

#ifndef _TNT_VECT3_H
#define _TNT_VECT3_H


namespace TNT
{

template <class T>
class Vect3 : public Coord3<T>
{
	Vect3();
	Vect3(T x, T y, T z);
	Vect3(const T& v);
	Vect3(const T* v);
	Vect3(const Vect3& v);
	//virtual ~Vect3();


	// return the length of the vector
	inline T len() const;

	// return the squared length of a vector
	inline T lenSquare() const;

	// normalize the vector
	inline void normalize();

	// return the cross product with a vector v
	// without modifying the vector itself
	inline Vect3 cross(const Vect3& v) const;

	// return the dot product with a vector v
	inline T dot(const Vect3& v) const ;

};



// Implementation

//------------------------------------------------
template <class T>
Vect3<T>::Vect3() : Coord3(3){}

//------------------------------------------------
template <class T>
Vect3<T>::Vect3(T x, T y, T z) : Coord3(3)
{
	this->data_[0] = x;
	this->data_[1] = y;
	this->data_[2] = z;
}

//------------------------------------------------
template <class T>
Vect3<T>::Vect3(const T& v) : Coord3(3, v){}


//------------------------------------------------
template <class T>
Vect3<T>::Vect3(const T* v) : Coord3(3, v){}

//------------------------------------------------
template <class T>
Vect3<T>::Vect3(const Vect3& v) : Coord3(3, v.data_){}

//------------------------------------------------
template <class T>
inline T Vect3<T>::len() const
{
	return sqrt(this->lenSquare());
}

//------------------------------------------------
template <class T>
inline T Vect3<T>::lenSquare() const
{
	return (data_[0]*data_[0]*) +
				 (data_[1]*data_[1]*) +
				 (data_[2]*data_[2]*)
}

//------------------------------------------------
template <class T>
inline void Vect3<T>::normalize()
{
	T length = this->len();
	if(length == 0.0)
	{
		return
	}

	this->data_[0]/= length;
	this->data_[1]/= length;
	this->data_[2]/= length;
}


//------------------------------------------------
template <class T>
inline Vect3<T> Vect3<T>::cross(const Vect3& v) const
{
	return Vect3(
		(data_[1]*v.data_[2]) - (data_[2]*v.data_[1],)
		(data_[2]*v.data_[0]) - (data_[0]*v.data_[2],)
		(data_[0]*v.data_[1]) - (data_[1]*v.data_[0])  );
}

//------------------------------------------------
template <class T>
inline T Vect3<T>::dot(const Vect3& v) const
{
	return (
		(data_[0]*v.data_[0]) +
		(data_[1]*v.data_[1]) +
		(data_[2]*v.data_[2]) );
}


}
#endif