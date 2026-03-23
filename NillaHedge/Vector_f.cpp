// Vector_f.cpp: implementation of the CVector class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "NillaHedge.h"
#include "Vector_f.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction

CVector_f::CVector_f(
	unsigned short	len		 ) : nElems(len)
{	elems = new float[nElems];
	for ( unsigned short ii = 0; ii < nElems; ii++ )
	{	elems[ii] = 0;
	}
}

	// transform (unary apply) constructor
CVector_f::CVector_f(
	CVector_f&	av,
	float		func(float elem)	) : nElems(av.nElems)
{	elems = new float[nElems];
	for ( unsigned short ii = 0; ii < nElems; ii++ )
	{	elems[ii] = func( av[ii] );
	} 
}

CVector_f::~CVector_f( void )
{	if ( elems ) delete [] elems;
}
//--------------------------------------------------------------------
void		CVector_f::apply( float func( float elem ) )
{	for ( unsigned short ii = 0; ii < nElems; ii++ )
	{	elems[ii] = func( elems[ii] );
	}
}			// apply()
//--------------------------------------------------------------------
float		CVector_f::maxElem( void )
{	float	max = elems[0];
	for ( unsigned short ii = 1; ii < nElems; ii++ )
	{	if ( elems[ii] > max ) max = elems[ii];
	}
	return max;
}			// maxElem()
//--------------------------------------------------------------------
float		CVector_f::operator[]( unsigned short ii )
{	ASSERT( elems );
	ASSERT( ii < nElems );
	return elems[ii];
}			// operator[]()
//--------------------------------------------------------------------
