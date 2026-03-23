// StockDefinitionDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "StockDefinitionDialog.h"
#include "Stock.h"
#include "StockManager.h"
#include "RegistryManager.h"
//#include "NillaHedgeDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC( CStockDefinitionDialog, CNillaDialog )

/////////////////////////////////////////////////////////////////////////////
// CStockDefinitionDialog dialog

CStockDefinitionDialog::CStockDefinitionDialog( CWnd* pParent /* =NULL */ )
	: CNillaDialog( IDD, pParent )
	, m_activeComboBox( NULL )
	, criticalRegion( false )
{
	//{{AFX_DATA_INIT(CStockDefinitionDialog)
	m_Volatility = 0.3f;
	m_StockSymbol = _T("");
	m_Desc = _T("");
	m_Q1exDividendDate = 0.0f;
	m_Q2exDividendDate = 0.0f;
	m_Q3exDividendDate = 0.0f;
	m_Q4exDividendDate = 0.0f;
	m_Q4exDividendDateCheck = FALSE;
	m_Q3exDividendDateCheck = FALSE;
	m_Q2exDividendDateCheck = FALSE;
	m_Q1exDividendDateCheck = FALSE;
	m_Q1Dividend = 0.0;
	m_Q2Dividend = 0.0;
	m_Q3Dividend = 0.0;
	m_Q4Dividend = 0.0;
	m_StockPrice = 0.0;
	//}}AFX_DATA_INIT
}

void CStockDefinitionDialog::DoDataExchange(CDataExchange* pDX)
{
	CNillaDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStockDefinitionDialog)
	DDX_Control(pDX, IDC_Q4DividendDatePicker, c_Q4DividendDatePicker);
	DDX_Control(pDX, IDC_Q3DividendDatePicker, c_Q3DividendDatePicker);
	DDX_Control(pDX, IDC_Q2DividendDatePicker, c_Q2DividendDatePicker);
	DDX_Control(pDX, IDC_Q1DividendDatePicker, c_Q1DividendDatePicker);
	DDX_Control(pDX, IDC_Volatility, c_Volatility);
	DDX_Control(pDX, IDC_StockSymbol, c_StockSymbol);
	DDX_Control(pDX, IDC_Q4ExDivDateCheck, c_Q4ExDivDateCheck);
	DDX_Control(pDX, IDC_Q4Dividend, c_Q4Dividend);
	DDX_Control(pDX, IDC_Q3ExDivDateCheck, c_Q3ExDivDateCheck);
	DDX_Control(pDX, IDC_Q3Dividend, c_Q3Dividend);
	DDX_Control(pDX, IDC_Q2ExDivDateCheck, c_Q2ExDivDateCheck);
	DDX_Control(pDX, IDC_Q2Dividend, c_Q2Dividend);
	DDX_Control(pDX, IDC_Q1ExDivDateCheck, c_Q1ExDivDateCheck);
	DDX_Control(pDX, IDC_Q1Dividend, c_Q1Dividend);
	DDX_Control(pDX, IDC_StockPrice, c_StockPrice);
	DDX_Control(pDX, IDC_CurrentYieldResult, c_CurrentYieldResult);
	DDX_Control(pDX, IDC_Desc, c_Desc);
	DDX_Text(pDX, IDC_Volatility, m_Volatility);
	DDV_MinMaxFloat(pDX, m_Volatility, 0., 1e15 );
	DDX_CBString(pDX, IDC_StockSymbol, m_StockSymbol);
	DDV_MaxChars(pDX, m_StockSymbol, 254);
	DDX_Text(pDX, IDC_Desc, m_Desc);
	DDV_MaxChars(pDX, m_Desc, 254);
	DDX_DateTimeCtrl(pDX, IDC_Q1DividendDatePicker, m_Q1exDividendDate);
	DDX_DateTimeCtrl(pDX, IDC_Q2DividendDatePicker, m_Q2exDividendDate);
	DDX_DateTimeCtrl(pDX, IDC_Q3DividendDatePicker, m_Q3exDividendDate);
	DDX_DateTimeCtrl(pDX, IDC_Q4DividendDatePicker, m_Q4exDividendDate);
	DDX_Check(pDX, IDC_Q4ExDivDateCheck, m_Q4exDividendDateCheck);
	DDX_Check(pDX, IDC_Q3ExDivDateCheck, m_Q3exDividendDateCheck);
	DDX_Check(pDX, IDC_Q2ExDivDateCheck, m_Q2exDividendDateCheck);
	DDX_Check(pDX, IDC_Q1ExDivDateCheck, m_Q1exDividendDateCheck);
	DDX_Text(pDX, IDC_Q1Dividend, m_Q1Dividend);
	DDV_MinMaxFloat(pDX, m_Q1Dividend, 0., 1e15 );
	DDX_Text(pDX, IDC_Q2Dividend, m_Q2Dividend);
	DDV_MinMaxFloat(pDX, m_Q2Dividend, 0., 1e15 );
	DDX_Text(pDX, IDC_Q3Dividend, m_Q3Dividend);
	DDV_MinMaxFloat(pDX, m_Q3Dividend, 0., 1e15 );
	DDX_Text(pDX, IDC_Q4Dividend, m_Q4Dividend);
	DDV_MinMaxFloat(pDX, m_Q4Dividend, 0., 1e15 );
	DDX_Text(pDX, IDC_StockPrice, m_StockPrice);
	DDV_MinMaxDouble(pDX, m_StockPrice, 0., 1e15 );
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStockDefinitionDialog, CNillaDialog)
	//{{AFX_MSG_MAP(CStockDefinitionDialog)
	ON_EN_KILLFOCUS(IDC_Volatility, OnKillFocusVolatility)
	ON_EN_KILLFOCUS(IDC_Q1Dividend, OnKillFocusQ1Dividend)
	ON_EN_KILLFOCUS(IDC_Q2Dividend, OnKillFocusQ2Dividend)
	ON_EN_KILLFOCUS(IDC_Q3Dividend, OnKillFocusQ3Dividend)
	ON_EN_KILLFOCUS(IDC_Q4Dividend, OnKillFocusQ4Dividend)
	ON_BN_CLICKED(IDC_Q1ExDivDateCheck, OnQ1ExDivDateCheck)
	ON_BN_CLICKED(IDC_Q4ExDivDateCheck, OnQ4ExDivDateCheck)
	ON_BN_CLICKED(IDC_Q3ExDivDateCheck, OnQ3ExDivDateCheck)
	ON_BN_CLICKED(IDC_Q2ExDivDateCheck, OnQ2ExDivDateCheck)
	ON_EN_KILLFOCUS(IDC_StockPrice, OnKillFocusStockPrice)
	ON_EN_SETFOCUS(IDC_Q4Dividend, OnSetFocusQ4Dividend)
	ON_EN_SETFOCUS(IDC_Volatility, OnSetFocusVolatility)
	ON_EN_SETFOCUS(IDC_Q3Dividend, OnSetFocusQ3Dividend)
	ON_EN_SETFOCUS(IDC_Q2Dividend, OnSetFocusQ2Dividend)
	ON_EN_SETFOCUS(IDC_Q1Dividend, OnSetFocusQ1Dividend)
	ON_EN_SETFOCUS(IDC_StockPrice, OnSetFocusStockPrice)
	ON_EN_SETFOCUS(IDC_Desc, OnSetFocusDesc)
	ON_EN_KILLFOCUS(IDC_Desc, OnKillFocusDesc)
	ON_COMMAND(ID_Delete_Definition, OnDeleteDefinition)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_SETFOCUS, IDC_Q1DividendDatePicker, OnSetFocusQ1DividendDatePicker)
	ON_NOTIFY(NM_KILLFOCUS, IDC_Q1DividendDatePicker, OnKillFocusQ1DividendDatePicker)
	ON_NOTIFY(NM_SETFOCUS, IDC_Q2DividendDatePicker, OnSetFocusQ2DividendDatePicker)
	ON_NOTIFY(NM_KILLFOCUS, IDC_Q2DividendDatePicker, OnKillFocusQ2DividendDatePicker)
	ON_NOTIFY(NM_SETFOCUS, IDC_Q3DividendDatePicker, OnSetFocusQ3DividendDatePicker)
	ON_NOTIFY(NM_KILLFOCUS, IDC_Q3DividendDatePicker, OnKillFocusQ3DividendDatePicker)
	ON_NOTIFY(NM_SETFOCUS, IDC_Q4DividendDatePicker, OnSetFocusQ4DividendDatePicker)
	ON_NOTIFY(NM_KILLFOCUS, IDC_Q4DividendDatePicker, OnKillFocusQ4DividendDatePicker)

	ON_CBN_SELCHANGE(IDC_StockSymbol, OnSelChangeStockSymbol)
	ON_CBN_KILLFOCUS(IDC_StockSymbol, OnKillFocusStockSymbol)			// HotKeys +
	ON_CBN_SETFOCUS(IDC_StockSymbol, OnSetFocusStockSymbol)				// HotKeys
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()													// HotKeys
	ON_MESSAGE(WM_HOTKEY, OnHotKey)										// HotKeys

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStockDefinitionDialog message handlers

BOOL	CStockDefinitionDialog::OnInitDialog( void )
{	CNillaDialog::OnInitDialog();
	RegisterHotKeys( IDC_StockSymbol, IDC_Desc );		// screens for WM5 internally

		// build the DropDown list of stock symbols (the pDB approach)
	CMapStringToPtr*	stocksMap = theApp.stkMgr->GetSymbolTable();
	c_StockSymbol.LoadMap( stocksMap );

		// Restrict Dividend DatePickers to fit within the associated Quarters
	COleDateTime minDate = COleDateTime::GetCurrentTime();
	COleDateTime maxDate = minDate;
	int	yr = minDate.GetYear();
	m_Q1exDividendDate.SetDate( yr,  2, 15 );
	m_Q2exDividendDate.SetDate( yr,  5, 15 );
	m_Q3exDividendDate.SetDate( yr,  8, 15 );
	m_Q4exDividendDate.SetDate( yr, 11, 15 );

		// Q1 Dividend DatePicker
	minDate.SetDate( yr, 1,  1 );
	maxDate.SetDate( yr, 3, 31 );
	c_Q1DividendDatePicker.SetRange( &minDate, &maxDate );

		// Q2 Dividend DatePicker
	minDate.SetDate( yr, 4,  1 );
	maxDate.SetDate( yr, 7, 31 );
	c_Q2DividendDatePicker.SetRange( &minDate, &maxDate );

		// Q3 Dividend DatePicker
	minDate.SetDate( yr, 7,  1 );
	maxDate.SetDate( yr, 9, 30 );
	c_Q3DividendDatePicker.SetRange( &minDate, &maxDate );

		// Q4 Dividend DatePicker
	minDate.SetDate( yr, 10,  1 );
	maxDate.SetDate( yr, 12, 31 );
	c_Q4DividendDatePicker.SetRange( &minDate, &maxDate );

	c_Q1DividendDatePicker.SetTime( m_Q1exDividendDate );
	c_Q2DividendDatePicker.SetTime( m_Q2exDividendDate );
	c_Q3DividendDatePicker.SetTime( m_Q3exDividendDate );
	c_Q4DividendDatePicker.SetTime( m_Q4exDividendDate );

	c_Q1DividendDatePicker.EnableWindow( false );
	c_Q2DividendDatePicker.EnableWindow( false );
	c_Q3DividendDatePicker.EnableWindow( false );
	c_Q4DividendDatePicker.EnableWindow( false );

	c_Q1Dividend.EnableWindow( false );
	c_Q2Dividend.EnableWindow( false );
	c_Q3Dividend.EnableWindow( false );
	c_Q4Dividend.EnableWindow( false );

		// allow for preloading a selection into the stock symbol ComboBox
	if ( m_StockSymbol != _T("") )		// modified since the constructor
	{	int	indx = c_StockSymbol.FindStringExact( 0, m_StockSymbol );
		if ( indx != CB_ERR )
			c_StockSymbol.SetCurSel( indx );
	
			// now clear m_StockSymbol so OnKillFocusStockSymbol()
			// will think the ComboBox selection has changed...
		CString sym = m_StockSymbol;
		m_StockSymbol = _T("");

			// load up all of the stock attributes...
		KillFocusStockSymbolWork( sym );
	}
		// Note, we still haven't formatted default values in the CEdit boxes
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//----------------------------------------------------------------------------------------
LRESULT		CStockDefinitionDialog::OnHotKey( WPARAM wParam, LPARAM lParam )
{		//	UINT fuModifiers = (UINT)LOWORD(lParam);		// don't care about modifiers
		//	DWORD ctrlID = (int)wParam;						// don't care which control the HotKey is registered to (any more)
		//	CWnd* wnd = GetDlgItem( ctrlID );				// also about the registered control
	if ( ! m_activeComboBox )
		return	0;									// nothing to do

	UINT uVirtKey = (UINT)HIWORD(lParam);
	if ( uVirtKey != VK_DOWN  &&  uVirtKey != VK_UP )
		return	0;											// only care about the up/down arrow keys

	CSeekComboBox* scb = (CSeekComboBox*)GetDlgItem( m_activeComboBox );
	scb->lastChar = uVirtKey;										// this in combination with SendMessage()
	WPARAM wp = MAKEWPARAM( m_activeComboBox, CBN_SELCHANGE );
	SendMessage( WM_COMMAND, wp, (LPARAM)scb->GetSafeHwnd() );		// emulates VK_UP/VK_DOWN as on PPC'02
	return	0;
}			// OnHotKey()
//----------------------------------------------------------------------------
void	CStockDefinitionDialog::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	CNillaDialog::OnActivate( nState, pWndOther, bMinimized );
	if ( nState == WA_ACTIVE || nState == WA_CLICKACTIVE )
		RegisterHotKeys( IDC_StockSymbol, IDC_Desc );		// screens for WM5 internally
}			// OnActivate()
//----------------------------------------------------------------------------
HBRUSH CStockDefinitionDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	return	CNillaDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}			// OnCtlColor()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::KillFocusStockSymbolWork( CString sym )
{
	if ( sym == m_StockSymbol )
		return;												// nothing to do
	SaveIfChanged( true );
	m_StockSymbol = sym;

		// lookup the stock Symbol in the Stocks Map
	CStock* aStock = theApp.stkMgr->GetStock( m_StockSymbol );
	if ( aStock == NULL )
		return;

		// load dialog local variables from aStock
	m_Volatility = aStock->volatility;
	m_Desc = aStock->getDesc();
	m_Q1exDividendDateCheck = aStock->dividendsArePaid[0];
	m_Q2exDividendDateCheck = aStock->dividendsArePaid[1];
	m_Q3exDividendDateCheck = aStock->dividendsArePaid[2];
	m_Q4exDividendDateCheck = aStock->dividendsArePaid[3];
	
	m_Q1exDividendDate = aStock->exDividendDates[0];
	m_Q2exDividendDate = aStock->exDividendDates[1];
	m_Q3exDividendDate = aStock->exDividendDates[2];
	m_Q4exDividendDate = aStock->exDividendDates[3];
	
	m_Q1Dividend = aStock->dividends[0];
	m_Q2Dividend = aStock->dividends[1];
	m_Q3Dividend = aStock->dividends[2];
	m_Q4Dividend = aStock->dividends[3];

	m_StockPrice = aStock->getMktPrice();
	delete	aStock;			// we're done with aStock

		// apply default values to undefined dividend dates
	SYSTEMTIME		st;
	GetLocalTime( &st );	// we actually only need yr (below) when aStock is 0

		// Q1 Dividends
	if ( m_Q1exDividendDate == (COleDateTime)0.0f )
		m_Q1exDividendDate.SetDate( st.wYear, 2, 15 );

		// Q2 Dividends
	if ( m_Q2exDividendDate == (COleDateTime)0.0f )
		m_Q2exDividendDate.SetDate( st.wYear,  5, 15 );

		// Q3 Dividends
	if ( m_Q3exDividendDate == (COleDateTime)0.0f )
		m_Q3exDividendDate.SetDate( st.wYear,  8, 15 );

		// Q4 Dividends
	if ( m_Q4exDividendDate == (COleDateTime)0.0f )
		m_Q4exDividendDate.SetDate( st.wYear,  11, 15 );


		// update the GUI
	setEditBox( "%.2f", m_StockPrice, IDC_StockPrice );
	setEditBox( "%.4f", m_Volatility, IDC_Volatility );
	c_Desc.SetWindowText( m_Desc );
	c_Q1ExDivDateCheck.SetCheck( m_Q1exDividendDateCheck ? BST_CHECKED : BST_UNCHECKED );
	c_Q2ExDivDateCheck.SetCheck( m_Q2exDividendDateCheck ? BST_CHECKED : BST_UNCHECKED );
	c_Q3ExDivDateCheck.SetCheck( m_Q3exDividendDateCheck ? BST_CHECKED : BST_UNCHECKED );
	c_Q4ExDivDateCheck.SetCheck( m_Q4exDividendDateCheck ? BST_CHECKED : BST_UNCHECKED );

	c_Q1DividendDatePicker.SetTime( m_Q1exDividendDate );
	c_Q2DividendDatePicker.SetTime( m_Q2exDividendDate );
	c_Q3DividendDatePicker.SetTime( m_Q3exDividendDate );
	c_Q4DividendDatePicker.SetTime( m_Q4exDividendDate );

	setEditBox( "%.2f", m_Q1Dividend, IDC_Q1Dividend );
	setEditBox( "%.2f", m_Q2Dividend, IDC_Q2Dividend );
	setEditBox( "%.2f", m_Q3Dividend, IDC_Q3Dividend );
	setEditBox( "%.2f", m_Q4Dividend, IDC_Q4Dividend );

		// compute analytics (current yield) and display results
	UpdateStatics();

		// Enable (dividend) controls based on m_variable state
	EnableDialogItems();
}			// KillFocusStockSymbolWork()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnKillFocusStockSymbol( void )
{	CString sym;
	m_activeComboBox = NULL;
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	if ( criticalRegion )
		return;
	c_StockSymbol.GetWindowText( sym );
	sym.MakeUpper();
	KillFocusStockSymbolWork( sym );
}			// OnKillFocusStockSymbol()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnSetFocusStockSymbol( void )
{
	m_activeComboBox = IDC_StockSymbol;
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusStockSymbol()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnSelChangeStockSymbol()
{	CString			sym;
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
	if ( sel < 0 ) sel = 0;							// should be superfluous
	c_StockSymbol.GetLBText( sel, sym );
	KillFocusStockSymbolWork( sym );
}			// OnSelChangeStockSymbol()
//----------------------------------------------------------------------------------------
void	CStockDefinitionDialog::SaveIfChanged( bool updateGUI /* = false */ )
{
	CStock*	aStock = NULL;
	short	prefs = DefinitionChangeSymbol | DefinitionClose;
	bool	saveVerify = true;
	if ( m_StockSymbol == _T("") )
		goto	Exit;						// nothing to look up

	if ( GetSaveVerifyPrefs(&prefs) )						// was:  theApp.regMgr->
	{		// updateGUI tells us whether this is a close or change symbol action
			// updateGUI should be true from killFocusStockSymbolWork, representing a change symbol action
			//			 and false from MainFrame, representing a close dialog action
		short	mask = updateGUI ? DefinitionChangeSymbol : DefinitionClose;
		saveVerify = (prefs & mask) != 0;
	}

	aStock = theApp.stkMgr->GetStock( m_StockSymbol );
	if ( aStock != NULL )
	{	bool	changed =
				aStock->dividendsArePaid[0] != m_Q1exDividendDateCheck
			||	aStock->dividendsArePaid[1] != m_Q2exDividendDateCheck
			||	aStock->dividendsArePaid[2] != m_Q3exDividendDateCheck
			||	aStock->dividendsArePaid[3] != m_Q4exDividendDateCheck
			||	aStock->exDividendDates[0]  != m_Q1exDividendDate
			||	aStock->exDividendDates[1]  != m_Q2exDividendDate
			||	aStock->exDividendDates[2]  != m_Q3exDividendDate
			||	aStock->exDividendDates[3]  != m_Q4exDividendDate
			||	fabs(m_Q1Dividend - aStock->dividends[0])	 > 1e-4
			||	fabs(m_Q2Dividend - aStock->dividends[1])	 > 1e-4
			||	fabs(m_Q3Dividend - aStock->dividends[2])	 > 1e-4
			||	fabs(m_Q4Dividend - aStock->dividends[3])	 > 1e-4
			||	fabs(m_StockPrice - aStock->getMktPrice())  > 1e-4
			||	fabs(m_Volatility - aStock->volatility)		 > 1e-5
			||	aStock->getDesc()			!= m_Desc;
		if ( ! changed )
			goto	Exit;
			// we have changes
		if ( saveVerify )
		{		// get permission to update the Stock definition?
				// if the caller is MainFrame, updateGUI will be false (the default value)
				// so we won't post a MessageBox, we'll just update and return.
			wchar_t	buf[320];
			swprintf( buf, _T("Save changes to '%s'?"), m_StockSymbol );
			criticalRegion = true;
			UINT result = MessageBox( buf, _T("Save changes?"),
							MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST );
			criticalRegion = false;
			if ( result != IDYES )
				goto	Exit;
		}
	}
	else
	{		// didn't find m_StockSymbol in the stocks map, so this is a new entry
		if ( saveVerify )
		{		// get permission to create a new Stock definition...
			wchar_t	buf[320];
			swprintf( buf, _T("Save '%s'?"), m_StockSymbol );
			criticalRegion = true;
			UINT result = MessageBox( buf, _T("Define new stock?"),
							  MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST );
			criticalRegion = false;
			if ( result != IDYES )
				goto	Exit;				// aStock == NULL --> no heap to cleanup
		}
		m_StockSymbol.MakeUpper();
		aStock = new CStock( m_StockSymbol );

			// add this entry to the StockSymbol ComboBox
		if ( updateGUI )
		{		// you'll get an access violation if you try this from MainFrame
				// because at that point (dialog has returned from DoModal)
				// the control doesn't exist anymore
			c_StockSymbol.AddString( m_StockSymbol );
		}
	}
	SaveStock( aStock );		// overwrite aStock's parameters with dialog values, then save
Exit:
	if ( aStock )
		delete	aStock;
}			// SaveIfChanged()
//----------------------------------------------------------------------------------------
void	CStockDefinitionDialog::SaveStock( CStock* aStock )
{
	aStock->dividends[0]		= m_Q1Dividend;
	aStock->dividends[1]		= m_Q2Dividend;
	aStock->dividends[2]		= m_Q3Dividend;
	aStock->dividends[3]		= m_Q4Dividend;
	aStock->dividendsArePaid[0] = m_Q1exDividendDateCheck;
	aStock->dividendsArePaid[1] = m_Q2exDividendDateCheck;
	aStock->dividendsArePaid[2] = m_Q3exDividendDateCheck;
	aStock->dividendsArePaid[3] = m_Q4exDividendDateCheck;
	aStock->exDividendDates[0]  = m_Q1exDividendDate;
	aStock->exDividendDates[1]  = m_Q2exDividendDate;
	aStock->exDividendDates[2]  = m_Q3exDividendDate;
	aStock->exDividendDates[3]  = m_Q4exDividendDate;
	aStock->setMktPrice( m_StockPrice );
	aStock->volatility			= m_Volatility;
	aStock->setDesc( m_Desc );
	aStock->saveDesc();
	short	res = theApp.stkMgr->WriteStock( aStock );
#ifdef _DEBUG
	if ( res != 0 )
		TRACE( _T("StockDefinitionDialog::SaveStock: Stock write failed, res = %d\n"), res );
#endif
}			// SaveStock()
//----------------------------------------------------------------------------------------
void	CStockDefinitionDialog::UpdateStatics( void )
{
	if ( m_StockPrice == 0.0 )
		return;

	double	aggDiv = (m_Q1exDividendDateCheck ? m_Q1Dividend : 0.0)
				   + (m_Q2exDividendDateCheck ? m_Q2Dividend : 0.0)
				   + (m_Q3exDividendDateCheck ? m_Q3Dividend : 0.0)
				   + (m_Q4exDividendDateCheck ? m_Q4Dividend : 0.0);
	double	eqYield = log( ((double)m_StockPrice + aggDiv) / (double)m_StockPrice );
	setStatic( "%.3f", 100.0 * eqYield, IDC_CurrentYieldResult );
}			// UpdateStatics() 
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnKillFocusStockPrice( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_StockPrice );
	if ( ff == m_StockPrice )
		return;
	m_StockPrice = ff;

	setEditBox( "%.2f", m_StockPrice, IDC_StockPrice );
	UpdateStatics();
}			// OnKillFocusStockPrice()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnSetFocusStockPrice()
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	c_StockPrice.SetSel( 0, -1 );
}			// OnSetFocusStockPrice() 
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnKillFocusVolatility() 
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_Volatility );
	if ( ff == m_Volatility )
		return;
	m_Volatility = ff;

//	setEditBox( "%.3f", ff, IDC_Volatility );	
	UpdateStatics();
}			// OnKillFocusVolatility() 
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnSetFocusVolatility()
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	c_Volatility.SetSel( 0, -1 );
}			// OnSetFocusVolatility()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnKillFocusQ1Dividend()
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_Q1Dividend );
	if ( ff == m_Q1Dividend )
		return;
	m_Q1Dividend = ff;

	setEditBox( "%.2f", ff, IDC_Q1Dividend );		// echo dividend back to user
	UpdateStatics();
}			// OnKillFocusQ1Dividend()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnSetFocusQ1Dividend()
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	c_Q1Dividend.SetSel( 0, -1 );
}			// OnSetFocusQ1Dividend()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnKillFocusQ2Dividend()
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_Q2Dividend );
	if ( ff == m_Q2Dividend )
		return;
	m_Q2Dividend = ff;

	setEditBox( "%.2f", ff, IDC_Q2Dividend );		// echo dividend back to user
	UpdateStatics();
}			// OnKillFocusQ2Dividend()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnSetFocusQ2Dividend()
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	c_Q2Dividend.SetSel( 0, -1 );
}			// OnSetFocusQ2Dividend()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnKillFocusQ3Dividend()
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_Q3Dividend );
	if ( ff == m_Q3Dividend )
		return;
	m_Q3Dividend = ff;
	
	setEditBox( "%.2f", ff, IDC_Q3Dividend );		// echo dividend back to user
	UpdateStatics();
}			// OnKillFocusQ3Dividend()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnSetFocusQ3Dividend()
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	c_Q3Dividend.SetSel( 0, -1 );
}			// OnSetFocusQ3Dividend()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnKillFocusQ4Dividend()
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_Q4Dividend );
	if ( ff == m_Q4Dividend )
		return;
	m_Q4Dividend = ff;

	setEditBox( "%.2f", ff, IDC_Q4Dividend );		// echo dividend back to user
	UpdateStatics();
}			// OnKillFocusQ4Dividend()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnSetFocusQ4Dividend()
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	c_Q4Dividend.SetSel( 0, -1 );
}			// OnSetFocusQ4Dividend()
//----------------------------------------------------------------------------------------
void	CStockDefinitionDialog::OnKillFocusDesc()
{	CString cs;
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	c_Desc.GetWindowText( cs );
	cs.TrimLeft();
	cs.TrimRight();
	if ( cs == m_Desc )
		return;
	m_Desc = cs;
}			// OnKillFocusDesc()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnSetFocusDesc()
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	c_Desc.SetSel( 0, -1 );
}			// OnSetFocusDesc()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnQ1ExDivDateCheck()
{
	m_Q1exDividendDateCheck = c_Q1ExDivDateCheck.GetCheck() == BST_CHECKED;
	UpdateStatics();

	c_Q1DividendDatePicker.EnableWindow( m_Q1exDividendDateCheck );

	c_Q1Dividend.EnableWindow( m_Q1exDividendDateCheck );
}			// OnQ1ExDivDateCheck()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnQ2ExDivDateCheck()
{
	m_Q2exDividendDateCheck = c_Q2ExDivDateCheck.GetCheck() == BST_CHECKED;
	UpdateStatics();

	c_Q2DividendDatePicker.EnableWindow( m_Q2exDividendDateCheck );

	c_Q2Dividend.EnableWindow( m_Q2exDividendDateCheck );
}			// OnQ2ExDivDateCheck()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnQ3ExDivDateCheck()
{
	m_Q3exDividendDateCheck = c_Q3ExDivDateCheck.GetCheck() == BST_CHECKED;
	UpdateStatics();

	c_Q3DividendDatePicker.EnableWindow( m_Q3exDividendDateCheck );
//	c_Q3DividendDatePicker.SetFocus();

	c_Q3Dividend.EnableWindow( m_Q3exDividendDateCheck );
}			// OnQ3ExDivDateCheck()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnQ4ExDivDateCheck()
{
	m_Q4exDividendDateCheck = c_Q4ExDivDateCheck.GetCheck() == BST_CHECKED;
	UpdateStatics();

	c_Q4DividendDatePicker.EnableWindow( m_Q4exDividendDateCheck );
//	c_Q4DividendDatePicker.SetFocus();

	c_Q4Dividend.EnableWindow( m_Q4exDividendDateCheck );
}			// OnQ4ExDivDateCheck()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::EnableDialogItems( void )
{
	c_Q1DividendDatePicker.EnableWindow( m_Q1exDividendDateCheck );
	c_Q2DividendDatePicker.EnableWindow( m_Q2exDividendDateCheck );
	c_Q3DividendDatePicker.EnableWindow( m_Q3exDividendDateCheck );
	c_Q4DividendDatePicker.EnableWindow( m_Q4exDividendDateCheck );

	c_Q1Dividend.EnableWindow( m_Q1exDividendDateCheck );
	c_Q2Dividend.EnableWindow( m_Q2exDividendDateCheck );
	c_Q3Dividend.EnableWindow( m_Q3exDividendDateCheck );
	c_Q4Dividend.EnableWindow( m_Q4exDividendDateCheck );
}			// EnableDialogItems()
//----------------------------------------------------------------------------------------
void	CStockDefinitionDialog::OnDeleteDefinition() 
{		// respond to Ctrl-K (kill) keyboard command
	if ( m_StockSymbol == _T("") )
		return;

	CStock*	stk = theApp.stkMgr->GetStock( m_StockSymbol );
	if ( stk == NULL )
		return;

	CMapPtrToPtr*	posLst = stk->getPositionList();
	short	nItems = posLst->GetCount();
	if ( nItems > 0 )
	{	wchar_t	buf[400];
		swprintf( buf, _T("Deleting '%s' will irrevocably delete its %d positions. Okay to proceed?"),
					m_StockSymbol, nItems );
		UINT result = MessageBox( buf, _T("Delete stock/positions?"),
							  MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST );
		if ( result == IDYES )
		{		// okay to delete the definition and all of its positions
			short	res = theApp.stkMgr->DeleteBOS( (CBOS*)stk );
			if ( res != 0 )
			{
#ifdef _DEBUG
				TRACE( _T("StockDefinitionDialog::OnDeleteDefinition: DeleteBOS failed, res=%d"), res );
#endif
			}
				// now remove m_StockSymbol from c_StockSymbol and reset it to _T("")
			int	idx = c_StockSymbol.FindStringExact( -1, m_StockSymbol );
			if ( idx != CB_ERR )
				c_StockSymbol.DeleteString( idx );
			c_StockSymbol.SetCurSel( -1 );				// clears the Edit portion of the ComboBox
		}
	}
	delete	stk;
	return;
}			// OnDeleteDefinition()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnKillFocusQ1DividendDatePicker(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	*pResult = 0;
}			// OnKillFocusQ1DividendDatePicker()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnSetFocusQ1DividendDatePicker(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	*pResult = 0;
}			// OnSetFocusQ1DividendDatePicker()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnKillFocusQ2DividendDatePicker(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	*pResult = 0;
}			// OnKillFocusQ2DividendDatePicker()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnSetFocusQ2DividendDatePicker(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	*pResult = 0;
}			// OnSetFocusQ2DividendDatePicker()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnKillFocusQ3DividendDatePicker(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	*pResult = 0;
}			// OnKillFocusQ3DividendDatePicker()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnSetFocusQ3DividendDatePicker(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	*pResult = 0;
}			// OnSetFocusQ3DividendDatePicker()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnKillFocusQ4DividendDatePicker(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	*pResult = 0;
}			// OnKillFocusQ4DividendDatePicker()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnSetFocusQ4DividendDatePicker(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	*pResult = 0;
}			// OnSetFocusQ4DividendDatePicker()
//----------------------------------------------------------------------------------------
/*
void CStockDefinitionDialog::killFocusQ1DividendDateWork( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	COleDateTime	aDate;
	c_Q1exDividendDate.GetTime( aDate );
	if ( m_Q1exDividendDate == aDate )
		return;
	m_Q1exDividendDate = aDate;
	*pResult = 0;
}			// killFocusQ1DividendDateWork()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnCloseUpQ1DividendDatePicker(NMHDR* pNMHDR, LRESULT* pResult) 
{
	killFocusQ1DividendDateWork( NMHDR* pNMHDR, LRESULT* pResult );
}			// OnCloseUpQ1DividendDatePicker()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnKillFocusQ1DividendDatePicker(NMHDR* pNMHDR, LRESULT* pResult) 
{
	killFocusQ1DividendDateWork( NMHDR* pNMHDR, LRESULT* pResult );
}			// OnKillFocusQ1DividendDatePicker()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::killFocusQ2DividendDateWork( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	COleDateTime	aDate;
	c_Q2exDividendDate.GetTime( aDate );
	if ( m_Q2exDividendDate == aDate )
		return;
	m_Q2exDividendDate = aDate;
	*pResult = 0;
}			// killFocusQ2DividendDateWork()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnCloseUpQ2DividendDatePicker(NMHDR* pNMHDR, LRESULT* pResult) 
{
	killFocusQ2DividendDateWork( NMHDR* pNMHDR, LRESULT* pResult );
}			// OnCloseUpQ2DividendDatePicker()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnKillFocusQ2DividendDatePicker(NMHDR* pNMHDR, LRESULT* pResult) 
{
	killFocusQ2DividendDateWork( NMHDR* pNMHDR, LRESULT* pResult );
}			// OnKillFocusQ2DividendDatePicker
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::killFocusQ3DividendDateWork( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	COleDateTime	aDate;
	c_Q3exDividendDate.GetTime( aDate );
	if ( m_Q3exDividendDate == aDate )
		return;
	m_Q3exDividendDate = aDate;
	*pResult = 0;
}			// killFocusQ3DividendDateWork()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnCloseUpQ3DividendDatePicker(NMHDR* pNMHDR, LRESULT* pResult) 
{
	killFocusQ3DividendDateWork( NMHDR* pNMHDR, LRESULT* pResult );
}			// OnCloseUpQ3DividendDatePicker()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnKillFocusQ3DividendDatePicker(NMHDR* pNMHDR, LRESULT* pResult) 
{
	killFocusQ3DividendDateWork( NMHDR* pNMHDR, LRESULT* pResult );
}			// OnKillFocusQ3DividendDatePicker()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::killFocusQ4DividendDateWork( NMHDR* pNMHDR, LRESULT* pResult )
{
	COleDateTime	aDate;
	c_Q4exDividendDate.GetTime( aDate );
	if ( m_Q4exDividendDate == aDate )
		return;
	m_Q4exDividendDate = aDate;
	*pResult = 0;
}			// killFocusQ4DividendDateWork()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnCloseUpQ4DividendDatePicker(NMHDR* pNMHDR, LRESULT* pResult) 
{
	killFocusQ4DividendDateWork( NMHDR* pNMHDR, LRESULT* pResult );
}			// OnCloseUpQ4DividendDatePicker()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnKillFocusQ4DividendDatePicker(NMHDR* pNMHDR, LRESULT* pResult) 
{
	killFocusQ4DividendDateWork( NMHDR* pNMHDR, LRESULT* pResult );
}			// OnKillFocusQ4DividendDatePicker()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::resetControls( void )
{
	c_StockPrice.SetWindowText( _T("") );
	c_Volatility.SetWindowText( _T("") );
	c_Desc.SetWindowText( _T("") );
	c_Q1ExDivDateCheck.SetCheck( BST_UNCHECKED );
	c_Q2ExDivDateCheck.SetCheck( BST_UNCHECKED );
	c_Q3ExDivDateCheck.SetCheck( BST_UNCHECKED );
	c_Q4ExDivDateCheck.SetCheck( BST_UNCHECKED );
	c_Q1DividendDatePicker.EnableWindow( m_Q1exDividendDateCheck );
	c_Q2DividendDatePicker.EnableWindow( m_Q2exDividendDateCheck );
	c_Q3DividendDatePicker.EnableWindow( m_Q3exDividendDateCheck );
	c_Q4DividendDatePicker.EnableWindow( m_Q4exDividendDateCheck );
	c_Q1Dividend.EnableWindow( m_Q1exDividendDateCheck );
	c_Q2Dividend.EnableWindow( m_Q2exDividendDateCheck );
	c_Q3Dividend.EnableWindow( m_Q3exDividendDateCheck );
	c_Q4Dividend.EnableWindow( m_Q4exDividendDateCheck );
}			// resetControls()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::OnKillFocusDesc( void )
{
	CString cs = getEditBoxCString( IDC_Desc );
	c_Note.SetWindowText( m_Note );
	setEditBox( cs, IDC_Desc );		// echo trimmed Desc back to user
}			// OnKillFocusDesc()
//----------------------------------------------------------------------------------------
void CStockDefinitionDialog::enableDialogItems( bool enDis )
{		// a stock symbol has been selected ...
	ASSERT( m_StockSymbol != _T("") );

		// make Volatility & Desc CEdit boxes readOnly or readWrite
	c_Desc.EnableWindow( enDis );
	c_StockPrice.EnableWindow( enDis );
	c_Volatility.EnableWindow( enDis );

		// enable the Dividend check boxes
	c_Q1ExDivDateCheck.EnableWindow( enDis );
	c_Q2ExDivDateCheck.EnableWindow( enDis );
	c_Q3ExDivDateCheck.EnableWindow( enDis );
	c_Q4ExDivDateCheck.EnableWindow( enDis );

		// enable Dividend CEdit boxes and DateTime pickers
		// based on the associated check button state

		// enable the date/time pickers
	c_Q1DividendDatePicker.EnableWindow( m_Q1exDividendDateCheck );
	c_Q2DividendDatePicker.EnableWindow( m_Q2exDividendDateCheck );
	c_Q3DividendDatePicker.EnableWindow( m_Q3exDividendDateCheck );
	c_Q4DividendDatePicker.EnableWindow( m_Q4exDividendDateCheck );

		// enable the CEdit Dividend boxes
	c_Q1Dividend.EnableWindow( m_Q1exDividendDateCheck );
	c_Q2Dividend.EnableWindow( m_Q2exDividendDateCheck );
	c_Q3Dividend.EnableWindow( m_Q3exDividendDateCheck );
	c_Q4Dividend.EnableWindow( m_Q4exDividendDateCheck );
}			// enableDialogItems()
*/
//----------------------------------------------------------------------------------------
