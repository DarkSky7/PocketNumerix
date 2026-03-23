// PNMX_InstallerDlg.cpp : implementation file
//
#include "StdAfx.h"
#include "PNMX_Installer.h"
#include "PNMX_InstallerDlg.h"
#include "StartRapi.h"
#include <stdio.h>
#include <io.h>
#include "HttpGetFile.h"
#include "GetBOM.h"
#include "GetBinary.h"
#include "Upload.h"
#include "cfb.h"
#include "MimeEncodeDecode.h"
#include "crypt.h"

inline void bumpReset( unsigned short& jj, short lim )
{	if ( jj++ >= lim )
	{	TRACE( _T("\n") );
		jj = 0;
	}
}

#pragma warning( disable : 4996 )				// scanf may be unsafe

#ifndef	PNMX_COM
			// SantaFe is the download server
char*	ServerPagePrefix = "/Install_Process";
#else
			// when using non-dedicated IP, pnmx.com used to map to /~pnmxcom on the host file system,
			// when using dedicated IP, there used to be no prefix in front of /Install_Process
			// now on hostmonster optimal1, we're trying /~optimal1
const char*	ServerPagePrefix = "/~optimal1/Install_Process";
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About
CString		months[12] = {	_T("Jan"), _T("Feb"), _T("Mar"), _T("Apr"),
							_T("May"), _T("Jun"), _T("Jul"), _T("Aug"),
							_T("Sep"), _T("Oct"), _T("Nov"), _T("Dec") };

extern CPNMX_InstallerApp theApp;

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog( CAboutDlg::IDD )
{
}
//--------------------------------------------------------------------------------------------
void CAboutDlg::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CPNMX_InstallerDlg dialog
//--------------------------------------------------------------------------------------------
CPNMX_InstallerDlg::CPNMX_InstallerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPNMX_InstallerDlg::IDD, pParent)
	, m_FirstName( _T("") )						// Given Name caption in the dialog
	, m_LastName( _T("") )						// Family Name caption in the dialog
	, m_EmailAddress( _T("") )
	, m_InvoiceNumber( _T("") )
	, m_InvoiceDate( COleDateTime::GetCurrentTime() )
	, m_BondManager( FALSE )
	, m_NillaHedge( FALSE )
	, m_StockManager( FALSE )
	, m_StrategyExplorer( FALSE )
	, m_YieldCurveFitter( FALSE )
	, m_OptionChains( FALSE )
	, m_TBillRates( FALSE )
	, m_CovarAcct( FALSE )
	, m_DeviceConnected( FALSE )
	, m_BOM_Found( FALSE )
	, m_ButtonState( 0 )					// invoice data enables FetchButton
	, m_InvoiceFields( 0 )					// ditto (why both?)
	, m_MemoryRequired( 0 )
	, m_BMmemReq( 0 )
	, m_MFCmemReq( 0 )
	, m_NHmemReq( 0 )
	, m_SMmemReq( 0 )
	, m_SXmemReq( 0 )
	, m_YCFmemReq( 0 )
	, m_MemoryAvail( 0 )
	, m_DeskTempPath( 0 )
	, m_ProductsMask( 0 )
	, m_BondManagerInstalled( false )
	, m_DLLsInstalled( false )
	, m_NillaHedgeInstalled( false )
	, m_StockManagerInstalled( false )
	, m_StrategyExplorerInstalled( false )
	, m_YieldCurveFitterInstalled( false )
	, m_OptionChainsInstalled( false )
	, m_TBillRatesInstalled( false )
	, m_CovarAcctInstalled( false )
	, m_NumProdsRequiringMFClibs( 0 )
	, m_UUID( NULL )
	, m_HUID_BM( NULL )
	, m_HUID_NH( NULL )
	, m_HUID_SM( NULL )
	, m_HUID_SX( NULL )
	, m_HUID_YCF( NULL )
	, m_BMckiv( NULL )
	, m_NHckiv( NULL )
	, m_SMckiv( NULL )
	, m_SXckiv( NULL )
	, m_YCFckiv( NULL )
	, m_MFCckiv( NULL )
	, m_PPCckiv( NULL )
	, m_OCRckiv( NULL )
	, m_TBRckiv( NULL )
	, m_VARckiv( NULL )
	, lpFileName( NULL )
	, urlRequest( NULL )
	, lpServerName( NULL )
	, fetchInProgress( false )
	, m_BondManagerLicensed( false )
	, m_NillaHedgeLicensed( false )
	, m_StockManagerLicensed(false)
	, m_StrategyExplorerLicensed( false )
	, m_YieldCurveFitterLicensed( false )
{
		// MAINFRAME icon
	m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
	m_hSmallIcon = (HICON)::LoadImage(AfxGetResourceHandle(), 
		MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 16, 16, 0);

//	DWORD res = CeGetSpecialFolderPath( CSIDL_PROGRAMS, MAX_PATH, (wchar_t*)&m_progFilesDir );
	wcsncpy( m_progFilesDir, _T("\\Program Files\\PocketNumerix"), MAX_PATH );
}			// CPNMX_InstallerDlg()
//--------------------------------------------------------------------------------------------
CPNMX_InstallerDlg::~CPNMX_InstallerDlg( void )
{
	if ( m_DeviceConnected )
		CeRapiUninit();

	if ( lpServerName ) delete [] lpServerName;
	if ( urlRequest ) delete [] urlRequest;
	if ( lpFileName ) delete [] lpFileName;
	if ( m_UUID ) delete [] m_UUID;
	if ( m_HUID_BM ) delete [] m_HUID_BM;
	if ( m_HUID_NH ) delete [] m_HUID_NH;
	if ( m_HUID_SM ) delete [] m_HUID_SM;
	if ( m_HUID_SX ) delete [] m_HUID_SX;
	if ( m_HUID_YCF ) delete [] m_HUID_YCF;
	if ( m_DeskTempPath ) delete [] m_DeskTempPath;
	if ( m_BMckiv ) delete [] m_BMckiv;
	if ( m_NHckiv ) delete [] m_NHckiv;
	if ( m_SMckiv ) delete [] m_SMckiv;
	if ( m_SXckiv ) delete [] m_SXckiv;
	if ( m_YCFckiv ) delete [] m_YCFckiv;
	if ( m_MFCckiv ) delete [] m_MFCckiv;
	if ( m_PPCckiv ) delete [] m_PPCckiv;
	if ( m_OCRckiv ) delete [] m_OCRckiv;
	if ( m_TBRckiv ) delete [] m_TBRckiv;
	if ( m_VARckiv ) delete [] m_VARckiv;
}			// ~CPNMX_InstallerDlg()
//--------------------------------------------------------------------------------------------
void CPNMX_InstallerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EmailAddress, c_EmailAddress);
	DDX_Text(pDX, IDC_EmailAddress, m_EmailAddress);
	DDV_MaxChars(pDX, m_EmailAddress, 128);

	DDX_Control(pDX, IDC_InvoiceNumber, c_InvoiceNumber);
	DDX_Text(pDX, IDC_InvoiceNumber, m_InvoiceNumber);
	DDV_MaxChars(pDX, m_InvoiceNumber, 32);

	DDX_Control(pDX, IDC_InvoiceDate, c_InvoiceDate);
	DDX_DateTimeCtrl(pDX, IDC_InvoiceDate, m_InvoiceDate);

	DDX_Control(pDX, IDC_FirstName, c_FirstName);
	DDX_Text(pDX, IDC_FirstName, m_FirstName);
	DDV_MaxChars(pDX, m_FirstName, 128);

	DDX_Control(pDX, IDC_LastName, c_LastName);
	DDX_Text(pDX, IDC_LastName, m_LastName);
	DDV_MaxChars(pDX, m_LastName, 128);

	DDX_Control(pDX, IDFETCH, c_FetchButton);
	DDX_Control(pDX, IDC_DeviceIdResult, c_DeviceIdResult);
	DDX_Control(pDX, IDC_OsVersionResult, c_OsVersionResult);
	DDX_Control(pDX, IDC_StatusResult, c_StatusResult);
	DDX_Control(pDX, IDC_PlatformIDResult, c_PlatformIdResult);
	DDX_Control(pDX, IDC_MemoryRequiredResult, c_MemoryRequiredResult);
	DDX_Control(pDX, IDC_MemoryAvailResult, c_MemoryAvailResult);

	DDX_Control(pDX, IDC_Device_Connected, c_DeviceConnected);
	DDX_Check(pDX, IDC_Device_Connected, m_DeviceConnected);
	DDX_Control(pDX, IDC_Device_Connected_Label, c_DeviceConnectedLabel);

	DDX_Control(pDX, IDC_BILL_OF_MATERIALS_FOUND, c_BOM_Found);
	DDX_Check(pDX, IDC_BILL_OF_MATERIALS_FOUND, m_BOM_Found);
	DDX_Control(pDX, IDC_BILL_OF_MATERIALS_FOUND_LABEL, c_BOM_Label);

	DDX_Text(pDX, IDC_MemoryRequiredResult, m_MemoryRequired);
	DDX_Text(pDX, IDC_MemoryAvailResult, m_MemoryAvail);

	DDX_Control(pDX, IDC_BondManager, c_BondManager);
	DDX_Check(pDX, IDC_BondManager, m_BondManager);

	DDX_Control(pDX, IDC_NillaHedge, c_NillaHedge);
	DDX_Check(pDX, IDC_NillaHedge, m_NillaHedge);

	DDX_Control(pDX, IDC_StockManager, c_StockManager);
	DDX_Check(pDX, IDC_StockManager, m_StockManager);

	DDX_Control(pDX, IDC_StrategyExplorer, c_StrategyExplorer);
	DDX_Check(pDX, IDC_StrategyExplorer, m_StrategyExplorer);

	DDX_Control(pDX, IDC_YieldCurveFitter, c_YieldCurveFitter);
	DDX_Check(pDX, IDC_YieldCurveFitter, m_YieldCurveFitter);

	DDX_Control(pDX, IDC_OptionChains, c_OptionChains);
	DDX_Check(pDX, IDC_OptionChains, m_OptionChains);

	DDX_Control(pDX, IDC_TBillRates, c_TBillRates);
	DDX_Check(pDX, IDC_TBillRates, m_TBillRates);

	DDX_Control(pDX, IDC_CovarAcct, c_CovarAcct);
	DDX_Check(pDX, IDC_CovarAcct, m_CovarAcct);

	DDX_Control(pDX, IDC_UUIDResult, c_UUIDresult);
	//	DDX_Text(pDX, IDC_UUIDResult, m_UUIDresult);

	DDX_Control(pDX, IDC_Ppc_Downloaded, c_Ppc_Downloaded);
	DDX_Control(pDX, IDC_MFC_Downloaded, c_MFC_Downloaded);
	DDX_Control(pDX, IDC_BM_Downloaded,  c_BM_Downloaded);
	DDX_Control(pDX, IDC_NH_Downloaded,  c_NH_Downloaded);
	DDX_Control(pDX, IDC_SM_Downloaded, c_SM_Downloaded);
	DDX_Control(pDX, IDC_SX_Downloaded,  c_SX_Downloaded);
	DDX_Control(pDX, IDC_YCF_Downloaded, c_YCF_Downloaded);
	DDX_Control(pDX, IDC_OCR_Downloaded, c_OCR_Downloaded);
	DDX_Control(pDX, IDC_TBR_Downloaded, c_TBR_Downloaded);
	DDX_Control(pDX, IDC_VAR_Downloaded, c_VAR_Downloaded);			// goes with CovarAcct

	DDX_Control(pDX, IDC_Ppc_Uploaded, c_Ppc_Uploaded);
	DDX_Control(pDX, IDC_MFC_Uploaded, c_MFC_Uploaded);
	DDX_Control(pDX, IDC_BM_Uploaded,  c_BM_Uploaded);
	DDX_Control(pDX, IDC_NH_Uploaded,  c_NH_Uploaded);
	DDX_Control(pDX, IDC_SM_Uploaded, c_SM_Uploaded);
	DDX_Control(pDX, IDC_SX_Uploaded,  c_SX_Uploaded);
	DDX_Control(pDX, IDC_YCF_Uploaded, c_YCF_Uploaded);

	DDX_Control(pDX, IDC_Ppc_Installed, c_Ppc_Installed);
	DDX_Control(pDX, IDC_MFC_Installed, c_MFC_Installed);
	DDX_Control(pDX, IDC_BM_Installed,  c_BM_Installed);
	DDX_Control(pDX, IDC_NH_Installed,  c_NH_Installed);
	DDX_Control(pDX, IDC_SM_Installed, c_SM_Installed);
	DDX_Control(pDX, IDC_SX_Installed,  c_SX_Installed);
	DDX_Control(pDX, IDC_YCF_Installed, c_YCF_Installed);
	DDX_Control(pDX, IDC_OCR_Installed, c_OCR_Installed);
	DDX_Control(pDX, IDC_TBR_Installed, c_TBR_Installed);
	DDX_Control(pDX, IDC_VAR_Installed, c_VAR_Installed);			// goes with CovarAcct

	DDX_Control(pDX, IDC_DownloadStatusResult, c_DownloadStatusResult);
	DDX_Control(pDX, IDC_UploadStatusResult, c_UploadStatusResult);
	DDX_Control(pDX, IDC_InstallStatusResult, c_InstallStatusResult);

	DDX_Control(pDX, IDI_APP_IMAGE, c_AppImage);
	DDX_Control(pDX, IDC_Binaries_Installed, c_BinariesInstalled);
	DDX_Control(pDX, IDC_Binaries_Installed_Label, c_BinariesInstalledLabel);
	DDX_Control(pDX, IDC_Subscriptions_Activated, c_SubscriptionsActivated);
	DDX_Control(pDX, IDC_Subscriptions_Activated_Label, c_SubscriptionsActivatedLabel);

	DDX_Control(pDX, IDC_Licenses_Installed_Label, c_LicensesInstalledLabel);
	DDX_Control(pDX, IDC_Licenses_Installed, c_LicensesInstalled);

	DDX_Control(pDX, IDC_BM_Licensed, c_BM_Licensed);
	DDX_Control(pDX, IDC_NH_Licensed, c_NH_Licensed);
	DDX_Control(pDX, IDC_SM_Licensed, c_SM_Licensed);
	DDX_Control(pDX, IDC_SX_Licensed, c_SX_Licensed);
	DDX_Control(pDX, IDC_YCF_Licensed, c_YCF_Licensed);
}
//--------------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CPNMX_InstallerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDFETCH, &CPNMX_InstallerDlg::OnBnClickedFetch)
	ON_REGISTERED_MESSAGE( WM_PNMX_INSTALLER_ALREADY_RUNNING, OnPNMX_InstallerAlreadyRunning)
	ON_REGISTERED_MESSAGE( WM_PNMX_INSTALLER_DEVICE_CONNECTED, OnPNMX_InstallerDeviceConnected)
//	ON_EN_CHANGE(IDC_FirstName, &CPNMX_InstallerDlg::OnEnChangeFirstName)
//	ON_EN_CHANGE(IDC_LastName, &CPNMX_InstallerDlg::OnEnChangeLastName)
//	ON_EN_CHANGE(IDC_EmailAddress, &CPNMX_InstallerDlg::OnEnChangeEmailAddress)
//	ON_EN_CHANGE(IDC_InvoiceNumber, &CPNMX_InstallerDlg::OnEnChangeInvoicNnumber)
	ON_BN_CLICKED(IDC_BondManager, &CPNMX_InstallerDlg::OnBnClickedBondManager)
	ON_BN_CLICKED(IDC_NillaHedge, &CPNMX_InstallerDlg::OnBnClickedNillaHedge)
	ON_BN_CLICKED(IDC_StockManager, &CPNMX_InstallerDlg::OnBnClickedStockManager)
	ON_BN_CLICKED(IDC_OptionChains, &CPNMX_InstallerDlg::OnBnClickedOptionChains)
	ON_BN_CLICKED(IDC_StrategyExplorer, &CPNMX_InstallerDlg::OnBnClickedStrategyExplorer)
	ON_BN_CLICKED(IDC_TBillRates, &CPNMX_InstallerDlg::OnBnClickedTBillRates)
	ON_BN_CLICKED(IDC_YieldCurveFitter, &CPNMX_InstallerDlg::OnBnClickedYieldCurveFitter)
END_MESSAGE_MAP()
//--------------------------------------------------------------------------------------------
// CPNMX_InstallerDlg message handlers
//--------------------------------------------------------------------------------------------
BOOL CPNMX_InstallerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu( FALSE );
	if ( pSysMenu != NULL )
	{
		CString strAboutMenu;
		strAboutMenu.LoadString( IDS_ABOUTBOX );
		if ( ! strAboutMenu.IsEmpty() )
		{
			pSysMenu->AppendMenu( MF_SEPARATOR );
			pSysMenu->AppendMenu( MF_STRING, IDM_ABOUTBOX, strAboutMenu );
		}
	}
		// Set the icon for this dialog.  The framework does this automatically
		// when the application's main window is not a dialog
	SetIcon( m_hIcon, TRUE );						// Set big icon
	SetIcon( m_hSmallIcon, FALSE );					// Set small icon

	c_BondManager.SetCheck( FALSE );
	c_BondManager.EnableWindow( FALSE );

	c_NillaHedge.SetCheck( FALSE );
	c_NillaHedge.EnableWindow( FALSE );

	c_StockManager.SetCheck( FALSE );
	c_StockManager.EnableWindow( FALSE );

	c_OptionChains.SetCheck( FALSE );
	c_OptionChains.EnableWindow( FALSE );

	c_StrategyExplorer.SetCheck( FALSE );
	c_StrategyExplorer.EnableWindow( FALSE );

	c_YieldCurveFitter.SetCheck( FALSE );
	c_YieldCurveFitter.EnableWindow( FALSE );

	c_TBillRates.SetCheck( FALSE );
	c_TBillRates.EnableWindow( FALSE );

	c_CovarAcct.SetCheck( FALSE );
	c_CovarAcct.EnableWindow( FALSE );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	CString cs1;
	CString cs2 = _T("");									// sometimes you just need a CString value
	const unsigned short WBUF_SIZE = 256;
	short	retVal = 0;
	c_StatusResult.SetWindowTextW( _T(" No Device Connected") );
	wchar_t	temp[ WBUF_SIZE ];
	DWORD len = GetTempPath( WBUF_SIZE, temp );				// temp = file system value for %temp% 
	if ( len == 0 )
	{		// GetTempPath failed
		wchar_t wbuf[ WBUF_SIZE ];
		swprintf( wbuf, WBUF_SIZE, _T(" Unable to get the %temp% directory") );
		c_StatusResult.SetWindowTextW( wbuf );
#ifdef _DEBUG
		TRACE( _T("PNMX_InstallerDlg::OnInitDialog: GetTempPath failed\n"), wbuf );
#endif
		retVal = -1;
		goto	Exit;
	}

/*		// this block succeeds, but is unnecessary
		// GetTempPath() succeeded
	BOOL B1 = SetCurrentDirectory( temp );					// cd %temp%
	if ( B1 == 0 )
	{		// SetCurrentDirectory(%temp%) failed
		wchar_t wbuf[WBUF_SIZE];
		swprintf_s( wbuf, WBUF_SIZE, _T(" Unable to set diretory to %s"), temp );
		c_StatusResult.SetWindowTextW( wbuf );
#ifdef _DEBUG
		TRACE( _T("PNMX_InstallerDlg::OnInitDialog: SetCurrentDirectory(%s) failed\n"), temp );
#endif
		retVal = -2;
		goto	Exit;
	}
		// SetCurrentDirectory(%temp%) succeeded
*/	
		// Create the m_DeskTempPath value
	wchar_t wbuf2[ WBUF_SIZE ];
	swprintf_s( wbuf2, WBUF_SIZE, _T("%s\\PocketNumerix"), temp );		// should this be a \\ ?
	size_t	slen = wcslen( wbuf2 ) + 1;
	m_DeskTempPath = new wchar_t[ slen ];
	if ( m_DeskTempPath == NULL )
	{		// heap memory allocation failed
		wchar_t wbuf[WBUF_SIZE];
		swprintf_s( wbuf, WBUF_SIZE, _T(" Unable to allocate memory for %s"), wbuf2 );
		c_StatusResult.SetWindowTextW( wbuf );
#ifdef _DEBUG
		TRACE( _T("PNMX_InstallerDlg::OnInitDialog: Unable to allocate memory for %s\n"), wbuf2 );
#endif
		retVal = -3;
		goto	Exit;
	}
	wcsncpy_s( m_DeskTempPath, slen, wbuf2, slen );		// m_DeskTempPath = %temp%\PocketNumerix

		// search temp for _T("PocketNumerix"), the m_DeskTempPath directory
	WIN32_FIND_DATA findFileData;
//	cs1 = _T("C:\\Users\\Mike\\AppData\\Local\\Temp");	// this works, but single backslashes don't
	cs1 = m_DeskTempPath;
	int ii = 0;
	while ( 1 )
	{	int jj = cs1.Find( _T("\\"), ii );
		if ( jj >= 0 )
		{	cs2 = cs2 + cs1.Mid( ii, jj-ii );
			cs2 += _T("\\\\");
			ii = jj + 1;
		}
		else
		{	cs2 = cs2 + cs1.Mid( ii );
			break;
		}
	} 
	HANDLE hSearch = FindFirstFile( cs2, &findFileData );
	if ( hSearch == INVALID_HANDLE_VALUE )
	{	DWORD msgID = GetLastError();
		wchar_t	MsgBuf[WBUF_SIZE];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			0, msgID, 0, (LPTSTR)&MsgBuf, WBUF_SIZE, NULL );
		wchar_t wbuf[WBUF_SIZE];
		swprintf_s( wbuf, WBUF_SIZE, _T(" FindFile(%s) -> %s"), cs2, MsgBuf );
		c_StatusResult.SetWindowTextW( wbuf );
#ifdef _DEBUG
		TRACE( _T("PNMX_InstallerDlg::OnInitDialog: FindFirstFile(%s) -> %s\n"), cs2, MsgBuf );
#endif
			// create the m_DeskTempPath directory
		BOOL B1 = CreateDirectoryEx( temp, cs2, NULL );
		if ( B1 == 0 )
		{	DWORD msgID = GetLastError();
			FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				0, msgID, 0, (LPTSTR)&MsgBuf, WBUF_SIZE, NULL );
			swprintf_s( wbuf, WBUF_SIZE, _T(" CreateDirectory(%s) -> %s"), cs2, MsgBuf );
			c_StatusResult.SetWindowTextW( wbuf );
#ifdef _DEBUG
			TRACE( _T("PNMX_InstallerDlg::OnInitDialog CreateDirectory(%s) -> %s"), cs2, MsgBuf );
#endif
			retVal = -4;
			goto	Exit;
		}
	}
		// set up server name
	m_InvoiceDate = COleDateTime::GetCurrentTime();
	lpServerName = new char[ 16 ];
#ifdef PNMX_COM
	strncpy_s( lpServerName, 16, "pnmx.com", 12 );
#else
	strncpy_s( lpServerName, 16, "192.168.2.51", 12 );
#endif
		// set up RAPI
	int	sz = sizeof( rapiInit );
	ZeroMemory( &rapiInit, sz );
	rapiInit.cbSize = sz;					// rapiInit is a dialog variable
	HRESULT hr = CeRapiInitEx( &rapiInit );
		
		// start another thread to set up the device connected thread
	if ( SUCCEEDED(hr) )
	{	DWORD	dwThreadId, dwExitCode = 0;
		m_ThreadHandle = CreateThread( NULL, 0, InitDeviceConnectedMsg, (void*)this, 0, &dwThreadId );
	}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	GetInvoice();	// fill in values for firstMame, lastName, etc.
/*
	{		// communicate desk temp directory create failure mechanism
		CString cs = _T("");
		switch ( res )
		{	case ERROR_BAD_PATHNAME:
				cs = _T("BAD_PATHNAME");
				break;
			case ERROR_FILENAME_EXCED_RANGE:
				cs = _T("FILENAME_EXCED_RANGE");
				break;
			case ERROR_PATH_NOT_FOUND:
				cs = _T("PATH_NOT_FOUND");
				break;
			case ERROR_FILE_EXISTS:
				cs = _T("FILE_EXISTS");
				break;
			case ERROR_CANCELLED:
				cs = _T("CANCELLED");
				break;
			case ERROR_NOACCESS:
				cs = _T("NOACCESS");
				break;
			default:
				cs = _T("UNKNOWN");
				break;
		}
		wchar_t wbuf[256];
		swprintf_s( wbuf, 256, _T(" CreateDirectory(%s) -> %s"), m_DeskTempPath, cs );
		c_StatusResult.SetWindowTextW( wbuf );
#ifdef _DEBUG
		TRACE( _T("PNMX_InstallerDlg::OnInitDialog:%s(%d)\n"), wbuf, res );
#endif
	}
*/
Exit:
	return TRUE;	// return TRUE  unless you set the focus to a control
}			// InitDialog()
//--------------------------------------------------------------------------------------------
#define DEBUG_TOKEN		1
void	CPNMX_InstallerDlg::GetInvoice( void )
{	FILE*	fp = NULL;
	errno_t	res = _wfopen_s( &fp, _T("PNMX_InstallToken"), _T("rb") );
	if ( res != 0 )
		return;
	size_t	bytesRead;
	char*	buf = NULL;
	unsigned char*	cryptBuf = NULL;

	long flen = _filelength( _fileno( fp ) );		// replaces the following three lines
	if ( 0 < flen  &&  flen <= 1024 )
	{		// expecting a Rijndael-256 encrypted file with the crypt key in the first 32 bytes
#if DECRYPT_BOM										// PNMX_InstallToken.dat and PNMX_BOM.dat are encrypted
/*			// cipher key is the first 32 bytes of the invoice header token
		bytesRead = fread( ck, sizeof(char), AES_BLOCK_SIZE, fp );
		if ( bytesRead != AES_BLOCK_SIZE )
			goto	Exit;
		flen -= AES_BLOCK_SIZE;						// we're only interested in the payload
*/
		cryptBuf = new unsigned char[ flen ];
		if ( cryptBuf == NULL )
			goto	Exit;
		bytesRead = fread( cryptBuf, sizeof(char), flen, fp );
		if ( bytesRead != flen )
			goto	Exit;
#if DEBUG_TOKEN
		{	unsigned short	jj = 0, ii = 0;
			TRACE( _T("GetInvoice(cryptBuf):\n") );
			while ( ii < bytesRead )
			{	bumpReset( jj, AES_BLOCK_SIZE );
				TRACE( _T(" %02x"), *(cryptBuf+ii++) );
			}
			TRACE( _T("\n") );
		}
#endif
		buf = new char[ flen + 1 ];					// the plain text version needs a '\0'
		if ( buf == NULL )
			goto	Exit;

			// the invoice header...

			// rjDecrypt( unsigned char *iv, unsigned char *ibuf, unsigned long iBytes,
			//				unsigned char *obuf, const unsigned long *keySchedule );
			// the first nColumns * nRows bytes is the initialization vector
		const unsigned long tokenCk[8] = { 0xCE36E79E, 0x4EC1B163, 0x52184885, 0x176CFC7B,
										   0x885DCBFB, 0x52390815, 0x78E9E224, 0x5A031076  };
		unsigned long rjrk[ nRjRoundKeys ];									// 120 round keys for Rijndael256
		rjKeyExpansion( (unsigned char*)tokenCk, (unsigned int*)rjrk );		// create the RoundKeys

		unsigned long tokenIv[8] = { 0x151F5CB2, 0xCFDDC08B, 0x668CAE9F, 0x40C68EA5,
									 0x16396394, 0xE6CDFC12, 0xF0A82BDB, 0x21DBAA5E  };
		rjDecrypt( (unsigned char*)tokenIv, cryptBuf, flen, (unsigned char*)buf, (unsigned int *)rjrk );
		*(buf + flen) = '\0';
#else												// no decryption required
		buf = new char[ flen + 1 ];					// the plain text version needs a '\0'
		if ( buf == NULL )
			goto	Exit;
		bytesRead = fread( buf, sizeof(char), flen, fp );
		if ( bytesRead != flen )
			goto	Exit;
		*(buf + flen) = '\0';
#endif

			// from here on, buf should look something like:
			// firstName lastName <user@domain.com> invoiceNumber dd-mmm-yyyy
		int bb = 0;
		char* token = GetToken( buf, bb );				// the first name
		if ( token )
		{	m_FirstName = token;
			c_FirstName.SetWindowTextW( m_FirstName );
			EnableFetchButton( m_FirstName, 0 );
			delete [] token;
			token = GetToken( buf, bb );
		}
		if ( token )									// the last name
		{	m_LastName = token;
			c_LastName.SetWindowTextW( m_LastName );
			EnableFetchButton( m_LastName, 1 );
			delete [] token;
			token = GetToken( buf, bb );
		}
		if ( token )									// the email address
		{	short	ii = ((*token == '<')  ?  1  :  0);
			size_t	toklen = strlen( (char*)token );
			if ( *(token+toklen-1) == '>' )
				*(token+toklen-1) = '\0';				// chop the '>' off
			m_EmailAddress = token + ii;				// skip the '<'
			c_EmailAddress.SetWindowTextW( m_EmailAddress );
			EnableFetchButton( m_EmailAddress, 2 );
			delete [] token;
			token = GetToken( buf, bb );
		}
		if ( token )									// the invoice nunber
		{	m_InvoiceNumber = token;
			c_InvoiceNumber.SetWindowTextW( m_InvoiceNumber );
			EnableFetchButton( m_InvoiceNumber, 3 );
			delete [] token;
			token = GetToken( buf, bb );
		}
		if ( token )									// invoice date
		{	int day=0, year=0;
			char  month[4];
			int items = sscanf( (char*)token, "%d-%3s-%d", &year, &month, &day );
			CString	mon( (char*)month );
			for ( short ii = 0; ii < 12; ii++ )
			{	if ( mon.CompareNoCase( months[ii] ) == 0 )
				{	m_InvoiceDate = COleDateTime( year, ii+1, day, 0, 0, 0 );
					c_InvoiceDate.SetTime( m_InvoiceDate );
					break;
				}
			}
		}
		if ( token )
			delete [] token;
	}
Exit:
	if ( buf ) delete [] buf;
	if ( cryptBuf ) delete [] cryptBuf;
	if ( fp ) fclose( fp );
	return;					// without reading anything
}			// GetInvoice()
//--------------------------------------------------------------------------------------------
void	CPNMX_InstallerDlg::OnBnClickedFetch( void )
{
		// buttonstate is a mask consisting of flags in the following positions%
		// bit 0 - osVer
		// bit 1 - devID
	CString 	btnTxt;
	c_FetchButton.GetWindowTextW( btnTxt );
	if ( 0 == btnTxt.Compare( _T("Fetch BOM") ) )
		FetchBOM();
	else if ( 0 == btnTxt.Compare( _T("Install") ) )
		Install();
}			// OnBnClickedFetch()
//--------------------------------------------------------------------------------------------
short	CPNMX_InstallerDlg::Install( void )
{
		// download and install everything the user still has enabled
	fetchInProgress = true;
	c_FetchButton.EnableWindow( FALSE );
	DWORD	dwThreadId, dwExitCode = 0;
	m_ThreadHandle = CreateThread( NULL, 0, GetBinary, (void*)this, 0, &dwThreadId );
	return	0;
}			// Install()
//--------------------------------------------------------------------------------------------
void	CPNMX_InstallerDlg::FetchBOM( void )
{	DownloadCtx_T*	dwnLdCtx = new DownloadCtx_T;
	dwnLdCtx->p_PID = this;
	short	retVal = 0;

	fetchInProgress = true;
	c_FetchButton.EnableWindow( FALSE );

		// redundant on the first FetchBOM pass, but necessary if retries are allowed
	c_DownloadStatusResult.SetWindowTextW( _T("") );
	c_UploadStatusResult.SetWindowTextW( _T("") );
	c_InstallStatusResult.SetWindowTextW( _T("") );

	c_Ppc_Downloaded.SetCheck( BST_UNCHECKED );
	c_Ppc_Downloaded.ShowWindow( SW_HIDE );

	c_Ppc_Uploaded.SetCheck( BST_UNCHECKED );
	c_Ppc_Uploaded.ShowWindow( SW_HIDE );

	c_Ppc_Installed.SetCheck( BST_UNCHECKED );
	c_Ppc_Installed.ShowWindow( SW_HIDE );

		// all passes
	int		day = m_InvoiceDate.GetDay();
	int		mon = m_InvoiceDate.GetMonth();
	int		yr  = m_InvoiceDate.GetYear();
		// ServerPagePrefix
	char*	thinOsVer = NULL;
	char*	thinFirstName = NULL;
	char*	thinLastName = NULL;
	char*	thinEmail = NULL;
	char*	thinInvoice = NULL;

	thinOsVer = GetThinString( m_OsVersion );
	if ( thinOsVer == NULL )
	{	c_StatusResult.SetWindowTextW( _T(" FetchBOM: thinOsVer mem request failed") );
		goto	Exit;
	}

	thinFirstName = GetThinString( m_FirstName );
	if ( thinFirstName == NULL )
	{	c_StatusResult.SetWindowTextW( _T(" FetchBOM: thinFirstName mem request failed") );
		goto	Exit;
	}

	thinLastName = GetThinString( m_LastName );
	if ( thinLastName == NULL )
	{	c_StatusResult.SetWindowTextW( _T(" FetchBOM: thinLastName mem request failed") );
		goto	Exit;
	}

	thinEmail = GetThinString( m_EmailAddress );
	if ( thinEmail == NULL )
	{	c_StatusResult.SetWindowTextW( _T(" FetchBOM: thinEmail mem request failed") );
		goto	Exit;
	}

	thinInvoice = GetThinString( m_InvoiceNumber );
	if ( thinInvoice == NULL )
	{	c_StatusResult.SetWindowTextW( _T(" FetchBOM: thinInvoice mem request failed") );
		goto	Exit;
	}

		// when we we're sending customer/device info in the clear
//	sprintf_s( dwnLdCtx->urlRequest, DwnldCtxUrlRequestSize,
//			   "%s%s?dev=%d?first=%s?last=%s?email=%s?num=%s?date=%4d-%02d-%02d",
//			leader, thinOsVer, m_DeviceID, thinFirstName, thinLastName,
//			thinEmail, thinInvoice, yr, mon, day );

		// mySQL months go from 1 to 12
	char ibuf[ DwnldCtxUrlRequestSize ];
	sprintf_s( ibuf, DwnldCtxUrlRequestSize, "?os=%s?dv=%X?em=%s?in=%s?dt=%4d-%02d-%02d?ln=%s?fn=%s",
				thinOsVer, m_DeviceID, thinEmail, thinInvoice, yr, mon, day, thinLastName, thinFirstName );
	unsigned long slen = (unsigned long)strlen( ibuf );
	const unsigned int BOMdsCk[8] = { 0xBB37AE71, 0x35AF1FE0, 0x72BC8042, 0x7C02EEDD,
									  0x80C97380, 0x24CC7ABE, 0x4B7A0BE6, 0x0D5B8391  };
	unsigned int rcrk[ nRcRoundKeys ];						// 44 round keys in RC6
	rcKeyExpansion( (unsigned int *)BOMdsCk, rcrk );		// the second time we've generated tokenCk specific RoundKeys (oh well)

	unsigned int BOMdsIv[8]	= { 0x16D3760A, 0xC0DC3BBB, 0xA8C0716B, 0x6CBD4B86,
								0xA12E9274, 0xFC71123F, 0x14B48695, 0x0CCEC600  };
	unsigned char obuf[ DwnldCtxUrlRequestSize ];
	rcCrypt( (unsigned char*)BOMdsIv, (unsigned char*)ibuf, slen, obuf, rcrk );
	*(obuf+slen) = '\0';									// righteous except URLs can't include binary data

		// need to MIME encode obuf
	unsigned char mimeBuf[ DwnldCtxUrlRequestSize * 4/3 ];				// 768 * 4/3 ==> 1024
	unsigned long mimeBufLen = MimeEncode( obuf, slen, mimeBuf );
	sprintf_s( dwnLdCtx->urlRequest, DwnldCtxUrlRequestSize, "%s/BillOfMaterials.php?ds=%s", ServerPagePrefix, mimeBuf );

		// where we're putting the downloaded file
	swprintf_s( dwnLdCtx->fileName, DwnldCtxFileNameSize, _T("%s\\PNMX_BOM.dat"), m_DeskTempPath );

		// get the BOM
	DWORD	dwThreadId, dwExitCode = 0;
	dwnLdCtx->c_StatusResult = &c_StatusResult;
	wcsncpy_s( dwnLdCtx->progName, DwnldCtxProgNameSize, _T("BOM"), DwnldCtxProgNameSize );
	HANDLE	hndl = CreateThread( NULL, 0, GetBOM, (void*)dwnLdCtx, 0, &dwThreadId );
Exit:
	if ( thinOsVer )     delete [] thinOsVer;
	if ( thinFirstName ) delete [] thinFirstName;
	if ( thinLastName )  delete [] thinLastName;
	if ( thinEmail )     delete [] thinEmail;
	if ( thinInvoice )   delete [] thinInvoice;
}			// FetchBOM()
//------------------------------------------------------------------------------------
short	CPNMX_InstallerDlg::LoadBOMcache( wchar_t* deskTempPath, char*& cache )
{	wchar_t	deskTempFile[ 300 ];
	short	retVal = 0;
	swprintf_s( deskTempFile, 300, _T("%s\\PNMX_BOM.dat"), deskTempPath );
	size_t	res;

		// moved to OnInitDialog
//	SECURITY_ATTRIBUTES	security;
//	security.nLength = sizeof( SECURITY_ATTRIBUTES );
//	BOOL B1 = CreateDirectory( deskTempPath, &security );

	FILE* fp = NULL;
	errno_t err = _wfopen_s( &fp, deskTempFile, _T("rb") );
	if ( err != 0 )				{	retVal = -1;	goto	Exit;	}
	size_t	flen = _filelength( _fileno(fp) );							// from <io.h>
		// see if we can get the memory we need
	cache  = new char[ flen + 1 ];										// room for terminating '\0'
	if ( cache == NULL )		{	retVal = -2;	goto	Exit;	}

#if DECRYPT_BOM
	unsigned char * cryptBuf  = new unsigned char[ flen ];				// no need for a terminator
	if ( cryptBuf == NULL )		{	retVal = -3;	goto	Exit;	}	// only if we're decrypting BOM

		// buffer the BOM for decryption
	res = fread( cryptBuf, sizeof(char), flen, fp );					// if we're decrypting BOM ...
	if ( res < flen )			{	retVal = -4;	goto	Exit;	}

		// decrypt the BOM into cache ...
	const unsigned int BOMrespCk[8] = { 0xFD3C65CF, 0x63FA3547, 0xBAE55009, 0x9728F296,
									    0xF7D9743A, 0x192F8FB9, 0xBDB00F95, 0xDEBD503B  };
	unsigned int rjrk[ nRjRoundKeys ];									// 120 round keys in Rijndael256
	rjKeyExpansion( (unsigned char*)BOMrespCk, (unsigned int*)rjrk );	// create tokenCk RoundKeys (again)

	unsigned int BOMrespIv[8] = { 0xF2D36385, 0x329EAB4E, 0xEB3829E1, 0xA79F59EE,
								  0x7A33AED3, 0xF2E899C7, 0xB531CACF, 0x633C549D  };
	rjDecrypt( (unsigned char*)BOMrespIv, cryptBuf, (unsigned long)flen, (unsigned char*)cache, (unsigned int *)rjrk );
	delete [] cryptBuf;													// now done with temp & ibuf
#else
		// cache the BOM
	long res = fread( cache, sizeof(char), flen, fp );					// if the BOM is not encrypted
	if ( res < flen )			{	retVal = -4;	goto	Exit;	}
#endif
	*(cache+flen) = '\0';												// stops the parser

Exit:
	BOOL B2 = 1;
	if ( fp )
	{	fclose( fp );
//		B2 = DeleteFile( deskTempFile );							// success == 0
	}
	if ( B2 == 0 )
	{	wchar_t wbuf[ 100 ];
		swprintf_s( wbuf, 100, _T(" BOM: Unable to delete %s"), deskTempFile );
		c_StatusResult.SetWindowTextW( wbuf );
	}
	else
		c_StatusResult.SetWindowTextW( _T(" BOM: Cleanup OK") );

	return	retVal;
}			// LoadBOMcache()
//------------------------------------------------------------------------------------
short	CPNMX_InstallerDlg::ParseBOM( void )
{		// sort of an abbreviated version of ParseBOM
	char*	cache = NULL;
	short	retVal = LoadBOMcache( m_DeskTempPath, cache );
	if ( retVal < 0 )				{	retVal = -1;	goto	Exit;	}

	char*	progName = NULL;
	char*	memReq   = NULL;
	char*	theLine  = NULL;
	int	ii = 0;

		// get the server name
	if ( lpServerName ) delete [] lpServerName;
	lpServerName = GetToken( cache, ii );									// we'll use this to fetch...
	if ( lpServerName == NULL )		{	retVal = -1;	goto	Exit;	}
		
		// get the ranDir
//	if ( m_RanDir ) delete [] m_RanDir;
//	m_RanDir = GetToken( cache, ii );				// we'll use this later...
//	if ( m_RanDir == NULL )			{	retVal = -2;	goto	Exit;	}

		// get the memory required (one of the things we do want)
	if ( memReq ) delete [] memReq;
	memReq = GetToken( cache, ii );
	if ( memReq == NULL )			{	retVal = -2;	goto	Exit;	}
	int	BmemReq = atoi( memReq );

	m_MemoryRequired = 3 * BmemReq;			// estimate that twice the total { CAB file sizes } is required
	wchar_t	wbuf[64];
	FormatMemStr( m_MemoryRequired, wbuf, 63 );
	c_MemoryRequiredResult.SetWindowTextW( wbuf );

	m_NumProdsRequiringMFClibs = 0;			// somewhat superfluous since constructor zeroes it out too
	m_MFCmemReq = 0;						// ditto - superluous since constructor zeroes it out too

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	while ( 1 )
	{	if ( theLine ) delete [] theLine;
		theLine = GetToken( cache, ii, '\n' );
		if ( theLine == NULL )
			break;
		int	jj = 0;
		if ( progName ) delete [] progName;
		progName = GetToken( theLine, jj );			// offset from beginning of theLine
		if ( progName == NULL ) break;

		if ( _stricmp( progName, "mfcDLLs" ) == 0 )
		{	if ( memReq ) delete [] memReq;
			memReq = GetToken( theLine, jj );
			if ( memReq == NULL )			{	retVal = -3;	goto	Exit;	}
			m_MFCmemReq = atoi( memReq );
			if ( m_MFCckiv ) delete [] m_MFCckiv;
			m_MFCckiv = (unsigned char*)GetToken( theLine, jj );				// should be "ditto" right now
			if ( m_MFCckiv == NULL )		{	retVal = -4;	goto	Exit;	}
			m_ProductsMask |= 0x40;												// SMYTSONB, M at 64
			DWORD	atl, mfc, vcr;
			GetInstalledDLLSizes( &atl, &mfc, &vcr );
			m_MemoryRequired -= atl + mfc + vcr;
		}
		else if ( _stricmp( progName, "BondManager" ) == 0 )
		{	c_BondManager.EnableWindow( TRUE );
			c_BondManager.SetCheck( TRUE );
			m_BondManager = true;
			if ( memReq ) delete [] memReq;
			memReq = GetToken( theLine, jj );
			if ( memReq == NULL )		{	retVal = -5;	goto	Exit;	}
			m_BMmemReq = atoi( memReq );
			if ( m_BMckiv ) delete [] m_BMckiv;
			m_BMckiv = (unsigned char*)GetToken( theLine, jj );					// should be "ditto" right now
			if ( m_BMckiv == NULL )		{	retVal = -6;	goto	Exit;	}
			m_NumProdsRequiringMFClibs++;
			m_ProductsMask |= 0x1;												// SMYTSONB, B at 1
			m_MemoryRequired -= GetPNMXfileSize( _T("BondManager.exe") );
		}
		else if ( _stricmp( progName, "NillaHedge" ) == 0 )
		{	c_NillaHedge.EnableWindow( TRUE );
			c_NillaHedge.SetCheck( TRUE );
			m_NillaHedge = true;
			if ( memReq ) delete [] memReq;
			memReq = GetToken( theLine, jj );
			if ( memReq == NULL )		{	retVal = -7;	goto	Exit;	}
			m_NHmemReq = atoi( memReq );									// double the CAB file size
			if ( m_NHckiv ) delete [] m_NHckiv;
			m_NHckiv = (unsigned char*)GetToken( theLine, jj );				// should be "ditto" right now
			if ( m_NHckiv == NULL )		{	retVal = -8;	goto	Exit;	}
			m_NumProdsRequiringMFClibs++;
			m_ProductsMask |= 0x2;											// SMYTSONB, N at 2
			m_MemoryRequired -= GetPNMXfileSize( _T("NillaHedge.exe") );
		}
		else if ( _stricmp( progName, "OptionChains" ) == 0 )
		{	c_OptionChains.EnableWindow( TRUE );
			c_OptionChains.SetCheck( TRUE );
			if ( m_OCRckiv ) delete [] m_OCRckiv;
			m_OCRckiv = (unsigned char*)GetToken( theLine, jj );
			if ( m_OCRckiv == NULL )	{	retVal = -9;	goto	Exit;	}
			m_OptionChains = true;
		}
		else if ( _stricmp( progName, "StrategyExplorer" ) == 0 )
		{	c_StrategyExplorer.EnableWindow( TRUE );
			c_StrategyExplorer.SetCheck( TRUE );
			m_StrategyExplorer = true;
			if ( memReq ) delete [] memReq;
			memReq = GetToken( theLine, jj );
			if ( memReq == NULL )		{	retVal = -10;	goto	Exit;	}
			m_SXmemReq = atoi( memReq );									// double the CAB file size
			if ( m_SXckiv ) delete [] m_SXckiv;
			m_SXckiv = (unsigned char*)GetToken( theLine, jj );				// should be "ditto" right now
			if ( m_SXckiv == NULL )		{	retVal = -11;	goto	Exit;	}
			m_NumProdsRequiringMFClibs++;
			m_ProductsMask |= 0x8;											// SMYTSONB, SX at 8
			m_MemoryRequired -= GetPNMXfileSize( _T("StrategyExplorer.exe") );
		}
		else if ( _stricmp( progName, "PpcInstall" ) == 0 )
		{	if ( memReq ) delete [] memReq;
			memReq = GetToken( theLine, jj );
			if ( memReq == NULL )		{	retVal = -12;	goto	Exit;	}
			m_PpcInstallMemReq = atoi( memReq );
			if ( m_PPCckiv ) delete [] m_PPCckiv;
			m_PPCckiv = (unsigned char*)GetToken( theLine, jj );
			if ( m_PPCckiv == NULL )	 {	retVal = -13;	goto	Exit;	}
		}
		else if ( _stricmp( progName, "YieldCurveFitter" ) == 0 )
		{	c_YieldCurveFitter.EnableWindow( TRUE );
			c_YieldCurveFitter.SetCheck( TRUE );
			m_YieldCurveFitter = true;
			if ( memReq ) delete [] memReq;
			memReq = GetToken( theLine, jj );
			if ( memReq == NULL )		{	retVal = -14;	goto	Exit;	}
			m_YCFmemReq = atoi( memReq );
			if ( m_YCFckiv ) delete [] m_YCFckiv;
			m_YCFckiv = (unsigned char*)GetToken( theLine, jj );
			if ( m_YCFckiv == NULL )	{	retVal = -15;	goto	Exit;	}
			m_NumProdsRequiringMFClibs++;
			m_ProductsMask |= 0x20;											// SMYTSONB, Y at 32
			m_MemoryRequired -= GetPNMXfileSize( _T("YieldCurveFitter.exe") );
		}
		else if ( _stricmp( progName, "TBillRates" ) == 0 )
		{	c_TBillRates.EnableWindow( TRUE );
			c_TBillRates.SetCheck( TRUE );
			if ( m_TBRckiv ) delete [] m_TBRckiv;
			m_TBRckiv = (unsigned char*)GetToken( theLine, jj );
			if ( m_TBRckiv == NULL )	{	retVal = -16;	goto	Exit;	}
			m_TBillRates = true;
		}
		else if ( _stricmp( progName, "StockManager" ) == 0 )
		{	c_StockManager.EnableWindow( TRUE );
			c_StockManager.SetCheck( TRUE );
			m_StockManager = true;
			if ( memReq ) delete [] memReq;
			memReq = GetToken( theLine, jj );
			if ( memReq == NULL )		{	retVal = -17;	goto	Exit;	}
			m_SMmemReq = atoi( memReq );									// double the CAB file size
			if ( m_SMckiv ) delete [] m_SMckiv;
			m_SMckiv = (unsigned char*)GetToken( theLine, jj );				// should be "ditto" right now
			if ( m_SMckiv == NULL )		{	retVal = -18;	goto	Exit;	}
			m_NumProdsRequiringMFClibs++;
			m_ProductsMask |= 0x80;											// SMYTSONB, SM at 128
			m_MemoryRequired -= GetPNMXfileSize( _T("StockManager.exe") );
		}
		else if ( _stricmp( progName, "CovarAcct" ) == 0 )
		{	c_CovarAcct.EnableWindow( TRUE );
			c_CovarAcct.SetCheck( TRUE );
			if ( m_VARckiv ) delete [] m_VARckiv;
			m_VARckiv = (unsigned char*)GetToken( theLine, jj );
			if ( m_VARckiv == NULL )	{	retVal = -19;	goto	Exit;	}
			m_CovarAcct = true;
		}
	}

	if ( m_MemoryAvail <= m_MemoryRequired )
		c_StatusResult.SetWindowTextW( _T(" Insufficient memory") );
	else if ( retVal >= 0 )
	{	c_BOM_Label.SetWindowText( _T("Bill of Materials Found") );
		c_BOM_Label.EnableWindow( TRUE );
		c_BOM_Found.SetCheck( TRUE );
		c_BOM_Found.ShowWindow( SW_SHOWNOACTIVATE );
		m_BOM_Found = true;
		m_ButtonState |= 0x8;
	}
Exit:
	if ( cache    ) delete [] cache;
	if ( theLine  ) delete [] theLine;
	if ( progName ) delete [] progName;
	if ( memReq   ) delete [] memReq;
	return	retVal;	
}			// ParseBOM()
//------------------------------------------------------------------------------------
short	CPNMX_InstallerDlg::RegisterKeyValuePair(
	HKEY			hKey,
	wchar_t*		subkey,
	DWORD			type,
	unsigned short	nBytes,
	void*			value	)
{	HKEY			result;
	DWORD			disposition;
	LPWSTR			cls = _T("binary");
	CString			cs(subkey);
	int	lastBackslash = cs.ReverseFind( _T('\\') );
	CString prefix = cs.Left( lastBackslash );				// everything to the left of the last backslash
	CString suffix = cs.Mid( lastBackslash+1 );				// everything to the right of the last backslash
	long retVal = CeRegCreateKeyEx( hKey, prefix, 0, cls, 0, 0, NULL, &result, &disposition );
	if ( retVal != ERROR_SUCCESS )
		return	-1;									// the key couldn't be created or opened
	retVal = CeRegSetValueEx( result, suffix, 0, type, (LPBYTE)value, nBytes );
	if ( retVal != ERROR_SUCCESS )
		return	-2;									// the key couldn't be written
	CeRegCloseKey( result );
	CeRegCloseKey( hKey );
	return	0;
}			// RegisterKeyValuePair()
//------------------------------------------------------------------------------------
BOOL	CPNMX_InstallerDlg::GetOsVersion( void )
{	CEOSVERSIONINFO		verInfo;
	SetLastError( 0 );								// coredll.lib, winbase.h
	BOOL	B1 = CeGetVersionEx( &verInfo );		// req:  rapi.h, rapi.lib
#ifdef _DEBUG
	if ( ! B1 )
	{	wchar_t msgBuf[256];
		DWORD	err = CeGetLastError();
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, err, 0, (LPWSTR)&msgBuf, 0, NULL );
		TRACE( _T("Couldn't get the OS version: %s\n"), msgBuf );
	}
#endif
	const unsigned short WBUF_SIZE = 64;
	wchar_t	wbuf[ WBUF_SIZE ];
	m_OsMajVerNum = verInfo.dwMajorVersion;
	if ( m_OsMajVerNum < 0 ) m_OsMajVerNum = -1;

	long	minVer = verInfo.dwMinorVersion;
	long	bld = verInfo.dwBuildNumber;
	if ( minVer < 0 ) minVer = -1;
	swprintf_s( wbuf, WBUF_SIZE, _T("%li.%li.%li"), m_OsMajVerNum, minVer, bld );
	m_OsVersion = wbuf;
	c_OsVersionResult.SetWindowTextW( wbuf );

	long plat = verInfo.dwPlatformId;
	if ( plat < 0 ) plat = -1;
	swprintf_s( wbuf, WBUF_SIZE, _T("%li"), plat );
	c_PlatformIdResult.SetWindowTextW( wbuf );

	if ( B1 )
		m_ButtonState |= 0x01;						// xxxx xx1x -> 01 
	else
		m_ButtonState &= 0xFE;						// xxxx xx0x -> FE
	return	B1;
}			// GetOsVersion()
//--------------------------------------------------------------------------------------------
bool	CPNMX_InstallerDlg::GetMemoryAvail( void )
{	MEMORYSTATUS	memStats;
	ZeroMemory( &memStats, sizeof(MEMORYSTATUS) );
	CeGlobalMemoryStatus( &memStats );

	size_t Bavail = memStats.dwAvailPhys;
	if ( Bavail < 0 )
	{
		c_MemoryAvailResult.SetWindowTextW( _T("??? KB") );
		return	false;
	}
	m_MemoryAvail = (long)Bavail;
	wchar_t	wbuf[64];
	FormatMemStr( m_MemoryAvail, wbuf, 63 );
	c_MemoryAvailResult.SetWindowTextW( wbuf );
	return	true;
}			// GetMemoryAvail()
//--------------------------------------------------------------------------------------------
bool	CPNMX_InstallerDlg::GetDeviceID( void )
{
//	GetSerialNumber();								// only works on the device!
	DEVICEID devID = CeGetDeviceId();				// really a DWORD (requires ceutil.lib)
	m_DeviceID = (unsigned long)devID;

	const unsigned short WBUF_SIZE = 64;
	wchar_t	wbuf[ WBUF_SIZE ];
	swprintf_s( wbuf, WBUF_SIZE, _T("0x%X"), devID );
	c_DeviceIdResult.SetWindowText( wbuf );

	bool	b1 = ( devID > 0 );
	if ( b1 )
		m_ButtonState |= 0x02;						// xxxx xx1x |-> mask = 0x02 
	else
		m_ButtonState &= 0xFD;						// xxxx xx0x &-> mask = 0xFD
	return	b1;
}			// GetDeviceID()
//--------------------------------------------------------------------------------------------
void CPNMX_InstallerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
//--------------------------------------------------------------------------------------------
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPNMX_InstallerDlg::OnPaint()
{
	if ( IsIconic() )
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

			// Center icon in client rectangle
		int cxIcon = GetSystemMetrics( SM_CXICON );
		int cyIcon = GetSystemMetrics( SM_CYICON );
		CRect rect;
		GetClientRect( &rect );
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}			// OnPaint()s
//--------------------------------------------------------------------------------------------
// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPNMX_InstallerDlg::OnQueryDragIcon()
{
	return	static_cast<HCURSOR>(m_hIcon);
}			// OnQueryDragIcon()
//--------------------------------------------------------------------------------------------
LRESULT		CPNMX_InstallerDlg::OnPNMX_InstallerAlreadyRunning( WPARAM, LPARAM )
{
	return	WM_PNMX_INSTALLER_ALREADY_RUNNING;
}			// OnPNMX_InstallerAlreadyRunning()
//--------------------------------------------------------------------------------------------
LRESULT		CPNMX_InstallerDlg::OnPNMX_InstallerDeviceConnected( WPARAM, LPARAM )
{
	c_DeviceConnected.ShowWindow( SW_SHOWNOACTIVATE );
	c_DeviceConnected.SetCheck( TRUE );
	c_DeviceConnectedLabel.SetWindowTextW( _T("Device Connected") );
	m_DeviceConnected = TRUE;

		// snag info and indicate status
	c_StatusResult.SetWindowTextW( _T(" Device Connected.") );
	GetOsVersion();		// writes to the screen too
	GetDeviceID();		// ditto
	GetMemoryAvail();	// ditto
//	CeGetTempPath( 256, m_DeviceTempPath );				// can't specify this in the INF file
	m_DeviceTempPath = _T("\\Temp\\PocketNumerix");		// a fixed directory

		// create the temp directory if it doesn't already exist
//	wchar_t	msgBuf[ 256 ];
	CString path = m_DeviceTempPath;
	path += _T("\\");									// the terminating '\' is just a signpost for token extraction
	int		len = path.GetLength();
	int		bs = 1;
	while ( bs < len )
	{	bs = path.Find( _T('\\'), bs );
		if ( bs < 0 )
			break;
		CString prefix = path.Left( bs );				// e.g. \temp\ --> bs=5, want \temp, so length=5
		BOOL b1 = CeCreateDirectory( prefix, NULL );
#ifdef _DEBUG
		TRACE( _T("PNMX_InstallerDlg::OnPNMX_InstallerDeviceConnected: CreateDirectory('%s') --> %s\n"),
				prefix, ( b1 ? _T("succeeded") : _T("failed") ) );
#endif
		bs++;				// skip over the most recent backslash
	}
	EnableFetchButton( _T("Device Connected"), 4 );
	return	WM_PNMX_INSTALLER_DEVICE_CONNECTED;
}			// OnPNMX_InstallerDeviceConnected()
//--------------------------------------------------------------------------------------------
void	CPNMX_InstallerDlg::EnableFetchButton( CString cs, unsigned short bb )
{		//  0 - first name
		//  1 - last name
		//  2 - email address
		//  3 - invoice number
		//  4 - device connected
	unsigned short mask = ( 1 << bb );
	if ( cs == _T("") )		m_InvoiceFields &= ~mask;	// force a zero in the given bit
	else					m_InvoiceFields |=  mask;	// force a one in the given bit

	bool enableFetch = ( m_InvoiceFields >= 0x1F );
	c_FetchButton.EnableWindow( enableFetch );
	int	mode = SW_HIDE;
	if ( enableFetch )
	{	m_ButtonState |= 0x4;				// all invoice fields are represented by the third bit
		mode = SW_SHOWNORMAL;
	}
	c_FetchButton.ShowWindow( mode );
}			// EnableFetchButton()
//--------------------------------------------------------------------------------------------
void CPNMX_InstallerDlg::OnBnClickedOptionChains( void )
{
	m_OptionChains = ( c_OptionChains.GetCheck() == BST_CHECKED );
	UpdateMemoryStatus();
}			// OnBnClickedOptionChains()
//--------------------------------------------------------------------------------------------
void CPNMX_InstallerDlg::OnBnClickedTBillRates( void )
{
	m_TBillRates = ( c_TBillRates.GetCheck() == BST_CHECKED );
	UpdateMemoryStatus();
}			// OnBnClickedTBillRates()
//--------------------------------------------------------------------------------------------
void CPNMX_InstallerDlg::OnBnClickedBondManager( void )
{	DWORD	atl, mfc, vcr;
	DWORD	installedSize = GetPNMXfileSize( _T("BondManager.exe") );

	m_BondManager = ( c_BondManager.GetCheck() == BST_CHECKED );
	if ( m_BondManager )
	{	m_MemoryRequired += 3 * m_BMmemReq;
		m_MemoryRequired -= installedSize;
		if ( m_NumProdsRequiringMFClibs == 0 )
		{	m_MemoryRequired += 3 * m_MFCmemReq;
			GetInstalledDLLSizes( &atl, &mfc, &vcr );
			m_MemoryRequired -= atl + mfc + vcr;
			m_ProductsMask |= 0x40;						// SMYTSONB, M at 64
		}
		m_NumProdsRequiringMFClibs++;
		m_ProductsMask |= 0x1;							// SMYTSONB, B at 1
	}
	else
	{	m_MemoryRequired -= 3 * m_BMmemReq;
		m_MemoryRequired += installedSize;
		m_NumProdsRequiringMFClibs--;
		if ( m_NumProdsRequiringMFClibs == 0 )
		{	m_MemoryRequired -= 3 * m_MFCmemReq;
			GetInstalledDLLSizes( &atl, &mfc, &vcr );
			m_MemoryRequired += atl + mfc + vcr;
			m_ProductsMask &= (~0x40);					// SMYTSONB, M at 64
		}
		m_ProductsMask &= (~0x1);						// SMYTSONB, B at 1
	}
	UpdateMemoryStatus();
}			// OnBnClickedBondManager()
//--------------------------------------------------------------------------------------------
void CPNMX_InstallerDlg::OnBnClickedNillaHedge( void )
{	DWORD	atl, mfc, vcr;
	DWORD	installedSize = GetPNMXfileSize( _T("NillaHedge.exe") );

	m_NillaHedge = ( c_NillaHedge.GetCheck() == BST_CHECKED );
	if ( m_NillaHedge )
	{	m_MemoryRequired += 3 * m_NHmemReq;
		m_MemoryRequired -= installedSize;
		if ( m_NumProdsRequiringMFClibs == 0 )
		{	m_MemoryRequired += 3 * m_MFCmemReq;
			GetInstalledDLLSizes( &atl, &mfc, &vcr );
			m_MemoryRequired -= atl + mfc + vcr;
			m_ProductsMask |= 0x40;						// SMYTSONB, M at 64
		}
		m_NumProdsRequiringMFClibs++;
		m_ProductsMask |= 0x2;							// SMYTSONB, N at 2
	}
	else
	{	m_MemoryRequired -= 3 * m_NHmemReq;
		m_MemoryRequired += installedSize;
		m_NumProdsRequiringMFClibs--;
		if ( m_NumProdsRequiringMFClibs == 0 )
		{	m_MemoryRequired -= 3 * m_MFCmemReq;
			GetInstalledDLLSizes( &atl, &mfc, &vcr );
			m_MemoryRequired += atl + mfc + vcr;
			m_ProductsMask &= (~0x40);					// SMYTSONB, M at 64
		}
		m_ProductsMask &= (~0x2);						// SMYTSONB, N at 2
	}
	UpdateMemoryStatus();
}			// OnBnClickedNillaHedge()
//--------------------------------------------------------------------------------------------
void CPNMX_InstallerDlg::OnBnClickedStockManager( void )
{	DWORD	atl, mfc, vcr;
	DWORD	installedSize = GetPNMXfileSize( _T("StockManager.exe") );

	m_StockManager = ( c_StockManager.GetCheck() == BST_CHECKED );
	if ( m_StockManager )
	{	m_MemoryRequired += 3 * m_SMmemReq;
		m_MemoryRequired -= installedSize;
		if ( m_NumProdsRequiringMFClibs == 0 )
		{	m_MemoryRequired += 3 * m_MFCmemReq;
			GetInstalledDLLSizes( &atl, &mfc, &vcr );
			m_MemoryRequired -= atl + mfc + vcr;
			m_ProductsMask |= 0x40;						// SMYTSONB, M at 64
		}
		m_NumProdsRequiringMFClibs++;
		m_ProductsMask |= 0x80;							// SMYTSONB, SM at 128
	}
	else
	{	m_MemoryRequired -= 3 * m_SMmemReq;
		m_MemoryRequired += installedSize;
		m_NumProdsRequiringMFClibs--;
		if ( m_NumProdsRequiringMFClibs == 0 )
		{	m_MemoryRequired -= 3 * m_MFCmemReq;
			GetInstalledDLLSizes( &atl, &mfc, &vcr );
			m_MemoryRequired += atl + mfc + vcr;
			m_ProductsMask &= (~0x40);					// SMYTSONB, M at 64
		}
		m_ProductsMask &= (~0x80);						// SMYTSONB, SM at 128
	}
	UpdateMemoryStatus();
}			// OnBnClickedStockManager()
//--------------------------------------------------------------------------------------------
void CPNMX_InstallerDlg::OnBnClickedStrategyExplorer( void )
{	DWORD	atl, mfc, vcr;
	DWORD	installedSize = GetPNMXfileSize( _T("StrategyExplorer.exe") );

	m_StrategyExplorer = ( c_StrategyExplorer.GetCheck() == BST_CHECKED );
	if ( m_StrategyExplorer )
	{	m_MemoryRequired += 3 * m_SXmemReq;
		m_MemoryRequired -= installedSize;
		if ( m_NumProdsRequiringMFClibs == 0 )
		{	m_MemoryRequired += 3 * m_MFCmemReq;
			GetInstalledDLLSizes( &atl, &mfc, &vcr );
			m_MemoryRequired -= atl + mfc + vcr;
			m_ProductsMask |= 0x40;						// SMYTSONB, M at 64
		}
		m_NumProdsRequiringMFClibs++;
		m_ProductsMask |= 0x8;							// SMYTSONB, S at 8
	}
	else
	{	m_MemoryRequired -= 3 * m_SXmemReq;
		m_MemoryRequired += installedSize;
		m_NumProdsRequiringMFClibs--;
		if ( m_NumProdsRequiringMFClibs == 0 )
		{	m_MemoryRequired -= 3 * m_MFCmemReq;
			GetInstalledDLLSizes( &atl, &mfc, &vcr );
			m_MemoryRequired += atl + mfc + vcr;
			m_ProductsMask &= (~0x40);					// SMYTSONB, M at 64
		}
		m_ProductsMask &= (~0x8);						// SMYTSONB, S at 8
	}
	UpdateMemoryStatus();
}			// OnBnClickedStrategyExplorer()
//--------------------------------------------------------------------------------------------
void CPNMX_InstallerDlg::OnBnClickedYieldCurveFitter( void )
{	DWORD	atl, mfc, vcr;
	DWORD	installedSize = GetPNMXfileSize( _T("YieldCurveFitter.exe") );

	m_YieldCurveFitter = ( c_YieldCurveFitter.GetCheck() == BST_CHECKED );
	if ( m_YieldCurveFitter )
	{	m_MemoryRequired += 3 * m_YCFmemReq;
		m_MemoryRequired -= installedSize;
		if ( m_NumProdsRequiringMFClibs == 0 )
		{	m_MemoryRequired += 3 * m_MFCmemReq;
			GetInstalledDLLSizes( &atl, &mfc, &vcr );
			m_MemoryRequired -= atl + mfc + vcr;
			m_ProductsMask |= 0x40;						// SMYTSONB, M at 64
		}
		m_NumProdsRequiringMFClibs++;
		m_ProductsMask |= 0x20;							// SMYTSONB, Y at 32
	}
	else
	{	m_MemoryRequired -= 3 * m_YCFmemReq;
		m_MemoryRequired += installedSize;
		m_NumProdsRequiringMFClibs--;
		if ( m_NumProdsRequiringMFClibs == 0 )
		{	m_MemoryRequired -= 3 * m_MFCmemReq;
			GetInstalledDLLSizes( &atl, &mfc, &vcr );
			m_MemoryRequired += atl + mfc + vcr;
			m_ProductsMask &= (~0x40);					// SMYTSONB, M at 64
		}
		m_ProductsMask &= (~0x20);						// SMYTSONB, Y at 32
	}
	UpdateMemoryStatus();
}			// OnBnClickedYieldCurveFitter()
//--------------------------------------------------------------------------------------------
DWORD	CPNMX_InstallerDlg::GetTempFileSize( wchar_t* tempFilename )
{
	return	GetFileSize( m_DeviceTempPath, tempFilename );
}			// GetTempFileSize()
//--------------------------------------------------------------------------------------------
DWORD	CPNMX_InstallerDlg::GetPNMXfileSize( wchar_t* pnmxFilename )
{
	return	GetFileSize( m_progFilesDir, pnmxFilename );
}			// GetPNMXfileSize()
//--------------------------------------------------------------------------------------------
DWORD	CPNMX_InstallerDlg::GetFileSize( wchar_t* dirName, wchar_t* pnmxFilename )
{	BOOL			B1;
	HANDLE			handle;
	wchar_t			pathName[ MAX_PATH ];
	swprintf_s( pathName, MAX_PATH, _T("%s\\%s"), dirName, pnmxFilename );
	CE_FIND_DATA	findData;
	findData.nFileSizeLow = 0;
	handle = CeFindFirstFile( pathName, &findData );
	if ( handle != INVALID_HANDLE_VALUE )
		B1 = CeFindClose( handle );			// do nothing if it fails (B1 == 0)
	return	findData.nFileSizeLow;
}			// GetFileSize()
//--------------------------------------------------------------------------------------------
void	CPNMX_InstallerDlg::GetInstalledDLLSizes( DWORD* atl, DWORD* mfc, DWORD* vcr )
{
	*atl = GetPNMXfileSize( _T("atl80.DLL") );
	*mfc = GetPNMXfileSize( _T("MFC80U.DLL") );
	*vcr = GetPNMXfileSize( _T("msvcr80.DLL") );
}			// GetInstlaledDLLSizes()
//--------------------------------------------------------------------------------------------
void	CPNMX_InstallerDlg::UpdateMemoryStatus( void )
{	
	bool	success = GetMemoryAvail();			// sets m_MemoryAvail and updates the GUI

	const unsigned short WBUF_SIZE = 64;
	wchar_t	wbuf[ WBUF_SIZE ];
	FormatMemStr( m_MemoryRequired, wbuf, 63 );
	c_MemoryRequiredResult.SetWindowText( wbuf );

	bool	enoughMemory = ( m_MemoryAvail > m_MemoryRequired );
	bool	productsToInstall = ( (m_BondManager == TRUE)	   != m_BondManagerInstalled		)
							||  ( (m_NillaHedge == TRUE)	   != m_NillaHedgeInstalled			)
							||  ( (m_StrategyExplorer == TRUE) != m_StrategyExplorerInstalled	)
							||  ( (m_StockManager == TRUE)	   != m_StockManagerInstalled		)
							||  ( (m_YieldCurveFitter == TRUE) != m_YieldCurveFitterInstalled	)
							||  ( (m_OptionChains == TRUE)	   != m_OptionChainsInstalled		)
							||  ( (m_CovarAcct == TRUE)		   != m_CovarAcctInstalled			)
							||  ( (m_TBillRates == TRUE)	   != m_TBillRatesInstalled			);

	if ( ! fetchInProgress )
	{	c_FetchButton.EnableWindow( productsToInstall  &&  enoughMemory );
		if ( productsToInstall )
			c_StatusResult.SetWindowTextW( enoughMemory
				? _T(" Sufficient memory for install is available")
				: _T(" Insufficient memory available for install") );
		else
			c_StatusResult.SetWindowTextW( _T(" Done") );
	}
}			// UpdateMemoryStatus
//--------------------------------------------------------------------------------------------
short	CPNMX_InstallerDlg::Subscribe(
	wchar_t*		progName,
	char*			hashUUID,
	CButton&		downloadedButton,
	CButton&		installedButton		)
{	const unsigned short WBUF_SIZE = 256;
	wchar_t			wbuf[ WBUF_SIZE ];
	short			retVal = 0;
	CString			cs = progName;			// wide string to CString conversion
	wchar_t*		siteKey = NULL;
	wchar_t*		pageKey = NULL;
	wchar_t*		recvWaitKey = NULL;
	wchar_t*		sendWaitKey = NULL;
	unsigned char*	ckiv_mime = NULL;
	char*			fbuf = NULL;
	char*			clrBuf = NULL;
	FILE*			fp = NULL;

	if ( wcscmp( _T("OptionChains"), progName ) == 0 )
	{	siteKey = _T("SOFTWARE\\PocketNumerix\\OptionChains\\Server");
		pageKey = _T("SOFTWARE\\PocketNumerix\\OptionChains\\Page");
		recvWaitKey = _T("SOFTWARE\\PocketNumerix\\OptionChains\\RecvWait");
		sendWaitKey = _T("SOFTWARE\\PocketNumerix\\OptionChains\\SendWait");
		ckiv_mime = m_OCRckiv;
	}
	else if ( wcscmp( _T("TBillRates"), progName ) == 0 )
	{	siteKey = _T("SOFTWARE\\PocketNumerix\\TBillRates\\Server");
		pageKey = _T("SOFTWARE\\PocketNumerix\\TBillRates\\Page");
		recvWaitKey = _T("SOFTWARE\\PocketNumerix\\TBillRates\\RecvWait");
		sendWaitKey = _T("SOFTWARE\\PocketNumerix\\TBillRates\\SendWait");
		ckiv_mime = m_TBRckiv;
	}
	else if ( wcscmp( _T("CovarAcct"), progName ) == 0 )
	{	siteKey = _T("SOFTWARE\\PocketNumerix\\Covar\\Server");
		pageKey = _T("SOFTWARE\\PocketNumerix\\Covar\\Page");
		recvWaitKey = _T("SOFTWARE\\PocketNumerix\\Covar\\RecvWait");
		sendWaitKey = _T("SOFTWARE\\PocketNumerix\\Covar\\SendWait");
		ckiv_mime = m_VARckiv;
	}
	if ( siteKey == NULL  ||  pageKey == NULL )
	{	retVal = -1;
		goto	Exit;
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// DownloadURL uses:  ctx->c_StatusResult, ctx->progName, ctx->serverName, ctx->urlRequest, ctx->fileName
	DownloadCtx_T	downloadCtx;
	downloadCtx.c_StatusResult = &c_DownloadStatusResult;
	downloadCtx.expectedFileSize = 0;
	wcsncpy_s( downloadCtx.progName, DwnldCtxProgNameSize, progName, DwnldCtxProgNameSize );
	strncpy_s( downloadCtx.serverName, DwnldCtxServerNameSize, lpServerName, DwnldCtxServerNameSize );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// build the urlRequest
	char* osVer = GetThinString( m_OsVersion );
	if ( osVer == NULL )
	{	retVal = -2;
		goto	Exit;
	}
	char* thinProgName = GetThinString( progName );
	if ( thinProgName == NULL )
	{	retVal = -3;
		goto	Exit;
	}
	char* thinInvoiceNo = GetThinString( m_InvoiceNumber );
	if ( thinInvoiceNo == NULL )
	{	retVal = -4;
		goto	Exit;
	}

	const unsigned short	BUF_SIZE = 1024;		// should be overkill
	char	ibuf[ BUF_SIZE ];
	sprintf_s( ibuf, BUF_SIZE, "?in=%s?pn=%s?os=%s?dv=%X?uu=%s?hu=%s",
		thinInvoiceNo, thinProgName, osVer, m_DeviceID,	(m_UUID ? m_UUID : "0"), (hashUUID ? hashUUID : "0") );
#ifdef _DEBUG
	{	wchar_t* ws = MakeWideString( ibuf );
		TRACE( _T("Subscribe: ds='%s'\n"), ws );
		delete [] ws;
	}
#endif
	size_t slen = strlen( ibuf );
	if ( osVer )		 delete [] osVer;			osVer = NULL;
	if ( thinProgName )  delete [] thinProgName;	thinProgName = NULL;
	if ( thinInvoiceNo ) delete [] thinInvoiceNo;	thinInvoiceNo = NULL;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// setup encryption
	const unsigned int subscribeDsCk[8] = {	0x430EA57B, 0x2953C588, 0x3FE4A1C3, 0xFBA136E5,
											0xDAE65968, 0xD3882CEF, 0x81FEBE6E, 0xFA8CDF63	};
	unsigned int rcrk[ nRcRoundKeys ];					// 44 round keys in RC6
	rcKeyExpansion( (unsigned int *)subscribeDsCk, rcrk );

	unsigned char obuf[ BUF_SIZE ];
	unsigned int subscribeDsIv[8] = {	0xD08BCE8F, 0x79E45A5F, 0xEBB6BCC0, 0xAFFE561C,
										0x353CF5C4, 0xBDE8B8FF, 0xAF52D323, 0xED183B77 	};
	rcCrypt( (unsigned char*)subscribeDsIv, (unsigned char*)ibuf, (unsigned long)slen, obuf, rcrk );
	*(obuf+slen) = '\0';								// for nicer debugger display

		// MIME encode obuf
	unsigned char mimeBuf[ BUF_SIZE * 4/3 ];							// BUF_SIZE * 4/3
	unsigned long mimeBufLen = MimeEncode( obuf, (unsigned long)slen, mimeBuf );
	sprintf_s( downloadCtx.urlRequest, DwnldCtxUrlRequestSize, "%s/pnmxSubscribe.php?ds=%s", ServerPagePrefix, mimeBuf );
#if _DEBUG
	{	wchar_t* ws = MakeWideString( downloadCtx.urlRequest );
		TRACE( _T("Subscribe:urlRequest: %s\n"), ws );
		delete [] ws;		
	}
#endif
		// end of request encryption
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// generate a desktop filename for the download
	char*	ranString = MakeRanName();
	if ( ranString == NULL )
	{	CString cs = progName;
		swprintf_s( wbuf, WBUF_SIZE, _T(" %s: desk memory alloc failed"), cs );
		c_StatusResult.SetWindowTextW( wbuf );
		retVal = -5;
		goto	Exit;
	}
	wchar_t*	w_ranString = MakeWideString( ranString );
	delete [] ranString;									// needed a wide version
	if ( w_ranString == NULL )
	{	CString cs = progName;
		swprintf_s( wbuf, WBUF_SIZE, _T(" %s: desk memory alloc failed"), cs );
		c_StatusResult.SetWindowTextW( wbuf );
		retVal = -6;
		goto	Exit;
	}
	swprintf_s( downloadCtx.fileName, DwnldCtxFileNameSize, _T("%s\\%s"), m_DeskTempPath, w_ranString );	// where the download goes on the dektop machine
	DWORD dres = DownloadURL( &downloadCtx );
	if ( dres != 0 )
	{	swprintf_s( wbuf, WBUF_SIZE, _T(" %s: download failed -> %d"), progName, (short)dres );
		c_StatusResult.SetWindowTextW( wbuf );
		c_FetchButton.EnableWindow( FALSE );
		retVal = -7;
		goto	Exit;
	}
		// notify the user that the subscription has been downloaded
	swprintf_s( wbuf, WBUF_SIZE, _T(" %s downloaded"), progName );
	c_StatusResult.SetWindowTextW( wbuf );
	downloadedButton.SetCheck( BST_CHECKED );
	downloadedButton.ShowWindow( SW_SHOWNOACTIVATE );

		// read the response
	errno_t	err = _wfopen_s( &fp, downloadCtx.fileName, _T("rb") );
	if ( err != 0 )
	{	retVal = -8;
		goto	Exit;
	}
	size_t flen = _filelength( _fileno( fp ) );
	if ( flen <= 0 )
	{	retVal = -9;
		goto	Exit;
	}

	fbuf = new char[ flen + 1 ];
	size_t	bytesRead = fread( fbuf, sizeof(char), flen+1, fp );
	if ( bytesRead < flen )
	{	retVal = -10;
		goto	Exit;
	}
	fclose( fp );		fp = NULL;
	DeleteDesktopFile( progName, downloadCtx.fileName );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// decrypt the response
	slen = strlen( (char*)ckiv_mime );
	unsigned char	ckiv[ 2 * AES_BLOCK_SIZE + 1 ];		// the +1 prevents stack corruption
	long lres = MimeDecode( ckiv_mime, (unsigned long)slen, ckiv );	// extract the decrypt key from the token captured by ParseBOM

	unsigned int rjrk[ nRjRoundKeys ];					// 120 round keys in Rijndael256
	rjKeyExpansion( (unsigned char*)ckiv, (unsigned int*)rjrk );
	clrBuf = new char[ flen + 1 ];
		// iv starts AES_BLOCK_SIZE bytes from the BOF
	rjDecrypt( (unsigned char*)(ckiv+AES_BLOCK_SIZE), (unsigned char*)fbuf, (unsigned long)flen, (unsigned char*)clrBuf, (unsigned int *)rjrk );
	*(clrBuf+flen) = '\0';								// for nicer debugger display

	int	ii = 0;											// token start
	char* token = GetToken( clrBuf, ii );
	if ( token == NULL )
	{	retVal = -11;
		goto	Exit;
	}

		// put 'Server' in HKLM \\ { SOFTWARE\\PocketNumerix\\TBillRates\\Server  or  SOFTWARE\\PocketNumerix\\OptionChains\\Server }
	slen = strlen( token );
	short sres = RegisterKeyValuePair( HKEY_LOCAL_MACHINE, siteKey, REG_BINARY,
									   (unsigned short)(sizeof(char)*(slen+1)), (void*)token );
	delete [] token;	token = NULL;
	if ( sres < 0 )
	{	retVal = -12;
		goto	Exit;
	}

	token = GetToken( clrBuf, ii );
	if ( token == NULL )
	{	retVal = -13;
		goto	Exit;
	}

		// put 'page' in HKLM \\ { SOFTWARE\\PocketNumerix\\TBillRates\\Page  or  SOFTWARE\\PocketNumerix\\OptionChains\\Page }
	slen = strlen( token );
	sres = RegisterKeyValuePair( HKEY_LOCAL_MACHINE, pageKey, REG_BINARY,
								 (unsigned short)(sizeof(char)*(slen+1)), (void*)token );
	delete [] token;	token = NULL;
	if ( sres < 0 )
	{	retVal = -14;
		goto	Exit;
	}

		// put 'SocketRecvWait' in HKLM \\ SOFTWARE\\PocketNumerix\\SocketRecvWait
	DWORD	milliseconds = 30000;							// allow 30 seconds before failure
	sres = RegisterKeyValuePair( HKEY_LOCAL_MACHINE, recvWaitKey, REG_DWORD, sizeof(DWORD), (void*)&milliseconds );
	if ( sres < 0 )	c_StatusResult.SetWindowTextW( _T("Failed to register RecvWait") );

		// put 'SocketSendWait' in HKLM \\ SOFTWARE\\PocketNumerix\\SocketRecvWait
	sres = RegisterKeyValuePair( HKEY_LOCAL_MACHINE, sendWaitKey, REG_DWORD, sizeof(DWORD), (void*)&milliseconds );
	if ( sres < 0 )	c_StatusResult.SetWindowTextW( _T("Failed to register SendWait") );


		// notify the user that the subscription has been installed
	swprintf_s( wbuf, WBUF_SIZE, _T(" %s: installed"), progName );
	c_InstallStatusResult.SetWindowTextW( wbuf );
	installedButton.SetCheck( BST_CHECKED );
	installedButton.ShowWindow( SW_SHOWNOACTIVATE );
Exit:
	if ( fp )			 fclose( fp );
	if ( clrBuf )		 delete [] clrBuf;
	if ( fbuf )			 delete [] fbuf;
	if ( w_ranString )	 delete [] w_ranString;
	if ( thinInvoiceNo ) delete [] thinInvoiceNo;
	if ( thinProgName )  delete [] thinProgName;
	if ( osVer )		 delete [] osVer;
	return	retVal;
}			// Subscribe()
//--------------------------------------------------------------------------------------------
short	CPNMX_InstallerDlg::License(
	wchar_t*		progName,
	char*			hashUUID,
	CButton&		licensedButton		)
{	DownloadCtx_T	downloadCtx;
	FILE*			fp = NULL;
	char*			key = NULL;
	char*			thinProgName = NULL;
	char*			thinInvoiceNo = NULL;
	char*			ranString = NULL;
	wchar_t*		w_ranString = NULL;
	short			retVal = 0;
	const			unsigned short WBUF_SIZE = 256;
	wchar_t			wbuf[WBUF_SIZE];

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// DownloadURL uses:  ctx->c_StatusResult, ctx->progName, ctx->serverName, ctx->urlRequest, ctx->fileName
	downloadCtx.c_StatusResult = &c_StatusResult;				// use the general status line ?
	downloadCtx.expectedFileSize = 0;
	wcsncpy_s( downloadCtx.progName, DwnldCtxProgNameSize, progName, DwnldCtxProgNameSize );
	strncpy_s( downloadCtx.serverName, DwnldCtxServerNameSize, lpServerName, DwnldCtxServerNameSize );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// build the urlRequest
	char* osVer = GetThinString( m_OsVersion );
	if ( osVer == NULL )			{	retVal = -1;	goto Exit;	}
	thinProgName = GetThinString( progName );
	if ( thinProgName == NULL )		{	retVal = -2;	goto Exit;	}
	thinInvoiceNo = GetThinString( m_InvoiceNumber );
	if ( thinInvoiceNo == NULL )	{	retVal = -3;	goto Exit;	}

	const unsigned short	BUF_SIZE = 1024;								// should be overkill
#ifdef _DEBUG
	{	CString csUu( m_UUID );
		TRACE( _T("PNMX_InstallerDlg::License: uu='%s'\n"), csUu );
	}
#endif
	char	ibuf[ BUF_SIZE ];
	sprintf_s( ibuf, BUF_SIZE, "?in=%s?pn=%s?os=%s?dv=%X?uu=%s?hu=%s",
		thinInvoiceNo, thinProgName, osVer, m_DeviceID,
		(m_UUID ? m_UUID : "0"), (hashUUID ? hashUUID : "0") );				// we should have the UUID and HUID by now
	if ( osVer )		 delete [] osVer;			osVer = NULL;			// we're done with osVer
	if ( thinProgName )  delete [] thinProgName;	thinProgName = NULL;	// ditto
	if ( thinInvoiceNo ) delete [] thinInvoiceNo;	thinInvoiceNo = NULL;
	size_t slen = strlen( ibuf );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// setup encryption
	const unsigned int LicenseDsCk[8] = { 0x3BFA6B70, 0xB7828174, 0x11A216FB, 0x88F14AA3,
										  0xA92D1A25, 0xDDDC6331, 0xDFFEE4EF, 0x5172952F  };
	unsigned int rcrk[ nRcRoundKeys ];										// 44 round keys in RC6
	rcKeyExpansion( (unsigned int *)LicenseDsCk, rcrk );

	unsigned char obuf[ BUF_SIZE ];
	unsigned int LicenseDsIv[8] = { 0x9FCDDF8F, 0x61B3A81D, 0xE2A18B9B, 0x6BEBDCF1,
									0x1A4D6923, 0x6D0094CC, 0xD3601547, 0xEBECDDED  };
	rcCrypt( (unsigned char*)LicenseDsIv, (unsigned char*)ibuf, (unsigned long)slen, obuf, rcrk );
	*(obuf+slen) = '\0';													// for nicer debugger display

		// MIME encode obuf
	unsigned char mimeBuf[ BUF_SIZE * 4/3 ];								// mime bufsize = BUF_SIZE * 4/3
	unsigned long mimeBufLen = MimeEncode( obuf, (unsigned long)slen, mimeBuf );
	sprintf_s( downloadCtx.urlRequest, DwnldCtxUrlRequestSize, "%s/License.php?ds=%s", ServerPagePrefix, mimeBuf );
#ifdef _DEBUG
	{	wchar_t*	ws = MakeWideString( downloadCtx.urlRequest );
		TRACE( _T("PNMX_InstallerDlg::License:urlRequest: %s\n"), ws );
		delete [] ws;
	}
#endif
		// end of request encryption
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// generate a desktop filename for the download
	ranString = MakeRanName();
	if ( ranString == NULL )
	{	swprintf_s( wbuf, WBUF_SIZE, _T(" %s: desk memory alloc failed"), progName );
		c_StatusResult.SetWindowTextW( wbuf );
		retVal = -4;
		goto	Exit;
	}

	w_ranString = MakeWideString( ranString );
	if ( w_ranString == NULL )
	{	swprintf_s( wbuf, WBUF_SIZE, _T(" %s: desk memory alloc failed"), progName );
		c_StatusResult.SetWindowTextW( wbuf );
		retVal = -5;
		goto	Exit;
	}
	swprintf_s( downloadCtx.fileName, DwnldCtxFileNameSize, _T("%s\\%s"), m_DeskTempPath, w_ranString );	// where the download goes on the dektop machine
	DWORD res = DownloadURL( &downloadCtx );
	if ( res != 0 )
	{	swprintf_s( wbuf, WBUF_SIZE, _T(" %s: license download failed -> %d"), progName, (short)res );
		c_StatusResult.SetWindowTextW( wbuf );
		c_FetchButton.EnableWindow( FALSE );
		retVal = -6;
		goto	Exit;
	}
	swprintf_s( wbuf, WBUF_SIZE, _T(" %s: license downloaded"), progName );
	c_StatusResult.SetWindowTextW( wbuf );

		// open and read the license
	errno_t err = _wfopen_s( &fp, downloadCtx.fileName, _T("rb") );
	if ( err != 0 )		{	retVal = -5;	goto	Exit;	}
	size_t	flen = _filelength( _fileno(fp) );

	key = new char[ flen + 1 ];
	if ( key == NULL )	{	retVal = -6;	goto	Exit;	}

	size_t lres = fread( key, sizeof(char), flen, fp );
	if ( lres < flen )	{	retVal = -7;	goto	Exit;	}
	*(key+flen) = '\0';							// we won't write the terminator to the registry
	lres = fclose( fp );						// reuse lres as a return value
	fp = NULL;
	
	short sres = DeleteDesktopFile( progName, downloadCtx.fileName );
	swprintf_s( wbuf, WBUF_SIZE, (( res == 0 ) ? _T(" %s: deskTemp cleanup OK")
											   : _T(" %s: deskTemp cleanup failed")), progName );
	c_StatusResult.SetWindowTextW( wbuf );

	swprintf_s( wbuf, WBUF_SIZE, _T("SOFTWARE\\PocketNumerix\\%s"), progName );
	sres = RegisterKeyValuePair( HKEY_LOCAL_MACHINE, wbuf, REG_BINARY, (unsigned short)(sizeof(char)*flen), (void*)key );
	if ( sres < 0 )		{	retVal = -8;	goto	Exit;	}

		// not sure whether the RAPI DeviceID is unique or not, but ...
		// we encrypted the RAPI DeviceID into the software license and it's not obtainable any other way
//	sres = RegisterKeyValuePair( HKEY_LOCAL_MACHINE, wbuf, REG_BINARY, sizeof(int), (void*)m_DeviceID );
//	if ( sres < 0 )		{	retVal = -9;	goto	Exit;	}

	swprintf_s( wbuf, WBUF_SIZE, _T(" %s: licensed"), progName );
	c_StatusResult.SetWindowTextW( wbuf );

	licensedButton.SetCheck( BST_CHECKED );
	licensedButton.ShowWindow( SW_SHOWNOACTIVATE );
Exit:
	if ( fp )			 fclose( fp );
	if ( osVer )		 delete [] osVer;			osVer = NULL;			// we're done with osVer
	if ( thinProgName )  delete [] thinProgName;	thinProgName = NULL;	// ditto
	if ( thinInvoiceNo ) delete [] thinInvoiceNo;	thinInvoiceNo = NULL;
	if ( ranString )	 delete [] ranString;
	if ( w_ranString )	 delete [] w_ranString;
	if ( key )			 delete [] key;
	return	retVal;
}			// License()
//--------------------------------------------------------------------------------------------
short	CPNMX_InstallerDlg::DownUp(
//	char*					ranString,					// generate the ranString internally
	wchar_t*				progName,
	char*					hashUUID,
	wchar_t*				testPath,
	unsigned long			expectedFileSize,
	CButton&				downloadedButton,
	CButton&				uploadedButton,
	CButton&				installedButton,
	DWORD					memReq,
	unsigned char*			mimeSig,
	wchar_t*				deviceFileName				)
{	const unsigned short	WBUF_SIZE = 256;
	wchar_t					wbuf[ WBUF_SIZE ];
	short					retVal = 0;
	CString					cs;

		// how about checking available memory first???
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// is there enough memory to install the software on the BOM
		// need 3x to accommodate the executable, plus some space for the results
/*
	long remainingMem = p_PID->m_MemoryAvail - p_PID->m_MemoryRequired - p_PID->m_PpcInstallMemReq;
	if ( remainingMem <= 65535 )			// comfort margin
	{	p_PID->c_StatusResult.SetWindowTextW( _T(" Insufficient device memory to install products") );
		return	-1;
	}
*/
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// DownloadURL uses:  ctx->c_StatusResult, ctx->progName, ctx->serverName, ctx->urlRequest, ctx->fileName
	DownloadCtx_T	downloadCtx;
	downloadCtx.c_StatusResult = &c_DownloadStatusResult;
	downloadCtx.expectedFileSize = expectedFileSize;
	wcsncpy_s( downloadCtx.progName, DwnldCtxProgNameSize, progName, DwnldCtxProgNameSize );
	strncpy_s( downloadCtx.serverName, DwnldCtxServerNameSize, lpServerName, DwnldCtxServerNameSize );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// build the urlRequest
	char* osVer = GetThinString( m_OsVersion );
	cs = progName;
	int pos = cs.Find( _T("DLLs") );
	if ( pos >= 0 )									// if true, we're installing the MFC 8.0 DLLs...
		cs = cs.Mid( pos );							// clip off anything in front of DLLs
	char* thinProgName = GetThinString( cs );
	char* thinInvoiceNo = GetThinString( m_InvoiceNumber );

	const unsigned short	BUF_SIZE = 1024;						// should be overkill
	char	ibuf[ BUF_SIZE ];
	sprintf_s( ibuf, BUF_SIZE, "?in=%s?pn=%s?os=%s?dv=%X?uu=%s?hu=%s",
		thinInvoiceNo, thinProgName, osVer,m_DeviceID,
		(m_UUID ? m_UUID : "0"), (hashUUID ? hashUUID : "0") );		// PpcInstall gathers uuid and huid, so they could be unknown here
#ifdef _DEBUG
	{	CString cs( ibuf );
		TRACE( _T("PNMX_InstallerDlg::DownUp: ibuf=%s\n"), cs );
	}
#endif
	if ( osVer ) delete [] osVer;									// we're done with osVer
	if ( thinProgName ) delete [] thinProgName;						// ditto
	if ( thinInvoiceNo ) delete [] thinInvoiceNo;
	size_t slen = strlen( ibuf );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// setup encryption
	const unsigned int RequestDsCk[8] = { 0xF1CC7A8C, 0x937EB438, 0xD9A3BFD1, 0x0BCF0906,
										  0x4B7095D2, 0x7F43E1BD, 0x7457BEA4, 0x96EEAB1F  };
	unsigned int rcrk[ nRcRoundKeys ];								// 44 round keys in RC6
	rcKeyExpansion( (unsigned int *)RequestDsCk, rcrk );

	unsigned char obuf[ BUF_SIZE ];
	unsigned int RequestDsIv[8] = { 0x0F7CB802, 0xC67C6887, 0x0EBAAF2C, 0xE2D78673,
									0xB5D5C702, 0xC3E8D907, 0xCBDA2CC4, 0x14E474A5  };
	rcCrypt( (unsigned char*)RequestDsIv, (unsigned char*)ibuf, (unsigned long)slen, obuf, rcrk );
	*(obuf+slen) = '\0';											// for nicer debugger display

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// MIME encode obuf
	unsigned char mimeBuf[ BUF_SIZE * 4/3 ];						// mime bufsize = BUF_SIZE * 4/3
	unsigned long mimeBufLen = MimeEncode( obuf, (unsigned long)slen, mimeBuf );
// was ...	sprintf_s( downloadCtx.urlRequest, DwnldCtxUrlRequestSize, "/Downloads/%s/%s/%s", thinDate, m_RanDir, ranString );
	sprintf_s( downloadCtx.urlRequest, DwnldCtxUrlRequestSize, "%s/Request.php?ds=%s", ServerPagePrefix, mimeBuf );
#ifdef _DEBUG
	{	CString cs( downloadCtx.urlRequest );
		TRACE( _T("PNMX_InstallerDlg::DownUp: urlRequest=%s\n"), cs );
	}
#endif

		// get the binary
//	downloadCtx.c_Button = NULL;
	downloadCtx.c_StatusResult = &c_DownloadStatusResult;
		// end of request encryption

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	wchar_t* w_ranString = NULL;
	char* ranString = MakeRanName();
	if ( ranString == NULL )
	{	CString cs = progName;
		swprintf_s( wbuf, WBUF_SIZE, _T(" %s: desk memory alloc failed"), cs );
		c_StatusResult.SetWindowTextW( wbuf );
		retVal = -4;
		goto	Exit;
	}
	w_ranString = MakeWideString( ranString );
	if ( w_ranString == NULL )
	{	CString cs = progName;
		swprintf_s( wbuf, WBUF_SIZE, _T(" %s: desk memory alloc failed"), cs );
		c_StatusResult.SetWindowTextW( wbuf );
		retVal = -5;
		goto	Exit;
	}
	delete [] ranString;
	swprintf_s( downloadCtx.fileName, DwnldCtxFileNameSize, _T("%s\\%s"), m_DeskTempPath, w_ranString );	// where the download goes on the dektop machine
	DWORD dres = DownloadURL( &downloadCtx );
	if ( dres != 0 )
	{	swprintf_s( wbuf, WBUF_SIZE, _T(" %s: download failed -> %d"), progName, dres );
		c_StatusResult.SetWindowTextW( wbuf );
		c_FetchButton.EnableWindow( FALSE );
		retVal = -6;
		goto	Exit;
	}
	swprintf_s( wbuf, WBUF_SIZE, _T(" %s: downloaded"), progName );
	c_StatusResult.SetWindowTextW( wbuf );
	downloadedButton.SetCheck( BST_CHECKED );
	downloadedButton.ShowWindow( SW_SHOWNOACTIVATE );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// upload to the device
	FILETIME UploadCreateTime;
	short	sres = DeviceUpload( (void*)this, progName, downloadCtx.fileName, mimeSig, expectedFileSize,
								 deviceFileName, &c_UploadStatusResult, &UploadCreateTime );
	bool UnknownUploadCreationTime = ( sres == -6 );
	if ( sres < 0  &&  ! UnknownUploadCreationTime )		// -6 means we couldn't get the file creation time
	{	swprintf_s( wbuf, WBUF_SIZE, _T(" %s: upload failed -> %d"), progName, sres );
		c_UploadStatusResult.SetWindowTextW( wbuf );
		c_StatusResult.SetWindowTextW( wbuf );
		retVal = -7;
		goto	Exit;
	}
	swprintf_s( wbuf, WBUF_SIZE, _T(" %s: uploaded"), progName );
	c_StatusResult.SetWindowTextW( wbuf );

		// verify that the upload file is present
	CE_FIND_DATA	findData;
	HANDLE handle = CeFindFirstFile( deviceFileName, &findData );
	if ( handle == INVALID_HANDLE_VALUE )
	{	swprintf_s( wbuf, WBUF_SIZE, _T(" %s: upload verify failed"), progName );
		c_UploadStatusResult.SetWindowTextW( wbuf );
		c_StatusResult.SetWindowTextW( wbuf );
		retVal = -8;
		goto	Exit;
	}
	BOOL B1 = CeFindClose( handle );				// do nothing if it doesn't succeed
//	CeCloseHandle( handle );

		// did we really crerate a new file?
	if ( ! UnknownUploadCreationTime  &&											// upload creation time is known
		 CompareFileTime( &findData.ftCreationTime, &UploadCreateTime ) != -1 )		// file found is not older than uploaded file
	{	swprintf_s( wbuf, WBUF_SIZE, _T(" %s: upload verified"), progName );
		c_UploadStatusResult.SetWindowTextW( wbuf );
		uploadedButton.SetCheck( BST_CHECKED );
		uploadedButton.ShowWindow( SW_SHOWNOACTIVATE );
	}
		// cleanup the DeskTemp file
	sres = DeleteDesktopFile( progName, downloadCtx.fileName );			// the deskTempFile...  0 --> failure
	swprintf_s( wbuf, WBUF_SIZE, (( sres == 0 ) ? _T(" %s: deskTemp cleanup OK")
											    : _T(" %s: deskTemp cleanup failed")), progName );
	c_StatusResult.SetWindowTextW( wbuf );

		// update memory required and available
	GetMemoryAvail();											// updates the GUI too
	m_MemoryRequired -= 3 * memReq;					// used to be in GetBinary
	FormatMemStr( m_MemoryRequired, wbuf, WBUF_SIZE );
	c_MemoryRequiredResult.SetWindowTextW( wbuf );

Exit:
	if ( ranString )
		delete [] w_ranString;
	return	retVal;
}			// DownUp()
//--------------------------------------------------------------------------------------------
short	CPNMX_InstallerDlg::DownUpInstall(
//	char*					ranString,						// generate the ranString internally
	wchar_t*				progName,
	char*					hashUUID,
	wchar_t*				testPath,
	unsigned long			expectedFileSize,
	CButton&				downloadedButton,
	CButton&				uploadedButton,
	CButton&				installedButton,
	DWORD					memReq,
	unsigned char*			mimeSig,
	wchar_t*				cabName,
	wchar_t*				devPath	/* = NULL */	)		// if NULL, we'll make up a random name
{	short					retVal = 0;
	const unsigned short	WBUF_SIZE = 256;
	wchar_t					wbuf[ WBUF_SIZE ];
	char*					ranString = NULL;

		// generate a desktop filename for the download
	wchar_t	deviceFileName[ 256 ];
	if ( devPath == NULL )	
	{	ranString = MakeRanName();
		if ( ranString == NULL )
		{	CString cs = progName;
			swprintf_s( wbuf, WBUF_SIZE, _T(" %s: device memory alloc failed"), cs );
			c_StatusResult.SetWindowTextW( wbuf );
			retVal = -1;
			goto	Exit;
		}
		wchar_t*	w_ranString = MakeWideString( ranString );
		if ( w_ranString == NULL )
		{	CString cs = progName;
			swprintf_s( wbuf, WBUF_SIZE, _T(" %s: device memory alloc failed"), cs );
			c_StatusResult.SetWindowTextW( wbuf );
			retVal = -2;
			goto	Exit;
		}
		swprintf_s( deviceFileName, 256, _T("%s\\%s.CAB"), m_DeviceTempPath, w_ranString );
		delete [] w_ranString;
	}
	else
		wcsncpy_s( deviceFileName, 256, devPath, _TRUNCATE );

	retVal = DownUp( progName, hashUUID, testPath, expectedFileSize,
					 downloadedButton, uploadedButton, installedButton,
					 memReq, mimeSig, deviceFileName );
	if ( retVal < 0 )
		goto	Exit;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// Install - run wceload.exe on the device filename
	LoadApp( progName, cabName, deviceFileName );

		// Verify that <testPath> is present
		// if the installed exe exists, and the RanString.CAB is still in /Temp/PocketNumerix
		// then, delete the CAB from /Temp/PocketNumerix
	CE_FIND_DATA	findData;
	HANDLE			handle;
	for ( short cnt = 50; cnt > 0; cnt-- )
	{	handle = CeFindFirstFile( testPath, &findData );
		swprintf( wbuf, WBUF_SIZE, _T(" %s: %s handle(%d) ==> %ld"), progName, testPath, cnt, handle );
		c_StatusResult.SetWindowTextW( wbuf );
		if ( handle != INVALID_HANDLE_VALUE )
			break;
		Sleep( 200 );
	}

	if ( handle == INVALID_HANDLE_VALUE )
	{	swprintf_s( wbuf, WBUF_SIZE, _T(" %s: install verify failed"), progName );
		c_InstallStatusResult.SetWindowTextW( wbuf );
		c_StatusResult.SetWindowTextW( wbuf );
		retVal = -3;
		goto	Exit;
	}

		// <progName> installed successfully
	BOOL B1 = CeFindClose( handle );				// do nothing if it doesn't succeed
//	BOOL B1 = CeCloseHandle( handle );				// zero indicates failure

		// discovered wceload /delete option, so don't need to do this
		// ensure that the CAB file was deleted (automatically wiped out by wceload.exe on PPC'02)
//	if ( m_OsMajVerNum > 4 )										// (i.e. only for WM5)
//		sres = DeleteDeviceFile( progName, deviceFileName );		// the CAB file

	swprintf_s( wbuf, WBUF_SIZE, _T(" %s: install verified"), progName );
	c_InstallStatusResult.SetWindowTextW( wbuf );
	installedButton.SetCheck( BST_CHECKED );
	installedButton.ShowWindow( SW_SHOWNOACTIVATE );
	swprintf_s( wbuf, WBUF_SIZE, _T(" %s: complete"), progName );
	c_StatusResult.SetWindowTextW( wbuf );						// clear the general status area

Exit:
	if ( ranString )
		delete [] ranString;
	return	retVal;
}			// DownUpInstall()
//------------------------------------------------------------------------------------
short CPNMX_InstallerDlg::DeleteDeviceFile( wchar_t* progName, wchar_t* fullPathName )
{	CE_FIND_DATA findData;
	HANDLE handle = INVALID_HANDLE_VALUE;

	const unsigned short sleepMicroseconds = 200;
	wchar_t msgBuf[ 240 ];
	BOOL B1 = 0;
	for ( short cnt = 50; cnt > 0; cnt-- )
	{	handle = CeFindFirstFile( fullPathName, &findData );
		if ( handle == INVALID_HANDLE_VALUE )
			return	0;							// 0 ==> success
		B1 = CeFindClose( handle );				// do nothing if it doesn't succeed
		B1 = CeDeleteFile( fullPathName );		// 0 ==> failure
		if ( B1 )
			return	0;							// 0 ==> success
//		swprintf_s( msgBuf, 240, _T(" %s: Deleting(%d) %s"), progName, cnt, fullPathName );
		swprintf_s( msgBuf, 240, _T(" %s: Deleting temp file(%d)"), progName, cnt );
		c_StatusResult.SetWindowTextW( msgBuf );
		Sleep( sleepMicroseconds );
	}
#ifdef _DEBUG
	TRACE( _T("PNMX_InstallerDlg::DeleteDeviceFile: '%s'\n"), fullPathName );
#endif
	return	-1;									// 0 ==> success
}			// DeleteDeviceFile()
//------------------------------------------------------------------------------------
short	CPNMX_InstallerDlg::DeleteDesktopFile( wchar_t* progName, wchar_t* T_path )
{
	BOOL	B1 = DeleteFile( T_path );			// 0 ==> failure
#ifdef _DEBUG
	if ( ! B1 )
	{	wchar_t	MsgBuf[128];
		DWORD msgID = GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			0, msgID, 0, (LPTSTR)&MsgBuf, 127, NULL );
		TRACE( _T("PNMX_InstallerDlg::DeleteDesktopFile: %d --> '%s'\n"), msgID, MsgBuf );
	}
#endif
	wchar_t msgBuf[ 300 ];
//	swprintf_s( msgBuf, 300, _T(" %s: Deleting %s"), progName, T_path );
	swprintf_s( msgBuf, 300, _T(" %s: Deleting desk temp file"), progName );
	c_StatusResult.SetWindowTextW( msgBuf );
	return	( B1 )  ?  0  :  -1;			// we think of 0 as success
}			// DeleteDesktopFile()
//------------------------------------------------------------------------------------
void	CPNMX_InstallerDlg::LoadApp( wchar_t* progName, wchar_t* cabName, wchar_t* deviceFileName )
{	PROCESS_INFORMATION	procInfo;
	const unsigned short wBufSize = 256;
	wchar_t		args[ wBufSize ];
	wchar_t		exePath[ MAX_PATH ];

		// eliminate evidence that the app was previously installed
		// by deleting the HKLM\Software\Apps\PocketNumerix <progName> registry key
		// if it already exists
	long	res = UnloadApp( progName, cabName );
	swprintf_s( args, wBufSize, _T(" %s: UnloadApp --> %d"), progName, res );
	c_InstallStatusResult.SetWindowTextW( args );

	swprintf_s( args, wBufSize, _T("i %s"), deviceFileName );
	swprintf_s( exePath, MAX_PATH, _T("%s\\PpcInstall.exe"), m_DeviceTempPath );
#ifdef _DEBUG
	TRACE( _T("PNMX_InstallerDlg::LoadApp: progname=%s, deviceFileName=%s\n"),
			progName, deviceFileName );
#endif
	short sres2 = Execute( exePath, args, &procInfo );

		// reuse args as a message buffer
	swprintf_s( args, wBufSize, ((sres2 == 0) ? _T(" %s: PpcInstall complete")
											   : _T(" %s: PpcInstall failed")), progName );
	c_InstallStatusResult.SetWindowTextW( args );
}			// LoadApp()
//------------------------------------------------------------------------------------
long	CPNMX_InstallerDlg::UnloadApp( wchar_t* progName, wchar_t* cabName )
{	const unsigned short	wBufSize = 256;
	wchar_t					keyName[ wBufSize ];
	long					retVal;
	HKEY					hKey = NULL;
/*
		// WM5's unload keys are located at HKLM\Security\AppInstall\PocketNumerix <progName>\Uninstall
		// open the key
	swprintf_s( keyName, wBufSize, _T("\\Security\\AppInstall\\PocketNumerix %s"), progName );
	long res = CeRegOpenKeyEx( HKEY_LOCAL_MACHINE, keyName, 0, 0, &hKey );
	if ( res != ERROR_SUCCESS )
	{	retVal = -1;				// occurs when product hasn't been installed before
		goto	Exit;
	}
		// read the key value
	DWORD	cbData = wBufSize * sizeof(wchar_t);
	DWORD	type = REG_SZ;
		// reusing keyName to capture the value
	res = CeRegQueryValueEx( hKey, _T("Uninstall"), NULL, &type, (LPBYTE)keyName, &cbData );
	if ( res != ERROR_SUCCESS )
	{	retVal = -2;				// if the key exists, this value should exist
		goto	Exit;
	}
		// at this point we know an unload XML file exists
*/
		// we're only going to address the breadcrumbs wceload uses in PPC03, namely:
		// HKLM\SOFTWARE\Apps\PocketNumerix <progName>
	if ( m_OsMajVerNum == 4 )
	{	swprintf_s( keyName, wBufSize, _T("\\SOFTWARE\\Apps\\PocketNumerix %s"), cabName );
		retVal = CeRegDeleteKey( HKEY_LOCAL_MACHINE, keyName );		// if it's not there, that's okay too
	}
//Exit:
	return	retVal;
}			// UnloadApp()
//------------------------------------------------------------------------------------
short	CPNMX_InstallerDlg::Execute( wchar_t* T_path, wchar_t* T_cmdLine, PROCESS_INFORMATION* procInfo )
{		// second argument contains command line options
		//  3) process attributes NULL
		//  4) thread attributes NULL
		//  5) inherit handles FALSE
		//  6) creation flags - there are possible values, but we don't need anything special
		//  7) environment NULL
		//  8) current directory NULL
		//  9) startup info NULL
		// 10) process info structure pointer
	BOOL	B1 = CeCreateProcess( T_path, T_cmdLine, NULL, NULL, FALSE,		// 1: appName; 2:commandLine
								  0, NULL, NULL, NULL, procInfo );			// 0 ==> failure
	if ( B1 == 0 )
	{	DWORD msgID = CeGetLastError();
#ifdef _DEBUG
		wchar_t	MsgBuf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			0, msgID, 0, (LPTSTR)&MsgBuf, 127, NULL );
		TRACE( _T("PNMX_InstallerDlg::Execute: CeCreateProcess(%s %s) --> %d:'%s'\n"),
				T_path, T_cmdLine, msgID, MsgBuf );
#endif
	}
	return	( B1 )  ?  0  :  -1;			// we think of 0 as success
}			// Execute()
//------------------------------------------------------------------------------------
/*		// prepEpilog calls from LoadApp()
	short sres = WceloadPrepEpilogue( progName, true );		// capture "Microsoft Application Installer" registry settings
#ifdef _DEBUG
	if ( sres < 0 )
		TRACE( _T("PNMX_InstallerDlg::Load: WceloadPrepEpilogue(%s, true) --> %d\n"), progName, sres );
#endif
*/
/*
	sres = WceloadPrepEpilogue( progName, false );		// restore "Microsoft Application Installer" registry settings
#ifdef _DEBUG
	if ( sres < 0 )
		TRACE( _T("PNMX_InstallerDlg::Load: WceloadPrepEpilogue(%s, false) --> %d\n"), progName, sres );
#endif
*/

//------------------------------------------------------------------------------------
/*
short	CPNMX_InstallerDlg::WceloadPrepEpilogue( wchar_t* progName, bool capture )
{		// Note: caller is responsible for calling first using false
		//		 (to initialize static storage) and subsequently using true
	HKEY		hKey;
	short		retVal = 0;
		// first reset the Instl key for <providerName appName>
	wchar_t*	baseKeyName = _T("Software\\Apps\\Microsoft Application Installer");
	long res = CeRegOpenKeyEx( HKEY_LOCAL_MACHINE, baseKeyName, 0, 0, &hKey );
	if ( res != ERROR_SUCCESS )
	{	retVal = -1;					// if we can't open the key, there's not much we can do
		goto	Exit;
	}
	DWORD size = sizeof( DWORD );
	DWORD resetValue = 0;				// only used when restore is false
	const unsigned short nKeys = 4;
	static DWORD keyValue[ nKeys ];
	const wchar_t*	keyName[ nKeys ] = { _T("fEnableCef"), _T("nDynamicDelete"), _T("fAskDest"), _T("fAskOptions") };
	for ( short ii = 0; ii < nKeys; ii++ )
	{	if ( capture )
		{		// i.e. we're capturing keyValue[ii]
			DWORD	type;
			res = CeRegQueryValueEx( hKey, *(keyName+ii), 0, &type, (LPBYTE)(keyValue+ii), (LPDWORD)&size );
			if ( res != ERROR_SUCCESS )
			{	retVal = -2 - ii;			// the value couldn't be captured --> -2, -3, -4, -5
				goto	Exit;
			}
		}
			// restore value captured in the previous call, or reset after sampling above
			// when restore is false, we captured keyValue[ii] above, so we're resetting here
			// when restore is true, setValue is the static keyValue[ii] captured (above) in a previous call
		if ( *(keyValue+ii) != resetValue )
		{		// mode -->			   capture			restore
			DWORD*	pValue = ( capture )  ?  &resetValue  :  (keyValue+ii);
			res = CeRegSetValueEx( hKey, *(keyName+ii), 0, REG_DWORD, (LPBYTE)pValue, sizeof(DWORD) );
			if ( res != ERROR_SUCCESS )
			{	retVal = -6 - ii;				// the value couldn't be reset --> -6, -7, -8, -9
				goto	Exit;
			}
		}
	}
Exit:
	CeRegCloseKey( hKey );
#ifdef _DEBUG
	if ( retVal < 0 )
		TRACE( _T("PNMX_InstallerDlg::WceloadPrepEpilogue: returning %d\n"), retVal );
#endif
	return	retVal;
}			// WceloadPrepEpilogue()
*/
//------------------------------------------------------------------------------------
/*
short	CPNMX_InstallerDlg::UnloadApp( CString appName )
{	PROCESS_INFORMATION	procInfo;
	wchar_t		wbuf[ 128 ];
	swprintf( wbuf, _T("PocketNumerix %s"), appName );
	return	Execute( _T("unload.exe"), wbuf, &procInfo );
}			// UnloadApp()
*/
//------------------------------------------------------------------------------------
/*
short	CPNMX_InstallerDlg::DeviceUpload( wchar_t* deskFile, wchar_t* deviceFile, wchar_t* progName )
{	
	FILE*	fpIn  = NULL;
	HANDLE	h_Out = NULL;
	short	retVal = 0;

		//	start moving bytes...
	errno_t err = _wfopen_s( &fpIn, deskFile, _T("rb") );
		// filename, access, sharedMode, security, creation, flags, template
	h_Out = CeCreateFile( deviceFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
									CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	long	bytesUploaded = 0;
	const short MSG_BUF_SIZE = 80;
	wchar_t	msgBuf[ MSG_BUF_SIZE ];
	if ( fpIn  &&  h_Out )
	{	const short	BUF_SIZE = 4096;
		char	buf[BUF_SIZE];
		int	bytesAvail = 1;
		do
		{	bytesAvail = fread( buf, 1, BUF_SIZE, fpIn );
			if ( bytesAvail <= 0 )
				break;
				// zero indicates failure
			DWORD	bytesWritten;
			BOOL	B1 = CeWriteFile( h_Out, buf, bytesAvail, &bytesWritten, NULL );
			bytesUploaded += bytesWritten;
			swprintf( msgBuf, MSG_BUF_SIZE, _T(" %s: %d bytes uploaded."),
						progName, bytesUploaded  );
			c_StatusResult.SetWindowTextW( msgBuf );
		} while ( 1 );
	}

	if ( fpIn  ) fclose( fpIn  );
	if ( h_Out ) CeCloseHandle( h_Out );
	if ( retVal < 0 )
		return	retVal;						// token extraction error exit
	return	retVal;							// we think of 0 as success
}			// DeviceUpload()
*/
//------------------------------------------------------------------------------------
/*
short	CPNMX_InstallerDlg::Delete( char* commandBuffer, int& ii )
{
	short	retVal = 0;
	wchar_t*	T_token = NULL;

	char*	token = GetToken( commandBuffer, ii );
	if ( token == NULL )
		return	-1;

	T_token = MakeWideString( token );
	if ( T_token == NULL )	{	retVal = -2;	goto	Exit;	}

	BOOL	B1 = CeDeleteFile( T_token );		// 0 ==> failure
Exit:
	if ( token ) delete [] token;
	if ( T_token ) delete [] T_token;
	if ( retVal < 0 )
		return	retVal;
	return	( B1 )  ?  0  :  -3;			// we think of 0 as success
}			// Delete()
*/
//------------------------------------------------------------------------------------
/*
// assumes that theApp.m_DeskTempPath and theApp.m_DeviceTempPath are already defined
short	CPNMX_InstallerDlg::Move( void )
{	char*		tgt = NULL;
	wchar_t*	T_src = NULL;
	wchar_t*	T_tgt = NULL;
	short		retVal = 0;
	char* src = GetToken( m_CommandBuffer, m_cbo );
	if ( src == NULL )		return	-1;

	tgt = GetToken( m_CommandBuffer, m_cbo );
	if ( tgt == NULL )		{	retVal = -2;	goto	Exit;	}

	T_src = MakeWideString( src );
	if ( T_src == NULL )	{	retVal = -3;	goto	Exit;	}

	T_tgt = MakeWideString( tgt );
	if ( T_tgt == NULL )	{	retVal = -4;	goto	Exit;	}

	BOOL	B1 = CeMoveFile( T_src, T_tgt );
Exit:
	if (   src ) delete []   src;
	if (   tgt ) delete []   tgt;
	if ( T_src ) delete [] T_src;
	if ( T_tgt ) delete [] T_tgt;
	if ( retVal < 0 )
		return	retVal;						// token extraction error exit
	return	( B1 )  ?  0  :  -5;			// we think of 0 as success
}			// Move()
*/
//------------------------------------------------------------------------------------
/*
short	CPNMX_InstallerDlg::Register( void )
{		// read /Temp/PocketNumerix/UUID.dat, run Register.php with the contents
	wchar_t*	T_token = NULL;
	wchar_t*	T_fName = NULL;
	char*		buf = NULL;
	HANDLE		h_UUIDfile = NULL;
	short		retVal = 0;

	char*	token = GetToken( m_CommandBuffer, m_cbo );
	if ( token == NULL )		return	-1;

	T_token = MakeWideString( token );
	if ( T_token == NULL )		{	retVal = -2;	goto	Exit;	}

	DWORD	len = wcslen( T_token )
				+ wcslen( m_DeviceTempPath )
				+ 15;					// strlen( "\PocketNumerix\" );
	T_fName = new wchar_t[ len + 1 ];
	swprintf( T_fName, len, _T("%s\\PocketNumerix\\%s"), m_DeviceTempPath, T_token );

		// filename, access, sharedMode, security, creation, flags, template
	h_UUIDfile = CeCreateFile( T_fName, GENERIC_READ, FILE_SHARE_READ, NULL,
									OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( h_UUIDfile == NULL )	{	retVal = -3;	goto	Exit;	}

	DWORD	res = CeGetFileSize( h_UUIDfile, &len );
	if ( res != NO_ERROR )		{	retVal = -4;	goto	Exit;	}

	buf = new char[ len + 1 ];
	if ( buf == NULL )			{	retVal = -5;	goto	Exit;	}

	DWORD	bytesRead;
	BOOL	B1 = CeReadFile( h_UUIDfile, buf, len, &bytesRead, NULL );
	if ( B1 == 0 )				{	retVal = -6;	goto	Exit;	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// launch a download thread and wait for the result
	int day = m_InvoiceDate.GetDay();
	int mon = m_InvoiceDate.GetMonth();
	int yr  = m_InvoiceDate.GetYear();
	char* leader = "/License.php?ver=";
		// believe that mySQL months go from 1 to 12
	sprintf( buf, "%s%s?dev=%d?first=%s?last=%s?email=%s?num=%d?date=%4d-%02d-%02d",
			leader, m_OsVersion, m_DeviceID, m_FirstName, m_LastName,
			m_EmailAddress, m_InvoiceNumber, yr, mon, day );
		// here's where we kick off a thread to do the download

//	len = strlen( buf );
//	urlRequest = new char[ len + 1 ];
//	strncpy( urlRequest, buf, len + 1 );

//	wchar_t	wbuf[ 256 ];
//	swprintf( wbuf, _T("%s\\PocketNumerix\\%s"), theApp.m_DeskTempPath, tgtFile );
//	slen = wcslen( wbuf );
//	if ( lpFileName ) delete [] lpFileName;
//	lpFileName = new wchar_t[ slen+1 ];
//	wcsncpy( lpFileName, wbuf, slen+1 );

DWORD	dwThreadId, dwExitCode = 0;
	HANDLE downloadThread = CreateThread( NULL, 0, DownloadURL, (void*)this, 0, &dwThreadId );
	if ( WaitForSingleObject( downloadThread, 600000 ) == WAIT_TIMEOUT )	// ten minutes
	{	c_StatusResult.SetWindowTextW( _T(" Download is stalled.") );
		retVal = -4;
	}
Exit:
	if ( token ) delete [] token;
	if ( T_token ) delete [] T_token;
	if ( T_fName ) delete [] T_fName;
	if ( buf )	 delete [] buf;
	if ( h_UUIDfile ) CeCloseHandle( h_UUIDfile );
	return	retVal;
}			// Register()
*/
//------------------------------------------------------------------------------------
/*
// #include, extern, KernelIoControl declaration, and the #define are
// from:  <http://www.pocketpcdn.com/articles/serial_number2002.html>
#include <WINIOCTL.H>
extern "C" __declspec(dllimport)

BOOL KernelIoControl( DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize,
					  LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned );

//#include <Windev.h>				// trying in vain to capture FILE_DEVICE_HAL withinin a desktop project
//#define FILE_DEVICE_HAL	0x101	// seems to be the consensus, though not needed when compiling for a PPC project
#define IOCTL_HAL_GET_DEVICEID	CTL_CODE( FILE_DEVICE_HAL, 21, METHOD_BUFFERED, FILE_ANY_ACCESS )

// the code is from http://64.41.105.202/forum/viewtopic.php?t=138&view=previous&sid=035c3fe1310d5e0f9fe80037765f71eb
void		CPNMX_InstallerDlg::GetSerialNumber( void )
{	DWORD		dwOutBytes;
	const int	nBuffSize = 80;
	deviceID = _T("");							// dialog instance var
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
		deviceID = strDeviceInfo.Mid(40,16)
					+ strDeviceInfo.Mid(56,4)
					+ strDeviceInfo.Mid(64,12);
//		MessageBox( strDeviceId, _T("UUID") );
#ifdef _DEBUG
		wprintf( _T("YieldCurveFitterDlg::GetSerialNumber: UUID=%s\n"), deviceID );
#endif
		LocalFree( arrOutBuff );
	}
}			// GetSerialNumber()
*/
//------------------------------------------------------------------------------
/*
short	CPNMX_InstallerDlg::ParseBOM( void )
{	wchar_t		deskTempFile[300];
	char*		memReq = NULL;
	char*		progName = NULL;
	char*		ranDir = NULL;
	char*		ranName = NULL;
	char*		BOMcache = NULL;
	short		retVal = 0;
	FILE*	fp = NULL;

	short	res = LoadBOMcache();			// should be superfluous
	if ( res < 0 )			{	retVal = -1;	goto	Exit;	}

		// get the ranDir
	int	ii = 0;
	ranDir = GetToken( m_BOMcache, ii );
	if ( ranDir == NULL )	{	retVal = -2;	goto	Exit;	}

		// get the memory required
	memReq = GetToken( m_BOMcache, ii );
	if ( memReq == NULL )	{	retVal = -3;	goto	Exit;	}

		// get the rest of the BOM entries
	swprintf( deskTempFile, 300, _T("%s\\PocketNumerix\\ToDo.dat"), m_DeskTempPath );
	fp = _wfopen( deskTempFile, _T("w") );
	if ( fp == NULL )		{	retVal = -4;	goto	Exit;	}

	fprintf( fp, "CheckSpace %s\n", memReq );		// update the GUI and possibly bail
	while ( 1 )
	{	int		progSize = 0;
		retVal = ParseBOMentry( ii, progName, progSize, ranName );
		if ( progName == NULL ) break;				// nothing more to read (retVal == -1)
		if ( retVal < 0 )	{	retVal = -5;	goto	Exit;	}

		if ( _stricmp( progName, "BondManager" ) == 0 )
		{	c_BondManager.EnableWindow( TRUE );
			c_BondManager.SetCheck( TRUE );
			m_BondManager = true;
		}
		else if ( _stricmp( progName, "NillaHedge" ) == 0 )
		{	c_NillaHedge.EnableWindow( TRUE );
			c_NillaHedge.SetCheck( TRUE );
			m_NillaHedge = true;
		}
		else if ( _stricmp( progName, "OptionChains" ) == 0 )
		{	c_OptionChains.EnableWindow( TRUE );
			c_OptionChains.SetCheck( TRUE );
			m_OptionChain = true;
		}
		else if ( _stricmp( progName, "StrategyExplorer" ) == 0 )
		{	c_StrategyExplorer.EnableWindow( TRUE );
			c_StrategyExplorer.SetCheck( TRUE );
			m_StrategyExplorer = true;
		}
		else if ( _stricmp( progName, "YieldCurveFitter" ) == 0 )
		{	c_YieldCurveFitter.EnableWindow( TRUE );
			c_YieldCurveFitter.SetCheck( TRUE );
			m_YieldCurveFitter = true;
		}
		else if ( _stricmp( progName, "TBillRates" ) == 0 )
		{	c_TBillRates.EnableWindow( TRUE );
			c_TBillRates.SetCheck( TRUE );
			m_TBillRates = true;
		}

			// we have a BOM entry in hand
		fprintf( fp, "WebDownload %s %s %s\n", ranDir, ranName, ranName );
		fprintf( fp, "DeviceUpload %s %s.cab\n", ranName, progName );
		fprintf( fp, "Execute %s\\PocketNumerix\\%s.cab\n", m_DeviceTempPath, progName );

		if ( _stricmp( progName, "UUID" ) == 0 )
		{	fprintf( fp, "Execute %s\\PocketNumerix\\UUID.exe\n", m_DeviceTempPath );
			fprintf( fp, "Register UUID\n" );			// assume: "\\Temp\\PocketNumerix\\ "
		}
	}
	long	flen = ftell( fp );
	fclose( fp );

		// suck the entire ToDo list into the m_CommandBuffer
//	swprintf( deskTempFile, _T("%s\\PocketNumerix\\ToDo.dat"), theApp.m_DeskTempPath );
	fp = _wfopen( deskTempFile, _T("r") );
	if ( m_CommandBuffer ) delete [] m_CommandBuffer;
	m_CommandBuffer = new char[ flen + 1 ];
	fread( m_CommandBuffer, 1, flen, fp );
	*(m_CommandBuffer+flen) = '\0';
	m_cbo = 0;

Exit:
	if ( fp ) fclose( fp );
	if ( retVal < 0  &&  m_CommandBuffer != NULL ) delete [] m_CommandBuffer;
	if ( memReq   ) delete [] memReq;
	if ( progName ) delete [] progName;
	if ( ranName  ) delete [] ranName;
	if ( ranDir   ) delete [] ranDir;
	return	retVal;
}			// ParseBOM()
*/
//------------------------------------------------------------------------------------
/*
short	CPNMX_InstallerDlg::ParseBOMentry( int& bomOffset, char*& progName, int& size, char*& ranName )
{		// return program name, size, and source filename (within the <ranDir>
	short	retVal = 0;
		// get the progName
	char*	theLine = GetToken( m_BOMcache, bomOffset, '\n' );
	if ( theLine == NULL )		return	-1;
	int		tt = 0;

		// avoid stepping on memory without releasing it first
	if ( progName ) delete [] progName;
	progName = GetToken( theLine, tt );
	if ( progName == NULL )
		retVal = -2;
	else
	{
		char* sizeSt = GetToken( theLine, tt );
		if ( sizeSt == NULL )
			retVal = -3;
		else
		{		// convert the sizeSt to an integer
			size = atoi( sizeSt );
			delete	sizeSt;

				// avoid stepping on memory without releasing it first
			if ( ranName ) delete [] ranName;
			ranName = GetToken( theLine, tt );
			if ( ranName == NULL )
				retVal = -4;
		}
	}
Exit:
	if ( theLine ) delete [] theLine;
	if ( retVal < -2  &&  progName ) delete [] progName;
	if ( retVal < -3  &&   ranName ) delete []  ranName;
	return	retVal;
}			// ParseBOMentry()
*/
//------------------------------------------------------------------------------------
/*
short	CPNMX_InstallerDlg::Dispatch( void )
{		// commands we understand:
//	WebDownload <dir> <srcF> <tgtF>		// pnmx.com/Downloads/			<ranDir> <srcF> <tgtF> 
//	ParseBOM <file>						// <deskTemp>					<BOMfile>
//	CheckSpace <memReq>					// space is in bytes			<memReq>
//	DeviceUpload <src> <tgt>			// <deskTemp>, <devTemp>		<srcF> <tgtF>		
//	Execute	<program>					// full <device> path
//	Register							// looks for <device>\\Temp\\PocketNumerix\\UUID.dat 
//	Move <srcF> <tgtF>
//	Delete <file>

	short	retVal = 0;
	char*	token = NULL;
	while ( token = GetToken( m_CommandBuffer, m_cbo ) )
	{	if ( _stricmp( token, "WebDownload") == 0 )
		{	if ( WebDownload() < 0 )	{	retVal = -1;	break;	}
		}
		else if ( _stricmp( token, "DeviceUpload") == 0 )
		{	if ( DeviceUpload() < 0 )	{	retVal = -2;	break;	}
		}
		else if ( _stricmp( token, "Execute") == 0 )
		{	if ( Execute() < 0 )		{	retVal = -3;	break;	}
		}
		else if ( _stricmp( token, "Register") == 0 )
		{	if ( Register() < 0 )		{	retVal = -4;	break;	}
		}
		else if ( _stricmp( token, "Move") == 0 )
		{	if ( Move() < 0 )			{	retVal = -5;	break;	}
		}
		else if ( _stricmp( token, "Delete") == 0 )
		{	if ( Delete() < 0 )			{	retVal = -6;	break;	}
		}
		if ( token ) delete [] token;
	}
Exit:
	if ( token ) delete [] token;
	return	retVal;
}			// Dispatch() commands on the ToDo list
*/
//------------------------------------------------------------------------------------
/*
short	CPNMX_InstallerDlg::WebDownload( void )
{
	char*	srcFile = NULL;
	char*	tgtFile = NULL;
	short	retVal = 0;
		// the next three tokens represent:
		// <ranDir>, <srcFile>, <tgtFile> as in:
		// download http://pnmx.com/Downloads/<ranDir>/<srcFile>
		//		to: <deskTemp>/PocketNumerix/<tgtFile>
	char*	ranDir = GetToken( m_CommandBuffer, m_cbo );
	if ( ranDir == NULL )	{	retVal = -1;		goto	Exit;	}

	srcFile = GetToken( m_CommandBuffer, m_cbo );
	if ( srcFile == NULL )	{	retVal = -2;		goto	Exit;	}

	tgtFile = GetToken( m_CommandBuffer, m_cbo );
	if ( tgtFile == NULL )	{	retVal = -3;		goto	Exit;	}

		// launch a download thread and wait for the result
	char	buf[ 256 ];
	sprintf( buf, "Downloads/%s/%s", ranDir, srcFile );
	short	slen = strlen( buf );
	if ( urlRequest ) delete [] urlRequest;
	urlRequest = new char[ slen+1 ];
	strncpy( urlRequest, buf, slen+1 );

	wchar_t	wbuf[ 256 ];
	swprintf( wbuf, 256, _T("%s\\PocketNumerix\\%s"), m_DeskTempPath, tgtFile );
	slen = wcslen( wbuf );
	if ( lpFileName ) delete [] lpFileName;
	lpFileName = new wchar_t[ slen+1 ];
	wcsncpy( lpFileName, wbuf, slen+1 );

	DWORD	dwThreadId, dwExitCode = 0;
	HANDLE downloadThread = CreateThread( NULL, 0, DownloadURL, (void*)this, 0, &dwThreadId );
	if ( WaitForSingleObject( downloadThread, 600000 ) == WAIT_TIMEOUT )	// ten minutes
	{	c_StatusResult.SetWindowTextW( _T(" Download is stalled.") );
		retVal = -4;
	}
Exit:
	if ( ranDir )  delete [] ranDir;
	if ( srcFile ) delete [] srcFile;
	if ( tgtFile ) delete [] tgtFile;
	return	retVal;
}			// WebDownload()
*/
//------------------------------------------------------------------------------------
/*
void	CPNMX_InstallerDlg::ResetStatus( void )
{
	c_DownloadStatusResult.SetWindowTextW( _T("") );
	c_UploadStatusResult.SetWindowTextW( _T("") );
	c_InstallStatusResult.SetWindowTextW( _T("") );

	c_Ppc_Downloaded.SetCheck( BST_UNCHECKED );
	c_Ppc_Downloaded.ShowWindow( SW_HIDE );

	c_MFC_Downloaded.SetCheck( BST_UNCHECKED );
	c_MFC_Downloaded.ShowWindow( SW_HIDE );

	c_BM_Downloaded.SetCheck( BST_UNCHECKED );
	c_BM_Downloaded.ShowWindow( SW_HIDE );

	c_NH_Downloaded.SetCheck( BST_UNCHECKED );
	c_NH_Downloaded.ShowWindow( SW_HIDE );

	c_SX_Downloaded.SetCheck( BST_UNCHECKED );
	c_SX_Downloaded.ShowWindow( SW_HIDE );

	c_YCF_Downloaded.SetCheck( BST_UNCHECKED );
	c_YCF_Downloaded.ShowWindow( SW_HIDE );


	c_Ppc_Uploaded.SetCheck( BST_UNCHECKED );
	c_Ppc_Uploaded.ShowWindow( SW_HIDE );

	c_MFC_Uploaded.SetCheck( BST_UNCHECKED );
	c_MFC_Uploaded.ShowWindow( SW_HIDE );

	c_BM_Uploaded.SetCheck( BST_UNCHECKED );
	c_BM_Uploaded.ShowWindow( SW_HIDE );

	c_NH_Uploaded.SetCheck( BST_UNCHECKED );
	c_NH_Uploaded.ShowWindow( SW_HIDE );

	c_SX_Uploaded.SetCheck( BST_UNCHECKED );
	c_SX_Uploaded.ShowWindow( SW_HIDE );

	c_YCF_Uploaded.SetCheck( BST_UNCHECKED );
	c_YCF_Uploaded.ShowWindow( SW_HIDE );


	c_Ppc_Installed.SetCheck( BST_UNCHECKED );
	c_Ppc_Installed.ShowWindow( SW_HIDE );

	c_MFC_Installed.SetCheck( BST_UNCHECKED );
	c_MFC_Installed.ShowWindow( SW_HIDE );

	c_BM_Installed.SetCheck( BST_UNCHECKED );
	c_BM_Installed.ShowWindow( SW_HIDE );

	c_NH_Installed.SetCheck( BST_UNCHECKED );
	c_NH_Installed.ShowWindow( SW_HIDE );

	c_SX_Installed.SetCheck( BST_UNCHECKED );
	c_SX_Installed.ShowWindow( SW_HIDE );

	c_YCF_Installed.SetCheck( BST_UNCHECKED );
	c_YCF_Installed.ShowWindow( SW_HIDE );

}			// ResetStatus()
*/
//--------------------------------------------------------------------------------------------
/*
void	CPNMX_InstallerDlg::OnEnChangeFirstName( void )
{	CString cs;
	c_FirstName.GetWindowTextW( cs );
	if ( cs == m_FirstName )
		return;
	m_FirstName = cs;
	EnableFetchButton( cs, 0 );				// 0 for FirstName
}			// OnEnChangeFirstName()
//--------------------------------------------------------------------------------------------
void	CPNMX_InstallerDlg::OnEnChangeLastName( void )
{	CString cs;
	c_LastName.GetWindowTextW( cs );
	if ( cs == m_LastName )
		return;
	m_LastName = cs;
	EnableFetchButton( cs, 1 );				// 1 for LastName
}			// OnEnChangeLastName()
//--------------------------------------------------------------------------------------------
void	CPNMX_InstallerDlg::OnEnChangeEmailAddress( void )
{	CString cs;
	c_EmailAddress.GetWindowTextW( cs );
	if ( cs == m_EmailAddress )
		return;
	m_EmailAddress = cs;
	EnableFetchButton( cs, 2 );				// 2 for EmailAddress
}			// OnEnChangeEmailAddress()
//--------------------------------------------------------------------------------------------
void CPNMX_InstallerDlg::OnEnChangeInvoicNnumber( void )
{	CString cs;
	c_InvoiceNumber.GetWindowTextW( cs );
	if ( cs == m_InvoiceNumber )
		return;
	m_InvoiceNumber = cs;
	EnableFetchButton( cs, 3 );				// 3 for InvoiceNumber
}			// OnEnChangeInvoicNnumber()
*/
//--------------------------------------------------------------------------------------------
