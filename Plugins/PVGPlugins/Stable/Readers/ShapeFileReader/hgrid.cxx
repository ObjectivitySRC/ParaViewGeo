////////////////////////////////////////////////////////////////////////////////
//
//  File:        hgrid.cxx
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

#include "hgrid.h"


MGFloat Angle( const Vect2D &nd, const Vect2D &nd1, const Vect2D &nd2)
{
	static Vect2D	v1, v2;
	static MGFloat	dvect, dsin, dcos;

	v1 = (nd1 - nd).module();
	v2 = (nd2 - nd).module();

	dsin = v1.X()*v2.Y() - v2.X()*v1.Y();
	dcos = v1.X()*v2.X() + v1.Y()*v2.Y();
	if ( fabs( dsin) < ZERO && fabs( dcos) < ZERO)
		return M_PI_2;
	dvect = atan2( dsin, dcos);
	return dvect;
}

bool CheckCrossing( const Vect2D& v1, const Vect2D& v2, const Vect2D& v3, const Vect2D& v4)
{
	Vect2D	vv, vv1, vv2;
	MGFloat	t1, t2, t;
	MGFloat	h1, h2;
	
	vv = v2 - v1;
	vv1 = v3 - v1;
	vv2 = v4 - v1;
	
	ASSERT( fabs( vv.module() ) > ZERO );

	t1 = vv * vv1 / vv.module();
	t2 = vv * vv2 / vv.module();
	
	h1 = (vv.X()*vv1.Y() - vv.Y()*vv1.X()) / vv.module();
	h2 = (vv.X()*vv2.Y() - vv.Y()*vv2.X()) / vv.module();

	if ( fabs( h2 - h1) < ZERO)
		return false;

	t = t1 - (t2 - t1)/(h2 - h1) * h1;
	
	if ( t > 0.0 && t < vv.module() && h1 * h2 < ZERO )
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////
/// class HGrdTri
//////////////////////////////////////////////////////////////////////
HGrdTri::HGrdTri( HGrid *parent ) : mbIsOutside( false)	
{
	this->Parent = parent;
	for ( MGInt i=0; i<NUM_TRI; i++)
	{
		mlstNod[i] = this->Parent->PntEnd();
		mlstCell[i] = this->Parent->CellEnd(); //  this->Parent->CellEnd();
	}
}

bool HGrdTri::IsVisible( const IterGCell& icl, const Vect2D& vct)
{
	static Vect2D	vfac, v1, v2;
	static MGFloat	d1, d2;
	static char		sbuf[1024];

	if ( Cell(0) == icl )
	{
		v1 = *( (*Node(0)) ) - *( (*Node(2)) );
		v2 = *( (*Node(1)) ) - *( (*Node(2)) );
		vfac = vct - *( (*Node(2)) );
	}
	else if ( Cell(1) == icl )
	{
		v1 = *( (*Node(1)) ) - *( (*Node(0)) );
		v2 = *( (*Node(2)) ) - *( (*Node(0)) );
		vfac = vct - *( (*Node(0)) );
	}
	else if ( Cell(2) == icl )
	{
		v1 = *( (*Node(2)) ) - *( (*Node(1)) );
		v2 = *( (*Node(0)) ) - *( (*Node(1)) );
		vfac = vct - *( (*Node(1)) );
	}
	else
	{
		ASSERT( 0);
	}
	d1 = v1.X()*vfac.Y() - v1.Y()*vfac.X();
	d2 = v2.X()*vfac.Y() - v2.Y()*vfac.X();

	d1 /= vfac.module();
	d2 /= vfac.module();
	
    if ( (d2 > ZERO && d1 < -ZERO) )
        return false;
    else
        return true;
}

void HGrdTri::SetNeighbour( const IterGCell& itrcl)
{
	static HGrdTri	*ptr;
	
	ASSERT( itrcl != this->Parent->CellEnd());
	ptr = (*itrcl);
	ASSERT( ptr);
	
	if ( ( ptr->Node(1) == Node(0) && ptr->Node(0) == Node(1) ) ||
		 ( ptr->Node(0) == Node(0) && ptr->Node(2) == Node(1) ) ||
		 ( ptr->Node(2) == Node(0) && ptr->Node(1) == Node(1) ) )
	{
		rCell(0) = itrcl;
	}
	else
	if ( ( ptr->Node(1) == Node(1) && ptr->Node(0) == Node(2) ) ||
		 ( ptr->Node(0) == Node(1) && ptr->Node(2) == Node(2) ) ||
		 ( ptr->Node(2) == Node(1) && ptr->Node(1) == Node(2) ) )
	{
		rCell(1) = itrcl;
	}
	else
	if ( ( ptr->Node(1) == Node(2) && ptr->Node(0) == Node(0) ) ||
		 ( ptr->Node(0) == Node(2) && ptr->Node(2) == Node(0) ) ||
		 ( ptr->Node(2) == Node(2) && ptr->Node(1) == Node(0) ) )
	{
		rCell(2) = itrcl;
	}
}

void HGrdTri::NullifyThis( HGrdTri *pcl)
{
	for ( MGInt i=0; i<NUM_TRI; i++)
	{
		if ( mlstCell[i] != this->Parent->CellEnd())
			if ( (*mlstCell[i]) == pcl)
				mlstCell[i] = this->Parent->CellEnd();
	}
}

void HGrdTri::NullifyThis( HFroSeg *pseg)
{
	if ( ( pseg->PntLf() == Node(0) && pseg->PntLf() == Node(1) ) ||
		 ( pseg->PntRt() == Node(0) && pseg->PntRt() == Node(1) ) )
	{
		rCell(0) = this->Parent->CellEnd();
	}
	else
	if ( ( pseg->PntLf() == Node(1) && pseg->PntLf() == Node(2) ) ||
		 ( pseg->PntRt() == Node(1) && pseg->PntRt() == Node(2) ) )
	{
		rCell(1) = this->Parent->CellEnd();
	}
	else
	if ( ( pseg->PntLf() == Node(2) && pseg->PntLf() == Node(0) ) ||
		 ( pseg->PntRt() == Node(2) && pseg->PntRt() == Node(0) ) )
	{
		rCell(2) = this->Parent->CellEnd();
	}
}

void HGrdTri::InvalidateNeighb()
{
	for ( MGInt i=0; i<NUM_TRI; i++)
	{
		if ( mlstCell[i] != this->Parent->CellEnd())
			(*mlstCell[i])->NullifyThis( this);
	}
}

bool HGrdTri::IsInside( const Vect2D& vct)
{
// ::TODO:: new and faster algorithm should be introduced
	MGFloat	alf;

	alf  = ::Angle( vct, *(*Node(0)), *(*Node(1)) );
	alf += ::Angle( vct, *(*Node(1)), *(*Node(2)) );
	alf += ::Angle( vct, *(*Node(2)), *(*Node(0)) );

	if ( fabs(alf) < M_PI )
		return false;
	else 
		return true;
}

bool HGrdTri::SetCircCenter()
{
	static MGFloat	x1, y1, x2, y2, x3, y3;
	static MGFloat	xr, yr, d;

	x1 = (*Node(0))->X();
	y1 = (*Node(0))->Y();
	x2 = (*Node(1))->X();
	y2 = (*Node(1))->Y();
	x3 = (*Node(2))->X();
	y3 = (*Node(2))->Y();

	d = y3*(x2 - x1) + y2*(x1 - x3) + y1*(x3 - x2);
	if ( fabs( d) < ZERO)
	{
		DumpTri();
		return true;
	}

	xr = x1*x1*(y3-y2) + x2*x2*(y1-y3) + x3*x3*(y2-y1) + 
		 y1*y1*(y3-y2) + y2*y2*(y1-y3) + y3*y3*(y2-y1);
	xr *= -0.5/d;

	yr = x1*x1*(x3-x2) + x2*x2*(x1-x3) + x3*x3*(x2-x1) + 
		 y1*y1*(x3-x2) + y2*y2*(x1-x3) + y3*y3*(x2-x1);
	yr *= 0.5/d;

	mCircCenter =  Vect2D( xr, yr);

	return false;
}

Vect2D HGrdTri::Center()
{
	return ( *(*Node(0)) + *(*Node(1)) + *(*Node(2)) )/3.0;
}

IterGCell HGrdTri::NextCell( const Vect2D& vct)
{
	static Vect2D	v1, v2;
	
	if(  this->Parent->CellEnd() != Cell(0) )//he is not accepting this passing this->Parent->CellEnd()
	{
		v1 = *(*Node(1)) - *(*Node(0));
		v1 = Vect2D( -v1.Y(), v1.X() );
		v2 = ( *(*Node(1)) + *(*Node(0)) )/2.0;
		v2 = vct - v2;
		if ( v1 * v2 < 0.0 )
			return Cell(0);
	}
	
	if( Cell(1) != this->Parent->CellEnd())
	{
		v1 = *(*Node(2)) - *(*Node(1));
		v1 = Vect2D( -v1.Y(), v1.X() );
		v2 = ( *(*Node(2)) + *(*Node(1)) )/2.0;
		v2 = vct - v2;
		if ( v1 * v2 < 0.0 )
			return Cell(1);
	}

	if( Cell(2) != this->Parent->CellEnd()) 
	{
		v1 = *(*Node(0)) - *(*Node(2));
		v1 = Vect2D( -v1.Y(), v1.X() );
		v2 = ( *(*Node(0)) + *(*Node(2)) )/2.0;
		v2 = vct - v2;
		if ( v1 * v2 < 0.0 )
			return Cell(2);
	}
	return  this->Parent->CellEnd();
}

IterGCell  HGrdTri::NextCell( HFroSeg *pseg, const IterGCell& iclprv)
{
	IterGCell	itrnb, itr1, itr2;
	Vect2D		v1, v2, v3, v4;

	v1 = *(*(pseg->PntLf()));
	v2 = *(*(pseg->PntRt()));

	if ( iclprv == this->Parent->CellEnd())
	{

		if ( Node(0) == pseg->PntLf() )
		{
			v3 = *(*Node(1));
			v4 = *(*Node(2));
			itrnb = Cell(1);
			if ( Node(1) == pseg->PntRt() || Node(2) == pseg->PntRt() )
				return this->Parent->CellEnd();
		}
		else if ( Node(1) == pseg->PntLf() )
		{
			v3 = *(*Node(2));
			v4 = *(*Node(0));
			itrnb = Cell(2);
			if ( Node(2) == pseg->PntRt() || Node(0) == pseg->PntRt() )
				return this->Parent->CellEnd();
		}
		else if ( Node(2) == pseg->PntLf() )
		{
			v3 = *(*Node(0));
			v4 = *(*Node(1));
			itrnb = Cell(0);
			if ( Node(0) == pseg->PntRt() || Node(1) == pseg->PntRt() )
				return this->Parent->CellEnd();
		}
		else
		{
			THROW_INTERNAL("NextCell - seg: node not found");
		}

		if ( ::CheckCrossing( v1, v2, v3, v4 ) == true)
		{
			return itrnb;
		}
		else
			return this->Parent->CellEnd();
	}
	else
	{
		int	k;
		for ( int i=0; i<NUM_TRI; ++i)
		{
			if ( iclprv != Cell(i) )
			{
				if ( i == NUM_TRI-1)
					k = 0;
				else
					k = i+1;

				v3 = *(*Node(i));
				v4 = *(*Node(k));

				if ( ::CheckCrossing( v1, v2, v3, v4 ) == true)
					return Cell(i);
			}
		}

		return this->Parent->CellEnd();
	}
}

bool HGrdTri::IsInsideCirc( const Vect2D& vct)
{
	static MGFloat	R2, r2;
	static Vect2D	vtmp, v0;

	v0 = CircCenter();
	vtmp = v0 - *( (*Node(0)) );
	R2 = vtmp * vtmp;

	vtmp = v0 - vct;
	r2 = vtmp * vtmp;

	if ( r2 < R2) 
		return true;
	
	return false;
}

const Vect2D& HGrdTri::CircCenter() const
{
  return mCircCenter;
}

IterGPnt HGrdTri::Node( const MGInt& i) const
{
  ASSERT( i>=0 && i<NUM_TRI);
  return mlstNod[i];
}

IterGCell HGrdTri::Cell( const MGInt& i) const
{
  ASSERT( i>=0 && i<NUM_TRI);
  return mlstCell[i];
}

IterGPnt& HGrdTri::rNode( const MGInt& i)
{
  ASSERT( i>=0 && i<NUM_TRI);
  return mlstNod[i];
}

IterGCell& HGrdTri::rCell( const MGInt& i)
{
  ASSERT( i>=0 && i<NUM_TRI);
  return mlstCell[i];
}

bool HGrdTri::Check()
{
  Vect2D  v1 = *(*Node(1)) - *(*Node(0));
  Vect2D  v2 = *(*Node(2)) - *(*Node(0));
  MGFloat d = v1.X()*v2.Y() - v1.Y()*v2.X();

  if ( fabs(d) < 1.0e-16)
    return false;
  else
    return true;
}

MGFloat HGrdTri::Area() const
{
  Vect2D  v1 = *(*Node(1)) - *(*Node(0));
  Vect2D  v2 = *(*Node(2)) - *(*Node(0));
  MGFloat d = v1.X()*v2.Y() - v1.Y()*v2.X();

  return 0.5*d;
}

void HGrdTri::DumpTri()
{
  char  sbuf[256];
  sprintf( sbuf, "n0 = (%lg %lg) n1 = (%lg %lg) n2 = (%lg %lg)",
    (*Node(0))->X(), (*Node(0))->Y(),
    (*Node(1))->X(), (*Node(1))->Y(),
    (*Node(2))->X(), (*Node(2))->Y() );

  TRACE( sbuf);

  void    *cp0, *cp1, *cp2;

  cp0 = cp1 = cp2 = NULL;
	if ( Cell(0) != this->Parent->CellEnd())
    cp0 = *Cell(0);

  if ( Cell(1) != this->Parent->CellEnd())
    cp1 = *Cell(1);

  if ( Cell(2) != this->Parent->CellEnd())
    cp2 = *Cell(2);

  //sprintf( sbuf, "c0 = (%p %p) c1 = (%p %p) c2 = (%p %p)", Cell(0), cp0, Cell(1), cp1, Cell(2), cp2);
  sprintf( sbuf, "c0 = (%p) c1 = (%p) c2 = (%p)", cp0, cp1, cp2);

  TRACE( sbuf);
}

void HGrdTri::DumpTEC( FILE *f)
{
  fprintf( f, "VARIABLES = \"X\", \"Y\"\n");
  fprintf( f, "ZONE T=\"TRIANGLES\", ");
  fprintf( f, "N=%2ld, ", 3 );
  fprintf( f, "E=%2ld, F=FEPOINT, ET=QUADRILATERAL C=BLACK\n ", 1 );

  fprintf( f, "%lg %lg\n%lg %lg\n%lg %lg\n",
    (*Node(0))->X(), (*Node(0))->Y(),
    (*Node(1))->X(), (*Node(1))->Y(),
    (*Node(2))->X(), (*Node(2))->Y() );
  fprintf( f, "1 2 3 3\n");
}

//////////////////////////////////////////////////////////////////////
// class HFront
//////////////////////////////////////////////////////////////////////
HFront::~HFront()
{
	iterator	i;
	for ( i= begin(); i != end(); i++)
		if ( *i != NULL) delete (*i);
}

MGFloat HFront::Angle( const Vect2D& vct)
{
	iterator	itr;
	MGFloat		alf;
	IterGPnt	ipnt1, ipnt2;
	
	alf = 0.0;
	for ( itr = begin(); itr != end(); itr++)
	{
		ipnt1 = (*itr)->PntLf();
		ipnt2 = (*itr)->PntRt();
		alf += ::Angle( vct, *(*ipnt1), *(*ipnt2) );
	}
	return alf;
}

//////////////////////////////////////////////////////////////////////
// class HFroSeg
//////////////////////////////////////////////////////////////////////

HFroSeg::HFroSeg( HGrid *parent)
{
	this->Parent = parent;
  miPntLf = miPntRt = this->Parent->PntEnd();
  miCellUp = miCellLo = this->Parent->CellEnd();
}

HFroSeg::HFroSeg( const HFroSeg& seg, HGrid *parent)
{
	this->Parent = parent;
  miPntLf = seg.miPntLf;
  miPntRt = seg.miPntRt;
  miCellUp = seg.miCellUp;
  miCellLo = seg.miCellLo;
}

HFroSeg::HFroSeg( IterGPnt ip1, IterGPnt ip2, HGrid *parent)
{
	this->Parent = parent;
  miPntLf = ip1;
  miPntRt = ip2;
  miCellUp = this->Parent->CellEnd();
  miCellLo = this->Parent->CellEnd();
}


//////////////////////////////////////////////////////////////////////
// class HGrid
//////////////////////////////////////////////////////////////////////

HGrid::~HGrid()			
{
	CollGPnt::iterator	itrpnt;
	CollGCell::iterator	itrcell;
	
	for ( itrpnt = mcolPnt.begin(); itrpnt != mcolPnt.end(); itrpnt++)
		if ( (*itrpnt) != NULL) delete (*itrpnt);

	for ( itrcell = mcolCell.begin(); itrcell != mcolCell.end(); itrcell++)
		if ( (*itrcell) != NULL) delete (*itrcell);
}

void HGrid::Init( const vector<Vect2D>& tabp, const vector<MGInt>& tabn )
{
	MGInt	i, j, nprev;
	IterFro	ifro;

	HGrdPnt		*ppnt;
	IterGPnt	ip0, ipp, ipa;
	HFroSeg		*pfro;
	Vect2D		v0, v1, v2;

	map< Vect2D, IterGPnt>				mapNod;
	map< Vect2D, IterGPnt>::iterator	imap;

	double	d;

	nprev = 0;
	for ( i=0; i<(MGInt)tabn.size(); ++i)
	{
		v1 = tabp[nprev];
		v2 = tabp[nprev+tabn[i]-1];

		d = (v2-v1).module();

		if ( (v2-v1).module() < ZERO)
		{
			ifro = mcolFro.insert( mcolFro.end(), HFront() );

			imap = mapNod.find( tabp[nprev]);
			if ( imap != mapNod.end() )
			{
				ip0 = ipp = ipa = (*imap).second;
			}
			else
			{
				ppnt = MGNEW HGrdPnt( tabp[nprev]);
				ppnt->rIndex() = nprev;
				ip0 = ipp = ipa = InsertPoint( ppnt);
				mapNod.insert( map< Vect2D, IterGPnt>::value_type( *ppnt, ipa) );
			}

			v0 = *(*ip0); 

			for ( j=1; j<tabn[i]; ++j)
			{
				v1 = *(*ipp);
				v2 = tabp[nprev+j];

				if ( (v2 - v1).module() > ZERO)
				{
					if ( j != tabn[i]-1 || (tabp[nprev+j] - v0 ).module() > ZERO)
					{
						imap = mapNod.find( tabp[nprev+j]);
						if ( imap != mapNod.end() )
						{
							ipa = (*imap).second;
							ppnt = *ipa;
						}
						else
						{
							ppnt = MGNEW HGrdPnt( tabp[nprev+j]);
							ppnt->rIndex() = nprev+j;
							ipa = InsertPoint( ppnt);
							mapNod.insert( map< Vect2D, IterGPnt>::value_type( *ppnt, ipa) );
						}
						pfro = MGNEW HFroSeg( ipp, ipa, this);
						(*ifro).insert( (*ifro).end(), pfro);
						ipp = ipa;
					}
				}
			}

			v1 = *(*ipp);
			v2 = *(*ip0);

			if ( (v2 - v1).module() > ZERO)
			{
				pfro = MGNEW HFroSeg( ipp, ip0, this);
				(*ifro).insert( (*ifro).end(), pfro);
			}
		}

		nprev += tabn[i];
	}
	IterFro		itrfro;
	IterFSeg	itrsg;
	IterGPnt	ip1, ip2;

	for ( itrfro = mcolFro.begin(); itrfro != mcolFro.end(); ++itrfro)
	{
		itrsg = (*itrfro).begin();
		ip0 = (*itrsg)->PntLf();
		ip1 = (*itrsg)->PntRt();

		for ( ++itrsg; itrsg != (*itrfro).end(); ++itrsg)
		{
			if ( (*itrsg)->PntLf() != ip1 )
			{
				TRACE( "Front not consistent !!!\n");
			}
			ip1 = (*itrsg)->PntRt();

			v1 = *(*(*itrsg)->PntLf());
			v2 = *(*(*itrsg)->PntRt());

			if ( (v2 - v1).module() < ZERO)
				TRACE1( "seg length = %24.16lg\n", (v2 - v1).module() );
		}

		if ( ip0 != ip1 )
		{
			TRACE( "Front not consistent (closure problem) !!!\n");
		}
	}
}

HFroSeg* HGrid::NewFace( MGInt i, IterGCell icl)
{
	HFroSeg*	psg;
	
	THROW_ALLOC( psg = MGNEW HFroSeg( this ) );
	switch ( i)
	{
		case  0:
			psg->rPntLf() = (*icl)->Node(0);
			psg->rPntRt() = (*icl)->Node(1);
			if ( icl != this->CellEnd() )
			{
				psg->rCellUp() = icl;
				psg->rCellLo() = (*icl)->Cell(0);
			}
			break;

		case  1:
			psg->rPntLf() = (*icl)->Node(1);
			psg->rPntRt() = (*icl)->Node(2);
			if ( icl != this->CellEnd())
			{
				psg->rCellUp() = icl;
				psg->rCellLo() = (*icl)->Cell(1);
			}
			break;
			
		case  2:
			psg->rPntLf() = (*icl)->Node(2);
			psg->rPntRt() = (*icl)->Node(0);
			if ( icl != this->CellEnd())
			{
				psg->rCellUp() = icl;
				psg->rCellLo() = (*icl)->Cell(2);
			}
			break;
			
		default:
			if ( psg) delete psg;
			ASSERT( 0);
			return NULL;
	};
	return psg;
}

bool HGrid::CheckNeighb( IterGCell icl, CollFSeg& lstsg, 
						 const Vect2D& vct, const IterGCell& ipvcl)
{
	HGrdTri	*pthis;
	
	pthis = (HGrdTri*)( (*icl) );

	if ( pthis->IsInsideCirc( vct ) == true )
	{
		HGrdTri		*ptri;
		HFroSeg		*pseg;
		IterGCell	itri;
		IterGPnt	ipnt;
		bool		bVis;

		for ( int i=0; i<NUM_TRI; i++)
		{		
			if ( (*icl)->Cell(i) != ipvcl || ipvcl == this->CellEnd())
			{
				pseg = NewFace( i, icl);	// this allocate memory for pseg !!!
				
				itri = (*icl)->Cell(i);
				bVis = false;
				if ( itri != this->CellEnd() )
					if ( (*itri)->IsOutside() == true)
						itri = this->CellEnd();
				
				if ( itri != this->CellEnd())
				{
					ptri = (HGrdTri*)( (*itri) );
					ASSERT( ptri);
	          	  	bVis = ptri->IsVisible( icl, vct);
				}
				
				if ( itri != this->CellEnd() && bVis)
				{
            	    if ( CheckNeighb( itri, lstsg, vct, icl ) == true )
                	{
                        delete pseg;
                    }
	                else
    	            {
						lstsg.insert( lstsg.end(), pseg );
						pseg->rCellUp() = this->CellEnd();
					}
	            }
				else
				{
					lstsg.insert( lstsg.end(), pseg );
					pseg->rCellUp() = this->CellEnd();
				}
			}
		}
					
    ptri = (HGrdTri*)( (*icl) );
   	ptri->InvalidateNeighb();

    mcolCell.erase( icl);
		delete ptri;
		return true;
	}
	return false;
}

MGInt HGrid::InsertPointIntoMesh( IterGPnt pntitr)
{
	Vect2D		vct;
	IterGCell	itrcl, itrcl2, itrcl0, itrclout;
	HGrdTri		*ptri;
	CollFSeg	*plstsg;
	IterFSeg	itrsg, itrsg2;
	HFroSeg		*pseg;
	
	static int 	num = 0;
	++num;

	vct = *(*pntitr);
	
	itrcl = mcolCell.begin();

	do
	{
		itrcl0 = (*itrcl)->NextCell( vct);
		if ( itrcl0 == this->CellEnd())
			break;
		itrcl = itrcl0;
	}
	while ( true);
	
	THROW_ALLOC( plstsg = MGNEW CollFSeg );
	
	// next function creates segments bounding Delaunay cavity (stored in plstsg)
	// removes cavity triangles from mcolCell;
	// ALL ITERATORS TO THOSE CELLS ARE THEN INVALID !!!
	// iterators to those cells are set to NULL
	CheckNeighb( itrcl, *plstsg, vct, this->CellEnd());

	// sorting segments stored in plstsg 
	itrsg = plstsg->end();
	itrsg--;
	do
	{
		for ( itrsg2 = plstsg->begin(); itrsg2 != plstsg->end(); itrsg2++)
		{
			if ( (*itrsg)->PntLf() == (*itrsg2)->PntRt() )
			{
				pseg = (*itrsg2);
				plstsg->erase( itrsg2);
				itrsg = plstsg->insert( itrsg, pseg );
				break;
			}
		}
	}
	while ( itrsg != plstsg->begin() );

	// creating new triangles and connections between triangles in Delaunay cavity	
	itrcl0 = itrcl2 = this->CellEnd();
	for ( itrsg = plstsg->begin(); itrsg != plstsg->end(); itrsg++)
	{
		THROW_ALLOC( ptri = MGNEW HGrdTri( this ) );
		
		itrclout = (*itrsg)->CellLo();
		ptri->rNode(0) = (*itrsg)->PntLf();
		ptri->rNode(1) = (*itrsg)->PntRt();
		ptri->rNode(2) = pntitr;
		ptri->rCell(0) = itrclout;
			
		if ( ptri->SetCircCenter() )
		{
			FILE *f = fopen( "cavity.plt", "wt");
			ExportTECTmp( f);
			fclose( f);

			FILE *ff = fopen( "cavity_front.plt", "wt");
			fprintf( ff, "VARIABLES = \"X\", \"Y\"\n");
			fprintf( ff, "ZONE I=%d F=POINT\n", plstsg->size()+1 );

			fprintf( ff, "%lg %lg\n", (*(*plstsg->begin())->PntLf())->X(), (*(*plstsg->begin())->PntLf())->Y() );
			for ( itrsg = plstsg->begin(); itrsg != plstsg->end(); itrsg++)
				fprintf( ff, "%lg %lg\n", (*(*itrsg)->PntRt())->X(), (*(*itrsg)->PntRt())->Y() );

			fclose( ff);

			THROW_INTERNAL("Flat triangle !!!");
		}
		
		itrcl = InsertCell( ptri);

		if ( itrclout != this->CellEnd())
			(*itrclout)->SetNeighbour( itrcl);

		if ( itrcl0 == this->CellEnd())
			itrcl0 = itrcl;
			
		if ( itrcl2 != this->CellEnd())
		{
			(*itrcl)->rCell(2)  = itrcl2;
			(*itrcl2)->rCell(1) = itrcl;
		}
		itrcl2 = itrcl;
	}
	if ( itrcl2 != this->CellEnd() && itrcl0 != this->CellEnd())
	{
		(*itrcl0)->rCell(2) = itrcl2;
		(*itrcl2)->rCell(1) = itrcl0;
	}	
		
	// removing all segments stored in plstsg
	for ( itrsg = plstsg->begin(); itrsg != plstsg->end(); itrsg++)
		if ( (*itrsg) != NULL ) delete (*itrsg);
		
	if ( plstsg) delete plstsg;

	return num;
}

// creates basic trinagulation (two triangles) and inserts all boundary points
void HGrid::InitTriangles()
{
	CollGPnt::iterator	itr;
	Vect2D		vmin, vmax, vct;
	bool		bFirst = true;
	HGrdPnt		*pnd1, *pnd2, *pnd3, *pnd4;
	HGrdTri		*ptri1, *ptri2;
	IterGPnt	ind1, ind2, ind3, ind4;
	IterGCell	itri1, itri2;
	
	ind1 = ind2 = ind3 = ind4 = this->PntEnd();
	itri1 = itri2 = this->CellEnd();

	// finding limits
	for ( itr = mcolPnt.begin(); itr != mcolPnt.end(); itr++)
	{
		vct = *(*itr);
		if ( bFirst)
		{
			vmin = vmax = vct;
			bFirst = false;
		}
		else
		{
			if ( vct.X() > vmax.X() ) vmax.rX() = vct.X();
			if ( vct.Y() > vmax.Y() ) vmax.rY() = vct.Y();
			if ( vct.X() < vmin.X() ) vmin.rX() = vct.X();
			if ( vct.Y() < vmin.Y() ) vmin.rY() = vct.Y();
		}
	}
	
	vct = (vmax - vmin)/1.5;
	vmax += vct;
	vmin -= vct;
	
	mBox = HRect( vmin.X(), vmin.Y(), vmax.X(), vmax.Y() ); 

	//---------
	
	
	// creating starting triangulation containing two cells and four points
	THROW_ALLOC( pnd1 = MGNEW HGrdPnt( vmin) );
	THROW_ALLOC( pnd2 = MGNEW HGrdPnt( vmax.X(), vmin.Y()) );
	THROW_ALLOC( pnd3 = MGNEW HGrdPnt( vmax) );
	THROW_ALLOC( pnd4 = MGNEW HGrdPnt( vmin.X(), vmax.Y()) );
	
	THROW_ALLOC( ptri1 = MGNEW HGrdTri(this) );
	THROW_ALLOC( ptri2 = MGNEW HGrdTri(this) );
		
	mind1 = ind1 = InsertPoint( pnd1);
	mind2 = ind2 = InsertPoint( pnd2);
	mind3 = ind3 = InsertPoint( pnd3);
	mind4 = ind4 = InsertPoint( pnd4);

	itri1 = InsertCell( ptri1);
	itri2 = InsertCell( ptri2);

	ptri1->rNode(0) = ind1;
	ptri1->rNode(1) = ind2;
	ptri1->rNode(2) = ind3;
	
	ptri2->rNode(0) = ind3;
	ptri2->rNode(1) = ind4;
	ptri2->rNode(2) = ind1;
	
	ptri1->rCell(2) = itri2;
	ptri2->rCell(2) = itri1;
	ptri1->SetCircCenter();
	ptri2->SetCircCenter();

	// inserting frontal points into mesh
	IterFro		itrfro;
	IterFSeg	itrsg;

	map<HGrdPnt*,int>			mapNod;
	map<HGrdPnt*,int>::iterator	imap;

	for ( itrfro = mcolFro.begin(); itrfro != mcolFro.end(); itrfro++)
	{
		for ( itrsg = (*itrfro).begin(); itrsg != (*itrfro).end(); itrsg++)
		{
			itr = (*itrsg)->rPntRt();

			if ( ( imap = mapNod.find( *itr) ) == mapNod.end() )
			{
				InsertPointIntoMesh( itr);
				mapNod.insert( map<HGrdPnt*,int>::value_type( *itr, 0) );
			}

		}
	}			
}

bool HGrid::IsOutside( const Vect2D& vct)
{
		IterFro		itrfro;
	IterFSeg	itrsg;
	Vect2D		v1, v2;
	double		x;

// ray casting algorithm
	MGInt	cross = 0;
	for ( itrfro = mcolFro.begin(); itrfro != mcolFro.end(); itrfro++)
		for ( itrsg = (*itrfro).begin(); itrsg != (*itrfro).end(); itrsg++)
		{
			v1 = *(*(*itrsg)->PntLf());
			v2 = *(*(*itrsg)->PntRt());

			if ( ( v1.Y() > vct.Y() && v2.Y() <= vct.Y() ) ||
				 ( v2.Y() > vct.Y() && v1.Y() <= vct.Y() ) )
			{

				x = ( v2.X() - v1.X() ) * ( vct.Y() - v1.Y() ) / ( v2.Y() - v1.Y() ) + v1.X();

				if ( x > vct.X() ) 
					++cross;
			}
		}

	if ( (cross % 2) == 1 )
		return false;
	else 
		return true;
}

void HGrid::FlagOuterTris()
{
	IterFro		itrfro;
	IterFSeg	itrsg;
	IterGCell	itr, itrnb, itrcl;;
	Vect2D		vout, vcnt, vc1, vc2;

	CollGCell	colCell;

	Vect2D	v1, v2, vct;
	MGInt	cross = 0;
	HGrdTri	*ptri;
	MGFloat	x, y1, y2;

	multimap<MGFloat, HGrdTri*>				mapCell;
	multimap<MGFloat, HGrdTri*>::iterator	imap, ifirst, ilast;

	for ( itr = mcolCell.begin(); itr != mcolCell.end(); itr++)
	{
		vct = (*itr)->Center();
		(*itr)->rCross() = 0;
		mapCell.insert( multimap<MGFloat, HGrdTri*>::value_type( vct.Y(), (*itr) ) );
	}

	for ( itrfro = mcolFro.begin(); itrfro != mcolFro.end(); itrfro++)
		for ( itrsg = (*itrfro).begin(); itrsg != (*itrfro).end(); itrsg++)
		{
			v1 = *(*(*itrsg)->PntLf());
			v2 = *(*(*itrsg)->PntRt());
			if ( v1.Y() > v2.Y() )
			{
				y1 = v2.Y();
				y2 = v1.Y();
			}
			else
			{
				y1 = v1.Y();
				y2 = v2.Y();
			}

			ifirst = mapCell.lower_bound( y1 );
			ilast  = mapCell.upper_bound( y2 );

			for ( imap = ifirst; imap != ilast; ++imap)
			{
				ptri = (*imap).second;
				vct = ptri->Center();

				if ( ( v1.Y() > vct.Y() && v2.Y() <= vct.Y() ) ||
					 ( v2.Y() > vct.Y() && v1.Y() <= vct.Y() ) )
				{
					x = ( v2.X() - v1.X() ) * ( vct.Y() - v1.Y() ) / ( v2.Y() - v1.Y() ) + v1.X();

					if ( x > vct.X() ) 
						++(ptri->rCross());
				}
			}
		}
	
	for ( itr = mcolCell.begin(); itr != mcolCell.end(); itr++)
	{
		if ( ((*itr)->rCross() % 2) == 1 )
			(*itr)->rIsOutside() = false;
		else
			(*itr)->rIsOutside() = true;
	}
}

void HGrid::RemoveOuterTris()
{
	IterGCell	itr, itr2;
	HGrdTri		*ptri;

	itr2 = this->CellEnd();
	for ( itr = mcolCell.begin(); itr != mcolCell.end(); itr++)
	{
		if ( itr2 != this->CellEnd())
		{
			ptri = *itr2;
    		(*itr2)->InvalidateNeighb();
			mcolCell.erase( itr2);
			delete ptri;
			itr2 = this->CellEnd();
		}
		if ( (*itr)->IsOutside() )
			itr2 = itr;
	}

	if ( itr2 != this->CellEnd())
	{		
    (*itr2)->InvalidateNeighb();
		mcolCell.erase( itr2);		
		itr2 = this->CellEnd();
	}

	if ( *mind1) delete *mind1;
	if ( *mind2) delete *mind2;
	if ( *mind3) delete *mind3;
	if ( *mind4) delete *mind4;
	mcolPnt.erase( mind1);
	mcolPnt.erase( mind2);
	mcolPnt.erase( mind3);
	mcolPnt.erase( mind4);
	mind1 = this->PntEnd();
	mind2 = this->PntEnd();
	mind3 = this->PntEnd();
	mind4 = this->PntEnd();
}

bool HGrid::CheckSwapTriangles( HGrdTri *ptri1, HGrdTri *ptri2)
{
	HGrdTri tri1(this) , tri2(this);

	tri1 = *ptri1;
	tri2 = *ptri2;

	SwapTriangles( &tri1, &tri2, false);

	if ( !tri1.Check() || !tri2.Check() )
		return false;

	if ( tri1.Area() < 0 || tri2.Area() < 0 )
		return false;

	return true;
}

void HGrid::SwapTriangles( HGrdTri *ptri1, HGrdTri *ptri2, bool bgo)
{
	MGInt		ifc1, ifc2;
	IterGPnt	ip1, ip2, ip3, ip4;
	IterGCell	ic1, ic2, ic3, ic4;

	IterGCell itri1, itri2;
	
	if ( ptri2->Node(1) == ptri1->Node(0) && ptri2->Node(0) == ptri1->Node(1) )
	{
		ip1 = ptri2->Node(1);
		ip2 = ptri2->Node(2);
		ip3 = ptri2->Node(0);
		ip4 = ptri1->Node(2);
		ifc1 = 0;
		ifc2 = 0;
		ic1 = ptri2->Cell(1);
		ic2 = ptri2->Cell(2);
		ic3 = ptri1->Cell(1);
		ic4 = ptri1->Cell(2);
		itri1 = ptri2->Cell(0);
		itri2 = ptri1->Cell(0);
	}
	else
	if ( ptri2->Node(0) == ptri1->Node(0) && ptri2->Node(2) == ptri1->Node(1) )
	{
		ip1 = ptri2->Node(0);
		ip2 = ptri2->Node(1);
		ip3 = ptri2->Node(2);
		ip4 = ptri1->Node(2);
		ifc1 = 0;
		ifc2 = 2;
		ic1 = ptri2->Cell(0);
		ic2 = ptri2->Cell(1);
		ic3 = ptri1->Cell(1);
		ic4 = ptri1->Cell(2);
		itri1 = ptri2->Cell(2);
		itri2 = ptri1->Cell(0);
	}
	else 
	if ( ptri2->Node(2) == ptri1->Node(0) && ptri2->Node(1) == ptri1->Node(1) )
	{
		ip1 = ptri2->Node(2);
		ip2 = ptri2->Node(0);
		ip3 = ptri2->Node(1);
		ip4 = ptri1->Node(2);
		ifc1 = 0;
		ifc2 = 1;
		ic1 = ptri2->Cell(2);
		ic2 = ptri2->Cell(0);
		ic3 = ptri1->Cell(1);
		ic4 = ptri1->Cell(2);
		itri1 = ptri2->Cell(1);
		itri2 = ptri1->Cell(0);
	}
	else
	
	if ( ptri2->Node(1) == ptri1->Node(2) && ptri2->Node(0) == ptri1->Node(0) )
	{
		ip1 = ptri2->Node(1);
		ip2 = ptri2->Node(2);
		ip3 = ptri2->Node(0);
		ip4 = ptri1->Node(1);
		ifc1 = 2;
		ifc2 = 0;
		ic1 = ptri2->Cell(1);
		ic2 = ptri2->Cell(2);
		ic3 = ptri1->Cell(0);
		ic4 = ptri1->Cell(1);
		itri1 = ptri2->Cell(0);
		itri2 = ptri1->Cell(2);
	}
	else
	if ( ptri2->Node(0) == ptri1->Node(2) && ptri2->Node(2) == ptri1->Node(0) )
	{
		ip1 = ptri2->Node(0);
		ip2 = ptri2->Node(1);
		ip3 = ptri2->Node(2);
		ip4 = ptri1->Node(1);
		ifc1 = 2;
		ifc2 = 2;
		ic1 = ptri2->Cell(0);
		ic2 = ptri2->Cell(1);
		ic3 = ptri1->Cell(0);
		ic4 = ptri1->Cell(1);
		itri1 = ptri2->Cell(2);
		itri2 = ptri1->Cell(2);
	}
	else
	if ( ptri2->Node(2) == ptri1->Node(2) && ptri2->Node(1) == ptri1->Node(0) )
	{
		ip1 = ptri2->Node(2);
		ip2 = ptri2->Node(0);
		ip3 = ptri2->Node(1);
		ip4 = ptri1->Node(1);
		ifc1 = 2;
		ifc2 = 1;
		ic1 = ptri2->Cell(2);
		ic2 = ptri2->Cell(0);
		ic3 = ptri1->Cell(0);
		ic4 = ptri1->Cell(1);
		itri1 = ptri2->Cell(1);
		itri2 = ptri1->Cell(2);
	}
	else
	
	if ( ptri2->Node(1) == ptri1->Node(1) && ptri2->Node(0) == ptri1->Node(2) )
	{
		ip1 = ptri2->Node(1);
		ip2 = ptri2->Node(2);
		ip3 = ptri2->Node(0);
		ip4 = ptri1->Node(0);
		ifc1 = 1;
		ifc2 = 0;
		ic1 = ptri2->Cell(1);
		ic2 = ptri2->Cell(2);
		ic3 = ptri1->Cell(2);
		ic4 = ptri1->Cell(0);
		itri1 = ptri2->Cell(0);
		itri2 = ptri1->Cell(1);
	}
	else
	if ( ptri2->Node(0) == ptri1->Node(1) && ptri2->Node(2) == ptri1->Node(2) )
	{
		ip1 = ptri2->Node(0);
		ip2 = ptri2->Node(1);
		ip3 = ptri2->Node(2);
		ip4 = ptri1->Node(0);
		ifc1 = 1;
		ifc2 = 2;
		ic1 = ptri2->Cell(0);
		ic2 = ptri2->Cell(1);
		ic3 = ptri1->Cell(2);
		ic4 = ptri1->Cell(0);
		itri1 = ptri2->Cell(2);
		itri2 = ptri1->Cell(1);
	}
	else
	if ( ptri2->Node(2) == ptri1->Node(1) && ptri2->Node(1) == ptri1->Node(2) )
	{
		ip1 = ptri2->Node(2);
		ip2 = ptri2->Node(0);
		ip3 = ptri2->Node(1);
		ip4 = ptri1->Node(0);
		ifc1 = 1;
		ifc2 = 1;
		ic1 = ptri2->Cell(2);
		ic2 = ptri2->Cell(0);
		ic3 = ptri1->Cell(2);
		ic4 = ptri1->Cell(0);
		itri1 = ptri2->Cell(1);
		itri2 = ptri1->Cell(1);
	}

	ASSERT( itri1 != this->CellEnd() && itri2 != this->CellEnd());
	
	ptri1->rNode(0) = ip2;	
	ptri1->rNode(1) = ip4;	
	ptri1->rNode(2) = ip1;	
	
	ptri1->rCell(0) = itri2;	
	ptri1->rCell(1) = ic4;	
	ptri1->rCell(2) = ic1;	
	
	ptri2->rNode(0) = ip4;	
	ptri2->rNode(1) = ip2;	
	ptri2->rNode(2) = ip3;	
	
	ptri2->rCell(0) = itri1;	
	ptri2->rCell(1) = ic2;	
	ptri2->rCell(2) = ic3;
	
	if ( bgo)
	{
		if ( ic1 != this->CellEnd() )
		{
			if ( (*ic1)->Cell(0) == itri2)
				(*ic1)->rCell(0) = itri1;
			else if ( (*ic1)->Cell(1) == itri2)
				(*ic1)->rCell(1) = itri1;
			else if ( (*ic1)->Cell(2) == itri2)
				(*ic1)->rCell(2) = itri1;
			else
				ASSERT(0);
		}

		if ( ic3 != this->CellEnd())
		{
			if ( (*ic3)->Cell(0) == itri1)
				(*ic3)->rCell(0) = itri2;
			else if ( (*ic3)->Cell(1) == itri1)
				(*ic3)->rCell(1) = itri2;
			else if ( (*ic3)->Cell(2) == itri1)
				(*ic3)->rCell(2) = itri2;
			else
				ASSERT(0);
		}
	}

}

void HGrid::CheckBoundIntegr()
{
//	char		sbuf[256];
	IterFro		itrfro;
	IterFSeg	itrsg;
	Vect2D		v1, v2, v3, v4;
	CollGCell	colPath;
	CollFSeg	colSeg;
	IterFSeg	iseg;
	IterGCell	itr, itrnb, itrcl;
	Vect2D		vcnt;
	bool		bReady, bFound;
	MGInt		k, i=0;

	list<IterGCell>				lstCell;
	list<IterGCell>::iterator	itrtmp;

	multimap< HGrdPnt*, IterGCell >						mapNod;
	pair< multimap< HGrdPnt*, IterGCell >::iterator, 
	      multimap< HGrdPnt*, IterGCell >::iterator>	range[2];	// no of points per froseg

	multimap< HGrdPnt*, IterGCell >::iterator			itrmap1, itrmap2;

	// getting number of front segments
	int no = 0;
	for ( itrfro = mcolFro.begin(); itrfro != mcolFro.end(); itrfro++)
		for ( itrsg = (*itrfro).begin(); itrsg != (*itrfro).end(); itrsg++)
			++no;


	for ( itr = mcolCell.begin(); itr != mcolCell.end(); itr++)
		for ( k=0; k<NUM_TRI; ++k)
			mapNod.insert( multimap<HGrdPnt*, IterGCell>::value_type( *(*itr)->Node(k), itr) );

	int	nrec = 0;
	for ( itrfro = mcolFro.begin(); itrfro != mcolFro.end(); itrfro++)
		for ( itrsg = (*itrfro).begin(); itrsg != (*itrfro).end(); itrsg++)
		{
			range[0] = mapNod.equal_range( *(*itrsg)->PntLf() );
			range[1] = mapNod.equal_range( *(*itrsg)->PntRt() );
			
			bReady = false;

			for ( itrmap1=range[0].first; itrmap1!=range[0].second; itrmap1++)
			{
				itrcl = (*itrmap1).second;
				lstCell.insert( lstCell.begin(), itrcl );
				for ( itrmap2=range[1].first; itrmap2!=range[1].second; itrmap2++)
				{
					if ( *itrcl == *(*itrmap2).second )
					{
						bReady = true;
						goto out_of_loops;
					}
				}
			}
			out_of_loops:
	
			// the segment is missing - must be recovered
			if ( ! bReady)
			{
				++nrec;

				v1 = *(*((*itrsg)->PntLf()));
				v2 = *(*((*itrsg)->PntRt()));

				bFound = false;
				for ( itrtmp = lstCell.begin(); itrtmp != lstCell.end(); ++itrtmp)
				{
					itr = *itrtmp;
					if ( ( itrnb = (*itr)->NextCell( *itrsg, this->CellEnd()) ) != this->CellEnd())
					{
						bFound = true;
						break;
					}
				}
				if ( bFound)
				{
					IterGCell	itr1, itr2,	itro1=this->CellEnd(), itro2=this->CellEnd();
					stack<IterGCell>	stackCell;

					itr1 = itr;
					itr2 = itrnb;

					int niter = 0;

					do 
					{
						++niter;
						// check if swap is possible if not then try with another triangles
						while ( /*! (*itr2)->IsVisible( itr1, v1) ||*/ 
							! CheckSwapTriangles( *itr1, *itr2 )
							|| ( itr1 == itro1 && itr2 == itro2 ) ) // avoid swaping the same triangles again
						{
							stackCell.push( itr1);
							itr = itr2;
							itr2 = (*itr)->NextCell( *itrsg, itr1);
							itr1 = itr;
							ASSERT( itr2 != this->CellEnd());
						}

						itro1 = itr1;
						itro2 = itr2;

						// pre swapping - remove itr1 and itr2 from map
						for ( k=0; k< NUM_TRI; ++k)
						{
							range[0] = mapNod.equal_range( *(*itr1)->Node(k) );
							for ( itrmap1=range[0].first; itrmap1!=range[0].second; itrmap1++)
							{
								if ( (*itrmap1).second == itr1 )
								{
									mapNod.erase( itrmap1);
									break;
								}
							}
							range[0] = mapNod.equal_range( *(*itr2)->Node(k) );
							for ( itrmap1=range[0].first; itrmap1!=range[0].second; itrmap1++)
							{
								if ( (*itrmap1).second == itr2 )
								{
									mapNod.erase( itrmap1);
									break;
								}
							}
						}
						if ( ! CheckSwapTriangles( *itr1, *itr2 ) )
							THROW_INTERNAL( "Recovery: Swap not possible !!!");
				
						SwapTriangles( *itr1, *itr2 );

						// post swapping - insert modified itr1 and itr2 into map
						for ( k=0; k< NUM_TRI; ++k)
						{
							mapNod.insert( multimap<HGrdPnt*, IterGCell>::value_type( *(*itr1)->Node(k), itr1) );
							mapNod.insert( multimap<HGrdPnt*, IterGCell>::value_type( *(*itr2)->Node(k), itr2) );
						}

						if ( stackCell.empty() )
						{
							itrcl = this->CellEnd();
							itrnb = this->CellEnd();

							if ( (itr = (*itr1)->NextCell( (*itrsg), this->CellEnd() )) != this->CellEnd())
							{
								itrcl = itr1;
								itrnb = itr;
							}

							if ( (itr = (*itr2)->NextCell( (*itrsg), this->CellEnd() )) != this->CellEnd())
							{
								itrcl = itr2;
								itrnb = itr;
							}

							itr1 = itrcl;
							itr2 = itrnb;
						}
						else
						{
							itr1 = stackCell.top();
							stackCell.pop();

							if ( stackCell.empty() )
								itr2 = (*itr1)->NextCell( *itrsg, this->CellEnd());
							else
								itr2 = (*itr1)->NextCell( *itrsg, stackCell.top());
						}
					}
					while ( itr1 != this->CellEnd() && itr2 != this->CellEnd() );

					(*itrsg)->mbtmp = true;
				}
				else
				{
					char	sbuf[1024];
					sprintf( sbuf, "recovery problem with seg (%lg, %lg) (%lg, %lg)", v1.X(), v1.Y(), v2.X(), v2.Y() );

					FILE *ff = fopen( "lstcell.plt", "wt");
					for ( itrtmp = lstCell.begin(); itrtmp != lstCell.end(); ++itrtmp)
					{
						(*(*itrtmp))->DumpTEC( ff);
					}
					fclose( ff);

					printf( "%s\n", sbuf);
					printf( "recovery lstCell.size = %d\n", lstCell.size() );

					(*itrsg)->mbtmp = true;

					FILE *f = fopen( "recovery.plt", "wt");
					ExportTECTmp( f);
					fclose( f);

					THROW_INTERNAL( "Recovery problem !!!");
				}
			}
			lstCell.erase( lstCell.begin(), lstCell.end() );
		}
}

void HGrid::Generate()
{
	try
	{
		InitTriangles();

		CheckBoundIntegr();

		FlagOuterTris();

		RemoveOuterTris();
	}
	catch ( Except *pe)
	{
		ASSERT( pe);
		delete pe;
	}
}

void HGrid::ExportTECTmp( FILE *f)
{
	IterGPnt	pntitr;
	Vect2D		vct;

	IterGCell	cellitr, itrcl;
	MGInt		id1, id2, id3, id4;
	MGInt		ltri, ltmp;
	map<void*,MGInt,less<void*> >	tmpMap;

	fprintf( f, "TITLE = \"surface\"\n");
	fprintf( f, "VARIABLES = \"X\", \"Y\"\n");
	fprintf( f, "ZONE T=\"TRIANGLES\", ");
	fprintf( f, "N=%2ld, ", mcolPnt.size() );
	fprintf( f, "E=%2ld, F=FEPOINT, ET=QUADRILATERAL C=BLACK\n ", ltri = (MGInt)mcolCell.size() );
	

	ltmp = 0;
	for ( pntitr = mcolPnt.begin(); pntitr != mcolPnt.end(); pntitr++)
	{
		ltmp++;
		vct = *(*pntitr);
		fprintf( f, "%20.10lg %20.10lg\n", vct.X(), vct.Y() );
		tmpMap.insert( map<void*,MGInt,less<void*> >::value_type( (void*)(*pntitr), ltmp ));
	}
	
	ltri = 0;
	for ( cellitr = mcolCell.begin(); cellitr != mcolCell.end(); cellitr++)
	{	
		ltri++;

		pntitr = (*cellitr)->Node(0);
		id1 = tmpMap[ (*pntitr)];

		pntitr = (*cellitr)->Node(1);
		id2 = tmpMap[ (*pntitr)];

		pntitr = (*cellitr)->Node(2);
		id4 = id3 = tmpMap[ (*pntitr)];
		
		fprintf( f, "%9ld %9ld %9ld %9ld\n", id1, id2, id3, id4 );
	}
}

void HGrid::CheckGrid()
{
	bool		bFound;
	IterGCell	itr, itrnb;

	for ( itr = mcolCell.begin(); itr != mcolCell.end(); ++itr)
	{
		for ( int i=0; i<NUM_TRI; ++i)
		{
			bFound = true;
			itrnb = (*itr)->Cell(i);
			if ( itrnb != this->CellEnd())
			{
				ASSERT( *itrnb );
				bFound = false;
				for ( int k=0; k<NUM_TRI; ++k)
				{
					if ( (*itrnb)->Cell(k) == itr )
						bFound = true;
				}
			}

			if ( ! bFound)
			{
				THROW_INTERNAL( "Cells pointers are not consistent !");
			}
		}
	}
}
