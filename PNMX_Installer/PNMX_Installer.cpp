// PNMX_Installer.cpp : Defines the class behaviors for the application.
//
#include "StdAfx.h"
#include "PNMX_Installer.h"
#include <assert.h>
#include "PNMX_InstallerDlg.h"
#include "LicenseDialog.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPNMX_InstallerApp

BEGIN_MESSAGE_MAP(CPNMX_InstallerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPNMX_InstallerApp construction

CPNMX_InstallerApp::CPNMX_InstallerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}
//-------------------------------------------------------------------------------
// The one and only CPNMX_InstallerApp object

CPNMX_InstallerApp theApp;

//-------------------------------------------------------------------------------
// CPNMX_InstallerApp initialization
//---------------------------------------------------------------------------
BOOL	CALLBACK	AppSearcher( HWND hWnd, LPARAM lParam )
{
/*
	DWORD	dwAnswer;
	if (	! SendMessageTimeout( hWnd, WM_PNMX_INSTALLER_ALREADY_RUNNING, 0, 0, 
								SMTO_BLOCK | SMTO_ABORTIFHUNG, 500, &dwAnswer )
		 ||  dwAnswer != WM_PNMX_INSTALLER_ALREADY_RUNNING )
*/
	LRESULT	res = SendMessage( hWnd, (UINT)WM_PNMX_INSTALLER_ALREADY_RUNNING, 0, 0 );
    if ( res == WM_PNMX_INSTALLER_ALREADY_RUNNING )
	{		// Application responded to our message: grab the HWND for further processing
		*(HWND*)lParam = hWnd;
		return	FALSE;							// found it
	}
	return	TRUE;			// indicates to EnumWindows that we can stop looking
}			// AppSearcher()
//---------------------------------------------------------------------------
BOOL CPNMX_InstallerApp::InitInstance()
{		// prevent multiple instances from running...
	hMutex = CreateMutex( 0, FALSE, _T("PocketNumerix/PNMX_Installer/Mutex") );
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
		// okay, we're clear
	CWinApp::InitInstance();
/*
		// ---------------------------------------------
		// all of this crap to set the application icon
	WNDCLASS wndcls;
	memset( &wndcls, 0, sizeof(WNDCLASS) );		// start with NULL defaults
	wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wndcls.lpfnWndProc = ::DefWindowProc; 
	wndcls.hInstance = AfxGetInstanceHandle();
	wndcls.hIcon = LoadIcon(IDR_MAINFRAME);		// or load a different icon
	wndcls.hCursor = LoadCursor( IDC_ARROW );
	wndcls.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wndcls.lpszMenuName = NULL;

		// Specify your own class name for using FindWindow later
	wndcls.lpszClassName = _T("PNMX_Installer");

		// Register the new class and exit if it fails
	if ( ! AfxRegisterClass(&wndcls) )
	{
		TRACE("Class Registration Failed\n");
		return FALSE;
	}
		// -------- end of crap to set the application icon ---------
*/
	if ( !AfxSocketInit() )
	{
		AfxMessageBox( IDP_SOCKETS_INIT_FAILED );
		return	FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("PocketNumerix Installer"));

/*
		// attempt to load rapi.dll, ceutil.dll, shlwapi.dll
		// this code has no effect on detection of missing dlls - the OS gets there first
	wchar_t* libNames[3] = { _T("rapi.dll"), _T("ceutil.dll"), _T("shlwapi.dll") };
	HMODULE	hMod[3];
	short	libCnt = 0;
	for ( short ii = 0; ii < 3; ii++ )
	{	*(hMod+ii) = LoadLibrary( libNames[ii] );
		if ( *(hMod+ii) )
			libCnt++;
	}
	if ( libCnt < 3 )
	{	CString libList = _T("");
		for ( short ii = 0; ii < 3; ii++ )
			if ( *(hMod+ii) == NULL )
			{	if ( libList != _T("") )
					libList += _T(", ");
				libList += *(libNames+ii);
			}
		libList = _T("Couldn't load: ") + libList;
		MessageBox( NULL, libList, _T("Missing DLLs"), MB_OK );
	}
	else ... the rest of the function
*/
	CLicenseDialog licenseDlg;
	INT_PTR nResponse = licenseDlg.DoModal();
	if ( nResponse == IDOK )
	{		// Here's where we capture the device ID & operating system version
		CPNMX_InstallerDlg dlg;
		m_pMainWnd = &dlg;				// terminates dialog thread when dlg is closed

			// both of these fail asserts
	//	AfxGetMainWnd()->SetIcon( theApp.LoadIcon( IDR_MAINFRAME ), TRUE );
			// this one finds m_pMainWnd itself to be nonNULL, but points to NULL anyway
	//	theApp.GetMainWnd()->SetIcon( theApp.LoadIcon( IDR_MAINFRAME ), TRUE );
		// the following works m_hIconfor everything but the File Explorer icon
	//	SendMessage( dlg.GetSafeHwnd(), WM_SETICON, TRUE, (LPARAM)LoadIcon( IDR_MAINFRAME ) );

		nResponse = dlg.DoModal();
	//	if ( dlg.m_DeviceConnected )
	//		CeRapiUninit();
			// nResponse = 1 upon RET; nResponse = 2 upon ESC (cancel button)

			// Since the dialog has been closed, return FALSE so that we exit the
			// application, rather than start the application's message pump.
	}
	return	FALSE;
}			// InitInstance()
//---------------------------------------------------------------------------
int		CPNMX_InstallerApp::ExitInstance( void )
{	
		// close the WinSock DLL
	WSACleanup();

	BOOL	res = ReleaseMutex( hMutex );
#ifdef _DEBUG
	if ( res != 0 )
		TRACE( _T("CPNMX_InstallerApp::ExitInstance: ReleaseMutex failed\n") );
#endif
	return	CWinApp::ExitInstance();
}			// ExitInstance()
//----------------------------------------------------------------------------
wchar_t*	MakeWideString( const char* src )
{
	size_t len = strlen( src );
	TCHAR* tgt = new TCHAR[ len + 1 ];
	TCHAR* result = tgt;
	while ( *src != '\0' )
		*tgt++ = (wchar_t)*src++;
	*tgt = '\0';
	return	result;
}			// MakeWideString()
//----------------------------------------------------------------------------
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
char*	GetToken( char* buf, int& ii, char term /* = NULL */ )
{		// bypass leading whitespace
	while ( *(buf+ii) != '\0'  &&  iswhite( *(buf+ii) )  )
		ii++;
		// *(buf+ii) is either non-white or the EOS
	short	jj = ii;									// bookmark this position in the string
	if ( term )							// break on term match
		while ( *(buf+ii) != '\0' )
		{	if ( *(buf+ii) == term )
				break;
			ii++;
		}
	else								// break on white space
		while ( *(buf+ii) != '\0' )
		{	if ( iswhite( *(buf+ii) ) )
				break;
			ii++;
		}

	if( ii == jj )
		return	NULL;									// hit the end of buf
	short	len = ii - jj;
	char* res = new char[ len + 1 ];
	strncpy_s( res, len+1, buf+jj, len );		// from the beginning of the token (jj)
		// from here on, ii is the token length, not an offset into buf
	*(res+len) = '\0';
	return	res;							
}			// GetToken()
//-----------------------------------------------------------------------------------------------
void	FormatMemStr( int byteCount, wchar_t* wbuf, DWORD maxChars )
{	
	int KBavail = ( byteCount >> 10 );				// still has MB's in it
//	Bavail -= (KBavail << 10 );						// the residual byte count
	int MBavail = ( KBavail >> 10 );				// it's final value
	KBavail -= ( MBavail << 10 );					// it's final value 

	if ( MBavail > 0 )
		swprintf( wbuf, maxChars, _T("%d,%03d KB"), MBavail, KBavail );
	else if ( KBavail > 0 )
		swprintf( wbuf, maxChars, _T("   %3d KB"),           KBavail );
	else
		swprintf( wbuf, maxChars, _T("   %3d  B"),			 byteCount );
}			// FormatMemStr()
//--------------------------------------------------------------------------------------------
char*	NumToBase62( unsigned int num )
{		// convert non-negative integers to base 62
	char	buf[ 16 ];					// moderate overkill
	char*	bb = buf;
	do
	{	unsigned int quo = (unsigned int)( num / 62 );
		unsigned int rem = num - 62 * quo;
		if ( rem < 10  )
			*bb++ = (char)( rem + 48 );			// -0 + 48 ('0')
		else if ( 10 <= rem  &&  rem < 36 )
			*bb++ = (char)( rem + 87 );			// -10 + 97 ('a')
		else									// hopefully,  36 <= $num  &&  $num < 62
			*bb++ = (char)( rem  + 29 );		// -36 + 65 ('A')
		num = quo;
	} while ( num > 0 );
	*bb = '\0';
	size_t slen = strlen( buf );
	char* res = new char[ slen + 1 ];			// on the heap
	strcpy_s( res, slen+1, buf );
	return  res;
}			// NumToBase62()
//--------------------------------------------------------------------------------------------
// rand_s() support
char*	MakeRanName( void )
{	unsigned int rndy;
	errno_t err = rand_s( &rndy );
	char* n1 = NumToBase62( rndy );				// n1 on the heap
	if ( n1 == NULL ) return NULL;

	err = rand_s( &rndy );
	char* n2 = NumToBase62( rndy ); 			// n2 on the heap
	if ( n2 == NULL ) return NULL;

	err = rand_s( &rndy );
	char* n3 = NumToBase62( rndy ); 			// n3 on the heap
	if ( n3 == NULL ) return NULL;

	err = rand_s( &rndy );
	char* n4 = NumToBase62( rndy );				// n4 on the heap
	if ( n4 == NULL ) return NULL;

	char	buf[ 48 ];
	sprintf_s( buf, 48, "%s%s%s%s", n1, n2, n3, n4 );
	size_t slen = strlen( buf );
	char*	res = new char[ slen + 1 ];				// on the heap
	strcpy_s( res, slen+1, buf );
	delete [] n1;	delete [] n2;	delete [] n3;	delete [] n4;
	return	res;
}			// MakeRanName()
//--------------------------------------------------------------------------------------------
