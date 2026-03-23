#if !defined(AFX_OPTIONDEFINITIONDIALOG_H__EE764FBB_E81D_44F6_A83B_97BA0A0EAC56__INCLUDED_)
#define AFX_OPTIONDEFINITIONDIALOG_H__EE764FBB_E81D_44F6_A83B_97BA0A0EAC56__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// OptionDefinitionDialog.h : header file
//
#include "NillaDialog.h"
#include "OptionType.h"
#include "SeekComboBox.h"
#include "NumEdit.h"
//#include "OleDateTimeEx.h"

/////////////////////////////////////////////////////////////////////////////
// COptionDefinitionDialog dialog

#include "Option.h"			// needed for definition of COption::PutOrCall
#include "afxwin.h"

class COptionDefinitionDialog : public CNillaDialog
{
	DECLARE_DYNAMIC( COptionDefinitionDialog )

// Construction
public:
	COptionDefinitionDialog( CWnd* pParent = NULL );   // standard constructor
	~COptionDefinitionDialog( void );

// Dialog Data
	//{{AFX_DATA(COptionDefinitionDialog)
	enum { IDD = IDD_OptionDefinitionDialog };
	CDateTimeCtrl	c_ExpiryDate;
	CSeekComboBox	c_StockSymbol;
	CStatic			c_VegaResult;
	CStatic			c_ThetaResult;
	CNumEdit		c_StrikePrice;
	CStatic			c_RhoRresult;
	CStatic			c_RhoDresult;
	CButton			c_PutRadio;
	CSeekComboBox	c_OptionSymbol;
	CNumEdit		c_OptionPrice;
	CStatic			c_ImpliedVolatilityResult;
	CStatic			c_GammaResult;
	CEdit			c_Desc;
	CStatic			c_DeltaResult;
	CButton			c_CallRadio;
	CStatic			c_BlackScholesResult;
	CString			m_OptionSymbol;
	MarketPrice_t	m_OptionPrice;
	CString			m_StockSymbol;		// in the definition dialog
	MarketPrice_t	m_StrikePrice;		// in the definition dialog
	CString			m_Desc;
	COleDateTime	m_ExpiryDate;
	//}}AFX_DATA
	OptionEvaluationContext	evcs;
	OptionType		putOrCall;			// in the definition dialog
	CButton			c_BScheckButton;
	float			m_RiskFreeRate;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionDefinitionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionDefinitionDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnCallButtonClicked();
	afx_msg void OnPutButtonClicked();
	afx_msg void OnKillFocusStrikePrice();
	afx_msg void OnKillFocusOptionPrice();
	afx_msg void OnKillFocusExpiryDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetFocusDesc();
	afx_msg void OnSetFocusOptionPrice();
	afx_msg void OnSetFocusStrikePrice();
	afx_msg void OnSelChangeOptionSymbol();
	afx_msg void OnSelChangeStockSymbol();
	afx_msg void OnKillFocusDesc();
	afx_msg void OnDeleteDefinition();
	afx_msg void OnCloseUpExpiryDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSetFocusExpiryDate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnClickBS();

	afx_msg void OnKillFocusOptionSymbol();										// HotKeys +
	afx_msg void OnKillFocusStockSymbol();										// HotKeys +
	afx_msg void OnSetFocusOptionSymbol();										// HotKeys
	afx_msg void OnSetFocusStockSymbol();										// HotKeys
	//}}AFX_MSG
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);		// HotKeys
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);						// HotKeys

	DECLARE_MESSAGE_MAP()

public:
	void	SaveIfChanged( bool updateGUI = false );	// MainFrame uses the default
	bool	m_BSchecked;

protected:
	void	ClearStatics( void );
	void	EvalOption( void );
	void	KillFocusExpiryDateWork( NMHDR* pNMHDR, LRESULT* pResult );
	void	KillFocusOptionSymbolWork( CString sym );
	void	KillFocusStockSymbolWork( CString sym );
	void	SaveOption( COption* theOption );
	bool	criticalRegionStock;
	bool	criticalRegionOption;
private:
	DWORD	m_activeComboBox;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//	void	resetControls( void );
//	void	enableDialogItems( bool enDis );
//	COption*	theOption;

#endif // !defined(AFX_OPTIONDEFINITIONDIALOG_H__EE764FBB_E81D_44F6_A83B_97BA0A0EAC56__INCLUDED_)
