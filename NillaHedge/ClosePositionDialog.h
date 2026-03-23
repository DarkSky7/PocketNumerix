#if !defined(AFX_CLOSEPOSITIONDIALOG_H__F5EB30AE_F20D_441B_8CCE_AD562567FB91__INCLUDED_)
#define AFX_CLOSEPOSITIONDIALOG_H__F5EB30AE_F20D_441B_8CCE_AD562567FB91__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ClosePositionDialog.h : header file
//
#include "NillaDialog.h"
#include "NumEdit.h"
//#include "OleDateTimeEx.h"

/////////////////////////////////////////////////////////////////////////////
// CClosePositionDialog dialog

class CClosePositionDialog : public CNillaDialog
{
// Construction
public:
	CClosePositionDialog(CWnd* pParent = NULL);   // standard constructor
	COleDateTime	m_MinDate;		// initialized by caller

// Dialog Data
	//{{AFX_DATA(CClosePositionDialog)
	enum { IDD = IDD_ClosePositionDialog };
	CDateTimeCtrl	c_ClosingDate;
	CNumEdit		c_NetProceeds;
	CEdit			c_ClosingNote;
	COleDateTime	m_ClosingDate;
	CString			m_ClosingNote;
	double			m_NetProceeds;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClosePositionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:
	void	killFocusClosingDateWork( NMHDR* pNMHDR, LRESULT* pResult );

	// Generated message map functions
	//{{AFX_MSG(CClosePositionDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnCloseUpClosingDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnKillFocusNetProceeds();
	afx_msg void OnSetFocusNetProceeds();
	afx_msg void OnKillFocusClosingNote();
	afx_msg void OnSetFocusClosingNote();
	afx_msg void OnKillFocusClosingDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetFocusClosingDate(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLOSEPOSITIONDIALOG_H__F5EB30AE_F20D_441B_8CCE_AD562567FB91__INCLUDED_)
