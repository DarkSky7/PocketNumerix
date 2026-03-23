// LineSeg.h: interface for the CLineSeg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINESEG_H__D301D2F9_E5EB_4A0D_AE7D_12CC6B2A3F69__INCLUDED_)
#define AFX_LINESEG_H__D301D2F9_E5EB_4A0D_AE7D_12CC6B2A3F69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FPoint.h"

class CLineSeg
{					// two points specifying a line segment
public:
	CLineSeg();
	CLineSeg( CLineSeg& seg );				// copy constructor
	CLineSeg( CFPoint& pt1, CFPoint& pt2 );
	virtual		~CLineSeg();

	bool		HasSlope( void )		{	return pt1.x != pt2.x;	}
	bool		IncMemberX( float x )	{	return GetLeft().x <= x  &&  x <= GetRight().x;		}
	bool		ExcMemberX( float x )	{	return GetLeft().x <  x  &&  x <  GetRight().x;		}
	CFPoint&	GetLeft( void )			{	return pt1;	}		// constructor guarantees
	CFPoint&	GetRight( void )		{	return pt2;	}		// left-right ordering

	bool		GetYatX( float xx, float& yy );
	bool		OverlapX( CLineSeg& seg, float& x1, float& x2 );
	CLineSeg*	operator+( CLineSeg& seg );		// NULL indicates failure

		// Accessors
	void		SetPt1y( float yIn )	{	pt1.y = yIn;	}
	void		SetPt2y( float yIn )	{	pt2.y = yIn;	}

	float		GetPseudoYintercept( void );	// caller must ensure HasSlope()
	float		GetSlope( void );				// caller must ensure HasSlope()

private:
	CFPoint		pt1;
	CFPoint		pt2;
};
#endif // !defined(AFX_LINESEG_H__D301D2F9_E5EB_4A0D_AE7D_12CC6B2A3F69__INCLUDED_)

