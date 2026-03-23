// FPoint.h: interface for the CFPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FPOINT_H__EA7B8BDF_6B68_46F5_96E2_3B39D9BC1B8F__INCLUDED_)
#define AFX_FPOINT_H__EA7B8BDF_6B68_46F5_96E2_3B39D9BC1B8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFPoint : public CObject 
{
public:
	CFPoint();
	CFPoint( float xin, float yin );
	CFPoint( CFPoint& pt1 );


	virtual ~CFPoint();
	void	operator=( CFPoint& pt1 );

//	float	GetX( void )	{	return	x;	}
//	float	GetY( void )	{	return	y;	}

//protected:
	float	x;
	float	y;
};

#endif // !defined(AFX_FPOINT_H__EA7B8BDF_6B68_46F5_96E2_3B39D9BC1B8F__INCLUDED_)
