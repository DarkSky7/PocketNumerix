// PositionsDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
//#include "BOSDatabase.h"
#include "PositionsDialog.h"
#include "ClosePositionDialog.h"
//#include "NillaHedgeDoc.h"
#include "NillaHedge.h"
#include "ClosedPosition.h"
#include "Position.h"
//#include "Bond.h"
#include "Option.h"
#include "Stock.h"

#include "DataManager.h"
//#include "BondManager.h"
#include "OptionManager.h"
#include "StockManager.h"
#include "RegistryManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern	CString months[];
extern	CString numberStrings[];
extern	CNillaHedgeApp	theApp;

/////////////////////////////////////////////////////////////////////////////
// CPositionsDialog dialog

IMPLEMENT_DYNAMIC( CPositionsDialog, CNillaDialog )

CPositionsDialog::CPositionsDialog( CWnd* pParent /*=NULL*/ )
	: CNillaDialog(CPositionsDialog::IDD, pParent)
	, m_activeComboBox( NULL )
	, instrument( UnknownAssetType )
	, theAsset( NULL )
{
	//{{AFX_DATA_INIT(CPositionsDialog)
	m_TotalCost = 0.0;
	m_Symbol = _T("");
	m_PurchaseDate = COleDateTime::GetCurrentTime();
	m_Note = _T("");
	m_Units = 0.0;
	//}}AFX_DATA_INIT
}

CPositionsDialog::~CPositionsDialog( void )
{
	delete	theAsset;		// theAsset deletes its position list
}

void CPositionsDialog::DoDataExchange( CDataExchange* pDX )
{
	CNillaDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPositionsDialog)
	DDX_Control(pDX, IDC_Modify, c_Modify);
	DDX_Control(pDX, IDC_Close, c_Close);
	DDX_Control(pDX, IDC_Delete, c_Delete);
//	DDX_Control(pDX, IDC_Bond, c_Bond);
	DDX_Control(pDX, IDC_Option, c_Option);
	DDX_Control(pDX, IDC_Stock, c_Stock);
	DDX_Control(pDX, IDC_Symbol, c_Symbol);
	DDX_Control(pDX, IDC_PurchaseDate, c_PurchaseDate);
	DDX_Control(pDX, IDC_Units, c_Units);
	DDX_Control(pDX, IDC_TotalCost, c_TotalCost);
	DDX_Control(pDX, IDC_Note, c_Note);
	DDX_Control(pDX, IDC_PositionsList, c_PositionsList);
	DDX_Control(pDX, IDC_Enter, c_Enter);
	DDX_Text(pDX, IDC_TotalCost, m_TotalCost);
	DDX_CBString(pDX, IDC_Symbol, m_Symbol);
	DDV_MaxChars(pDX, m_Symbol, 255);
	DDX_DateTimeCtrl(pDX, IDC_PurchaseDate, m_PurchaseDate);
	DDX_Text(pDX, IDC_Note, m_Note);
	DDV_MaxChars(pDX, m_Note, 255);
	DDX_Text(pDX, IDC_Units, m_Units);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( CPositionsDialog, CNillaDialog )
	//{{AFX_MSG_MAP(CPositionsDialog)
	ON_BN_CLICKED(IDC_Option, OnOption)
	ON_BN_CLICKED(IDC_Stock, OnStock)
	ON_NOTIFY(NM_KILLFOCUS, IDC_PurchaseDate, OnKillFocusPurchaseDate)
	ON_EN_KILLFOCUS(IDC_Units, OnKillFocusUnits)
	ON_EN_KILLFOCUS(IDC_TotalCost, OnKillFocusTotalCost)
	ON_EN_KILLFOCUS(IDC_Note, OnKillFocusNote)
	ON_EN_SETFOCUS(IDC_Units, OnSetFocusUnits)
	ON_EN_SETFOCUS(IDC_TotalCost, OnSetFocusTotalCost)
	ON_EN_SETFOCUS(IDC_Note, OnSetFocusNote)
	ON_BN_CLICKED(IDC_Enter, OnEnter)
	ON_BN_CLICKED(IDC_Delete, OnDelete)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_PositionsList, OnGetDispInfoPositionsList)
	ON_NOTIFY(NM_DBLCLK, IDC_PositionsList, OnDblClickPositionsList)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_PositionsList, OnItemChangingPositionsList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_PositionsList, OnColumnClickPositionsList)
	ON_BN_CLICKED(IDC_Close, OnClose)
	ON_BN_CLICKED(IDC_Modify, OnModify)
	ON_NOTIFY(NM_CLICK, IDC_PositionsList, OnClickPositionsList)
	ON_NOTIFY(NM_KILLFOCUS, IDC_PositionsList, OnKillFocusPositionsList)
	ON_NOTIFY(NM_SETFOCUS, IDC_PositionsList, OnSetFocusPositionsList)
	ON_CBN_SELCHANGE(IDC_Symbol, OnSelChangeSymbol)
	ON_COMMAND(ID_Delete_Definition, OnDeleteDefinition)
	ON_NOTIFY(DTN_CLOSEUP, IDC_PurchaseDate, OnCloseUpPurchaseDate)
	ON_NOTIFY(NM_SETFOCUS, IDC_PurchaseDate, OnSetFocusPurchaseDate)

	ON_CBN_KILLFOCUS(IDC_Symbol, OnKillFocusSymbol)				// HotKeys +
	ON_CBN_SETFOCUS(IDC_Symbol, OnSetFocusSymbol)				// HotKeys
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()											// HotKeys
	ON_MESSAGE(WM_HOTKEY, OnHotKey)								// HotKeys

	ON_MESSAGE( WM_MarqueeEnd, OnMarqueeEnd )
	ON_NOTIFY( NM_CUSTOMDRAW, IDC_PositionsList, OnCustomDrawPositionsList )
END_MESSAGE_MAP()

//----------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////
//									Message Handlers									//
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
BOOL	CPositionsDialog::OnInitDialog() 
{
	CNillaDialog::OnInitDialog();
	RegisterHotKeys( IDC_Symbol, IDC_Note );		// screens for WM5 internally
		// Default symbol table load -
		// Logic table, potential outcomes ...
		//		L - load table
		//		E - load empty table?
		//		C - make candidate
		//		X - eliminate as a candidate?
		//													regEntry
		//											|	0  bnd opt stk
		//									  ------+----------------------
		// bndTab:	  doesn't exist 			0	|	C	C
		//					  exists			1	|	C	L
		// optTab:	  doesn't exist 			0	|	C		C
		//					  exists			1	|	C		L
		// stkTab:	  doesn't exist 			0	|	C			C
		//					  exists			1	|	C			L
		// if a regEntry is present, but no tables are present
		// then we'll load the reg specified empty table
		// if some tables are present, but not the reg specified entry, 
		// load one of the existing tables
	if ( ! GetRecentInstrument(&instrument) )						// was:  theApp.regMgr->
	{	
#ifdef _DEBUG
		TRACE( _T("PositionsDialog::OnInitDialog: Failed to find an initial instrument.\n") );
#endif
	}

	if (	   instrument == UnknownAssetType						// no most recent instrument registered
		 ||  theApp.GetSymbolTableSize(instrument) <= 0 )			// no associated symbol table
	{		// no initial instrument registered
		int	nDefs[3];
//		nDefs[0] = ( theApp.bndMgr != NULL )  ?  theApp.bndMgr->GetSymbolTableSize()  :  0;
		nDefs[1] = ( theApp.optMgr != NULL )  ?  theApp.optMgr->GetSymbolTableSize()  :  0;
		nDefs[2] = ( theApp.stkMgr != NULL )  ?  theApp.stkMgr->GetSymbolTableSize()  :  0;

			// could also decide based on nunmber of Indexed Issues (issues with positions)
//		int	nIndexedIssues[3];
//		nIndexedIssues[0] = theApp.bndMgr->numIndexedIssues( Bond   );
//		nIndexedIssues[1] = theApp.optMgr->numIndexedIssues( Option );
//		nIndexedIssues[2] = theApp.stkMgr->numIndexedIssues( Stock  );

		int	maxDefs = 0;
		int	bestInst = -1;
		for ( int ii = 1; ii < 2; ii++ )
		{	if ( nDefs[ii] > maxDefs )
			{	maxDefs = nDefs[ii];
				bestInst = ii;
			}
		}
		if ( bestInst >= 0 )
			instrument = (bestInst == 1)  ?  Option  :  Stock;
	}
	if ( instrument == UnknownAssetType )
		c_Symbol.EnableWindow( FALSE );			// disable the Symbol ComboBox
	else
	{	LoadSymbols();

			// we have a preferred instrument and its symbol table has been loaded
			// see if we can select a symbol and set the Asset too
		bool	res;
		long	def_ID;
		switch ( instrument )
		{
//			case Bond:
//				CheckRadioButton( IDC_Bond, IDC_Stock, IDC_Bond );
//					// see if there's a recent bond registered
//				res = theApp.regMgr->GetRecentBond( def_ID );
//				if ( res  &&  def_ID >= 0 )
//					theAsset = (CBOS*)theApp.bndMgr->ReadBond( def_ID );
//				break;
			case Option:
				CheckRadioButton( IDC_Option, IDC_Stock, IDC_Option );
					// see if there's a recent option registered
				res = GetRecentOption(&def_ID);					// was:  theApp.regMgr->
				if ( res  &&  def_ID >= 0 )
					theAsset = (CBOS*)theApp.optMgr->ReadOption( def_ID );
				break;
			case Stock:
				CheckRadioButton( IDC_Option, IDC_Stock, IDC_Stock );
					// see if there's a recent stock registered
				res = GetRecentStock(&def_ID);					// was:  theApp.regMgr->
				if ( res  &&  def_ID >= 0 )
					theAsset = (CBOS*)theApp.stkMgr->ReadStock( def_ID );
				break;
			default:
				ASSERT( false );
				break;
		}
		if ( theAsset )
		{	m_Symbol = theAsset->getSymbol();
			int	indx = c_Symbol.FindStringExact( 0, m_Symbol );
			if ( indx != CB_ERR )
				c_Symbol.SetCurSel( indx );
			CString sym = m_Symbol;
			m_Symbol = _T("");				// make it look like m_Symbol has changed
			killFocusSymbolWork( sym );
		}
	}

		// set the state of the Close, Delete, Enter, & Modify buttons
	EnableEnterButton();
	EnablePositionButtons();

	COleDateTime	minDate( 1900, 1, 1, 0, 0, 0);
	COleDateTime	maxDate = COleDateTime::GetCurrentTime();
	c_PurchaseDate.SetRange( &minDate, &maxDate );

		// loadSymbols() sets the focus to IDC_Symbol
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::LoadSymbols( void ) 
{
	if ( instrument == UnknownAssetType )
		return;
	
		// unload any existing entries in c_Symbol
	c_Symbol.Clear();

		// load the symbols associated with the current instrument type
	c_Symbol.LoadMap( theApp.GetSymbolTable( instrument ) );

		// initialize the position list
	InitListCtrl();

	c_Symbol.EnableWindow( true );
	c_Symbol.SetFocus();
}			// LoadSymbols()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::killFocusSymbolWork( CString sym )
{		// clean up the heap
	if ( sym == m_Symbol )
		return;
	m_Symbol = sym;

	if ( theAsset  &&  theAsset->getSymbol() != m_Symbol )
	{	delete	theAsset;
		theAsset = NULL;
	}

		// Reset accumulators
	aggUnits = 0.0;
	aggCost = 0.0;
	aggIncome = 0.0;
	wtdYield = 0.0;

		// clear out the PositionsList
	c_PositionsList.DeleteAllItems();	// do this only if theAsset->getSymbol() != m_Symbol
		// if we know of an instrument with m_Symbol,
		// go get it and its positions, but don't
		// create an asset if it doesn't already exist
	CMapPtrToPtr*	posLst = GetPositionList();					// no create mode
	if ( posLst == NULL )
		return;

		// for options, display a string in c_Note, consisting of
		// Stock symbol, expiration date, type of option { Put, Call }, '@', strike price
	AssetType at = theAsset->getAssetType();
	if ( at == Option )
	{	wchar_t	wbuf[256];
		COption* opt = ((COption*)theAsset);
		CStock* stk = opt->getUnderlying();
		COleDateTime	exp = opt->getExpiry();
		CString 	dtStr = MonYrFormat( exp );
		swprintf( wbuf, _T("%s %s %s@%g for %.2f"), stk->getSymbol(), dtStr,
			(opt->getPutCall() == Put  ?  _T("Put")  :  _T("Call") ),
			opt->strikePrice, opt->getMktPrice() );
		c_Note.SetWindowTextW( wbuf );
	}
	
		// Load up the positions for this instrument
	int		nItems = posLst->GetCount();
		// preallocate the ListCtrl memory with a little extra space
	c_PositionsList.SetItemCount( nItems + 3 );

		// load the positions
	CPosition*	pos;
	long		posOffset = theAsset->openPosOffset;
	while ( posLst->Lookup( (void*)posOffset, (void*&)pos ) )
	{	int	res = LoadOnePosition( pos );						// trouble if res < 0
#ifdef _DEBUG
	if ( res < 0 )
			TRACE( _T("PositionsDialog::OnKillFocusSymbol: LoadOnePosition returned %d\n"),
				res );
#endif

			// prepare for next loop pass
		posOffset = pos->prevPos_ID;
	}
	LoadColumnTotals();

		// prepare the purchase date
	m_PurchaseDate = COleDateTime::GetCurrentTime();
	c_PurchaseDate.SetTime( m_PurchaseDate );
}			// killFocusSymbolWork()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::OnKillFocusSymbol( void )
{	CString sym;
	m_activeComboBox = NULL;
//	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	c_Symbol.GetWindowText( sym );
	sym.MakeUpper();
	killFocusSymbolWork( sym );
}			// OnKillFocusSymbol()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::OnSetFocusSymbol( void )
{
	m_activeComboBox = IDC_Symbol;
//	SHSipPreference( GetSafeHwnd(), SIP_UP );			// quicker to pick from a list
}			// OnSetFocusSymbol()
//----------------------------------------------------------------------------
void CPositionsDialog::OnSelChangeSymbol( void )
{	CString			sym;
	int cnt = c_Symbol.GetCount();
	if ( cnt < 1 )
		return;
	int	sel = c_Symbol.GetCurSel();
#if ( _WIN32_WCE >= 0x420 )					// just WM5
	if ( c_Symbol.lastChar == VK_DOWN  ||  c_Symbol.lastChar == VK_UP )
	{	int	delta = ( c_Symbol.lastChar == VK_DOWN  ?  1  :  -1 );
		sel += delta;
		if ( sel < 0 )
			sel = cnt - 1;
		if ( sel >= cnt )
			sel = 0;
		c_Symbol.SetCurSel( sel );
	}
#endif
	if ( sel < 0 ) sel = 0;							// should be superfluous
	c_Symbol.GetLBText( sel, sym );
	killFocusSymbolWork( sym );
}			// OnSelChangeSymbol()
//----------------------------------------------------------------------------------------
LRESULT		CPositionsDialog::OnHotKey( WPARAM wParam, LPARAM lParam )
{		//	UINT fuModifiers = (UINT)LOWORD(lParam);		// don't care about modifiers
		//	DWORD ctrlID = (int)wParam;						// don't care which control the HotKey is registered to (any more)
		//	CWnd* wnd = GetDlgItem( ctrlID );				// also about the registered control
	if ( ! m_activeComboBox )
		return	0;											// nothing to do

	UINT uVirtKey = (UINT)HIWORD(lParam);
	if ( uVirtKey != VK_DOWN  &&  uVirtKey != VK_UP )
		return	0;											// only care about the up/down arrow keys

	CSeekComboBox* scb = (CSeekComboBox*)GetDlgItem( m_activeComboBox );
	scb->lastChar = uVirtKey;										// this in combination with SendMessage()
	WPARAM wp = MAKEWPARAM( m_activeComboBox, CBN_SELCHANGE );
	SendMessage( WM_COMMAND, wp, (LPARAM)scb->GetSafeHwnd() );		// emulates VK_UP/VK_DOWN as on PPC'02
	return	0;
}			// OnHotKey()
//----------------------------------------------------------------------------
void	CPositionsDialog::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	CNillaDialog::OnActivate( nState, pWndOther, bMinimized );
	if ( nState == WA_ACTIVE || nState == WA_CLICKACTIVE )
		RegisterHotKeys( IDC_Symbol, IDC_Note );	// screens for WM5 internally
}			// OnActivate()
//----------------------------------------------------------------------------
CMapPtrToPtr*	CPositionsDialog::GetPositionList( bool create )
{		// sets theAsset as a side effect
		// called by OnKillFocusSymbol(), so we can't assume that theAsset exists
		// also called by:  LoadColumnTotals() & DeleteOrModifyPosition() in non-create mode
		// caller OnEnter() uses create mode
	if ( theAsset != NULL )
		return	theAsset->getPositionList();

		// if theAsset or itsPositions already exist, prevent memory leaks by deleting them first
	ASSERT( instrument != UnknownAssetType );
	switch ( instrument )
	{
//		case Bond:
//		{	CBond* aBond = theApp.bndMgr->GetBond( m_Symbol );
//			if ( aBond == NULL )
//			{	if ( !create )
//					return NULL;
//					// didn't find m_Symbol in the bonds map, so this is a new entry
//				aBond = new CBond( m_Symbol );
//				ASSERT_VALID( aBond );
//				short	res = theApp.bndMgr->WriteBond( aBond );
//				if ( res != 0 )
//				{
//#ifdef _DEBUG
//					TRACE( _T("PositionsDialog::GetPositionList: Bond write failed, res = %d\n"), res );
//#endif
//					delete	aBond;
//					return	NULL;
//				}
//			}
//			theAsset = (CBOS*)aBond;
//			break;
//		}
		case Option:
		{	COption* anOption = theApp.optMgr->GetOption( m_Symbol );
			if ( anOption == NULL )
			{	if ( !create )
					return NULL;
					// didn't find m_Symbol in the options map, so this is a new entry
				anOption = new COption( m_Symbol );
				ASSERT_VALID( anOption );
				short	res = theApp.optMgr->WriteOption( anOption );
				if ( res != 0 )
				{
#ifdef _DEBUG
					TRACE( _T("PositionsDialog::GetPositionList: Option write failed, res = %d\n"), res );
#endif
					delete	anOption;
					return	NULL;
				}
			}
			theAsset = (CBOS*)anOption;
			break;
		}
		case Stock:
		{	CStock* aStock = theApp.stkMgr->GetStock( m_Symbol );
			if ( aStock == NULL )
			{	if ( !create )
					return NULL;
					// didn't find m_Symbol in the stocks map, so this is a new entry
				aStock = new CStock( m_Symbol );
				ASSERT_VALID( aStock );
				short	res = theApp.stkMgr->WriteStock( aStock );
				if ( res != 0 )
				{
#ifdef _DEBUG
					TRACE( _T("PositionsDialog::GetPositionList: Stock write failed, res = %d\n"), res );
#endif
					delete	aStock;
					return	NULL;
				}
			}
			theAsset = (CBOS*)aStock;
			break;
		}
		default:
			ASSERT( false );
			break;
	}
	return	theAsset->getPositionList();
}			// GetPositionList()
//----------------------------------------------------------------------------------------
CPosition*	CPositionsDialog::AddPosition( CPosition* pos /* = NULL */ )
{		// the following gate is redundant with OnEnter(), but
		// it relieves CMainFrame::OnCreatePosition of knowing the criteria
	if ( m_Units == 0.00  ||  m_TotalCost == 0.0  ||  m_Symbol == _T("") )
		return	NULL;
/*
		// gating requirement:  m_Symbol is non-empty and m_Units > 0
	int		sLen = m_Symbol.GetLength();
	wchar_t*	buf = m_Symbol.GetBuffer( sLen );
	bool noSym = ( *buf == '\0' );
	if ( noSym  ||  m_Units <= 0.00 )
		return	NULL;
*/
		// find m_Symbol in the doc's list of instruments and get its positions
		// if necessary, create a Option, or Stock, so we can attach the position to it
	CMapPtrToPtr*	posLst = GetPositionList( true );		// creates theAsset if it doesn't already exist
	ASSERT( theAsset != NULL );

		// now we can add the new position using attributes
		// of the position from the InitialPositionDialog
	if ( pos == NULL )
	{
		pos    = new CPosition;
		pos->setNote( m_Note );
		pos->nUnits       = m_Units;
		pos->purchaseDate = m_PurchaseDate;
		pos->totalCost    = m_TotalCost;
		pos->inst		  = instrument;
		pos->def_ID		  = theAsset->getDef_ID();
	}
		// the assumption is that this is a brand new position
	ASSERT( pos->pos_ID < 0 );

		// theAsset manages the position list
	theAsset->addPosition( pos );
	return	pos;
}			// AddPosition()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::OnEnter()
{		// the following buffer operation on m_Symbol would seem equivalent to
		// m_Symbol.IsEmpty(), but that returns true after a switch from
		// Stock to Option, even when m_Symbol is non-empty as evidenced by
		// the value displayed in the debugger and the dialog's Edit box
		// however, direct examination of the buffer works...
		// it seems as though we stepped on the IsEmpty() code
		// if we use a shorter length, the symbol passed onto AddPosition()
		// will be that length
	if ( m_Units == 0.00  ||  m_TotalCost == 0.0  ||  m_Symbol == _T("") )
		return;
/*
		// gating requirement:  m_Symbol is non-empty and m_Units > 0
	int		sLen = m_Symbol.GetLength();
	wchar_t*	buf = m_Symbol.GetBuffer( sLen );
	bool noSym = ( *buf == '\0' );
	if ( noSym  ||  m_Units <= 0.00 )
		return;
*/
		// an informative gate
	if ( m_Units * m_TotalCost < 0.0 )								// an XOR construct 
	{	UINT result = MessageBox(
			_T("Units and Cost must have the same sign. Positive values for a long position; negative values for a short position."),
			 _T("Invalid Data"),
			 MB_ICONINFORMATION | MB_OK | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST );
		return;
	}

		// Okay, now we do much of the same stuff we'd do if the user closed and reopened the dialog

		// before changing the number of positions in the list...
		// If there's a total row, delete it now
	c_PositionsList.DeleteTotalRow();

		// handle in-memory & on-disk position list modifications
	CPosition*	pos = AddPosition();
	if ( pos != NULL )
	{		// tell the the ListView about the new position
		int res = LoadOnePosition( pos );
	}
		// Replace the total row (if warranted)
	LoadColumnTotals();

		// signify no position created by leaving CEdit boxes intact
		// and not changing the contents of the list view
		// tell the the ListView about the new position
	if ( pos == NULL )
		return;

		//////////////////////////////////////////////////////////////
		// Blank out the Units, TotalCost & Note EditBoxes & assoc. variables
	m_Units = 0.0;
	setEditBox( "%.1f", m_Units, IDC_Units );
	
	m_TotalCost = 0.0;
	setEditBox( "%.2f", m_TotalCost, IDC_TotalCost );

	m_Note = _T("");
	c_Note.SetWindowText( m_Note );

	EnableEnterButton();

		// set the focus on the Symbol
	c_PurchaseDate.SetFocus();
}			// OnEnter()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::OnClose() 
{
	UINT	nSel = c_PositionsList.GetSelectedCount();
	ASSERT( nSel == 1 );

		// fill in default closing position parameters and get confirmation
	CClosePositionDialog	dlg;

		// get nUnits from the CPosition
	int		iItem = c_PositionsList.GetSelectionMark();
	DWORD	pItem = c_PositionsList.GetItemData( iItem );
	CPosition*	pos = ((BOSPositionListItemInfo*)pItem)->thePosition;
	ASSERT_VALID( theAsset );
	dlg.m_NetProceeds = theAsset->getMktPrice() * pos->nUnits;
	dlg.m_MinDate = pos->purchaseDate;			// can't sell before the purchase date

		// We're Closing a position - now is the time to get an OK or CANCEL
	int nDisposition = dlg.DoModal();
	if ( nDisposition != IDOK )
	{		// the user has cancelled the Close request
			// restore the appearance of the GUI before the call
		c_PositionsList.SetFocus();
		return;					// user changed his mind, don't delete pos
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// user gave the OK to Close the selected position
	c_PositionsList.DeleteTotalRow();
		// make a ClosedPosition before calling DeleteOrModifyPosition where pos is deleted
	CClosedPosition		cPos( *pos );
		// pull pos out of the CList and the positions list, delete pos, update the UI
	DeleteOrModifyPosition( iItem, ClosedPosition );
	LoadColumnTotals();							// put the total row back

		// delete the CPosition, and write a derivative CClosedPosition to pDB
	cPos.closingNote = dlg.m_ClosingNote;
	cPos.dateClosed = dlg.m_ClosingDate;
	cPos.netProceeds = dlg.m_NetProceeds;
	CDataManager*	dm = cPos.getDataManager();
	short	res = dm->WriteClosedPosition( &cPos );
#ifdef _DEBUG
	if ( res != 0 )
		TRACE( _T("CPositionsDialog::OnClose: Error writing the closed position, res=%d\n"),
			res );
#endif
		// now that we've handled the message, we can disable the button
	c_Close.EnableWindow( false );
}			// OnClose()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::OnDelete()
{
	unsigned short nSelected = c_PositionsList.GetSelectedCount();
	ASSERT ( nSelected >= 1 );

	bool	verify = true;
	short	prefs = PositionsDelete;					// i.e. affirmative
	if ( GetSaveVerifyPrefs(&prefs) )					// was:  theApp.regMgr->
	{	verify = (prefs & PositionsDelete) != 0;
	}
	if ( verify )
	{	CString cst( "You are about to permanently delete ");
		if ( nSelected == 1 )
			cst += _T("the");
		else if ( nSelected < 13 )
			cst += numberStrings[nSelected];
		else
		{	wchar_t	buf[8];
			swprintf( buf, _T("%d"), nSelected );
			cst += buf;
		}
		cst += _T(" selected position");
		if ( nSelected > 1 )
			cst += _T("s");
		cst += _T(".  Okay to proceed?");
		UINT result = MessageBox( cst, _T("Warning!"),
						MB_ICONWARNING | MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST );
		if ( result != IDYES )
		{		// the user has cancelled the Delete request
				// restore the appearance of the GUI before the call
			c_PositionsList.SetFocus();
			return;
		}
	}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// user gave the OK to Delete the selected positions
		// we're preventing selection of the total row (in OnClickPositionsList())
		// which should mean that the selections are actually positions (not a total row)
		// so the selected count being >= 1 can't include the total row
	c_PositionsList.DeleteTotalRow();

		// create our own list of the selected items, so we can traverse it backwards
	int*	selItems = new int[nSelected];
	int	ii = 0;
	POSITION itemPos = c_PositionsList.GetFirstSelectedItemPosition();
	while ( itemPos )
		*(selItems+ii++) = c_PositionsList.GetNextSelectedItem( itemPos );

		// now traverse our copy of the selected list backwards, deleting positions along the way
	ii = nSelected - 1;												// location of the last entry
	while ( ii >= 0 )
	{		// this is why this function lives in the dialog!
		DeleteOrModifyPosition( *(selItems+ii--), DeletedPosition );	// { Closed, Deleted }
	}
	delete [] selItems;												// we're done with these

	LoadColumnTotals();												// put the total row back

		// we handled the message (successfully)
		// therefore, the selected list has been obliterated, so we can disable the Delete button
	c_Delete.EnableWindow( false );
}			// OnDelete()
//----------------------------------------------------------------------------------------
void CPositionsDialog::OnModify() 
{
	UINT	nSel = c_PositionsList.GetSelectedCount();
	ASSERT( nSel == 1 );

	int	iItem = c_PositionsList.GetSelectionMark();
	bool	verify = true;
	short	prefs = PositionsModify;							// i.e. affirmative
	if ( GetSaveVerifyPrefs(&prefs) )							// was:  theApp.regMgr->
	{	verify = (prefs & PositionsModify) != 0;
	}
	if ( verify )
	{
		CString cst = _T("You are about to permanently delete the selected position");
		cst		   += _T(" and overwrite current dialog values.  Okay to proceed?");
		UINT result = MessageBox( cst, _T("Warning!"),
						MB_ICONWARNING | MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST );
		if ( result != IDYES )
		{		// the user has cancelled the Modify request
			// restore the appearance of the GUI before the call
		c_PositionsList.SetFocus();
			return;
		}
	}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// user gave the OK to Modify the selected position
	c_PositionsList.DeleteTotalRow();
		// we're preventing selection of the total row (in OnClickPositionsList())
		// which should mean that the selections are actually positions (not a total row)
		// so the selected count being >= 1 can't include the total row
	DeleteOrModifyPosition( iItem, OpenPosition );		// Open -> modify

	LoadColumnTotals();							// put the total row back
		// now that we've handled the message, we can disable the button
	c_Modify.EnableWindow( false );
}			// OnModify()
//----------------------------------------------------------------------------------------
/*
void	CPositionsDialog::OnBond()
{
	if ( instrument == Bond )
		return;
	instrument = Bond;
	CheckRadioButton( IDC_Bond, IDC_Stock, IDC_Bond );
	loadSymbols();
}			// OnBond()
*/
//----------------------------------------------------------------------------------------
void	CPositionsDialog::OnOption()
{
	if ( instrument == Option )
		return;
	instrument = Option;
	CheckRadioButton( IDC_Option, IDC_Stock, IDC_Option );
	LoadSymbols();
}			// OnOption()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::OnStock()
{
	if ( instrument == Stock )
		return;
	instrument = Stock;
	CheckRadioButton( IDC_Option, IDC_Stock, IDC_Stock );
	LoadSymbols();
}			// OnStock()
//----------------------------------------------------------------------------------------
void CPositionsDialog::killFocusPurchaseDateWork( NMHDR* pNMHDR, LRESULT* pResult )
{
	COleDateTime	odt;
	c_PurchaseDate.GetTime( odt );
	if ( odt == m_PurchaseDate )
		return;
	m_PurchaseDate = odt;
	
	c_Units.SetSel( 0, -1 );
//	c_Units.SetFocus();
	
	*pResult = 0;
}			// killFocusPurchaseDateWork()
//----------------------------------------------------------------------------------------
void CPositionsDialog::OnCloseUpPurchaseDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	killFocusPurchaseDateWork( pNMHDR, pResult );
}			// OnCloseUpPurchaseDate()
//--------------------------------------------------------------------------------------
void CPositionsDialog::OnKillFocusPurchaseDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	killFocusPurchaseDateWork( pNMHDR, pResult );
}			// OnKillFocusPurchaseDate()
//----------------------------------------------------------------------------------------
void CPositionsDialog::OnSetFocusPurchaseDate(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	*pResult = 0;
}			// OnSetFocusPurchaseDate()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::OnKillFocusUnits()
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	m_Units = getEditBoxDouble( IDC_Units );

	ShowTotalCost();

	EnableEnterButton();

	c_TotalCost.SetSel( 0, -1 );
//	c_TotalCost.SetFocus();	
}			// OnKillFocusUnits()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::OnSetFocusUnits( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusUnits()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::ShowTotalCost( void )
{
	if ( theAsset == NULL )
		return;

	m_TotalCost = m_Units * theAsset->getMktPrice();
	setEditBox( "%.2f", m_TotalCost, IDC_TotalCost );
}			// ShowTotalCost()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::OnKillFocusTotalCost()
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	double	dd = getEditBoxDouble( IDC_TotalCost );
	if ( m_TotalCost == dd )
		return;

	m_TotalCost = dd;
	setEditBox( "%.2f", dd, IDC_TotalCost );	

	EnableEnterButton();

	c_Note.SetSel( 0, -1 );
//	c_Note.SetFocus();	
}			// OnKillFocusTotalCost()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::OnSetFocusTotalCost( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusTotalCost()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::EnableEnterButton( void )
{
	bool	enable = m_Symbol != _T("")  &&  m_TotalCost != 0.0  &&  m_Units != 0.0;
	c_Enter.EnableWindow( enable );
}			// EnableEnterButton()
//----------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////
//							PositionList Messages/Methods								//
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
void	CPositionsDialog::OnGetDispInfoPositionsList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO*	pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	if ( pDispInfo->item.mask  &  LVIF_TEXT )
	{		// this function has to be here, because only the dialog knows whether
			// this ia a Option or Stock Positions Dialog
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
//----------------------------------------------------------------------------------------
CString		CPositionsDialog::GetPositionItemText( int rr, int si )
{		// return a CString representation of the data in row rr, subitem si
		// get the sortCol for this subitem
	short	msi = MapSubitem( si );
	unsigned short	lastCol = ( instrument == Option )  ?  7  :  8;			// zero based
	if ( msi < 0  ||  msi > lastCol )		// not including the dummy column
		return _T("");

		// get the item info for the row of interest (rr)
	BOSPositionListItemInfo*	pItem = (BOSPositionListItemInfo*)c_PositionsList.GetItemData( rr );
	ASSERT ( pItem != NULL  &&  theAsset  &&  instrument != UnknownAssetType );

		// get the CString for the requested subitem (si)
	wchar_t	buf[20];				// where we construct the return string
	while ( 1 )
	{
		if ( msi == 0  ||  msi == lastCol )
		{	CPosition*	pos = pItem->thePosition;
			if ( pos == NULL )
				return	_T("Total");			// it's the Total row
				//							msi == 0					 msi == lastCol
			return	( msi == 0 )  ?  EuroFormat( pos->purchaseDate )  :  pos->getNote();
		}
		if ( msi == 1  ||  msi == 2 )
		{	CPosition*	pos = pItem->thePosition;
			if ( msi == 1 )
			{		// Its the # of Units (shares/options) in the Position or total row
				swprintf( buf, _T("%g"), pos ? pos->nUnits : aggUnits );
				break;
			}
			else														// msi == 2
			{		// Its the cost of the Position or total row
				swprintf( buf, _T("%.2f"), pos ? pos->totalCost : aggCost );
				break;
			}
		}
		if ( msi == 3 )
		{		// Its the market value of the Position
			swprintf( buf, _T("%.2f"), pItem->mktValue );
			break;
		}
			// capital gain is 4th (zero based)
		if ( msi == 4 )
		{
			swprintf( buf, _T("%.2f"), pItem->capGain );
			break;
		}

			// income is 5th for Stocks, exerVal is 5th (zero based) for Options
		bool isOption = ( instrument == Option );
		if (  (isOption  &&  msi == 6)  ||  (!isOption  &&  msi == 5) )
		{		// dividend or coupon income (zero based 5th for Stocks)
				//			  exercise value (zero based 6th for Options)
			swprintf( buf, _T("%.2f"), pItem->income );
			break;
		}

		if ( !isOption  &&  msi == 6 )
		{		// NetGain
			double	netGain = pItem->capGain + pItem->income;
			swprintf( buf, _T("%.2f"), netGain );
			break;
		}

			// the only thing left is pctYield
			// pctYield is 5th for Options, but 7th for Stocks
		double	plusYld = fabs(pItem->pctYield);
		swprintf( buf,  ((plusYld <   10.0) ? _T("%.3f")
					  : ((plusYld <  100.0) ? _T("%.2f")
					  : ((plusYld < 1000.0) ? _T("%.1f")
					  :						  _T("%.0f") ))), pItem->pctYield );
		break;
	}
	CString	cst( buf );
	return	cst;
}			// GetPositionItemText()
//----------------------------------------------------------------------------------------
void CPositionsDialog::OnItemChangingPositionsList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	NM_LISTVIEW* pNMLV = (NM_LISTVIEW*)pNMHDR;

	int  itemCnt = c_PositionsList.GetItemCount();
	if ( itemCnt > 2  &&  pNMLV->iItem == itemCnt-1  &&  pNMLV->uChanged == 8 )
	{		// uChanged == 8 --> the totalRow is being selected
		UINT	newState = pNMLV->uNewState;
			// prevent selection of the total row by returning TRUE
		*pResult = MAKELRESULT( TRUE, TRUE );			// ( lowWord, highWord )
	}
}			// OnItemChangingPositionsList()
//----------------------------------------------------------------------------------------
void CPositionsDialog::OnColumnClickPositionsList(NMHDR* pNMHDR, LRESULT* pResult) 
{		// this is a column sort event
	if ( ! theAsset )
		return;

		// if there is a total row, delete it before sorting
	c_PositionsList.DeleteTotalRow();

		// set up the SortInfo struct
	SortInfo	sortInfo;
	sortInfo.aBOS = (CBOS*)theAsset;

		// create a 1-based column index (sortKey)
	NMLISTVIEW*	pNMLV = (NMLISTVIEW*)pNMHDR;
	short sortKey = MapSubitem( pNMLV->iSubItem ) + 1;		// the +1 is so we can reverse direction of column 0
/*
		// tell the header what's going on so it can paint direction arrows
	CBOSListCtrl::CSortHeaderCtrl* pHdr = (CBOSListCtrl::CSortHeaderCtrl*)c_PositionsList.GetHeaderCtrl();
	if ( sortKey == pHdr->GetSortedColumn() )
		pHdr->ComplementAscending();
	pHdr->SetSortedColumn( sortKey );
*/
		// tell the sort function what's going on too
	sortInfo.sortKey = ( sortKey == abs(lastSortKey) )
					 ? -lastSortKey : sortKey;

		// sort items according to the column header button clicked
	c_PositionsList.SortItems( BOSPositionCompareFunc, (LPARAM)(&sortInfo) );
	lastSortKey = sortInfo.sortKey;		// remember for next time

		// re-insert the "Total" row
	LoadColumnTotals();

		// show which column was sorted, ascending or descending

		// indicate completion
	*pResult = 0;
}			// OnColumnClickPositionsList()
//----------------------------------------------------------------------------------------
void CPositionsDialog::OnDblClickPositionsList( NMHDR* pNMHDR, LRESULT* pResult ) 
{		// if the selected count in the positions list == 1, do OnModify() ...
	if ( c_PositionsList.GetSelectedCount() == 1 )
	{		// true --> modify (is destructive - if user doesn't re-enter, its gone)
		OnModify();
	}
	*pResult = 0;
}			// OnDblClickPositionsList()
//----------------------------------------------------------------------------------------
// load a position into the dialog fields
void	CPositionsDialog::EditPosition( CPosition* pos )
{
	m_Units = pos->nUnits;
	setEditBox( "%g", m_Units, IDC_Units );

	m_TotalCost = pos->totalCost;
	setEditBox( "%.2f", m_TotalCost, IDC_TotalCost );

	m_Note = pos->getNote();
	c_Note.SetWindowText( m_Note );

	m_PurchaseDate = pos->purchaseDate;
	c_PurchaseDate.SetTime( m_PurchaseDate );
	c_PurchaseDate.SetFocus();

	EnableEnterButton();
}			// EditPosition()
//----------------------------------------------------------------------------------------
// delete the given item from the ListCtrl and from the Stock/Option's Positions list
void	CPositionsDialog::DeleteOrModifyPosition( int iItem, PositionStatus stat )
{
		// the first element of the itemInfo struct is a CPosition*
	BOSPositionListItemInfo*	pItem = (BOSPositionListItemInfo*)c_PositionsList.GetItemData( iItem );
	CPosition*		pos = pItem->thePosition;
	ASSERT( pos != NULL );

		// we're going to delete this position from the list control
		// remove its influence on the total row...

		// update accumulators, so we can create a new total row later
	aggUnits  -= pos->nUnits;
	aggCost	  -= pos->totalCost;
	aggIncome -= pItem->income;
	wtdYield  -= pItem->pctYield * pos->totalCost;

		// remove references to this position in the Positions list and the ListCtrl
	c_PositionsList.DeleteItem( iItem );

		// load the controls with values from this position
	if ( stat == OpenPosition )
		EditPosition( pos );

		// mark the CPosition as Closed or Deleted in the database...
		// Open imputes deletion, but
		// the CPosition gets reloaded in to the dialog's CEdit boxes
	ASSERT( stat != UnknownPositionStatus );

		// delete the position from the positions list and the database (release memory too)
	short	res = theAsset->deletePosition( pos );
#ifdef _DEBUG
	if ( res != 0 )
		TRACE( _T("PositionsDialog::DeleteOrModifyPosition: Error deleting the Position, res=%d.\n"),
			res );
#endif

		// the following cleans up residual floating point errors
	if ( c_PositionsList.GetItemCount() == 0 )
	{	aggUnits = 0.0;
		aggCost = 0.0;
		aggIncome = 0.0;
		wtdYield = 0.0;
	}
}			// DeleteOrModifyPosition()
//----------------------------------------------------------------------------------------
int		CPositionsDialog::LoadOnePosition( CPosition* pos )
{		// yrs affects all asset types in the pctYield
	ASSERT( pos != NULL );
	COleDateTime	today = COleDateTime::GetCurrentTime();
//	COleDateTimeSpan	timeSpan = today - pos->purchaseDate;
//	double	yrs = timeSpan.GetTotalDays() / 365.0;			// XXX need something more accurate?
	long	spDays;
	double	yrs = calcYears( pos->purchaseDate, today, spDays );

	double	income = 0.0;
	double	exerVal = 0.0;
	if ( instrument == Stock )
		income = pos->nUnits * ((CStock*)theAsset)->PresentValueDivs( pos->purchaseDate, today, 0.0 );
	else			// it's an Option
	{	if ( ((COption*)theAsset)->ExerVal( exerVal ) )
			exerVal *= pos->nUnits;							// modified param
	}


		// the following is needed for a non-virtual ListCtrl
	BOSPositionListItemInfo*	pItem = new BOSPositionListItemInfo;
	pItem->thePosition	= pos;

		// will also have to modify LoadColumnTotals, the sort routine
		// and GetPositionItemText()
	pItem->mktValue		= theAsset->getMktPrice() * pos->nUnits;		// new 3
	pItem->income = ( instrument == Option )  ?  exerVal  :  income;
	pItem->capGain		= pItem->mktValue - pos->totalCost;				// new 4

	double	topLine = pItem->mktValue + income;
	double	rateArg = topLine / pos->totalCost;
	bool	skipCalc = ( rateArg == 0.0  ||  yrs < 1e-4 );
	double	pctYld = skipCalc  ?  0.0  :  ( 100.0 * log(fabs(rateArg)) / yrs );
#ifdef _DEBUG
	double	netGain = topLine - pos->totalCost;
	TRACE( _T("PositionsDialog::LoadOnePosition: topLine=%f, totCost=%f, netGain=%f, rateArg=%f, pctYld=%f\n"),
				topLine, pos->totalCost, netGain, rateArg, pctYld );
#endif
	pItem->pctYield = pctYld;


		// update accumulators
	aggUnits  += pos->nUnits;
	aggCost	  += pos->totalCost;
	aggIncome += income;							// already scaled by nUnits
	wtdYield  += pctYld * pos->totalCost;

		// insert at the top of the positions list
	return	c_PositionsList.AddItem( (LPARAM)pItem, 0 );
}			// LoadOnePosition()
//----------------------------------------------------------------------------------------
int		CPositionsDialog::LoadColumnTotals( void )
{
	CMapPtrToPtr*	posLst = GetPositionList();
	ASSERT( posLst != NULL );

		// purchaseDate must be enabled and there must be more than one position
	long	areVisible = 0xFFFF;
	if ( ! GetPosListColVisibility(&areVisible) )				// was:  theApp.regMgr->
	{
#ifdef _DEBUG
		TRACE( _T("PositionsDialog::LoadColumnTotals: no column visibility preferences found.\n") );
#endif
	}

	int	nItems = posLst->GetCount();
//	bool	purchDateIsVisible = (areVisible & PurchaseDate) != 0;
//	bool showTotalRow = purchDateIsVisible  &&  (nItems > 1);
	bool showTotalRow = (nItems > 1);
	if ( !showTotalRow )
		return nItems;

		// set up an LVITEM and tell the list control about it
	BOSPositionListItemInfo*	pItem = new BOSPositionListItemInfo;
	pItem->thePosition = 0;							// signal that this is the total row

//	pItem->nUnits		= ( fabs(aggUnits) > 0.01 )  ?  aggUnits  :  0.0;	// new 1
//	pItem->cost			= ( fabs(aggCost) > 0.01 )   ?  aggCost   :  0.0;	// new 2
	pItem->mktValue		= theAsset->getMktPrice() * aggUnits;				// new 3
//	if ( fabs(pItem->mktValue) < 0.01 )
//		pItem->mktValue = 0.0;

	pItem->capGain		= pItem->mktValue - aggCost;						// new 4
//	if ( fabs(pItem->capGain) < 0.01 )
//		pItem->capGain = 0.0;
	double	exerVal = 0.0;
	if ( ((COption*)theAsset)->ExerVal( exerVal ) )
		exerVal *= aggUnits;
	pItem->income = ( instrument == Option )  ?  exerVal  :  aggIncome;
	pItem->pctYield = (fabs(aggCost) < 0.01)  ?  0.0  :  wtdYield / aggCost;

		// insert at the bottom of the positions list
	return	c_PositionsList.AddItem( (LPARAM)pItem, c_PositionsList.GetItemCount() );
}			// LoadColumnTotals()
//----------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////
//									CListCtrl Messages									//
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
void	CPositionsDialog::InitListCtrl( void )
{		// ensure that the next sort on the first column will be ascending
	lastSortKey = -1;		// this is a 1-based column index

		// map user-specified columns for use by GetPositionItemText() later
	bool	isOption = ( instrument == Option );

		// nCols initialized in InsertColumns and
		//       will reflect inclusion of a dummy column
	InsertColumns();

	const short		maxCols = 10;
	bool			optns[maxCols];
	unsigned short	lastCol = ( isOption ) ? 7 : 8;
	short			op = 0;
	long	areVisible = 0xFFFF;	// everything is visible
	if ( ! GetPosListColVisibility(&areVisible) )				// was:  theApp.regMgr->
	{
#ifdef _DEBUG
		TRACE( _T("PositionsDialog::InitListCtrl: no column visibility preferences found.\n") );
#endif
	}
															  // msi =  Bond  Option  Stock
	optns[op++] = (areVisible & PurchaseDate)			!= 0;		//    0		 0		0
	optns[op++] = (areVisible & NumUnits)				!= 0;		//	  1		 1		1
	optns[op++] = (areVisible & InitialCost)			!= 0;		//	  2		 2		2
	optns[op++] = (areVisible & MarketValue)			!= 0;		//	  3		 3		3
	optns[op++] = (areVisible & CapitalGain)			!= 0;		//	  4		 4		4

	if ( !isOption )
	{		// options don't have income, but stocks do
		optns[op++] = (areVisible & AggregateIncome)	!= 0;		//	  5				5
		optns[op++] = (areVisible & NetGain)			!= 0;		//	  6				6
	}
		// annualizedNetGain is pctYield...
	optns[op++] = (areVisible & AnnualizedNetGain)		!= 0;		//	  7		 5		7

	if ( isOption )
		optns[op++] = (areVisible & ExerciseValue)		!= 0;		//			 6

	optns[op++] = (areVisible & Note)					!= 0;		//	  8		 7		8

	unsigned short nCols = op, cm = 0;
	for ( op = 0; op < nCols; op++ )
	{
		if ( optns[ op ] )
			colMap[cm++] = op;
	}
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
//----------------------------------------------------------------------------
void	CPositionsDialog::InsertColumns( void )
{		// delete any existing columns
	c_PositionsList.DeleteAllItems();
	c_PositionsList.DeleteAllColumns();

		// in the order they will be displayed in the list control
	wchar_t*		colHdrs[] = {
		_T("OpenDate"),				  _T("Cost "),   _T("MktValue"), _T("CapGain"),
		_T("Income"), _T("NetGain"), _T("% Yield"),  _T("ExerVal"),  _T(" Note")	  };

//	bool	isBond = instrument == Bond;
	bool	isOption = instrument == Option;
	bool	isStock = instrument == Stock;

		// add columns to the List Control
	unsigned short	nCols = 0;
	unsigned short	hdrIdx = 0;
	long	areVisible = 0xFFFF;							// everything is visible by default
	if ( ! GetPosListColVisibility(&areVisible) )			// was:  theApp.regMgr->
	{
#ifdef _DEBUG
		TRACE( _T("PositionsDialog::InsertColumns: no column visibility preferences found.\n") );
#endif
	}

	if (				areVisible & PurchaseDate )
	{		// this insert/delete/insert routine moves the inevitable crash
			// with insert once, the first header label goes bad deleting 
			// and reinserting the column fixes that problem, but columnClick
			// events crash after the entire list control redisplays
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_LEFT, 70 );
	}
	hdrIdx++;			// whether dspOptns->purchaseDate is true or not

		// continue with initialization...
	if (				areVisible & NumUnits )
	{	CString		cst = _T("Units");
		short	pixels = 50;
		if		( isStock )
		{	cst = _T( "Shares"  );
			pixels = 48;
		}
//		else if	( isBond )
//		{	cst = _T( "Bonds"   );
//			pixels = 45;
//		}
		else if ( isOption )
		{	cst = _T( "Options" );
			pixels = 56;
		}
		c_PositionsList.InsertColumn( nCols++, cst, LVCFMT_RIGHT, pixels );		
	}
	//hdrIdx++;			// next header string is already "Cost "

	if (				areVisible & InitialCost )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_RIGHT, 66 );
	hdrIdx++;			// whether areVisible & InitialCost is true or not

	if (				areVisible & MarketValue )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_RIGHT, 66 );
	hdrIdx++;			// whether areVisible & MarketValue is true or not

	if (				areVisible & CapitalGain )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_RIGHT, 60 );
	hdrIdx++;			// whether areVisible & CapitalGain is true or not

	if ( !isOption  &&  (areVisible & AggregateIncome) )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_RIGHT, 60 );
	hdrIdx++;			// whether areVisible & AggregateIncome is true or not

	if ( !isOption  &&  (areVisible & NetGain) )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_RIGHT, 60 );
	hdrIdx++;			// whether areVisible & NetGain is true or not

	if (				 areVisible & AnnualizedNetGain )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_RIGHT, 60 );
	hdrIdx++;			// whether areVisible & AnnualizedNetGain is true or not

	if (  isOption  &&  (areVisible & ExerciseValue) )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_RIGHT, 66 );
	hdrIdx++;			// whether areVisible & ExerciseValue is true or not

	if (				 areVisible & Note )
		c_PositionsList.InsertColumn( nCols++, colHdrs[hdrIdx], LVCFMT_LEFT,  150 );
	hdrIdx++;			// whether areVisible & Note is true or not

		// insert a dummy column (so the previous column can be resized)
	if ( nCols > 0 )
		c_PositionsList.InsertColumn( nCols, _T(""), LVCFMT_LEFT,  30 );
}			// InsertColumns()
//----------------------------------------------------------------------------
void	CPositionsDialog::OnCustomDrawPositionsList( NMHDR* pNMHDR, LRESULT* pResult ) 
{		// direct NM_CUSTOMDRAW messages to the CustomDraw handler in RedBlackListCtrl
//	c_PositionsList.OnCustomDraw( pNMHDR, pResult );
//	COLORREF	posBck = RGB( 196, 232, 232 );			// Powder Blue
//	COLORREF	instBck = RGB( 224, 224, 180 );			// Manilla
	COLORREF	hiliteBck = RGB( 196, 232, 232 );		// Powder Blue

	NMLVCUSTOMDRAW* pCustomDraw = (NMLVCUSTOMDRAW*)pNMHDR;
	NMCUSTOMDRAW	nmcd = pCustomDraw->nmcd;
	int		si, rr = nmcd.dwItemSpec;			// item of interest (row: rr)
	switch ( nmcd.dwDrawStage )
	{	case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
		case CDDS_ITEMPREPAINT:
		{	BOSPositionListItemInfo*	plii = (BOSPositionListItemInfo*)c_PositionsList.GetItemData( rr );
			if ( plii->thePosition == 0 )
				pCustomDraw->clrTextBk = hiliteBck;

			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			break;
		}
		case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		{		// which subitem?
			BOSPositionListItemInfo*	plii = (BOSPositionListItemInfo*)c_PositionsList.GetItemData( rr );
			if ( plii->thePosition == 0 )
				pCustomDraw->clrTextBk = hiliteBck;
			
			si = pCustomDraw->iSubItem;
			CString cst = c_PositionsList.GetItemText( rr, si );
			if ( cst.GetLength() > 0  &&  cst[0] == '-' )	// it's negative
			{	pCustomDraw->clrText = RGB(255,0,0);		// use red pen
			}
			*pResult = CDRF_NOTIFYPOSTPAINT;
			break;
		}
		case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM:
		{	pCustomDraw->clrTextBk = RGB(255,255,255);		// background back to white
			pCustomDraw->clrText = RGB(0,0,0);				// text back to black
			*pResult = CDRF_DODEFAULT;
			break;
		}
		default:
			break;
	}
}			// OnCustomDrawPositionsList()
//----------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////
//									Global Functions									//
//////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------
int	CALLBACK	BOSPositionCompareFunc( LPARAM lp1, LPARAM lp2, LPARAM lpSort )
{		// Recall that the Total Row is always deleted before sorting
	BOSPositionListItemInfo*	lps1 = (BOSPositionListItemInfo*)lp1;
	BOSPositionListItemInfo*	lps2 = (BOSPositionListItemInfo*)lp2;
	SortInfo*	ssi = (SortInfo*)lpSort;
	int		plusSortKey = abs(ssi->sortKey) - 1;	// use a 0-based column index here

	CBOS*	theAsset = (CBOS*)ssi->aBOS;
//	BOOL isBond  = theAsset->IsKindOf( RUNTIME_CLASS( CBond  ) );
	BOOL isStock = theAsset->IsKindOf( RUNTIME_CLASS( CStock ) );
	BOOL isOption = !isStock;
	unsigned short	lastCol = ( isOption ) ? 7 : 8;

	int		res = 0;				// the return value
	while ( 1 )
	{		// What column is deciding our destiny?
		CPosition*	pos1 = lps1->thePosition;
		CPosition*	pos2 = lps2->thePosition;
		if ( plusSortKey == 0  ||  plusSortKey == lastCol )
		{	if	( plusSortKey == 0 )
			{		// Its the Position's purchase date (first column)
				COleDateTime&	d1 = pos1->purchaseDate;
				COleDateTime&	d2 = pos2->purchaseDate;
				res =  (d1 <  d2) ? -1 : (d1 == d2 ? 0 : 1);
				break;
			}
			if ( plusSortKey == lastCol )
			{		// Its the Position's note field (last column)
				CString&	cs1 = pos1->getNote();
				CString&	cs2 = pos2->getNote();
				res =  cs1.Compare( cs2 );
				break;
			}
		}
		if ( plusSortKey == 1 )
		{		// Its the # of Units (shares/options) in the Position
				// For Options with plusSortKey == 5 (exerVal), both Positions
				// will have the same mktPrice, so
				// nUnits is all we need to compare the positions
			double	nU1 = pos1->nUnits;
			double	nU2 = pos2->nUnits;
			res =  (nU1 -  nU2) ? -1 : ((nU1 == nU2) ? 0 : 1);
			break;
		}
		if ( plusSortKey == 2 )
		{		// Its the total cost of the Position
			double	tC1 = pos1->totalCost;
			double	tC2 = pos2->totalCost;
			res =  (tC1 <  tC2) ? -1 : ((tC1 == tC2) ? 0 : 1);
			break;
		}
		if ( plusSortKey == 3 )
		{		// Its the market value of the Position
			double	mV1 = lps1->mktValue;
			double	mV2 = lps2->mktValue;
			res =  (mV1 <  mV2) ? -1 : ((mV1 == mV2) ? 0 : 1);
			break;
		}

			// common data is the income field
		if ( (isOption  &&  plusSortKey == 6)  ||  (plusSortKey == 5  &&  !isOption) )
		{		// dividend income (zero based 5th for Stocks)
				//	exercise value (zero based 6th for Options)
			double	inc1 = lps1->income;
			double	inc2 = lps2->income;
			res =  (inc1 <  inc2) ? -1 : ((inc1 == inc2) ?  0 : 1);
			break;
		}

			// everything has Capital Gain
		double	cG1 = lps1->capGain;
		double	cG2 = lps2->capGain;
		if ( plusSortKey == 4 )
		{		// Its the capital gain for this Position
			res =  (cG1 <  cG2) ? -1 : ((cG1 == cG2) ? 0 : 1);
			break;
		}

		if ( !isOption  &&  plusSortKey == 6 )
		{		// Its the net gain (capital gain + income) from this Position
			double	cGpI1 = cG1 + lps1->income;
			double	cGpI2 = cG2 + lps2->income;
			res =  (cGpI1 <  cGpI2) ? -1 : ((cGpI1 == cGpI2) ? 0 : 1);
			break;
		}

			// the only thing left is pctYield
			// pctYield is 5th for Options, but 7th for Stocks
		double	pY1 = lps1->pctYield;
		double	pY2 = lps2->pctYield;
		res =  (pY1 < pY2) ? -1 : ((pY1 == pY2) ? 0 : 1);
		break;
	}
		// switch sense if descending
	return ( ssi->sortKey > 0 ) ? res : -res;
}			// BOSPositionCompareFunc() CALLBACK
//--------------------------------------------------------------------------------------
void	CPositionsDialog::OnKillFocusPositionsList( NMHDR* pNMHDR, LRESULT* pResult )
{		// when the positions list loses focus, the
		// positions buttons (Close, Delete, Modify) should be disabled.
	CWnd*	focusWnd = GetFocus();
		// ... this routine actually exists within a CRITICAL REGION ...
		// if the KillFocus message we're handling here was the result of clicking
		// on one of the position buttons and the following if conditions weren't there,
		// we would be disabling the very button that generated the KillFocus() message
		// before the OnClose, OnDelete or OnModify message (whichever applies)
		// got around to popping off the end of the message queue (OUCH!)
	if ( focusWnd != GetDlgItem(IDC_Close) )
		c_Close.EnableWindow( false );

	if ( focusWnd != GetDlgItem(IDC_Delete) )
		c_Delete.EnableWindow( false );
	
	if ( focusWnd != GetDlgItem(IDC_Modify) )
		c_Modify.EnableWindow( false );
	
	*pResult = 0;
}			// OnKillFocusPositionsList()
//----------------------------------------------------------------------------------------
LRESULT	CPositionsDialog::OnMarqueeEnd( WPARAM wParam, LPARAM lParam )
{		// only need to update the status of the Position Buttons
	EnablePositionButtons();
	return	0;
}			// OnMarqueeEnd()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::EnablePositionButtons( void )
{		// this function exists to avoid generating duplicate Windows messages
		// called from: OnClickPositionsList(), OnSetFocusPositionsList() & InitDialog()
	UINT	nSel = c_PositionsList.GetSelectedCount();
	bool	enable = ( nSel == 1 );
	c_Close.EnableWindow( enable );
	c_Delete.EnableWindow( nSel > 0 );
	c_Modify.EnableWindow( enable );
}			// EnablePositionButtons()
//--------------------------------------------------------------------------------------
void	CPositionsDialog::OnClickPositionsList( NMHDR* pNMHDR, LRESULT* pResult )
{
	EnablePositionButtons();
	*pResult = 0;
}			// OnClickPositionsList()
//----------------------------------------------------------------------------------------
void CPositionsDialog::OnSetFocusPositionsList( NMHDR* pNMHDR, LRESULT* pResult )
{
	EnablePositionButtons();
	*pResult = 0;
}			// OnSetFocusPositionsList()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::OnKillFocusNote( void )
{	CString note;
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	c_Note.GetWindowText( note );
	if ( note == m_Note )
		return;
	m_Note = note;
}			// OnKillFocusNote()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::OnSetFocusNote( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusNote()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::OnDeleteDefinition() 
{		// respond to Ctrl-K (kill) keyboard command
	if ( instrument == UnknownAssetType  ||  m_Symbol == _T("") )
		return;

	CDataManager*	dm = theApp.GetDataManager( instrument );
	long	def_ID = dm->AssetExists( m_Symbol );
	if ( def_ID < 0 )
		return;

	CBOS*	bos = dm->ReadBOS( def_ID );
	if ( bos == NULL )
		return;

	CMapPtrToPtr*	posLst = bos->getPositionList();
	short	nItems = posLst->GetCount();
	if ( nItems > 0 )
	{	wchar_t	buf[400];
		swprintf( buf, _T("Deleting '%s' will irrevocably delete its %d positions. Okay to proceed?"),
					m_Symbol, nItems );
		CString	caption = _T("Delete ");
		if		( instrument == Stock )		caption += _T("stock");
		else								caption += _T("option");
		caption += _T("/positions?");
		UINT result = MessageBox( buf, caption,
							  MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST );
		if ( result == IDYES )
		{		// okay to delete the definition and all of its positions
			short	res = dm->DeleteBOS( bos );
			if ( res != 0 )
			{
#ifdef _DEBUG
				TRACE( _T("PositionsDialog::OnDeleteDefinition: DeleteBOS failed, res=%d"), res );
#endif
			}
				// remove all items from the positions list
			POSITION itemPos = c_PositionsList.GetFirstSelectedItemPosition();
			while ( itemPos )
			{	int	iItem = c_PositionsList.GetNextSelectedItem( itemPos );
				c_PositionsList.DeleteItem( iItem );
			}
			c_PositionsList.DeleteAllItems();		// in case there are remnants

				// now remove m_Symbol from c_Symbol and reset it to _T("")
			int	idx = c_Symbol.FindStringExact( -1, m_Symbol );
			if ( idx != CB_ERR )
				c_Symbol.DeleteString( idx );
			c_Symbol.SetCurSel( -1 );				// clears the Edit portion of the ComboBox
		}
	}
	delete	bos;
	return;
}			// OnDeleteDefinition()
//----------------------------------------------------------------------------------------
/*
void CPositionsDialog::OnEditChangeSymbol()
{		// this functionality replaced by CSeekComboBox::OnEditChange()
	seekComboBoxEntry( IDC_Symbol );	
}			// OnEditChangeSymbol()
*/
//----------------------------------------------------------------------------------------
/*
double		CPositionsDialog::calcIncomeFV(
	CPosition*		pos,
	double&			FV						)
{
	double			income = 0.0;				// a return value
	ASSERT( theAsset );
	COleDateTime	today = COleDateTime::GetCurrentTime();
	switch ( instrument )
	{	case Bond:
		{
			CBond*	theBond = (CBond*)theAsset;
				// don't discount the coupons
//			double	err = 0.01, ytm = theBond->YieldToMaturity( err );
			income = pos->nUnits * theBond->PresentValueCoupons( pos->purchaseDate, today, 0.0 );
			FV = pos->nUnits * theAsset->mktPrice;
				// Future Value (with purchaseDate being PV) = market value + income
//			FV = pos->nUnits * theBond->PresentValuePar( today, ytm );
			break;
		}

		case Option:
		{
			COption* theOption = (COption*)theAsset;
			CStock* theStock = theOption->getUnderlying();
			if ( theStock )
			{		// use income = 0.0 for options
					// use Black-Scholes value for FV
				float	rfr = 0.03f;		// 3.0 %
				if ( theApp.pDB->getRiskFreeRate(rfr) == 0 )
				{
#ifdef _DEBUG
					TRACE( _T("PositionsDialog::calcIncomeFV: no registered Risk Free Rate.\n") );
#endif
				}
					// XXX is this the most accurate way to get yrsToExpiry?
				COleDateTimeSpan timeSpan = theOption->expiry - today;
				double	yrs = timeSpan.GetTotalDays() / 365.0;
					// for Options, FV is the current market value,
					// PV will be the position's total cost
				double	pvDivs = 0.0;
				bool	calcPvDivs = true;
				FV = pos->nUnits
				   * theOption->euroValue( theStock->mktPrice,
						theStock->volatility, yrs, rfr, pvDivs, calcPvDivs );
				// delete	theStock;	// deleting theAsset will clean this up later
			}
			break;
		}

		case Stock:
		{
			CStock* theStock = (CStock*)theAsset;
			income = pos->nUnits
					 * theStock->PresentValueDivs( pos->purchaseDate, today, 0.0 );
				// Future Value (with PV at purchaseDate) = market value + income
			FV = theStock->mktPrice * pos->nUnits + income;
			break;
		}
		default:
			break;
	}
	return	income;
}			// calcIncomeFV()
*/
//----------------------------------------------------------------------------------------
/*
void CPositionsDialog::OnSetFocusUnits()
{
	c_Units.SetSel( 0, -1 );
}			// OnSetFocusUnits()
//----------------------------------------------------------------------------------------
void CPositionsDialog::OnSetFocusTotalCost()
{
	c_TotalCost.SetSel( 0, -1 );
}			// OnSetFocusTotalCost()
//----------------------------------------------------------------------------------------
void CPositionsDialog::OnSetFocusNote()
{
	c_Note.SetSel( 0, -1 );
}			// OnSetFocusNote()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::enableDialogItems( bool enDis )
{		// enable the PurchaseDate Picker
	c_PurchaseDate.EnableWindow( enDis );

		// make the Units CEdit box readWrite
	c_Units.EnableWindow( enDis );

		// make the TotalCost CEdit box readWrite
	c_TotalCost.EnableWindow( enDis );

		// make the Note CEdit box readWrite
	c_Note.EnableWindow( enDis );

		// make the Enter CButton readWrite
	c_Enter.EnableWindow( enDis );

		// make the Positions CBOSListCtrl readWrite
	c_PositionsList.EnableWindow( enDis );
}			// enableDialogItems()
//----------------------------------------------------------------------------------------
void CPositionsDialog::OnClickPositionsList(NMHDR* pNMHDR, LRESULT* pResult) 
{	POINT	pt = ((NMRGINFO*)pNMHDR)->ptAction;
	SHRGINFO shrgi = {0};
	shrgi.cbSize = sizeof(SHRGINFO);
	shrgi.hwndClient = m_hWnd;
	shrgi.ptDown.x = pt.x;
	shrgi.ptDown.y = pt.y;
	shrgi.dwFlags = SHRG_RETURNCMD;
	if ( GN_CONTEXTMENU == ::SHRecognizeGesture(&shrgi) )
	{		// a tap and hold event has occurred, post a popup menu
		CMenu	popupMenu;
		popupMenu.LoadMenu( IDR_PositionsPopup );
		CMenu*	subMenu = popupMenu.GetSubMenu(0);

			// "Modify" is only possible with 1 position selected
		subMenu->EnableMenuItem( ID_POSITIONS_POPUP_MODIFY, 
					( c_PositionsList.GetSelectedCount() == 1 ) ? MF_ENABLED : MF_GRAYED ); 

			// 4th param indicates which HWND to send the command notifications to
		UINT cmdID = subMenu->TrackPopupMenu( TPM_RETURNCMD, pt.x, pt.y, this );
		switch ( cmdID )
		{	case ID_POSITIONS_POPUP_DELETE:
			{	OnDelete();
				break;
			}
			case ID_POSITIONS_POPUP_MODIFY:
			{	OnDblClickPositionsList( pNMHDR, pResult );
				break;
			}
			default:
				break;
		}
	}
	else		// its an ordinary click event

//	int	itemCnt = c_PositionsList.GetItemCount();
//	UINT uFlags;
//	int nItem = c_PositionsList.SetItemState( itemCnt-1, LVIS_SELECTED );
//	if ( uFlags & LVHT_ONITEMLABEL )
	
	*pResult = 0;
}			// OnClickPositionsList()
//----------------------------------------------------------------------------------------
void	CPositionsDialog::DeleteSelectedPositions( void )
{
	int	 nSelected = c_PositionsList.GetSelectedCount();
	if ( nSelected < 1 )
		return;

		// we're preventing selection of the total row (in OnClickPositionsList())
		// which should mean that the selections are actually positions (not a total row)
		// so the selected count being >= 1 can't include the total row
	POSITION itemPos = c_PositionsList.GetFirstSelectedItemPosition();
	while ( itemPos )
	{	int	iItem = c_PositionsList.GetNextSelectedItem( itemPos );
			// this is why this function lives in the dialog!
		DeleteOrModifyPosition( iItem, Deleted );
	}
}			// DeleteSelectedPositions()
*/
//----------------------------------------------------------------------------------------
/*
int	CALLBACK	BOSPositionCompareFunc( LPARAM lp1, LPARAM lp2, LPARAM lpSort )
{	BOSPositionListItemInfo*	lps1 = (BOSPositionListItemInfo*)lp1;
	BOSPositionListItemInfo*	lps2 = (BOSPositionListItemInfo*)lp2;
	SortInfo*	ssi = (SortInfo*)lpSort;
	int		plusSortKey = abs(ssi->sortKey) - 1;	// use a 0-based column index here

	CBOS*	theAsset = (CBOS*)ssi->aBOS;
	BOOL isBond  = theAsset->IsKindOf( RUNTIME_CLASS( CBond  ) );
	BOOL isStock = theAsset->IsKindOf( RUNTIME_CLASS( CStock ) );
	BOOL isOption = !isBond  &&  !isStock;
	unsigned short	lastCol = ( isOption ) ? 7 : 8;
	
	CPosition*	pos1 = lps1->thePosition;
	CPosition*	pos2 = lps2->thePosition;

	int		res = 0;				// the return value
	while ( 1 )
	{		// What column is deciding our destiny?
		if ( plusSortKey == lastCol-1 )
		{		// Its the annualizedYield (the second to last column)
			double	pY1 = lps1->pctYield;
			double	pY2 = lps2->pctYield;
			res =  (pY1 < pY2) ? -1 : ((pY1 == pY2) ? 0 : 1);
			break;
		}
		if ( plusSortKey == lastCol )
		{		// Its the Position's note field (last column)
			CString&	cs1 = pos1->getNote();
			CString&	cs2 = pos2->getNote();
			res =  cs1.Compare( cs2 );
			break;
		}
		if	( plusSortKey == 0 )
		{		// Its the Position's purchase date (first column)
			COleDateTime&	d1 = pos1->purchaseDate;
			COleDateTime&	d2 = pos2->purchaseDate;
			res =  (d1 <  d2) ? -1 : (d1 == d2 ? 0 : 1);
			break;
		}
		double	nU1 = pos1->nUnits;
		double	nU2 = pos2->nUnits;
		if ( plusSortKey == 1  ||  (isOption  &&  plusSortKey == 5) )
		{		// Its the # of Units (shares/options/bonds) in the Position
				// For Options with plusSortKey == 5 (exerVal), both Positions
				// will have the same mktPrice, so
				// nUnits is all we need to compare the positions
			res =  (nU1 -  nU2) ? -1 : ((nU1 == nU2) ? 0 : 1);
			break;
		}
		double	tC1 = pos1->totalCost;
		double	tC2 = pos2->totalCost;
		if ( plusSortKey == 2 )
		{		// Its the total cost of the Position
			res =  (tC1 <  tC2) ? -1 : ((tC1 == tC2) ? 0 : 1);
			break;
		}
		double	mktPrc;
		if ( isStock )
			mktPrc = ((CStock*)theAsset)->mktPrice;
		else if ( isBond )
			mktPrc = ((CBond*)theAsset)->mktPrice;
		else
			mktPrc = ((COption*)theAsset)->mktPrice;
		double	mV1 = mktPrc * nU1;
		double	mV2 = mktPrc * nU2;
		if ( plusSortKey == 3 )
		{		// Its the market value of the Position
			res =  (mV1 <  mV2) ? -1 : ((mV1 == mV2) ? 0 : 1);
			break;
		}
		double	cG1 = mV1 - tC1;
		double	cG2 = mV2 - tC2;
		if ( plusSortKey == 4 )
		{		// Its the capital gain for this Position
			res =  (cG1 <  cG2) ? -1 : ((cG1 == cG2) ? 0 : 1);
			break;
		}
			// Options are now done...
			// Option/Stock's pctYield & note are both handled above
			// exerVal is handled as part of nUnits
		if ( plusSortKey == 5 )
		{		// dividend or coupon income
			res =  (lps1->income <  lps2->income) ? -1
				: ((lps1->income == lps2->income) ?  0
				:  1);
			break;
		}
		if ( plusSortKey == 6 )
		{		// Its the net gain (capital gain + income) from this Position
			double	cGpI1 = cG1 + lps1->income;
			double	cGpI2 = cG2 + lps2->income;
			res =  (cGpI1 <  cGpI2) ? -1 : ((cGpI1 == cGpI2) ? 0 : 1);
			break;
		}
		ASSERT( false );
	}
		// switch sense if descending
	return ( ssi->sortKey > 0 ) ? res : -res;
}			// BOSPositionCompareFunc() CALLBACK
*/
//--------------------------------------------------------------------------------------
