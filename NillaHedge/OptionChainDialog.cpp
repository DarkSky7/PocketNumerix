// OptionChainDialog.cpp : implementation file
//
#include "StdAfx.h"
#include <string.h>
#include <stdlib.h>
#include "resource.h"
#include "OptionChainDialog.h"
#include "GetDeviceInfo.h"
#include "crypt.h"
#include "NillaHedge.h"
#include "StockManager.h"
#include "OptionManager.h"
#include "RegistryManager.h"
#include "Option.h"
#include "utils.h"
#include "hwnds.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const	short	DummyColumn = 10;

	// globals
extern	CString months[];
extern	CNillaHedgeApp	theApp;

/////////////////////////////////////////////////////////////////////////////
// COptionChainDialog dialog

COptionChainDialog::COptionChainDialog(CWnd* pParent /*=NULL*/)
	: CNillaDialog(COptionChainDialog::IDD, pParent)
	, m_activeComboBox( NULL )
	, m_nCalls( 0 )
	, m_nPuts( 0 )
//	, lpServerName( NULL )
//	, lpPageName( NULL )
//	, lpFileName( NULL )
//	, urlRequest( NULL )
	, uuid( NULL )
//	, sendWaitTime( 5000 )
//	, recvWaitTime( 5000 )
	, osVer( NULL )
{
	//{{AFX_DATA_INIT(COptionChainDialog)
	m_StockSymbol = _T("");
	m_Expiration = _T("");
	//}}AFX_DATA_INIT

//	hInet = NULL;
//	hInetSession = NULL;
//	m_lastKillFocusChangedStockSymbol = false;
	m_StockSymbolForOptionChain = _T("");
}

void COptionChainDialog::DoDataExchange(CDataExchange* pDX)
{
	CNillaDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionChainDialog)
	DDX_Control(pDX, IDC_StatusResult, c_StatusResult);
	DDX_Control(pDX, IDC_Fetch, c_Fetch);
	DDX_Control(pDX, IDC_StockSymbol, c_StockSymbol);
	DDX_CBString(pDX, IDC_StockSymbol, m_StockSymbol);
	DDV_MaxChars(pDX, m_StockSymbol, 255);
	DDX_Control(pDX, IDC_StockPriceResult, c_StockPriceResult);
	DDX_Control(pDX, IDC_CompanyNameResult, c_CompanyNameResult);
	DDX_Control(pDX, IDC_Expiration, c_Expiration);
	DDX_CBString(pDX, IDC_Expiration, m_Expiration);
	DDX_Control(pDX, IDC_OptionsList, c_OptionsList);
	DDX_Control(pDX, IDC_Save, c_Save);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COptionChainDialog, CNillaDialog)
	//{{AFX_MSG_MAP(COptionChainDialog)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_OptionsList, OnGetDispInfoOptionsList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_OptionsList, OnColumnClickOptionsList)
	ON_NOTIFY(NM_CLICK, IDC_OptionsList, OnClickOptionsList)
	ON_BN_CLICKED(IDC_Fetch, OnFetch)
	ON_BN_CLICKED(IDC_Save, OnSave)
	ON_CBN_EDITUPDATE(IDC_Expiration, OnEditUpdateExpiration)
	ON_CBN_EDITUPDATE(IDC_StockSymbol, OnEditUpdateStockSymbol)
//	ON_CBN_SELENDOK(IDC_Expiration, OnSelEndOkExpiration )				// occurs before ON_CBN_SELCHANGE() which we already have

	ON_CBN_SELCHANGE(IDC_Expiration, OnSelChangeExpiration)
	ON_CBN_SELCHANGE(IDC_StockSymbol, OnSelChangeStockSymbol)
	ON_CBN_KILLFOCUS(IDC_Expiration, OnKillFocusExpiration)				// HotKeys +
	ON_CBN_KILLFOCUS(IDC_StockSymbol, OnKillFocusStockSymbol)			// HotKeys +
	ON_CBN_SETFOCUS(IDC_Expiration, OnSetFocusExpiration)				// HotKeys
	ON_CBN_SETFOCUS(IDC_StockSymbol, OnSetFocusStockSymbol)				// HotKeys
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()													// HotKeys
	ON_MESSAGE(WM_HOTKEY, OnHotKey)										// HotKeys

	ON_REGISTERED_MESSAGE( WM_NILLAHEDGE_PARSE_DATA, OnParseData )

	ON_NOTIFY( NM_CUSTOMDRAW, IDC_OptionsList, OnCustomDrawOptionsList )
//	ON_CBN_SELENDOK(IDC_Expiration, &COptionChainDialog::OnSelEndOkExpiration )
END_MESSAGE_MAP()

COptionChainDialog::~COptionChainDialog()
{
/*
		// we're done with the internet handle
	if ( hInet )
		if ( ! InternetCloseHandle( hInet ) )
		{
#ifdef _DEBUG
			TRACE( _T("OptionChainDialog::~COptionChainDialog: InternetCloseHandle(hInet) failed.s\n") );
#endif
		}
		// we're done with any session handle that might still be around
	if ( hInetSession )
		if ( ! InternetCloseHandle( hInetSession ) )
		{
#ifdef _DEBUG
			TRACE( _T("OptionChainDialog::~COptionChainDialog: InternetCloseHandle(hInetSession) failed.\n") );
#endif
		}
*/
//	if ( lpServerName )		delete [] lpServerName;
//	if ( lpPageName )		delete [] lpPageName;
//	if ( lpFileName	)		delete [] lpFileName;
//	if ( urlRequest	)		delete [] urlRequest;
	if ( osVer )			delete [] osVer;
	if ( uuid		)		delete [] uuid;
}
//---------------------------------------------------------------------------
/*
void*	COptionChainDialog::LocalHeapNewHandler( size_t size )
{
	return	( localHeap == NULL )  ?  NULL  :  HeapAlloc( localHeap, HEAP_NO_SERIALIZE, size );
}			// LocalHeapNewHandler()
*/
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// COptionChainDialog message handlers

BOOL	COptionChainDialog::OnInitDialog() 
{	DWORD	slen;											// we're ignoring the length returned
	bool	enableSymbol = false;
	CNillaDialog::OnInitDialog();
	RegisterHotKeys( IDC_StockSymbol, IDC_Expiration );		// screens for WM5 internally

		// no hInet handle (a dialog instance variable) to reuse from the last pass...
		// asynchronous operation is only supported in InternetReadFile() and InternetQueryDataAvailable()
		// so it's best NOT to use INTERNET_FLAG_ASYNC for the last parameter
		// For now, we'll masquerade as MSIE 6.0:  Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)
	CString optDir = theApp.optMgr->GetDBdir();
	slen = optDir.GetLength();
	wchar_t* fName = _T("ocr.pnh");
	if ( theApp.lpFileName == NULL )
	{	theApp.lpFileName = new wchar_t[ slen + wcslen(fName) + 2 ];		// +1 for '\0', +1 for the backslash
		if ( theApp.lpFileName )
			swprintf( theApp.lpFileName, _T("%s\\%s"), optDir, fName );
	}
			// get the server name from the registry...
	if ( theApp.lpServerName == NULL )
		GetOCRServer( (BYTE**)&theApp.lpServerName, &slen );
	if ( theApp.lpPageName == NULL )
		GetOCRPage( (BYTE**)&theApp.lpPageName, &slen );				// modified in OnFetch()
	if ( osVer == NULL )
		osVer = GetThinString( GetOsVersion() );						// on the heap
	if ( uuid == NULL )
		uuid = CString2char( GetSerialNumber() );
//		CString	uuid = _T("BC000DE6DF85D2D138000050BFE45CE5");			// good UUID
//		CString	uuid = _T("XC000DE6DF85D2D138000050BFE45CE5");			// bad UUID

#ifdef _DEBUG
	{
		CString sn = theApp.lpServerName;
		CString pn = theApp.lpPageName;
		TRACE( _T("OptionChainDialog::OnInitDialog: lpServerName=%s, lpPageName=%s\n"), sn, pn );
	}
#endif
	c_Fetch.EnableWindow( FALSE );								// need a stock symbol first

		// OnClickOptionsList() is in charge of the Save button's state
	c_Save.EnableWindow( FALSE );

		// Expiration is enabled when Links are present...  see ImportOptionChain()
	c_Expiration.EnableWindow( FALSE );

	enableSymbol = uuid  &&  osVer  && theApp.lpFileName  && theApp.lpServerName  &&  theApp.lpPageName;
	if ( enableSymbol )
	{
			// set up the StockSymbol ComboBox
		CMapStringToPtr* stocksMap = theApp.stkMgr->GetSymbolTable();
		c_StockSymbol.LoadMap( stocksMap );
		InsertColumns();									// initialize the OptionsList
#ifdef	_DEBUG
//		TraceMemoryStatus(0);
#endif
			// prep for MessageBox posts when a stock price could be updated
		b_saveVerifyStockPriceUpdate = false;							// the default is to ask first
		short	prefs = OptionChainStockPriceUpdateVerify;
		if ( GetSaveVerifyPrefs(&prefs) )								// was:  theApp.regMgr->
		{	short	mask = OptionChainStockPriceUpdateVerify;
			b_saveVerifyStockPriceUpdate = (prefs & mask) != 0;
		}
	}
	c_StockSymbol.EnableWindow( enableSymbol );					// if we can't get OCRServer, OCRPage, osVer, and UUID, we won't succeed anyway
	return	TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//--------------------------------------------------------------------------------------
void	COptionChainDialog::OnFetch() 
{		// download and parse an html file for underlying stock's options
		// possibly qualified by an expiration date
	HANDLE	hThread;
	int		slen;
	short	res;
	short	retVal;
	DWORD	mimelen;
	char*	clrBuf;
	char*	huidHex;
	hwnds_t* zz = NULL;

	unsigned long OCRdscfg[ 2*AES_BLOCK_SIZE/sizeof(long) ] =		// we'll only use 3/4 of these in RC6
	{	0x050D3E8F,	0xB64EF204,	0x2832AEBC, 0xD82A2016,			
		0x4D9CA148,	0xC359A1AD,	0xDDD11A43, 0x0A2C2D67,			
		0x9B856BCE,	0xE32C712A,	0x2D11B605, 0x6A27F85E,			
		0x79B10B96,	0xDC535698,	0x1DA7F92B,	0x21384956
	};
	c_Fetch.EnableWindow( FALSE );
	if ( osVer == NULL  ||  m_StockSymbol[0] == _T('\0') )
		return;
//	SipShowIM( SIPF_OFF );										// hide the input panel
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );					// express a preference instead of forcing it down
	char* stkSym = NULL;
	char* mimeBuf = NULL;

		// An example fetch url...								   /q/op?s=intc&m=2005-05
	if ( m_StockSymbolForOptionChain != _T("")  &&  m_StockSymbolForOptionChain != m_StockSymbol )
		PrepareForNewStockOptions();			// clean out the options chain
	m_StockSymbolForOptionChain = m_StockSymbol;

		// stkSym is good!
		// if we're still here, we'll use yrSt and monIdx in the urlRequest
	char expBuf[16] = { 0 };
	m_specificDateRequested = m_Expiration != _T("");
#ifdef _DEBUG
	TRACE( _T("OptionChainDialog::OnFetch: m_Expiration=%s\n"), m_Expiration );
#endif
	if ( m_specificDateRequested )
	{		// verify that m_Expiration is in already in the c_Expiration list
		if ( c_Expiration.FindStringExact(-1, m_Expiration) < 0 )
			return;

		short	monIdx;									// Jan is 0-based
		CString	yrSt;
		if ( ! VerifyExpiration(yrSt, monIdx) )			// also breaks the date string into pieces
			return;
		char* tmpStr = CString2char(yrSt);						// memory leak repaired
		sprintf( expBuf, "?exp=20%s-%02d", tmpStr, monIdx+1 );
		delete [] tmpStr;
	}

		// sometimes m_StockSymbol.GetLength() doesn't work...
	stkSym = CString2char( m_StockSymbol );				// must delete stkSym later
	if ( stkSym == NULL )
		goto Exit;

		// decode the license
	retVal = GetDeviceInfo( theApp.dv, theApp.uu, theApp.hu, theApp.na, theApp.em );
	if ( retVal != 0 )
		goto Exit;										// error condition

	huidHex = NULL;
#if	( _WIN32_WCE >= 0x500 )
	BYTE	huid[48];
	DWORD	lenOut = 47;
	char*	appData = "pcktNumerixNillaHedge";
	HRESULT hr = GetDeviceUniqueID( (BYTE*)appData, strlen(appData), 1, huid, &lenOut );
		// HEX ANSI  <--  BYTE* (actually a very long integer)
	if ( hr == S_OK )
		huidHex = MakeHexString( huid, lenOut );		// uuidHex is little-endian reversed
#endif
	clrBuf = RandomizeParams( osVer, theApp.dv, uuid, huidHex, stkSym, expBuf );
	if ( huidHex ) delete [] huidHex;

	mimelen = 0;
	res = EEcode( clrBuf, OCRdscfg, mimeBuf, mimelen );
	delete [] clrBuf;
	if ( res < 0 )
		goto Exit;

	char largeBuf[ 512 ];
	sprintf( largeBuf, "/%s?ds=%s", theApp.lpPageName, mimeBuf );

	slen = strlen( largeBuf );
	if ( theApp.urlRequest ) delete [] theApp.urlRequest;
	theApp.urlRequest = new char[ slen+1 ];
	strncpy( theApp.urlRequest, largeBuf, slen+1 );

#ifdef _DEBUG
	{	CString	msg = theApp.urlRequest;
		TRACE( _T("OptionChainDialog::OnFetch: urlRequest=%s\n"), msg );
	}
#endif
		// the only part that needs to be enabled to use a second thread
	DWORD	dwThreadId;
	zz = new hwnds_t;
	zz->ocrDialog = GetSafeHwnd();
	zz->fetchButton = c_Fetch.GetSafeHwnd();
	zz->statusResult = c_StatusResult.GetSafeHwnd();
		// the thread is responsible for deleting the hwnds_t struct
	hThread = CreateThread( NULL, 0, DownloadURL, (void*)zz, 0, &dwThreadId );
Exit:
	if ( stkSym ) delete stkSym;
	if ( mimeBuf ) delete [] mimeBuf;
}			// OnFetch()
//--------------------------------------------------------------------------------------
void	COptionChainDialog::InsertColumns( void )
{		// in the order they will be displayed in the list control
	wchar_t*		colHdrs[] = {
		_T("PC"),  _T("Strike"), _T("Expiry"), _T("Symbol"), _T("Last"),
		_T("Chg"), _T("Bid"),	 _T("Ask"),	   _T("Volume"), _T("OpenInt"), _T("")	};

	int	flags = LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT;
	c_OptionsList.SetExtendedStyle( c_OptionsList.GetExtendedStyle() | flags );

		// add columns to the List Control
	wchar_t	nCols = 0;
	c_OptionsList.InsertColumn( nCols, colHdrs[nCols++], LVCFMT_LEFT,  26 );	// P/C			 0 - have color too
	c_OptionsList.InsertColumn( nCols, colHdrs[nCols++], LVCFMT_RIGHT, 44 );	// Strike		 1
	c_OptionsList.InsertColumn( nCols, colHdrs[nCols++], LVCFMT_RIGHT, 48 );	// Expiry		 2
	c_OptionsList.InsertColumn( nCols, colHdrs[nCols++], LVCFMT_LEFT,  66 );	// Symbol		 3
	c_OptionsList.InsertColumn( nCols, colHdrs[nCols++], LVCFMT_RIGHT, 45 );	// Last			 4
	c_OptionsList.InsertColumn( nCols, colHdrs[nCols++], LVCFMT_RIGHT, 42 );	// Chg			 5
	c_OptionsList.InsertColumn( nCols, colHdrs[nCols++], LVCFMT_RIGHT, 45 );	// Bid			 6
	c_OptionsList.InsertColumn( nCols, colHdrs[nCols++], LVCFMT_RIGHT, 45 );	// Ask			 7
	c_OptionsList.InsertColumn( nCols, colHdrs[nCols++], LVCFMT_RIGHT, 42 );	// Vol			 8
	c_OptionsList.InsertColumn( nCols, colHdrs[nCols++], LVCFMT_RIGHT, 60 );	// OpenInt		 9
	c_OptionsList.InsertColumn( nCols, colHdrs[nCols  ], LVCFMT_RIGHT, 14 );	// Dummy Column	10 - the original insertion idx lives here

		// for use by ColumnClick and the Compare function
	lastSortKey = -1;		// this is a 1-based column index
}			// InsertColumns()
//--------------------------------------------------------------------------------------
void COptionChainDialog::OnEditUpdateStockSymbol() 
{	CString	sym;
	c_StockSymbol.GetWindowText( sym );
	sym.MakeUpper();
	KillFocusStockSymbolWork( sym );
}			// OnEditUpdateStockSymbol()
//--------------------------------------------------------------------------------------
void	COptionChainDialog::KillFocusStockSymbolWork( CString sym )
{
	if ( sym == m_StockSymbol )
		return;						// nothing to do
		// save company name if the stock exists and desc is blank
		// this occurs in OnParseData now
//	SaveIfChanged();
	m_StockSymbol = sym;
	c_Fetch.EnableWindow( m_StockSymbol.GetLength() > 0 );
}			// KillFocusStockSymbolWork()
//--------------------------------------------------------------------------------------
void	COptionChainDialog::OnKillFocusStockSymbol( void )
{	CString sym;
	m_activeComboBox = NULL;
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	c_StockSymbol.GetWindowText( sym );
	sym.MakeUpper();
	KillFocusStockSymbolWork( sym );
}			// OnKillFocusStockSymbol()
//--------------------------------------------------------------------------------------
void	COptionChainDialog::OnSetFocusStockSymbol( void )
{
	m_activeComboBox = IDC_StockSymbol;
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusStockSymbol()
//----------------------------------------------------------------------------------------
void COptionChainDialog::OnSelChangeStockSymbol()
{	CString sym;
	int cnt = c_StockSymbol.GetCount();
	if ( cnt < 1 )
		return;
	int sel = c_StockSymbol.GetCurSel();
#if ( _WIN32_WCE >= 0x420 )					// just WM5
	if ( c_StockSymbol.lastChar == VK_DOWN  ||  c_StockSymbol.lastChar == VK_UP )
	{	int	delta = ( c_StockSymbol.lastChar == VK_DOWN  ?  1  :  -1 );
		sel += delta;
		if ( sel < 0 )
			sel = cnt - 1;
		if ( sel >= cnt )
			sel = 0;
		c_StockSymbol.SetCurSel( sel );
	}
#endif
	if ( sel < 0 ) sel = 0;					// should be superfluous
	c_StockSymbol.GetLBText( sel, sym );
	KillFocusStockSymbolWork( sym );
}			// OnSelChangeStockSymbol()
//--------------------------------------------------------------------------------------
void	COptionChainDialog::KillFocusExpirationWork( void )
{		// if the date selected is not the one currently loaded in the list box,
		// fetch the webpage for the option chain for the selected date	and
		// load the list box with the items found

		// m_Expiration verified by m_Links.Lookup
		// We don't do anything with the yrSt & monIdx returned
		// lookup m_Expiration in the m_Links list
	long	idx = -1;
	if ( ! m_Links.Lookup( m_Expiration, (void*&)idx ) )
		c_Expiration.SetEditSel( -1, -1 );
	else
		SortOptionsAndScrollTo( idx );
}			// KillFocusExpirationWork()
//--------------------------------------------------------------------------------------
void	COptionChainDialog::OnKillFocusExpiration( void )
{
	m_activeComboBox = NULL;
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	KillFocusExpirationWork();
}			// OnKillFocusExpiration()
//--------------------------------------------------------------------------------------
void	COptionChainDialog::OnSelChangeExpiration( void )
{
	int cnt = c_Expiration.GetCount();
	if ( cnt < 1 )
		return;
	long sel = c_Expiration.GetCurSel();		// this is what we use when LB Closeup happened first
#if ( _WIN32_WCE >= 0x420 )							// just WM5
	if ( c_Expiration.lastChar == VK_DOWN  ||  c_Expiration.lastChar == VK_UP )
	{	int	delta = ( c_Expiration.lastChar == VK_DOWN  ?  1  :  -1 );
		sel += delta;
		if ( sel < 0 )
			sel = cnt - 1;
		if ( sel >= cnt )
			sel = 0;
		c_Expiration.SetCurSel( sel );
	}
#endif
	if ( sel < 0 ) sel = 0;							// should be superfluous
#ifdef _DEBUG
	TRACE( _T("OptionChainDialog::OnSelChangeExpiration: m_Expiration=%s before GetLBText\n"), m_Expiration );
#endif
	c_Expiration.GetLBText( sel, m_Expiration );
#ifdef _DEBUG
	TRACE( _T("OptionChainDialog::OnSelChangeExpiration: m_Expiration=%s after GetLBText\n"), m_Expiration );
#endif
	EnableFetch( m_Expiration );

	BOOL found = m_Links.Lookup( m_Expiration, (void*&)sel );		// reuse sel
	if ( found )
		SortOptionsAndScrollTo( sel );
}			// OnSelChangeExpiration()
//-------------------------------------------------------------------------------------------------
void	COptionChainDialog::OnSetFocusExpiration( void )
{
	m_activeComboBox = IDC_Expiration;
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusExpiration()
//----------------------------------------------------------------------------------------
LRESULT		COptionChainDialog::OnHotKey( WPARAM wParam, LPARAM lParam )
{		//	UINT fuModifiers = (UINT)LOWORD(lParam);		// don't care about modifiers
		//	DWORD ctrlID = (int)wParam;						// don't care which control the HotKey is registered to (any more)
		//	CWnd* wnd = GetDlgItem( ctrlID );				// also about the registered control
	if ( ! m_activeComboBox )
		return	0;											// nothing to do

	UINT uVirtKey = (UINT)HIWORD(lParam);
	if ( uVirtKey != VK_DOWN  &&  uVirtKey != VK_UP )
		return	0;													// only care about the up/down arrow keys

	CSeekComboBox* scb = (CSeekComboBox*)GetDlgItem( m_activeComboBox );
	scb->lastChar = uVirtKey;										// this in combination with SendMessage()
	WPARAM wp = MAKEWPARAM( m_activeComboBox, CBN_SELCHANGE );
	SendMessage( WM_COMMAND, wp, (LPARAM)scb->GetSafeHwnd() );		// emulates VK_UP/VK_DOWN as on PPC'02
	return	0;
}			// OnHotKey()
//----------------------------------------------------------------------------
void	COptionChainDialog::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	CNillaDialog::OnActivate( nState, pWndOther, bMinimized );
	if ( nState == WA_ACTIVE  ||  nState == WA_CLICKACTIVE )
		RegisterHotKeys( IDC_StockSymbol, IDC_Expiration );			// screens for WM5 internally
}			// OnActivate()
//----------------------------------------------------------------------------
/*
void	COptionChainDialog::SaveIfChanged()
{		// if we have a company name and the stock is in the database
		// but its description is not filled in, store the company name there
	if ( m_CompanyName != _T("") )
	{	CStock*	stk = theApp.stkMgr->GetStock( m_StockSymbol );
		if ( stk  &&  stk->desc_ID < 0 )
		{		// stock definition found with blank description
			stk->setDesc( m_CompanyName );
			stk->saveDesc();										// make it persistent
			theApp.stkMgr->WriteStock( stk );						// rewrite the stock too
		}
		delete	stk;
	}
}			// SaveIfChanged()
*/
//--------------------------------------------------------------------------------------
bool	COptionChainDialog::VerifyExpiration( CString& yrSt, short& monIdx )
{		// in case GetLength() is not working, we could try...

		// is GetLength() working?
	int	expLen = m_Expiration.GetLength();
	if ( expLen < 1 )
		return	true;										// _T("") is okay too

	const	CString numeric( _T("0123456789") );

	CString	moSt = m_Expiration.Mid( 0, 3 );
	monIdx = intMonthFrom3CharStr( moSt );					// Jan is 0-based here
	ASSERT( monIdx >= 0  &&  monIdx <= 11 );

	yrSt = m_Expiration.Mid( 3 );							// get the remainder of monYr
	yrSt = yrSt.SpanIncluding( numeric );					// does yrSt has any non-numeric characters

	bool   verified = monIdx >= 0  &&  yrSt.GetLength() == 2;
	if ( ! verified )
	{	// c_Expiration.SetWindowText( m_Expiration );
		c_Fetch.EnableWindow( FALSE );
		m_Expiration = _T("");
		c_Expiration.SetEditSel( -1, -1 );
		c_Expiration.SetFocus();
	}		
	return	verified;
}			// VerifyExpiration()
//--------------------------------------------------------------------------------------
void	COptionChainDialog::PrepareForNewStockOptions( void )
{
		// kill all entries in the OptionsList (why doesn't this work?)
	int	nOptions = c_OptionsList.GetItemCount();
	for ( short ii = 0; ii < nOptions; ii++ )
	{	OptionChainItemInfo* pInfo = (OptionChainItemInfo*)c_OptionsList.GetItemData( ii );
		delete	pInfo;
	}
	m_nCalls = 0;
	m_nPuts = 0;

		// DeleteAllItems() is NOT superfluous,
		// some stuff always gets left behind by the for loop above
	c_OptionsList.DeleteAllItems();
	c_StatusResult.SetWindowText( _T("") );		// no items --> status should be ""

	c_Save.EnableWindow( FALSE );				// no items, thus impossible to Save anything
	c_StockPriceResult.SetWindowText( _T("") );
	c_CompanyNameResult.SetWindowText( _T("") );

		// kill all elements in the Expiration ComboBox
	c_Expiration.ResetContent();				// wipe out the Expiry dates
	c_Expiration.EnableWindow( FALSE );
	m_Expiration = _T("");						// wipe out the currently selected value too
	m_Links.RemoveAll();						// instead of the paragraph below
/*
		// kill all elements in the m_Links map
	CString		key;
	LinkInfo*	pLinkInfo;		// key value is a date string, target value is a urlTail
	POSITION	pos = m_Links.GetStartPosition();
	while ( pos != NULL )
	{	m_Links.GetNextAssoc( pos, key, (void*&)pLinkInfo );
		m_Links.RemoveKey( key );
//		delete	pLinkInfo;						// it's just an int now
	}
*/
}			// PrepareForNewStockOptions()
//--------------------------------------------------------------------------------------
LRESULT		COptionChainDialog::OnParseData( WPARAM, LPARAM )
{	DWORD			retVal = 0;
	CString			expSt;
	CFile			fp;
	CString			csFileName = theApp.lpFileName;
	long			nBeforeOptions = m_nPuts + m_nCalls;

	if ( ! fp.Open( csFileName, CFile::modeRead ) )
	{
#ifdef _DEBUG
		{	CString	msg = theApp.lpFileName;
			TRACE( _T("OnParseData(-1): Can't open(r) '%s'\n"), msg );
		}
#endif
		wchar_t	wbuf[64];
		swprintf( wbuf, _T("Can't open(r) %s\n"), theApp.lpFileName );
		c_StatusResult.SetWindowText( wbuf );
		return  -1;
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// parse phase
	c_StatusResult.SetWindowText( _T("Reading...") );
	theApp.DoWaitCursor( 1 );									// reset(-1) at Cleanup

		// title
	char*	title = NULL;
	if ( ReadPast( fp, "<title>" )  &&  GetToken( fp, "</title>", title ) )
	{	char*	invalidSymbol = "Invalid Ticker Symbol";
		bool found = ( FindSubStr( title, 0, invalidSymbol ) >= 0 );
		delete [] title;
		if ( found )
		{	CString cs = invalidSymbol;
			c_CompanyNameResult.SetWindowText( cs );
			c_StatusResult.SetWindowText( _T("") );
			goto	CloseFile;								// technically, a successful fetch
		}
	}
	else
	{	retVal = -2;
		goto	CloseFile;
	}

		// companyName
	char*	companyName;
	companyName = NULL;
	if ( ReadPast( fp, "<CoName>" )  &&  GetToken( fp, "</CoName>", companyName ) )
	{	CString cs = companyName;
		m_CompanyName = companyName;						// to CString conversion
		c_CompanyNameResult.SetWindowText( cs );
		delete [] companyName;
	}
	else
	{	retVal = -3;
		goto	CloseFile;
	}

		// stockPrice
	char*	stockPrice;
	stockPrice = NULL;
	if ( ReadPast( fp, "<StkPrc>" )  &&  GetToken( fp, "</StkPrc>", stockPrice ) )
	{
		HandleStockPrice( stockPrice );
		delete [] stockPrice;
	}
	else
	{	retVal = -4;
		goto	CloseFile;
	}

		// option expiration (<exp> and <date> entries)
	char*	expBuf;
	expBuf = NULL;				// can be put directly into the list control
	if ( ! ReadPast( fp, "<exp>" )  ||  ! GetToken( fp, "</exp>", expBuf ) )
	{	retVal = -5;
		goto	CloseFile;
	}
	expSt = expBuf;
	delete [] expBuf;

		// expiration housekeeping (the options list needs the coded value)
	long	codedExpiry;
	codedExpiry = MakeLongMonYr( expSt );
	short	nOptions;
	nOptions = m_nCalls + m_nPuts;

//	wchar_t*	expLit = _T("&exp");
//	bool   specificDateRequested = ( urlRequest.Find( expLit, 0 ) >= 0 );
//	bool   specificDateRequested;													// with encrypted param strings, this needs to be a dialog instance variable
//	specificDateRequested = ( FindSubStr(urlRequest, 0, "&exp") >= 0 );
	if ( ! m_specificDateRequested )
	{		// we didn't requested any specific expiration...
			// if there are any expiration dates, they're for another stock
		c_Expiration.SetRedraw( FALSE );
		if ( c_Expiration.FindStringExact( -1, expSt ) == LB_ERR )
		{
#ifdef _DEBUG
			TRACE( _T("OptionChainDialog::OnParseData: adding %s to c_Expiration\n"), expSt );
#endif
			c_Expiration.AddString( expSt );
		}
			// any other expiration dates?
		char*	expDates = NULL;
		if ( ReadPast( fp, "<Dates>" ) )
		{	unsigned long	b4expDates = (unsigned long)fp.GetPosition();			// after "<Dates>"
			if ( GetToken( fp, "</Dates>", expDates) )
			{	unsigned long	afterDates = (unsigned long)fp.GetPosition();
				long	datesLen = afterDates - b4expDates - 8;						// 8 = strlen("</Dates>");
				if ( datesLen > 0 )
				{	long	ii = 0;													// index within expDates
					while ( 1 )
					{	ii = FindSubStr( expDates, ii, "<date>" );
						if ( ii < 0 )
							break;
						ii += 6;									// length of "<date>"
						long jj = FindSubStr( expDates, ii, "</date>" );
						if ( jj < 0 )	
							break;
						char*	aDate = ExtractToken( expDates+ii, jj - ii );
						CString	cs = aDate;
						delete [] aDate;
						if ( c_Expiration.FindStringExact( -1, cs ) == LB_ERR )
						{
#ifdef _DEBUG
							TRACE( _T("OptionChainDialog::OnParseData: adding %s to c_Expiration\n"), cs );
#endif
							c_Expiration.AddString( cs );
						}
						ii = jj + 7;								// skip past the "</date>"
					}
				}
				delete [] expDates;
			}
		}
	}
	c_Expiration.SetRedraw( TRUE );							// may have to Invalidate and UpdateWindow later

		// handle the <Calls> (ii=0) and <Puts> (ii=1)
	c_OptionsList.SetRedraw( FALSE );						// inhibit redraw until all the options are inserted
	short	nOptionsLoaded;
	nOptionsLoaded = 0;
	char*	startToken[2];			// e.g. char*	startToken[2] = { "<Calls>", "<Puts>" };
	startToken[0] = "<Calls>";
	startToken[1] = "<Puts>";		// presumes that the Calls come first
	char*	endToken[2];			// e.g. char*	endToken[2] = { "</Calls>", "</Puts>" };
	endToken[0] = "</Calls>";
	endToken[1] = "</Puts>";

	short	lenEndToken[2];			// e.g. lenEndToken[2] = { 8, 7 };
	lenEndToken[0] = 8;
	lenEndToken[1] = 7;

	char*	allOpts;				// in VS'05, you can do:  char*	allOpts = NULL;
	allOpts = NULL;
	unsigned short ii;
	for ( ii = 0; ii < 2; ii++ )
	{	bool isCall = ( ii == 0 );
		bool readSuccess = ReadPast( fp, startToken[ii] );
#ifdef _DEBUG
		{	CString cs = startToken[ii];
			TRACE( _T("OptionChainDialog::OnParseData: ReadPast(%s) --> %s\n"), cs, (readSuccess ? _T("true") : _T("false")) );
			if ( ! readSuccess )
				AfxDebugBreak();
		}
#endif
		if ( readSuccess )
		{	unsigned long	b4Opts = (unsigned long)fp.GetPosition();				// after "<Calls>" or "<Puts>"
			bool gotToken = GetToken( fp, endToken[ii], allOpts );
#ifdef _DEBUG
			{	CString cs = endToken[ii];
				TRACE( _T("OptionChainDialog::OnParseData: GetToken(%s) --> %s\n"), cs, (gotToken ? _T("true") : _T("false")) );
				if ( ! gotToken )
					AfxDebugBreak();
			}
#endif
			if ( gotToken )
			{
#ifdef _DEBUG
//				{	CString cs = allOpts;		// the following TRACE causes an exception
//					TRACE( _T("OptionChainDialog::OnParseData: allOpts=%s\n"), cs );
//				}
#endif
				unsigned long afterOpts = (unsigned long)fp.GetPosition();
				long	optsLen = afterOpts - b4Opts - strlen(endToken[ii]);
#ifdef _DEBUG
				TRACE( _T("OptionChainDialog::OnParseData: b4Opts=%d, afterOpts=%d, optsLen=%d\n"), b4Opts, afterOpts, optsLen );
#endif
				if ( optsLen > 0 )
				{	long	jj = 0, kk;									// index within expDates
					char*	buf;
					while ( 1 )
					{	jj = FindSubStr( allOpts, jj, "<option>" );		if ( jj < 0 ) break;
						jj += 8;																// 8 = length of "<option>"
						kk = FindSubStr( allOpts, jj, "</option>" );	if ( kk < 0 ) break;
						char*	optBuf = ExtractToken( allOpts+jj, kk - jj );
						if ( optBuf == NULL )
						{
#ifdef _DEBUG
							TRACE( _T("OptionChainDialog::OnParseData: optBuf --> NULL\n") );
							AfxDebugBreak();
#endif
							break;
						}
#ifdef _DEBUG
						{	CString cs = optBuf;
							TRACE( _T("OptionChainDialog::OnParseData: optBuf=%s, <option> at jj=%d, </option> at kk=%d\n"), cs, jj, kk );
						}
#endif
						jj = kk;									// saves time next pass through the loop

							//-------------------------------------------------------
							// symbol
						kk = FindSubStr( optBuf, 0, "<sym>" );			if ( kk < 0 ) break;
						kk += 5;																// pass up "<sym>"
						long ll = FindSubStr( optBuf, kk, "</sym>" );	if ( ll < 0 ) break;
						buf = ExtractToken( optBuf+kk, ll - kk );
						CString	symbol = buf;
						kk = ll + 6;															// pass up "</sym>"
						delete [] buf;

							// strike price
						kk = FindSubStr( optBuf, kk, "<strk>" );		if ( kk < 0 ) break;
						kk += 6;																// pass up "<strk>"
						ll = FindSubStr( optBuf, kk, "</strk>" );		if ( ll < 0 ) break;
						buf = ExtractToken( optBuf+kk, ll - kk );
						float	strike;
						if ( sscanf( buf, "%f", &strike ) < 1 )
							strike = -1;
						kk = ll + 7;															// pass up "</strk>"
						delete [] buf;

							// last
						kk = FindSubStr( optBuf, kk, "<last>" );		if ( kk < 0 ) break;
						kk += 6;																// pass up "<last>"
						ll = FindSubStr( optBuf, kk, "</last>" );		if ( ll < 0 ) break;
						buf = ExtractToken( optBuf+kk, ll - kk );
						float	last;
						if ( sscanf( buf, "%f", &last ) < 1 )
							last = -1;
						kk = ll + 7;															// pass up "</last>"
						delete [] buf;

							// one day change
						kk = FindSubStr( optBuf, kk, "<chng>" );		if ( kk < 0 ) break;
						kk += 6;																// pass up "<chng>"
						ll = FindSubStr( optBuf, kk, "</chng>" );		if ( ll < 0 ) break;
						buf = ExtractToken( optBuf+kk, ll - kk );
						float	change;
						if ( sscanf( buf, "%f", &change ) < 1 )
							change = -1;
						kk = ll + 7;															// pass up "</chng>"
						delete [] buf;

							// bid
						kk = FindSubStr( optBuf, kk, "<bid>" );			if ( kk < 0 ) break;
						kk += 5;																// pass up "<bid>"
						ll = FindSubStr( optBuf, kk, "</bid>" );		if ( ll < 0 ) break;
						buf = ExtractToken( optBuf+kk, ll - kk );
						float	bid;
						if ( sscanf( buf, "%f", &bid ) < 1 )
							bid = -1;
						kk = ll + 6;															// pass up "</bid>"
						delete [] buf;

							// ask
						kk = FindSubStr( optBuf, kk, "<ask>" );			if ( kk < 0 ) break;
						kk += 5;																// pass up "<ask>"
						ll = FindSubStr( optBuf, kk, "</ask>" );		if ( ll < 0 ) break;
						buf = ExtractToken( optBuf+kk, ll - kk );
						float	ask;
						if ( sscanf( buf, "%f", &ask ) < 1 )
							ask = -1;
						kk = ll + 6;															// pass up "</ask>"
						delete [] buf;

							// volume
						kk = FindSubStr( optBuf, kk, "<vol>" );			if ( kk < 0 ) break;
						kk += 5;																// pass up "<vol>"
						ll = FindSubStr( optBuf, kk, "</vol>" );		if ( ll < 0 ) break;
						buf = ExtractToken( optBuf+kk, ll - kk );
						int	volume;
						if ( sscanf( buf, "%d", &volume ) < 1 )
							volume = -1;
						kk = ll + 6;															// pass up "</vol>"
						delete [] buf;

							// opening interest
						kk = FindSubStr( optBuf, kk, "<opnInt>" );		if ( kk < 0 ) break;
						kk += 8;																// pass up "<opnInt>"
						ll = FindSubStr( optBuf, kk, "</opnInt>" );		if ( ll < 0 ) break;
						buf = ExtractToken( optBuf+kk, ll - kk );
						int	openInt;
						if ( sscanf( buf, "%d", &openInt ) < 1 )
							openInt = -1;
						kk = ll + 9;															// pass up "</opnInt>"
						delete [] buf;

						OptionChainItemInfo*	pInfo = new OptionChainItemInfo;
						pInfo->ask = ask;
						pInfo->bid = bid;
						pInfo->change = change;
						pInfo->expiry = codedExpiry;
						pInfo->idx = c_OptionsList.GetItemCount();
						pInfo->isCall = isCall;
						pInfo->last = last;
						pInfo->openInt = openInt;
						pInfo->strike = strike;
						pInfo->symbol = symbol;
						pInfo->volume = volume;
						UpdateDB( pInfo );

						c_OptionsList.AddItem( (LPARAM)pInfo, pInfo->idx );
						nOptionsLoaded++;
						if ( ii == 0 )
							m_nCalls++;
						else
							m_nPuts++;
#ifdef _DEBUG
						TRACE( _T("OptionChainDialog::OnParseData: added %s, nOptionsLoaded=%d, m_nCalls=%d, m_nPuts=%d\n"),
								pInfo->symbol, nOptionsLoaded, m_nCalls, m_nPuts );
#endif
							//-------------------------------------------------------
						delete [] optBuf;
					}						// while option XML lines can be found
				}							// option XML lines exist
				delete [] allOpts;
			}								// GetToken( </Calls> or </Puts> ) succeeded
		}									// ReadPast( <Calls> or <Puts> ) succeeded
	}										// for ( ii = 0; ii < 2; ii++ )

		// track the (date,url) pair in m_Links
	if ( nOptionsLoaded > 0 )
	{	m_Links.SetAt( expSt, (void*)nOptions );		// the pre-load total of Calls and Puts
		c_Expiration.SetCurSel( c_Expiration.FindString(0, expSt) );
	}
	else
	{	nOptions = m_nCalls + m_nPuts;
		if ( nOptions == 0 )
		{	int	cnt = c_Expiration.GetCount();
			for ( short ii = 0; ii < cnt; ii++ )
				c_Expiration.DeleteString( 0 );
//			c_Expiration.Clear();			// the loop above is there because Clear() doesn't work
		}
	}
	DisplayStatusLine();					// the number of calls and puts
	c_OptionsList.SetRedraw( TRUE );		// now we can let it redraw
											// downloaded file has non-zero length

CloseFile:
	fp.Close();
	theApp.DoWaitCursor(-1);

//	childThreadActive = false;			// we're not using this at the moment
//	c_Fetch.EnableWindow( TRUE );
//	c_Fetch.SetWindowText( _T("Fetch") );
	c_Expiration.EnableWindow( c_Expiration.GetCount() > 0 );
	if ( nOptionsLoaded > 0 )
		SortOptionsAndScrollTo( nBeforeOptions );

	return	retVal;
}			// OnParseData()
//-------------------------------------------------------------------------------------------------
void	COptionChainDialog::DisplayStatusLine( void )
{	CString	message;
	message.Format( _T("%d calls, %d puts"), m_nCalls, m_nPuts );
	c_StatusResult.SetWindowText( message );
}			// DisplayStatusLine()
//--------------------------------------------------------------------------------------
void		COptionChainDialog::HandleStockPrice( char*	szStockPrice )
{		// other inputs:  m_CompanyName, c_StockPriceResult, m_StockSymbol
		// side effects:  sets m_StockPrice, updates the database (if m_StockSymbol is in DB)

		// update stock price display in the dialog
	if ( strcmp( szStockPrice, "???" ) != 0 )
	{	CString	stockPrice = szStockPrice;
		c_StockPriceResult.SetWindowText( stockPrice );
			// set the m_StockPrice instance var
		sscanf( szStockPrice, "%f", &m_StockPrice );
	}

		// see if there's a stock to update in the database
	CStock*	stk = theApp.stkMgr->GetStock( m_StockSymbol );
	if ( stk )
	{		// stock definition found
		if ( stk->getMktPrice() != m_StockPrice )
		{		// the current (downloaded) stock price is NOT equal to the stored value
			bool	update = true;
			if ( b_saveVerifyStockPriceUpdate )
			{		// need user confirmation that stock price should be updated...
				wchar_t	buf[320];
				swprintf( buf, _T("%s (%s) price (%g) will overwritten by %g?"),
					m_CompanyName, m_StockSymbol, m_StockPrice, stk->getMktPrice() );
				UINT result = MessageBox( buf, _T("Update stock price?"),
								MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST );
				update = (result == IDYES);
			}
			if ( update )
			{	stk->setMktPrice( m_StockPrice );
					// if the description is currently null, put the company name into it
				if ( stk->desc_ID < 0  &&  m_CompanyName != _T("") )
				{	stk->setDesc( m_CompanyName );
					stk->saveDesc();
				}
				short	res = theApp.stkMgr->WriteBOS( (CBOS*)stk );
#ifdef _DEBUG
				if ( res != 0 )
					TRACE( _T("OptionChainDialog::HandleStockPrice: BOS write failed, res=%d\n"), res );
#endif
			}
		}
		delete	stk;
	}
}			// HandleStockPrice()
//--------------------------------------------------------------------------------------
bool		COptionChainDialog::GetToken( CFile& fp, char* terminator, char*& token )
{		// we want the stuff between the initial file position and the beginning of the terminator
		// side effects: the file position moves after the terminator
	unsigned long	startPos = (unsigned long)fp.GetPosition();								// the initial file position
	unsigned long	currentPos;
	short	termLen;
	long	nChars;
	bool termFound = ReadPast( fp, terminator );	
	if ( termFound )
	{	currentPos = (unsigned long)fp.GetPosition();
		fp.Seek( startPos, CFile::begin );
		termLen = strlen( terminator );
		nChars = currentPos - termLen - startPos;

			// caller should provide an unused char* to receive token
#ifdef _DEBUG
//		CString	term = terminator;
//		TRACE( _T("OptionChainDialog::GetToken: term=%s, termLen=%d, startPos=%d, currentPos=%d, nChars=%d\n"),
//												term,	   termLen,	   startPos,	currentPos,	   nChars	   );
#endif
		if ( token )
			delete [] token;									// ensure that we don't leak tokens
		SetLastError( 0 );
		token = new char[ nChars + 1 ];
#ifdef _DEBUG
		DWORD	err = GetLastError();
		if ( err )
		{	wchar_t	buf[128];
			FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, NULL, buf, 127, NULL );
			TRACE( _T("OptionChainDialog::GetToken: %s.\n"), buf );
			AfxDebugBreak();
		}
#endif
		if ( nChars == (long)fp.Read( token, nChars ) )
		{		// at this point, termFound is true - can terminate the token and return
			*(token + nChars) = '\0';
			fp.Seek( termLen, CFile::current );						// pass up terminator
		}
		else													// Read() failed
		{	delete [] token;
			token = NULL;		// NOTE: token can be NULL even though termFound is true
		}
	}
#ifdef _DEBUG
//	if ( termFound )
//	{	CString	csTok = token;
//		TRACE( _T("OptionChainDialog::GetToken: token=%s\n"), csTok );
//	}
#endif
	return	termFound;
}			// GetToken()
//--------------------------------------------------------------------------------------
bool		COptionChainDialog::ReadPast( CFile& fp, char* terminator )
{		// side effect: file position moves after the terminator
	char	buf[64];
	short	termLen = strlen( terminator );
	short	te = 0;												// index within terminator
	unsigned long	startPos = (unsigned long)fp.GetPosition();
	unsigned long	fpLen = (unsigned long)fp.GetLength();
	long	bufLen = fp.Read( buf, sizeof(buf) );
	long	bu = 0;												// index within buf
	char	matchChar;
	bool	match = false;
	while ( bu < bufLen  &&  te < termLen )
	{	matchChar = *(terminator + te);
		te = ( *(buf + bu++) == matchChar ) ? (te + 1) : 0;		// bu always increments, te can reset
		match = te >= termLen;
		if ( match )
		{		// move the file pointer to just after the
				// *(buf + bu) we compared above (bu has already moved)
			fp.Seek( startPos + bu, CFile::begin );
			break;												// matched, so we're done
		}
		if ( bu >= bufLen )										// bu has already moved up
		{	startPos += bufLen;									// bufLen will be obliterated next
			bufLen = fp.Read( buf, sizeof(buf) );				// get a new buffer from the file
			bu = 0;												// reset bu
		}
	}
#ifdef _DEBUG
//	unsigned long	tellPos = (unsigned long)fp.GetPosition();
//	CString	csBuf = buf;
//	CString	csTerm = terminator;
//	TRACE( _T("OptionChainDialog::ReadPast: startPos=%d, buf='%s', terminator='%s', tellPos=%d\n"),
//			startPos, csBuf, csTerm, tellPos );
#endif
	return	match;
}			// ReadPast()
//--------------------------------------------------------------------------------------
void		COptionChainDialog::UpdateDB( OptionChainItemInfo* pInfo )
{	COption*	opt = theApp.optMgr->GetOption( pInfo->symbol );
	if ( opt == NULL )
		return;
	if ( opt->getMktPrice() != pInfo->last )
	{	opt->setMktPrice( pInfo->last );
		short res = theApp.optMgr->WriteBOS( (CBOS*)opt );	// less I/O than writeOption
#ifdef _DEBUG
		if ( res != 0 )
			TRACE( _T("OptionChainDialog::UpdateDB: Option write failed, res=%d\n"), res );	
#endif
	}
	delete	opt;
}			// UpdateDB()
//--------------------------------------------------------------------------------------
CString		COptionChainDialog::GetPositionItemText( int rr, int si )
{		// return a CString representation of the data in row rr, subitem si
	ASSERT( rr >= 0  &&  si >= 0 );

		// get the item info for the row of interest (rr)
	OptionChainItemInfo*	pItem = (OptionChainItemInfo*)c_OptionsList.GetItemData( rr );
	if ( pItem == NULL  ||  si > 9 )		// really an error, or the dummy column
		return	_T("");						// not sure why this would happen

		// Columns are...
		//	   0		1		2	   3	   4	  5		6	 7		8		9
		// Put/Call  Strike  Expiry  Symbol  Last  Change  Bid  Ask  Volume  OpenInt

		// 0th column (0-based) - 'Put' or 'Call'
	if ( si == 0 )
		return	pItem->isCall ? _T("Call") : _T("Put");

		// 2nd column (0-based) - Expiry
	CString	formatSpec = _T("%.2f");
	wchar_t	buf[256];			// if needed, where we construct the return string
	if ( si == 2 )
	{	long	mo = pItem->expiry & 0xF;
		long	yr = (pItem->expiry >> 4) - 2000;
		swprintf( buf, _T("%s%02d"), months[mo], yr );
		goto	GoHome;
	}

		// 3rd column (0-based) - Symbol
	if ( si == 3 )
		return	pItem->symbol;

	if ( 1 <= si  &&  si <= 7 )
	{	float	data;
		switch ( si )
		{	case 1:						// 1st column (0-based) - Strike
				data = pItem->strike;
//				formatSpec = _T("%g");
				break;
			case 4:						// 4th column (0-based) - Last
				data = pItem->last;
				break;
			case 5:						// 5th column (0-based) - Chg
				data = pItem->change;
				break;
			case 6:						// 6th column (0-based) - Bid
				data = pItem->bid;
				break;
			case 7:						// 7th column (0-based) - Ask
				data = pItem->ask;
				break;
			default:					// 2nd & 3rd are handled before the switch
				ASSERT( false );
				break;
		}
			// negative bid and ask prices represent "N/A" (i.e. there are no offers) 
		if ( (si == 6  ||  si == 7)  && data < 0.0 )
			return _T("N/A");

		swprintf( buf, formatSpec, data );
	}
	else			// integer data not compatible with float data above
		swprintf( buf, _T("%d"), si == 8 ? pItem->volume		// 8th column (0-based) - Vol
										 : pItem->openInt );	// 9th column (0-based) - OpenInt
GoHome:
	CString	cst( buf );
	return	cst;
}			// GetPositionItemText()
//----------------------------------------------------------------------------------------
void	COptionChainDialog::OnClickOptionsList( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	c_Save.EnableWindow( c_OptionsList.GetSelectedCount() > 0 );
	*pResult = 0;
}			// OnClickOptionsList()
//----------------------------------------------------------------------------------------
void	COptionChainDialog::OnColumnClickOptionsList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLISTVIEW*	pNMLV = (NMLISTVIEW*)pNMHDR;
	short	sortKey = pNMLV->iSubItem;
	if ( sortKey == abs(lastSortKey) )
		sortKey = -lastSortKey;

	c_OptionsList.SortItems( OptionChainCompareFunc, (LPARAM)sortKey );

		// remember for next time
	lastSortKey = sortKey;
	*pResult = 0;
}			// OnColumnClickOptionsList()
//--------------------------------------------------------------------------------------
void	COptionChainDialog::OnSave( void )
{		// the OptionsList control is configured to disallow multiple selections though
		// this code supports multiple selections too.  Allowing multiple selections
		// would encourage jamming all available definitions into the database.
		// Since the database nevers cull definitions (even after options have expired),
		// over time, the database would become clogged with unused definitions.
	POSITION pos = c_OptionsList.GetFirstSelectedItemPosition();
	if ( pos == NULL )
		return;

	while ( pos )
	{	int nItem = c_OptionsList.GetNextSelectedItem(pos);
		OptionChainItemInfo* pInfo = (OptionChainItemInfo*)c_OptionsList.GetItemData( nItem );
		long def_ID = theApp.optMgr->AssetExists( pInfo->symbol );
#ifdef _DEBUG
		TRACE( _T("OptionChainDialog::OnSave: optMgr->AssetExists(%s) --> %d\n"), pInfo->symbol, def_ID );
#endif
			// we only care about those options that don't already exist in the database,
			// because pre-existing options were automatically updated in OnParseData()
		if ( def_ID < 0 )
		{		// the options doesn't already exist ... create a stack based COption
			pInfo->symbol.MakeUpper();
			COption	opt( pInfo->symbol );
			opt.setMktPrice( pInfo->last );
			opt.strikePrice = pInfo->strike;
				// CBOE trading closes at 3PM Central Time.  Options expire on
				// the Saturday after the Third Friday, but the last day to trade
				// is Third Friday, so expiry is effectively the close of trading on
				// Third Friday, thus at 15:00 hours, hence the OleDateTime below...
				// Should we account for time zone differences ???
				// Note:  pInfo->expiry's month is 0-based, but COleDateTime needs 1-based months
			COleDateTime expires( pInfo->expiry >> 4, (pInfo->expiry & 0xF) + 1, 1, 15, 0, 0 );
			opt.setExpiry( ThirdFriday( expires ) );
			opt.setPutCall( pInfo->isCall ? Call : Put );
			opt.status = ActiveDefinition;

				// now for the underlying
			long	stkSym_ID = theApp.stkMgr->GetSymbol_ID( m_StockSymbol );
				// if no stock def exists, we need to create one
			if ( stkSym_ID < 0 )
			{		// need to create a new stock with this name
				CStock	stk( m_StockSymbol );
				AssetType	at = stk.getAssetType();
#ifdef _DEBUG
				TRACE( _T("OptionChainDialog::OnSave: Stock's assetType=%s\n"),
					((at == Stock) ? "Stock" : ((at == Option) ? "Option" : "Unknown"))  );
				if ( at != Stock  &&  at != Option )
					AfxDebugBreak();
#endif
				stk.setMktPrice( m_StockPrice );					// a floating point dialog var
				if ( m_CompanyName != _T("") )
				{	stk.setDesc( m_CompanyName );
					stk.saveDesc();
				}
				short	res = theApp.stkMgr->WriteStock( &stk );
#ifdef _DEBUG
				if ( res != 0 )
					TRACE( _T("OptionChainDialog::OnSave: Stock write failed, res=%d\n"), res );
#endif
				stkSym_ID = stk.symbol_ID;
			}
			opt.stockSymbol_ID = stkSym_ID;

				// write the option to the database...  writeOption() calls
				// writeBOS(), which fixes up the symbol table
			short	res = theApp.optMgr->WriteOption( &opt );
#ifdef _DEBUG
			if ( res != 0 )
				TRACE( _T("OptionChainDialog::OnSave: Option write failed, res=%d\n"), res );
#endif
		}
	}
		// add m_StockSymbol to the c_StockSymbol ComboBox
	if ( c_StockSymbol.FindStringExact( -1, m_StockSymbol ) == LB_ERR )
	{
#ifdef _DEBUG
		TRACE( _T("OptionChainDialog::OnSave: adding m_StockSymbol=%s to c_StockSymbol\n"), m_StockSymbol );
#endif
		c_StockSymbol.AddString( m_StockSymbol );
	}
		// disable the Save button
	c_Save.EnableWindow( false );
}			// OnSave()
//--------------------------------------------------------------------------------------
void	COptionChainDialog::SortOptionsAndScrollTo( long idx )
{		// we've already loaded options for that expiration date, so we'll just
		// sort using the dummy column and show the first option
		// in the selected date range at the top of the OptionsList
	c_OptionsList.SortItems( OptionChainCompareFunc, DummyColumn );		// the dummy column
	bool   inRange = ( 0 <= idx  &&  idx < c_OptionsList.GetItemCount() );
	if ( ! inRange )
		return;

		// the first option matching the expiry date selected is pInfo->firstIdx
		// 'scroll' the OptionsList so the found entry
		// is completely (that's what FALSE does) visible
	RECT	topItemRect, itemRect;
	int	topIdx = c_OptionsList.GetTopIndex();
	c_OptionsList.GetItemRect( topIdx, &topItemRect, LVIR_BOUNDS );
	c_OptionsList.GetItemRect( idx, &itemRect, LVIR_BOUNDS );
	CSize	csiz;
	csiz.cx = 0;
	csiz.cy = itemRect.top - topItemRect.top;
#ifdef _DEBUG
	TRACE( _T("OptionChainDialog::SortOptionsAndScrollTo: csiz,cy=%d\n"), csiz.cy );
#endif
	if ( csiz.cy != 0 )
		c_OptionsList.Scroll( csiz );
}			// SortOptionsAndScrollTo()
//--------------------------------------------------------------------------------------
long	COptionChainDialog::EnableFetch( CString expSt )
{
	long	idx = -1;
	if ( expSt != _T("") )
		m_Links.Lookup( expSt, (void*&)idx );		// don't care about the return value
	c_Fetch.EnableWindow( idx < 0 );
	return	idx;
}			// EnableFetch()
//--------------------------------------------------------------------------------------
void	COptionChainDialog::OnEditUpdateExpiration( void )
{	CString	sym;
	c_Expiration.GetWindowText( sym );
		// the trimming is necessary,
		// because we see the changes before SeekComboBox::SeekEntry() does
	sym.TrimLeft();
	sym.TrimRight();
//	c_Fetch.EnableWindow( sym == _T("")  ||  c_Expiration.FindStringExact(-1,sym) >= 0 );
	m_Expiration = sym;
	EnableFetch( m_Expiration );
}			// OnEditUpdateExpiration()
//--------------------------------------------------------------------------------------
/*
void	COptionChainDialog::OnSelEndOkExpiration( void )
{
	int		lbii = c_Expiration.GetCurSel();	// this is what we use when LB Closeup happened first
	m_Links.Lookup( expSt, (void*&)lbii );		// don't care about the return value
	if ( lbii >= 0 )
		SortOptionsAndScrollTo( idx );
}			// OnSelEndOkExpiration()
*/
//-------------------------------------------------------------------------------------------------
void	COptionChainDialog::OnGetDispInfoOptionsList(
	NMHDR*			pNMHDR,
	LRESULT*		pResult									) 
{	LV_DISPINFO*	pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	if ( pDispInfo->item.mask  &  LVIF_TEXT )
	{		// this function has to be here, because only the dialog knows whether
			// this ia a Option or Stock Positions Dialog
			// GetPositionItemText resides in the <BOS>PositionDialog
		CString	cst = GetPositionItemText( pDispInfo->item.iItem,
										   pDispInfo->item.iSubItem );
		// Mike Blaszczak's approach...
		// LPTSTR	pBuf = AddPool( &cst );
		// pDispInfo->item.pszText = pBuf;

		// Tomasz Sowinski's straightforward strcpy
		//			dest		   source		length
		wcsncpy( pDispInfo->item.pszText, cst, pDispInfo->item.cchTextMax);
	}
	*pResult = 0;
}			// OnGetDispInfoOptionsList()
//----------------------------------------------------------------------------------------
void	COptionChainDialog::OnCustomDrawOptionsList( NMHDR* pNMHDR, LRESULT* pResult ) 
{		// direct NM_CUSTOMDRAW messages to the CustomDraw handler in RedBlackListCtrl
	c_OptionsList.OnCustomDraw( pNMHDR, pResult );
}			// OnCustomDrawOptionsList()
//----------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////
//								Global Callback Functions								//
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
int	CALLBACK	OptionChainCompareFunc( LPARAM lp1, LPARAM lp2, LPARAM lpSort )
{		// return a negative value if the first item should precede the second,
		//		  a positive value if the first item should follow the second,
		//		  or zero if the two items are equivalent.
	OptionChainItemInfo*	oci1 = (OptionChainItemInfo*)lp1;
	OptionChainItemInfo*	oci2 = (OptionChainItemInfo*)lp2;
	long	sortKey = (long)lpSort;
	bool	reverse = sortKey < 0;				// ascending always happens first
	sortKey = abs( sortKey );

		// use a 0-based column index
		// Columns are...
		//	0			1		2	   3	   4	  5		6	 7		8		9
		// Put/Call  Strike  Expiry  Symbol  Last  Change  Bid  Ask  Volume  OpenInt
	int		res;
		// comparison is based on ascending order...
	if ( sortKey < 4 )
	{		// 0 .. 3
		if ( sortKey == 0 )
			res = oci1->isCall == oci2->isCall ? 0 :	// two true or two false
				( oci1->isCall ?  -1 : 1);				// say Calls < Puts

		else if ( sortKey == 1 )
			res = oci1->strike == oci2->strike ? 0 :
				( oci1->strike <  oci2->strike ? -1 : 1);

		else if ( sortKey == 2 )
			res = oci1->expiry - oci2->expiry;

		else	// ( sortKey == 3 )
			res = oci1->symbol == oci2->symbol ? 0 :
				  ( oci1->symbol <  oci2->symbol ? -1 : 1 );
	}
	else if ( sortKey < 8 )
	{		// 4 .. 7
		if ( sortKey == 4 )
			res = oci1->last == oci2->last ? 0 :
				( oci1->last <  oci2->last ? -1 : 1);

		else if ( sortKey == 5 )
			res = oci1->change == oci2->change ? 0 :
				( oci1->change <  oci2->change ? -1: 1);

		else if ( sortKey == 6 )
			res = oci1->bid == oci2->bid ? 0 :
				( oci1->bid <  oci2->bid ? -1: 1);

		else	// ( sortKey == 7 )
			res = oci1->ask == oci2->ask ? 0 :
				( oci1->ask <  oci2->ask ? -1: 1);
	}
	else
	{		// 8 .. 10
		if ( sortKey == 8 )
			res = oci1->volume - oci2->volume;

		else if ( sortKey == 9 )
			res = oci1->openInt - oci2->openInt;

		else	// ( sortKey == 10 )
			res = oci1->idx - oci2->idx;		// Dummy column (original insertion order)
	}

		// switch sense if descending
	return	reverse  ?  -res  :  res;
}			// OptionChainCompareFunc() CALLBACK
//----------------------------------------------------------------------------------------
/*
	const	CString	agentName( _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)") );
	hInet = InternetOpen( agentName,
		INTERNET_OPEN_TYPE_DIRECT,		// access type
		NULL,							// proxy
		NULL,							// bypass address
		0							);	// DON'T use INTERNET_FLAG_ASYNC
#ifdef _DEBUG
	if ( hInet == NULL )
	{	wchar_t	msg[64];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, msg, 63, NULL );
		TRACE( _T("OptionChainDialog::OnInitDialog: InternetOpen() failed -> %s\n"), msg );
	}
#endif
*/
	// attempt to connect to serverName (InternetConnect can also take ipAddr, but why bother)
		// no callback (last parameter) at this time
		// no cached connection available to reuse
/*
	const	CString serverName( _T("192.168.1.101") );
	if ( hInet )
	{		// hInetSession is needed in the InternetOpen/InternetConnect/HttpOpenRequest/HttpSendRequest scenario
			// no benefit in the initial call, but possibly some savings for subsequent retrievals
			// get the size of the url download
//		INTERNET_STATUS_CALLBACK isc = InternetSetStatusCallback( hInet,
//			(INTERNET_STATUS_CALLBACK)DisplayOptionChainInetStatus );
//		if ( isc == INTERNET_INVALID_STATUS_CALLBACK )
//			TRACE( _T("OptionChainDialog::OnInitDialog: bad callback function\n") );
		hInetSession = InternetConnect( hInet, serverName, INTERNET_DEFAULT_HTTP_PORT,
			_T(""), _T(""), INTERNET_SERVICE_HTTP, 0, NULL );
		
#ifdef _DEBUG
		if ( hInetSession == NULL )
			TRACE( _T("OptionChainDialog::OnInitDialog: can't get a connection to %s\n"), serverName );
#endif
	}
*/
/*
void	COptionChainDialog::OnEditChangeStockSymbol()-
{		// XXX revisit this in light of CSeekComboBox::OnEditChange()
		// Result:  this is longer catching messages, but OnEditUpdateStockSymbol() is
		// normally, this whole routine would be taken care of inCSeekComboBox::OnEditChange()
//	seekComboBoxEntry( IDC_StockSymbol );
//	c_StockSymbol.SeekEntry();						// here's what CSeekComboBox::OnEditChange() would do
//	CString	sym = getComboSel( IDC_StockSymbol );
	CString	sym = c_StockSymbol.GetText();
//	sym.TrimLeft();									// SeekEntry() does this now
//	sym.TrimRight();								// SeekEntry() does this now
		// this is why we can't leave this to CSeekComboBox::OnEditChange()
	c_Fetch.EnableWindow( sym != _T("")  &&  sym != m_StockSymbol );
}			// OnEditChangeStockSymbol()
//--------------------------------------------------------------------------------------
short	COptionChainDialog::ImportOptionChain( CFile& fp, CString url, bool linksToo )
{		// linksToo indicates that the user has just changed the stock symbol, so
		// we need stock price, company name, links & the option chain
		// however when linksToo is false, the user has merely selected an Expiry date from
		// c_Expiration, so we need only retrieve the stock price and the option chain
		//
		// Here's the flow in table form:		 linksToo -->   | false	| true
		//										----------------+-------+------
		//										   company name	|		|	+
		//											stock price |	+	|	+
		//												  links |		|	+
		//												options |	+	|	+
	c_StatusResult.SetWindowText( _T("Extracting...") );

		// verify that user entered a valid ticker symbol
		// Sometimes the title version of the company name doesn't quite match the parenthesized one
		// Intel Corp (INTC) and Ryder Systems Inc (R) exhibit these inconsistencies
	char*	title = "<title>";
	if ( ! ReadPast( fp, title ) )							// e.g. INTC: Summary for INTEL CP - Yahoo! Finance
	{
#ifdef _DEBUG
		TRACE( _T("OptionChainDialog::ImportOptionChain(-1): read the whole file but didn't find %s\n"),
			title );
#endif
		return	-1;
	}

	char*	htmlTitle = NULL;								// allocated in GetToken(), must delete htmlTitle later
	char*	endTitle = "</title>";
	if ( ! GetToken( fp, endTitle, htmlTitle )  ||  htmlTitle == NULL )
	{	if ( htmlTitle )
			delete [] htmlTitle;
#ifdef _DEBUG
		TRACE( _T("OptionChainDialog::ImportOptionChain(-2): unable to locate %s\n"), endTitle );
#endif
		return	-2;
	}

	char*	invalidSymbol = "Invalid Ticker Symbol";
	if ( FindSubStr(htmlTitle, 0, invalidSymbol) >= 0 )
	{		// user has entered an Invalid Ticker Symbol
		if ( htmlTitle )
			delete [] htmlTitle;
		setStatic( invalidSymbol, IDC_CompanyNameResult );
		c_StatusResult.SetWindowText( _T("") );
		return	-3;
	}
	if ( htmlTitle )
		delete [] htmlTitle;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// load links
	char*	startBold = "<b>";
	if ( linksToo )
	{		// skip to the end of the meta stuff
		char*	yfmoz = "indexOf('gecko')";
		if ( ! ReadPast( fp, yfmoz ) )		// e.g. var yfimoz  = ((yfiagt.indexOf('gecko')>-1&&!yfisaf)?1:0);
		{	return	-4;
		}
		
			// get the company name (in close proximity to the stock price)
		CString		parenSymbol = _T("(");
		parenSymbol += m_StockSymbol;
		parenSymbol.MakeUpper();
		parenSymbol += _T(")");
#ifdef _DEBUG
		TRACE( _T("OptionChainDialog::ImportOptionChain: parenSymbol=%s\n"), parenSymbol );
#endif
		char*	pSym = CString2char( parenSymbol );				// must delete pSym later
		if ( ! ReadPast( fp, pSym ) )							// e.g. (INTC)
		{	if ( pSym )
				delete	[] pSym;
			return	-5;
		}
			// we're positioned after '(SYMBOL)' - now backup a bit, 
			// assuming that the companyName is reasonably short
		fp.Seek( -64, CFile::current );
		if ( ! ReadPast( fp, startBold ) )						// <b>
		{	if ( pSym )
				delete [] pSym;
			return	-6;
		}

		char*	szCompName = NULL;								// must delete szCompName later
		if ( ! GetToken( fp, pSym, szCompName )  ||  szCompName == NULL )
		{	if ( pSym )
				delete [] pSym;
			if ( szCompName )
				delete [] szCompName;
			return	-7;
		}
		if ( pSym )
			delete [] pSym;

		m_CompanyName = szCompName;								// char to CString conversion
		m_CompanyName.TrimLeft();								// does Trimming cause strcpy's ?
		m_CompanyName.TrimRight();
#ifdef _DEBUG
		TRACE( _T("OptionChainDialog::ImportOptionChain: m_CompanyName=%s\n"), m_CompanyName );
#endif
		if ( szCompName )
			delete [] szCompName;


			// now we have to delete '&amp;' stings embedded within the name
		const CString	htmlAmpersand = _T("&amp;");			// stock symbols like AT&T makes this loop necessary
		short	jj = m_CompanyName.Find( htmlAmpersand );
		while ( jj >= 0 )
		{	CString tmp = m_CompanyName.Mid( 0, jj );			// get the piece left of htmlAmpersand
			tmp += _T("&&");
			jj	+= htmlAmpersand.GetLength();					// position jj after htmlAmpersand
			tmp += m_CompanyName.Mid( jj );						// get the piece right of htmlAmpersand
			m_CompanyName = tmp;
				// prepare for next pass
			jj = m_CompanyName.Find( htmlAmpersand );
		}
	}
		// whether or not we loaded links, we still have to restore Company Name because
		// we've been using it as a Status field during download...
	c_CompanyNameResult.SetWindowText( m_CompanyName );


		// get the current price of the underlying
		// assuming there's only one <big><b> between BOF or (INTC) and the price
	char*	bigBold = "<big><b>";
	if ( ! ReadPast( fp, bigBold ) )
		return	-8;

	char*	endBold = "</b>";
	char*	szStockPrice = NULL;									// must delete szStockPrice later
	if ( ! GetToken( fp, endBold, szStockPrice )  ||  szStockPrice == NULL )		// everything up to </b>
	{	if ( szStockPrice )
			delete [] szStockPrice;
		return	-9;
	}
#ifdef _DEBUG
	CString	tmp = szStockPrice;
		TRACE( _T("OptionChainDialog::ImportOptionChain: szStockPrice=%s\n"), tmp );
#endif

		// need the following to complete all aspects of the following call:
		//		m_CompanyName, c_StockPriceResult, m_StockSymbol, szStockPrice
	HandleStockPrice( szStockPrice );
	if ( szStockPrice )
		delete [] szStockPrice;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// now for the option expiration dates and Links
	char*	szCache = NULL;											// must delete szCache later
	if ( linksToo )
	{	char*	viewByExpiration = "View By Expiration:";
		if ( ! ReadPast( fp, viewByExpiration ) )					// View By Expiration:
		{		// this is where issues with ZERO options exit ImportOptionChain()
				// e.g. stock symbol = IVAN
				// the negative return code will inhibit displaying the status line in the caller,
				// so we force its display here
			DisplayStatusLine();
			return	-10;
		}

		char*	tableStart = "<table";
		if ( ! GetToken( fp, tableStart, szCache )  ||  szCache == NULL )
		{	delete [] szCache;
			return	-11;
		}
			// load the expiration dates
		short	nLinks = LoadLinks( szCache );
		if ( szCache )
		{	delete [] szCache;
			szCache = NULL;											// we're going to use szCache again
		}
		c_Expiration.EnableWindow( nLinks > 0 );
	}

		// setup a numeric encoding of expirySt for use in LoadOptions() OptionsList pInfo structs
#ifdef _DEBUG
	int		lenExp = m_Expiration.GetLength();
	if ( lenExp < 5 )
	{	TRACE( _T("OptionChainDialog::ImportOptionCain: GetLength(%s) --> %d\n"), m_Expiration, lenExp );
		AfxDebugBreak();
	}
#endif
	long	codedExpiry = MakeLongMonYr( m_Expiration );

			// save nItems so we can track it in the LinkInfo struct below
	short	nItems = c_OptionsList.GetItemCount();
#ifdef _DEBUG
	TRACE( _T("OptionChainDialog::ImportOptionChain: incoming OptionsList has %d items\n"), nItems );
#endif

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// read the Calls
	char*	callOptions = "CALL OPTIONS";
	if ( ! ReadPast( fp, callOptions ) )
		return	-12;

		// isolate the Calls in a local heap cache
	c_OptionsList.SetRedraw( FALSE );								// trying to fix the double paint
	char*	putOptions = "PUT OPTIONS";
	bool match = GetToken( fp, putOptions, szCache );				// cache now contains the Call rows
	if ( match  &&  szCache != NULL )
		m_nCalls += LoadOptions( szCache, codedExpiry, true );		// isCall is true
	if ( szCache )
	{	delete [] szCache;
		szCache = NULL;
	}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// read the Puts
	char*	expandToStraddleView = "Expand to Straddle View";
	match = GetToken( fp, expandToStraddleView, szCache );			// cache now contains the Put rows
	if ( match  &&  szCache != NULL )
		m_nPuts += LoadOptions( szCache, codedExpiry, false );		// isCall is false
	if ( szCache )
	{	delete [] szCache;
		szCache = NULL;
	}
	c_OptionsList.SetRedraw( TRUE );								// see if this fixes the double paint

	LinkInfo*	pInfo = NULL;
	if ( m_Links.Lookup( m_Expiration, (void*&)pInfo )  &&  pInfo )
	{	pInfo->optionsLoaded = true;
		pInfo->firstIdx = nItems;			// the mechanism behind EnsureVisible in killFocusExpirationWork()
	}
	return	m_nCalls + m_nPuts - nItems;
}			// ImportOptionChain()
//--------------------------------------------------------------------------------------
/*
short		COptionChainDialog::LoadLinks( char* szCache )
{	char*	token;
		// all we need from LoadLinks() are the date strings
	char*	startBold = "<b>";
	char*	endBold = "</b>";
	char*	startA = "<a href=";
	short	lenStartA = strlen( startA );
	char*	endA = "</a>";
	long	ii, jj;
		// presume the file is positioned after 'View By Expiration'
		// Parse html like the following (can't assume that '<b> date </b>' will be first) ...
		//	<b>Apr 05</b> |
		//	<a href="/q/op?s=INTC&amp;m=2005-05">May 05</a> | 
		//	<a href="/q/op?s=INTC&amp;m=2005-07">Jul 05</a> |
		//	<a href="/q/op?s=INTC&amp;m=2005-10">Oct 05</a> |
		//	<a href="/q/op?s=INTC&amp;m=2006-01">Jan 06</a> |
		//	<a href="/q/op?s=INTC&amp;m=2007-01">Jan 07</a>
		// the first expiration date is bold and has no hyperlink
	ii = FindSubStr( szCache, 0, startBold );			if ( ii < 0 ) return 0;
	ii += sizeof( startBold ) - 1;										// skip startBold; sizeof() includes '\0'
	jj = FindSubStr( szCache, ii, endBold );			if ( jj < 0 ) return 0;
	token = ExtractToken( szCache+ii, jj-ii );							// delete token later
	EatWhitespace( token );
		// stuff the expiry string into the Expiry combo box
	m_Expiration = token;												// type conversion to CString
	delete [] token;
	c_Expiration.AddString( m_Expiration );

		// track the (date,url) pair in m_Links
	LinkInfo*	pInfo = new LinkInfo;
	pInfo->optionsLoaded = true;						// indicates that download/parsing has already occurred
	pInfo->firstIdx = 0;								// we'll never have to correct this index
	m_Links.SetAt( m_Expiration, pInfo );

		// process the remainder of the cache
	short	nDates = 1;
	while ( *(szCache+ii) != '\0' )
	{	jj = FindSubStr( szCache, jj, startA );			if ( jj < 0 ) break;

			// snag the expiration date
		ii = jj + lenStartA;										// skip startA
		ii = FindSubStr( szCache, ii, ">" );			if ( ii < 0 ) break;
		ii += 1;													// pass up the '>'
		jj = FindSubStr( szCache, ii, endA );			if ( jj < 0 ) break;
		token = ExtractToken( szCache+ii, jj-ii );
		EatWhitespace( token );
		CString	expirySt = token;
		
		delete [] token;

		c_Expiration.AddString( expirySt );							// load the Expiration combo box

			// track the (date,url) pair in m_Links
		pInfo = new LinkInfo;
		pInfo->optionsLoaded = false;					// indicates that no download (or parsing) has occurred
		pInfo->firstIdx = 0;							// we'll correct this later when we load these options
		m_Links.SetAt( expirySt, pInfo );

		nDates++;
	}
	c_Expiration.SetCurSel( 0 );						// XXX don't use 0 if the <b> date isn't first !!!
	return	nDates;
}			// LoadLinks()
//--------------------------------------------------------------------------------------
short		COptionChainDialog::LoadOptions( char* szCache, long codedExpiry, bool isCall )
{	char*	token = NULL;
	char*	startBold = "<b>";
	char*	endBold = "</b>";
	char*	startA = "<a href=";
	char*	endA = "</a>";
	char*	notApplicable = "N/A";
	const short	startBoldLen = 3;
	const short endBoldLen = 4;
	const short	startALen = 8;
	const short endALen = 4;
		// parse a cache that looks like one or more copies of the following...
		//					  <a href="/q/op?s=INTC&amp;k=7.500000">7.50</a></b></td>
		// <td class="yfnc_h"><a href="/q?s=NQDU.X">NQDU.X</a></td>
		// <td class="yfnc_h" align="right"><b>15.97</b></td>
		// <td class="yfnc_h" align="right"> <b style="color:#000000;">0.00</b></td>
		// <td class="yfnc_h" align="right">15.60</td>
		// <td class="yfnc_h" align="right">15.80</td>
		// <td class="yfnc_h" align="right">0</td>
		// <td class="yfnc_h" align="right">495</td>
		// </tr><tr><td class="yfnc_h" nowrap><b>

		// now for the parsing
	short	nOptions = 0;
	long	ii, jj = 0;
	while ( *(szCache+jj) != '\0' )							// not much of a test
	{		// read the strike price
			// <a href="/q/op?s=INTC&amp;k=7.500000">7.50</a></b></td>
//		ii = cache.Find( startA, jj );					if ( ii < 0 ) break;
//		ii += startA.GetLength() + 1;
//		ii = cache.Find( _T(">"), ii );					if ( ii < 0 ) break;
//		ii++;																	// skip the ">"
//		jj = cache.Find( endA, ii );					if ( jj < 0 ) break;
//		CString strikeSt = cache.Mid( ii, jj - ii );
		ii = FindSubStr( szCache, jj, startA );			if ( ii < 0 ) break;
		ii = FindSubStr( szCache, ii, ">" );			if ( ii < 0 ) break;
		ii++;																	// skip the ">"
		jj = FindSubStr( szCache, ii, endA );			if ( jj < 0 ) break;
		token = ExtractToken( szCache+ii, jj-ii );								// delete token later
		float	strike;
		sscanf( token, "%f", &strike );
		delete [] token;


			// read the option symbol
			// <td class="yfnc_h"><a href="/q?s=NQDU.X">NQDU.X</a></td>
//		ii = cache.Find( startA, jj );					if ( ii < 0 ) break;
//		ii = cache.Find( _T(">"), ii );					if ( ii < 0 ) break;
//		ii++;																	// skip the ">"
//		jj = cache.Find( endA, ii );					if ( jj < 0 ) break;
//		CString	symbol = cache.Mid( ii, jj - ii );
		ii = FindSubStr( szCache, jj, startA );			if ( ii < 0 ) break;
		ii = FindSubStr( szCache, ii, ">" );			if ( ii < 0 ) break;
		ii++;																	// skip the ">"
		jj = FindSubStr( szCache, ii, endA );			if ( jj < 0 ) break;
		token = ExtractToken( szCache+ii, jj-ii );
		CString	symbol = token;													// type conversion to CString 
		delete [] token;


			// read the price of the last trade
			// <td class="yfnc_h" align="right"><b>15.97</b></td>
//		ii = cache.Find( startBold, jj );				if ( ii < 0 ) break;
//		ii += startBold.GetLength();
//		jj = cache.Find( endBold, ii );					if ( jj < 0 ) break;
//		CString	lastSt = cache.Mid( ii, jj - ii );
		ii = FindSubStr( szCache, jj, startBold );		if ( ii < 0 ) break;
		ii += startBoldLen;
		jj = FindSubStr( szCache, ii, endBold );		if ( jj < 0 ) break;
		token = ExtractToken( szCache+ii, jj-ii );
		float	last;
		sscanf( token, "%f", &last );
		delete [] token;
		
			// read the colorspec for the 'change' in price from end of day yesterday
			// <td class="yfnc_h" align="right"> <b style="color:#000000;">0.00</b></td>
		char*	clrStr = "color:#";
		const short clrStrLen = 7;
			// color indicates arithmetic sign
			// colorSpecs look like 000000 for black, cc0000 for red, and 008800 for green
//		ii = cache.Find( clrStr, jj );					if ( ii < 0 ) break;
//		ii += clrStr.GetLength();
//		jj = cache.Find( _T(";"), ii );					if ( jj < 0 ) break;
//		CString colorSpec = cache.Mid( ii, jj - ii );							// don't neeed colorSpec anymore
		ii = FindSubStr( szCache, jj, clrStr );			if ( ii < 0 ) break;
		ii += clrStrLen;
		jj = FindSubStr( szCache, ii, ";" );			if ( jj < 0 ) break;
		bool colorIsRed = *(szCache+ii) != '0';									// Red bits are ON


			// read the 'change' in price from end of day yesterday
//		ii = cache.Find( _T(">"), jj );					if ( ii < 0 ) break;
//		ii++;																	// skip the ">"
//		jj = cache.Find( endBold, ii );					if ( jj < 0 ) break;
//		CString changeSt = cache.Mid( ii, jj - ii );
		ii = FindSubStr( szCache, jj, ">" );			if ( ii < 0 ) break;
		ii++;																	// skip the ">"
		jj = FindSubStr( szCache, ii, endBold );		if ( jj < 0 ) break;
		token = ExtractToken( szCache+ii, jj-ii );
		float	change;
		sscanf( token, "%f", &change );
		delete [] token;
		if ( colorIsRed )														// it's RED !
			change = -change;


			// read the current bid price (XXX problems here)
			// <td class="yfnc_h" align="right">15.60</td>
//		ii = cache.Find( _T("<td"), jj );				if ( ii < 0 ) break;
//		ii = cache.Find( _T(">"), ii );					if ( ii < 0 ) break;
//		ii++;																	// skip the ">"
//		jj = cache.Find( _T("</td>"), ii );				if ( jj < 0 ) break;
//		CString	bidSt = cache.Mid( ii, jj - ii );
		ii = FindSubStr( szCache, jj, "<td" );			if ( ii < 0 ) break;
		ii = FindSubStr( szCache, ii, ">" );			if ( ii < 0 ) break;
		ii++;																	// skip the ">"
		jj = FindSubStr( szCache, ii, "</td>" );		if ( jj < 0 ) break;
		token = ExtractToken( szCache+ii, jj-ii );
		float	bid;
		if ( strcmp(token, notApplicable) == 0 )
			bid = -1.0;
		else
			sscanf( token, "%f", &bid );
		delete [] token;


			// read the current ask price
			// <td class="yfnc_h" align="right">15.80</td>
//		ii = cache.Find( _T("<td"), jj );				if ( ii < 0 ) break;
//		ii = cache.Find( _T(">"), ii );					if ( ii < 0 ) break;
//		ii++;																	// skip the ">"
//		jj = cache.Find( _T("</td>"), ii );				if ( jj < 0 ) break;
//		CString	askSt = cache.Mid( ii, jj - ii );
		ii = FindSubStr( szCache, jj, "<td" );			if ( ii < 0 ) break;
		ii = FindSubStr( szCache, ii, ">" );			if ( ii < 0 ) break;
		ii++;																	// skip the ">"
		jj = FindSubStr( szCache, ii, "</td>" );		if ( jj < 0 ) break;
		token = ExtractToken( szCache+ii, jj-ii );
		float	ask;
		if ( strcmp(token, notApplicable) == 0 )
			ask = -1.0;
		else
			sscanf( token, "%f", &ask );
		delete [] token;


			// read the current trading volume
			// <td class="yfnc_h" align="right">0</td>
//		ii = cache.Find( _T("<td"), jj );				if ( ii < 0 ) break;
//		ii = cache.Find( _T(">"), ii );					if ( ii < 0 ) break;
//		ii++;																	// skip the ">"
//		jj = cache.Find( _T("</td>"), ii );				if ( jj < 0 ) break;
//		CString	volumeSt = cache.Mid( ii, jj - ii );
//		volumeSt = EjectChar( volumeSt, _T(',') );
		ii = FindSubStr( szCache, jj, "<td" );			if ( ii < 0 ) break;
		ii = FindSubStr( szCache, ii, ">" );			if ( ii < 0 ) break;
		ii++;																	// skip the ">"
		jj = FindSubStr( szCache, ii, "</td>" );		if ( jj < 0 ) break;
		token = ExtractToken( szCache+ii, jj-ii );
		EjectChar( token, ',' );
		long	volume;
		sscanf( token, "%d", &volume );
		delete [] token;

			// read the opening interest
			// <td class="yfnc_h" align="right">495</td>
//		ii = cache.Find( _T("<td"), jj );				if ( ii < 0 ) break;
//		ii = cache.Find( _T(">"), ii );					if ( ii < 0 ) break;
//		ii++;																	// skip the ">"
//		jj = cache.Find( _T("</td>"), ii );				if ( jj < 0 ) break;
//		CString	openIntSt = cache.Mid( ii, jj - ii );
//		openIntSt = EjectChar( openIntSt, _T(',') );
		ii = FindSubStr( szCache, jj, "<td" );			if ( ii < 0 ) break;
		ii = FindSubStr( szCache, ii, ">" );			if ( ii < 0 ) break;
		ii++;																	// skip the ">"
		jj = FindSubStr( szCache, ii, "</td>" );		if ( jj < 0 ) break;
		token = ExtractToken( szCache+ii, jj-ii );
		EjectChar( token, ',' );
		long	openInt;
		sscanf( token, "%d", &openInt );				// sscanf() will hiccup on commas
		delete [] token;


		OptionChainItemInfo*	pInfo = new OptionChainItemInfo;
		pInfo->ask = ask;
		pInfo->bid = bid;
		pInfo->change = change;
		pInfo->expiry = codedExpiry;
		pInfo->idx = c_OptionsList.GetItemCount();
		pInfo->isCall = isCall;
		pInfo->last = last;
		pInfo->openInt = openInt;
		pInfo->strike = strike;
		pInfo->symbol = symbol;
		pInfo->volume = volume;
		UpdateDB( pInfo );

		c_OptionsList.AddItem( (LPARAM)pInfo, pInfo->idx );
		nOptions++;
	}
		// kill the stuff we just parsed
//	cache = cache.Right( cache.GetLength() - jj );
	return	nOptions;
}			// LoadOptions()
//--------------------------------------------------------------------------------------
void COptionChainDialog::OnStatusResult( void ) 
{	CString	statusText;
	unsigned long	dwStatus = (unsigned long)GetMessagePos();		// the WPARAM ?
	switch ( dwStatus )
	{	case INTERNET_STATUS_CLOSING_CONNECTION:
			statusText = _T("Connection closing");
			break;
		case INTERNET_STATUS_CONNECTED_TO_SERVER:
			statusText = _T("Connected");
			break;
		case INTERNET_STATUS_CONNECTING_TO_SERVER:
			statusText = _T("Connecting...");
			break;
		case INTERNET_STATUS_CONNECTION_CLOSED:
			statusText = _T("Closed");
			break;
		case INTERNET_STATUS_HANDLE_CLOSING:
			statusText = _T("Handle closing");
			break;
		case INTERNET_STATUS_HANDLE_CREATED:
			statusText = _T("Handle created");
			break;
		case INTERNET_STATUS_INTERMEDIATE_RESPONSE:
			statusText = _T("Intermediate response");
			break;
		case INTERNET_STATUS_NAME_RESOLVED:
			statusText = _T("Name resolved");
			break;
		case INTERNET_STATUS_RECEIVING_RESPONSE:
			statusText = _T("Receiving response...");
			break;
		case INTERNET_STATUS_RESPONSE_RECEIVED:
			statusText = _T("Response received");
			break;
		case INTERNET_STATUS_REDIRECT:
			statusText = _T("Redirect");
			break;
		case INTERNET_STATUS_REQUEST_COMPLETE:
			statusText = _T("Request complete");
			break;
		case INTERNET_STATUS_REQUEST_SENT:
			statusText = _T("Request sent");
			break;
		case INTERNET_STATUS_RESOLVING_NAME:
			statusText = _T("Resolving name");
			break;
		case INTERNET_STATUS_SENDING_REQUEST:
			statusText = _T("Sending request");
			break;
		case INTERNET_STATUS_STATE_CHANGE:
			statusText = _T("State change");
			break;
		default:
		{	char	buf[80];
			sprintf( buf, "Unknown: %d", dwStatus );
			statusText = buf;
			break;
		}	
	}
	CStatic*	pStatus = (CStatic*)GetDlgItem( IDC_StatusResult );
	pStatus->SetWindowText( statusText );
}			// OnStatusResult()
*/
//----------------------------------------------------------------------------------------
/*
void	CALLBACK	DisplayOptionChainInetStatus(
	HINTERNET		hInternet,
	unsigned long*	dwContext,
	unsigned long	dwInternetStatus,
	void*			lpvStatusInformation,
	unsigned long	dwStatusInformationLength	)
{		// send a message to the control represented by dwContext
		//				  OnStatusResult	  wParam			lParam
#ifdef _DEBUG
	TRACE( _T("DisplayOptionChainInetStatus: status=%d\n"), dwInternetStatus );
#endif
	SendMessage( (HWND)dwContext, BN_CLICKED, dwInternetStatus, dwInternetStatus );
}			// DisplayOptionChainInetStatus()
*/
//----------------------------------------------------------------------------------------
/*
short	COptionChainDialog::DownloadURL( CString url, bool linksToo )
{		// inspired by the example code on p.192 of Nick Grattan and Marshall Brain's
		//											Windows CE 3.0 Applications Programming
	if ( ! hInet )
	{
#ifdef _DEBUG
		TRACE( _T("OptionChainDialog::DownloadURL(-1): hInet not initialized\n") );
#endif
		return	-1;
	}

		// in the Open/OpenUrl scenario, InternetOpenUrl() fails (overlapped I/O as the culprit)
		// same result in an Open/OpenUrl scenario
		// open the url...				   hInet, sz_url, sz_hdrs, lenHdrs, flags, callback context
	const	CString serverName( _T("finance.yahoo.com") );
	CString fullUrl = _T("http://") + serverName;
	fullUrl += url;
	HINTERNET	hInetUrl = InternetOpenUrl( hInet, fullUrl, NULL, 0,
			// Nick Grattan uses 0 for the flags
		INTERNET_FLAG_RELOAD  |  INTERNET_FLAG_NO_UI  |  INTERNET_FLAG_NO_COOKIES  |  INTERNET_FLAG_NEED_FILE,
		NULL );
	if ( hInetUrl == NULL )
	{
#ifdef _DEBUG
		wchar_t	msg[64];
		int	err = GetLastError();
			// given a server relative url (the incoming argument)
			//		--> error 12006, the url represents an unrecognized scheme (i.e. something ohter than http or https)
			// given a full url (fullUrl) --> error 997, Overlapped I/O operation is in progress.
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, NULL, msg, 63, NULL );
		TRACE( _T("OptionChainDialog::DownloadURL(-2): InternetOpenUrl(%s) failed %d -> %s\n"), fullUrl, err, msg );
#endif
		return	-2;
	}

		// keep the downloaded in the DB directory
	CString	fName = theApp.pDB->getDBdir();
	fName += _T("\\tmp.pnh");									// an arbitrary file name
	CFile*	fp = new CFile;
	CFileException fileException;
	if ( !fp->Open( fName, CFile::modeReadWrite | CFile::modeCreate) )
	{
#ifdef _DEBUG
		TRACE( _T("COptionChainDialog::DownloadURL(-3):  Can't open file %s, error -> %s\n"),
			fName, fileException.m_cause );
#endif
		InternetCloseHandle( hInetUrl );
		return	-3;
	}

		// iteratively write the receive buffer to a temporary file
//	m_Abort = false;
//	c_Abort.EnableWindow( TRUE );
	unsigned long	bytesRead;
	char			rcvBuf[1024];
	BOOL	  done = FALSE;
	while ( ! done )
	{
//		if ( m_Abort )
//			break;	 
		done = InternetReadFile( hInetUrl, rcvBuf, sizeof(rcvBuf), &bytesRead );
#ifdef _DEBUG
		TRACE( _T("COptionChainDialog::DownloadURL:  Received %d bytes\n"), bytesRead );
#endif
		fp->Write( rcvBuf, bytesRead );
		done  =  done  &&  bytesRead == 0;
	}
//	c_Abort.EnableWindow( FALSE );
	InternetCloseHandle( hInetUrl );


		// how did we exit the while(1) loop?
	short	nOptions;							// return value
//	if ( m_Abort )
//		TRACE( _T("COptionChainDialog::DownloadURL: Abort signal received\n") );
//	else
//	{		// flush the file and rewind it so it's ready for parsing
		fp->Flush();							// the download/writing phase is hereby concluded
		fp->Seek( 0, CFile::begin );			// Rewind

			// now we can parse the downloaded file
		short	nItems = c_OptionsList.GetItemCount();
		nOptions = ImportOptionChain( fp, url, linksToo );
		if ( nOptions > 0 )
				// reposition the OptionsList so the new options are visible
			c_OptionsList.EnsureVisible( nItems, FALSE );		// recall that's the (nItems+1)st element
//	}

		// we're done with the file...
	fp->Close();
	BOOL   res = DeleteFile( fName );
	if ( ! res )
		TRACE( _T("OptionChainDialog::DownloadURL: DeleteFile(%s) failed\n"), fName );

	return	nOptions;
}			// DownloadURL() - InternetOpen/InternetOpenURL/InternetReadFile version
*/
//-------------------------------------------------------------------------------------------------
/*
	CString fullUrl = _T("http://");
	const	CString serverName( _T("finance.yahoo.com") );
	fullUrl += serverName;
	fullUrl += url;
		// URLDownloadToFile() doesn't compile even though it's defined in <sUrlmon.h>
	HRESULT hres = URLDownloadToFile( NULL, fullUrl, fName, 0, NULL );
	if ( hres != S_OK )
	{	TRACE( _T("OptionChainDialog::DownloadURL(-11): %d download failed.\n"), fullUrl );
		return	-11;
	}
*/
//-------------------------------------------------------------------------------------------------
/*
			// this version doesn't work - errors out on the connect...
short	COptionChainDialog::DownloadURL( CString url, bool linksToo )
{		// inspired by the TCP Stream Socket Client code example in the EVT3.0 help files
		// as well as several CodeGuru and CodeProject examples with striking similarities

		// get yahoo's IP address
	struct hostent* hostEnt = gethostbyname( "finance.yahoo.com" );
	if ( hostEnt == NULL  ||  hostEnt->h_length != 4 )
	{	TRACE( _T("OptionChainDialog::DownloadURL(-1): couldn't resolve '%s' to an IP address, h_length=%d\n"),
			hostEnt->h_aliases[0], hostEnt->h_length );
		delete	hostEnt;
		return	-1;
	}
	char*	addr = hostEnt->h_addr_list[0];
	char	tmp[16];
	sprintf( tmp, "%d.%d.%d.%d", (short)(unsigned char)addr[0],
								 (short)(unsigned char)addr[1],
								 (short)(unsigned char)addr[2],
								 (short)(unsigned char)addr[3] );
	CString	ipAddr( tmp );
	const	CString serverName( _T("finance.yahoo.com") );
	TRACE( _T("OptionChainDialog::DownloadURL: %s's IP address --> %s\n"), serverName, ipAddr);

		// open a server socket
	SOCKET		srvrSock = INVALID_SOCKET;

		// the following is probably NOT necessary, since InitInstance already does an AfxSocketInit()
		// in any case, it doesn't error out and has no effect on the failure to connect()
//	WSADATA	WSAData;
//	if ( WSAStartup( MAKEWORD(1,1), &WSAData) != 0 ) 
//		TRACE( _T("OptionChainDialog::DownloadURL(-1.5): WSAStartup failed. Error: %d\n"), WSAGetLastError() );
	
		// AF_IRDA & PF_INET both produce srvrSock == NULL
		// AF_IRDA --> 10014 in connect() - Bad address.
		// The system detected an invalid pointer address in attempting to use a pointer
		// argument of a call. This error occurs if an application passes an invalid pointer
		// value, or if the length of the buffer is too small. For instance, if the length of
		// an argument, which is a sockaddr structure, is smaller than the sizeof(sockaddr).

		// PF_INET --> 10047 in connect() - Address family not supported by protocol family.
		// An address incompatible with the requested protocol was used.  All sockets are created with
		// an associated address family (that is, AF_INET for Internet Protocols) and a generic protocol
		// type (that is, SOCK_STREAM). This error is returned if an incorrect protocol is explicitly
		// requested in the socket call, or if an address of the wrong family is used for a socket, for
		// example, in sendto.
	srvrSock = socket( PF_INET, SOCK_STREAM, 0 );		// have seen 3rd parameter as IPPROTO_TCP (and it compiles)
	if ( srvrSock == INVALID_SOCKET )
	{	TRACE( _T("OptionChainDialog::DownloadURL(-2): Socket allocation failed, INVALID_SOCKET = %d, error = %d\n"),
			INVALID_SOCKET, WSAGetLastError() );
		delete	hostEnt;									// we're done with hostEnt
		return	-2;
	}

		// Assign the socket's IP address
	SOCKADDR_IN	destAddr;									// Server socket address
	memcpy( (char*)&(destAddr.sin_addr), hostEnt->h_addr, hostEnt->h_length );
		// Convert port number to network ordering
	destAddr.sin_port = htons( 80 );						// port 80 for HTTP service
	delete	hostEnt;										// we're done with hostEnt

		// connect to the (serverName) HTTP server
		// this is where the sockets approach fails...
	if ( connect( srvrSock, (PSOCKADDR)&destAddr, sizeof(destAddr) ) == SOCKET_ERROR )
	{	TRACE( _T("OptionChainDialog::DownloadURL(-3): Connecting to the server failed, error = %d\n"),
			WSAGetLastError() );
		closesocket( srvrSock );
		return	-3;
	}

		// send a GET request to serverName
		// we temporarily reuse the main receive buffer to send a GET request to the server
	char	rcvBuf[1024];
	sprintf( rcvBuf, "GET %s\n", url );
	int	nRet = send( srvrSock, rcvBuf, strlen(rcvBuf), 0);
	if ( nRet == SOCKET_ERROR )
	{	TRACE( _T("OptionChainDialog::DownloadURL(-4): Sending '%s' to %s failed, error = %d\n"),
			rcvBuf, serverName, WSAGetLastError() );
		closesocket( srvrSock );
		return	-4;
	}

		// keep the download file in the DB directory
	CString	fName = theApp.pDB->getDBdir();
	fName += _T("\\tmp.pnh");									// an arbitrary file name
	CFile*	fp = new CFile;
	CFileException fileException;
	if ( !fp->Open( fName, CFile::modeReadWrite | CFile::modeCreate) )
	{	TRACE( _T("COptionChainDialog::DownloadURL(-5):  Can't open file %s, error -> %s\n"),
			fName, fileException.m_cause );
		closesocket( srvrSock );
		return	-5;
	}

		// read the downloaded the URL to a file
//	m_Abort = false;
//	c_Abort.EnableWindow( TRUE );
	unsigned long	bytesRead;
	BOOL	done = FALSE;
	while ( ! done )
	{
//		if ( m_Abort )
//			break;

		bytesRead = recv( srvrSock, rcvBuf, sizeof(rcvBuf), 0 );
		TRACE( _T("COptionChainDialog::DownloadURL:  Received %d bytes\n"), bytesRead );
		if ( bytesRead == SOCKET_ERROR )
		{	TRACE( _T("COptionChainDialog::DownloadURL(-6):  Socket error = %d\n"), bytesRead );
			break;
		}
		if ( bytesRead < 1 )
			break;
		fp->Write( rcvBuf, bytesRead );
		done = done  &&  bytesRead == 0;
	}
//	c_Abort.EnableWindow( FALSE );
	closesocket( srvrSock );

		// how'd we exit the while(1) loop?
	short	nOptions;							// return value
//	if ( m_Abort )
//		TRACE( _T("COptionChainDialog::DownloadURL: Abort signal received\n") );
//	else
//	{		// flush the file and rewind it so it's ready for parsing
		fp->Flush();							// the download/writing phase is hereby concluded
		fp->Seek( 0, CFile::begin );			// Rewind

			// now we can parse the downloaded file
		short	nItems = c_OptionsList.GetItemCount();
		nOptions = ImportOptionChain( fp, url, linksToo );
		if ( nOptions > 0 )
				// reposition the OptionsList so the new options are visible
			c_OptionsList.EnsureVisible( nItems, FALSE );		// recall that's the (nItems+1)st element
//	}

		// we're done with the file...
	fp->Close();
	BOOL	res = DeleteFile( fName );
	if ( ! res )
		TRACE( _T("OptionChainDialog::DownloadURL: DeleteFile(%s) failed\n"), fName );
	return	nOptions;
}			// DownloadURL()
	// TCP Stream Socket Client version
*/
//--------------------------------------------------------------------------------------
/*
VOID (CALLBACK * INTERNET_STATUS_CALLBACK )(
	HINTERNET  hInternet,
	DWORD  dwContext,
	DWORD  dwInternetStatus,
	LPVOID  lpvStatusInformation  OPTIONAL,
	DWORD  dwStatusInformationLength				)
{
}
//----------------------------------------------------------------------------------------

HRESULT		COptionChainDialog::OnProgress(
	ULONG	ulProgress,
	ULONG	ulProgressMax,
	ULONG	ulStatusCode,
	LPCWSTR	statusText					)
{	CURLCallback	ucb;
	int	nLower, nUpper;
	c_ProgressIndicator.GetRange( nLower, nUpper );
	int	range = nUpper - nLower;
	int	pos = nLower + range * ulProgress / ulProgressMax;
	return	c_ProgressIndicator.SetPos( pos );
}			// OnProgress()
*/
//----------------------------------------------------------------------------------------
/*
	Googlebot/2.X (http://www.googlebot.com/bot.html)
		The Google Web crawler. 
	Mozilla/3.0 (Win95; I)
		Netscape Navigator 3.0 on Windows 95. 
	Mozilla/3.01 (Macintosh; PPC)
		Netscape Navigator 3.01 on a Macintosh. 
	Mozilla/4.0 (compatible; MSIE 4.01; AOL 4.0; Windows 98)
		The AOL browser, based on Microsoft Internet Explorer 4.01, on Windows 98. 
	Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)
		Microsoft Internet Explorer 6.0 on Windows 2000. 
	Mozilla/5.0 (compatible; Konqueror/2.2.2; Linux 2.4.14-xfs; X11; i686)
		Konqueror 2.2.2 for Linux. 
	Mozilla/5.0 (Windows; U; Win98; en-US; rv:0.9.2) Gecko/20010726 Netscape6/6.1
		Netscape 6.1 on Windows 98. 
	Opera/6.x (Windows NT 4.0; U) [de]
		The German version of Opera 6.x on Windows NT. 
	Opera/7.x (Windows NT 5.1; U) [en]
		The English version of Opera 7.x on Windows XP.

	Opera uses the following of UserAgent strings...

	Opera 6
		Identify as	:	 Useragent string
		Opera		  :	 Opera/X.Y (OS; U)  [la]
		Mozilla 5.0	:	 Mozilla/5.0 (OS; U) Opera X.Y  [la]
		Mozilla 4.78   :	 Mozilla/4.78 (OS; U) Opera X.Y  [la]
		Mozilla 3.0	:	 Mozilla/3.0 (OS; U) Opera X.Y  [la]
		MSIE 5.0	   :	 Mozilla/4.0 (compatible; MSIE 5.0; OS) Opera X.Y  [la]

	Opera 7
		Identify as	:	 Useragent string
		Opera		  :	 Opera/X.Y (OS; U)  [la]
		Mozilla 5.0	:	 Mozilla/5.0 (OS; U) Opera X.Y  [la]
		Mozilla 4.78   :	 Mozilla/4.78 (OS; U) Opera X.Y  [la]
		Mozilla 3.0	:	 Mozilla/3.0 (OS; U) Opera X.Y  [la]
		MSIE 5.0	   :	 Mozilla/4.0 (compatible; MSIE 6.0; OS) Opera X.Y  [la]

	Opera version ("X.Y"), operating system ("OS"), or language ("la") may change.
	Opera is identified as MSIE by default.
	[la] should be [en]
	For more details, see:  <http://www.opera.com/support/search/supsearch.dml?index=570>

*/
/*
		// see if the computer is set up to go online
	DWORD	dw_Res = InternetAttemptConnect( 0 );
	if ( dw_Res != ERROR_SUCCESS )
	{	wchar_t	buf[64];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 63, NULL );
		TRACE( _T("OptionChainDialog::DownloadURL(-1): InternetAttemptConnect() failed, error code = %d --> %s\n"),
			dw_Res, buf );
		return	-1;
	}

		// ping the url's server
	BOOL	b_res = InternetCheckConnection( url, FLAG_ICC_FORCE_CONNECTION, 0 );
	if ( ! b_res )
	{	wchar_t	buf[64];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 63, NULL );
		TRACE( _T("OptionChainDialog::DownloadURL(-2): InternetCheckConnection() failed, last  error = %s\n"),
			buf );
		return	-2;
	}
*/
		// when URLCallback compiles...
		// need to put the URLDownloadToFile into its own thread
//	CURLCallback	callback( &c_ProgressIndicator );
		// fifth param could be an LPBINDSTATUSCALLBACK (CURLCallback class)
		// then we could show incremental progress in the CProgressCtrl
//	HRESULT hres = URLDownloadToFile( NULL, url, fName, 0, callback );

		// try the no progress version first...
//	HRESULT hres = URLDownloadToFile( NULL, url, fName, 0, NULL );
//	if ( hres != S_OK )
//	{	TRACE( _T("OptionChainDialog::DownloadURL(-1): URL download failed.\n") );
//		return	-1;
//	}

/*
		// here's where we'd normally disclose the calling program's name...
		// For now, we'll masquerade as MSIE 6.0:  Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)
		//								Agent, AccessType, Proxy, ProxyBypass, flags
		// For now, we'll masquerade as MSIE 6.0:  Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)
	hInet = InternetOpen( _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0)"),
						  INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, INTERNET_FLAG_ASYNC );
	if ( hInet == NULL )
	{	wchar_t	msg[64];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, msg, 63, NULL );
		TRACE( _T("OptionChainDialog::DownloadURL(-1): InternetOpen() failed -> %s\n"), msg );
		return	-1;
	}

		// open the url...				   hInet, sz_url, sz_hdrs, lenHdrs, flags, callback context
	HINTERNET	hInetUrl = InternetOpenUrl( hInet, url, NULL, 0, INTERNET_FLAG_RELOAD, NULL );
	if ( hInetUrl == NULL )
	{	wchar_t	msg[64];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, msg, 63, NULL );
		TRACE( _T("OptionChainDialog::DownloadURL(-2): InternetOpenUrl() failed -> %s\n"), msg );
		return	-2;
	}

	CFile*	fp = new CFile;
	CFileException fileException;
	if ( !fp->Open( fName, CFile::modeReadWrite | CFile::modeCreate) )
	{
		TRACE( _T("COptionChainDialog::DownloadURL(-3): Can't open file %s, error -> %s\n"),
			fName, fileException.m_cause );
		return	-3;
	}

		// download the URL to a file
	unsigned long		bytesRead;
	BOOL	done = false;
	while ( ! done )
	{	bytesRead = 0;
		const	short	bufSize = 4096;
		char	buf[bufSize];
		done = InternetReadFile( hInetUrl, buf, bufSize, &bytesRead );
		if ( bytesRead > 0 )
			fp->Write( buf, bytesRead );
		done = done  &&  bytesRead == 0;	// both conditions must be satisfied
	}
		// flush the file and rewind it so it's ready for parsing
	fp->Flush();							// the download/writing phase is hereby concluded
	fp->Seek( 0, CFile::begin );			// Rewind

		// we're done with the URL handle
	BOOL	b_res = InternetCloseHandle( hInetUrl );
	if ( ! b_res )
	{	TRACE( _T("OptionChainDialog::DownloadURL(-4): InternetCloseHandle(hInetUrl) failed\n") );
		return	-4;
	}

		// we're done with the internet handle
	if ( ! InternetCloseHandle( hInet ) )
	{	TRACE( _T("OptionChainDialog::~COptionChainDialog(-5): InternetCloseHandle(hInet) failed\n") );
		return	-5;
	}

*/
/*
	char*	addr = hostEnt->h_addr_list[0];
	char	buf[16];
	sprintf( buf, "%d.%d.%d.%d", (short)(unsigned char)addr[0],
								 (short)(unsigned char)addr[1],
								 (short)(unsigned char)addr[2],
								 (short)(unsigned char)addr[3] );
	CString	ipAddr( buf );
*/
/*
		// get Yahoo Finance's ip address (as a CString)
	struct hostent* hostEnt = gethostbyname( "finance.yahoo.com" );
	if ( hostEnt == NULL  ||  hostEnt->h_length != 4 )
	{	TRACE( _T("OptionChainDialog::DownloadURL(-1): couldn't resolve '%s' to an IP address, h_length=%d\n"),
			hostEnt->h_aliases[0], hostEnt->h_length );
		delete	hostEnt;
		return	-1;
	}
	char*	addr = inet_ntoa( hostEnt->h_addr_list[0] );
	delete	hostEnt;											// we're done with hostEnt
	CString	ipAddr( addr );
	delete [] addr;
*/
/*
		// create a local (non-blocking) stream socket
	CAsyncSocket asynSckt;
	if ( ! asynSckt.Create() )								// bind to this (80, SOCK_STREAM, ipAddr) )
	{	TRACE( _T("OptionChainDialog::DownloadURL(-2): couldn't create a local socket\n") );
		return	-2;
	}

//	c_Abort.EnableWindow( TRUE );

		// get the port number for HTTP service on TCP
	SOCKADDR_IN	saServer;
	lpServEnt = getservbyname("http", "tcp");
	if (lpServEnt == NULL)
		saServer.sin_port = htons(80);
	else
		saServer.sin_port = lpServEnt->s_port;

		// wchar_t	msg[64];
		// int	lastErr = WSAGetLastError();
		// FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, lastErr, NULL, msg, 63, NULL );

	CSocketFile*	fp = new CSocketFile( &socket, TRUE );		// we're CArchive compatible here
	CFileException fileException;
	if ( !fp->Open( fName, CFile::modeReadWrite | CFile::modeCreate) )
	{
		TRACE( _T("COptionChainDialog::DownloadURL(-3): Can't open file '%s', error -> %s\n"),
			fName, fileException.m_cause );
		return	-3;
	}
*/
/*

		// send the request.  1st NULL is for headers, 2nd NULL is for additional POST/PUT info
	BOOL res = HttpSendRequest( hInetRequest, NULL, 0, NULL, 0 );
	if ( ! res )
	{	wchar_t	buf[64];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 63, NULL );
		TRACE( _T("OptionChainDialog::DownloadURL(-3): can't send the request on %s, error = %s\n"),
			url, buf );
		return	-3;
	}
*/
//--------------------------------------------------------------------------------------------------
/*
MEMORYSTATUS	COptionChainDialog::TraceMemoryStatus( short id )
{
	MEMORYSTATUS ms;
	ms.dwLength = sizeof( ms );
	SYSTEM_INFO	sysInfo;
	GetSystemInfo( &sysInfo );
	GlobalMemoryStatus( &ms );
	TRACE( _T("Memory Status (%d):\n"), id );
	TRACE( _T("   Total Phys: %d\n"), ms.dwTotalPhys );
	TRACE( _T("   Avail Phys: %d\n"), ms.dwAvailPhys );
	TRACE( _T("   Total Page File: %d\n"), ms.dwTotalPageFile );
	TRACE( _T("   Avail Page File: %d\n"), ms.dwAvailPageFile );
	TRACE( _T("   Total Virtual: %d\n"), ms.dwTotalVirtual );
	TRACE( _T("   Avail Virtual: %d\n"), ms.dwAvailVirtual );
	return	ms;
}			// TraceMemoryStatus()
*/
//--------------------------------------------------------------------------------------
/*
//short		COptionChainDialog::DownloadURL( CString url, bool linksToo )
DWORD	WINAPI	DownloadURL( void* threadCtx )
{
	COptionChainDialog* p_OCD = (COptionChainDialog*)threadCtx;
		// enable the following code to download html files for parsing...

//	ASSERT_VALID( p_OCD );
		// inspired by the diagram on p.194 of Nick Grattan and Marshall Brain's
		//									   Windows CE 3.0 Applications Programming
		// which tracks with code on p. 198-199 as well as being the skeleton of
		// Aaron Skonnard's approach in his
		//		Essential Wininet: Developing Applications Using the Windows Internet API
		//						   with RAS, ISAPI, ASP, & COM
	if ( ! p_OCD->hInetSession )
	{
#ifdef _DEBUG
		TRACE( _T("DownloadURL(-1): hInetSession not initialized\n") );
#endif
		return	-1;
	}

		// open an HTTP session on the url
		// 1st NULL for _T("GET"), 2nd NULL for HTTP version 1.1,
		// 3rd NULL for Referrer, 4th NULL indicates AcceptTypes are limited to text,
		// 5th NULL is for a callback context)
	p_OCD->c_StatusResult.SetWindowText( _T("Opening request...") );
//	CString		urlReq = p_OCD->urlRequest;								// type conversion to CString
	HINTERNET	hInetUrl = HttpOpenRequest( p_OCD->hInetSession, NULL,
											p_OCD->urlRequest, NULL, NULL, NULL,
			// Nick Grattan uses 0 for the flags
		INTERNET_FLAG_RELOAD	  |  INTERNET_FLAG_NO_UI	  |
		INTERNET_FLAG_NO_COOKIES  |  INTERNET_FLAG_NEED_FILE,	 NULL );
	if ( hInetUrl == NULL )
	{
#ifdef _DEBUG
		wchar_t	msg[64];
		int	err = GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, NULL, msg, 63, NULL );
		TRACE( _T("DownloadURL(-2): can't open a request on %s, error = %d --> %s\n"),
			p_OCD->urlRequest, err, msg );
#endif
		MessageBeep( MB_ICONEXCLAMATION );		// 0xFFFFFFFF is the system default
		p_OCD->c_StatusResult.SetWindowText( _T("OpenRequest failed!") );
		p_OCD->c_Fetch.EnableWindow( TRUE );
		return	-2;
	}

		// send the request.  1st NULL is for headers, 2nd NULL is for additional POST/PUT info
		// we'd really like to be able to abort HttpSendRequest(), but it would seem the
		// only option is TerminateThread, which would likely result in dangling references
	p_OCD->c_StatusResult.SetWindowText( _T("Sending request...") );
	BOOL   res = HttpSendRequest( hInetUrl, NULL, 0, NULL, 0 );
	if ( ! res )
	{
#ifdef _DEBUG
		wchar_t	msg[64];
		int	err = GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, NULL, msg, 63, NULL );
		TRACE( _T("DownloadURL(-3): can't send the request on %s, error = %d --> %s\n"),
			p_OCD->urlRequest, err, msg );
#endif
		MessageBeep( MB_ICONEXCLAMATION );		// 0xFFFFFFFF is the system default
		p_OCD->c_StatusResult.SetWindowText( _T("SendRequest failed!") );
		p_OCD->c_Fetch.EnableWindow( TRUE );
		return	-3;
	}

		// keep the downloaded file in the DB directory
	CString	fName = theApp.GetDBdir();
	fName += _T("\\tmp.pnh");							// an arbitrary file name
	CFile*	fp = new CFile;
	CFileException* fileException = NULL;
	if ( !fp->Open( fName, CFile::modeReadWrite | CFile::modeCreate), fileException )
	{
#ifdef _DEBUG
		TRACE( _T("OptionChainDialog::DownloadURL(-4):  Can't open file %s, error -> %s\n"),
			fName, fileException->m_cause );
#endif
		InternetCloseHandle( hInetUrl );
		return	-4;
	}
	fp->SetLength( 0 );

		// iteratively write the receive buffer to a temporary file
	p_OCD->c_StatusResult.SetWindowText( _T("Downloading...") );
	unsigned long	totalBytesRead = 0;			// totalBytesRead = 0;
	unsigned long	bytesRead;
	char			rcvBuf[1024];
	BOOL	  done = FALSE;
	while ( ! done )
	{
		done = InternetReadFile( hInetUrl, rcvBuf, sizeof(rcvBuf), &bytesRead );

			// stuff the received bytes into the temporary file
		fp->Write( rcvBuf, bytesRead );
		totalBytesRead += bytesRead;

		done  =  done  &&  bytesRead == 0;
	}
		// cleanup wininet
	InternetCloseHandle( hInetUrl );


//	wchar_t	statusBuf[32];
//	swprintf( statusBuf, _T("Downloaded %.1f KB"), totalBytesRead / 1024.0 );
//	p_OCD->c_StatusResult.SetWindowText( statusBuf );

		// flush the file and rewind it so it's ready for parsing
	fp->Flush();							// the download/writing phase is hereby concluded
	fp->Seek( 0, CFile::begin );			// Rewind


		// parse the downloaded file
		// four lines above are for testing with an already downloaded file
	short	nItems = p_OCD->c_OptionsList.GetItemCount();
	int	ii = p_OCD->urlRequest.Find( _T("&m=") );
	bool	linksToo = ii < 0;				// if we didn't find a date modifier, then we need links too!

	int		preExistingOptions = p_OCD->m_nCalls + p_OCD->m_nPuts;
	short	nOptions = p_OCD->ImportOptionChain( fp, p_OCD->urlRequest, linksToo );
		// we're done with the file...
	fp->Close();
	delete	fp;
	fp = NULL;

	if ( nOptions > -1 )
		p_OCD->DisplayStatusLine();

		// reposition the OptionsList so the new options are visible
	if ( preExistingOptions > 0  &&  nOptions > 0 )
	{	RECT	topItemRect, itemRect;
		int	topIdx = p_OCD->c_OptionsList.GetTopIndex();
		p_OCD->c_OptionsList.GetItemRect( topIdx, &topItemRect, LVIR_BOUNDS );
		p_OCD->c_OptionsList.GetItemRect( preExistingOptions + 1, &itemRect, LVIR_BOUNDS );
		CSize	cs;
		cs.cx = 0;
		cs.cy = itemRect.top - topItemRect.bottom;
		p_OCD->c_OptionsList.Scroll( cs );
	}

		// for now, we'll leave the downloaded file lying around
//	BOOL	failed = DeleteFile( fName );
//	if ( ! DeleteFile(fName) )
//	{
#ifdef _DEBUG
//		TRACE( _T("DownloadURL: DeleteFile(%s) failed\n"), fName );
#endif
//	}

	if ( fileException )
		delete	fileException;

	return	nOptions;
}			// DownloadUrl()
	// InternetOpen/InternetConnect/InternetOpenRequest/InternetSendRequest/InternetReadFile version
*/
//-------------------------------------------------------------------------------------------------
