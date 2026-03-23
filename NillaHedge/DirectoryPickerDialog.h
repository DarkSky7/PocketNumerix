#if !defined(AFX_DIRECTORYPICKERDIALOG_H__21F243AB_FB87_446A_B9E3_F5E7E0840FE1__INCLUDED_)
#define AFX_DIRECTORYPICKERDIALOG_H__21F243AB_FB87_446A_B9E3_F5E7E0840FE1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DirectoryPickerDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDirectoryPickerDialog dialog

class CDirectoryPickerDialog : public CFileDialog
{
// Construction
public:
    CDirectoryPickerDialog( BOOL bOpenFileDialog,			// TRUE for FileOpen, FALSE for FileSaveAs
							wchar_t*		lpszDefExt = NULL,
							const wchar_t*	lpszFileName = NULL,
							DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
							const wchar_t*	lpszFilter = NULL,
							CWnd* pParentWnd = NULL );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirectoryPickerDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDirectoryPickerDialog)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	bool	m_bDlgJustCameUp;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRECTORYPICKERDIALOG_H__21F243AB_FB87_446A_B9E3_F5E7E0840FE1__INCLUDED_)
