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
/*
BOOL	DateTimeCtrl_SetRange( HWND hWnd, COleDateTime* pMinTime, COleDateTime* pMaxTime )
{	SYSTEMTIME sysTime[2];
	ASSERT(::IsWindow(hWnd));
	ASSERT(pMinTime == NULL || pMinTime->GetStatus() != COleDateTime::invalid);
	ASSERT(pMaxTime == NULL || pMaxTime->GetStatus() != COleDateTime::invalid);
	WPARAM wFlags = 0;
	if (pMinTime != NULL && pMinTime->GetStatus() != COleDateTime::null)
	{	if (pMinTime->GetAsSystemTime(sysTime[0]))
			wFlags |= GDTR_MIN;
	}
	if (pMaxTime != NULL && pMaxTime->GetStatus() != COleDateTime::null)
	{	if (pMaxTime->GetAsSystemTime(sysTime[1]))
			wFlags |= GDTR_MAX;
	}
	return (BOOL) ::SendMessage(hWnd, DTM_SETRANGE, wFlags, (LPARAM) sysTime);
}			// SetRange()
*/
//----------------------------------------------------------------------------------------
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
