// OptionChainListCtrl.cpp: implementation of the COptionChainListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RedBlackListCtrl.h"
#include "NillaHedge.h"
#include "OptionChainListCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction											//
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( COptionChainListCtrl, CRedBlackListCtrl )

BEGIN_MESSAGE_MAP( COptionChainListCtrl, CRedBlackListCtrl )
	//{{AFX_MSG_MAP(COptionChainListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

COptionChainListCtrl::COptionChainListCtrl() : CRedBlackListCtrl()
{
}

COptionChainListCtrl::~COptionChainListCtrl()
{
		// when the dialog is about to fall off the stack, 
		// we get called.  You'd expect that GetItemCount() would work, but
		// in fact, the hWnd is already 0x0 !!!   Not sure why this would happen.
		// Suspect this is a memory leak...
//	int	nItems = GetItemCount();
//	for ( short ii = 0; ii < nItems; ii++ )
//	{	OptionChainItemInfo* pInfo = (OptionChainItemInfo*)GetItemData( ii );
//		DeleteItem( ii );
//		delete	pInfo;
//	}
	CRedBlackListCtrl::~CRedBlackListCtrl();
}
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//							Message function Overrides						//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	COptionChainListCtrl::OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult ) 
{	NMLVCUSTOMDRAW* pCustomDraw = (NMLVCUSTOMDRAW*)pNMHDR;
	NMCUSTOMDRAW	nmcd = pCustomDraw->nmcd;
		// for a color table, see:  http://www.w3schools.com/html/html_colornames.asp
	COLORREF	callBck = RGB( 196, 232, 232 );		// Cyan (Calls)
	COLORREF	putBck  = RGB( 241, 180, 241 );		// Plum (Puts), at least that's what I call it

	int		si, rr = nmcd.dwItemSpec;				// item of interest (row: rr)
	switch ( nmcd.dwDrawStage )
	{	case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
		case CDDS_ITEMPREPAINT:
		{		// only change the background color of items
			OptionChainItemInfo*	ocii = (OptionChainItemInfo*)GetItemData( rr );
			pCustomDraw->clrTextBk = ocii->isCall ? callBck: putBck;
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			break;
		}
		case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		{		// which subitem?
			si = pCustomDraw->iSubItem;
			CString cst = GetItemText( rr, si );

				// text color
			if ( cst.GetLength() > 0  &&  cst[0] == '-' )		// it's negative
				pCustomDraw->clrText = RGB(255,0,0);			// use red pen

				// background color
			OptionChainItemInfo*	ocii = (OptionChainItemInfo*)GetItemData( rr );
			pCustomDraw->clrTextBk = ocii->isCall ? callBck: putBck;			
			*pResult = CDRF_NOTIFYPOSTPAINT;
			break;
		}
		case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM:
		{	pCustomDraw->clrText = RGB(0,0,0);					// text goes back to black
			pCustomDraw->clrTextBk = RGB(255,255,255);			// background back to white
			*pResult = CDRF_DODEFAULT;
			break;
		}
		default:
			break;
	}
}			// OnCustomDraw()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Interface Extensions						//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
int		COptionChainListCtrl::AddItem( LPARAM pItem, int iItemNo )
{		// set up an LVITEM and tell the list control about it
	LVITEM		lvi;
	lvi.iItem = iItemNo;					// zero based - load new at the top
	lvi.iSubItem = 0;
	lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_DI_SETITEM;
	lvi.pszText = LPSTR_TEXTCALLBACK;
	lvi.lParam = pItem;					// what we get in the CompareFunc()
	return	InsertItem( &lvi );
}			// AddItem()
//----------------------------------------------------------------------------
