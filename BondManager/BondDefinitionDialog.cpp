// BondDefinitionDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "BondDefinitionDialog.h"
#include "BondManager.h"
#include "RegistryManager.h"
//#include "BOSDatabase.h"
//#include "NillaHedgeDoc.h"
#include "Bond.h"

#include <stdlib.h>					// _itoa(), atoi()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "BondManagerApp.h"
extern CBondManagerApp	theApp;

IMPLEMENT_DYNAMIC( CBondDefinitionDialog, CNillaDialog );

/////////////////////////////////////////////////////////////////////////////
// CBondDefinitionDialog dialog

CBondDefinitionDialog::CBondDefinitionDialog( CWnd* pParent /*= NULL */ )
	: CNillaDialog( IDD, pParent )
	, m_activeComboBox( NULL )
{
	//{{AFX_DATA_INIT(CBondDefinitionDialog)
	m_ParValue = 100.0;
	m_CouponsPerYear = 2;
	m_BondSymbol = _T("");
	m_Desc = _T("");
	m_MarketPrice = 0.0;
	m_CouponRate = 0.0f;
	//}}AFX_DATA_INIT
	criticalRegion = false;

	SYSTEMTIME    st;
	GetLocalTime( &st );
	m_IncepDate.SetDate( st.wYear - 1, st.wMonth, 15 );
		// Set the MaturityDate near a typical expiry date of a near future month.
	m_MaturityDate.SetDate( st.wYear + 5, st.wMonth, 15 );
	dayCounting = UnknownDayCountMethod;

		// reset relevant elements of the bond evaluation context
	evalCtx.yrsToMaturityIsValid = false;
	evalCtx.ytmIsValid = false;
	evalCtx.macDurIsValid = false;
	evalCtx.fromDate = COleDateTime::GetCurrentTime();
}

void CBondDefinitionDialog::DoDataExchange(CDataExchange* pDX)
{
	CNillaDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBondDefinitionDialog)
	DDX_Control(pDX, IDC_IncepDate, c_IncepDate);
	DDX_Control(pDX, IDC_RadioActualActual, c_RadioActualActual);
	DDX_Control(pDX, IDC_Radio30_360, c_Radio30_360);
	DDX_Control(pDX, IDC_RadioActual_360, c_RadioActual_360);
	DDX_Control(pDX, IDC_YieldToMaturityResult, c_YieldToMaturityResult);
	DDX_Control(pDX, IDC_ParValue, c_ParValue);
	DDX_Control(pDX, IDC_MaturityDate, c_MaturityDate);
	DDX_Control(pDX, IDC_MarketPrice, c_MarketPrice);
	DDX_Control(pDX, IDC_MacaulayDurationResult, c_MacaulayDurationResult);
	DDX_Control(pDX, IDC_Desc, c_Desc);
	DDX_Control(pDX, IDC_CurrentYieldResult, c_CurrentYieldResult);
	DDX_Control(pDX, IDC_CouponsPerYear, c_CouponsPerYear);
	DDX_Control(pDX, IDC_CouponRate, c_CouponRate);
	DDX_Text(pDX, IDC_ParValue, m_ParValue);
	DDX_Control(pDX, IDC_BondSymbol, c_BondSymbol);
	DDX_LBIndex(pDX, IDC_CouponsPerYear, m_CouponsPerYear);
	DDX_CBString(pDX, IDC_BondSymbol, m_BondSymbol);
	DDV_MaxChars(pDX, m_BondSymbol, 254);
	DDX_DateTimeCtrl(pDX, IDC_MaturityDate, m_MaturityDate);
	DDX_Text(pDX, IDC_Desc, m_Desc);
	DDV_MaxChars(pDX, m_Desc, 254);
	DDX_Text(pDX, IDC_MarketPrice, m_MarketPrice);
	DDV_MinMaxDouble(pDX, m_MarketPrice, 0., 1.e+019);
	DDX_Text(pDX, IDC_CouponRate, m_CouponRate);
	DDV_MinMaxFloat(pDX, m_CouponRate, 0.f, 9999.f);
	DDX_DateTimeCtrl(pDX, IDC_IncepDate, m_IncepDate);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( CBondDefinitionDialog, CNillaDialog )
	//{{AFX_MSG_MAP(CBondDefinitionDialog)
	ON_BN_CLICKED(IDC_Radio30_360, OnThirty360Clicked)
	ON_BN_CLICKED(IDC_RadioActual_360, OnActual360Clicked)
	ON_BN_CLICKED(IDC_RadioActualActual, OnActualActualClicked)
	ON_EN_KILLFOCUS(IDC_ParValue, OnKillFocusParValue)
	ON_EN_KILLFOCUS(IDC_CouponRate, OnKillFocusCouponRate)
	ON_LBN_KILLFOCUS(IDC_CouponsPerYear, OnKillFocusCouponsPerYear)
	ON_EN_KILLFOCUS(IDC_Desc, OnKillFocusDesc)
	ON_EN_KILLFOCUS(IDC_MarketPrice, OnKillFocusMarketPrice)
	ON_NOTIFY(NM_KILLFOCUS, IDC_MaturityDate, OnKillFocusMaturityDate)
	ON_LBN_SELCHANGE(IDC_CouponsPerYear, OnSelChangeCouponsPerYear)
	ON_EN_SETFOCUS(IDC_MarketPrice, OnSetFocusMarketPrice)
	ON_EN_SETFOCUS(IDC_Desc, OnSetFocusDesc)
	ON_EN_SETFOCUS(IDC_CouponRate, OnSetFocusCouponRate)
	ON_EN_SETFOCUS(IDC_ParValue, OnSetFocusParValue)
	ON_NOTIFY(NM_KILLFOCUS, IDC_IncepDate, OnKillFocusIncepDate)
	ON_COMMAND(ID_Delete_Definition, OnDeleteDefinition)
	ON_NOTIFY(DTN_CLOSEUP, IDC_IncepDate, OnCloseUpIncepDate)
	ON_NOTIFY(DTN_CLOSEUP, IDC_MaturityDate, OnCloseUpMaturityDate)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_SETFOCUS, IDC_MaturityDate, OnSetFocusMaturityDate)
	ON_NOTIFY(NM_SETFOCUS, IDC_IncepDate, OnSetFocusIncepDate)

	ON_CBN_SELCHANGE(IDC_BondSymbol, OnSelChangeBondSymbol)
	ON_CBN_KILLFOCUS(IDC_BondSymbol, OnKillFocusBondSymbol)				// HotKeys +
	ON_CBN_SETFOCUS(IDC_BondSymbol, OnSetFocusBondSymbol)				// HotKeys
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATE()													// HotKeys
	ON_MESSAGE(WM_HOTKEY, OnHotKey)										// HotKeys

	ON_LBN_SETFOCUS(IDC_CouponsPerYear, OnSetFocusCouponsPerYear)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBondDefinitionDialog message handlers

BOOL	CBondDefinitionDialog::OnInitDialog() 
{
	CNillaDialog::OnInitDialog();
	RegisterHotKeys( IDC_BondSymbol, IDC_DeltaYTM );	// screens for WM5 internally

		// load up the ComboBox from the bonds map (the pDB approach)
	CMapStringToPtr*	bondsMap = theApp.bndMgr->GetSymbolTable();
	c_BondSymbol.LoadMap( bondsMap );

		// preload some reasonable default dates and valid date ranges
	c_IncepDate.SetTime( m_IncepDate );
	c_MaturityDate.SetTime( m_MaturityDate );

	COleDateTimeSpan	oneDay( 1, 0, 0, 0);

	COleDateTime		invalidDate;
	invalidDate.SetStatus( COleDateTime::null );

	COleDateTime		dtMinMax = m_IncepDate + oneDay;	// 'min' this time
	c_MaturityDate.SetRange( &dtMinMax, &invalidDate );

	dtMinMax = m_MaturityDate - oneDay;						// 'max' this time
	c_IncepDate.SetRange( &invalidDate, &dtMinMax );

		// allow for preloading a selection into the bond symbol ComboBox
	if ( m_BondSymbol != _T("") )							// modified since the constructor
	{	CString sym = m_BondSymbol;
		m_BondSymbol = _T("");
		KillFocusBondSymbolWork( sym );
		return TRUE;
	}
		// load the Coupons/Yr ListBox
	int err = c_CouponsPerYear.InitStorage(5, 3);		// five strings, three bytes each
	ASSERT(err != LB_ERRSPACE);

		// add strings to the ListBox
	CString cs[5] = { _T("0"), _T("1"), _T("2"), _T("4"), _T("12") };
	for ( short ii = 0; ii < 5; ii++ )
	{	int err = c_CouponsPerYear.AddString( cs[ii] );
		ASSERT(err != LB_ERR);
		if ( err == LB_ERRSPACE )
		{	CString	mss = _T("Insufficient space to store '");
			mss += cs[ii];
			mss += _T("' in the list box.");
			MessageBox( mss );
			break;
		}
	}
		// 0, 1, 2 are < 3, but "4" is at position 3 and "12" is at position 4
	int idx = m_CouponsPerYear;
	if ( idx > 2)
		idx = (m_CouponsPerYear == 4)  ?  3  :  4;
	err = c_CouponsPerYear.SetCurSel( idx );

		// Note, we still haven't formatted default values in the CEdit boxes
	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//----------------------------------------------------------------------------------------
HBRUSH	CBondDefinitionDialog::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor )
{
	return	CNillaDialog::OnCtlColor( pDC, pWnd, nCtlColor );
}			// OnCtlColor()
//----------------------------------------------------------------------------------------
void	CBondDefinitionDialog::OnSetFocusBondSymbol( void )
{
	m_activeComboBox = IDC_BondSymbol;
	int cnt = c_BondSymbol.GetCount();
	if ( cnt < 1 )
		SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusBondSymbol()
//----------------------------------------------------------------------------------------
void	CBondDefinitionDialog::KillFocusBondSymbolWork( CString sym )
{		// Did user select an existing value or create a new one?
#ifdef _DEBUG
	TRACE( _T("BondDefinitionDialog::KillFocusBondSymbolWork: sym=%s, m_BondSymbol=%s\n"), sym, m_BondSymbol );
#endif
	if ( sym == m_BondSymbol )
		return;											// nothing to do
	SaveIfChanged( true );
	m_BondSymbol = sym;
		// lookup the bond Symbol in the Bonds Map
	CBond* aBond = theApp.bndMgr->GetBond( m_BondSymbol );
	if ( aBond == NULL )
		return;

		// load dialog variables from aBond
	m_CouponRate = 100.0f * aBond->getCouponRate();			// stored fraction, but dialog var is a percentage
	m_CouponsPerYear = aBond->getCouponsPerYear() > 0  ?  aBond->getCouponsPerYear()  :  0;
	m_Desc = aBond->getDesc();
	m_ParValue = aBond->getParValue();
	m_IncepDate = aBond->getIncepDate();
	m_MaturityDate = aBond->getMaturityDate();
	m_MarketPrice = aBond->getMktPrice();
	dayCounting = aBond->getDayCounting();
	delete	aBond;								// we're done with it

		// if sentinel values found, revert to defaults
	if ( m_IncepDate == (COleDateTime)0.0f )
	{	SYSTEMTIME    st;
		GetLocalTime( &st );
		m_IncepDate.SetDate( st.wYear, st.wMonth, 15 );
	}
	if ( m_MaturityDate == (COleDateTime)0.0f )
		m_MaturityDate.SetDate( m_IncepDate.GetYear() + 5, m_IncepDate.GetMonth(), 15 );

	if ( m_ParValue == 0.0 )
		m_ParValue = 100.0;


		// update the GUI
	c_IncepDate.SetTime( m_IncepDate );
	c_MaturityDate.SetTime( m_MaturityDate );
	c_Desc.SetWindowText( m_Desc );
	setEditBox( "%.2f", m_MarketPrice, IDC_MarketPrice );
	setEditBox( "%.3f", m_CouponRate, IDC_CouponRate );			// display the percentage
	setEditBox( "%.0f", m_ParValue, IDC_ParValue );
		// set the coupons per year
	int idx = m_CouponsPerYear;
	if ( m_CouponsPerYear > 2 )
		idx = m_CouponsPerYear == 4  ?  3  :  4;
	int err = c_CouponsPerYear.SetCurSel( idx );
		// set the Day Counting radio buttons
	c_RadioActualActual.SetCheck( (dayCounting == ActualActual) ? BST_CHECKED : BST_UNCHECKED );
	c_RadioActual_360.SetCheck( (dayCounting == Actual360) ? BST_CHECKED : BST_UNCHECKED );
	c_Radio30_360.SetCheck( (dayCounting == Thirty360) ? BST_CHECKED : BST_UNCHECKED );

		// compute analytics and display results
	evalCtx.yrsToMaturityIsValid = false;
	evalCtx.ytmIsValid = false;
	evalCtx.macDurIsValid = false;
	updateStatics();
}			// KillFocusBondSymbolWork()
//----------------------------------------------------------------------------------------
void	CBondDefinitionDialog::OnKillFocusBondSymbol( void )
{	CString sym;
	m_activeComboBox = NULL;
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	if ( criticalRegion )
		return;
	c_BondSymbol.GetWindowText( sym );
	KillFocusBondSymbolWork( sym );
}			// OnKillFocusBondSymbol()
//----------------------------------------------------------------------------------------
void	CBondDefinitionDialog::OnSelChangeBondSymbol( void )
{	CString		sym;
	int cnt = c_BondSymbol.GetCount();
	if ( cnt < 1 )
		return;
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
	if ( sel < 0 ) sel = 0;					// should be superfluous
	c_BondSymbol.GetLBText( sel, sym );
	KillFocusBondSymbolWork( sym );
}			// OnSelChangeBondSymbol()
//----------------------------------------------------------------------------------------
LRESULT		CBondDefinitionDialog::OnHotKey( WPARAM wParam, LPARAM lParam )
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
void	CBondDefinitionDialog::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	CNillaDialog::OnActivate( nState, pWndOther, bMinimized );
	if ( nState == WA_ACTIVE || nState == WA_CLICKACTIVE )
		RegisterHotKeys( IDC_BondSymbol, IDC_DeltaYTM );	// screens for WM5 internally
}			// OnActivate()
//----------------------------------------------------------------------------
void CBondDefinitionDialog::SaveIfChanged( bool updateGUI /* = false */ )
{
	CBond*	aBond = NULL;
	if ( m_BondSymbol == _T("") )
		goto	Exit;						// nothing to look up

	bool	saveVerify = true;
	short	prefs = DefinitionChangeSymbol | DefinitionClose;
	if ( GetSaveVerifyPrefs( &prefs ) )
	{		// updateGUI tells us whether this is a close or change symbol action
			// updateGUI should be true from killFocusBondSymbolWork, representing a change symbol action
			//			 and false from MainFrame, representing a close dialog action
		short	mask = updateGUI  ?  DefinitionChangeSymbol  :  DefinitionClose;
		saveVerify = (prefs & mask) != 0;
	}
		// the following fetches only the invariant elements of the Bond definition 
	aBond = theApp.bndMgr->GetBond( m_BondSymbol );
	if ( aBond != NULL )
	{	bool	changed =
					aBond->getCouponsPerYear() != m_CouponsPerYear  ||  fabs( m_CouponRate / 100.0  - aBond->getCouponRate() ) > 1e-5
				||  aBond->getDayCounting()	   != dayCounting	    ||  fabs( m_MarketPrice			- aBond->getMktPrice() )   > 1e-4
				||  aBond->getMaturityDate()   != m_MaturityDate	||  fabs( m_ParValue			- aBond->getParValue() )   > 1e-4
				||  aBond->getIncepDate()	   != m_IncepDate
				||  aBond->getDesc()		   != m_Desc;
		if ( ! changed )
			goto	Exit;
			// we have changes ...
		if ( saveVerify )
		{		// get permission to update the Bond definition?
				// if the caller is MainFrame, updateGUI will be false (the default value)
				// so we won't post a MessageBox, we'll just update and return.
			wchar_t	buf[320];
			swprintf( buf, _T("Save changes to '%s'?"), m_BondSymbol );
			criticalRegion = true;
			UINT result = MessageBox( buf, _T("Save changes?"),
							MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST );
			criticalRegion = false;
			if ( result != IDYES )
				goto	Exit;
		}
	}
	else
	{		// didn't find m_BondSymbol in the bonds map, so this is a new entry
		if ( saveVerify )
		{		// get permission to create a new Bond definition...
			wchar_t	buf[320];
			swprintf( buf, _T("Save '%s'?"), m_BondSymbol );
			UINT result = MessageBox( buf, _T("Define new bond?"),
							  MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST );
			if ( result != IDYES )
				goto	Exit;				// aBond == NULL --> no heap to cleanup
		}
		aBond = new CBond( m_BondSymbol );

			// add this entry to the BondSymbol ComboBox
		if ( updateGUI )
		{		// you'll get an access violation if you try this from MainFrame
				// because at that point (dialog has returned from DoModal)
				// the control doesn't exist anymore
			c_BondSymbol.AddString( m_BondSymbol );
		}
	}
	SaveBond( aBond );		// overwrite aBond's parameters with dialog values, then save
Exit:
	if ( aBond )
		delete	aBond;
}			// SaveIfChanged()
//----------------------------------------------------------------------------------------
void	CBondDefinitionDialog::SaveBond( CBond* aBond )
{
#ifdef _DEBUG
	ASSERT_VALID( aBond );
#endif
	aBond->setCouponsPerYear( m_CouponsPerYear );
	aBond->setCouponRate( (float)(m_CouponRate / 100.0) );
	aBond->setDayCounting( dayCounting );
	aBond->setParValue( m_ParValue );
	aBond->setIncepDate( m_IncepDate );
	aBond->setMaturityDate( m_MaturityDate );
	aBond->setMktPrice( m_MarketPrice );
	aBond->setDesc( m_Desc );
	aBond->saveDesc();				// commit the description to the DB
	short	res = theApp.bndMgr->WriteBond( aBond );
#ifdef _DEBUG
	if ( res != 0 )
		TRACE( _T("BondDefinitionDialog::SaveBond: Bond write failed, res=%d\n"), res );
#endif
}			// SaveBond()
//----------------------------------------------------------------------------------------
void	CBondDefinitionDialog::updateStatics( void )
{	if (	 m_CouponRate  <  0.0  ||  m_ParValue  <= 0.0
		 ||  m_MarketPrice <= 0.0  ||  dayCounting == UnknownDayCountMethod )
		return;

		// find Imputed Yield (%) and show it on the dialog
	CBond	aBond;
	aBond.setCouponsPerYear( m_CouponsPerYear );
		// stored fraction <-- displayed percentage
	aBond.setCouponRate( (float)(m_CouponRate / 100.0) );
	aBond.setDayCounting( dayCounting );
	aBond.setParValue( m_ParValue );
	aBond.setIncepDate( m_IncepDate );
	aBond.setMaturityDate( m_MaturityDate );
	aBond.setMktPrice( m_MarketPrice );

	double yy = aBond.currentYield();
	setStatic( "%.3f", 100.0 * yy, IDC_CurrentYieldResult );

		// the tolerance here is in the predicted market price
		// a tolerance going in, the absolute error coming out
	double	ytm = aBond.YieldToMaturity( evalCtx );
#ifdef _DEBUG
	TRACE( _T("CBondDefinitionDialog::updateStatics: ytm=%g\n"), ytm );
#endif
	if ( fabs(evalCtx.ytmErr) > CBond::GetErrTol() )
	{	setStatic( "~", IDC_CurrentYieldResult );
		setStatic( "~", IDC_YieldToMaturityResult );
		setStatic( "~", IDC_MacaulayDurationResult );
		return;
	}
	setStatic( "%.3f", 100.0 * ytm, IDC_YieldToMaturityResult );

	double	macDur = aBond.MacaulayDuration( evalCtx );
	setStatic( "%.3f", macDur, IDC_MacaulayDurationResult );
}			// updateStatics()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnThirty360Clicked()
{
	if ( dayCounting == Thirty360 )
		return;
	dayCounting = Thirty360;
	CheckRadioButton( IDC_RadioActualActual, IDC_RadioActual_360, IDC_Radio30_360 );

	evalCtx.yrsToMaturityIsValid = false;
	evalCtx.ytmIsValid = false;
	evalCtx.macDurIsValid = false;
	updateStatics();
}			// OnThirty360Clicked()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnActual360Clicked()
{
	if ( dayCounting == Actual360 )
		return;
	dayCounting = Actual360;
	CheckRadioButton( IDC_RadioActualActual, IDC_RadioActual_360, IDC_RadioActual_360 );

	evalCtx.yrsToMaturityIsValid = false;
	evalCtx.ytmIsValid = false;
	evalCtx.macDurIsValid = false;
	updateStatics();
}			// OnActual360Clicked()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnActualActualClicked()
{
	if ( dayCounting == ActualActual )
		return;
	dayCounting = ActualActual;
	CheckRadioButton( IDC_RadioActualActual, IDC_RadioActual_360, IDC_RadioActualActual );

	evalCtx.yrsToMaturityIsValid = false;
	evalCtx.ytmIsValid = false;
	evalCtx.macDurIsValid = false;
	updateStatics();
}			// OnActualActualClicked()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnKillFocusCouponsPerYear( void )
{
//	m_activeComboBox = NULL;						// sending hotkeys to a ListBox crashes BM
	int sel = c_CouponsPerYear.GetCurSel();
	if ( sel > 2 )
		sel = (sel == 3)  ?  4  :  12;
	if ( m_CouponsPerYear == sel )
		return;
	m_CouponsPerYear = sel;

	evalCtx.macDurIsValid = false;
	evalCtx.ytmIsValid = false;
	updateStatics();
}			// OnKillFocusCouponsPerYear()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnSetFocusCouponsPerYear( void )
{
//	m_activeComboBox = IDC_CouponsPerYear;			// sending hotkeys to a ListBox crashes BM
}			// OnSetFocusCouponsPerYear()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnSelChangeCouponsPerYear( void )
{
	OnKillFocusCouponsPerYear();
}			// OnSelChangeCouponsPerYear()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnKillFocusDesc()
{	CString	desc;
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	c_Desc.GetWindowText( desc );
	if ( desc == m_Desc )
		return;
	m_Desc = desc;
}			// OnKillFocusDesc()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::killFocusIncepDateWork( NMHDR* pNMHDR, LRESULT* pResult )
{	COleDateTime	aDate;
	c_IncepDate.GetTime( aDate );
	if ( m_IncepDate == aDate )
		return;

		// capture the currently displayed date
	m_IncepDate = aDate;

		// update the valid date range for maturityDate
	COleDateTime		invalidDate;
	invalidDate.SetStatus( COleDateTime::null );
	COleDateTimeSpan	oneDay( 1, 0, 0, 0);

	COleDateTime		dtMin = m_IncepDate + oneDay;
	c_MaturityDate.SetRange( &dtMin, &invalidDate );

		// prep to re-evaluate the Bond
	evalCtx.yrsToMaturityIsValid = false;
	evalCtx.ytmIsValid = false;
	evalCtx.macDurIsValid = false;
	COleDateTime	today = COleDateTime::GetCurrentTime();
	evalCtx.fromDate = m_IncepDate > today  ?  m_IncepDate  :  today;
	updateStatics();
	*pResult = 0;
}			// killFocusIncepDateWork()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnCloseUpIncepDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	killFocusIncepDateWork( pNMHDR, pResult );
}			// OnCloseUpIncepDate()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnKillFocusIncepDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	killFocusIncepDateWork( pNMHDR, pResult );
}			// OnKillFocusIncepDate()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnSetFocusIncepDate(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	*pResult = 0;
}			// OnSetFocusIncepDate()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::killFocusMaturityDateWork( NMHDR* pNMHDR, LRESULT* pResult )
{	COleDateTime	aDate;
	c_MaturityDate.GetTime( aDate );
	if ( m_MaturityDate == aDate )
		return;

		// capture the currently displayed date
	m_MaturityDate = aDate;

		// update the valid date range for maturityDate
	COleDateTime		invalidDate;
	invalidDate.SetStatus( COleDateTime::null );
	COleDateTimeSpan	oneDay( 1, 0, 0, 0);

	COleDateTime		dtMax = m_MaturityDate - oneDay;
	c_IncepDate.SetRange( &invalidDate, &dtMax );

		// prep to re-evaluate the Bond
	evalCtx.yrsToMaturityIsValid = false;
	evalCtx.ytmIsValid = false;
	evalCtx.macDurIsValid = false;
	updateStatics();
	*pResult = 0;
}			// killFocusMaturityDateWork()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnCloseUpMaturityDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	killFocusMaturityDateWork( pNMHDR, pResult );
}			// OnCloseUpMaturityDate()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnKillFocusMaturityDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	killFocusMaturityDateWork( pNMHDR, pResult );
}			// OnKillFocusMaturityDate()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnSetFocusMaturityDate(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );	
	*pResult = 0;
}			// OnSetFocusMaturityDate()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnKillFocusMarketPrice() 
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_MarketPrice );
	if ( ff == m_MarketPrice )
		return;
	m_MarketPrice = ff;
	setEditBox( "%.2f", m_MarketPrice, IDC_MarketPrice );

	evalCtx.ytmIsValid = false;
	evalCtx.macDurIsValid = false;
	updateStatics();
}			// OnKillFocusMarketPrice()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnKillFocusParValue()
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	float	ff = getEditBoxFloat( IDC_ParValue );
	if ( ff == m_ParValue )
		return;
	m_ParValue = ff;
	setEditBox( "%.0f", m_ParValue, IDC_ParValue );

	evalCtx.ytmIsValid = false;
	evalCtx.macDurIsValid = false;
	updateStatics();
}			// OnKillFocusParValue()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnKillFocusCouponRate()
{	float	ff = getEditBoxFloat( IDC_CouponRate );			// dialog var is a percentage
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	if ( ff == m_CouponRate )
		return;
	m_CouponRate = ff;
//	setEditBox( "%.3f", ff, IDC_CouponRate );

	evalCtx.ytmIsValid = false;
	evalCtx.macDurIsValid = false;
	updateStatics();
}			// OnKillFocusCouponRate()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnSetFocusMarketPrice( void )
{
	c_MarketPrice.SetSel( 0, -1 );
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusMarketPrice()
//----------------------------------------------------------------------------------------
void CBondDefinitionDialog::OnSetFocusDesc( void )
{
	c_Desc.SetSel( 0, -1 );
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusDesc()
//----------------------------------------------------------------------------------------
void	CBondDefinitionDialog::OnSetFocusParValue( void )
{
	c_ParValue.SetSel( 0, -1 );
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusParValue()
//----------------------------------------------------------------------------------------
void	CBondDefinitionDialog::OnSetFocusCouponRate( void )
{
	c_CouponRate.SetSel( 0, -1 );
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusCouponRate()
//----------------------------------------------------------------------------------------
void	CBondDefinitionDialog::OnDeleteDefinition() 
{		// respond to Ctrl-K (kill) keyboard command
	if ( m_BondSymbol == _T("") )
		return;

	CBond*	bnd = theApp.bndMgr->GetBond( m_BondSymbol );
	if ( bnd == NULL )
		return;

	CMapPtrToPtr*	posLst = bnd->getPositionList();
	short	nItems = posLst->GetCount();
	if ( nItems > 0 )
	{	wchar_t	buf[400];
		swprintf( buf, _T("Deleting '%s' will irrevocably delete its %d positions. Okay to proceed?"),
					m_BondSymbol, nItems );
		UINT result = MessageBox( buf, _T("Delete bond/positions?"),
							  MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL | MB_SETFOREGROUND | MB_TOPMOST );
		if ( result == IDYES )
		{		// okay to delete the definition and all of its positions
			short	res = theApp.bndMgr->DeleteBOS( (CBOS*)bnd );
			if ( res != 0 )
			{
#ifdef _DEBUG
				TRACE( _T("BondDefinitionDialog::OnDeleteDefinition: DeleteBOS failed, res=%d"), res );
#endif
			}
				// now remove m_BondSymbol from c_BondSymbol and reset it to _T("")
			int	idx = c_BondSymbol.FindStringExact( -1, m_BondSymbol );
			if ( idx != CB_ERR )
				c_BondSymbol.DeleteString( idx );
			c_BondSymbol.SetCurSel( -1 );				// clears the Edit portion of the ComboBox
		}
	}
	delete	bnd;
	return;
}			// OnDeleteDefinition()
//----------------------------------------------------------------------------------------
/*
void CBondDefinitionDialog::OnSelEndOkSymbol()
{
	killFocusBondSymbolWork();
}			// OnSelEndOkSymbol()
*/
//----------------------------------------------------------------------------------------
/*
void	CBondDefinitionDialog::OnEditChangeSymbol( void )
{		// this is now handled by CSeekComboBox
	seekComboBoxEntry( IDC_BondSymbol );
}			// OnEditChangeSymbol()
//----------------------------------------------------------------------------------------
void	CBondDefinitionDialog::clearStatics( void )
{
	setStatic( "", IDC_CurrentYieldResult );
	setStatic( "", IDC_YieldToMaturityResult );
	setStatic( "", IDC_MacaulayDurationResult );
}			// clearStatics()
//----------------------------------------------------------------------------------------
void	CBondDefinitionDialog::enableDialogItems( bool enDis )
{		// a Bond Symbol has been selected ...

		// make CurrentPrice CEdit box readWrite
	c_MarketPrice.EnableWindow( enDis );

		// make the Desc CEdit box readWrite
	c_Desc.EnableWindow( enDis );

		// make Coupon Amount CEdit box readWrite
	c_CouponRate.EnableWindow( enDis );

		// make the Face Value CEdit box readWrite
	c_ParValue.EnableWindow( enDis );

		// enable the IncepDate DatePicker
	c_IncepDate.EnableWindow( enDis );

		// enable the Maturity DatePicker
	c_MaturityDate.EnableWindow( enDis );

		// make the Coupons/Yr ListBox readWrite
	c_CouponsPerYear.EnableWindow( enDis );

		// enable the DayCounting radio buttons
	c_RadioActualActual.EnableWindow( enDis );

	c_RadioActual_360.EnableWindow( enDis );

	c_Radio30_360.EnableWindow( enDis );
}				// enableDialogItems()
//----------------------------------------------------------------------------------------
void	CBondDefinitionDialog::resetAll( void )
{		// set the Market Price
	m_MarketPrice = 0.0;
	setEditBox( "0.00", IDC_MarketPrice );

		// set the coupons per year to '2'
	m_CouponsPerYear = 2;
	int idx = ( m_CouponsPerYear < 3 ) ? m_CouponsPerYear : 3;
	int err = c_CouponsPerYear.SetCurSel( idx );

		// clear the description
	m_Desc = _T("");
	c_Desc.SetWindowText( m_Desc );

		// set the Inception Date
	m_IncepDate = COleDateTime::GetCurrentTime();
	c_IncepDate.SetTime( m_IncepDate );

		// set the Maturity Date
	m_MaturityDate.SetDate( m_IncepDate.GetYear()+5,
							m_IncepDate.GetMonth(),
							m_IncepDate.GetDay()	 );
	c_MaturityDate.SetTime( m_MaturityDate );

		// reset the Par Value to $100
	m_ParValue = 100.0;
	setEditBox( "%.0f", m_ParValue, IDC_ParValue );

		// clear the Coupon Rate
	m_CouponRate = 0.0;
	setEditBox( "%.2f", m_CouponRate, IDC_CouponRate );

		// reset the Day Counting radio buttons
	dayCounting = ActualActual;
	c_RadioActualActual.SetCheck( BST_CHECKED );
	c_RadioActual_360.SetCheck( BST_UNCHECKED );
	c_Radio30_360.SetCheck( BST_UNCHECKED );

		// reset the results statics
	setStatic( "0.000", IDC_CurrentYieldResult );
	setStatic( "0.000", IDC_YieldToMaturityResult );
	setStatic( "0.000", IDC_MacaulayDurationResult );
}			// resetAll()
*/
//----------------------------------------------------------------------------------------
/*
void	CBondDefinitionDialog::resetControls( void )
{		// leave Desc, ExpiryDate, StockSymbol & putOrCall alone, but ...
		// clear out two (sentinel) controls
	m_MarketPrice = 0.00;
	m_CouponRate = 0.00;
	dayCounting = UnknownDayCountMethod;

	c_MarketPrice.SetWindowText( _T("") );
	c_CouponRate.SetWindowText( _T("") );
	c_RadioActualActual.SetCheck( BST_UNCHECKED );
	c_RadioActual_360.SetCheck( BST_UNCHECKED );
	c_Radio30_360.SetCheck( BST_UNCHECKED );

}			// resetControls()
*/
//----------------------------------------------------------------------------------------
