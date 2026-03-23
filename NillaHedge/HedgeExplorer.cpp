// HedgeExplorer.cpp : implementation file
//
#include "StdAfx.h"
#include "NillaDialog.h"
#include "resource.h"
#include "HedgeExplorer.h"
#include "OptionManager.h"
#include "RegistryManager.h"
#include "StockManager.h"
#include "Polyline.h"
#include "LineSeg.h"
#include "NillaHedge.h"					// for calcYears()
#include "Stock.h"
#include "Option.h"
#include "BOS.h"
#include <stdlib.h>						// srand()
#include <winbase.h>					// SYSTEMTIME structure

//#include "BOSDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHedgeExplorer dialog

CHedgeExplorer::CHedgeExplorer( CWnd* pParent /*=NULL*/ )
	: CPlotDialog( CHedgeExplorer::IDD, pParent )
	, m_activeComboBox( NULL )
	, pStock( NULL )
	, criticalRegion( false )
{
	//{{AFX_DATA_INIT(CHedgeExplorer)
	m_StockSymbol = _T("");
	m_Option1Symbol = _T("");
	m_Option2Symbol = _T("");
	m_Option3Symbol = _T("");
	m_EvalDate = COleDateTime::GetCurrentTime();
	m_Volatility = 0.0f;
	m_StockPrice = 0.0f;
	m_RiskFreeRate = 3.0f;
	//}}AFX_DATA_INIT

	for ( short ii = 0; ii < nHedgeBOS; ii++ )
	{	os3oh.qty[ii] = 0.0;
		os3oh.buySell[ii] = UnknownBuySellStatus;
		os3oh.def_ID[ii] = -1;
		if ( ii < nHedgeOpts )
			pOption[ii] = NULL;
	}
	copyrightOffset[0] =  5;			// All rights reserved
	copyrightOffset[1] =  3;			// PocketNumerix
	copyrightAxisRelative = false;		// so offsets represent number of half grid lines from bottom of plot

	m_EvalDate = COleDateTime::GetCurrentTime();
}

CHedgeExplorer::~CHedgeExplorer( void )
{	
	for ( short ii = 0; ii < nHedgeOpts; ii++ )
	{	if ( pOption[ii] )
		{	delete	pOption[ii];
			pOption[ii] = NULL;
		}
	}
	if ( pStock )
	{	delete	pStock;
		pStock = NULL;
	}
	CPlotDialog::~CPlotDialog();
}

void CHedgeExplorer::DoDataExchange( CDataExchange* pDX )
{
	CPlotDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHedgeExplorer)
	DDX_Control(pDX, IDC_StockQty, c_StockQty);
	DDX_Control(pDX, IDC_Option3Qty, c_Option3Qty);
	DDX_Control(pDX, IDC_Option1Qty, c_Option1Qty);
	DDX_Control(pDX, IDC_Option2Qty, c_Option2Qty);
	DDX_Control(pDX, IDC_RiskFreeRate, c_RiskFreeRate);
	DDX_Control(pDX, IDC_EvalDate, c_EvalDate);
	DDX_Control(pDX, IDC_StockSymbol, c_StockSymbol);
	DDX_Control(pDX, IDC_Option1Symbol, c_Option1Symbol);
	DDX_Control(pDX, IDC_Option2Symbol, c_Option2Symbol);
	DDX_Control(pDX, IDC_Option3Symbol, c_Option3Symbol);
	DDX_CBString(pDX, IDC_StockSymbol, m_StockSymbol);
	DDX_CBString(pDX, IDC_Option1Symbol, m_Option1Symbol);
	DDX_CBString(pDX, IDC_Option2Symbol, m_Option2Symbol);
	DDX_CBString(pDX, IDC_Option3Symbol, m_Option3Symbol);
	DDX_DateTimeCtrl(pDX, IDC_EvalDate, m_EvalDate);
	DDX_Control(pDX, IDC_SellStock,	c_SellStock);
	DDX_Control(pDX, IDC_SellOption1, c_SellOption1);
	DDX_Control(pDX, IDC_SellOption2, c_SellOption2);
	DDX_Control(pDX, IDC_SellOption3, c_SellOption3);
	DDX_Control(pDX, IDC_BuyStock, c_BuyStock);
	DDX_Control(pDX, IDC_BuyOption1, c_BuyOption1);
	DDX_Control(pDX, IDC_BuyOption2, c_BuyOption2);
	DDX_Control(pDX, IDC_BuyOption3, c_BuyOption3);
	DDX_Text(pDX, IDC_Volatility, m_Volatility);
	DDV_MinMaxFloat(pDX, m_Volatility, 0.f, 10.f);
	DDX_Text(pDX, IDC_StockPrice, m_StockPrice);
	DDV_MinMaxDouble(pDX, m_StockPrice, 0.0, 1.e+009);
	DDX_Text(pDX, IDC_RiskFreeRate, m_RiskFreeRate);
	DDV_MinMaxFloat(pDX, m_RiskFreeRate, 0.f, 100.f);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( CHedgeExplorer, CPlotDialog )
	//{{AFX_MSG_MAP(CHedgeExplorer)
	ON_EN_KILLFOCUS(IDC_StockPrice, OnKillFocusStockPrice)
	ON_EN_KILLFOCUS(IDC_Volatility, OnKillFocusVolatility)
	ON_BN_CLICKED(IDC_BuyOption1, OnBuyOption1)
	ON_BN_CLICKED(IDC_BuyOption2, OnBuyOption2)
	ON_BN_CLICKED(IDC_BuyOption3, OnBuyOption3)
	ON_BN_CLICKED(IDC_BuyStock, OnBuyStock)
	ON_BN_CLICKED(IDC_SellOption1, OnSellOption1)
	ON_BN_CLICKED(IDC_SellOption2, OnSellOption2)
	ON_BN_CLICKED(IDC_SellOption3, OnSellOption3)
	ON_BN_CLICKED(IDC_SellStock, OnSellStock)
	ON_WM_PAINT()
	ON_EN_KILLFOCUS(IDC_RiskFreeRate, OnKillFocusRiskFreeRate)
	ON_NOTIFY(DTN_CLOSEUP, IDC_EvalDate, OnCloseUpEvalDate)
	ON_NOTIFY(NM_KILLFOCUS, IDC_EvalDate, OnKillFocusEvalDate)
	ON_WM_KEYDOWN()
	ON_WM_CTLCOLOR()

	ON_CBN_SELCHANGE(IDC_Option1Qty, OnSelChangeOption1Qty)
	ON_CBN_SELCHANGE(IDC_Option2Qty, OnSelChangeOption2Qty)
	ON_CBN_SELCHANGE(IDC_Option3Qty, OnSelChangeOption3Qty)
	ON_CBN_SELCHANGE(IDC_StockQty, OnSelChangeStockQty)
	ON_CBN_SELCHANGE(IDC_Option1Symbol, OnSelChangeOption1Symbol)
	ON_CBN_SELCHANGE(IDC_Option2Symbol, OnSelChangeOption2Symbol)
	ON_CBN_SELCHANGE(IDC_Option3Symbol, OnSelChangeOption3Symbol)
	ON_CBN_SELCHANGE(IDC_StockSymbol, OnSelChangeStockSymbol)

	ON_CBN_KILLFOCUS(IDC_Option1Qty, OnKillFocusOption1Qty)			// HotKeys +
	ON_CBN_KILLFOCUS(IDC_Option2Qty, OnKillFocusOption2Qty)			// HotKeys +
	ON_CBN_KILLFOCUS(IDC_Option3Qty, OnKillFocusOption3Qty)			// HotKeys +
	ON_CBN_KILLFOCUS(IDC_StockQty, OnKillFocusStockQty)				// HotKeys +
	ON_CBN_KILLFOCUS(IDC_Option1Symbol, OnKillFocusOption1Symbol)	// HotKeys +
	ON_CBN_KILLFOCUS(IDC_Option2Symbol, OnKillFocusOption2Symbol)	// HotKeys +
	ON_CBN_KILLFOCUS(IDC_Option3Symbol, OnKillFocusOption3Symbol)	// HotKeys +
	ON_CBN_KILLFOCUS(IDC_StockSymbol, OnKillFocusStockSymbol)		// HotKeys +

	ON_CBN_SETFOCUS(IDC_Option1Qty, OnSetFocusOption1Qty)			// HotKeys
	ON_CBN_SETFOCUS(IDC_Option2Qty, OnSetFocusOption2Qty)			// HotKeys
	ON_CBN_SETFOCUS(IDC_Option3Qty, OnSetFocusOption3Qty)			// HotKeys
	ON_CBN_SETFOCUS(IDC_StockQty, OnSetFocusStockQty)				// HotKeys
	ON_CBN_SETFOCUS(IDC_StockSymbol, OnSetFocusStockSymbol)			// HotKeys
	ON_CBN_SETFOCUS(IDC_Option1Symbol, OnSetFocusOption1Symbol)		// HotKeys
	ON_CBN_SETFOCUS(IDC_Option2Symbol, OnSetFocusOption2Symbol)		// HotKeys
	ON_CBN_SETFOCUS(IDC_Option3Symbol, OnSetFocusOption3Symbol)		// HotKeys
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()												// HotKeys
	ON_MESSAGE(WM_HOTKEY, OnHotKey)									// HotKeys

	ON_EN_SETFOCUS(IDC_StockPrice, OnSetFocusStockPrice)
	ON_EN_SETFOCUS(IDC_Volatility, OnSetFocusVolatility)
	ON_EN_SETFOCUS(IDC_RiskFreeRate, OnSetFocusRiskFreeRate)
	ON_NOTIFY(NM_SETFOCUS, IDC_EvalDate, OnSetFocusEvalDate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHedgeExplorer message handlers

BOOL CHedgeExplorer::OnInitDialog()
{	unsigned short	ii;
		// set the extents of plot region (and other initialization)
	CPlotDialog::OnInitDialog();	// sets plotExtents to ( 0, 0, SM_CXSCREEN, SM_CYSCREEN )
	RegisterHotKeys( IDC_StockQty, IDC_StockSymbol );	// screens for WM5 internally
	plotExtents.top = 125;			// leaves enough room for the controls at the top

		// get the risk free rate from the registry
	if ( ! GetRiskFreeRate(&m_RiskFreeRate) )					// was:  theApp.regMgr->
	{
#ifdef _DEBUG
		TRACE( _T("HedgeExplorer::OnInitDialog: no registered value for RiskFreeRate.\n") );
#endif
	}
	setEditBox( "%.3f", m_RiskFreeRate, IDC_RiskFreeRate );

		// initialize the Evaluation Date
	c_EvalDate.SetTime( m_EvalDate );

	CMapStringToPtr*	stocksMap = theApp.stkMgr->GetSymbolTable();
	bool thereAreStocks = stocksMap != NULL  &&  stocksMap->GetCount() > 0;
	if ( thereAreStocks )
	{	c_StockSymbol.LoadMap( stocksMap );

			// retrieve the most recently analyzed hedge
		if ( ! GetHedge(&os3oh) )										// was:  theApp.regMgr->
		{		// don't really care if it fails (we have a backup plan)
#ifdef _DEBUG
			TRACE( _T("HedgeExplorer::OnInitDialog: GetHedge failed.\n") );
#endif
		}

			// ensure that we have a valid stock def_ID
		if ( os3oh.def_ID[stkIdx] < 0 )
		{		// either no registered Hedge was found or the def_ID is invalid (corrupted?)
				// this is the first time that user has tapped the HedgeExplorer
				// try to find a recently used Option (for it's underlying)
				// if GetRecentOption() succeeds, it overwrites os3oh.def_ID[0]
				// try starting from the recent option
			GetRecentOption( &(os3oh.def_ID[0]) );							// was:  theApp.regMgr->
			if ( os3oh.def_ID[0] >= 0 )
			{	pOption[0] = theApp.optMgr->ReadOption( os3oh.def_ID[0] );
				if ( pOption[0] )
				{	CStock* stk = (pOption[0])->getUnderlying();
					if ( stk )
						os3oh.def_ID[stkIdx] = stk->getDef_ID();
				}
			}
			else
			{		// try starting from the recent stock
				GetRecentStock( &(os3oh.def_ID[stkIdx]) );					// was:  theApp.regMgr->
			}

			if ( os3oh.def_ID[0] >= 0 )
				os3oh.qty[0] = 1.0;										// display the first option

			if ( os3oh.def_ID[stkIdx] >= 0 )
				os3oh.qty[stkIdx] = 1.0;								// display the stock
		}

			// load what we found
		if ( os3oh.def_ID[stkIdx] >= 0 )
		{		// load according to the os3oh structure
			pStock = theApp.stkMgr->ReadStock( os3oh.def_ID[stkIdx] );
			for ( ii = 0; ii < nHedgeOpts; ii++ )
				if ( os3oh.def_ID[ii] >= 0  &&  pOption[ii] == NULL )
					pOption[ii] = theApp.optMgr->ReadOption( os3oh.def_ID[ii] );
		}

			// set the stock symbol in it's comboBox
		if ( pStock )
		{	CString stkSym = pStock->getSymbol();
			int	idx = c_StockSymbol.FindStringExact( 0, stkSym );
			if ( idx != CB_ERR )
				c_StockSymbol.SetCurSel( idx );
		}

			// load option symbols into comboBoxes and possibly select the first one
		SetThreeOptions( pOption[0]  ?  (pOption[0])->getSymbol()  :  _T("") );
		for ( ii = 1; ii < nHedgeOpts; ii++ )								// already handled pOption[0]
		{	if ( pOption[ii] )
			{	CString	optSym = pOption[ii]->getSymbol();
				CSeekComboBox*	scb = ( ii == 1  ?  &c_Option2Symbol  :  &c_Option3Symbol );
				int idx = scb->FindStringExact( 0, optSym );
				if ( idx != CB_ERR )
					scb->SetCurSel( idx );
			}
		}
	}
	c_StockSymbol.EnableWindow( thereAreStocks );
	c_StockSymbol.GetLBText( c_StockSymbol.GetCurSel(), m_StockSymbol );
#ifdef _DEBUG
	TRACE( _T("HedgeExplorer::OnInitDialog: m_StockSymbol=%s\n"), m_StockSymbol );
#endif
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// load up controls from initialized values
		// Buy/Sell buttons ...
	
		// Seed the random-number generator with current time
	SYSTEMTIME	sysTime;
	GetSystemTime( &sysTime );
	srand( sysTime.wMilliseconds + sysTime.wSecond + sysTime.wMinute + sysTime.wHour );
	for ( ii = 0; ii < nHedgeBOS; ii++ )
	{		// if there is already a BuySell status, we do nothing, otherwise
			// we randomly assign either Buy or Sell
		if ( os3oh.buySell[ii] == UnknownBuySellStatus )
		{
			os3oh.buySell[ii] = (rand() & 1)  ?  Buy  :  Sell;
		}
	}

		// now load them up
	c_BuyOption1.SetCheck(  os3oh.buySell[0] == Buy ? BST_CHECKED : BST_UNCHECKED );
	c_SellOption1.SetCheck( os3oh.buySell[0] == Sell ? BST_CHECKED : BST_UNCHECKED );
	
	c_BuyOption2.SetCheck(  os3oh.buySell[1] == Buy ? BST_CHECKED : BST_UNCHECKED );
	c_SellOption2.SetCheck( os3oh.buySell[1] == Sell ? BST_CHECKED : BST_UNCHECKED );
	
	c_BuyOption3.SetCheck(  os3oh.buySell[2] == Buy ? BST_CHECKED : BST_UNCHECKED );
	c_SellOption3.SetCheck( os3oh.buySell[2] == Sell ? BST_CHECKED : BST_UNCHECKED );

	c_BuyStock.SetCheck(  os3oh.buySell[stkIdx] == Buy ? BST_CHECKED : BST_UNCHECKED );
	c_SellStock.SetCheck( os3oh.buySell[stkIdx] == Sell ? BST_CHECKED : BST_UNCHECKED );


		// Qty EditBoxes ...
	short	nZeros = 0;
	for ( ii = 0; ii < nHedgeBOS; ii++ )
		nZeros += (os3oh.qty[ii] < 0.0001)  ?  0  :  1;
	if ( nZeros == 0 )
	{	for ( ii = 0; ii < nHedgeBOS; ii++ )
			os3oh.qty[ii] = (float)(rand() & 1);
	}
	for ( ii = 0; ii < nHedgeBOS; ii++ )
	{
		int	wholePart = (int)floor( os3oh.qty[ii] );
		float	fracPart = os3oh.qty[ii] - wholePart;
		CSeekComboBox* scb = (ii == 0  ?  &c_Option1Qty  :
							 (ii == 1  ?  &c_Option2Qty  :
							 (ii == 2  ?  &c_Option3Qty  :  &c_StockQty )));
		scb->SetCurSel( (int)((0 <= wholePart  &&  wholePart <= 9)  ?  wholePart  : 0) );
		if ( fabs(fracPart) > 0.001  ||  os3oh.qty[ii] < 0  ||  os3oh.qty[ii] > 9 )
		{	wchar_t buf[16];
			swprintf( buf, _T("%g"), os3oh.qty[ii] );
			scb->SetWindowText( buf );
		}
	}
	setComboBox( "%g", os3oh.qty[0], IDC_Option1Qty );
	setComboBox( "%g", os3oh.qty[1], IDC_Option2Qty );
	setComboBox( "%g", os3oh.qty[2], IDC_Option3Qty );
	setComboBox( "%g", os3oh.qty[stkIdx], IDC_StockQty );

	KillFocusStockDisplay();				// display stock price & volatility

		// Evaluate stock and/or options loaded 
	Plot();
	c_StockSymbol.SetFocus();
	return	FALSE;		// return TRUE unless you set the focus to a control
						// EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//----------------------------------------------------------------------------------------
void	CHedgeExplorer::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	CNillaDialog::OnActivate( nState, pWndOther, bMinimized );
	if ( nState == WA_ACTIVE  ||  nState == WA_CLICKACTIVE )
		RegisterHotKeys( IDC_StockQty, IDC_StockSymbol );	// screens for WM5 internally
}			// OnActivate()
//----------------------------------------------------------------------------------------
LRESULT		CHedgeExplorer::OnHotKey( WPARAM wParam, LPARAM lParam )
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
//----------------------------------------------------------------------------------------
HBRUSH	CHedgeExplorer::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	return	CPlotDialog ::OnCtlColor(pDC, pWnd, nCtlColor);
}			// OnCtlColor()
//----------------------------------------------------------------------------------------
void	CHedgeExplorer::SetThreeOptions( CString opt1Sym )
{	CString sym;
	int	nOptionsFound = LoadOptionSymbols();
	if ( nOptionsFound > 0 )
	{	int opt1idx = ( opt1Sym == _T("") )
						?  0
						:  c_Option1Symbol.FindStringExact( 0, opt1Sym );
		if ( opt1idx != CB_ERR )
		{	c_Option1Symbol.SetCurSel( opt1idx );
			sym = c_Option1Symbol.GetText();
			pOption[0] = theApp.optMgr->GetOption( sym );
			if ( pOption[0] )
				os3oh.def_ID[0] = pOption[0]->getDef_ID();
#ifdef _DEBUG
			TRACE( _T("HedgeExplorer::SetThreeOptions(1): pOption[0] is '%s', os3oh.def_ID[0]=%d\n"),
				pOption[0]->getSymbol(), os3oh.def_ID[0] );
#endif
		}
			// pick the middle symbol in the list
		if ( nOptionsFound > 1 )
		{	int opt2idx = nOptionsFound / 2;
			c_Option2Symbol.SetCurSel( opt2idx );
			sym = c_Option2Symbol.GetText();
			pOption[1] = theApp.optMgr->GetOption( sym );
			if ( pOption[1] )
				os3oh.def_ID[1] = pOption[1]->getDef_ID();
#ifdef _DEBUG
			TRACE( _T("HedgeExplorer::SetThreeOptions(2): pOption[1] is '%s', os3oh.def_ID[1]=%d\n"),
				pOption[1]->getSymbol(), os3oh.def_ID[1] );
#endif
		}
			// pick the last symbol in the list
		if ( nOptionsFound > 2 )
		{	int opt3idx = nOptionsFound - 1;
			c_Option3Symbol.SetCurSel( opt3idx );
			sym = c_Option3Symbol.GetText();
			pOption[2] = theApp.optMgr->GetOption( sym );
			if ( pOption[2] )
				os3oh.def_ID[2] = pOption[2]->getDef_ID();
#ifdef _DEBUG
			TRACE( _T("HedgeExplorer::SetThreeOptions(3): pOption[2] is '%s', os3oh.def_ID[2]=%d\n"),
				pOption[2]->getSymbol(), os3oh.def_ID[2] );
#endif
		}
	}
}			// SetThreeOptions()
//----------------------------------------------------------------------------
int		CHedgeExplorer::LoadOptionSymbols( void )
{		// depends on pStock being meaningful
	if ( pStock == NULL )
		return	0;
	
	CWaitCursor	wait;							// this may take a while
	int		nOptionsFound = 0;

		// start with all of the options in the database,
		// select those with pStock as the underlying
	CMapStringToPtr*	optionsMap = theApp.optMgr->GetSymbolTable();
	CMapStringToPtr		stkOptsMap;				// the down selected options map
	CString key;
	long	offset;
	POSITION pos = optionsMap->GetStartPosition();
	while ( pos )
	{	optionsMap->GetNextAssoc( pos, key, (void*&)offset );
#ifdef _DEBUG
//		TRACE( _T("HedgeExplorer::LoadOptionSymbols(1): fetched %s @ %d\n"),
//			key, offset );
#endif
		COption* anOption = theApp.optMgr->ReadOption( offset );
		if ( anOption )
		{	if ( anOption->stockSymbol_ID == pStock->symbol_ID )
			{
#ifdef _DEBUG
//		TRACE( _T("HedgeExplorer::LoadOptionSymbols(2): storing %s @ %d\n"),
//			key, offset );
#endif
				stkOptsMap.SetAt( key, (void*&)offset );
				nOptionsFound++;
			}
			delete	anOption;
		}
	}
#ifdef _DEBUG
	TRACE( _T("HedgeExplorer::LoadOptionSymbols(3): %d options found for %s\n"),
		nOptionsFound, pStock->getSymbol() );
#endif

	if ( nOptionsFound > 0 )
	{		// load up the options that refer to pStock
		c_Option1Symbol.LoadMap( &stkOptsMap );
		c_Option2Symbol.LoadMap( &stkOptsMap );
		c_Option3Symbol.LoadMap( &stkOptsMap );
	}

	c_Option1Symbol.EnableWindow( nOptionsFound > 0 );
	c_Option2Symbol.EnableWindow( nOptionsFound > 0 );
	c_Option3Symbol.EnableWindow( nOptionsFound > 0 );
	return	nOptionsFound;
}			// LoadOptionSymbols()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnPaint() 
{	unsigned short	ii;

		// see what's valid
	bool	stockValid = InstComplete( stkIdx );
	bool	somethingValid = stockValid;

	bool	optionValid[nHedgeOpts];
	short	nvOptions = 0;										// the number of valid options
	for ( ii = 0; ii < nHedgeOpts; ii++ )
	{	*(optionValid+ii) = InstComplete( ii );
		somethingValid = somethingValid  ||  *(optionValid+ii);
		if ( *(optionValid+ii) )
			nvOptions++;
	}

		// nothing to paint if there are no instruments to evaluate
	if ( ! somethingValid )
	{	CPaintDC	dc( this );
		DrawPlotBackground( dc );								// blanks out any existing plots
		CNillaDialog::OnPaint();										// solves the initially transparent background problem
		return;													// doesn't unpaint
	}
		// calculate the domain X-extents
	float	minXval = 2e38f;
	float	maxXval = -minXval;
	if ( pStock  &&  pStock->getMktPrice() > 0.0 )
	{	minXval = (float)m_StockPrice;
		maxXval = (float)m_StockPrice;
	}
	double	maxYrs = 0.0;
	for ( ii = 0; ii < nHedgeOpts; ii++ )
	{	if ( optionValid[ii] )
		{
#ifdef	_DEBUG
//			TRACE( _T("HedgeExplorer::OnPaint: m_EvalDate=%s, pOption[%d]->expiry=%s\n"),
//				m_EvalDate.Format(VAR_DATEVALUEONLY), ii, pOption[ii]->getExpiry().Format(VAR_DATEVALUEONLY) );
#endif
			double	yrs = (pOption[ii])->YrsToExpiry( m_EvalDate );
			if ( yrs > maxYrs )
				maxYrs = yrs;
				// strikes need to be in the window
			if ( (pOption[ii])->strikePrice < minXval )
				minXval = (pOption[ii])->strikePrice;
			if ( maxXval < (pOption[ii])->strikePrice )
				maxXval = (pOption[ii])->strikePrice;
		}
	}
#ifdef	_DEBUG
	TRACE( _T("HedgeExplorer::OnPaint: minXval=%g, maxXval=%g\n"), minXval, maxXval );
#endif
		// expand the X-range
	float	xRange = maxXval - minXval;
	float	stkPriceChange = (float)(3.0 * m_Volatility * maxYrs * m_StockPrice);
	float	minX = (float)(m_StockPrice - stkPriceChange);
	float	maxX = (float)(m_StockPrice + stkPriceChange);
	if ( minXval < minX )
		minX = minXval;
	if ( maxXval > maxX )
		maxX = maxXval;
	minX *= 0.85f;
	maxX *= 1.15f;
	if ( minX < 0.01 )
		minX = 0.01f;
#ifdef _DEBUG
	TRACE( _T("HedgeExplorer::OnPaint: stkPriceChange=%f, minX=%f, maxX=%f\n"),
		stkPriceChange, minX, maxX );
#endif
	topLeft.x = minX;
	bottomRight.x = maxX;

		// calculate the domain Y-extents
		// set up the summary array
		// include the effect of the stock if it's valid
	CFPoint	sumFpts[nHedgePts];
	double	yVal = 0.0;
	float	xVal = topLeft.x;
	float	xInc = (bottomRight.x - xVal) / (nHedgePts - 1);
	float	minYval = 2e38f;								// a really big positive number
	float	maxYval = -minYval;								// a really big negative number
	for ( short jj = 0; jj < nHedgePts; jj++ )
	{		// start at some stock price less than m_StockPrice (topLeft.x),
			// end at a higher one (bottomRight.x)
		yVal = stockValid ? (xVal - (float)m_StockPrice) * os3oh.qty[stkIdx] : 0.0f;	// long position's start negative, end positive
		if ( os3oh.buySell[stkIdx] == Sell )
			yVal = -yVal;

			// yVal could kick out the y-range
		if ( yVal < minYval )
			minYval = (float)yVal;
		if ( yVal > maxYval )
			maxYval = (float)yVal;

		sumFpts[jj].x = xVal;
		sumFpts[jj].y = (float)yVal;
		xVal += xInc;
	}
		// the following elements of a call to EuroValue don't change inside the loop
	OptionEvaluationContext	evcs;
	evcs.riskFreeRate = m_RiskFreeRate / 100.0;
	evcs.sigma = m_Volatility;
	evcs.fromDate = m_EvalDate;

		// collect option plot points
	CFPoint	optFpts[ nHedgeOpts * nHedgePts ];				// row-major order
	for ( ii = 0; ii < nHedgeOpts; ii++ )
	{	if ( optionValid[ii] )
		{	xVal = topLeft.x;								// re-copy the lowest x value
			evcs.calcYrsToExpiry = true;					// options can have different expiration dates
			evcs.calcPvDivs = true;							// expiration date impacts pvDivs too
			COption*	opt = pOption[ii];
			float	mktPrice = (float)(pOption[ii])->getMktPrice();
			for ( short jj = 0; jj < nHedgePts; jj++ )
			{	(optFpts + ii*nHedgePts + jj)->x = xVal;
				evcs.stockPrice = xVal;
				yVal = 0.0;										// we want this overwritten
				if ( opt->EuroValue( evcs, yVal ) )
				{	yVal -= mktPrice;
					yVal *= os3oh.qty[ii];						// scale by Qty
					if ( os3oh.buySell[ii] == Sell )
						yVal = -yVal;
#ifdef _DEBUG
//					TRACE( _T("HedgeExplorer:OnPaint: option(%d), pt=%d, x=%f, y=%f\n"),
//							ii, jj, xVal, yVal );
					if ( _isnan( yVal ) )
						AfxDebugBreak();
#endif
				}
					// store this y-value and update the sum
				optFpts[ii*nHedgePts + jj].y = (float)yVal;
				sumFpts[jj].y += (float)yVal;						// update the sum
				float	ySum = sumFpts[jj].y;				// use ySum instead of indexing sumFpts below

					// yVal could kick out the y-range
				if ( yVal < minYval )
					minYval = (float)yVal;
				if ( yVal > maxYval )
					maxYval = (float)yVal;

					// sumY could also kick out the y-range
				if ( ySum < minYval )
					minYval = ySum;
				if ( ySum > maxYval )
					maxYval = ySum;

					// update the xVal for the next pass
				xVal += xInc;
			}
		}
	}

		// inject the improved Y range values into topLeft and bottomRight
	float	yRange = maxYval - minYval;
	topLeft.y = maxYval + 0.08f * yRange;
		// leave space at the bottom for the grid lables, (0.10 is conservativev)
		// 0.05 can ocassionally draw under the grid labels are always visible
		// not using 0.12f here because there's less plot area in the HedgeExplorer
		// than there is in the PlotOptionsDialogs
	bottomRight.y = minYval - 0.15f * yRange;

#ifdef _DEBUG
//	TRACE( _T("HedgeExplorer::OnPaint: minYval=%g, maxYval=%g\n"), minYval, maxYval );
#endif

		// we have domain extents, so we can scale against available space and plot
	xScale = (float)(plotExtents.Width()  / (bottomRight.x - topLeft.x));	// ditto
	yScale = (float)(plotExtents.Height() / (topLeft.y - bottomRight.y));	// pixels / dollar

#ifdef _DEBUG
//	TRACE( _T("HedgeExplorer::OnPaint: xScale=%g, yScale=%g\n"), xScale, yScale );
#endif

		// now we can paint it
	CPaintDC	dc( this );				// device context for painting
	DrawGrid( dc );
	CPolyline	pl;						// was the sum of the stock & option positions, now just satisfies DrawStock()
	DrawStock( dc, pl );				// unchanged from the StrategyExplorer version
	for ( ii = 0; ii < nHedgeOpts; ii++ )
	{	if ( optionValid[ii] )
			DrawOption( ii, dc, (optFpts + ii*nHedgePts) );
	}
	if ( nvOptions + (stockValid ? 1 : 0) > 1 )
		DrawSum( dc, sumFpts );
	DrawGridLabels( dc );
}			// OnPaint()
//----------------------------------------------------------------------------
bool	CHedgeExplorer::InstComplete( short ii )
{		// ii is 0-based, the stock is at ii = stkIdx
	CBOS*	bos = ii == stkIdx ?  (CBOS*)pStock  :  (CBOS*)pOption[ii];
	bool	res =  os3oh.buySell[ii] != UnknownBuySellStatus
			  &&	   os3oh.qty[ii] > 0.0
			  &&				 bos != NULL
			  &&  bos->getMktPrice() > 0.0;
	if ( res  &&  ii != stkIdx )						// options
		res = res  &&  (m_Volatility > 0.0)
				   &&  ((pOption[ii])->strikePrice > 0.0);
	return	res;
}			// InstComplete()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//									Plot Support							//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CHedgeExplorer::DrawSum( CPaintDC& dc, CFPoint* sumFpts )
{		// plot grid, stock, options
//	DrawGrid( dc );										// know nothing of the domain extents
//	CPolyline	pl;										// sum of the stock & option positions
//	short	itemsDrawn = DrawStock( dc, pl );
//	for ( short ii = 0; ii < nOpts; ii++ )
//		itemsDrawn += DrawOption( ii, dc, pl );

		// draw the solution Polyline
	CPen		polyPen;
	COLORREF	polyColor  = RGB(0,0,0);				// black
	polyPen.CreatePen( PS_SOLID, 2, polyColor );
	CPen*		pOldPen = dc.SelectObject( &polyPen );

		// convert to screen coords
	CPoint	pts[nHedgePts];
	for ( short jj = 0; jj < nHedgePts; jj++ )
		MapToPlot( *(sumFpts+jj), *(pts+jj) );
	
		// use some ink
	BOOL	res = dc.Polyline( pts, nHedgePts );
#ifdef _DEBUG
	if ( res == 0 )
	{	TRACE( _T("HedgeExplorer::DrawSum: res=%d (should be nonzero)\n"), res );
		AfxDebugBreak();
	}
#endif
	dc.SelectObject( pOldPen );
}			// DrawSum()
//----------------------------------------------------------------------------
void	CHedgeExplorer::DrawOption( short ii, CPaintDC& dc, CFPoint* optFpts )
{
	CPen		optionPen;
		// 0 -->  255, 127, 127;	1 --> 127, 255, 127;	2 --> 127, 127, 255
	COLORREF	optionColor = RGB( ii == 0 ? 255 : 127,
								   ii == 1 ? 255 : 127,
								   ii == 2 ? 255 : 127 );		// creates Red, Green, or Blue
	optionPen.CreatePen( PS_SOLID, 2, optionColor );
	CPen*	pOldPen = dc.SelectObject( &optionPen );

	CPoint	pts[nHedgePts];
	for ( short jj = 0; jj < nHedgePts; jj++ )
		MapToPlot( *(optFpts+jj), *(pts+jj) );
	
	BOOL	res = dc.Polyline( pts, nHedgePts );
#ifdef _DEBUG
	if ( res == 0 )
	{	TRACE( _T("HedgeExplorer::DrawOption: res=%d (should be nonzero)\n"), res );
		AfxDebugBreak();
	}
#endif
	dc.SelectObject( pOldPen );
}			// DrawOption()
//----------------------------------------------------------------------------
short	CHedgeExplorer::DrawStock( CPaintDC& dc, CPolyline& pl )
{	if ( ! InstComplete(stkIdx) )
		return	0;
	CPen	stockPen;
	COLORREF	stockColor = RGB(232,0,232);				// pastel purple
	stockPen.CreatePen( PS_SOLID, 2, stockColor );
	CPen*	pOldPen = dc.SelectObject( &stockPen );

		// create a CLineSeg and map to the plot region
	float	fxx[2], fyy[2], polyX[2], polyY[2];
	fxx[0] = polyX[0] = topLeft.x;
	fxx[1] = polyX[1] = bottomRight.x;
	for ( short ii = 0; ii < 2; ii++ )
	{	float	buySell = (float)(os3oh.buySell[stkIdx] == Sell  ?  -1.0  :  1.0);			// the slope
			// the strategy polyline should not be clipped to the plot region
		fyy[ii] = polyY[ii] = (float)( buySell * (fxx[ii] - m_StockPrice) * os3oh.qty[stkIdx] );
			// the stock line has to be clipped to the plot region
		if ( fyy[ii] < bottomRight.y )
		{	fyy[ii] = bottomRight.y;
			fxx[ii] = fyy[ii] / (os3oh.qty[stkIdx] * buySell) + (float)m_StockPrice;
		}
		if ( fyy[ii] > topLeft.y )
		{	fyy[ii] = topLeft.y;
			fxx[ii] = fyy[ii] / (os3oh.qty[stkIdx] * buySell) + (float)m_StockPrice;	
		}
	}

		// update the polyline
	CFPoint	rtPolyPt( polyX[0], polyY[0] );
	CFPoint	leftPolyPt( polyX[1], polyY[1] );

	short	nPolyPts = pl.GetPointCount();
	pl.InsertPoint( leftPolyPt );
	pl.InsertPoint( rtPolyPt );
	if ( nPolyPts != 0 )
	{		// there was already something there to accummulate
		CLineSeg	seg( leftPolyPt, rtPolyPt );
		pl.BoostSegment( seg, rtPolyPt );
		pl.BoostPoint( rtPolyPt );
	}

		// update the plot
	CFPoint	rtPlotPt( fxx[0], fyy[0] );
	CFPoint	leftPlotPt( fxx[1], fyy[1] );

	CPoint	ptOut;
	MapToPlot( rtPlotPt, ptOut );
	dc.MoveTo( ptOut );

	MapToPlot( leftPlotPt, ptOut );
	dc.LineTo( ptOut );

		// Put the old pen back
	dc.SelectObject( pOldPen );
	return	1;		// a stub which will return number of segments (1) inserted	
}			// DrawStock()
//---------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Database Update								//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CHedgeExplorer::SaveIfChanged( bool updateGUI /* = false */ )
{
	CStock* aStock = NULL;
	bool	saveVerify;
	short	prefs;

		// works off of m_StockSymbol
	if ( m_StockSymbol == _T("") )
		goto	Exit;
		// to be thread safe, we fetch a fresh copy for MainFrame calls
		// true -> theOption is valid, false -> it might not be
	aStock = updateGUI ? pStock : theApp.stkMgr->GetStock( m_StockSymbol );
	if ( aStock == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("CHedgeExplorer::SaveIfChanged: no Stock found ... returning.") );
#endif
		goto	Exit;
	}

		// if the caller is MainFrame, dialog thread memory may have been reclaimed
		// so, we could produce an access violation accessing members of theOption
	saveVerify = true;
	prefs = AnalyzerChangeSymbol | AnalyzerClose;
	if ( GetSaveVerifyPrefs(&prefs) )									// was:  theApp.regMgr->
	{		// updateGUI tells us whether this is a close or change symbol action
			// updateGUI should be true from killFocusBondSymbolWork, representing a change symbol action
			//			 and false from MainFrame, representing a close dialog action
		short	mask = updateGUI ? AnalyzerChangeSymbol : AnalyzerClose;
		saveVerify = (prefs & mask) != 0;
	}

		// update the current stock's market price (before switching stocks)
		// obtain permission to save changes to aStock's market price
	UINT  result;

		// handle aStock's update query
	if ( aStock )
	{		// obtain permission to save changes to anOption's underlying stock
		bool	stkPrcChanged = fabs( m_StockPrice - aStock->getMktPrice() ) > 1e-4;
		bool	stkVolChanged = fabs( m_Volatility - aStock->volatility )	 > 1e-5;
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
					TRACE( _T("CHedgeExplorer::SaveIfChanged: Stock write failed, res=%d\n"), res );
#endif
			}
		}
	}
		// calls from MainFrame (updateGUI == false), fetch a fresh copy from pDB
Exit:
	if ( ! updateGUI )
		delete	aStock;			// not simply a reference to a dialog var
}			// SaveIfChanged()
//---------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//						Control Messages start here							//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CHedgeExplorer::KillFocusEvalDateWork( void )
{
	COleDateTime	aDate;
	c_EvalDate.GetTime( aDate );
	if ( aDate == m_EvalDate )
		return;

	m_EvalDate = aDate;
	Plot();
}			// killFocusEvalDateWork()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnCloseUpEvalDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	KillFocusEvalDateWork();
	*pResult = 0;
}			// OnCloseUpEvalDate()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnKillFocusEvalDate( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	KillFocusEvalDateWork();
	*pResult = 0;
}			// OnKillFocusEvalDate()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//					Symbol KillFocus workhorse functions					//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CHedgeExplorer::KillFocusStockSymbolWork( void )
{		// delete stock
	if ( pStock )
	{	delete	pStock;
		pStock = NULL;
		os3oh.def_ID[stkIdx] = -1;
	}

		// get the Stock and load the options ComboBoxes
	os3oh.def_ID[stkIdx] = theApp.stkMgr->AssetExists( m_StockSymbol );
	if ( os3oh.def_ID[stkIdx] < 0 )
	{	return;								// no such Stock in the database
	}

	pStock = theApp.stkMgr->ReadStock( os3oh.def_ID[stkIdx] );
	if ( pStock )
	{	KillFocusStockResetOptions();
			// load option symbols & three options associated with this stock
		SetThreeOptions( _T("") );		// (if that many exist)
		KillFocusStockDisplay();		// show stock price & volatility
		Plot();
	}
}			// KillFocusStockSymbolWork()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnKillFocusStockSymbol( void )
{	CString stkSym;
	if ( criticalRegion )
		return;								// we're in the middle of the SelChange confirm save process
	c_StockSymbol.GetWindowText( stkSym );
	stkSym.MakeUpper();
#ifdef _DEBUG
	TRACE( _T("HedgeExplorer::OnKillFocusStockSymbol: stkSym=%s, m_StockSymbol=%s\n"), stkSym, m_StockSymbol );
#endif
	if ( stkSym == m_StockSymbol )
		return;								// nothing to do
	SaveIfChanged( true );
	m_StockSymbol = stkSym;

	KillFocusStockSymbolWork();
}			// OnKillFocusStockSymbol()
//----------------------------------------------------------------------------
void CHedgeExplorer::OnSelChangeStockSymbol( void )
{	CString stkSym;
	int cnt = c_StockSymbol.GetCount();
	if ( cnt < 1 )
		return;
	int sel = c_StockSymbol.GetCurSel();
#if ( _WIN32_WCE >= 0x420 )							// just WM5
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
	c_StockSymbol.GetLBText( sel, stkSym );
	if ( stkSym == m_StockSymbol )
		return;											// nothing to do
	SaveIfChanged( true );
	m_StockSymbol = stkSym;
//	c_StockSymbol.SetWindowText( stkSym );		// does this need to be here?

	KillFocusStockSymbolWork();
}			// OnSelChangeStockSymbol()
//----------------------------------------------------------------------------
void	CHedgeExplorer::KillFocusStockDisplay( void )
{		// display the stock's market price
	if ( pStock == NULL )
		return;

	m_StockPrice = pStock->getMktPrice();
	setEditBox( "%g", m_StockPrice, IDC_StockPrice );

		// display the stock's volatility
	m_Volatility = pStock->volatility;
	setEditBox( "%g", m_Volatility, IDC_Volatility );
}			// killFocusStockDisplay()
//----------------------------------------------------------------------------
void	CHedgeExplorer::KillFocusStockResetOptions( void )
{		// delete options
	for ( short ii = 0; ii < nHedgeOpts; ii++ )
	{	if ( pOption[ii] )
		{	delete	pOption[ii];
			pOption[ii] = NULL;
			os3oh.def_ID[ii] = -1;
		}
	}
		// clear option ComboBoxes
	c_Option1Symbol.ResetContent();
	c_Option2Symbol.ResetContent();
	c_Option3Symbol.ResetContent();
}			// killFocusStockResetOptions()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//							OptionSymbol messages							//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void	CHedgeExplorer::KillFocusOptionSymbolCore(
	CSeekComboBox*	scb,
	short			ii,
	CString			optSym							)
{		// this routine is called by:  KillFocusOptionXSymbolWork() routines 
		// and by:  SelCHange OptionXSymbolWork routines
		// ii is the zero-based index of pOption[]

		// get the Option
	if ( pOption[ii] )
	{	delete	pOption[ii];
		pOption[ii] = NULL;
	}

	os3oh.def_ID[ii] = theApp.optMgr->AssetExists( optSym );
	if ( os3oh.def_ID[ii] < 0 )
		return;

	CButton*	pBuyButton = ( ii == 0 ? &c_BuyOption1
						   : ( ii == 1 ? &c_BuyOption2 : &c_BuyOption3 ));
	CButton*	pSellButton = ( ii == 0 ? &c_SellOption1
						    : ( ii == 1 ? &c_SellOption2 : &c_SellOption3 ));
	pOption[ii] = theApp.optMgr->ReadOption( os3oh.def_ID[ii] );
	if (	 pOption[ii]  &&  os3oh.qty[ii] > 0
		 && ( pBuyButton->GetCheck() == BST_CHECKED  ||  pSellButton->GetCheck() == BST_CHECKED ) )
		Plot();
}			// KillFocusOptionSymbolCore()
//----------------------------------------------------------------------------
void	CHedgeExplorer::KillFocusOptionSymbolWork(
	CSeekComboBox*	scb,
	short			ii,
	CString&		optSym							)
{	CString			sym;
	m_activeComboBox = NULL;									// HotKeys
	scb->GetWindowText( sym );
	if ( sym == optSym )
		return;													// nothing to do
	optSym = sym;
	sym.MakeUpper();
	KillFocusOptionSymbolCore( scb, ii, optSym );
}			// KillFocusOptionSymbolWork()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnKillFocusOption1Symbol( void )
{	
	KillFocusOptionSymbolWork( &c_Option1Symbol, 0, m_Option1Symbol );
}			// OnKillFocusOption1Symbol()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnKillFocusOption2Symbol( void )
{
	KillFocusOptionSymbolWork( &c_Option2Symbol, 1, m_Option2Symbol );
}			// OnKillFocusOption2Symbol()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnKillFocusOption3Symbol( void )
{
	KillFocusOptionSymbolWork( &c_Option3Symbol, 2, m_Option3Symbol );
}			// OnKillFocusOption3Symbol()
//----------------------------------------------------------------------------
void	CHedgeExplorer::SelChangeOptionSymbolWork(
	CSeekComboBox*	scb,
	short			ii,
	CString&		optSym							)
{	CString	sym;
	int cnt = scb->GetCount();
	if ( cnt < 1 )
		return;
	int sel = scb->GetCurSel();
#if ( _WIN32_WCE >= 0x420 )							// just WM5
	if ( scb->lastChar == VK_DOWN  ||  scb->lastChar == VK_UP )
	{	int	delta = ( scb->lastChar == VK_DOWN  ?  1  :  -1 );
		sel += delta;
		if ( sel < 0 )
			sel = cnt - 1;
		if ( sel >= cnt )
			sel = 0;
		scb->SetCurSel( sel );
	}
#endif
	if ( sel < 0 ) sel = 0;							// should be superfluous
	scb->GetLBText( sel, sym );
	if ( sym == optSym )
		return;										// nothing to do
	optSym = sym;

	KillFocusOptionSymbolCore( scb, ii, optSym );
}			// SelChangeOptionSymbolWork()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnSelChangeOption1Symbol( void )
{
	SelChangeOptionSymbolWork( &c_Option1Symbol, 0, m_Option1Symbol );
}			// OnSelChangeOption1Symbol()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnSelChangeOption2Symbol( void )
{
	SelChangeOptionSymbolWork( &c_Option2Symbol, 1, m_Option2Symbol );
}			// OnSelChangeOption2Symbol()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnSelChangeOption3Symbol( void )
{
	SelChangeOptionSymbolWork( &c_Option3Symbol, 2, m_Option3Symbol );
}			// OnSelChangeOption3Symbol()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Qty Edit boxes								//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CHedgeExplorer::KillFocusQtyWork( CSeekComboBox* scb, float* qty )
{	float	ff;
	m_activeComboBox = NULL;								// HotKeys
		// passing the gate requires that GetFloat() works
		// and that ff differs from what we know
	if ( scb->GetFloat(ff)  &&  ff == *qty )
		return;
	*qty = (float)fabs(ff);

	Plot();
}			// KillFocusQtyWork()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnKillFocusOption1Qty( void )
{	
	KillFocusQtyWork( &c_Option1Qty, &(os3oh.qty[0]) );
}			// OnKillFocusOption1Qty()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnKillFocusOption2Qty( void )
{	
	KillFocusQtyWork( &c_Option2Qty, &(os3oh.qty[1]) );
}			// OnKillFocusOption2Qty()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnKillFocusOption3Qty( void )
{	
	KillFocusQtyWork( &c_Option3Qty, &(os3oh.qty[2]) );
}			// OnKillFocusOption3Qty()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnKillFocusStockQty( void )
{
	KillFocusQtyWork( &c_StockQty, &(os3oh.qty[stkIdx]) );
}			// OnKillFocusStockQty()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnSetFocusOption1Qty( void )
{	
	m_activeComboBox = IDC_Option1Qty;
}			// OnSetFocusOption1Qty()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnSetFocusOption2Qty( void )
{	
	m_activeComboBox = IDC_Option2Qty;
}			// OnSetFocusOption2Qty()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnSetFocusOption3Qty( void )
{	
	m_activeComboBox = IDC_Option3Qty;
}			// OnSetFocusOption3Qty()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnSetFocusStockQty( void )
{
	m_activeComboBox = IDC_StockQty;
}			// OnSetFocusStockQty()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnSetFocusOption1Symbol( void )
{	
	m_activeComboBox = IDC_Option1Symbol;
}			// OnSetFocusOption1Symbol()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnSetFocusOption2Symbol( void )
{	
	m_activeComboBox = IDC_Option2Symbol;
}			// OnSetFocusOption2Symbol()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnSetFocusOption3Symbol( void )
{	
	m_activeComboBox = IDC_Option3Symbol;
}			// OnSetFocusOption3Symbol()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnSetFocusStockSymbol( void )
{
	m_activeComboBox = IDC_StockSymbol;
}			// OnSetFocusStockSymbol()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CHedgeExplorer::SelChangeQtyWork( CSeekComboBox* scb, float* qty )
{	float	ff0, ebff, ffn;
	int		ebii;
	int	cnt = scb->GetCount();
	if ( cnt < 1 )
		return;
#ifdef _DEBUG
	TRACE( _T("HedgeExplorer::SelChangeQtyWork: VK_DOWN=%d, VK_UP=%d, scb->lastChar=%d\n"),
			VK_DOWN, VK_UP, scb->lastChar );
#endif
	if ( scb->lastChar == VK_DOWN  ||  scb->lastChar == VK_UP )
	{		// here we ignore the LB selection and start from the EditBox value
		if ( scb->GetFloat(ebff) != 1 )				// the EditBox float
			return;
		if (	 scb->GetFloat(  0,ff0 ) != 1		// the first ListBox float
			 ||  scb->GetFloat(cnt,ffn ) != 1 )		// the last  ListBox float
			return;

		float	step  = ( ffn - ff0) / (cnt-1);		// eleven entries --> 10 steps
		float	ebIdx = (ebff - ff0) / step;		// a ListBox equivalent 'index'
		double	frac = fabs( ebIdx - floor( ebIdx ) );
		bool	approxLBmatch = ( frac < 1e-9 );
		if ( scb->lastChar == VK_DOWN )				// towards higher values
		{	ebii = (int)ceil( ebIdx );
			if ( approxLBmatch )
				ebii++;
			if ( ebii >= cnt )
				ebii = cnt - 1;
		}
		else	// if ( scb->lastChar == VK_UP )	// towards lower values
		{	ebii = (int)floor( ebIdx );
			if ( approxLBmatch )
				ebii--;
			if ( ebii < 0 )
				ebii = 0;
		}
		scb->SetCurSel( ebii );
	}
	else
		ebii = scb->GetCurSel();
	if ( ebii < 0 ) ebii = 0;						// should be superfluous

	float	ff;
	if ( scb->GetFloat( ebii, ff ) != 1 )			// load the list box float into ff
	{
#ifdef _DEBUG
		TRACE( _T("HedgeExplorer::SelChangeQtyWork: Couldn't get a float at index %d\n"), ebii );
#endif
		return;										// this would be inconvenient
	}
	*qty = ff;

	Plot();
}			// OnSelChangeQtyWork()
//----------------------------------------------------------------------------
void CHedgeExplorer::OnSelChangeOption1Qty( void )
{
	SelChangeQtyWork( &c_Option1Qty, &(os3oh.qty[0]) );
}			// OnSelChangeOption1Qty()
//----------------------------------------------------------------------------
void CHedgeExplorer::OnSelChangeOption2Qty( void )
{
	SelChangeQtyWork( &c_Option2Qty, &(os3oh.qty[1]) );
}			// OnSelChangeOption2Qty()
//----------------------------------------------------------------------------
void CHedgeExplorer::OnSelChangeOption3Qty( void )
{
	SelChangeQtyWork( &c_Option3Qty, &(os3oh.qty[2]) );
}			// OnSelChangeOption3Qty()
//----------------------------------------------------------------------------
void CHedgeExplorer::OnSelChangeStockQty( void )
{
	SelChangeQtyWork( &c_StockQty, &(os3oh.qty[stkIdx]) );
}			// OnSelChangeStockQty()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Buy/Sell Buttons							//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CHedgeExplorer::BuySellWork(
	CButton*			pBuyButton,
	CButton*			pSellButton,
	BuySellStatus*		pBuySellMemory,
	BuySellStatus		bsVal			)
{
	if ( *pBuySellMemory == bsVal )
		return;
	*pBuySellMemory = bsVal;

	pBuyButton->SetCheck( bsVal == Buy  ?  BST_CHECKED  :  BST_UNCHECKED );
	pSellButton->SetCheck( bsVal == Sell  ?  BST_CHECKED  :  BST_UNCHECKED );

	Plot();
}			// BuySellWork()
//---------------------------------------------------------------------------
void	CHedgeExplorer::OnBuyStock() 
{
	BuySellWork( &c_BuyStock, &c_SellStock, &(os3oh.buySell[stkIdx]), Buy );
}			// OnBuyStock()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnSellStock() 
{
	BuySellWork( &c_BuyStock, &c_SellStock, &(os3oh.buySell[stkIdx]), Sell );
}			// OnSellStock()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnBuyOption1() 
{
	BuySellWork( &c_BuyOption1, &c_SellOption1, &(os3oh.buySell[0]), Buy );
}			// OnBuyOption1()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnSellOption1() 
{
	BuySellWork( &c_BuyOption1, &c_SellOption1, &(os3oh.buySell[0]), Sell );
}			// OnSellOption1()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnBuyOption2() 
{
	BuySellWork( &c_BuyOption2, &c_SellOption2, &(os3oh.buySell[1]), Buy );
}			// OnBuyOption2()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnSellOption2() 
{
	BuySellWork( &c_BuyOption2, &c_SellOption2, &(os3oh.buySell[1]), Sell );
}			// OnSellOption2()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnBuyOption3() 
{
	BuySellWork( &c_BuyOption3, &c_SellOption3, &(os3oh.buySell[2]), Buy );
}			// OnBuyOption3()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnSellOption3() 
{
	BuySellWork( &c_BuyOption3, &c_SellOption3, &(os3oh.buySell[2]), Sell );
}			// OnSellOption3()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//					Last Row Edit box messages	(except Qty)				//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CHedgeExplorer::KillFocusEditBoxWork( int ctrlID, float* storedFloat )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( ctrlID );
	if ( ff == *storedFloat )
		return;

	*storedFloat = (float)fabs(ff);
	setEditBox( "%g", *storedFloat, ctrlID );

	Plot();
}			// killFocusEditBoxWork()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnKillFocusStockPrice( void )
{
	KillFocusEditBoxWork( IDC_StockPrice, &m_StockPrice );
}			// OnKillFocusStockPrice()
//----------------------------------------------------------------------------
void CHedgeExplorer::OnSetFocusStockPrice( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusStockPrice()
//----------------------------------------------------------------------------
void CHedgeExplorer::OnKillFocusRiskFreeRate( void )
{
	KillFocusEditBoxWork( IDC_RiskFreeRate, &m_RiskFreeRate );
}			// OnKillFocusRiskFreeRate()
//----------------------------------------------------------------------------
void CHedgeExplorer::OnSetFocusRiskFreeRate( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusRiskFreeRate()
//----------------------------------------------------------------------------
void	CHedgeExplorer::OnKillFocusVolatility( void )
{
	KillFocusEditBoxWork( IDC_Volatility, &m_Volatility );
}			// OnKillFocusVolatility()
//----------------------------------------------------------------------------
void CHedgeExplorer::OnSetFocusVolatility( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusVolatility
//----------------------------------------------------------------------------
void CHedgeExplorer::OnSetFocusEvalDate(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	*pResult = 0;
}			// OnSetFocusEvalDate()
//----------------------------------------------------------------------------
/*
void	CHedgeExplorer::OnPaint() 
{		// nothing to paint if there are no instruments to evaluate
	if ( ! CalcDomainExtents() )
	{	CNillaDialog::OnPaint();			// solves the initially transparent background problem
		return;
	}

		// we have domain extents, so we can scale against available space and plot
	xScale = (float)(plotExtents.Width()  / (bottomRight.x - topLeft.x));	// ditto
	yScale = (float)(plotExtents.Height() / (topLeft.y - bottomRight.y));	// pixels / dollar
	if ( xScale < 0.0  ||  yScale < 0.0 )
	{
#ifdef _DEBUG
		TRACE( _T("HedgeExplorer::OnPaint: negative scale detected, xScale=%g, yScale=%g\n"),
			xScale, yScale );
		AfxDebugBreak();
#endif
	}

	CPaintDC	dc( this );				// device context for painting
	DrawChart( dc );
}			// OnPaint()
*/
//----------------------------------------------------------------------------
/*
void	CHedgeExplorer::DrawChart( CPaintDC& dc )
{		// plot grid, stock, options
	DrawGrid( dc );										// known nothing of the domain extents
	CPolyline	pl;										// sum of the stock & option positions
	short	itemsDrawn = DrawStock( dc, pl );
	for ( short ii = 0; ii < nOpts; ii++ )
		itemsDrawn += DrawOption( ii, dc, pl );

		// draw the solution Polyline
	CPen		polyPen;
	COLORREF	polyColor  = RGB(0,0,0);				// black
	polyPen.CreatePen( PS_SOLID, 2, polyColor );
	CPen*		pOldPen = dc.SelectObject( &polyPen );
	const	unsigned short	nHedgePts = pl.GetPointCount();
	for ( ii = 0; ii < nHedgePts - 1; ii++ )
	{	CLineSeg* seg = pl.GetSegment( ii );
		if ( seg == NULL )
		{
#ifdef _DEBUG
			TRACE( _T("HedgeExplorer::DrawChart: received an unexpected NULL segment from the Polyline\n") );
			AfxDebugBreak();
#endif
		}
		CPoint	ptOut;
		if ( ii == 0 )
		{	MapToPlot( seg->GetLeft(), ptOut );			// after ii = 0, this is superfluous
			dc.MoveTo( ptOut );							// ditto
		}
		MapToPlot( seg->GetRight(), ptOut );
		dc.LineTo( ptOut );
		delete	seg;
	}
	DrawGridLabels( dc );
	dc.SelectObject( pOldPen );
}			// DrawChart()
*/
//----------------------------------------------------------------------------
/*
short	CHedgeExplorer::DrawOption( short ii, CPaintDC& dc, CPolyline& pl )
{		// Draws the extrinsic profit line,
		// calls DrawBlackScholes() to plot the sum of the intrinsic and extrinsic values
		// idx is 0-based for compatibility with MFC variable names
		// idx based gates...
	if (	 os3oh.buySell[ii] == UnknownBuySellStatus
		 ||	 os3oh.qty[ii] == 0.0
		 ||  pOption[ii] == NULL 
		 ||	 (pOption[ii])->putCall == UnknownOptionType
		 ||  (pOption[ii])->mktPrice == 0.0
		 ||  (pOption[ii])->strikePrice == 0.0 )
		return	0;

	CPen		optionPen;
	COLORREF	optionColor = RGB( ii == 0 ? 255 : 127,
								   ii == 1 ? 255 : 127,
								   ii == 2 ? 255 : 127 );		// creates Red, Green, or Blue
	optionPen.CreatePen( PS_SOLID, 2, optionColor );
	CPen*	pOldPen = dc.SelectObject( &optionPen );

		// create CLineSegs and map to the plot region
		// horizontal segment for Calls starts at 0 and ends at strike
		// slope is only used for remapping clipped segments.  The optionY's are
		// generated by OptionY(), which knows nothing about expiration dates
		// hence the need for the bool 'expired' below.
	float	slope = (float)(os3oh.qty[ii] * (os3oh.buySell[ii] == Buy  ?  1.0 : -1.0)
									  * (pOption[ii]->putCall == Call ?  1.0 : -1.0) );

	BOOL expired = (m_EvalDate > (pOption[ii])->expiry);

	if ( os3oh.qty[stkIdx] > 0.0 )
		slope /= os3oh.qty[stkIdx];							s	// rescale by stock Qty

	CFPoint	strikePt;
	float	fxx[2], fyy[2], polyX[2], polyY[2];
	fxx[0] = polyX[0] = topLeft.x;
	fxx[1] = polyX[1] = bottomRight.x;
	for ( short jj = 0; jj < 2; jj++ )
	{		// for each of the three points on the Option profit profile ...
			// the strategy polyline should not be clipped
		polyY[jj] = fyy[jj] = OptionY( ii, fxx[jj], strikePt );
			// handle expired options
		if ( expired )
			polyY[jj] = fyy[jj] = strikePt.y;  
			// the Option's profit line may have to be clipped to the plot region
		if ( fyy[jj] > topLeft.y )
		{	float	yDiff = topLeft.y - fyy[jj];				// yDiff < 0
			fyy[jj] = topLeft.y;
			fxx[jj] += yDiff / slope;							// Put slope < 0, Call slope > 0
		}
		if ( fyy[jj] < bottomRight.y )
		{	float	yDiff = bottomRight.y - fyy[jj];			// yDiff > 0
			fyy[jj] = bottomRight.y;
			fxx[jj] += yDiff / slope;							// Put slope > 0, Call slope < 0
		}
	}
		// update the polyline
	CFPoint	rtPolyPt( polyX[0], polyY[0] );
	CFPoint	leftPolyPt( polyX[1], polyY[1] );

	short	nHedgePts = pl.GetPointCount();
	pl.InsertPoint( leftPolyPt );
	pl.InsertPoint( strikePt );
	pl.InsertPoint( rtPolyPt );
	if ( nHedgePts != 0 )
	{		// there was already something there to accummulate
		CLineSeg	seg1( leftPolyPt, strikePt );
		pl.BoostSegment( seg1, strikePt );			// skip the strikePt
		CLineSeg	seg2( strikePt, rtPolyPt );
		pl.BoostSegment( seg2, rtPolyPt );
		pl.BoostPoint( rtPolyPt );
	}

		// update the plot
	CFPoint	rtPlotPt( fxx[0], fyy[0] );
	CFPoint	leftPlotPt( fxx[1], fyy[1] );

		// plot this Option
	CPoint	ptOut;
	MapToPlot( rtPlotPt, ptOut );
	dc.MoveTo( ptOut );
	MapToPlot( strikePt, ptOut );
	dc.LineTo( ptOut );
	MapToPlot( leftPlotPt, ptOut );
	dc.LineTo( ptOut );

		// Put the old pen back
	dc.SelectObject( pOldPen );

		// the inflection point in the profit diagram is the CPoint of interest
	return	2;		// a stub which will return number of segments (2) inserted
}			// DrawOption()
*/
//---------------------------------------------------------------------------
/*
float	CHedgeExplorer::OptionY( short ii, float fxx, CFPoint& strikePt )
{		// OptionY() is only called from functions that gate validity of the option first
	ASSERT ( ii < nOpts  &&  InstComplete(ii) );
	float	fyy = (float)((pOption[ii])->mktPrice * os3oh.qty[ii] * (os3oh.buySell[ii] == Buy ? -1.0 : 1.0));
	bool haveStockQty = os3oh.qty[stkIdx] != 0.0;
	if ( haveStockQty )
		fyy /= os3oh.qty[stkIdx];						// downscale by StockQty
	strikePt = CFPoint( (pOption[ii])->strikePrice, fyy );
		// if we're on the horizontal section of the profit profile, so we're done ...
	if (	(fxx > (pOption[ii])->strikePrice  &&  (pOption[ii])->putCall == Put)
		||	(fxx < (pOption[ii])->strikePrice  &&  (pOption[ii])->putCall == Call) )
		return	fyy;		// anything above (below) a Put (Call) strike is a premium loss

		// we're on the profit diagonal
	float	slope = (float)(os3oh.qty[ii] * (os3oh.buySell[ii] == Buy		    ?  1.0 : -1.0)
									  * ((pOption[ii])->putCall == Call ?  1.0 : -1.0) );
		// dxx is the x displacement of interest; dxx > 0 for 
	float	dxx = fxx - strikePt.x;				// dxx > 0 for Calls; dxx < 0 for Puts
	fyy = slope * dxx + strikePt.y;
	if ( haveStockQty )
		fyy /= os3oh.qty[stkIdx];						// downscale by StockQty
	return	fyy;
}			// optionY()
*/
//---------------------------------------------------------------------------
/*
bool		CHedgeExplorer::CalcDomainExtents( void )
{	float	loStrike, hiStrike;
		// calculate the extents (topLeft & bottomRight) of the strike-profit domain
		// modify topLeft, bottomRight, xSc
		// return false to inhibit plotting (i.e. no plot is possible)
	loStrike = (float)1e38;			// something really far positive
	hiStrike = - loStrike;			// something really far negative
	bool	complete[4];
	for ( short ii = 0; ii < 4; ii++ )
	{	complete[ii] = InstComplete(ii);
		if ( complete[ii] )
		{		// the stock price serves in place of a strike
			float	priceStrike	= ( ii < nOpts )  ?  (pOption[ii])->strikePrice  :  m_StockPrice;
			loStrike  = priceStrike < loStrike  ?  priceStrike  :  loStrike;
			hiStrike  = priceStrike > hiStrike  ?  priceStrike  :  hiStrike;
		}
	}
	if ( loStrike > 1e37 )
		return	false;
		// center about the available strikes
	float	midPoint = (float)( (hiStrike + loStrike)/2.0 );

		// set up X test points
	float	xTest[5];
	for ( ii = 0; ii < nOpts; ii++ )
		xTest[ii] = complete[ii]  ?  (pOption[ii])->strikePrice  :  midPoint;	// if incomplete, renders the test point harmless

	xTest[3] = (float)(loStrike - midPoint * 0.15);			// left end
	xTest[4] = (float)(hiStrike + midPoint * 0.15);			// right end

	float	fyy, polyY, maxY, minY = (float)1e38;
	maxY = -minY;
	for ( short tt = 0; tt < 5; tt++ )
	{		// for each test point
		polyY = 0.0;
		for ( ii = 0; ii < 4; ii++ )
		{		// for each complete Option
			if ( complete[ii] )
			{	if ( ii < nOpts )
				{		// Options
					CFPoint	strikePt;
					fyy = OptionY( ii, xTest[tt], strikePt );

					float	premium = strikePt.y;
					minY = premium < minY  ?  premium  :  minY;
					maxY = premium > maxY  ?  premium  :  maxY;
				}
				else
				{		// Stocks
					fyy = (float)( (os3oh.buySell[stkIdx] == Sell  ?  -1.0  :  1.0)
						*		   (xTest[tt] - m_StockPrice) * os3oh.qty[ii]		);
				}
				polyY += fyy;
					// don't consider the Y-extents of the stock line
				// minY = fyy < minY  ?  fyy  :  minY;
				// maxY = fyy > maxY  ?  fyy  :  maxY;
			}
		}
			// collect the min/max polyY values
		maxY = polyY > maxY  ?  polyY  :  maxY;
		minY = polyY < minY  ?  polyY  :  minY;
	}
	float	plotHeight = maxY - minY;
		// provide some plot space above (below) the max (min) values
	maxY += (float)( 0.15 * plotHeight );
	minY -= (float)( 0.15 * plotHeight );

		// we now have a real valued strike spread and Y-range
	topLeft.x = xTest[3];			// loStrike - 15% of midPoint
	topLeft.y = maxY;
	bottomRight.x = xTest[4];		// hiStrike + 15% of midPoint
	bottomRight.y = minY;
	return	true;
}			// CalcDomainExtents()
*/
//----------------------------------------------------------------------------
