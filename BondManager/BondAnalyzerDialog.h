#if !defined(AFX_BONDANALYZERDIALOG_H__B51854FE_9FB1_4F13_890A_0C741E983CA9__INCLUDED_)
#define AFX_BONDANALYZERDIALOG_H__B51854FE_9FB1_4F13_890A_0C741E983CA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BondAnalyzerDialog.h : header file
//
#include "NillaDialog.h"
#include "NumEdit.h"
#include "SeekComboBox.h"
#include "Bond.h"

/////////////////////////////////////////////////////////////////////////////
// CBondAnalyzerDialog dialog

class CBondAnalyzerDialog : public CNillaDialog
{
// Construction
public:
	CBondAnalyzerDialog(CWnd* pParent = NULL);	// standard constructor
	~CBondAnalyzerDialog( void );				// standard destructor

// Dialog Data
	//{{AFX_DATA(CBondAnalyzerDialog)
	enum { IDD = IDD_BondAnalyzerDialog };
	CSeekComboBox	c_DeltaYTM;
	CDateTimeCtrl	c_CouponEndDate;
	CDateTimeCtrl	c_CouponStartDate;
	CStatic			c_CouponsPerYear;
	CStatic			c_DateRange;
	CStatic			c_CouponRate;
	CSeekComboBox	c_BondSymbol;
	CNumEdit		c_MarketPrice;
	CString			m_BondSymbol;
	MarketPrice_t	m_MarketPrice;		// (i.e. float) Is this a problem?
	COleDateTime	m_CouponStartDate;
	COleDateTime	m_CouponEndDate;
	float			m_DeltaYTM;
	//}}AFX_DATA
private:
	BondEvaluationContext	evalCtx;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBondAnalyzerDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBondAnalyzerDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillFocusMarketPrice();
	afx_msg void OnSetFocusMarketPrice();
	afx_msg void OnKillFocusCouponStartDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusCouponEndDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCloseUpCouponEndDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCloseUpCouponStartDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangeDeltaYTM();
	afx_msg void OnSelChangeBondSymbol();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnKillFocusBondSymbol();										// HotKeys +
	afx_msg void OnKillFocusDeltaYTM();											// HotKeys +
	afx_msg void OnSetFocusBondSymbol();										// HotKeys
	afx_msg void OnSetFocusDeltaYTM();											// HotKeys
	//}}AFX_MSG
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);		// HotKeys
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);						// HotKeys

	DECLARE_MESSAGE_MAP()

		// Implementation
public:
	void		SaveIfChanged( bool updateGUI = false );

protected:
	void		KillFocusCouponStartDateWork( NMHDR* pNMHDR, LRESULT* pResult );
	void		KillFocusCouponEndDateWork( NMHDR* pNMHDR, LRESULT* pResult );
	void		LoadTheBond( void );
	void		ClearStatics( void );
	void		KillFocusDeltaYTMwork();
	void		KillFocusBondSymbolWork( CString sym );
	void		ResetControls( void );
	void		UpdateStatics( void );
		// when the CouponEndDate, CouponStartDate, or WhatIfYTM value changes
	void		UpdatePresentValueCoupons( void );

public:
	DWORD		m_activeComboBox;												// HotKeys
	CBond*		theBond;
	float		m_WhatIfYTM;

private:
	bool		criticalRegion;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//	void		enableDialogItems( bool enDis );

#endif // !defined(AFX_BONDANALYZERDIALOG_H__B51854FE_9FB1_4F13_890A_0C741E983CA9__INCLUDED_)
