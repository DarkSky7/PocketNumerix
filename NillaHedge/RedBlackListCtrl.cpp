// RedBlackListCtrl.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "RedBlackListCtrl.h"
#include "NillaHedge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRedBlackListCtrl

IMPLEMENT_DYNAMIC( CRedBlackListCtrl, CListCtrl )

/*
CRedBlackListCtrl::CSortHeaderCtrl::CSortHeaderCtrl()
{
	m_iSortColumn = -1;
	m_bSortAscending = TRUE;
}
*/

BEGIN_MESSAGE_MAP( CRedBlackListCtrl, CListCtrl )
	//{{AFX_MSG_MAP(CRedBlackListCtrl)
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

CRedBlackListCtrl::CRedBlackListCtrl() : CListCtrl()
{
	m_bTracking = false;
	startItem = -1;
	recentItem = -1;
}

CRedBlackListCtrl::~CRedBlackListCtrl()
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
	CListCtrl::~CListCtrl();
}
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Message functions							//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CRedBlackListCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	m_bTracking = true;
	SetCapture();

#ifdef _DEBUG
//	TRACE( _T("RedBlackListCtrl::OnLButtonDown(1): m_bTracking(%d,%d)=%s\n"),
//		point.x, point.y, m_bTracking ? _T("true") : _T("false") );
#endif

	UINT	uFlags;
	int	idx = HitTest( point, &uFlags );

#ifdef _DEBUG
	TRACE( _T("RedBlackListCtrl::OnLButtonDown(2): idx=%d\n"), idx );
#endif

	if ( idx >= 0 )
	{
//		SetSelectionMark( idx );
//		SetItemState( idx, LVIS_SELECTED, LVIS_SELECTED );
//		RedrawItems( idx, idx );
//		UpdateWindow();
		startItem = idx;
		recentItem = idx;
	}
	CListCtrl::OnLButtonDown( nFlags, point );		// seems to be critical to the success of OnMouseMove()
}			// OnLButtonDown()
//----------------------------------------------------------------------------
/*
	// a strategy including the following code for OnMouseMove()
	// and the commented out code for OnLButtonUp() immediately below
	// is too performance intensive for the emulator and 400Mhz PocketPC.
	// However, ignoring MouseMoves (which prevents real-time highlighting
	// of selected items) and pushing the selection code into OnLButtonUp()
	// allows both { LButtonDown, drag, LButtonUp } to select multiple items
	// as well as the old fashioned Shift-Click and Ctrl-Click modalities

void	CRedBlackListCtrl::OnMouseMove( UINT nFlags, CPoint point )
{	MSG		msg;
	HWND	hWnd = GetSafeHwnd();
	BOOL	success = PeekMessage( &msg, hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_NOREMOVE );
	if ( success )
	{
#ifdef _DEBUG
//		TRACE( _T("RedBlackListCtrl::OnMouseMove(1): more WM_MOUSEMOVE messages are queued --> return.\n") );
#endif
		return;			// there's another WM_MOUSEMOVE waiting to be processed, skip this one
	}

	if ( ! m_bTracking )
	{
#ifdef _DEBUG
		TRACE( _T("RedBlackListCtrl::OnMouseMove(2): NOT tracking --> return.\n") );
#endif
		return;
	}

	UINT	uFlags;
	int	idx = HitTest( point, &uFlags );
	if ( idx < 0  ||  idx == recentItem )
	{
#ifdef _DEBUG
		TRACE( _T("RedBlackListCtrl::OnMouseMove(3): idx=%d, recentItem=%d --> return.\n"), idx, recentItem );
#endif
		return;
	}

		// find the beginning and end of the selection range
	int		start, stop;
		// idx == recentItem has already been excluded
	if ( idx < startItem )
	{	start = idx;
		stop = startItem;
	}
	else		// startItem > idx
	{	start = startItem;
		stop = idx;
	}
#ifdef _DEBUG
	TRACE( _T("RedBlackListCtrl::OnMouseMove(4): Select items [%d,%d]\n"), start, stop );
#endif

		// select the items on the [start,stop] interval
	for ( int ii = start; ii <= stop; ii++ )
	{	UINT	state = GetItemState( ii, LVIS_SELECTED );
		if ( ! (state & LVIS_SELECTED) )
		{		// the ii-th item is not currently selected --> select it now
			SetItemState( ii, LVIS_SELECTED, LVIS_SELECTED );
			RedrawItems( ii, ii );					// invalidates the item 
		}
	}

		// deselect items outside the [start,stop] interval
	POSITION	pos = GetFirstSelectedItemPosition();
	while ( pos )
	{	ii = GetNextSelectedItem( pos );
		if ( ii < start  ||  ii > stop )
		{	SetItemState( ii, 0, LVIS_SELECTED );
			RedrawItems( ii, ii );					// invalidates the item 
		}
	}
	UpdateWindow();									// signals a repaint

	recentItem = idx;		// recentItem becomes the row under the mouse

		// notify parent ...
		// normally this would be in OnLButtonUp(), but
		// since OnLButtonUp() almost never gets called, we put it here.
	CWnd* pWnd = GetParent();
	pWnd->SendMessage( WM_MarqueeEnd );

//	CListCtrl::OnMouseMove( nFlags, point );
}			// OnMouseMove()
//----------------------------------------------------------------------------
void	CRedBlackListCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
#ifdef _DEBUG
		TRACE( _T("RedBlackListCtrl::OnLButtonUp\n") );
#endif
  	m_bTracking = false;
	startItem = -1;
	recentItem = -1;
	if ( GetCapture() == this )
		ReleaseCapture();
//	CListCtrl::OnLButtonUp( nFlags, point );
}			// OnLButtonUp()
*/
//----------------------------------------------------------------------------
void	CRedBlackListCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
#ifdef _DEBUG
		TRACE( _T("RedBlackListCtrl::OnLButtonUp\n") );
#endif

	if ( ! m_bTracking )
	{
#ifdef _DEBUG
		TRACE( _T("RedBlackListCtrl::OnLButtonUp(1): NOT tracking --> return.\n") );
#endif
		return;
	}

	UINT	uFlags;
	int	idx = HitTest( point, &uFlags );
	if ( idx < 0  ||  idx == recentItem )
	{
#ifdef _DEBUG
		TRACE( _T("RedBlackListCtrl::OnLButtonUp(2): idx=%d, recentItem=%d --> return.\n"), idx, recentItem );
#endif
		return;
	}

		// find the beginning and end of the selection range
	int		start, stop;
		// idx == recentItem has already been excluded
	if ( idx < startItem )
	{	start = idx;
		stop = startItem;
	}
	else		// startItem > idx
	{	start = startItem;
		stop = idx;
	}
#ifdef _DEBUG
	TRACE( _T("RedBlackListCtrl::OnLButtonUp(3): Select items [%d,%d]\n"), start, stop );
#endif

		// select the items on the [start,stop] interval
	for ( int ii = start; ii <= stop; ii++ )
	{	UINT	state = GetItemState( ii, LVIS_SELECTED );
		if ( ! (state & LVIS_SELECTED) )
		{		// the ii-th item is not currently selected --> select it now
			SetItemState( ii, LVIS_SELECTED, LVIS_SELECTED );
			RedrawItems( ii, ii );					// invalidates the item 
		}
	}

		// deselect items outside the [start,stop] interval
	POSITION	pos = GetFirstSelectedItemPosition();
	while ( pos )
	{	long ii = GetNextSelectedItem( pos );
		if ( ii < start  ||  ii > stop )
		{	SetItemState( ii, 0, LVIS_SELECTED );
			RedrawItems( ii, ii );					// invalidates the item 
		}
	}
	UpdateWindow();									// signals a repaint

	recentItem = idx;		// recentItem becomes the row under the mouse

		// notify parent ...
	CWnd* pWnd = GetParent();
	pWnd->SendMessage( WM_MarqueeEnd );
	
		// reset for next OnLButtonDown()		
	m_bTracking = false;
	startItem = -1;
	recentItem = -1;
	if ( GetCapture() == this )
		ReleaseCapture();
//	CListCtrl::OnLButtonUp( nFlags, point );
}			// OnLButtonUp()
//----------------------------------------------------------------------------
void	CRedBlackListCtrl::OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult ) 
{	NMLVCUSTOMDRAW* pCustomDraw = (NMLVCUSTOMDRAW*)pNMHDR;
	NMCUSTOMDRAW	nmcd = pCustomDraw->nmcd;
	int		si, rr = nmcd.dwItemSpec;			// item of interest (row: rr)
	switch ( nmcd.dwDrawStage )
	{	case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
		case CDDS_ITEMPREPAINT:
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			break;
		case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		{		// which subitem?
			si = pCustomDraw->iSubItem;
			CString cst = GetItemText( rr, si );
			if ( cst.GetLength() > 0  &&  cst[0] == '-' )	// it's negative
			{	pCustomDraw->clrText = RGB(255,0,0);		// use red pen
			}
			*pResult = CDRF_NOTIFYPOSTPAINT;
			break;
		}
		case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM:
		{	pCustomDraw->clrText = RGB(0,0,0);				// back to black
			*pResult = CDRF_DODEFAULT;
			break;
		}
		default:
			break;
	}
}			// OnCustomDraw()
//--------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////
//									Overrides									//
//////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------
/*
void	CRedBlackListCtrl::CSortHeaderCtrl::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	// attath to the device context.
	CDC dc;
	VERIFY(dc.Attach( lpDrawItemStruct->hDC ));

	// save the device context.
	const int iSavedDC = dc.SaveDC();

	// get the column rect.
	CRect rc( lpDrawItemStruct->rcItem );

	// set the clipping region to limit drawing within the column.
	CRgn rgn;
	VERIFY(rgn.CreateRectRgnIndirect( &rc ));
	(void)dc.SelectObject( &rgn );
	VERIFY( rgn.DeleteObject() );

	// draw the background,
	CBrush brush(GetSysColor(COLOR_3DFACE));
	dc.FillRect( rc, &brush );

	// get the column text and format.
	TCHAR szText[ 256 ];
	HD_ITEM hditem;

	hditem.mask = HDI_TEXT | HDI_FORMAT;
	hditem.pszText = szText;
	hditem.cchTextMax = 255;

	VERIFY(GetItem(lpDrawItemStruct->itemID, &hditem));

		// determine the format for drawing the column label.
		// embedded C++ not recognizing DT_END_ELLIPSIS
//	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS ;
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER;

	if		( hditem.fmt & HDF_CENTER )	uFormat |= DT_CENTER;
	else if ( hditem.fmt & HDF_LEFT   ) uFormat |= DT_LEFT;
	else								uFormat |= DT_RIGHT;

	// adjust the rect if the mouse button is pressed on it.
	if ( lpDrawItemStruct->itemState == ODS_SELECTED )
	{
		rc.left++;
		rc.top += 2;
		rc.right++;
	}

	CRect rcIcon(lpDrawItemStruct->rcItem);
	const int iOffset = (rcIcon.bottom - rcIcon.top) / 4;

		// adjust the rect further if the sort arrow is to be displayed.
	if ( lpDrawItemStruct->itemID == (UINT)m_iSortColumn )
		rc.right -= 3 * iOffset;

	rc.left += iOffset;
	rc.right -= iOffset;

		// draw the column label.
	if ( rc.left < rc.right )
		(void)dc.DrawText(szText, -1, rc, uFormat);

		// draw the sort arrow.
	if ( lpDrawItemStruct->itemID == (UINT)m_iSortColumn )
	{
		// set up the pens to use for drawing the arrow.
		CPen penLight( PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT) );
		CPen penShadow( PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW) );
		CPen* pOldPen = dc.SelectObject( &penLight );

		if ( m_bSortAscending )
		{		// draw the arrow pointing upwards.
			dc.MoveTo( rcIcon.right - 2 * iOffset, iOffset );
			dc.LineTo( rcIcon.right - iOffset, rcIcon.bottom - iOffset - 1 );
			dc.LineTo( rcIcon.right - 3 * iOffset - 2, rcIcon.bottom - iOffset - 1 );
			(void)dc.SelectObject( &penShadow );
			dc.MoveTo( rcIcon.right - 3 * iOffset - 1, rcIcon.bottom - iOffset - 1 );
			dc.LineTo( rcIcon.right - 2 * iOffset, iOffset - 1 );
		}
		else
		{		// draw the arrow pointing downwards.
			dc.MoveTo( rcIcon.right - iOffset - 1, iOffset );
			dc.LineTo( rcIcon.right - 2 * iOffset - 1, rcIcon.bottom - iOffset );
			(void)dc.SelectObject( &penShadow );
			dc.MoveTo( rcIcon.right - 2 * iOffset - 2, rcIcon.bottom - iOffset );
			dc.LineTo( rcIcon.right - 3 * iOffset - 1, iOffset );
			dc.LineTo( rcIcon.right - iOffset - 1, iOffset );		
		}
			// restore the pen.
		(void)dc.SelectObject( pOldPen );
	}

	// restore the previous device context.
	VERIFY( dc.RestoreDC(iSavedDC) );

	// detach the device context before returning.
	(void)dc.Detach();
}
*/
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Interface Extensions						//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
int		CRedBlackListCtrl::AddItem( LPARAM pItem, int iItemNo )
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
/*
void	CRedBlackListCtrl::CSortHeaderCtrl::UpdateSortArrow()
{
	// change the item to owner drawn.
	HD_ITEM hditem;

	hditem.mask = HDI_FORMAT;
	VERIFY( GetItem( m_iSortColumn, &hditem ) );
	hditem.fmt |= HDF_OWNERDRAW;
	VERIFY( SetItem( m_iSortColumn, &hditem ) );

	// invalidate the header control so it gets redrawn
	Invalidate();
}
*/
//----------------------------------------------------------------------------
/*	typedef struct tagNMLVCUSTOMDRAW
	{	NMCUSTOMDRAW	nmcd;
		COLORREF		clrText;
		COLORREF		clrTextBk;
	} NMLVCUSTOMDRAW, *LPNMLVCUSTOMDRAW;
	typedef struct tagNMCUSTOMDRAWINFO
	{	NMHDR	hdr;
		DWORD	dwDrawStage;		// PREPAINT or whatever
		HDC		hdc;
		RECT	rc;
		DWORD	dwItemSpec;			// the list control row index
		UINT	uItemState;
		LPARAM	lItemlParam
	} NMCUSTOMDRAW, FAR * LPNMCUSTOMDRAW;

#define CDRF_DODEFAULT          0x0
#define CDRF_NEWFONT            0x00000002
#define CDRF_SKIPDEFAULT        0x00000004
#define CDRF_NOTIFYPOSTPAINT    0x00000010
#define CDRF_NOTIFYITEMDRAW     0x00000020
#define CDRF_NOTIFYSUBITEMDRAW  0x00000020
#define CDRF_NOTIFYPOSTERASE    0x00000040
#define CDRF_NOTIFYITEMERASE    0x00000080      //  obsolete ???

// drawstage flags 

#define CDDS_PREPAINT           1
#define CDDS_POSTPAINT          2
#define CDDS_PREERASE           3
#define CDDS_POSTERASE          4

#define CDDS_ITEM				0x00010000
#define CDDS_ITEMPREPAINT	(CDDS_ITEM | CDDS_PREPAINT)
#define CDDS_ITEMPOSTPAINT	(CDDS_ITEM | CDDS_POSTPAINT)
#define CDDS_ITEMPREERASE	(CDDS_ITEM | CDDS_PREERASE)
#define CDDS_ITEMPOSTERASE	(CDDS_ITEM | CDDS_POSTERASE)
#define CDDS_SUBITEM            0x00020000
*/
