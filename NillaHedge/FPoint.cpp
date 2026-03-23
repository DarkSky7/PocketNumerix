// FPoint.cpp: implementation of the CFPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nillahedge.h"
#include "FPoint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFPoint::CFPoint()
{
}

CFPoint::~CFPoint()
{
}

CFPoint::CFPoint( float xin, float yin ) : x(xin), y(yin)
{
}

CFPoint::CFPoint( CFPoint& pt1 ) : x(pt1.x), y(pt1.y)
{
}

void	CFPoint::operator=( CFPoint& pt1 )
{
	x = pt1.x;
	y = pt1.y;

}

