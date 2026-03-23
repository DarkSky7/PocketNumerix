// NillaDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "NillaDialog.h"
#include "SeekComboBox.h"
#include "NillaHedge.h"
#include "NumEdit.h"
#include "atlconv.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNillaDialog

IMPLEMENT_DYNAMIC( CNillaDialog, CDialog );

CNillaDialog::CNillaDialog( int IDD, CWnd* pParent /* =NULL */ )
	: CDialog( IDD, pParent )
{		// IDD comes from subclass !
	m_hbGray = CreateSolidBrush( RGB(208,208,208) );		// gray
	m_hbWhite = CreateSolidBrush( RGB(255,255,255) );		// white
}

void CNillaDialog::DoDataExchange( CDataExchange* pDX )
{
	CDialog::DoDataExchange( pDX );
	//{{AFX_DATA_MAP(CNillaDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

//BEGIN_MESSAGE_MAP( CNillaDialog, CDialog )
	//{{AFX_MSG_MAP( CNillaDialog )
	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()

//----------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////
//										GUI Support										//
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
BOOL	CNillaDialog::PreCreateWindow( CREATESTRUCT& cs )
{	RECT	re;
	BOOL b1 = SystemParametersInfo( SPI_GETWORKAREA, NULL, &re, NULL );	// returns nonzero == success
		// Create a window without min/max buttons or sizable border
	cs.style = WS_SYSMENU | WS_DLGFRAME;
	cs.x = re.left;									// dialog x origin
	cs.y = re.bottom;								// dialog y origin
	cs.cx = re.right - re.left;
	cs.cy = re.top - re.bottom;
		// Size the window to 1/3 screen size and center it
//	int menuHeight = ::GetSystemMetrics( SM_CYMENU );		// single bar menu height
//	cs.cx = ::GetSystemMetrics( SM_CXSCREEN );		// screen width
//	cs.cy = ::GetSystemMetrics( SM_CYSCREEN ) - 52;	// screen height - banner - menu
    return	CWnd::PreCreateWindow(cs);
}
//----------------------------------------------------------------------------------------
BOOL	CNillaDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CenterWindow();
	
	return	TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//---------------------------------------------------------------------------
void	CNillaDialog::RegisterHotKeys( DWORD ctrlId1, DWORD ctrlId2 )
{		// ignore the incoming CWnd
	if ( theApp.m_osVer < 5 )
		return;

	HWND hwnd = GetSafeHwnd();				// the dialog
	BOOL res = RegisterHotKey( hwnd, ctrlId1, NULL, VK_UP );
#ifdef _DEBUG
	if ( res == 0 )							// failure is zero
	{	wchar_t	wbuf[128];
		DWORD err = GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, NULL, wbuf, 127, NULL );
		TRACE( _T("NillaDialog::RegisterHotKeys(%d):RegisterHotKey(VK_UP) --> %d: %s\n"),
			ctrlId1, err, wbuf );
	}
#endif		// _DEBUG
	res = RegisterHotKey( hwnd, ctrlId2, NULL, VK_DOWN );
#ifdef _DEBUG
	if ( res == 0 )							// failure is zero
	{	wchar_t	wbuf[128];
		DWORD err = GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, NULL, wbuf, 127, NULL );
		TRACE( _T("NillaDialog::RegisterHotKeys(%d):RegisterHotKey(VK_DOWN) --> %d: %s\n"),
			ctrlId2, err, wbuf );
	}
#endif		// _DEBUG
}			// RegisterHotKeys()
//----------------------------------------------------------------------------
HBRUSH	CNillaDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	CDialog::OnCtlColor( pDC, pWnd, nCtlColor );	// does this really need to be here?
	pDC->SetBkMode( TRANSPARENT );
	return	( nCtlColor == CTLCOLOR_EDIT )  ?  m_hbWhite  :  m_hbGray;
}			// OnCtlColor()
//---------------------------------------------------------------------------
float  CNillaDialog::getComboBoxFloat( int dialogItemID )
{
	USES_CONVERSION;
	CString sym;

		// Did user select an existing value or create a new one?
	CComboBox* pCB = (CComboBox*)GetDlgItem( dialogItemID );
	int  len, cbi = pCB->GetCurSel();
	if ( cbi == -1 )
	{		// nothing selected from the ComboBox (turns out CB_ERR == -1)
			// try looking at the CEdit portion of the control
			// guessing that the following two statements are superfluous...
			// XXX we're not using the return value from GetBuffer() ???
		pCB->GetWindowText( sym );
		sym.TrimLeft();
		sym.TrimRight();
	}
	else	// (cbi >= 0) something has been selected from the ListBox
	{	len = pCB->GetLBTextLen( cbi ) + 1;			// with space for '\0' terminator
		pCB->GetLBText( cbi, sym.GetBuffer(len) );	// copy the ListBox item to sym
		sym.ReleaseBuffer();
#ifdef _DEBUG
		int	symLen = sym.GetLength();
		if ( symLen + 1 != len )
		{
			TRACE( _T("NillaDialog::getComboSel: len=%d, but symLen=%d\n"), len, symLen );
			AfxDebugBreak();
		}
#endif
	}
		// read the contents of the dialogItemID CEdit control
	wchar_t*	buf1 = sym.GetBuffer( len+1 );
	const char*		buf2 = W2CA( buf1 );

	float ff = (float)atof( buf2 );
	return ff;
}				// getComboBoxFloat()
//--------------------------------------------------------------------
/*
void CNillaDialog::seekComboBoxEntry( int dialogItemID ) 
{	CString sym;
		// this has be deprecated by CSeekComboBox->GetText()
	CComboBox* pCB = (CComboBox*)GetDlgItem( dialogItemID );
		// try to find a ComboBox item with the same prefix as the user currently has
		// entered in the CEdit part of the CComboBox.  If found, insert the entire
		// listBox item and highlight the portion of the string that the user hasn't
		// typed in himself.
	pCB->GetWindowText( sym );
//	sym.MakeLower();
	int	 symLen = sym.GetLength();
	int	 cbi = pCB->FindString( 0, sym );
	if ( cbi >= 0 )
	{	CString	listSym;
		pCB->GetLBText( cbi, listSym );					// copy the ListBox item to listSym
		pCB->SetWindowText( listSym );
			// highlight the  characters after the found string, else nothing highlighted
		pCB->SetEditSel( (cbi >= 0) ? pCB->GetLBTextLen( cbi ) : symLen, symLen );
	}
}				// seekComboBoxEntry()
*/
//--------------------------------------------------------------------
double  CNillaDialog::getEditBoxDouble( int dialogItemID )
{	
	USES_CONVERSION;
	CString		cs;

		// read the contents of the dialogItemID CEdit control
	CNumEdit* pEb = (CNumEdit*)GetDlgItem( dialogItemID );
	int  len = pEb->GetWindowTextLength();		// plus terminator

	wchar_t*	buf1 = cs.GetBuffer( len+1 );
	pEb->GetWindowText( cs );
	const char*		buf2 = W2CA( buf1 );

//	double dd = wcstod( buf, &buf+len );		// wipes out the stack!
	double	dd = atof( buf2 );
	return dd;
}				// getEditBoxDouble()
//--------------------------------------------------------------------
void  CNillaDialog::setEditBox(
	char*		format,
	double		dd,
	int			dialogItemID	)
{	char		buf[16];
//	CString		cs;
//	cs.Format( format, ff );
	sprintf( buf, format, dd );
	CString cs(buf);

		// write the contents of the dialogItemID CEdit control
	CEdit* pEb = (CEdit*)GetDlgItem( dialogItemID );
	pEb->SetWindowText( cs );
}				// setEditBox( char* format, float ff, int dialogItemID )
//--------------------------------------------------------------------
void  CNillaDialog::setEditBox(
	char*		format,
	float		ff,
	int			dialogItemID	)
{	char		buf[16];
//	CString		cs;
//	cs.Format( format, ff );
	sprintf( buf, format, ff );
	CString cs(buf);

		// write the contents of the dialogItemID CEdit control
	CEdit* pEb = (CEdit*)GetDlgItem( dialogItemID );
	pEb->SetWindowText( cs );
}				// setEditBox( char* format, float ff, int dialogItemID )
//--------------------------------------------------------------------
void  CNillaDialog::setComboBox(
	char*		format,
	float		ff,
	int			dialogItemID	)
{	char		buf[16];
	sprintf( buf, format, ff );
	CString cs(buf);

		// write the contents of the dialogItemID CEdit control
	CSeekComboBox* pCB = (CSeekComboBox*)GetDlgItem( dialogItemID );
	pCB->SetWindowText( cs );
}				// setComboBox( char* format, float ff, int dialogItemID )
//--------------------------------------------------------------------
void  CNillaDialog::setStatic(
	char*		format,
	float		ff,
	int			dialogItemID	)
{	char		buf[16];
	sprintf( buf, format, ff );
	CString cs(buf);

		// write the contents of the dialogItemID CEdit control
	CStatic* pS = (CStatic*)GetDlgItem( dialogItemID );
	pS->SetWindowText( cs );
}				// setStatic( char* format, float ff, int dialogItemID )
//--------------------------------------------------------------------
void  CNillaDialog::setStatic(
	char*		format,
	double		dd,
	int			dialogItemID	)
{	char		buf[32];
	sprintf( buf, format, dd );
	CString cs(buf);

		// write the contents of the dialogItemID CEdit control
	CStatic* pS = (CStatic*)GetDlgItem( dialogItemID );
	pS->SetWindowText( cs );
}				// setStatic( char* format, double dd, int dialogItemID )
//--------------------------------------------------------------------
void  CNillaDialog::setStatic(
	bool		valid,
	char*		format,
	double		dd,
	int			dialogItemID	)
{	CString		cs;
	char		buf[32];
	if ( valid )
	{	sprintf( buf, format, dd );
		cs = buf;
	}
	else
		cs = _T("~");

		// write the contents of the dialogItemID CEdit control
	CStatic* pS = (CStatic*)GetDlgItem( dialogItemID );
	pS->SetWindowText( cs );
}				// setStatic( char* format, double dd, int dialogItemID )
//--------------------------------------------------------------------
void  CNillaDialog::setStatic( CString cs, int dialogItemID )
{		// write the contents of the dialogItemID CEdit control
	CStatic* pS = (CStatic*)GetDlgItem( dialogItemID );
	pS->SetWindowText( cs );
}				// setStatic( CString cs, int dialogItemID )
//--------------------------------------------------------------------
CString  CNillaDialog::getListSel( int dialogItemID )
{	CString sym = _T("");

	CListBox* pLB = (CListBox*)GetDlgItem( dialogItemID );
	int	lbi = pLB->GetCurSel();
	if ( lbi >= 0 )
	{	int len = pLB->GetTextLen( lbi ) + 1;		// need space for '\0' terminators
		pLB->GetText( lbi, sym.GetBuffer(len) );	// copy the ListBox item to sym
		sym.ReleaseBuffer();
	}
	return sym;
}				// getListSel()
//--------------------------------------------------------------------
/*
void	CNillaDialog::loadMapIntoComboBox(
	CMapStringToPtr*	theMap,
	int					dialogItemID	)
{	if ( theMap == NULL )
		return;
	long				anOffset;
	CString				key;

		// clean up existing ComboBox entries & columns
	CComboBox*		pCB = (CComboBox*)GetDlgItem( dialogItemID );
	pCB->ResetContent();			// kill any existing entries
	
		// find the size of the list
	int				nObjs = theMap->GetCount();
//	SetCursor( theApp.LoadCursor(IDC_WAIT) );
	if ( nObjs > 99 )
	{		// determine # of items & how much memory is needed by the DropDownBox
		int	nChars = 0;
		for ( POSITION pos = theMap->GetStartPosition(); pos != NULL; )
		{	theMap->GetNextAssoc( pos, key, (void*&)anOffset );
			nChars += key.GetLength();
		}
			// preallocate storage for the ListBox
		int err = pCB->InitStorage(nObjs, (nChars+nObjs)/nObjs);
		ASSERT(err != CB_ERRSPACE);
	}

		// add object strings to the drop down part of the ComboBox
	for ( POSITION pos = theMap->GetStartPosition(); pos != NULL; )
	{	theMap->GetNextAssoc( pos, key, (void*&)anOffset );
		int err = pCB->AddString(key);
		ASSERT( err != CB_ERR );
		if ( err == CB_ERRSPACE )
		{	CString mss("Insufficient space to store '" + key + "' in the drop down list.");
			MessageBox( mss );
			break;
		}
	}
//	SetCursor( theApp.LoadCursor(IDC_ARROW) );
}				// loadMapIntoComboBox()
*/
//--------------------------------------------------------------------
void CNillaDialog::loadMapIntoListBox(
	CMapStringToPtr*	theMap,
	int					dialogItemID		)
{	if ( theMap == NULL )
		return;
	long				anOffset;
	CString				key;

//	SetCursor( theApp.LoadCursor(IDC_WAIT) );
	CListBox*  pLB = (CListBox*)GetDlgItem( dialogItemID );
	pLB->ResetContent();			// kill any existing entries

	int		 nObjs = theMap->GetCount();
	if ( nObjs > 99 )
	{		// determine # of items & how much memory is needed by the DropDownBox
		int	nChars = 0;
		for ( POSITION pos = theMap->GetStartPosition(); pos != NULL; )
		{	theMap->GetNextAssoc( pos, key, (void*&)anOffset );
			nChars += key.GetLength();
		}
			// preallocate storage for the ListBox
		int err = pLB->InitStorage(nObjs, (nChars+nObjs)/nObjs);
		ASSERT(err != LB_ERRSPACE);
	}

		// add object strings to the ListBox
	for ( POSITION pos = theMap->GetStartPosition(); pos != NULL; )
	{	theMap->GetNextAssoc( pos, key, (void*&)anOffset );
		int err = pLB->AddString(key);
		ASSERT(err != LB_ERR);
		if ( err == LB_ERRSPACE )
		{	CString mss("Insufficient space to store '" + key + "' in the list box.");
			MessageBox( mss );
			break;
		}
	}
	pLB->SetCurSel( 0 );
//	SetCursor( theApp.LoadCursor(IDC_ARROW) );
}				// loadMapIntoListBox()
//--------------------------------------------------------------------
float  CNillaDialog::getEditBoxFloat( int dialogItemID )
{	
	USES_CONVERSION;
	CString		cs;

		// read the contents of the dialogItemID CEdit control
	CNumEdit* pEb = (CNumEdit*)GetDlgItem( dialogItemID );
	int  len = pEb->GetWindowTextLength();		// plus terminator

	wchar_t*	buf1 = cs.GetBuffer( len+1 );
	pEb->GetWindowText( cs );
	const char*		buf2 = W2CA( buf1 );

//	double dd = wcstod( buf, &buf+len );		// wipes out the stack!
	float ff = (float)atof( buf2 );
	return ff;
}				// getEditBoxFloat()
//--------------------------------------------------------------------
/*
CObject*  CNillaDialog::getMapObject(
	CMapStringToOb&		theMap,
	CString				sym			)
{	CObject*			anObject;		// return value

		// lookup sym, get anObject
	if ( !theMap.Lookup(sym, anObject) )
	{		// Couldn't find sym in the map
		return 0;
	}
	ASSERT_VALID( anObject );
	return anObject;
}				// getObject( CMapStringToOb& theMap, CString sym )
//--------------------------------------------------------------------
void	CNillaDialog::getDateTimeCtrl( int dlgItemID, COleDateTime& theDate )
{		CDateTimeCtrl* pDTC = (CDateTimeCtrl*)GetDlgItem( dlgItemID );
		pDTC->GetTime( theDate );
}				// getDateTimeCtrl()
//--------------------------------------------------------------------
void	CNillaDialog::setDateTimeCtrl( COleDateTime& theDate, int dlgItemID )
{		CDateTimeCtrl* pDTC = (CDateTimeCtrl*)GetDlgItem( dlgItemID );
		pDTC->SetTime( theDate );
}				// setDateTimeCtrl()
//--------------------------------------------------------------------
void  CNillaDialog::setEditBox( char* st, int dialogItemID )
{
		// write the contents of the dialogItemID CEdit control
	CString cs( st );
	CEdit* pEb = (CEdit*)GetDlgItem( dialogItemID );
	pEb->SetWindowText( cs );
}				// setEditBox( char* st, int dialogItemID )
//--------------------------------------------------------------------
void  CNillaDialog::setEditBox( CString  cs, int dialogItemID )
{
		// write the contents of the dialogItemID CEdit control
	CEdit* pEb = (CEdit*)GetDlgItem( dialogItemID );
	pEb->SetWindowText( cs );
}				// setEditBox( CString  cs, int dialogItemID )
//--------------------------------------------------------------------
CString  CNillaDialog::getEditBoxCString( int dialogItemID )
{	CString		cs;

		// read the contents of the dialogItemID CEdit control
	CEdit* pEb = (CEdit*)GetDlgItem( dialogItemID );
//	int  len = pEb->GetWindowTextLength();
//	cs.GetBuffer( len+1 );			// XXX why isn't the return value of interest ???
	pEb->GetWindowText( cs );
	cs.TrimLeft();
	cs.TrimRight();
	return cs;
}				// getEditBoxCString()
*/
//--------------------------------------------------------------------
