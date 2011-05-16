/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $ Stope.h $
  Authors:    Arolde VIDJINNAGNI
  MIRARCO, Laurentian University
  Date:   October 2009    
  Version:   0.1
=========================================================================*/


#ifndef __Stope_h
#define __Stope_h

#include "vtkStdString.h"
#include "vtkIdList.h"
class InternalStopeMap;

class Stope
{
public:
	Stope();
	Stope(const vtkIdType &Id, const vtkIdType &nBlocks);	
	~Stope();

	inline void SetStopeId( const vtkIdType & id ){ StopeId = id; };
	inline void SetNumberOfBlocks( const vtkIdType & nblocks )
							{ NumberOfBlocks = nblocks; };

	inline void SetFitness( const double & fi ){ Fitness = fi; };

	inline void SetWeight( const double & We ){ Weight = We; };

	inline void SetVolume( const double & Vol ){ Volume = Vol; };
		
	inline void SetBounds( const double bounds[6] ){	
							Bounds[0]=bounds[0]; Bounds[1]=bounds[1]; 
							Bounds[2]=bounds[2]; Bounds[3]=bounds[3];
							Bounds[4]=bounds[4]; Bounds[5]=bounds[5]; };


	inline vtkIdType GetStopeId()const { return StopeId; };

	inline vtkIdType GetNumberOfBlocks()const { 
														 return NumberOfBlocks; };
	inline double GetFitness()const { return Fitness; };

	inline double GetWeight()	const { return Weight; };

	inline double GetVolume()	const { return Volume; };

	inline void   GetBounds(double bounds[6])	const {
								bounds[0]=Bounds[0]; bounds[1]=Bounds[1];
								bounds[2]=Bounds[2]; bounds[3]=Bounds[3]; 
								bounds[4]=Bounds[4]; bounds[5]=Bounds[5];};

	vtkIdType GetBlockId(vtkIdType i);
	int AddBlock( vtkIdType id);
	int RemoveBlock( vtkIdType id);

protected:  	
	vtkIdType	StopeId;
	vtkIdType	NumberOfBlocks; //the number of blocks in the stope
	double		Fitness; //the fitness of the stope
	double		Weight;//the weight of he stope
	double		Volume;//the volume
 	double		Bounds[6];

private:
	vtkIdList* ListOfBlocks; 
};


/***********************StopeContainer****************************************/
class StopeContainer
{
public:
	 StopeContainer();
	~StopeContainer();

	void AddStope(Stope *stope);
	Stope* GetStope( const int Id);
	int RemoveBlock(vtkIdType StopeId, vtkIdType id);
	
private:	
	InternalStopeMap* map;
};

#endif 