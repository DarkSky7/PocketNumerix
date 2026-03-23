#if !defined(AFX_OPTIONANALYZERDIALOG_H__62B7D3C5_448E_4E92_92A0_AFB8F7E60D11__INCLUDED_)
#define AFX_OPTIONANALYZERDIALOG_H__62B7D3C5_448E_4E92_92A0_AFB8F7E60D11__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OptionAnalyzerDialog.h : header file
//
#include "NillaDialog.h"
#include "SeekComboBox.h"
#include "Option.h"
#include "NumEdit.h"
#include "afxwin.h"

/////////////////////////////////////////////////////////////////////////////
// COptionAnalyzer dialog

class COptionAnalyzerDialog : public CNillaDialog
{
	DECLARE_DYNAMIC( COptionAnalyzerDialog )

// Construction
public:
	COptionAnalyzerDialog( CWnd* pParent = NULL );   // standard constructor
	~COptionAnalyzerDialog( void );

// Dialog Data
	//{{AFX_DATA(COptionAnalyzerDialog)
	enum { IDD = IDD_OptionAnalyzerDialog };
	CDateTimeCtrl	c_EvalDate;
	CStatic			c_StrikePriceResult;
	CStatic			c_StockName;
	CStatic			c_BlackScholesResult;
	CStatic			c_DeltaResult;
	CStatic			c_ExpiryResult;
	CStatic			c_GammaResult;
	CStatic			c_ImpliedVolatilityResult;
	CNumEdit		c_OptionPrice;
	CSeekComboBox	c_OptionSymbol;
	CStatic			c_PutCallResult;
	CStatic			c_RhoRresult;
	CStatic			c_RhoDresult;
	CNumEdit		c_RiskFreeRate;
	CNumEdit		c_StockPrice;
	CStatic			c_ThetaResult;
	CStatic			c_VegaResult;
	CNumEdit		c_Volatility;
	CString			m_OptionSymbol;
	MarketPrice_t	m_OptionPrice;
	MarketPrice_t	m_StockPrice;			// analyzer dialog only
	float 			m_Volatility;			// analyzer dialog only
	float			m_RiskFreeRate;			// in the analyzer dialog
	//}}AFX_DATA
	OptionEvaluationContext	evcs;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionAnalyzerDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COptionAnalyzerDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillFocusOptionPrice();
	afx_msg void OnKillFocusRiskFreeRate();
	afx_msg void OnKillFocusStockPrice();
	afx_msg void OnKillFocusVolatility();
	afx_msg void OnSetFocusStockPrice();
	afx_msg void OnSetFocusRiskFreeRate();
	afx_msg void OnSetFocusOptionPrice();
	afx_msg void OnSetFocusVolatility();
	afx_msg void OnCloseUpEvalDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusEvalDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetFocusEvalDate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnClickBS();

	afx_msg void OnSelChangeOptionSymbol();
	afx_msg void OnKillFocusOptionSymbol();										// HotKeys +
	afx_msg void OnSetFocusOptionSymbol();										// HotKeys
	//}}AFX_MSG
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);		// HotKeys
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);						// HotKeys

	DECLARE_MESSAGE_MAP()

public:
	CButton		c_BScheckButton;
	void		SaveIfChanged( bool updateGUI = false );	
	COption*	theOption;

protected:
	void	KillFocusEvalDateWork( NMHDR* pNMHDR, LRESULT* pResult );
	void	ClearStatics( void );
	void	ResetControls( void );
	void	EvalOption( void );
	void	KillFocusOptionSymbolWork( CString sym );
	DWORD	m_activeComboBox;
	bool	criticalRegion;
public:
	bool		m_BSchecked;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONANALYZERDIALOG_H__62B7D3C5_448E_4E92_92A0_AFB8F7E60D11__INCLUDED_)
