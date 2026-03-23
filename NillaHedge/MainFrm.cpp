// MainFrm.cpp : implementation of the CMainFrame class
//

#include "StdAfx.h"
#include "NillaHedge.h"
#include "OneStk3OptStrategy.h"
//#include "BOSDatabase.h"
#include "NillaHedgeDoc.h"
#include "MainFrm.h"
//#include "BitArray.h"
#include "Stock.h"
#include "Option.h"
//#include "Bond.h"
#include "Position.h"

//#include "BondDefinitionDialog.h"
#include "OptionDefinitionDialog.h"
#include "StockDefinitionDialog.h"
#include "RateSensitivity.h"

#include "PortfolioOverviewDialog.h"
#include "PositionsDialog.h"

//#include "BondAnalyzerDialog.h"
#include "OptionAnalyzerDialog.h"
#include "OptionChainDialog.h"
#include "PositionListOptionsDialog.h"
//#include "SpotRateEstimatorDialog.h"
#include "PositionTranscriptDialog.h"
//#include "StrategyExplorer.h"
#include "HedgeExplorer.h"
#include "RateSensitivityExplorer.h"
#include "TimeDecayExplorer.h"
#include "VolatilityExplorer.h"

#include "ConfirmSavePreferencesDialog.h"
#include "PositionDisplayOptions.h"

//#include "BondManager.h"
#include "OptionManager.h"
#include "StockManager.h"
#include "RegistryManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PSPC_TOOLBAR_HEIGHT 24

const DWORD dwAdornmentFlags = 0; // exit button

extern	CNillaHedgeApp	theApp;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE( CMainFrame, CFrameWnd )

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
//	ON_NOTIFY(DLN_CE_CREATE, AFXCE_ID_DOCLIST, OnCreateDocList)
//	ON_NOTIFY(DLN_CE_DESTROY, AFXCE_ID_DOCLIST, OnDestroyDocList)
	ON_WM_DESTROY()
	ON_COMMAND(ID_DEFINE_CONFIRM_SAVE_OPTIONS, OnDefineConfirmSaveOptions)
	ON_COMMAND(ID_DEFINE_OPTION, OnDefineOption)
	ON_COMMAND(ID_DEFINE_STOCK, OnDefineStock)
	ON_COMMAND(ID_CREATE_DISPLAY_PREFERENCES, OnCreateDisplayPreferences)
	ON_COMMAND(ID_CREATE_POSITION, OnCreatePosition)
	ON_COMMAND(ID_TOOLS_OPTION_ANALYZER, OnToolsOptionAnalyzer)
	ON_COMMAND(ID_TOOLS_PORTFOLIO_NAVIGATOR, OnToolsPortfolioNavigator)
	ON_COMMAND(ID_TOOLS_VIEW_POSITION_TRANSCRIPT, OnToolsViewPositionTranscript)
	ON_COMMAND(ID_TOOLS_OPTION_CHAIN_RETRIEVER, OnToolsOptionChainRetriever)
	ON_COMMAND(ID_FILE_CLOSE_DATABASE, OnFileCloseDatabase)
	ON_COMMAND(ID_FILE_OPEN_DATABASE, OnFileOpenDatabase)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE_DATABASE, OnUpdateFileCloseDatabase)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN_DATABASE, OnUpdateFileOpenDatabase)
	ON_UPDATE_COMMAND_UI(ID_DEFINE_OPTION, OnUpdateDefineOption)
	ON_UPDATE_COMMAND_UI(ID_DEFINE_STOCK, OnUpdateDefineStock)
	ON_UPDATE_COMMAND_UI(ID_CREATE_POSITION, OnUpdateCreatePosition)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_OPTION_ANALYZER, OnUpdateToolsOptionAnalyzer)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_OPTION_CHAIN_RETRIEVER, OnUpdateToolsOptionChainRetriever)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_PORTFOLIO_NAVIGATOR, OnUpdateToolsPortfolioNavigator)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_VIEW_POSITION_TRANSCRIPT, OnUpdateToolsViewPositionTranscript)
	ON_COMMAND(ID_TOOLS_HEDGE_EXPLORER, OnToolsHedgeExplorer)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_HEDGE_EXPLORER, OnUpdateToolsHedgeExplorer)
	ON_COMMAND(ID_TOOLS_RATE_SENSITIVITY_EXPLORER, OnToolsRateSensitivityExplorer)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_RATE_SENSITIVITY_EXPLORER, OnUpdateToolsRateSensitivityExplorer)
	ON_COMMAND(ID_TOOLS_TIME_DECAY_EXPLORER, OnToolsTimeDecayExplorer)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_TIME_DECAY_EXPLORER, OnUpdateToolsTimeDecayExplorer)
	ON_COMMAND(ID_TOOLS_VOLATILITY_VALUE_EXPLORER, OnToolsVolatilityValueExplorer)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_VOLATILITY_VALUE_EXPLORER, OnUpdateToolsVolatilityValueExplorer)
//	ON_COMMAND( WM_ICON_NOTIFY, OnIconNotify )		// never gets called
	ON_MESSAGE( WM_ICON_NOTIFY, OnIconNotify )		// this gets called !!!
	ON_REGISTERED_MESSAGE( WM_NILLAHEDGE_ALREADY_RUNNING, OnNillaHedgeAlreadyRunning)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}
//----------------------------------------------------------------------------
#if ( PPC03_TARGET  ||  WM5_TARGET )
	// this method isn't necessary in MFC3.0, but it's not part of MFC8.0. 
	// We're approximatng MFC8.0 using the platform, but the real condition
	// would examine the version of MFC available in the build
	// Even in MFC3.0, it's an undocumented function.  
void	CMainFrame::ActivateTopParent( void )
{		// special activate logic for floating toolbars and palettes
	CWnd* pTopLevel = GetTopLevelParent();
	CWnd* pActiveWnd = GetForegroundWindow();
	if ( pActiveWnd == NULL  ||
		! ( pActiveWnd->m_hWnd == m_hWnd || ::IsChild(pActiveWnd->m_hWnd, m_hWnd) ) )
	{
		// clicking on floating frame when it does not have
		// focus itself -- activate the toplevel frame instead.
		pTopLevel->SetForegroundWindow();
	}
}			// this is CWnd::ActivateTopParent() in EVT3
#endif
//----------------------------------------------------------------------------
LRESULT	CMainFrame::OnIconNotify( WPARAM wp, LPARAM lp )
{		// what should the return value be ?
#ifdef _DEBUG
	TRACE( _T("MainFrame::OnIconNotify: wp=%d, lp=%d\n"), wp, lp );
#endif
	if ( lp == WM_LBUTTONDOWN )					// tap !
	{		// ShowWindow( SW_SHOWNORMAL );		// this doesn't work !
		ActivateTopParent();					// undocumented, but works great
	}
	return	1;						// indicates we processed the message ?
}			// OnIconNotify()
//----------------------------------------------------------------------------
LRESULT		CMainFrame::OnNillaHedgeAlreadyRunning( WPARAM, LPARAM )
{
	return	WM_NILLAHEDGE_ALREADY_RUNNING;
}			// OnNillaHedgeAlreadyRunning()
//---------------------------------------------------------------------------
LRESULT CMainFrame::DefWindowProc( UINT message, WPARAM wp, LPARAM lp ) 
{
#ifdef _DEBUG
	CString	cs;
	if ( message == 256  ||  message == 258 )		// there are lots of both of these
		return	CFrameWnd::DefWindowProc( message, wp, lp );

	CString	commandSt = _T("");
	switch ( message )
	{	case WM_IME_SETCONTEXT:
		{	cs = _T("WM_IME_SETCONTEXT");
			switch ( wp )
			{	case ISC_SHOWUICANDIDATEWINDOW:
					commandSt = _T("ISC_SHOWUICANDIDATEWINDOW");
					break;
				case ISC_SHOWUICOMPOSITIONWINDOW:
					commandSt = _T("ISC_SHOWUICOMPOSITIONWINDOW");
					break;
				default:
					commandSt = _T("Unknown");
					break;
			}
			break;
		}
		case WM_IME_NOTIFY:
		{	cs = _T("WM_IME_NOTIFY");
			switch ( wp )
			{	case IMN_CHANGECANDIDATE:
					commandSt = _T("IMN_CHANGECANDIDATE");
					break;
				case IMN_CLOSECANDIDATE:
					commandSt = _T("IMN_CLOSECANDIDATE");
					break;
				case IMN_CLOSESTATUSWINDOW:
					commandSt = _T("IMN_CLOSESTATUSWINDOW");
					break;
				case IMN_GUIDELINE:
					commandSt = _T("IMN_GUIDELINE");
					break;
				case IMN_OPENCANDIDATE:
					commandSt = _T("IMN_OPENCANDIDATE");
					break;
				case IMN_OPENSTATUSWINDOW:
					commandSt = _T("IMN_OPENSTATUSWINDOW");
					break;
				case IMN_SETCANDIDATEPOS:
					commandSt = _T("IMN_SETCANDIDATEPOS");
					break;
				case IMN_SETCOMPOSITIONFONT:
					commandSt = _T("IMN_SETCOMPOSITIONFONT");
					break;
				case IMN_SETCOMPOSITIONWINDOW:
					commandSt = _T("IMN_SETCOMPOSITIONWINDOW");
					break;
				case IMN_SETCONVERSIONMODE:
					commandSt = _T("IMN_SETCONVERSIONMODE");
					break;
				case IMN_SETOPENSTATUS:
					commandSt = _T("IMN_SETOPENSTATUS");
					break;
				case IMN_SETSENTENCEMODE:
					commandSt = _T("IMN_SETSENTENCEMODE");
					break;
				case IMN_SETSTATUSWINDOWPOS:
					commandSt = _T("IMN_SETSTATUSWINDOWPOS");
					break;
				default:
					commandSt = _T("Unknown");
					break;
			}
			break;
		}
		case WM_IME_CONTROL:
			cs = _T("WM_IME_CONTROL");
			break;
		case WM_IME_COMPOSITION:
			cs = _T("WM_IME_COMPOSITION");
			break;
		default:
			cs = _T("Unknown");
			break;
	}
	TRACE( _T("MainFrame::DefWindowProc: received '%s' = %d, command '%s' = %d\n"),
			cs, message, commandSt, wp );
#endif
	if ( message == WM_IME_SETCONTEXT  &&  wp == ISC_SHOWUICANDIDATEWINDOW )
	{
		ShowWindow( SW_SHOW );			// this is why this function is here
	}
	return CFrameWnd::DefWindowProc( message, wp, lp );
}			// DefWindowProc()
//--------------------------------------------------------------------------------------
int		CMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if ( CFrameWnd::OnCreate(lpCreateStruct) == -1 )
	{	return -1;
	}
#if ( PPC02_TARGET )			// this was commented out in all versions until 09-Feb-07
	m_wndCommandBar.m_bShowSharedNewButton = FALSE;
	m_ToolTipsTable[0] = MakeString(IDS_NEW);
	m_ToolTipsTable[1] = MakeString(IDS_FILE);
	m_ToolTipsTable[2] = MakeString(IDS_MHELP);
	m_ToolTipsTable[3] = MakeString(IDS_CUT);
	m_ToolTipsTable[4] = MakeString(IDS_COPY);
	m_ToolTipsTable[5] = MakeString(IDS_PASTE);
#endif

	if (	! m_wndCommandBar.CreateEx(this)
		 || ! m_wndCommandBar.InsertMenuBar(IDR_MAINFRAME) )
	{
#ifdef _DEBUG
		TRACE0("MainFrame::OnCreate: Failed to create menubar.");
#endif
		return	-1;      // fail to create
	}
	return 0;
}			// OnCreate()
//----------------------------------------------------------------------------
BOOL	CMainFrame::PreCreateWindow( CREATESTRUCT& cs )
{
	cs.style |= FWS_PREFIXTITLE;
//	cs.style |= WS_EX_APPWINDOW;		// a new idea 12-Feb-07
//	cs.style &= ~WS_SYSMENU;			// the close box
	if ( ! CFrameWnd::PreCreateWindow(cs) )
		return	FALSE;

		// TODO: Modify the Window class or styles here by modifying
		//  the CREATESTRUCT cs
	return	TRUE;
}			// PreCreateWindow()
//----------------------------------------------------------------------------
/*
void CMainFrame::OnCreateDocList( DLNHDR* pNotifyStruct, LRESULT* result )
{
#if ( PPC02_TARGET )
	CCeDocList* pDocList = (CCeDocList*)FromHandle( pNotifyStruct->nmhdr.hwndFrom );
	ASSERT_KINDOF( CCeDocList, pDocList );

	CCeCommandBar* pDocListCB = pDocList->GetCommandBar();
	ASSERT( pDocListCB != NULL );

	pDocListCB->InsertMenuBar(IDM_DOCLIST);
	pDocListCB->SendMessage(TB_SETTOOLTIPS, (WPARAM)(1), (LPARAM)(m_ToolTipsTable));
#else
	CDocList* pDocList = (CDocList*)FromHandle( pNotifyStruct->nmhdr.hwndFrom );
	ASSERT_KINDOF( CDocList, pDocList );

// nothing else works - no documentation on CDocListCommandBar methods
#endif

	CFrameWnd::OnCreateDocList( pNotifyStruct, result );
}			// OnCreateDocList()
//----------------------------------------------------------------------------
void CMainFrame::OnDestroyDocList( DLNHDR* pNotifyStruct, LRESULT* result )
{
	CFrameWnd::OnDestroyDocList(pNotifyStruct, result);	
}
//----------------------------------------------------------------------------
*/
void CMainFrame::OnDestroy( void )
{
#if ( PPC02_TARGET )
//	for ( int ii = 0; ii < NUM_TOOL_TIPS; ii++ )
//		delete	m_ToolTipsTable[ii];
#endif
	CFrameWnd::OnDestroy();
}			// OnDestroy()
//----------------------------------------------------------------------------
#if PPC02_TARGET
LPTSTR CMainFrame::MakeString( UINT stringID )
{
	TCHAR buffer[255];
	TCHAR* theString;

	LoadString(AfxGetInstanceHandle(), stringID, buffer, 255);
	theString = new TCHAR[lstrlen(buffer) + 1];
	lstrcpy(theString, buffer);
	return	theString;
}			// MakeString()
#endif
//----------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}			// AssertValid()
//----------------------------------------------------------------------------
/*
void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump( dc );
}			// Dump()
*/
//----------------------------------------------------------------------------

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

//--------------------------------------------------------------------------------------
//									Update UI Messages
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateFileOpenDatabase( CCmdUI* pCmdUI )
{
	bool	enableIt = theApp.optMgr == NULL  ||  theApp.stkMgr == NULL;
	pCmdUI->Enable( enableIt );
}			// OnUpdateFileOpenDatabase()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateFileCloseDatabase( CCmdUI* pCmdUI )
{
	bool	enableIt = theApp.optMgr != NULL  ||  theApp.stkMgr != NULL;
	pCmdUI->Enable( enableIt );
}			// OnUpdateFileCloseDatabase()
//--------------------------------------------------------------------------------------
/*
void	CMainFrame::OnUpdateDefineBond( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( theApp.bndMgr != NULL );
}			// OnUpdateDefineBond()
*/
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateDefineOption( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( theApp.optMgr  &&  theApp.stkMgr );
}			// OnUpdateDefineOption()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateDefineStock( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( theApp.stkMgr != NULL );
}			// OnUpdateDefineStock()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateCreatePosition( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( theApp.optMgr  ||  theApp.stkMgr );
}			// OnUpdateCreatePosition()
//--------------------------------------------------------------------------------------
/*
void	CMainFrame::OnUpdateToolsBondAnalyzer( CCmdUI* pCmdUI )
{
	bool	enableIt = theApp.bndMgr  &&  theApp.bndMgr->GetSymbolTableSize() > 0;
	pCmdUI->Enable( enableIt );
}			// OnUpdateToolsBondAnalyzer()
*/
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateToolsHedgeExplorer( CCmdUI* pCmdUI ) 
{
	bool	enableIt = theApp.stkMgr  &&  theApp.optMgr  &&  theApp.optMgr->GetSymbolTableSize() > 0;
	pCmdUI->Enable( enableIt );
}			// OnUpdateToolsHedgeExplorer()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateToolsOptionAnalyzer( CCmdUI* pCmdUI )
{
	bool	enableIt = theApp.optMgr  &&  theApp.optMgr->GetSymbolTableSize() > 0;
	pCmdUI->Enable( enableIt );	
}			// OnUpdateToolsOptionAnalyzer()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateToolsOptionChainRetriever( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( theApp.optMgr  &&  theApp.stkMgr );
}		// OnUpdateToolsOptionChainRetriever()
//--------------------------------------------------------------------------------------
void CMainFrame::OnUpdateToolsTimeDecayExplorer( CCmdUI* pCmdUI )
{
	bool	enableIt = theApp.optMgr  &&  theApp.optMgr->GetSymbolTableSize() > 0;
	pCmdUI->Enable( enableIt );	
}			// OnUpdateToolsTimeDecayExplorer()
//--------------------------------------------------------------------------------------
void CMainFrame::OnUpdateToolsVolatilityValueExplorer(CCmdUI* pCmdUI) 
{
	bool	enableIt = theApp.optMgr  &&  theApp.optMgr->GetSymbolTableSize() > 0;
	pCmdUI->Enable( enableIt );	
}			// OnUpdateToolsVolatilityValueExplorer()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateToolsPortfolioNavigator( CCmdUI* pCmdUI )
{		// enable when there are positions in options or stocks
	bool	enableIt = theApp.optMgr  &&  theApp.optMgr->NumIndexedIssues() > 0;
	if ( ! enableIt )
		enableIt = theApp.stkMgr  &&  theApp.stkMgr->NumIndexedIssues() > 0;

	pCmdUI->Enable( enableIt );
}			// OnUpdateToolsPortfolioNavigator()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateToolsViewPositionTranscript( CCmdUI* pCmdUI )
{		// enable when there are positions in options or stocks
	bool	enableIt = theApp.optMgr  &&  theApp.optMgr->NumIndexedIssues() > 0;
	if ( ! enableIt )
		enableIt = theApp.stkMgr  &&  theApp.stkMgr->NumIndexedIssues() > 0;

	pCmdUI->Enable( enableIt );
}			// OnUpdateToolsViewPositionTranscript()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateToolsRateSensitivityExplorer(CCmdUI* pCmdUI) 
{		// enable when there are options or positions of either
		// since having a position requires having a definition,
		// we only need to consider whether there are definitions
	bool	enableIt = theApp.optMgr  &&  theApp.optMgr->GetSymbolTableSize() > 0;

	pCmdUI->Enable( enableIt );
}			// OnUpdateToolsRateSensitivityExplorer()
//--------------------------------------------------------------------------------------
//								Open/Close Database Messages
//--------------------------------------------------------------------------------------
void CMainFrame::OnFileCloseDatabase()
{
	if ( theApp.optMgr )
	{	delete	theApp.optMgr;
		theApp.optMgr = NULL;
	}
	if ( theApp.stkMgr )
	{	delete	theApp.stkMgr;
		theApp.stkMgr = NULL;
	}
/*
	if ( theApp.regMgr )
	{	delete	theApp.regMgr;
		theApp.regMgr = NULL;
	}
*/
}			// OnFileCloseDatabase()
//--------------------------------------------------------------------------------------
void CMainFrame::OnFileOpenDatabase()
{
	if ( ! theApp.optMgr )
		theApp.optMgr = new COptionManager( _T("\\Options") );

	if ( ! theApp.stkMgr )
		theApp.stkMgr = new CStockManager( _T("\\Stocks") );

//	if ( ! theApp.regMgr )
//		theApp.regMgr = new CRegistryManager;

}			// OnFileOpenDatabase()
//--------------------------------------------------------------------------------------
//							Define Instrument/Position Messages
//--------------------------------------------------------------------------------------
/*
void CMainFrame::OnDefineBond( void )
{		// Define a Bond...
	CBondDefinitionDialog	dlg;
	int nDisposition = dlg.DoModal();

	bool	res = theApp.regMgr->SetRecentInstrument( Bond );
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnDefineBond: SetRecentInstrument failed.\n") );
#endif

		// save the recent bond's definition ID
	if ( dlg.m_BondSymbol != _T("") )
	{	long	def_ID = theApp.bndMgr->AssetExists( dlg.m_BondSymbol );
		if ( def_ID >= 0 )
		{	res = theApp.regMgr->SetRecentBond( def_ID );
#ifdef _DEBUG
			if ( ! res )
				TRACE( _T("MainFrame::OnDefineBond: SetRecentBond failed.\n") );
#endif
		}
	}

	if ( nDisposition == IDOK )
	{		// SaveIfChanged() becomes SaveIfChanged(false)
			// false tells it not to update the Symbol's ComboBox,
			// but also allows it to differentiate between callers.
			//   false --> caller is MainFrame, thus a close dialog event.
			//    true --> caller is the BondDefinitionDialog, thus a SelChange event.
			// calling SaveIfChanged(true) here will produce an access violation
		dlg.SaveIfChanged();		
		SetForegroundWindow();		// SaveIfChanged() messes up the Z-order
	}
}			// OnDefineBond()
*/
//--------------------------------------------------------------------------------------
void CMainFrame::OnDefineOption( void )
{		// Define an Option (StockSymbol, StrikePrice, PutCall, Expiry, ...)
	COptionDefinitionDialog dlg;
	int nDisposition = dlg.DoModal();

	bool	res = SetRecentInstrument( Option );			// was:  theApp.regMgr->
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnDefineOption: SetRecentInstrument failed.\n") );
#endif

		// save the recent option's definition ID
	if ( dlg.m_OptionSymbol != _T("") )
	{	long	def_ID = theApp.optMgr->AssetExists( dlg.m_OptionSymbol );
		if ( def_ID >= 0 )
		{	res = SetRecentOption( def_ID );				// was:  theApp.regMgr->
#ifdef _DEBUG
			if ( ! res )
				TRACE( _T("MainFrame::OnDefineOption: SetRecentOption failed.\n") );
#endif
		}
	}

	if (  nDisposition == IDOK )
	{		// SaveIfChanged() becomes SaveIfChanged(false)
			// false tells it not to update the Symbol's ComboBox,
			// but also allows it to differentiate between callers.
			//   false --> caller is MainFrame, thus a close dialog event.
			//    true --> caller is the OptionDefinitionDialog, thus a SelChange event.
			// calling SaveIfChanged(true) here will produce an access violation
		dlg.SaveIfChanged();
		SetForegroundWindow();		// SaveIfChanged() messes up the Z-order
	}
}			// OnDefineOption()
//--------------------------------------------------------------------------------------
void CMainFrame::OnDefineStock( void )
{		// Define a Stock (Volatility, Symbol, Dividends, ...)
	CStockDefinitionDialog	dlg;
	int nDisposition = dlg.DoModal();

	bool	res = SetRecentInstrument( Stock );		// was:  theApp.regMgr->
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnDefineStock: SetRecentInstrument failed.\n") );
#endif

		// save the recent option's definition ID
	if ( dlg.m_StockSymbol != _T("") )
	{	long	def_ID = theApp.stkMgr->AssetExists( dlg.m_StockSymbol );
		if ( def_ID >= 0 )
		{	res = SetRecentStock( def_ID );			// was:  theApp.regMgr->
#ifdef _DEBUG
			if ( ! res )
				TRACE( _T("MainFrame::OnDefineStock: SetRecentStock failed.\n") );
#endif
		}
	}

	if ( nDisposition == IDOK )
	{		// SaveIfChanged() becomes SaveIfChanged(false)
			// false tells it not to update the Symbol's ComboBox,
			// but also allows it to differentiate between callers.
			//   false --> caller is MainFrame, thus a close dialog event.
			//    true --> caller is the StockDefinitionDialog, thus a SelChange event.
			// calling SaveIfChanged(true) here will produce an access violation
		dlg.SaveIfChanged();
		SetForegroundWindow();		// SaveIfChanged() messes up the Z-order
	}
}			// OnDefineStock()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnDefineConfirmSaveOptions( void )
{	CConfirmSavePreferencesDialog	dlg;
	int	nDisposition = dlg.DoModal();

	if ( nDisposition != IDOK )
		return;

	short	prefs = 0;
	if ( dlg.m_DefinitionClose )		prefs |= DefinitionClose;
	if ( dlg.m_DefinitionChangeSymbol )	prefs |= DefinitionChangeSymbol;
	if ( dlg.m_AnalyzerClose )			prefs |= AnalyzerClose;
	if ( dlg.m_AnalyzerChangeSymbol )	prefs |= AnalyzerChangeSymbol;
	if ( dlg.m_PositionsDelete )		prefs |= PositionsDelete;
	if ( dlg.m_PositionsModify )		prefs |= PositionsModify;
	if ( dlg.m_OptionChainStockPriceUpdateVerify )
		prefs |= OptionChainStockPriceUpdateVerify;

	bool	res = SetSaveVerifyPrefs( prefs );				// was:  theApp.regMgr->

#ifdef _DEBUG
	if ( ! res )
	{	TRACE( _T("MainFrame::OnDefineConfirmSaveOptions: SetSaveVerifyPrefs failed\n") );
		AfxDebugBreak();
	}
#endif
}			// OnDefineConfirmSaveOptions()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnCreateDisplayPreferences( void )
{		// Set display preferences for the existing PositionList that
		// appears at the bottom of the PositionDialog
	CPositionListOptionsDialog dlg;
	int nDisposition = dlg.DoModal();
	if ( nDisposition != IDOK  )
		return;

	DWORD	areVisible = 0;
	if ( dlg.m_AggregateIncome	 ) areVisible |= AggregateIncome;
	if ( dlg.m_AnnualizedNetGain ) areVisible |= AnnualizedNetGain;
	if ( dlg.m_CapitalGain		 ) areVisible |= CapitalGain;
	if ( dlg.m_ExerciseValue	 ) areVisible |= ExerciseValue;
	if ( dlg.m_InitialCost		 ) areVisible |= InitialCost;
	if ( dlg.m_MarketValue		 ) areVisible |= MarketValue;
	if ( dlg.m_NetGain			 ) areVisible |= NetGain;
	if ( dlg.m_Note				 ) areVisible |= Note;
	if ( dlg.m_NumUnits			 ) areVisible |= NumUnits;
	if ( dlg.m_PurchaseDate		 ) areVisible |= PurchaseDate;

	if ( dlg.m_AggregateIncomeOverview	  ) areVisible |= AggregateIncomeOverview;
	if ( dlg.m_AnnualizedNetGainOverview  ) areVisible |= AnnualizedNetGainOverview;
	if ( dlg.m_CapitalGainOverview		  ) areVisible |= CapitalGainOverview;
	if ( dlg.m_ExerciseValueOverview	  ) areVisible |= ExerciseValueOverview;
	if ( dlg.m_InitialCostOverview		  ) areVisible |= InitialCostOverview;
	if ( dlg.m_MarketValueOverview		  ) areVisible |= MarketValueOverview;
	if ( dlg.m_NetGainOverview			  ) areVisible |= NetGainOverview;
	if ( dlg.m_NoteOverview				  ) areVisible |= NoteOverview;
	if ( dlg.m_NumUnitsOverview			  ) areVisible |= NumUnitsOverview;
	if ( dlg.m_PurchaseDateOverview		  ) areVisible |= PurchaseDateOverview;

	if ( dlg.m_InitiallySuppressPositions ) areVisible |= InitiallySuppressPositions;

	bool	res = SetPosListColVisibility( areVisible );				// was:  theApp.regMgr->

#ifdef _DEBUG
	if ( ! res )
	{	TRACE( _T("MainFrame::OnCreateDisplayPreferences: SetPosListColVisibility failed\n") );
		AfxDebugBreak();
	}
#endif
}			// OnCreateDisplayPreferences()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnCreatePosition( void )
{	CPositionsDialog	dlg;
	int nDisposition = dlg.DoModal();
	
		// register the rececnt instrument
	if ( dlg.instrument != UnknownAssetType )
	{	bool	res = SetRecentInstrument( dlg.instrument );			// was:  theApp.regMgr->
#ifdef _DEBUG
		if ( ! res )
			TRACE( _T("MainFrame::OnCreatePosition: SetRecentInstrument failed.\n") );
#endif
			// perhaps we can also register the rececnt issue
		if ( dlg.theAsset )
		{	long def_ID = dlg.theAsset->getDef_ID();
			switch ( dlg.instrument )
			{	case Option:
						// save the recent option's definition ID
					res = SetRecentOption( def_ID );					// was:  theApp.regMgr->
#ifdef _DEBUG
					if ( ! res )
						TRACE( _T("MainFrame::OnCreatePosition: SetRecentOption failed.\n") );
#endif
					break;
				case Stock:
						// save the recent stock's definition ID
					res = SetRecentStock( def_ID );						// was:  theApp.regMgr->
#ifdef _DEBUG
					if ( ! res )
						TRACE( _T("MainFrame::OnCreatePosition: SetRecentStock failed.\n") );
#endif
					break;
				default:
					ASSERT( false );
					break;
			}
		}
	}

	if ( nDisposition != IDOK )
		return;

		// Enabling AddPosition() below adds a position without hitting the Enter button
		// if the dialog Edit boxes have enough information to create the position.
		// For now, we'll force the user to hit the Enter button to accomplish that.
//	CPosition* pos = dlg.AddPosition();			// handles writing to the pDB
		// no need to delete pos because the dialog destructor deletes the entire position list
}			// OnCreatePosition()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnToolsOptionAnalyzer( void )
{	COptionAnalyzerDialog	dlg;
		// XXX is the following still true
		// this function is similar to COptionValueDialog::OnAnalyticalAssessment()
		// except we have no local variables to update and no OptionSymbol to preset
	int nDisposition = dlg.DoModal();

		// the most recently used instrument is now Option
	bool	res = SetRecentInstrument( Option );					// was:  theApp.regMgr->
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsOptionAnalyzer: SetRecentInstrument failed.\n") );
#endif

		// save the recent option's definition ID
	if ( dlg.theOption )
	{	res = SetRecentOption( dlg.theOption->getDef_ID() );		// was:  theApp.regMgr->
#ifdef _DEBUG
		if ( ! res )
			TRACE( _T("MainFrame::OnToolsOptionAnalyzer: SetRecentOption failed.\n") );
#endif
	}
		// riskFreeRate avoids modification by cancelling the dialog.  Everything else
		// sets the modify flag at the point of modification (nonstandard behavior)!
		// update the portfolio's risk free rate
	res = SetRiskFreeRate( dlg.m_RiskFreeRate );					// was:  theApp.regMgr->
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsOptionAnalyzer: SetRiskFreeRate failed.\n") );
#endif

		// SaveIfChanged() can mess up window z-order
	if ( nDisposition == IDOK )
	{	dlg.SaveIfChanged();
		SetForegroundWindow();			// SaveIfChanged messes up the Z-order ...
	}
}			// OnToolsOptionAnalyzer()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnToolsOptionChainRetriever( void )
{
	COptionChainDialog	dlg;
	int	nDisposition = dlg.DoModal();
//	dlg.SaveIfChanged();		// save company name if the stock exists and desc is blank

		// set recent instrument to Option
	bool	res = SetRecentInstrument( Option );					// was:  theApp.regMgr->
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsOptionChainRetriever: SetRecentInstrument failed.\n") );
#endif

		// save the most recent stock's definition ID
	if ( dlg.m_StockSymbol != _T("") )
	{	long def_ID = theApp.stkMgr->AssetExists( dlg.m_StockSymbol );
		res = SetRecentStock( def_ID );								// was:  theApp.regMgr->
#ifdef _DEBUG
		if ( ! res )
			TRACE( _T("MainFrame::OnToolsOptionChainRetriever: SetRecentStock failed.\n") );
#endif
	}
}			// OnToolsOptionChainRetriever()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnToolsPortfolioNavigator( void )
{
	CPortfolioOverviewDialog	dlg;
	int nDisposition = dlg.DoModal();
}			// OnToolsPortfolioNavigator()
//--------------------------------------------------------------------------------------
void CMainFrame::OnToolsViewPositionTranscript( void ) 
{
	CPositionTranscriptDialog	dlg;
	int nDisposition = dlg.DoModal();
}			// OnToolsViewPositionTranscript()
//--------------------------------------------------------------------------------------
void CMainFrame::OnToolsHedgeExplorer( void )
{
	CHedgeExplorer	dlg;
	int nDisposition = dlg.DoModal();

		// always save the state of the hedge analyzer (cancelled or not)
	bool	res = SetHedge( dlg.os3oh );						// was:  theApp.regMgr->
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsHedgeExplorer(1): SetHedge failed.\n") );
#endif

		// riskFreeRate avoids modification by cancelling the dialog.  Everything else
		// sets the modify flag at the point of modification (nonstandard behavior)!
		// update the portfolio's risk free rate
		// get MainFrame back on top after the MessageBox closes
	res = SetRiskFreeRate( dlg.m_RiskFreeRate );				// was:  theApp.regMgr->
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsHedgeExplorer(2): SetRiskFreeRate failed.\n") );
#endif

		// save changes to the stock price and its volatility
	if ( nDisposition == IDOK )
	{	dlg.SaveIfChanged();
		SetForegroundWindow();				// SaveIfChanged() messes up the Z-order...
	}
}			// OnToolsHedgeExplorer()
//--------------------------------------------------------------------------------------
void CMainFrame::OnToolsRateSensitivityExplorer( void )
{	bool	res;
	CRateSensitivityExplorer	dlg;
	int nDisposition = dlg.DoModal();

		// the most recent instrument is now Option
	res = SetRecentInstrument( Option );						// was:  theApp.regMgr->
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsRateSensitivityExplorer(1): SetRecentInstrument failed.\n") );
#endif
		// riskFreeRate avoids modification by canceling the dialog.  Everything else
		// sets the modify flag at the point of modification (nonstandard behavior)!
		// update the portfolio's risk free rate
		// get MainFrame back on top after the MessageBox closes
	res = SetRiskFreeRate( dlg.m_RiskFreeRate );				// was:  theApp.regMgr->
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsRateSensitivityExplorer(2): SetRiskFreeRate failed.\n") );
#endif

	dlg.rateSens.deltaLow = dlg.m_DeltaLow;
	dlg.rateSens.deltaHigh = dlg.m_DeltaHigh;
	res = SetOptionRateSensitivity( dlg.rateSens );				// was:  theApp.regMgr->
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsRateSensitivityExplorer(3): SetRateSensitivity failed.\n") );
#endif
//	SetForegroundWindow();				// a failed experiment
}			// OnToolsRateSensitivityExplorer()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnToolsTimeDecayExplorer( void )
{	bool	res;
	CTimeDecayExplorer	dlg;
	int nDisposition = dlg.DoModal();

		// riskFreeRate avoids modification by cancelling the dialog.  Everything else
		// sets the modify flag at the point of modification (nonstandard behavior)!
		// update the portfolio's risk free rate
		// get MainFrame back on top after the MessageBox closes
	res = SetRiskFreeRate( dlg.m_RiskFreeRate );				// was:  theApp.regMgr->
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsTimeDecayExplorer(1): SetRiskFreeRate failed.\n") );
#endif

	res = SetTimeDecay( dlg.timeDecay );						// was:  theApp.regMgr->
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsTimeDecayExplorer(2): SetTimeDecay failed.\n") );
#endif
//	SetForegroundWindow();				// a failed experiment
}			// OnToolsTimeDecayExplorer()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnToolsVolatilityValueExplorer( void )
{	bool	res;
	CVolatilityExplorer	dlg;
	int nDisposition = dlg.DoModal();

		// riskFreeRate avoids modification by cancelling the dialog.  Everything else
		// sets the modify flag at the point of modification (nonstandard behavior)!
		// update the portfolio's risk free rate
		// get MainFrame back on top after the MessageBox closes
	res = SetRiskFreeRate( dlg.m_RiskFreeRate );				// was:  theApp.regMgr->
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsVolatilityValueExplorer(1): SetRiskFreeRate failed.\n") );
#endif

	res = SetVolatilityExplorerState( dlg.volExpState );		// was:  theApp.regMgr->
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsVolatilityValueExplorer(2): SetVolatilityExplorerState failed.\n") );
#endif
}			// OnToolsVolatilityValueExplorer()
//--------------------------------------------------------------------------------------
/*
void	CMainFrame::OnToolsBondAnalyzer( void )
{	CBondAnalyzerDialog	dlg;
		// this function is similar to COptionValueDialog::OnAnalyticalAssessment()
		// except we have no local variables to update and no OptionSymbol to preset
	int nDisposition = dlg.DoModal();

		// the most recently used instrument is now Bond
	bool	res = theApp.regMgr->SetRecentInstrument( Bond );
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsBondAnalyzer: SetRecentInstrument failed.\n") );
#endif

		// save the recent bond's definition ID
	if ( dlg.theBond )
	{	res = theApp.regMgr->SetRecentBond( dlg.theBond->getDef_ID() );
#ifdef _DEBUG
		if ( ! res )
			TRACE( _T("MainFrame::OnToolsBondAnalyzer: SetRecentBond failed.\n") );
#endif
	}

		// save the DeltaYTM
	res = theApp.regMgr->SetDeltaYTM( dlg.m_DeltaYTM );
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsBondAnalyzer: SetDeltaYTM failed.\n") );
#endif

	if ( nDisposition == IDOK )
	{	dlg.SaveIfChanged();
			// SaveIfChanged() becomes SaveIfChanged(false)
			// false tells it not to update the Symbol's ComboBox,
			// but also allows it to differentiate between callers.
			//   false --> caller is MainFrame, thus a close dialog event.
			//    true --> caller is the BondDefinitionDialog, thus a SelChange event.
			// calling SaveIfChanged(true) here will produce an access violation
		SetForegroundWindow();			// SaveIfChanged() messes up the Z-order
	}
}			// OnToolsBondAnalyzer() 
*/
//--------------------------------------------------------------------------------------
/*			// create a seperate executable for the Spot Rate Estimator
void	CMainFrame::OnToolsSpotRateEstimator( void )
{	CSpotRateEstimatorDialog	dlg;
	int nDisposition = dlg.DoModal();
	if ( nDisposition != IDOK )
		return;

		// register the spot rate as the new RiskFreeRate (which has been rescaled)
	bool	res = theApp.pDB->setRiskFreeRate( dlg.spotRate );
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsSpotRateEstimator: Registry write failed, res=%s\n"),
				res ? _T("true") : _T("false") );
#endif

		// save the short rate calibration values
	dlg.SaveSRB();
}			// OnToolsSpotRateEstimator()
*/
//--------------------------------------------------------------------------------------
/*			// created a seperate executable for the Strategy Explorer
void CMainFrame::OnToolsStrategyExplorer( void ) 
{
	CStrategyExplorer		dlg;
	int nDisposition = dlg.DoModal();
		
		// always save the state of the strategy explorer (cancelled or not)
	OneStk3OptStrategy	os3os;
	for ( short ii = 0; ii < 4; ii++ )
	{	os3os.buySell[ii] = dlg.m_BuySell[ii];
		os3os.cost[ii]	  = dlg.m_Cost[ii];
		os3os.qty[ii]	  = dlg.m_Qty[ii];
		if ( ii < 3 )
		{	os3os.putCall[ii] = dlg.m_PutCall[ii];
			os3os.strike[ii] = dlg.m_Strike[ii];
//			os3os.exp[ii] = dlg.m_Exp[ii];
		}
	}
//	os3os.volatility = dlg.m_Volatility;
//	os3os.evalDate = dlg.m_EvalDate;
	bool	res = theApp.pDB->setRecentStrategy( os3os );
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsStrategyExplorer: Refistry write failed, res=%s\n"),
				res ? _T("true") : _T("false") );
#endif
}			// OnToolsStrategyExplorer( void )
*/
//--------------------------------------------------------------------------------------
/*
void	CMainFrame::OnDefineStockPutCallParity( void )
{		// Partially define a Stock a Put and a Call all at once
	CPutCallParityDialog	dlg;
	int nDisposition = dlg.DoModal();
	if ( nDisposition == IDOK )
	{	CFrameWnd* pFW = GetTopLevelFrame();
		CNillaHedgeDoc* pDoc = (CNillaHedgeDoc*)pFW->GetActiveDocument();

		CStock*	aStock = NULL;
		if ( m_StockSymbol != _T("") )
		{	aStock = (CStock*)dlg.getMapObject( pDoc->stocks, dlg.m_StockSymbol );
			if ( aStock == NULL )
			{		// didn't recognize this symbol, so this is a new entry
				aStock = new CStock( dlg.m_StockSymbol );
					// create an entry in the stocks map in the active document
				pDoc->stocks[ aStock->symbol ] = aStock;
			}
					// attributes coming from StockDefinitionDialog
				// (possibly to be provided by the Net in the future)
			ASSERT_VALID( aStock );
			aStock->mktPrice	= dlg.m_StockPrice;
			aStock->volatility	= dlg.m_Volatility;
		}

		COption*	aPut = NULL;
		if ( m_PutSymbol != _T("") )
		{	aPut = (COption*)dlg.getMapObject( pDoc->options, dlg.m_PutSymbol );
			if ( aPut == NULL )
			{		// didn't recognize this symbol, so this is a new entry
				aPut = new COption( dlg.m_PutSymbol );
					// create an entry in the stocks map in the active document
				pDoc->options[ aPut->optionSymbol ] = aPut;
			}
					// attributes coming from StockDefinitionDialog
				// (possibly to be provided by the Net in the future)
			ASSERT_VALID( aPut );
			aPut->putCall		= COption::Put;
			aPut->strikePrice	= dlg.m_StrikePrice;
			aPut->mktPrice		= dlg.m_PutPrice;
			aPut->expiry		= dlg.m_ExpiryDate;
			aPut->underlying	= aStock;
		}
		
		COption*	aCall = NULL;
		if ( m_CallSymbol != _T("") )
		{	aCall = (COption*)dlg.getMapObject( pDoc->options, dlg.m_CallSymbol );
			if ( aCall == NULL )
			{		// didn't recognize this symbol, so this is a new entry
				aCall = new COption( dlg.m_CallSymbol );
					// create an entry in the stocks map in the active document
				pDoc->options[ aCall->optionSymbol ] = aCall;
			}
					// attributes coming from StockDefinitionDialog
				// (possibly to be provided by the Net in the future)
			ASSERT_VALID( aCall );
			aCall->putCall		= COption::Call;
			aCall->strikePrice	= dlg.m_StrikePrice;
			aCall->mktPrice		= dlg.m_CallPrice;
			aCall->expiry		= dlg.m_ExpiryDate;
			aCall->underlying	= aStock;
		}
		if ( aStock  ||  aPut  ||  aCall )
		{		// indicate that the document has been modified
			pDoc->SetModifiedFlag();
			pDoc->UpdateAllViews( NULL );
		}
	}
}			// OnDefineStockPutCallParity()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnToolsHedgeCalculator( void )
{	CHedgeCalculatorDialog dlg;
	int nDisposition = dlg.DoModal();
	if ( nDisposition == IDOK )
	{		// Update various CStatic's presenting the results
			// What if user changes the OptionSymbol in the AssessmentDialog?   XXX
			// Consider posting it in 'readOnly' mode (can't change the OptionSymbol)
			// For now, changes in the child dialog, affect this dialog!

			// riskFreeRate is the only value captured by AnalyticalAssessment Dialog
			// that avoids modification by cancelling the dialog.  Everything else
			// sets the modify flag at the point of modification (nonstandard behavior)!
		CFrameWnd* pFW = GetTopLevelFrame();
		CNillaHedgeDoc* pDoc = (CNillaHedgeDoc*)pFW->GetActiveDocument();

			// update the portfolio's risk free rate
		pDoc->shortRateBasis.riskFreeRate = dlg.m_RiskFreeRate / 100.0);

			// update the Option's underlying Stock
		COption* anOption = (COption*)dlg.getMapObject( pDoc->options, dlg.m_OptionSymbol );
		if ( anOption )
		{	CStock* theStock = (CStock*)dlg.getMapObject( pDoc->stocks, anOption->stockSymbol );
			if ( theStock )
			{	theStock->mktPrice = dlg.m_MarketPrice;
				theStock->volatility = dlg.m_Volatility;
			}
		}

			// indicate that the document has been modified
			// even though we can't be sure that any of the values above
			// have actually changed
		pDoc->SetModifiedFlag();
		pDoc->UpdateAllViews( NULL );
	}
}			// OnToolsHedgeCalculator()
//--------------------------------------------------------------------------------------
*/

