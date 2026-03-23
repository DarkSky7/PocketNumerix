// OptionDefinitionDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "OptionDefinitionDialog.h"
#include "StockDefinitionDialog.h"
//#include "Bond.h"					// for calcYears
#include "Option.h"
#include "OptionType.h"
#include "Stock.h"
//#include "NillaHedgeDoc.h"
#include "OptionManager.h"
#include "StockManager.h"
#include "RegistryManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern	CString months[];

IMPLEMENT_DYNAMIC( COptionDefinitionDialog, CNillaDialog )

/////////////////////////////////////////////////////////////////////////////
// COptionDefinitionDialog dialog

COptionDefinitionDialog::COptionDefinitionDialog( CWnd* pParent /* =NULL */ )
	: CNillaDialog( COptionDefinitionDialog::IDD, pParent)
	, m_activeComboBox( NULL )
	, m_BSchecked( false )
	, criticalRegionOption( false )
	, criticalRegionStock( false )
{
	//{{AFX_DATA_INIT(COptionDefinitionDialog)
	m_OptionSymbol	= _T("");			// inherited from COptionDialog
	m_OptionPrice	= 0.0;				// inherited from COptionDialog
	m_StockSymbol	= _T("");
	m_StrikePrice	= 0.0;
	m_Desc			= _T("");
	m_RiskFreeRate	= 3.0f;
	m_ExpiryDate = (COleDateTime)0.0f;
	//}}AFX_DATA_INIT
	evcs.fromDate = COleDateTime::GetCurrentTime();

	putOrCall = UnknownOptionType;
}

COptionDefinitionDialog::~COptionDefinitionDialog( void )
{	//	delete	theOption;
}

void COptionDefinitionDialog::DoDataExchange(CDataExchange* pDX)
{
	CNillaDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionDefinitionDialog)
	DDX_Control(pDX, IDC_ExpiryDate, c_ExpiryDate);
	DDX_Control(pDX, IDC_StockSymbol, c_StockSymbol);
	DDX_Control(pDX, IDC_VegaResult, c_VegaResult);
	DDX_Control(pDX, IDC_ThetaResult, c_ThetaResult);
	DDX_Control(pDX, IDC_StrikePrice, c_StrikePrice);
	DDX_Control(pDX, IDC_RhoRresult, c_RhoRresult);
	DDX_Control(pDX, IDC_RhoDresult, c_RhoDresult);
	DDX_Control(pDX, IDC_PutRadio, c_PutRadio);
	DDX_Control(pDX, IDC_OptionSymbol, c_OptionSymbol);
	DDX_Control(pDX, IDC_OptionPrice, c_OptionPrice);
	DDX_Control(pDX, IDC_ImpliedVolatilityResult, c_ImpliedVolatilityResult);
	DDX_Control(pDX, IDC_GammaResult, c_GammaResult);
	DDX_Control(pDX, IDC_Desc, c_Desc);
	DDX_Control(pDX, IDC_DeltaResult, c_DeltaResult);
	DDX_Control(pDX, IDC_CallRadio, c_CallRadio);
	DDX_Control(pDX, IDC_BlackScholesResult, c_BlackScholesResult);
	DDX_CBString(pDX, IDC_OptionSymbol, m_OptionSymbol);
	DDV_MaxChars(pDX, m_OptionSymbol, 254);
	DDX_Text(pDX, IDC_OptionPrice, m_OptionPrice);
	DDX_CBString(pDX, IDC_StockSymbol, m_StockSymbol);
	DDV_MaxChars(pDX, m_StockSymbol, 254);
	DDX_Text(pDX, IDC_StrikePrice, m_StrikePrice);
	DDX_Text(pDX, IDC_Desc, m_Desc);
	DDV_MaxChars(pDX, m_Desc, 254);
	DDX_DateTimeCtrl(pDX, IDC_ExpiryDate, m_ExpiryDate);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BS, c_BScheckButton);
}

BEGIN_MESSAGE_MAP(COptionDefinitionDialog, CNillaDialog)
	//{{AFX_MSG_MAP(COptionDefinitionDialog)
	ON_BN_CLICKED(IDC_CallRadio, OnCallButtonClicked)
	ON_BN_CLICKED(IDC_PutRadio, OnPutButtonClicked)
	ON_EN_KILLFOCUS(IDC_StrikePrice, OnKillFocusStrikePrice)
	ON_EN_KILLFOCUS(IDC_OptionPrice, OnKillFocusOptionPrice)
	ON_NOTIFY(NM_KILLFOCUS, IDC_ExpiryDate, OnKillFocusExpiryDate)
	ON_EN_SETFOCUS(IDC_Desc, OnSetFocusDesc)
	ON_EN_SETFOCUS(IDC_OptionPrice, OnSetFocusOptionPrice)
	ON_EN_SETFOCUS(IDC_StrikePrice, OnSetFocusStrikePrice)
	ON_EN_KILLFOCUS(IDC_Desc, OnKillFocusDesc)
	ON_COMMAND(ID_Delete_Definition, OnDeleteDefinition)
	ON_NOTIFY(DTN_CLOSEUP, IDC_ExpiryDate, OnCloseUpExpiryDate)
	ON_WM_CTLCOLOR()

	ON_CBN_SELCHANGE(IDC_OptionSymbol, OnSelChangeOptionSymbol)
	ON_CBN_SELCHANGE(IDC_StockSymbol, OnSelChangeStockSymbol)
	ON_CBN_KILLFOCUS(IDC_OptionSymbol, OnKillFocusOptionSymbol)			// HotKeys +
	ON_CBN_KILLFOCUS(IDC_StockSymbol, OnKillFocusStockSymbol)			// HotKeys +
	ON_CBN_SETFOCUS(IDC_OptionSymbol, OnSetFocusOptionSymbol)			// HotKeys
	ON_CBN_SETFOCUS(IDC_StockSymbol, OnSetFocusStockSymbol)				// HotKeys
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()													// HotKeys
	ON_MESSAGE(WM_HOTKEY, OnHotKey)										// HotKeys
	ON_NOTIFY(NM_SETFOCUS, IDC_ExpiryDate, OnSetFocusExpiryDate)
	ON_BN_CLICKED(IDC_BS, OnClickBS)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionDefinitionDialog message handlers

BOOL	COptionDefinitionDialog::OnInitDialog() 
{
	CNillaDialog::OnInitDialog();
	RegisterHotKeys( IDC_OptionSymbol, IDC_StockSymbol );	// screens for WM5 internally

		// set up the RiskFreeRate
	if ( ! GetRiskFreeRate(&m_RiskFreeRate) )			// was:  theApp.regMgr->
	{
#ifdef _DEBUG
		TRACE( _T("OptionAnalyzerDialog::OnInitDialog: no registerd value for RiskFreeRate.\n") );
#endif
	}
		// NOTE:  there's no control to update - m_RiskFreeRate is for eval purposes only
	evcs.riskFreeRate = m_RiskFreeRate / 100.0;

		// set up the OptionSymbol ComboBox
	CMapStringToPtr*	optionsMap = theApp.optMgr->GetSymbolTable();
	c_OptionSymbol.LoadMap( optionsMap );

		// now set up the StockSymbol ComboBox
	CMapStringToPtr*	stocksMap = theApp.stkMgr->GetSymbolTable();
	c_StockSymbol.LoadMap( stocksMap );

		// Set the m_ExpiryDate to the third Friday of the next month (default)
	COleDateTime	today = COleDateTime::GetCurrentTime();
	COleDateTime	monthAhead = NextMonth( today );
	m_ExpiryDate = ThirdFriday( monthAhead );
	c_ExpiryDate.SetTime( m_ExpiryDate );

	m_BSchecked = true;
	c_BScheckButton.SetCheck( m_BSchecked ? BST_CHECKED : BST_UNCHECKED );

		// allow for preloading a selection into the option symbol ComboBox
	if ( m_OptionSymbol != _T("") )		// modified since the constructor
	{
		int	indx = c_OptionSymbol.FindStringExact( 0, m_OptionSymbol );
		if ( indx != CB_ERR )
			c_OptionSymbol.SetCurSel( indx );

			// now clear m_OptionSymbol so OnKillFocusOptionSymbol()
			// will think the ComboBox selection has changed...
		CString sym = m_OptionSymbol;
		m_OptionSymbol = _T("");
		KillFocusOptionSymbolWork( sym );

			// override the focus set in OnKillFocusOptionSymbol()
		c_OptionSymbol.SetFocus();
		return	FALSE;
	}
	return	TRUE;		// return TRUE unless you set the focus to a control
						// EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//----------------------------------------------------------------------------------------
HBRUSH	COptionDefinitionDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	return	CNillaDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}			// OnCtlColor()
//----------------------------------------------------------------------------
void	COptionDefinitionDialog::ClearStatics( void )
{
	setStatic( "", IDC_ImpliedVolatilityResult );
	setStatic( "", IDC_BlackScholesResult );
	setStatic( "", IDC_DeltaResult );
	setStatic( "", IDC_ElasticityResult );
	setStatic( "", IDC_GammaResult );
	setStatic( "", IDC_RhoRresult );
	setStatic( "", IDC_RhoDresult );
	setStatic( "", IDC_ThetaResult );
	setStatic( "", IDC_VegaResult );
}			// ClearStatics()
//----------------------------------------------------------------------------------------
void	COptionDefinitionDialog::EvalOption( void )
{
	CStock*		theStock = theApp.stkMgr->GetStock( m_StockSymbol );
		// evcs.fromDate initialized in the constructor
		// evcs.riskFreeRate initialized in OnInitDialog()
		// evcs.calcYrsToExpiry set in killFocusOptionSymbolWork()
	evcs.sigma = theStock->volatility;
	evcs.stockPrice = theStock->getMktPrice();
	evcs.calcPvDivs = true;

		// construct a STACK-resident Option for analytical purposes
	COption	theOption( "eval" );
	theOption.setMktPrice( m_OptionPrice );
	theOption.strikePrice = m_StrikePrice;
	theOption.putCall = putOrCall;
	theOption.setExpiry( m_ExpiryDate );
	theOption.underlying = theStock;

		// display computed values for:  implied volatility, European value, 
		// American value, and the Exercise boundary
		// American Value, either: Roll-Geske-Whaley (Call); or Geske-Johnson (Put)
/*	double	amerVal = amerValue( theStock, stockPrice,
							sigma, rfr, yrsToExpiry );
	setStatic( "%.3f", amerVal, IDC_AmerValueResult );

		// Exercise Boundary
	COleDateTime	exDivDate;
	double	lastDiv = theStock->lastDivB4Date( theOption.expiry, exDivDate );
		// sStar really isn't the exercise boundary anyway...
	double	err, exerBound = findSstar( theStock, stockPrice, lastDiv,
												   rfr, yrsToExpiry, err );
	if ( exerBound != 0.0 )
		setStatic( "%.3f", exerBound, IDC_ExerciseBoundaryResult );
	else
		setStatic( _T("~"), IDC_ExerciseBoundaryResult );
*/
		// implied volatility
	double	priceError = 0.001;
	bool	valid = theOption.ImpliedVolatility( evcs, priceError );
	setStatic( valid, "%.4f", evcs.sigma, IDC_ImpliedVolatilityResult );
	setStatic( valid, "%.3f", priceError, IDC_IvPricingErrorResult );

			// Black-Scholes value
	double euroVal;
	evcs.sigma = theStock->volatility;						// reload evcs' volatility
	bool euroValValid = theOption.EuroValue( evcs, euroVal );
	setStatic( euroValValid, "%.3f", euroVal, IDC_BlackScholesResult );

		// the Greeks ...
	double	delta;				// more definitive than res
	valid = theOption.Delta( evcs, delta );
	setStatic( valid, "%.4f", delta, IDC_DeltaResult );

	double	elasticity;
	if ( valid )
	{	double	denom = m_BSchecked  ?  euroVal  :  m_OptionPrice;
		valid = euroValValid  &&  ( denom != 0.0 );
		if ( valid )
		{	double	stockPrice = theOption.getStockPrice();
			elasticity = delta * stockPrice / denom;
#ifdef _DEBUG
			TRACE( _T("OptionDefinitionDialog::EvalOption: elasticity=%g <-- delta=%g * stockPrice=%g / %s=%g\n"),
				elasticity, delta, stockPrice, (denom==euroVal ? _T("euroVal") : _T("optPrice")), denom );
#endif
		}
	}
	setStatic( valid, "%.2f", elasticity, IDC_ElasticityResult );
	double	res;
	valid = theOption.Gamma( evcs, res );
	setStatic( valid, "%.4f", res, IDC_GammaResult );
	valid = theOption.RhoR( evcs, res );
	setStatic( valid, "%.4f", res, IDC_RhoRresult );
	valid = theOption.RhoD( evcs, res );
	setStatic( valid, "%.4f", res, IDC_RhoDresult );
	valid = theOption.Theta( evcs, res );
	setStatic( valid, "%.4f", res, IDC_ThetaResult );
	valid = theOption.Vega( evcs, res );
	setStatic( valid, "%.4f", res, IDC_VegaResult );
	valid = theOption.pITM( evcs, res );
	setStatic( valid, "%.1f%%", 100.0 * res, IDC_pITMResult );

	// unrolling the stack deletes theOption's underlying (theStock) as well
}			// EvalOption()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//							Option Symbol stuff								//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void COptionDefinitionDialog::KillFocusOptionSymbolWork( CString sym )
{		// the OptionSymbol ComboBox has lost focus.
	if ( sym == m_OptionSymbol )
		return;												// nothing to do
	SaveIfChanged( true );
	m_OptionSymbol = sym;

		// update the dialog according to the new option symbol ...
	COption*	theOption = theApp.optMgr->GetOption( m_OptionSymbol );
	if ( theOption == NULL )
	{		// no heap to cleanup in this case
		ClearStatics();
		return;
	}

		// load dialog variables from anOption (m_StockSymbol handled later)
	m_OptionPrice = theOption->getMktPrice();
	m_StrikePrice = theOption->strikePrice;
	m_ExpiryDate = theOption->getExpiry();
	evcs.calcYrsToExpiry = true;
		// trap a sentinel and substitute a default value
	if ( m_ExpiryDate == (COleDateTime)0.0f )
	{
		COleDateTime	monthAhead = NextMonth( COleDateTime::GetCurrentTime() );
		m_ExpiryDate = ThirdFriday( monthAhead );
	}
	m_Desc = theOption->getDesc();
	putOrCall = theOption->putCall;

		// update the GUI (c_StockSymbol handled later)
	setEditBox( "%.2f", m_OptionPrice, IDC_OptionPrice );
	setEditBox( "%.2f", m_StrikePrice, IDC_StrikePrice );
	c_ExpiryDate.SetTime( m_ExpiryDate );
	c_Desc.SetWindowText( m_Desc );
		// set the Vanilla Option Put/Call radio selector buttons
	c_PutRadio.SetCheck( putOrCall == Put ? BST_CHECKED : BST_UNCHECKED );
	c_CallRadio.SetCheck( putOrCall == Call ? BST_CHECKED : BST_UNCHECKED );

		// now handle m_StockSymbol, c_StockSymbol and the analytics
	if ( theOption->stockSymbol_ID >= 0 )
	{	m_StockSymbol = theApp.stkMgr->ReadSymbol( theOption->stockSymbol_ID );
		int indx = c_StockSymbol.FindStringExact( 0, m_StockSymbol );
		if( indx != CB_ERR )
			c_StockSymbol.SetCurSel( indx );

			// compute analytics and display results
		EvalOption();							// update statics
	}
	delete	theOption;
}			// killFocusOptionSymbolWork()
//----------------------------------------------------------------------------------------
void	COptionDefinitionDialog::OnKillFocusOptionSymbol( void )
{	CString sym;
	m_activeComboBox = NULL;								// HotKeys
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	if ( criticalRegionOption )
		return;
	c_OptionSymbol.GetWindowText( sym );
	sym.MakeUpper();
	KillFocusOptionSymbolWork( sym );
}			// OnKillFocusOptionSymbol()
//----------------------------------------------------------------------------
void	COptionDefinitionDialog::OnSetFocusOptionSymbol( void )
{
	m_activeComboBox = IDC_OptionSymbol;
	int cnt = c_OptionSymbol.GetCount();
	if ( cnt < 1 )
		SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusOptionSymbol()
//----------------------------------------------------------------------------
void	COptionDefinitionDialog::OnSelChangeOptionSymbol( void )
{	CString			sym;
	int cnt = c_OptionSymbol.GetCount();
	if ( cnt < 1 )
		return;
	int sel = c_OptionSymbol.GetCurSel();
#if ( _WIN32_WCE >= 0x420 )					// just WM5
	if ( c_OptionSymbol.lastChar == VK_DOWN  ||  c_OptionSymbol.lastChar == VK_UP )
	{	int	delta = ( c_OptionSymbol.lastChar == VK_DOWN  ?  1  :  -1 );
		sel += delta;
		if ( sel < 0 )
			sel = cnt - 1;
		if ( sel >= cnt )
			sel = 0;
		c_OptionSymbol.SetCurSel( sel );
	}
#endif
	if ( sel < 0 ) sel = 0;							// should be superfluous
	c_OptionSymbol.GetLBText( sel, sym );
#ifdef _DEBUG
	TRACE( _T("OptionDefinitionDialog::OnSelChangeOptionSymbol: sym=%s\n"), m_OptionSymbol );
#endif
	KillFocusOptionSymbolWork( sym );
}			// OnSelChangeOptionSymbol()
//----------------------------------------------------------------------------
LRESULT		COptionDefinitionDialog::OnHotKey( WPARAM wParam, LPARAM lParam )
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
void	COptionDefinitionDialog::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	CNillaDialog::OnActivate( nState, pWndOther, bMinimized );
	if ( nState == WA_ACTIVE || nState == WA_CLICKACTIVE )
		RegisterHotKeys( IDC_OptionSymbol, IDC_StockSymbol );	// screens for WM5 internally
}			// OnActivate()
//----------------------------------------------------------------------------
void	COptionDefinitionDialog::SaveIfChanged( bool updateGUI /* = false */ )
{
	COption* theOption = NULL;
	short	prefs = DefinitionChangeSymbol | DefinitionClose;
	bool	saveVerify = true;
	bool*	criticalRegion = NULL;					// ignore KillFocus events from the control originating the SelChange (if any)

		// compare database COption to dialog values
	if ( m_OptionSymbol == _T("") )
		goto	Exit;								// nothing to look up

	if ( m_activeComboBox == IDC_StockSymbol )
		criticalRegion = &criticalRegionStock;
	else if ( m_activeComboBox == IDC_OptionSymbol )
		criticalRegion = &criticalRegionOption;

	if ( GetSaveVerifyPrefs(&prefs) )						// was:  theApp.regMgr->
	{		// updateGUI tells us whether this is a close or change symbol action
			// updateGUI should be true from killFocusOptionSymbolWork, representing a change symbol action
			//			 and false from MainFrame, representing a close dialog action
		short	mask = updateGUI ? DefinitionChangeSymbol : DefinitionClose;
		saveVerify = (prefs & mask) != 0;
	}
	theOption = theApp.optMgr->GetOption( m_OptionSymbol );
	if ( theOption != NULL )
	{	bool	changed = 
					theOption->getExpiry()	!= m_ExpiryDate	 ||  fabs(m_StrikePrice - theOption->strikePrice)	> 1e-4
				||  theOption->getPutCall() != putOrCall	 ||  fabs(m_OptionPrice - theOption->getMktPrice())	> 1e-4
				||	theOption->getDesc()    != m_Desc;
		if ( ! changed )
		{		// this is a more expensive comparison, hence only checking if all others are unchanged
			CString stkSym = _T("");
			CStock* stk = theOption->getUnderlying();
			if ( stk )
			{	stkSym = stk->getSymbol();
				changed = ( stkSym != _T("")  &&  m_StockSymbol != stkSym );
			}
		}
		if ( ! changed )
			goto	Exit;
			// we have changes
		if ( saveVerify )
		{		// get permission to update the Option definition?
				// if the caller is MainFrame, updateGUI will be false (the default value)
				// so we won't post a MessageBox, we'll just update and return.
			wchar_t	buf[320];
			swprintf( buf, _T("Save changes to '%s'?"), m_OptionSymbol );
		
			if ( criticalRegion )
				*criticalRegion = true;
			UINT result = MessageBox( buf, _T("Save changes?"),
							MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST);
			if ( criticalRegion )
				*criticalRegion = false;
			if ( result != IDYES )
				goto	Exit;
		}
	}
	else
	{		// get permission to create a new Option definition...
			// currently only allowing caller to specify permission for saving new definitions
		if ( saveVerify )
		{		// updateGUI is true when called by killFocusOptionSymbolWork()	
				// we were obviously not called from MainFrame
			wchar_t	buf[320];
			swprintf( buf, _T("Save '%s'?"), m_OptionSymbol );

			if ( criticalRegion )
				*criticalRegion = true;
			UINT result = MessageBox( buf, _T("Define new option?"),
							MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST );
			if ( criticalRegion )
				*criticalRegion = false;

			if ( result != IDYES )
				goto	Exit;
		}
		m_OptionSymbol.MakeUpper();
		theOption = new COption( m_OptionSymbol );
			// add this symbol to the ComboBox list
		if ( updateGUI )
		{		// you'll get an access violation if you try this from MainFrame
				// because at that point (dialog has returned from DoModal)
				// the control doesn't exist anymore
			c_OptionSymbol.AddString( m_OptionSymbol );
		}
	}
	SaveOption( theOption );
Exit:
	if ( theOption )
		delete	theOption;
}			// SaveIfChanged()
//----------------------------------------------------------------------------
void	COptionDefinitionDialog::SaveOption( COption* theOption )
{
	ASSERT_VALID( theOption );

		// if the StockSymbol is new, create a CStock and insert it in the DB
	if ( m_StockSymbol != _T("") )
	{
		long	stkSym_ID = theApp.stkMgr->GetSymbol_ID( m_StockSymbol );
			// if no stock def exists, we need to create one
		if ( stkSym_ID < 0 )
		{		// need to create a new stock with this name
			CStock	stk( m_StockSymbol );
			short	res = theApp.stkMgr->WriteStock( &stk );
#ifdef _DEBUG
			if ( res != 0 )
				TRACE( _T("OptionDefinitionDialog::SaveOption: Stock write failed, res=%d\n"), res );
#endif
			stkSym_ID = stk.symbol_ID;
		}
		theOption->stockSymbol_ID = stkSym_ID;
	}
	// else, go with the defaults in COption

		// attributes coming from OptionDefinitionDialog
	theOption->setMktPrice( m_OptionPrice );
	theOption->strikePrice = m_StrikePrice;
	theOption->setExpiry( m_ExpiryDate );
	theOption->setPutCall( putOrCall );
	theOption->setDesc( m_Desc );
	theOption->saveDesc();			// commit the description to the DB
		// at this point, we'd have to write the option (if desc_ID changed)
		// even if mktPrice, et. al. hadn't changed
	short	res = theApp.optMgr->WriteOption( theOption );
#ifdef _DEBUG
	if ( res != 0 )
		TRACE( _T("OptionDefinitionDialog::SaveOption: Option write failed, res=%d\n"), res );
#endif
}			// SaveOption()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//							Stock Symbol stuff								//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void COptionDefinitionDialog::KillFocusStockSymbolWork( CString sym )
{		// the StockSymbol ComboBox has lost focus.
	if ( sym == m_StockSymbol )
		return;												// nothing to do
	m_StockSymbol = sym;

	evcs.calcPvDivs = true;
	evcs.stockPrice = 0.0;					// a sentinal value
	evcs.sigma = 0.0;						// another sentinal
	EvalOption();							// update CStatics
}			// KillFocusStockSymbolWork()
//----------------------------------------------------------------------------
void COptionDefinitionDialog::OnSelChangeStockSymbol( void )
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
//----------------------------------------------------------------------------
void	COptionDefinitionDialog::OnKillFocusStockSymbol( void )
{	CString sym;
	m_activeComboBox = NULL;						// HotKeys
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	if ( criticalRegionStock )
		return;
	c_StockSymbol.GetWindowText( sym );
	sym.MakeUpper();
	KillFocusStockSymbolWork( sym );
}			// OnKillFocusStockSymbol()
//----------------------------------------------------------------------------
void	COptionDefinitionDialog::OnSetFocusStockSymbol( void )
{
	m_activeComboBox = IDC_StockSymbol;
	int cnt = c_StockSymbol.GetCount();
	if ( cnt < 1 )
		SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusStockSymbol()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Other Controls								//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void COptionDefinitionDialog::OnCallButtonClicked() 
{		// this is called even when a tab is entered
		// (i.e. the control merely gets the focus)
		// the control is only getting focus on the first time through the dialog
		// we're using IsDlgButtonChecked because
		// pBu->GetCheck() returns the opposite of the expected BST_ value
//	unsigned int	preState = IsDlgButtonChecked(IDC_CallRadio);
//	unsigned int	postState = (preState == BST_CHECKED) ? BST_UNCHECKED : BST_CHECKED;
//	putOrCall = (postState == BST_CHECKED) ? COption::Call : COption::Put;
		// we're using CheckRadioButton because pBu->SetCheck() appears only to be
		// capable of setting the button state to CHECKED (never UNCHECKED).
		// Auto Style should NOT set in the resource editor
//	CheckRadioButton(IDC_PutRadio, IDC_CallRadio, (preState) ? IDC_PutRadio : IDC_CallRadio);
		// try this...
	putOrCall = Call;
	CheckRadioButton( IDC_PutRadio, IDC_CallRadio, IDC_CallRadio );

	EvalOption();			// update CStatic(s)

//	c_ExpiryDate.SetFocus();
}			// OnCallButtonClicked()
//----------------------------------------------------------------------------------------
void COptionDefinitionDialog::OnPutButtonClicked() 
{		// this is called even when a tab is entered
		// (i.e. the control merely gets the focus)
		// the control is only getting focus on the first time through the dialog
		// we're using IsDlgButtonChecked because
		// pBu->GetCheck() returns the opposite of the expected BST_ value
//	unsigned int	preState = IsDlgButtonChecked(IDC_PutRadio);
//	unsigned int	postState = (preState == BST_CHECKED) ? BST_UNCHECKED : BST_CHECKED;
//	putOrCall = (postState == BST_CHECKED) ? COption::Put : COption::Call;

		// we're using CheckRadioButton because pBu->SetCheck() appears only to be
		// capable of setting the button state to CHECKED (never UNCHECKED).
		// Auto Style should NOT set in the resource editor
//	CheckRadioButton(IDC_PutRadio, IDC_CallRadio, (preState) ? IDC_CallRadio : IDC_PutRadio);
		// try this...
	putOrCall = Put;
	CheckRadioButton( IDC_PutRadio, IDC_CallRadio, IDC_PutRadio );

	EvalOption();			// update CStatic(s)

//	c_ExpiryDate.SetFocus();
}			// OnPutButtonClicked()
//----------------------------------------------------------------------------------------
void COptionDefinitionDialog::OnKillFocusOptionPrice() 
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_OptionPrice );
	if ( ff == m_OptionPrice )
		return;
	m_OptionPrice = ff;

	setEditBox( "%.2f", m_OptionPrice, IDC_OptionPrice );

	EvalOption();				// update CStatic(s)
//	c_StrikePrice.SetFocus();
}			// OnKillFocusMarketPrice()
//----------------------------------------------------------------------------------------
void	COptionDefinitionDialog::OnSetFocusOptionPrice( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	c_OptionPrice.SetSel( 0, -1 );
}			// OnSetFocusOptionPrice()
//----------------------------------------------------------------------------------------
void COptionDefinitionDialog::OnKillFocusStrikePrice() 
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_StrikePrice );
	if ( ff == m_StrikePrice )
		return;
	m_StrikePrice = ff;

	setEditBox( "%.2f", m_StrikePrice, IDC_StrikePrice );	
	EvalOption();				// update CStatic(s)
}			// OnKillFocusStrikePrice()
//----------------------------------------------------------------------------------------
void	COptionDefinitionDialog::OnSetFocusStrikePrice( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	c_StrikePrice.SetSel( 0, -1 );
}			// OnSetFocusStrikePrice()
//----------------------------------------------------------------------------------------
void COptionDefinitionDialog::KillFocusExpiryDateWork( NMHDR* pNMHDR, LRESULT* pResult )
{
	COleDateTime	dt;
	DWORD status = c_ExpiryDate.GetTime( dt );
	if ( dt == m_ExpiryDate )
		return;
	m_ExpiryDate = dt;
	evcs.calcYrsToExpiry = true;
	evcs.yrsToExpiry = 0.0;						// a sentinal value

	EvalOption();			// update CStatic(s)
	*pResult = 0;
}			// KillFocusExpiryDateWork()
//----------------------------------------------------------------------------------------
void COptionDefinitionDialog::OnKillFocusExpiryDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	KillFocusExpiryDateWork( pNMHDR, pResult );
}			// OnKillfocusExpiryDate()
//----------------------------------------------------------------------------------------
void COptionDefinitionDialog::OnSetFocusExpiryDate(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	*pResult = 0;
}
//----------------------------------------------------------------------------------------
void COptionDefinitionDialog::OnCloseUpExpiryDate(NMHDR* pNMHDR, LRESULT* pResult)
{
	KillFocusExpiryDateWork( pNMHDR, pResult );
}			// OnCloseUpExpiryDate()
//----------------------------------------------------------------------------------------
void	COptionDefinitionDialog::OnKillFocusDesc( void )
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
void	COptionDefinitionDialog::OnSetFocusDesc( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	c_Desc.SetSel( 0, -1 );
}			// OnSetFocusDesc()
//----------------------------------------------------------------------------------------
void	COptionDefinitionDialog::OnDeleteDefinition( void )
{		// respond to Ctrl-K (kill) keyboard command
	if ( m_OptionSymbol == _T("") )
		return;

	COption*	opt = theApp.optMgr->GetOption( m_OptionSymbol );
	if ( opt == NULL )
		return;

	CMapPtrToPtr*	posLst = opt->getPositionList();
	short	nItems = posLst->GetCount();
	if ( nItems > 0 )
	{	wchar_t	buf[400];
		swprintf( buf, _T("Deleting '%s' will irrevocably delete its %d positions. Okay to proceed?"),
					m_OptionSymbol, nItems );
		UINT result = MessageBox( buf, _T("Delete option/positions?"),
							  MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST );
		if ( result == IDYES )
		{		// okay to delete the definition and all of its positions
			short	res = theApp.optMgr->DeleteBOS( (CBOS*)opt );
			if ( res != 0 )
			{
#ifdef _DEBUG
				TRACE( _T("OptionDefinitionDialog::OnDeleteDefinition: DeleteBOS failed, res=%d"), res );
#endif
			}
				// now remove m_OptionSymbol from c_OptionSymbol and reset it to _T("")
			int	idx = c_OptionSymbol.FindStringExact( -1, m_OptionSymbol );
			if ( idx != CB_ERR )
				c_OptionSymbol.DeleteString( idx );
			c_OptionSymbol.SetCurSel( -1 );				// clears the Edit portion of the ComboBox
		}
	}
	delete	opt;
	return;
}			// OnDeleteDefinition()
//----------------------------------------------------------------------------
void COptionDefinitionDialog::OnClickBS()
{
	bool	bsCheck = c_BScheckButton.GetCheck() == BST_CHECKED;
	if ( bsCheck == m_BSchecked )
		return;
	m_BSchecked = bsCheck;
	EvalOption();
}			// OnClickBS()
//----------------------------------------------------------------------------
/*
void	COptionDefinitionDialog::resetControls( void )
{		// leave Desc, ExpiryDate, StockSymbol & putOrCall alone, but ...
		// clear out two (sentinel) controls
	m_OptionPrice = 0.00;
	m_StrikePrice = 0.00;

	c_OptionPrice.SetWindowText( _T("") );
	c_StrikePrice.SetWindowText( _T("") );
}			// resetControls()
//----------------------------------------------------------------------------------------
void COptionDefinitionDialog::OnSelEndOkStockSymbol( void )
{
	killFocusStockSymbolWork();
}			// OnSelEndOkStockSymbol()
//----------------------------------------------------------------------------------------
void COptionDefinitionDialog::enableDialogItems( bool enDis )
{
		// an option symbol has been selected (or not) ...

		// enable (disable) the StockSymbol ComboBox
	c_StockSymbol.EnableWindow( enDis );

		// enable (disable) the Put/Call radio buttons
	c_PutRadio.EnableWindow( enDis );
	c_CallRadio.EnableWindow( enDis );

		// make StrikePrice CEdit box readWrite
	c_StrikePrice.EnableWindow( enDis );

		// enable (disable) the ExpiryDate
	c_ExpiryDate.EnableWindow( enDis );

		// make the Desc CEdit box readWrite (or not)
	c_Desc.EnableWindow( enDis );
}			// enableDialogItems()
*/
//----------------------------------------------------------------------------------------
/*
void COptionDefinitionDialog::OnDblClickStockSymbol() 
{		// this is based on CMainFrame::OnStockDefinition() with some wrapper code...
		// the wrapper stuff consists of:
		// (1) Preset m_StockSymbol before posting the stock definition dialog;
		// (2) Insert the symbol into our stocks ComboBox
		// (3) Grab the stock symbol after the dialog closes; and
		// (4) Make the selected symbol the current entry in the stocks ComboBox

		// Step (1):  preset m_StockSymbol in the StockDefinitionDialog
		//			  before posting it...
	CStockDefinitionDialog	dlg;
		// The following might be _T("") or an actual stock symbol
	dlg.m_StockSymbol = m_StockSymbol;

		// Now do everything that CMainFrame::OnStockDefinition() would do, plus Step (2)

		// (re)define a Stock (Volatility, Symbol, Dividends, ...)
	int nDisposition = dlg.DoModal();
	if ( nDisposition == IDOK  &&  m_StockSymbol != _T("") )
	{	CComboBox*	pSCB = (CComboBox*)GetDlgItem( IDC_StockSymbol );
		CFrameWnd*	pFW = GetTopLevelFrame();
		CNillaHedgeDoc* pDoc = (CNillaHedgeDoc*)pFW->GetActiveDocument();
		CStock*	aStock = (CStock*)dlg.getMapObject( pDoc->stocks, dlg.m_StockSymbol );
		if ( aStock == NULL )
		{		// didn't recognize this symbol, so this is a new entry
			aStock = new CStock( dlg.m_StockSymbol );
				// create an entry in the stocks map in the active document
			pDoc->stocks[ dlg.m_StockSymbol ] = aStock;

				// Step (2):  Add the new symbol to our stocks ComboBox
			pSCB->AddString( dlg.m_StockSymbol );
		}
				// attributes coming from StockDefinitionDialog
			// (some of which may be provided by the Net in the future)
		ASSERT_VALID( aStock );
		aStock->desc				= dlg.m_Desc;
		aStock->dividends[0]		= dlg.m_Q1Dividend;
		aStock->dividends[1]		= dlg.m_Q2Dividend;
		aStock->dividends[2]		= dlg.m_Q3Dividend;
		aStock->dividends[3]		= dlg.m_Q4Dividend;
		aStock->dividendsArePaid[0] = dlg.m_Q1exDividendDateCheck;
		aStock->dividendsArePaid[1] = dlg.m_Q2exDividendDateCheck;
		aStock->dividendsArePaid[2] = dlg.m_Q3exDividendDateCheck;
		aStock->dividendsArePaid[3] = dlg.m_Q4exDividendDateCheck;
		aStock->exDividendDates[0]  = dlg.m_Q1exDividendDate;
		aStock->exDividendDates[1]  = dlg.m_Q2exDividendDate;
		aStock->exDividendDates[2]  = dlg.m_Q3exDividendDate;
		aStock->exDividendDates[3]  = dlg.m_Q4exDividendDate;
		aStock->volatility			= dlg.m_Volatility;

			// indicate that the document has been modified
		pDoc->SetModifiedFlag();
		pDoc->UpdateAllViews( NULL );

			// Step (3):  Grab the stock symbol from the selection
		m_StockSymbol = dlg.m_StockSymbol;

			// Step (4):  Make this symbol the current selection in our stocks ComboBox
		int indx = pSCB->FindStringExact( 0, m_StockSymbol );
		if ( indx != CB_ERR )
			pSCB->SetCurSel( indx );
	}
}			// OnDblClickStockSymbol()
*/
//----------------------------------------------------------------------------------------
/*
void	COptionDefinitionDialog::OnSelEndOkOptionSymbol( void )
{
	killFocusOptionSymbolWork();
}			// OnSelEndOkOptionSymbol()
*/
//----------------------------------------------------------------------------
/*
		// if theOption exists, we're in the option analyzer dialog or
		// in KillFocusOptionSymbol() in the option definition dialog
	if ( theOption == NULL )
	{		// NULL argument...
			// called from somewhere other than KillFocusOptionSymbol()
			// in the option definition dialog or from the option analyzer dialog
		theOption = theApp.pDB->getOption( m_OptionSymbol );
		if ( theOption == NULL )
		{		// okay, so we don't have a saved option, how's the stock?
				// we know we're in the option definition dialog, so we need to
				// get theStock from m_StockSymbol
			theStock = theApp.pDB->getStock( m_StockSymbol );
			if ( theStock != NULL )
			{	if ( theStock->mktPrice > 0.0  &&  theStock->volatility > 0.0 )
				{		// stock exists and seems to be valid...
						// make a copy of the option and insert the dialog values
					theOption = new COption( "temp" );
					theOption->mktPrice = m_OptionPrice;
					theOption->strikePrice = m_StrikePrice;
					theOption->putCall = putOrCall;
					theOption->expiry = m_ExpiryDate;
					theOption->stockSymbol_ID = theStock->symbol_ID;
						// now we can evaluate the temp option 
					evalOption( theOption );					// update CStatics
					delete	theOption;		// we're the caller, so we have to delete theOption
					// return;
				}
				delete	theStock;
				// else, no underlying to evaluate...
			}
		}
		// else, no option in the map, so no underlying to get ...
		return;
	}
	// we were called from: KillFocusOptionSymbol() in the option definition dialog,
	//						updateControls() in the option analyzer dialog,
	//						or recursively in the code above from either of the option dialogs
	if ( putOrCall == COption::UnknownOptionType )
		return;

	theStock = theOption->getUnderlying();
	if ( theStock == NULL )
		return;

	bool  valid = ( theStock->mktPrice > 0.0  &&  theStock->volatility > 0.0 );
	if ( !valid ) return;								// no underlying to evaluate...
//--------------------------------------------------------------------------------------------------
void	COptionDefinitionDialog::SaveIfChanged( void )
{		// compare database COption to dialog values
	CString	stkSym;
	COption* theOption = theApp.pDB->getOption( m_OptionSymbol );
	if ( theOption == NULL )
		return;		// no previous option loaded, so no changes to lose

	if ( theOption->stockSymbol_ID >= 0 )
		stkSym = theApp.pDB->readSymbol( Stock, theOption->stockSymbol_ID );

		// have any values changed?
	bool	optPrcChanged	 = ( theOption->mktPrice != m_OptionPrice );
	bool	strikePrcChanged = ( theOption->strikePrice != m_StrikePrice );
	bool	putCallChanged	 = ( theOption->putCall != putOrCall );
	BOOL	expiryChanged	 = ( theOption->expiry != m_ExpiryDate );
	bool	stkSymChanged	 = ( stkSym != m_StockSymbol );
	CString	desc = theOption->getDesc();
	bool	descChanged		 = ( desc != m_Desc );

		// did anything change?
	bool	foundChanges = ( optPrcChanged   ||  strikePrcChanged  ||  expiryChanged
						 ||  putCallChanged  ||  stkSymChanged	   ||  descChanged	 );
	if ( ! foundChanges )
		return;

		// post a dialog, asking if user wants to save the changes
	CString	opc = _T("");				// option price changed
	if ( optPrcChanged )
	{	wchar_t buf[32];
		swprintf( buf, _T("Market: %.2f\n"), theOption->mktPrice );
		opc = buf;
	}

	CString	spc = _T("");				// strike price changed
	if ( strikePrcChanged )
	{	wchar_t buf[32];
		swprintf( buf, _T("Strike: %.2f\n"), theOption->strikePrice );
		spc = buf;
	}

	CString	pcc = _T("");				// put-call changed
	short tmp = theOption->getPutCall();
	if ( putCallChanged  &&  tmp != COption::UnknownOptionType )
	{		// we don't bother to notify user if change is from UnknownOptionType
		pcc = _T("Put/Call: ");
		pcc += ( tmp == COption::Call ) ? _T("Call\n") : _T("Put\n");
	}

	CString	exc = _T("");				// expiry date changed
	if ( expiryChanged )
	{	wchar_t buf[32];
		swprintf( buf, _T("Expiry: %2d-%3s-%4d\n"), theOption->expiry.GetDay(),
				  months[theOption->expiry.GetMonth()-1], theOption->expiry.GetYear() );
		exc = buf;
	}

	CString	usc = _T("");				// underlying stock changed
	if ( stkSymChanged )
	{	usc = _T("Stock: ");
		usc += stkSym;
		usc += _T("\n");
	}

	CString dec = _T("");
	if ( descChanged )
	{	dec = _T("Desc: ");
		dec += desc;
		dec += _T("\n");
	}

		// now bring it all together for the MessageBox
	CString	sct = opc;				// option price
	sct += spc;						// strike price
	sct += pcc;						// Put/Call
	sct += exc;						// expiry date
	sct += usc;						// underlying stock
	sct += dec;						// description

		// get user confirmation to save or discard the changes we found
	UINT  result = MessageBox( sct, _T("Save Changes?"), MB_ICONQUESTION | MB_YESNO );
	if ( result == IDYES )
	{	if ( stkSymChanged )
		{	long	stkSym_ID = theApp.pDB->getSymbol_ID( Stock, m_StockSymbol );
				// no stock def exists, we need to create one
			if ( stkSym_ID < 0 )
			{		// must create a new stock with this name
				CStock	stk( m_StockSymbol );
				theApp.pDB->writeStock( &stk );
#ifdef _DEBUG
				if ( res != 0 )
					TRACE( _T("OptionDefinitionDialog::SaveIfChanged: Stock write failed, res=%d\n"), res );
#endif
				stkSym_ID = stk.symbol_ID;
			}
			theOption->stockSymbol_ID = stkSym_ID;
		}
		theOption->mktPrice = m_OptionPrice;
		theOption->strikePrice = m_StrikePrice;
		theOption->setPutCall( putOrCall );
		theOption->expiry = m_ExpiryDate;
		theOption->setDesc( m_Desc );
		theOption->saveDesc();			// commit the description to pDB
			// at this point, we'd have to write the option (if desc_ID changed)
			// even if mktPrice, et. al. hadn't changed
		short	res = theApp.pDB->writeOption(theOption);
#ifdef _DEBUG
		if ( res != 0 )
			TRACE( _T("OptionDefinitionDialog::SaveIfChanged: Option write failed, res=%d\n"), res );
#endif
	}
	delete	theOption;
}			// SaveIfChanged()
//----------------------------------------------------------------------------
*/
