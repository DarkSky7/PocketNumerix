// Polyline.h: interface for the CPolyline class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POLYLINE_H__494DE183_D29D_48B5_A164_2B785584A75B__INCLUDED_)
#define AFX_POLYLINE_H__494DE183_D29D_48B5_A164_2B785584A75B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LineSeg.h"

class CPolyline
{
public:
	CPolyline();
	virtual			~CPolyline();
	void			InsertPoint( CFPoint& anFPoint );	// if it's not already there
	void			BoostPoint( CFPoint& cfp );			// adds cfp.y to the poly point matching cfp.x
	void			BoostSegment( CLineSeg& seg1, CFPoint& skipPt );
		// Accessors
	short			GetPointCount( void )		{	return pts.GetSize();	}
	CLineSeg*		GetSegment( short ii );		// ii is 0-based
	void			Dump( void );

protected:
	CObArray		pts;		// at x=0; at each of 3 strikes; at a big number
};
#endif // !defined(AFX_POLYLINE_H__494DE183_D29D_48B5_A164_2B785584A75B__INCLUDED_)

//	void			BoostPoint( CFPoint& cfp );
