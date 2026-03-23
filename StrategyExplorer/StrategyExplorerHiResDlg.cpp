// StrategyExplorerHiResDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StrategyExplorer.h"
#include "StrategyExplorerHiResDlg.h"


// CStrategyExplorerHiResDlg dialog

IMPLEMENT_DYNAMIC(CStrategyExplorerHiResDlg, CDialog)

CStrategyExplorerHiResDlg::CStrategyExplorerHiResDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStrategyExplorerHiResDlg::IDD, pParent)
{

}

CStrategyExplorerHiResDlg::~CStrategyExplorerHiResDlg()
{
}

void CStrategyExplorerHiResDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CStrategyExplorerHiResDlg, CDialog)
END_MESSAGE_MAP()

const	unsigned short	nStrategies = 21;

// CStrategyExplorerHiResDlg message handlers
virtual BOOL CStrategyExplorerHiResDlg::OnInitDialog()
{	extern	CString		strategyNames[];

	CStrategyExplorerDlg::OnInitDialog();
		// add two more strategy names in hi-res
	c_Strategy.AddString( strategyNames[nStrategies] );

		// we know that vga is true - initialize option4 controls
}			// OnInitDialog()
//--------------------------------------------------------------------------//
//////////////////////////////////////////////////////////////////////////////
//								Plot related								//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
