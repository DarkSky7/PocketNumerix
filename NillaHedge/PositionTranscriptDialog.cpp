// PositionsTranscriptDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "PositionTranscriptDialog.h"
//#include "BOSDatabase.h"
#include "ClosedPosition.h"
//#include "Bond.h"
//#include "BondManager.h"
#include "Option.h"
#include "OptionManager.h"
#include "OptionType.h"
#include "Stock.h"
#include "StockManager.h"

#if ( ! SEH_EXCEPTIONS )
#include <exception>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CNillaHedgeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPositionTranscriptDialog dialog

CPositionTranscriptDialog::CPositionTranscriptDialog(CWnd* pParent /*=NULL*/)
	: CNillaDialog(CPositionTranscriptDialog::IDD, pParent)
	, m_activeComboBox( NULL )
	, m_itemSelected( -1 )
{
	//{{AFX_DATA_INIT(CPositionTranscriptDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
//	dMgr[0] = (CDataManager*)theApp.bndMgr;
	dMgr[0] = (CDataManager*)theApp.optMgr;		// options get position 0 (was 1)
	dMgr[1] = (CDataManager*)theApp.stkMgr;		// stocks get postion 1 (was 2)
}

void CPositionTranscriptDialog::DoDataExchange(CDataExchange* pDX)
{
	CNillaDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPositionTranscriptDialog)
	DDX_Control(pDX, IDC_YearList, c_YearList);
	DDX_Control(pDX, IDC_SaveTranscript, c_SaveTranscript);
	DDX_Control(pDX, IDC_PositionsTranscript, c_PositionsTranscript);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPositionTranscriptDialog, CNillaDialog)
	//{{AFX_MSG_MAP(CPositionTranscriptDialog)
	ON_BN_CLICKED(IDC_SaveTranscript, OnSaveTranscript)
	ON_WM_CTLCOLOR()

	ON_CBN_SELCHANGE(IDC_YearList, OnSelChangeYearList)
	ON_CBN_KILLFOCUS(IDC_YearList, OnKillFocusYearList)					// HotKeys +
	ON_CBN_SETFOCUS(IDC_YearList,  OnSetFocusYearList)					// HotKeys
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()													// HotKeys
	ON_MESSAGE(WM_HOTKEY, OnHotKey)										// HotKeys

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPositionTranscriptDialog message handlers

BOOL	CPositionTranscriptDialog::OnInitDialog() 
{	WIN32_FIND_DATA		findFileData;
	unsigned short	ii;

	CNillaDialog::OnInitDialog();
	RegisterHotKeys( IDC_YearList, IDC_PositionsTranscript );			// WM5 filtered within
	c_SaveTranscript.EnableWindow( false );

		// find all .pnh files in the database directory
	for ( ii = 0; ii < 2; ii++ )			// starting at 1 eliminates Bonds
	{	CDataManager*	dm = dMgr[ii];
		if ( dm == NULL )
			continue;
		CString	dbDir = dm->GetDBdir();

			// look for closed position files
		CString closedPosFileRegExp = dbDir + _T("\\ClosedPositions????.pnh");
		HANDLE	hSearch = FindFirstFile( closedPosFileRegExp, &findFileData );
		BOOL	done = ( hSearch == INVALID_HANDLE_VALUE );
		while ( ! done )
		{	CString	fName = findFileData.cFileName;
			CString	yrSt = fName.Mid( 15, 4 );						// get the <year> characters
			if ( c_YearList.FindString( -1, yrSt ) == CB_ERR )
				c_YearList.AddString( yrSt );						// add year to the ListBox in sorted order
			done = ( FindNextFile( hSearch, &findFileData ) == 0 );
		}
	}

	for ( ii = 0; ii < 2; ii++ )
	{	CDataManager*	dm = dMgr[ii];								// could be Options or Stocks
		if ( dm == NULL )
			continue;
		CString	dbDir = dm->GetDBdir();
		
			// look for an open positions file
		CString	openPosFileName = dbDir + _T("\\OpenPositions.pnh");
		HANDLE	hSearch = FindFirstFile( openPosFileName, &findFileData );
		if ( hSearch != INVALID_HANDLE_VALUE  &&  c_YearList.FindString( -1, _T("Open") ) == CB_ERR )
			c_YearList.InsertString( 0, _T("Open") );			// 'Open' located at index 0
	}

		// if only one year of closed positions exists, select it and load its transcript
	int	idx = 0;							// should be _T("Open")
	int	cnt = c_YearList.GetCount();
	if ( cnt > 1 )
	{	idx = c_YearList.FindStringExact( -1, _T("Open") );
		if ( idx == LB_ERR )
			idx = cnt - 1;					// if no Open, use the last (most recent) entry
	}
	c_YearList.SetCurSel( idx );
	LoadTranscript( idx );
	return	TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//---------------------------------------------------------------------------
HBRUSH CPositionTranscriptDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	return	CNillaDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}			// OnCtlColor()
//---------------------------------------------------------------------------
LRESULT		CPositionTranscriptDialog::OnHotKey( WPARAM wParam, LPARAM lParam )
{		//	UINT fuModifiers = (UINT)LOWORD(lParam);		// don't care about modifiers
		//	DWORD ctrlID = (int)wParam;						// don't care which control the HotKey is registered to (any more)
		//	CWnd* wnd = GetDlgItem( ctrlID );				// also about the registered control
	if ( ! m_activeComboBox )
		return	0;									// nothing to do

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
void	CPositionTranscriptDialog::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	CNillaDialog::OnActivate( nState, pWndOther, bMinimized );
	if ( nState == WA_ACTIVE || nState == WA_CLICKACTIVE )
		RegisterHotKeys( IDC_YearList, IDC_PositionsTranscript );	// screens for WM5 internally
}			// OnActivate()
//----------------------------------------------------------------------------
void	CPositionTranscriptDialog::OnSetFocusYearList( void )
{
	m_activeComboBox = IDC_YearList;
//	SHSipPreference( GetSafeHwnd(), SIP_UP );			// quicker to pick from a list
}			// OnSetFocusYearList()
//----------------------------------------------------------------------------
void	CPositionTranscriptDialog::OnKillFocusYearList( void )
{	CString yrSt;
//	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	m_activeComboBox = NULL;
	c_YearList.GetWindowText( yrSt );
	int sel = c_YearList.FindStringExact( -1, yrSt );
	LoadTranscript( sel );
}			// OnKillFocusYearList()
//----------------------------------------------------------------------------
void CPositionTranscriptDialog::OnSelChangeYearList( void ) 
{		// unfortunately, GetCurSel() returns the value BEFORE the change takes affect
	int cnt = c_YearList.GetCount();
	if ( cnt < 1 )
		return;
	int	sel = c_YearList.GetCurSel();
#if ( _WIN32_WCE >= 0x420 )					// just WM5
	if ( c_YearList.lastChar == VK_DOWN  ||  c_YearList.lastChar == VK_UP )
	{	int	delta = ( c_YearList.lastChar == VK_DOWN  ?  1  :  -1 );
		sel += delta;
		if ( sel < 0 )
			sel = cnt - 1;
		if ( sel >= cnt )
			sel = 0;
		c_YearList.SetCurSel( sel );
	}
#endif
	LoadTranscript( sel );
}			// OnSelChangeYearList()
//---------------------------------------------------------------------------
void	CPositionTranscriptDialog::LoadTranscript( int item )
{
	int	cnt = c_YearList.GetCount();
	if ( cnt < 0  ||  item < 0  ||  cnt <= item  ||  item == m_itemSelected )
		return;
	m_itemSelected = item;
	CString yrSt;
	COleDateTime	toDate;							// closedPositions get toDate assigned later
	wchar_t	buf[1024];
		// folderNames coincide with the order we loaded the dataManagers in { Option, Stock }
	const CString folderName[2] = { _T("Option"), _T("Stock") };
	short yr = 0;									// represents Open
	c_YearList.GetLBText( item, yrSt );
	bool itsAnOpenPosition = ( yrSt.Compare(_T("Open")) == 0 );		// 0 -> active positions, otherwise closed
	if ( itsAnOpenPosition )						// i.e. Open
		toDate = COleDateTime::GetCurrentTime();	// open positions can use the current time
	else
		yr = ParseYear( yrSt );
#ifdef _DEBUG
	TRACE( _T("PositionTranscriptDialog::LoadTranscript: item=%d, yrSt=%s, yr=%d, it %s Open\n"),
		item, yrSt, yr, (itsAnOpenPosition ? _T("is") : _T("isn't")) );
#endif

		// create a temporary file
	CFile	fp;
	CString	fName = theApp.GetDBdir();
	fName += _T("\\transcript.txt");
	CFileException fileException;
	if ( !fp.Open( fName, CFile::modeReadWrite | CFile::modeCreate) )
	{
#ifdef _DEBUG
		TRACE( _T("PositionTranscriptDialog::loadTranscript(-1):  Can't open file %s, error -> %s\n"),
			fName, fileException.m_cause );
#endif
		return;
	}

		// reset portfolio wide accumulators
	double	aggCost = 0.0;
	double	aggGain = 0.0;					// this is capGain, not netGain
	double	aggIncome = 0.0;
	double	wtdYield = 0.0;
	double	netGain;
	double	folderExerVal = 0.0;			// only the options folder can update this so no need to reset it on each pass
	int		foldersWithPositions = 0;
	int		aggPos = 0;						// number of positoins in the portfolio

		// loop through all positions
	for ( short ii = 0; ii < 2; ii++ )		// Bonds were 0, now 0 is for options, problems could arise
	{	CDataManager*	dm = dMgr[ii];
		if ( dm == NULL )
			continue;
		int		nPos = 0;					// number of positions in the folder
		double	folderCost = 0.0;
		double	folderGain = 0.0;			// this is the sum of folder capGains, not netGains
		double	folderIncome = 0.0;
		double	folderYield = 0.0;			// this is weighted by totalCost
		
		CClosedPosition* cPos = NULL;
		PositionIterationContext*	ctx = dm->GetPositionIterationContext( yr );
		while ( ctx != NULL )
		{	CPosition* pos = dm->GetNextPosition( ctx );
			if ( pos == NULL )
				break;
				// we're scanning through a Position file serially
				// if we're looking at the active list (the OpenPositions file), we could retrieve a deleted Position
				// if so, discard it and continue scanning
			if ( pos->status == DeletedPosition )
			{	delete	pos;
				continue;
			}

				// we have a valid position
			nPos++;

			if ( ! itsAnOpenPosition )							// i.e. Closed
			{		// it's a closed position
				cPos = (CClosedPosition*)pos;
				toDate = cPos->dateClosed;
			}

			CString	instUnits = _T("???");
			double	income = 0.0;
			double	exerVal = 0.0;

				// format the Position for the transcript

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				// get the BOS definition
			CBOS*	bos;					// a polymorphic handle on the current instrument definition
//			if ( pos->inst == Bond )
//			{	instUnits = _T("bonds");
//					// need to discount coupons, so we get the full instrument definition
//				CBond*	bnd = theApp.bndMgr->ReadBond( pos->def_ID );
//				if ( bnd )
//					income = bnd->PresentValueCoupons( pos->purchaseDate, toDate, 0.0 );	// not scaled by pos->nUnits
//				bos = (CBOS*)bnd;
//			}
//			else
			if ( pos->inst == Option )
			{	instUnits = _T("options");
				COption*	opt = theApp.optMgr->ReadOption( pos->def_ID );
				double	exerVal = 0.0;
				if ( opt )
					opt->ExerVal( exerVal );							// not scaled by pos->nUnits
#ifdef _DEBUG
//			TRACE( _T("0: nPos=%d, pos=%d, pos->inst=%d, pos->def_ID=%d, opt=%d\n"),
//					nPos, pos, pos->inst, pos->def_ID, opt );
#endif			
				if ( opt->putCall == Call )
					instUnits = _T("calls");
				else if ( opt->putCall == Put )
					instUnits = _T("puts");
				bos = (CBOS*)opt;
			}
			else if ( pos->inst == Stock )
			{	instUnits = _T("shares");
					// need to discount dividends, so we get the full instrument definition
				CStock*	stk = theApp.stkMgr->ReadStock( pos->def_ID );
				if ( stk )
					income = stk->PresentValueDivs( pos->purchaseDate, toDate, 0.0 );		// not scaled by pos->nUnits
				bos = (CBOS*)stk;
			}

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				// append position information
			CString	purchDate = (( nPos > 1 )  ?  _T("\r\n")  :  _T(""));	// at this point, nPos is either 1 or greater
			purchDate += EuroFormat( pos->purchaseDate );
			swprintf( buf, _T("%s:  %s %g '%s' {%s} %s for %.2f.\r\nOpening Note: {%s}\r\n"),
							purchDate, (pos->totalCost > 0.0 ? _T("Bought") : _T("Sold")),
							fabs(pos->nUnits), bos->getSymbol(), bos->getDesc(), instUnits,
							pos->totalCost, pos->getNote() );
			folderCost += pos->totalCost;
#ifdef _DEBUG
//			TRACE( _T("1: %s"), buf );
#endif
			fp.Write( buf, sizeof(buf[0])*wcslen(buf) );

				// include closed position information
			if ( ! itsAnOpenPosition )											// i.e. Closed
			{	CString	saleDate = EuroFormat( cPos->dateClosed );
				swprintf( buf,
					_T("%s:  Sold for %.2f\r\nClosing Note: {%s}\r\n"),
					saleDate, cPos->netProceeds, cPos->closingNote );
#ifdef _DEBUG
//				TRACE( _T("2: %s\n"), buf );
#endif
				fp.Write( buf, sizeof(buf[0])*wcslen(buf) );
			}

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				// append the capital gain
				// temporarily, capGain is the market value of an open position or
				//						   the proceeds from a closed position
			double	capGain = itsAnOpenPosition ? (bos->getMktPrice() * pos->nUnits) : cPos->netProceeds;
			capGain -= pos->totalCost;
				// now capGain is really the capital gain
			folderGain += capGain;
			swprintf( buf, _T("Capital Gain: %.2f\r\n"), capGain );
			fp.Write( buf, sizeof(buf[0])*wcslen(buf) );

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				// stock specific stuff (Income)
			if ( pos->inst != Option )			// stocks
			{	income *= pos->nUnits;
				folderIncome += income;
				swprintf( buf, _T("Income: %.2f\r\n"), income );
				fp.Write( buf, sizeof(buf[0])*wcslen(buf) );
			}

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				// calculate the netGain for everything, but just display it for stocks
			double	netGain = capGain + income;
			if ( pos->inst != Option )			// stocks
			{	swprintf( buf, _T("Net Gain: %.2f\r\n"), netGain );
				fp.Write( buf, sizeof(buf[0])*wcslen(buf) );
			}

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				// append the pctYield
			long	spDays;
			double	yrs = calcYears( pos->purchaseDate, toDate, spDays );
			double	topLine = netGain + pos->totalCost;
			double	rateArg = topLine / pos->totalCost;
			bool	skipCalc = ( rateArg == 0.0  ||  yrs < 1e-4 );
			double	pctYield = skipCalc  ?  0.0  :  ( 100.0 * log(fabs(rateArg)) / yrs );
			folderYield	  += pos->totalCost * pctYield;
			swprintf( buf, _T("Yield: %.3f%%\r\n"), pctYield );
			fp.Write( buf, sizeof(buf[0])*wcslen(buf) );

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				// option specific stuff (ExerVal)
			if ( pos->inst == Option )
			{	exerVal *= pos->nUnits;
				folderExerVal += exerVal;
				swprintf( buf, _T("Exercise Value: %.2f\r\n"), exerVal );
				fp.Write( buf, sizeof(buf[0])*wcslen(buf) );
			}

				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				// cleanup & see if there are any more closed positions in this year
			delete	bos;
			delete	pos;
		}

		wtdYield  += folderYield;			// want the weighted version to scale against other folders
			// unweight the folderYield so we can print something meaningful
		folderYield = (fabs(folderCost) < 1e-5)  ?  0.0  :  (folderYield / folderCost);

			// write the folder totals...
		if ( nPos > 0 )
		{	foldersWithPositions++;
			if ( nPos > 1 )
			{	if ( ii == 0 )
				{		// options (0)
					swprintf( buf, _T("\r\n%s Totals:\r\nCost: %.2f\r\nCapital Gain: %.2f\r\nYield: %.3f%%\r\nExercise Value: %.2f\r\n"),
							folderName[ii], folderCost, folderGain, folderYield, folderExerVal );
				}
				else
				{		// stocks (1)
					netGain = folderGain + folderIncome;			// folderGain only includes capGain
					swprintf( buf, _T("\r\n%s Totals:\r\nCost: %.2f\r\nCapital Gain: %.2f\r\nIncome: %.2f\r\nNet Gain: %.2f\r\nYield: %.3f%%\r\n"),
							folderName[ii], folderCost, folderGain, folderIncome, netGain, folderYield );
				}
				fp.Write( buf, sizeof(buf[0])*wcslen(buf) );
			}
			const	CString	seperator = _T("---------------------------------------");
			const	short	sepLen = seperator.GetLength();
			fp.Write( seperator, sizeof(seperator[0]) * sepLen );
			CString	trailer = folderName[ii] + _T("s\r\n");
			short	trlrLen = trailer.GetLength();
			fp.Write( trailer, sizeof(trailer[0]) * trlrLen );
		}

			// wtdYield was incremented above the prior if(), but other portfolio aggregates increment here
		aggPos	  += nPos;
		aggCost	  += folderCost;
		aggGain	  += folderGain;
		aggIncome += folderIncome;
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// rescale the weighted yield by the transcript's aggregate cost
	wtdYield = (fabs(aggCost) < 1e-3)  ?  0.0  :  (wtdYield / aggCost);	// it's now the true transcript yield


		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// print the portfolio totals
	if ( foldersWithPositions > 1 )
	{	netGain = aggGain + aggIncome;		// aggGain only includes capGain
		swprintf( buf, _T("Transcript Totals:\r\nCost: %.2f\r\nCapital Gain: %.2f\r\nIncome: %.2f\r\nNet Gain: %.2f\r\nYield: %.3f%%\r\n"),
				aggCost, aggGain, aggIncome, netGain, wtdYield );
		fp.Write( buf, sizeof(buf[0])*wcslen(buf) );
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// save, rewind, and read the file into the control all at once
	fp.Flush();
	const	unsigned long	fLen = (unsigned long)fp.GetLength();
	wchar_t*	transText = new wchar_t[fLen+1];
	fp.SeekToBegin();						// rewind
	fp.Read( transText, fLen );			// read the whole file into the transText heap buffer
	fp.Close();

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// put the contents of transText into the control

		// reset the control contents
	c_PositionsTranscript.SetSel( 0, -1, FALSE );		// select everything (don't scroll)
	c_PositionsTranscript.Clear();						// delete selection
	c_PositionsTranscript.SetSel( 0, -1, FALSE );		// select everything (don't scroll)
	c_PositionsTranscript.ReplaceSel( transText );
	c_PositionsTranscript.FmtLines( TRUE );				// insert soft carriage returns as needed

		// enable the Save button
	c_SaveTranscript.EnableWindow( aggPos > 0 );

		// we're done with the buffer too
	delete [] transText;
}			// LoadTranscript()
//---------------------------------------------------------------------------
short	CPositionTranscriptDialog::ParseYear( CString yrSt )
{	short	yr;
	swscanf( yrSt, _T("%d"), &yr );
	return	yr;
}			// ParseYear()
//---------------------------------------------------------------------------
void	CPositionTranscriptDialog::OnSaveTranscript( void ) 
{		// FALSE indicates Save As... functionality
	int	curSel = c_YearList.GetCurSel();
	CString	fName = _T("%ce5%\\");
	if ( curSel == 0 )
		fName += _T("Open");
	else
	{	CString	yrSt;
		c_YearList.GetLBText( curSel, yrSt );
		fName += yrSt;
		fName += _T(" Closed");
	}
	fName += _T(" Positions Transcript.txt");

		// pick a path name
	CFileDialog	dlg( FALSE, _T(".txt"), fName );
	int	nDisposition = dlg.DoModal();
	if ( nDisposition != IDOK )
		return;
	fName = dlg.GetPathName();

		// open the specified file
	CFile	fp;
	CFileException ex;
	if( ! fp.Open( fName, CFile::modeCreate | CFile::modeWrite, &ex ) )
	{
#ifdef _DEBUG
		TRACE( _T("ClosedPositionTranscriptDialog::OnSaveTranscript:  Error(1) opening '%s' for writing.\n"),
			fName );
#endif
		return;
	}
		// save CEdit contents to disk
	CString	transText;
	c_PositionsTranscript.GetWindowText( transText );
#if ( SEH_EXCEPTIONS )
	__try
#else
	try
#endif
	{
		fp.Write( &transText, transText.GetLength() );
	}
#if ( SEH_EXCEPTIONS )
	__except ( EXCEPTION_EXECUTE_HANDLER )
#else
	catch ( std::exception& ex )
#endif
	{
#ifdef _DEBUG
		TRACE( _T("ClosedPositionTranscriptDialog::OnSaveTranscript:  Error(2) writing to '%s'.\n"),
			fName );
#endif
		return;
	}
	fp.Flush();
	fp.Close();
}			// OnSaveTranscript()
//---------------------------------------------------------------------------
/*
void CPositionTranscriptDialog::OnEditChangeYearList( void )
{		// this functionality now preformed by CSeekComboBox::OnEditChange()
	seekComboBoxEntry( IDC_YearList );
}			// OnEditChangeYearList()
*/
//---------------------------------------------------------------------------
