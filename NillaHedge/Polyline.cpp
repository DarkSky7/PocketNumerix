// Polyline.cpp: implementation of the CPolyline class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "NillaHedge.h"
#include "Polyline.h"
#include "FPoint.h"
#include "LineSeg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPolyline::CPolyline()
{
}

CPolyline::~CPolyline()
{
	int	nPts = pts.GetSize();
	for ( short ii = nPts - 1; ii >= 0; ii-- )
	{	CFPoint*	cfp = (CFPoint*)pts[ii];
		pts.RemoveAt( ii );
		delete	cfp;
	}
}			// ~Polyline()
//--------------------------------------------------------------------
void	CPolyline::Dump( void )
{
	short	nPts = pts.GetSize();
#ifdef _DEBUG
//	TRACE( _T("Polyline::Dump: %d points\n"), nPts );
#endif
	for ( short ii = 0; ii < nPts; ii++ )
	{	CFPoint*	pt = (CFPoint*)pts[ii];
#ifdef _DEBUG
//		TRACE( _T("   pts[%d]=( %g, %g )\n"), ii, pt->x, pt->y );
#endif
	}
}			// Dump()
//--------------------------------------------------------------------
void	CPolyline::InsertPoint( CFPoint& anFPoint )
{	CFPoint*	cfp = NULL;			// possible return value
	short	nPts = pts.GetSize();
	if ( nPts < 1 )
	{		// insert both ends of seg1
		cfp = new CFPoint( anFPoint );
#ifdef _DEBUG
//		TRACE( _T("Polyline::InsertPoint(1): Initial point, x=%g, y=%g\n"),
//				cfp->x, cfp->y );
#endif
		pts.InsertAt( 0, cfp );
#ifdef _DEBUG
//		Dump();
#endif
		return;
	}
		// there are existing points in the polyline
		// insert the point in ascending order, by x-coordinate
	float	cx = anFPoint.x;

		// handle new right terminus
	CFPoint*	rtPt = (CFPoint*)pts[nPts-1];
	if ( rtPt->x < cx )
	{	cfp = new CFPoint( cx, anFPoint.y );
#ifdef _DEBUG
//		TRACE( _T("Polyline::InsertPoint(2): New right terminus, x=%g, y=%g\n"),
//				cfp->x, cfp->y );
#endif
		pts.InsertAt( nPts, cfp );
#ifdef _DEBUG
//		Dump();
#endif
		return;
	}
		// handle new left terminus
	CFPoint*	leftPt = (CFPoint*)pts[0];
	if ( cx < leftPt->x )
	{	cfp = new CFPoint( cx, anFPoint.y );
#ifdef _DEBUG
//		TRACE( _T("Polyline::InsertPoint(3): New left terminus, x=%g, y=%g\n"),
//				cfp->x, cfp->y );
#endif
		pts.InsertAt( 0, cfp );
#ifdef _DEBUG
//		Dump();
#endif
		return;
	}

		// scan through pts from high X (high index) to low X
		// inserting colinear points between existingPolyline points
	bool	okay;
	for ( short ii = nPts-1; ii > 0; ii-- )
	{		// construct a segment between pts[ii-1] and pts[ii]
		rtPt = (CFPoint*)pts[ii];						// right endpoint
		if ( cx == rtPt->x )
			return;										// cfp is already in the Polyline
		leftPt = (CFPoint*)pts[ii-1];					// left endpoint
		if ( leftPt->x < cx  &&  cx < rtPt->x )
		{	float	cy;
			CLineSeg	polySeg( *leftPt, *rtPt );
			okay = polySeg.GetYatX( cx, cy );
			if ( okay )
			{	cfp = new CFPoint( cx, cy );
#ifdef _DEBUG
//				TRACE( _T("Polyline::InsertPoint(4): New %d point, x=%g, y=%g\n"),
//						ii, cfp->x, cfp->y );
#endif
				pts.InsertAt( ii, cfp );
				break;									// we're done
			}
		}
	}
//	Dump();
	return;
}			// InsertPoint()
//--------------------------------------------------------------------
void	CPolyline::BoostPoint( CFPoint& cfp )
{	CFPoint*	polyPt = NULL;		// possible return value
		// scan the pts array, boosting the y-value of
		// the poly point matching cfp's x-Value by cfp's y-Value
	short	nPts = pts.GetSize();
	short	ii = nPts - 1;
	while ( ii >= 0 )
	{	polyPt = (CFPoint*)pts[ii];
		if ( cfp.x == polyPt->x )
		{	polyPt->y += cfp.y;
#ifdef _DEBUG
//			TRACE( _T("Polyline::BoostPoint:  boosted point[%d], at x=%g to y=%g\n"),
//					ii, cfp.x, polyPt->y );
#endif
			break;
		}
		ii--;
	}
//	Dump();
	return;			//	polyPt;
}			// BoostPoint()
//--------------------------------------------------------------------
void	CPolyline::BoostSegment( CLineSeg& seg1, CFPoint& skipPt )
{	CFPoint*	polyPt = NULL;		// possible return value
		// scan the pts array, boosting the y-value of
		// the poly point matching cfp's x-Value by cfp's y-Value
	short	nPts = pts.GetSize();
	short	ii = 0;
	while ( ii < nPts )
	{	polyPt = (CFPoint*)pts[ii];
		if ( seg1.IncMemberX( polyPt->x )  &&  skipPt.x != polyPt->x )
		{	float	yInc;
			bool okay = seg1.GetYatX( polyPt->x, yInc );
			if ( okay )
			{	polyPt->y += yInc;
#ifdef _DEBUG
//				TRACE( _T("Polyline::BoostPoint:  boosted point at ii=%d, at x=%g to y=%g\n"),
//						ii, polyPt->x, polyPt->y );
#endif
			}
		}
		ii++;
	}
//	Dump();
	return;
}			// BoostPoint()
//--------------------------------------------------------------------
CLineSeg*	CPolyline::GetSegment( short ii )
{		// ii is 0-based, representing the left endpoint of the segment
	CLineSeg*	seg = NULL;
	if ( ii < pts.GetSize() - 1 )
		seg = new CLineSeg( *(CFPoint*)pts.GetAt(ii), *(CFPoint*)pts.GetAt(ii+1) );
	return	seg;
}			// GetSegment()
//--------------------------------------------------------------------
