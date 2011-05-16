////////////////////////////////////////////////////////////////////////////////
//
//  File:        hgrid.h
//  Created by:  Jerzy Majewski - jmajewsk@meil.pw.edu.pl
//  Modified by:
//    2005-04-19 JM: mbIsOutside flag for HGrdTri was not initialized;
//                   CheckBoundIntegr was creating flat or negative triangles
//
// Copyright notice:
//    Copyright (C) 2000-2003 Jerzy Majewski
//
//    This is free software. You can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public License
//    as published by the Free Software Foundation.
//
//    This software is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY. See the GNU Lesser General Public License
//    for more details.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __HGRID_H__
#define __HGRID_H__

#include "mgdecl.h"
#include "vect.h"
#include "hrect.h"

//#ifndef this->Parent->CellEnd()
//#define this->Parent->CellEnd() CollGCell().end()
//#endif
//#ifndef NULLPNTPTR
//#define NULLPNTPTR CollGPnt().end()
//#endif

class HGrid;

//////////////////////////////////////////////////////////////////////
// class HGrdPnt
//////////////////////////////////////////////////////////////////////
class HGrdPnt : public Vect2D
{
public:
  HGrdPnt()       {}
  HGrdPnt( const Vect2D& vec) : Vect2D( vec)  {}
  HGrdPnt( const MGFloat& dx, const MGFloat& dy) : Vect2D( dx, dy)  {}
  ~HGrdPnt()    {}

  const MGInt&  Index() const { return mInd;}
  MGInt&      rIndex()    { return mInd;}

protected:
  MGInt mInd;
};
//--------------------------------------------------------------------

typedef std::list<HGrdPnt*>    CollGPnt;
typedef CollGPnt::iterator  IterGPnt;

const MGInt NUM_TRI   = 3;

class HFroSeg;

//////////////////////////////////////////////////////////////////////
/// class HGrdTri
//////////////////////////////////////////////////////////////////////
class HGrdTri
{
public:

  typedef std::list<HGrdTri*>  CollGCell;
  typedef CollGCell::iterator IterGCell;

  HGrdTri( HGrid *parent );
  ~HGrdTri()    {}

  HGrid *Parent;
  const bool&   IsOutside() const { return mbIsOutside;}
  bool&     rIsOutside()    { return mbIsOutside;}

  const MGInt&  Cross() const   { return mCross;}
  MGInt&      rCross()      { return mCross;}

  IterGPnt  Node( const MGInt& i) const;
  IterGCell Cell( const MGInt& i) const;

  IterGPnt& rNode( const MGInt& i);
  IterGCell&  rCell( const MGInt& i);

  bool    IsInside( const Vect2D& vct);
  void    NullifyThis( HGrdTri *pcl);
  void    NullifyThis( HFroSeg *pseg);
  void    InvalidateNeighb();
  void    SetNeighbour( const IterGCell& itrcl);

  MGFloat   Area() const;
  Vect2D    Center();
  IterGCell NextCell( const Vect2D& vct);
  IterGCell   NextCell( HFroSeg *pseg, const IterGCell& iclprv );

  const Vect2D& CircCenter() const;
  bool      SetCircCenter();

  bool  IsInsideCirc( const Vect2D& vct);
  bool  IsVisible( const IterGCell& icl, const Vect2D& vct);
  bool  IsVisibleDump( const IterGCell& icl, const Vect2D& vct);

  bool  Check();
  void  DumpTri();
  void  DumpTEC( FILE *f);

protected:
  IterGPnt  mlstNod[NUM_TRI]; // ref. to three nodes
  IterGCell mlstCell[NUM_TRI];  // ref. to three neighbours	  
  
  bool    mbIsOutside;
  Vect2D    mCircCenter;

  MGInt   mCross;
};
//--------------------------------------------------------------------

typedef HGrdTri::CollGCell CollGCell;
typedef HGrdTri::IterGCell IterGCell;

//////////////////////////////////////////////////////////////////////
/// class HFroSeg
//////////////////////////////////////////////////////////////////////
class HFroSeg
{
public:
  HFroSeg(   HGrid *parent );
  HFroSeg( const HFroSeg& seg,   HGrid *parent);
  HFroSeg( IterGPnt ip1, IterGPnt ip2, HGrid *parent);

  ~HFroSeg()  {}

  const IterGPnt& PntLf() const { return miPntLf;}
  const IterGPnt& PntRt() const { return miPntRt;}
  IterGPnt&   rPntLf()    { return miPntLf;}
  IterGPnt&   rPntRt()    { return miPntRt;}

  const IterGCell&  CellUp() const  { return miCellUp;}
  const IterGCell&  CellLo() const  { return miCellLo;}
  IterGCell&      rCellUp()   { return miCellUp;}
  IterGCell&      rCellLo()   { return miCellLo;}

protected:
	HGrid *Parent;
  IterGPnt  miPntLf;    // ref. to left GrdPoint
  IterGPnt  miPntRt;    // ref. to right GrdPoint
  IterGCell miCellUp;   // ref. to upper cell
  IterGCell miCellLo;   // ref. to lower cell

public:
  bool    mbtmp;
};
//--------------------------------------------------------------------

typedef list<HFroSeg*>    CollFSeg;
typedef CollFSeg::iterator  IterFSeg;


//////////////////////////////////////////////////////////////////////
// class HFront
//////////////////////////////////////////////////////////////////////
class HFront : public CollFSeg
{
  friend class HFrontLoop;

public:
  HFront()    {}
  ~HFront();

  MGFloat Angle( const Vect2D& vct);

};
//--------------------------------------------------------------------

typedef list<HFront>    CollFro;
typedef CollFro::iterator IterFro;

//////////////////////////////////////////////////////////////////////
/// class HGrid
//////////////////////////////////////////////////////////////////////
class HGrid
{
public:
  HGrid()   {}
  ~HGrid();

  void  Init( const vector<Vect2D>& tabp, const vector<MGInt>& tabn );
  void  Generate();
  void  ExportTECTmp( FILE *f);


  IterGCell CellBegin()   { return mcolCell.begin(); }
  IterGCell CellEnd()   { return mcolCell.end(); }

  IterGPnt  PntBegin() { return mcolPnt.begin(); }
  IterGPnt  PntEnd()   { return mcolPnt.end(); }

protected:
// protected attributes
  CollGPnt    mcolPnt;  // list of boundary grid points
  CollGCell   mcolCell; // list of cells
  CollFro     mcolFro;

// auxiliary attributes
  IterGPnt    mind1, mind2, mind3, mind4;   //iterators to 4 corners of the bounding box
  HRect     mBox;


// protected methods for internal use
  bool  IsOutside( const Vect2D& vct);
  bool  IsBoxCorner( const IterGPnt& ipnt);

  IterGPnt  InsertPoint( HGrdPnt *gpnt);
  IterGCell InsertCell( HGrdTri *gcell);

// UNSTRUCTURED GRID GENERATIUON
  void  CheckBoundIntegr();
  void  FlagOuterTris();
  void  InitTriangles();  // init. triangulation (2 cells)
  void  RemoveOuterTris();

  bool  CheckSwapTriangles( HGrdTri *ptri1, HGrdTri *ptri2);
  void  SwapTriangles( HGrdTri *ptri1, HGrdTri *ptri2, bool bgo = true);

  bool  CheckNeighb( IterGCell icl, CollFSeg& lstsg,
             const Vect2D& vct, const IterGCell& ipvcl);

  MGInt InsertPointIntoMesh( IterGPnt itr);

  //HFroSeg*  NewFace( MGInt i, IterGCell icl = this->Parent->CellEnd());
	HFroSeg*  NewFace( MGInt i, IterGCell icl );

  void  CheckPoints( char st[]);
  void  CheckGrid();
};
//--------------------------------------------------------------------

inline IterGPnt HGrid::InsertPoint( HGrdPnt *gpnt)
{
  return mcolPnt.insert( mcolPnt.end(), gpnt );
}

inline IterGCell HGrid::InsertCell( HGrdTri *gcell)
{
  return mcolCell.insert( mcolCell.end(), gcell );
}

inline bool HGrid::IsBoxCorner( const IterGPnt& ipnt)
{
  if ( ipnt == mind1 || ipnt == mind2 || ipnt == mind3 || ipnt == mind4 )
    return true;

  return false;
}

#endif // __HGRID_H__
