// Line.cpp: implementation of the CLine class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "NillaHedge.h"
#include "Line.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLine::CLine()
{
}

CLine::~CLine()
{
}

	// copy constructor
CLine::CLine( CLine& aLine )
{
	pt[0] = aLine.GetLeft();
	pt[1] = aLine.GetRight();
}

CLine::CLine( CFPoint& pt1, CFPoint& pt2 )
{
	if ( pt1.x < pt2.x )
	{		// store the points left to right
		pts[0] = pt1;
		pts[1] = pt2;
	}
	else
	{	pts[0] = pt2;
		pts[1] = pt1;
	}
}
//------------------------------------------------------
CLine*	CLine::operator+( CLine& aLine )
{	CFPoint		intersectPts[2];
	if (	GetRight() <= aLine.GetLeft()
		||  GetLeft() )
		
			slope = GetSlope();
	yCept = GetYintercept();
	aLineSlope = a
	if ( pt
}			// operator+()
//------------------------------------------------------
CLine&	CLine::operator-( CLine& aLine )
{
	if ( ! aLine.HasInfiniteSlope() )
	{	slope -= aLine.GetSlope();
		intercept -= aLine.GetIntercept();
	}
	return	*this;
}			// operator-()
//------------------------------------------------------
CLine::GetYintercept( void )
{

}			// 
//------------------------------------------------------
float	CLine::GetSlope( void )
{
	if ( ! HasInfiniteSlope() )
		return (pt2.y - pt1.y) / ( pt2.x - pt1.x );
		
}			// 
//------------------------------------------------------
/*
	infiniteSlope = ( pt2.x == pt1.x );
	if ( ! infiniteSlope )
	{	slope = (pt2.y - pt1.y) / ( pt2.x - pt1.x );
		intercept = pt1.y - slope * pt1.x;
	}
	else
		intercept = pt1.x;		// hence an x-intercept
*/
/*
	slope	  = aLine.GetSlope();
	intercept = aLine.GetIntercept();
	infiniteSlope = aLine.HasInfiniteSlope();
*/
/*
	if ( ! aLine.HasInfiniteSlope() )
	{	slope += aLine.GetSlope();
		intercept += aLine.GetIntercept();
	}
	return	*this;
*/