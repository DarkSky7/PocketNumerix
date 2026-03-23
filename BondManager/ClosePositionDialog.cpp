// ClosePositionDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "ClosePositionDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "BondManagerApp.h"
extern	CBondManagerApp	theApp;

/////////////////////////////////////////////////////////////////////////////
// CClosePositionDialog dialog

CClosePositionDialog::CClosePositionDialog(CWnd* pParent /*=NULL*/)
	: CNillaDialog(CClosePositionDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClosePositionDialog)
	m_ClosingDate = COleDateTime::GetCurrentTime();
	m_NetProceeds = 0.0;
	m_ClosingNote = _T("");
	//}}AFX_DATA_INIT

	m_MinDate.SetDate( 0, 1, 1 );
}

void CClosePositionDialog::DoDataExchange(CDataExchange* pDX)
{
	CNillaDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClosePositionDialog)
	DDX_Control(pDX, IDC_ClosingDate, c_ClosingDate);
	DDX_Control(pDX, IDC_NetProceeds, c_NetProceeds);
	DDX_Control(pDX, IDC_ClosingNote, c_ClosingNote);
	DDX_DateTimeCtrl(pDX, IDC_ClosingDate, m_ClosingDate);
	DDX_Text(pDX, IDC_ClosingNote, m_ClosingNote);
	DDX_Text(pDX, IDC_NetProceeds, m_NetProceeds);
	DDV_MinMaxDouble(pDX, m_NetProceeds, 1.e-002, 9.e+307);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CClosePositionDialog, CNillaDialog)
	//{{AFX_MSG_MAP(CClosePositionDialog)
	ON_NOTIFY(DTN_CLOSEUP, IDC_ClosingDate, OnCloseUpClosingDate)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_KILLFOCUS, IDC_ClosingDate, OnKillFocusClosingDate)
	ON_NOTIFY(NM_SETFOCUS, IDC_ClosingDate, OnSetFocusClosingDate)
	ON_EN_KILLFOCUS(IDC_NetProceeds, OnKillFocusNetProceeds)
	ON_EN_SETFOCUS(IDC_NetProceeds, OnSetFocusNetProceeds)
	ON_EN_SETFOCUS(IDC_ClosingNote, OnSetFocusClosingNote)
	ON_EN_KILLFOCUS(IDC_ClosingNote, OnKillFocusClosingNote)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClosePositionDialog message handlers

BOOL CClosePositionDialog::OnInitDialog() 
{
	CNillaDialog::OnInitDialog();
	
	setEditBox( "%.2f", m_NetProceeds, IDC_NetProceeds );
	c_ClosingDate.SetTime( m_ClosingDate );
		// by now, we expect caller to have modified m_MinDate
	COleDateTime	maxDate = COleDateTime::GetCurrentTime();
	c_ClosingDate.SetRange( &m_MinDate, &maxDate );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//---------------------------------------------------------------------------
HBRUSH	CClosePositionDialog::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor ) 
{
	return	CNillaDialog::OnCtlColor( pDC, pWnd, nCtlColor );
}			// OnCtlColor()
//---------------------------------------------------------------------------
void	CClosePositionDialog::OnKillFocusNetProceeds()
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	double	dd = getEditBoxDouble( IDC_NetProceeds );
	if ( m_NetProceeds == dd )
		return;

	m_NetProceeds = dd;
	setEditBox( "%.2f", dd, IDC_NetProceeds );	

	c_ClosingNote.SetSel( 0, -1 );
	c_ClosingNote.SetFocus();	
}			// OnKillFocusNetProceeds()
//---------------------------------------------------------------------------
void CClosePositionDialog::OnSetFocusNetProceeds()
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}
//---------------------------------------------------------------------------
void CClosePositionDialog::killFocusClosingDateWork( NMHDR* pNMHDR, LRESULT* pResult )
{
	COleDateTime	odt;
	c_ClosingDate.GetTime( odt );
	if ( odt == m_ClosingDate )
		return;
	m_ClosingDate = odt;
	
	c_NetProceeds.SetSel( 0, -1 );
	c_NetProceeds.SetFocus();
	
	*pResult = 0;
}			// OnKillFocusClosingDate()
//---------------------------------------------------------------------------
void CClosePositionDialog::OnCloseUpClosingDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	killFocusClosingDateWork( pNMHDR, pResult );
}			// OnCloseUpClosingDate()
//---------------------------------------------------------------------------
void CClosePositionDialog::OnKillFocusClosingDate( NMHDR* pNMHDR, LRESULT* pResult )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
	killFocusClosingDateWork( pNMHDR, pResult );
}			// OnKillFocusClosingDate()
//---------------------------------------------------------------------------
void CClosePositionDialog::OnSetFocusClosingDate(NMHDR *pNMHDR, LRESULT *pResult)
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
	*pResult = 0;
}
//---------------------------------------------------------------------------
void CClosePositionDialog::OnKillFocusClosingNote( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_DOWN );
}			// OnKillfocusClosingNote()
//---------------------------------------------------------------------------
void CClosePositionDialog::OnSetFocusClosingNote( void )
{
	SHSipPreference( GetSafeHwnd(), SIP_UP );
}			// OnSetFocusClosingNote()
//---------------------------------------------------------------------------
