#include "StdAfx.h"
//#include "PNMX_InstallerDlg.h"
#include "StartRapi.h"
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// pThreadCtx is really a (CPNMX_InstallerDlg*)
// DWORD	WINAPI	InitRapi( void* pThreadCtx )
DWORD	WINAPI	InitDeviceConnectedMsg( void* pThreadCtx )
{	DWORD		dwTimeOut = INFINITE;
	HRESULT		hr = E_FAIL;
	CPNMX_InstallerDlg*	pCtx = (CPNMX_InstallerDlg*)pThreadCtx;
	RAPIINIT	rapiInit = pCtx->rapiInit;
/*
	int	sz = sizeof( rapiInit );
	ZeroMemory( &rapiInit, sz );
	rapiInit.cbSize = sz;
	hr = CeRapiInitEx( &rapiInit );
	if ( SUCCEEDED(hr) )
	{
*/
		DWORD dwRapiInit = 0;
		dwRapiInit = WaitForSingleObject( pCtx->rapiInit.heRapiInit, dwTimeOut );
		if ( dwRapiInit == WAIT_OBJECT_0 )
		{		// heRapiInit signaled:
				// set return error code to return value of RAPI Init function
			hr = rapiInit.hrRapiInit;
			LRESULT lr = pCtx->SendMessage( (UINT)WM_PNMX_INSTALLER_DEVICE_CONNECTED, 0, 0 );
#ifdef _DEBUG
			TRACE( _T("StartRapi.InitDeviceConnectedMsg: PostMessage(WM_PNMX_INSTALLER_DEVICE_CONNECTED) --> %d\n"), lr );
#endif
		}
		else if ( dwRapiInit == WAIT_TIMEOUT )
		{		// timed out: device is probably not connected
				// or not responding
			hr = HRESULT_FROM_WIN32( ERROR_TIMEOUT );
		}
		else
		{		// WaitForSingleObject failed
			hr = HRESULT_FROM_WIN32( GetLastError() );
		}

		if ( FAILED(hr) )
			CeRapiUninit();
//	}
	return	hr;
}
//----------------------------------------------------------------------------
/*
//	The following code is from:
//	 http://msdn.microsoft.com/library/default.asp?url=/library/en-us/wceconct/html/_wcesdk_Initializing_and_Terminating_Remote_Applications.asp
#define ARRAYSIZE( hArray )   ( sizeof( hArray ) / sizeof( HANDLE ) )
enum
{
	WAD_ALLINPUT    = 0x0000,
	WAD_SENDMESSAGE = 0x0001,
};
//----------------------------------------------------------------------------
DWORD	WaitAndDispatch( DWORD nCount, HANDLE *phWait, DWORD dwTimeout, UINT uFlags )
{	DWORD dwObj;
	DWORD dwStart = GetTickCount();
	DWORD dwTimeLeft = dwTimeout;

	while ( 1 )
	{	dwObj = MsgWaitForMultipleObjects( nCount, phWait, FALSE, dwTimeLeft,
			(uFlags & WAD_SENDMESSAGE) ? QS_SENDMESSAGE : QS_ALLINPUT );

		if ( dwObj == (DWORD)(-1) )
		{
			dwObj = WaitForMultipleObjects( nCount, phWait, FALSE, 100 );
			if ( dwObj == (DWORD)(-1) )
				break;
		}
		else
		{
			if ( dwObj == WAIT_TIMEOUT )
				break;					// timed-out, don't send message
		}
			// are we below the number of objects we're waiting on?
		if ( (UINT)(dwObj - WAIT_OBJECT_0) < nCount )
			break;							// not done gating yet

		MSG msg;
		if ( uFlags & WAD_SENDMESSAGE )
			PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE );
		else
		{	while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
				DispatchMessage( &msg );
		}
		if ( dwTimeout != INFINITE )
		{	dwTimeLeft = dwTimeout - ( GetTickCount() - dwStart );
			if ( (int)dwTimeLeft < 0 )
				break;
		}
	}
	return	dwObj;
}			// WaitAndDispatch()
//----------------------------------------------------------------------------
HRESULT	InitRapi( HANDLE hExit )
{
	RAPIINIT	rapiinit = { sizeof( RAPIINIT ) };
	HRESULT		hResult = CeRapiInitEx( &rapiinit );
	if ( FAILED(hResult) )
		return	hResult;

	HANDLE	hWait[] = { hExit, rapiinit.heRapiInit };
	enum { WAIT_EXIT=WAIT_OBJECT_0, WAIT_INIT };
	DWORD	dwObj = WaitAndDispatch( ARRAYSIZE(hWait), hWait, 1000, 1 );
		// Event signaled by RAPI
	if ( dwObj == WAIT_INIT )           
	{		// If the connection failed, uninitialize the Windows CE RAPI.
		if ( FAILED( rapiinit.hrRapiInit ) )
			CeRapiUninit();
		return	rapiinit.hrRapiInit;
	}

		// Either event signaled by user or a time-out occurred.
	CeRapiUninit();
	if ( dwObj == WAIT_EXIT )
		return	HRESULT_FROM_WIN32( ERROR_CANCELLED );
	return	E_FAIL;
}			// InitRapi()
*///----------------------------------------------------------------------------
