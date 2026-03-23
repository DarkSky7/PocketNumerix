// StrategyExplorer.cpp : implementation file
//
#include "StdAfx.h"
//#include "OptionType.h"
#include "resource.h"
#include "LineSeg.h"
#include "Polyline.h"
#include "StrategyExplorer.h"
#include "OneStk3OptStrategy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const	COLORREF	bkgndColor = RGB(233,233,188);		// background fill color (manilla 228,228,184)
const	COLORREF	gridColor  = RGB(191,191,191);		// most grid lines (light Gray)
const	COLORREF	axisColor  = RGB(159,159,159);		// X-axis (light-mid Gray)
const	COLORREF	textColor  = RGB( 63, 63, 63);		// labels (dark Gray)

/////////////////////////////////////////////////////////////////////////////
// CStrategyExplorer dialog

CStrategyExplorer::CStrategyExplorer( CWnd* pParent /*=NULL*/ )
	: CNillaDialog( CStrategyExplorer::IDD, pParent )
{
	//{{AFX_DATA_INIT( CStrategyExplorer )
	m_Strategy = _T("");
	//}}AFX_DATA_INIT
}

void	CStrategyExplorer::DoDataExchange( CDataExchange* pDX )
{
	CNillaDialog::DoDataExchange( pDX );
	//{{AFX_DATA_MAP( CStrategyExplorer )
	DDX_Control(pDX, IDC_SellStock,	c_SellStock);
	DDX_Control(pDX, IDC_SellOption1, c_SellOption1);
	DDX_Control(pDX, IDC_SellOption2, c_SellOption2);
	DDX_Control(pDX, IDC_SellOption3, c_SellOption3);
	DDX_Control(pDX, IDC_BuyStock, c_BuyStock);
	DDX_Control(pDX, IDC_BuyOption1, c_BuyOption1);
	DDX_Control(pDX, IDC_BuyOption2, c_BuyOption2);
	DDX_Control(pDX, IDC_BuyOption3, c_BuyOption3);
	DDX_Control(pDX, IDC_Strategy, c_Strategy);
	DDX_CBString(pDX, IDC_Strategy, m_Strategy);
	DDV_MaxChars(pDX, m_Strategy, 24);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( CStrategyExplorer, CNillaDialog )
	//{{AFX_MSG_MAP( CStrategyExplorer )
	ON_CBN_SELCHANGE(IDC_Strategy, OnSelChangeStrategy)
	ON_BN_CLICKED(IDC_BuyOption1, OnBuyOption1)
	ON_BN_CLICKED(IDC_BuyOption2, OnBuyOption2)
	ON_BN_CLICKED(IDC_BuyOption3, OnBuyOption3)
	ON_BN_CLICKED(IDC_BuyStock, OnBuyStock)
	ON_BN_CLICKED(IDC_SellOption1, OnSellOption1)
	ON_BN_CLICKED(IDC_SellOption2, OnSellOption2)
	ON_BN_CLICKED(IDC_SellOption3, OnSellOption3)
	ON_BN_CLICKED(IDC_SellStock, OnSellStock)
	ON_BN_CLICKED(IDC_Option1Call, OnOption1Call)
	ON_BN_CLICKED(IDC_Option2Call, OnOption2Call)
	ON_BN_CLICKED(IDC_Option3Call, OnOption3Call)
	ON_BN_CLICKED(IDC_Option1Put, OnOption1Put)
	ON_BN_CLICKED(IDC_Option2Put, OnOption2Put)
	ON_BN_CLICKED(IDC_Option3Put, OnOption3Put)
	ON_EN_KILLFOCUS(IDC_Option1Cost, OnKillFocusOption1Cost)
	ON_EN_KILLFOCUS(IDC_Option2Cost, OnKillFocusOption2Cost)
	ON_EN_KILLFOCUS(IDC_Option3Cost, OnKillFocusOption3Cost)
	ON_EN_KILLFOCUS(IDC_StockPrice, OnKillFocusStockPrice)
	ON_EN_KILLFOCUS(IDC_Option1Qty, OnKillFocusOption1Qty)
	ON_EN_KILLFOCUS(IDC_Option2Qty, OnKillFocusOption2Qty)
	ON_EN_KILLFOCUS(IDC_Option3Qty, OnKillFocusOption3Qty)
	ON_EN_KILLFOCUS(IDC_StockQty, OnKillFocusStockQty)
	ON_EN_KILLFOCUS(IDC_Option1Strike, OnKillFocusOption1Strike)
	ON_EN_KILLFOCUS(IDC_Option2Strike, OnKillFocusOption2Strike)
	ON_EN_KILLFOCUS(IDC_Option3Strike, OnKillFocusOption3Strike)
	ON_CBN_KILLFOCUS(IDC_Strategy, OnKillFocusStrategy)
	ON_WM_PAINT()
	ON_CBN_SELENDOK(IDC_Strategy, OnSelEndOkStrategy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CString		strategyNames[] =
{		// Option only strategies - Bullish
	_T("Synthetic Long Stock"),
	_T("Bull Call Spread"),	
	_T("Bull Put Spread"),

		// Option only strategies - Bearish
	_T("Synthetic Short Stock"),
	_T("Bear Call Spread"),
	_T("Bear Put Spread"),

		// Option only strategies - Neutral, Low Volatility
	_T("Short Straddle"),				// win on small moves, lose on big moves
	_T("Butterfly Call Spread"),		// win on small moves capped losses
	_T("Butterfly Put Spread"),			// win on small moves capped losses (identical results)

		// Option only strategies - Neutral, High Volatility
	_T("Long Straddle"),				// win on big moves, lose on small moves
	_T("Long Strangle"),				// win on really big moves, lose on everything else
	_T("Short Strangle"),				// lose on really big moves, win on everything else

		// the rest of the strategies involve a stock position (short or long)

		// Covered
	_T("Covered Call"),				// _T("Synthetic Short Put")	capped bullish	(long stock)
	_T("Covered Put"),				// _T("Synthetic Short Call")	capped bearish	(short stock)

		// Protectie
	_T("Protective Put"),			// _T("Synthetic Long Call")	stop loss bullish	(long stock)
	_T("Protective Call"),			// _T("Synthetic Long Put")		stop loss bearish	(short stock)

		// Bracketed
	_T("Collar"),					// capped bullish (long stock)
	_T("Fence"),					// capped bearish (short stock)

		// Arbitrage
	_T("Conversion"),				// SynShortStock + Long Stock (to profit from option overpricing)
	_T("Reversal")					// SynLongStock + Short Stock (to profit from option underpricing)

//	_T("Risk Reversal")				// Wilmott's definition (p.45 red book)
};

const	unsigned short	nStrategies = 20;

//----------------------------------------------------------------------------
void	CStrategyExplorer::killFocusStrategyWork( void )
{		// Strategies are keyed by name, but characterized by a unique
		// combination of Qty, Put/Call (for the Options in the Strategy) & Buy/Sell
		// If we assume Qty is limited to 0 or 1, (not counting expiration date
		// variations) there are (2 * 2 * 2)^3 * 2 * 2 = 2048 possible strategies
		// involving 3 options and 1 stock
	CString sym = c_Strategy.GetText();
	if ( sym == m_Strategy )
		return;
	m_Strategy = sym;

	inhibitEval = true;
	if (  m_Strategy != _T("Previous Settings...") )
	{		// disable existing positions, strategy functions will turn on the needed positions
		m_Qty[0] = 0.0;
		setEditBox( "%g", m_Qty[0], IDC_Option1Qty );
		m_Qty[1] = 0.0;
		setEditBox( "%g", m_Qty[1], IDC_Option2Qty );
		m_Qty[2] = 0.0;
		setEditBox( "%g", m_Qty[2], IDC_Option3Qty );
		m_Qty[3] = 0.0;
		setEditBox( "%g", m_Qty[3], IDC_StockQty );
	}

		// indicate selection of a Strategy by selecting Put/Call & Buy/Sell buttons
		// and Qty, but leaving the magnitudes to the user
	if ( m_Strategy == _T("Synthetic Long Stock") )
			// a short Put and a long Call
		SyntheticLongStock();
	else if ( m_Strategy == _T("Synthetic Short Stock") )
			// a short Call and a long Put
		SyntheticShortStock();

	else if ( m_Strategy == _T("Bull Call Spread") )
			// Buy a lower strike call and sell a higher strike call
		BullCallSpread();
	else if ( m_Strategy == _T("Bull Put Spread") )
			// Sell a high strike put and buy a lower strike put
		BullPutSpread();

	else if ( m_Strategy == _T("Bear Call Spread") )
			// Sell a lower strike call and buy a higher strike call
		BearCallSpread();
	else if ( m_Strategy == _T("Bear Put Spread") )
			// Sell a lower strike put and buy a higher strike put
		BearPutSpread();

	else if ( m_Strategy == _T("Long Straddle") )
			// long Call and a long Put
		LongStraddle();		
	else if ( m_Strategy == _T("Short Straddle") )
			// short Call and a short Put
		ShortStraddle();

	else if ( m_Strategy == _T("Long Strangle") )
			// long Call and a long Put
		LongStrangle();
	else if ( m_Strategy == _T("Short Strangle") )
			// short Call and a short Put
		ShortStrangle();

	else if ( m_Strategy == _T("Butterfly Call Spread") )
		ButterflyCallSpread();
	else if ( m_Strategy == _T("Butterfly Put Spread") )
		ButterflyPutSpread();
	
		// the rest require a stock position (short or long)

	else if ( m_Strategy == _T("Protective Put") )
			// a long Put and a long stock or future
		ProtectivePut();
	else if ( m_Strategy == _T("Protective Call") )
			// a long Call and a short stock or future
		ProtectiveCall();
	else if ( m_Strategy == _T("Covered Call") )
			// a short Call and a long stock or future
		CoveredCall();
	else if ( m_Strategy == _T("Covered Put") )
			// a short Put and a short stock or future
		CoveredPut();

	else if ( m_Strategy == _T("Collar") )
		Collar();										// long OTM put, long underlying, short OTM call
	else if ( m_Strategy == _T("Fence") )
		Fence();										// short OTM put, short underlying, long OTM call

	else if ( m_Strategy == _T("Conversion") )
		Conversion();						
	else if ( m_Strategy == _T("Reversal") )				
		Reversal();							

		// Risk Reversals don't require a stock position, but
		// they don't serve much of an investment purpose either
//	else if ( m_Strategy == _T("Risk Reversal") )
//		RiskReversal();									// Long OTM Call & Short OTM Put


		// etc. for each of the strategyNames
	inhibitEval = false;
	EvalStrategy();
}			// killFocusStrategyWork()
//---------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
// Other CStrategyExplorer message handlers ...

BOOL	CStrategyExplorer::OnInitDialog( void )
{
		// set the extents of plot region (among other initialization)
	CPlotDialog::OnInitDialog();		// was CNillaDialog::OnInitDialog()
	plotExtents.top = 122;		// leaves enough room for the controls at the top

		// restore the most recent strategy settings
	OneStk3OptStrategy	os3os;
	bool	savedStrategyExists = theApp.pDB->getRecentStrategy( os3os );
	for ( short ii = 0; ii < 4; ii++ )
	{	m_BuySell[ii] = savedStrategyExists  ?  os3os.buySell[ii]  :  UnknownBuySellStatus;
		m_Cost[ii]	  = savedStrategyExists  ?  os3os.cost[ii]	   :  0.0f;
		m_Qty[ii]	  = savedStrategyExists  ?  os3os.qty[ii]	   :  0.0f;
		if ( ii < 3 )
		{	m_PutCall[ii] = savedStrategyExists  ?  os3os.putCall[ii]  :  UnknownOptionType;
			m_Strike[ii]  = savedStrategyExists  ?  os3os.strike[ii]   :  0.0f;
		}
	}
		
		// insert the strategy names in the Strategy combo-box
	for ( ii = 0; ii < nStrategies; ii++ )
		c_Strategy.AddString( strategyNames[ii] );

		// load up controls from initialized values
		// Buy/Sell buttons ...
	if ( m_BuySell[0] != UnknownBuySellStatus )
	{	c_BuyOption1.SetCheck( m_BuySell[0] == Buy ? BST_CHECKED : BST_UNCHECKED );
		c_SellOption1.SetCheck( m_BuySell[0] == Sell ? BST_CHECKED : BST_UNCHECKED );
	}
	if ( m_BuySell[1] != UnknownBuySellStatus )
	{	c_BuyOption2.SetCheck( m_BuySell[1] == Buy ? BST_CHECKED : BST_UNCHECKED );
		c_SellOption2.SetCheck( m_BuySell[1] == Sell ? BST_CHECKED : BST_UNCHECKED );
	}
	if ( m_BuySell[2] != UnknownBuySellStatus )
	{	c_BuyOption3.SetCheck( m_BuySell[2] == Buy ? BST_CHECKED : BST_UNCHECKED );
		c_SellOption3.SetCheck( m_BuySell[2] == Sell ? BST_CHECKED : BST_UNCHECKED );
	}
	if ( m_BuySell[3] != UnknownBuySellStatus )
	{	c_BuyStock.SetCheck( m_BuySell[3] == Buy ? BST_CHECKED : BST_UNCHECKED );
		c_SellStock.SetCheck( m_BuySell[3] == Sell ? BST_CHECKED : BST_UNCHECKED );
	}
		// Put/Call buttons ...
	if ( m_PutCall[0] != UnknownOptionType )
	{	CButton* c_Option1Call = (CButton*)GetDlgItem( IDC_Option1Call );
		c_Option1Call->SetCheck( m_PutCall[0] == Call ? BST_CHECKED : BST_UNCHECKED );
	
		CButton* c_Option1Put = (CButton*)GetDlgItem( IDC_Option1Put );
		c_Option1Put->SetCheck( m_PutCall[0] == Put ? BST_CHECKED : BST_UNCHECKED );
	}
	if ( m_PutCall[1] != UnknownOptionType )
	{	CButton* c_Option2Call = (CButton*)GetDlgItem( IDC_Option2Call );
		c_Option2Call->SetCheck( m_PutCall[1] == Call ? BST_CHECKED : BST_UNCHECKED );
	
		CButton* c_Option2Put = (CButton*)GetDlgItem( IDC_Option2Put );
		c_Option2Put->SetCheck( m_PutCall[1] == Put ? BST_CHECKED : BST_UNCHECKED );
	}
	if ( m_PutCall[2] != UnknownOptionType )
	{	CButton* c_Option3Call = (CButton*)GetDlgItem( IDC_Option3Call );
		c_Option3Call->SetCheck( m_PutCall[2] == Call ? BST_CHECKED : BST_UNCHECKED );
	
		CButton* c_Option3Put = (CButton*)GetDlgItem( IDC_Option3Put );
		c_Option3Put->SetCheck( m_PutCall[2] == Put ? BST_CHECKED : BST_UNCHECKED );
	}

		// Qty EditBoxes ...
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );
	setEditBox( "%g", m_Qty[2], IDC_Option3Qty );
	setEditBox( "%g", m_Qty[3], IDC_StockQty );

		// Cost EditBoxes ...
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );
	setEditBox( "%g", m_Cost[2], IDC_Option3Cost );
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );	

		// Strike EditBoxes ...
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );	
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );	
	setEditBox( "%g", m_Strike[2], IDC_Option3Strike );	

	inhibitEval = false;

		// handle saved strategy or default to first strategy in the list

		// it used to be good enough to just EvalStrategy at startup, but since
		// killing off the DocList stuff in InitInstance(), the background didn't get
		// an initial paint message and EvalStrategy wouldn't generate the necessary
		// Paint() events to cause the plot area to paint (ever), so it became
		// necessary to forcably select a strategy
	if ( savedStrategyExists )
	{	c_Strategy.SetWindowText( _T("Previous Settings...") );
		EvalStrategy();		
	}
	else
	{	c_Strategy.SetCurSel( 0 );
		OnSelChangeStrategy();
	}
	return	TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//						Strategy ComboBox messages							//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorer::BearCallSpread( void )
{		// http://biz.yahoo.com/opt/glossary1.html  says ...
		// Involves selling a lower strike call and buying a higher strike call
		// Has limited profit and limited risk.  The spread increases in value
		// when the underlying falls.  Net credit transaction;
		// Maximum loss = difference between the strike prices less credit;
		// Maximum gain = credit;
		// Requires margin.
	OnSellOption1();											// short position in Option1
	OnOption1Call();											// Option1 is a Call
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 20.0;											// low strike = 20.0
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );
	
	m_Cost[0] = (float)6.0;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnBuyOption2();												// long position in Option2
	OnOption2Call();											// Option2 is a Call
	m_Qty[1] = 1.0;
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );

	m_Strike[1] = 30.0;											// high strike = 30.0
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

	m_Cost[1] = (float)1.0;
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );


	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
}			// BearCallSpread()
//----------------------------------------------------------------------------
void	CStrategyExplorer::BearPutSpread( void )
{		// http://biz.yahoo.com/opt/glossary1.html  says ...
		// Involves selling a lower strike put and buying a higher strike put
		// Has limited profit and limited risk. The spread increases in value
		// when the underlying falls.  Net debit transaction;
		// Maximum loss = difference between strike prices less the debit;
		// No margin.
	OnSellOption1();											// short position in Option1
	OnOption1Put();												// Option1 is a Put
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 20.0;											// low strike = 20.0
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)1.0;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnBuyOption2();												// long position in Option2
	OnOption2Put();												// Option2 is a Put
	m_Qty[1] = 1.0;
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );

	m_Strike[1] = 30.0;											// high strike = 30.0
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

	m_Cost[1] = (float)6.0;
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );


	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
}			// BearPutSpread()
//----------------------------------------------------------------------------
void	CStrategyExplorer::BullCallSpread( void )
{		// http://biz.yahoo.com/opt/glossary1.html  says ...
		// Involves buying a lower strike call and selling a higher strike call
		// Has limited profit and limited risk. The spread increases in value
		// when the underlying rises. Net debit transaction;
		// Maximum loss = debit;
		// Maximum gain = difference between strike prices less the debit;
		// No margin.
	OnBuyOption1();											// long position in Option1
	OnOption1Call();										// Option1 is a Call
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 20.0;										// low strike = 20.0
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)6.0;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnSellOption2();										// short position in Option2
	OnOption2Call();										// Option2 is a Call
	m_Qty[1] = 1.0;
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );

	m_Strike[1] = 30.0;										// high strike = 30.0
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

	m_Cost[1] = (float)1.0;
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );


	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
}			// BullCallSpread()
//----------------------------------------------------------------------------
void	CStrategyExplorer::BullPutSpread( void )
{		// http://biz.yahoo.com/opt/glossary1.html  says ...
		// Involves buying a low strike put and selling a higher strike put
		// Has limited profit and limited risk. The spread increases in value
		// when the underlying rises. Net credit transaction;
		// Maximum loss = difference between strike prices less credit;
		// Maximum gain = credit;
		// Requires margin.
	OnBuyOption1();											// long position in Option1
	OnOption1Put();											// Option1 is a Put
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 20.0;										// low strike = 20.0
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)1.0;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnSellOption2();										// short position in Option2
	OnOption2Put();											// Option2 is a Put
	m_Qty[1] = 1.0;
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );

	m_Strike[1] = 30.0;										// high strike = 30.0
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

	m_Cost[1] = (float)6.0;
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );


	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
}			// BullPutSpread()
//----------------------------------------------------------------------------
void	CStrategyExplorer::ButterflyCallSpread( void )
{		// p.190 John Hull's Options, Futures and Other Derivatives, Fifth Ed.
		// Involves buying a low strike Call, selling two mid strike Calls, and buying a high strike Call
	OnBuyOption1();											// long position in Option1
	OnOption1Call();										// Option1 is a Call
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 20.0;										// lowest strike = 20.0
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)6.0;										// a relatively high cost option
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnSellOption2();										// short position in Option2
	OnOption2Call();										// Option2 is a Call
	m_Qty[1] = 2.0;											// two identical contracts
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );

	m_Strike[1] = 25.0;										// mid strike = 25.0
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

	m_Cost[1] = (float)2.0;								// mid cost options
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );


	OnBuyOption3();											// long position in Option3
	OnOption3Call();										// Option3 is a Call
	m_Qty[2] = 1.0;											// two identical contracts
	setEditBox( "%g", m_Qty[2], IDC_Option3Qty );

	m_Strike[2] = 30.0;										// high strike = 30.0
	setEditBox( "%g", m_Strike[2], IDC_Option3Strike );

	m_Cost[2] = (float)1.0;								// lowest cost option
	setEditBox( "%g", m_Cost[2], IDC_Option3Cost );


	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
}			// ButterflyCallSpread()
//----------------------------------------------------------------------------
void	CStrategyExplorer::ButterflyPutSpread( void )
{		// p.190 John Hull's Options, Futures and Other Derivatives, Fifth Ed.
		// Involves buying a low strike Put, selling two mid strike Puts, and buying a high strike Put
	OnBuyOption1();											// long position in Option1
	OnOption1Put();											// Option1 is a Put
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 20.0;										// lowest strike = 20.0
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)1.0;								// a relatively high cost option
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnSellOption2();										// short position in Option2
	OnOption2Put();											// Option2 is a Put
	m_Qty[1] = 2.0;											// two identical contracts
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );

	m_Strike[1] = 25.0;										// mid strike = 25.0
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

	m_Cost[1] = (float)2.0;								// mid cost options
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );


	OnBuyOption3();											// long position in Option3
	OnOption3Put();											// Option3 is a Put
	m_Qty[2] = 1.0;											// two identical contracts
	setEditBox( "%g", m_Qty[2], IDC_Option3Qty );

	m_Strike[2] = 30.0;										// high strike = 30.0
	setEditBox( "%g", m_Strike[2], IDC_Option3Strike );

	m_Cost[2] = (float)6.0;										// lowest cost option
	setEditBox( "%g", m_Cost[2], IDC_Option3Cost );


	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
}			// ButterflyPutSpread()
//----------------------------------------------------------------------------
void	CStrategyExplorer::Collar( void )
{		// long put, long stock, short call
		// http://www.interactivebrokers.com/php/marginRequirements/stockIndexOptions.php?ib_entity=
	OnBuyOption1();											// long position in Option1
	OnOption1Put();											// Option1 is a Put
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 20.0;										// low strike = 20.0
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)1.0;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnSellOption2();										// short position in Option2
	OnOption2Call();										// Option2 is a Call
	m_Qty[1] = 1.0;
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );

	m_Strike[1] = 30.0;										// high strike = 30.0
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

	m_Cost[1] = (float)1.0;
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );


	OnBuyStock();											// long position in the Stock
	m_Qty[3] = 1.0;
	setEditBox( "%g", m_Qty[3], IDC_StockQty );

	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
}			// Collar()
//----------------------------------------------------------------------------
void	CStrategyExplorer::Fence( void )
{		// long put, long stock, short call
		// http://www.interactivebrokers.com/php/marginRequirements/stockIndexOptions.php?ib_entity=
	OnSellOption1();										// short position in Option1
	OnOption1Put();											// Option1 is a Put
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 20.0;										// low strike = 20.0
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)1.0;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnBuyOption2();											// long position in Option2
	OnOption2Call();										// Option2 is a Call
	m_Qty[1] = 1.0;
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );

	m_Strike[1] = 30.0;										// high strike = 30.0
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

	m_Cost[1] = (float)1.0;
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );


	OnSellStock();											// short position in the Stock
	m_Qty[3] = 1.0;
	setEditBox( "%g", m_Qty[3], IDC_StockQty );

	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
}			// Fence()
//----------------------------------------------------------------------------
void	CStrategyExplorer::CoveredCall( void )			// a.k.a. Synthetic Short Put
{		// long stock, short call (same as a SyntheticShortPut)
		// see:  p.186 of John Hull's Options, Futures & Other Derivatives
	OnSellOption1();										// short position in Option1
	OnOption1Call();										// Option1 is a Call
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 27.0;										// default strike = 27.0
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)1.0;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnBuyStock();											// long position in the Stock
	m_Qty[3] = 1.0;
	setEditBox( "%g", m_Qty[3], IDC_StockQty );

	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
}			// CoveredCall()
//----------------------------------------------------------------------------
void	CStrategyExplorer::CoveredPut( void )			// a.k.a. SyntheticShortCall
{		// a short Put and a short stock or future
		// see:  http://biz.yahoo.com/opt/glossary5.html
	OnSellOption1();										// short position in Option1
	OnOption1Put();											// Option1 is a Put
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 23.0;										// defaul strike = 23.0
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)1.0;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnSellStock();											// short position in the Stock
	m_Qty[3] = 1.0;
	setEditBox( "%g", m_Qty[3], IDC_StockQty );

	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
}			// CoveredPut()
//---------------------------------------------------------------------------
void	CStrategyExplorer::ProtectivePut( void )		// a.k.a. SyntheticLongCall
{		// long stock, long Put
		// see:  p.186 of John Hull's Options, Futures & Other Derivatives
	OnBuyOption1();											// long position in Option1
	OnOption1Put();											// Option1 is a Put
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 23.0;										// default strike = 23.0
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)1.0;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnBuyStock();											// long position in the Stock
	m_Qty[3] = 1.0;
	setEditBox( "%g", m_Qty[3], IDC_StockQty );

	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
}			// ProtectivePut()
//----------------------------------------------------------------------------
void	CStrategyExplorer::ProtectiveCall( void )		// a.k.a. SyntheticLongPut
{		// a long Call and a short stock or future
		// see:  http://biz.yahoo.com/opt/glossary5.html
	OnBuyOption1();											// long position in Option1
	OnOption1Call();										// Option1 is a Call
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );
	
	m_Strike[0] = 27.0;									// default strike = 27.0
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)1.0;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnSellStock();											// long position in the Stock
	m_Qty[3] = 1.0;
	setEditBox( "%g", m_Qty[3], IDC_StockQty );

	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
}			// ProtectiveCall()
//---------------------------------------------------------------------------
void	CStrategyExplorer::LongStraddle( void )
{		// A long Call and a long Put, where
		// both options have the same strike price and expiration date.
		// see:  http://biz.yahoo.com/opt/glossary5.html
		// a Straddle is a special case of a Strangle where the strikes are the current stock price
		// a Volatility play
	OnBuyOption1();											// long position in Option1
	OnOption1Call();										// Option1 is a Call
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 25.0;										// same strike as the Put
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)1.0;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnBuyOption2();											// long position in Option2
	OnOption2Put();											// Option2 is a Put
	m_Qty[1] = 1.0;
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );
	
	m_Strike[1] = 25.0;										// same strike as the Call
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

	m_Cost[1] = (float)1.0;
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );


	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
	// ??? set up state variables that cause
	// Option1 strike and daysToExpiry to move in lockstep with 
	// Option2 strike and daysToExpiry
	// then insert those controls into message functions for strike & expiry ...
}			// LongStraddle()
//---------------------------------------------------------------------------
void	CStrategyExplorer::ShortStraddle( void )
{		// A short Call and a short Put, where
		// both options have the same strike price and expiration date.
		// see:  http://biz.yahoo.com/opt/glossary5.html
		// a Straddle is a special case of a Strangle where the strikes are the current stock price
		// a Volatility play
	OnSellOption1();										// short position in Option1
	OnOption1Call();										// Option1 is a Call
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 25.0;										// same strike as the Put
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)1.0;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnSellOption2();										// short position in Option2
	OnOption2Put();											// Option2 is a Put
	m_Qty[1] = 1.0;
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );

	m_Strike[1] = 25.0;										// same strike as the Call
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

	m_Cost[1] = (float)1.0;
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );


	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
	// ??? set up state variables that cause
	// Option1 strike and daysToExpiry to move in lockstep with 
	// Option2 strike and daysToExpiry
	// then insert those controls into message functions for strike & expiry ...
}			// ShortStraddle()
//---------------------------------------------------------------------------
void	CStrategyExplorer::LongStrangle( void )
{		// A long Call and a long Put where
		// both options have the same underlying, the same expiration date.
		// A strangle is similar to a straddle, but both options are struck out of the money.
		// state variables would prevent going into the money
		// (requires a stockPrice be set or adjustment when it is set)
	OnBuyOption1();											// long position in Option1
	OnOption1Put();											// Option1 is a Put
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 20.0;										// lower strike than the Call
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );
	
	m_Cost[0] = (float)1.0;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnBuyOption2();											// long position in Option2
	OnOption2Call();										// Option2 is a Call
	m_Qty[1] = 1.0;
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );

	m_Strike[1] = 30.0;										// higher strike than the Put
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

	m_Cost[1] = (float)1.0;
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );


	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
}			// LongStrangle()
//---------------------------------------------------------------------------
void	CStrategyExplorer::ShortStrangle( void )
{		// A short Call and a short Put where
		// both options have the same underlying, the same expiration date, but
		// different strike prices. Most strangles involve OTM options.
		// A strangle is similar to a straddle, but both options are struck out of the money.
	OnSellOption1();										// short position in Option1
	OnOption1Put();											// Option1 is a Put
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 20.0;										// lower strike than the Call
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)1.0;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnSellOption2();										// short position in Option2
	OnOption2Call();										// Option2 is a Call
	m_Qty[1] = 1.0;
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );

	m_Strike[1] = 30.0;										// higher strike than the Put
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

	m_Cost[1] = (float)1.0;
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );


	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
}			// ShortStrangle()
//---------------------------------------------------------------------------
/*
void	CStrategyExplorer::RiskReversal( void )
{		// a long Put and a long stock or future
		// see:  Paul Wilmott on Quantitative Finance (Red volume) p.45 
	OnSellOption1();										// short position in Option1
	OnOption1Put();											// Option1 is a Put
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 20.0;
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = 1.0f;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );

	OnBuyOption2();											// long position in Option2
	OnOption2Call();										// Option2 is a Call
	m_Qty[1] = 1.0;
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );

	m_Strike[1] = 30.0;
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

	m_Cost[1] = 1.0f;
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );

		// Stock price should be between the options
	m_Cost[3] = 25.0f;
}			// RiskReversal()
*/
//---------------------------------------------------------------------------
void	CStrategyExplorer::Conversion( void )
{		// a long stock and a synthetic short stock (using overpriced options)
		// an arbitrage strategy used by floor traders to profit from option overpricing
		// see:  http://www.optionsxpress.com/educate/strategies/neutral.aspx?sessionid=
//	SyntheticShortStock();

	OnSellOption1();										// short position in Option1
	OnOption1Call();										// Option1 is a Call
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 25.0;
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)1.5;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnBuyOption2();											// long position in Option2
	OnOption2Put();											// Option2 is a Put
	m_Qty[1] = 1.0;
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );

	m_Cost[1] = (float)2.0;
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );

	m_Strike[1] = 25.0;
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

		// options are at the money
	m_Cost[3] = 24.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );


	OnBuyStock();											// long position in the Stock
	m_Qty[3] = 1.0;
	setEditBox( "%g", m_Qty[3], IDC_StockQty );
}			// Conversion()
//---------------------------------------------------------------------------
void	CStrategyExplorer::SyntheticShortStock( void )
{		// a short Call and a long Put
		// see:  http://biz.yahoo.com/opt/glossary5.html
	OnSellOption1();										// short position in Option1
	OnOption1Call();										// Option1 is a Call
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 25.0;
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)2.0;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnBuyOption2();											// long position in Option2
	OnOption2Put();											// Option2 is a Put
	m_Qty[1] = 1.0;
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );

	m_Cost[1] = (float)2.0;
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );

	m_Strike[1] = 25.0;
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

		// options are at the money
	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
}			// SyntheticShortStock()
//---------------------------------------------------------------------------
void	CStrategyExplorer::Reversal( void )
{		// a short stock and a synthetic long stock
		// an arbitrage strategy used by floor traders to profit from option underpricing
		// see:  http://www.optionsxpress.com/educate/strategies/neutral.aspx?sessionid=
//	SyntheticLongStock();

	OnSellOption1();										// short position in Option1
	OnOption1Put();											// Option1 is a Put
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 25.0;
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)1.5;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnBuyOption2();											// long position in Option2
	OnOption2Call();										// Option2 is a Call
	m_Qty[1] = 1.0;
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );

	m_Strike[1] = 25.0;
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

	m_Cost[1] = (float)2.0;
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );

		// options are at the money
	m_Cost[3] = 26.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );


	OnSellStock();											// short position in the Stock
	m_Qty[3] = 1.0;
	setEditBox( "%g", m_Qty[3], IDC_StockQty );
}			// Reversal()
//---------------------------------------------------------------------------
void	CStrategyExplorer::SyntheticLongStock( void )
{		// a short Put and a long Call
		// see:  http://biz.yahoo.com/opt/glossary5.html
	OnSellOption1();										// short position in Option1
	OnOption1Put();											// Option1 is a Put
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	m_Strike[0] = 25.0;
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );

	m_Cost[0] = (float)2.0;
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );


	OnBuyOption2();											// long position in Option2
	OnOption2Call();										// Option2 is a Call
	m_Qty[1] = 1.0;
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );

	m_Strike[1] = 25.0;
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );

	m_Cost[1] = (float)2.0;
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );

		// options are at the money
	m_Cost[3] = 25.0;
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
}			// SyntheticLongStock()
//--------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////
//							Control messages								//
//////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------//
void	CStrategyExplorer::OnSelChangeStrategy( void ) 
{
	killFocusStrategyWork();
}			// OnSelChangeStrategy()
//---------------------------------------------------------------------------
/*
void	CStrategyExplorer::OnEditChangeStrategy( void )
{
	seekComboBoxEntry( IDC_Strategy );
}			// OnEditChangeStrategy()
*/
//---------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusStrategy() 
{
	killFocusStrategyWork();
}
//---------------------------------------------------------------------------
void CStrategyExplorer::OnSelEndOkStrategy( void )
{
	killFocusStrategyWork();
}			// OnSelEndOkStrategy()
//----------------------------------------------------------------------------
void	CStrategyExplorer::MapToPlot( CFPoint ptIn, CPoint& ptOut )
{		// uses dialog globals:  origin, xScale, yScale
	ptOut.x = plotExtents.left + (short)((ptIn.x - topLeft.x) * xScale);
		// recall that +y is down in MM_TEXT mode
	ptOut.y = plotExtents.top + (short)((topLeft.y - ptIn.y) * yScale);
	if ( ptOut.x < plotExtents.left )
	{
#ifdef _DEBUG
		TRACE( _T("StrategyExplorer::MapToPlot: clipped x=%d to %d\n"),
			ptOut.x, plotExtents.left );
#endif
		ptOut.x = plotExtents.left;
	}
	if ( ptOut.x > plotExtents.right )
	{
#ifdef _DEBUG
		TRACE( _T("StrategyExplorer::MapToPlot: clipped x=%d to %d\n"),
			ptOut.x, plotExtents.right );
#endif
		ptOut.x = plotExtents.right;
	}
	if ( ptOut.y < plotExtents.top )
	{
#ifdef _DEBUG
		TRACE( _T("StrategyExplorer::MapToPlot: clipped y=%d to %d\n"),
			ptOut.y, plotExtents.top );
#endif
		ptOut.y = plotExtents.top;
	}
	if ( ptOut.y > plotExtents.bottom )
	{
#ifdef _DEBUG
		TRACE( _T("StrategyExplorer::MapToPlot: clipped y=%d to %d\n"),
			ptOut.y, plotExtents.bottom );
#endif
		ptOut.y = plotExtents.bottom;
	}
}			// MapToPlot()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Plot related								//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorer::EvalStrategy( void )
{		// gutted this - moving functionality into OnPaint() so we could handle Update events too
	InvalidateRect( plotExtents, FALSE );
	UpdateWindow();
}			// EvalStrategy()
//----------------------------------------------------------------------------
void CStrategyExplorer::OnPaint() 
{		// nothing to paint if we're in a critical region or there are no instruments to evaluate
	if ( inhibitEval  ||  ! CalcDomainExtents() )
	{	CPaintDC	dc( this );
		DrawPlotBackground( dc );								// blanks out any existing plots
		CNillaDialog::OnPaint();
		return;
	}
		// we have domain extents, so we can scale against available space and plot
	yScale = (float)(plotExtents.Height() / (topLeft.y - bottomRight.y));	// pixels / dollar
	xScale = (float)(plotExtents.Width()  / (bottomRight.x - topLeft.x));	// ditto
	if ( xScale < 0.0  ||  yScale < 0.0 )
	{
#ifdef _DEBUG
		TRACE( _T("StrategyExplorer::OnPaint: negative scale detected, xScale=%g, yScale=%g\n"),
			xScale, yScale );
#endif
		ASSERT( false );
	}

	CPaintDC	dc( this );				// device context for painting
	DrawChart( dc );
}			// OnPaint()
//----------------------------------------------------------------------------
void	CStrategyExplorer::DrawChart( CPaintDC& dc )
{
		// plot grid, stock, options
	DrawGrid( dc );
	CPolyline	pl;
	short	itemsDrawn = DrawStock( dc, pl );
	for ( short ii = 0; ii < 3; ii++ )
		itemsDrawn += DrawOption( ii, dc, pl );
			;

		// draw the solution Polyline
	if ( itemsDrawn > 1 )
	{	CPen		polyPen;
		COLORREF	polyColor  = RGB(0,0,0);				// black
		polyPen.CreatePen( PS_SOLID, 2, polyColor );
		CPen*		pOldPen = dc.SelectObject( &polyPen );
		const	unsigned short	nPts = pl.GetPointCount();
		for ( ii = 0; ii < nPts - 1; ii++ )
		{	CLineSeg* seg = pl.GetSegment( ii );
			if ( seg == NULL )
			{
#ifdef _DEBUG
				TRACE( _T("StrategyExplorer::DrawChart: received an unexpected NULL segment from the Polyline\n") );
#endif
				break;
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
		dc.SelectObject( pOldPen );
	}
	DrawGridLabels( dc );
}			// DrawChart()
//----------------------------------------------------------------------------
bool	CStrategyExplorer::InstComplete( short ii )
{		// ii is 0-based ...
	bool	res = m_BuySell[ii] != UnknownBuySellStatus
			  &&	 m_Cost[ii]	!= 0.0
			  &&	  m_Qty[ii]	!= 0.0;
	if ( ii < 3 )
		res = res  &&  m_Strike[ii] != 0.0  &&  m_PutCall[ii] != UnknownOptionType;	
	return	res;
}			// InstComplete()
//----------------------------------------------------------------------------
bool		CStrategyExplorer::CalcDomainExtents( void )
{	float	loStrike, hiStrike;
		// calculate the extents of the strike-profit domain
		// modify topLeft, bottomRight, xSc
		// return false to inhibit plotting (i.e. no plot is possible)
	loStrike = 1e38f;				// something really far positive
	hiStrike = - loStrike;			// something really far negative
	bool	complete[4];
	for ( short ii = 0; ii < 4; ii++ )
	{	complete[ii] = InstComplete(ii);
		if ( complete[ii] )
		{		// the stock price serves in place of a strike
			float	priceStrike	= ( ii < 3 ) ? m_Strike[ii] : m_Cost[3];
			loStrike  = priceStrike < loStrike   ?  priceStrike  :  loStrike;
			hiStrike  = priceStrike > hiStrike   ?  priceStrike  :  hiStrike;
		}
	}
	if ( loStrike > 1e37 )
		return	false;
		// center about the available strikes
	float	midPoint = (float)( (hiStrike + loStrike) / 2.0 );

		// set up X test points
	float	xTest[5];
	for ( ii = 0; ii < 3; ii++ )
		xTest[ii] = complete[ii]  ?  m_Strike[ii]  :  midPoint;		// if incomplete, renders the test point harmless

	xTest[3] = (float)(loStrike - midPoint * 0.15);					// left end
	xTest[4] = (float)(hiStrike + midPoint * 0.15);					// right end

	float	fyy, polyY, maxY, minY = 1e38f;
	maxY = -minY;
	for ( short tt = 0; tt < 5; tt++ )
	{		// for each test point
		polyY = 0.0;
		for ( ii = 0; ii < 4; ii++ )
		{		// for each complete Option
			if ( complete[ii] )
			{	if ( ii < 3 )
				{		// Options
					CFPoint	strikePt;
					fyy = OptionY( ii, xTest[tt], strikePt );		// accounts for Qty
					float	premium = strikePt.y;
					if ( premium < minY )	minY = premium;
					if ( premium > maxY )	maxY = premium;
				}
				else
				{		// Stocks
					fyy = (float)( (m_BuySell[3] == Sell  ?  -1.0  :  1.0)
						*		   (xTest[tt] - m_Cost[3]) * m_Qty[3]		);
					if ( fyy < minY )	minY = fyy;
					if ( fyy > maxY )	maxY = fyy;
				}
				polyY += fyy;
			}
		}
			// collect the min/max polyY values
		maxY = polyY > maxY  ?  polyY  :  maxY;
		minY = polyY < minY  ?  polyY  :  minY;
	}
	float	plotHeight = maxY - minY;
		// provide some plot space above (below) the max (min) values
	maxY += (float)( 0.05 * plotHeight );
	minY -= (float)( 0.15 * plotHeight );

		// we now have a real valued strike spread and Y-range
	topLeft.x = xTest[3];			// loStrike - 15% of plotHeight
	topLeft.y = maxY;
	bottomRight.x = xTest[4];		// hiStrike + 05% of plotHeight
	bottomRight.y = minY;
#ifdef _DEBUG
//	TRACE( _T("StrategyExplorer::CalcDomainExtents: topLeft=(%g,%g), bottomRight=(%g,%g)\n"),
//				topLeft.x, topLeft.y, bottomRight.x, bottomRight.y );
#endif
	return	true;
}			// CalcDomainExtents()
//----------------------------------------------------------------------------
void	CStrategyExplorer::DrawGrid( CPaintDC& dc ) 
{		// slightly lighter Manilla than in the PortfolioNavigator
	CBrush		brushBkgnd( bkgndColor );					// bkgndColor is a CStrategyExplorer const
	CBrush*		pOldBrush = dc.SelectObject( &brushBkgnd );	// for graphics (e.g. Rect) purposes

		// Create and select a thin gray pen.
	CPen		gridPen;
	gridPen.CreatePen( PS_SOLID, 1, gridColor );			// gridColor is a CStrategyExplorer const
	CPen*		pOldPen = dc.SelectObject( &gridPen );

		// Draw a thin gray rectangle filled with Manilla
		// CE mapping mode is always MM_TEXT, so +y is down
	dc.Rectangle( plotExtents );

	COLORREF	oldBkColor = dc.SetBkColor( bkgndColor );		// gridColor is a CStrategyExplorer const
	COLORREF	oldTextColor = dc.SetTextColor( textColor );

		// Draw horizontal grid lines
	double	incY = Discretize( (topLeft.y - bottomRight.y) / 8.0 );
	double	fyy = incY * ceil( bottomRight.y / incY );
	while ( topLeft.y > fyy  &&  fyy > bottomRight.y )
	{	bool	itsTheXaxis = ( fabs(fyy) < 1e-15 );
		short	yy = MapToPlotY( (float)fyy );
#ifdef _DEBUG
//		TRACE( _T("DrawGrid: fyy=%g, yy=%d\n"), fyy, yy );
#endif
		if ( itsTheXaxis )
		{		// the X-axis is visible, switch to a broader pen
			CPen	axisPen;
			axisPen.CreatePen( PS_SOLID, 2, axisColor );
			dc.SelectObject( &axisPen );
		}
		dc.MoveTo( plotExtents.left, yy );
		dc.LineTo( plotExtents.right, yy );
		if ( itsTheXaxis )						// put the gridPen back
			dc.SelectObject( &gridPen );
		
			// next pass
		fyy += incY;
	}

		// Draw vertical grid lines
	double	incX = Discretize( (bottomRight.x - topLeft.x) / 14.0 );
	double	fxx = incX * ceil ( topLeft.x / incX );
	while ( fxx < bottomRight.x )
	{	short	xx = MapToPlotX( (float)fxx );
#ifdef _DEBUG
//		TRACE( _T("DrawGrid: fxx=%g, xx=%d\n"), fxx, xx );
#endif
		dc.MoveTo( xx, plotExtents.top	  );
		dc.LineTo( xx, plotExtents.bottom );
			// next pass
		fxx += incX;
	}

		// Put back the old objects.
	dc.SelectObject( pOldBrush );
	dc.SelectObject( pOldPen );
}			// DrawGrid()
//----------------------------------------------------------------------------
void	CStrategyExplorer::DrawGridLabels( CPaintDC& dc ) 
{		// for text purposes
	COLORREF	oldBkColor = dc.SetBkColor( bkgndColor );		// gridColor is a CStrategyExplorer const
	COLORREF	oldTextColor = dc.SetTextColor( textColor );

		// Draw labels for horizontal grid lines equal to and above the X-axis
	double	incY = Discretize( (topLeft.y - bottomRight.y) / 8.0 );
	double	fyy = incY * floor( topLeft.y / incY );
	CPoint	horLabelBR;						// the bottom right corner of the text extent rectangle
	while ( fyy > bottomRight.y )
	{	if ( fabs(fyy) < 1e-15 )
			fyy = 0.0;
		short	yy = MapToPlotY( (float)fyy );
#ifdef _DEBUG
//		TRACE( _T("DrawGridLabels: fyy=%g, yy=%d\n"), fyy, yy );
#endif
		if ( plotExtents.top + 7 <= yy  &&  yy <= plotExtents.bottom - 7 )
		{	wchar_t wbuf[16];
			swprintf( wbuf, _T("%g"), fyy );
			CString	str = wbuf;
			CSize	cs = dc.GetTextExtent( str );
			CPoint	txtOrigin( plotExtents.left + 6, yy - 7 );
			horLabelBR = txtOrigin + cs;
			dc.ExtTextOut( txtOrigin.x, txtOrigin.y, ETO_OPAQUE, NULL, buf, NULL );
		}		
			// next pass
		fyy -= incY;
	}

		// Draw labels for vertical grid lines
	double	incX = Discretize( (bottomRight.x - topLeft.x) / 14.0 );
	double	fxx = incX * ceil( topLeft.x / incX );
#ifdef _DEBUG
//	TRACE( _T("DrawGridLabels: horLabelBR=(%d,%d), fxx=%g, incX=%g\n"),
//			horLabelBR.x, horLabelBR.y, fxx, incX );
#endif
	int	verLabelX = 0;
	while ( fxx < bottomRight.x )
	{	short	xx = plotExtents.left + (short)((fxx - topLeft.x) * xScale);
#ifdef _DEBUG
//		TRACE( _T("DrawGridLabels: fxx=%g, xx=%d\n"), fxx, xx );
#endif
			// skip the first vertical line label to avoid conflicts with Y-axis labels
		wchar_t wbuf[16];
		swprintf( wbuf, _T("%g"), fxx );
		CString	str = wbuf;
		CSize cs = dc.GetTextExtent( str );
		CPoint	txtOrigin = CPoint( xx - cs.cx / 2, plotExtents.bottom - 16 );
#ifdef _DEBUG
//		TRACE( _T("DrawGridLabels: fxx=%g, xx=%d, verLabelX=%d cs=(%d,%d), txtOrigin=(%d,%d)\n"),
//				fxx, xx, verLabelX, cs.cx, cs.cy, txtOrigin.x, txtOrigin.y );
#endif
		if (	    txtOrigin.x - 8 > verLabelX
			  &&  ( txtOrigin.x     > horLabelBR.x  ||  txtOrigin.y - 2 > horLabelBR.y ) )
		{	dc.ExtTextOut( txtOrigin.x, txtOrigin.y, ETO_OPAQUE, NULL, buf, NULL );
			verLabelX = txtOrigin.x + cs.cx;
		}
			// next pass
		fxx += incX;
	}

		// Put back the old objects.
	dc.SetTextColor( oldTextColor );
	dc.SetBkColor( oldBkColor );
}			// DrawGridLabels()
//----------------------------------------------------------------------------
short	CStrategyExplorer::DrawStock( CPaintDC& dc, CPolyline& pl )
{	if ( ! InstComplete(3) )
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
	{	float	buySell = (float)(m_BuySell[3] == Sell  ?  -1.0  :  1.0);				// the slope
			// the strategy polyline should not be clipped to the plot region
		fyy[ii] = polyY[ii] = (float)( buySell * (fxx[ii] - m_Cost[3]) * m_Qty[3] );
			// the stock line has to be clipped to the plot region
		if ( fyy[ii] < bottomRight.y )
		{	fyy[ii] = bottomRight.y;
			fxx[ii] = fyy[ii] / (m_Qty[3] * buySell) + m_Cost[3];
		}
		if ( fyy[ii] > topLeft.y )
		{	fyy[ii] = topLeft.y;
			fxx[ii] = fyy[ii] / (m_Qty[3] * buySell) + m_Cost[3];	
		}
	}
		// update the polyline
	CFPoint	rtPolyPt( polyX[0], polyY[0] );
	CFPoint	leftPolyPt( polyX[1], polyY[1] );

	short	nPts = pl.GetPointCount();
	pl.InsertPoint( leftPolyPt );
	pl.InsertPoint( rtPolyPt );
	if ( nPts != 0 )
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
short	CStrategyExplorer::DrawOption( short ii, CPaintDC& dc, CPolyline& pl )
{		// Draws the extrinsic profit line,
		// calls DrawBlackScholes() to plot the sum of the intrinsic and extrinsic values
		// idx is 0-based for compatibility with MFC variable names
		// idx based gates...
	if (	 m_PutCall[ii] == UnknownOptionType
		 ||  m_BuySell[ii] == UnknownBuySellStatus
		 ||  m_Cost[ii] == 0.0  ||  m_Qty[ii] == 0.0  ||  m_Strike[ii] == 0.0 )
		return	0;

	CPen		optionPen;
	COLORREF	optionColor = RGB( ii == 0 ? 255 : 127,
								   ii == 1 ? 255 : 127,
								   ii == 2 ? 255 : 127 );		// creates Red, Green, or Blue
	optionPen.CreatePen( PS_SOLID, 2, optionColor );
	CPen*	pOldPen = dc.SelectObject( &optionPen );

		// create CLineSegs and map to the plot region
		// horizontal segment for Calls starts at 0 and ends at strike
	float	slope = (float)(m_Qty[ii] * (m_BuySell[ii] == Buy  ?  1.0 : -1.0)
									  * (m_PutCall[ii] == Call ?  1.0 : -1.0) );
	if ( m_Qty[3] > 0.0 )
		slope /= m_Qty[3];										// rescale by stock Qty
	CFPoint	strikePt;
	float	fxx[2], fyy[2], polyX[2], polyY[2];
	fxx[0] = polyX[0] = topLeft.x;
	fxx[1] = polyX[1] = bottomRight.x;
	for ( short jj = 0; jj < 2; jj++ )
	{		// for each of the three points on the Option profit profile ...
			// the strategy polyline should not be clipped
		polyY[jj] = fyy[jj] = OptionY( ii, fxx[jj], strikePt );
			// the Option's profit line has to be clipped to the plot region
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

	short	nPts = pl.GetPointCount();
	pl.InsertPoint( leftPolyPt );
	pl.InsertPoint( strikePt );
	pl.InsertPoint( rtPolyPt );
	if ( nPts != 0 )
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
	return	1;
}			// DrawOption()
//---------------------------------------------------------------------------
float	CStrategyExplorer::OptionY( short ii, float fxx, CFPoint& strikePt )
{
	ASSERT ( ii < 3  &&  InstComplete(ii) );
	float	fyy = (float)(m_Cost[ii] * m_Qty[ii] * (m_BuySell[ii] == Buy ? -1.0 : 1.0));
//	bool haveStockQty = m_Qty[3] != 0.0;
//	if ( haveStockQty )
//		fyy /= m_Qty[3];						// downscale by StockQty
	strikePt = CFPoint( m_Strike[ii], fyy );
		// if we're on the horizontal section of the profit profile, so we're done ...
	if (	(fxx > m_Strike[ii]  &&  m_PutCall[ii] == Put)
		||	(fxx < m_Strike[ii]  &&  m_PutCall[ii] == Call) )
		return	fyy;		// anything above (below) a Put (Call) strike is a premium loss

		// we're on the profit diagonal
	float	slope = (float)(m_Qty[ii] * (m_BuySell[ii] == Buy		    ?  1.0 : -1.0)
									  * (m_PutCall[ii] == Call ?  1.0 : -1.0) );
		// dxx is the x displacement of interest; dxx > 0 for 
	float	dxx = fxx - strikePt.x;				// dxx > 0 for Calls; dxx < 0 for Puts
	fyy = slope * dxx + strikePt.y;
//	if ( haveStockQty )
//		fyy /= m_Qty[3];						// downscale by StockQty
	return	fyy;
}			// optionY()
//---------------------------------------------------------------------------
void	CStrategyExplorer::DrawPlotBackground( CPaintDC& dc ) 
{		// only called by OnPaint(), but identical to code within DrawGrid()
		// slightly lighter Manilla than in the PortfolioNavigator
	CBrush		brushBkgnd( bkgndColor );					// bkgndColor is a CStrategyExplorer const
	CBrush*		pOldBrush = dc.SelectObject( &brushBkgnd );	// for graphics (e.g. Rect) purposes

	CPen		gridPen;
	gridPen.CreatePen( PS_SOLID, 1, gridColor );			// gridColor is a CStrategyExplorer const
	CPen*		pOldPen = dc.SelectObject( &gridPen );

		// Draw a thin gray rectangle filled with Manilla
		// CE mapping mode is always MM_TEXT, so +y is down
	dc.Rectangle( plotExtents );
	dc.SelectObject( pOldBrush );
	dc.SelectObject( pOldPen );
}			// DrawPlotBackground()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//			Option1 messages (except Buy/Sell & Put/Call buttons)			//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusOption1Qty( void )
{
	float	ff = getEditBoxFloat( IDC_Option1Qty );
	if ( ff == m_Qty[0] )
		return;

	m_Qty[0] = (float)fabs(ff);
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );
	EvalStrategy();
}			// OnKillFocusOption1Qty()
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusOption1Strike( void )
{
	float	ff = getEditBoxFloat( IDC_Option1Strike );
	if ( ff == m_Strike[0] )
		return;

	m_Strike[0] = (float)fabs(ff);
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );
	EvalStrategy();
}			// OnKillFocusOption1Strike()
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusOption1Cost( void )
{
	float	ff = getEditBoxFloat( IDC_Option1Cost );
	if ( ff == m_Cost[0] )
		return;

	m_Cost[0] = (float)fabs(ff);
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );
	EvalStrategy();
}			// OnKillFocusOption1Cost()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//			Option2 messages (except Buy/Sell & Put/Call buttons)			//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusOption2Qty( void )
{
	float	ff = getEditBoxFloat( IDC_Option2Qty );
	if ( ff == m_Qty[1] )
		return;

	m_Qty[1] = (float)fabs(ff);
	setEditBox( "%g", m_Qty[1], IDC_Option2Qty );
	EvalStrategy();
}			// OnKillFocusOption2Qty()
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusOption2Strike( void )
{
	float	ff = getEditBoxFloat( IDC_Option2Strike );
	if ( ff == m_Strike[1] )
		return;

	m_Strike[1] = (float)fabs(ff);
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );
	EvalStrategy();
}			// OnKillFocusOption2Strike()
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusOption2Cost( void )
{
	float	ff = getEditBoxFloat( IDC_Option2Cost );
	if ( ff == m_Cost[1] )
		return;

	m_Cost[1] = (float)fabs(ff);
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );
	EvalStrategy();
}			// OnKillFocusOption2Cost()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//			Option3 messages (except Buy/Sell & Put/Call buttons)			//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusOption3Qty( void )
{
	float	ff = getEditBoxFloat( IDC_Option3Qty );
	if ( ff == m_Qty[2] )
		return;

	m_Qty[2] = (float)fabs(ff);
	setEditBox( "%g", m_Qty[2], IDC_Option3Qty );
	EvalStrategy();
}			// OnKillFocusOption3Qty()
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusOption3Strike( void )
{
	float	ff = getEditBoxFloat( IDC_Option3Strike );
	if ( ff == m_Strike[2] )
		return;

	m_Strike[2] = (float)fabs(ff);
	setEditBox( "%g", m_Strike[2], IDC_Option3Strike );
	EvalStrategy();
}			// OnKillFocusOption3Strike()
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusOption3Cost( void )
{
	float	ff = getEditBoxFloat( IDC_Option3Cost );
	if ( ff == m_Cost[2] )
		return;

	m_Cost[2] = (float)fabs(ff);
	setEditBox( "%g", m_Cost[2], IDC_Option3Cost );
	EvalStrategy();
}			// OnKillFocusOption3Cost()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//					Stock messages (except Buy/Sell buttons)				//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusStockQty( void )
{
	float	ff = getEditBoxFloat( IDC_StockQty );
	if ( ff == m_Qty[3] )
		return;

	m_Qty[3] = (float)fabs(ff);
	setEditBox( "%g", m_Qty[3], IDC_StockQty );
	EvalStrategy();
}			// OnKillFocusStockQty()
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusStockPrice( void )
{
	float	ff = getEditBoxFloat( IDC_StockPrice );
	if ( ff == m_Cost[3] )
		return;

	m_Cost[3] = (float)fabs(ff);
	setEditBox( "%g", m_Cost[3], IDC_StockPrice );
	EvalStrategy();
}			// OnKillFocusStockPrice()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Buy/Sell Buttons							//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnBuyOption1( void )
{
	if ( m_BuySell[0] == Buy )
		return;

	m_BuySell[0] = Buy;
	c_BuyOption1.SetCheck( BST_CHECKED );
	c_SellOption1.SetCheck( BST_UNCHECKED );

	EvalStrategy();
}			// OnBuyOption1()
//---------------------------------------------------------------------------
void	CStrategyExplorer::OnSellOption1( void )
{
	if ( m_BuySell[0] == Sell )
		return;

	m_BuySell[0] = Sell;
	c_BuyOption1.SetCheck( BST_UNCHECKED );
	c_SellOption1.SetCheck( BST_CHECKED );

	EvalStrategy();
}			// OnSellOption1()
//---------------------------------------------------------------------------
void	CStrategyExplorer::OnBuyOption2( void )
{
	if ( m_BuySell[1] == Buy )
		return;

	m_BuySell[1] = Buy;
	c_BuyOption2.SetCheck( BST_CHECKED );
	c_SellOption2.SetCheck( BST_UNCHECKED );

	EvalStrategy();
}			// OnBuyOption2()
//---------------------------------------------------------------------------
void	CStrategyExplorer::OnSellOption2( void )
{
	if ( m_BuySell[1] == Sell )
		return;

	m_BuySell[1] = Sell;
	c_BuyOption2.SetCheck( BST_UNCHECKED );
	c_SellOption2.SetCheck( BST_CHECKED );

	EvalStrategy();
}			// OnSellOption2()
//---------------------------------------------------------------------------
void	CStrategyExplorer::OnBuyOption3( void )
{
	if ( m_BuySell[2] == Buy )
		return;

	m_BuySell[2] = Buy;
	c_BuyOption3.SetCheck( BST_CHECKED );
	c_SellOption3.SetCheck( BST_UNCHECKED );

	EvalStrategy();
}			// OnBuyOption3()
//---------------------------------------------------------------------------
void	CStrategyExplorer::OnSellOption3( void )
{
	if ( m_BuySell[2] == Sell )
		return;

	m_BuySell[2] = Sell;
	c_BuyOption3.SetCheck( BST_UNCHECKED );
	c_SellOption3.SetCheck( BST_CHECKED );

	EvalStrategy();
}			// OnSellOption3()
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnBuyStock( void )
{
	if ( m_BuySell[3] == Buy )
		return;

	m_BuySell[3] = Buy;
	c_BuyStock.SetCheck( BST_CHECKED );
	c_SellStock.SetCheck( BST_UNCHECKED );

	EvalStrategy();
}			// OnBuyStock()
//---------------------------------------------------------------------------
void	CStrategyExplorer::OnSellStock( void )
{
	if ( m_BuySell[3] == Sell )
		return;

	m_BuySell[3] = Sell;
	c_BuyStock.SetCheck( BST_UNCHECKED );
	c_SellStock.SetCheck( BST_CHECKED );

	EvalStrategy();
}			// OnSellStock()
//---------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Put/Call Buttons							//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnOption1Call( void )
{
	if ( m_PutCall[0] == Call )
		return;

	m_PutCall[0] = Call;
	CButton* c_Option1Call = (CButton*)GetDlgItem( IDC_Option1Call );
	c_Option1Call->SetCheck( BST_CHECKED );

	CButton* c_Option1Put = (CButton*)GetDlgItem( IDC_Option1Put );
	c_Option1Put->SetCheck( BST_UNCHECKED );

	EvalStrategy();
}			// OnOption1Call()
//---------------------------------------------------------------------------
void	CStrategyExplorer::OnOption1Put( void )
{
	if ( m_PutCall[0] == Put )
		return;

	m_PutCall[0] = Put;
	CButton* c_Option1Call = (CButton*)GetDlgItem( IDC_Option1Call );
	c_Option1Call->SetCheck( BST_UNCHECKED );

	CButton* c_Option1Put = (CButton*)GetDlgItem( IDC_Option1Put );
	c_Option1Put->SetCheck( BST_CHECKED );

	EvalStrategy();
}			// OnOption1Put()
//---------------------------------------------------------------------------
void	CStrategyExplorer::OnOption2Call( void )
{
	if ( m_PutCall[1] == Call )
		return;

	m_PutCall[1] = Call;
	CButton* c_Option2Call = (CButton*)GetDlgItem( IDC_Option2Call );
	c_Option2Call->SetCheck( BST_CHECKED );

	CButton* c_Option2Put = (CButton*)GetDlgItem( IDC_Option2Put );
	c_Option2Put->SetCheck( BST_UNCHECKED );

	EvalStrategy();
}			// OnOption2Call()
//---------------------------------------------------------------------------
void	CStrategyExplorer::OnOption2Put( void )
{
	if ( m_PutCall[1] == Put )
		return;

	m_PutCall[1] = Put;
	CButton* c_Option2Call = (CButton*)GetDlgItem( IDC_Option2Call );
	c_Option2Call->SetCheck( BST_UNCHECKED );

	CButton* c_Option2Put = (CButton*)GetDlgItem( IDC_Option2Put );
	c_Option2Put->SetCheck( BST_CHECKED );

	EvalStrategy();
}			// OnOption2Put()
//---------------------------------------------------------------------------
void	CStrategyExplorer::OnOption3Call( void )
{
	if ( m_PutCall[2] == Call )
		return;

	m_PutCall[2] = Call;
	CButton* c_Option3Call = (CButton*)GetDlgItem( IDC_Option3Call );
	c_Option3Call->SetCheck( BST_CHECKED );

	CButton* c_Option3Put = (CButton*)GetDlgItem( IDC_Option3Put );
	c_Option3Put->SetCheck( BST_UNCHECKED );

	EvalStrategy();
}			// OnOption3Call()
//---------------------------------------------------------------------------
void	CStrategyExplorer::OnOption3Put( void )
{
	if ( m_PutCall[2] == Put )
		return;

	m_PutCall[2] = Put;
	CButton* c_Option3Call = (CButton*)GetDlgItem( IDC_Option3Call );
	c_Option3Call->SetCheck( BST_UNCHECKED );

	CButton* c_Option3Put = (CButton*)GetDlgItem( IDC_Option3Put );
	c_Option3Put->SetCheck( BST_CHECKED );

	EvalStrategy();
}			// OnOption3Put()
//----------------------------------------------------------------------------
/*
// same as CoveredCall()
void	CStrategyExplorer::SyntheticShortPut( void )
{		// a short Call and a long Stock
		// see:  http://biz.yahoo.com/opt/glossary5.html
	OnSellOption1();									// short position in Option1
	OnOption1Call();									// Option1 is a Call
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	OnBuyStock();										// long position in the Stock
	m_Qty[3] = 1.0;
	setEditBox( "%g", m_Qty[3], IDC_StockQty );
}			// SyntheticShortPut()
//---------------------------------------------------------------------------
// same as SyntheticShortCall()
void	CStrategyExplorer::CoveredPut( void )
{		// A short Put and a short stock or futures contract
		// http://biz.yahoo.com/opt/glossary1.html
	OnSellOption1();									// short position in Option1
	OnOption1Put();										// Option1 is a Put
	m_Qty[0] = 1.0;
	setEditBox( "%g", m_Qty[0], IDC_Option1Qty );

	OnSellStock();										// short position in the Stock
	m_Qty[3] = 1.0;
	setEditBox( "%g", m_Qty[3], IDC_StockQty );
}			// CoveredPut()
*/
//----------------------------------------------------------------------------
/*
	float	qtyCost = (float)(qty * ((m_BuySell[idx] == Buy) ? -1.0 : 1.0));
	CFPoint	x0Pt, rtPt, strkPt( strike, qtyCost );
	float	dxx, dyy;
	if ( m_PutCall[idx-1] == Call )
	{	x0Pt = CFPoint( topLeft.x, qtyCost );
		dxx = bottomRight.x - strike;
		dyy = dxx * (float)((m_BuySell[idx-1] == Sell) ? -1.0 : 1.0);
		rtPt = CFPoint( bottomRight.x, qtyCost + dyy );
	}
	else	// ( m_PutCall[idx-1] == Put )
	{	rtPt = CFPoint( bottomRight.x, qtyCost );
		dxx = topLeft.x - strike;
		dyy = -dxx * (float)((m_BuySell[idx-1] == Sell) ? -1.0 : 1.0);
		x0Pt = CFPoint( topLeft.x, qtyCost + dyy );
	}
	CPoint	ptOut;
	MapToPlot( x0Pt, ptOut );
	dc.MoveTo( ptOut );
	MapToPlot( strkPt, ptOut );
	dc.LineTo( ptOut );
	MapToPlot( rtPt, ptOut );
	dc.LineTo( ptOut );
*/
/*
	CPaintDC	dc(this);
	CString	cs[] = {	 _T("Notes:"),
						 _T("1) 'Days to Expiry' and 'Eval Day' are"),
						 _T("assumed to be equal (and therefore"),
						 _T("ignored) for all but 'Calendar Spread'"),
						 _T("2) Low volatility effectively zooms in on"),
						 _T("the strike points; high volatility zooms"),
						 _T("out to include less likely outcomes.")		};
	short	yy = (short)(plotExtents.top + 8);
	for ( ii = 0; ii < 7; ii++ )
	{	yy += 14 * ii;
		dc.ExtTextOut( plotExtents.left + 4, yy, ETO_OPAQUE, NULL, cs[ii], NULL );
		if ( ii == 3 )
			yy += 10;		// insert some vertical space
	}
//---------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusOption1DaysToExpiry( void )
{
	float	ff = getEditBoxFloat( IDC_Option1DaysToExpiry );
	if ( ff == m_DaysToExpiry[0] )
		return;

	m_DaysToExpiry[0] = ff;
	EvalStrategy();
}			// OnKillFocusOption1DaysToExpiry()
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusOption2DaysToExpiry( void )
{
	float	ff = getEditBoxFloat( IDC_Option2DaysToExpiry );
	if ( ff == m_DaysToExpiry[1] )
		return;

	m_DaysToExpiry[1] = ff;
	EvalStrategy();
}			// OnKillFocusOption2DaysToExpiry()
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusOption3DaysToExpiry( void )
{
	float	ff = getEditBoxFloat( IDC_Option3DaysToExpiry );
	if ( ff == m_DaysToExpiry[2] )
		return;

	m_DaysToExpiry[2] = ff;
	EvalStrategy();
}			// OnKillFocusOption3DaysToExpiry()
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusEvalDay( void )
{
	float	ff = getEditBoxFloat( IDC_EvalDay );
	if ( ff == m_EvalDay )
		return;

	m_EvalDay = ff;
	EvalStrategy();
}			// OnKillFocusEvalDay()
//----------------------------------------------------------------------------
void	CStrategyExplorer::EvalProbabilityClosingITM( void )
{		// gating conditions
	CStatic*	pITMStatic = (CStatic*)GetDlgItem( IDC_ITMResult );
	if ( m_Volatility <= 0.0  ||  m_Cost[3] <= 0.0 )
	{	pITMStatic->SetWindowText( _T("~") );
		return;
	}
		// find the probabilities for each of the options
	float		riskFreeRate = 0.03f;				// 3.0% default
	bool	res = theApp.pDB->getRiskFreeRate( riskFreeRate );
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("StrategyExplorer::EvalProbabilityClosingITM: no registered Risk Free Rate, using %.2f%%\n"),
			100.0 * riskFreeRate );
#endif
	double	pMin = 1.0;
//	bool	pIsValid = false;
	for ( short ii = 0; ii < 3; ii++ )
	{	if ( InstComplete(ii)  &&  m_YrsToExpiry[ii] > 0.0 )
		{		// use Black-Scholes N(d2) as the probability of closing In The Money, see:
				// http://www.duke.edu/~charvey/Classes/ba350/optval/optval.htm
			double	d2 = log( m_Cost[3] / m_Strike[ii] )
					   - ( riskFreeRate + 0.5 * m_Volatility * m_Volatility ) * m_YrsToExpiry[ii];
			d2 /= m_Volatility * sqrt( m_YrsToExpiry[ii] );
			double	pITM = normCDF( d2 );
			TRACE( _T("Probability of Closing ITM [%d]=%.3f\n"), ii, pITM );
//			pIsValid = true;
		}
	}
//	if ( pIsValid )
		setEditBox( "%.1f%%", 100.0 * pMin, IDC_ITMResult );
//	else
//		pITMStatic->SetWindowText( _T("~") );

	// what's the probability of the stock price being where it is now?

}			// EvalProbabilityClosingITM()
//----------------------------------------------------------------------------
void CStrategyExplorer::OnKillFocusOption1YrsToExpiry() 
{
	float	ff = (float)fabs(getEditBoxFloat( IDC_Option1YrsToExpiry ));
	if ( ff == m_YrsToExpiry[0] )
		return;

	m_YrsToExpiry[0] = ff;
	setEditBox( "%g", m_YrsToExpiry[0], IDC_Option1YrsToExpiry );
//	EvalProbabilityClosingITM();	
}			// OnKillFocusOption1YrsToExpiry()
//----------------------------------------------------------------------------
void CStrategyExplorer::OnKillFocusOption2YrsToExpiry() 
{
	float	ff = (float)fabs(getEditBoxFloat( IDC_Option2YrsToExpiry ));
	if ( ff == m_YrsToExpiry[1] )
		return;

	m_YrsToExpiry[1] = ff;
	setEditBox( "%g", m_YrsToExpiry[1], IDC_Option2YrsToExpiry );
	EvalProbabilityClosingITM();	
}			// OnKillFocusOption2YrsToExpiry()
//----------------------------------------------------------------------------
void CStrategyExplorer::OnKillFocusOption3YrsToExpiry() 
{
	float	ff = (float)fabs(getEditBoxFloat( IDC_Option3YrsToExpiry ));
	if ( ff == m_YrsToExpiry[2] )
		return;

	m_YrsToExpiry[2] = ff;
	setEditBox( "%g", m_YrsToExpiry[2], IDC_Option3YrsToExpiry );
	EvalProbabilityClosingITM();	
}			// OnKillFocusOption23rsToExpiry()
//----------------------------------------------------------------------------
void	CStrategyExplorer::OnKillFocusVolatility( void )
{
	float	ff = (float)fabs(getEditBoxFloat( IDC_Volatility ));
	if ( ff == m_Volatility )
		return;

	m_Volatility = ff;
	setEditBox( "%g", m_Volatility, IDC_Volatility );
	EvalStrategy();							// affects plot width & probability of closing ITM
}			// OnKillFocusVolatility()
//----------------------------------------------------------------------------
void CStrategyExplorer::OnKillFocusVolatility( void ) 
{
	// TODO: Add your control notification handler code here
	
}			// OnKillFocusVolatility()
//----------------------------------------------------------------------------
*/
