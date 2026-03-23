// BOSListCtrl.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "BOSListCtrl.h"
#include "PositionDisplayOptions.h"
#include "Bond.h"
//#include "Stock.h"
#include "Position.h"
//#include "NillaHedgeDoc.h"

//#include "Option.h"
//#include "BondPositionDialog.h"
//#include "OptionPositionDialog.h"
//#include "StockPositionDialog.h"
#include "PositionsDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBOSListCtrl

IMPLEMENT_DYNAMIC( CBOSListCtrl, CRedBlackListCtrl )

CBOSListCtrl::CBOSListCtrl() : CRedBlackListCtrl()
{
}

CBOSListCtrl::~CBOSListCtrl()
{
	CRedBlackListCtrl::~CRedBlackListCtrl();
}

BEGIN_MESSAGE_MAP(CBOSListCtrl, CRedBlackListCtrl)
	//{{AFX_MSG_MAP(CBOSListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//						CBOSListCtrl message handlers						//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//						Delete/Modify Positions/Total Row					//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CBOSListCtrl::DeleteTotalRow( void )
{
	int		nItems = GetItemCount();
	if ( nItems > 2 )
	{	LPARAM	iItem = GetItemData( nItems-1 );
			// the first element in any (non-Total row) ItemInfo
			// is some sort of PositionListItemInfo*
			// for the total row, thePosition == 0 
		if ( ((BOSPositionListItemInfo*)iItem)->thePosition == 0 )
		{		// its the total row
			DeleteItem( nItems-1 );
		}
	}
}			// DeleteTotalRow()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//									Overrides								//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
BOOL	CBOSListCtrl::DeleteItem( int iItem )
{
	DWORD dwData = GetItemData( iItem );
	BOOL success = CListCtrl::DeleteItem(iItem);	// use CListCtrl's version
	if ( success )
	{		// Free the heap resident struct
		BOSPositionListItemInfo* pData = reinterpret_cast<BOSPositionListItemInfo*>(dwData);
		if ( pData != NULL )
			delete	pData;
	}
	return success;
}			// DeleteItem()
//----------------------------------------------------------------------------
void	CBOSListCtrl::DeleteAllItems( void )
{		// Delete all of the row entries.
	short	nItemCount = GetItemCount();
	for ( short	ii = 0; ii < nItemCount; ii++ )
		DeleteItem(0);
}			// DeleteAllItems()
//----------------------------------------------------------------------------
void	CBOSListCtrl::DeleteAllColumns( void )
{		// Delete all of the columns.
	short	nColumnCount = GetHeaderCtrl()->GetItemCount();
	for ( short	ii = 0; ii < nColumnCount; ii++ )
		DeleteColumn(0);
}			// DeleteAllColumns()
//----------------------------------------------------------------------------
