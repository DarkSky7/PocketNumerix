#include "StdAfx.h"
#include "resource.h"
#include "TreeListCtrl.h"
//#include "BOSDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern	CNillaHedgeApp	theApp;
const	short	RowHeight = 24;
const	short	ImageWidth = 16;

BEGIN_MESSAGE_MAP( CTreeListCtrl, CListCtrl )
	//{{AFX_MSG_MAP(CTreeListCtrl)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
//	ON_WM_KEYDOWN()
//	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl

CTreeListCtrl::CTreeListCtrl()
{	m_ItemCount = 0;
}

CTreeListCtrl::~CTreeListCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl overrides

void	CTreeListCtrl::PreSubclassWindow()
{		// LVS_EX_GRIDLINES // 5.0: LVS_EX_LABELTIP 
	SetExtendedStyle( LVS_EX_FULLROWSELECT );

	if ( m_ilState.GetSafeHandle() == NULL )
		m_ilState.Create( IDB_STATE, 16, 1, RGB(255, 0, 0) );
	SetImageList( &m_ilState, LVSIL_STATE );

	CListCtrl::PreSubclassWindow();
}			// PreSubclassWindow()
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl operations

BOOL	CTreeListCtrl::Create(
	DWORD			dwStyle,
	const RECT&		rect,
	CWnd*			pParentWnd,
	UINT			nID				)
{		// initialize m_SuppressPositions
/*
	unsigned long	areVisible = 0xFFFFFFFF;
	if ( ! theApp.pDB->getPosListColVisibility(areVisible) )
	{
#ifdef _DEBUG
		TRACE( _T("TreeListCtrl::Create:  no column visibility preferences found.\n") );
#endif
	}
	m_SuppressPositions = (areVisible & InitiallySuppressPositions != 0);
*/
	if ( ! CListCtrl::Create( dwStyle, rect, pParentWnd, nID ) )
		return	FALSE;

		// LVS_EX_GRIDLINES // 5.0: LVS_EX_LABELTIP 
	SetExtendedStyle( LVS_EX_FULLROWSELECT );
	if ( m_ilState.GetSafeHandle() == NULL )
		m_ilState.Create( IDB_STATE, ImageWidth, 1, RGB(255, 0, 0) );

	SetImageList( &m_ilState, LVSIL_STATE );
	return TRUE;
}			// Create()
//----------------------------------------------------------------------------
int		CTreeListCtrl::AddItem( PortfolioListItemInfo* plii )
{		// either track plii in the ListCtrl or shunt it into m_ItemsNotDisplayed
	ASSERT( plii );

		// decide whether to show this item (initially)
	if ( plii->level != Positions )
		plii->visible = true;

		// always store it in m_RowData
	int	ret = m_RowData.Add( (void*)plii );
#ifdef _DEBUG
	if ( ret < 0 )
		TRACE( _T("TreeListCtrl::AddItem(1): couldn't store an item in m_RowData.\n") );
#endif
	plii->rowDataIdx = ret;

		// track the hierarchy
	OverviewItemType	level = plii->level;
	if ( level == Instruments )
		m_Portfolio.Add( (void*)ret );		// Instrument summaries - Options, Stocks
	else if ( level == Issues )
	{	AssetType		inst = plii->inst;
		switch ( inst )
		{	case Option:
				m_Options.Add( (void*)ret );	// Option summaries
				break;
			case Stock:
				m_Stocks.Add( (void*)ret );		// Stock summaries
				break;
			default:							// UnknownAssetType
				break;
		}
	}

		// if it's visible, insert it into the ListCtrl
	if ( plii->visible )
	{	ret = InsertItem( plii, plii->rowDataIdx );
#ifdef _DEBUG
		if ( ret < 0 )
			TRACE( _T("TreeListCtrl::AddItem(2): couldn't insert the item in the ListCtrl.\n") );
#endif
	}
	return	ret;
}			// AddItem()
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl protected functions

int		CTreeListCtrl::InsertItem( PortfolioListItemInfo* plii, int iItem )
{		// decide whether to display leaf nodes, insert nodes we want to be visible
	ASSERT( plii );
	LVITEM		lvi;
	lvi.iItem	  = iItem;
	lvi.iSubItem  = 0;
	lvi.mask	  = LVIF_TEXT | LVIF_INDENT | LVIF_STATE | LVIF_PARAM;
	lvi.pszText   = LPSTR_TEXTCALLBACK;
	lvi.stateMask = LVIS_STATEIMAGEMASK;
	lvi.lParam	  = (LPARAM)plii;
	lvi.iIndent   = plii->level * ImageWidth;
		// temporarily, hasChildren represents the potential of having children
	bool	prevItemIsVisible = false;		// have to pick some default behavior
	int		prevIdx = plii->rowDataIdx - 1;
	bool	hasChildren = plii->level < 3  &&  prevIdx >= 0;
	if ( prevIdx >= 0 )
	{	PortfolioListItemInfo*	pli2 = (PortfolioListItemInfo*)m_RowData[ prevIdx ];
			// there is a previous entry in the ListCtrl or in m_ItemsNotDisplayed
		prevItemIsVisible = pli2  &&  pli2->visible;
		hasChildren		  = pli2  &&  pli2->level > plii->level;
	}
		// the state image mask index represents ...
		// 3 - a blank (doesn't have children)
		// 2 - a minus sign (has children, but they're already displayed)
		// 1 - a plus sign (has children that aren't displayed)
	int	idx = ( !hasChildren ) ? 3 : (prevItemIsVisible ? 2 : 1);
	lvi.state = INDEXTOSTATEIMAGEMASK( idx );
	return	CListCtrl::InsertItem( &lvi );
}			// InsertItem()
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl message handlers

void	CTreeListCtrl::OnClick(
	NMHDR*				pNMHDR,
	LRESULT*			pResult		)
{		// Get the current mouse location and convert it to client coordinates.
	DWORD	pos = GetMessagePos();
	CPoint	pt( LOWORD(pos), HIWORD(pos) );
	ScreenToClient( &pt );
#ifdef _DEBUG
	TRACE( _T("TreeListCtrl::OnClick: Client Pt(%d,%d)\n"), pt.x, pt.y );
#endif
		// get the item index
	int	selIdx = HitTest( pt );
	if ( selIdx >= 0  &&  5 <= pt.x  &&  pt.x <= 13 )						// selIdx is 0-based
	{	LVITEM	item;
		item.iItem = selIdx;
		item.iSubItem = 0;
		item.mask = LVIF_INDENT | LVIF_STATE | LVIF_PARAM;
		int	res = GetItem( &item );
		if ( res >= 0 )
		{	PortfolioListItemInfo*	plii = (PortfolioListItemInfo*)item.lParam;
			if ( plii  &&  plii->rowDataIdx > 0 )
			{		// children are possible
				PortfolioListItemInfo*	pli2 = (PortfolioListItemInfo*)m_RowData[ plii->rowDataIdx - 1 ];
				if ( pli2 )
				{		// plii definitely has children
					SetRedraw( 0 );
					if ( pli2->visible )
					{		// children are visible - hide them
#ifdef _DEBUG
						TRACE( _T("TreeListCtrl::OnClick: Collapse iItem=%d, rdi=%d, level=%d, visible=%s\n"),
							selIdx, plii->rowDataIdx, plii->level, plii->visible ? _T("true") : _T("false") );
#endif
						SetItemState( selIdx,
							INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK );
						ExpandContract( plii->rowDataIdx, selIdx, false );
					}
					else
					{		// children are not visible - show them
#ifdef _DEBUG
						TRACE( _T("TreeListCtrl::OnClick: Expand iItem=%d, rdi=%d, level=%d, visible=%s\n"),
							selIdx, plii->rowDataIdx, plii->level, plii->visible ? _T("true") : _T("false") );
#endif
						SetItemState( selIdx,
							INDEXTOSTATEIMAGEMASK(2), LVIS_STATEIMAGEMASK );
						ExpandContract( plii->rowDataIdx, selIdx, true );
					}
					SetRedraw( 1 );
				}
#ifdef _DEBUG
				else
					TRACE( _T("TreeListCtrl::OnClick: bad predecessor item retrived from m_RowData.\n") );
#endif
			}
		}
	}
	*pResult = 0;
}			// OnClick()
//---------------------------------------------------------------------------
int		CTreeListCtrl::ExpandContract( int prdi, int pIdx, bool expand )
{		// insert 'invisible' items from m_RowData into the ListCtrl
	PortfolioListItemInfo*	plii = (PortfolioListItemInfo*)m_RowData[ prdi ];
	if ( ! plii )
	{
#ifdef _DEBUG
		TRACE( _T("TreeListCtrl::ExpandContract(1): bad parent PortfolioListItemInfo retrieved.\n") );
#endif
		return	0;
	}
	int		itemsAffected = 0;
	OverviewItemType	pLev = plii->level;
	switch ( pLev )
	{	case Issues:
		{		// conduct a linear scan through m_RowData above prdi for Position level entries
			int		crdi = prdi - 1;
			while ( crdi >= 0 )
			{	PortfolioListItemInfo*	pli2 = (PortfolioListItemInfo*)m_RowData[ crdi-- ];
				if ( pli2 == NULL )
				{
#ifdef _DEBUG
					TRACE( _T("TreeListCtrl::ExpandContract(Issues): bad child PortfolioListItemInfo retrieved.\n") );
#endif
				}
				else if ( pli2->level == Positions )
				{		// ensure that we're not counting deletion/insertion
						//	   of items that are already invisible/visible
					if ( expand != pli2->visible )
					{	pli2->visible = expand;
						itemsAffected++;			// 1, whether it's an Insert or a Delete
						if ( expand )
							InsertItem( pli2, pIdx++ );
						else
							DeleteItem( --pIdx );
					}
				}
				else break;		// this is a valid stopping condition
			}		
			break;
		}
		case Instruments:
		{		// walk through the Option/Stock suummaries
			AssetType	inst = plii->inst;
			ASSERT( inst != UnknownAssetType );
				// decide which type of Instrument summary we're working on
			CPtrArray*	instSummaries = &m_Stocks;
			if ( inst == Option ) instSummaries = &m_Options;
			// else if ( inst == Stock  ) instSummaries = &m_Stocks;
				// loop through the Instrument summaries
			ASSERT( instSummaries != NULL );
			const int	nSummaries = instSummaries->GetSize();
			for ( int ii = 0; ii < nSummaries; ii++ )
			{	PortfolioListItemInfo*	pli2 = (PortfolioListItemInfo*)m_RowData[ (int)(*instSummaries)[ii] ];
				if ( pli2  &&  pli2->visible != expand )
				{	pli2->visible = expand;
					if ( expand )
					{	itemsAffected++;
						InsertItem( pli2, pIdx + ii );
					}
					else
					{		// recurse 1 level down...
						int	killed = ExpandContract( pli2->rowDataIdx, --pIdx, expand );
						itemsAffected += killed;
						pIdx -= killed;
						DeleteItem( pIdx );			// reduce pIdx by number of lower level items deleted
					}
				}
#ifdef _DEBUG
				else
					TRACE( _T("TreeListCtrl::ExpandContract(Instruments): bad child PortfolioListItemInfo retrieved.\n") );
#endif
			}
			break;
		}
		case Portfolio:
				// walk through the Instrument summaries
			if ( expand )
			{	const int	portSize = m_Portfolio.GetSize();
				for ( int ii = 0; ii < portSize; ii++ )
				{	PortfolioListItemInfo*	pli2 = (PortfolioListItemInfo*)m_RowData[ (int)m_Portfolio[ii] ];
					if ( pli2  &&  pli2->visible != expand )
					{	pli2->visible = expand;										// true
						itemsAffected++;
						InsertItem( pli2, pIdx + ii );
					}
				}
			}
			else
			{		// selective deletion (deleting all but the Portfolio row) failed, but
					// the following works fine...
					// (1) delete all ListCtrl rows
					// (2) mark all but the last RowData entry as invisible
					// (3) reinsert the Portfolio row ...
					// no recursive calls can land here, so we can overwrite itemsAffected
					// rather than adding the itemCount to it
				DeleteAllItems();						
				const	unsigned short	nRows = m_RowData.GetSize();
				for ( int jj = 0; jj < nRows - 1; jj++ )
				{
#ifdef _DEBUG
					TRACE( _T("Updating RowData[%d]\n"), jj );
#endif
					((PortfolioListItemInfo*)m_RowData[jj])->visible = expand;		// false
				}
				itemsAffected = m_RowData.GetSize() - 1;
				PortfolioListItemInfo*	pli2 = (PortfolioListItemInfo*)m_RowData[ itemsAffected ];
				InsertItem( pli2, 0 );
			}
			break;
		default:
			break;
	}
	return	itemsAffected;
}			// Expand()
//---------------------------------------------------------------------------
void	CTreeListCtrl::OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult ) 
{	NMLVCUSTOMDRAW* pCustomDraw = (NMLVCUSTOMDRAW*)pNMHDR;
	NMCUSTOMDRAW	nmcd = pCustomDraw->nmcd;
		// for a color table, see:  http://www.w3schools.com/html/html_colornames.asp
	COLORREF	posBck = RGB( 196, 232, 232 );		// Powder Blue (position rows)
	COLORREF	instBck = RGB( 224, 224, 180 );		// Manilla (instrument summary rows)

	int		si, rr = nmcd.dwItemSpec;				// item of interest (row: rr)
	switch ( nmcd.dwDrawStage )
	{	case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
		case CDDS_ITEMPREPAINT:
		{		// change the background color of items
			PortfolioListItemInfo*	plii = (PortfolioListItemInfo*)GetItemData( rr );
			if ( plii->level == Instruments )
				pCustomDraw->clrTextBk = instBck;
			else if ( plii->level == Positions )
				pCustomDraw->clrTextBk = posBck;

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
			PortfolioListItemInfo*	plii = (PortfolioListItemInfo*)GetItemData( rr );
			if ( plii->level == Instruments )
				pCustomDraw->clrTextBk = instBck;
			else if ( plii->level == Positions )
				pCustomDraw->clrTextBk = posBck;
			
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
//-----------------------------------------------------------------------------
/*
void	CTreeListCtrl::OnKeyDown(
	UINT	nChar,
	UINT	nRepCnt,
	UINT	nFlags			)
{	int		nItem = GetNextItem( -1, LVNI_SELECTED );	

	if ( nItem == -1 )
	{
		CRedBlackListCtrl::OnKeyDown( nChar, nRepCnt, nFlags );
		return;
	}
	LVITEM		item;
	item.mask = LVIF_INDENT | LVIF_PARAM;
	item.iItem = nItem;
	item.iSubItem = 0;
	GetItem(&item);

	switch (nChar)
	{
		case VK_ADD:
			if ( m_RowData[item.lParam].m_bCollapsed )
			{	int		nInsert = nItem;
				SetRedraw( 0 );
				Expand( item.lParam, false, nInsert );
				SetRedraw( 1 );
				SetItemState( nItem,
					INDEXTOSTATEIMAGEMASK(2), LVIS_STATEIMAGEMASK );
			}
			break;
		case VK_SUBTRACT:
			if ( !m_RowData[item.lParam].m_bCollapsed )
			{	SetRedraw( 0 );
				Collapse( item.lParam, nItem );
				SetRedraw( 1 );
				SetItemState( nItem,
					INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK );
			}
			break;
		case VK_RIGHT:
			if ( m_RowData[item.lParam].m_bCollapsed )
			{	int		nInsert = nItem;
				SetRedraw( 0 );
				Expand( item.lParam, false, nInsert );
				SetRedraw( 1 );
				SetItemState( nItem,
					INDEXTOSTATEIMAGEMASK(2), LVIS_STATEIMAGEMASK );
			}
			else
			{		// If any child, move to the first one
				int nNextItem	= item.lParam + 1;
				if (     nNextItem < m_RowData.size()
					 &&  m_RowData[nNextItem].m_nLevel > m_RowData[item.lParam].m_nLevel )
					SetItemState( nItem + 1,
						LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
			}
			break;
		case VK_LEFT:
			if ( !m_RowData[item.lParam].m_bCollapsed )
			{	SetRedraw( 0 );
				Collapse( item.lParam, nItem );
				SetRedraw( 1 );
				SetItemState( nItem,
					INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK );
			}
			else
			{		// Move to our parent
				int nPrevItem = item.lParam - 1;
				int	nLevel = m_RowData[item.lParam].m_nLevel;
				while (     nPrevItem >= 0
						&&  nLevel <= m_RowData[nPrevItem].m_nLevel )
					--nPrevItem;

				if (nPrevItem >= 0)
				{	LVFINDINFO		info;	
					info.flags = LVFI_PARAM;
					info.lParam	= nPrevItem;
					SetItemState( FindItem(&info),
						LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED );
				}
			}
			break;
		default:
			CRedBlackListCtrl::OnKeyDown( nChar, nRepCnt, nFlags );
			break;
	}
}			// KeyDown()
//---------------------------------------------------------------------------
void	CTreeListCtrl::OnChar(
	UINT	nChar,
	UINT	nRepCnt,
	UINT	nFlags			)
{	int		nItem = GetNextItem( -1, LVNI_SELECTED );	

	if ( nItem != -1 )
	{	LVITEM			item;
		item.mask		= LVIF_INDENT | LVIF_PARAM;
		item.iItem		= nItem;
		item.iSubItem	= 0;
		GetItem( &item );
		if ( nChar == '*' )
		{		// TODO: Recursively expand items 
			if (m_RowData[item.lParam].m_bCollapsed)
			{	int		nInsert = nItem;
				SetRedraw( 0 );
				Expand( item.lParam, true, nInsert );
				SetRedraw( 1 );
				SetItemState( nItem,
					INDEXTOSTATEIMAGEMASK(2), LVIS_STATEIMAGEMASK );
			}
		}	
	}
	CRedBlackListCtrl::OnChar( nChar, nRepCnt, nFlags );
}			// OnChar()
*/
//---------------------------------------------------------------------------
/*
BOOL	CTreeListCtrl::SetItemText(
	int			nItem,
	int			nSubItem,
	LPCTSTR		lpszText			)
{
	m_RowData.at(nItem).m_lstCols[nSubItem] = lpszText;
	return	CRedBlackListCtrl::SetItemText( nItem, nSubItem, lpszText );
}			// SetItemText()
*/
//---------------------------------------------------------------------------
/*
BOOL	CTreeListCtrl::DeleteAllItems()
{
	m_RowData.clear();
	return	CRedBlackListCtrl::DeleteAllItems();
}			// DeleteAllItems()
*/
//---------------------------------------------------------------------------
