// YieldCurveFittrDlg.cpp : implementation file
//
#include "StdAfx.h"
#include "YieldCurveFitter.h"
#include "YieldCurveFitterDlg.h"
#include "ShortRateCalibration.h"
#include "BitArray.h"
#include "Vector.h"
#include "Registry.h"
#include "Matrix.h"
#include "GetDeviceInfo.h"
#include "HttpGetFile.h"
#include "crypt.h"
#include "utils.h"
#include "hwnds.h"
#if	( _WIN32_WCE > 0x420 )
#include "GetDeviceUniqueID.h"
#endif
//#include <stdio.h>

// these headers are supposed to help us run IOCTL_HAL_GET_DEVICE_INFO,
// but only platform development systems have the needed header files
//#include <Winbase.h>			// for KernelIoControl & PROCESSOR_INFO
//#include <kfuncs.h>			// for KernelIoControl & PROCESSOR_INFO

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_TRAY 5000
#define TRAY_NOTIFYICON WM_USER + 2001

extern	CYieldCurveFitterApp	theApp;
extern	CString	months[12];

/////////////////////////////////////////////////////////////////////////////
// CYieldCurveFitterDlg dialog

IMPLEMENT_DYNAMIC( CYieldCurveFitterDlg, CNillaDialog )

CYieldCurveFitterDlg::CYieldCurveFitterDlg( CWnd* pParent /*=NULL*/ )
	: CNillaDialog( CYieldCurveFitterDlg::IDD, pParent )
{
	//{{AFX_DATA_INIT( CYieldCurveFitterDlg )
	m_3moYield  = 0.0;
	m_6moYield  = 0.0;
	m_1yrYield  = 0.0;
	m_2yrYield  = 0.0;
	m_3yrYield  = 0.0;
	m_5yrYield  = 0.0;
	m_7yrYield  = 0.0;
	m_10yrYield = 0.0;
	m_15yrYield = 0.0;
	m_20yrYield = 0.0;
	m_25yrYield = 0.0;
	m_30yrYield = 0.0;
	//}}AFX_DATA_INIT
	m_hbGray = CreateSolidBrush( RGB(208,208,208) );		// gray
	m_hbWhite = CreateSolidBrush( RGB(255,255,255) );		// white
	threadHandle = NULL;

	spotRate = 3.0f;
	userSupplied = new CBitArray(12);
	userSupplied->clear();

		// NOTE: LoadIcon() does not require a subsequent DestroyIcon in Win32
//	m_hIcon = AfxGetApp()->LoadIcon( IDI_YieldCurveFitter );

//	hInet = NULL;
//	hInetSession = NULL;

		// moved the following to YieldCurveFitterApp
//	urlRequest = NULL;
//	lpFileName = NULL;
//	lpServerName = NULL;
}

CYieldCurveFitterDlg::~CYieldCurveFitterDlg( void )
{	// the following have been moved to YieldCurveFitterApp
//	if ( docDir ) delete [] docDir;
//	if ( urlRequest )		delete [] urlRequest;
//	if ( lpFileName )		delete [] lpFileName;
//	if ( lpServerName )		delete [] lpServerName;

	/*		// we're done with the internet handle
	if ( hInet )
		if ( ! InternetCloseHandle( hInet ) )
		{
#ifdef _DEBUG
			TRACE( _T("YieldCurveFitterDlg::~YieldCurveFitterDlg: InternetCloseHandle(hInet) failed.s\n") );
#endif
		}
		// we're done with any session handle that might still be around
	if ( hInetSession )
		if ( ! InternetCloseHandle( hInetSession ) )
		{
#ifdef _DEBUG
			TRACE( _T("YieldCurveFitterDlg::~YieldCurveFitterDlg: InternetCloseHandle(hInetSession) failed.\n") );
#endif
		}
*/
}

void CYieldCurveFitterDlg::DoDataExchange( CDataExchange* pDX )
{
	CNillaDialog::DoDataExchange( pDX );
	//{{AFX_DATA_MAP( CYieldCurveFitterDlg )
	DDX_Control(pDX, IDC_Fetch, c_Fetch);
	DDX_Control(pDX, IDC_StatusResult, c_StatusResult);
	DDX_Control(pDX, IDC_RateDate, c_RateDate);
	DDX_Text(pDX, IDC_3moYield, m_3moYield);
	DDV_MinMaxFloat(pDX, m_3moYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_6moYield, m_6moYield);
	DDV_MinMaxFloat(pDX, m_6moYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_1yrYield, m_1yrYield);
	DDV_MinMaxFloat(pDX, m_1yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_2yrYield, m_2yrYield);
	DDV_MinMaxFloat(pDX, m_2yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_3yrYield, m_3yrYield);
	DDV_MinMaxFloat(pDX, m_3yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_5yrYield, m_5yrYield);
	DDV_MinMaxFloat(pDX, m_5yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_7yrYield, m_7yrYield);
	DDV_MinMaxFloat(pDX, m_7yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_10yrYield, m_10yrYield);
	DDV_MinMaxFloat(pDX, m_10yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_15yrYield, m_15yrYield);
	DDV_MinMaxFloat(pDX, m_15yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_20yrYield, m_20yrYield);
	DDV_MinMaxFloat(pDX, m_20yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_25yrYield, m_25yrYield);
	DDV_MinMaxFloat(pDX, m_25yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_30yrYield, m_30yrYield);
	DDV_MinMaxFloat(pDX, m_30yrYield, 0.f, 9000.f);
	DDX_Control(pDX, IDC_3moYield, c_3moYield);
	DDX_Control(pDX, IDC_6moYield, c_6moYield);
	DDX_Control(pDX, IDC_1yrYield, c_1yrYield);
	DDX_Control(pDX, IDC_2yrYield, c_2yrYield);
	DDX_Control(pDX, IDC_3yrYield, c_3yrYield);
	DDX_Control(pDX, IDC_5yrYield, c_5yrYield);
	DDX_Control(pDX, IDC_7yrYield, c_7yrYield);
	DDX_Control(pDX, IDC_10yrYield, c_10yrYield);
	DDX_Control(pDX, IDC_15yrYield, c_15yrYield);
	DDX_Control(pDX, IDC_20yrYield, c_20yrYield);
	DDX_Control(pDX, IDC_25yrYield, c_25yrYield);
	DDX_Control(pDX, IDC_30yrYield, c_30yrYield);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CYieldCurveFitterDlg, CNillaDialog)
	//{{AFX_MSG_MAP( CYieldCurveFitterDlg )
	ON_EN_KILLFOCUS(IDC_10yrYield, OnKillFocus10yrYield)
	ON_EN_KILLFOCUS(IDC_15yrYield, OnKillFocus15yrYield)
	ON_EN_KILLFOCUS(IDC_1yrYield, OnKillFocus1yrYield)
	ON_EN_KILLFOCUS(IDC_20yrYield, OnKillFocus20yrYield)
	ON_EN_KILLFOCUS(IDC_25yrYield, OnKillFocus25yrYield)
	ON_EN_KILLFOCUS(IDC_2yrYield, OnKillFocus2yrYield)
	ON_EN_KILLFOCUS(IDC_30yrYield, OnKillFocus30yrYield)
	ON_EN_KILLFOCUS(IDC_3moYield, OnKillFocus3moYield)
	ON_EN_KILLFOCUS(IDC_3yrYield, OnKillFocus3yrYield)
	ON_EN_KILLFOCUS(IDC_5yrYield, OnKillFocus5yrYield)
	ON_EN_KILLFOCUS(IDC_6moYield, OnKillFocus6moYield)
	ON_EN_KILLFOCUS(IDC_7yrYield, OnKillFocus7yrYield)
	ON_EN_SETFOCUS(IDC_10yrYield, OnSetFocus10yrYield)
	ON_EN_SETFOCUS(IDC_15yrYield, OnSetFocus15yrYield)
	ON_EN_SETFOCUS(IDC_1yrYield, OnSetFocus1yrYield)
	ON_EN_SETFOCUS(IDC_20yrYield, OnSetFocus20yrYield)
	ON_EN_SETFOCUS(IDC_25yrYield, OnSetFocus25yrYield)
	ON_EN_SETFOCUS(IDC_2yrYield, OnSetFocus2yrYield)
	ON_EN_SETFOCUS(IDC_30yrYield, OnSetFocus30yrYield)
	ON_EN_SETFOCUS(IDC_3moYield, OnSetFocus3moYield)
	ON_EN_SETFOCUS(IDC_3yrYield, OnSetFocus3yrYield)
	ON_EN_SETFOCUS(IDC_5yrYield, OnSetFocus5yrYield)
	ON_EN_SETFOCUS(IDC_6moYield, OnSetFocus6moYield)
	ON_EN_SETFOCUS(IDC_7yrYield, OnSetFocus7yrYield)
#ifdef ServerAccess
	ON_BN_CLICKED(IDC_Fetch, OnFetch)
#endif
	//}}AFX_MSG_MAP

	ON_REGISTERED_MESSAGE( WM_YIELDCURVEFITTER_ALREADY_RUNNING, OnYieldCurveFitterAlreadyRunning)
#ifdef ServerAccess
	ON_REGISTERED_MESSAGE( WM_YIELDCURVEFITTER_PARSE_DATA, OnParseData )
#endif
	//	ON_MESSAGE( TRAY_NOTIFYICON, OnNotifyIcon )
	ON_WM_CTLCOLOR()

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CYieldCurveFitterDlg message handlers

HBRUSH CYieldCurveFitterDlg::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	pDC->SetBkMode( TRANSPARENT );
	return	( nCtlColor == CTLCOLOR_EDIT )  ?  m_hbWhite  :  m_hbGray;
}			// OnCtlColor()
//------------------------------------------------------------------------------
LRESULT		CYieldCurveFitterDlg::OnYieldCurveFitterAlreadyRunning( WPARAM, LPARAM )
{
	return	WM_YIELDCURVEFITTER_ALREADY_RUNNING;
}			// OnYieldCurveFitterAlreadyRunning()
//---------------------------------------------------------------------------
/*
LRESULT CYieldCurveFitterDlg::OnNotifyIcon( WPARAM wParam, LPARAM lParam )
{
	bool	handleIt = lParam == WM_LBUTTONDOWN  &&  wParam == ID_TRAY;
	return	(LRESULT)( handleIt  ?  SetForegroundWindow()  :  0 );
}			// OnNotifyIcon()
*/
//------------------------------------------------------------------------------
BOOL	CYieldCurveFitterDlg::OnInitDialog()
{	bool		success;
	CShortRateCalibration	srb;
	CNillaDialog::OnInitDialog();

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// Set the icon for this dialog.  The framework does this automatically
		// when the application's main window is not a dialog
//	SetIcon( m_hIcon, TRUE );				// Set big icon
//	SetIcon( m_hIcon, FALSE );				// Set small icon
	CenterWindow( GetDesktopWindow() );		// center to the hpc screen

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// set up the urlRequest string something like: /<page>?os=%s?dv=%s?uu=%s?hu=%s
	bool	enableFetch = false;
#ifdef ServerAccess
	unsigned long TBRdscfg[2*AES_BLOCK_SIZE/sizeof(long)] =		// we'll only use 3/4 of these in RC6
	{
		0x1DA7F92B, 0x21384956, 0xBB5B80B6, 0x302DFF9C,
		0x3DF1A711, 0x7C404840, 0x4ED53EA2, 0xAF08FE06,
		0xC39F4444, 0x9DCBA1B8, 0xCA99CC82, 0xBBDCF37F,
		0x29C4175C, 0x5E03FAAC, 0xAEF1598D, 0x836378E0
	};

	char*	serverPage = NULL;
	char*	osVer		 = NULL;
	char*	thinUuid	 = NULL;
		// now for the urlRequest
		// set up the document directory and lpFileName
		// look up the PocketNumerix document directory in the registry
	DWORD	slen = 0;						// so we can tell whether we calculated it in the if() below
	wchar_t*	docDir = NULL;				// allocated by ReadRegString on the heap
	wchar_t*	docDirKey = _T("SOFTWARE\\PocketNumerix\\DocumentDirectory");
	success = ReadRegString( HKEY_CURRENT_USER, docDirKey, (BYTE**)&docDir, &slen );
	if ( success  &&  docDir )
	{	BOOL B1 = CreateDirectory( docDir, NULL );					// accept failure as indicating that directory already exists
		wchar_t* fName = _T("tBillRates.pnh");
		theApp.lpFileName = new wchar_t[ slen + wcslen(fName) + 2 ];		// +1 for '\0', +1 for the backslash
		if ( theApp.lpFileName )
		{	swprintf( theApp.lpFileName, _T("%s\\%s"), docDir, fName );
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				// get the server name from the registry...
			success = GetTBRServer( (BYTE**)&theApp.lpServerName, &slen );
			if ( success  &&  theApp.lpServerName )
			{	success = GetTBRPage( (BYTE**)&serverPage, &slen );
				if ( success  &&  serverPage )
				{	osVer = GetThinString( GetOsVersion() );						// on the heap
					if ( osVer )
					{
//						CString	uuid = _T("BC000DE6DF85D2D138000050BFE45CE5");		// good UUID
//						CString	uuid = _T("XC000DE6DF85D2D138000050BFE45CE5");		// bad UUID
						thinUuid = GetThinString( GetSerialNumber() );				// on the heap
						if ( thinUuid )
						{		// on PPC'02 and PPC'03, we're done - on WM5, we'll still need to capture YCF's huid
							char	largeBuf[256];
							short retVal = GetDeviceInfo( theApp.dv, theApp.uu, theApp.hu, theApp.na, theApp.em );
							if ( retVal == 0 )
							{
#if	( _WIN32_WCE < 0x500 )
//								sprintf( largeBuf, "/%s?os=%s?dv=%d?uu=%s?hu=0", serverPage, osVer, m_DeviceId, thinUuid );
								sprintf( largeBuf, "?os=%s?dv=%d?uu=%s?hu=0", osVer, theApp.dv, thinUuid );
#else
								BYTE	huid[48];
								DWORD	lenOut;
								char*	appData = "pknmxYieldCurveFitter";
								HRESULT hr = GetDeviceUniqueID( reinterpret_cast<BYTE*>(appData), strlen(appData), 1, huid, &lenOut );

#if HUID_DEBUG
								FILE* fp = _wfopen( _T("\\Temp\\PocketNumerix\\huid.txt"), _T("w") );
								fprintf( fp, "%s", huid );
								fclose( fp );
#endif
									// HEX ANSI  <--  BYTE* (actually a very long integer)
								if ( hr == S_OK )
								{	char* huidHex = MakeHexString( huid, lenOut );			// uuidHex is little-endian reversed
									if ( huidHex )
									{
										sprintf( largeBuf, "?os=%s?dv=%d?uu=%s?hu=%s", osVer, theApp.dv, thinUuid, huidHex );
										delete [] huidHex;
#endif
										char* mimebuf = NULL;
										DWORD mimelen = 0;
										short res = EEcode( largeBuf, TBRdscfg, mimebuf, mimelen );
										if ( res == 0 )
										{	sprintf( largeBuf, "/%s?ds=%s", serverPage, mimebuf );
											delete [] mimebuf;								// heap allocated
											slen = strlen( largeBuf );
											theApp.urlRequest = new char[ slen + 1 ];

											if ( theApp.urlRequest )
											{	strncpy( theApp.urlRequest, largeBuf, slen+1 );
#ifdef _DEBUG
												{	CString cs(theApp.urlRequest);
													TRACE( _T("YieldCurveFitterDlg::InitDialog: urlRequest=%s\n"), cs );
												}
#endif
												enableFetch = true;
											}
#if	( _WIN32_WCE > 0x420 )								// WM5
									}	}
#endif
	}	}	}	}	}	}	}	}
		// clean up the heap
	if ( docDir )		{	delete [] docDir;		docDir	   = NULL;	}
	if ( thinUuid	)	{	delete [] thinUuid;		thinUuid   = NULL;	}
	if ( osVer		)	{	delete [] osVer;		osVer	   = NULL;	}
	if ( serverPage )	{	delete [] serverPage;	serverPage = NULL;	}
#endif																		// ServerAccess

	c_Fetch.EnableWindow( enableFetch );
	c_Fetch.ShowWindow( enableFetch ? SW_SHOW : SW_HIDE );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	char*	briefFmt = "%.2f";
	char*	longFmt = "%.3f";

		// get the short rate basis
	success = srb.GetShortRateBasis( srb );
	if ( success )
	{	m_3moYield  = srb.r3mo;
		bool	userData = m_3moYield > 0.0;
		if ( userData )	userSupplied->set(0);
		setEditBox( (userData ? briefFmt : longFmt), m_3moYield,  IDC_3moYield  );
		
		m_6moYield  = srb.r6mo;
		userData = m_6moYield > 0.0;
		if ( userData )	userSupplied->set(1);
		setEditBox( (userData ? briefFmt : longFmt), m_6moYield,  IDC_6moYield  );

		m_1yrYield  = srb.r1yr;
		userData = m_1yrYield > 0.0;
		if ( userData )	userSupplied->set(2);
		setEditBox( (userData ? briefFmt : longFmt), m_1yrYield,  IDC_1yrYield  );

		m_2yrYield  = srb.r2yr;
		userData = m_2yrYield > 0.0;
		if ( userData )	userSupplied->set(3);
		setEditBox( (userData ? briefFmt : longFmt), m_2yrYield,  IDC_2yrYield  );

		m_3yrYield  = srb.r3yr;
		userData = m_3yrYield > 0.0;
		if ( userData ) userSupplied->set(4);
		setEditBox( (userData ? briefFmt : longFmt), m_3yrYield,  IDC_3yrYield  );

		m_5yrYield  = srb.r5yr;
		userData = m_5yrYield > 0.0;
		if ( userData ) userSupplied->set(5);
		setEditBox( (userData ? briefFmt : longFmt), m_5yrYield,  IDC_5yrYield  );

		m_7yrYield  = srb.r7yr;
		userData = m_7yrYield > 0.0;
		if ( userData ) userSupplied->set(6);
		setEditBox( (userData ? briefFmt : longFmt), m_7yrYield,  IDC_7yrYield  );

		m_10yrYield = srb.r10yr;
		userData = m_10yrYield > 0.0;
		if ( userData ) userSupplied->set(7);
		setEditBox( (userData ? briefFmt : longFmt), m_10yrYield, IDC_10yrYield );

		m_15yrYield = srb.r15yr;
		userData = m_15yrYield > 0.0;
		if ( userData ) userSupplied->set(8);
		setEditBox( (userData ? briefFmt : longFmt), m_15yrYield, IDC_15yrYield );

		m_20yrYield = srb.r20yr;
		userData = m_20yrYield > 0.0;
		if ( userData ) userSupplied->set(9);
		setEditBox( (userData ? briefFmt : longFmt), m_20yrYield, IDC_20yrYield );

		m_25yrYield = srb.r25yr;
		userData = m_25yrYield > 0.0;
		if ( userData ) userSupplied->set(10);
		setEditBox( (userData ? briefFmt : longFmt), m_25yrYield, IDC_25yrYield );

		m_30yrYield = srb.r30yr;
		userData = m_30yrYield > 0.0;
		if ( userData ) userSupplied->set(11);
		setEditBox( (userData ? briefFmt : longFmt), m_30yrYield, IDC_30yrYield );
	}

		// display the last stored rate date
	ServerRates	SR;
	success = GetServerRates( &SR );			// just want the date at this point
	if ( success )
	{	wchar_t		wbuf[16];
		swprintf( wbuf, _T("%02d%3s%04d"), SR.da, months[SR.mo-1], SR.yr );
#ifdef _DEBUG
		TRACE( _T("YieldCurveFitterDlg::OnInitDialog: RateDate=%s\n"), wbuf );
#endif
		c_RateDate.SetWindowText( wbuf );
	}
	else
		c_RateDate.SetWindowText( _T("") );		// no ServerRates have ever been registered

		// update the GUI
	UpdateRates();
//	SipShowIM( SIPF_ON );		// show the input panel
	return	TRUE;	// return TRUE  unless you set the focus to a control
}			// OnInitDialog()
//---------------------------------------------------------------------------
void	CYieldCurveFitterDlg::UpdateRates( void )
{		// calculate the Spot Rate (two ways) & update the dialog
		// count the nonzero rates available
	double	spotSlope;
	if ( m_3moYield  > 0.0  &&  m_6moYield  > 0.0 )
	{		// calculate the spotSlope and the extrapolated spot rate
		spotSlope = (m_6moYield - m_3moYield) / (0.5 - 0.25);
		spotRate = (float)(m_3moYield - 0.25 * spotSlope);
		setStatic( "%.3f", spotRate, IDC_RiskFreeRateResult );

			// register the spot rate as the new RiskFreeRate (which has been rescaled)
		bool	success = SetRiskFreeRate( spotRate );
#ifdef _DEBUG
		if ( ! success )
			TRACE( _T("YieldCurveFitterDlg::UpdateRates: SetRiskFreeRate() failed\n") );
#endif

	}
	unsigned short nzr = 2										// already guaranteed to have
		//			   + (m_3moYield  > 0.0 ? 1 : 0)			// 3mo & 6mo U.S. Treasury rates
		//			   + (m_6moYield  > 0.0 ? 1 : 0)			// so there's no need to test values again.
					   + (m_1yrYield  > 0.0 ? 1 : 0)			// non-U.S. Treasury yield
					   + (m_2yrYield  > 0.0 ? 1 : 0)			// U.S. Treasury
					   + (m_3yrYield  > 0.0 ? 1 : 0)			// U.S. Treasury
					   + (m_5yrYield  > 0.0 ? 1 : 0)			// U.S. Treasury
					   + (m_7yrYield  > 0.0 ? 1 : 0)			// non-U.S. Treasury yield
					   + (m_10yrYield > 0.0 ? 1 : 0)			// U.S. Treasury
					   + (m_15yrYield > 0.0 ? 1 : 0)			// non-U.S. Treasury yield
					   + (m_20yrYield > 0.0 ? 1 : 0)			// non-U.S. Treasury yield
					   + (m_25yrYield > 0.0 ? 1 : 0)			// non-U.S. Treasury yield
					   + (m_30yrYield > 0.0 ? 1 : 0);			// U.S. Treasury
	if ( nzr < 5 )
		return;

	CShortRateCalibration srb;									// include a 1/8th yr rate to
	srb.r45da = (float)(m_3moYield - 0.125 * spotSlope);		// stabilize curve fitting
	srb.r3mo  = (*userSupplied)[ 0] ? m_3moYield  : 0.0f;
	srb.r6mo  = (*userSupplied)[ 1] ? m_6moYield  : 0.0f;
	srb.r1yr  = (*userSupplied)[ 2] ? m_1yrYield  : 0.0f;
	srb.r2yr  = (*userSupplied)[ 3] ? m_2yrYield  : 0.0f;
	srb.r3yr  = (*userSupplied)[ 4] ? m_3yrYield  : 0.0f;
	srb.r5yr  = (*userSupplied)[ 5] ? m_5yrYield  : 0.0f;
	srb.r7yr  = (*userSupplied)[ 6] ? m_7yrYield  : 0.0f;
	srb.r10yr = (*userSupplied)[ 7] ? m_10yrYield : 0.0f;
	srb.r15yr = (*userSupplied)[ 8] ? m_15yrYield : 0.0f;
	srb.r20yr = (*userSupplied)[ 9] ? m_20yrYield : 0.0f;
	srb.r25yr = (*userSupplied)[10] ? m_25yrYield : 0.0f;
	srb.r30yr = (*userSupplied)[11] ? m_30yrYield : 0.0f;

		// compute the spot rate (and all the rates not supplied by the user)
		// from a fitted normal yield curve
	srb.ComputeRates();
	if ( !(*userSupplied)[ 0] )	setEditBox( "%.3f", srb.r3mo,  IDC_3moYield  );
	if ( !(*userSupplied)[ 1] )	setEditBox( "%.3f", srb.r6mo,  IDC_6moYield  );
	if ( !(*userSupplied)[ 2] )	setEditBox( "%.3f", srb.r1yr,  IDC_1yrYield  );
	if ( !(*userSupplied)[ 3] )	setEditBox( "%.3f", srb.r2yr,  IDC_2yrYield  );
	if ( !(*userSupplied)[ 4] )	setEditBox( "%.3f", srb.r3yr,  IDC_3yrYield  );
	if ( !(*userSupplied)[ 5] )	setEditBox( "%.3f", srb.r5yr,  IDC_5yrYield  );
	if ( !(*userSupplied)[ 6] )	setEditBox( "%.3f", srb.r7yr,  IDC_7yrYield  );
	if ( !(*userSupplied)[ 7] )	setEditBox( "%.3f", srb.r10yr, IDC_10yrYield );
	if ( !(*userSupplied)[ 8] )	setEditBox( "%.3f", srb.r15yr, IDC_15yrYield );
	if ( !(*userSupplied)[ 9] )	setEditBox( "%.3f", srb.r20yr, IDC_20yrYield );
	if ( !(*userSupplied)[10] )	setEditBox( "%.3f", srb.r25yr, IDC_25yrYield );
	if ( !(*userSupplied)[11] )	setEditBox( "%.3f", srb.r30yr, IDC_30yrYield );
}			// UpdateRates()
//--------------------------------------------------------------------
#ifdef ServerAccess
LRESULT		CYieldCurveFitterDlg::OnParseData( WPARAM, LPARAM )
{		// WPARAM & LPARAM are ignored
	int		itemsRead = 0;
	FILE*	fp = _wfopen( theApp.lpFileName, _T("r") );
	if ( ! fp )
	{
#ifdef _DEBUG
	TRACE( _T("ParseFile(-1): Can't open '%s' for reading\n"), theApp.lpFileName );
#endif
		c_StatusResult.SetWindowText( _T("Can't open('r') download file.") );
		return	-1;
	}

	ServerRates	SR;
		// 01-Mar-2001
	int	yr, mo, da;		// can't read directly into a (byte) 
	c_StatusResult.SetWindowText( _T("Reading...") );
	itemsRead = fscanf( fp, "%4d-%2d-%2d %f %f %f %f %f %f %f",
						&yr, &mo, &da,
						&SR.ff[0], &SR.ff[1], &SR.ff[2],
						&SR.ff[3], &SR.ff[4], &SR.ff[5], &SR.ff[6] );
	fclose( fp );

	if ( itemsRead < 10 )
	{
#ifdef _DEBUG
		TRACE( _T("ParseFile(-2): found %d items in %s\n"), itemsRead, theApp.lpFileName );
#endif
		CString msg = ( itemsRead == 4  &&  yr == 0  &&  mo == 0  &&  da == 0 )
			?  _T("Authentication failed.")
			:  _T("Server is down.");
		c_StatusResult.SetWindowText( msg );
		return	-2;
	}
	SR.da = (byte)da;
	SR.mo = (byte)mo;
	SR.yr = (unsigned short)yr;

		// save the ServerRates struct to the registry
	bool success = SetServerRates( SR );
#ifdef _DEBUG
	if ( ! success )
		TRACE( _T("DownloadURL: SetServerRates() failed\n") );
#endif

		// update the rateDate on the dialog
	wchar_t	dateStr[ 16 ];
	swprintf( dateStr, _T("%02d%3s%04d"), SR.da, months[SR.mo-1], SR.yr );
	c_RateDate.SetWindowText( dateStr );

		// update the rates on the dialog
	RestoreServerRates( SR );
	c_StatusResult.SetWindowText( _T("Fitting...") );
	UpdateRates();
	c_StatusResult.SetWindowText( _T("") );
	return	0;
}			// OnParseData()
#endif						// ServerAccess
//-------------------------------------------------------------------------------------------------
void	CYieldCurveFitterDlg::SaveSRB( void )
{	CShortRateCalibration		srb;
	srb.r3mo  = userSupplied->query( 0) ?  m_3moYield : 0.0f;	// MFC samples the rates
	srb.r6mo  = userSupplied->query( 1) ?  m_6moYield : 0.0f;	// from the CEdit box contents
	srb.r1yr  = userSupplied->query( 2) ?  m_1yrYield : 0.0f;	//    .
	srb.r2yr  = userSupplied->query( 3) ?  m_2yrYield : 0.0f;	//    .
	srb.r3yr  = userSupplied->query( 4) ?  m_3yrYield : 0.0f;	//    .
	srb.r5yr  = userSupplied->query( 5) ?  m_5yrYield : 0.0f;
	srb.r7yr  = userSupplied->query( 6) ?  m_7yrYield : 0.0f;
	srb.r10yr = userSupplied->query( 7) ? m_10yrYield : 0.0f;
	srb.r15yr = userSupplied->query( 8) ? m_15yrYield : 0.0f;
	srb.r20yr = userSupplied->query( 9) ? m_20yrYield : 0.0f;
	srb.r25yr = userSupplied->query(10) ? m_25yrYield : 0.0f;
	srb.r30yr = userSupplied->query(11) ? m_30yrYield : 0.0f;

	bool	success = srb.SetShortRateBasis( srb );
#ifdef _DEBUG
	if ( ! success )
		TRACE( _T("YieldCurveFitterDlg::SaveSRB: ShortRateBasis write failed\n") );
#endif
}			// SaveSRB()
//--------------------------------------------------------------------
void	CYieldCurveFitterDlg::KillFocusYieldWork( int ctl_ID, float* yield, short usi ) 
{	
//	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( ctl_ID );
	if ( ff == *yield )
		return;
	*yield = ff;
	if ( ff > 0.0f )
		userSupplied->set( usi );
	else
		userSupplied->reset( usi );
	UpdateRates();
}			// KillFocusYieldWork()
//--------------------------------------------------------------------
void	CYieldCurveFitterDlg::OnKillFocus3moYield() 
{
	KillFocusYieldWork( IDC_3moYield, &m_3moYield, 0 );
}			// OnKillFocus3moYield()
//--------------------------------------------------------------------
void	CYieldCurveFitterDlg::OnKillFocus6moYield() 
{
	KillFocusYieldWork( IDC_6moYield, &m_6moYield, 1 );
}			// OnKillFocus6moYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnKillFocus1yrYield() 
{
	KillFocusYieldWork( IDC_1yrYield, &m_1yrYield, 2 );
}			// OnKillFocus1yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnKillFocus2yrYield() 
{
	KillFocusYieldWork( IDC_2yrYield, &m_2yrYield, 3 );
}			// OnKillFocus2yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnKillFocus3yrYield() 
{
	KillFocusYieldWork( IDC_3yrYield, &m_3yrYield, 4 );
}			// OnKillFocus3yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnKillFocus5yrYield() 
{
	KillFocusYieldWork( IDC_5yrYield, &m_5yrYield, 5 );
}			// OnKillFocus5yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnKillFocus7yrYield() 
{
	KillFocusYieldWork( IDC_7yrYield, &m_7yrYield, 6 );
}			// OnKillFocus7yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnKillFocus10yrYield() 
{
	KillFocusYieldWork( IDC_10yrYield, &m_10yrYield, 7 );
}			// OnKillFocus10yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnKillFocus15yrYield() 
{
	KillFocusYieldWork( IDC_15yrYield, &m_15yrYield, 8 );
}			// OnKillFocus15yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnKillFocus20yrYield() 
{
	KillFocusYieldWork( IDC_20yrYield, &m_20yrYield, 9 );
}			// OnKillFocus20yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnKillFocus25yrYield() 
{
	KillFocusYieldWork( IDC_25yrYield, &m_25yrYield, 10 );
}			// OnKillFocus25yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnKillFocus30yrYield() 
{
	KillFocusYieldWork( IDC_30yrYield, &m_30yrYield, 11 );
}			// OnKillFocus30yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::SetFocusYieldWork( CNumEdit* ceb )
{
//	SHSipPreference( GetSafeHwnd(), SIP_UP );
	ceb->SetSel( 0, -1 );
}			// SetFocusYieldWork()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnSetFocus3moYield()
{
	SetFocusYieldWork( &c_3moYield );
}			// OnSetFocus3moYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnSetFocus6moYield()
{
	SetFocusYieldWork( &c_6moYield );
}			// OnSetFocus6moYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnSetFocus1yrYield()
{
	SetFocusYieldWork( &c_1yrYield );
}			// OnSetFocus1yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnSetFocus2yrYield()
{
	SetFocusYieldWork( &c_2yrYield );
}			// OnSetFocus2yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnSetFocus3yrYield()
{
	SetFocusYieldWork( &c_3yrYield );
}			// OnSetFocus3yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnSetFocus5yrYield()
{
	SetFocusYieldWork( &c_5yrYield );
}			// OnSetFocus5yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnSetFocus7yrYield()
{
	SetFocusYieldWork( &c_7yrYield );
}			// OnSetFocus7yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnSetFocus10yrYield()
{
	SetFocusYieldWork( &c_10yrYield );
}			// OnSetFocus10yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnSetFocus15yrYield()
{
	SetFocusYieldWork( &c_15yrYield );
}			// OnSetFocus15yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnSetFocus20yrYield()
{
	SetFocusYieldWork( &c_20yrYield );
}			// OnSetFocus20yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnSetFocus25yrYield()
{
	SetFocusYieldWork( &c_25yrYield );
}			// OnSetFocus25yrYield()
//--------------------------------------------------------------------
void CYieldCurveFitterDlg::OnSetFocus30yrYield()
{
	SetFocusYieldWork( &c_30yrYield );
}			// OnSetFocus30yrYield()
//--------------------------------------------------------------------
void	CYieldCurveFitterDlg::RestoreServerRates( ServerRates SR )
{		// cache gate based on today's date
		// retrieved a ServerRates structure from the registry and the dates match
	m_3moYield  = SR.ff[0];   userSupplied->set( 0);   setEditBox( "%.2f", SR.ff[0], IDC_3moYield  );
	m_6moYield  = SR.ff[1];   userSupplied->set( 1);   setEditBox( "%.2f", SR.ff[1], IDC_6moYield  );
	m_2yrYield  = SR.ff[2];   userSupplied->set( 3);   setEditBox( "%.2f", SR.ff[2], IDC_2yrYield  );
	m_3yrYield  = SR.ff[3];   userSupplied->set( 4);   setEditBox( "%.2f", SR.ff[3], IDC_3yrYield  );
	m_5yrYield  = SR.ff[4];   userSupplied->set( 5);   setEditBox( "%.2f", SR.ff[4], IDC_5yrYield  );
	m_10yrYield = SR.ff[5];   userSupplied->set( 7);   setEditBox( "%.2f", SR.ff[5], IDC_10yrYield );
	m_30yrYield = SR.ff[6];   userSupplied->set(11);   setEditBox( "%.2f", SR.ff[6], IDC_30yrYield );
}			// RestoreServerRates()
//--------------------------------------------------------------------
#ifdef ServerAccess
void	CYieldCurveFitterDlg::OnFetch() 
{	ServerRates		SR;
		// Clear any text in the messaging area
	c_StatusResult.SetWindowText( _T("") );

		// retrieve the ServerRate structure from the registry
	if ( GetServerRates( &SR ) )
	{
		COleDateTime	now = COleDateTime::GetCurrentTime();
#ifdef _DEBUG
		TRACE( _T("YieldCurveFitterDlg::OnFetch: now=%s\n"), now.Format() );
#endif
		byte			da = (byte)now.GetDay();
		byte			mo = (byte)now.GetMonth();
		unsigned short	yr = (unsigned short)now.GetYear();
		bool	yy = ( SR.yr == yr );		// it looks like you could just substitute these logical
		bool	mm = ( SR.mo == mo );		// operations for the bools in the if(), but the
		bool	dd = ( SR.da == da );		// compiler does something in that case and the if always
		bool	ymMatch = ( yy  &&  mm );
		bool	fullMatch = ymMatch  &&  dd;
		if ( ymMatch  &&  !dd )
		{
			int  dow = now.GetDayOfWeek();
			if ( dow == 1  ||  dow == 7 )			// Sunday == 1, Saturday == 7
			{	COleDateTime		regDate( SR.yr, SR.mo, SR.da, 0, 0, 0 );
				COleDateTimeSpan	gap = now - regDate;
				int					gapDays = gap.GetDays();
					// redefine the meaning of a fullMatch to include weekend days,
					// if the locally cached rates are from a Friday
				fullMatch = ( gapDays <= 1  &&  dow == 7 )  ||  ( gapDays <= 2  &&  dow == 1 ); 
			}
		}
		if ( fullMatch )			// fails.  This if() will succeed when the values match
		{		// install those values in the GUI and curve fit them (thus updating the remaining terms)
			RestoreServerRates( SR );
			UpdateRates();
#if LIMIT_OF_ONE_FETCH_PER_DAY
			return;
#endif
		}
	}

		// fetch the latest tBillRates
	DWORD	dwThreadId, dwExitCode = 0;
	hwnds_t* zz = new hwnds_t;
	zz->ycfDialog = GetSafeHwnd();
	zz->fetchButton = c_Fetch.GetSafeHwnd();
	zz->statusResult = c_StatusResult.GetSafeHwnd();
		// the thread is responsible for deleting the hwnds_t struct
	threadHandle = CreateThread( NULL, 0, DownloadURL, (void*)zz, 0, &dwThreadId );

		// the does its own Restore & Update
	return;
}			// OnFetch()
#endif					// ServerAccess
//------------------------------------------------------------------------------
