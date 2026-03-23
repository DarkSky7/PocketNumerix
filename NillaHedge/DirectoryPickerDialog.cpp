// DirectoryPickerDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "DirectoryPickerDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirectoryPickerDialog dialog

CDirectoryPickerDialog::CDirectoryPickerDialog(
	BOOL bOpenFileDialog,			// TRUE for FileOpen, FALSE for FileSaveAs
	wchar_t*				lpszDefExt,
	const wchar_t*			lpszFileName,
	DWORD					dwFlags,
	const wchar_t*			lpszFilter,
	CWnd* pParentWnd) : CFileDialog( bOpenFileDialog, lpszDefExt, lpszFileName,
									 dwFlags, lpszFilter, pParentWnd )
{
	m_bDlgJustCameUp = true;
    //{{AFX_DATA_INIT(CMyFileDlg)
    //}}AFX_DATA_INIT
}

void CDirectoryPickerDialog::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirectoryPickerDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( CDirectoryPickerDialog, CFileDialog )
	//{{AFX_MSG_MAP(CDirectoryPickerDialog)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirectoryPickerDialog message handlers

void CDirectoryPickerDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

//This code makes the directory listbox "highlight" an entry when it first
//comes up.  W/O this code, the focus is on the directory listbox, but no
//focus rectangle is drawn and no entries are selected.  Ho hum.

	if ( m_bDlgJustCameUp )
	{
		m_bDlgJustCameUp = false;
//		SendDlgItemMessage( lst2, LB_SETCURSEL, 0, 0L );
	}
    // Do not call CFileDialog::OnPaint() for painting messages
}			// OnPaint()
//---------------------------------------------------------------------------
BOOL CDirectoryPickerDialog::OnInitDialog() 
{
	CFileDialog::OnInitDialog();

//Let's hide these windows so the user cannot tab to them.  Note that in
//the private template (in cddemo.dlg) the coordinates for these guys are
//*outside* the coordinates of the dlg window itself.  Without the following
//ShowWindow()'s you would not see them, but could still tab to them.
/*
	GetDlgItem(stc2)->ShowWindow( SW_HIDE );
	GetDlgItem(stc3)->ShowWindow( SW_HIDE );
	GetDlgItem(edt1)->ShowWindow( SW_HIDE );
	GetDlgItem(lst1)->ShowWindow( SW_HIDE );
	GetDlgItem(cmb1)->ShowWindow( SW_HIDE );

//We must put something in this field, even though it is hidden.  This is
//because if this field is empty, or has something like "*.txt" in it,
//and the user hits OK, the dlg will NOT close.  We'll jam something in
//there (like "Junk") so when the user hits OK, the dlg terminates.
//Note that we'll deal with the "Junk" during return processing (see below)

	SetDlgItemText( edt1, "Junk" );

//Now set the focus to the directories listbox.  Due to some painting
//problems, we *must* also process the first WM_PAINT that comes through
//and set the current selection at that point.  Setting the selection
//here will NOT work.  See comment below in the on paint handler.
            
	GetDlgItem(lst2)->SetFocus();
*/
	m_bDlgJustCameUp = true;

	return	FALSE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//---------------------------------------------------------------------------
