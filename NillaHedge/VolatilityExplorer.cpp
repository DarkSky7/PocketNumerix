// VolatilityExplorer.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "VolatilityExplorer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Option.h"
#include "OptionManager.h"
#include "RegistryManager.h"
#include "FPoint.h"

/////////////////////////////////////////////////////////////////////////////
// CVolatilityExplorer dialog

CVolatilityExplorer::CVolatilityExplorer( CWnd* pParent /*=NULL*/ )
	: CPlotOptionsDialog( CVolatilityExplorer::IDD, pParent )
	, nIndexedIssues( 0 )
	, folderFptsValid( false )
	, m_activeComboBox( NULL )
{
	//{{AFX_DATA_INIT(CVolatilityExplorer)
	m_RiskFreeRate = 3.0f;
	//}}AFX_DATA_INIT

	volExpState.evalDate = COleDateTime::GetCurrentTime();
	copyrightOffset[0] =  5;			// All rights reserved
	copyrightOffset[1] =  3;			// PocketNumerix
	copyrightAxisRelative = false;		// so offsets represent number of half grid lines from bottom of plot
	topLeft.x = (float)0.01;			// we never change these here
	bottomRight.x = 2.5;				// plot points in the code

		// registered (saved) state
	for ( short jj = 0; jj < nBOS; jj++ )
	{	volExpState.optChk[jj] = true;
		volExpState.optDef_ID[jj] = -1;
		instFptsValid[jj] = false;
	}
	volExpState.pureBS = false;
	volExpState.optPortf = false;
}

CVolatilityExplorer::~CVolatilityExplorer( void	)
{
//	CPlotOptionsDialog::~CPlotOptionsDialog();
}

void	CVolatilityExplorer::DoDataExchange(CDataExchange* pDX)
{
	CPlotOptionsDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVolatilityExplorer)
	DDX_Control(pDX, IDC_PureBS, c_PureBS);
	DDX_Control(pDX, IDC_EvalDate, c_EvalDate);
	DDX_Control(pDX, IDC_Portfolio, c_Portfolio);
	DDX_Control(pDX, IDC_Symbol1, c_Symbol1);
	DDX_Control(pDX, IDC_Symbol2, c_Symbol2);
	DDX_Control(pDX, IDC_Symbol3, c_Symbol3);
	DDX_Control(pDX, IDC_Symbol4, c_Symbol4);
	DDX_Control(pDX, IDC_Check1, c_Check1);
	DDX_Control(pDX, IDC_Check2, c_Check2);
	DDX_Control(pDX, IDC_Check3, c_Check3);
	DDX_Control(pDX, IDC_Check4, c_Check4);
	DDX_Text(pDX, IDC_RiskFreeRate, m_RiskFreeRate);
	DDV_MinMaxFloat(pDX, m_RiskFreeRate, 1.e-003f, 9999.f);
	DDX_CBString(pDX, IDC_Symbol1, m_Symbol1);
	DDV_MaxChars(pDX, m_Symbol1, 254);
	DDX_CBString(pDX, IDC_Symbol2, m_Symbol2);
	DDV_MaxChars(pDX, m_Symbol2, 254);
	DDX_CBString(pDX, IDC_Symbol3, m_Symbol3);
	DDV_MaxChars(pDX, m_Symbol3, 254);
	DDX_CBString(pDX, IDC_Symbol4, m_Symbol4);
	DDV_MaxChars(pDX, m_Symbol4, 254);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVolatilityExplorer, CPlotOptionsDialog)
	//{{AFX_MSG_MAP(CVolatilityExplorer)
	ON_BN_CLICKED(IDC_Check1, OnCheck1)
	ON_BN_CLICKED(IDC_Check2, OnCheck2)
	ON_BN_CLICKED(IDC_Check3, OnCheck3)
	ON_BN_CLICKED(IDC_Check4, OnCheck4)
	ON_EN_KILLFOCUS(IDC_RiskFreeRate, OnKillFocusRiskFreeRate)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_Portfolio, OnPortfolio)
	ON_NOTIFY(DTN_CLOSEUP, IDC_EvalDate, OnCloseUpEvalDate)
	ON_NOTIFY(NM_KILLFOCUS, IDC_EvalDate, OnKillFocusEvalDate)
	ON_BN_CLICKED(IDC_PureBS, OnPureBS)
	ON_WM_CTLCOLOR()

	ON_CBN_SELCHANGE(IDC_Symbol1, OnSelChangeSymbol1)
	ON_CBN_SELCHANGE(IDC_Symbol2, OnSelChangeSymbol2)
	ON_CBN_SELCHANGE(IDC_Symbol3, OnSelChangeSymbol3)
	ON_CBN_SELCHANGE(IDC_Symbol4, OnSelChangeSymbol4)
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

	ON_EN_SETFOCUS(IDC_RiskFreeRate, OnSetFocusRiskFreeRate)
	ON_NOTIFY(NM_SETFOCUS, IDC_EvalDate, OnSetFocusEvalDate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVolatilityExplorer message handlers

BOOL CVolatilityExplorer::OnInitDialog( void )
{
		// set the extents of plot region (among other initialization)
	CPlotOptionsDialog ::OnInitDialog();
	RegisterHotKeys( IDC_Symbol1, IDC_Symbol2 );				// screens for WM5 internally
	plotExtents.top = 104;			// leaves enough room for the controls at the top

		// get the risk free rate from the registry
	if ( ! GetRiskFreeRate(&m_RiskFreeRate) )				// was:  theApp.regMgr->
	{
#ifdef _DEBUG
		TRACE( _T("VolatilityExplorer::OnInitDialog: GetRiskFreeRate failed, using m_RiskFreeRate=%g.\n"),
			m_RiskFreeRate );
#endif
	}
	setEditBox( "%.3f", m_RiskFreeRate, IDC_RiskFreeRate );

		// this dialog global helps us decide whether to enable the portfolio check button
	nIndexedIssues = theApp.optMgr->NumIndexedIssues();		// numOptionsWithPositions
	c_Portfolio.EnableWindow( nIndexedIssues > 0 );

		// see if there's a registered VolatilityExplorerState structure
	if ( ! GetVolatilityExplorerState(&volExpState) )		// was:  theApp.regMgr->
	{
#ifdef _DEBUG
		TRACE( _T("VolatilityExplorer::OnInitDialog: GetVolatilityExplorerState failed.\n") );
#endif
	}
	COleDateTime		today = COleDateTime::GetCurrentTime();
	COleDateTimeSpan	fourYears( 365 * 4, 0, 0, 0 );
	COleDateTime		highTime = today + fourYears;
	c_EvalDate.SetRange( &today, &highTime );					// limit the range of the EndDate control
	c_EvalDate.SetTime( volExpState.evalDate );

		// check the selected instrument and choose a CDataManager
	c_PureBS.SetCheck( volExpState.pureBS  ?  BST_CHECKED  :  BST_UNCHECKED );
	SetCheckButtons();					// the five plot enablers from timeDecay	
	LoadBOSs( volExpState.optDef_ID );	// fetch options recently evaluated
	LoadSymbols();						// load ComboBoxes & select items matching the current pOptions
	RecalcAll();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//----------------------------------------------------------------------------
HBRUSH	CVolatilityExplorer::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	return	CPlotOptionsDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}			// OnCtlColor()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnPaint( void )
{
	unsigned short ii = 0;
#ifdef _DEBUG
	TRACE( _T("VolatilityExplorer::OnPaint(1): begin\n") );
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
	{	if ( InstValid(ii, volExpState.optChk) )
		{
#ifdef _DEBUG
	TRACE( _T("VolatilityExplorer::OnPaint(2): %d inst curve is valid and enabled\n"), ii );
#endif
			if ( instMinY[ii] < bottomRight.y )
				bottomRight.y = instMinY[ii];
			if ( instMaxY[ii] > topLeft.y )
				topLeft.y = instMaxY[ii];
		}
	}
		// the portfolio curve
	if ( volExpState.optPortf  &&  folderFptsValid )
	{	if ( folderMinY < bottomRight.y )
			bottomRight.y = folderMinY;
		if ( folderMaxY > topLeft.y )
			topLeft.y = folderMaxY;
	}
		// bail out criteria
	if ( topLeft.y == -2e38f  ||  bottomRight.y == 2e38f )
		return;

		// we always want the X-axis visible in TDX plots
	if ( fabs(topLeft.y) < 1e-2  &&  fabs(bottomRight.y) < 1e-2 )
	{	topLeft.y = 0.0;
		bottomRight.y = -100.0;
	}
	double	yRange = topLeft.y - bottomRight.y;						// percent
	topLeft.y = (float)(topLeft.y + 0.08 * yRange);
	bottomRight.y = (float)(bottomRight.y - 0.12 * yRange);
#ifdef _DEBUG
	TRACE( _T("VolatilityExplorer::OnPaint(3): topLeft.x=%g, topLeft.y=%g, bottomRight.x=%g, bottomRight.y=%g\n"),
		topLeft.x, topLeft.y, bottomRight.x, bottomRight.y );
#endif

		// scale against available space and plot
	xScale = (float)(plotExtents.Width()  / (bottomRight.x - topLeft.x));
	yScale = (float)(plotExtents.Height() / (topLeft.y - bottomRight.y));
#ifdef _DEBUG
		TRACE( _T("VolatilityExplorer::OnPaint(4): xScale=%g, yScale=%g\n"), xScale, yScale );
#endif
		// paint time
	DrawGrid( dc );
	for ( ii = 0; ii < nBOS; ii++ )
	{	if ( InstValid(ii, volExpState.optChk)  &&  instFptsValid[ii] )				// check button state examined by InstValid()
			DrawPolyline( ii, dc, (instFpts + ii*nInstVolPts), nInstVolPts );
	}
		// plot the portfolio line
	if ( volExpState.optPortf  &&  folderFptsValid )				// portfolio plotting is enabled & the points are valid
		DrawPolyline( nBOS, dc, folderFpts, nFolderVolPts );		// nBOS --> black

	DrawGridLabels( dc );
}			// OnPaint()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::CalcInstFpts( unsigned short ii )
{		// the domain X-extents are simply from today (x=0.0) to timeDecay.endDate (x=years after today)
	if ( ! InstValid( ii, volExpState.optChk ) )
		return;
	CWaitCursor	wait;						// we could be gone a while - display the wait cursor
	double minXval = topLeft.x;
	double maxXval = bottomRight.x;
	double xDispInc = (maxXval - minXval) / (nInstVolPts - 1);		// ratios w.r.t. underlying volatility
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// compute the issue plot points and the domain Y-extents
	COption* opt = (COption*) *(pBOS+ii);
	float refSigma = opt->getVolatility();
	evcs.sigma = refSigma;									// <-- we'll vary sigma within the loop
	evcs.calcYrsToExpiry = true;
	evcs.calcPvDivs = true;									// every initial euroValue call will reset this
	evcs.fromDate = volExpState.evalDate;
	evcs.riskFreeRate = m_RiskFreeRate / 100.0;				// the reference RFR that the option's value is based on
	evcs.stockPrice = opt->getStockPrice();
		// evcs.fromDate is initialized in the constructor
	double optRefVal;
	if ( ! opt->EuroValue( evcs, optRefVal ) )
		return;
	double refPrice = volExpState.pureBS  ?  optRefVal  :  opt->getMktPrice();
		// collect Y-values (instFpts[ii*nTimeBOS+0] is always (0.0, 0.0) in pureBS mode
	instMinY[ii] =  2e38f;									// dialog var
	instMaxY[ii] = -2e38f;									// dialog var
	double xDispVal = minXval;								// displayed X-value
	CFPoint* instPts = instFpts + ii * nInstVolPts;
	for ( short jj = 0; jj < nInstVolPts; jj++ )			// for each of the FPoints
	{	double yVal = 1.0;									// in case the if condition is false
		if ( fabs(refPrice) >= 1e-3 )						// avoid divide by zero
		{		// convert Black-Scholes value to a fraction of the Option's referece value
			evcs.sigma = xDispVal * refSigma;				// the internal x-value
			double bsVal;
			if ( ! opt->EuroValue( evcs, bsVal ) )
				return;
			yVal = bsVal / refPrice;						// result is a ratio relative to reference value
		}
#ifdef _DEBUG
//		TRACE( _T("VolatilityExplorer::OnPaint: jj=%d, yVal=%g\n"), jj, yVal );
#endif
			// update min and max Y-values
		if ( yVal < instMinY[ii] )
			instMinY[ii] = (float)yVal;
		if ( yVal > instMaxY[ii] )
			instMaxY[ii] = (float)yVal;

		CFPoint* cfp = instPts + jj;
		cfp->x = (float)xDispVal;
		cfp->y = (float)yVal;						// the computed Y-value (also a percentage)
#ifdef _DEBUG
//		TRACE( _T("VolatilityExplorer::CalcInstFpts: cfp->x=%g, cfp->y=%g\n"), cfp->x, cfp->y );
#endif
			// prepare for the next point
		xDispVal += xDispInc;						// move the fractional (displayed) value forward
	}
	instFptsValid[ii] = true;
#ifdef _DEBUG
	TRACE( _T("VolatilityExplorer::CalcInstFpts: instFptsValid[%d]=%s\n"), ii, ( instFptsValid[ii] ? _T("true") : _T("false") ) );
#endif
}			// CalcInstFpts()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::CalcFolderFpts( void )
{	int		ii, jj;
	double	yVal;
		// compute the folder's rate sensitivity
		// overwrite either optionFolderPts with results (instrument decides which)
	CWaitCursor	wait;						// we could be gone a while - display the wait cursor
	double minXval = topLeft.x;
	double maxXval = bottomRight.x;
	double xDispInc = (maxXval - minXval) / (nFolderVolPts - 1);	// ratios w.r.t. underlying volatility

		// initialize the portfolio points
	double	cfpY[ nFolderVolPts ];
	double	xDispVal = minXval;									// starting X-value (modified in the for loop)
	for ( jj = 0; jj < nFolderVolPts; jj++ )
	{	cfpY[jj] = 0.0;											// working array contributes to folderFpts below
		(folderFpts+jj)->x = (float)xDispVal;					// dialog instance variable result
		xDispVal += xDispInc;									// ratio x-values
	}
	evcs.fromDate = volExpState.evalDate;
	evcs.riskFreeRate = m_RiskFreeRate / 100.0;		// the reference RFR that the option's value is based on
	double folderCost = 0.0;									// all option positions
	COption* opt;
	folderMinY =  2e38f;									// dialog var
	folderMaxY = -2e38f;									// dialog var
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// for each issue with positions
	for ( ii = 0; ii < nIndexedIssues; ii++ )
	{	long	def_ID = theApp.optMgr->GetPosIndexedDef_ID( ii );
			// set up for Option evaluation and subsequent rescaling (for options)
		opt = ( def_ID < 0 )  ?  NULL  :  theApp.optMgr->ReadOption( def_ID );
		if ( opt == NULL )
		{
#ifdef _DEBUG
			TRACE( _T("RateSensitivityExplorer::CalcFolderFpts: NULL option encountered.\n") );
			AfxDebugBreak();
#endif
			continue;
		}
		CMapPtrToPtr*	posLst = opt->getPositionList();
		if ( posLst == NULL )
		{
#ifdef _DEBUG
			TRACE( _T("RateSensitivityExplorer::CalcFolderFpts: %s has no position list\n"), opt->getSymbol() );
			AfxDebugBreak();
#endif
			delete	opt;	// here we're pulling options out of the database, not hanging on to them in the dialog
			continue;
		}
#ifdef _DEBUG
		TRACE( _T("RateSensitivityExplorer::CalcFolderFpts: Processing position(s) for %s\n"), opt->getSymbol() );
#endif
			// compute the option's reference Y-value
			// set up the Black-Scholes evaluation context for Harvey's PV(dividends) model
		float refSigma = opt->getVolatility();
		evcs.sigma = refSigma;							// save refSigma and manipulate evcs.sigma
		evcs.stockPrice = opt->getStockPrice();
		evcs.calcYrsToExpiry = true;
		evcs.calcPvDivs = true;							// every initial euroValue call will reset this
		double optRefVal;
		if ( ! opt->EuroValue( evcs, optRefVal ) )		// pvDivs approach
			return;
		double refPrice = volExpState.pureBS  ?  optRefVal  :  opt->getMktPrice();
#ifdef _DEBUG
//		TRACE( _T("RateSensitivityExplorer::CalcFolderFpts: %g <- %s(stkPrc=%g, sig=%g, refYrs=%g, rfr=%g, pvDivs=%g)\n"),
//			optRefVal, opt->getSymbol(), stkPrice, sigma, refYrs, rfr, pvDivs );
#endif
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// compute the weighting factor (issueBasis) for this issue
		double	issueBasis = 0.0;							// sum of totalCost across all of opt's positions
		long	posOffset = opt->openPosOffset;
		CPosition*	pos;
		while ( posLst->Lookup( (void*)posOffset, (void*&)pos ) )
		{		// process this position
			issueBasis += pos->totalCost;
				// prepare for next loop pass
			posOffset = pos->prevPos_ID;
		}													// for each position
#ifdef _DEBUG
		TRACE( _T("RateSensitivityExplorer::CalcFolderFpts: %s has basis=%.2f\n"), opt->getSymbol(), issueBasis );
#endif
			// update the Y-values (optionFolderPts[0] is (0.0, 0.0 ) by definition
		double xVal = minXval * refSigma;					// reset x to beginning of range
		double xInc = refSigma * (maxXval - minXval) / (nFolderVolPts - 1);
		for ( jj = 0; jj < nFolderVolPts; jj++ )			// for each of the FPoints
		{		// convert Black-Scholes value to a percent change from the Option's reference value
			yVal = 1.0;							// in case the if condition is false
			if ( fabs(refPrice) > 1e-3 )
			{	evcs.sigma = xVal;
				double bsVal;
				if ( ! opt->EuroValue( evcs, bsVal ) )
					return;
				yVal = issueBasis * bsVal / refPrice;
			}
				// upscale the Y-value by the total cost (issueBasis) of the issue's positions 
			*(cfpY+jj) += yVal;					// update the Y-value for this position

				// prepare for next pass
			xVal += xInc;
		}										// for each point
		folderCost += issueBasis;				// this will be our descaling factor
	}											// for each issue
#ifdef _DEBUG
	TRACE( _T("RateSensitivityExplorer::CalcFolderFpts: %d options have aggregate basis=%.2f\n"),
		nIndexedIssues, folderCost );
#endif

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// post issue-loop downscaling by folderCost, simultaneously upscaling to percent
	folderMinY = 1e38f;								// a big positive number
	folderMaxY = -1e38f;							// a big negative number
	for ( jj = 0; jj < nFolderVolPts; jj++ )
	{		// rescale the yVal from a fractional value to percent
		yVal = (fabs(folderCost) < 0.001)  ?  0.0  :  (cfpY[jj] / folderCost);
		if ( yVal < folderMinY )
			folderMinY = (float)yVal;				// percent
		if ( yVal > folderMaxY )
			folderMaxY = (float)yVal;				// percent
		(folderFpts + jj)->y = (float)yVal;			// the rescaled Y-value (cost weighted, percent change)
	}
	folderFptsValid = true;
}			// CalcFolderFpts()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::SetCheckButtons( void ) 
{
	bool*	chkBtns = volExpState.optChk;
	CPlotOptionsDialog::SetCheckButtons( chkBtns );

		// set Option and Portfolio buttons
	volExpState.optPortf = volExpState.optPortf  &&  ( nIndexedIssues > 0 );
	c_Portfolio.EnableWindow( nIndexedIssues > 0 );

	c_Portfolio.SetCheck( volExpState.optPortf  ?  BST_CHECKED  :  BST_UNCHECKED );
}			// SetCheckButtons()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
LRESULT		CVolatilityExplorer::OnHotKey( WPARAM wParam, LPARAM lParam )
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
void	CVolatilityExplorer::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	CNillaDialog::OnActivate( nState, pWndOther, bMinimized );
	if ( nState == WA_ACTIVE || nState == WA_CLICKACTIVE )
		RegisterHotKeys( IDC_Symbol1, IDC_Symbol2 );				// screens for WM5 internally
}			// OnActivate()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void	CVolatilityExplorer::SymbolWorkCore( short ii, CString sym, long* def_ID )
{		// ii is the zero-based index of the option

		// kill any Option at this index
	if ( *(pBOS+ii) )
		delete	*(pBOS+ii);

	*(pBOS+ii) = (CBOS*)theApp.optMgr->GetOption( sym );
		
		// update the dialog's persistent state
	*(def_ID+ii) = ( *(pBOS+ii) == NULL )  ?  -1  :  (*(pBOS+ii))->getDef_ID();

	instFptsValid[ii] = false;
	CalcInstFpts( ii );
		// if the check button is checked, a repaint is called for
	if (	( ii == 0  &&  c_Check1.GetCheck() == BST_CHECKED )
		 || ( ii == 1  &&  c_Check2.GetCheck() == BST_CHECKED )
		 || ( ii == 2  &&  c_Check3.GetCheck() == BST_CHECKED )
		 || ( ii == 3  &&  c_Check4.GetCheck() == BST_CHECKED ) )
		Plot();
}			// SymbolWorkCore()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::KillFocusSymbolWork(
	CSeekComboBox*	scb,
	short			ii,
	CString&		optSym						)
{	CString sym;
	m_activeComboBox = NULL;
	scb->GetWindowText( sym );
	sym.MakeUpper();
	if ( sym == optSym )
		return;								// nothing to do
	optSym = sym;
	SymbolWorkCore( ii, optSym, volExpState.optDef_ID );
}			// KillFocusSymbolWork()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnKillFocusSymbol1( void )
{
	KillFocusSymbolWork( &c_Symbol1, 0, m_Symbol1 );
}			// OnKillFocusSymbol1()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnKillFocusSymbol2( void )
{
	KillFocusSymbolWork( &c_Symbol2, 1, m_Symbol2 );
}			// OnKillFocusSymbol2()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnKillFocusSymbol3( void )
{
	KillFocusSymbolWork( &c_Symbol3, 2, m_Symbol3 );
}			// OnKillFocusSymbol3()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnKillFocusSymbol4( void )
{
	KillFocusSymbolWork( &c_Symbol4, 3, m_Symbol4 );
}			// OnKillFocusSymbol4()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnSetFocusSymbol1( void )
{
	m_activeComboBox = IDC_Symbol1;
}			// OnSetFocusSymbol1()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnSetFocusSymbol2( void )
{
	m_activeComboBox = IDC_Symbol2;
}			// OnSetFocusSymbol2()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnSetFocusSymbol3( void )
{
	m_activeComboBox = IDC_Symbol3;
}			// OnSetFocusSymbol3()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnSetFocusSymbol4( void )
{
	m_activeComboBox = IDC_Symbol4;
}			// OnSetFocusSymbol4()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void	CVolatilityExplorer::SelChangeSymbolWork(
	CSeekComboBox*	scb,
	short			ii,
	CString&		optSym							)
{	CString sym;
	int cnt = scb->GetCount();
	if ( cnt < 1 )
		return;
	int sel = scb->GetCurSel();
#if ( _WIN32_WCE >= 0x420 )					// just WM5
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
	scb->GetLBText( sel, sym );
	if ( sym == optSym )
		return;								// nothing to do
	optSym = sym;
	SymbolWorkCore( ii, optSym, volExpState.optDef_ID );
}			// SelChangeSymbolWork
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnSelChangeSymbol1() 
{
	SelChangeSymbolWork( &c_Symbol1, 0, m_Symbol1 );
}			// OnSelChangeSymbol1()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnSelChangeSymbol2() 
{
	SelChangeSymbolWork( &c_Symbol2, 1, m_Symbol2 );
}			// OnSelChangeSymbol2()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnSelChangeSymbol3() 
{
	SelChangeSymbolWork( &c_Symbol3, 2, m_Symbol3 );
}			// OnSelChangeSymbol3()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnSelChangeSymbol4() 
{
	SelChangeSymbolWork( &c_Symbol4, 3, m_Symbol4 );
}			// OnSelChangeSymbol4()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::RecalcAll( void )
{
	for ( unsigned short ii = 0; ii < nBOS; ii++ )
	{	instFptsValid[ii] = false;
		CalcInstFpts( ii );				// if okay, set instFptsValid[ii] here
	}
	folderFptsValid = false;
	CalcFolderFpts();					// if okay, set folderFptsValid here
}			// RecalcAll()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void	CVolatilityExplorer::KillFocusEvalDateWork( NMHDR* pNMHDR, LRESULT* pResult )
{
	COleDateTime	aDate;
	c_EvalDate.GetTime( aDate );
	if ( volExpState.evalDate == aDate )
		return;
	volExpState.evalDate = aDate;
	RecalcAll();
	Plot();
	*pResult = 0;
}			// KillFocusEvalDateWork()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnCloseUpEvalDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	KillFocusEvalDateWork( pNMHDR, pResult );
}			// OnCloseUpEvalDate()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnKillFocusEvalDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	KillFocusEvalDateWork( pNMHDR, pResult );
}			// OnKillFocusEvalDate()
//----------------------------------------------------------------------------
void CVolatilityExplorer::OnSetFocusEvalDate(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	*pResult = 0;
}			// OnSetFocusEvalDate()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnKillFocusRiskFreeRate( void	)
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_RiskFreeRate );
	if ( ff == m_RiskFreeRate )
		return;

	m_RiskFreeRate = (float)fabs(ff);
	setEditBox( "%g", m_RiskFreeRate, IDC_RiskFreeRate );
	RecalcAll();
	Plot();
}			// OnKillFocusRiskFreeRate()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnSetFocusRiskFreeRate( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusRiskFreeRate()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::CheckWork( CButton* chkButton, unsigned short ii )
{
	int		status = chkButton->GetCheck();
	bool*	chkState = volExpState.optChk;
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
void	CVolatilityExplorer::OnCheck1( void )
{
	CheckWork( &c_Check1, 0 );
}			// OnCheck1()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnCheck2( void )
{
	CheckWork( &c_Check2, 1 );
}			// OnCheck2()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnCheck3( void )
{
	CheckWork( &c_Check3, 2 );
}			// OnCheck3()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnCheck4( void )
{
	CheckWork( &c_Check4, 3 );
}			// OnCheck4()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnPortfolio( void )
{		// status could be BST_CHECKED, BST_UNCHECKED, or BST_INDETERMINATE
	int	status = c_Portfolio.GetCheck();
	if (								  status == BST_INDETERMINATE
		 ||	 (  volExpState.optPortf  &&  status == BST_CHECKED			)
		 ||  ( !volExpState.optPortf  &&  status == BST_UNCHECKED		) )
		return;					// nothing to do
		// recapture the state of the portfolio check button
	volExpState.optPortf = ( status == BST_CHECKED );
	if ( ! folderFptsValid )
		CalcFolderFpts();
	Plot();
}			// OnPortfolio()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnPureBS( void )
{		// status could be BST_CHECKED, BST_UNCHECKED, or BST_INDETERMINATE
	int	status = c_PureBS.GetCheck();
	if (							    status == BST_INDETERMINATE
		 ||	 (  volExpState.pureBS  &&  status == BST_CHECKED		  )
		 ||  ( !volExpState.pureBS  &&  status == BST_UNCHECKED		  ) )
		return;					// nothing to do
	volExpState.pureBS = (status == BST_CHECKED );
	RecalcAll();
	Plot();
}			// OnPureBS()
//----------------------------------------------------------------------------
/*
void	CVolatilityExplorer::CalcFolderFpts( void )
{		// compute the folder's rate sensitivity
		// overwrite either optionFolderPts with results (instrument decides which)
	COption*	opt;
	double		folderCost = 0.0;						// all option positions

		// initialize the portfolio points
	int		ii, jj;
	double	xVal = minXval;									// copy the lowest X-value
	double	xInc = ( maxXval - minXval ) / (nFolderVolPts - 1);
	double	cfpY[ nFolderVolPts ];
	for ( jj = 0; jj < nFolderVolPts; jj++ )
	{
		cfp = folderFpts + jj;
		cfp->x = (float)xVal;					// we could easily do this later, but
		cfpY[jj] = 0.0;							// the loop is really here to initialize cfpY
		xVal += xInc;
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// for each issue with positions
	double	yVal;
	double	optRefVal;
	evcs.riskFreeRate = m_RiskFreeRate / 100.0;
	evcs.fromDate = volExpState.evalDate;				// fromDate doesn't change inside the loop
	for ( ii = 0; ii < nIndexedIssues; ii++ )
	{	long	def_ID = theApp.optMgr->GetPosIndexedDef_ID( ii );
			// set up for Option evaluation and subsequent rescaling (for options)
		opt = ( def_ID < 0 )  ?  NULL  :  theApp.optMgr->ReadOption( def_ID );
		if ( opt == NULL )
		{
#ifdef _DEBUG
			TRACE( _T("VolatilityExplorer::CalcFolderPts: NULL option encountered.\n") );
			AfxDebugBreak();
#endif
			continue;
		}
		CMapPtrToPtr*	posLst = opt->getPositionList();
		if ( posLst == NULL )
		{
#ifdef _DEBUG
			TRACE( _T("VolatilityExplorer::CalcFolderPts: %s has no position list\n"), opt->getSymbol() );
			AfxDebugBreak();
#endif
			delete	opt;
			continue;
		}

		// compute the option's reference Y-value
//		refYrs = opt->YrsToExpiry( volExpState.evalDate );
		evcs.calcPvDivs = true;							// every initial euroValue call will reset this
		evcs.calcYrsToExpiry = true;
		evcs.stockPrice = opt->getStockPrice();
		evcs.sigma = opt->getVolatility();
		optRefVal = opt->EuroValue( evcs );			// stkPrice, sigma, refYrs, rfr, pvDivs, calcPvDivs
		double	refPrice = volExpState.pureBS  ?  optRefVal  :  opt->getMktPrice();
#ifdef _DEBUG
//		TRACE( _T("VolatilityExplorer::CalcFolderPts: %g <- %s(stkPrc=%g, sig=%g, refYrs=%g, rfr=%g, pvDivs=%g)\n"),
//			optRefVal, opt->getSymbol(), stkPrice, sigma, refYrs, rfr, pvDivs );
#endif

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// compute the weighting factor (issueBasis) for this issue
		long	posOffset = opt->openPosOffset;
		double	issueBasis = 0.0;							// sum of totalCost across all of bos's positions
		CPosition*	pos;
		while ( posLst->Lookup( (void*)posOffset, (void*&)pos ) )
		{		// process this position
			issueBasis += pos->totalCost;
				// prepare for next loop pass
			posOffset = pos->prevPos_ID;
		}													// for each position
#ifdef _DEBUG
//		TRACE( _T("VolatilityExplorer::CalcFolderPts: %s has basis=%.2f\n"), opt->getSymbol(), issueBasis );
#endif
		
			// update the Y-values (optionFolderPts[0] is (minXval, 0.0 )
		xVal = minXval;										// we're starting at the first non-refValue point
		for ( jj = 0; jj < nFolderVolPts; jj++ )			// for each of the FPoints
		{		// rescale xVal from percent to a fractional value before calculating Black-Scholes value
				// convert Black-Scholes value to a fraction of the Option's referece value
			yVal = 0.0;										// in case the if condition is false
			if ( fabs(optRefVal) > 1e-3 )
			{	evcs.sigma = xVal * opt->getVolatility();
				double	bsVal = opt->EuroValue( evcs );		// stkPrice, xVal * sigma, refYrs, rfr, pvDivs, calcPvDivs
				yVal = bsVal / refPrice;					// ratioed against the reference, not a difference relative to a reference
			}
			yVal *= issueBasis;								// scale Y-value up by the issue's basis (cost)

				// upscale the Y-value by the total cost (issueBasis) of the issue's positions 
			CFPoint* cfp = folderFpts + jj;
			cfp->x = (float)xVal;							// display X in its fractional form
			*(cfpY+jj) += yVal;								// update the Y-value for this position
#ifdef _DEBUG
//			TRACE( _T("VolatilityExplorer::CalcFolderPts: bsVal=%g <-(stkPrc=%g, sig=%g, yrsLeft=%g, rfr=%g, pvDivs=%g), yVal[%d]=%g, cfp->y=%g\n"),
//				bsVal, stkPrice, sigma, yrsLeft, rfr, pvDivs, jj, yVal[jj], cfp->y );
#endif
			xVal += xInc;
		}								// for each point
		folderCost += issueBasis;		// this will be our descaling factor
		delete	opt;					// we're done with this BOS
	}									// for each issue
#ifdef _DEBUG
	TRACE( _T("VolatilityExplorer::CalcFolderPts: %d options have aggregate basis=%.2f\n"),
		nIndexedIssues, folderCost );
#endif
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// post issue-loop downscaling by folderCost, simultaneously upscaling to percentages
	folderMinY =  2e38f;
	folderMaxY = -2e38f;
	for ( jj = 0; jj < nFolderVolPts; jj++ )
	{		// rescale the yVal from a fractional value to percent
//		double	syVal = (fabs(folderCost) < 0.001)  ?  0.0  :  (100.0 * cfpY[jj] / folderCost);
		double	syVal = (fabs(folderCost) < 0.001)  ?  0.0  :  (cfpY[jj] / folderCost);
		if ( syVal < folderMinY )
			folderMinY = (float)syVal;		// percent
		if ( syVal > folderMaxY )
			folderMaxY = (float)syVal;		// percent
		(folderFpts+jj)->y = (float)syVal;			// the rescaled Y-value (cost weighted, percent change)
#ifdef _DEBUG
//		TRACE( _T("VolatilityExplorer::CalcFolderPts: descaled cfp->x=%g, cfp->y=%g\n"), cfp->x, cfp->y );
#endif
	}
	folderFptsValid = true;
}			// CalcFolderPts()
//----------------------------------------------------------------------------
void	CVolatilityExplorer::OnPaint( void )
{	bool	bosValid[ nBOS ];
	bool	somethingValid = false;
	long	ii;
	long	jj;											// most of the time shorts would work
	for ( jj = 0; jj < nBOS; jj++ )
	{	*(bosValid+jj) = InstValid( jj, volExpState.optChk );
		somethingValid = somethingValid  ||  *(bosValid+jj);
	}
	somethingValid = somethingValid  ||  ( nIndexedIssues > 0 );		// positions exist

		// nothing to paint if there are no instruments to evaluate
	CPaintDC	dc( this );								// device context for painting
	DrawPlotBackground( dc );							// blanks out the plot space
	if ( ! somethingValid )
	{	CPlotOptionsDialog::OnPaint();					// solves the initially transparent background problem
		return;											// doesn't unpaint
	}
//	CWaitCursor	wait;							// we could be gone a long time - display the wait cursor, but not inside OnPaint()

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// the domain X-extents are simply from 0.25 to 2.5 (a multiple of the current volatility)
	double	minXval = 0.1;										// 1/4 of current volatility
	double	maxXval = 2.5;										// 2.5 times current volatility
	double	xInc = (maxXval - minXval) / (nInstVolPts - 1);
	topLeft.x = (float)minXval;									// percent change
	bottomRight.x = (float)maxXval;								// percent change

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// compute the issue plot points and the domain Y-extents
	double	xVal, yVal, minYval = 2e38f, maxYval = -2e38f;
	double	optRefVal;
	evcs.fromDate = volExpState.evalDate;
	evcs.riskFreeRate = m_RiskFreeRate / 100.0;
	COption*	opt;
	evcs.riskFreeRate = m_RiskFreeRate / 100.0;			// no change to risk free rate inside the loop
	for ( ii = 0; ii < nBOS; ii++ )
	{	if ( *(bosValid+ii) )
		{	xVal = minXval;								// start at the lowest X-value

				// set up for Option evaluation and subsequent rescaling operations
			opt = (COption*) *(pBOS+ii);
			evcs.calcPvDivs = true;
			evcs.calcYrsToExpiry = true;
			evcs.stockPrice = opt->getStockPrice();
			evcs.sigma = opt->getVolatility();
			optRefVal = opt->EuroValue( evcs );
			double	refPrice = volExpState.pureBS  ?  optRefVal  :  opt->getMktPrice();
				// collect Y-values (instFpts[ii*nTimeBOS+0] is always (0.0, 0.0)
//	only need these three lines if we are computing pure Black-Scholes
//			cfp = instFpts + ii*nVolPts + 0;
//			cfp->x = 0.0;
//			cfp->y = 0.0;
				// the following loop could start at jj=1 if ...
				// the previous three lines were compiled AND pureBS == true
				// its simpler to potentially waste the first computation (when pureBS is true)
			for ( jj = 0; jj < nVolPoints; jj++ )			// for each of the FPoints
			{	// rescale xVal from percent to a fractional value before calculating Black-Scholes value
				if ( fabs(optRefVal) >= 1e-3 )
				{		// convert Black-Scholes value to a fraction of the Option's referece value
					evcs.sigma = xVal * opt->getVolatility();
					double	bsVal = opt->EuroValue( evcs );
//					yVal = ( bsVal - refPrice ) / optRefVal;		// a fraction
					yVal = bsVal / refPrice;						// a multiple of the referece value
#ifdef _DEBUG
//					TRACE( _T("VolatilityExplorer::OnPaint: option(%d)=%s, bsVal=%g, optRefVal=%g, yVal=%g\n"),
//						ii, opt->getSymbol(), bsVal, optRefVal, yVal );
#endif
				}
				else
					yVal = 0.0;

					// update min and max Y-values
				if ( yVal < minYval )
					minYval = yVal;						// fractional increase

				if ( yVal > maxYval )
					maxYval = yVal;						// fractional increase

				CFPoint* cfp = instFpts + ii*nVolPoints + jj;
				cfp->x = (float)xVal;					// display X in its fractional form
				cfp->y = (float)yVal;					// display Y as a fraction
				xVal += xInc;							// prepare for the next point
			}
		}		
	}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifdef _DEBUG
//		TRACE( _T("VolatilityExplorer::OnPaint: minYval=%g, optionFolderYmin=%g, maxYval=%g, optionFolderYmax=%g\n"),
//			minYval, optionFolderYmin, maxYval, optionFolderYmax );
#endif
	optionPtsAreValid = optionPtsAreValid  &&  minYval <= optionFolderYmin  &&  maxYval >= optionFolderYmax;
	if ( volExpState.optPortf  &&  !optionPtsAreValid  &&  nIndexedIssues > 0 )
	{		// the check box is checked, there are issues with positions
			// but the portfolio plot points are not valid
			// minYval & maxYval are updated within CalcFolderPts()
		CalcFolderPts( minXval, maxXval );	// also sets optionPtsAreValid, optionFolderYmin, optionFolderYmax
		if ( optionFolderYmin < minYval )
			minYval = optionFolderYmin;
		if ( optionFolderYmax > maxYval )
			maxYval = optionFolderYmax;
	}
#ifdef _DEBUG
//		TRACE( _T("VolatilityExplorer::OnPaint: minXval=%g, xInc=%g, maxXval=%g, minYval=%g, maxYval=%g\n"),
//			minXval, xInc, maxXval, minYval, maxYval );
#endif
	double	yRange = maxYval - minYval;					// percent
	topLeft.y = (float)(maxYval + 0.08 * yRange);
	bottomRight.y = (float)(minYval - 0.12 * yRange);
#ifdef _DEBUG
//		TRACE( _T("VolatilityExplorer::OnPaint: topLeft.x=%g, topLeft.y=%g, bottomRight.x=%g, bottomRight.y=%g\n"),
//			topLeft.x, topLeft.y, bottomRight.x, bottomRight.y );
#endif

		// scale against available space and plot
	xScale = (float)(plotExtents.Width()  / (bottomRight.x - topLeft.x));		// used by MapToPlot
	yScale = (float)(plotExtents.Height() / (topLeft.y - bottomRight.y));		// ditto
#ifdef _DEBUG
		TRACE( _T("VolatilityExplorer::OnPaint: xScale=%g, yScale=%g\n"), xScale, yScale );
#endif

		// now we can paint the four Option instances
	DrawGrid( dc );
	for ( ii = 0; ii < nBOS; ii++ )
	{	if ( *(bosValid+ii) )							// check button state already examined by InstValid()
			DrawPolyline( ii, dc, (instFpts + ii*nVolPoints), nVolPoints );
	}
		// plot the portfolio line
	if ( volExpState.optPortf  &&  optionPtsAreValid )	// portfolio plotting is enabled & the points are valid
		DrawPolyline( nBOS, dc, optionFolderPts, nVolFolderPts );
	DrawGridLabels( dc );
}			// OnPaint()
*/
//----------------------------------------------------------------------------
