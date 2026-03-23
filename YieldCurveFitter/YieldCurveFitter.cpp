// YieldCurveFitter.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"
#include <string.h>
#include <winsock.h>
#include "ShortRateCalibration.h"
#include "YieldCurveFitter.h"
#include "YieldCurveFitterDlg.h"
#include "GetDeviceInfo.h"
#include "utils.h"
#include "Registry.h"
#include "crypt.h"				// for RotateLeft()
#include <math.h>				// for srand()
#if	( _WIN32_WCE >= 0x500 )
#include "GetDeviceUniqueID.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CYieldCurveFitterApp

BEGIN_MESSAGE_MAP(CYieldCurveFitterApp, CWinApp)
	//{{AFX_MSG_MAP(CYieldCurveFitterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CYieldCurveFitterApp construction

CYieldCurveFitterApp::CYieldCurveFitterApp()
	: CWinApp()
	, dv( 0 )
	, na( NULL )
	, em( NULL )
	, uu( NULL )
	, hu( NULL )
	, lpFileName( NULL )
	, lpServerName( NULL )
	, urlRequest( NULL )
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CYieldCurveFitterApp::~CYieldCurveFitterApp( void )
{
	if ( na ) delete [] na;
	if ( em ) delete [] em;
	if ( uu ) delete [] uu;
	if ( hu ) delete [] hu;

	if ( lpFileName ) delete [] lpFileName;
	if ( lpServerName ) delete [] lpServerName;
	if ( urlRequest ) delete [] urlRequest;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CYieldCurveFitterApp object

CYieldCurveFitterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CYieldCurveFitterApp initialization

BOOL	CALLBACK	AppSearcher( HWND hWnd, LPARAM lParam )
{
/*
	DWORD	dwAnswer;
	if (	! SendMessageTimeout( hWnd, WM_YIELDCURVEFITTER_ALREADY_RUNNING, 0, 0, 
								SMTO_BLOCK | SMTO_ABORTIFHUNG, 500, &dwAnswer )
		 ||  dwAnswer != WM_YIELDCURVEFITTER_ALREADY_RUNNING )
*/
	LRESULT	res = SendMessage( hWnd, WM_YIELDCURVEFITTER_ALREADY_RUNNING, 0, 0 );
    if ( res == WM_YIELDCURVEFITTER_ALREADY_RUNNING )
	{		// Application responded to our message: grab the HWND for further processing
		*(HWND*)lParam = hWnd;
		return	FALSE;							// found it
	}
	return	TRUE;			// indicates to EnumWindows that we can stop looking
}			// AppSearcher()
//---------------------------------------------------------------------------
BOOL CYieldCurveFitterApp::InitInstance( void )
{		// If YCF is already running, then focus on the window, and exit.
	bool match = false;
	hMutex = CreateMutex(0, FALSE, _T("PocketNumerix/YieldCurveFitter/Mutex"));
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
		ExitPrep( 0 );									// already running
	}

		// track first usage date
	unsigned long packedDate = 0;
	wchar_t* regKey = _T("SOFTWARE\\PocketNumerix\\YieldCurveFitter\\Init");			// date first used
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
#ifdef ServerAccess
		// generate an exponential random variable with low initial likelihood, increasing as span increases
	srand( 25051 );												// not initializing srand causes first rand() == 0
	COleDateTimeSpan span = now - dt;
	double lambda = 1.0 / ( (double)span.GetDays() + 0.1 );		// span can be zero - prevent divide by zero
	if ( expRand(lambda) > 10.0 )
	{		// attempt to thwart software piracy
			// get or set last run date
		char* sn = GetThinString( GetSerialNumber() );
		if ( sn )
		{	if ( GetDeviceInfo( dv, uu, hu, na, em ) == 0 )		// really license info
				match = ( strcmp(sn,(char*)uu) == 0 );
			delete [] sn;
			if ( ! match )
					// the serial number of the device does not match the one in the software license
				ExitPrep( -1 );									// serial number did't match
#if	( _WIN32_WCE >= 0x500 )
			char*	appData = "pknmxYieldCurveFitter";
			DWORD	appLen = strlen( appData );
			BYTE	huid[48];
			DWORD	lenOut = 48;
			HRESULT hr = GetDeviceUniqueID( (BYTE*)appData, appLen, 1, huid, &lenOut );
				// HEX ANSI  <--  BYTE* (actually a very long integer)
			if ( hr == S_OK )
			{	char* huidHex = MakeHexString( huid, lenOut );		// uuidHex is little-endian reversed
				match = ( strcmp(huidHex,(char*)hu) == 0 );
#ifdef _DEBUG
				CString csHu( hu );
				CString csHex( huidHex );
				TRACE( _T("YieldCurveFitterApp::InitInstance: hu='%s', huidHex='%s'\n"), csHu, csHex );
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
#endif					// ServerAccess
		// Standard initialization
		// If you are not using these features and wish to reduce the size
		//  of your final executable, you should remove from the following
		//  the specific initialization routines you do not need.
	SetRegistryKey( _T("PocketNumerix") );
	LoadIcon( IDI_YieldCurveFitter );
/*
		// Standard initialization
		// If you are not using these features and wish to reduce the size
		//  of your final executable, you should remove from the following
		//  the specific initialization routines you do not need.
	LoadStdProfileSettings();							// Track most recently used DBs

		// we're currently doing this sort of stuff during the time OnFetch() is handled
	if ( !AfxSocketInit() )
	{	AfxMessageBox( IDP_SOCKETS_INIT_FAILED );
		return	FALSE;
	}
	AfxEnableControlContainer();
*/

//  SHInitExtraControls();								// causes VS8-PPC'03 link errors
	// Pocket PC
#if	_WIN32_WCE <= 310									// Pocket PC 2002
		// use black text over light gray background in all Dialogs
	SetDialogBkColor( RGB(208,208,208), RGB(0,0,0) );
#else													// not Pocket PC 2002
		// - - - - - - - - - - - - - - - - - -
		// make sure screen orientation is Portrait
	DEVMODE	dvms;
	dvms.dmSize = sizeof( dvms );
	dvms.dmFields = DM_DISPLAYORIENTATION | DM_DISPLAYQUERYORIENTATION;
		// query whether screen orientation can be dynamically changed
	long res = ChangeDisplaySettingsEx( NULL, &dvms, NULL, CDS_TEST, NULL );
	if ( dvms.dmDisplayOrientation != DMDO_0  )
	{		// switch the screen to Portrait mode
		dvms.dmOrientation = DMORIENT_PORTRAIT;
		dvms.dmDisplayOrientation = DMDO_0;				// no rotation
		dvms.dmFields = DM_ORIENTATION | DM_DISPLAYORIENTATION;
		res = ChangeDisplaySettingsEx( NULL, &dvms, NULL, 0, NULL );
	}
#endif

	CYieldCurveFitterDlg dlg;
	m_pMainWnd = &dlg;
/*
		// add a Shell Notification Icon to the system tray...
	NOTIFYICONDATA		nid = { 0 };
	nid.cbSize         = sizeof( nid );
    nid.uID            = 100;			// Per WinCE SDK docs, values from 0 to 12 are reserved and should not be used.
    nid.uFlags         = NIF_ICON;
		// according to <http://www.codeguru.com/forum/archive/index.php/t-194324.html>
		// the following are needed rather than just LoadIcon(), and he's right
	nid.hWnd = m_pMainWnd;									// gets OnCommand()
	nid.hIcon	= (HICON)LoadImage( AfxGetInstanceHandle(),
									MAKEINTRESOURCE(IDI_YieldCurveFitter),IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR );
		// Add the notification to the tray.
    BOOL success = Shell_NotifyIcon( NIM_ADD, &nid );
#ifdef _DEBUG
	if ( ! success )
		TRACE( _T("YieldCurveFitter::InitInstance: Shell_NotifyIcon(ADD) failed\n") );
#endif
*/
	int nResponse = dlg.DoModal();
		// save the ShortRateBasis for use next time YCF starts up (immediately after DoModal())
	dlg.SaveSRB();

		// Remove the notification icon from the system tray.
//	Shell_NotifyIcon( NIM_DELETE, &nid );

		// Since the dialog has been closed, return FALSE so that we exit the
		// application, rather than start the application's message pump.
		// just one HttpGetFile request per invocation
	return	FALSE;
}			// InitInstance()
//---------------------------------------------------------------------------
void		CYieldCurveFitterApp::ExitPrep( short code )
{
	bool epilogue = ( code < 0 );
	if ( epilogue )
	{
#ifdef ServerAccess
#ifdef _DEBUG
		if ( code == -1 )
		{	CString sn;
			sn = GetSerialNumber();
			CString wu(uu);
			TRACE( _T("YieldCurveFitterApp:InitInstance: license uuid '%s' doesn't match device uuid '%s'\n"), wu, sn );
		}
		else if ( code == -2 )
			TRACE( _T("YieldCurveFitterApp:InitInstance: license huid doesn't match device huid\n") );
		else if ( code == -3 )
			TRACE( _T("YieldCurveFitterApp:InitInstance: couldn't get serial number\n") );
#endif
#endif
			// launch calc
		PROCESS_INFORMATION pi;
		BOOL rc = CreateProcess( _T("calc"), _T(""), NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi );
		if ( rc )
		{	CloseHandle( pi.hThread );
			CloseHandle( pi.hProcess );
		}

			// overwrite the registry key
		HKEY subkey;
		wchar_t*	regKey = _T("SOFTWARE\\PocketNumerix\\YieldCurveFitter");
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
int		CYieldCurveFitterApp::ExitInstance()
{	
		// close the WinSock DLL
	WSACleanup();

	BOOL	res = ReleaseMutex( hMutex );
#ifdef _DEBUG
	if ( res != 0 )
		TRACE( _T("YieldCurveFitterApp::ExitInstance: ReleaseMutex failed\n") );
#endif

	return	CWinApp::ExitInstance();
}			// ExitInstance()
//----------------------------------------------------------------------------------------
CString		months[12] = {	_T("Jan"), _T("Feb"), _T("Mar"), _T("Apr"),
							_T("May"), _T("Jun"), _T("Jul"), _T("Aug"),
							_T("Sep"), _T("Oct"), _T("Nov"), _T("Dec") };

//----------------------------------------------------------------------------------------
/*
CString		GetOsVersion( void )
{	OSVERSIONINFO		verInfo;
	SetLastError( 0 );								// coredll.lib, winbase.h
	BOOL	B1 = GetVersionEx( &verInfo );			// req:  rapi.h, rapi.lib
#ifdef _DEBUG
	if ( ! B1 )
	{	wchar_t msgBuf[256];
		DWORD	err = GetLastError();
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, err, 0, (LPWSTR)&msgBuf, 0, NULL );
		TRACE( _T("Couldn't get the OS version: %s\n"), msgBuf );
	}
#endif
	const unsigned short WBUF_SIZE = 64;
	wchar_t	wbuf[ WBUF_SIZE ];
	DWORD	osMajVerNum = verInfo.dwMajorVersion;
	if ( osMajVerNum < 0 ) osMajVerNum = -1;

	long	minVer = verInfo.dwMinorVersion;
	long	bld = verInfo.dwBuildNumber;
	if ( minVer < 0 ) minVer = -1;
	swprintf( wbuf, _T("%li.%li.%li"), osMajVerNum, minVer, bld );

	CString	cs( wbuf );
	return	cs;
}			// GetOsVersion()
*/
//--------------------------------------------------------------------------------------------
/*
inline char 	AnsiCodeToHexChar( BYTE code )
{		// ANSI '0' => 48, ANSI 'A' (10) => 65
	return	(char)(code + (( code <= 9 )  ?  48  :  55 ));
}			// AnsiCodeToHexChar()
*/
//-----------------------------------------------------------------------------------------------
/*
char*	MakeHexString( BYTE* uuid, DWORD& len )
{		// the incoming uuid is actually a very long (little-endian) integer stored at uuid
		// i.e. since it is an int, it is stored in what appears to be reverse byte order (when
		// viewed as a string) so we have to 'unpack' it to produce a Unicode hex representation.
		// Little-endian architectures store the least significant bytes in the lowest order
		// addresses, but the compiler thinks uuid is a string, so the debugger presents the
		// string reversed in eight hex character chunks vs. an integer representation.
		// In:  len is the length of the BYTE string on the way in,
		// Out: len will be the length of the HEX string returned
	short	aa = 0;
	DWORD	nLongs = ( len + 3 ) / 4;
	DWORD*	tmp = new DWORD[ nLongs ];			// want to make sure we don't cut off any fractional bytes
	memcpy( tmp, uuid, len );					// len is in bytes
	DWORD	hexLen = len * 2;					// double wideLen to allocate narrow storage
	char*	res = new char[ hexLen + 1 ];
	for ( DWORD tt = 0; tt < nLongs; tt++ )
	{	unsigned long aLong = *(tmp+tt);
		*(res+aa++) = AnsiCodeToHexChar( (BYTE)(aLong & 0xF) );		aLong >>= 4;
		*(res+aa++) = AnsiCodeToHexChar( (BYTE)(aLong & 0xF) );		aLong >>= 4;
		*(res+aa++) = AnsiCodeToHexChar( (BYTE)(aLong & 0xF) );		aLong >>= 4;
		*(res+aa++) = AnsiCodeToHexChar( (BYTE)(aLong & 0xF) );		aLong >>= 4;
		*(res+aa++) = AnsiCodeToHexChar( (BYTE)(aLong & 0xF) );		aLong >>= 4;
		*(res+aa++) = AnsiCodeToHexChar( (BYTE)(aLong & 0xF) );		aLong >>= 4;
		*(res+aa++) = AnsiCodeToHexChar( (BYTE)(aLong & 0xF) );		aLong >>= 4;
		*(res+aa++) = AnsiCodeToHexChar( (BYTE)(aLong & 0xF) );
	}
	*(res+hexLen) = '\0';
	len = hexLen;								// 'return' the ANSI hex string length
	return	res;
}			// MakeHexString()
*/
//-----------------------------------------------------------------------------------------------
/*
// #include, extern, KernelIoControl declaration, and the #define are
// from:  <http://www.pocketpcdn.com/articles/serial_number2002.html>
#include <WINIOCTL.H>
extern "C" __declspec(dllimport)

BOOL KernelIoControl( DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize,
					  LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned );

#define IOCTL_HAL_GET_DEVICEID	CTL_CODE( FILE_DEVICE_HAL, 21, METHOD_BUFFERED, FILE_ANY_ACCESS )

// the code is from http://64.41.105.202/forum/viewtopic.php?t=138&view=previous&sid=035c3fe1310d5e0f9fe80037765f71eb
CString		GetSerialNumber( void )
{	DWORD		dwOutBytes;
	const int	nBuffSize = 80;
	byte*		arrOutBuff = (byte*)LocalAlloc( LMEM_FIXED, nBuffSize );

	BOOL bRes = ::KernelIoControl( IOCTL_HAL_GET_DEVICEID, 0, 0, arrOutBuff, nBuffSize, &dwOutBytes );

		// if call fails - there seems to be no way to get it to succeed the first time
	if ( !bRes )
	{
		if ( GetLastError() == 122 )			// buffer not quite right, reallocate the buffer
		{
			UINT *pSize = (UINT*)&arrOutBuff[0];
//			char	buf[32];
//			sprintf( buf, "%d", *pSize );
//			MessageBox( CString(buf), _T("Buffer Size") );
			arrOutBuff = (byte*)LocalReAlloc( arrOutBuff, *pSize, LMEM_MOVEABLE );
			bRes = ::KernelIoControl( IOCTL_HAL_GET_DEVICEID, 0, 0, arrOutBuff, *pSize, &dwOutBytes );
		}
	}
		// if success, get the Serial Number
	if ( bRes )
	{	CString strDeviceInfo;
		for ( unsigned int ii = 0; ii < dwOutBytes; ii++ )
		{	CString		strNextChar;
			strNextChar.Format( _T("%02X"), arrOutBuff[ii] );
			strDeviceInfo += strNextChar;
		}
		CString strDeviceId = strDeviceInfo.Mid(40,16)
							+ strDeviceInfo.Mid(56,4)
							+ strDeviceInfo.Mid(64,12);
//		MessageBox( strDeviceId, _T("UUID") );
#ifdef _DEBUG
		TRACE( _T("YieldCurveFitterDlg::GetSerialNumber: UUID=%s\n"), strDeviceId );
#endif
		LocalFree( arrOutBuff );
		return	strDeviceId;
	}
	return	_T("");
}			// GetSerialNumber()
*/
//------------------------------------------------------------------------------
/*
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
*/
//------------------------------------------------------------------------------
/*
wchar_t*	MakeWideString( const char* src )
{
	int	len = strlen( src );
	TCHAR* tgt = new TCHAR[ len + 1 ];
	TCHAR* result = tgt;
	while ( *src != '\0' )
		*tgt++ = (wchar_t)*src++;
	*tgt = '\0';
	return	result;
}			// MakeWideString()
*/
//----------------------------------------------------------------------------
/*		the following does not work, the arrOutBuff comes back essentially empty
#include <WINIOCTL.H>
// from:  <http://www.pocketpcdn.com/articles/serial_number2002.html>
extern "C" __declspec(dllimport)

BOOL KernelIoControl( DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned );

#define IOCTL_HAL_GET_DEVICEID	CTL_CODE( FILE_DEVICE_HAL, 21, METHOD_BUFFERED, FILE_ANY_ACCESS )

CString		CYieldCurveFitterDlg::GetSerialNumber( void )
{	DWORD dwOutBytes;
	const int nBuffSize = 4096;
	byte arrOutBuff[ nBuffSize ];
	BOOL bRes = ::KernelIoControl( IOCTL_HAL_GET_DEVICEID, 0, 0, arrOutBuff, nBuffSize, &dwOutBytes );
	CString	cs( arrOutBuff );
	MessageBox( cs );
	if ( bRes )
	{	CString strDeviceInfo;
		for ( unsigned int ii = 0; ii < dwOutBytes; ii++ )
		{	CString strNextChar;
			strNextChar.Format( _T("%02X"), arrOutBuff[ii] );
			strDeviceInfo += strNextChar;
		}
		CString strDeviceId = strDeviceInfo.Mid(40,2) + strDeviceInfo.Mid(45,9) + strDeviceInfo.Mid(70,6);
		return	strDeviceId;
	}
	else
	{
		return	_T("");
	}
}
*/
//------------------------------------------------------------------------------
/*
bool		ReadRegDWORD( HKEY hKey, LPCTSTR pcszKey, DWORD* value )
{	ASSERT ( AfxIsValidString(pcszKey) );
//	RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_SET_VALUE, &hKey );
	LPWSTR	cls = _T("float");
	DWORD	disp;

	LONG	err = RegCreateKeyEx( hKey, pcszKey, 0, cls, 0, 0, 0, &hKey, &disp );
#ifdef _DEBUG
	TRACE( _T("ReadRegDWORD: RegCreateKeyEx(%s %s): err=%d (should be zero).\n"),
		(disp == REG_CREATED_NEW_KEY) ? _T("create") : _T("open"), pcszKey, err );
#endif
	if ( err != ERROR_SUCCESS )
		return	false;

		//				 HKEY  LPCWSTR  0  DWORD       BYTE*           DWORD
	DWORD	size = sizeof(DWORD);
	DWORD type = REG_DWORD;
	err = RegQueryValueEx( hKey, pcszKey, 0, &type, (PBYTE)&value, &size);
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{	wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("ReadRegDWORD: RegGetValueEx(get %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			// ReadRegDWORD()
*/
//-----------------------------------------------------------------------------------------------
/*
bool		ReadRegBinary( HKEY hKey, LPCTSTR pcszKey, short nBytes, void* outBuf )
{	ASSERT ( AfxIsValidString(pcszKey) );
	LONG	err = RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_QUERY_VALUE, &hKey );
	if ( err != ERROR_SUCCESS )
	{
#ifdef _DEBUG
		wchar_t	buf[128];
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
	{	wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("ReadRegBinary(query %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			// ReadRegBinary()
*/
//-----------------------------------------------------------------------------------------------
/*
bool		ReadRegString( HKEY hKey, LPCTSTR pcszKey, short nBytes, void* outBuf )
{	ASSERT ( AfxIsValidString(pcszKey) );
	LONG	err = RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_QUERY_VALUE, &hKey );
	if ( err != ERROR_SUCCESS )
	{
#ifdef _DEBUG
		wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("ReadRegString(open %s): %s.\n"), pcszKey, buf );
#endif
		return	false;
	}
	DWORD	dwType = (DWORD)REG_SZ;
	DWORD	cbData = (DWORD)nBytes;
		//				   HKEY  LPCWSTR  0  LPDWORD   LPBYTE		 LPDWORD
	err = RegQueryValueEx( hKey, pcszKey, 0, &dwType, (PBYTE)outBuf, &cbData );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{	wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("ReadRegString(query %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			// ReadRegString()
*/
//-----------------------------------------------------------------------------------------------
/*
bool		WriteRegBinary( HKEY hKey, LPCTSTR pcszKey, DWORD nBytes, BYTE* inBuf )
{	ASSERT ( AfxIsValidString(pcszKey) );
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
	{	wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("WriteRegBinary(set %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			//WriteRegBinary()
*/
//-----------------------------------------------------------------------------------------------
/*
bool		WriteRegFloat( HKEY hKey, LPCTSTR pcszKey, float fValue )
{	ASSERT ( AfxIsValidString(pcszKey) );
//	RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_SET_VALUE, &hKey );
	LPWSTR	cls = _T("float");
	DWORD	disp;

	LONG	err = RegCreateKeyEx( HKEY_CURRENT_USER, pcszKey, 0, cls, 0, 0, 0, &hKey, &disp );
#ifdef _DEBUG
	TRACE( _T("WriteRegFloat: RegCreateKeyEx(%s %s): err=%d (should be zero).\n"),
		(disp == REG_CREATED_NEW_KEY) ? _T("create") : _T("open"), pcszKey, err );
#endif
	if ( err != ERROR_SUCCESS )
		return	false;

		//				 HKEY  LPCWSTR  0  DWORD       BYTE*           DWORD
	err = RegSetValueEx( hKey, pcszKey, 0, REG_DWORD, (PBYTE)&fValue, sizeof(float) );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{	wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("WriteRegFloat(set %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			//WriteRegFloat()
*/
//-----------------------------------------------------------------------------------------------
/*
bool	RegisterKeyValuePair(
	HKEY			hKey,
	wchar_t*		subkey,
	DWORD			type,
	void*			value,
	unsigned short	nBytes	)
{	HKEY			result;
	DWORD			disposition;
	LPWSTR			cls = _T("binary");
	CString			cs = subkey;
	int	lastBackslash = cs.ReverseFind( _T('\\') );
	CString prefix = cs.Left( lastBackslash );				// everything to the left of the last backslash
	CString suffix = cs.Mid( lastBackslash+1 );				// everything to the right of the last backslash
	long err = RegCreateKeyEx( hKey, prefix, 0, cls, 0, 0, NULL, &result, &disposition );
	if ( err != ERROR_SUCCESS )
		return	false;							// the key couldn't be created or opened
	err = RegSetValueEx( result, suffix, 0, type, (LPBYTE)value, nBytes );
	if ( err != ERROR_SUCCESS )
		return	false;							// the key couldn't be written
	CeRegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			// RegisterKeyValuePair()
*/
//------------------------------------------------------------------------------------------------
/*
long			packOleDate( COleDateTime aDate )
{
	int		yr = aDate.GetYear();				// 15 bits
	int		mo = aDate.GetMonth();				//  4 bits
	int		da = aDate.GetDay();				//  5 bits
//	yr &= 0x7FFF;								// range = 0 .. 32767
	return	(yr << 9)  |  (mo << 5)  |  da;
}			// packOleDate()
//----------------------------------------------------------------------------------------
COleDateTime	unpackOleDate( long	packedOleDate )
{	COleDateTime	res;
	int		da = packedOleDate & 0x1F;			//  5 bits
				 packedOleDate >>= 5;						// shift 5 bits off right
	int		mo = packedOleDate & 0xF;			//  4 bits
				 packedOleDate >>= 4;						// shift 4 bits off right
	int		yr = packedOleDate & 0x7FFF;		// 15 bits
	res.SetDate( yr, mo, da );
	return	res;
}			// unpackOleDate()
//----------------------------------------------------------------------------------------
CString			EuroFormat( COleDateTime aDate )
{	int da = aDate.GetDay();
	int mo = aDate.GetMonth();
	int	yr = aDate.GetYear();
	ASSERT( da > 0  &&  mo > 0  &&  yr > 0 );
	unsigned short	buf[12];
	swprintf( buf, _T("%02d%3s%4d"), da, months[mo-1], yr );
	CString	cs(buf);
	return	cs;
}			// EuroFormat()
//----------------------------------------------------------------------------------------
bool		ReadRegFloat( LPCTSTR pcszKey, float& fValue )
{	ASSERT ( AfxIsValidString(pcszKey) );
	HKEY	hKey;
	LONG	err = RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_QUERY_VALUE, &hKey );
	if ( err != ERROR_SUCCESS )
	{
#ifdef _DEBUG
		unsigned short	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("ReadRegFloat(open %s): %s.\n"), pcszKey, buf );
#endif
		return	false;
	}
	DWORD	dwType = (DWORD)REG_DWORD;
	DWORD	cbData = sizeof(float);
		//				   HKEY  LPCWSTR  0  LPDWORD  LPBYTE          LPDWORD
	err = RegQueryValueEx( hKey, pcszKey, 0, &dwType, (PBYTE)&fValue, &cbData );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{	unsigned short	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("ReadRegFloat(query %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			// ReadRegFloat()
*/
//-----------------------------------------------------------------------------------------------
