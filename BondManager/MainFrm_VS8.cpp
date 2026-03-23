// MainFrm.cpp : implementation of the CMainFrame class
//

#include "StdAfx.h"
#include "BondManager.h"
//#include "OneStk3OptStrategy.h"
//#include "BOSDatabase.h"
//#include "NillaHedgeDoc.h"
#include "MainFrm.h"
#include "BitArray.h"
//#include "Stock.h"
//#include "Option.h"
#include "Bond.h"
#include "Position.h"

#include "BondDefinitionDialog.h"
//#include "OptionDefinitionDialog.h"
//#include "StockDefinitionDialog.h"
#include "BondRateSensitivity.h"

#include "PortfolioOverviewDialog.h"
#include "PositionsDialog.h"

#include "BondAnalyzerDialog.h"
//#include "OptionAnalyzerDialog.h"
//#include "OptionChainDialog.h"
#include "PositionListOptionsDialog.h"
//#include "SpotRateEstimatorDialog.h"
#include "PositionTranscriptDialog.h"
//#include "StrategyExplorer.h"
//#include "HedgeExplorer.h"
#include "RateSensitivityExplorer.h"
//#include "TimeDecayExplorer.h"
//#include "VolatilityExplorer.h"

#include "ConfirmSavePreferencesDialog.h"
#include "PositionDisplayOptions.h"

//#include "OptionManager.h"
//#include "StockManager.h"
#include "RegistryManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PSPC_TOOLBAR_HEIGHT 24

const DWORD dwAdornmentFlags = 0; // exit button

#include "BondManagerApp.h"
extern	CBondManagerApp	theApp;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE( CMainFrame, CFrameWnd )

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
#if ( PPC02_TARGET )
	ON_NOTIFY(DLN_CE_CREATE, AFXCE_ID_DOCLIST, OnCreateDocList)				// MFC 3.0
	ON_NOTIFY(DLN_CE_DESTROY, AFXCE_ID_DOCLIST, OnDestroyDocList)			// MFC 3.0
#else
	ON_NOTIFY(DLN_DOCLIST_CREATE, AFX_ID_DOCLIST, OnCreateDocList)			// MFC 8.0
	ON_NOTIFY(DLN_DOCLIST_DESTROY, AFX_ID_DOCLIST, OnDestroyDocList)		// MFC 8.0 replacement of DLN_CE_DESTROY (documentation says DLN_CD_DESTROY)
#endif
		// accomplishes the commented out lines here
	ON_WM_DESTROY()
	ON_COMMAND(ID_DEFINE_BOND, OnDefineBond)
	ON_COMMAND(ID_DEFINE_CONFIRM_SAVE_OPTIONS, OnDefineConfirmSaveOptions)
	ON_COMMAND(ID_CREATE_DISPLAY_PREFERENCES, OnCreateDisplayPreferences)
	ON_COMMAND(ID_CREATE_POSITION, OnCreatePosition)
	ON_COMMAND(ID_TOOLS_BOND_ANALYZER, OnToolsBondAnalyzer)
	ON_COMMAND(ID_TOOLS_PORTFOLIO_NAVIGATOR, OnToolsPortfolioNavigator)
	ON_COMMAND(ID_TOOLS_VIEW_POSITION_TRANSCRIPT, OnToolsViewPositionTranscript)
	ON_COMMAND(ID_FILE_CLOSE_DATABASE, OnFileCloseDatabase)
	ON_COMMAND(ID_FILE_OPEN_DATABASE, OnFileOpenDatabase)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE_DATABASE, OnUpdateFileCloseDatabase)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN_DATABASE, OnUpdateFileOpenDatabase)
	ON_UPDATE_COMMAND_UI(ID_DEFINE_BOND, OnUpdateDefineBond)
	ON_UPDATE_COMMAND_UI(ID_CREATE_POSITION, OnUpdateCreatePosition)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_BOND_ANALYZER, OnUpdateToolsBondAnalyzer)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_PORTFOLIO_NAVIGATOR, OnUpdateToolsPortfolioNavigator)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_VIEW_POSITION_TRANSCRIPT, OnUpdateToolsViewPositionTranscript)
	ON_COMMAND(ID_TOOLS_RATE_SENSITIVITY_EXPLORER, OnToolsRateSensitivityExplorer)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_RATE_SENSITIVITY_EXPLORER, OnUpdateToolsRateSensitivityExplorer)
//	ON_COMMAND( WM_ICON_NOTIFY, OnIconNotify )		// never gets called
	ON_MESSAGE( WM_ICON_NOTIFY, OnIconNotify )		// this gets called !!!
	ON_REGISTERED_MESSAGE( WM_BONDMANAGER_ALREADY_RUNNING, OnBondManagerAlreadyRunning)
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
	if (pActiveWnd == NULL ||
		!(pActiveWnd->m_hWnd == m_hWnd || ::IsChild(pActiveWnd->m_hWnd, m_hWnd)))
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
		ActivateTopParent();
	}
	return	1;							// indicates we processed the message ?
}			// OnIconNotify()
//----------------------------------------------------------------------------
LRESULT		CMainFrame::OnBondManagerAlreadyRunning( WPARAM wp, LPARAM lp )
{
	return	WM_BONDMANAGER_ALREADY_RUNNING;
}			// OnBondManagerAlreadyRunning()
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
		ShowWindow( SW_SHOW );			// this is why this fundtion is here
	}
	return CFrameWnd::DefWindowProc( message, wp, lp );
}			// DefWindowProc()
//--------------------------------------------------------------------------------------
int		CMainFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if ( CFrameWnd::OnCreate(lpCreateStruct) == -1 )
	{	return -1;
	}
#if ( PPC02_TARGET )
	m_wndCommandBar.m_bShowSharedNewButton = FALSE;
	m_ToolTipsTable[0] = MakeString(IDS_NEW);
	m_ToolTipsTable[1] = MakeString(IDS_FILE);
	m_ToolTipsTable[2] = MakeString(IDS_MHELP);
	m_ToolTipsTable[3] = MakeString(IDS_CUT);
	m_ToolTipsTable[4] = MakeString(IDS_COPY);
	m_ToolTipsTable[5] = MakeString(IDS_PASTE);
#endif
	if (	!m_wndCommandBar.CreateEx(this)
		 || !m_wndCommandBar.InsertMenuBar(IDR_MAINFRAME) )
	{
		TRACE0("MainFrame::OnCreate: Failed to create menubar.");
		return -1;      // fail to create
	}
	return 0;
}			// OnCreate()
//----------------------------------------------------------------------------
BOOL	CMainFrame::PreCreateWindow( CREATESTRUCT& cs )
{	cs.style |= FWS_PREFIXTITLE;

	if ( !CFrameWnd::PreCreateWindow(cs) )
		return	FALSE;

		// TODO: Modify the Window class or styles here by modifying
		//  the CREATESTRUCT cs
	return TRUE;
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
*/
//----------------------------------------------------------------------------
/*
void CMainFrame::OnDestroyDocList( DLNHDR* pNotifyStruct, LRESULT* result )
{
	CFrameWnd::OnDestroyDocList(pNotifyStruct, result);	
}
*/
//----------------------------------------------------------------------------
void CMainFrame::OnDestroy( void )
{
#if ( PPC02_TARGET )
	for(int i = 0; i < NUM_TOOL_TIPS; i++)
		delete m_ToolTipsTable[i];
#endif
	CFrameWnd::OnDestroy();
}			// OnDestroy()
//----------------------------------------------------------------------------
LPTSTR CMainFrame::MakeString( UINT stringID )
{
	TCHAR buffer[255];
	TCHAR* theString;

	LoadString(AfxGetInstanceHandle(), stringID, buffer, 255);
	theString = new TCHAR[lstrlen(buffer) + 1];
	lstrcpy(theString, buffer);
	return theString;
}			// MakeString()
//----------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
	void CMainFrame::AssertValid() const
	{		// this will compile
		CFrameWnd::AssertValid();
	}			// AssertValid()
	//----------------------------------------------------------------------------
/*
	void CMainFrame::Dump(CDumpContext& dc) const
	{		// this will not compile - CDumpContext isn't defined anywhere...
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
	bool	enableIt = theApp.bndMgr == NULL;
	pCmdUI->Enable( enableIt );
}			// OnUpdateFileOpenDatabase()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateFileCloseDatabase( CCmdUI* pCmdUI )
{
	bool	enableIt = theApp.bndMgr != NULL;
	pCmdUI->Enable( enableIt );
}			// OnUpdateFileCloseDatabase()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateDefineBond( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( theApp.bndMgr != NULL );
}			// OnUpdateDefineBond()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateCreatePosition( CCmdUI* pCmdUI )
{
	pCmdUI->Enable( theApp.bndMgr != NULL );
}			// OnUpdateCreatePosition()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateToolsBondAnalyzer( CCmdUI* pCmdUI )
{
	bool	enableIt = theApp.bndMgr  &&  theApp.bndMgr->GetSymbolTableSize() > 0;
	pCmdUI->Enable( enableIt );
}			// OnUpdateToolsBondAnalyzer()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateToolsPortfolioNavigator( CCmdUI* pCmdUI )
{		// enable when there are positions in bonds, options, or stocks
	bool	enableIt = theApp.bndMgr  &&  theApp.bndMgr->NumIndexedIssues() > 0;
	pCmdUI->Enable( enableIt );
}			// OnUpdateToolsPortfolioNavigator()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateToolsViewPositionTranscript( CCmdUI* pCmdUI )
{		// enable when there are positions in bonds, options, or stocks
	bool	enableIt = theApp.bndMgr  &&  theApp.bndMgr->NumIndexedIssues() > 0;
	pCmdUI->Enable( enableIt );
}			// OnUpdateToolsViewPositionTranscript()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnUpdateToolsRateSensitivityExplorer(CCmdUI* pCmdUI) 
{		// enable when there are bonds or options or positions of either
		// since having a position requires having a definition,
		// we only need to consider whether there are definitions
	bool	enableIt = theApp.bndMgr  &&  theApp.bndMgr->GetSymbolTableSize() > 0;
	pCmdUI->Enable( enableIt );
}			// OnUpdateToolsRateSensitivityExplorer()
//--------------------------------------------------------------------------------------
//								Open/Close Database Messages
//--------------------------------------------------------------------------------------
void CMainFrame::OnFileCloseDatabase()
{
	if ( theApp.bndMgr )
	{	delete	theApp.bndMgr;
		theApp.bndMgr = NULL;
	}
}			// OnFileCloseDatabase()
//--------------------------------------------------------------------------------------
void CMainFrame::OnFileOpenDatabase()
{
	if ( ! theApp.bndMgr )
		theApp.bndMgr = new CBondManager( _T("\\Bonds") );

}			// OnFileOpenDatabase()
//--------------------------------------------------------------------------------------
//							Define Instrument/Position Messages
//--------------------------------------------------------------------------------------
void CMainFrame::OnDefineBond( void )
{		// Define a Bond...
	CBondDefinitionDialog	dlg;
	int nDisposition = dlg.DoModal();
//	theApp.SetNID_HWND( GetSafeHwnd() );

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
		// save the recent bond's definition ID
	if ( dlg.m_BondSymbol != _T("") )
	{	long	def_ID = theApp.bndMgr->AssetExists( dlg.m_BondSymbol );
		if ( def_ID >= 0 )
		{	bool	res = SetRecentBond( def_ID );
#ifdef _DEBUG
			if ( ! res )
				TRACE( _T("MainFrame::OnDefineBond: SetRecentBond failed.\n") );
#endif
		}
	}
}			// OnDefineBond()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnDefineConfirmSaveOptions( void )
{	CConfirmSavePreferencesDialog	dlg;
	int	nDisposition = dlg.DoModal();
//	theApp.SetNID_HWND( GetSafeHwnd() );

	if ( nDisposition != IDOK )
		return;

	short	prefs = 0;
	if ( dlg.m_DefinitionClose )		prefs |= DefinitionClose;
	if ( dlg.m_DefinitionChangeSymbol )	prefs |= DefinitionChangeSymbol;
	if ( dlg.m_AnalyzerClose )			prefs |= AnalyzerClose;
	if ( dlg.m_AnalyzerChangeSymbol )	prefs |= AnalyzerChangeSymbol;
	if ( dlg.m_PositionsDelete )		prefs |= PositionsDelete;
	if ( dlg.m_PositionsModify )		prefs |= PositionsModify;
//	if ( dlg.m_OptionChainStockPriceUpdateVerify )
//		prefs |= OptionChainStockPriceUpdateVerify;

	bool	res = SetSaveVerifyPrefs( prefs );

#ifdef _DEBUG
	if ( ! res )
	{	TRACE( _T("MainFrame::OnDefineConfirmSaveOptions: SetSaveVerifyPrefs failed\n") );
		AfxDebugBreak();
	}
#endif
}			// OnDefineConfirmSaveOptions()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnCreateDisplayPreferences( void )
{		// Set display preferences for the existing PositionList that appears at the bottom
		// of the InitialBondPosition, InitialOptionPosition & InitialStockPosition dialogs
	CPositionListOptionsDialog dlg;
	int nDisposition = dlg.DoModal();
//	theApp.SetNID_HWND( GetSafeHwnd() );
	if ( nDisposition != IDOK  )
		return;

	DWORD	areVisible = 0;
	if ( dlg.m_AggregateIncome	 ) areVisible |= AggregateIncome;
	if ( dlg.m_AnnualizedNetGain ) areVisible |= AnnualizedNetGain;
	if ( dlg.m_CapitalGain		 ) areVisible |= CapitalGain;
//	if ( dlg.m_ExerciseValue	 ) areVisible |= ExerciseValue;
	if ( dlg.m_InitialCost		 ) areVisible |= InitialCost;
	if ( dlg.m_MarketValue		 ) areVisible |= MarketValue;
	if ( dlg.m_NetGain			 ) areVisible |= NetGain;
	if ( dlg.m_Note				 ) areVisible |= Note;
	if ( dlg.m_NumUnits			 ) areVisible |= NumUnits;
	if ( dlg.m_PurchaseDate		 ) areVisible |= PurchaseDate;

	if ( dlg.m_AggregateIncomeOverview	  ) areVisible |= AggregateIncomeOverview;
	if ( dlg.m_AnnualizedNetGainOverview  ) areVisible |= AnnualizedNetGainOverview;
	if ( dlg.m_CapitalGainOverview		  ) areVisible |= CapitalGainOverview;
//	if ( dlg.m_ExerciseValueOverview	  ) areVisible |= ExerciseValueOverview;
	if ( dlg.m_InitialCostOverview		  ) areVisible |= InitialCostOverview;
	if ( dlg.m_MarketValueOverview		  ) areVisible |= MarketValueOverview;
	if ( dlg.m_NetGainOverview			  ) areVisible |= NetGainOverview;
	if ( dlg.m_NoteOverview				  ) areVisible |= NoteOverview;
	if ( dlg.m_NumUnitsOverview			  ) areVisible |= NumUnitsOverview;
	if ( dlg.m_PurchaseDateOverview		  ) areVisible |= PurchaseDateOverview;

	if ( dlg.m_InitiallySuppressPositions ) areVisible |= InitiallySuppressPositions;

	bool	res = SetPosListColVisibility( areVisible );

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
//	theApp.SetNID_HWND( GetSafeHwnd() );
	
		// perhaps we can also register the rececnt issue
	if ( dlg.theAsset )
	{	long def_ID = dlg.theAsset->getDef_ID();
			// save the recent bond's definition ID
		bool	res = SetRecentBond( def_ID );
#ifdef _DEBUG
		if ( ! res )
			TRACE( _T("MainFrame::OnCreatePosition: SetRecentBond failed.\n") );
#endif
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
void	CMainFrame::OnToolsPortfolioNavigator( void )
{
	CPortfolioOverviewDialog	dlg;
	int nDisposition = dlg.DoModal();
//	theApp.SetNID_HWND( GetSafeHwnd() );
}			// OnToolsPortfolioNavigator()
//--------------------------------------------------------------------------------------
void CMainFrame::OnToolsViewPositionTranscript( void ) 
{
	CPositionTranscriptDialog	dlg;
	int nDisposition = dlg.DoModal();
//	theApp.SetNID_HWND( GetSafeHwnd() );
}			// OnToolsViewPositionTranscript()
//--------------------------------------------------------------------------------------
void CMainFrame::OnToolsRateSensitivityExplorer( void )
{
	CRateSensitivityExplorer	dlg;
	int nDisposition = dlg.DoModal();
//	theApp.SetNID_HWND( GetSafeHwnd() );

	dlg.rateSens.deltaLow = dlg.m_DeltaLow;
	dlg.rateSens.deltaHigh = dlg.m_DeltaHigh;
	bool	res = SetBondRateSensitivity( dlg.rateSens );
#ifdef _DEBUG
	if ( ! res )
		TRACE( _T("MainFrame::OnToolsRateSensitivityExplorer(3): SetBondRateSensitivity failed.\n") );
#endif
}			// OnToolsRateSensitivityExplorer()
//--------------------------------------------------------------------------------------
void	CMainFrame::OnToolsBondAnalyzer( void )
{
	CBondAnalyzerDialog	dlg;
		// this function is similar to COptionValueDialog::OnAnalyticalAssessment()
		// except we have no local variables to update and no OptionSymbol to preset
	int nDisposition = dlg.DoModal();
//	theApp.SetNID_HWND( GetSafeHwnd() );

		// save the recent bond's definition ID
	if ( dlg.theBond )
	{	bool	res = SetRecentBond( dlg.theBond->getDef_ID() );
#ifdef _DEBUG
		if ( ! res )
			TRACE( _T("MainFrame::OnToolsBondAnalyzer: SetRecentBond failed.\n") );
#endif
	}

		// save the DeltaYTM
	bool	res = SetDeltaYTM( dlg.m_DeltaYTM );
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
//--------------------------------------------------------------------------------------
