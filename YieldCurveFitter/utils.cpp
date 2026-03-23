// utils.cpp
#include "StdAfx.h"
#include <stdlib.h>				// rand, srand
#include <math.h>				// sqrt

char*		GetThinString( CString cs )
{	int	len = cs.GetLength();
	char*	res = new char[ len+1 ];

	for ( unsigned short ii = 0; ii < len; ii++ )
	{
		wchar_t us = cs.GetAt( ii );
		*(res+ii) = (char)us;
	}
	*(res+len) = '\0';
	return	res;
}			// GetThinString()
//----------------------------------------------------------------------------------------
long			PackOleDate( COleDateTime aDate )
{
	int		yr = aDate.GetYear();				// 15 bits
	int		mo = aDate.GetMonth();				//  4 bits
	int		da = aDate.GetDay();				//  5 bits
//	yr &= 0x7FFF;								// range = 0 .. 32767
	return	(yr << 9)  |  (mo << 5)  |  da;
}			// PackOleDate()
//----------------------------------------------------------------------------------------
COleDateTime	UnpackOleDate( long	packedOleDate )
{	COleDateTime	res;
	int		da = packedOleDate & 0x1F;			//  5 bits
				 packedOleDate >>= 5;						// shift 5 bits off right
	int		mo = packedOleDate & 0xF;			//  4 bits
				 packedOleDate >>= 4;						// shift 4 bits off right
	int		yr = packedOleDate & 0x7FFF;		// 15 bits
	res.SetDate( yr, mo, da );
	return	res;
}			// UnpackOleDate()
//----------------------------------------------------------------------------------------
double	expRand( double lambda )
{	double uu;
	do	uu = (double)rand();
	while ( uu == 0.0 );
	return -log( uu / RAND_MAX ) / lambda;
}			// expRand()
//----------------------------------------------------------------------------------------
/*
double	BoxMuller( void )
{
	srand( (unsigned)time( NULL ) );
	double aa = rand() / RAND_MAX;
	double bb = rand() / RAND_MAX;
	double pi = 3.14159265358979323846;
	return	sqrt( -2.0 * log((double)aa)) * cos( 2*pi*(double)bb );
}			// BoxMuller()
*/
//----------------------------------------------------------------------------------------
