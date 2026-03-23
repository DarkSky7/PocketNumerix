// SpotRateEstimatorDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "BitArray.h"
//#include "BOSDatabase.h"
#include "Vector.h"
#include "Matrix.h"
#include "SpotRateEstimatorDialog.h"
#include "ShortRateCalibration.h"
#include "NillaHedge.h"
#include "NillaHedgeDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CNillaHedgeApp theApp;

IMPLEMENT_DYNAMIC( CSpotRateEstimatorDialog, CNillaDialog )

extern	CNillaHedgeApp	theApp;

/////////////////////////////////////////////////////////////////////////////
// CSpotRateEstimatorDialog dialog

CSpotRateEstimatorDialog::CSpotRateEstimatorDialog( CWnd* pParent /* =NULL */ )
	: CNillaDialog( IDD, pParent )
{
	//{{AFX_DATA_INIT(CSpotRateEstimatorDialog)
	m_3moYield  = 0.0;
	m_6moYield  = 0.0;
	m_1yrYield  = 0.0;
	m_2yrYield  = 0.0;
	m_3yrYield  = 0.0;
	m_5yrYield  = 0.0;
	m_7yrYield  = 0.0;
	m_10yrYield = 0.0;
	m_15yrYield = 0.0;
	m_20yrYield = 0.0;
	m_25yrYield = 0.0;
	m_30yrYield = 0.0;
	//}}AFX_DATA_INIT

	spotRate = 3.0f;
	userSupplied = new CBitArray(12);
	userSupplied->clear();
}

void CSpotRateEstimatorDialog::DoDataExchange(CDataExchange* pDX)
{
	CNillaDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpotRateEstimatorDialog)
	DDX_Control(pDX, IDC_3moYield, c_3moYield);
	DDX_Control(pDX, IDC_6moYield, c_6moYield);
	DDX_Control(pDX, IDC_1yrYield, c_1yrYield);
	DDX_Control(pDX, IDC_2yrYield, c_2yrYield);
	DDX_Control(pDX, IDC_3yrYield, c_3yrYield);
	DDX_Control(pDX, IDC_5yrYield, c_5yrYield);
	DDX_Control(pDX, IDC_7yrYield, c_7yrYield);
	DDX_Control(pDX, IDC_10yrYield, c_10yrYield);
	DDX_Control(pDX, IDC_15yrYield, c_15yrYield);
	DDX_Control(pDX, IDC_20yrYield, c_20yrYield);
	DDX_Control(pDX, IDC_25yrYield, c_25yrYield);
	DDX_Control(pDX, IDC_30yrYield, c_30yrYield);
	DDX_Text(pDX, IDC_3moYield, m_3moYield);
	DDV_MinMaxFloat(pDX, m_3moYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_6moYield, m_6moYield);
	DDV_MinMaxFloat(pDX, m_6moYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_1yrYield, m_1yrYield);
	DDV_MinMaxFloat(pDX, m_1yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_2yrYield, m_2yrYield);
	DDV_MinMaxFloat(pDX, m_2yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_3yrYield, m_3yrYield);
	DDV_MinMaxFloat(pDX, m_3yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_5yrYield, m_5yrYield);
	DDV_MinMaxFloat(pDX, m_5yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_7yrYield, m_7yrYield);
	DDV_MinMaxFloat(pDX, m_7yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_10yrYield, m_10yrYield);
	DDV_MinMaxFloat(pDX, m_10yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_15yrYield, m_15yrYield);
	DDV_MinMaxFloat(pDX, m_15yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_20yrYield, m_20yrYield);
	DDV_MinMaxFloat(pDX, m_20yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_25yrYield, m_25yrYield);
	DDV_MinMaxFloat(pDX, m_25yrYield, 0.f, 9000.f);
	DDX_Text(pDX, IDC_30yrYield, m_30yrYield);
	DDV_MinMaxFloat(pDX, m_30yrYield, 0.f, 9000.f);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSpotRateEstimatorDialog, CNillaDialog)
	//{{AFX_MSG_MAP(CSpotRateEstimatorDialog)
	ON_EN_KILLFOCUS(IDC_10yrYield, OnKillFocus10yrYield)
	ON_EN_KILLFOCUS(IDC_15yrYield, OnKillFocus15yrYield)
	ON_EN_KILLFOCUS(IDC_1yrYield, OnKillFocus1yrYield)
	ON_EN_KILLFOCUS(IDC_20yrYield, OnKillFocus20yrYield)
	ON_EN_KILLFOCUS(IDC_25yrYield, OnKillFocus25yrYield)
	ON_EN_KILLFOCUS(IDC_2yrYield, OnKillFocus2yrYield)
	ON_EN_KILLFOCUS(IDC_30yrYield, OnKillFocus30yrYield)
	ON_EN_KILLFOCUS(IDC_3moYield, OnKillFocus3moYield)
	ON_EN_KILLFOCUS(IDC_3yrYield, OnKillFocus3yrYield)
	ON_EN_KILLFOCUS(IDC_5yrYield, OnKillFocus5yrYield)
	ON_EN_KILLFOCUS(IDC_6moYield, OnKillFocus6moYield)
	ON_EN_KILLFOCUS(IDC_7yrYield, OnKillFocus7yrYield)
	ON_EN_SETFOCUS(IDC_10yrYield, OnSetFocus10yrYield)
	ON_EN_SETFOCUS(IDC_15yrYield, OnSetFocus15yrYield)
	ON_EN_SETFOCUS(IDC_1yrYield, OnSetFocus1yrYield)
	ON_EN_SETFOCUS(IDC_20yrYield, OnSetFocus20yrYield)
	ON_EN_SETFOCUS(IDC_25yrYield, OnSetFocus25yrYield)
	ON_EN_SETFOCUS(IDC_2yrYield, OnSetFocus2yrYield)
	ON_EN_SETFOCUS(IDC_30yrYield, OnSetFocus30yrYield)
	ON_EN_SETFOCUS(IDC_3moYield, OnSetFocus3moYield)
	ON_EN_SETFOCUS(IDC_3yrYield, OnSetFocus3yrYield)
	ON_EN_SETFOCUS(IDC_5yrYield, OnSetFocus5yrYield)
	ON_EN_SETFOCUS(IDC_6moYield, OnSetFocus6moYield)
	ON_EN_SETFOCUS(IDC_7yrYield, OnSetFocus7yrYield)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpotRateEstimatorDialog message handlers

BOOL	CSpotRateEstimatorDialog::OnInitDialog() 
{	CShortRateCalibration	srb;
	CNillaDialog::OnInitDialog();
/*
		// push any existing riskFreeRate into the short rate extrapolated result
	res = theApp.pDB->getRiskFreeRate( spotRate );
		// if getRiskFreeRate() fails, we still have the constructor value
	setStatic( "%.3f", 100.0f * spotRate, IDC_RiskFreeRateResult );
*/
	bool res = theApp.pDB->getShortRateBasis( srb );
#ifdef _DEBUG
	TRACE( _T("SpotRateEstimatorDialog::OnInitDialog: getShortRateBasis() --> %s\n"),
		res ? _T("true") : _T("false") );
#endif
	m_3moYield  = srb.r3mo;
	if ( m_3moYield > 0.0 ) userSupplied->set(0);
	setEditBox( "%.3f", m_3moYield,  IDC_3moYield  );

	m_6moYield  = srb.r6mo;
	if ( m_6moYield > 0.0 ) userSupplied->set(1);
	setEditBox( "%.3f", m_6moYield,  IDC_6moYield  );

	m_1yrYield  = srb.r1yr;
	if ( m_1yrYield > 0.0 ) userSupplied->set(2);
	setEditBox( "%.3f", m_1yrYield,  IDC_1yrYield  );

	m_2yrYield  = srb.r2yr;
	if ( m_2yrYield > 0.0 ) userSupplied->set(3);
	setEditBox( "%.3f", m_2yrYield,  IDC_2yrYield  );

	m_3yrYield  = srb.r3yr;
	if ( m_3yrYield > 0.0 ) userSupplied->set(4);
	setEditBox( "%.3f", m_3yrYield,  IDC_3yrYield  );

	m_5yrYield  = srb.r5yr;
	if ( m_5yrYield > 0.0 ) userSupplied->set(5);
	setEditBox( "%.3f", m_5yrYield,  IDC_5yrYield  );

	m_7yrYield  = srb.r7yr;
	if ( m_7yrYield > 0.0 ) userSupplied->set(6);
	setEditBox( "%.3f", m_7yrYield,  IDC_7yrYield  );

	m_10yrYield = srb.r10yr;
	if ( m_10yrYield > 0.0 ) userSupplied->set(7);
	setEditBox( "%.3f", m_10yrYield, IDC_10yrYield );

	m_15yrYield = srb.r15yr;
	if ( m_15yrYield > 0.0 ) userSupplied->set(8);
	setEditBox( "%.3f", m_15yrYield, IDC_15yrYield );

	m_20yrYield = srb.r20yr;
	if ( m_20yrYield > 0.0 ) userSupplied->set(9);
	setEditBox( "%.3f", m_20yrYield, IDC_20yrYield );

	m_25yrYield = srb.r25yr;
	if ( m_25yrYield > 0.0 ) userSupplied->set(10);
	setEditBox( "%.3f", m_25yrYield, IDC_25yrYield );

	m_30yrYield = srb.r30yr;
	if ( m_30yrYield > 0.0 ) userSupplied->set(11);
	setEditBox( "%.3f", m_30yrYield, IDC_30yrYield );

		// update the GUI
	updateRates();
	SipShowIM( SIPF_ON );		// show the input panel

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//--------------------------------------------------------------------
void	CSpotRateEstimatorDialog::updateRates( void )
{		// calculate the Spot Rate (two ways) & update the dialog
		// count the nonzero rates available
	double	spotSlope;
	if ( m_3moYield  > 0.0  &&  m_6moYield  > 0.0 )
	{		// calculate the spotSlope and the extrapolated spot rate
		spotSlope = (m_6moYield - m_3moYield) / (0.5 - 0.25);
		spotRate = (float)(m_3moYield - 0.25 * spotSlope);
		setStatic( "%.3f", spotRate, IDC_RiskFreeRateResult );
	}
	unsigned short nzr = 2										// already guaranteed to have
		//			   + (m_3moYield  > 0.0 ? 1 : 0)			// 3mo & 6mo U.S. Treasury rates
		//			   + (m_6moYield  > 0.0 ? 1 : 0)			// so there's no need to test values again.
					   + (m_1yrYield  > 0.0 ? 1 : 0)			// non-U.S. Treasury yield
					   + (m_2yrYield  > 0.0 ? 1 : 0)			// U.S. Treasury
					   + (m_3yrYield  > 0.0 ? 1 : 0)			// U.S. Treasury
					   + (m_5yrYield  > 0.0 ? 1 : 0)			// U.S. Treasury
					   + (m_7yrYield  > 0.0 ? 1 : 0)			// non-U.S. Treasury yield
					   + (m_10yrYield > 0.0 ? 1 : 0)			// U.S. Treasury
					   + (m_15yrYield > 0.0 ? 1 : 0)			// non-U.S. Treasury yield
					   + (m_20yrYield > 0.0 ? 1 : 0)			// non-U.S. Treasury yield
					   + (m_25yrYield > 0.0 ? 1 : 0)			// non-U.S. Treasury yield
					   + (m_30yrYield > 0.0 ? 1 : 0);			// U.S. Treasury
	if ( nzr < 5 )
		return;

	CShortRateCalibration srb;									// include a 1/8th yr rate to
	srb.r45da = (float)(m_3moYield - 0.125 * spotSlope);		// stabilize curve fitting
	srb.r3mo  = (*userSupplied)[0]  ? m_3moYield  : 0.0f;
	srb.r6mo  = (*userSupplied)[1]  ? m_6moYield  : 0.0f;
	srb.r1yr  = (*userSupplied)[2]  ? m_1yrYield  : 0.0f;
	srb.r2yr  = (*userSupplied)[3]  ? m_2yrYield  : 0.0f;
	srb.r3yr  = (*userSupplied)[4]  ? m_3yrYield  : 0.0f;
	srb.r5yr  = (*userSupplied)[5]  ? m_5yrYield  : 0.0f;
	srb.r7yr  = (*userSupplied)[6]  ? m_7yrYield  : 0.0f;
	srb.r10yr = (*userSupplied)[7]  ? m_10yrYield : 0.0f;
	srb.r15yr = (*userSupplied)[8]  ? m_15yrYield : 0.0f;
	srb.r20yr = (*userSupplied)[9]  ? m_20yrYield : 0.0f;
	srb.r25yr = (*userSupplied)[10] ? m_25yrYield : 0.0f;
	srb.r30yr = (*userSupplied)[11] ? m_30yrYield : 0.0f;

		// compute the spot rate (and all the rates not supplied by the user)
		// from a fitted normal yield curve
	srb.computeRates();
	if ( !(*userSupplied)[0]  )	setStatic( "%.3f", srb.r3mo,  IDC_3moYield  );
	if ( !(*userSupplied)[1]  )	setStatic( "%.3f", srb.r6mo,  IDC_6moYield  );
	if ( !(*userSupplied)[2]  )	setStatic( "%.3f", srb.r1yr,  IDC_1yrYield  );
	if ( !(*userSupplied)[3]  )	setStatic( "%.3f", srb.r2yr,  IDC_2yrYield  );
	if ( !(*userSupplied)[4]  )	setStatic( "%.3f", srb.r3yr,  IDC_3yrYield  );
	if ( !(*userSupplied)[5]  )	setStatic( "%.3f", srb.r5yr,  IDC_5yrYield  );
	if ( !(*userSupplied)[6]  )	setStatic( "%.3f", srb.r7yr,  IDC_7yrYield  );
	if ( !(*userSupplied)[7]  )	setStatic( "%.3f", srb.r10yr, IDC_10yrYield );
	if ( !(*userSupplied)[8]  )	setStatic( "%.3f", srb.r15yr, IDC_15yrYield );
	if ( !(*userSupplied)[9]  )	setStatic( "%.3f", srb.r20yr, IDC_20yrYield );
	if ( !(*userSupplied)[10] )	setStatic( "%.3f", srb.r25yr, IDC_25yrYield );
	if ( !(*userSupplied)[11] )	setStatic( "%.3f", srb.r30yr, IDC_30yrYield );
}			// updateRates()
//--------------------------------------------------------------------
void	CSpotRateEstimatorDialog::SaveSRB( void )
{	CShortRateCalibration		srb;
	srb.r3mo  = userSupplied->query( 0) ?  m_3moYield : 0.0f;	// MFC samples the rates
	srb.r6mo  = userSupplied->query( 1) ?  m_6moYield : 0.0f;	// from the CEdit box contents
	srb.r1yr  = userSupplied->query( 2) ?  m_1yrYield : 0.0f;	//    .
	srb.r2yr  = userSupplied->query( 3) ?  m_2yrYield : 0.0f;	//    .
	srb.r3yr  = userSupplied->query( 4) ?  m_3yrYield : 0.0f;	//    .
	srb.r5yr  = userSupplied->query( 5) ?  m_5yrYield : 0.0f;
	srb.r7yr  = userSupplied->query( 6) ?  m_7yrYield : 0.0f;
	srb.r10yr = userSupplied->query( 7) ? m_10yrYield : 0.0f;
	srb.r15yr = userSupplied->query( 8) ? m_15yrYield : 0.0f;
	srb.r20yr = userSupplied->query( 9) ? m_20yrYield : 0.0f;
	srb.r25yr = userSupplied->query(10) ? m_25yrYield : 0.0f;
	srb.r30yr = userSupplied->query(11) ? m_30yrYield : 0.0f;
	short	res = theApp.pDB->setShortRateBasis( srb );
#ifdef _DEBUG
	if ( res != 0 )
		TRACE( _T("SpotRateEstimatorDialog::SaveSRB: ShortRateBasis write failed, res = %d\n"), res );
#endif
}			// SaveSRB()
//--------------------------------------------------------------------
void	CSpotRateEstimatorDialog::OnKillFocus3moYield() 
{	float	ff = getEditBoxFloat( IDC_3moYield );
	if ( ff == m_3moYield )
		return;
	m_3moYield = ff;
	if ( m_3moYield > 0.0f )
		userSupplied->set(0);
	else
		userSupplied->reset(0);
//	setEditBox( "%.3f", ff, IDC_3moYield );	// echo it back
	updateRates();
}			// OnKillFocus3moYield()
//--------------------------------------------------------------------
void	CSpotRateEstimatorDialog::OnKillFocus6moYield() 
{	float	ff = getEditBoxFloat( IDC_6moYield );
	if ( ff == m_6moYield )
		return;
	m_6moYield = ff;
	if ( m_6moYield > 0.0f )
		userSupplied->set(1);
	else
		userSupplied->reset(1);
//	setEditBox( "%.3f", ff, IDC_6moYield );	// echo it back
	updateRates();
}			// OnKillFocus6moYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnKillFocus1yrYield() 
{	float	ff = getEditBoxFloat( IDC_1yrYield );
	if ( ff == m_1yrYield )
		return;
	m_1yrYield = ff;
	if ( m_1yrYield > 0.0f )
		userSupplied->set(2);
	else
		userSupplied->reset(2);
//	setEditBox( "%.3f", ff, IDC_1yrYield );	// echo it back
	updateRates();
}			// OnKillFocus1yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnKillFocus2yrYield() 
{	float  ff = getEditBoxFloat( IDC_2yrYield );
	if ( ff == m_2yrYield )
		return;
	m_2yrYield = ff;
	if ( m_2yrYield > 0.0f )
		userSupplied->set(3);
	else
		userSupplied->reset(3);
//	setEditBox( "%.3f", ff, IDC_2yrYield );	// echo it back
	updateRates();
}			// OnKillFocus2yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnKillFocus3yrYield() 
{	float  ff = getEditBoxFloat( IDC_3yrYield );
	if ( ff == m_3yrYield )
		return;
	m_3yrYield = ff;
	if ( m_3yrYield > 0.0f )
		userSupplied->set(4);
	else
		userSupplied->reset(4);
//	setEditBox( "%.3f", ff, IDC_3yrYield );	// echo it back
	updateRates();
}			// OnKillFocus3yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnKillFocus5yrYield() 
{	float	ff = getEditBoxFloat( IDC_5yrYield );
	if ( ff == m_5yrYield )
		return;
	m_5yrYield = ff;
	if ( m_5yrYield > 0.0f )
		userSupplied->set(5);
	else
		userSupplied->reset(5);
//	setEditBox( "%.3f", ff, IDC_5yrYield );	// echo it back
	updateRates();
}			// OnKillFocus5yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnKillFocus7yrYield() 
{	float	ff = getEditBoxFloat( IDC_7yrYield );
	if ( ff == m_7yrYield )
		return;
	m_7yrYield = ff;
	if ( m_7yrYield > 0.0f )
		userSupplied->set(6);
	else
		userSupplied->reset(6);
//	setEditBox( "%.3f", ff, IDC_7yrYield );	// echo it back
	updateRates();
}			// OnKillFocus7yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnKillFocus10yrYield() 
{	float	ff = getEditBoxFloat( IDC_10yrYield );
	if ( ff == m_10yrYield )
		return;
	m_10yrYield = ff;
	if ( m_10yrYield > 0.0f )
		userSupplied->set(7);
	else
		userSupplied->reset(7);
//	setEditBox( "%.3f", ff, IDC_10yrYield );	// echo it back
	updateRates();
}			// OnKillFocus10yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnKillFocus15yrYield() 
{	float	ff = getEditBoxFloat( IDC_15yrYield );
	if ( ff == m_15yrYield )
		return;
	m_15yrYield = ff;
	if ( m_15yrYield > 0.0f )
		userSupplied->set(8);
	else
		userSupplied->reset(8);
//	setEditBox( "%.3f", ff, IDC_15yrYield );	// echo it back
	updateRates();
}			// OnKillFocus15yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnKillFocus20yrYield() 
{	float	ff = getEditBoxFloat( IDC_20yrYield );
	if ( ff == m_20yrYield )
		return;
	m_20yrYield = ff;
	if ( m_20yrYield > 0.0f )
		userSupplied->set(9);
	else
		userSupplied->reset(9);
//	setEditBox( "%.3f", ff, IDC_20yrYield );	// echo it back
	updateRates();
}			// OnKillFocus20yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnKillFocus25yrYield() 
{	float	ff = getEditBoxFloat( IDC_25yrYield );
	if ( ff == m_25yrYield )
		return;
	m_25yrYield = ff;
	if ( m_25yrYield > 0.0f )
		userSupplied->set(10);
	else
		userSupplied->reset(10);
//	setEditBox( "%.3f", ff, IDC_25yrYield );	// echo it back
	updateRates();
}			// OnKillFocus25yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnKillFocus30yrYield() 
{	float	ff = getEditBoxFloat( IDC_30yrYield );
	if ( ff == m_30yrYield )
		return;
	m_30yrYield = ff;
	if ( m_30yrYield > 0.0f )
		userSupplied->set(11);
	else
		userSupplied->reset(11);
//	setEditBox( "%.3f", ff, IDC_30yrYield );	// echo it back
	updateRates();
}			// OnKillFocus30yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnSetFocus3moYield()
{
	c_3moYield.SetSel( 0, -1 );
}			// OnSetFocus3moYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnSetFocus6moYield()
{
	c_6moYield.SetSel( 0, -1 );
}			// OnSetFocus6moYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnSetFocus1yrYield()
{
	c_1yrYield.SetSel( 0, -1 );
}			// OnSetFocus1yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnSetFocus2yrYield()
{
	c_2yrYield.SetSel( 0, -1 );
}			// OnSetFocus2yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnSetFocus3yrYield()
{
	c_3yrYield.SetSel( 0, -1 );
}			// OnSetFocus3yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnSetFocus5yrYield()
{
	c_5yrYield.SetSel( 0, -1 );
}			// OnSetFocus5yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnSetFocus7yrYield()
{
	c_7yrYield.SetSel( 0, -1 );
}			// OnSetFocus7yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnSetFocus10yrYield()
{
	c_10yrYield.SetSel( 0, -1 );
}			// OnSetFocus10yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnSetFocus15yrYield()
{
	c_15yrYield.SetSel( 0, -1 );
}			// OnSetFocus15yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnSetFocus20yrYield()
{
	c_20yrYield.SetSel( 0, -1 );
}			// OnSetFocus20yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnSetFocus25yrYield()
{
	c_25yrYield.SetSel( 0, -1 );
}			// OnSetFocus25yrYield()
//--------------------------------------------------------------------
void CSpotRateEstimatorDialog::OnSetFocus30yrYield()
{
	c_30yrYield.SetSel( 0, -1 );
}			// OnSetFocus30yrYield()
//--------------------------------------------------------------------
