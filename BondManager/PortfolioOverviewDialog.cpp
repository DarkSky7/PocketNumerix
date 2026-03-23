// PortfolioOverviewDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
//#include "NillaHedge.h"
#include "AssetType.h"
#include "PortfolioOverviewDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Bond.h"
//#include "Stock.h"
//#include "Option.h"
#include "Position.h"
#include "BondManager.h"
//#include "OptionManager.h"
//#include "StockManager.h"
#include "RegistryManager.h"
//#include "NillaHedgeDoc.h"
//#include "OleDateTimeEx.h"

#include "BondManagerApp.h"
extern	CBondManagerApp	theApp;
extern	CString months[];

IMPLEMENT_DYNAMIC( CPortfolioOverviewDialog, CNillaDialog )

/////////////////////////////////////////////////////////////////////////////
// CPortfolioOverviewDialog dialog

CPortfolioOverviewDialog::CPortfolioOverviewDialog( CWnd* pParent /* =NULL */ )
	: CNillaDialog( IDD, pParent )
{
	//{{AFX_DATA_INIT(CPortfolioOverviewDialog)
	//}}AFX_DATA_INIT
}

CPortfolioOverviewDialog::~CPortfolioOverviewDialog( void )
{
}

void CPortfolioOverviewDialog::DoDataExchange(CDataExchange* pDX)
{
	CNillaDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPortfolioOverviewDialog)
	DDX_Control( pDX, IDC_PositionsList, c_PositionsList );
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( CPortfolioOverviewDialog, CNillaDialog )
	//{{AFX_MSG_MAP(CPortfolioOverviewDialog)
	ON_NOTIFY( LVN_GETDISPINFO, IDC_PositionsList, OnGetDispInfoPositionsList )
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_NOTIFY( NM_CUSTOMDRAW, IDC_PositionsList, OnCustomDrawPositionsList )
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
// CPortfolioOverviewDialog message handlers

BOOL CPortfolioOverviewDialog::OnInitDialog() 
{
	CNillaDialog::OnInitDialog();

	InitListCtrl();

	LoadRows();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//----------------------------------------------------------------------------------------
HBRUSH	CPortfolioOverviewDialog::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	return	CNillaDialog::OnCtlColor( pDC, pWnd, nCtlColor );
}			// OnCtlColor()
//----------------------------------------------------------------------------------------
void	CPortfolioOverviewDialog::InitListCtrl( void )
{		// ensure that the next sort on the first column will be ascending
	lastSortKey = -1;		// this is a 1-based column index

		// nCols initialized in InsertColumns and
		//       will reflect inclusion of a dummy column
	InsertColumns();

	const short		maxCols = 10;
	bool			optns[maxCols];
	short			op = 0;
	long	areVisible = 0xFFFFFFFF;							// everything is visible by default
	if ( ! GetPosListColVisibility( &areVisible ) )
	{
#ifdef _DEBUG
		TRACE( _T("PortfolioOverviewDialog::InitListCtrl: no column visibility preferences found.\n") );
#endif
	}
	optns[op++] = (areVisible & PurchaseDateOverview)		!= 0;		//    0
	optns[op++] = (areVisible & NumUnitsOverview)			!= 0;		//	  1
	optns[op++] = (areVisible & InitialCostOverview)		!= 0;		//	  2
	optns[op++] = (areVisible & MarketValueOverview)		!= 0;		//	  3
	optns[op++] = (areVisible & CapitalGainOverview)		!= 0;		//	  4
	optns[op++] = (areVisible & AggregateIncomeOverview)	!= 0;		//	  5
	optns[op++] = (areVisible & NetGainOverview)			!= 0;		//	  6
	optns[op++] = (areVisible & AnnualizedNetGainOverview)	!= 0;		//	  7
	optns[op++] = (areVisible & NoteOverview)				!= 0;		//	  8

	initiallyShowPositions = (areVisible & InitiallySuppressPositions)	== 0;

	unsigned short nCols = op, cm = 0;
	for ( op = 0; op < nCols; op++ )
		if ( optns[ op ] )
			colMap[cm++] = op;

		// the following is probably moot (because these indices will
		// never get used), but it eliminates zeros which would 
		// otherwise appear to be valid map targets
	for ( ; cm < maxCols; cm++ )
		colMap[cm] = -1;

		// set extended styles, but skip LVS_EX_GRIDLINES
		// because they don't match up with text heights
	int	flags = LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT;
	c_PositionsList.SetExtendedStyle( c_PositionsList.GetExtendedStyle() | flags );
}			// InitListCtrl()
//----------------------------------------------------------------------------------------
void	CPortfolioOverviewDialog::InsertColumns( void )
{		// in the order they will be displayed in the list control
	wchar_t*		colHdrs[] = {
		_T("Date/Name"), _T("Bonds "),	_T("Cost "),   _T("MktValue"), _T("CapGain"),
		_T("Income"),	 _T("NetGain"), _T("% Yield"), _T("Note/Desc")	};

		// add columns to the List Control
	unsigned short	nCols = 0;
	unsigned short	hdrIdx = 0;
	long	areVisible = 0xFFFFFFFF;					// everything is visible by default
	if ( ! GetPosListColVisibility( &areVisible ) )
	{
#ifdef _DEBUG
		TRACE( _T("PortfolioOverviewDialog::InsertColumns: no column visibility preferences found.\n") );
#endif
	}

	if ( areVisible & PurchaseDateOverview )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_LEFT,  84 );		// was 92
	hdrIdx++;			// whether dspOptns->purchaseDate is true or not

		// continue with initialization...
	if ( areVisible & NumUnitsOverview )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_RIGHT, 43 );		// was 50	
	hdrIdx++;

	if ( areVisible & InitialCostOverview )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_RIGHT, 66 );
	hdrIdx++;			// whether areVisible & InitialCost is true or not

	if ( areVisible & MarketValueOverview )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_RIGHT, 66 );
	hdrIdx++;			// whether areVisible & MarketValue is true or not

	if ( areVisible & CapitalGainOverview )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_RIGHT, 60 );
	hdrIdx++;			// whether areVisible & CapitalGain is true or not

	if ( areVisible & AggregateIncomeOverview )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_RIGHT, 60 );
	hdrIdx++;			// whether areVisible & AggregateIncome is true or not

	if ( areVisible & NetGainOverview )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_RIGHT, 60 );
	hdrIdx++;			// whether areVisible & NetGain is true or not

	if ( areVisible & AnnualizedNetGainOverview )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_RIGHT, 60 );
	hdrIdx++;			// whether areVisible & AnnualizedNetGain is true or not

	if ( areVisible & NoteOverview )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_LEFT,  120 );
	hdrIdx++;			// whether areVisible & Note is true or not

		// insert a dummy column (so the previous column can be resized)
	if ( nCols > 0 )
		c_PositionsList.InsertColumn( nCols, _T(""), LVCFMT_LEFT, 30 );
}			// InsertColumns()
//----------------------------------------------------------------------------
void		CPortfolioOverviewDialog::LoadRows( void )
{	int		zbi;
	double	tCost;
	PortfolioListItemInfo*	portfolio = new PortfolioListItemInfo;

		// load bonds
	PortfolioListItemInfo*	bondSummary = NULL;
	int	res = theApp.bndMgr->NumIndexedIssues();
	if ( res > 0 )
	{	bondSummary = new PortfolioListItemInfo;
		bondSummary->inst = Bond;
		bondSummary->level = Instruments;			// i.e. all Bonds
		bondSummary->dateName = bondSummary->noteDesc = _T("Total Bonds");
		bondSummary->nUnits		= 0.0;
		bondSummary->totalCost	= 0.0;
		bondSummary->mktValue	= 0.0;
		bondSummary->exerIncome = 0.0;
		double		   wtdYield	= 0.0;
		for ( zbi = 0; ; zbi++ )
		{	CBond*	aBond = theApp.bndMgr->GetIndexedBond( zbi ); 
			if ( aBond == NULL )
				break;
			PortfolioListItemInfo*	issueSummary = LoadPositions( aBond );
			bondSummary->nUnits		+= issueSummary->nUnits;
			bondSummary->totalCost	+= issueSummary->totalCost;
			bondSummary->mktValue	+= issueSummary->mktValue;
			bondSummary->exerIncome += issueSummary->exerIncome;
			wtdYield += issueSummary->totalCost * issueSummary->pctYield;
			delete	aBond;		// deletes the position list too
		}
		tCost = bondSummary->totalCost;
		bondSummary->pctYield = (float) ((fabs(tCost) < 1e-5)  ?  0.0  :  (wtdYield / tCost));
			// put the bond summary row into the rowData array and the PositionsList
		c_PositionsList.AddItem( bondSummary );
	}
}			// LoadRows()
//----------------------------------------------------------------------------
PortfolioListItemInfo*		CPortfolioOverviewDialog::LoadPositions( CBOS* bos )
{	AssetType	inst = bos->getAssetType();
	ASSERT( inst != UnknownAssetType );
	double	wtdYield = 0.0;

	CMapPtrToPtr*	posLst = bos->getPositionList();
	ASSERT( posLst );
	PortfolioListItemInfo*	issueSummary = new PortfolioListItemInfo;
	issueSummary->inst = inst;					// Bond/Option/Stock
	issueSummary->level = Issues;				// issueSummary for an issue (a Bond/Option/Stock)
	issueSummary->dateName = bos->getSymbol();
	issueSummary->nUnits = 0.0;
	issueSummary->totalCost = 0.0;
	issueSummary->mktValue = 0.0;
	issueSummary->exerIncome = 0.0;
	issueSummary->pctYield = 0.0;
	issueSummary->noteDesc = bos->getDesc();
		
		// for each position, insert a row, then insert the issueSummary row
	CPosition*	pos;
	long	posOffset = bos->openPosOffset;
	while ( posLst->Lookup( (void*)posOffset, (void*&)pos ) )
	{		// 'copy' the easy stuff
		PortfolioListItemInfo*	plii = new PortfolioListItemInfo;
		plii->inst = inst;						// Bond/Option/Stock
		plii->level = Positions;				// leaf level position data in the TreeList
		plii->dateName = EuroFormat( pos->purchaseDate );
		plii->noteDesc = pos->getNote();
		plii->nUnits = pos->nUnits;
		plii->totalCost = pos->totalCost;
		plii->mktValue = pos->nUnits * bos->getMktPrice();
		plii->visible = initiallyShowPositions;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// aggregate income or exercise value
		COleDateTime	today = COleDateTime::GetCurrentTime();
		double	income = ((CBond*)bos)->PresentValueCoupons( pos->purchaseDate, today, 0.0 );
		income *= pos->nUnits;
		plii->exerIncome = income;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// netGain
		double	topLine = plii->mktValue + income;
		double	netGain = topLine - plii->totalCost;
		double	rateArg = topLine / plii->totalCost;

			// percent yield calculation
		long	spDays;
		double	yrs = calcYears( pos->purchaseDate, today, ActualActual, spDays );
		bool	skipCalc = ( rateArg == 0.0  ||  yrs < 1e-4 );
		double	pctYield = skipCalc  ?  0.0
								     :  ( 100.0 * log(fabs(rateArg)) / yrs );
		plii->pctYield = (float)pctYield;

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// put the row into the rowData array and the PositionsList
		c_PositionsList.AddItem( plii );

			// contribute to the issueSummary row
		issueSummary->nUnits	 += plii->nUnits;
		issueSummary->totalCost  += plii->totalCost;
		issueSummary->mktValue	 += plii->mktValue;
		issueSummary->exerIncome += plii->exerIncome;
			// rescale(up) the (double)pctYield by dollars invested
		wtdYield += plii->totalCost * pctYield;

			// prepare for next loop pass
		posOffset = pos->prevPos_ID;
	}
	double	tCost = issueSummary->totalCost;
		// rescale(down) the issue's percent yield by the aggregate cost
	issueSummary->pctYield = (float) ( (fabs(tCost) < 1e-7)  ?  0.0  :  (wtdYield / tCost) );
	c_PositionsList.AddItem( issueSummary );
	return	issueSummary;
}			// LoadPositions()
//----------------------------------------------------------------------------
CString		CPortfolioOverviewDialog::GetPositionItemText( int rr, int si )
{		// return a CString representation of the data in row rr, subitem si

		// get the sortCol for this subitem
	short	msi = MapSubitem( si );
	if ( msi < 0  ||  msi > 8 )				// not including the dummy column
		return	_T("");

		// get the item info for the row of interest (rr)
	PortfolioListItemInfo*	pItem = (PortfolioListItemInfo*)c_PositionsList.GetItemData( rr );
	if ( pItem == NULL )
		return	_T("");						// not sure why this would happen

		// 0th column - purchaseDate, issue name, or issueSummary row identifier
	if ( msi == 0 )
		return	pItem->dateName;

		// 8th (zero based) column - note for positions, desc for issues
	if ( msi == 8 )
		return	pItem->noteDesc;

	wchar_t	buf[256];			// if needed, where we construct the return string
	while ( 1 )
	{		// number of bonds/options/shares
		if ( msi == 1 )
		{	swprintf( buf, _T("%g"), pItem->nUnits );
			break;
		}
			// total cost
		if ( msi == 2 )
		{		// It's the total cost of the Position
			swprintf( buf, _T("%.2f"), pItem->totalCost );
			break;
		}
			// market value
		if ( msi == 3 )
		{		// It's the market value of the Position
			swprintf( buf, _T("%.2f"), pItem->mktValue );
			break;
		}
			// capital gain
		double	capGain = pItem->mktValue - pItem->totalCost;
		if ( msi == 4 )
		{	swprintf( buf, _T("%.2f"), capGain );
			break;
		}
			// aggregateIncome
		if ( msi == 5 )
		{		// have already screened out Options above
			swprintf( buf, _T("%.2f"), pItem->exerIncome );
			break;
		}
			// netGain
		double	netGain = capGain + pItem->exerIncome;
		if ( msi == 6 )
		{		// for Options, netGain == capGain (so the columns are redundant)
			swprintf( buf, _T("%.2f"), netGain );
			break;
		}
			// pctYield is 7th (zero based)
		if ( msi == 7 )
		{	double	plusYld = fabs( pItem->pctYield );
			swprintf( buf,  ((plusYld <   10.0) ? _T("%.3f")
						  : ((plusYld <  100.0) ? _T("%.2f")
						  : ((plusYld < 1000.0) ? _T("%.1f")
						  :						  _T("%.0f") ))), pItem->pctYield );
			break;
		}
			// at this point, there's nothing left to do ...
		ASSERT( false );		// we shouldn't be here
	}
	CString	cst( buf );
	return	cst;
}			// GetPositionItemText()
//----------------------------------------------------------------------------------------
void	CPortfolioOverviewDialog::OnGetDispInfoPositionsList(
	NMHDR*			pNMHDR,
	LRESULT*		pResult										) 
{	LV_DISPINFO*	pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	if ( pDispInfo->item.mask  &  LVIF_TEXT )
	{		// this function has to be here, because only the dialog knows whether
			// this ia a Bond, Option, or Stock Positions Dialog
			// GetPositionItemText resides in the <BOS>PositionDialog
		CString	cst = GetPositionItemText( pDispInfo->item.iItem,
										   pDispInfo->item.iSubItem );
		// Mike Blaszczak's approach...
		// LPTSTR	pBuf = AddPool( &cst );
		// pDispInfo->item.pszText = pBuf;

		// Tomasz Sowinski's straightforward strcpy
		//            dest           source        length
		wcsncpy( pDispInfo->item.pszText, cst, pDispInfo->item.cchTextMax);
	}
	*pResult = 0;
}			// OnGetDispInfoPositionsList()
//----------------------------------------------------------------------------
void	CPortfolioOverviewDialog::OnCustomDrawPositionsList( NMHDR* pNMHDR, LRESULT* pResult ) 
{		// direct NM_CUSTOMDRAW messages to the CustomDraw handler in RedBlackListCtrl
	c_PositionsList.OnCustomDraw( pNMHDR, pResult );
}			// OnCustomDrawPositionsList()
//----------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////
//										Button Messages									//
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
/*
CBond*		CPortfolioOverviewDialog::getBond( long offset )
{	if ( cacheBond  &&  cacheBond->def_ID == offset )
		return	cacheBond;
	delete	cacheBond;
	cacheBond = theApp.pDB->readBond( offset );
	return	cacheBond;
}			// getBond()
//----------------------------------------------------------------------------
COption*		CPortfolioOverviewDialog::getOption( long offset )
{	if ( cacheOption  &&  cacheOption->def_ID == offset )
		return	cacheOption;
	delete	cacheOption;
	cacheOption = theApp.pDB->readOption( offset );
	return	cacheOption;
}			// getOption()
//----------------------------------------------------------------------------
CStock*		CPortfolioOverviewDialog::getStock( long offset )
{	if ( cacheStock  &&  cacheStock->def_ID == offset )
		return	cacheStock;
	delete	cacheStock;
	cacheStock = theApp.pDB->readStock( offset );
	return	cacheStock;
}			// getStock()
//----------------------------------------------------------------------------
CPosition*		CPortfolioOverviewDialog::getPosition( long offset )
{	if ( cachePosition  && cachePosition->pos_ID == offset )
		return	cachePosition;
	delete	cachePosition;
	cachePosition = theApp.pDB->readOpenPosition( offset );
	return	cachePosition;
}			// getPosition()
//----------------------------------------------------------------------------
void CPortfolioOverviewDialog::OnBonds()
{	long				anOffset;
	CString				symbol;
	if ( c_Bonds.GetCheck() == BST_CHECKED )
	{		// for all symbols in theMap, look for defs with positions
		CMapStringToPtr*	theMap = theApp.pDB->getSymbolTable( Bond );
		for ( POSITION pos = theMap->GetStartPosition(); pos != NULL; )
		{
			theMap->GetNextAssoc( pos, symbol, (void*&)anOffset );
			CBond*	inst = theApp.pDB->getBond( symbol );
			if ( inst == NULL )
			{
#ifdef _DEBUG
				TRACE( _T("PortfolioOverviewDialog::OnBonds: Symbol found, but no associated Bond def.\n") );
#endif
				continue;
			}
				// create a position list entry for the instrument (which can be expanded)
			if ( inst->hasPositions() )
			{		// add a node for the instrument
				InsertInstrument( (CBOS*)inst );
				CObArray*	posLst = inst->getPositionList();
				for ( short ii = 0; ii < posLst->GetSize(); ii++ )
				{		// add a node for each position
					CPosition*	pos = posLst->GetAt( ii );
					InsertPosition( pos );
				}
			}
			delete inst;		// kills the position list too
		}
		InsertSubTotalRow();
	}
}			// OnBonds()
//----------------------------------------------------------------------------------------
void CPortfolioOverviewDialog::OnOptions() 
{	long				anOffset;
	CString				symbol;
	CMapStringToPtr*	theMap = theApp.pDB->getSymbolTable( Bond );

		// for all symbols in theMap, look for defs with positions
	for ( POSITION pos = theMap->GetStartPosition(); pos != NULL; )
	{
		theMap->GetNextAssoc( pos, symbol, (void*&)anOffset );
		COption*	inst = theApp.pDB->getOption( symbol );
		if ( inst == NULL )
		{
#ifdef _DEBUG
			TRACE( _T("PortfolioOverviewDialog::OnOptions: Symbol found, but no associated Option def.\n") );
#endif
			continue;
		}
		if ( inst->hasPositions() )
		{		// add a node for the instrument
			InsertInstrument( (CBOS*)inst );
			CObArray*	posLst = inst->getPositionList();
			for ( short ii = 0; ii < posLst->GetSize(); ii++ )
			{		// add a node for each position
				CPosition*	pos = posLst->GetAt( ii );
				InsertPosition( pos );
			}
		}
		delete inst;		// kills the position list too
	}	
}			// OnOptions()
//----------------------------------------------------------------------------------------
void CPortfolioOverviewDialog::OnStocks()
{	long				anOffset;
	CString				symbol;
	CMapStringToPtr*	theMap = theApp.pDB->getSymbolTable( Stock );

		// for all symbols in theMap, look for defs with positions
	for ( POSITION pos = theMap->GetStartPosition(); pos != NULL; )
	{
		theMap->GetNextAssoc( pos, symbol, (void*&)anOffset );
		CStock*	inst = theApp.pDB->getStock( symbol );
		if ( inst == NULL )
		{
#ifdef _DEBUG
			TRACE( _T("PortfolioOverviewDialog::OnStocks: Symbol found, no associated Stock def.\n") );
#endif
			continue;
		}
		if ( inst->hasPositions() )
		{		// add a node for the instrument
			InsertInstrument( (CBOS*)inst );
			CObArray*	posLst = inst->getPositionList();
			for ( short ii = 0; ii < posLst->GetSize(); ii++ )
			{		// add a node for each position
				CPosition*	pos = posLst->GetAt( ii );
				InsertPosition( pos );
			}
		}
		delete inst;		// kills the position list too
	}
}			// OnStocks()
*/
//----------------------------------------------------------------------------------------
