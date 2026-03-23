// BondAnalyzerDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "NillaDialog.h"
#include "BondAnalyzerDialog.h"
#include "BondManager.h"
#include "RegistryManager.h"
#include "Bond.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "BondManagerApp.h"

extern CBondManagerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBondAnalyzerDialog dialog

CBondAnalyzerDialog::CBondAnalyzerDialog( CWnd* pParent /* =NULL */)
	: CNillaDialog( IDD, pParent)
	, m_activeComboBox( NULL )					// HotKeys
	, criticalRegion( false )
{
	//{{AFX_DATA_INIT(CBondAnalyzerDialog)
	m_BondSymbol = _T("");
	m_MarketPrice = 0.0;
	m_CouponStartDate = COleDateTime::GetCurrentTime();
	m_CouponEndDate = COleDateTime::GetCurrentTime();
	m_DeltaYTM = 0.0f;
	//}}AFX_DATA_INIT
	theBond = NULL;
	m_WhatIfYTM = 0.0f;

	evalCtx.yrsToMaturityIsValid = false;
	evalCtx.ytmIsValid = false;
	evalCtx.macDurIsValid = false;
	evalCtx.cvxtyIsValid = false;
	evalCtx.fromDate = COleDateTime::GetCurrentTime();
}

CBondAnalyzerDialog::~CBondAnalyzerDialog( void )
{	delete	theBond;
	theBond = NULL;
}

void CBondAnalyzerDialog::DoDataExchange(CDataExchange* pDX)
{
	CNillaDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBondAnalyzerDialog)
	DDX_Control(pDX, IDC_CouponEndDate, c_CouponEndDate);
	DDX_Control(pDX, IDC_CouponStartDate, c_CouponStartDate);
	DDX_Control(pDX, IDC_CouponsPerYear, c_CouponsPerYear);
	DDX_Control(pDX, IDC_BondSymbol, c_BondSymbol);
	DDX_Control(pDX, IDC_MarketPrice, c_MarketPrice);
	DDX_Control(pDX, IDC_DeltaYTM, c_DeltaYTM);
	DDX_CBString(pDX, IDC_BondSymbol, m_BondSymbol);
	DDV_MaxChars(pDX, m_BondSymbol, 254);
	DDX_Text(pDX, IDC_MarketPrice, m_MarketPrice);
	DDV_MinMaxDouble(pDX, m_MarketPrice, 0., 1.e+038);
	DDX_DateTimeCtrl(pDX, IDC_CouponStartDate, m_CouponStartDate);
	DDX_DateTimeCtrl(pDX, IDC_CouponEndDate, m_CouponEndDate);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBondAnalyzerDialog, CNillaDialog)
	//{{AFX_MSG_MAP(CBondAnalyzerDialog)
	ON_EN_KILLFOCUS(IDC_MarketPrice, OnKillFocusMarketPrice)
	ON_EN_SETFOCUS(IDC_MarketPrice, OnSetFocusMarketPrice)
	ON_NOTIFY(NM_KILLFOCUS, IDC_CouponStartDate, OnKillFocusCouponStartDate)
	ON_NOTIFY(NM_KILLFOCUS, IDC_CouponEndDate, OnKillFocusCouponEndDate)
	ON_NOTIFY(DTN_CLOSEUP, IDC_CouponEndDate, OnCloseUpCouponEndDate)
	ON_NOTIFY(DTN_CLOSEUP, IDC_CouponStartDate, OnCloseUpCouponStartDate)
	ON_WM_CTLCOLOR()

	ON_CBN_SELCHANGE(IDC_DeltaYTM, OnSelChangeDeltaYTM)
	ON_CBN_SELCHANGE(IDC_BondSymbol, OnSelChangeBondSymbol)
	ON_CBN_KILLFOCUS(IDC_BondSymbol, OnKillFocusBondSymbol)		// HotKeys +
	ON_CBN_KILLFOCUS(IDC_DeltaYTM, OnKillFocusDeltaYTM)			// HotKeys +
	ON_CBN_SETFOCUS(IDC_BondSymbol, OnSetFocusBondSymbol)		// HotKeys
	ON_CBN_SETFOCUS(IDC_DeltaYTM, OnSetFocusDeltaYTM)			// HotKeys
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()											// HotKeys
	ON_MESSAGE(WM_HOTKEY, OnHotKey)								// HotKeys

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBondAnalyzerDialog message handlers

BOOL	CBondAnalyzerDialog::OnInitDialog() 
{
	CNillaDialog::OnInitDialog();
	RegisterHotKeys( IDC_BondSymbol, IDC_DeltaYTM );	// screens for WM5 internally

		// see if we can override the default value (0.0) for DeltaYTM
	if ( ! GetDeltaYTM( &m_DeltaYTM ) )
	{
#ifdef _DEBUG
		TRACE( _T("BondAnalyzerDialog::OnInitDialog: no registered value for DeltaYTM.\n") );
#endif
	}
			// display the DeltaYTM
	c_DeltaYTM.SetFloat( m_DeltaYTM );
//	setComboBox( "%g", m_DeltaYTM, IDC_DeltaYTM );
//	c_DeltaYTM.SetEditSel( -1, 0 );

	setStatic( "", IDC_WhatIfYTMResult );					// clear it, just in case

		// load up the ComboBox from the bonds map (the pDB approach)
	CMapStringToPtr*	theMap = theApp.bndMgr->GetSymbolTable();

		// situations:  MainFrame presets m_BondSymbol - overriding everything else
		//				there's a RecentBond - use that
		//				if no other option - use the first symbol in the list
		// allow for preloading a selection into the bond symbol ComboBox
	if ( theMap  &&  theMap->GetCount() > 0 )
	{		// what if a bond symbol was not preloaded?
		c_BondSymbol.LoadMap( theMap );
		if ( m_BondSymbol == _T("") )
		{		// no MainFrame override...
				// see if there's a registered Recent Bond
			long	def_ID;
			bool	res = GetRecentBond( &def_ID );
			if ( res  &&  def_ID >= 0 )							// success
			{	theBond = theApp.bndMgr->ReadBond( def_ID );
				m_BondSymbol = theBond->getSymbol();
			}
		}
		else
		{		// MainFrame m_BondSymbol override 
				// also serves as a continueation of continuation of == _T("") above
			int	 indx = c_BondSymbol.FindStringExact( 0, m_BondSymbol );
			if ( indx != CB_ERR )
			{	c_BondSymbol.SetCurSel( indx );
				theBond = theApp.bndMgr->GetBond( m_BondSymbol );
			}
		}
		if ( m_BondSymbol == _T("") )
		{		// no override and no RecentBond, pick the first symbol
			c_BondSymbol.GetLBText( 0, m_BondSymbol );
			theBond = theApp.bndMgr->GetBond( m_BondSymbol );
		}
		else if ( theBond )
		{		// just to be sure...
			int	indx = c_BondSymbol.FindStringExact( 0, m_BondSymbol );
			if ( indx != CB_ERR )
				c_BondSymbol.SetCurSel( indx );
				// why we're really here
			LoadTheBond();
		}
			// convince KillFocusBondSymbolWork() that the BondSymbol has changed
//		KillFocusBondSymbolWork();
	}
	else
		c_BondSymbol.EnableWindow( FALSE );

		// Note, we still haven't formatted default values in the CEdit boxes
	return	TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//----------------------------------------------------------------------------------------
void	CBondAnalyzerDialog::OnKillFocusBondSymbol( void )
{	CString sym;
	m_activeComboBox = NULL;
//	SHSipPreference( GetSafeHwnd(), SIP_DOWN );		// not posting the SIP in the BAD
	if ( criticalRegion )
		return;										// ignore KillFocus events when SaveIfChanged is posting a confirm save dialog
		// Did user select an existing value or create a new one?
	c_BondSymbol.GetWindowText( sym );
	KillFocusBondSymbolWork( sym );
}			// OnKillFocusBondSymbol()
//----------------------------------------------------------------------------------------
void	CBondAnalyzerDialog::OnSetFocusBondSymbol( void )
{
	m_activeComboBox = IDC_BondSymbol;
//	SHSipPreference( GetSafeHwnd(), SIP_UP );			// faster to select from the list
}			// OnSetFocusBondSymbol()
//----------------------------------------------------------------------------
void	CBondAnalyzerDialog::KillFocusBondSymbolWork( CString sym )
{		// switch to the new bond, load needed values and update the controls
		// we'll communicate theBond dependent values and setup to evaluate theBond,
		// but we'll leave the details of updating the statics to
		// UpdateStatics() and UpdatePresentValueCoupons()
	if ( sym == m_BondSymbol )
		return;
#ifdef _DEBUG
	TRACE( _T("BondAnalyzerDialog::KillFocusBondSymbolWork: sym=%s, m_BondSymbol=%s\n"),
			sym, m_BondSymbol );
#endif
		// symbol has changed, save (approved) changes
	SaveIfChanged( true );							// just has to look at MarketPrice
	m_BondSymbol = sym;

		// save a little time if OnInitDialog() fetched theBond for us
	if ( theBond == NULL  &&  m_BondSymbol != _T("") )
		theBond = theApp.bndMgr->GetBond( m_BondSymbol );
	else if ( theBond  &&  theBond->getSymbol() != m_BondSymbol )
	{	delete	theBond;
		theBond = theApp.bndMgr->GetBond( m_BondSymbol );
	}

		// still NULL after looking up m_BondSymbol?
	if ( theBond == NULL )
	{	ClearStatics();
		ResetControls();
		return;
	}
	LoadTheBond();
		// NOTE:  we're saving theBond for subsequent calls to UpdateStatics()
		//		  which could get multiple calls with the same bond
}			// KillFocusBondSymbolWork()
//----------------------------------------------------------------------------------------
void	CBondAnalyzerDialog::LoadTheBond( void )
{		// load dialog variables from theBond
	ASSERT( theBond );
#ifdef _DEBUG
	TRACE( _T("BondAnalyzerDialog::LoadTheBond: theBond->symbol=%s, m_BondSymbol=%s\n"),
			theBond->getSymbol(), m_BondSymbol );
#endif
		// re-enable market price (Edit box) and update the GUI
	m_MarketPrice = theBond->getMktPrice();
	c_MarketPrice.EnableWindow( true );
	setEditBox( "%.2f", m_MarketPrice, IDC_MarketPrice );

		// Coupon Start Date can't be less than incepDate
	m_CouponStartDate = COleDateTime::GetCurrentTime();							// current time or incepDate ?
	c_CouponStartDate.SetTime( m_CouponStartDate );
	c_CouponStartDate.SetRange( &theBond->getIncepDate(), &theBond->getMaturityDate() );

		// Coupon End Date can't be after than maturityDate
	m_CouponEndDate = theBond->getMaturityDate();
	c_CouponEndDate.SetTime( m_CouponEndDate );
	c_CouponEndDate.SetRange( &theBond->getIncepDate(), &theBond->getMaturityDate() );

		// disallow the Coupon Calculator's dates when there are no coupons
	bool thereAreCoupons = (theBond->getCouponRate() > 0.0  &&  theBond->getCouponsPerYear() > 0);
	c_CouponStartDate.EnableWindow( thereAreCoupons );
	c_CouponEndDate.EnableWindow( thereAreCoupons );

		// coupon rate (Static)
		// coupons per year (Static)
	wchar_t	buf[64];
	swprintf( buf, _T("%d Cpns/Yr"), theBond->getCouponsPerYear() );
	c_CouponsPerYear.SetWindowText( buf );
	setStatic( "@ %.3f %%", 100.0 * theBond->getCouponRate(), IDC_CouponRate );

		// compute analytics (incl. fixing up WhatIfYTM) and display results
	evalCtx.cvxtyIsValid = false;
	evalCtx.macDurIsValid = false;
	evalCtx.yrsToMaturityIsValid = false;
	evalCtx.ytmIsValid = false;
	UpdateStatics();
	UpdatePresentValueCoupons();
}			// LoadTheBond()
//----------------------------------------------------------------------------------------
HBRUSH	CBondAnalyzerDialog::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	return	CNillaDialog::OnCtlColor( pDC, pWnd, nCtlColor );
}			// OnCtlColor()
//----------------------------------------------------------------------------------------
void	CBondAnalyzerDialog::ClearStatics( void )
{
	setStatic( "", IDC_CurrentYieldResult );
	setStatic( "", IDC_YieldToMaturityResult );
	setStatic( "", IDC_MacaulayDurationResult );
	setStatic( "", IDC_ModifiedDurationResult );
	setStatic( "", IDC_ConvexityResult );
	setStatic( "", IDC_ImputedMktPriceResult );
	setStatic( "", IDC_WhatIfYTMResult );
}			// ClearStatics()
//----------------------------------------------------------------------------------------
void CBondAnalyzerDialog::KillFocusCouponStartDateWork( NMHDR* pNMHDR, LRESULT* pResult )
{
	COleDateTime	aDate;
	c_CouponStartDate.GetTime( aDate );
	if ( m_CouponStartDate == aDate )
		return;
	m_CouponStartDate = aDate;

		// constrain the CouponEndDate
	c_CouponEndDate.SetRange( &theBond->getIncepDate(), &m_CouponEndDate );
	
	UpdatePresentValueCoupons();

	*pResult = 0;
}			// KillFocusCouponStartDateWork()
//----------------------------------------------------------------------------------------
void CBondAnalyzerDialog::OnKillFocusCouponStartDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	KillFocusCouponStartDateWork( pNMHDR, pResult );
}			// OnKillFocusCouponStartDate()
//----------------------------------------------------------------------------------------
void CBondAnalyzerDialog::OnCloseUpCouponStartDate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	KillFocusCouponStartDateWork( pNMHDR, pResult );
}			// OnCloseUpCouponStartDate()
//----------------------------------------------------------------------------------------
void CBondAnalyzerDialog::KillFocusCouponEndDateWork( NMHDR* pNMHDR, LRESULT* pResult )
{
	COleDateTime	aDate;
	c_CouponEndDate.GetTime( aDate );
	if ( m_CouponEndDate == aDate )
		return;
	m_CouponEndDate = aDate;

		// constrain the CouponStartDate
	c_CouponEndDate.SetRange( &m_CouponStartDate, &theBond->getMaturityDate() );
	
	UpdatePresentValueCoupons();

	*pResult = 0;
}			// KillFocusCouponEndDateWork()
//----------------------------------------------------------------------------------------
void CBondAnalyzerDialog::OnKillFocusCouponEndDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	KillFocusCouponEndDateWork( pNMHDR, pResult );
}			// OnKillFocusCouponEndDate()
//----------------------------------------------------------------------------------------
void CBondAnalyzerDialog::OnCloseUpCouponEndDate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	KillFocusCouponEndDateWork( pNMHDR, pResult );
}			// OnCloseUpCouponEndDate()
//----------------------------------------------------------------------------------------
void	CBondAnalyzerDialog::ResetControls( void )
{
	c_MarketPrice.SetWindowText( _T("") );

	c_MarketPrice.EnableWindow( false );
}			// ResetControls()
//----------------------------------------------------------------------------------------
void	CBondAnalyzerDialog::SaveIfChanged( bool updateGUI /* = false */ )
{		// no prior value of m_BondSymbol, so nothing to look up
	CBond*	aBond = NULL;
	short	res;
	short	prefs;
	bool	saveVerify;

		// also return if bond exists, but marketPrice is unchanged
	if (	  m_BondSymbol == _T("")
		  ||  ( theBond != NULL  &&  fabs(m_MarketPrice - theBond->getMktPrice()) < 1e-4 ) )
		goto	Exit;
#ifdef _DEBUG
	TRACE( _T("BondAnalyzerDialog::SaveIfChanged: m_BondSymbol=%s\n"), m_BondSymbol );
#endif
		// to be thread safe, we fetch a fresh copy of theBond for MainFrame calls
		// true -> theBond is valid, false -> it might not be
	aBond = updateGUI  ?  theBond  :  theApp.bndMgr->GetBond( m_BondSymbol );
	if ( aBond == NULL  ||  m_MarketPrice == aBond->getMktPrice() )
	{
#ifdef _DEBUG
		{	long	def_ID = NULL;
			float	mktPrc = -1.0;
			if ( aBond )
			{	def_ID = aBond->getDef_ID();
				mktPrc = aBond->getMktPrice();
			}
			TRACE( _T("BondAnalyzerDialog::SaveIfChanged: aBond->def_ID=%d, aBond->mktPrice=%g, m_MarketPrice=%g\n"),
					def_ID, mktPrc, m_MarketPrice );
		}
#endif
		goto	Exit;
	}
	saveVerify = true;
	prefs = AnalyzerChangeSymbol | AnalyzerClose;
	if ( GetSaveVerifyPrefs( &prefs ) )
	{		// updateGUI tells us whether this is a close or change symbol action
			// updateGUI should be true from KillFocusBondSymbolWork, representing a change symbol action
			//			 and false from MainFrame, representing a close dialog action
		short	mask = updateGUI ? AnalyzerChangeSymbol : AnalyzerClose;
		saveVerify = (prefs & mask) != 0;
	}
	if ( saveVerify )
	{		// get permission to update the Bond definition...
		wchar_t	buf[320];
		swprintf( buf, _T("Update the market price for '%s'?"),
					m_BondSymbol );
		criticalRegion = true;
		UINT result = MessageBox( buf, _T("Save changes?"),
						MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST );
		criticalRegion = false;
		if ( result != IDYES )
			goto	Exit;				// user wants to skip the update
	}
#ifdef _DEBUG
	TRACE( _T("BondAnalyzerDialog::SaveIfChanged: setting MarketPrice=%g\n"), m_MarketPrice );
#endif
	aBond->setMktPrice( m_MarketPrice );
	res = theApp.bndMgr->WriteBOS( (CBOS*)aBond );		// we're just updating the mktPrice
#ifdef _DEBUG
	if ( res != 0 )
		TRACE( _T("BondAnalyzerDialog::SaveIfChanged: Bond write failed, res=%d\n"), res );
#endif
Exit:
		// calls from MainFrame (updateGUI == false), fetch a fresh copy from pDB
	if ( ! updateGUI )					// indicates that aBond is not an alias for theBond
		delete	aBond;					// but was freshly allocated on the heap
}			// SaveIfChanged()
//----------------------------------------------------------------------------------------
void	CBondAnalyzerDialog::UpdateStatics( void )
{
	if ( theBond == NULL )
		return;
#ifdef _DEBUG
	TRACE( _T("BondAnalyzerDialog::UpdateStatics: theBond->symbol=%s\n"), theBond->getSymbol() );
#endif
		// make a copy we can modify
	CBond	aBond( *theBond );

		// override theBond's market price in our local copy
	aBond.setMktPrice( m_MarketPrice );

		// show current yield (%)
	double yy = aBond.currentYield();
	setStatic( "%.3f", 100.0 * yy, IDC_CurrentYieldResult );
	
		// we're certainly not using the evalCtx to our advantage here
		// the tolerance here is in the predicted market price
		// a tolerance going in, the absolute error coming out
	aBond.YieldToMaturity( evalCtx );
	if ( fabs(evalCtx.ytmErr) > CBond::GetErrTol() )	// CBond's default errTol in the YTM
	{
		setStatic( "~", IDC_CurrentYieldResult );
		setStatic( "~", IDC_YieldToMaturityResult );
		setStatic( "~", IDC_MacaulayDurationResult );
		setStatic( "~", IDC_ModifiedDurationResult );
		setStatic( "~", IDC_ConvexityResult );
		setStatic( "~", IDC_ImputedMktPriceResult );
		setStatic( "~", IDC_WhatIfYTMResult );
		evalCtx.ytmIsValid = false;
		return;
	}
		// WhatIfYTM (as a percentage) ...
	double	fracDeltaYTM = m_DeltaYTM / 100.0;
	m_WhatIfYTM = (float)(fracDeltaYTM + evalCtx.ytm);
	setStatic( "%.3f", 100.0f * m_WhatIfYTM, IDC_WhatIfYTMResult );

/*		// the old way ...
	double	d1 = fracDeltaYTM / ( 1.0 + ytm );
		// deltaP / P = -modDur * d1 + convexity * d1 * d1
	double	deltaPdivP = d1 * (convexity * d1 - modDur);
	double	imputedPrice = m_MarketPrice * ( 1.0 + deltaPdivP );
*/
		// this is Fabozzi's approach...
		// deltaPdivP( evalCtx ) sets ytm, macDur & cvxty
	double	deltaPdivP = aBond.DeltaPdivP( evalCtx, fracDeltaYTM );
	double	imputedPrice = m_MarketPrice * ( 1.0 + deltaPdivP );
	setStatic( "%.3f", imputedPrice, IDC_ImputedMktPriceResult );
	setStatic( "%.3f", 100.0 * evalCtx.ytm, IDC_YieldToMaturityResult );

	double	modDur = aBond.ModifiedDuration( evalCtx );
	setStatic( "%.3f", modDur, IDC_ModifiedDurationResult );
	setStatic( "%.3f", evalCtx.macDur, IDC_MacaulayDurationResult );
	setStatic( "%.3f", evalCtx.cvxty, IDC_ConvexityResult );
}			// UpdateStatics()
//----------------------------------------------------------------------------------------
void	CBondAnalyzerDialog::UpdatePresentValueCoupons( void )
{		// update the static IDC_CouponPresentValueResult
	double	pvCpns = theBond->PresentValueCoupons( m_CouponStartDate, m_CouponEndDate, m_WhatIfYTM );
	setStatic( "%.3f", pvCpns, IDC_PresentValueCouponResult );

	double	pvPar = theBond->PresentValuePar( m_CouponStartDate, m_WhatIfYTM );
	setStatic( "%.3f", pvPar, IDC_PresentValueParResult );
}			// UpdatePresentValueCoupons()
//----------------------------------------------------------------------------------------
void	CBondAnalyzerDialog::OnKillFocusMarketPrice( void ) 
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_MarketPrice );
	if ( ff == m_MarketPrice )
		return;
	m_MarketPrice = ff;
	evalCtx.cvxtyIsValid = false;
	evalCtx.macDurIsValid = false;
	evalCtx.yrsToMaturityIsValid = false;
	evalCtx.ytmIsValid = false;
	UpdateStatics();
	UpdatePresentValueCoupons();
}			// OnKillFocusMarketPrice()
//----------------------------------------------------------------------------------------
void CBondAnalyzerDialog::OnSetFocusMarketPrice()
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	c_MarketPrice.SetSel(0,-1);
}			// OnSetFocusMarketPrice()
//----------------------------------------------------------------------------------------
void	CBondAnalyzerDialog::KillFocusDeltaYTMwork()
{		// DeltaYTM expressed as a percentage
	float	ff;
	if ( getComboBoxFloat(IDC_DeltaYTM,ff) != 1  ||  ff == m_DeltaYTM )
		return;
	m_DeltaYTM = ff;

	UpdateStatics();					// really only Imputed Market Value changes
	UpdatePresentValueCoupons();

		// the time between SetCurSel() and here is needed to prevent
		// setComboBox from getting blocked
	setComboBox( "%g", m_DeltaYTM, IDC_DeltaYTM );
}			// KillFocusDeltaYTMwork()
//----------------------------------------------------------------------------------------
void	CBondAnalyzerDialog::OnKillFocusDeltaYTM()
{
	m_activeComboBox = NULL;
//	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	KillFocusDeltaYTMwork();
}			// OnKillFocusDeltaYTM()
//----------------------------------------------------------------------------------------
void	CBondAnalyzerDialog::OnSetFocusDeltaYTM( void )
{
	m_activeComboBox = IDC_DeltaYTM;
//	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusDeltaYTM()
//----------------------------------------------------------------------------------------
void CBondAnalyzerDialog::OnSelChangeDeltaYTM( void )
{	float	ff0, ebff, ffn;
	int	cnt = c_DeltaYTM.GetCount();
	if ( cnt < 1 )
		return;
	int		ebii;										// this is what we use when LB Closeup happened first
	if ( c_DeltaYTM.lastChar == VK_DOWN  ||  c_DeltaYTM.lastChar == VK_UP )
	{		// here we ignore the LB selection and start from the EditBox value
		if ( c_DeltaYTM.GetFloat(ebff) != 1 )			// the EditBox float
			return;
		if (	 c_DeltaYTM.GetFloat(    0,ff0 ) != 1		// the first ListBox float
			 ||  c_DeltaYTM.GetFloat(cnt-1,ffn ) != 1 )		// the last  ListBox float
			return;

		float	step  = ( ffn - ff0) / (cnt-1);				// eleven entries --> 10 steps
		float	ebIdx = (ebff - ff0) / step;				// a ListBox equivalent 'index'
		double	frac = fabs( ebIdx - floor( ebIdx ) );
		bool	approxLBmatch = ( frac < 1e-9 );
		if ( c_DeltaYTM.lastChar == VK_DOWN )				// towards higher values
		{	ebii = (int)ceil( ebIdx );
			if ( approxLBmatch )
				ebii++;
			if ( ebii >= cnt )
				ebii = 0;
		}
		else	// if ( c_DeltaYTM.lastChar == VK_UP )		// towards lower values
		{	ebii = (int)floor( ebIdx );
			if ( approxLBmatch )
				ebii--;
			if ( ebii < 0 )
				ebii = cnt - 1;
		}
		c_DeltaYTM.SetCurSel( ebii );
	}
	else
		ebii = c_DeltaYTM.GetCurSel();
	if ( ebii < 0 ) ebii = 0;								// should be superfluous

	if ( c_DeltaYTM.GetFloat( ebii, m_DeltaYTM ) != 1 )		// load the float directly into qty
	{
#ifdef _DEBUG
		TRACE( _T("BondAnalyzerDialog::OnSelChangeDeltaYTM: Couldn't get a float at index %d\n"), ebii );
#endif
		return;												// this would be inconvenient
	}
	UpdateStatics();										// really only Imputed Market Value changes
	UpdatePresentValueCoupons();
}			// OnSelChangeDeltaYTM()
//----------------------------------------------------------------------------------------
void CBondAnalyzerDialog::OnSelChangeBondSymbol( void )
{	CString sym;
	int cnt = c_BondSymbol.GetCount();
	if ( cnt < 1 )
		return;
#ifdef _DEBUG
	TRACE( _T("BondAnalyzerDialog::OnSelChangeBondSymbol: theBond->symbol=%s, m_BondSymbol=%s\n"),
		theBond->getSymbol(), m_BondSymbol );
#endif
	int sel = c_BondSymbol.GetCurSel();
#if ( _WIN32_WCE >= 0x420 )					// just WM5
	if ( c_BondSymbol.lastChar == VK_DOWN  ||  c_BondSymbol.lastChar == VK_UP )
	{	int	delta = ( c_BondSymbol.lastChar == VK_DOWN  ?  1  :  -1 );
		sel += delta;
		if ( sel < 0 )
			sel = cnt - 1;
		if ( sel >= cnt )
			sel = 0;
		c_BondSymbol.SetCurSel( sel );
	}
#endif
	if ( sel < 0 ) sel = 0;							// should be superfluous
	c_BondSymbol.GetLBText( sel, sym );
#ifdef _DEBUG
	TRACE( _T("BondAnalyzerDialog::OnSelChangeBondSymbol: sym=%s\n"), m_BondSymbol );
#endif
	KillFocusBondSymbolWork( sym );
}			// OnSelChangeBondSymbol()
//----------------------------------------------------------------------------------------
LRESULT		CBondAnalyzerDialog::OnHotKey( WPARAM wParam, LPARAM lParam )
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
void	CBondAnalyzerDialog::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	CNillaDialog::OnActivate( nState, pWndOther, bMinimized );
	if ( nState == WA_ACTIVE || nState == WA_CLICKACTIVE )
		RegisterHotKeys( IDC_BondSymbol, IDC_DeltaYTM );	// screens for WM5 internally
}			// OnActivate()
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
/*
void	CBondAnalyzerDialog::OnEditChangeBondSymbol()
{		// this is now handled by CSeekComboBox
	seekComboBoxEntry( IDC_BondSymbol );
}			// OnEditChangeBondSymbol()
//----------------------------------------------------------------------------------------
void	CBondAnalyzerDialog::enableDialogItems( bool enDis )
{		// a Bond Symbol has been selected ...

		// make BondSymbol CCombo box readWrite
//	c_BondSymbol.EnableWindow( enDis );

		// make MarketPrice CEdit box readWrite
	c_MarketPrice.EnableWindow( enDis );

		// make WhatIfYTM CEdit box readWrite
	c_WhatIfYTM.EnableWindow( enDis );

	if ( enDis )								// we're ENabling 
		c_MarketPrice.SetFocus();				// grab the focus for CurrentPrice
}				// enableDialogItems()
*/
//----------------------------------------------------------------------------------------

