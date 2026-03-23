#if !defined(AFX_POSITIONTRANSCRIPTDIALOG_H__90DF3AB4_98A4_4DD9_AF2F_1F7987672BD0__INCLUDED_)
#define AFX_POSITIONTRANSCRIPTDIALOG_H__90DF3AB4_98A4_4DD9_AF2F_1F7987672BD0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PositionTranscriptDialog.h : header file
//
#include "NillaDialog.h"
#include "SeekComboBox.h"

/////////////////////////////////////////////////////////////////////////////
// CPositionTranscriptDialog dialog

class CPositionTranscriptDialog : public CNillaDialog
{
// Construction
public:
	CPositionTranscriptDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPositionTranscriptDialog)
	enum { IDD = IDD_PositionsTranscripts };
	CSeekComboBox	c_YearList;
	CButton			c_SaveTranscript;
	CEdit			c_PositionsTranscript;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPositionTranscriptDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	void	LoadTranscript( int item );
	short	ParseYear( CString yrSt );

protected:
	int		itemSelected;

	// Generated message map functions
	//{{AFX_MSG(CPositionTranscriptDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnSaveTranscript();
	afx_msg void OnSelChangeYearList();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	afx_msg void OnKillFocusYearList();											// HotKeys
	afx_msg void OnSetFocusYearList();											// HotKeys
	//}}AFX_MSG
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);		// HotKeys
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);						// HotKeys

	DECLARE_MESSAGE_MAP()

protected:
	DWORD		m_activeComboBox;												// HotKeys
	short		m_itemSelected;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSITIONTRANSCRIPTDIALOG_H__90DF3AB4_98A4_4DD9_AF2F_1F7987672BD0__INCLUDED_)
