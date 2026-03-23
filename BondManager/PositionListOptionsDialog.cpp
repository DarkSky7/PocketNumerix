// PositionListOptionsDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "BondManagerApp.h"
//#include "BOSDatabase.h"
#include "PositionListOptionsDialog.h"
#include "RegistryManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC( CPositionListOptionsDialog, CNillaDialog )

extern	CBondManagerApp	theApp;

/////////////////////////////////////////////////////////////////////////////
// CPositionListOptionsDialog dialog

CPositionListOptionsDialog::CPositionListOptionsDialog(CWnd* pParent /* =NULL */ )
	: CNillaDialog( CPositionListOptionsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPositionListOptionsDialog)
	m_AggregateIncome = TRUE;
	m_AnnualizedNetGain = TRUE;
	m_CapitalGain = TRUE;
	m_InitialCost = TRUE;
	m_MarketValue = TRUE;
	m_NetGain = TRUE;
	m_Note = TRUE;
	m_NumUnits = TRUE;
	m_PurchaseDate = TRUE;
	m_AggregateIncomeOverview = TRUE;
	m_AnnualizedNetGainOverview = TRUE;
	m_CapitalGainOverview = TRUE;
	m_InitialCostOverview = TRUE;
	m_MarketValueOverview = TRUE;
	m_NetGainOverview = TRUE;
	m_NoteOverview = TRUE;
	m_NumUnitsOverview = TRUE;
	m_PurchaseDateOverview = TRUE;
//	m_ExerciseValue = TRUE;
//	m_ExerciseValueOverview = TRUE;
	m_InitiallySuppressPositions = TRUE;
	//}}AFX_DATA_INIT
}

void CPositionListOptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CNillaDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPositionListOptionsDialog)
	DDX_Control(pDX, IDC_InitiallySuppressPositions, c_InitiallySuppressPositions);
//	DDX_Control(pDX, IDC_ExerciseValueOverview, c_ExerciseValueOverview);
//	DDX_Control(pDX, IDC_ExerciseValue, c_ExerciseValue);
	DDX_Control(pDX, IDC_PurchaseDateOverview, c_PurchaseDateOverview);
	DDX_Control(pDX, IDC_NumUnitsOverview, c_NumUnitsOverview);
	DDX_Control(pDX, IDC_NoteOverview, c_NoteOverview);
	DDX_Control(pDX, IDC_NetGainOverview, c_NetGainOverview);
	DDX_Control(pDX, IDC_MarketValueOverview, c_MarketValueOverview);
	DDX_Control(pDX, IDC_InitialCostOverview, c_InitialCostOverview);
	DDX_Control(pDX, IDC_CapitalGainOverview, c_CapitalGainOverview);
	DDX_Control(pDX, IDC_AnnualizedNetGainOverview, c_AnnualizedNetGainOverview);
	DDX_Control(pDX, IDC_AggregateIncomeOverview, c_AggregateIncomeOverview);
	DDX_Control(pDX, IDC_PurchaseDate, c_PurchaseDate);
	DDX_Control(pDX, IDC_NumUnits, c_NumUnits);
	DDX_Control(pDX, IDC_Note, c_Note);
	DDX_Control(pDX, IDC_NetGain, c_NetGain);
	DDX_Control(pDX, IDC_MarketValue, c_MarketValue);
	DDX_Control(pDX, IDC_InitialCost, c_InitialCost);
	DDX_Control(pDX, IDC_CapitalGain, c_CapitalGain);
	DDX_Control(pDX, IDC_AnnualizedNetGain, c_AnnualizedNetGain);
	DDX_Control(pDX, IDC_AggregateIncome, c_AggregateIncome);
	DDX_Check(pDX, IDC_AggregateIncome, m_AggregateIncome);
	DDX_Check(pDX, IDC_AnnualizedNetGain, m_AnnualizedNetGain);
	DDX_Check(pDX, IDC_CapitalGain, m_CapitalGain);
	DDX_Check(pDX, IDC_InitialCost, m_InitialCost);
	DDX_Check(pDX, IDC_MarketValue, m_MarketValue);
	DDX_Check(pDX, IDC_NetGain, m_NetGain);
	DDX_Check(pDX, IDC_Note, m_Note);
	DDX_Check(pDX, IDC_NumUnits, m_NumUnits);
	DDX_Check(pDX, IDC_PurchaseDate, m_PurchaseDate);
	DDX_Check(pDX, IDC_AggregateIncomeOverview, m_AggregateIncomeOverview);
	DDX_Check(pDX, IDC_AnnualizedNetGainOverview, m_AnnualizedNetGainOverview);
	DDX_Check(pDX, IDC_CapitalGainOverview, m_CapitalGainOverview);
	DDX_Check(pDX, IDC_InitialCostOverview, m_InitialCostOverview);
	DDX_Check(pDX, IDC_MarketValueOverview, m_MarketValueOverview);
	DDX_Check(pDX, IDC_NetGainOverview, m_NetGainOverview);
	DDX_Check(pDX, IDC_NoteOverview, m_NoteOverview);
	DDX_Check(pDX, IDC_NumUnitsOverview, m_NumUnitsOverview);
	DDX_Check(pDX, IDC_PurchaseDateOverview, m_PurchaseDateOverview);
//	DDX_Check(pDX, IDC_ExerciseValue, m_ExerciseValue);
//	DDX_Check(pDX, IDC_ExerciseValueOverview, m_ExerciseValueOverview);
	DDX_Check(pDX, IDC_InitiallySuppressPositions, m_InitiallySuppressPositions);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPositionListOptionsDialog, CNillaDialog)
	//{{AFX_MSG_MAP(CPositionListOptionsDialog)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPositionListOptionsDialog message handlers

BOOL CPositionListOptionsDialog::OnInitDialog() 
{
	CNillaDialog::OnInitDialog();
	
		// load PositionList display options saved in the document
	long	areVisible = 0xFFFFFFFF;		// default --> all columns are visible
	if ( ! GetPosListColVisibility( &areVisible ) )
	{
#ifdef _DEBUG
		TRACE( _T("PositionListOptionsDialog::OnInitDialog: no registered column visibility preferences found.\n") );
#endif
	}
	else
	{		// 'unpack' the Column Visibility preferences
		m_AggregateIncome	= areVisible & AggregateIncome;
		m_AnnualizedNetGain	= areVisible & AnnualizedNetGain;
		m_CapitalGain		= areVisible & CapitalGain;
//		m_ExerciseValue		= areVisible & ExerciseValue;
		m_InitialCost		= areVisible & InitialCost;
		m_MarketValue		= areVisible & MarketValue;
		m_NetGain			= areVisible & NetGain;
		m_Note				= areVisible & Note;
		m_NumUnits			= areVisible & NumUnits;
		m_PurchaseDate		= areVisible & PurchaseDate;

		m_AggregateIncomeOverview	 = areVisible & AggregateIncomeOverview;
		m_AnnualizedNetGainOverview	 = areVisible & AnnualizedNetGainOverview;
		m_CapitalGainOverview		 = areVisible & CapitalGainOverview;
//		m_ExerciseValueOverview		 = areVisible & ExerciseValueOverview;
		m_InitialCostOverview		 = areVisible & InitialCostOverview;
		m_MarketValueOverview		 = areVisible & MarketValueOverview;
		m_NetGainOverview			 = areVisible & NetGainOverview;
		m_NoteOverview				 = areVisible & NoteOverview;
		m_NumUnitsOverview			 = areVisible & NumUnitsOverview;
		m_PurchaseDateOverview		 = areVisible & PurchaseDateOverview;

		m_InitiallySuppressPositions = areVisible & InitiallySuppressPositions;
	}
	c_AggregateIncome.SetCheck(	  m_AggregateIncome	  ? BST_CHECKED : BST_UNCHECKED );
	c_AnnualizedNetGain.SetCheck( m_AnnualizedNetGain ? BST_CHECKED : BST_UNCHECKED );
	c_CapitalGain.SetCheck(		  m_CapitalGain		  ? BST_CHECKED : BST_UNCHECKED );
//	c_ExerciseValue.SetCheck(	  m_ExerciseValue	  ? BST_CHECKED : BST_UNCHECKED );
	c_InitialCost.SetCheck(		  m_InitialCost		  ? BST_CHECKED : BST_UNCHECKED );
	c_MarketValue.SetCheck(		  m_MarketValue		  ? BST_CHECKED : BST_UNCHECKED );
	c_NetGain.SetCheck(			  m_NetGain			  ? BST_CHECKED : BST_UNCHECKED );
	c_Note.SetCheck(			  m_Note			  ? BST_CHECKED : BST_UNCHECKED );
	c_NumUnits.SetCheck(		  m_NumUnits		  ? BST_CHECKED : BST_UNCHECKED );
	c_PurchaseDate.SetCheck(	  m_PurchaseDate	  ? BST_CHECKED : BST_UNCHECKED );

	c_AggregateIncomeOverview.SetCheck(	   m_AggregateIncomeOverview    ? BST_CHECKED : BST_UNCHECKED );
	c_AnnualizedNetGainOverview.SetCheck(  m_AnnualizedNetGainOverview  ? BST_CHECKED : BST_UNCHECKED );
	c_CapitalGainOverview.SetCheck(		   m_CapitalGainOverview	    ? BST_CHECKED : BST_UNCHECKED );
//	c_ExerciseValueOverview.SetCheck(	   m_ExerciseValueOverview	    ? BST_CHECKED : BST_UNCHECKED );
	c_InitialCostOverview.SetCheck(		   m_InitialCostOverview	    ? BST_CHECKED : BST_UNCHECKED );
	c_MarketValueOverview.SetCheck(		   m_MarketValueOverview	    ? BST_CHECKED : BST_UNCHECKED );
	c_NetGainOverview.SetCheck(			   m_NetGainOverview		    ? BST_CHECKED : BST_UNCHECKED );
	c_NoteOverview.SetCheck(			   m_NoteOverview			    ? BST_CHECKED : BST_UNCHECKED );
	c_NumUnitsOverview.SetCheck(		   m_NumUnitsOverview		    ? BST_CHECKED : BST_UNCHECKED );
	c_PurchaseDateOverview.SetCheck(	   m_PurchaseDateOverview	    ? BST_CHECKED : BST_UNCHECKED );

	c_InitiallySuppressPositions.SetCheck( m_InitiallySuppressPositions	? BST_CHECKED : BST_UNCHECKED );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//---------------------------------------------------------------------------
HBRUSH CPositionListOptionsDialog::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	return	CNillaDialog::OnCtlColor( pDC, pWnd, nCtlColor );
}			// OnCtlColor()
//---------------------------------------------------------------------------
