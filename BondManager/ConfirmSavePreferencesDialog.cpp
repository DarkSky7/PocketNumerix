// ConfirmSavePreferencesDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "BondManagerApp.h"
//#include "BOSDatabase.h"
#include "ConfirmSavePreferencesDialog.h"
#include "RegistryManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern	CBondManagerApp	theApp;

/////////////////////////////////////////////////////////////////////////////
// CConfirmSavePreferencesDialog dialog

CConfirmSavePreferencesDialog::CConfirmSavePreferencesDialog( CWnd* pParent /*=NULL*/ )
	: CNillaDialog( CConfirmSavePreferencesDialog::IDD, pParent )
{
	//{{AFX_DATA_INIT(CConfirmSavePreferencesDialog)	m_AnalyzerChangeSymbol = FALSE;
	m_AnalyzerClose = TRUE;
	m_AnalyzerChangeSymbol = TRUE;
	m_DefinitionClose = TRUE;
	m_DefinitionChangeSymbol = TRUE;
	m_PositionsDelete = TRUE;
	m_PositionsModify = TRUE;
//	m_OptionChainStockPriceUpdateVerify = FALSE;
	//}}AFX_DATA_INIT
}

void CConfirmSavePreferencesDialog::DoDataExchange( CDataExchange* pDX )
{
	CNillaDialog::DoDataExchange( pDX );
	//{{AFX_DATA_MAP(CConfirmSavePreferencesDialog)
//	DDX_Control(pDX, IDC_OptionChainStockPriceUpdateVerify, c_OptionChainStockPriceUpdateVerify );
	DDX_Control(pDX, IDC_AnalyzerDialogCloseVerify, c_AnalyzerClose );
	DDX_Control(pDX, IDC_AnalyzerDialogChangeSymbolVerify, c_AnalyzerChangeSymbol );
	DDX_Control(pDX, IDC_DefinitionDialogCloseVerify, c_DefinitionClose );
	DDX_Control(pDX, IDC_DefinitionDialogChangeSymbolVerify, c_DefinitionChangeSymbol );
	DDX_Control(pDX, IDC_PositionsDialogDeletePositionVerify, c_PositionsDelete );
	DDX_Control(pDX, IDC_PositionsDialogModifyPositionVerify, c_PositionsModify );
	DDX_Check(pDX, IDC_AnalyzerDialogCloseVerify, m_AnalyzerClose );
	DDX_Check(pDX, IDC_AnalyzerDialogChangeSymbolVerify, m_AnalyzerChangeSymbol );
	DDX_Check(pDX, IDC_DefinitionDialogCloseVerify, m_DefinitionClose );
	DDX_Check(pDX, IDC_DefinitionDialogChangeSymbolVerify, m_DefinitionChangeSymbol );
	DDX_Check(pDX, IDC_PositionsDialogDeletePositionVerify, m_PositionsDelete );
	DDX_Check(pDX, IDC_PositionsDialogModifyPositionVerify, m_PositionsModify );
//	DDX_Check(pDX, IDC_OptionChainStockPriceVerify, m_OptionChainStockPriceUpdateVerify );
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( CConfirmSavePreferencesDialog, CNillaDialog )
	//{{AFX_MSG_MAP(CConfirmSavePreferencesDialog)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfirmSavePreferencesDialog message handlers

BOOL CConfirmSavePreferencesDialog::OnInitDialog() 
{
	CNillaDialog::OnInitDialog();

		// by default, there's no confirmation required for saves/updates, but
		// the default behavior requires confirmation for Closing or Deleting a Position
	short	prefs = AnalyzerClose	| AnalyzerChangeSymbol
				  |	DefinitionClose | DefinitionChangeSymbol
				  |	PositionsDelete | PositionsModify;

		// note that OptionChainStockPriceUpdateVerify is NOT or'd in above...
		// the default is not to include it.

	if ( ! GetSaveVerifyPrefs( &prefs ) )
	{		// stick with the constructor defaults
#ifdef _DEBUG
		TRACE( _T("ConfirmSavePreferencesDialog::OnInitDialog: no registered preferences found.\n") );
#endif
	}
	else
	{		// 'unpack' the registered Save Verify preferences
		m_AnalyzerClose			 = prefs & AnalyzerClose;
		m_AnalyzerChangeSymbol	 = prefs & AnalyzerChangeSymbol;
	
		m_DefinitionClose		 = prefs & DefinitionClose;
		m_DefinitionChangeSymbol = prefs & DefinitionChangeSymbol;

		m_PositionsDelete		 = prefs & PositionsDelete;
		m_PositionsModify		 = prefs & PositionsModify;

//		m_OptionChainStockPriceUpdateVerify = prefs & OptionChainStockPriceUpdateVerify;
	}
		// set the state of associated controls
	c_AnalyzerClose.SetCheck(		   m_AnalyzerClose			? BST_CHECKED : BST_UNCHECKED );
	c_AnalyzerChangeSymbol.SetCheck(   m_AnalyzerChangeSymbol	? BST_CHECKED : BST_UNCHECKED );

	c_DefinitionClose.SetCheck(		   m_DefinitionClose		? BST_CHECKED : BST_UNCHECKED );
	c_DefinitionChangeSymbol.SetCheck( m_DefinitionChangeSymbol ? BST_CHECKED : BST_UNCHECKED );

	c_PositionsDelete.SetCheck(		   m_PositionsDelete		? BST_CHECKED : BST_UNCHECKED );
	c_PositionsModify.SetCheck(		   m_PositionsModify		? BST_CHECKED : BST_UNCHECKED );

//	c_OptionChainStockPriceUpdateVerify.SetCheck( m_OptionChainStockPriceUpdateVerify ? BST_CHECKED : BST_UNCHECKED );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//--------------------------------------------------------------------------
HBRUSH CConfirmSavePreferencesDialog::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	return	CNillaDialog::OnCtlColor( pDC, pWnd, nCtlColor );
}			// OnCtlColor()
//--------------------------------------------------------------------------
