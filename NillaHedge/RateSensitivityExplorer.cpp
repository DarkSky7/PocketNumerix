// RateSensitivityExplorer.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "RateSensitivityExplorer.h"
#include "RateSensitivity.h"			// registry retrieval struct

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "BOS.h"
//#include "BondManager.h"
#include "OptionManager.h"
#include "RegistryManager.h"
#include "Position.h"				// for the PositionIterationContext
#include "FPoint.h"
#include "Option.h"
//#include "Bond.h"

/////////////////////////////////////////////////////////////////////////////
// CRateSensitivityExplorer dialog


CRateSensitivityExplorer::CRateSensitivityExplorer( CWnd* pParent /*=NULL*/ )
	: CPlotOptionsDialog( IDD, pParent )
	, m_activeComboBox( NULL )
	, nIndexedIssues( 0 )
	, folderFptsValid( false )
{
	//{{AFX_DATA_INIT(CRateSensitivityExplorer)
	m_DeltaHigh = 2.5f;
	m_DeltaLow = 1.5f;
	m_RiskFreeRate = 3.0f;
	//}}AFX_DATA_INIT
	evcs.fromDate = COleDateTime::GetCurrentTime();

		// registered (saved) state
	for ( short jj = 0; jj < nBOS; jj++ )
	{	rateSens.optChk[jj] = true;
		rateSens.optDef_ID[jj] = -1;
		instFptsValid[jj] = false;
	}
	rateSens.optPortf = false;
}

CRateSensitivityExplorer::~CRateSensitivityExplorer( void )
{
//	CPlotOptionsDialog::~CPlotOptionsDialog();
}

void	CRateSensitivityExplorer::DoDataExchange( CDataExchange* pDX )
{
	CPlotOptionsDialog::DoDataExchange( pDX );
	//{{AFX_DATA_MAP( CRateSensitivityExplorer )
	DDX_Control(pDX, IDC_RiskFreeRateLabel, c_RiskFreeRateLabel);
	DDX_Control(pDX, IDC_DeltaHigh, c_DeltaHigh);
	DDX_Control(pDX, IDC_DeltaLow, c_DeltaLow);
	DDX_Control(pDX, IDC_RiskFreeRate, c_RiskFreeRate);
//	DDX_Control(pDX, IDC_Bond, c_Bond);
//	DDX_Control(pDX, IDC_Option, c_Option);
	DDX_Control(pDX, IDC_Portfolio, c_Portfolio);
	DDX_Text(pDX, IDC_DeltaHigh, m_DeltaHigh);
	DDV_MinMaxFloat(pDX, m_DeltaHigh, 0.f, 9999.f);
	DDX_Text(pDX, IDC_DeltaLow, m_DeltaLow);
	DDV_MinMaxFloat(pDX, m_DeltaLow, 0.f, 9999.f);
	DDX_Text(pDX, IDC_RiskFreeRate, m_RiskFreeRate);
	DDV_MinMaxFloat(pDX, m_RiskFreeRate, 1.e-003f, 9999.f);
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
	ON_EN_KILLFOCUS(IDC_RiskFreeRate, OnKillFocusRiskFreeRate)
	ON_WM_PAINT()
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
	ON_EN_SETFOCUS(IDC_DeltaLow, OnSetFocusDeltaLow)
	ON_EN_SETFOCUS(IDC_DeltaHigh, OnSetFocusDeltaHigh)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRateSensitivityExplorer message handlers

BOOL CRateSensitivityExplorer::OnInitDialog()
{
		// set the extents of plot region
	CPlotOptionsDialog ::OnInitDialog();
	RegisterHotKeys( IDC_Symbol1, IDC_Symbol2 );
	plotExtents.top = 104;			// leaves enough room for the controls at the top

		// get the risk free rate from the registry
	if ( ! GetRiskFreeRate(&m_RiskFreeRate) )				// was: theApp.regMgr->
	{
#ifdef _DEBUG
		TRACE( _T("RateSensitivityExplorer::OnInitDialog: GetRiskFreeRate failed, using m_RiskFreeRate=%g.\n"),
			m_RiskFreeRate );
#endif
	}
	setEditBox( "%.3f", m_RiskFreeRate, IDC_RiskFreeRate );


		// enable Bond and Option buttons
//	int		nBnds = theApp.bndMgr  ?  theApp.bndMgr->GetSymbolTableSize()  :  0;
//	c_Bond.EnableWindow( nBnds > 0 );

	int		nOpts = theApp.optMgr  ?  theApp.optMgr->GetSymbolTableSize()  :  0;
//	c_Option.EnableWindow( nOpts > 0 );

		// these dialog globals (help us decide whether to enable the portfolio check button)
//	nBndsWpos = theApp.bndMgr->NumIndexedIssues();				// numBondsWithPositions
	nIndexedIssues = theApp.optMgr->NumIndexedIssues();			// numOptionsWithPositions


		// see if there's a registered RateSensitivity structure
	if ( GetOptionRateSensitivity(&rateSens) )					// was:  theApp.regMgr->
	{		// deltas are managed via m_Delta* variables,
			// so rateSens must be updated when the dialog closes
		m_DeltaHigh = rateSens.deltaHigh;
		m_DeltaLow  = rateSens.deltaLow;
	}
	else
	{
#ifdef _DEBUG
		TRACE( _T("RateSensitivityExplorer::OnInitDialog: GetRateSensitivity failed.\n") );
#endif
			// instrument and def_IDs are still in a useless state --> try to pick an instrument type
			// try to load the recent Bond or Option
			// Borrowed the following from CPositionsDialog and carved out unneeded pieces
		if ( ! GetRecentOption(&(rateSens.optDef_ID[0]) ) )		// was:  theApp.regMgr->
		{	
#ifdef _DEBUG
			TRACE( _T("RateSensitivityExplorer::OnInitDialog: GetRecentOption failed.\n") );
#endif
		}
	}

		// check the selected instrument and choose a CDataManager
//	CheckRadioButton( IDC_Bond, IDC_Option, (rateSens.instrument == Bond ? IDC_Bond : IDC_Option) );
	SetCheckButtons();				// the five plot enablers from rateSens	
	LoadBOSs();						// fetch bond/options recently evaluated
	LoadSymbols();					// load ComboBoxes & select items matching the current BOSs
	RecalcAll();

			// load up the NumEdit boxes (either registered values or defaults)
	setEditBox( "%.3f", m_DeltaLow,		IDC_DeltaLow  );
	setEditBox( "%.3f", m_DeltaHigh,	IDC_DeltaHigh );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//----------------------------------------------------------------------------
HBRUSH	CRateSensitivityExplorer::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	return	CPlotOptionsDialog ::OnCtlColor(pDC, pWnd, nCtlColor);
}			// OnCtlColor()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::LoadBOSs( void ) 
{	unsigned short	jj;
		// we switched instruments or are initializing the dialog
//	ASSERT( rateSens.instrument == Bond  ||  rateSens.instrument == Option );
	if ( pBOS[0]  &&  pBOS[0]->getAssetType() == Option )
	{
#ifdef _DEBUG
		TRACE( _T("RateSensitivityExplorer::LoadBOSs: LoadBOSs called with no change in instrument type\n") );
		AfxDebugBreak();
#endif
	}
	DeleteBOSs();						// delete any existing BOSs

		// select a def_ID block
	long*	def_ID = rateSens.optDef_ID;

		// fill any open slots (containing -1) in the selected def_ID
	FillSlots( def_ID );

	COptionManager*	dm = theApp.optMgr;
		// load BOSs corresponding to the def_IDs
	long	maxOptDef_ID = dm->GetDefFileLen();
	for ( jj = 0; jj < nBOS; jj++ )
	{	long	optDef_ID = *(def_ID+jj);
		if ( optDef_ID >= 0  &&  optDef_ID < maxOptDef_ID )
			*(pBOS+jj) = (CBOS*)dm->ReadOption( optDef_ID );
	}
}			// LoadBOSs()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::SetCheckButtons( void ) 
{
	bool*	chkBtns = rateSens.optChk;
	CPlotOptionsDialog::SetCheckButtons( chkBtns );

		// set Option and Portfolio buttons
	rateSens.optPortf = rateSens.optPortf  &&  ( nIndexedIssues > 0 );
	c_Portfolio.EnableWindow( nIndexedIssues > 0 );
	
//	c_RiskFreeRate.EnableWindow( true );			// is this the place to do this?
//	c_RiskFreeRateLabel.EnableWindow( true );		// why ???
	c_Portfolio.SetCheck( rateSens.optPortf  ?  BST_CHECKED  :  BST_UNCHECKED );
}			// SetCheckButtons()
//----------------------------------------------------------------------------------------
void	CRateSensitivityExplorer::LoadSymbols( void ) 
{		// load the symbols associated with the current instrument type
//	ASSERT( rateSens.instrument == Bond  ||  rateSens.instrument == Option );
	CMapStringToPtr*	theMap = theApp.GetSymbolTable( Option );
	LoadSymbolsCore( theMap );
}			// LoadSymbols()
//----------------------------------------------------------------------------------------
void	CRateSensitivityExplorer::RecalcAll( void )
{
	for ( unsigned short ii = 0; ii < nBOS; ii++ )
	{	instFptsValid[ii] = false;
		CalcInstFpts( ii );				// if okay, set instFptsValid[ii] here
	}
	folderFptsValid = false;
	CalcFolderFpts();					// if okay, set folderFptsValid here
}			// RecalcAll()
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

		// now we can paint the four Option instances
	topLeft.y = -2e38f;						// ~ negative infinity
	bottomRight.y = 2e38f;					// ~ positive infinity
	for ( ii = 0; ii < nBOS; ii++ )
	{	if ( InstValid(ii, rateSens.optChk) )
		{
#ifdef _DEBUG
	TRACE( _T("RateSensitivityExplorer::OnPaint: %d inst curve is valid and enabled\n"), ii );
#endif
			if ( instMinY[ii] < bottomRight.y )
				bottomRight.y = instMinY[ii];
			if ( instMaxY[ii] > topLeft.y )
				topLeft.y = instMaxY[ii];
		}
	}
		// the portfolio curve
	if ( rateSens.optPortf  &&  folderFptsValid )
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
	TRACE( _T("RateSensitivityExplorer::OnPaint(6): topLeft.x=%g, topLeft.y=%g, bottomRight.x=%g, bottomRight.y=%g\n"),
		topLeft.x, topLeft.y, bottomRight.x, bottomRight.y );
#endif

		// scale against available space and plot
	xScale = (float)(plotExtents.Width()  / (bottomRight.x - topLeft.x));
	yScale = (float)(plotExtents.Height() / (topLeft.y - bottomRight.y));
#ifdef _DEBUG
		TRACE( _T("RateSensitivityExplorer::OnPaint: xScale=%g, yScale=%g\n"), xScale, yScale );
#endif
		// paint time
	DrawGrid( dc );
	for ( ii = 0; ii < nBOS; ii++ )
	{	if ( InstValid(ii, rateSens.optChk)  &&  instFptsValid[ii] )				// check button state examined by InstValid()
			DrawPolyline( ii, dc, (instFpts + ii*nInstRatePts), nInstRatePts );
	}
		// plot the portfolio line
	if ( rateSens.optPortf  &&  folderFptsValid )				// portfolio plotting is enabled & the points are valid
		DrawPolyline( nBOS, dc, folderFpts, nFolderRatePts );	// nBOS --> black

	DrawGridLabels( dc );
}			// OnPaint()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::CalcInstFpts( unsigned short ii )
{		// the domain X-extents are simply from today (x=0.0) to timeDecay.endDate (x=years after today)
	if ( ! InstValid( ii, rateSens.optChk ) )
		return;
	CWaitCursor	wait;			// we could be gone a while - display the wait cursor

	float	maxXval = m_RiskFreeRate + m_DeltaHigh;
	float	minXval = m_RiskFreeRate - m_DeltaLow;
	if ( minXval < 0.0 )
		minXval = 0.1f;
	float	range = maxXval - minXval;
	if ( range*range < 4.0 )
		maxXval = minXval + 2.0f;
	topLeft.x = minXval;
	bottomRight.x = maxXval;
	double xDispInc = (maxXval - minXval) / (nInstRatePts - 1);		// percentage because min and max are percentages
	double xCalcInc = xDispInc / 100.0;								// fractional representation of the provisional RFR
#ifdef _DEBUG
	TRACE( _T("RateSensitivityExplorer::CalcInstFpts: today=%s, minXval=%g, maxXval=%g, xDispInc=%g\n"),
		evcs.fromDate.Format(VAR_DATEVALUEONLY), minXval, maxXval, xDispInc );
#endif
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// compute the issue plot points and the domain Y-extents
	COption* opt = (COption*) *(pBOS+ii);
	evcs.calcYrsToExpiry = true;
	evcs.calcPvDivs = true;								// every initial euroValue call will reset this
	evcs.riskFreeRate = m_RiskFreeRate / 100.0;			// the reference RFR that the option's value is based on
	evcs.sigma = opt->getVolatility();
	evcs.stockPrice = opt->getStockPrice();
		// evcs.fromDate initialized in the constructor
	double	optRefVal;
	if ( ! opt->EuroValue( evcs, optRefVal ) )
		return;

		// collect Y-values (instFpts[ii*nTimeBOS+0] is always (0.0, 0.0) in pureBS mode
		// the previous three lines were compiled AND pureBS == true
		// its simpler to potentially waste the first computation (when pureBS is true)
	instMinY[ii] =  2e38f;									// dialog var
	instMaxY[ii] = -2e38f;									// dialog var
	double xDispVal = minXval;								// user coordinates (percent)
	evcs.riskFreeRate = minXval / 100.0;					// a fractional representation of the provisional RFR
	CFPoint* instPts = instFpts + ii*nInstRatePts;
	for ( short jj = 0; jj < nInstRatePts; jj++ )			// for each of the FPoints
	{	double yVal = 0.0;									// in case the if condition is false
		if ( fabs(optRefVal) >= 1e-3 )						// avoid divide by zero
		{		// convert Black-Scholes value to a fraction of the Option's referece value
			double bsVal = 0.0;
			if ( ! opt->EuroValue( evcs, bsVal ) )
				return;
			yVal = 100 * ( bsVal - optRefVal ) / optRefVal;	// result is a percentage relative to reference value
		}
#ifdef _DEBUG
//		TRACE( _T("RateSensitivityExplorer::OnPaint: jj=%d, yVal=%g\n"), jj, yVal );
#endif
			// update min and max Y-values
		if ( yVal < instMinY[ii] )
			instMinY[ii] = (float)yVal;
		if ( yVal > instMaxY[ii] )
			instMaxY[ii] = (float)yVal;

		CFPoint* cfp = instPts + jj;
		cfp->x = (float)xDispVal;					// the displayed X-value (a percentage)
		cfp->y = (float)yVal;						// the computed Y-value (also a percentage)
#ifdef _DEBUG
//		TRACE( _T("RateSensitivityExplorer::CalcInstFpts: cfp->x=%g, cfp->y=%g\n"), cfp->x, cfp->y );
#endif
			// prepare for the next point
		evcs.calcPvDivs = true;
		evcs.riskFreeRate += xCalcInc;				// move the fractional value forward
		xDispVal += xDispInc;						// move the percentage (displayed) value forward
	}
	instFptsValid[ii] = true;
#ifdef _DEBUG
	TRACE( _T("RateSensitivityExplorer::CalcInstFpts: instFptsValid[%d]=%s\n"), ii, ( instFptsValid[ii] ? _T("true") : _T("false") ) );
#endif
}			// CalcInstFpts()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::CalcFolderFpts( void )
{	COption* opt;
	int	ii, jj;
	CWaitCursor	wait;			// we could be gone a while - display the wait cursor
		// compute the folder's rate sensitivity
		// overwrite either optionFolderPts with results (instrument decides which)
	float	maxXval = m_RiskFreeRate + m_DeltaHigh;
	float	minXval = m_RiskFreeRate - m_DeltaLow;
	if ( minXval < 0.0 )
		minXval = 0.1f;
	float	range = maxXval - minXval;
	if ( range*range < 4.0 )
		maxXval = minXval + 2.0f;
	topLeft.x = minXval;
	bottomRight.x = maxXval;
	double xDispInc = (maxXval - minXval) / (nFolderRatePts - 1);		// a percentage
	double xCalcInc = xDispInc / 100.0;
#ifdef _DEBUG
	TRACE( _T("RateSensitivityExplorer::CalcFolderFpts: today=%s, minXval=%g, maxXval=%g, xDispInc=%g\n"),
		evcs.fromDate.Format(VAR_DATEVALUEONLY), minXval, maxXval, xDispInc );
#endif
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// compute the folder plot points and the domain Y-extents
		// initialize a working array of Fpts
	double cfpY[ nFolderRatePts ];
	double xDispVal = minXval;									// lowest X-value (in user coordinates - percent)
	for ( jj = 0; jj < nFolderRatePts; jj++ )
	{	cfpY[jj] = 0.0;											// working array contributes to folderFpts below
		(folderFpts + jj)->x = (float)xDispVal;					// dialog instance variable result
		xDispVal += xDispInc;									// in user coordinates (percentages)
	}
	double	 folderCost = 0.0;		// all option positions

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// for each issue with positions
	for ( ii = 0; ii < nIndexedIssues; ii++ )
	{	long	def_ID = theApp.optMgr->GetPosIndexedDef_ID( ii );
			// set up for Option evaluation and subsequent rescaling (for options)
		opt = ( def_ID < 0 )  ?  NULL  :  theApp.optMgr->ReadOption( def_ID );
		if ( opt == NULL )
		{
#ifdef _DEBUG
			TRACE( _T("TimeDecayExplorer::CalcFolderFpts: NULL option encountered.\n") );
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
			// compute the option's reference Y-value using Harvey's (below)
		evcs.calcYrsToExpiry = true;
		evcs.calcPvDivs = true;
		evcs.riskFreeRate = m_RiskFreeRate / 100.0;		// this is what the option's reference value is based on
		evcs.sigma = opt->getVolatility();				// 02Feb07
		evcs.stockPrice = opt->getStockPrice();			// 02Feb07
			// evcs.fromDate initialized in the constructor
		double optRefVal;
		if ( ! opt->EuroValue( evcs, optRefVal ) )
			return;
#ifdef _DEBUG
//		TRACE( _T("RateSensitivityExplorer::CalcFolderFpts: %g <- %s(stkPrc=%g, sig=%g, refYrs=%g, rfr=%g, pvDivs=%g)\n"),
//			optRefVal, opt->getSymbol(), stkPrice, sigma, refYrs, rfr, pvDivs );
#endif
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// compute the weighting factor (issueBasis) for this issue
		long	posOffset = opt->openPosOffset;
		double	issueBasis = 0.0;							// sum of totalCost across all of opt's positions
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
		evcs.riskFreeRate = minXval / 100.0;
		for ( jj = 0; jj < nFolderRatePts; jj++ )			// for each of the FPoints
		{		// convert Black-Scholes value to a percent change from the Option's reference value
			double yVal = 0.0;								// in case the if condition is false
			if ( fabs(optRefVal) > 1e-3 )
			{	double	bsVal;
				if ( ! opt->EuroValue( evcs, bsVal ) )
					return;
				yVal = issueBasis * ( bsVal - optRefVal ) / optRefVal;		// a cost weighted value
			}
				// upscale the Y-value by the total cost (issueBasis) of the issue's positions 
			*(cfpY+jj) += yVal;				// update the Y-value for this position

				// prepare for the next pass
			evcs.riskFreeRate += xCalcInc;
		}									// for each point
		folderCost += issueBasis;			// this will be our descaling factor
	}										// for each issue
#ifdef _DEBUG
	TRACE( _T("RateSensitivityExplorer::CalcFolderFpts: %d options have aggregate basis=%.2f\n"),
		nIndexedIssues, folderCost );
#endif

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// post issue-loop downscaling by folderCost, simultaneously upscaling to percent
	folderMinY = 2e38f;								// a big positive number
	folderMaxY = -2e38f;							// a big negative number
	for ( jj = 0; jj < nFolderRatePts; jj++ )
	{		// rescale the yVal from a fractional value to percent
		double	syVal = (fabs(folderCost) < 0.001)  ?  0.0  :  (100.0 * cfpY[jj] / folderCost);
		if ( syVal < folderMinY )
			folderMinY = (float)syVal;				// percent
		if ( syVal > folderMaxY )
			folderMaxY = (float)syVal;				// percent

		(folderFpts + jj)->y = (float)syVal;			// the rescaled Y-value (cost weighted, percent change)
#ifdef _DEBUG
//		TRACE( _T("RateSensitivityExplorer::CalcFolderFpts: cfp->x=%g, cfp->y=%g\n"),
//			(optionFolderPts + jj)->x, (optionFolderPts + jj)->y );
#endif
	}
	folderFptsValid = true;
}			// CalcFolderFpts()
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
	if ( nState == WA_ACTIVE || nState == WA_CLICKACTIVE )
		RegisterHotKeys( IDC_Symbol1, IDC_Symbol2 );				// screens for WM5 internally
}			// OnActivate()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::SymbolWorkCore( short ii, CString sym, long* def_ID )
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
void	CRateSensitivityExplorer::KillFocusSymbolWork(
	CSeekComboBox*	scb,
	short			ii,
	CString&		optSym								)
{	CString			sym;
	m_activeComboBox = NULL;
	scb->GetWindowText( sym );
	sym.MakeUpper();
	if ( sym == optSym )
		return;								// nothing to do
	optSym = sym;
	SymbolWorkCore( ii, optSym, rateSens.optDef_ID );
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
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSetFocusSymbol1()
{
	m_activeComboBox = IDC_Symbol1;
}			// OnSetFocusSymbol1()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSetFocusSymbol2()
{
	m_activeComboBox = IDC_Symbol2;
}			// OnSetFocusSymbol2()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSetFocusSymbol3()
{
	m_activeComboBox = IDC_Symbol3;
}			// OnSetFocusSymbol3()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnSetFocusSymbol4()
{
	m_activeComboBox = IDC_Symbol4;
}			// OnSetFocusSymbol4()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::SelChangeSymbolWork(
	CSeekComboBox*	scb,
	short			ii,
	CString&		optSym								)
{	CString			sym;
	int cnt = scb->GetCount();
	if ( cnt < 1 )
		return;
	int	sel = scb->GetCurSel();
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
	if ( sel < 0 ) sel = 0;							// should be superfluous
	scb->GetLBText( sel, sym );
	if ( sym == optSym )
		return;								// nothing to do
	optSym = sym;
	SymbolWorkCore( ii, optSym, rateSens.optDef_ID );
}			// SelChangeSymbolWork()
//----------------------------------------------------------------------------
void CRateSensitivityExplorer::OnSelChangeSymbol1() 
{
	SelChangeSymbolWork( &c_Symbol1, 0, m_Symbol1 );
}			// OnSelChangeSymbol1()
//----------------------------------------------------------------------------
void CRateSensitivityExplorer::OnSelChangeSymbol2() 
{
	SelChangeSymbolWork( &c_Symbol2, 1, m_Symbol2 );
}			// OnSelChangeSymbol2()
//----------------------------------------------------------------------------
void CRateSensitivityExplorer::OnSelChangeSymbol3() 
{
	SelChangeSymbolWork( &c_Symbol3, 2, m_Symbol3 );
}			// OnSelChangeSymbol3()
//----------------------------------------------------------------------------
void CRateSensitivityExplorer::OnSelChangeSymbol4() 
{
	SelChangeSymbolWork( &c_Symbol4, 3, m_Symbol4 );
}			// OnSelChangeSymbol4()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::CheckWork( CButton* chkButton, unsigned short ii )
{
	int		status = chkButton->GetCheck();
	bool*	chkState = rateSens.optChk;
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
void	CRateSensitivityExplorer::OnCheck1( void )
{
	CheckWork( &c_Check1, 0 );
}			// OnCheck1()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnCheck2( void )
{
	CheckWork( &c_Check2, 1 );
}			// OnCheck2()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnCheck3( void )
{
	CheckWork( &c_Check3, 2 );
}			// OnCheck3()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnCheck4( void )
{
	CheckWork( &c_Check4, 3 );
}			// OnCheck4()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnPortfolio( void )
{		// status could be BST_CHECKED, BST_UNCHECKED, or BST_INDETERMINATE
	int	status = c_Portfolio.GetCheck();
	if (						status == BST_INDETERMINATE
		 ||	 (  rateSens.optPortf  &&  status == BST_CHECKED		    )
		 ||  ( !rateSens.optPortf  &&  status == BST_UNCHECKED	  ) )
		return;					// nothing to do

		// recapture the state of the portfolio check button
	rateSens.optPortf = ( status == BST_CHECKED );		// polymorphic control (now redundant with rateSens.optPortf)
	Plot();
}			// OnPortfolio()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnKillFocusDeltaHigh()
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_DeltaHigh );
	if ( ff == m_DeltaHigh )
		return;
	m_DeltaHigh = ff;
	setEditBox( "%g", m_DeltaHigh, IDC_DeltaHigh );
	RecalcAll();
	Plot();
}			// OnKillFocusDeltaHigh()
//----------------------------------------------------------------------------
void CRateSensitivityExplorer::OnSetFocusDeltaHigh()
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusDeltaHigh()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnKillFocusDeltaLow()
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_DeltaLow );
	if ( ff == m_DeltaLow )
		return;
	if ( m_RiskFreeRate - ff < 0.01 )
		ff = m_RiskFreeRate - 0.01f;
	m_DeltaLow = ff;
	setEditBox( "%g", m_DeltaLow, IDC_DeltaLow );
	RecalcAll();
	Plot();
}			// OnKillFocusDeltaLow()
//----------------------------------------------------------------------------
void CRateSensitivityExplorer::OnSetFocusDeltaLow()
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusDeltaLow()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnKillFocusRiskFreeRate()
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
void CRateSensitivityExplorer::OnSetFocusRiskFreeRate()
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusRiskFreeRate()
//----------------------------------------------------------------------------
/*
void	CRateSensitivityExplorer::OnPaint()
{	long	ii;
	short	jj;											// most of the time shorts would work
	bool	bosValid[ nBOS ];
	bool	somethingValid = false;
	for ( jj = 0; jj < nBOS; jj++ )
	{	*(bosValid+jj) = InstValid( jj );
		somethingValid = somethingValid  ||  *(bosValid+jj);
	}
	somethingValid = somethingValid  ||  ( nIndexedIssues > 0 );

		// nothing to paint if there are no instruments to evaluate
	CPaintDC	dc( this );								// device context for painting
	DrawPlotBackground( dc );							// blanks out the plot space
	if ( ! somethingValid )
	{	CPlotOptionsDialog::OnPaint();								// solves the initially transparent background problem
		return;									// doesn't unpaint
	}
//	CWaitCursor	wait;							// we could be gone a long time - display the wait cursor, but not inside OnPaint()

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// the domain X-extents are provided by the user (in percent)
	float	minXval = -m_DeltaLow;
	float	maxXval =  m_DeltaHigh;
		// for options
	minXval += m_RiskFreeRate;
	if ( minXval < 0.0 )
		minXval = 0.1f;
	maxXval += m_RiskFreeRate;

	float	range = maxXval - minXval;
	if ( range*range < 4.0 )
		maxXval = minXval + 2.0f;

	topLeft.x = minXval;
	bottomRight.x = maxXval;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// compute the issue plot points and the domain Y-extents
	double	xVal, yVal, minYval = 2e38f, maxYval = -2e38f;
	double	xInc = (maxXval - minXval) / (nInstRatePts - 1);
	double	optRefVal;

	COption*		opt;
	COleDateTime	today = COleDateTime::GetCurrentTime();
	CFPoint	instFpts[ nBOS * nInstRatePts ];					// row-major order
	for ( ii = 0; ii < nBOS; ii++ )
	{	if ( bosValid[ii] )
		{	xVal = topLeft.x;								// re-copy the lowest x value				
				// set up for Option evaluation and subsequent rescaling operations
			opt = (COption*) *(pBOS+ii);
				// evcs.fromDate initialized in the constructor
			evcs.calcYrsToExpiry = true;
			evcs.calcPvDivs = true;							// every initial euroValue call will reset this
			evcs.stockPrice = opt->getStockPrice();
			evcs.sigma = opt->getVolatility();
			evcs.riskFreeRate = m_RiskFreeRate / 100.0;
			optRefVal = opt->EuroValue( evcs );

				// collect Y-values
			for ( jj = 0; jj < nInstRatePts; jj++ )						// for each of the FPoints
			{	(instFpts + ii*nInstRatePts + jj)->x = (float)xVal;
				evcs.riskFreeRate = xVal / 100.0;
					// Option...
					// rescale xVal from percent to a fractional value before calculating Black-Scholes value
				if ( fabs(optRefVal) >= 0.0001 )
				{		// convert Black-Scholes value to a fraction of the Option's referece value
					evcs.riskFreeRate = xVal / 100.0;
					evcs.calcPvDivs = true;
					double bsVal = opt->EuroValue( evcs );
					yVal = ( bsVal - optRefVal ) / optRefVal;
#ifdef _DEBUG
//					TRACE( _T("RateSensitivityExplorer::OnPaint: jj=%d, yVal=%g\n"), jj, yVal );
#endif
				}
				else
					yVal = 0.0;
				
					// rescale yVal from a fractional value to percent
				double	syVal = 100.0 * yVal;

					// update min and max Y-values
				if ( syVal < minYval )
					minYval = syVal;

				if ( syVal > maxYval )
					maxYval = syVal;
				
				(instFpts + ii*nInstRatePts + jj)->y = (float)syVal;	// store the rescaled Y-value
				xVal += xInc;											// prepare for the next point
			}
		}		
	}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	bool	folderFptsValid = optionPtsAreValid;
	if ( bPortfolio  &&  !folderPtsAreValid  &&  nIndexedIssues > 0 )
	{		// the check box is checked, there are issues with positions
			// but the portfolio plot points are not valid
			// minYval & maxYval are updated within CalcFolderPts()
		CalcFolderPts( minXval, maxXval, minYval, maxYval );
		folderPtsAreValid = true;
	}
	CFPoint*	folderPts = NULL;
	if ( folderPtsAreValid )
	{	folderPts = optionFolderPts;
		for ( jj = 0; jj < nInstRatePts; jj++ )
		{	if ( (folderPts+jj)->y < minYval )
				minYval = (folderPts+jj)->y;
			if ( (folderPts+jj)->y > maxYval )
				maxYval = (folderPts+jj)->y;
		}
	}

#ifdef _DEBUG
		TRACE( _T("RateSensitivityExplorer::OnPaint: minYval=%g, maxYval=%g\n"), minYval, maxYval );
#endif
	if ( fabs( maxYval - minYval ) < 1e-2 )
	{		// prevent very high yScale
		maxYval = 0.5;
		minYval = -0.5;
	}
	topLeft.y = (float)maxYval;
	bottomRight.y = (float)minYval;

		// scale against available space and plot
	xScale = (float)(plotExtents.Width()  / (bottomRight.x - topLeft.x));
	yScale = (float)(plotExtents.Height() / (topLeft.y - bottomRight.y));
#ifdef _DEBUG
		TRACE( _T("RateSensitivityExplorer::OnPaint: xScale=%g, yScale=%g\n"), xScale, yScale );
#endif

		// now we can paint the four Option instances
	DrawGrid( dc );
	for ( jj = 0; jj < nBOS; jj++ )
	{	if ( *(bosValid+jj) )						// check button state already examined by InstValid()
			DrawPolyline( jj, dc, (instFpts + jj*nInstRatePts), nInstRatePts );
	}
		// plot the portfolio line
	if ( bPortfolio  &&  folderPtsAreValid )		// portfolio plotting is enabled & the points are valid
		DrawPolyline( nBOS, dc, folderPts, nFolderRatePts );
	DrawGridLabels( dc );
}			// OnPaint()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::CalcFolderPts( double	minXval, double maxXval, double& minYval, double& maxYval )
{		// compute the folder's rate sensitivity
		// overwrite optionFolderPts with results
	CBOS*			bos;
	CFPoint*		cfp;									// a handy pointer into portfolioPts
//	CBond*			bnd;
	COption*		opt;
	COleDateTime	today = COleDateTime::GetCurrentTime();
	double			xInc = (maxXval - minXval) / (nFolderRatePts - 1);
	double			folderCost = 0.0;						// all option positions

		// initialize the portfolio points
	long	ii;
	short	jj;
	double	xVal = topLeft.x;								// re-copy the lowest x value
	CFPoint*	folderPts = optionFolderPts;
	for ( jj = 0; jj < nFolderRatePts; jj++ )
	{	cfp = folderPts + jj;
		cfp->x = (float)xVal;
		cfp->y = 0.0f;
		xVal += xInc;
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// for each issue with positions
	double	yVal, optRefVal;
//	double	stkPrice, sigma, yrs, pvDivs, rfr = m_RiskFreeRate / 100.0;
//	ASSERT( rateSens.instrument == Bond  ||  rateSens.instrument == Option );
	CDataManager*	dm = (CDataManager*)theApp.optMgr;
	for ( ii = 0; ii < nIndexedIssues; ii++ )
	{	long	def_ID = dm->GetPosIndexedDef_ID( ii );
			// set up for Bond/Option evaluation and subsequent rescaling (for options)
//		if ( rateSens.instrument == Bond )
//		{	bnd = theApp.bndMgr->ReadBond( def_ID );
//			bos = (CBOS*)bnd;
//		}
//		else	// instrument == Option
		{	opt = theApp.optMgr->ReadOption( def_ID );
			bos = (CBOS*)opt;
		}

			// continue if critical conditions aren't met
		if ( bos == NULL )
		{
#ifdef _DEBUG
			TRACE( _T("RateSensitivityExplorer::CalcFolderPts: NULL bos encountered.\n") );
			AfxDebugBreak();
#endif
			continue;
		}

		CMapPtrToPtr*	posLst = bos->getPositionList();
		if ( posLst == NULL )
		{
#ifdef _DEBUG
			TRACE( _T("RateSensitivityExplorer::CalcFolderPts: %s has no position list\n"),
				bos->getSymbol() );
			AfxDebugBreak();
#endif
			delete	bos;
			continue;
		}

			// so, we've got something to plot
		if ( bos->getAssetType() == Option )
		{		// compute the option's reference Y-value
				// evcs.fromDate initialized in the constructor
			evcs.calcYrsToExpiry = true;
			evcs.calcPvDivs = true;							// every euroValue call will reset this
			evcs.stockPrice = opt->getStockPrice();
			evcs.sigma = opt->getVolatility();
			evcs.riskFreeRate = m_RiskFreeRate / 100.0;
			optRefVal = opt->EuroValue( evcs );
		}

			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// compute the weighting factor (issueBasis) for this issue
		long	posOffset = bos->openPosOffset;
		double	issueBasis = 0.0;							// sum of totalCost across all of bos's positions
		CPosition*	pos;
		while ( posLst->Lookup( (void*)posOffset, (void*&)pos ) )
		{		// process this position
			issueBasis += pos->totalCost;
				// prepare for next loop pass
			posOffset = pos->prevPos_ID;
		}														// for each position
#ifdef _DEBUG
		TRACE( _T("RateSensitivityExplorer::CalcFolderPts: %s has basis=%.2f\n"), bos->getSymbol(), issueBasis );
#endif
		
			// update the Y-values
		for ( jj = 0; jj < nFolderRatePts; jj++ )				// for each of the FPoints
		{	cfp = folderPts + jj;
				// Option...
				// rescale xVal from percent to a fractional value before calculating Black-Scholes value
			if ( fabs(optRefVal) >= 1e-4 )
			{		// convert Black-Scholes value to a fraction of the Option's referece value
				evcs.riskFreeRate = cfp->x / 100.0;
				evcs.calcPvDivs = true;						// every euroValue call will reset this
				double	bsVal = opt->EuroValue( evcs );
				yVal = ( bsVal - optRefVal ) / optRefVal;
#ifdef _DEBUG
//				TRACE( _T("RateSensitivityExplorer::CalcFolderPts: ii=%d, jj=%d, yVal=%g\n"),
//					ii, jj, yVal );
#endif
			}
			else
			{	yVal = 0.0;
#ifdef _DEBUG
				TRACE( _T("RateSensitivityExplorer::CalcFolderPts: %s produces 0.0 @ %g\n"), bos->getSymbol(), cfp->x );
#endif
			}
				// upscale the Y-value by the total cost (issueBasis) of the issue's positions 
			yVal *= issueBasis;
			cfp->y += (float)yVal;		// update the Y-value for this position
		}								// for each point
		folderCost += issueBasis;
		delete	bos;					// we're done with this BOS
	}									// for each issue
#ifdef _DEBUG
	TRACE( _T("RateSensitivityExplorer::OnPaint: %d options have aggregate basis=%.2f\n"),
		nIndexedIssues, folderCost );
#endif

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// post issue-loop downscaling by folderCost, upscaling to percentages
	for ( jj = 0; jj < nFolderRatePts; jj++ )
	{	cfp = folderPts + jj;
				// rescale the yVal from a fractional value to percent
		double	syVal = (fabs(folderCost) < 0.001)  ?  0.0  :  (100.0 * cfp->y / folderCost);
		cfp->y = (float)syVal;			// replace the Y-value with the final rescaled (percent change) version
	}
	optionPtsAreValid = true;
}			// CalcFolderPts()
*/
//----------------------------------------------------------------------------
/*
bool	CRateSensitivityExplorer::InstValid( short jj )
{		// ii is 0-based
	CBOS*	bos = *(pBOS+jj);

//	ASSERT( rateSens.instrument == Bond  ||  rateSens.instrument == Option );
	bool*	chkBtns = rateSens.optChk;
	if ( ! *(chkBtns+jj)  ||  bos == NULL  ||  bos->getMktPrice() < 0.001 )
		return	false;									// nothing to do

	bool	res;
		// Option
	COption*	opt = (COption*)bos;
	res = opt->getVolatility() > 0.001
	  &&  opt->getStockPrice() > 0.001
	  &&  opt->strikePrice	   > 0.001;
	
	return	res;
}			// InstValid()
//----------------------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnCheck1( void )
{
	int	status = c_Check1.GetCheck();
//	ASSERT( rateSens.instrument == Bond  ||  rateSens.instrument == Option );
	bool*	chkBtns = rateSens.optChk;
	if (						  status == BST_INDETERMINATE
		 ||	 (  *(chkBtns+0)  &&  status == BST_CHECKED			)
		 ||	 ( !*(chkBtns+0)  &&  status == BST_UNCHECKED		) )
		return;					// nothing to do
	
	*(chkBtns+0) = ( status == BST_CHECKED );
	Plot();
}			// OnCheck1()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnCheck2( void )
{
	int	status = c_Check2.GetCheck();
//	ASSERT( rateSens.instrument == Bond  ||  rateSens.instrument == Option );
	bool*	chkBtns = rateSens.optChk;
	if (						  status == BST_INDETERMINATE
		 ||	 (  *(chkBtns+1)  &&  status == BST_CHECKED			)
		 ||	 ( !*(chkBtns+1)  &&  status == BST_UNCHECKED		) )
		return;					// nothing to do

	*(chkBtns+1) = ( status == BST_CHECKED );
	Plot();
}			// OnCheck2()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnCheck3( void )
{
	int	status = c_Check3.GetCheck();
//	ASSERT( rateSens.instrument == Bond  ||  rateSens.instrument == Option );
	bool*	chkBtns = rateSens.optChk;
	if (						  status == BST_INDETERMINATE
		 ||	 (  *(chkBtns+2)  &&  status == BST_CHECKED			)
		 ||	 ( !*(chkBtns+2)  &&  status == BST_UNCHECKED		) )
		return;					// nothing to do

	*(chkBtns+2) = ( status == BST_CHECKED );
	Plot();
}			// OnCheck3()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnCheck4( void )
{
	int	status = c_Check4.GetCheck();
//	ASSERT( rateSens.instrument == Bond  ||  rateSens.instrument == Option );
	bool*	chkBtns = rateSens.optChk;
	if (						  status == BST_INDETERMINATE
		 ||	 (  *(chkBtns+3)  &&  status == BST_CHECKED			)
		 ||	 ( !*(chkBtns+3)  &&  status == BST_UNCHECKED		) )
		return;					// nothing to do

	*(chkBtns+3) = ( status == BST_CHECKED );
	Plot();
}			// OnCheck4()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnBond()
{		// status could be BST_CHECKED, BST_UNCHECKED, or BST_INDETERMINATE
	int	status = c_Bond.GetCheck();
	if ( status == BST_INDETERMINATE  ||  rateSens.instrument == Bond )
		return;					// nothing to do

		// store the state of the portfolio check button
	rateSens.optPortf = ( c_Portfolio.GetCheck() == BST_CHECKED );
	c_Option.SetCheck( BST_UNCHECKED );		// deselect the Option check button

		// switch instrument types
	rateSens.instrument = Bond;
	SetCheckButtons();						// restore previous check box settings - sets bPortfolio
	LoadBOSs();								// retrieve Bonds from the database
	LoadSymbols();							// displays the BOSs selected above too
	Plot();
}			// OnBond()
//----------------------------------------------------------------------------
void	CRateSensitivityExplorer::OnOption()
{		// status could be BST_CHECKED, BST_UNCHECKED, or BST_INDETERMINATE
	int	status = c_Option.GetCheck();
	if ( status == BST_INDETERMINATE  ||  rateSens.instrument == Option )
		return;					// nothing to do

		// store the state of the portfolio check button
	rateSens.bndPortf = ( c_Portfolio.GetCheck() == BST_CHECKED );
	c_Bond.SetCheck( BST_UNCHECKED );		// deselect the Bond check button
	
		// switch instrument types
	rateSens.instrument = Option;
	SetCheckButtons();						// restore previous check box settings - sets bPortfolio
	LoadBOSs();								// retrieve Options from the database
	LoadSymbols();							// displays the BOSs selected above too

		// are the deltas valid for an option?
	if ( m_RiskFreeRate < m_DeltaLow )
	{	m_DeltaLow = m_RiskFreeRate - 0.1f;
		setEditBox( "%g", m_DeltaLow, IDC_DeltaLow );
	}
	Plot();
}			// OnOption()
*/
//----------------------------------------------------------------------------
/*
void	CRateSensitivityExplorer::FillSlots( long* def_ID ) 
{	unsigned short	jj;
		// override CPlotOptionsDialog::FillSlots()
//	ASSERT( rateSens.instrument == Bond  ||  rateSens.instrument == Option );
	CDataManager*	dm = (CDataManager*)theApp.optMgr;
	POSITION pos = dm->GetDefIterCtx();
	while ( pos )
	{	short	firstSlot = -1;
		for ( jj = 0; jj < nBOS; jj++ )
		{	if ( *(def_ID+jj) < 0 )
			{	firstSlot = jj;
				break;
			}
		}
		if ( firstSlot < 0 )
			break;
		CString	sym;			// don't really care
		long	offset = dm->GetNextDef_ID( pos, sym );
		if (	offset >= 0
			&&  offset != *(def_ID+0)
			&&  offset != *(def_ID+1)
			&&  offset != *(def_ID+2)
			&&  offset != *(def_ID+3) )
			*(def_ID+firstSlot) = offset;
	}
}			// FillSlots()
*/
//----------------------------------------------------------------------------
