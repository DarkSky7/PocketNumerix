#if !defined(AFX_POSITIONLISTOPTIONSDIALOG_H__C52C2481_E7C2_4FC3_8572_C8FA177E6A93__INCLUDED_)
#define AFX_POSITIONLISTOPTIONSDIALOG_H__C52C2481_E7C2_4FC3_8572_C8FA177E6A93__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PositionListOptionsDialog.h : header file
//
#include "NillaDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CPositionListOptionsDialog dialog

class CPositionListOptionsDialog : public CNillaDialog
{
	DECLARE_DYNAMIC( CPositionListOptionsDialog )

// Construction
public:
	CPositionListOptionsDialog( CWnd* pParent = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPositionListOptionsDialog)
	enum { IDD = IDD_PositionListDisplayOptions };
	CButton	c_InitiallySuppressPositions;
//	CButton	c_ExerciseValueOverview;
//	CButton	c_ExerciseValue;
	CButton	c_PurchaseDateOverview;
	CButton	c_NumUnitsOverview;
	CButton	c_NoteOverview;
	CButton	c_NetGainOverview;
	CButton	c_MarketValueOverview;
	CButton	c_InitialCostOverview;
	CButton	c_CapitalGainOverview;
	CButton	c_AnnualizedNetGainOverview;
	CButton	c_AggregateIncomeOverview;
	CButton	c_PurchaseDate;
	CButton	c_NumUnits;
	CButton	c_Note;
	CButton	c_NetGain;
	CButton	c_MarketValue;
	CButton	c_InitialCost;
	CButton	c_CapitalGain;
	CButton	c_AnnualizedNetGain;
	CButton	c_AggregateIncome;
	BOOL	m_AggregateIncome;
	BOOL	m_AnnualizedNetGain;
	BOOL	m_CapitalGain;
	BOOL	m_InitialCost;
	BOOL	m_MarketValue;
	BOOL	m_NetGain;
	BOOL	m_Note;
	BOOL	m_NumUnits;
	BOOL	m_PurchaseDate;
	BOOL	m_AggregateIncomeOverview;
	BOOL	m_AnnualizedNetGainOverview;
	BOOL	m_CapitalGainOverview;
	BOOL	m_InitialCostOverview;
	BOOL	m_MarketValueOverview;
	BOOL	m_NetGainOverview;
	BOOL	m_NoteOverview;
	BOOL	m_NumUnitsOverview;
	BOOL	m_PurchaseDateOverview;
//	BOOL	m_ExerciseValue;
//	BOOL	m_ExerciseValueOverview;
	BOOL	m_InitiallySuppressPositions;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPositionListOptionsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPositionListOptionsDialog)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSITIONLISTOPTIONSDIALOG_H__C52C2481_E7C2_4FC3_8572_C8FA177E6A93__INCLUDED_)
