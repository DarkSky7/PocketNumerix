// LineSeg.cpp: implementation of the CLineSeg class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "NillaHedge.h"
#include "LineSeg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLineSeg::CLineSeg()
{
}

CLineSeg::~CLineSeg()
{
}

CLineSeg::CLineSeg( CLineSeg& seg )
{
	ASSERT( seg.GetLeft().x <= seg.GetRight().x );
	pt1 = seg.GetLeft();
	pt2 = seg.GetRight();
}			// copy constructor
//--------------------------------------------------------------
CLineSeg::CLineSeg( CFPoint& pt1in, CFPoint& pt2in )
{
	if ( pt1in.x < pt2in.x )
	{		// store the points left to right
		pt1 = pt1in;	// pt1 on Left
		pt2 = pt2in;	// pt2 on Right
	}
	else
	{	pt1 = pt2in;	// pt2 on Left
		pt2 = pt1in;	// pt1 on Right
	}
}			// two-point constructor
//--------------------------------------------------------------
float	CLineSeg::GetSlope( void )
{
	ASSERT( HasSlope() );
	return	( pt1.y - pt2.y ) / ( pt1.x - pt2.x );
}			// GetSlope()
//--------------------------------------------------------------
float	CLineSeg::GetPseudoYintercept( void )
{		// don't care if the segment actually crosses the Y-axis or not,
		// just need that portion of the gradient form of the segment's
		// equation						XXX this is untested...
	return	pt1.y - GetSlope() * pt1.x;
}			// GetPseudoYintercept()
//--------------------------------------------------------------
bool		CLineSeg::GetYatX( float xx, float& yy )
{		// 
	bool okay = HasSlope()  &&  IncMemberX(xx);
	if ( okay )
	{	yy = GetLeft().y + GetSlope() * ( xx - GetLeft().x );
#ifdef _DEBUG
//		TRACE(_T("LineSeg::GetYatX: yy=%g <-- xx=%g, Lx=%g, Ly=%g, Rx=%g, Ry=%g\n"),
//			yy, xx, GetLeft().x, GetLeft().y, GetRight().x, GetRight().y );
#endif
	}
	return	okay;
}			// GetYatX
//--------------------------------------------------------------
bool	CLineSeg::OverlapX( CLineSeg& seg, float& x1, float& x2 )
{		// find two x values bracketing the inclusive overlap
		// of two line segments.  Overlap is defined by the
		// X-axis projections of the segments.
		// 
	bool	found = false;				// the return value
	float	slx = seg.GetLeft().x;
	float	srx = seg.GetRight().x;
	float	tlx =	  GetLeft().x;
	float	trx =	  GetRight().x;
	if ( tlx <= srx  &&  srx <= trx )
	{		// found overlap of this LineSeg with the Right end of seg
			// the left end of the overlap seg is the greater of tlx & slx
		x1 = srx;
		x2 = tlx > slx ? tlx : slx;
		found = true;
	}
	else if ( tlx <= slx  &&  slx <= trx )
	{		// found overlap of this LineSeg with the Left end of seg
			// the right end of the overlap seg is the lesser of trx & srx
		x1 = slx;
		x2 = trx < srx ? trx : srx;
		found = true;		
	}
	return	found;
}			// OverlapX()
//--------------------------------------------------------------
CLineSeg*	CLineSeg::operator+( CLineSeg& seg )
{		// return a new line segment which has slope resulting from the addition
		// of seg and this segment's slope and the extent of their overlap
		// on the X-axis
		// protect the GetSlope() calls
	CLineSeg*	sumSeg = NULL;					// failure indicator
	if ( HasSlope()  &&  seg.HasSlope() )
	{	float	x1, x2;
			// make sure there's overlap before creating any segments
		if ( OverlapX( seg, x1, x2 ) )
		{		// we found X overlap -> construct a segment with Overlap extents
				// and slope of the segments added together
			float	sumSlope = GetSlope() + seg.GetSlope();
				// calculate y values
			float	sumIntercept = GetPseudoYintercept() + seg.GetPseudoYintercept();
			float	y1 = x1 * sumSlope + sumIntercept;
			float	y2 = x2 * sumSlope + sumIntercept;
			CFPoint	pt1( x1, y1 );
			CFPoint	pt2( x2, y2 );
			sumSeg = new CLineSeg( pt1, pt2 );
		}
	}
	return	sumSeg;
}			// operator+()
//--------------------------------------------------------------
