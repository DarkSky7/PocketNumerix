#if !defined(AFX_CONFIRMSAVEPREFERENCESDIALOG_H__E5ABF8FB_3DED_4D83_A146_4CC0E00C2EFC__INCLUDED_)
#define AFX_CONFIRMSAVEPREFERENCESDIALOG_H__E5ABF8FB_3DED_4D83_A146_4CC0E00C2EFC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ConfirmSavePreferencesDialog.h : header file
//
#include "NillaDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CConfirmSavePreferencesDialog dialog

class CConfirmSavePreferencesDialog : public CNillaDialog
{
// Construction
public:
	CConfirmSavePreferencesDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfirmSavePreferencesDialog)
	enum { IDD = IDD_ConfirmSavePreferencesDialog };
//	CButton	c_OptionChainStockPriceUpdateVerify;
	CButton	c_AnalyzerClose;
	CButton	c_AnalyzerChangeSymbol;
	CButton	c_DefinitionClose;
	CButton	c_DefinitionChangeSymbol;
	CButton	c_PositionsDelete;
	CButton	c_PositionsModify;
	BOOL	m_AnalyzerClose;
	BOOL	m_AnalyzerChangeSymbol;
	BOOL	m_DefinitionClose;
	BOOL	m_DefinitionChangeSymbol;
	BOOL	m_PositionsDelete;
	BOOL	m_PositionsModify;
//	BOOL	m_OptionChainStockPriceUpdateVerify;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfirmSavePreferencesDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfirmSavePreferencesDialog)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIRMSAVEPREFERENCESDIALOG_H__E5ABF8FB_3DED_4D83_A146_4CC0E00C2EFC__INCLUDED_)
