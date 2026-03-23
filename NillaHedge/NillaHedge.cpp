// NillaHedge.cpp : Defines the class behaviors for the application.
//

#include "StdAfx.h"
#include "NillaHedge.h"
#include "NillaDialog.h"
#include "MainFrm.h"
#include "DateSupport.h"
//#include "BOSDatabase.h"
//#include "NillaHedgeDoc.h"		// unused since substitution of BOSDatabase for NillaHedgeDoc
//#include "NillaHedgeView.h"
//#include "DirectoryPickerDialog.h"
//#include	"BondManager.h"
#include	"OptionManager.h"
#include	"StockManager.h"
#include	"RegistryManager.h"
#include "GetDeviceInfo.h"
#include "utils.h"
//#include "Position.h"
//#include "Stock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern	CString	months[];

/////////////////////////////////////////////////////////////////////////////
// CNillaHedgeApp

BEGIN_MESSAGE_MAP(CNillaHedgeApp, CWinApp)
	//{{AFX_MSG_MAP(CNillaHedgeApp)
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
// The one and only CNillaHedgeApp object

CNillaHedgeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CNillaHedgeApp construction

CNillaHedgeApp::CNillaHedgeApp()
	: CWinApp()
	, dv( 0 )
	, na( NULL )
	, em( NULL )
	, uu( NULL )
	, hu( NULL )
	, lpFileName( NULL )
	, lpPageName( NULL )
	, lpServerName( NULL )
	, urlRequest( NULL )
{	OSVERSIONINFO osVer;
	GetVersionEx( &osVer );
	m_osVer = osVer.dwMajorVersion;

	// No special construction code ...
	// Place all significant initialization in InitInstance
}
CNillaHedgeApp::~CNillaHedgeApp()
{
	if ( na ) delete [] na;
	if ( em ) delete [] em;
	if ( uu ) delete [] uu;
	if ( hu ) delete [] hu;

	if ( lpFileName ) delete [] lpFileName;
	if ( lpPageName ) delete [] lpPageName;
	if ( lpServerName ) delete [] lpServerName;
	if ( urlRequest ) delete [] urlRequest;
}
//----------------------------------------------------------------------------
CString		CNillaHedgeApp::GetDBdir( void )
{
	return	dbDir;
}			// GetDBdir()
//---------------------------------------------------------------------------
CDataManager*	CNillaHedgeApp::GetDataManager( AssetType inst )
{
	CDataManager*	dm = NULL;
	if ( inst == Option )
		dm = optMgr;
	else if ( inst == Stock )
		dm = stkMgr;
	return	dm;
}			// GetDataManager()
//----------------------------------------------------------------------------
CMapStringToPtr*	CNillaHedgeApp::GetSymbolTable( AssetType inst )
{
	CDataManager*		dm = GetDataManager( inst );
	return ( dm != NULL )  ?  dm->GetSymbolTable()  :  NULL;
}			// GetSymbolTable()
//---------------------------------------------------------------------------
int		CNillaHedgeApp::GetSymbolTableSize( AssetType inst )
{
	CMapStringToPtr*	symTab = GetSymbolTable( inst );
	return	( symTab != NULL )  ?  symTab->GetCount()  :  0;
}			// GetSymbolTableSize()
//----------------------------------------------------------------------------
void	CNillaHedgeApp::OnActivate()
{		// read bread crumbs created in OnHibernate() and restore the previous state
#ifdef _DEBUG
	TRACE( _T("NillaHedgeApp::OnActivate: ...") );
#endif
}			// OnActivate()
//---------------------------------------------------------------------------
void	CNillaHedgeApp::OnHibernate()
{		// recover some working memory
		// symbolTables are probably taking up the most memory, so just kill those
#ifdef _DEBUG
	TRACE( _T("NillaHedgeApp::OnHibernate: ...") );
#endif
	if ( theApp.optMgr )
		theApp.optMgr->DeleteSymbolTables();
	
	if ( theApp.stkMgr )
		theApp.stkMgr->DeleteSymbolTables();
		// create bread crumbs so OnActivate() can bring back the current state later
}			// OnHibernate()
//---------------------------------------------------------------------------
void	CNillaHedgeApp::OnClose()
{		// close the database
#ifdef _DEBUG
	TRACE( _T("NillaHedgeApp::OnClose: ...") );
#endif
	if ( optMgr )
	{	delete	optMgr;
		optMgr = NULL;
	}
	if ( stkMgr )
	{	delete	stkMgr;
		stkMgr = NULL;
	}
/*
	if ( regMgr )
	{	delete	regMgr;
		regMgr = NULL;
	}
*/
//	delete	pDB;		// deletes symbol tables and closes DB files
//	pDB = NULL;
}			// OnClose()
//---------------------------------------------------------------------------
/*
void	CNillaHedgeApp::OnFileOpen()
{	
	CString	progName = _T("NillaHedge");
	CString	keyName = _T("DatabaseDirectory");
	CString	defDir = _T("\\My Documents\\NillaHedgeDB");
	CString dbDir = GetProfileString( progName, keyName, defDir );

	if ( CreateDirectory(dbDir, NULL) == 0 )
	{		// create failure, maybe the directory already exists
#ifdef _DEBUG
		TRACE( _T("NillaHedgeApp::OnFileOpen: Error(1) creating %s - last error = %d.\n"),
				dbDir, GetLastError() );
#endif
	}
	pDB = new CBOSDatabase( dbDir );
//	AddToRecentFileList( dbDir );
}			// OnFileOpen()
*/
//---------------------------------------------------------------------------
/*
void	CNillaHedgeApp::OnFileNew()
{	
	CString	progName = _T("NillaHedge");
	CString	keyName = _T("DatabaseDirectory");
	CString	defDir = _T("\\My Documents\\NillaHedgeDB");
	CWinApp*	theApp = AfxGetApp();
	dbDir = theApp->GetProfileString( progName, keyName, defDir );

	if ( CreateDirectory(dbDir, NULL) == 0 )
	{		// create failure, maybe the directory already exists
#ifdef _DEBUG
		TRACE( _T("NillaHedgeApp::OnFileNew: Error(1) creating %s - last error = %d.\n"),
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
	if (	! SendMessageTimeout( hWnd, WM_NILLAHEDGE_ALREADY_RUNNING, 0, 0, 
								SMTO_BLOCK | SMTO_ABORTIFHUNG, 500, &dwAnswer )
		 ||  dwAnswer != WM_NILLAHEDGE_ALREADY_RUNNING )
*/
	LRESULT	res = SendMessage( hWnd, WM_NILLAHEDGE_ALREADY_RUNNING, 0, 0 );
    if ( res == WM_NILLAHEDGE_ALREADY_RUNNING )
	{		// Application responded to our message: grab the HWND for further processing
		*(HWND*)lParam = hWnd;
		return	FALSE;							// found it
	}
	return	TRUE;			// indicates to EnumWindows that we can stop looking
}			// AppSearcher()
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CNillaHedgeApp initialization
BOOL	CNillaHedgeApp::InitInstance()
{		// if NH is already running, then focus on the window, and exit.
	bool	match = false;
	hMutex = CreateMutex(0, FALSE, _T("PocketNumerix/NillaHedge/Mutex"));
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
	bool success;

		// do license verification
	unsigned long packedDate = 0;
	wchar_t* regKey = _T("SOFTWARE\\PocketNumerix\\NillaHedge\\Init");			// date first used
	COleDateTime now = COleDateTime::GetCurrentTime();
		// cannot use "dt = now" on PPC02 because of the year 2000 bug in MFC4, hence dt = GetCurrentTime() instead
	COleDateTime dt = COleDateTime::GetCurrentTime();				// if we've already set a date, override the 'now' value in dt
	success = ReadRegBinary( HKEY_LOCAL_MACHINE, regKey, (BYTE*)&packedDate, 3 );
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
			char*	appData = "pcktNumerixNillaHedge";
			DWORD	appLen = strlen( appData );
			BYTE	huid[48];
			DWORD	lenOut = 48;
			HRESULT hr = GetDeviceUniqueID( reinterpret_cast<BYTE*>(appData), appLen, 1, huid, &lenOut );
				// HEX ANSI  <--  BYTE* (actually a very long integer)
			if ( hr == S_OK )
			{		// MessageBox( GetParent(0), _T("GetDeviceUniqueID"), _T("S_OK"), 0 );
				char* huidHex = MakeHexString( huid, lenOut );		// uuidHex is little-endian reversed
				match = ( strcmp(huidHex,(char*)hu) == 0 );
#ifdef _DEBUG
				CString csHu( hu );
				CString csHex( huidHex );
				TRACE( _T("NillaHedgeApp::InitInstance: hu='%s', huidHex='%s'\n"), csHu, csHex );
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
	LoadIcon( IDI_NillaHedge );

		// Standard initialization
		// If you are not using these features and wish to reduce the size
		//  of your final executable, you should remove from the following
		//  the specific initialization routines you do not need.
//	LoadStdProfileSettings();					// Track most recently used DBs

/*
	if ( !AfxSocketInit() )
	{	AfxMessageBox( IDP_SOCKETS_INIT_FAILED );
		return	FALSE;
	}
	AfxEnableControlContainer();
*/
/*
	CCeDocListDocTemplate* pDocTemplate;
	pDocTemplate = new CCeDocListDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CBOSDatabase),
		RUNTIME_CLASS(CMainFrame),				// main SDI frame window
		RUNTIME_CLASS(CNillaHedgeView)	);
	AddDocTemplate(pDocTemplate);
*/
/*
		// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	ParseCommandLine(cmdInfo);

		// Dispatch commands specified on the command line
	BOOL  bResult = ProcessShellCommand(cmdInfo);
*/
/*
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
*/
	SHInitExtraControls();
		// Background (light gray)	& Text (black) colors
#if ( PPC02_TARGET )			// Pocket PC 2002
			// use black text over light gray background in all Dialogs
		SetDialogBkColor( RGB(208,208,208), RGB(0,0,0) );
//		SetDialogBkColor( RGB(224,224,224), RGB(0,0,0) );
#else
		// - - - - - - - - - - - - - - - - - -
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

		// create the MainFrame
		// this Create is required when we don't use a DocList above
	CMainFrame*	pWnd = new CMainFrame;
	pWnd->Create( NULL, _T("NillaHedge") );
	pWnd->ShowWindow( SW_SHOW );				// SW_SHOW gets you a caption and a Switcher entry
	m_pMainWnd = pWnd;

//	CWnd*	pWnd = CWnd::FromHandle( m_pMainWnd );
//	hWnd = CreateWindow( NULL, _T("NillaHedge"), WS_CAPTION,
//				  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
//				  pWnd, NULL, m_hInstance, NULL );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// add a Shell Notification Icon to the system tray...
		// displays an icon, but haven't figured out how to ShowWindow after clicking on it
	nid.cbSize	= sizeof( nid );		// on PPC'02 this is 152 bytes long
#ifdef _DEBUG
//	DWORD	ullVersion = GetDllVersion( _T("shell32.dll") );
//	TRACE( _T("NillaHedgeApp::InitInstance: ullVersion=%ld, sizeof(nid)=%d\n"),
//			ullVersion, nid.cbSize );
#endif
	nid.uID	   = 100;								// values from 0 to 12 are reserved.
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_ICON_NOTIFY;
	nid.hWnd = pWnd->GetSafeHwnd();					// receives uCallbackMessage = WM_ICON_NOTIFY
	wcscpy( nid.szTip, _T("NillaHedge") );			// balloon help?
		// according to <http://www.codeguru.com/forum/archive/index.php/t-194324.html>
		// the following are needed rather than just LoadIcon(), and he's right
	nid.hIcon	 = (HICON)LoadImage( AfxGetInstanceHandle(),
									MAKEINTRESOURCE(IDI_NillaHedge),
									IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR );
    BOOL	okay = Shell_NotifyIcon( NIM_ADD, &nid );
#ifdef _DEBUG
	if ( ! okay )
		TRACE( _T("NillaHedgeApp::InitInstance: Shell_NotifyIcon(ADD) failed\n") );
#endif

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// do housekeeping needed to open the database
//	regMgr = new CRegistryManager(); 

		// look up the PocketNumerix document directory in the registry
	DWORD			slen = 0;
	wchar_t*		dirName = NULL;
	const wchar_t*	docDirKey = _T("SOFTWARE\\PocketNumerix\\DocumentDirectory");
	success	= ReadRegBinary( HKEY_CURRENT_USER, docDirKey, (BYTE**)&dirName, &slen );			// was:  regMgr->
#ifdef _DEBUG
	TRACE( _T("NillaHedge::InitInstance: DocumentDirectory='%s', slen=%d\n"), dirName, slen );
#endif
	if ( ! success )
	{	delete [] dirName;
		ExitPrep( 0 );
	}
/*
	if ( ! success )
	{
#ifdef _DEBUG
		TRACE( _T("NillaHedge::InitInstance: Couldn't write %s to %s\n"), dirName, docDirKey );
#endif
			// regstered directory should be set up in the installer, so we should never run this else block
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
			TRACE( _T("NillaHedge::InitInstance: Unable to create directory: %s (could already exist)\n"), dirName );
#endif
		}
		success = WriteRegBinary( HKEY_CURRENT_USER, docDirKey, (BYTE*)dirName, (slen+1)*sizeof(wchar_t) );	// was:  regMgr->
	}
*/
		// set up the dbDir instance variable
	dbDir = dirName;
	delete [] dirName;
#ifdef _DEBUG
	TRACE( _T("NillaHedgeApp::InitInstance: dbDir=%s\n"), dbDir );
#endif
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// open the database... NH's data directory is directly beneath dbDir
	optMgr = new COptionManager( _T("\\Options") );
	stkMgr = new CStockManager( _T("\\Stocks") );
	return TRUE;
}			// InitInstance()
//---------------------------------------------------------------------------
void	CNillaHedgeApp::ExitPrep( short code )
{
	bool epilogue = ( code < 0 );
	if ( epilogue )
	{
#ifdef _DEBUG
		if ( code == -1 )
		{	CString sn = GetSerialNumber();
			CString wu = uu;
			TRACE( _T("NillaHedgeApp:InitInstance: license uuid '%s' doesn't match device uuid '%s'\n"), wu, sn );
		}
		else if ( code == -2 )
			TRACE( _T("NillaHedgeApp:InitInstance: license huid doesn't match device huid\n") );
		else if ( code == -3 )
			TRACE( _T("NillaHedgeApp:InitInstance: couldn't get serial number\n") );
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
		wchar_t*	regKey = _T("SOFTWARE\\PocketNumerix\\NillaHedge");
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
int		CNillaHedgeApp::ExitInstance() 
{
	    // Remove the notification icon from the system tray.
	BOOL	okay = Shell_NotifyIcon( NIM_DELETE, &nid );
#ifdef _DEBUG
	if ( ! okay )
		TRACE( _T("NillaHedge::InitInstance: Shell_NotifyIcon(DELETE) failed\n") );
#endif

	BOOL	res = ReleaseMutex( hMutex );
#ifdef _DEBUG
	if ( res != 0 )
		TRACE( _T("NillaHedge::ExitInstance: ReleaseMutex failed\n") );
#endif
//	delete [] dbDir;
	delete	stkMgr;
	delete	optMgr;
//	delete	regMgr;
	return CWinApp::ExitInstance();
}			// ExitInstance()
//---------------------------------------------------------------------------
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
	virtual BOOL OnInitDialog();		// Added for WCE apps
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void	CNillaHedgeApp::OnAppAbout()
{
	CAboutDlg	aboutDlg;
	aboutDlg.DoModal();
}

CString		numberStrings[13] = {	_T("zero"), _T("one"),	_T("two"),	_T("three"), _T("four"),
									_T("five"),	_T("six"),	_T("seven"), _T("eight"), _T("nine"),
									_T("ten"),	_T("eleven"), _T("twelve") };

	/////////////////////////////////////////////////////////////////////////////
// CNillaHedgeApp commands
// Added for WCE apps
BOOL	CAboutDlg::OnInitDialog() 
{
	CNillaDialog::OnInitDialog();
		
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
//---------------------------------------------------------------------------
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
	long	res = ( ii == strlen(sub)  ?  tokStart - origStr  :  -1 );
#ifdef _DEBUG
//	if ( res < 0 )
//		AfxDebugBreak();
#endif
	return	res;
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
//----------------------------------------------------------------------------------------------------
/*
// #include, extern, KernelIoControl declaration, and the #define are
// from:  <http://www.pocketpcdn.com/articles/serial_number2002.html>
#include <WINIOCTL.H>
extern "C" __declspec(dllimport)

BOOL KernelIoControl( DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize,
					  LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned );

#define IOCTL_HAL_GET_DEVICEID	CTL_CODE( FILE_DEVICE_HAL, 21, METHOD_BUFFERED, FILE_ANY_ACCESS )

// the code is from http://64.41.105.202/forum/viewtopic.php?t=138&view=previous&sid=035c3fe1310d5e0f9fe80037765f71eb
			// moved to GetDeviceInfo.*
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
//			The following routines support the bivariate Normal integral
//----------------------------------------------------------------------------------------------------
/*
double fxy( double xx, double yy, double aaPrime, double bbPrime, double rho )
{	double	t1 = aaPrime * (2.0 * xx - aaPrime);
	double	t2 = bbPrime * (2.0 * yy - bbPrime);
	double	t3 = 2.0 * rho * (xx - aaPrime) * (yy - bbPrime);
	return	exp( t1 + t2 + t3 );
}

double	bivarNormIntegral(
	double			aa,
	double			bb,
	double			rho				)
{		// Return M(aa,bb; rho):  the cummulative probability that
		// a first variable is less than aa, and a second variable is less than bb;
		// given that the coefficient of correlation between aa and bb is rho.
		// this the corrected form of Z. Dresner's approximation
		// Refer to p. 266, John Hull's  __Options, Futures, & Other Derivatives__
	double			aaPrime, bbPrime;
	double			M_a_b_rho = 0.0;

		// parameter conditioning ...
	if ( rho >=  1.0 )
		rho =  0.999999;
	if ( rho <= -1.0 )
		rho = -0.999999;
		// Its okay if either aa == 0 or bb == 0, but not both.
		// Can eliminate infinite recursion just by conditioning aa.
    if ( aa == 0.0 )
		aa = -1e-100;		// the sign is very important!

		// the terminal case (for recursion) - all parameters <= 0 ...
	if ( aa <= 0.0  &&  bb <= 0.0  &&  rho <= 0.0 )
	{	const double	AA[4] = { 0.3253030, 0.4211071, 0.1334425, 0.006374323 };
		const double	BB[4] = { 0.1337764, 0.6243247, 1.3425378, 2.2626645   };
		double	sqRt_OneMinusRhoSq = sqrt( 1.0 - rho * rho );
		double	sqRt_2_OneMinusRhoSq = sqrt( 2.0 ) * sqRt_OneMinusRhoSq;
		if ( sqRt_2_OneMinusRhoSq != 0.0 )
		{	aaPrime = aa / sqRt_2_OneMinusRhoSq;
			bbPrime = bb / sqRt_2_OneMinusRhoSq;
		}
		else
		{	aaPrime = aa * 9e300;
			bbPrime = bb * 9e300;
		}
		unsigned short	ii, jj;
		for ( ii = 0; ii < 4; ii++ )
		{	for ( jj = 0; jj < 4; jj++ )
			{		// use the direct method
				M_a_b_rho += AA[ii] * AA[jj]
						   * fxy( BB[ii], BB[jj], aaPrime, bbPrime, rho );
			}
		}
		M_a_b_rho *= sqRt_OneMinusRhoSq / PI;
	}
	else if ( aa * bb * rho <= 0.0 )
	{		// Three cases handled here.  Can use one of the following identities:
			// M(a,b;rho) = N(a) - M(a,-b;-rho)
			// M(a,b;rho) = N(b) - M(-a,b;-rho)
			// M(a,b;rho) = N(a) + N(b) - 1 + M(-a,-b;rho)
			// get best efficiency in one of the first two cases
		if ( aa <= 0.0 )				// using Drezner's (9)
		{	M_a_b_rho = normCDF( aa )
					  - bivarNormIntegral( aa, -bb, -rho );
		}
		else if ( bb <= 0.0 )			// using Drezner's (8)
		{	M_a_b_rho = normCDF( bb )
					  - bivarNormIntegral( -aa, bb, -rho );
		}
		else							// rho <= 0.0, using Drezner's (7)
		{	M_a_b_rho = normCDF( aa ) + normCDF( bb ) - 1.0
					  + bivarNormIntegral( -aa, -bb, rho );
		}
	}
	else								// aa * bb * rho > 0
	{		// four of eight cases are potentially handled here (doubly recursive) ...
			// all parameters are positive or one is positive and the other two are negative
			// use the identity:  M(a,b; rho) = M(a,0; rho1) + M(b,0; rho2) - delta
			// where ...
			// denom = 0 when aa == bb and rho == 1, and when rho = (a^2 + b^2) / 2ab
		double	denom = sqrt( aa*aa - 2.0*rho*aa*bb + bb*bb );
			// prevent divide by zero
		if ( denom == 0.0 )
			denom = 9e-300;
		double	rho1  = (rho*aa - bb) * sign(aa) / denom;
		double	rho2  = (rho*bb - aa) * sign(bb) / denom;
		double	delta = (1.0 - sign(aa)*sign(bb)) / 4.0;
		M_a_b_rho = bivarNormIntegral( aa, 0.0, rho1 )
				  + bivarNormIntegral( bb, 0.0, rho2 )
				  - delta;
	}
	return	M_a_b_rho;
}			// bivarNormIntegral()
*/
//----------------------------------------------------------------------------------------------------
//			The following globals & routines support the trivariate Normal integral
//----------------------------------------------------------------------------------------------------
/*
double			hh[3];
double			rho[3];		// rho12 [1], rho13 [2], rho23 [3]

double	trivarNormIntegral(	double aa,    double bb,    double cc,
						    double rho12, double rho13, double rho23 )
{		// Genz's version
	extern double			hh[3];
	extern double			rho[3];		// r12 [1], r13 [2], r23 [3]
	hh[1] = aa;
	hh[2] = bb;
	hh[3] = cc;
	rho[1] = rho12;
	rho[2] = rho13;
	rho[3] = rho23;

		// Compute trivariate normal probability...
		// Calls:  adonet1, bivarNormIntegral, max, normCDF, tvtmfn1
		// Algorithms developed in these papers:o
		//   R.L. Plackett, Biometrika 41(1954), pp. 351-360.
		//   Z. Drezner, Math. Comp. 62(1994), pp. 289-294.
		//   with adaptive integration from (0,0,1) to (0,0,r23) to R. 
		//
		// Calculate the probability that x(i) < h(i), for i = 1, 2, 3.	 
		//	hh	real 'array' of three upper limits for probability distribution 
		//	rho   real 'array' of three correlation coefficients, r should 
		//		  contain the lower left portion of the correlation matrix R. 
		//		  r should contain the values r21, r31, r23 in that order.
		//	epsi  real required absolute accuracy; maximum accuracy for most
		//		  computations is approximately 1e-14.

	// if we had an adaptive integration routine, we would reinstate the following line
	double	epst = 1.0e-8;		// max( 1.0e-14, epsi );

		// Sort rhos
	if ( fabs(rho12) > fabs(rho13) ) 
	{	swap( hh[2],  hh[3]  );
		swap( rho[1], rho[2] );		// rho12 [1] <--> rho13 [2]
	}
	if ( fabs(rho13) > fabs(rho23) ) 
	{	swap( hh[1],  hh[2]  );
		swap( rho[3], rho[2] );		// rho23 [3] <--> rho13 [2]
	}

		// Check for special cases
	double	tvn = 0.0;						// the function result
	if ( fabs(hh[1]) + fabs(hh[2]) + fabs(hh[3]) < epst )
	{	tvn = ( 1.0 + ( asin(rho[1]) + asin(rho[2]) + asin(rho[3]) ) / (PI/2.0) ) / 8.0;
	}
	else if ( fabs(rho[1]) + fabs(rho[2]) < epst )
	{	tvn = normCDF( hh[1] )
			* bivarNormIntegral( -hh[2], -hh[3], rho[3] );
	}
	else if ( fabs(rho[2]) + fabs(rho[3]) < epst )
	{	tvn = normCDF( hh[3] )
			* bivarNormIntegral( -hh[1], -hh[2], rho[1] );
	}
	else if ( fabs(rho[1]) + fabs(rho[3]) < epst )
	{	tvn = normCDF( hh[2] )
			* bivarNormIntegral( -hh[1], -hh[3], rho[2] );
	}
	else if ( 1.0 - rho[3] < epst )
	{	tvn = bivarNormIntegral( -hh[1], -min(hh[2], hh[3]), rho[1] );
	}
	else if ( rho[3] + 1.0 < epst ) 
	{	if  ( hh[2] > -hh[3] )
		{	tvn = bivarNormIntegral( -hh[1], -hh[2], rho[1] )
				- bivarNormIntegral( -hh[1],  hh[3], rho[1] );
		}
	}
	else
	{		// Compute singular TVNI value
		tvn = normCDF(hh[1])
			* bivarNormIntegral( -hh[2], -hh[3], rho[3] );

			// Use numerical integration to compute probability;
			// We're using a simple quadrature routine.
			// May have to replace it with an adaptive routine later.
		tvn += ShoveltonQuadrature( &pfInt, 0.0, 1.0 ) / (2.0*PI);
	}
	return	max( 0.0, min(tvn, 1.0) ) ;
}			// trivarNormIntegral()
*/
//----------------------------------------------------------------------------------------------------
/*
double	ShoveltonQuadrature( 
	double	(*fcn)(double),
	double	aa,
	double	bb				)
{		// Shovelton's Rule:  5h/126 * (8(f1+f11) + 35(f2+f4+f8+f10) + 15(f3+f5+f7+f9) + 36f6)
	double	hh = (bb - aa) / 10;
	return	5*hh * ( 8*(fcn(aa)      + fcn(bb))									+
				 35*(fcn(aa+  hh) + fcn(aa+3*hh) + fcn(bb-3*hh) + fcn(bb-hh))	+
				 15*(fcn(aa+2*hh) + fcn(aa+4*hh) + fcn(bb-4*hh) + fcn(bb-2*hh))	+
				 36* fcn(aa+5*hh)												  ) / 126;
}
*/
//----------------------------------------------------------------------------------------------------
/*
double	pfInt( double xx )
{		// set up to compute Plackett formula integrands
	double	pfi = 0.0;					// return value
		// globals ...
	extern double	hh[3], rho[3];
	double	rua = asin( rho[1] );
	double	rub = asin( rho[2] );
    double	rr2, r12 = sincos2( rua*xx, rr2 );		// r12 <-- sin(rua*xx), rr2 <-- cos^2(rua*xx)
	double	rr3, r13 = sincos2( rub*xx, rr3 );		// r13 <-- sin(rub*xx), rr3 <-- cos^2(rub*xx)
	if ( fabs(rua) > 0.0 )
		pfi += rua * pntGnd( hh[1], hh[2], hh[3], r13, rho[3], r12, rr2 );
	if ( fabs(rub) > 0.0 )
		pfi += rub * pntGnd( hh[1], hh[3], hh[2], r12, rho[3], r13, rr3 );
	return	pfi;
}				// pfInt()
*/
//----------------------------------------------------------------------------------------------------
/*
double	pntGnd( double ba, double bb, double bc,
			    double ra, double rb, double sr, double rr )
{		// Computes a Plackett formula integrand
	double	pg = 0.0;					// return value
	double	span = ra - rb;
	double	dt = rr*(rr - span*span - 2.0*ra*rb*(1.0 - sr) );
	if ( dt > 0.0 ) 
	{	double	bt = ( bc*rr + ba*(sr*rb - ra) + bb*(sr*ra - rb) ) / sqrt(dt);
		double	tmp = ba - sr*bb; 
		double	ft = tmp*tmp/rr + bb*bb;
		if ( bt > -10.0  &&  ft < 100.0 ) 
		{	pg = exp( -ft / 2.0 );
			if ( bt < 10.0 )
				pg *= normCDF( bt );
		}
	}
	return	pg;
}				// pntGnd()
*/
//----------------------------------------------------------------------------------------------------
/*
double	sincos2( double xx, double& cs2 )
{
	double	tmp = (PI/2.0) - fabs(xx);
	double	ee = tmp*tmp;
	double	sx;						// return value
	if ( ee < 5.0e-5 )
	{		// series approx. for |xx| near PI/2
		sx = ( 1.0 - ee*(1.0 - ee/12.0 ) / 2.0 )*sign( xx );
		cs2 = ee*( 1.0 - ee*(1.0 - 2.0*ee / 15.0 ) / 3.0 );
	}
	else
	{	sx = sin(xx);
		cs2 = 1.0 - sx*sx;
	}
	return	sx;
}				// sincos2()
*/
//----------------------------------------------------------------------------------------------------
/*
short		MakeShortMonYr( CString monYr )
{		// want to be able to handle MMM strings as well as MMMYY strings
	int	len = monYr.GetLength();
	if ( len < 3 )
		return	-1;
#ifdef _DEBUG
	if ( len != 3  || len != 5 )
	{	TRACE( _T("MakeShortMonYr: GetLength(%s) --> %d\n"), monYr, len );
		AfxDebugBreak();
	}
#endif
	CString	moSt = monYr.Mid( 0, 3 );
	int		ii = intMonthFrom3CharStr( moSt );		// Jan is 0-based
	if ( ii < 0 )
	{
#ifdef _DEBUG
		TRACE( _T("MakeShortMonYr: moSt = %s --> %d\n"), monSt, ii );
		AfxDebugBreak();
#endif
		return	ii;
	}
	int	yr;
	if ( len == 5 )
	{	CString	yrSt = monYr.Mid( 3, 2 );		// the YY characters
		yr = _wtoi( yrSt );
	}
	return	(yr << 4) + ii;			// e.g. Dec05 --> 0511
}			// MakeShortMonYr()
*/
//--------------------------------------------------------------------------------------
/*
void	CSquickSort( CString aa[], short ll, short rr )
{		// based on 3-way partitioning quicksort algorithm in
		// http://www.cs.princeton.edu/~rs/talks/QuicksortIsOptimal.pdf
	if ( rr <= ll )
		return;
	short	ii = ll - 1,  jj = rr,  kk;
	short	pp = ll - 1,  qq = rr;
	CString	vv = aa[rr];
	for ( ; ; )
	{	while ( CScompareFunc(aa[++ii], vv) < 0 );			// scan forward
		while ( CScompareFunc(vv, aa[--jj]) < 0 )	if ( jj == ll )
			break;
		if ( ii >= jj )
			break;
		swap( aa[ii], aa[jj] );
		if ( CScompareFunc( aa[ii], vv) == 0 )
		{	pp++;
			swap( aa[pp], aa[ii] );
		}
		if ( CScompareFunc( vv, aa[jj]) == 0 )
		{	qq--;
			swap( aa[jj], aa[qq] );
		}
	}
	swap( aa[ii], aa[rr] );
	jj = ii++ - 1;
	for ( kk = ll; kk < pp; kk++, jj-- ) swap( aa[kk], aa[jj] );
	for ( kk = rr - 1; kk > qq; kk--, ii++ ) swap( aa[ii], aa[kk] );
	quicksort( aa, ll, jj );
	quicksort( aa, ii, rr );
}			// CSquicksort()
//----------------------------------------------------------------------------------------------------
int	CScompareFunc( CString& st1, CString& st2 )
{		// supports sorting CStrings in the ordinary way, but makes a special case of
		// date strings formated as dd-Mmm-yyyy, so that chronological order results
	if (	st1.GetLength() == 10  &&  st1[2] == '-'  &&  st1[6] == '-'
		&&	st2.GetLength() == 10  &&  st2[2] == '-'  &&  st2[6] == '-'  )
	{	CString	mo1 = st1.Mid( 3, 3 );
		CString mo2 = st2.Mid( 3, 3 );
		int	nmo1 = intMonthFrom3CharStr( mo1 );
		int	nmo2 = intMonthFrom3CharStr( mo1 );
		if ( nmo1 >= 0  &&  nmo2 >= 0 )
		{		// that's about all the characterization we can do to
				// ensure that these are date strings formatted 15-Jan-2004
			CString yr1 = st1.Right( 4 );
			CString yr2 = st2.Right( 4 );
				// suppose the year is enough
			if (  yr1 != yr2  )
				return yr1.Compare( yr2 );
				// we need to compare months
			if ( nmo1 != nmo2 )
				return nmo1 - nmo2;
				// its down to the day of the month
			return	st1.Compare( st2 );
		}
	}
		// they're not date strings
	return st1.Compare( st2 );
}			// CompareFunc() CALLBACK
//----------------------------------------------------------------------------------------------------
void	quicksort( CString aa[], short ll, short rr )
{		// based on 3-way partitioning quicksort algorithm in
		// http://www.cs.princeton.edu/~rs/talks/QuicksortIsOptimal.pdf
	if ( rr <= ll )
		return;
	short	ii = ll - 1,  jj = rr,  kk;
	short	pp = ll - 1,  qq = rr;
	CString	vv = aa[rr];
	for ( ; ; )
	{	while ( aa[++ii] < vv );		// scan forward
		while ( vv < aa[--jj] )	if ( jj == ll )
			break;
		if ( ii >= jj )
			break;
		swap( aa[ii], aa[jj] );
		if ( aa[ii] == vv )
		{	pp++;
			swap( aa[pp], aa[ii] );
		}
		if ( vv == aa[jj] )
		{	qq--;
			swap( aa[jj], aa[qq] );
		}
	}
	swap( aa[ii], aa[rr] );
	jj = ii++ - 1;
	for ( kk = ll; kk < pp; kk++, jj-- ) swap( aa[kk], aa[jj] );
	for ( kk = rr - 1; kk > qq; kk--, ii++ ) swap( aa[ii], aa[kk] );
	quicksort( aa, ll, jj );
	quicksort( aa, ii, rr );
}			// quicksort()
//----------------------------------------------------------------------------------------------------
double erf( xx )
{	// use a Chebyshev fitted polynomial to approximate erf()
	// seems to be accurate to about 1.2e-7
	const double	aa1  =  1.26551223;
	const double	aa2  =  1.00002368;
	const double	aa3  =  0.37409196;
	const double	aa4  =  0.09678418;
	const double	aa5  = -0.18628806;
	const double	aa6  =  0.27886807;
	const double	aa7  = -1.13520398;
	const double	aa8  =  1.48851587;
	const double	aa9  = -0.82215223;
	const double	aa10 =  0.17087277;

	tt = 1.0 / (1.0 + 0.5 * fabs(xx));
	yy = 1.0 - tt * exp( -xx*xx - aa1
	   + tt * (aa2 + tt*( aa3 + tt * (aa4 + tt*( aa5
	   + tt * (aa6 + tt*( aa7 + tt * (aa8 + tt*( aa9 + tt*aa10 )))))))));
	return (xx >= 0.0) ? yy : -yy;
}
*/
//----------------------------------------------------------------------------------------------------
/*
double phi( xx )
{		//	(1+erf(xx/sqrt2) / 2
		// not quite as accurate as Wilmott's version (called normCDF above)
	return (1.0 + erf(xx/1.4142135623731)) / 2.0;
}
*/
//----------------------------------------------------------------------------------------------------
/*
void	NillaHedgeApp::NotFileOpenEnabled( bool enabled )
{		// enable (disable) menus other than the File menu, also
		// enable (disable) submenus other than (File/Open and File/DocList)
	CMenu* menu = GetMenu();
	UINT	nItems = menu->GetMenuItemCount();
	for ( int ii = 0; ii < nItems; ii++ )
	{	UINT	itemID = menu->GetMenuItemID( ii );
		if ( itemID == ID_FILE )
		{	CMenu* subMenu = menu->GetSubMenu( itemID );
			UINT	nSubitems = subMenu->GetMenuItemCount();
			for ( int jj = 0; jj < nSubitems; jj++ )
			{	UINT	subitemID = subMenu->GetMenuItemID( jj );
				if ( subitemID != ID_FILE_OPEN  &&  subitemID != IDM_DOCLIST )
					submenu->EnableMenuItem( subitemID, enabled ? MF_ENABLED : MF_GRAYED );
			}
		}
		else	menu->EnableMenuItem( itemID, enabled ? MF_ENABLED : MF_GRAYED );
	}
}			// NotFileOpenEnabled()
*/
