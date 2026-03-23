// RateSensitivityExplorer.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "RateSensitivityExplorer.h"
#include "BondRateSensitivity.h"			// registry retrieval struct

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "BOS.h"
#include "BondManager.h"
//#include "OptionManager.h"
#include "RegistryManager.h"
#include "Position.h"				// for the PositionIterationContext
#include "FPoint.h"
//#include "Option.h"
#include "Bond.h"

#include "BondManagerApp.h"
extern CBondManagerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRateSensitivityExplorer dialog


CRateSensitivityExplorer::CRateSensitivityExplorer( CWnd* pParent /*=NULL*/ )
	: CPlotOptionsDialog( IDD, pParent )
	, m_activeComboBox( NULL )
	, folderFptsValid( false )
{	unsigned short ii;
	//{{AFX_DATA_INIT(CRateSensitivityExplorer)
//	m_DeltaHigh = 2.5f;
//	m_DeltaLow = 1.5f;
	//}}AFX_DATA_INIT

	topLeft.x = -1.5f;
	bottomRight.x = 2.5f;

		// override this default if there's a saved rateSens in the registry
		// registered (saved) state
	for ( ii = 0; ii < nBOS; ii++ )
	{	rateSens.bndChk[ii] = true;
		rateSens.bndDef_ID[ii] = -1;
		instFptsValid[ii] = false;
	}
	rateSens.bndPortf = false;
}

CRateSensitivityExplorer::~CRateSensitivityExplorer( void )
{
}

void	CRateSensitivityExplorer::DoDataExchange( CDataExchange* pDX )
{
	CPlotOptionsDialog::DoDataExchange( pDX );
	//{{AFX_DATA_MAP( CRateSensitivityExplorer )
	DDX_Control(pDX, IDC_DeltaHigh, c_DeltaHigh);
	DDX_Control(pDX, IDC_DeltaLow, c_DeltaLow);
	DDX_Control(pDX, IDC_Portfolio, c_Portfolio);
//	DDX_Text(pDX, IDC_DeltaHigh, m_DeltaHigh);
//	DDV_MinMaxFloat(pDX, m_DeltaHigh, 0.f, 9999.f);
//	DDX_Text(pDX, IDC_DeltaLow, m_DeltaLow);
//	DDV_MinMaxFloat(pDX, m_DeltaLow, 0.f, 9999.f);
	DDX_Control(pDX, IDC_Symbol1, c_Symbol1);
	DDX_Control(pDX, IDC_Symbol2, c_Symbol2);
	DDX_Control(pDX, IDC_Symbol3, c_Symbol3);
	DDX_Control(pDX, IDC_Symbol4, c_Symbol4);
	DDX_Control(pDX, IDC_Check1, c_Check1);
	DDX_Control(pDX, IDC_Check2, c_Check2);
	DDX_Control(pDX, IDC_Check3, c_Check3);
	DDX_Control(pDX, IDC_Check4, c_Check4);
	DDX_CBString(pDX, IDC_Symbol1, m_Symbol1);
	DDV_MaxChars(pDX, m_Symbol1, 254);
	DDX_CBString(pDX, IDC_Symbol2, m_Symbol2);
	DDV_MaxChars(pDX, m_Symbol2, 254);
	DDX_CBString(pDX, IDC_Symbol4, m_Symbol4);
	DDV_MaxChars(pDX, m_Symbol4, 254);
	DDX_CBString(pDX, IDC_Symbol3, m_Symbol3);
	DDV_MaxChars(pDX, m_Symbol3, 254);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( CRateSensitivityExplorer, CPlotOptionsDialog )
	//{{AFX_MSG_MAP( CRateSensitivityExplorer )
	ON_BN_CLICKED(IDC_Check1, OnCheck1)
	ON_BN_CLICKED(IDC_Check2, OnCheck2)
	ON_BN_CLICKED(IDC_Check3, OnCheck3)
	ON_BN_CLICKED(IDC_Check4, OnCheck4)
	ON_BN_CLICKED(IDC_Portfolio, OnPortfolio)
	ON_EN_KILLFOCUS(IDC_DeltaHigh, OnKillFocusDeltaHigh)
	ON_EN_KILLFOCUS(IDC_DeltaLow, OnKillFocusDeltaLow)
	ON_EN_SETFOCUS(IDC_DeltaHigh, OnSetFocusDeltaHigh)
	ON_EN_SETFOCUS(IDC_DeltaLow, OnSetFocusDeltaLow)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_Symbol1, OnSelChangeSymbol1)
	ON_CBN_SELCHANGE(IDC_Symbol2, OnSelChangeSymbol2)
	ON_CBN_SELCHANGE(IDC_Symbol3, OnSelChangeSymbol3)
	ON_CBN_SELCHANGE(IDC_Symbol4, OnSelChangeSymbol4)
	ON_WM_CTLCOLOR()

	ON_CBN_KILLFOCUS(IDC_Symbol1, OnKillFocusSymbol1)			// HotKeys +
	ON_CBN_KILLFOCUS(IDC_Symbol2, OnKillFocusSymbol2)			// HotKeys +
	ON_CBN_KILLFOCUS(IDC_Symbol3, OnKillFocusSymbol3)			// HotKeys +
	ON_CBN_KILLFOCUS(IDC_Symbol4, OnKillFocusSymbol4)			// HotKeys +
	ON_CBN_SETFOCUS(IDC_Symbol1, OnSetFocusSymbol1)				// HotKeys
	ON_CBN_SETFOCUS(IDC_Symbol2, OnSetFocusSymbol2)				// HotKeys
	ON_CBN_SETFOCUS(IDC_Symbol3, OnSetFocusSymbol3)				// HotKeys
	ON_CBN_SETFOCUS(IDC_Symbol4, OnSetFocusSymbol4)				// HotKeys
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()											// HotKeys
	ON_MESSAGE(WM_HOTKEY, OnHotKey)								// HotKeys

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRateSensitivityExplorer message handlers

BOOL CRateSensitivityExplorer::OnInitDialog()
{
		// set the extents of plot region
	CPlotOptionsDialog::OnInitDialog();
	RegisterHotKeys( IDC_Symbol1, IDC_Symbol2 );		// screens for WM5 internally

	plotExtents.top = 104;			// leaves enough room for the controls at the top

		// enable Bond and Option buttons
	int		nBnds = theApp.bndMgr  ?  theApp.bndMgr->GetSymbolTableSize()  :  0;

		// these dialog globals (help us decide whether to enable the portfolio check button)
	nIndexedIssues = theApp.bndMgr->NumIndexedIssues();		// numBondsWithPositions

		// see if there's a registered RateSensitivity structure
	if ( GetBondRateSensitivity( &rateSens ) )
	{		// deltas are managed via m_Delta* variables,
			// so rateSens must be updated when the dialog closes
		topLeft.x    = -rateSens.deltaLow;
		bottomRight.x = rateSens.deltaHigh;
	}
	else
	{
#ifdef _DEBUG
		TRACE( _T("RateSensitivityExplorer::OnInitDialog: GetBondRateSensitivity failed.\n") );
#endif
	}

		// check the selected instrument and choose a CDataManager
	SetCheckButtons();				// the five plot enablers from rateSens	
	LoadBOSs();						// fetch bond/options recently evaluated
	LoadSymbols();					// load ComboBoxes & select items matching the current BOSs

			// load up the NumEdit boxes (either registered values or defaults)
	setEditBox( "%.3f",   -topLeft.x,  IDC_DeltaLow  );
	setEditBox( "%.3f", bottomRight.x, IDC_DeltaHigh );
	RecalcAll();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSelChangeSymbol1( void )
{
	SelChangeSymbolWork( &c_Symbol1, 0, m_Symbol1 );
}			// OnSelChangeSymbol1()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSelChangeSymbol2( void )
{
	SelChangeSymbolWork( &c_Symbol2, 1, m_Symbol2 );
}			// OnSelChangeSymbol2()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSelChangeSymbol3( void )
{
	SelChangeSymbolWork( &c_Symbol3, 2, m_Symbol3 );
}			// OnSelChangeSymbol3()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSelChangeSymbol4( void )
{
	SelChangeSymbolWork( &c_Symbol4, 3, m_Symbol4 );
}			// OnSelChangeSymbol4()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::SelChangeSymbolWork(
	CSeekComboBox*	scb,
	short			ii,
	CString&		bndSym								)
{
	int cnt = scb->GetCount();
	if ( cnt < 1 )
		return;
	int	sel = scb->GetCurSel();
#if ( _WIN32_WCE >= 0x420 )						// just PPC03SE & WM5
	if ( scb->lastChar == VK_DOWN  ||  scb->lastChar == VK_UP )
	{	int	delta = ( scb->lastChar == VK_DOWN  ?  1  :  -1 );
		sel += delta;
		if ( sel < 0 )
			sel = cnt - 1;
		if ( sel >= cnt )
			sel = 0;
		scb->SetCurSel( sel );
	}
#endif
	if ( sel < 0 ) sel = 0;						// should never happen
	scb->GetLBText( sel, bndSym );				// set the m_Symbol<n> variable
	KillFocusSymbolWorkCore( ii, bndSym );
}			// SelChangeSymbolWork()
//----------------------------------------------------------------------------
LRESULT		CRateSensitivityExplorer::OnHotKey( WPARAM wParam, LPARAM lParam )
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
void	CRateSensitivityExplorer::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	CNillaDialog::OnActivate( nState, pWndOther, bMinimized );
	if ( nState == WA_ACTIVE  ||  nState == WA_CLICKACTIVE )
		RegisterHotKeys( IDC_Symbol1, IDC_Symbol2 );	// screens for WM5 internally
}			// OnActivate()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::SetFocusSymbolWork( DWORD ctrlID )
{
	m_activeComboBox = ctrlID;
//	SHSipPreference( GetSafeHwnd(), SIP_UP );		// much faster to pick from list than type on SIP
}			// SetFocusSymbolWork()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSetFocusSymbol1( void )
{
	SetFocusSymbolWork( IDC_Symbol1 );
}			// OnSetFocusSymbol1()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSetFocusSymbol2( void )
{
	SetFocusSymbolWork( IDC_Symbol2 );
}			// OnSetFocusSymbol2()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSetFocusSymbol3( void )
{
	SetFocusSymbolWork( IDC_Symbol3 );
}			// OnSetFocusSymbol3()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSetFocusSymbol4( void )
{
	SetFocusSymbolWork( IDC_Symbol4 );
}			// OnSetFocusSymbol4()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
HBRUSH	CRateSensitivityExplorer::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	return	CPlotOptionsDialog::OnCtlColor( pDC, pWnd, nCtlColor );
}			// OnCtlColor()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::LoadBOSs( void ) 
{	short	jj;
		// we're initializing the dialog
		// delete any existing BOSs
	for ( jj = 0; jj < nBOS; jj++ )
	{	if ( *(pBOS+jj) )
		{	delete	*(pBOS+jj);
			*(pBOS+jj) = NULL;
		}
	}

		// select a def_ID block
	long*	def_ID = rateSens.bndDef_ID;

		// fill any open slots (containing -1) in the selected def_ID
	FillSlots( def_ID );

		// load BOSs corresponding to the def_IDs
	for ( jj = 0; jj < nBOS; jj++ )
	{	if ( *(def_ID+jj) >= 0 )
		{	*(pBOS+jj) = theApp.bndMgr->ReadBond( *(def_ID+jj) );
		}
	}
}			// LoadBOSs()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::FillSlots( long* def_ID ) 
{		// override CPlotOptionsDialog::FillSlots()
	CBondManager*  bm = theApp.bndMgr;
	POSITION pos = bm->GetDefIterCtx();
	while ( pos )
	{	short	firstSlot = -1;
		for ( short jj = 0; jj < nBOS; jj++ )
		{	if ( *(def_ID+jj) < 0 )
			{	firstSlot = jj;
				break;
			}
		}
		if ( firstSlot < 0 )
			break;
		CString	sym;			// don't really care
		long	offset = bm->GetNextDef_ID( pos, sym );
		if (	offset >= 0
			&&  offset != *(def_ID+0)
			&&  offset != *(def_ID+1)
			&&  offset != *(def_ID+2)
			&&  offset != *(def_ID+3) )
			*(def_ID+firstSlot) = offset;
	}
}			// FillSlots()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::SetCheckButtons( void ) 
{
	CPlotOptionsDialog::SetCheckButtons( rateSens.bndChk );

		// set Bond/Option and Portfolio buttons
	c_Portfolio.EnableWindow( nIndexedIssues > 0 );
	bPortfolio = rateSens.bndPortf  &&  ( nIndexedIssues > 0 );
	c_Portfolio.SetCheck( bPortfolio  ?  BST_CHECKED  :  BST_UNCHECKED );
}			// SetCheckButtons()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::LoadSymbols( void ) 
{		// load the symbols associated with the current instrument type
	CMapStringToPtr* theMap = theApp.bndMgr->GetSymbolTable();
	LoadSymbolsCore( theMap );
}			// LoadSymbols()
//----------------------------------------------------------------------------------------
bool	CRateSensitivityExplorer::InstValid( unsigned short jj )
{		// jj is 0-based
	ASSERT( jj < nBOS );
	if ( ! rateSens.bndChk[jj] )
		return	false;

	if ( *(pBOS+jj) == NULL )
		return	false;

	return  (*(pBOS+jj))->getMktPrice() >= 0.001;
}			// InstValid()
//----------------------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnPaint( void )
{
	unsigned short ii = 0;
#ifdef _DEBUG
	TRACE( _T("RateSensitivityExplorer::OnPaint: begin\n") );
#endif
	bool	somethingValid = folderFptsValid;
	while ( ! somethingValid  &&  ii < nBOS )
		somethingValid = somethingValid  ||  instFptsValid[ii++];
	
		// nothing to paint if there are no issues to evaluate
	CPaintDC	dc( this );								// device context for painting
	DrawPlotBackground( dc );							// blanks out the plot space
	if ( ! somethingValid )
	{	CDialog::OnPaint();					// paints empty dialog if there's nothing to do
		return;
	}

		// now we can paint the four Bond instances
	topLeft.y = -2e38f;						// ~ negative infinity
	bottomRight.y = 2e38f;					// ~ positive infinity
	for ( ii = 0; ii < nBOS; ii++ )
	{	if ( InstValid(ii) )
		{	if ( instMinY[ii] < bottomRight.y )
				bottomRight.y = instMinY[ii];
			if ( instMaxY[ii] > topLeft.y )
				topLeft.y = instMaxY[ii];
		}
	}
	if ( bPortfolio  &&  folderFptsValid )
	{	if ( folderMinY < bottomRight.y )
			bottomRight.y = folderMinY;
		if ( folderMaxY > topLeft.y )
			topLeft.y = folderMaxY;
	}

		// prevent very high scale
	if ( (topLeft.y - bottomRight.y) < 1.0 )
	{	topLeft.y += 0.5;
		bottomRight.y -= 0.5;
	}
			// scale against available space and plot
	xScale = (float)(plotExtents.Width()  / (bottomRight.x - topLeft.x));
	yScale = (float)(plotExtents.Height() / (topLeft.y - bottomRight.y));
#ifdef _DEBUG
		TRACE( _T("RateSensitivityExplorer::OnPaint: xScale=%g, yScale=%g\n"), xScale, yScale );
#endif
		// paint time
	DrawGrid( dc );
	for ( ii = 0; ii < nBOS; ii++ )
	{	if ( InstValid(ii)  &&  instFptsValid[ii] )				// check button state examined by InstValid()
			DrawPolyline( ii, dc, (instFpts + ii*nInstRatePts), nInstRatePts );
	}
		// plot the portfolio line
	if ( bPortfolio  &&  folderFptsValid )						// portfolio plotting is enabled & the points are valid
		DrawPolyline( nBOS, dc, folderFpts, nFolderRatePts );	// nBOS --> black

	DrawGridLabels( dc );
}			// OnPaint()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::CalcInstFpts( unsigned short ii )
{		// calculate the instFpts for 
	if ( ! InstValid( ii ) )
		return;
	double yVal, syVal;
	CWaitCursor	wait;			// we could be gone a while - display the wait cursor

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// compute the issue plot points and the domain Y-extents
		// set up for Bond evaluation
	CBond* bnd = (CBond*)*(pBOS+ii);
	bondEvalCtx.cvxtyIsValid = false;
	bondEvalCtx.macDurIsValid = false;
	bondEvalCtx.yrsToMaturityIsValid = false;
	bondEvalCtx.ytmIsValid = false;
	bondEvalCtx.fromDate = COleDateTime::GetCurrentTime();

		// collect Y-values and update instMinY[ii] and instMaxY[ii]
	double	xVal = topLeft.x;
	double	xInc = (bottomRight.x - topLeft.x) / (nInstRatePts - 1);
	instMinY[ii] = 2e38f;			// dialog var
	instMaxY[ii] = -2e38f;			// dialog var
	CFPoint* instPts = instFpts + ii * nInstRatePts;
	for ( unsigned short jj = 0; jj < nInstRatePts; jj++ )		// for each of the FPoints
	{		// rescale xVal from percent to a fractional value
		yVal = bnd->DeltaPdivP( bondEvalCtx, xVal/100.0 );		// create a fractional xVal from the percentage
		
			// rescale yVal from a fractional value to percent
		syVal = 100.0 * yVal;

			// update min and max Y-values
		if ( syVal < instMinY[ii] )
			instMinY[ii] = (float)syVal;

		if ( syVal > instMaxY[ii] )
			instMaxY[ii] = (float)syVal;
		
		CFPoint* cfp = instPts + jj;
		cfp->x = (float)xVal;
		cfp->y = (float)syVal;	// store the rescaled Y-value
		xVal += xInc;											// prepare for the next point
	}
#ifdef _DEBUG
	TRACE( _T("RateSensitivityExplorer::CalcInstFpts: minYval=%g, maxYval=%g\n"),
			instMinY[ii], instMaxY[ii] );
#endif
	instFptsValid[ii] = true;
}			// CalcInstFpts()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::CalcFolderFpts( void )
{	CFPoint* cfp;				// a handy pointer into portfolioPts
	if ( ! bPortfolio )
		return;
	CWaitCursor	wait;			// we could be gone a while - display the wait cursor

		// compute the folder's rate sensitivity
		// overwrite either folderFpts with results
	COleDateTime today = COleDateTime::GetCurrentTime();
	double	folderCost = 0.0;									// of all bond positions
	float	xInc = (bottomRight.x - topLeft.x) / (nFolderRatePts - 1);

		// initialize the portfolio points
	long	ii;								// issue index
	short	jj;								// points index
	float	xVal = topLeft.x;
	for ( jj = 0; jj < nFolderRatePts; jj++ )
	{	cfp = folderFpts + jj;
		cfp->x = xVal;
		cfp->y = 0.0f;
		xVal += xInc;
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// for each issue with positions
	double	yVal;
	CBondManager*	bm = theApp.bndMgr;
	for ( ii = 0; ii < nIndexedIssues; ii++ )
	{	long	def_ID = bm->GetIndexedDef_ID( ii );
			// set up for Bond evaluation
		CBond* bnd = bm->ReadBond( def_ID );

			// continue if critical conditions aren't met
		if ( bnd == NULL )
		{
#ifdef _DEBUG
			TRACE( _T("RateSensitivityExplorer::CalcFolderPts: NULL bnd encountered.\n") );
			AfxDebugBreak();
#endif
			continue;
		}

		CMapPtrToPtr*	posLst = bnd->getPositionList();
		if ( posLst == NULL )
		{
#ifdef _DEBUG
			TRACE( _T("RateSensitivityExplorer::CalcFolderPts: %s has no position list\n"),
				bnd->getSymbol() );
			AfxDebugBreak();
#endif
			delete	bnd;
			bnd = NULL;
			continue;
		}

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// compute the weighting factor (issueBasis) for this issue
		long	posOffset = bnd->openPosOffset;
		double	issueBasis = 0.0;							// sum of totalCost across all of bnd's positions
		CPosition*	pos;
		while ( posLst->Lookup( (void*)posOffset, (void*&)pos ) )
		{		// process this position
			issueBasis += pos->totalCost;
				// prepare for next loop pass
			posOffset = pos->prevPos_ID;
		}														// for each position
#ifdef _DEBUG
		TRACE( _T("RateSensitivityExplorer::CalcFolderPts: %s has basis=%.2f\n"),
				bnd->getSymbol(), issueBasis );
#endif
		
			// update the Y-values
		for ( jj = 0; jj < nFolderRatePts; jj++ )				// for each of the FPoints
		{	cfp = folderFpts + jj;
			yVal = (float)bnd->DeltaPdivP( cfp->x/100.0 );		// create a fractional xVal from the percentage
				// upscale the Y-value by the total cost (issueBasis) of the issue's positions 
			yVal *= issueBasis;
			cfp->y += (float)yVal;		// update the Y-value for this position
		}								// for each point
		folderCost += issueBasis;
		delete	bnd;					// we're done with this Bond
		bnd = NULL;
	}									// for each issue
#ifdef _DEBUG
	TRACE( _T("RateSensitivityExplorer::OnPaint: %d bonds have aggregate basis=%.2f\n"),
		nIndexedIssues, folderCost );
#endif
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// post issue-loop downscaling by folderCost, upscaling to percentages
	folderMinY = 2e38f;				// dialog var
	folderMaxY = -2e38f;			// dialog var
	for ( jj = 0; jj < nFolderRatePts; jj++ )
	{	cfp = folderFpts + jj;
				// rescale the yVal from a fractional value to percent
		float	syVal = (float)((fabs(folderCost) < 0.001)  ?  0.0  :  (100.0 * cfp->y / folderCost));

			// update min and max Y-values
		if ( syVal < folderMinY )
			folderMinY = syVal;

		if ( syVal > folderMaxY )
			folderMaxY = syVal;
		
		cfp->y = syVal;			// replace the Y-value with the final rescaled (percent change) version
	}
	folderFptsValid = true;
}			// CalcFolderFpts()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::KillFocusSymbolWorkCore( short ii, CString sym )
{		// ii is the zero-based index of the bond
		// kill any Bond at this index
	if ( *(pBOS+ii) )
		delete	*(pBOS+ii);

		// make a polymorphic pointer into rateSens
	long*	def_IDs = rateSens.bndDef_ID;
	*(pBOS+ii) = theApp.bndMgr->GetBond( sym );
		
		// update rateSens
	*(def_IDs+ii) = *(pBOS+ii) == NULL  ?  -1  :  (*(pBOS+ii))->getDef_ID();

		// if the appropriate check button is ckecked, plot the current Bond/Option
	instFptsValid[ii] = false;
	CalcInstFpts( ii );					// always calculate the points
	Plot();
}			// KillFocusSymbolWorkCore()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::KillFocusSymbolWork(
	CSeekComboBox*	scb,
	short			ii,
	CString&		bndSym								)
{	CString			sym;
	m_activeComboBox = NULL;				// HotKeys
//	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	scb->GetWindowText( sym );
	if ( sym == bndSym )
		return;								// nothing to do
	bndSym = sym;

	KillFocusSymbolWorkCore( ii, bndSym );
}			// KillFocusSymbolWork()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnKillFocusSymbol1()
{
	KillFocusSymbolWork( &c_Symbol1, 0, m_Symbol1 );
}			// OnKillFocusSymbol1()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnKillFocusSymbol2()
{
	KillFocusSymbolWork( &c_Symbol2, 1, m_Symbol2 );
}			// OnKillFocusSymbol2()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnKillFocusSymbol3()
{
	KillFocusSymbolWork( &c_Symbol3, 2, m_Symbol3 );
}			// OnKillFocusSymbol3()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnKillFocusSymbol4()
{
	KillFocusSymbolWork( &c_Symbol4, 3, m_Symbol4 );
}			// OnKillFocusSymbol4()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::CheckWork( CButton* chkButton, unsigned short ii )
{
	int		status = chkButton->GetCheck();
	bool*	chkState = rateSens.bndChk;
	if (							status == BST_INDETERMINATE
		 ||	 (  *(chkState+ii)  &&  status == BST_CHECKED		)
		 ||	 ( !*(chkState+ii)  &&  status == BST_UNCHECKED		) )
		return;					// nothing to do

	*(chkState+ii) = ( status == BST_CHECKED );
	if ( ! instFptsValid[ii] )
		CalcInstFpts( ii );
	Plot();					// should already have the instFpts at this point
}			// CheckWork()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnCheck1()
{
	CheckWork( &c_Check1, 0 );
}			// OnCheck1()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnCheck2()
{
	CheckWork( &c_Check2, 1 );
}			// OnCheck2()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnCheck3()
{
	CheckWork( &c_Check3, 2 );
}			// OnCheck3()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnCheck4()
{
	CheckWork( &c_Check4, 3 );
}			// OnCheck4()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnPortfolio()
{		// status could be BST_CHECKED, BST_UNCHECKED, or BST_INDETERMINATE
	int	status = c_Portfolio.GetCheck();
	if (						status == BST_INDETERMINATE
		 ||	 (  bPortfolio  &&  status == BST_CHECKED		    )
		 ||  ( !bPortfolio  &&  status == BST_UNCHECKED	  ) )
		return;					// nothing to do

		// recapture the state of the portfolio check button
	bPortfolio = ( status == BST_CHECKED );		// polymorphic control

		// track desired state for recall when switching from/to Bond to/from Option 
	rateSens.bndPortf = bPortfolio;				// persistent
	if ( ! folderFptsValid )
		CalcFolderFpts();
	Plot();					// should already have the folderFpts
}			// OnPortfolio()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::RecalcAll( void )
{
	for ( unsigned short ii = 0; ii < nBOS; ii++ )
	{	instFptsValid[ii] = false;
		CalcInstFpts( ii );				// if okay, set instFptsValid[ii] here
	}
	folderFptsValid = false;
	CalcFolderFpts();					// if okay, set folderFptsValid here
}			// RecalcAll()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSetFocusDeltaHigh( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusDeltaHigh()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSetFocusDeltaLow( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusDeltaLow()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnKillFocusDeltaHigh( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_DeltaHigh );
	if ( ff == bottomRight.x )
		return;
	bottomRight.x = ff;

		// the domain X-extents are provided by the user (in percent)
	float	range = bottomRight.x - topLeft.x;
	if ( fabs(range) < 2.0f )							// i.e. that the range is at least 2.0 ?
		bottomRight.x = topLeft.x + 2.0f;				// force a minimum X range of 2.0 (%)
	setEditBox( "%g", bottomRight.x, IDC_DeltaHigh );

	RecalcAll();
	Plot();
}			// OnKillFocusDeltaHigh()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnKillFocusDeltaLow( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = (float)fabs( getEditBoxFloat(IDC_DeltaLow) );
	if ( ff == -topLeft.x )
		return;
	topLeft.x = -ff;
	setEditBox( "%g", -topLeft.x, IDC_DeltaLow );

	RecalcAll();
	Plot();
}			// OnKillFocusDeltaLow()
//----------------------------------------------------------------------------
/*
void	CRateSensitivityExplorer::OnKillFocusSymbol1()
{	CString sym;
	c_Symbol1.GetWindowText( sym );
	if ( sym == m_Symbol1 )
		return;								// nothing to do
	m_Symbol1 = sym;
	KillFocusSymbolWork( 0, m_Symbol1 );
}			// OnKillFocusSymbol1()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnKillFocusSymbol2()
{	CString sym;
	c_Symbol2.GetWindowText( sym );
	if ( sym == m_Symbol2 )
		return;								// nothing to do
	m_Symbol2 = sym;
	KillFocusSymbolWork( 1, m_Symbol2 );
}			// OnKillFocusSymbol2()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnKillFocusSymbol3()
{	CString sym;
	c_Symbol3.GetWindowText( sym );
	if ( sym == m_Symbol3 )
		return;								// nothing to do
	m_Symbol3 = sym;
	KillFocusSymbolWork( 2, m_Symbol3 );
}			// OnKillFocusSymbol3()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnKillFocusSymbol4()
{	CString sym;
	c_Symbol4.GetWindowText( sym );
	if ( sym == m_Symbol4 )
		return;								// nothing to do
	m_Symbol4 = sym;
	KillFocusSymbolWork( 3, m_Symbol4 );
}			// OnKillFocusSymbol4()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSelChangeSymbol1()
{	CString sym;
	int ii = c_Symbol1.GetCurSel();
	c_Symbol1.GetLBText( ii, sym );
	if ( sym == m_Symbol1 )
		return;								// nothing to do
	m_Symbol1 = sym;
	c_Symbol1.SetWindowText( m_Symbol1 );
	KillFocusSymbolWork( 0, m_Symbol1 );
}			// OnSelChangeSymbol1()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSelChangeSymbol2()
{	CString sym;
	int ii = c_Symbol2.GetCurSel();
	c_Symbol2.GetLBText( ii, sym );
	if ( sym == m_Symbol2 )
		return;								// nothing to do
	m_Symbol2 = sym;
	c_Symbol2.SetWindowText( m_Symbol2 );
	KillFocusSymbolWork( 1, m_Symbol2 );
}			// OnSelChangeSymbol2()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSelChangeSymbol3()
{	CString sym;
	int ii = c_Symbol3.GetCurSel();
	c_Symbol3.GetLBText( ii, sym );
	if ( sym == m_Symbol3 )
		return;								// nothing to do
	m_Symbol3 = sym;
	c_Symbol3.SetWindowText( m_Symbol3 );
	KillFocusSymbolWork( 2, m_Symbol3 );
}			// OnSelChangeSymbol3()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSelChangeSymbol4()
{	CString sym;
	int ii = c_Symbol4.GetCurSel();
	c_Symbol4.GetLBText( ii, sym );
	if ( sym == m_Symbol4 )
		return;								// nothing to do
	m_Symbol4 = sym;
	c_Symbol4.SetWindowText( m_Symbol4 );
	KillFocusSymbolWork( 3, m_Symbol4 );
}			// OnSelChangeSymbol4()
*/
//----------------------------------------------------------------------------
