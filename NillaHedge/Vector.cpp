// Vector.cpp: implementation of the CVector class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "NillaHedge.h"
#include "Vector.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

CVector::CVector(
	unsigned short	len		 ) : nElems(len)
{	elems = new double[nElems];
	for ( unsigned short ii = 0; ii < nElems; ii++ )
	{	elems[ii] = 0;
	}
}

	// transform (unary apply) constructor
CVector::CVector(
	CVector&	av,
	double		func(double elem)	) : nElems(av.nElems)
{	elems = new double[nElems];
	for ( unsigned short ii = 0; ii < nElems; ii++ )
	{	elems[ii] = func( av[ii] );
	} 
}

CVector::~CVector( void )
{	if ( elems ) delete [] elems;
}
//--------------------------------------------------------------------
void		CVector::apply( double func( double elem ) )
{	for ( unsigned short ii = 0; ii < nElems; ii++ )
	{	elems[ii] = func( elems[ii] );
	}
}			// apply()
//--------------------------------------------------------------------
double		CVector::maxElem( void )
{	double	max = elems[0];
	for ( unsigned short ii = 1; ii < nElems; ii++ )
	{	if ( elems[ii] > max ) max = elems[ii];
	}
	return max;
}			// maxElem()
//--------------------------------------------------------------------
double		CVector::operator[]( unsigned short ii )
{	ASSERT( elems );
	ASSERT( ii < nElems );
	return elems[ii];
}			// operator[]()
//--------------------------------------------------------------------
