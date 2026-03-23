// SeekComboBox.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "SeekComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSeekComboBox

CSeekComboBox::CSeekComboBox()
{
	lastChar = 0;
}

CSeekComboBox::~CSeekComboBox()
{
}

BEGIN_MESSAGE_MAP(CSeekComboBox, CComboBox)
	//{{AFX_MSG_MAP(CSeekComboBox)
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnEditChange)
	//}}AFX_MSG_MAP
//	ON_WM_CHAR()
//	ON_CONTROL_REFLECT(WM_CHAR, OnChar)
//	ON_CONTROL_REFLECT(WM_GETDLGCODE, OnGetDlgCode)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSeekComboBox message handlers

BOOL CSeekComboBox::PreTranslateMessage( MSG* pMsg ) 
{
		// Make sure that the keystrokes continue to the appropriate handlers
//	if ( pMsg->message == WM_CHAR )
	{		// the if condition is disabled because arrow keys don't satisfy the WM_CHAR filter
		lastChar = pMsg->wParam;
	}
	return	CComboBox::PreTranslateMessage( pMsg );
}			// PreTranslateMessage()
//---------------------------------------------------------------------------
void	CSeekComboBox::OnEditChange( void ) 
{		// this Message handler replaces almost every Dialog's ComboBox OnEditChange
		// perform a Seek
	if ( lastChar != VK_BACK )
		SeekEntry();
}			// OnEditChange()
//---------------------------------------------------------------------------
void	CSeekComboBox::SeekEntry( void ) 
{		// this functionality used to belong to CNillaDialog::seekComboBoxEntry()
	CString sym;
		// try to find an item with the same prefix as the user currently has
		// entered in the CEdit part of the CSeekComboBox.  If found, insert the entire
		// listBox item and highlight the portion of the string that the user hasn't
		// typed in himself.
	GetWindowText( sym );
	sym.TrimLeft();
	sym.TrimRight();
	int	 symLen = sym.GetLength();
	int	 cbi = FindString( 0, sym );
	if ( cbi >= 0 )
	{	CString	listSym;
		GetLBText( cbi, listSym );					// copy the ListBox item to listSym
		SetWindowText( listSym );
			// highlight the  characters after the found string, else nothing highlighted
		SetEditSel( (cbi >= 0) ? GetLBTextLen( cbi ) : symLen, symLen );
	}
}				// SeekEntry()
//---------------------------------------------------------------------------
CString	CSeekComboBox::GetText( void ) 
{		// this functionality used to belong to CNillaDialog::getComboSel()
	CString sym;

		// Did user select an existing value or create a new one?
	GetWindowText( sym );
	sym.MakeUpper();
	if ( sym == _T("") )
	{	int  len, cbi = GetCurSel();
		if ( cbi >= 0 )
		{	// something has been selected from the ListBox
			len = GetLBTextLen( cbi ) + 1;			// with space for '\0' terminator
			GetLBText( cbi, sym.GetBuffer(len) );	// copy the ListBox item to sym
			sym.ReleaseBuffer(-1);
#ifdef _DEBUG
			int	symLen = sym.GetLength();
			if ( symLen + 1 != len )
			{	TRACE( _T("SeekComboBox::GetText: len=%d, but symLen=%d\n"), len, symLen );
				AfxDebugBreak();
			}
#endif
		}
	}
	sym.TrimLeft();
	sym.TrimRight();
	return sym;	
}			// GetText()
//---------------------------------------------------------------------------
void		CSeekComboBox::SetFloat( float ff ) 
{		// sets the WindowText of the control to the string eq. of ff
	wchar_t		wbuf[16];
	swprintf( wbuf, _T("%g"), ff );
	SetWindowText( wbuf );
}			// SetFloat()
//---------------------------------------------------------------------------
short		CSeekComboBox::GetFloat( float& ff )
{		// this float is always from the EditBox
	CString cs;
	GetWindowText( cs );
	return	swscanf( cs, _T("%f"), &ff );		// returns 1 on success
}			// GetFloat()
//---------------------------------------------------------------------------
short		CSeekComboBox::GetFloat( int idx, float& ff )
{		// this float is always from the ListBox
	int	cnt = GetCount();
	if ( idx >= cnt  ||  idx < 0 )
		idx = cnt - 1;

	CString cs;
	GetLBText( idx, cs );
	return	swscanf( cs, _T("%f"), &ff );		// returns 1 on success
}			// GetFloat()
//---------------------------------------------------------------------------
void	CSeekComboBox::LoadMap(	CMapStringToPtr* theMap )
{		// this functionality used to belong to CNillaDialog::loapMapIntoComboBox()
	if ( theMap == NULL )
		return;

	long		anOffset;
	CString		key;

		// clean up existing ComboBox entries & columns
	ResetContent();							// kill any existing entries

		// find the size of the list
	int	 nObjs = theMap->GetCount();
	if ( nObjs > 30 )
	{		// determine # of items & how much memory is needed by the DropDownBox
		int	nChars = 0;
		for ( POSITION pos = theMap->GetStartPosition(); pos != NULL; )
		{	theMap->GetNextAssoc( pos, key, (void*&)anOffset );
			nChars += key.GetLength();
		}
			// preallocate storage for the ListBox
		int err = InitStorage(nObjs, (nChars+nObjs)/nObjs);
		ASSERT(err != CB_ERRSPACE);
	}

		// add object strings to the drop down part of the ComboBox
	for ( POSITION pos = theMap->GetStartPosition(); pos != NULL; )
	{	theMap->GetNextAssoc( pos, key, (void*&)anOffset );
		int err = AddString(key);
		ASSERT( err != CB_ERR );
		if ( err == CB_ERRSPACE )
		{	CString mss("Insufficient space to store '" + key + "' in the drop down list.");
			MessageBox( mss );
			break;
		}
	}
}				// LoadMap()
//---------------------------------------------------------------------------
/*
unsigned int	CSeekComboBox::OnGetDlgCode()
{
#ifdef _DEBUG
	TRACE( _T("COptionAnalyzerDialog::OnGetDlgCode\n") );
#endif
	return	CComboBox::OnGetDlgCode() | DLGC_WANTCHARS;
}			// OnGetDlgCode()
*/
//---------------------------------------------------------------------------
