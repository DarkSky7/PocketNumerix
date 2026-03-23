// TimeDecayExplorer.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "TimeDecayExplorer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Option.h"
#include "OptionManager.h"
#include "RegistryManager.h"
//#include "Bond.h"					// for calcYears() and ActualActual
#include "FPoint.h"

/////////////////////////////////////////////////////////////////////////////
// CTimeDecayExplorer dialog

CTimeDecayExplorer::CTimeDecayExplorer( CWnd* pParent /*=NULL*/ )
	: CPlotOptionsDialog( CTimeDecayExplorer::IDD, pParent )
	, m_activeComboBox( NULL )
	, nIndexedIssues( 0 )
	, folderFptsValid( false )
{
	//{{AFX_DATA_INIT(CTimeDecayExplorer)
	m_RiskFreeRate = 3.0f;
	//}}AFX_DATA_INIT
	timeDecay.endDate = COleDateTime::GetCurrentTime();		// today
	COleDateTimeSpan	timespan( 365, 0, 0, 0 );
	timeDecay.endDate += timespan;							// plus 1 year
	copyrightOffset[0] =  5;			// All rights reserved
	copyrightOffset[1] =  3;			// PocketNumerix
	copyrightAxisRelative = false;

		// registered (saved) state
	for ( short jj = 0; jj < nBOS; jj++ )
	{	timeDecay.optChk[jj] = true;
		timeDecay.optDef_ID[jj] = -1;
		instFptsValid[jj] = false;
	}
	timeDecay.pureBS = false;
	timeDecay.optPortf = false;
}

CTimeDecayExplorer::~CTimeDecayExplorer( void )
{
//	CPlotOptionsDialog::~CPlotOptionsDialog();
}

void CTimeDecayExplorer::DoDataExchange(CDataExchange* pDX)
{
	CPlotOptionsDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimeDecayExplorer)
	DDX_Control(pDX, IDC_PureBS, c_PureBS);
	DDX_Control(pDX, IDC_Symbol1, c_Symbol1);
	DDX_Control(pDX, IDC_Symbol2, c_Symbol2);
	DDX_Control(pDX, IDC_Symbol3, c_Symbol3);
	DDX_Control(pDX, IDC_Symbol4, c_Symbol4);
	DDX_Control(pDX, IDC_Portfolio, c_Portfolio);
	DDX_Control(pDX, IDC_RiskFreeRate, c_RiskFreeRate);
	DDX_Control(pDX, IDC_EndDate, c_EndDate);
	DDX_Control(pDX, IDC_Check1, c_Check1);
	DDX_Control(pDX, IDC_Check2, c_Check2);
	DDX_Control(pDX, IDC_Check3, c_Check3);
	DDX_Control(pDX, IDC_Check4, c_Check4);
	DDX_Text(pDX, IDC_RiskFreeRate, m_RiskFreeRate);
	DDV_MinMaxFloat(pDX, m_RiskFreeRate, 1.e-002f, 9999.f);
	DDX_DateTimeCtrl(pDX, IDC_EndDate, m_EndDate);
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

BEGIN_MESSAGE_MAP(CTimeDecayExplorer, CPlotOptionsDialog)
	//{{AFX_MSG_MAP(CTimeDecayExplorer)
	ON_BN_CLICKED(IDC_Portfolio, OnPortfolio)
	ON_BN_CLICKED(IDC_Check1, OnCheck1)
	ON_BN_CLICKED(IDC_Check2, OnCheck2)
	ON_BN_CLICKED(IDC_Check3, OnCheck3)
	ON_BN_CLICKED(IDC_Check4, OnCheck4)
	ON_EN_KILLFOCUS(IDC_RiskFreeRate, OnKillFocusRiskFreeRate)
	ON_WM_PAINT()
	ON_NOTIFY(NM_KILLFOCUS, IDC_EndDate, OnKillFocusEndDate)
	ON_NOTIFY(DTN_CLOSEUP, IDC_EndDate, OnCloseUpEndDate)
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

	ON_NOTIFY(NM_SETFOCUS, IDC_EndDate, OnSetFocusEndDate)
	ON_EN_SETFOCUS(IDC_RiskFreeRate, OnSetFocusRiskFreeRate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeDecayExplorer message handlers

BOOL	CTimeDecayExplorer::OnInitDialog( void )
{
		// set the extents of plot region
	CPlotOptionsDialog ::OnInitDialog();
	RegisterHotKeys( IDC_Symbol1, IDC_Symbol2 );				// screens for WM5 internally
	plotExtents.top = 104;			// leaves enough room for the controls at the top

		// get the risk free rate from the registry
	if ( ! GetRiskFreeRate(&m_RiskFreeRate) )				// was:  theApp.regMgr->
	{
#ifdef _DEBUG
		TRACE( _T("TimeDecayExplorer::OnInitDialog: GetRiskFreeRate failed, using m_RiskFreeRate=%g.\n"),
			m_RiskFreeRate );
#endif
	}
	setEditBox( "%.3f", m_RiskFreeRate, IDC_RiskFreeRate );

		// this dialog global helps us decide whether to enable the portfolio check button
	nIndexedIssues = theApp.optMgr->NumIndexedIssues();		// numOptionsWithPositions
	c_Portfolio.EnableWindow( nIndexedIssues > 0 );

		// see if there's a registered TimeDecay structure
	if ( ! GetTimeDecay(&timeDecay) )						// was: theApp.regMgr->
	{
#ifdef _DEBUG
		TRACE( _T("TimeDecayExplorer::OnInitDialog: GetTimeDecay failed.\n") );
#endif
	}
	COleDateTime		today = COleDateTime::GetCurrentTime();
	COleDateTimeSpan	aSixth( 365 / 6, 0, 0, 0 );
	COleDateTimeSpan	fourYears( 365 * 4, 0, 0, 0 );
	COleDateTime		lowTime = today + aSixth;
	COleDateTime		highTime = today + fourYears;
	if ( timeDecay.endDate < lowTime  ||  timeDecay.endDate > highTime )
	{	COleDateTimeSpan	oneYear( 365, 0, 0, 0 );
		timeDecay.endDate = today + oneYear;
	}
	c_EndDate.SetTime( timeDecay.endDate );
	c_EndDate.SetRange( &lowTime, &highTime );					// limit the range of the EndDate control
#ifdef _DEBUG
		TRACE( _T("TimeDecayExplorer::OnInitDialog: endDate=%s\n"),
			timeDecay.endDate.Format(VAR_DATEVALUEONLY) );
#endif
	topLeft.x = 0.0f;
	bottomRight.x = 12.0f;				// we know this because we picked oneYear above

		// check the selected instrument and choose a CDataManager
	c_PureBS.SetCheck( timeDecay.pureBS  ?  BST_CHECKED  :  BST_UNCHECKED );
	SetCheckButtons();					// the five plot enablers from timeDecay	
	LoadBOSs( timeDecay.optDef_ID );	// fetch options recently evaluated
	LoadSymbols();						// load ComboBoxes & select items matching the current pOptions
	RecalcAll();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//----------------------------------------------------------------------------
HBRUSH	CTimeDecayExplorer::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	return	CPlotOptionsDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}			// OnCtlColor()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::SetCheckButtons( void ) 
{
	bool*	chkBtns = timeDecay.optChk;
	CPlotOptionsDialog::SetCheckButtons( chkBtns );

		// set Option and Portfolio buttons
	timeDecay.optPortf = timeDecay.optPortf  &&  ( nIndexedIssues > 0 );
	c_Portfolio.EnableWindow( nIndexedIssues > 0 );
	c_Portfolio.SetCheck( timeDecay.optPortf  ?  BST_CHECKED  :  BST_UNCHECKED );
}			// SetCheckButtons()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::RecalcAll( void )
{
	for ( unsigned short ii = 0; ii < nBOS; ii++ )
	{	instFptsValid[ii] = false;
		CalcInstFpts( ii );				// if okay, set instFptsValid[ii] here
	}
	folderFptsValid = false;
	CalcFolderFpts();					// if okay, set folderFptsValid here
}			// RecalcAll()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::CheckWork( CButton* chkButton, unsigned short ii )
{
	int		status = chkButton->GetCheck();
	bool*	chkState = timeDecay.optChk;
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
void	CTimeDecayExplorer::OnCheck1( void )
{
	CheckWork( &c_Check1, 0 );
}			// OnCheck1()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::OnCheck2( void )
{
	CheckWork( &c_Check2, 1 );
}			// OnCheck2()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::OnCheck3( void )
{
	CheckWork( &c_Check3, 2 );
}			// OnCheck3()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::OnCheck4( void )
{
	CheckWork( &c_Check4, 3 );
}			// OnCheck4()
//----------------------------------------------------------------------------
LRESULT		CTimeDecayExplorer::OnHotKey( WPARAM wParam, LPARAM lParam )
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
void	CTimeDecayExplorer::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	CNillaDialog::OnActivate( nState, pWndOther, bMinimized );
	if ( nState == WA_ACTIVE || nState == WA_CLICKACTIVE )
		RegisterHotKeys( IDC_Symbol1, IDC_Symbol2 );				// screens for WM5 internally
}			// OnActivate()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::SymbolWorkCore( short ii, CString sym, long* def_ID )
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
void	CTimeDecayExplorer::KillFocusSymbolWork(
	CSeekComboBox*	scb,
	short			ii,
	CString&		optSym						)
{	CString			sym;
	m_activeComboBox = NULL;
	scb->GetWindowText( sym );
	sym.MakeUpper();
	if ( sym == optSym )
		return;								// nothing to do
	optSym = sym;
	SymbolWorkCore( ii, optSym, timeDecay.optDef_ID );
}			// KillFocusSymbolWork()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::OnKillFocusSymbol1( void )
{
	KillFocusSymbolWork( &c_Symbol1, 0, m_Symbol1 );
}			// OnKillFocusSymbol1()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::OnKillFocusSymbol2( void )
{
	KillFocusSymbolWork( &c_Symbol2, 1, m_Symbol2 );
}			// OnKillFocusSymbol2()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::OnKillFocusSymbol3( void )
{
	KillFocusSymbolWork( &c_Symbol3, 2, m_Symbol3 );
}			// OnKillFocusSymbol3()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::OnKillFocusSymbol4( void )
{
	KillFocusSymbolWork( &c_Symbol4, 3, m_Symbol4 );
}			// OnKillFocusSymbol4()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::OnSetFocusSymbol1( void )
{
	m_activeComboBox = IDC_Symbol1;
}			// OnSetFocusSymbol1()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::OnSetFocusSymbol2( void )
{
	m_activeComboBox = IDC_Symbol2;
}			// OnSetFocusSymbol2()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::OnSetFocusSymbol3( void )
{
	m_activeComboBox = IDC_Symbol3;
}			// OnSetFocusSymbol3()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::OnSetFocusSymbol4( void )
{
	m_activeComboBox = IDC_Symbol4;
}			// OnSetFocusSymbol4()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::SelChangeSymbolWork(
	CSeekComboBox*	scb,
	short			ii,
	CString&		optSym						)
{	CString			sym;
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
	if ( sel < 0 ) sel = 0;					// should be superfluous
	scb->GetLBText( sel, sym );
	if ( sym == optSym )
		return;								// nothing to do
	optSym = sym;
	SymbolWorkCore( ii, optSym, timeDecay.optDef_ID );
}			// SelChangeSymbolWork
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::OnSelChangeSymbol1( void )
{
	SelChangeSymbolWork( &c_Symbol1, 0, m_Symbol1 );
}			// OnSelChangeSymbol1()
//----------------------------------------------------------------------------
void	 CTimeDecayExplorer::OnSelChangeSymbol2( void )
{
	SelChangeSymbolWork( &c_Symbol2, 1, m_Symbol2 );
}			// OnSelChangeSymbol2()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::OnSelChangeSymbol3( void )
{
	SelChangeSymbolWork( &c_Symbol3, 2, m_Symbol3 );
}			// OnSelChangeSymbol3()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::OnSelChangeSymbol4( void )
{
	SelChangeSymbolWork( &c_Symbol4, 3, m_Symbol4 );
}			// OnSelChangeSymbol4()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CTimeDecayExplorer::OnKillFocusRiskFreeRate( void ) 
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
void CTimeDecayExplorer::OnSetFocusRiskFreeRate( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusRiskFreeRate()
//----------------------------------------------------------------------------
void CTimeDecayExplorer::KillFocusEndDateWork( NMHDR* pNMHDR, LRESULT* pResult )
{
	COleDateTime	aDate;
	c_EndDate.GetTime( aDate );
	if ( timeDecay.endDate == aDate )
		return;
	timeDecay.endDate = aDate;
	RecalcAll();
	Plot();
	*pResult = 0;
}			// KillFocusEndDateWork()
//----------------------------------------------------------------------------
void CTimeDecayExplorer::OnKillFocusEndDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	KillFocusEndDateWork( pNMHDR, pResult );
}			// OnKillFocusEndDate()
//----------------------------------------------------------------------------
void CTimeDecayExplorer::OnSetFocusEndDate(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	*pResult = 0;
}			// OnSetFocusEndDate()
//----------------------------------------------------------------------------
void CTimeDecayExplorer::OnCloseUpEndDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	KillFocusEndDateWork( pNMHDR, pResult );
}			// OnCloseUpEndDate()
//----------------------------------------------------------------------------
void CTimeDecayExplorer::OnPortfolio( void )
{		// status could be BST_CHECKED, BST_UNCHECKED, or BST_INDETERMINATE
	int	status = c_Portfolio.GetCheck();
	if (								status == BST_INDETERMINATE
		 ||	 (  timeDecay.optPortf  &&  status == BST_CHECKED		  )
		 ||  ( !timeDecay.optPortf  &&  status == BST_UNCHECKED		  ) )
		return;					// nothing to do

		// recapture the state of the portfolio check button
	timeDecay.optPortf = ( status == BST_CHECKED );		// polymorphic control
	if ( ! folderFptsValid )
		CalcFolderFpts();
	Plot();
}			// OnPortfolio()
//----------------------------------------------------------------------------
void CTimeDecayExplorer::OnPureBS( void )
{		// status could be BST_CHECKED, BST_UNCHECKED, or BST_INDETERMINATE
	int	status = c_PureBS.GetCheck();
	if (							  status == BST_INDETERMINATE
		 ||	 (  timeDecay.pureBS  &&  status == BST_CHECKED		  )
		 ||  ( !timeDecay.pureBS  &&  status == BST_UNCHECKED	  ) )
		return;					// nothing to do

	timeDecay.pureBS = (status == BST_CHECKED );
	RecalcAll();
	Plot();
}			// OnPureBS()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::OnPaint( void )
{
	unsigned short ii = 0;
#ifdef _DEBUG
	TRACE( _T("TimeDecayExplorer::OnPaint: begin\n") );
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
	{	if ( InstValid(ii, timeDecay.optChk) )
		{
#ifdef _DEBUG
	TRACE( _T("TimeDecayExplorer::OnPaint: %d inst curve is valid and enabled\n"), ii );
#endif
			if ( instMinY[ii] < bottomRight.y )
				bottomRight.y = instMinY[ii];
			if ( instMaxY[ii] > topLeft.y )
				topLeft.y = instMaxY[ii];
		}
	}
		// the portfolio curve
	if ( timeDecay.optPortf  &&  folderFptsValid )
	{	if ( folderMinY < bottomRight.y )
			bottomRight.y = folderMinY;
		if ( folderMaxY > topLeft.y )
			topLeft.y = folderMaxY;
	}
		// bail out criteria
	if ( topLeft.y <= -2e38f  ||  bottomRight.y >= 2e38f )
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
	TRACE( _T("TimeDecayExplorer::OnPaint(6): topLeft.x=%g, topLeft.y=%g, bottomRight.x=%g, bottomRight.y=%g\n"),
		topLeft.x, topLeft.y, bottomRight.x, bottomRight.y );
#endif

		// scale against available space and plot
	xScale = (float)(plotExtents.Width()  / (bottomRight.x - topLeft.x));
	yScale = (float)(plotExtents.Height() / (topLeft.y - bottomRight.y));
#ifdef _DEBUG
		TRACE( _T("TimeDecayExplorer::OnPaint: xScale=%g, yScale=%g\n"), xScale, yScale );
#endif
		// paint time
	DrawGrid( dc );
	for ( ii = 0; ii < nBOS; ii++ )
	{	if ( InstValid(ii, timeDecay.optChk)  &&  instFptsValid[ii] )				// check button state examined by InstValid()
			DrawPolyline( ii, dc, (instFpts + ii*nInstTimePts), nInstTimePts );
	}
		// plot the portfolio line
	if ( timeDecay.optPortf  &&  folderFptsValid )				// portfolio plotting is enabled & the points are valid
		DrawPolyline( nBOS, dc, folderFpts, nFolderTimePts );	// nBOS --> black

	DrawGridLabels( dc );
}			// OnPaint()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::CalcInstFpts( unsigned short ii )
{		// the domain X-extents are simply from today (x=0.0) to timeDecay.endDate (x=years after today)
	if ( ! InstValid( ii, timeDecay.optChk ) )
		return;
	double yVal;
	CWaitCursor	wait;			// we could be gone a while - display the wait cursor

	long	spDays;									// ignored
	COleDateTime today = COleDateTime::GetCurrentTime();
	double	maxXval = calcYears( today, timeDecay.endDate, spDays );	// years on the X-axiz
#ifdef _DEBUG
//	TRACE( _T("TimeDecayExplorer::CalcInstFpts: today=%s, toDate=%s, maxXval=%g, spDays=%d\n"),
//		today.Format(VAR_DATEVALUEONLY), timeDecay.endDate.Format(VAR_DATEVALUEONLY), maxXval, spDays );
#endif
	double xInc = maxXval / (nInstTimePts - 1);	// years
	double xIncMonths = xInc * 12.0;
	double xVal = 0.0;								// the minimum x-value is 0 years from today
	topLeft.x = (float)xVal;						// zero * anything is still zero
	bottomRight.x = (float)(maxXval * 12.0);		// plotted values are in months

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// compute the issue plot points and the domain Y-extents
	OptionEvaluationContext	evcs;					// 02Feb07 - will be using Harvey's model
	evcs.riskFreeRate = m_RiskFreeRate / 100.0;		// unchanged within the loop
	COption* opt = (COption*) *(pBOS+ii);

		// set up for Option evaluation and subsequent rescaling operations
		// set up the Black-Scholes evaluation context
//	double	divYield = opt->DivYield();
//	double	yrsToExpiry = opt->YrsToExpiry( today );
//	optRefVal = opt->EuroValue( yrsToExpiry, riskFreeRate, divYield );		// Wilmott's continuous yield approach
		// replace Wilmott's approach (above) with Harvey's (below)
	evcs.fromDate = today;													// changes within the for loop
	evcs.sigma = opt->getVolatility();										// pulls through from the stock
	evcs.stockPrice = opt->getStockPrice();									// ditto
	evcs.calcYrsToExpiry = true;
	evcs.calcPvDivs = true;
	double optRefVal;
	if ( ! opt->EuroValue( evcs, optRefVal ) )								// pvDivs approach
		return;
	double refPrice = timeDecay.pureBS  ?  optRefVal  :  opt->getMktPrice();

		// collect Y-values (instFpts[ii*nTimeBOS+0] is always (0.0, 0.0) in pureBS mode
		// the following loop could start at jj=1 if ...
		// the previous three lines were compiled AND pureBS == true
		// its simpler to potentially waste the first computation (when pureBS is true)
	instMinY[ii] = 2e38f;			// dialog var
	instMaxY[ii] = -2e38f;			// dialog var
	CFPoint* instPts = instFpts + ii * nInstTimePts;
	COleDateTimeSpan ts( 365.25 * maxXval / nInstTimePts );			// COleDateTimeSpans are in days, want years
	for ( short jj = 0; jj < nInstTimePts; jj++ )					// for each of the FPoints
	{		// rescale xVal from percent to a fractional value before calculating Black-Scholes value
			// convert Black-Scholes value to a fraction of the Option's referece value using Harvey's model
			// adjust evcs.fromDate, evcs.yrsToExpiry & evcs.calcPvDivs at the end of the loop
		yVal = 0.0;
		if ( fabs(refPrice) >= 1e-3 )
		{	double bsVal;
			if ( jj == 0 )
				bsVal = optRefVal;							// no sense computing the same value twice
			else
			{	if ( ! opt->EuroValue( evcs, bsVal ) )
					return;
			}
			yVal = 100 * ( bsVal - refPrice ) / refPrice;
		}
#ifdef _DEBUG
//		TRACE( _T("TimeDecayExplorer::CalcInstFpts: (bsVal=%g - refPrice=%g) / optRefVal=%g --> yVal=%g @ yrsToExpiry=%g\n"),
//			bsVal, refPrice, optRefVal, yVal, evcs.yrsToExpiry );
#endif
			// update min and max Y-values
		if ( yVal < instMinY[ii] )
			instMinY[ii] = (float)yVal;
		if ( yVal > instMaxY[ii] )
			instMaxY[ii] = (float)yVal;

		CFPoint* cfp = instPts + jj;
		cfp->x = (float)xVal;					// xVal is in months
		cfp->y = (float)yVal;					// store the rescaled Y-value (percentages)
#ifdef _DEBUG
//		TRACE( _T("TimeDecayExplorer::CalcInstFpts: cfp->x=%g, cfp->y=%g\n"), cfp->x, cfp->y );
#endif
			// prepare for the next point
		xVal += xIncMonths;

			// we'll adjust evcs.fromDate, evcs.yrsToExpiry & evcs.calcPvDivs at the end of the loop
		evcs.yrsToExpiry -= xInc;								// xInc is in years (can become negative)
		evcs.fromDate += ts;
#ifdef _DEBUG
//		TRACE( _T("TimeDecayExplorer::CalcInstFpts: jj=%d, fromDate=%s, yrsToExpiry=%g\n"),
//			jj, evcs.fromDate.Format(VAR_DATEVALUEONLY), evcs.yrsToExpiry );
#endif
		evcs.calcPvDivs = true;									// affects the next option valuation
	}
	instFptsValid[ii] = true;
#ifdef _DEBUG
//	TRACE( _T("TimeDecayExplorer::CalcInstFpts: instFptsValid[%d]=%s\n"), ii, ( instFptsValid[ii] ? _T("true") : _T("false") ) );
#endif
}			// CalcInstFpts()
//----------------------------------------------------------------------------
void	CTimeDecayExplorer::CalcFolderFpts( void )
{		// compute the folder's rate sensitivity
		// overwrite either optionFolderPts with results (instrument decides which)
	long	spDays;														// ignored
	CWaitCursor	wait;			// we could be gone a while - display the wait cursor
	COleDateTime today = COleDateTime::GetCurrentTime();
	double	maxXval = calcYears( today, timeDecay.endDate, spDays );	// years on the X-axiz

		// initialize the portfolio points
	long	ii, jj;
	double xIncYrs = maxXval / (double)(nFolderTimePts - 1);	// in years
	double xIncMonths = xIncYrs * 12.0;							// in months
	double cfpY[ nFolderTimePts ];
	double xVal = 0.0;											// the lowest X-value (modified in the for loop)
	for ( jj = 0; jj < nFolderTimePts; jj++ )
	{	cfpY[jj] = 0.0;											// working array contributes to folderFpts below
		(folderFpts + jj)->x = (float)xVal;						// dialog instance variable result
		xVal += xIncMonths;										// months
	}
	xVal = 0.0;													// restore the starting X-value

	COption*	opt;
	double		folderCost = 0.0;								// all option positions
	double		yVal, optRefVal;
	OptionEvaluationContext	evcs;								// 02Feb07 - going back to Harvey's model
	evcs.riskFreeRate = m_RiskFreeRate / 100.0;					// 02Feb07 - unchanged within the loop

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
			TRACE( _T("TimeDecayExplorer::CalcFolderFpts: %s has no position list\n"), opt->getSymbol() );
			AfxDebugBreak();
#endif
			delete	opt;	// here we're pulling options out of the database, not hanging on to them in the dialog
			continue;
		}
#ifdef _DEBUG
		TRACE( _T("TimeDecayExplorer::CalcFolderFpts: Processing position(s) for %s\n"), opt->getSymbol() );
#endif
			// compute the option's reference Y-value
			// set up the Black-Scholes evaluation context for Harvey's PV(dividends) model
		evcs.fromDate = today;
		evcs.sigma = opt->getVolatility();										// 02Feb07
		evcs.stockPrice = opt->getStockPrice();									// 02Feb07
		evcs.calcYrsToExpiry = true;
		evcs.calcPvDivs = true;
		if ( ! opt->EuroValue( evcs, optRefVal ) )								// pvDivs approach
			return;
		double	refPrice = timeDecay.pureBS  ?  optRefVal  :  opt->getMktPrice();
#ifdef _DEBUG
//		TRACE( _T("TimeDecayExplorer::CalcFolderFpts: %g <- %s(stkPrc=%g, sig=%g, refYrs=%g, rfr=%g, pvDivs=%g)\n"),
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
		TRACE( _T("TimeDecayExplorer::CalcFolderFpts: %s has basis=%.2f\n"), opt->getSymbol(), issueBasis );
#endif
			// update the Y-values (optionFolderPts[0] is (0.0, 0.0 ) by definition
		COleDateTimeSpan ts( 365.25 * maxXval / nFolderTimePts );			// COleDateTimeSpans are in days, want years
		for ( jj = 0; jj < nFolderTimePts; jj++ )			// for each of the FPoints
		{		// convert Black-Scholes value to a percent change from the Option's reference value
			yVal = 0.0;										// in case the if condition is false
			if ( fabs(refPrice) > 1e-3 )
			{	double	bsVal;
				if ( ! opt->EuroValue( evcs, bsVal ) )
					return;
				yVal = issueBasis * ( bsVal - refPrice ) / refPrice;
			}
				// upscale the Y-value by the total cost (issueBasis) of the issue's positions 
			*(cfpY+jj) += yVal;				// update the Y-value for this position

				// prepare for next pass
			evcs.yrsToExpiry -= xIncYrs;	// reduce time to expiry - calcYrsToExpiry should be false by now
			evcs.fromDate += ts;			// move forward in time
			evcs.calcPvDivs = true;			// recalculate pvDivs on the next pass through the loop (i.e. at every time point)
		}									// for each point
		folderCost += issueBasis;			// this will be our descaling factor
	}										// for each issue
#ifdef _DEBUG
	TRACE( _T("TimeDecayExplorer::CalcFolderFpts: %d options have aggregate basis=%.2f\n"),
		nIndexedIssues, folderCost );
#endif

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// post issue-loop downscaling by folderCost, simultaneously upscaling to percent
	folderMinY = 1e38f;								// a big positive number
	folderMaxY = -1e38f;								// a big negative number
	for ( jj = 0; jj < nFolderTimePts; jj++ )
	{		// rescale the yVal from a fractional value to percent
		double	syVal = (fabs(folderCost) < 0.001)  ?  0.0  :  (100.0 * cfpY[jj] / folderCost);
		if ( syVal < folderMinY )
			folderMinY = (float)syVal;				// percent

		if ( syVal > folderMaxY )
			folderMaxY = (float)syVal;				// percent

		(folderFpts + jj)->y = (float)syVal;			// the rescaled Y-value (cost weighted, percent change)
#ifdef _DEBUG
//		TRACE( _T("TimeDecayExplorer::CalcFolderFpts: cfp->x=%g, cfp->y=%g\n"),
//			(optionFolderPts + jj)->x, (optionFolderPts + jj)->y );
#endif
	}
	folderFptsValid = true;
}			// CalcFolderFpts()
//----------------------------------------------------------------------------
/*
void	CTimeDecayExplorer::OnPaint( void )
{	COption* opt;
	long	ii, jj;										// most of the time shorts would work
	bool	bosValid[ nBOS ];
	bool	somethingValid = false;
	for ( jj = 0; jj < nBOS; jj++ )
	{	*(bosValid+jj) = InstValid( jj, timeDecay.optChk );
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
#ifdef _DEBUG
	TRACE( _T("TimeDecayExplorer::OnPaint(4): minYval=%g, optionFolderYmin=%g, maxYval=%g, maxXval=%g spDays=%d\n"),
		minYval, optionFolderYmin, maxYval, maxXval, spDays );
#endif
	optionPtsAreValid = optionPtsAreValid  &&  minYval <= optionFolderYmin  &&  maxYval >= optionFolderYmax;
	if ( timeDecay.optPortf  &&  !optionPtsAreValid  &&  nIndexedIssues > 0 )
	{		// the check box is checked, there are issues with positions
			// but the portfolio plot points are not valid
			// minYval & maxYval are updated within CalcFolderPts()
		CalcFolderPts( maxXval );					// also sets optionPtsAreValid, optionFolderYmin, optionFolderYmax
		if ( optionFolderYmin < minYval )
			minYval = optionFolderYmin;
		if ( optionFolderYmax > maxYval )
			maxYval = optionFolderYmax;
	}
	if ( maxYval < 0.0 )
		maxYval = 0.0;

	double	yRange = maxYval - minYval;					// percent
	topLeft.y = (float)(maxYval + 0.08 * yRange);
	bottomRight.y = (float)(minYval - 0.12 * yRange);
#ifdef _DEBUG
	TRACE( _T("TimeDecayExplorer::OnPaint(6): topLeft.x=%g, topLeft.y=%g, bottomRight.x=%g, bottomRight.y=%g\n"),
		topLeft.x, topLeft.y, bottomRight.x, bottomRight.y );
#endif

		// scale against available space and plot
	xScale = (float)(plotExtents.Width()  / (bottomRight.x - topLeft.x));		// used by MapToPlot
	yScale = (float)(plotExtents.Height() / (topLeft.y - bottomRight.y));		// ditto
#ifdef _DEBUG
	TRACE( _T("TimeDecayExplorer::OnPaint(7): xScale=%g, yScale=%g\n"), xScale, yScale );
#endif

		// now we can paint the four Option instances
	DrawGrid( dc );
	for ( ii = 0; ii < nBOS; ii++ )
	{	if ( *(bosValid+ii) )								// check button state already examined by InstValid()
			DrawPolyline( ii, dc, (instFpts + ii*nTimePts), nTimePts );
	}
		// plot the portfolio line
	if ( timeDecay.optPortf  &&  optionPtsAreValid )		// portfolio plotting is enabled & the points are valid
		DrawPolyline( nBOS, dc, optionFolderPts, nTimeFolderPts );
	DrawGridLabels( dc );
}			// OnPaint()
*/
//----------------------------------------------------------------------------
