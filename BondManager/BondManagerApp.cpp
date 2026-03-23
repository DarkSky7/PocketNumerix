// NillaHedge.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"
#include "BondManagerApp.h"
#include "MainFrm.h"
#include "NillaDialog.h"
//#include "BOSDatabase.h"
//#include "NillaHedgeDoc.h"		// unused since substitution of BOSDatabase for NillaHedgeDoc
//#include "NillaHedgeView.h"
//#include "DirectoryPickerDialog.h"
#include	"BondManager.h"
//#include	"OptionManager.h"
//#include	"StockManager.h"
#include	"RegistryManager.h"

//#include "Position.h"
//#include "Stock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern	CString	months[];

/////////////////////////////////////////////////////////////////////////////
// CBondManagerApp

BEGIN_MESSAGE_MAP(CBondManagerApp, CWinApp)
	//{{AFX_MSG_MAP(CBondManagerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//	DO NOT EDIT what you see in these blocks of generated code!
	// Standard file based document commands
//	ON_COMMAND(ID_FILE_NEW,  OnFileOpen)
//	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(WM_ACTIVATE,  OnActivate)
	ON_COMMAND(WM_HIBERNATE, OnHibernate)
	ON_COMMAND(WM_CLOSE,	 OnClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// The one and only CBondManagerApp object

CBondManagerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBondManagerApp construction

CBondManagerApp::CBondManagerApp()
	: CWinApp()
	, dv( 0 )
	, na( NULL )
	, em( NULL )
	, uu( NULL )
	, hu( NULL )
{	OSVERSIONINFO osVer;
	GetVersionEx( &osVer );
	m_osVer = osVer.dwMajorVersion;

//	memset( &nid, 0, sizeof(nid) );
	// No special construction code ...
	// Place all significant initialization in InitInstance
}
//----------------------------------------------------------------------------
CBondManagerApp::~CBondManagerApp( void )
{
	if ( na ) delete [] na;
	if ( em ) delete [] em;
	if ( uu ) delete [] uu;
	if ( hu ) delete [] hu;
}			// ~CBondManagerApp()
//----------------------------------------------------------------------------
CString		CBondManagerApp::GetDBdir( void )
{
	return	dbDir;
}			// GetDBdir()
//---------------------------------------------------------------------------
CDataManager*	CBondManagerApp::GetDataManager( AssetType inst )
{
	return	bndMgr;
}			// GetDataManager()
//----------------------------------------------------------------------------
CMapStringToPtr*	CBondManagerApp::GetSymbolTable( AssetType inst )
{
	CDataManager*		dm = GetDataManager( inst );
	return ( dm != NULL )  ?  dm->GetSymbolTable()  :  NULL;
}			// GetSymbolTable()
//---------------------------------------------------------------------------
int		CBondManagerApp::GetSymbolTableSize( AssetType inst )
{
	CMapStringToPtr*	symTab = GetSymbolTable( inst );
	return	( symTab != NULL )  ?  symTab->GetCount()  :  0;
}			// GetSymbolTableSize()
//----------------------------------------------------------------------------
void	CBondManagerApp::OnActivate()
{		// read bread crumbs created in OnHibernate() and restore the previous state
#ifdef _DEBUG
	TRACE( _T("BondManagerApp::OnActivate: ...") );
#endif
}			// OnActivate()
//---------------------------------------------------------------------------
void	CBondManagerApp::OnHibernate()
{		// recover some working memory
		// symbolTables are probably taking up the most memory, so just kill those
#ifdef _DEBUG
	TRACE( _T("BondManagerApp::OnHibernate: ...") );
#endif
	if ( theApp.bndMgr )
		theApp.bndMgr->DeleteSymbolTables();

		// create bread crumbs so OnActivate() can bring back the current state later
}			// OnHibernate()
//---------------------------------------------------------------------------
void	CBondManagerApp::OnClose()
{		// close the database
#ifdef _DEBUG
	TRACE( _T("BondManagerApp::OnClose: ...") );
#endif
	if ( bndMgr )
	{	delete	bndMgr;
		bndMgr = NULL;
	}

//	delete	pDB;		// deletes symbol tables and closes DB files
//	pDB = NULL;
}			// OnClose()
//---------------------------------------------------------------------------
/*
void	CBondManagerApp::OnFileOpen()
{	
	CString	progName = _T("NillaHedge");
	CString	keyName = _T("DatabaseDirectory");
	CString	defDir = _T("\\My Documents\\NillaHedgeDB");
	CString dbDir = GetProfileString( progName, keyName, defDir );

	if ( CreateDirectory(dbDir, NULL) == 0 )
	{		// create failure, maybe the directory already exists
#ifdef _DEBUG
		TRACE( _T("BondManagerApp::OnFileOpen: Error(1) creating %s - last error = %d.\n"),
				dbDir, GetLastError() );
#endif
	}
	pDB = new CBOSDatabase( dbDir );
//	AddToRecentFileList( dbDir );
}			// OnFileOpen()
*/
//---------------------------------------------------------------------------
/*
void	CBondManagerApp::OnFileNew()
{	
	CString	progName = _T("NillaHedge");
	CString	keyName = _T("DatabaseDirectory");
	CString	defDir = _T("\\My Documents\\NillaHedgeDB");
	CWinApp*	theApp = AfxGetApp();
	dbDir = theApp->GetProfileString( progName, keyName, defDir );

	if ( CreateDirectory(dbDir, NULL) == 0 )
	{		// create failure, maybe the directory already exists
#ifdef _DEBUG
		TRACE( _T("BondManagerApp::OnFileNew: Error(1) creating %s - last error = %d.\n"),
				dbDir, GetLastError() );
#endif
	}
		// ask where user wants to put a new database directory
	pDB = new CBOSDatabase();
}			// OnFileNew()
*/
//---------------------------------------------------------------------------
BOOL	CALLBACK	AppSearcher( HWND hWnd, LPARAM lParam )
{
/*
	DWORD	dwAnswer;
	if (	! SendMessageTimeout( hWnd, WM_BONDMANAGER_ALREADY_RUNNING, 0, 0, 
								SMTO_BLOCK | SMTO_ABORTIFHUNG, 500, &dwAnswer )
		 ||  dwAnswer != WM_BONDMANAGER_ALREADY_RUNNING )
*/
	LRESULT	res = SendMessage( hWnd, WM_BONDMANAGER_ALREADY_RUNNING, 0, 0 );
    if ( res == WM_BONDMANAGER_ALREADY_RUNNING )
	{		// Application responded to our message: grab the HWND for further processing
		*(HWND*)lParam = hWnd;
		return	FALSE;							// found it
	}
	return	TRUE;			// indicates to EnumWindows that we can stop looking
}			// AppSearcher()
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CBondManagerApp initialization
BOOL	CBondManagerApp::InitInstance()
{		// If BM is already running, then focus on the window, and exit.
	bool	match = false;
	hMutex = CreateMutex(0, FALSE, _T("PocketNumerix/BondManager/Mutex"));
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
	wchar_t* regKey = _T("SOFTWARE\\PocketNumerix\\BondManager\\Init");			// date first used
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
			char*	appData = "pktNumerixBondManager";
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
				TRACE( _T("BondManagerApp::InitInstance: hu='%s', huidHex='%s'\n"), csHu, csHex );
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

		// Change the registry key under which our settings are stored.
		// You should modify this string to be something appropriate
		// such as the name of your company or organization.
	SetRegistryKey( _T("PocketNumerix") );
	LoadIcon( IDI_BondManager );
/*
		// Standard initialization
		// If you are not using these features and wish to reduce the size
		//  of your final executable, you should remove from the following
		//  the specific initialization routines you do not need.
	LoadStdProfileSettings();					// Track most recently used DBs

		// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	ParseCommandLine(cmdInfo);

		// Dispatch commands specified on the command line
	BOOL  bResult = ProcessShellCommand(cmdInfo);
	if ( !bResult )
	{		// if there is no file in the doclist, we will create a new one.
		pDocTemplate->ShowDocList();
		CCeDocList* pDL = pDocTemplate->m_pWndDocList;
		short	nDocs = pDL->GetItemCount();
		if ( nDocs == 0 )
		{	pDL->OnClose();
			OnFileNew();		// the resulting FrameWin makes various MFC ASSERTs happy
		}
	}
		// using app variable pDB obviates the need to go through the
		// following everywhere we want to get to the database
	POSITION pos = pDocTemplate->GetFirstDocPosition();
	CDocument* pDoc = pDocTemplate->GetNextDoc( pos );

		// BondManager currently doesn't have a network interface
	if ( !AfxSocketInit() )
	{	AfxMessageBox( IDP_SOCKETS_INIT_FAILED );
		return	FALSE;
	}
	AfxEnableControlContainer();
*/
		// Background (light gray)	& Text (black) colors
#if ( PPC02_TARGET )
		// Pocket PC
//  SHInitExtraControls();					// causes VS8-PPC'03 link errors
			// use black text over light gray background in all Dialogs
		SetDialogBkColor( RGB(208,208,208), RGB(0,0,0) );
//		SetDialogBkColor( RGB(224,224,224), RGB(0,0,0) );
#else
		// - - - - - - - - - - - - - - - - - -
		// make sure screen orientation is Portrait
//#if ( PPC03_TARGET  ||  WM5_TARGET ) 						// not Pocket PPC02
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
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// create the MainFrame
		// this Create is required when we don't use a DocList above
	CMainFrame*	pWnd = new CMainFrame;
	pWnd->Create( NULL, _T("BondManager") );
	pWnd->ShowWindow( SW_SHOW );				// gets you a caption and a Switcher entry
	m_pMainWnd = pWnd;							// load a JPEG into the background ? (XXX)

//	CWnd*	pWnd = CWnd::FromHandle( m_pMainWnd );
//	hWnd = CreateWindow( NULL, _T("BondManager"), WS_CAPTION,
//				  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
//				  pWnd, NULL, m_hInstance, NULL );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// add a Shell Notification Icon to the system tray...
		// displays an icon, but haven't figured out how to ShowWindow after clicking on it
	nid.cbSize	= sizeof( nid );		// on PPC'02 this is 152 bytes long
#ifdef _DEBUG
//	DWORD	ullVersion = GetDllVersion( _T("shell32.dll") );
//	TRACE( _T("BondManagerApp::InitInstance: ullVersion=%ld, sizeof(nid)=%d\n"),
//			ullVersion, nid.cbSize );
#endif
	nid.uID	   = 100;								// values from 0 to 12 are reserved.
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_ICON_NOTIFY;
	nid.hWnd = pWnd->GetSafeHwnd();					// receives uCallbackMessage = WM_ICON_NOTIFY
	wcscpy( nid.szTip, _T("BondManager") );			// balloon help?
		// according to <http://www.codeguru.com/forum/archive/index.php/t-194324.html>
		// the following are needed rather than just LoadIcon(), and he's right
	nid.hIcon	 = (HICON)LoadImage( AfxGetInstanceHandle(),
									MAKEINTRESOURCE(IDI_BondManager),
									IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR );
    BOOL	okay = Shell_NotifyIcon( NIM_ADD, &nid );
#ifdef _DEBUG
	if ( ! okay )
		TRACE( _T("BondManagerApp::InitInstance: Shell_NotifyIcon(ADD) failed\n") );
#endif

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// do housekeeping needed to open the database

		// look up the PocketNumerix document directory in the registry
	DWORD			slen = 0;					// number of bytes in directory name (though it's a wide string).
	wchar_t*		dirName = NULL;
	const wchar_t*	docDirKey = _T("SOFTWARE\\PocketNumerix\\DocumentDirectory");
	success = ReadRegBinary( HKEY_CURRENT_USER, docDirKey, (BYTE**)&dirName, &slen );			// allocates
#ifdef _DEBUG
	TRACE( _T("BondManager::InitInstance: DocumentDirectory='%s', slen=%d\n"), dirName, slen );
#endif
	if ( ! success )
	{	delete [] dirName;
		ExitPrep( 0 );
	}
/*
	if ( ! success )
	{
#ifdef _DEBUG
		TRACE( _T("BondManager::InitInstance: Couldn't write %s to %s\n"), dirName, docDirKey );
#endif	
			// regstered directory should be set up in the installer, so we should never run this block
		wchar_t	docDir[MAX_PATH];
		if ( ! SHGetDocumentsFolder( _T("\\"), docDir) )
		{	
			wcscpy( docDir, _T("\\My Documents") );
			CreateDirectory( docDir, NULL );				// this will fail if the directory already exists
		}
		swprintf( dirName, _T("%s\\PocketNumerix"), docDir );
		if ( 0 != CreateDirectory(dirName, NULL) )			// it would be serious if this failed
		{
#ifdef _DEBUG
			TRACE( _T("BondManager::InitInstance: Unable to create directory: %s (could already exist)\n"), dirName );
#endif
		}
		success = WriteRegString( HKEY_CURRENT_USER, docDirKey, (BYTE*)dirName, (slen+1)*sizeof(wchar_t) );
	}
*/
		// set up the dbDir instance variable
	dbDir = dirName;				// char to CString conversion
	delete [] dirName;				// allocated by ReadRegString()
#ifdef _DEBUG
	TRACE( _T("BondManagerApp::InitInstance: dbDir=%s\n"), dbDir );
#endif
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// open the database... BM's data directory is directly beneath dbDir
	bndMgr = new CBondManager( _T("\\Bonds") );
	return	TRUE;
}			// InitInstance()
//---------------------------------------------------------------------------
void	CBondManagerApp::ExitPrep( short code )
{
	bool epilogue = ( code < 0 );
	if ( epilogue )
	{
#ifdef _DEBUG
		if ( code == -1 )
		{	CString sn = GetSerialNumber();
			CString wu = uu;
			TRACE( _T("BondManagerApp:InitInstance: license uuid '%s' doesn't match device uuid '%s'\n"), wu, sn );
		}
		else if ( code == -2 )
			TRACE( _T("BondManagerApp:InitInstance: license huid doesn't match device huid\n") );
		else if ( code == -3 )
			TRACE( _T("BondManagerApp:InitInstance: couldn't get serial number\n") );
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
		wchar_t*	regKey = _T("SOFTWARE\\PocketNumerix\\BondManager");
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
int		CBondManagerApp::ExitInstance()
{	
	    // Remove the notification icon from the system tray.
	BOOL	okay = Shell_NotifyIcon( NIM_DELETE, &nid );
#ifdef _DEBUG
	if ( ! okay )
		TRACE( _T("BondManagerApp::InitInstance: Shell_NotifyIcon(DELETE) failed\n") );
#endif

	BOOL	res = ReleaseMutex( hMutex );
#ifdef _DEBUG
	if ( res != 0 )
		TRACE( _T("BondManagerApp::ExitInstance: ReleaseMutex failed\n") );
#endif
	delete	bndMgr;
	return	CWinApp::ExitInstance();
}			// ExitInstance()
//----------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CNillaDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_AboutBox };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL	OnInitDialog();		// Added for WCE apps
	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CNillaDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CNillaDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CNillaDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void	CBondManagerApp::OnAppAbout()
{
	CAboutDlg	aboutDlg;
	aboutDlg.DoModal();
}

CString		months[12] = {	_T("Jan"), _T("Feb"), _T("Mar"), _T("Apr"),
							_T("May"), _T("Jun"), _T("Jul"), _T("Aug"),
							_T("Sep"), _T("Oct"), _T("Nov"), _T("Dec") };

CString		numberStrings[13] = {	_T("zero"), _T("one"),	_T("two"),	_T("three"), _T("four"),
									_T("five"),	_T("six"),	_T("seven"), _T("eight"), _T("nine"),
									_T("ten"),	_T("eleven"), _T("twelve") };

	/////////////////////////////////////////////////////////////////////////////
// CBondManagerApp commands
// Added for WCE apps

BOOL	CAboutDlg::OnInitDialog() 
{
	CNillaDialog::OnInitDialog();
	
	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
//----------------------------------------------------------------------------------------
HBRUSH	CAboutDlg::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor ) 
{
	return	CNillaDialog ::OnCtlColor( pDC, pWnd, nCtlColor );
}			// OnCtlColor()
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
	{	if		(     0.1  <= dd  &&  dd <	   0.2  )	dd =	  0.2;
		else if	(     0.2  <= dd  &&  dd <     0.5  )	dd =	  0.5;	
		else if (     0.5  <= dd					)	dd =	  1.0;
	}
	else if ( dd < 10.0 )
	{	if		(     1.0  <= dd  &&  dd <     2.0  )	dd =	  2.0;
		else if (     2.0  <= dd  &&  dd <     5.0  )	dd =	  5.0;
		else if (     5.0  <= dd					)	dd =	 10.0;
	}
	else if ( dd < 100.0 )
	{	if		(    10.0  <= dd  &&  dd <    20.0  )	dd =	 20.0;
		else if (    20.0  <= dd  &&  dd <    50.0  )	dd =	 50.0;
		else if (    50.0  <= dd					)	dd =	100.0;
	}
	else if ( dd < 1000.0 )
	{	if		(   100.0  <= dd  &&  dd <   200.0  )	dd =	200.0;
		else if (   200.0  <= dd  &&  dd <   500.0  )	dd =	500.0;
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
//----------------------------------------------------------------------------------------
long	FindSubStr( char* str, long skipCnt, char* sub )
{		// works like CString::Find()
		// skipCnt is an offset from Beginning of str
	char*	origStr = str;
	char*	tokStart = origStr + skipCnt;
	unsigned long	ii = 0;							// offset within sub
	str += skipCnt;									// begin after skipCnt characters
	while ( *str != '\0'  &&  *(sub+ii) != '\0' )
	{	bool	match = *str == *(sub+ii);
		if ( match  &&  ii == 0 )
			tokStart = str;
		ii = match  ?  ii+1  :  0;
		str++;
	}
	return	ii == strlen(sub)  ?  tokStart - origStr  :  -1;
}			// FindSubStr()
//----------------------------------------------------------------------------------------
char*	ExtractToken( char* szCache, long tokLength )
{		// creates a heap-based character string.
		// Caller is responsible for deleting the return string 
	char* token = new char[ tokLength + 1 ];
	strncpy( token, szCache, tokLength );
	*(token+tokLength) = '\0';
	return	token;
}			// ExtractToken()
//----------------------------------------------------------------------------------------
char*			CString2char( CString str )
{	long	ss = 0;
	long	sLen = str.GetLength();
	char*	result = new char[ sLen + 1 ];
	while ( ss < sLen )
		*(result + ss++) = (char)str.GetAt( ss );
	*(result + sLen) = '\0';
	return	result;
}			// CString2char()
//----------------------------------------------------------------------------------------
void	EatWhitespace( char* str )
{
	long	nb = 0;			// the number of whitespace characters in str
	while ( *str != '\0' )
	{	if ( *str == ' '  ||  *str == '\t'  ||  *str == '\n'  ||  *str == '\r' )
			nb++;
		if ( nb > 0 )
			*str = *(str+nb);
		str++;
	}
}			// EatWhitespace()
//----------------------------------------------------------------------------------------
void		EjectChar( char* str, char ch )
{		// modify str in place, killing any <ch> characters
	long	nb = 0;				// the number of <ch> characters in str
	while ( *str != '\0' )
	{	if ( *str == ch )
			nb++;
		if ( nb > 0 )
			*str = *(str+nb);
		str++;
	}
}			// EjectChar()
//--------------------------------------------------------------------
CString			EjectChar( CString src, wchar_t ch )
{		// return a copy of src without any <ch> characters
	int	ii = 0;
	CString	tgt = _T("");					// the return value
	int	sLen = src.GetLength();
	int	jj = src.Find( ch, ii );			// points at the character to eject
	while ( jj >= 0 )
	{	tgt += src.Mid( ii, jj - ii );		// copies up to but not including <ch>
			// skip that <ch>
		ii = jj + 1;
		if ( ii >= sLen )
			return	tgt;
		jj = src.Find( ch, ii );
	}
	return	tgt + src.Mid( ii, sLen - ii );
}			// EjectChar()
//--------------------------------------------------------------------
/*
long			packOleDate( COleDateTime aDate )
{
	int		yr = aDate.GetYear();				// 15 bits
	int		mo = aDate.GetMonth();				//  4 bits
	int		da = aDate.GetDay();				//  5 bits
//	yr &= 0x7FFF;								// range = 0 .. 32767
	return	(yr << 9)  |  (mo << 5)  |  da;
}			// packOleDate()
*/
//----------------------------------------------------------------------------------------
/*
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
*/
//----------------------------------------------------------------------------------------
CString			EuroFormat( COleDateTime aDate )
{	int da = aDate.GetDay();
	int mo = aDate.GetMonth();
	int	yr = aDate.GetYear();

	ASSERT( da > 0  &&  mo > 0  &&  yr > 0 );
	wchar_t	buf[12];
	swprintf( buf, _T("%02d%3s%4d"), da, months[mo-1], yr );
	CString	cs(buf);
	return	cs;
}			// EuroFormat()
//----------------------------------------------------------------------------------------
long		MakeLongMonYr( CString monYr )
{
#ifdef _DEBUG
	int	len = monYr.GetLength();
	if ( len < 5 )
	{	TRACE( _T("MakeLongMonYr: GetLength(%s) --> %d\n"), monYr, len );
		AfxDebugBreak();
	}
#endif
	CString	moSt = monYr.Mid( 0, 3 );
	CString	yrSt = monYr.Mid( 3 );					// gets the remainder of monYr
	int		ii = intMonthFrom3CharStr( moSt );		// Jan is 0-based
	int		yr = 2000 + _wtoi( yrSt );
	return	(yr << 4) + ii;			// e.g. Dec05 --> 200511 (so we can sort on it)
}			// MakeLongMonYr()
//--------------------------------------------------------------------------------------
int		intMonthFrom3CharStr( CString cs_mo )
{
	for ( unsigned short ii = 0; ii < 12; ii++ )
		if ( cs_mo.CompareNoCase( months[ii] ) == 0 )		// they're equal
			return ii;										// interval [0..11]
	return -1;
}			// intMonthFrom3CharStr()
//--------------------------------------------------------------------------------------
int		numLeapDays( COleDateTime d1, COleDateTime d2 )
{
	int leapDays = 0;								// return value
	COleDateTime ed = ( d1 < d2 ) ? d1 : d2;		// the earlier date
	COleDateTime ld = ( d1 < d2 ) ? d2 : d1;		// the later date
	int	ey = ed.GetYear() + 1;						// the earlier year
	int	ly = ld.GetYear() - 1;						// the later year

		// count leap years between the given dates
	int	ym1 = ly - ey - 1;
	if ( ym1 > 0 ) leapDays += (ym1 / 400) + (ym1 / 4) - (ym1 / 100);

		// if early date is a leap year and its on or before Feb-29, bump leaps
	if ( isLeapYr(ey) )
	{	COleDateTime eld( ey, 2, 29, 0, 0, 0 );
		if ( ed <= eld )
			leapDays++;
	}
		// if late date is a leap year and its on or after Feb-29, bump leaps
	if ( isLeapYr(ly) )
	{	COleDateTime lld( ly, 2, 29, 0, 0, 0 );
		if ( ld >= lld )
			leapDays++;
	}
	return 	leapDays;
}			// numLeapDays()
//----------------------------------------------------------------------------------------
COleDateTime	dayBefore( COleDateTime refDate, int yr )		// use refDate's year when yr = 0
{		// XXX forgot why this function is necessary - why not just subtract a day off the refDate?
		// useful for creating a pseudo (accrual accounting) dividend date from an Ex-Dividend date
	if ( yr == 0 ) yr = refDate.GetYear();		// valid COleDateTime's have yr >= 100, so...
	int		mo = refDate.GetMonth();			// we're not guaranteeing validity for MFC
	int		da = refDate.GetDay() - 1;			// consider divDate to be the day before exDivDate
		// Currently doesn't account for non-trading days... XXX
	if ( da < 1 )
	{	mo = (mo > 1) ? mo - 1 : yr--, 12;
		if ( mo == 2 )
			da = (yr % 4 == 0) ? 29 : 28;
		else if ( mo == 4  ||  mo ==  6  ||  mo ==  9  ||  mo == 11 )
			da = 30;		// April, June, September, November
		else				// mo == 1, 3, 5, 7, 8, 10, 12
			da = 31;
	}
	COleDateTime	dayBefore;			// return value
	dayBefore.SetDate( yr, mo, da );
	return	dayBefore;
}			// dayBefore()
//----------------------------------------------------------------------------------------------------
