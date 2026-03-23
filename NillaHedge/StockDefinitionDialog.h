#if !defined(AFX_STOCKDEFINITIONDIALOG_H__3A671C5B_4A6E_40FA_AC34_172C6039EA38__INCLUDED_)
#define AFX_STOCKDEFINITIONDIALOG_H__3A671C5B_4A6E_40FA_AC34_172C6039EA38__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// StockDefinitionDialog.h : header file
//

#include "NillaDialog.h"
#include "SeekComboBox.h"
#include "NumEdit.h"
//#include "OleDateTimeEx.h"
#include "BOS.h"				// for MarketPrice_t

class	CStock;

/////////////////////////////////////////////////////////////////////////////
// CStockDefinitionDialog dialog

class CStockDefinitionDialog : public CNillaDialog
{
	DECLARE_DYNAMIC( CStockDefinitionDialog )

// Construction
public:
	CStockDefinitionDialog( CWnd* pParent = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStockDefinitionDialog)
	enum { IDD = IDD_StockDefinitionDialog };
	CDateTimeCtrl	c_Q4DividendDatePicker;
	CDateTimeCtrl	c_Q3DividendDatePicker;
	CDateTimeCtrl	c_Q2DividendDatePicker;
	CDateTimeCtrl	c_Q1DividendDatePicker;
	CNumEdit		c_Volatility;
	CSeekComboBox	c_StockSymbol;
	CButton			c_Q4ExDivDateCheck;
	CNumEdit		c_Q4Dividend;
	CButton			c_Q3ExDivDateCheck;
	CNumEdit		c_Q3Dividend;
	CButton			c_Q2ExDivDateCheck;
	CNumEdit		c_Q2Dividend;
	CButton			c_Q1ExDivDateCheck;
	CNumEdit		c_Q1Dividend;
	CNumEdit		c_StockPrice;
	CStatic			c_CurrentYieldResult;
	CEdit			c_Desc;
	float			m_Volatility;
	CString			m_StockSymbol;
	CString			m_Desc;
	COleDateTime	m_Q1exDividendDate;
	COleDateTime	m_Q2exDividendDate;
	COleDateTime	m_Q3exDividendDate;
	COleDateTime	m_Q4exDividendDate;
	BOOL			m_Q4exDividendDateCheck;
	BOOL			m_Q3exDividendDateCheck;
	BOOL			m_Q2exDividendDateCheck;
	BOOL			m_Q1exDividendDateCheck;
	float			m_Q1Dividend;
	float			m_Q2Dividend;
	float			m_Q3Dividend;
	float			m_Q4Dividend;
	MarketPrice_t	m_StockPrice;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStockDefinitionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void	killFocusQ1DividendDateWork( NMHDR* pNMHDR, LRESULT* pResult );
	void	killFocusQ2DividendDateWork( NMHDR* pNMHDR, LRESULT* pResult );
	void	killFocusQ3DividendDateWork( NMHDR* pNMHDR, LRESULT* pResult );
	void	killFocusQ4DividendDateWork( NMHDR* pNMHDR, LRESULT* pResult );

	// Generated message map functions
	//{{AFX_MSG(CStockDefinitionDialog)
	virtual BOOL OnInitDialog();

	afx_msg void OnKillFocusQ1Dividend();
	afx_msg void OnSetFocusQ1Dividend();
	afx_msg void OnKillFocusQ2Dividend();
	afx_msg void OnSetFocusQ2Dividend();
	afx_msg void OnKillFocusQ3Dividend();
	afx_msg void OnSetFocusQ3Dividend();
	afx_msg void OnKillFocusQ4Dividend();
	afx_msg void OnSetFocusQ4Dividend();

	afx_msg void OnQ1ExDivDateCheck();
	afx_msg void OnQ2ExDivDateCheck();
	afx_msg void OnQ3ExDivDateCheck();
	afx_msg void OnQ4ExDivDateCheck();

	afx_msg void OnSetFocusDesc();
	afx_msg void OnKillFocusDesc();

	afx_msg void OnDeleteDefinition();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnKillFocusQ1DividendDatePicker(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSetFocusQ1DividendDatePicker(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKillFocusQ2DividendDatePicker(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSetFocusQ2DividendDatePicker(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKillFocusQ3DividendDatePicker(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSetFocusQ3DividendDatePicker(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKillFocusQ4DividendDatePicker(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSetFocusQ4DividendDatePicker(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnKillFocusStockPrice();
	afx_msg void OnSetFocusStockPrice();

	afx_msg void OnKillFocusVolatility();
	afx_msg void OnSetFocusVolatility();

	afx_msg void OnSelChangeStockSymbol();
	afx_msg void OnKillFocusStockSymbol();										// HotKeys +
	afx_msg void OnSetFocusStockSymbol();										// HotKeys
	//}}AFX_MSG
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);		// HotKeys
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);						// HotKeys

	DECLARE_MESSAGE_MAP()

		// utility functions
	void	EnableDialogItems( void );
	void	KillFocusStockSymbolWork( CString sym );
	void	UpdateStatics( void );
	void	SaveStock( CStock* aStock );

public:
	void	SaveIfChanged( bool updateGUI = false );	// MainFrame uses the default

protected:
	DWORD	m_activeComboBox;
	bool	criticalRegion;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//	void		enableDialogItems( bool enDis );
//	void		clearAll( void );
//	void		resetControls( void );

#endif // !defined(AFX_STOCKDEFINITIONDIALOG_H__3A671C5B_4A6E_40FA_AC34_172C6039EA38__INCLUDED_)

