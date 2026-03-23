// service.h

#include <windows.h>
#include <strsafe.h>

// How big is the state space? Need full option chain download
// QUEUE entries == PROFILE entries plus ... 
// 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/*
* pageFetchQueue == { stkSym date }, where date can be NULL or an empty string
* one thread handles all page fetches
*   maintains a queue of pages to fetch
*   work increment == processing one Yahoo page
*
* if the pageFetchQueue is empty
*	 all enabled issues in the profile are pushed onto the queue { stkSymX "" }
* top entry popped
* specified yahoo options page downloaded
* XML parsed
* entries for pages with later expiration dates are appended to the queue { stkSymX dateY }
* XML entries for options on this page are stuffed into the local dataase
*
* if there's no evaluation thread, launch one after the pageFetch thread completes one queue full
* if the profile is modified and a new issue is added, push it immediately at the top of the fetch queue
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* one thread operating on one evalQueue for each issue of interest
* work increment == each thread processing one queue entry
* evalQueue ==
*	{ V dM dN }				all vetical strategies; pushes { V dM dN outlookX } onto the queue
*	{ V dM dN neutral }		neutral strategies; pushes { V dM dN spreadY } onto the queue
*	{ V dM dN spread  }		pushes { V @ dX strangle }, { V @ dY strangle }, ... onto the queue
*							where dX, dY, ... are expiration dates between dM and dN inclusive
*
* One strike: Single options, SynShortStock, SynLongStock, Strip, Strap, LongStraddle, ShortStraddle
* Evaluate at date dX in one work increment
*
* Two strikes: spread = BullCall, BullPut, BearCall, BearPut, LongStrangle, ShortStrangle
* { V @ d1 spread x1 )			where x1 is the lowest strike to consider; high strike variable
*
* Three strikes: spread = ShortCallButterfly, ShortPutButterfly, LongCallButterfly, LongPutButterfly, 
*		IronButterfly, ReverseIronButterfly, LongCallLadder, ShortCallLadder, LongPutLadder, ShortPutLadder
* { V @ d1 spread  * x2 x3 )	where x2 x3 spec a strangle and * specs the clipping strike
* { V @ d1 spread x1 x2  * )	where x1 x2 spec a strangle and * specs the clipping strike
* { V @ d1 spread  * x2  * )	central straddle with variable high & low strike
*
* Four strikes: IronCondor, ReverseIronCondor, LongCallCondor, LongPutCondor, ShortCallCondor, ShortPutCondor
* { V @ d1 spread  * x2 x3  * )	central strangle with high and low variable options
* { V @ d1 spread  * x2 x3 x4 )	central strangle with variable low strike
* { V @ d1 spread x1 x2 x3  * )	central strangle with variable high strike
*
* for later... calendar spreads
* { C dM dN }					all calendar strategies; pushes { C dM dN outlookX } onto the queue
* { C dM dN bullish }			bullish strategies;		 pushes { C dM dN spreadY } onto the queue
* { C dM dN straddle }			pushes { C dX dX straddle }, { C dY dY straddle }, ... onto the queue
*
* if the evalQueue for this issue is empty
* {	    // fill the evalQueue
*    for all issues in the profile
*	    for all marketOutlook/dateRanges of interest
*		   append issue/outlook/dateRanges to the end of the evalQueue
* }
* for all possible instances of spread
* {		// XXX is this one increment of work?
*	evaluate probability of closing in the money and other greeks
* 	stuff result into a priority queue ordered by evaluation function
* }
*/

#define SVCNAME _T("OptionMiner")
#pragma comment(lib, "advapi32.lib")
#define SVC_ERROR		0							// not sure what this should be
//#define SVC_ERROR		((DWORD)0xC0020001L)		// an alternative

SERVICE_STATUS			gSvcStatus; 
SERVICE_STATUS_HANDLE	gSvcStatusHandle; 
HANDLE					ghSvcStopEvent = NULL;

VOID	SvcInstall( void );
VOID	WINAPI SvcCtrlHandler( DWORD ); 
VOID	WINAPI SvcMain( DWORD, LPTSTR * ); 
VOID	ReportSvcStatus( DWORD, DWORD, DWORD );
VOID	SvcInit( DWORD, LPTSTR * ); 
VOID	SvcReportEvent( LPTSTR );
VOID	SvcWork( void );							// one element of work

