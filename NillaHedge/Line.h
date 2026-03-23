// Line.h: interface for the CLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINE_H__AB91BAF2_0B6F_4CA5_B238_9550EF756E8B__INCLUDED_)
#define AFX_LINE_H__AB91BAF2_0B6F_4CA5_B238_9550EF756E8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	"FPoint.h"

class CLine  
{					// point-slope form of an infinite line
public:
	CLine();
	virtual		CLine( CLine& aLine );					// copy constructor
	virtual		CLine( CGPoint& pt1, CFPoint& pt2 );	// two point specification
	virtual		~CLine();

		// accessors
	float		GetIntercept( void )		{	return	intercept;	}
	float		GetSlope( void )			{	return	slope;		}

		// overloaded operators
	CLine&		operator+( CLine& aLine );
	CLine&		operator-( CLine& aLine );


private:
	float		intercept;
	float		slope;
};
#endif // !defined(AFX_LINE_H__AB91BAF2_0B6F_4CA5_B238_9550EF756E8B__INCLUDED_)
