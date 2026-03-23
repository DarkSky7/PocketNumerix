// OptionAnalyzerDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "OptionType.h"
#include "OptionAnalyzerDialog.h"
#include "OptionDefinitionDialog.h"
#include "StockManager.h"
#include "OptionManager.h"
#include "RegistryManager.h"
#include "NillaHedge.h"			// calcYears
#include "Stock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC( COptionAnalyzerDialog, CNillaDialog );

extern	CString	months[];

/////////////////////////////////////////////////////////////////////////////
// COptionAnalyzerDialog dialog

COptionAnalyzerDialog::COptionAnalyzerDialog( CWnd* pParent /*=NULL*/ )
	: CNillaDialog( COptionAnalyzerDialog::IDD, pParent)
	, m_activeComboBox( NULL )
	, theOption( NULL )
	, m_BSchecked( false )
	, criticalRegion( false )
{
	//{{AFX_DATA_INIT(COptionAnalyzerDialog)
	m_OptionSymbol	= _T("");
	m_OptionPrice	= 0.0;
	m_StockPrice	= 0.0;			// no COptionDefinition control
	m_Volatility	= 0.0;			// no COptionDefinition control
	m_RiskFreeRate	= 3.0f;
	//}}AFX_DATA_INIT
	evcs.fromDate = COleDateTime::GetCurrentTime();
}

COptionAnalyzerDialog::~COptionAnalyzerDialog( void )
{
	if ( theOption ) delete theOption;
}

void	COptionAnalyzerDialog::DoDataExchange(CDataExchange* pDX)
{
	CNillaDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionAnalyzerDialog)
	DDX_Control(pDX, IDC_EvalDate, c_EvalDate);
	DDX_Control(pDX, IDC_StrikePriceResult, c_StrikePriceResult);
	DDX_Control(pDX, IDC_StockNameLabel, c_StockName);
	DDX_Control(pDX, IDC_BlackScholesResult, c_BlackScholesResult);
	DDX_Control(pDX, IDC_DeltaResult, c_DeltaResult);
	DDX_Control(pDX, IDC_ExpiryResult, c_ExpiryResult);
	DDX_Control(pDX, IDC_GammaResult, c_GammaResult);
	DDX_Control(pDX, IDC_ImpliedVolatilityResult, c_ImpliedVolatilityResult);
	DDX_Control(pDX, IDC_OptionPrice, c_OptionPrice);
	DDX_Control(pDX, IDC_OptionSymbol, c_OptionSymbol);
	DDX_Control(pDX, IDC_PutCallResult, c_PutCallResult);
	DDX_Control(pDX, IDC_RhoRresult, c_RhoRresult);
	DDX_Control(pDX, IDC_RhoDresult, c_RhoDresult);
	DDX_Control(pDX, IDC_RiskFreeRate, c_RiskFreeRate);
	DDX_Control(pDX, IDC_StockPrice, c_StockPrice);
	DDX_Control(pDX, IDC_ThetaResult, c_ThetaResult);
	DDX_Control(pDX, IDC_VegaResult, c_VegaResult);
	DDX_Control(pDX, IDC_Volatility, c_Volatility);
	DDX_CBString(pDX, IDC_OptionSymbol, m_OptionSymbol);
	DDX_Text(pDX, IDC_OptionPrice, m_OptionPrice);
	DDV_MinMaxDouble(pDX, m_OptionPrice, 0., 1000000.);
	DDX_Text(pDX, IDC_StockPrice, m_StockPrice);
	DDV_MinMaxDouble(pDX, m_StockPrice, 0., 1000000.);
	DDX_Text(pDX, IDC_Volatility, m_Volatility);
	DDV_MinMaxFloat(pDX, m_Volatility, 0., 10000.);
	DDX_Text(pDX, IDC_RiskFreeRate, m_RiskFreeRate);
	DDV_MinMaxFloat(pDX, m_RiskFreeRate, 0., 100.);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_BS, c_BScheckButton);
}

BEGIN_MESSAGE_MAP(COptionAnalyzerDialog, CNillaDialog)
	//{{AFX_MSG_MAP(COptionAnalyzerDialog)
	ON_EN_KILLFOCUS(IDC_OptionPrice, OnKillFocusOptionPrice)
	ON_EN_KILLFOCUS(IDC_RiskFreeRate, OnKillFocusRiskFreeRate)
	ON_EN_KILLFOCUS(IDC_StockPrice, OnKillFocusStockPrice)
	ON_EN_KILLFOCUS(IDC_Volatility, OnKillFocusVolatility)
	ON_EN_SETFOCUS(IDC_StockPrice, OnSetFocusStockPrice)
	ON_EN_SETFOCUS(IDC_RiskFreeRate, OnSetFocusRiskFreeRate)
	ON_EN_SETFOCUS(IDC_OptionPrice, OnSetFocusOptionPrice)
	ON_EN_SETFOCUS(IDC_Volatility, OnSetFocusVolatility)
	ON_NOTIFY(DTN_CLOSEUP, IDC_EvalDate, OnCloseUpEvalDate)
	ON_NOTIFY(NM_KILLFOCUS, IDC_EvalDate, OnKillFocusEvalDate)
	ON_WM_CTLCOLOR()

	ON_CBN_SELCHANGE(IDC_OptionSymbol, OnSelChangeOptionSymbol)
	ON_CBN_KILLFOCUS(IDC_OptionSymbol, OnKillFocusOptionSymbol)			// HotKeys +
	ON_CBN_SETFOCUS(IDC_OptionSymbol, OnSetFocusOptionSymbol)			// HotKeys
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()													// HotKeys
	ON_MESSAGE(WM_HOTKEY, OnHotKey)										// HotKeys

	ON_NOTIFY(NM_SETFOCUS, IDC_EvalDate, OnSetFocusEvalDate)
	ON_BN_CLICKED(IDC_BS, OnClickBS)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionAnalyzerDialog message handlers

BOOL	COptionAnalyzerDialog::OnInitDialog() 
{
	CNillaDialog::OnInitDialog();
	RegisterHotKeys( IDC_OptionSymbol, IDC_EvalDate );

		// load up the risk free rate
	if ( ! GetRiskFreeRate(&m_RiskFreeRate) )				// was:  theApp.regMgr->
	{
#ifdef _DEBUG
		TRACE( _T("OptionAnalyzerDialog::OnInitDialog: no registered value for RiskFreeRate.\n") );
#endif
	}
	setEditBox( "%.3f", m_RiskFreeRate, IDC_RiskFreeRate );
	evcs.riskFreeRate = m_RiskFreeRate / 100.0;
	c_EvalDate.SetTime( evcs.fromDate );
	c_EvalDate.EnableWindow( false );

		// build the ComboBox of option symbols
	CMapStringToPtr*	optionsMap = theApp.optMgr->GetSymbolTable();
	c_OptionSymbol.LoadMap( optionsMap );

	m_BSchecked = true;
	c_BScheckButton.SetCheck( m_BSchecked ? BST_CHECKED : BST_UNCHECKED );

		// allow for preloading a selection into the OptionSymbol ListBox
	bool	thereAreOptions = optionsMap != NULL  &&  optionsMap->GetCount() > 0;
	if ( thereAreOptions )
	{		// what if an option symbol was not preloaded?
		if ( m_OptionSymbol == _T("") )								// modified since the constructor?
		{		// see if there's a registered Recent Option
			long	def_ID;
			bool	res = GetRecentOption(&def_ID);					// was:  theApp.regMgr->
			if ( res  &&  def_ID >= 0 )								// success
			{	theOption = theApp.optMgr->ReadOption( def_ID );
				if ( theOption )
					m_OptionSymbol = theOption->getSymbol();
			}
		}
		if ( m_OptionSymbol != _T("") )		// modified since the constructor
		{	int	indx = c_OptionSymbol.FindStringExact( 0, m_OptionSymbol );
			if ( indx != LB_ERR )
				c_OptionSymbol.SetCurSel( indx );
		}
		else
			c_OptionSymbol.SetCurSel( 0 );							// just pick the first Option

			// convince OnKillFocusOptionSymbol() that the OptionSymbol has changed
		m_OptionSymbol = _T("");
		OnKillFocusOptionSymbol();
	}
	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//---------------------------------------------------------------------------
HBRUSH COptionAnalyzerDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	return	CNillaDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}			// OnCtlColor()
//---------------------------------------------------------------------------
void	COptionAnalyzerDialog::ClearStatics( void )
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

	CString	dateString = EuroFormat( COleDateTime::GetCurrentTime() );
	c_ExpiryResult.SetWindowText( dateString );
	c_PutCallResult.SetWindowText( _T("Put | Call @") );
	c_StrikePriceResult.SetWindowText( _T("Strike") );

	c_StockName.SetWindowText( _T("-- Underlying Stock --") );
}			// ClearStatics()
//----------------------------------------------------------------------------------------
void	COptionAnalyzerDialog::ResetControls( void )
{		// so far, resetControls() is only called by updateControls()
		// clear Option value controls
	c_OptionPrice.SetWindowText( _T("0.00") );
	c_StockPrice.SetWindowText( _T("0.00") );
	c_Volatility.SetWindowText( _T("0.0000") );
	
	c_OptionPrice.EnableWindow( false );
	c_StockPrice.EnableWindow( false );
	c_Volatility.EnableWindow( false );
	c_EvalDate.EnableWindow( false );
}			// ResetControls()
//---------------------------------------------------------------------------
void	COptionAnalyzerDialog::KillFocusOptionSymbolWork( CString sym )
{		// lots of work to do here, but we'll focus on the stateful management of theOption,
		// essentially ask user if they want to save changes, then update controls
		// switch to the new option, load needed values and update the controls
		// we'll communicate theOption dependent values and setup to evaluate theOption,
		// but we'll leave the details of updating the statics to evalOption()
	if ( sym == m_OptionSymbol )
		return;
		// save (approved) changes to theOption before changing the OptionSymbol
	SaveIfChanged( true );		// has to appear before updating m_OptionSymbol
	m_OptionSymbol = sym;

		// save a little time if OnInitDialog() fetched theOption for us
	if ( theOption == NULL  &&  m_OptionSymbol != _T("") )
		theOption = theApp.optMgr->GetOption( m_OptionSymbol );
	else if ( theOption  &&  theOption->getSymbol() != m_OptionSymbol )
	{		// we have to switch options
		delete	theOption;
		theOption = theApp.optMgr->GetOption( m_OptionSymbol );
	}

		// still NULL after looking up m_OptionSymbol?
	if ( theOption == NULL )
	{		// no such option in the database
		ClearStatics();
		ResetControls();
		return;
	}

#ifdef _DEBUG
	TRACE( _T("OptionAnalyzerDialog::KillFocusOptionSymbolWork: opt_ID=%d\n"), theOption->getDef_ID() );
#endif
		// load theOption's dialog variables
	m_OptionPrice = theOption->getMktPrice();
	c_EvalDate.EnableWindow( true );

//	yrsToExpiry = theOption->YrsToExpiry( today );


		// re-enable the Option/Stock CEdit Boxes
	c_OptionPrice.EnableWindow( true );
	c_StockPrice.EnableWindow( true );
	c_Volatility.EnableWindow( true );

		// update the GUI for theOption
	setEditBox( "%.2f", m_OptionPrice, IDC_OptionPrice );
	OptionType	putOrCall = theOption->putCall;
	if ( putOrCall != UnknownOptionType )
	{
		CString t2( (putOrCall == Put) ? "Put @" : "Call @" );
		setStatic( t2, IDC_PutCallResult );
	}
		// display the Strike Price (no associated m_ variable)
	setStatic( "%.2f", theOption->strikePrice, IDC_StrikePriceResult );

		// display ExpiryDate for the option
	CString	t1 = EuroFormat( theOption->getExpiry() );
	setStatic( t1, IDC_ExpiryResult );

		// load the underlying's market price and volatility
	CStock* theStock = theOption->getUnderlying();
	ASSERT_VALID( theStock );
	if ( theStock == NULL )
	{
		m_StockPrice = 0.0;
		m_Volatility = 0.0;
		c_StockName.SetWindowText( _T("-- Underlying Stock --") );
		c_StockPrice.SetWindowText( _T("0.00") );
		c_Volatility.SetWindowText( _T("0.000") );
		return;
	}

		// load theStock's dialog variables
	evcs.calcPvDivs = true;						// theStock has changed
	m_StockPrice = theStock->getMktPrice();
	evcs.stockPrice = m_StockPrice;
	m_Volatility = theStock->volatility;
	evcs.sigma = m_Volatility;

		// update the GUI for theStock
		// m_StockSymbol is used by evalOption() when theOption isn't passed in
	CString	stkSym = theStock->getSymbol();
		// show Stock Symbol in IDC_StockPriceLabel and IDC_VolatilityLabel
	CString cs = _T("-- ") + stkSym;
	cs += _T(" --");
	c_StockName.SetWindowText( cs );
	setEditBox( "%.2f", m_StockPrice, IDC_StockPrice );
	setEditBox( "%.4f", m_Volatility, IDC_Volatility );

		// compute analytics and display results
	evcs.calcYrsToExpiry = true;			// c_OptionSymbol has changed
	EvalOption();							// update CStatic(s)
}			// KillFocusOptionSymbolWork()
//---------------------------------------------------------------------------
void COptionAnalyzerDialog::OnKillFocusOptionSymbol( void )
{	CString sym;
	m_activeComboBox = NULL;
//	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	if ( criticalRegion )
		return;
	c_OptionSymbol.GetWindowText( sym );
	sym.MakeUpper();
	KillFocusOptionSymbolWork( sym );
}			// OnKillFocusOptionSymbol()
//---------------------------------------------------------------------------
void	COptionAnalyzerDialog::OnSetFocusOptionSymbol( void )
{
	m_activeComboBox = IDC_OptionSymbol;
//	SHSipPreference( GetSafeHwnd(), SIP_UP );	// quicker to pick from a list
}			// OnSetFocusOptionSymbol()
//----------------------------------------------------------------------------
void	COptionAnalyzerDialog::OnSelChangeOptionSymbol( void )
{	CString sym;
	int cnt = c_OptionSymbol.GetCount();
	if ( cnt < 1 )
		return;
	int	sel = c_OptionSymbol.GetCurSel();	// this is what we use when LB Closeup happened first
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
	c_OptionSymbol.GetLBText( sel, sym );
	KillFocusOptionSymbolWork( sym );				// used to be all that was here
}			// OnSelChangeOptionSymbol()
//----------------------------------------------------------------------------------------
void	COptionAnalyzerDialog::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	CNillaDialog::OnActivate( nState, pWndOther, bMinimized );
	if ( nState == WA_ACTIVE  ||  nState == WA_CLICKACTIVE )
		RegisterHotKeys( IDC_OptionSymbol, IDC_EvalDate );	// screens for WM5 internally
}			// OnActivate()
//----------------------------------------------------------------------------------------
LRESULT		COptionAnalyzerDialog::OnHotKey( WPARAM wParam, LPARAM lParam )
{		//	UINT fuModifiers = (UINT)LOWORD(lParam);		// don't care about modifiers
		//	DWORD ctrlID = (int)wParam;						// don't care which control the HotKey is registered to (any more)
		//	CWnd* wnd = GetDlgItem( ctrlID );				// also about the registered control
	if ( ! m_activeComboBox )
		return	0;											// nothing to do

	UINT uVirtKey = (UINT)HIWORD(lParam);
	if ( uVirtKey != VK_DOWN  &&  uVirtKey != VK_UP )
		return	0;												// only care about the up/down arrow keys

	CSeekComboBox* scb = (CSeekComboBox*)GetDlgItem( m_activeComboBox );
	scb->lastChar = uVirtKey;									// this in combination with SendMessage()
	WPARAM wp = MAKEWPARAM( m_activeComboBox, CBN_SELCHANGE );
	SendMessage( WM_COMMAND, wp, (LPARAM)scb->GetSafeHwnd() );	// emulates VK_UP/VK_DOWN as on PPC'02
	return	0;
}			// OnHotKey()
//----------------------------------------------------------------------------
void	COptionAnalyzerDialog::SaveIfChanged( bool updateGUI /* = false */ )
{	CStock*	aStock;
	bool	saveVerify;
	short	prefs;
	COption* anOption = NULL;

		// works off of m_OptionSymbol
	if ( m_OptionSymbol == _T("") )
		goto	Exit;

		// to be thread safe, we fetch a fresh copy for MainFrame calls
		// true -> theOption is valid, false -> it might not be
	anOption = updateGUI  ?  theOption  :  theApp.optMgr->GetOption( m_OptionSymbol );
	if ( anOption == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("OptionAnalyzerDialog::SaveIfChanged: no Option found ... returning.") );
#endif
		goto	Exit;
	}

		// if the caller is MainFrame, dialog thread memory may have been reclaimed
		// so, we could produce an access violation accessing members of theOption
	saveVerify = true;
	prefs = AnalyzerChangeSymbol | AnalyzerClose;
	if ( GetSaveVerifyPrefs(&prefs) )							// was:  theApp.regMgr->
	{		// updateGUI tells us whether this is a close or change symbol action
			// updateGUI should be true from killFocusOptionSymbolWork, representing a change symbol action
			//			 and false from MainFrame, representing a close dialog action
		short	mask = updateGUI  ?  AnalyzerChangeSymbol  :  AnalyzerClose;
		saveVerify = (prefs & mask) != 0;
	}
		// update the current option's market price (before switching options)
		// obtain permission to save changes to anOption's market price
	UINT  result;
		// handle anOption's update query
	if ( fabs(m_OptionPrice - anOption->getMktPrice()) > 1e-4 )
	{		// get permission to update the Option definition...
		result = IDCANCEL;
		if ( saveVerify )
		{	wchar_t	buf[320];
			swprintf( buf, _T("Update the market price for '%s'?"),
						m_OptionSymbol );
			criticalRegion = true;
			result = MessageBox( buf, _T("Save Changes?"),
						MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST );
			criticalRegion = false;
		}
		if ( !saveVerify  ||  result == IDYES )
		{	
			anOption->setMktPrice( m_OptionPrice );
			short	res = theApp.optMgr->WriteOption( anOption );
#ifdef _DEBUG
			if ( res != 0 )
				TRACE( _T("OptionAnalyzerDialog::SaveIfChanged: Option write failed, res=%d\n"), res );
#endif
		}
	}
		// handle the underlying's update query
	aStock = anOption->getUnderlying();
	ASSERT_VALID( aStock );
	if ( aStock )
	{		// obtain permission to save changes to anOption's underlying stock
		bool	stkPrcChanged = fabs(m_StockPrice - aStock->getMktPrice()) > 1e-4;
		bool	stkVolChanged = fabs(m_Volatility - aStock->volatility) > 1e-5;
		if ( stkPrcChanged  ||  stkVolChanged )
		{		// get permission to update the Option definition...
			result = IDCANCEL;
			if ( saveVerify )
			{	CString	sct = _T("Save changes to '");
				sct += aStock->getSymbol();
				sct += _T("'s");
				CString spc = stkPrcChanged ? _T(" stock price,") : _T("");
				CString svc = stkVolChanged ? _T(" volatility,") : _T("");
				CString cst = sct + spc + svc;
				int len = cst.GetLength();
				cst.SetAt(len-1, '?');
				criticalRegion = true;
				result = MessageBox( cst, _T("Save Changes?"),
							MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST );
				criticalRegion = false;
			}
			if ( !saveVerify  ||  result == IDYES )
			{
				aStock->setMktPrice( m_StockPrice );
				aStock->volatility = m_Volatility;
				short	res = theApp.stkMgr->WriteStock( aStock );
#ifdef _DEBUG
				if ( res != 0 )
					TRACE( _T("OptionAnalyzerDialog::SaveIfChanged: Stock write failed, res=%d\n"), res );
#endif
			}
		}
		// deleting anOption cleans up aStock too
	}
		// calls from MainFrame (updateGUI == false), fetch a fresh copy from pDB
Exit:
	if ( ! updateGUI )
		delete	anOption;					// not simply a reference to a dialog var
}			// SaveIfChanged()
//---------------------------------------------------------------------------
void	COptionAnalyzerDialog::EvalOption()
{		// see if we can find the analytical inputs needed to evaluate theOption
		// pick up dialog globals theOption & divYield in killFocusOptionSymbolWork()
	ASSERT( theOption != NULL );
	COption	anOption( *theOption );
	anOption.setMktPrice( m_OptionPrice );
		// evcs.fromDate is set when c_EvalDate changes
		// evcs.riskFreeRate is set when c_RiskFreeRate changes
		// evcs.sigma is set when c_Volatility changes,
		//	but results of ImpliedVolatility write back to evcs, so it must be reloaded each time
		// evcs.stockPrice is set when c_StockPrice changes
		// evcs.calcYrsToExpiry is set when either c_OptionSymbol or c_EvalDate changes
		// evcs.calcPvDivs is set when a c_OptionSymbol changes and an underlying is found

		// implied volatility
	double	priceError = 0.001;
	bool	valid = anOption.ImpliedVolatility( evcs, priceError );
	setStatic( valid, "%.4f", evcs.sigma, IDC_ImpliedVolatilityResult );
	setStatic( valid, "%.3f", priceError, IDC_IvPricingErrorResult );

	double	euroVal;
	evcs.sigma = m_Volatility;
	bool euroValValid = anOption.EuroValue( evcs, euroVal );
	setStatic( euroValValid, "%.3f", euroVal, IDC_BlackScholesResult );

		// the Greeks ...
	double	delta, elasticity;
	valid = anOption.Delta( evcs, delta );			// need delta to compute elasticity
	setStatic( valid, "%.4f", delta, IDC_DeltaResult );

		// elasticity is based on the stock price, not a pvDivs adjusted value
	if ( valid )
	{	double	denom = m_BSchecked  ?  euroVal  :  m_OptionPrice;
		valid = euroValValid  &&  ( denom != 0.0 );
		if ( valid )
		{
			elasticity = delta * m_StockPrice / denom;
#ifdef _DEBUG
			TRACE( _T("OptionAnalyzerDialog::EvalOption: elasticity=%g <-- delta=%g * m_StockPrice=%g / %s=%g\n"),
				elasticity, delta, m_StockPrice, (denom==euroVal ? _T("euroVal") : _T("optPrice")), denom );
#endif
		}
	}
	setStatic( valid, "%.2f", elasticity, IDC_ElasticityResult );
	double	res;
	valid = anOption.Gamma( evcs, res );
	setStatic( valid, "%.4f", res, IDC_GammaResult );
	valid = anOption.RhoR( evcs, res );
	setStatic( valid, "%.4f", res, IDC_RhoRresult );
	valid = anOption.RhoD( evcs, res );
	setStatic( valid, "%.4f", res, IDC_RhoDresult );
	valid = anOption.Theta( evcs, res );
	setStatic( valid, "%.4f", res, IDC_ThetaResult );
	valid = anOption.Vega( evcs, res );
	setStatic( valid, "%.4f", res, IDC_VegaResult );
	valid = anOption.pITM( evcs, res );
	setStatic( valid, "%.1f%%", 100.0 * res, IDC_pITMResult );
}			// EvalOption()
//---------------------------------------------------------------------------
void COptionAnalyzerDialog::OnKillFocusOptionPrice( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_OptionPrice );
	if ( ff == m_OptionPrice )
		return;
	m_OptionPrice = ff;
//	setEditBox( "%.2f", m_OptionPrice, IDC_OptionPrice );		// echo the option price back
	EvalOption();
}			// OnKillFocusOptionPrice()
//---------------------------------------------------------------------------
void COptionAnalyzerDialog::OnSetFocusOptionPrice()
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	c_OptionPrice.SetSel( 0, -1 );
}			// OnSetFocusOptionPrice()
//---------------------------------------------------------------------------
void COptionAnalyzerDialog::OnKillFocusRiskFreeRate( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_RiskFreeRate );
	if ( ff == m_RiskFreeRate )
		return;
	m_RiskFreeRate = ff;
	evcs.riskFreeRate = m_RiskFreeRate / 100.0;
//	setEditBox( "%.3f", m_RiskFreeRate, IDC_RiskFreeRate );	// echo risk free rate back
	EvalOption();			// update CStatic(s)
}			// OnKillFocusRiskFreeRate()
//---------------------------------------------------------------------------
void COptionAnalyzerDialog::OnSetFocusRiskFreeRate()
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	c_RiskFreeRate.SetSel( 0, -1 );
}			// OnSetFocusRiskFreeRate()
//---------------------------------------------------------------------------
void COptionAnalyzerDialog::OnKillFocusStockPrice( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_StockPrice );
	if ( ff == m_StockPrice )
		return;
	m_StockPrice = ff;
	evcs.stockPrice = m_StockPrice;

//	setEditBox( "%.2f", m_StockPrice, IDC_StockPrice );
	EvalOption();			// update CStatic(s)
}			// OnKillFocusStockPrice()
//---------------------------------------------------------------------------
void COptionAnalyzerDialog::OnSetFocusStockPrice()
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	c_StockPrice.SetSel( 0, -1 );
}			// OnSetFocusStockPrice()
//---------------------------------------------------------------------------
void COptionAnalyzerDialog::OnKillFocusVolatility() 
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float ff = getEditBoxFloat( IDC_Volatility );
	if ( ff == m_Volatility )
		return;
	m_Volatility = ff;
	evcs.sigma = m_Volatility;				
//	setEditBox( "%.3f", ff, IDC_Volatility );
	EvalOption();			// update CStatic(s)
}			// OnKillFocusVolatility()
//---------------------------------------------------------------------------
void COptionAnalyzerDialog::OnSetFocusVolatility()
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	c_Volatility.SetSel( 0, -1 );
}			// OnSetFocusVolatility()
//---------------------------------------------------------------------------
void	COptionAnalyzerDialog::KillFocusEvalDateWork( NMHDR* pNMHDR, LRESULT* pResult )
{
	COleDateTime	aDate;
	c_EvalDate.GetTime( aDate );
	if ( evcs.fromDate == aDate )
		return;
	evcs.fromDate = aDate;
	evcs.calcYrsToExpiry = true;
	evcs.calcPvDivs = true;
	EvalOption();
	*pResult = 0;
}			// KillFocusEvalDateWork()
//----------------------------------------------------------------------------
void	COptionAnalyzerDialog::OnCloseUpEvalDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	KillFocusEvalDateWork( pNMHDR, pResult );
}			// OnCloseUpEvalDate()
//---------------------------------------------------------------------------
void COptionAnalyzerDialog::OnKillFocusEvalDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	KillFocusEvalDateWork( pNMHDR, pResult );
}			// OnKillFocusEvalDate()
//---------------------------------------------------------------------------
void COptionAnalyzerDialog::OnSetFocusEvalDate(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	*pResult = 0;
}			// OnSetFocusEvalDate()
//---------------------------------------------------------------------------
void COptionAnalyzerDialog::OnClickBS()
{
	bool	bsCheck = c_BScheckButton.GetCheck() == BST_CHECKED;
	if ( bsCheck == m_BSchecked )
		return;
	m_BSchecked = bsCheck;
	EvalOption();
}			// OnClickBS()
//---------------------------------------------------------------------------
/*
void	COptionAnalyzerDialog::OnEditUpdateOptionSymbol()
{		// effectively replaced by CSeekComboBox::OnEditChange()
		// weird, but CBN_EDITCHANGE events weren't being received
		// CBN_EDITUPDATE seems to be the ticket
	seekComboBoxEntry( IDC_OptionSymbol );
}			// OnEditUpdateOptionSymbol()
//---------------------------------------------------------------------------
void	COptionAnalyzerDialog::OnSelEndOkOptionSymbol( void )
{
	OnKillFocusOptionSymbol();
}			// OnSelEndOkOptionSymbol()
//---------------------------------------------------------------------------
void COptionAnalyzerDialog::OnDblClickOptionSymbol( void )
{		// this function is identical to COptionValueDialog::OnDblClickOptionSymbol()
		// it is based on CMainFrame::OnOptionDefinition() with some wrapper code...
		// the wrapper stuff consists of:
		// ...
		// (1) Preset m_OptionSymbol before posting the option definition dialog;
		// (2) Insert the symbol into our stocks ComboBox
		// (3) Grab the option symbol after the dialog closes; and
		// (4) Make the selected symbol the current entry in the stocks ComboBox
		// (re)define an Option (StockSymbol, StrikePrice, PutCall, Expiry, ...)
	SYSTEMTIME    st;
	GetLocalTime(&st);
	COleDateTime today(st);

		// Step (1):  preset m_OptionSymbol in the OptionDefinitionDialog
		//			  before posting it...
	COptionDefinitionDialog dlg( this );
			// The following might be _T("") or an actual option symbol
	dlg.m_OptionSymbol = m_OptionSymbol;

	int nDisposition = dlg.DoModal();
	if (  nDisposition == IDOK  &&  m_OptionSymbol != _T("")
	  &&  m_StockSymbol != _T("")  &&  m_StrikePrice > 0.0f
	  &&  m_ExpiryDate > today
	  &&  (putOrCall == COption::Put  ||  putOrCall == COption::Call) )
	{	CComboBox*	pOCB = (CComboBox*)GetDlgItem( IDC_OptionSymbol );
		CFrameWnd* pFW = GetTopLevelFrame();
		CNillaHedgeDoc* pDoc = (CNillaHedgeDoc*)pFW->GetActiveDocument();

		COption* anOption = (COption*)dlg.getMapObject( pDoc->options, dlg.m_OptionSymbol );
		if ( anOption == NULL )
		{		// didn't recognize OptionSymbol, so this is a new entry
			anOption = new COption( dlg.m_OptionSymbol );
				// create an entry in the stocks map in the active document
			pDoc->options[ anOption->optionSymbol ] = anOption;

				// Step (2):  Add the new symbol to our stocks ComboBox
			pOCB->AddString( anOption->optionSymbol );
		}
		ASSERT_VALID( anOption );
			// attributes coming from OptionDefinitionDialog
			// (possibly to be provided by the Net in the future)
		anOption->desc			= dlg.m_Desc;
		anOption->expiry		= dlg.m_ExpiryDate;
		anOption->putCall		= dlg.putOrCall;
		anOption->strikePrice	= dlg.m_StrikePrice;
		anOption->stockSymbol	= dlg.m_StockSymbol;

			// indicate that the document has been modified
		pDoc->SetModifiedFlag();
		pDoc->UpdateAllViews( NULL );

			// Step (3):  Grab the option symbol from the selection
		m_OptionSymbol = anOption->optionSymbol;

			// Step (4):  Make this symbol the current selection in our stocks ComboBox
		int indx = pOCB->FindStringExact( 0, m_OptionSymbol );
		if ( indx != CB_ERR )
			pOCB->SetCurSel( indx );
	}
}			// OnDblClickOptionSymbol()
*/
//---------------------------------------------------------------------------
/* from evalOption() ...
		// indicate whether the American Price is via:
		// 1) Roll-Geske-Whaley (Call); or
		// 2) Geske-Johnson (Put)
//		CString t3( (theOption->putCall == COption::Put) ? "Geske-Johnson" : "Roll-Geske-Whaley" );
//		setStatic( t3, IDC_AmerValueLabel );

	
		// American Value, either: Roll-Geske-Whaley (Call); or Geske-Johnson (Put)
	double	amerVal = amerValue( theStock, m_StockPrice,
							m_Volatility, m_RiskFreeRate, yrsToExpiry );
	setStatic( "%.3f", amerVal, IDC_AmerValueResult );

		// Exercise Boundary
	COleDateTime	exDivDate;
	double	lastDiv = theStock->lastDivB4Date( theOption->expiry, exDivDate );
		// sStar really isn't the exercise boundary anyway...
	double	err, exerBound = findSstar( theStock, m_StockPrice, lastDiv,
										m_RiskFreeRate, yrsToExpiry, err );
	if ( exerBound != 0.0 )
		setStatic( "%.3f", exerBound, IDC_ExerciseBoundaryResult );
	else
		setStatic( _T("~"), IDC_ExerciseBoundaryResult );
*/
//---------------------------------------------------------------------------
