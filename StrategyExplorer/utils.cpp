// utils.cpp
#include "StdAfx.h"
#include <stdlib.h>				// rand, srand
#include <math.h>				// sqrt

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
double	Discretize( double	dd )
{
	if		( dd < 0.001 )
	{	if		(     0.0001 <= dd  &&  dd <   0.0002 )	dd =	  0.0002;
		else if (     0.0002 <= dd  &&  dd <   0.0005 )	dd =	  0.0005;
		else if	(     0.0005 <= dd					  )	dd =	  0.001;
	}
	else if ( dd < 0.01 )
	{	if		(     0.001 <= dd  &&  dd <    0.002 )	dd =	  0.002;
		else if (     0.002 <= dd  &&  dd <    0.005 )	dd =	  0.005;
		else if	(     0.005 <= dd					 )	dd =	  0.01;
	}
	else if ( dd < 0.1 )
	{	if		(     0.01 <= dd  &&  dd <     0.02 )	dd =	  0.02;
		else if (     0.02 <= dd  &&  dd <     0.05 )	dd =	  0.05;
		else if	(     0.05 <= dd					)	dd =	  0.1;
	}
	else if ( dd < 1.0 )
	{	if		(     0.1  <= dd  &&  dd <	   0.2  )	dd =      0.2;
		else if	(     0.2  <= dd  &&  dd <     0.5  )	dd =      0.5;	
		else if (     0.5  <= dd					)	dd =      1.0;
	}
	else if ( dd < 10.0 )
	{	if		(     1.0  <= dd  &&  dd <     2.0  )	dd =      2.0;
		else if (     2.0  <= dd  &&  dd <     5.0  )	dd =      5.0;
		else if (     5.0  <= dd					)	dd =     10.0;
	}
	else if ( dd < 100.0 )
	{	if		(    10.0  <= dd  &&  dd <    20.0  )	dd =     20.0;
		else if (    20.0  <= dd  &&  dd <    50.0  )	dd =     50.0;
		else if (    50.0  <= dd					)	dd =    100.0;
	}
	else if ( dd < 1000.0 )
	{	if		(   100.0  <= dd  &&  dd <   200.0  )	dd =    200.0;
		else if (   200.0  <= dd  &&  dd <   500.0  )	dd =    500.0;
		else if (   500.0  <= dd					)	dd =   1000.0;
	}
	else if ( dd < 10000.0 )
	{	if		(  1000.0  <= dd  &&  dd <  2000.0  )	dd =   2000.0;
		else if (  2000.0  <= dd  &&  dd <  5000.0  )	dd =   5000.0;
		else if (  5000.0  <= dd					)	dd =  10000.0;
	}
	else
	{	if		( 10000.0  <= dd  &&  dd < 20000.0  )	dd =  20000.0;
		else if ( 20000.0  <= dd  &&  dd < 50000.0  )	dd =  50000.0;
		else if ( 50000.0  <= dd  					)	dd = 100000.0;
	}
	return	dd;
}			// Discretize()
//---------------------------------------------------------------------------
/*
// PFWGetFocus
//  Return handle to window with focus, regardless of thread
//      (based largely on Chris Branch (Windows Developer's Journal,
//       July 2000, p. 40)
//  See MSDN article Q97925 for info about threads and SetForeground
// Under Windows CE, ::AttachThreadInput seems unavailable, but this whole
//  function seems unneeded, because::GetFocus works across processes
HWND	PFWGetFocus( void )
{
    DWORD OurID = ::GetCurrentThreadId();
	DWORD ForegroundID = GetWindowThreadProcessId( ::GetForegroundWindow(), NULL );

		// NOTE: ID order seems to not matter in either call to ::AttachThreadInput
    if ( ForegroundID != OurID )
        AttachThreadInput( OurID, ForegroundID, TRUE );
    HWND hWnd = ::GetFocus();
    if ( ForegroundID != OurID )
        AttachThreadInput( OurID, ForegroundID, FALSE );
    return	hWnd;
}   // PFWGetFocus 
*/
//----------------------------------------------------------------------------------------
/*
HWND		GlobalGetFocus( void )
{		// remember the focus window of the current thread
	HWND hWndLocalFocus = ::GetFocus();

		// find foreground window
	HWND hWndFore = ::GetForegroundWindow();
	if ( hWndFore == NULL )
		return	NULL;

		// get IDs of the current thread and the thread that
		// owns foreground window
	DWORD dwCurrID = ::GetCurrentThreadId();
	DWORD dwForeID = ::GetWindowThreadProcessId(hWndFore, NULL);

		// if the current thread owns the foreground window then just
		// return hWndLocalFocus
	if ( dwForeID == dwCurrID )
		return	hWndLocalFocus;

		// attach input states of the current thread and the foreground thread
	if ( ! AttachThreadInput( dwCurrID, dwForeID, TRUE ) )
		return	NULL;

		// now the current thread and the foreground thread have common
		// input state and we can query for the focus window
	HWND hWndGlobalFocus = GetFocus();

		// detach threads
	AttachThreadInput( dwCurrID, dwForeID, FALSE );

		// restore local focus
	SetFocus( hWndLocalFocus );
	return	hWndGlobalFocus;
}			// GlobalGetFocus()
*/
//----------------------------------------------------------------------------
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
