#if !defined(AFX_SpotRateEstimatorDialog_H__A5076B15_5D5B_41F9_AB5B_379EC05869DF__INCLUDED_)
#define AFX_SpotRateEstimatorDialog_H__A5076B15_5D5B_41F9_AB5B_379EC05869DF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// SpotRateEstimatorDialog.h : header file
//
class	CBitArray;
class	CVector;
class	CMatrix;

#include "NillaDialog.h"
#include "NumEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CSpotRateEstimatorDialog dialog

class CSpotRateEstimatorDialog : public CNillaDialog
{
	DECLARE_DYNAMIC( CSpotRateEstimatorDialog )

// Construction
public:
	CSpotRateEstimatorDialog( CWnd* pParent = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpotRateEstimatorDialog)
	enum { IDD = IDD_YieldCurveDialog };
	float		m_3moYield;
	float		m_6moYield;
	float		m_1yrYield;
	float		m_2yrYield;
	float		m_3yrYield;
	float		m_5yrYield;
	float		m_7yrYield;
	float		m_10yrYield;
	float		m_15yrYield;
	float		m_20yrYield;
	float		m_25yrYield;
	float		m_30yrYield;
	CNumEdit	c_3moYield;
	CNumEdit	c_6moYield;
	CNumEdit	c_1yrYield;
	CNumEdit	c_2yrYield;
	CNumEdit	c_3yrYield;
	CNumEdit	c_5yrYield;
	CNumEdit	c_7yrYield;
	CNumEdit	c_10yrYield;
	CNumEdit	c_15yrYield;
	CNumEdit	c_20yrYield;
	CNumEdit	c_25yrYield;
	CNumEdit	c_30yrYield;
	//}}AFX_DATA

  public:
		// additional attributes
	float		spotRate;
	CBitArray*	userSupplied;

		// Implementation
	void	updateRates( void );
	void		SaveSRB( void );
	
		// methods
//void	fitFunc( float  xx, CVector<float>&  fitVec );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpotRateEstimatorDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CSpotRateEstimatorDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillFocus10yrYield();
	afx_msg void OnKillFocus15yrYield();
	afx_msg void OnKillFocus1yrYield();
	afx_msg void OnKillFocus20yrYield();
	afx_msg void OnKillFocus25yrYield();
	afx_msg void OnKillFocus2yrYield();
	afx_msg void OnKillFocus30yrYield();
	afx_msg void OnKillFocus3moYield();
	afx_msg void OnKillFocus3yrYield();
	afx_msg void OnKillFocus5yrYield();
	afx_msg void OnKillFocus6moYield();
	afx_msg void OnKillFocus7yrYield();
	afx_msg void OnSetFocus10yrYield();
	afx_msg void OnSetFocus15yrYield();
	afx_msg void OnSetFocus1yrYield();
	afx_msg void OnSetFocus20yrYield();
	afx_msg void OnSetFocus25yrYield();
	afx_msg void OnSetFocus2yrYield();
	afx_msg void OnSetFocus30yrYield();
	afx_msg void OnSetFocus3moYield();
	afx_msg void OnSetFocus3yrYield();
	afx_msg void OnSetFocus5yrYield();
	afx_msg void OnSetFocus6moYield();
	afx_msg void OnSetFocus7yrYield();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SpotRateEstimatorDialog_H__A5076B15_5D5B_41F9_AB5B_379EC05869DF__INCLUDED_)
