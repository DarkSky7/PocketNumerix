// StrategyExplorer.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"
#include "StrategyExplorer.h"
//#include "StrategyExplorerDlg.h"
#include "OneStk4OptStrategy.h"
#include "StrategyExplorerHiResDlg.h"
#include "Registry.h"
#include "GetDeviceInfo.h"
#include "utils.h"
#include <iostream>
#include <fstream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStrategyExplorerApp

BEGIN_MESSAGE_MAP( CStrategyExplorerApp, CWinApp )
	//{{AFX_MSG_MAP(CStrategyExplorerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStrategyExplorerApp construction

CStrategyExplorerApp::CStrategyExplorerApp( void )
	: CWinApp()
	, dv( 0 )
	, na( NULL )
	, em( NULL )
	, uu( NULL )
	, hu( NULL )
{	OSVERSIONINFO osVer;
	GetVersionEx( &osVer );
	m_osVer = osVer.dwMajorVersion;
}			// CStrategyExplorerApp()

CStrategyExplorerApp::~CStrategyExplorerApp( void )
{
	if ( na ) delete [] na;
	if ( em ) delete [] em;
	if ( uu ) delete [] uu;
	if ( hu ) delete [] hu;
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CStrategyExplorerApp object

CStrategyExplorerApp	theApp;

/////////////////////////////////////////////////////////////////////////////
// CStrategyExplorerApp initialization

BOOL	CALLBACK	AppSearcher( HWND hWnd, LPARAM lParam )
{
/*
	DWORD	dwAnswer;
	if (	! SendMessageTimeout( hWnd, WM_STRATEGYEXPLORER_ALREADY_RUNNING, 0, 0, 
								SMTO_BLOCK | SMTO_ABORTIFHUNG, 500, &dwAnswer )
		 ||  dwAnswer != WM_STRATEGYEXPLORER_ALREADY_RUNNING )
*/
	LRESULT	res = SendMessage( hWnd, WM_STRATEGYEXPLORER_ALREADY_RUNNING, 0, 0 );
    if ( res == WM_STRATEGYEXPLORER_ALREADY_RUNNING )
	{		// Application responded to our message: grab the HWND for further processing
		*(HWND*)lParam = hWnd;
		return	FALSE;							// found it
	}
	return	TRUE;			// indicates to EnumWindows that we can stop looking
}			// AppSearcher()
//---------------------------------------------------------------------------
BOOL CStrategyExplorerApp::InitInstance( void )
{		// If SX is already running, then focus on the window, and exit.
	bool	match = false;
	hMutex = CreateMutex(0, FALSE, _T("PocketNumerix/StrategyExplorer/Mutex"));
	if ( hMutex == NULL )
	{	DWORD	err = GetLastError();
		if ( err == ERROR_ALREADY_EXISTS  ||  err == ERROR_ACCESS_DENIED )
		{		// we already have an instance running somewhere
			HWND	hThisApp = NULL;

				// Call out for each and every window
			EnumWindows( AppSearcher, (LPARAM)&hThisApp );

				// Bring window to front
			if ( hThisApp )
				SetForegroundWindow( hThisApp );
		}
		exit( 0 );
	}
	unsigned long packedDate = 0;
	wchar_t* regKey = _T("SOFTWARE\\PocketNumerix\\StrategyExplorer\\Init");			// date first used
	COleDateTime now = COleDateTime::GetCurrentTime();
		// cannot use "dt = now" on PPC02 because of the year 2000 bug in MFC4, hence dt = GetCurrentTime() instead
	COleDateTime dt = COleDateTime::GetCurrentTime();				// if we've already set a date, override the 'now' value in dt
	bool success = ReadRegBinary( HKEY_LOCAL_MACHINE, regKey, (BYTE*)&packedDate, 3 );
	if ( packedDate == NULL )							// success is not a reliable measure due to return codes 87 & 234
	{		// no such registry key, create one with today's date
		packedDate = PackOleDate( dt );
		success = WriteRegBinary( HKEY_LOCAL_MACHINE, regKey, (BYTE*)&packedDate, 3 );
	}
	else												// already initialized
		dt = UnpackOleDate( packedDate );				// get the date

#ifdef	CheckLicense
		// generate an exponential random variable with low initial likelihood, increasing as span increases
	srand( 25051 );													// not initializing srand causes first rand() == 0
	COleDateTimeSpan span = now - dt;
	double lambda = 1.0 / ( (double)span.GetDays() + 0.1 );			// span can be zero - prevent divide by zero
	if ( expRand(lambda) > 10.0 )
	{		// attempt to thwart software piracy
			// get or set last run date
		char* sn = GetThinString( GetSerialNumber() );
		if ( sn )
		{	if ( GetDeviceInfo( dv, uu, hu, na, em ) == 0 )			// really license info
				match = ( strcmp(sn,(char*)uu) == 0 );
			delete [] sn;
			if ( ! match )
					// the serial number of the device does not match the one in the software license
				ExitPrep( -1 );										// serial number did't match
#if	( _WIN32_WCE >= 0x500 )
			char*	appData = "pknmxStrategyExplorer";
			DWORD	appLen = strlen( appData );
			BYTE	huid[48];
			DWORD	lenOut = 48;
			HRESULT hr = GetDeviceUniqueID( (BYTE*)appData, appLen, 1, huid, &lenOut );
				// HEX ANSI  <--  BYTE* (actually a very long integer)
			if ( hr == S_OK )
			{	
#if DEBUG_HUID
				MessageBox( GetParent(0), _T("GetDeviceUniqueID"), _T("S_OK"), 0 );
#endif
				char* huidHex = MakeHexString( huid, lenOut );		// uuidHex is little-endian reversed
				match = ( strcmp(huidHex,(char*)hu) == 0 );
#ifdef _DEBUG
				CString csHu( hu );
				CString csHex( huidHex );
				TRACE( _T("StrategyExplorerApp::InitInstance: hu='%s', huidHex='%s'\n"), csHu, csHex );
#endif
				delete [] huidHex;
				if ( ! match )
					ExitPrep( -2 );
			}
#endif
		}
		else
			ExitPrep( -3 );								// couldn't get the serial number
	}
#endif			// CheckLicense

		// Standard initialization
		// If you are not using these features and wish to reduce the size
		//  of your final executable, you should re6pmove from the following
		//  the specific initialization routines you do not need.
	SetRegistryKey( _T("PocketNumerix") );
	LoadIcon( IDI_StrategyExplorer );

// new stuff...
#ifdef	WIN32_PLATFORM_PSPC
		// Pocket PC
//  SHInitExtraControls();					// causes VS8-PPC'03 link errors
	#if	WIN32_PLATFORM_PSPC == 310			// Pocket PC 2002
			// use black text over light gray background in all Dialogs
		SetDialogBkColor( RGB(208,208,208), RGB(0,0,0) );
	#endif
#endif // WIN32_PLATFORM_PSPC

#if	WIN32_PLATFORM_PSPC != 310				// not Pocket PC 2002
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// make sure screen orientation is Portrait
	DEVMODE	dvms;
	dvms.dmSize = sizeof( dvms );
	dvms.dmFields = DM_DISPLAYORIENTATION | DM_DISPLAYQUERYORIENTATION;
		// query whether screen orientation can be dynamically changed
	long lres = ChangeDisplaySettingsEx( NULL, &dvms, NULL, CDS_TEST, NULL );
	if ( dvms.dmDisplayOrientation != DMDO_0  )
	{		// switch the screen to Portrait mode
		dvms.dmOrientation = DMORIENT_PORTRAIT;
		dvms.dmDisplayOrientation = DMDO_0;				// no rotation
		dvms.dmFields = DM_ORIENTATION | DM_DISPLAYORIENTATION;
		lres = ChangeDisplaySettingsEx( NULL, &dvms, NULL, 0, NULL );
	}
#endif
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// add a Shell Notification Icon to the system tray...
/*
	NOTIFYICONDATA nid = { 0 };
	nid.cbSize         = sizeof( nid );
	nid.uID            = 100;		// Per WinCE SDK docs, values from 0 to 12 are reserved and should not be used.
	nid.uFlags         = NIF_ICON;
//	nid.hIcon          = LoadIcon( IDI_StrategyExplorer );
		// according to <http://www.codeguru.com/forum/archive/index.php/t-194324.html>
		// the following are needed rather than just LoadIcon(), and he's right
	nid.hWnd = m_pMainWnd;			// gets OnCommand()
	nid.hIcon	= (HICON)LoadImage( AfxGetInstanceHandle(),
									MAKEINTRESOURCE(IDI_StrategyExplorer),IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR );
		// Add the notification to the tray.
    BOOL	okay = Shell_NotifyIcon( NIM_ADD, &nid );
#ifdef _DEBUG
	if ( ! okay )
		TRACE( _T("StrategyExplorer::InitInstance: Shell_NotifyIcon(ADD) failed\n") );
#endif
*/
	CStrategyExplorerDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// always save the state of the strategy explorer (cancelled or not)
	OneStk4OptStrategy	os4os;
	for ( short ii = 0; ii <= stkIdx; ii++ )
	{	os4os.buySell[ii] = dlg.m_BuySell[ii];
		os4os.cost[ii]	  = dlg.m_Cost[ii];
		os4os.qty[ii]	  = dlg.m_Qty[ii];
		if ( ii < stkIdx )
		{	os4os.putCall[ii] = dlg.m_PutCall[ii];
			os4os.strike[ii] = dlg.m_Strike[ii];
		}
	}
	bool	bres = SetRecentStrategy( os4os );
#ifdef _DEBUG
	if ( ! bres )
		TRACE( _T("StrategyExplorerApp::InitInstance: Registry write failed, bres=%s\n"),
				bres  ?  _T("true")  :  _T("false") );
#endif
		// save the hedges to a file (XXX)
	dlg.SaveStrategies();

	    // Remove the notification icon from the system tray.
//    Shell_NotifyIcon( NIM_DELETE, &nid );
// end of more new stuff

		// Since the dialog has been closed, return FALSE so that we exit the
		//  application, rather than start the application's message pump.
	return	FALSE;
}			// InitInstance()
//---------------------------------------------------------------------------
void		CStrategyExplorerApp::ExitPrep( short code )
{
	bool epilogue = ( code < 0 );
	if ( epilogue )
	{
#ifdef CheckLicense
#ifdef _DEBUG
		if ( code == -1 )
		{	CString sn = GetSerialNumber();				// GetSerialNumber() is only defined when CheckLicense is #defined
			CString wu = uu;
			TRACE( _T("StrategyExplorerApp:InitInstance: license uuid '%s' doesn't match device uuid '%s'\n"), wu, sn );
		}
		else if ( code == -2 )
			TRACE( _T("StrategyExplorerApp:InitInstance: license huid doesn't match device huid\n") );
		else if ( code == -3 )
			TRACE( _T("StrategyExplorerApp:InitInstance: couldn't get serial number\n") );
#endif
#endif
			// copy calc into the PocketNumerix directory

		PROCESS_INFORMATION pi;
		BOOL rc = CreateProcess( _T("calc"), _T(""), NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi );
		if ( rc )
		{	CloseHandle( pi.hThread );
			CloseHandle( pi.hProcess );
		}

			// overwrite the registry key
		HKEY subkey;
		wchar_t*	regKey = _T("SOFTWARE\\PocketNumerix\\StrategyExplorer");
		long res = RegOpenKeyEx( HKEY_LOCAL_MACHINE, regKey, 0, KEY_QUERY_VALUE, &subkey );
		if ( res == ERROR_SUCCESS )
		{	unsigned long cbMaxValueLen;
			res = RegQueryInfoKey( subkey, NULL, NULL, NULL, NULL, NULL, NULL,
										   NULL, NULL, &cbMaxValueLen, NULL, NULL );
			RegCloseKey( subkey );
			unsigned char*	tmp = new unsigned char[ cbMaxValueLen ];
			for ( unsigned short ii = 0; ii < cbMaxValueLen; ii++ )
			{	*(tmp+ii) = (unsigned char)( rand() & 0xFF );
			}
			WriteRegBinary( HKEY_LOCAL_MACHINE, regKey, tmp, cbMaxValueLen );
			delete [] tmp;
		}
	}
	ExitThread( code );
}			// ExitPrep()
//---------------------------------------------------------------------------
int CStrategyExplorerApp::ExitInstance() 
{
	BOOL	res = ReleaseMutex( hMutex );
#ifdef _DEBUG
	if ( res != 0 )
		TRACE( _T("StrategyExplorerApp::ExitInstance: ReleaseMutex failed\n") );
#endif
	
	return CWinApp ::ExitInstance();
}			// ExitInstance()
//-----------------------------------------------------------------------------------------------
bool		GetRecentStrategy( OneStk4OptStrategy& os4os )
{	unsigned short	ii;

	wchar_t*	flagsKey = _T("Software\\PocketNumerix\\StrategyExplorer\\StrategyFlags");
	int		flags = 0;
	bool	b0 = ReadRegBinary( HKEY_CURRENT_USER, flagsKey, (BYTE*)&flags, 3 );

		// unpack the 'booleans' (really tri-state)
	if ( b0 )
	{	for ( ii = 0; ii < 4; ii++ )
		{	os4os.putCall[ii] = (OptionType)(flags & 0x3);			// OptionType is tristate -> 2 bits
			flags >>= 2;
		}
		for ( ii = 0; ii < 5; ii++ )
		{	os4os.buySell[ii] = (BuySellStatus)(flags & 0x3);		// BuySellStatus is tristate -> 2 bits
			flags >>= 2;
		}
	}
	const short	nFloats = 14;
	float stratValues[nFloats] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	wchar_t*	floatsKey = _T("Software\\PocketNumerix\\StrategyExplorer\\StrategyValues");
	bool	b1 = ReadRegBinary( HKEY_CURRENT_USER, floatsKey, (BYTE*)&stratValues, nFloats*sizeof(float));
	if ( b1 )
	{		// get the floats back
		short sfi = 0;

			// get 5 instrument prices
		for ( ii = 0; ii < 5; ii++ )					// 0..4, options at 0..3, then stock
			os4os.cost[ii] = stratValues[sfi++];
		
			// get 5 quantities
		for ( ii = 0; ii < 5; ii++ )					// 0..4, options at 0..3, then stock
			os4os.qty[ii] = stratValues[sfi++];

			// get 4 strikes
		for ( ii = 0; ii < 4; ii++ )					// 0..3
			os4os.strike[ii] = stratValues[sfi++];
	}
	return	b0  &&  b1;
}			// GetRecentStrategy()
//----------------------------------------------------------------------------
bool		SetRecentStrategy( OneStk4OptStrategy& os4os )
{	unsigned short	ii;
	short			jj;
		// pack the 'booleans' (really tri-state)

		// save five buySell bools
	int	flags = 0;									// BuySell	PutCall	
		// the packed byte will look like:			     3210	  210
	for ( jj = 4; jj >= 0; jj-- )
	{	flags |= (int)(os4os.buySell[jj]);			// the high order bits
		flags <<= 2;
	}
		// save four putCall bools
	for ( jj = 3; jj >= 0; jj-- )
	{	flags |= (int)(os4os.putCall[jj]);			// the low order bits
		if ( jj > 0 )
			flags <<= 2;							// don't shift the last time through the loop
	}

		// save the flags to the registry
	wchar_t*	flagsKey = _T("Software\\PocketNumerix\\StrategyExplorer\\StrategyFlags");
	bool	b0 = WriteRegBinary( HKEY_CURRENT_USER, flagsKey, (BYTE*)&flags, 3 );	// bytes

	const short	nFloats = 14;
	float stratValues[nFloats];

		// save 5 instrument prices
	short sfi = 0;
	for ( ii = 0; ii < 5; ii++ )					// 0..4
		stratValues[sfi++] = os4os.cost[ii];

		// save 5 quantities
	for ( ii = 0; ii < 5; ii++ )					// 0..4
		stratValues[sfi++] = os4os.qty[ii];

		// save 4 strikes
	for ( ii = 0; ii < 4; ii++ )					// 0..3
		stratValues[sfi++] = os4os.strike[ii];

		// save the floats to the registry
	wchar_t*	floatsKey = _T("Software\\PocketNumerix\\StrategyExplorer\\StrategyValues");
	bool	b1 = WriteRegBinary( HKEY_CURRENT_USER, floatsKey, (BYTE*)&stratValues, nFloats*sizeof(float) );
	return	b0  &&  b1;
}			// SetRecentStrategy()
//----------------------------------------------------------------------------
/*
bool		ReadRegShort( LPCTSTR pcszKey, short& sValue )
{	ASSERT ( AfxIsValidString(pcszKey) );
	HKEY	hKey;
	LONG	err = RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_QUERY_VALUE, &hKey );
	if ( err != ERROR_SUCCESS )
	{
#ifdef _DEBUG
		unsigned short	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("ReadRegShort(open %s): %s.\n"), pcszKey, buf );
#endif
		return	false;
	}
	DWORD	dwType = (DWORD)REG_BINARY;
	DWORD	cbData = sizeof(short);
		//				   HKEY  LPCWSTR  0  LPDWORD  LPBYTE          LPDWORD
	err = RegQueryValueEx( hKey, pcszKey, 0, &dwType, (PBYTE)&sValue, &cbData );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{	unsigned short	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("ReadRegShort(query %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			// ReadRegShort()
//-----------------------------------------------------------------------------------------------
bool		WriteRegShort( LPCTSTR pcszKey, short sValue )
{	ASSERT ( AfxIsValidString(pcszKey) );
	HKEY	hKey;
//	RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_SET_VALUE, &hKey );
	LPWSTR	cls = _T("short");
	DWORD	disp;

	LONG	err = RegCreateKeyEx( HKEY_CURRENT_USER, pcszKey, 0, cls, 0, 0, 0, &hKey, &disp );
#ifdef _DEBUG
	TRACE( _T("WriteRegShort: RegCreateKeyEx(%s %s): err=%d (should be zero).\n"),
		(disp == REG_CREATED_NEW_KEY) ? _T("create") : _T("open"), pcszKey, err );
#endif
	if ( err != ERROR_SUCCESS )
		return	false;

		//				 HKEY  LPCWSTR  0  DWORD       BYTE*           DWORD
	err = RegSetValueEx( hKey, pcszKey, 0, REG_BINARY, (PBYTE)&sValue, sizeof(short) );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{	unsigned short	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("WriteRegShort(set %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			//WriteRegShort()
//----------------------------------------------------------------------------
bool		ReadRegBinary( LPCTSTR pcszKey, short nBytes, void* outBuf )
{	ASSERT ( AfxIsValidString(pcszKey) );
	HKEY	hKey;
	LONG	err = RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_QUERY_VALUE, &hKey );
	if ( err != ERROR_SUCCESS )
	{
#ifdef _DEBUG
		unsigned short	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("ReadRegBinary(open %s): %s.\n"), pcszKey, buf );
#endif
		return	false;
	}
	DWORD	dwType = (DWORD)REG_BINARY;
	DWORD	cbData = (DWORD)nBytes;
		//				   HKEY  LPCWSTR  0  LPDWORD   LPBYTE		 LPDWORD
	err = RegQueryValueEx( hKey, pcszKey, 0, &dwType, (PBYTE)outBuf, &cbData );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{	unsigned short	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("ReadRegBinary(query %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			// ReadRegBinary()
//-----------------------------------------------------------------------------------------------
bool		WriteRegBinary( LPCTSTR pcszKey, short nBytes, void* inBuf )
{	ASSERT ( AfxIsValidString(pcszKey) );
	HKEY	hKey;
//	RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_SET_VALUE, &hKey );
	LPWSTR	cls = _T("binary");
	DWORD	disp;

	LONG	err = RegCreateKeyEx( HKEY_CURRENT_USER, pcszKey, 0, cls, 0, 0, 0, &hKey, &disp );
#ifdef _DEBUG
	TRACE( _T("WriteRegBinary: RegCreateKeyEx(%s %s): err=%d (should be zero).\n"),
		(disp == REG_CREATED_NEW_KEY) ? _T("create") : _T("open"), pcszKey, err );
#endif
	if ( err != ERROR_SUCCESS )
		return	false;

		//				 HKEY  LPCWSTR  0  DWORD        BYTE*		  DWORD
	err = RegSetValueEx( hKey, pcszKey, 0, REG_BINARY, (PBYTE)inBuf, nBytes );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{	unsigned short	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("WriteRegBinary(set %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			//WriteRegBinary()
*/
//-----------------------------------------------------------------------------------------------
