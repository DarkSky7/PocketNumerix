// LicenseDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PNMX_Installer.h"
#include "LicenseDialog.h"


// CLicenseDialog dialog

IMPLEMENT_DYNAMIC(CLicenseDialog, CDialog)

CLicenseDialog::CLicenseDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CLicenseDialog::IDD, pParent)
{

}

CLicenseDialog::~CLicenseDialog()
{
}

void CLicenseDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, c_License);
}

BEGIN_MESSAGE_MAP(CLicenseDialog, CDialog)
END_MESSAGE_MAP()

// CLicenseDialog message handlers

BOOL CLicenseDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
/*
	c_License.SetSel( 0, -1, FALSE );		// select everything (don't scroll)
	c_License.Clear();						// delete selection
*/
	wchar_t*	useRights = _T("                                                    INSTALLATION \
& SOFTWARE LICENSE\r\n\
\r\n1. PLATFORM SUPPORT\r\n\
There are device specific versions of BondManager, NillaHedge, StrategyExplorer, and YieldCurveFitter \
for Pocket PC 2002, Pocket PC 2003 SE, Windows Mobile 5, and Windows Mobile 6.\r\n\
\r\n2. INSTALLATION\r\n\
PNMX_Installer performs numerous functions, e.g. selecting executables and dynamic link libraries \
appropriate to your device, downloading them from a server, and installing them on the connected device. \
Consequently, PNMX_Installer is the recommended method of installing BondManager, \
NillaHedge, StrategyExplorer, and YieldCurveFitter.\r\n\
\r\n3. UPDATES & REPAIR INSTALLS\r\n\
BondManager, NillaHedge, StrategyExplorer, and YieldCurveFitter licenses include \
software updates for the life of the device.  To upgrade to the latest version or to repair \
an existing installation, run PNMX_Installer again.\r\n\
\r\n4. USE RIGHTS\r\n\
PNMX_Installer may be installed and used on multiple computers.  \
BondManager, NillaHedge, StrategyExplorer, and YieldCurveFitter \
may be installed and used on one Pocket PC or Windows Mobile device.\r\n\
\r\n5. SUPPORT SERVICES\r\n\
Software is licensed “as is” so PocketNumerix may not provide support services for it.\r\n\
\r\n6. USE RESTRICTIONS\r\n\
PocketNumerix software may not be reverse engineered, decompiled, disassembled, modified,\
or redistributed in any form.  BondManager, NillaHedge, StrategyExplorer, and YieldCurveFitter \
licenses are not transferable from the device they were originally installed on.\r\n\
\r\n7. DISCLAIMER OF WARRANTY\r\n\
Software is licensed “as-is” so you bear the risk of using it.  PocketNumerix provides \
no express warranties, guarantees or conditions.  To the extent permitted under \
local laws, PocketNumerix excludes the implied warranties of merchantability, fitness \
for a particular purpose, and non-infringement.\r\n\
\r\n8. LIMITATION ON AND EXCLUSION OF REMEDIES AND DAMAGES\r\n\
You can recover direct damages up to U.S. $5.00 from PocketNumerix.  \
You cannot recover any other damages, including consequential, lost profits, special, \
indirect or incidental damages.  This limitation applies to claims for breach of contract, \
breach of warranty, guarantee or condition, strict liability, negligence, and other torts \
to the extent permitted by law.  The limitation applies even if PocketNumerix knew or \
should have known about the possibility of damages.\r\n\
\r\n9. ENTIRE AGREEMENT\r\n\
This agreement comprises the entire agreement between you and PocketNumerix.");

	c_License.SetSel( 0, -1, FALSE );		// select everything (don't scroll)
	c_License.ReplaceSel( useRights );
	c_License.SetReadOnly( TRUE );
	c_License.FmtLines( TRUE );				// insert soft carriage returns as needed

	return	TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}
