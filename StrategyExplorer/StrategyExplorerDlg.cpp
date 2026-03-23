// StrategyExplorerDlg.cpp : implementation file
#include "StdAfx.h"
#include "resource.h"
#include "LineSeg.h"
#include "Parser.h"
#include "utils.h"							// Discretize()

//#include "Pwinuser.h"						// for GetForegroundKeyboardTarget()
//#include "OneStk4OptStrategy.h"
#include "StrategyExplorerDlg.h"
#include "StrategyExplorer.h"
#include <set>
#include <ctype.h>							// tolower()
#include <iostream>
#include <fstream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_TRAY 5000
#define TRAY_NOTIFYICON WM_USER + 2001

const char	EOS = '\0';
extern	CStrategyExplorerApp	theApp;

/////////////////////////////////////////////////////////////////////////////
// CStrategyExplorerDlgDlg dialog

	// plot colors
const	COLORREF	bkgndColor = RGB(233,233,188);		// background fill color (manilla 228,228,184)
const	COLORREF	gridColor  = RGB(191,191,191);		// most grid lines (light Gray)
const	COLORREF	axisColor  = RGB(159,159,159);		// X-axis (light-mid Gray)
const	COLORREF	textColor  = RGB( 63, 63, 63);		// labels (dark Gray)

/////////////////////////////////////////////////////////////////////////////
// CStrategyExplorerDlg dialog

CStrategyExplorerDlg::CStrategyExplorerDlg( CWnd* pParent )
	: CNillaDialog( CStrategyExplorerDlg::IDD, pParent )
	, m_activeComboBox( NULL )
{
	//{{AFX_DATA_INIT( CStrategyExplorerDlg )
	m_Strategy = _T("");
	//}}AFX_DATA_INIT
//	m_hIcon = AfxGetApp()->LoadIcon( IDI_StrategyExplorer );
	m_hbWhite.CreateSolidBrush( RGB(255,255,255) );
	m_hbGray.CreateSolidBrush( RGB(208,208,208) );
}

void	CStrategyExplorerDlg::DoDataExchange( CDataExchange* pDX )
{
	CNillaDialog::DoDataExchange( pDX );
	//{{AFX_DATA_MAP( CStrategyExplorerDlg )
	DDX_Control(pDX, IDC_Option1Qty, c_Option1Qty);
	DDX_Control(pDX, IDC_Option2Qty, c_Option2Qty);
	DDX_Control(pDX, IDC_Option3Qty, c_Option3Qty);
	DDX_Control(pDX, IDC_Option4Qty, c_Option4Qty);
	DDX_Control(pDX, IDC_StockQty,   c_StockQty);

	DDX_Control(pDX, IDC_SellOption1, c_SellOption1);
	DDX_Control(pDX, IDC_SellOption2, c_SellOption2);
	DDX_Control(pDX, IDC_SellOption3, c_SellOption3);
	DDX_Control(pDX, IDC_SellOption4, c_SellOption4);
	DDX_Control(pDX, IDC_SellStock,	  c_SellStock);

	DDX_Control(pDX, IDC_BuyOption1, c_BuyOption1);
	DDX_Control(pDX, IDC_BuyOption2, c_BuyOption2);
	DDX_Control(pDX, IDC_BuyOption3, c_BuyOption3);
	DDX_Control(pDX, IDC_BuyOption4, c_BuyOption4);
	DDX_Control(pDX, IDC_BuyStock,   c_BuyStock);

	DDX_Control(pDX, IDC_Strategy, c_Strategy);
	DDX_CBString(pDX, IDC_Strategy, m_Strategy);
	DDV_MaxChars(pDX, m_Strategy, 24);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( CStrategyExplorerDlg, CNillaDialog )
	//{{AFX_MSG_MAP( CStrategyExplorerDlg )
	ON_CBN_SELCHANGE(IDC_Strategy, OnSelChangeStrategy)

	ON_BN_CLICKED(IDC_BuyOption1, OnBuyOption1)
	ON_BN_CLICKED(IDC_BuyOption2, OnBuyOption2)
	ON_BN_CLICKED(IDC_BuyOption3, OnBuyOption3)
	ON_BN_CLICKED(IDC_BuyOption4, OnBuyOption4)
	ON_BN_CLICKED(IDC_BuyStock,   OnBuyStock)

	ON_BN_CLICKED(IDC_SellOption1, OnSellOption1)
	ON_BN_CLICKED(IDC_SellOption2, OnSellOption2)
	ON_BN_CLICKED(IDC_SellOption3, OnSellOption3)
	ON_BN_CLICKED(IDC_SellOption4, OnSellOption4)
	ON_BN_CLICKED(IDC_SellStock,   OnSellStock)

	ON_BN_CLICKED(IDC_Option1Call, OnOption1Call)
	ON_BN_CLICKED(IDC_Option2Call, OnOption2Call)
	ON_BN_CLICKED(IDC_Option3Call, OnOption3Call)
	ON_BN_CLICKED(IDC_Option4Call, OnOption4Call)

	ON_BN_CLICKED(IDC_Option1Put, OnOption1Put)
	ON_BN_CLICKED(IDC_Option2Put, OnOption2Put)
	ON_BN_CLICKED(IDC_Option3Put, OnOption3Put)
	ON_BN_CLICKED(IDC_Option4Put, OnOption4Put)

	ON_EN_KILLFOCUS(IDC_Option1Cost, OnKillFocusOption1Cost)
	ON_EN_KILLFOCUS(IDC_Option2Cost, OnKillFocusOption2Cost)
	ON_EN_KILLFOCUS(IDC_Option3Cost, OnKillFocusOption3Cost)
	ON_EN_KILLFOCUS(IDC_Option4Cost, OnKillFocusOption4Cost)
	ON_EN_KILLFOCUS(IDC_StockPrice,  OnKillFocusStockPrice)

	ON_EN_KILLFOCUS(IDC_Option1Strike, OnKillFocusOption1Strike)
	ON_EN_KILLFOCUS(IDC_Option2Strike, OnKillFocusOption2Strike)
	ON_EN_KILLFOCUS(IDC_Option3Strike, OnKillFocusOption3Strike)
	ON_EN_KILLFOCUS(IDC_Option4Strike, OnKillFocusOption4Strike)

	ON_EN_SETFOCUS(IDC_Option1Cost, OnSetFocusOption1Cost)
	ON_EN_SETFOCUS(IDC_Option2Cost, OnSetFocusOption2Cost)
	ON_EN_SETFOCUS(IDC_Option3Cost, OnSetFocusOption3Cost)
	ON_EN_SETFOCUS(IDC_Option4Cost, OnSetFocusOption4Cost)
	ON_EN_SETFOCUS(IDC_StockPrice, OnSetFocusStockPrice)

	ON_EN_SETFOCUS(IDC_Option1Strike, OnSetFocusOption1Strike)
	ON_EN_SETFOCUS(IDC_Option2Strike, OnSetFocusOption2Strike)
	ON_EN_SETFOCUS(IDC_Option3Strike, OnSetFocusOption3Strike)
	ON_EN_SETFOCUS(IDC_Option4Strike, OnSetFocusOption4Strike)

	ON_EN_KILLFOCUS(IDC_Option1Qty, OnKillFocusOption1Qty)
	ON_EN_KILLFOCUS(IDC_Option2Qty, OnKillFocusOption2Qty)
	ON_EN_KILLFOCUS(IDC_Option3Qty, OnKillFocusOption3Qty)
	ON_EN_KILLFOCUS(IDC_Option4Qty, OnKillFocusOption4Qty)
	ON_EN_KILLFOCUS(IDC_StockQty,   OnKillFocusStockQty)

	ON_WM_PAINT()

	ON_CBN_SELCHANGE(IDC_Option1Qty, OnSelChangeOption1Qty)
	ON_CBN_SELCHANGE(IDC_Option2Qty, OnSelChangeOption2Qty)
	ON_CBN_SELCHANGE(IDC_Option3Qty, OnSelChangeOption3Qty)
	ON_CBN_SELCHANGE(IDC_Option4Qty, OnSelChangeOption4Qty)
	ON_CBN_SELCHANGE(IDC_StockQty,   OnSelChangeStockQty)

	ON_CBN_KILLFOCUS(IDC_Strategy,   OnKillFocusStrategy)

	ON_CBN_KILLFOCUS(IDC_Option1Qty, OnKillFocusOption1Qty)
	ON_CBN_KILLFOCUS(IDC_Option2Qty, OnKillFocusOption2Qty)
	ON_CBN_KILLFOCUS(IDC_Option3Qty, OnKillFocusOption3Qty)
	ON_CBN_KILLFOCUS(IDC_Option4Qty, OnKillFocusOption4Qty)
	ON_CBN_KILLFOCUS(IDC_StockQty,   OnKillFocusStockQty)

	ON_CBN_SETFOCUS(IDC_Strategy,    OnSetFocusStrategy)

	ON_CBN_SETFOCUS(IDC_Option1Qty, OnSetFocusOption1Qty)
	ON_CBN_SETFOCUS(IDC_Option2Qty, OnSetFocusOption2Qty)
	ON_CBN_SETFOCUS(IDC_Option3Qty, OnSetFocusOption3Qty)
	ON_CBN_SETFOCUS(IDC_Option4Qty, OnSetFocusOption4Qty)
	ON_CBN_SETFOCUS(IDC_StockQty,   OnSetFocusStockQty)

	//}}AFX_MSG_MAP
//	ON_WM_CHAR()
//	ON_WM_KEYDOWN()
	ON_WM_ACTIVATE()
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_REGISTERED_MESSAGE( WM_STRATEGYEXPLORER_ALREADY_RUNNING, OnStrategyExplorerAlreadyRunning)
	ON_WM_CTLCOLOR()
//	ON_MESSAGE( TRAY_NOTIFYICON, OnNotifyIcon )
//	ON_CBN_SETFOCUS(IDC_Strategy, &CStrategyExplorerDlg::OnSetFocusStrategy)
END_MESSAGE_MAP()

//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::killFocusStrategyWork( void )
{		// Strategies are keyed by name, but characterized by a unique
		// combination of Qty, Put/Call (for the Options in the Strategy) & Buy/Sell
		// If we assume Qty is limited to 0 or 1, (not counting expiration date
		// variations) there are (2 * 2 * 2)^3 * 2 * 2 = 2048 possible strategies
		// involving 3 options and 1 stock
	inhibitEval = true;

		// find the hedge or spread selected
	Spread_T*	spr = NULL;
	StockPos_T* stk = NULL;
	Hedge_T*	hdg = NULL;
	Hedges_T::iterator hdgsIt = hedges.find( m_Strategy );
	if ( hdgsIt != hedges.end() )
	{	hdg = (Hedge_T*)hdgsIt->second;
		stk = hdg->first;
		spr = hdg->second;
		if ( stk )
		{	m_BuySell[stkIdx] = stk->buySell == 'b'  ?  Buy  :  (stk->buySell == 's'  ?  Sell  :  UnknownBuySellStatus);
			c_BuyStock.SetCheck ( m_BuySell[stkIdx] == Buy  ? BST_CHECKED : BST_UNCHECKED );
			c_SellStock.SetCheck( m_BuySell[stkIdx] == Sell ? BST_CHECKED : BST_UNCHECKED );

			m_Cost[stkIdx] = stk->cost;
			setEditBox( "%g", m_Cost[stkIdx], IDC_StockPrice );	
		}
		m_Qty[stkIdx] = stk  ?  stk->qty  :  0.0f;
		c_StockQty.SetWindowText( CStringFromFloat( m_Qty[stkIdx] ) );
		if ( spr )
		{	CSeekComboBox*	qtyCombo[4]		= { &c_Option1Qty,     &c_Option2Qty,     &c_Option3Qty,     &c_Option4Qty     };
			CButton*		buyButton[4]	= { &c_BuyOption1,     &c_BuyOption2,     &c_BuyOption3,     &c_BuyOption4     };
			CButton*		sellButton[4]	= { &c_SellOption1,    &c_SellOption2,    &c_SellOption3,    &c_SellOption4    };
			int				priceBoxID[4]   = { IDC_Option1Cost,   IDC_Option2Cost,   IDC_Option3Cost,   IDC_Option4Cost   };
			int				callButtonID[4] = { IDC_Option1Call,   IDC_Option2Call,   IDC_Option3Call,   IDC_Option4Call   };
			int				putButtonID[4]  = { IDC_Option1Put,    IDC_Option2Put,    IDC_Option3Put,    IDC_Option4Put    };
			int				strikeBoxID[4]	= { IDC_Option1Strike, IDC_Option2Strike, IDC_Option3Strike, IDC_Option4Strike };

			Spread_T::iterator spit = spr->begin();
			short ii = 0;
			while ( spit != spr->end() )
			{	OptionPos_T* op = *spit++;
				if ( op )
				{	m_BuySell[ii] = op->buySell == 'b'  ?  Buy  :  (op->buySell == 's'  ?  Sell  :  UnknownBuySellStatus);
					(buyButton[ii])->SetCheck ( m_BuySell[ii] == Buy  ? BST_CHECKED : BST_UNCHECKED );
					(sellButton[ii])->SetCheck( m_BuySell[ii] == Sell ? BST_CHECKED : BST_UNCHECKED );
					
					m_Cost[ii] = op->cost;
					setEditBox( "%g", m_Cost[ii], priceBoxID[ii] );

					m_PutCall[ii] = op->putCall == 'p'  ?  Put  :  (op->putCall == 'c'  ?  Call  :  UnknownOptionType);
					CButton* putButton = (CButton*)GetDlgItem( putButtonID[ii] );
					putButton->SetCheck( m_PutCall[ii] == Put ? BST_CHECKED : BST_UNCHECKED );
					
					CButton* callButton = (CButton*)GetDlgItem( callButtonID[ii] );
					callButton->SetCheck ( m_PutCall[ii] == Call  ? BST_CHECKED : BST_UNCHECKED );

					m_Strike[ii] = op->strike;
					setEditBox( "%g", m_Strike[ii], strikeBoxID[ii] );
				}
				m_Qty[ii] = op  ?  op->qty  :  0.0f;
				(qtyCombo[ii])->SetWindowText( CStringFromFloat( m_Qty[ii] ) );
					
				ii++;
			}			// while ( spit )
			for ( short jj = spr->size(); jj < stkIdx; jj++ )
			{	m_Qty[jj] = 0.0f;									// if there's no option, the qty is ZERO
				(qtyCombo[jj])->SetWindowText( CStringFromFloat( m_Qty[jj] ) );
			}
		}
	}
		// etc. for each of the strategyNames
	inhibitEval = false;
	EvalStrategy();
}			// killFocusStrategyWork()
//---------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
// Other CStrategyExplorerDlg message handlers ...

/*
LRESULT		CStrategyExplorerDlg::OnNotifyIcon( WPARAM wParam, LPARAM lParam )
{
	bool	handleIt = ( lParam == WM_LBUTTONDOWN  &&  wParam == ID_TRAY );
	return	(LRESULT)( handleIt  ?  SetForegroundWindow()  :  0 );
}
*/
//---------------------------------------------------------------------------
HBRUSH		CStrategyExplorerDlg::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	pDC->SetBkMode( TRANSPARENT );
	return	( nCtlColor == CTLCOLOR_EDIT )  ?  m_hbWhite  :  m_hbGray;
}
//---------------------------------------------------------------------------
LRESULT		CStrategyExplorerDlg::OnStrategyExplorerAlreadyRunning( WPARAM, LPARAM )
{
	return	WM_STRATEGYEXPLORER_ALREADY_RUNNING;
}			// OnStrategyExplorerAlreadyRunning()
//---------------------------------------------------------------------------
void		CStrategyExplorerDlg::SaveStrategies( void )
{	wchar_t		moduleName[ MAX_PATH ];
	HMODULE hModule = GetModuleHandle( _T("StrategyExplorer.exe") );
	unsigned int nChars = GetModuleFileName( hModule, moduleName, sizeof(moduleName)-1 );
	if ( nChars < 1 )
		return;
	CString fName = moduleName;
	int lastSep = fName.ReverseFind( _T('\\') );				// find the last directory seperator
	fName = fName.Left( lastSep+1 );
	fName += _T("Strategies.txt");
	wchar_t	wbuf[511];
	CFileException ex;
	CFile fp;
	if ( ! fp.Open( fName, CFile::modeCreate | CFile::modeWrite, &ex ) )
	{	TCHAR	errMsg[ 256 ];
		ex.GetErrorMessage( errMsg, 256 );
#if _DEBUG
		TRACE( _T("SaveStrategies couldn't open '%s' because '%s'\n"), fName, errMsg );
#endif
		return;
	}
	fp.Write( (void*)&unicodeHeader, sizeof(unicodeHeader) );

	CString prevSettings = _T("Previous Settings...");
	Hedges_T::iterator hdgsIt = hedges.begin();
	while ( hdgsIt != hedges.end() )
	{	short	wb = 0;
		if ( prevSettings.Compare( hdgsIt->first ) == 0 )
		{	hdgsIt++;
			continue;
		}
		swprintf( wbuf+wb, _T("'%s'"), hdgsIt->first );			// the hedge name
		wb += wcslen( wbuf+wb );
		Hedge_T* hdg = hdgsIt->second;							// the hedge itself
		StockPos_T* stk = hdg->first;
		Spread_T* spr = hdg->second;
			// options in the hedge's spread
		Spread_T::iterator spit = spr->begin();
		while ( spit != spr->end() )
		{	OptionPos_T* op = (OptionPos_T*)(*spit);
			if ( op->qty > 0  &&  op->buySell != 'u'  &&  op->putCall != 'u' )
			{	swprintf( wbuf+wb, _T("\no[ %g %c %g %c %g ]"),
						op->qty, op->buySell, op->cost, op->putCall, op->strike );
				wb += wcslen( wbuf+wb );
			}
			spit++;
		}
		if ( stk  &&  stk->qty > 0  &&  stk->buySell != 'u' )
		{	swprintf( wbuf+wb, _T("\ns[ %g %c %g ]"), stk->qty, stk->buySell, stk->cost );
			wb += wcslen( wbuf+wb );
		}
		swprintf( wbuf+wb, _T(";\n\n") );
		wb += wcslen( wbuf+wb );
		fp.Write( wbuf, 2*wb );									// these are unicode characters
		hdgsIt++;
	}
	fp.Close();
}			// SaveStrategies()
//---------------------------------------------------------------------------
void		CStrategyExplorerDlg::GetStrategies( void )
{	char			*lp, *line;
	StockPos_T*		stk = NULL;
	OptionPos_T*	opt[4] = { NULL, NULL, NULL, NULL };
		// The format of a 'serialized strategy...
		//	'strategy Name'
		//	0..4 of O[ qty [b,s] cost [p,c] strike ]
		//	0..1 of s[ qty [b,s] cost ]
		// .e.g. 'Protective Put'  o[ 1 b .75 p 23 ]  s[ 1 b 25 ];
		// whitespace is not significant (not even CR), but
		// the semicolon is a sentinal for logical end of strategy
	wchar_t		fName[ MAX_PATH ];
	HMODULE hModule = GetModuleHandle( _T("StrategyExplorer.exe") );
	unsigned int nChars = GetModuleFileName( hModule, fName, sizeof(fName)-1 );
	if ( nChars < 1 )
		return;
	CString cs = fName;
	int lastSep = cs.ReverseFind( _T('\\') );			// find the last directory seperator
	cs = cs.Left( lastSep+1 );
	cs += _T("Strategies.txt");
	CFile fp;
	CFileException ex;
	if ( ! fp.Open( cs, CFile::modeRead, &ex ) )
	{	TCHAR	errMsg[ 256 ];
		ex.GetErrorMessage( errMsg, 256 );
#if _DEBUG
		TRACE( _T("GetStrategies couldn't open '%s' because '%s'\n"), fName, errMsg );
#endif
		return;
	}
	fp.Read( (void*)&unicodeHeader, sizeof(unicodeHeader ) );
	while ( ReadPast( fp, "\'" ) )
	{		// get a complete strategy incl opening '\'' and closing ';' terminator
		wchar_t* stratBuf = GetStratBuf( fp, ";" );			// backs up to pick up the opening '\'
		CString stratName;
		bool okay = CrackStratBuf( stratBuf, stratName, line );
		delete [] stratBuf;	stratBuf = NULL;				// only need unicode for strategy names
		if ( ! okay )
		{	delete [] line;
			continue;
		}
		lp = line;
		StopAtAny( lp, "oOsS" );

		short	nStocks = 0;
		Spread_T*	spr = NULL;								// on stack
		StockPos_T	stk;									// on stack
		while ( *lp != EOS )
		{	char stockOrOpt = tolower(*lp++);				// an 'o', 'O' or an 's', 'S', bypass the o/s toggle
			int	nItems = 0;									// scaned from position string
			EatWhatever( lp, "[ \t\r\n" );					// eat an open bracket and surrounding whitespace
			if ( stockOrOpt == 's'  &&  nStocks < 1 )		// so the first stock seen gets in, extras are ignored
			{		// a stock description
				nItems = sscanf( lp, "%f %c %f", &stk.qty, &stk.buySell, &stk.cost );
				if ( nItems > 2 )
				{	nStocks++;
					stk.buySell = tolower( stk.buySell );
				}
			}
			else if ( stockOrOpt == 'o' )
			{		// an option description
				float qty, cost, strike;	char buySell, putCall;
				nItems = sscanf( lp, "%f %c %f %c %f",
								&qty, &buySell, &cost, &putCall, &strike );
				buySell = tolower( buySell );
				putCall = tolower( putCall );
				if ( nItems > 4 )
				{	OptionPos_T* op = new OptionPos_T( qty, buySell, cost, putCall, strike );
					if ( op )
					{	if ( !spr )
							spr = new Spread_T();			// one option in a possible spread
						spr->push_back( op );
					}
				}
				else
					goto	Cleanup;
			}
			else
				goto	Cleanup;							// not 'o', 'O', 's', or 'S'
			StopAtAny( lp, "]" );
			EatWhatever( lp, "] \t\n\r" );					// eat a close bracket and surrounding whitespace
			if ( *lp == ';' )
				break;
		}
			// we may have a Spread_T and a StockPos_T
		StockPos_T* heapStock = NULL;
		if ( nStocks > 0 )
			heapStock = new StockPos_T( stk );					// copy constructor
		if ( spr  ||  heapStock )
		{	Hedge_T* hdg = new Hedge_T( heapStock, spr );			// spr could be NULL
			hedges.insert( make_pair(stratName,hdg) );
		}
		if ( line )
		{	delete [] line;
			line = NULL;
		}
	}
Cleanup:
	if ( line )
	{	delete [] line;
		line = NULL;
	}
	fp.Close();
}			// GetStrategies()
//---------------------------------------------------------------------------
BOOL	CStrategyExplorerDlg::OnInitDialog( void )
{
	CNillaDialog::OnInitDialog();
		// register VK_UP and VK_DOWN to any pair of controls
	RegisterHotKeys();			// screens for WM5 internally

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// Set the icon for this dialog.  The framework does this automatically
		// when the application's main window is not a dialog
//	SetIcon( m_hIcon, TRUE);				// Set big icon
//	SetIcon( m_hIcon, FALSE);				// Set small icon
	CenterWindow( GetDesktopWindow() );		// center to the hpc screen

		// restore the most recent strategy settings
	CString prevSet = _T("Previous Settings...");
	OneStk4OptStrategy	os4os;
	bool savedStrategyExists = GetRecentStrategy( os4os );
	if ( savedStrategyExists )
	{	Spread_T* spr = new Spread_T();
		char buySell, putCall;
		for ( short ii = 0; ii < stkIdx; ii++ )
		{	buySell = os4os.buySell[ii] == Buy ? 'b' : (os4os.buySell[ii] == Sell ? 's' : 'u');
			putCall = os4os.putCall[ii] == Put ? 'p' : (os4os.putCall[ii] == Call ? 'c' : 'u');
			OptionPos_T* op = new OptionPos_T( os4os.qty[ii], buySell, os4os.cost[ii], putCall, os4os.strike[ii] );
			spr->push_back( op );
		}
		buySell = os4os.buySell[stkIdx] == Buy ? 'b' : (os4os.buySell[stkIdx] == Sell ? 's' : 'u');
		StockPos_T* stk = new StockPos_T( os4os.qty[stkIdx], buySell, os4os.cost[stkIdx] );
		Hedge_T* hdg = new Hedge_T( make_pair( stk, spr ) );
		hedges.insert( make_pair( prevSet, hdg ) );
		c_Strategy.AddString( prevSet );
	}
		// load strategies
	GetStrategies();
	Hedges_T::iterator hdgIter = hedges.begin();
	while ( hdgIter != hedges.end() )
	{	c_Strategy.AddString( hdgIter->first );
		hdgIter++;
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// to keep the plot in the same place as the hardcoded values...
		// plotExtents = CRect( 0, 122, 240, 268 );
		// we'd use:	 CRect( 0, (vga ? 244 : 122), widthX+2, heightY-1 )
		// maybe:		 CRect( 0, (vga ? 244 : 122), widthX, heightY )
	AfxEnableDRA( true );										// lo res / hi res
	int widthX = GetSystemMetrics( SM_CXFULLSCREEN );			//	  238 / 476
	int heightY = GetSystemMetrics( SM_CYFULLSCREEN );			//	  269 / 538
	vga = widthX > 240  &&  heightY > 240;
	plotExtents = vga ? CRect( 0, 254, widthX+4, heightY-2 )
					  : CRect( 0, 127, widthX+2, heightY-0 );
		
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
	{	c_BuyOption4.SetCheck( m_BuySell[3] == Buy ? BST_CHECKED : BST_UNCHECKED );
		c_SellOption4.SetCheck( m_BuySell[3] == Sell ? BST_CHECKED : BST_UNCHECKED );
	}
	if ( m_BuySell[4] != UnknownBuySellStatus )
	{	c_BuyStock.SetCheck( m_BuySell[4] == Buy ? BST_CHECKED : BST_UNCHECKED );
		c_SellStock.SetCheck( m_BuySell[4] == Sell ? BST_CHECKED : BST_UNCHECKED );
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
	if ( m_PutCall[3] != UnknownOptionType )
	{	CButton* c_Option4Call = (CButton*)GetDlgItem( IDC_Option4Call );
		c_Option4Call->SetCheck( m_PutCall[3] == Call ? BST_CHECKED : BST_UNCHECKED );
	
		CButton* c_Option4Put = (CButton*)GetDlgItem( IDC_Option4Put );
		c_Option4Put->SetCheck( m_PutCall[3] == Put ? BST_CHECKED : BST_UNCHECKED );
	}

		// Clear Qty ComboBoxes ...
	c_Option1Qty.SetFloat( m_Qty[0] );
	c_Option2Qty.SetFloat( m_Qty[1] );
	c_Option3Qty.SetFloat( m_Qty[2] );
	c_Option4Qty.SetFloat( m_Qty[3] );
	c_StockQty.SetFloat( m_Qty[stkIdx] );

		// Cost EditBoxes ...
	setEditBox( "%g", m_Cost[0], IDC_Option1Cost );
	setEditBox( "%g", m_Cost[1], IDC_Option2Cost );
	setEditBox( "%g", m_Cost[2], IDC_Option3Cost );
	setEditBox( "%g", m_Cost[3], IDC_Option4Cost );
	setEditBox( "%g", m_Cost[stkIdx], IDC_StockPrice );	

		// Strike EditBoxes ...
	setEditBox( "%g", m_Strike[0], IDC_Option1Strike );	
	setEditBox( "%g", m_Strike[1], IDC_Option2Strike );	
	setEditBox( "%g", m_Strike[2], IDC_Option3Strike );	
	setEditBox( "%g", m_Strike[3], IDC_Option4Strike );

	inhibitEval = false;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		// handle saved strategy or default to first strategy in the list
//	c_Strategy.lastChar = VK_DOWN;
//	OnSelChangeStrategy();
	int sel = c_Strategy.FindString( -1, prevSet );
	c_Strategy.SetCurSel( sel >= 0 ? sel : 0 );
	c_Strategy.GetLBText( c_Strategy.GetCurSel(), m_Strategy );
	killFocusStrategyWork();
		// it used to be good enough to just EvalStrategy at startup, but since
		// killing off the DocList stuff in InitInstance(), the background didn't get
		// an initial paint message and EvalStrategy wouldn't generate the necessary
		// Paint() events to cause the plot area to paint (ever), so it became
		// necessary to forcably select a strategy
//	EvalStrategy();
	c_Strategy.SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//--------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////
//								Plot related								//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::MapToPlot( CFPoint ptIn, CPoint& ptOut )
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
void	CStrategyExplorerDlg::EvalStrategy( void )
{		// gutted this - moving functionality into OnPaint() so we could handle Update events too
	InvalidateRect( plotExtents, FALSE );
	UpdateWindow();
}			// EvalStrategy()
//----------------------------------------------------------------------------
void CStrategyExplorerDlg::OnPaint() 
{		// nothing to paint if we're in a critical region or there are no instruments to evaluate
	if ( inhibitEval  ||  ! CalcDomainExtents() )
	{	CPaintDC	dc( this );
		DrawPlotBackground( dc );						// blanks out any existing plots
		CDialog::OnPaint();
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
			// Delete the saved strategy - it may be corrupt
		long res = RegDeleteKey( HKEY_CURRENT_USER, _T("Software\\PocketNumerix\\StrategyExplorer\\StrategyFlags") );
		res = RegDeleteKey( HKEY_CURRENT_USER, _T("Software\\PocketNumerix\\StrategyExplorer\\StrategyValues") );
		exit( -1 );
//		ASSERT( false );
	}

	CPaintDC	dc( this );				// device context for painting
	DrawChart( dc );
//	ValidateRect( plotExtents );		// works without this
	CDialog::OnPaint();
}			// OnPaint()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::DrawChart( CPaintDC& dc )
{	short	ii;
		// plot grid, stock, options
	DrawGrid( dc );
	CPolyline	pl;
	short	itemsDrawn = DrawStock( dc, pl );
	for ( ii = 0; ii < stkIdx; ii++ )						// options before stock
		itemsDrawn += DrawOption( ii, dc, pl );

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
bool	CStrategyExplorerDlg::InstComplete( short ii )
{		// ii is 0-based ...
	bool	res = m_BuySell[ii] != UnknownBuySellStatus
			  &&	 m_Cost[ii]	!= 0.0
			  &&	  m_Qty[ii]	!= 0.0;
	if ( ii < stkIdx )
		res = res  &&  m_Strike[ii] != 0.0  &&  m_PutCall[ii] != UnknownOptionType;	
	return	res;
}			// InstComplete()
//----------------------------------------------------------------------------
bool	CStrategyExplorerDlg::InTheMoney( int ii, double xx )
{
#if _DEBUG
	if ( ii < 0  ||  ii >= stkIdx  ||  !InstComplete(ii) )
		throw "InTheMoney: bad issue index or issue incomplete";
#endif
	return	(( m_PutCall[ii] == Put   &&  xx < m_Strike[ii] )
		 ||  ( m_PutCall[ii] == Call  &&  xx > m_Strike[ii] ));
}			// InTheMoney()
//----------------------------------------------------------------------------
double	CStrategyExplorerDlg::Slope( int ii, double xx )
{	double	slope;
		// slope at an option strike is considered to be 0
		// because of the way that InTheMoney() works
#if _DEBUG
	if ( ii < 0  ||  ii > stkIdx  ||  !InstComplete(ii) )
		throw "Slope: bad issue index or issue incomplete";
#endif
	if ( ii == stkIdx )
		slope = m_BuySell[ii] == Buy  ?  1.0f  :  -1.0f;
	else
	{		// ITM long calls & short puts have positive slope
			// ITM short calls & long puts have negative slope
			// others have no slope
		slope = 0.0;				// not in the Money
		if ( InTheMoney(ii, xx ) )
			slope = (m_BuySell[ii] == Buy  &&  m_PutCall[ii] == Call)		// long call
				||  (m_BuySell[ii] == Sell  &&  m_PutCall[ii] == Put)		// short put
				  ?  1.0f  :  -1.0f;
	}
	return	m_Qty[ii] * slope;
}			// Slope()
//----------------------------------------------------------------------------
double		CStrategyExplorerDlg::SlopeCept( int ii, double xx, double& yCept )
{	double	slope, yy;
		// hiSide = true indicates that we're working on a hiStrike, otherwise a loStrike
#if _DEBUG
	if ( ii < 0  ||  ii > stkIdx  ||  !InstComplete(ii) )
		throw "SlopeCepts: bad issue index or issue incomplete";
#endif
	slope = Slope( ii, xx );				// xx is ignored for a stock
	if ( ii == stkIdx )
		yCept = - slope * m_Cost[ii];			// xCept = m_Cost[ii];
	else
	{	yy = m_Cost[ii];						// positive yy for short positions
		if ( m_BuySell[ii] == Buy )
			yy *= -1.0f;						// negative yy for long positions
			// solve: (yy - yCept) = m(xx - 0) for yCept with xx :== option's strike
		yCept = yy - slope * m_Strike[ii];		// xCept = yCept / slope;
	}
#if _DEBUG
//	TRACE( _T("SlopeCept: ii=%d, slope=%g, yCept=%g\n"), ii, slope, yCept );
#endif
	return	slope;
}			// SlopeCept()
//----------------------------------------------------------------------------
bool		CStrategyExplorerDlg::CalcDomainExtents( void )
{	short	ii, jj, kk;
	set<float>	uniqueXs;
	const	float bigNum = 1e38;
	float	loStrike = bigNum, hiStrike = -loStrike;
	double	loXcept  = bigNum, hiXcept  = -loStrike;
		// calculate the extents of the strike-profit domain
		// modify topLeft, bottomRight, xSc
		// return false to inhibit plotting (i.e. no plot is possible)
	bool	complete[ stkIdx+1 ];					// all issues

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// stuff strikes into a set (to eliminate duplicates)
	short	nComplete = 0;
	for ( ii = 0; ii <= stkIdx; ii++ )				// don't do stocks right now...
	{	complete[ii] = InstComplete(ii);
		if ( complete[ii] )
		{		// the stock price serves in place of a strike
			float	priceStrike	= ii < stkIdx  ?  m_Strike[ii]  :  m_Cost[stkIdx];
			uniqueXs.insert( priceStrike );
			if ( ii < stkIdx )
			{	loStrike = priceStrike < loStrike  ?  priceStrike  :  loStrike;
				hiStrike = priceStrike > hiStrike  ?  priceStrike  :  hiStrike;
			}
			nComplete++;
		}
	}
	if ( nComplete < 1 )
		return	false;
	float	midPoint = (float)( (hiStrike + loStrike) / 2.0 );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// set up X-test points (room for 2 for each issue).  Will use two for each
		// unique strike; later adding 2 extrema.
	set<float>::iterator	unit = uniqueXs.begin();
	jj = 0;
	const unsigned short nPts = 2 * stkIdx;
	double	xTest[nPts], slope[nPts], yCept[nPts], xCept[nPts];
	float adj[2] = { -0.001, 0.001 };
	while ( unit != uniqueXs.end() )
	{	for ( ii = 0; ii < 2; ii++ )
		{	*(xTest+jj) = *unit + *(adj+ii);					// brackets the strikes
			jj++;
		}
		unit++;
	}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// compute slope-intercept, associated x-intercepts (to discover extrema) for all 
		// complete issues and segments of the profit profile at test points.
	double	pftSlope, pftYcept, pftXcept, pftXceptLo = bigNum, pftXceptHi = -pftXceptLo;
	kk = 0;
	short	nQXpts = 2 * uniqueXs.size();
	while ( kk < nQXpts )
	{	pftSlope = 0;
		pftYcept = 0;
		for ( ii = 0; ii <= stkIdx; ii++ )						// all issues
		{	if ( complete[ii] )
			{	slope[kk] = SlopeCept( ii, xTest[kk], yCept[kk] );
				if ( slope[kk] != 0 )
				{	xCept[kk] = -yCept[kk]/slope[kk];
#if _DEBUG
//					TRACE( _T("xTest[%d]=%g, slope=%g, yCept=%g, xCept=%g\n"),
//							kk, xTest[kk], slope[kk], yCept[kk], xCept[kk] );
#endif
					if ( xCept[kk] < loXcept )
						loXcept = xCept[kk];
					if ( xCept[kk] > hiXcept )
						hiXcept = xCept[kk];
				}
				pftSlope += slope[kk];
				pftYcept += yCept[kk];
			}
		}
#if _DEBUG
//	TRACE( _T("xTest[%d]=%g --> pftSlope=%g, pftYcept=%g\n"), kk, xTest[kk], pftSlope, pftYcept );
#endif
		if ( pftSlope != 0 )
		{	pftXcept = -pftYcept / pftSlope;
			if ( pftXcept < pftXceptLo )
				pftXceptLo = pftXcept;
			if ( pftXcept > pftXceptHi )
				pftXceptHi = pftXcept;
		}
		kk++;
	}
#if _DEBUG
	TRACE( _T("loStrike=%g, hiStrike=%g, pftXceptLo=%g, pftXceptHi=%g\n"),
				loStrike, hiStrike, pftXceptLo, pftXceptHi );
#endif
		// do we always want to do this?
	if ( pftXceptLo != bigNum )
		uniqueXs.insert( (float)pftXceptLo );
	if ( pftXceptHi != -bigNum )
		uniqueXs.insert( (float)pftXceptHi );
#if _DEBUG
	TRACE( _T("uniqueXs: ") );
	unit = uniqueXs.begin();
	while ( unit != uniqueXs.end() )
		TRACE( _T("%g "), *unit++ );
	TRACE( _T("\n") );
#endif

	if ( hiStrike != loStrike )
	{		// covered (could use more margin), protective, strangles, butterflies, condors are okay
			// took care of short call condors with spreadWidth
		double	spreadWidth = hiStrike - loStrike;
		set<float>::iterator			loVal = uniqueXs.begin();	// use of hi/lo estrema instead of hi/lo strikes
		set<float>::reverse_iterator	hiVal = uniqueXs.rbegin();	// depicts more of the unlimited upside of ladders
		topLeft.x	  = (float)(*loVal - 0.15 * spreadWidth);		// label space
		bottomRight.x = (float)(*hiVal + 0.11 * spreadWidth);		// 'margin'
	}
	else
	{		// all strikes are equal (or there are no complete options)
			// synthetics too zoomed out; straddles should use width between pftXcepts
			// conversions/reversals (should incl. optionXcepts), protective/covered are too zoomed in
		topLeft.x	  = (float)(0.80 * midPoint);					// label space
		bottomRight.x = (float)(1.20 * midPoint);					// 'margin'
	}
	uniqueXs.insert( bottomRight.x );
	uniqueXs.insert( topLeft.x );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// now for the y-values (where is the maxY for Protectives coming from?)
	float	fyy, polyY, minY = 1e38f, maxY = -minY;
	unit = uniqueXs.begin();
	while ( unit != uniqueXs.end() )
	{		// for each test point
		polyY = 0.0;
		for ( ii = 0; ii <= stkIdx; ii++ )				// for each issue
		{		// for each complete Issue
			if ( complete[ii] )
			{	if ( ii < stkIdx )
				{		// Options
					CFPoint	strikePt;
					fyy = OptionY( ii, (float)*unit, strikePt );	// accounts for Qty
					float	premium = strikePt.y;
					if ( premium < minY )	minY = premium;
					if ( premium > maxY )	maxY = premium;
				}
				else
				{		// Stocks
					fyy = (float)( (m_BuySell[stkIdx] == Sell  ?  -1.0  :  1.0)
						*		   (*unit - m_Cost[stkIdx]) * m_Qty[stkIdx]	);
					if ( fyy < minY )	minY = fyy;
					if ( fyy > maxY )	maxY = fyy;
				}
				polyY += fyy;
			}
		}
			// collect the min/max polyY values
		if ( polyY > maxY )
			maxY = polyY;
		if ( polyY < minY )
			minY = polyY;
		unit++;
	}
		// provide some plot space above (below) the max (min) values
	float	plotHeight = maxY - minY;
	maxY += (float)( 0.08 * plotHeight );
	minY -= (float)( 0.15 * plotHeight );
	topLeft.y = maxY;							// the real valued Y-range
	bottomRight.y = minY;
#ifdef _DEBUG
//	TRACE( _T("StrategyExplorer::CalcDomainExtents: topLeft=(%g,%g), bottomRight=(%g,%g)\n"),
//				topLeft.x, topLeft.y, bottomRight.x, bottomRight.y );
#endif
	return	true;
}			// CalcDomainExtents()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::DrawGrid( CPaintDC& dc ) 
{		// slightly lighter Manilla than in the PortfolioNavigator
	CBrush		brushBkgnd( bkgndColor );						// bkgndColor is a CStrategyExplorerDlg const
	CBrush*		pOldBrush = dc.SelectObject( &brushBkgnd );		// for graphics (e.g. Rect) purposes

		// Create and select a thin gray pen.
	CPen		gridPen;
	gridPen.CreatePen( PS_SOLID, 1, gridColor );				// gridColor is a CStrategyExplorerDlg const
	CPen*		pOldPen = dc.SelectObject( &gridPen );

		// Draw a thin gray rectangle filled with Manilla
		// CE mapping mode is always MM_TEXT, so +y is down
	dc.Rectangle( plotExtents );

		// set up colors and the spacing between horizontal grid lines
	COLORREF	oldBkColor = dc.SetBkColor( bkgndColor );		// gridColor is a CStrategyExplorerDlg const
	COLORREF	oldTextColor = dc.SetTextColor( gridColor );	// draw the copyright notice in the grid color
	double	incY = Discretize( (topLeft.y - bottomRight.y) / 8.0 );

		// Display 'copyright' notice
	double	fyy = incY * ceil( bottomRight.y / incY );			// first horizontal grid line
	double	lyy = incY * ceil( topLeft.y / incY );				// last? horizontal grid line
	double	nGrids = floor( (lyy-fyy) / incY );					// estimate number of horizontal grid lines
	bool	isEven = ((int)nGrids) % 2 < 1;
#if _DEBUG
	TRACE( _T("nGrids=%g, %s\n"), nGrids, isEven ? _T("even") : _T("odd") );
#endif
	nGrids /= 2.0;												// need half of the total
	if ( isEven )
		nGrids -= 0.5;											// subtract a half grid to place the text between grid lines
		// plot copyright notice
	int		cpmyy = MapToPlotY( (float)(fyy + nGrids-- * incY) );
	const unsigned short	adj = vga ? 16 : 8;
	dc.ExtTextOut( (vga ? 144 : 72), cpmyy - adj, ETO_OPAQUE, NULL, _T("All rights reserved."), NULL );
	cpmyy = MapToPlotY( (float)(fyy + nGrids * incY ) );
	dc.ExtTextOut( (vga ? 86 : 43), cpmyy - adj, ETO_OPAQUE, NULL, _T("PocketNumerix <pnmx.com>"), NULL );
	dc.SetTextColor( textColor );

	CPen	axisPen;
	axisPen.CreatePen( PS_SOLID, 2, axisColor );

		// Draw horizontal grid lines
	while ( topLeft.y > fyy  &&  fyy > bottomRight.y )
	{	bool	itsTheXaxis = ( fabs(fyy) < 1e-15 );
		short	yy = MapToPlotY( (float)fyy );
#ifdef _DEBUG
//		TRACE( _T("DrawGrid: fyy=%g, yy=%d\n"), fyy, yy );
#endif
			// if we're painting the X-axis, switch to a broader pen
		if ( itsTheXaxis )
			dc.SelectObject( &axisPen );

		dc.MoveTo( plotExtents.left, yy );
		dc.LineTo( plotExtents.right, yy );
#if ( _WIN32_WCE >= 0x420 )
				// when the if statement is present in EVT3,
				// the compiler will optimize the SelectObject() out of the executable
		if ( itsTheXaxis )
#endif
			dc.SelectObject( &gridPen );					// put the gridPen back
		
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
void	CStrategyExplorerDlg::DrawGridLabels( CPaintDC& dc ) 
{		// for text purposes
	COLORREF	oldBkColor = dc.SetBkColor( bkgndColor );		// gridColor is a CStrategyExplorerDlg const
	COLORREF	oldTextColor = dc.SetTextColor( textColor );

		// Draw labels for horizontal grid lines
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
		const unsigned short adj = vga ? 14 : 7;
		if ( plotExtents.top + adj <= yy  &&  yy <= plotExtents.bottom - adj )
		{	wchar_t buf[16];
			swprintf( buf, _T("%g"), (fabs(fyy) < 1e-4 ? 0.0 : fyy) );
			CString	str = buf;
			CSize	cs = dc.GetTextExtent( str );
			CPoint	txtOrigin( plotExtents.left + 6, yy - adj );
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
	{	short	xx = (short)(plotExtents.left + (fxx - topLeft.x) * xScale);
#ifdef _DEBUG
//		TRACE( _T("DrawGridLabels: fxx=%g, xx=%d\n"), fxx, xx );
#endif
			// skip the first vertical line label to avoid conflicts with Y-axis labels
		wchar_t buf[16];
		swprintf( buf, _T("%g"), fxx );
		CString	str = buf;
		CSize cs = dc.GetTextExtent( str );
		CPoint	txtOrigin = CPoint( xx - cs.cx / 2, plotExtents.bottom - (vga ? 32 : 16) );
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
short	CStrategyExplorerDlg::DrawStock( CPaintDC& dc, CPolyline& pl )
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
	for ( short ii = 0; ii < 2; ii++ )						// two points on a line
	{	float	buySell = (float)(m_BuySell[stkIdx] == Sell  ?  -1.0  :  1.0);
			// the strategy polyline should not be clipped to the plot region
		fyy[ii] = polyY[ii] = (float)( buySell * (fxx[ii] - m_Cost[stkIdx]) * m_Qty[stkIdx] );	// slope
			// the stock line has to be clipped to the plot region
		if ( fyy[ii] < bottomRight.y )
		{	fyy[ii] = bottomRight.y;
			fxx[ii] = fyy[ii] / (m_Qty[stkIdx] * buySell) + m_Cost[stkIdx];
		}
		if ( fyy[ii] > topLeft.y )
		{	fyy[ii] = topLeft.y;
			fxx[ii] = fyy[ii] / (m_Qty[stkIdx] * buySell) + m_Cost[stkIdx];	
		}
	}
		// update the polyline
	CFPoint	rtPolyPt( polyX[0], polyY[0] );
	CFPoint	leftPolyPt( polyX[1], polyY[1] );

	short	nPts = pl.GetPointCount();
	pl.InsertPoint( leftPolyPt );
	pl.InsertPoint( rtPolyPt );
	if ( nPts > 0 )
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
short	CStrategyExplorerDlg::DrawOption( short ii, CPaintDC& dc, CPolyline& pl )
{		// Draws the extrinsic profit line,
		// calls DrawBlackScholes() to plot the sum of the intrinsic and extrinsic values
		// idx is 0-based for compatibility with MFC variable names
		// idx based gates...
	if (	 m_PutCall[ii] == UnknownOptionType
		 ||  m_BuySell[ii] == UnknownBuySellStatus
		 ||  m_Cost[ii] == 0.0  ||  m_Qty[ii] == 0.0  ||  m_Strike[ii] == 0.0 )
		return	0;

	CPen		optionPen;
	COLORREF	optionColor = ( ii < 3 )
							?  RGB( ii == 0 ? 255 : 127,
									ii == 1 ? 255 : 127,
									ii == 2 ? 255 : 127 )		// creates Red, Green, or Blue
							:  RGB( 255, 255, 0 );				// creates Yellow
	optionPen.CreatePen( PS_SOLID, 2, optionColor );
	CPen*	pOldPen = dc.SelectObject( &optionPen );

		// create CLineSegs and map to the plot region
		// horizontal segment for Calls starts at 0 and ends at strike
	float	slope = (float)(m_Qty[ii] * (m_BuySell[ii] == Buy  ?  1.0 : -1.0)
									  * (m_PutCall[ii] == Call ?  1.0 : -1.0) );
	if ( m_BuySell[stkIdx] != UnknownBuySellStatus  &&  m_Qty[stkIdx] > 0.0 )
		slope /= m_Qty[stkIdx];									// rescale by stock Qty
	CFPoint	strikePt;
	float	fxx[3], fyy[3], polyX[3], polyY[3];					// three points per option
	fxx[0] = polyX[0] = topLeft.x;
	fxx[1] = polyX[1] = bottomRight.x;
	for ( short jj = 0; jj < 3; jj++ )
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
	if ( nPts > 0 )
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
float	CStrategyExplorerDlg::OptionY( short ii, float fxx, CFPoint& strikePt )
{
	ASSERT ( ii < stkIdx  &&  InstComplete(ii) );
	float	fyy = (float)(m_Cost[ii] * m_Qty[ii] * (m_BuySell[ii] == Buy ? -1.0 : 1.0));
	strikePt = CFPoint( m_Strike[ii], fyy );
		// if we're on the horizontal section of the profit profile, so we're done ...
	if (	(fxx > m_Strike[ii]  &&  m_PutCall[ii] == Put)
		||	(fxx < m_Strike[ii]  &&  m_PutCall[ii] == Call) )
		return	fyy;		// anything above (below) a Put (Call) strike is a premium loss

		// we're on the profit diagonal
	float	slope = (float)(m_Qty[ii] * (m_BuySell[ii] == Buy  ?  1.0 : -1.0)
									  * (m_PutCall[ii] == Call ?  1.0 : -1.0) );
		// dxx is the x displacement of interest; dxx > 0 for 
	float	dxx = fxx - strikePt.x;				// dxx > 0 for Calls; dxx < 0 for Puts
	fyy = slope * dxx + strikePt.y;
	return	fyy;
}			// optionY()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::DrawPlotBackground( CPaintDC& dc ) 
{		// only called by OnPaint(), but identical to code within DrawGrid()
		// slightly lighter Manilla than in the PortfolioNavigator
	CBrush		brushBkgnd( bkgndColor );					// bkgndColor is a CStrategyExplorerDlg const
	CBrush*		pOldBrush = dc.SelectObject( &brushBkgnd );	// for graphics (e.g. Rect) purposes

	CPen		gridPen;
	gridPen.CreatePen( PS_SOLID, 1, gridColor );			// gridColor is a CStrategyExplorerDlg const
	CPen*		pOldPen = dc.SelectObject( &gridPen );

		// Draw a thin gray rectangle filled with Manilla
		// CE mapping mode is always MM_TEXT, so +y is down
	dc.Rectangle( plotExtents );
	dc.SelectObject( pOldBrush );
	dc.SelectObject( pOldPen );
}			// DrawPlotBackground()
//--------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////
//			Option1 messages (except Buy/Sell & Put/Call buttons)			//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::killFocusEditBoxWork( int ctrlID, float* storedFloat )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( ctrlID );
	if ( ff == *storedFloat )
		return;

	*storedFloat = (float)fabs(ff);
	setEditBox( "%g", *storedFloat, ctrlID );

	EvalStrategy();
}			// killFocusEditBoxWork()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnKillFocusOption1Strike( void )
{
	killFocusEditBoxWork( IDC_Option1Strike, &(m_Strike[0]) );
}			// OnKillFocusOption1Strike()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnKillFocusOption1Cost( void )
{
	killFocusEditBoxWork( IDC_Option1Cost, &(m_Cost[0]) );
}			// OnKillFocusOption1Cost()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnSetFocusOption1Strike( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusOption1Strike()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnSetFocusOption1Cost( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusOption1Cost()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//			Option2 messages (except Buy/Sell & Put/Call buttons)			//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnKillFocusOption2Strike( void )
{
	killFocusEditBoxWork( IDC_Option2Strike, &(m_Strike[1]) );
}			// OnKillFocusOption2Strike()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnKillFocusOption2Cost( void )
{
	killFocusEditBoxWork( IDC_Option2Cost, &(m_Cost[1]) );
}			// OnKillFocusOption2Cost()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnSetFocusOption2Strike( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusOption2Strike()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnSetFocusOption2Cost( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusOption2Cost()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//			Option3 messages (except Buy/Sell & Put/Call buttons)			//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnKillFocusOption3Strike( void )
{
	killFocusEditBoxWork( IDC_Option3Strike, &(m_Strike[2]) );
}			// OnKillFocusOption3Strike()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnKillFocusOption3Cost( void )
{
	killFocusEditBoxWork( IDC_Option3Cost, &(m_Cost[2]) );
}			// OnKillFocusOption3Cost()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnSetFocusOption3Strike( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusOption3Strike()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnSetFocusOption3Cost( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusOption3Cost()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//			Option4 messages (except Buy/Sell & Put/Call buttons)			//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnKillFocusOption4Strike( void )
{
	killFocusEditBoxWork( IDC_Option4Strike, &(m_Strike[3]) );
}			// OnKillFocusOption4Strike()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnKillFocusOption4Cost( void )
{
	killFocusEditBoxWork( IDC_Option4Cost, &(m_Cost[3]) );
}			// OnKillFocusOption4Cost()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnSetFocusOption4Strike( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusOption4Strike()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnSetFocusOption4Cost( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusOption4Cost()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Stock Price									//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnKillFocusStockPrice( void )
{
	killFocusEditBoxWork( IDC_StockPrice, &(m_Cost[stkIdx]) );
}			// OnKillFocusStockPrice()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnSetFocusStockPrice( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusStockPrice()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::SelChangeQtyWork( CSeekComboBox* scb, float* qty )
{	float	ff0, ebff, ffn;
	int	cnt = scb->GetCount();
	if ( cnt < 1 )
		return;
	int		ebii;
#ifdef _DEBUG
	TRACE( _T("StrategyExplorerDlg::SelChangeQtyWork: VK_DOWN=%d, VK_UP=%d, scb->lastChar=%d\n"),
			VK_DOWN, VK_UP, scb->lastChar );
#endif
	if ( scb->lastChar == VK_DOWN  ||  scb->lastChar == VK_UP )
	{		// here we ignore the LB selection and start from the EditBox value
		if ( scb->GetFloat(ebff) != 1 )				// the EditBox float
			return;
		if (	 scb->GetFloat(    0,ff0 ) != 1		// the first ListBox float
			 ||  scb->GetFloat(cnt-1,ffn ) != 1 )	// the last  ListBox float
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
		scb->SetCurSel( ebii );						// change effected by arrow key
	}
	else
		ebii = scb->GetCurSel();					// change effected by LB Closeup
	if ( ebii < 0 ) ebii = 0;						// this should be superfluous

	if ( scb->GetFloat( ebii, *qty ) != 1 )			// load the float directly into qty
	{
#ifdef _DEBUG
		TRACE( _T("StrategyExplorerDlg::SelChangeQtyWork: Couldn't get a float at index %d\n"), ebii );
#endif
		return;										// this would be inconvenient
	}
	EvalStrategy();
}			// SelChangeQtyWork()
//----------------------------------------------------------------------------
void CStrategyExplorerDlg::OnSelChangeOption1Qty( void )
{
	SelChangeQtyWork( &c_Option1Qty, &(m_Qty[0]) );
}			// OnSelChangeOption1Qty()
//----------------------------------------------------------------------------
void CStrategyExplorerDlg::OnSelChangeOption2Qty( void )
{
	SelChangeQtyWork( &c_Option2Qty, &(m_Qty[1]) );
}			// OnSelChangeOption2Qty()
//----------------------------------------------------------------------------
void CStrategyExplorerDlg::OnSelChangeOption3Qty( void )
{
	SelChangeQtyWork( &c_Option3Qty, &(m_Qty[2]) );
}			// OnSelChangeOption3Qty()
//----------------------------------------------------------------------------
void CStrategyExplorerDlg::OnSelChangeOption4Qty( void )
{
	SelChangeQtyWork( &c_Option4Qty, &(m_Qty[3]) );
}			// OnSelChangeOption4Qty()
//----------------------------------------------------------------------------
void CStrategyExplorerDlg::OnSelChangeStockQty( void )
{
	SelChangeQtyWork( &c_StockQty, &(m_Qty[stkIdx]) );
}			// OnSelChangeStockQty()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Buy/Sell Buttons							//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::buySellWork(
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

	EvalStrategy();
}			// OnBuyOption1()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnBuyOption1( void )
{
	buySellWork( &c_BuyOption1, &c_SellOption1, &(m_BuySell[0]), Buy );
}			// OnBuyOption1()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnSellOption1( void )
{
	buySellWork( &c_BuyOption1, &c_SellOption1, &(m_BuySell[0]), Sell );
}			// OnSellOption1()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnBuyOption2( void )
{
	buySellWork( &c_BuyOption2, &c_SellOption2, &(m_BuySell[1]), Buy );
}			// OnBuyOption2()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnSellOption2( void )
{
	buySellWork( &c_BuyOption2, &c_SellOption2, &(m_BuySell[1]), Sell );
}			// OnSellOption2()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnBuyOption3( void )
{
	buySellWork( &c_BuyOption3, &c_SellOption3, &(m_BuySell[2]), Buy );
}			// OnBuyOption3()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnSellOption3( void )
{
	buySellWork( &c_BuyOption3, &c_SellOption3, &(m_BuySell[2]), Sell );
}			// OnSellOption3()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnBuyOption4( void )
{
	buySellWork( &c_BuyOption4, &c_SellOption4, &(m_BuySell[3]), Buy );
}			// OnBuyOption4()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnSellOption4( void )
{
	buySellWork( &c_BuyOption4, &c_SellOption4, &(m_BuySell[3]), Sell );
}			// OnSellOption4()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnBuyStock( void )
{
	buySellWork( &c_BuyStock, &c_SellStock, &(m_BuySell[stkIdx]), Buy );
}			// OnBuyStock()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnSellStock( void )
{
	buySellWork( &c_BuyStock, &c_SellStock, &(m_BuySell[stkIdx]), Sell );
}			// OnSellStock()
//---------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Put/Call Buttons							//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::putCallWork(
	int				putButtonID,
	int				callButtonID,
	OptionType*		putCallMemory,
	OptionType		pcVal				)
{
	if ( *putCallMemory == pcVal )
		return;
	*putCallMemory = pcVal;

	CButton* callButton = (CButton*)GetDlgItem( callButtonID );
	callButton->SetCheck( pcVal == Call  ?  BST_CHECKED  :  BST_UNCHECKED );

	CButton* putButton = (CButton*)GetDlgItem( putButtonID );
	putButton->SetCheck( pcVal == Put  ?  BST_CHECKED  :  BST_UNCHECKED  );

	EvalStrategy();
}			// putCallWork()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnOption1Call( void )
{
	putCallWork( IDC_Option1Put, IDC_Option1Call, &(m_PutCall[0]), Call );
}			// OnOption1Call()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnOption1Put( void )
{
	putCallWork( IDC_Option1Put, IDC_Option1Call, &(m_PutCall[0]), Put );
}			// OnOption1Put()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnOption2Call( void )
{
	putCallWork( IDC_Option2Put, IDC_Option2Call, &(m_PutCall[1]), Call );
}			// OnOption2Call()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnOption2Put( void )
{
	putCallWork( IDC_Option2Put, IDC_Option2Call, &(m_PutCall[1]), Put );
}			// OnOption2Put()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnOption3Call( void )
{
	putCallWork( IDC_Option3Put, IDC_Option3Call, &(m_PutCall[2]), Call );
}			// OnOption3Call()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnOption3Put( void )
{
	putCallWork( IDC_Option3Put, IDC_Option3Call, &(m_PutCall[2]), Put );
}			// OnOption3Put()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnOption4Call( void )
{
	putCallWork( IDC_Option4Put, IDC_Option4Call, &(m_PutCall[3]), Call );
}			// OnOption4Call()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnOption4Put( void )
{
	putCallWork( IDC_Option4Put, IDC_Option4Call, &(m_PutCall[3]), Put );
}			// OnOption4Put()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Stock/Option Quantity						//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::KillFocusComboBoxWork( void )
{
	m_activeComboBox = NULL;
//	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
}			// KillFocusComboBoxWork()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::killFocusQtyWork( CSeekComboBox* scb, float* qty )
{	float	ff;
	KillFocusComboBoxWork();
		// passing the gate requires that GetFloat() works
		// and that ff differs from what we know
	if ( scb->GetFloat(ff)  &&  ff == *qty )
		return;
	*qty = (float)fabs(ff);
	EvalStrategy();
}			// killFocusQtyWork()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnKillFocusOption1Qty( void )
{		// m_activeComboBox set in killFocusQtyWork()
	killFocusQtyWork( &c_Option1Qty, &(m_Qty[0]) );
}			// OnKillFocusOption1Qty()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnKillFocusOption2Qty( void )
{		// m_activeComboBox set in killFocusQtyWork()
	killFocusQtyWork( &c_Option2Qty, &(m_Qty[1]) );
}			// OnKillFocusOption2Qty()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnKillFocusOption3Qty( void )
{		// m_activeComboBox set in killFocusQtyWork()
	killFocusQtyWork( &c_Option3Qty, &(m_Qty[2]) );
}			// OnKillFocusOption3Qty()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnKillFocusOption4Qty( void )
{		// m_activeComboBox set in killFocusQtyWork()
	killFocusQtyWork( &c_Option4Qty, &(m_Qty[3]) );
}			// OnKillFocusOption3Qty()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnKillFocusStockQty( void )
{		// m_activeComboBox set in killFocusQtyWork()
	killFocusQtyWork( &c_StockQty, &(m_Qty[stkIdx]) );
}			// OnKillFocusStockQty()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnKillFocusStrategy( void )
{	CString sym;
	KillFocusComboBoxWork();				// drop the SIP and HotKey
	c_Strategy.GetWindowText( sym );		// KillFocus called us
#ifdef _DEBUG
	TRACE( _T("StrategyExplorerDlg::OnKillFocusStrategy: sym=%s, m_Strategy=%s\n"),
			sym, m_Strategy );
#endif
	if ( m_Strategy == sym )
		return;
	m_Strategy = sym;
	killFocusStrategyWork();
}			// OnKillFocusStrategy()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::UpdateStrategy( void )
{
	Hedges_T::iterator hdgsIt = hedges.find( m_Strategy );
	if ( hdgsIt != hedges.end() )
	{	Hedge_T*	hdg = hdgsIt->second;
		StockPos_T*	stk = hdg->first;
		if ( !stk  &&  InstComplete(stkIdx) )
			stk = hdg->first = new StockPos_T();
		if ( stk )
		{	stk->qty	 = m_Qty[stkIdx];
			stk->buySell = m_BuySell[stkIdx] == Buy  ?  'b'  :  (m_BuySell[stkIdx] == Sell  ?  's'  :  'u');
			stk->cost	 = m_Cost[stkIdx];
		}
		Spread_T* spr = hdg->second;
		bool	complete[stkIdx+1], anyComplete = false;
		for ( short ii = 0; ii <= stkIdx; ii++ )
		{	complete[ii] = InstComplete(ii);
			anyComplete = anyComplete  ||  complete[ii];
		}
		if ( !spr  &&  anyComplete )
			spr = hdg->second = new Spread_T;
		if ( spr )
		{	short nOptions = spr->size();
			for ( short ii = 0; ii < stkIdx; ii++ )
			{	OptionPos_T* op = ii < nOptions  ?  spr->at(ii)  :  NULL;
				if ( !op  &&  complete[ii] )
				{		// add another options
					op = new OptionPos_T();
					if ( op )
						spr->push_back( op );
				}
				if ( op )
				{	op->qty		= m_Qty[ii];
					op->buySell	= m_BuySell[ii] == Buy  ?  'b'  :  (m_BuySell[ii] == Sell  ?  's'  :  'u');
					op->cost	= m_Cost[ii];
					op->putCall	= m_PutCall[ii] == Put  ?  'p'  :  (m_PutCall[ii] == Call  ?  'c'  :  'u');
					op->strike	= m_Strike[ii];
				}
			}
		}
	}			
}			// UpdateStrategy()
//---------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnSelChangeStrategy( void )
{		// modify stored parameters for m_Strategy in hedges, then switch to sel strategy
	UpdateStrategy();
		// switch to sel strategy
	int cnt = c_Strategy.GetCount();
	if ( cnt < 1 )
		return;
	int sel = c_Strategy.GetCurSel();
#if ( _WIN32_WCE >= 0x420 )					// just WM5
	if ( c_Strategy.lastChar == VK_DOWN  ||  c_Strategy.lastChar == VK_UP )
	{	int	delta = ( c_Strategy.lastChar == VK_DOWN  ?  1  :  -1 );
		sel += delta;
		if ( sel < 0 )
			sel = cnt - 1;
		if ( sel >= cnt )
			sel = 0;
		c_Strategy.SetCurSel( sel );
	}
#endif
	if ( sel < 0 ) sel = 0;						// should be superfluous
	c_Strategy.GetLBText( sel, m_Strategy );
	killFocusStrategyWork();
}			// OnSelChangeStrategy()
//---------------------------------------------------------------------------
LRESULT		CStrategyExplorerDlg::OnHotKey( WPARAM wParam, LPARAM lParam )
{		//	UINT fuModifiers = (UINT)LOWORD(lParam);		// don't care about modifiers
		//	DWORD ctrlID = (int)wParam;						// don't care which control the HotKey is registered to (any more)
		//	CWnd* wnd = GetDlgItem( ctrlID );				// also about the registered control
	if ( ! m_activeComboBox )
		return	0;											// nothing to do

	UINT uVirtKey = (UINT)HIWORD(lParam);
	if ( uVirtKey != VK_DOWN  &&  uVirtKey != VK_UP )
		return	0;											// only care about the up/down arrow keys

		// make the currently focued SeekComboBox think it saw the recent VK_UP or VK_DOWN
	CSeekComboBox* scb = (CSeekComboBox*)GetDlgItem( m_activeComboBox );
	scb->lastChar = uVirtKey;										// this in combination with SendMessage()
	WPARAM wp = MAKEWPARAM( m_activeComboBox, CBN_SELCHANGE );
	SendMessage( WM_COMMAND, wp, (LPARAM)scb->GetSafeHwnd() );		// handles all of the else clause below
	return	0;
}			// OnHotKey()
//----------------------------------------------------------------------------
void		CStrategyExplorerDlg::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	CNillaDialog::OnActivate( nState, pWndOther, bMinimized );
	if ( nState == WA_ACTIVE  ||  nState == WA_CLICKACTIVE )
		RegisterHotKeys();					// screens for WM5 internally
}			// OnActivate()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::RegisterHotKeys( void )
{		// ignore the incoming CWnd
	if ( theApp.m_osVer < 5 )
		return;

	HWND hwnd = GetSafeHwnd();				// the dialog
	DWORD ctrlID = IDC_Option1Qty;			// anything will do
	BOOL res = RegisterHotKey( hwnd, ctrlID, NULL, VK_UP );
#ifdef _DEBUG
	if ( res == 0 )							// failure is zero
	{	wchar_t	wbuf[128];
		DWORD err = GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, NULL, wbuf, 127, NULL );
		TRACE( _T("StrategyExplorerDlg::RegisterHotKeys(%d):RegisterHotKey(VK_UP) --> %d: %s\n"),
			ctrlID, err, wbuf );
	}
#endif		// _DEBUG
	ctrlID = IDC_Option2Qty;			// anything else will do
	res = RegisterHotKey( hwnd, ctrlID, NULL, VK_DOWN );
#ifdef _DEBUG
	if ( res == 0 )							// failure is zero
	{	wchar_t	wbuf[128];
		DWORD err = GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, NULL, wbuf, 127, NULL );
		TRACE( _T("StrategyExplorerDlg::RegisterHotKeys(%d):RegisterHotKey(VK_DOWN) --> %d: %s\n"),
			ctrlID, err, wbuf );
	}
#endif		// _DEBUG
}			// RegisterHotKeys()
//----------------------------------------------------------------------------
void CStrategyExplorerDlg::SetFocusComboBoxWork( CSeekComboBox* scb, DWORD ctrlID )
{
	m_activeComboBox = ctrlID;			// for SelChange events
//	int cnt = scb->GetCount();			// for keyboard entry
//	if ( cnt < 1 )
//	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// SetFocusComboBoxWork()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnSetFocusStrategy( void )
{
	SetFocusComboBoxWork( &c_Strategy, IDC_Strategy );
}			// OnSetFocusStrategy()
//----------------------------------------------------------------------------
void CStrategyExplorerDlg::OnSetFocusOption1Qty()
{
	SetFocusComboBoxWork( &c_Option1Qty, IDC_Option1Qty );
}			// OnSetFocusOption1Qty()
//----------------------------------------------------------------------------
void CStrategyExplorerDlg::OnSetFocusOption2Qty()
{
	SetFocusComboBoxWork( &c_Option2Qty, IDC_Option2Qty );
}			// OnSetFocusOption2Qty()
//----------------------------------------------------------------------------
void CStrategyExplorerDlg::OnSetFocusOption3Qty()
{
	SetFocusComboBoxWork( &c_Option3Qty, IDC_Option3Qty );
}			// OnSetFocusOption3Qty()
//----------------------------------------------------------------------------
void CStrategyExplorerDlg::OnSetFocusOption4Qty()
{
	SetFocusComboBoxWork( &c_Option4Qty, IDC_Option4Qty );
}			// OnSetFocusOption4Qty()
//----------------------------------------------------------------------------
void CStrategyExplorerDlg::OnSetFocusStockQty()
{
	SetFocusComboBoxWork( &c_StockQty, IDC_StockQty );
}			// OnSetFocusStockQty()
//----------------------------------------------------------------------------
/*
/*
double	CStrategyExplorerDlg::LeftSlope( int ii, double xx )
{		// affects slope of Puts when xx == the strike price
	if ( ii < stkIdx  &&  m_PutCall[ii] == Put  &&  fabs(xx - m_Strike[ii]) < 0.0001 )
		xx -= 0.01;
	return	Slope( ii, xx );
}			// LeftSlope()
//----------------------------------------------------------------------------
double	CStrategyExplorerDlg::RightSlope( int ii, double xx )
{		// affects slope of Calls when xx == the strike price
	if ( ii < stkIdx  &&  m_PutCall[ii] == Call  &&  fabs(xx - m_Strike[ii]) < 0.0001 )
		xx += 0.01;
	return	Slope( ii, xx );
}			// RightSlope()
//----------------------------------------------------------------------------
bool	StrategyExplorerDlg::LongCall( int ii )
{
#if _DEBUG
	if ( ii < 0  ||  ii >= stkIdx  ||  !InstComplete(ii) )
		throw "LongCall: bad issue index or issue incomplete";
#endif
	return	m_PutCall[ii] == Call  &&  m_BuySell[ii] == Buy;
}			// LongCall()
//----------------------------------------------------------------------------
bool	StrategyExplorerDlg::LongPut( int ii )
{	
#if _DEBUG
	if ( ii < 0  ||  ii >= stkIdx  ||  !InstComplete(ii) )
		throw "LongPut: bad issue index or issue incomplete";
#endif
	return	m_PutCall[ii] == Put  &&  m_BuySell[ii] == Buy;
}			// LongPut()
//----------------------------------------------------------------------------
bool	StrategyExplorerDlg::ShortCall( int ii )
{	
#if _DEBUG
	if ( ii < 0  ||  ii >= stkIdx  ||  !InstComplete(ii) )
		throw "ShortCall: bad issue index or issue incomplete";
#endif
	return	m_PutCall[ii] == Call  &&  m_BuySell[ii] == Sell;
}			// ShortCall()
//----------------------------------------------------------------------------
bool	StrategyExplorerDlg::ShortPut( int ii )
{	
#if _DEBUG
	if ( ii < 0  ||  ii >= stkIdx  ||  !InstComplete(ii) )
		throw "ShortPut: bad issue index or issue incomplete";
#endif
	return	m_PutCall[ii] == Put  &&  m_BuySell[ii] == Sell;
}			// ShortPut()
//----------------------------------------------------------------------------
		// this snippet was in OnHotKey()
	if ( m_activeComboBox == IDC_Strategy )
	{
		scb->SeekEntry();									// maybe there's an exact match ?
		int	delta = (uVirtKey == VK_UP)  ?  -1  :  1;		// true: VK_UP (-1), false: VK_DOWN (+1)
		int sel = scb->GetCurSel() + delta;					// delta modifies the currently selected list entry
			// make sure the new 'selection' is not out of range
		if ( sel < 0 )
			sel = 0;
		int cnt = scb->GetCount();
		if ( sel >= cnt )
			sel = cnt - 1;
		scb->SetCurSel( sel );
			// make the Strategy SeekComboBox think it saw the recent VK_UP or VK_DOWN
		OnSelChangeStrategy();
	}
	else
	{	scb->lastChar = uVirtKey;
			// don't know why, but the following switch is irrelevant
			// it accomplishes nothing that scb->lastChar doesn't already do
		switch ( m_activeComboBox )
		{	case IDC_Option1Qty:
				OnSelChangeOption1Qty();
				break;
			case IDC_Option2Qty:
				OnSelChangeOption2Qty();
				break;
			case IDC_Option3Qty:
				OnSelChangeOption3Qty();
				break;
			case IDC_StockQty:
				OnSelChangeStockQty();
				break;
		}
	}
*/
//----------------------------------------------------------------------------
/*
void	CStrategyExplorerDlg::UnregisterHotKeys( void )
{
	if ( theApp.m_osVer < 5 )
		return;

	HWND hwnd = GetSafeHwnd();
	BOOL res = UnregisterHotKey( hwnd, VK_UP );
#if _DEBUG
	if ( res == 0 )							// failure is zero
	{	wchar_t	wbuf[128];
		DWORD err = GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, NULL, wbuf, 127, NULL );
		TRACE( _T("StrategyExplorerDlg::UnregisterHotKeys(%d):UnregisterHotKey(VK_UP) --> %d: %s\n"),
			m_activeComboBox, err, wbuf );
	}
#endif		// _DEBUG
	res = UnregisterHotKey( hwnd, VK_DOWN );
#if _DEBUG
	if ( res == 0 )							// failure is zero
	{	wchar_t	wbuf[128];
		DWORD err = GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, NULL, wbuf, 127, NULL );
		TRACE( _T("StrategyExplorerDlg::UnregisterHotKeys(%d):UnregisterHotKey(VK_DOWN) --> %d: %s\n"),
			m_activeComboBox, err, wbuf );
	}
#endif		// _DEBUG
}			// UnregisterHotKeys()
*/
//----------------------------------------------------------------------------
/*
void	CStrategyExplorerDlg::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags )
{		// WM5				 up arrow		 down arrow
#ifdef _DEBUG
	TRACE( _T("StrategyExplorerDlg::OnChar: nChar=0x%x, nRepCnt=0x%x, nFlags=0x%x\n"), 
		nChar, nRepCnt, nFlags);
#endif
	if ( theApp.m_osVer >= 5  &&  (nChar == VK_UP  ||  nChar == VK_DOWN) )
	{	CWnd* ctl = GetFocus();
		if ( ctl->IsKindOf( RUNTIME_CLASS( CSeekComboBox ) ) )
		{
#ifdef _DEBUG
			TRACE( _T("StrategyExplorerDlg::OnKeyDown: SendMessage(WM_CHAR) to control\n") );
#endif
//			ctl->OnChar( nChar, nRepCnt, nFlags );
			return;
		}
	}
	CNillaDialog::OnChar( nChar, nRepCnt, nFlags );
}			// OnChar()
//----------------------------------------------------------------------------
void CStrategyExplorerDlg::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{
#ifdef _DEBUG
	TRACE( _T("StrategyExplorerDlg::OnKeyUp: nChar=0x%x, nRepCnt=0x%x, nFlags=0x%x\n"), 
		nChar, nRepCnt, nFlags);
#endif
	if ( theApp.m_osVer >= 5  &&  (nChar == VK_UP  ||  nChar == VK_DOWN) )
	{	CWnd* ctl = GetFocus();
		if ( ctl->IsKindOf( RUNTIME_CLASS( CSeekComboBox ) ) )
		{
#ifdef _DEBUG
			TRACE( _T("StrategyExplorerDlg::OnKeyDown: SendMessage(WM_KEYUP) to control\n") );
#endif
//			ctl->OnKeyUp( nChar, nRepCnt, nFlags );
			return;
		}
	}
	CNillaDialog::OnKeyUp( nChar, nRepCnt, nFlags );
}			// OnKeyUp()
//----------------------------------------------------------------------------
void	CStrategyExplorerDlg::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{		// WM5				 up arrow		 down arrow
#ifdef _DEBUG
	TRACE( _T("StrategyExplorerDlg::OnKeyDown: nChar=0x%x, nRepCnt=0x%x, nFlags=0x%x\n"), 
		nChar, nRepCnt, nFlags);
#endif
	if ( theApp.m_osVer >= 5  &&  (nChar == VK_UP  ||  nChar == VK_DOWN) )
	{	CWnd* ctl = GetFocus();
		if ( ctl->IsKindOf( RUNTIME_CLASS( CSeekComboBox ) ) )
		{	
#ifdef _DEBUG
			TRACE( _T("StrategyExplorerDlg::OnKeyDown: SendMessage(WM_KEYDOWN) to control\n") );
#endif
//			ctl->OnKeyDown( nChar, nRepCnt, nFlags );
			return;
		}
	}
	CNillaDialog::OnKeyDown( nChar, nRepCnt, nFlags );
}			// OnKeyDown()
*/
//----------------------------------------------------------------------------
/*
void CStrategyExplorerDlg::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags )
{		// WM5				 up arrow		 down arrow
	if ( theApp.m_osVer >= 5  &&  (nChar == 38  ||  nChar == 40) )
	{	CWnd* ctl = GetFocus();
		if ( ctl->IsKindOf( RUNTIME_CLASS( CSeekComboBox ) ) )
		{	ctl->OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
			return;
		}
	}
	CNillaDialog::OnChar( nChar, nRepCnt, nFlags );
}			// OnChar()
//----------------------------------------------------------------------------
void CStrategyExplorerDlg::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{		// WM5				 up arrow		 down arrow
	if ( theApp.m_osVer >= 5  &&  (nChar == 38  ||  nChar == 40) )
	{	CWnd* ctl = GetFocus();
		if ( ctl->IsKindOf( RUNTIME_CLASS( CSeekComboBox ) ) )
		{	ctl->OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
			return;
		}
	}
	CNillaDialog::OnKeyDown( nChar, nRepCnt, nFlags );
}			// OnKeyDown()
//----------------------------------------------------------------------------
#if 0
#if _WIN32_WCE > 0x420
	UnregisterHotKey( scb->GetSafeHwnd(), VK_UP );
	UnregisterHotKey( scb->GetSafeHwnd(), VK_DOWN );
#endif		// _WIN32_WCE > 0x420
#endif		// 0
*/
/*
void	CStrategyExplorerDlg::setFocusQtyWork( CSeekComboBox* scb )
{
#if _WIN32_WCE > 0x420
	long id = scb->GetDlgCtrlID();
	HWND hwnd = scb->GetSafeHwnd();
	BOOL res = RegisterHotKey( hwnd, id, NULL, VK_UP );
#if _DEBUG
	if ( res == 0 )							// failure is zero
	{	wchar_t	wbuf[128];
		DWORD err = GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, NULL, wbuf, 127, NULL );
		TRACE( _T("StrategyExplorerDlg::setFocusQtyWork: RegisterHotKey(VK_UP) --> %d : %s\n"), err, wbuf );
	}
#endif		// _DEBUG
	res = RegisterHotKey( hwnd, id, NULL, VK_DOWN );
#if _DEBUG
	if ( res == 0 )							// failure is zero
	{	wchar_t	wbuf[128];
		DWORD err = GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, NULL, wbuf, 127, NULL );
		TRACE( _T("StrategyExplorerDlg::setFocusQtyWork: RegisterHotKey(VK_DOWN) --> %d : %s\n"), err, wbuf );
	}
#endif		// _DEBUG	
#endif		// _WIN32_WCE > 0x420
}			// setFocusQtyWork()
*/
//----------------------------------------------------------------------------
