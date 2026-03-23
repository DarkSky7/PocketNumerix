// BOSDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "BOSDialog.h"
#include "NillaHedge.h"
//#include "NillaHedgeDoc.h"
//#include "BOSDatabase.h"
#include "Option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern	CNillaHedgeApp	theApp;

/////////////////////////////////////////////////////////////////////////////
// CBOSDialog dialog

IMPLEMENT_DYNAMIC( CBOSDialog, CNillaDialog )

CBOSDialog::CBOSDialog( int idd, CWnd* pParent /* =NULL */ )
	: CNillaDialog( idd, pParent )
{
}

void CBOSDialog::DoDataExchange(CDataExchange* pDX)
{
	CNillaDialog::DoDataExchange(pDX);
}

//BEGIN_MESSAGE_MAP( CBOSDialog, CNillaDialog )
	//{{AFX_MSG_MAP(CBOSDialog)
	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()

//----------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////
//								Handy Option/Stock tools							//
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
/*
CBond*		CBOSDialog::getBond( void )
{	CString sym = getComboSel( IDC_BondSymbol );
	return	getBond( sym );
}				// getBond( void )
//--------------------------------------------------------------------
CBond*		CBOSDialog::getBond( CString sym )
{		// it would seem logical to write the following, but it doesn't work
//	return	sym.IsEmpty() ? NULL : theApp.pDB->getBond( sym );
		// here's the fix...
	CString copy = sym;
//	int		sLen = sym.GetLength();
//	whar_t*	wbuf = sym.GetBuffer( sLen );
//	bool	noSym = ( *wbuf == '\0' );
//	return	noSym ? NULL : theApp.pDB->getBond( sym );
	return	copy.IsEmpty() ? NULL : theApp.pDB->getBond( sym );
}				// getBond( void )
//--------------------------------------------------------------------
COption*	CBOSDialog::getOption( void )
{	CString sym = getComboSel( IDC_OptionSymbol );
	return	getOption( sym );
}				// getOption( void )
//--------------------------------------------------------------------
COption*	CBOSDialog::getOption( CString sym )
{		// it would seem logical to write the following, but it doesn't work
//	return	sym.IsEmpty() ? NULL : theApp.pDB->getOption( sym );
	int		sLen = sym.GetLength();
	wchar_t*	wbuf = sym.GetBuffer( sLen );
	bool	noSym = ( *wbuf == '\0' );
	return	noSym ? NULL : theApp.pDB->getOption( sym );
}				// getOption( void )
//--------------------------------------------------------------------
CStock*		CBOSDialog::getStock( void )
{	CString sym = getComboSel( IDC_StockSymbol );
	return	getStock( sym );
}				// getStock( void )
//--------------------------------------------------------------------
CStock*		CBOSDialog::getStock( CString sym )
{		// it would seem logical to write the following, but it doesn't work
//	return	sym.IsEmpty() ? NULL : theApp.pDB->getStock( sym );
	int		sLen = sym.GetLength();
	whar_t*	wbuf = sym.GetBuffer( sLen );
	bool	noSym = ( *wbuf == '\0' );
	return	noSym ? NULL : theApp.pDB->getStock( sym );	
}				// getStock( void )
//--------------------------------------------------------------------
CBond*		CBOSDialog::getBond( CString sym )
{		// lookup sym, get aBond
	CFrameWnd* pFW = GetTopLevelFrame();
	CNillaHedgeDoc* pDoc = (CNillaHedgeDoc*)(pFW->GetActiveDocument());
	return (CBond*)getMapObject( pDoc->bonds, sym );
}				// getBond( CString sym )
//--------------------------------------------------------------------
COption*	CBOSDialog::getOption( CString sym )
{		// lookup sym, get theOption
	CFrameWnd* pFW = GetTopLevelFrame();
	CNillaHedgeDoc* pDoc = (CNillaHedgeDoc*)pFW->GetActiveDocument();
	return (COption*)getMapObject( pDoc->options, sym );
}				// getOption( CString sym )
//--------------------------------------------------------------------
CStock*		CBOSDialog::getStock( CString sym )
{		// lookup sym, get theStock
	CFrameWnd* pFW = GetTopLevelFrame();
	CNillaHedgeDoc* pDoc = (CNillaHedgeDoc*)pFW->GetActiveDocument();
	return (CStock*)getMapObject( pDoc->stocks, sym );
}				// getStock( CString sym )
*/
//--------------------------------------------------------------------
