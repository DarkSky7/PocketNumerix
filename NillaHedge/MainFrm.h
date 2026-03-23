// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__6CC7283A_1771_4196_A703_25ACAC67830F__INCLUDED_)
#define AFX_MAINFRM_H__6CC7283A_1771_4196_A703_25ACAC67830F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// Array for the toolbar buttons

#if (_WIN32_WCE < 201)
/*
static TBBUTTON g_arCBButtons[] = {
	{ 0,	ID_FILE_NEW,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0, 0,  0},
	{ 1,    ID_FILE_OPEN,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0, 0,  1},
	{ 2,	ID_FILE_SAVE,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0, 0,  2},
	{ 0,	0,				TBSTATE_ENABLED, TBSTYLE_SEP,		0, 0, 0, -1},
	{ 3,    ID_EDIT_CUT,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0, 0,  3},
	{ 4,	ID_EDIT_COPY,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0, 0,  4},
	{ 5,	ID_EDIT_PASTE,	TBSTATE_ENABLED, TBSTYLE_BUTTON,	0, 0, 0,  5}
};
*/
#endif

#if defined(_WIN32_WCE_PSPC) && (_WIN32_WCE >= 212)
#define NUM_TOOL_TIPS 8
#endif

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	CMainFrame();

#if ( PPC03_TARGET  ||  WM5_TARGET )
	// this method isn't necessary in MFC3.0, but it's not part of MFC8.0. 
	// We're approximatng MFC8.0 using the platform, but the real condition
	// would examine the version of MFC available in the build
	// Even in MFC3.0, it's an undocumented function.  
	void	CMainFrame::ActivateTopParent( void );
#endif

// Operations
public:
//	CBOSDatabase*	GetDB( void )	{	return	(CBOSDatabase*)GetTopLevelFrame()->GetActiveDocument();		}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
//	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
#if ( PPC02_TARGET )
	CCeCommandBar	m_wndCommandBar;			// EVT3
#else
	CCommandBar		m_wndCommandBar;			// VS8
#endif
//	CToolBar		m_wndToolBar;
//	CDialogBar		m_wndDlgBar;
//	CReBar			m_wndReBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int	 OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg void OnCreateDocList(DLNHDR* pNotifyStruct, LRESULT* result);
//	afx_msg void OnDestroyDocList(DLNHDR* pNotifyStruct, LRESULT* result);
	afx_msg void OnDestroy();
//	afx_msg void OnDefineBond();
	afx_msg void OnDefineConfirmSaveOptions();
	afx_msg void OnDefineOption();
	afx_msg void OnDefineStock();
	afx_msg void OnCreateDisplayPreferences();
	afx_msg void OnCreatePosition();
//	afx_msg void OnToolsBondAnalyzer();
	afx_msg void OnToolsOptionAnalyzer();
	afx_msg void OnToolsPortfolioNavigator();
	afx_msg void OnToolsViewPositionTranscript();
	afx_msg void OnToolsOptionChainRetriever();
	afx_msg void OnFileCloseDatabase();
	afx_msg void OnFileOpenDatabase();
	afx_msg void OnUpdateFileCloseDatabase(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpenDatabase(CCmdUI* pCmdUI);
//	afx_msg void OnUpdateDefineBond(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDefineOption(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDefineStock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCreatePosition(CCmdUI* pCmdUI);
//	afx_msg void OnUpdateToolsBondAnalyzer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolsOptionAnalyzer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolsOptionChainRetriever(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolsPortfolioNavigator(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolsViewPositionTranscript(CCmdUI* pCmdUI);
	afx_msg void OnToolsHedgeExplorer();
	afx_msg void OnUpdateToolsHedgeExplorer(CCmdUI* pCmdUI);
	afx_msg void OnToolsRateSensitivityExplorer();
	afx_msg void OnUpdateToolsRateSensitivityExplorer(CCmdUI* pCmdUI);
	afx_msg void OnToolsTimeDecayExplorer();
	afx_msg void OnUpdateToolsTimeDecayExplorer(CCmdUI* pCmdUI);
	afx_msg void OnToolsVolatilityValueExplorer();
	afx_msg void OnUpdateToolsVolatilityValueExplorer(CCmdUI* pCmdUI);
//	afx_msg BOOL OnIconNotify();										// ON_COMMAND never gets called
	afx_msg LRESULT OnIconNotify( WPARAM, LPARAM );						// ON_MESSAGE format
	afx_msg LRESULT OnNillaHedgeAlreadyRunning( WPARAM, LPARAM );
	//}}AFX_MSG

//	LPTSTR m_ToolTipsTable[NUM_TOOL_TIPS];			// may have to reinvigorate ToolTips
#if PPC02_TARGET
	LPTSTR MakeString(UINT stringID);				// supports ToolTips too
#endif
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__6CC7283A_1771_4196_A703_25ACAC67830F__INCLUDED_)
