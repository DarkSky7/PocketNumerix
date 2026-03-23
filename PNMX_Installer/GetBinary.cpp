#include "StdAfx.h"
#include "PNMX_Installer.h"
#include "PNMX_InstallerDlg.h"
#include "shlobj.h"								// for SHGetFolderPathAndSubDir()
#include <stdio.h>
//#include <rapi.h>
#include "HttpGetFile.h"

extern CPNMX_InstallerApp theApp;
//------------------------------------------------------------------------------------
// assumes that theApp.m_DeskTempPath and theApp.m_DeviceTempPath are already defined
DWORD WINAPI	GetBinary( void* pThreadCtx )
{	CPNMX_InstallerDlg* p_PID = (CPNMX_InstallerDlg*)pThreadCtx;
	wchar_t	testPath[ MAX_PATH ];
	bool	anyAppToInstall = ( p_PID->m_NumProdsRequiringMFClibs > 0 );
	bool	DLLsNeeded = (	   anyAppToInstall
						   &&  p_PID->m_OsMajVerNum > 3
						   &&  p_PID->m_OsMajVerNum < 7 );
	bool	subscriptionsToActivate = p_PID->m_OptionChains  ||  p_PID->m_TBillRates  ||  p_PID->m_CovarAcct;
	short	retVal = 0;
	short	res = 0;

		// conditionally hide Binaries Installed button and label
	if (  anyAppToInstall
		  && (	   DLLsNeeded						   != p_PID->m_DLLsInstalled
			   ||  (p_PID->m_BondManager == TRUE)	   != p_PID->m_BondManagerInstalled
			   ||  (p_PID->m_NillaHedge == TRUE)	   != p_PID->m_NillaHedgeInstalled
//			   ||  (p_PID->m_StockManager == TRUE)	   != p_PID->m_StockManagerInstalled
			   ||  (p_PID->m_StrategyExplorer == TRUE) != p_PID->m_StrategyExplorerInstalled
			   ||  (p_PID->m_YieldCurveFitter == TRUE) != p_PID->m_YieldCurveFitterInstalled  ) )
	{		// note that we don't change the state of the BinariesInstalled button unless there's something to install
		p_PID->c_BinariesInstalledLabel.SetWindowText( _T("") );
		p_PID->c_BinariesInstalledLabel.EnableWindow( FALSE );
		p_PID->c_BinariesInstalled.SetCheck( FALSE );
		p_PID->c_BinariesInstalled.ShowWindow( SW_HIDE );
			// for the LicensesInstalled button
		p_PID->c_LicensesInstalledLabel.SetWindowText( _T("") );
		p_PID->c_LicensesInstalledLabel.EnableWindow( FALSE );
		p_PID->c_LicensesInstalled.SetCheck( FALSE );
		p_PID->c_LicensesInstalled.ShowWindow( SW_HIDE );
	}
		// conditionally hide Subscriptions Activated button and label
	if (  subscriptionsToActivate
		  && (	   (p_PID->m_OptionChains == TRUE) != p_PID->m_OptionChainsInstalled
//			   ||  (p_PID->m_CovarAcct == TRUE)	   != p_PID->m_CovarAcctInstalled
			   ||  (p_PID->m_TBillRates == TRUE)   != p_PID->m_TBillRatesInstalled			   ) )
	{	p_PID->c_SubscriptionsActivatedLabel.SetWindowText( _T("") );
		p_PID->c_SubscriptionsActivatedLabel.EnableWindow( FALSE );
		p_PID->c_SubscriptionsActivated.SetCheck( FALSE );
		p_PID->c_SubscriptionsActivated.ShowWindow( SW_HIDE );
	}

	HINSTANCE hInstance = AfxGetInstanceHandle();
	if ( DLLsNeeded != p_PID->m_DLLsInstalled )
	{		// i.e. it's a PPC03, WM5, or WM6
		if ( DLLsNeeded )
		{	swprintf_s( testPath, MAX_PATH, _T("%s\\MFC80U.DLL"), p_PID->m_progFilesDir );
			p_PID->c_AppImage.SetIcon( theApp.LoadIcon( IDI_MFC ) );
#if EnableInstall
			res = p_PID->DownUpInstall( _T("MFC8.0 DLLs"), "0", testPath, p_PID->m_MFCmemReq,
										p_PID->c_MFC_Downloaded, p_PID->c_MFC_Uploaded, p_PID->c_MFC_Installed,
										p_PID->m_MFCmemReq, p_PID->m_MFCckiv, _T("MFC_DLLs") );
			if ( res < 0 )
			{	retVal = 2;
				goto	Exit;
			}
#endif
			p_PID->m_DLLsInstalled = true;
			p_PID->m_ProductsMask &= (~0x40);				// SMYTSONB, M at 64
//			p_PID->m_MemoryRequired -= 3 * p_PID->m_MFCmemReq;
		}
	}

	if ( (p_PID->m_YieldCurveFitter == TRUE) != p_PID->m_YieldCurveFitterInstalled )
	{	p_PID->c_YieldCurveFitter.EnableWindow( FALSE );
		swprintf_s( testPath, MAX_PATH, _T("%s\\YieldCurveFitter.exe"), p_PID->m_progFilesDir );
		p_PID->c_AppImage.SetIcon( theApp.LoadIcon( IDI_YIELDCURVEFITTER ) );
#if EnableInstall
		res = p_PID->DownUpInstall( _T("YieldCurveFitter"), p_PID->m_HUID_YCF, testPath, p_PID->m_YCFmemReq,
									p_PID->c_YCF_Downloaded, p_PID->c_YCF_Uploaded, p_PID->c_YCF_Installed,
									p_PID->m_YCFmemReq, p_PID->m_YCFckiv, _T("YieldCurveFitter") );
		if ( res < 0 )
		{	retVal = 3;
			goto	Exit;
		}
#endif
		p_PID->m_YieldCurveFitterInstalled = true;
		res = p_PID->License( _T("YieldCurveFitter"), p_PID->m_HUID_YCF, p_PID->c_YCF_Licensed );
		if ( res < 0 )
		{	retVal = 4;
			goto	Exit;
		}
		p_PID->m_NumProdsRequiringMFClibs--;
		p_PID->m_YieldCurveFitterLicensed = true;
		p_PID->m_ProductsMask &= (~0x20);					// SMYTSONB, Y at 32
//		p_PID->m_MemoryRequired -= 3 * p_PID->m_YCFmemReq;
	}

	if ( (p_PID->m_StrategyExplorer == TRUE) != p_PID->m_StrategyExplorerInstalled )
	{	p_PID->c_StrategyExplorer.EnableWindow( FALSE );
		swprintf_s( testPath, MAX_PATH, _T("%s\\StrategyExplorer.exe"), p_PID->m_progFilesDir );
		p_PID->c_AppImage.SetIcon( theApp.LoadIcon( IDI_STRATEGYEXPLORER ) );
#if EnableInstall
		res = p_PID->DownUpInstall( _T("StrategyExplorer"), p_PID->m_HUID_SX, testPath, p_PID->m_SXmemReq,
									p_PID->c_SX_Downloaded, p_PID->c_SX_Uploaded, p_PID->c_SX_Installed,
									p_PID->m_SXmemReq, p_PID->m_SXckiv, _T("StrategyExplorer") );
		if ( res < 0 )
		{	retVal = 5;
			goto	Exit;
		}
#endif
		p_PID->m_StrategyExplorerInstalled = true;
		res = p_PID->License( _T("StrategyExplorer"), p_PID->m_HUID_SX, p_PID->c_SX_Licensed );
		if ( res < 0 )
		{	retVal = 6;
			goto	Exit;
		}
		p_PID->m_NumProdsRequiringMFClibs--;
		p_PID->m_StrategyExplorerLicensed = true;
		p_PID->m_ProductsMask &= (~0x8);					// SMYTSONB, SX at 8
//		p_PID->m_MemoryRequired -= 3 * p_PID->m_SXmemReq;
	}
/*						// XXX need new PpcInstall (that collects SM_HUIDs before we can enable StockManager
		// StockManager
	if ( (p_PID->m_StockManager == TRUE) != p_PID->m_StockManagerInstalled )
	{	p_PID->c_StockManager.EnableWindow( FALSE );
		swprintf_s( testPath, MAX_PATH, _T("%s\\StockManager.exe"), p_PID->m_progFilesDir );
		p_PID->c_AppImage.SetIcon( theApp.LoadIcon( IDI_STOCKMANAGER ) );
#if EnableInstall
		res = p_PID->DownUpInstall( _T("StockManager"), p_PID->m_HUID_SM, testPath, p_PID->m_SMmemReq,
									p_PID->c_SM_Downloaded, p_PID->c_SM_Uploaded, p_PID->c_SM_Installed,
									p_PID->m_SMmemReq, p_PID->m_SMckiv, _T("StockManager") );
		if ( res < 0 )
		{	retVal = 9;
			goto	Exit;
		}
#endif
		p_PID->m_StockManagerInstalled = true;
		res = p_PID->License( _T("StockManager"), p_PID->m_HUID_SM, p_PID->c_SM_Licensed );
		if ( res < 0 )
		{	retVal = 10;
			goto	Exit;
		}
		p_PID->m_NumProdsRequiringMFClibs--;
		p_PID->m_StockManagerLicensed = true;
		p_PID->m_ProductsMask &= (~0x80);					// SMYTSONB, SM at 128
//		p_PID->m_MemoryRequired -= 3 * p_PID->m_SMmemReq;
	}
*/
	if ( (p_PID->m_BondManager == TRUE) != p_PID->m_BondManagerInstalled )
	{	p_PID->c_BondManager.EnableWindow( FALSE );
		swprintf_s( testPath, MAX_PATH, _T("%s\\BondManager.exe"), p_PID->m_progFilesDir );
		p_PID->c_AppImage.SetIcon( theApp.LoadIcon( IDI_BONDMANAGER ) );
#if EnableInstall
		res = p_PID->DownUpInstall( _T("BondManager"), p_PID->m_HUID_BM, testPath, p_PID->m_BMmemReq,
									p_PID->c_BM_Downloaded, p_PID->c_BM_Uploaded, p_PID->c_BM_Installed,
									p_PID->m_BMmemReq, p_PID->m_BMckiv, _T("BondManager") );
		if ( res < 0 )
		{	retVal = 7;
			goto	Exit;
		}
#endif
		p_PID->m_BondManagerInstalled = true;
		res = p_PID->License( _T("BondManager"), p_PID->m_HUID_BM, p_PID->c_BM_Licensed );
		if ( res < 0 )
		{	retVal = 8;
			goto	Exit;
		}
		p_PID->m_NumProdsRequiringMFClibs--;
		p_PID->m_BondManagerLicensed = true;
		p_PID->m_ProductsMask &= (~0x1);					// SMYTSONB, B at 1
//		p_PID->m_MemoryRequired -= 3 * p_PID->m_BMmemReq;
	}
		// NillaHedge
	if ( (p_PID->m_NillaHedge == TRUE) != p_PID->m_NillaHedgeInstalled )
	{	p_PID->c_NillaHedge.EnableWindow( FALSE );
		swprintf_s( testPath, MAX_PATH, _T("%s\\NillaHedge.exe"), p_PID->m_progFilesDir );
		p_PID->c_AppImage.SetIcon( theApp.LoadIcon( IDI_NILLAHEDGE ) );
#if EnableInstall
		res = p_PID->DownUpInstall( _T("NillaHedge"), p_PID->m_HUID_NH, testPath, p_PID->m_NHmemReq,
									p_PID->c_NH_Downloaded, p_PID->c_NH_Uploaded, p_PID->c_NH_Installed,
									p_PID->m_NHmemReq, p_PID->m_NHckiv, _T("NillaHedge") );
		if ( res < 0 )
		{	retVal = 9;
			goto	Exit;
		}
#endif
		p_PID->m_NillaHedgeInstalled = true;
		res = p_PID->License( _T("NillaHedge"), p_PID->m_HUID_NH, p_PID->c_NH_Licensed );
		if ( res < 0 )
		{	retVal = 10;
			goto	Exit;
		}
		p_PID->m_NumProdsRequiringMFClibs--;
		p_PID->m_NillaHedgeLicensed = true;
		p_PID->m_ProductsMask &= (~0x2);					// SMYTSONB, N at 2
//		p_PID->m_MemoryRequired -= 3 * p_PID->m_NHmemReq;
	}

		// Indicate that binaries have been installed
		// only perform the following if there were binaries selected
	if (	anyAppToInstall
		&&  DLLsNeeded == p_PID->m_DLLsInstalled
		&&	(p_PID->m_BondManager == TRUE) == p_PID->m_BondManagerInstalled
		&&	(p_PID->m_NillaHedge == TRUE) == p_PID->m_NillaHedgeInstalled
//		&&	(p_PID->m_StockManager == TRUE) == p_PID->m_StockManagerInstalled				// need new PpcInstall
		&&	(p_PID->m_StrategyExplorer == TRUE) == p_PID->m_StrategyExplorerInstalled
		&&	(p_PID->m_YieldCurveFitter == TRUE) == p_PID->m_YieldCurveFitterInstalled )
	{	p_PID->c_BinariesInstalledLabel.SetWindowText( _T("Binaries Installed") );
		p_PID->c_BinariesInstalledLabel.EnableWindow( TRUE );
		p_PID->c_BinariesInstalled.SetCheck( TRUE );
		p_PID->c_BinariesInstalled.ShowWindow( SW_SHOWNOACTIVATE );

		if (	(p_PID->m_BondManager == TRUE) == p_PID->m_BondManagerLicensed
			&&  (p_PID->m_NillaHedge == TRUE) == p_PID->m_NillaHedgeLicensed
//			&&  (p_PID->m_StockManager == TRUE) == p_PID->m_StockManagerLicensed			// need new PpcInstall
			&&  (p_PID->m_StrategyExplorer == TRUE) == p_PID->m_StrategyExplorerLicensed
			&&  (p_PID->m_YieldCurveFitter == TRUE) == p_PID->m_YieldCurveFitterLicensed )
		{	p_PID->c_LicensesInstalled.SetCheck( TRUE );
			p_PID->c_LicensesInstalled.ShowWindow( SW_SHOWNOACTIVATE );
			p_PID->c_LicensesInstalledLabel.SetWindowText( _T("Licenses Installed") );
			p_PID->c_LicensesInstalledLabel.EnableWindow( TRUE );
		}
	}
	if ( p_PID->m_BondManager  ||  p_PID->m_NillaHedge  ||  p_PID->m_YieldCurveFitter )
	{		// create the PocketNumerix DocumentDirectory and associated registry key
		wchar_t*	path = _T("\\My Documents\\PocketNumerix");
		BOOL b1 = CeCreateDirectory( path, NULL );
#ifdef _DEBUG
		TRACE( _T("GetBinary: CreateDirectory(%s) %s\n"), path, ( b1 ? _T("succeeded") : _T("failed") ) );
#endif
		unsigned short slen = (unsigned short)wcslen( path );
		wchar_t* key = _T("SOFTWARE\\PocketNumerix\\DocumentDirectory");
		int sres = p_PID->RegisterKeyValuePair( HKEY_CURRENT_USER, key, REG_BINARY, slen*sizeof(wchar_t), (void*)path );
		if ( sres < 0 )		{	retVal = 9;	goto	Exit;	}
	}

	if ( (p_PID->m_OptionChains == TRUE) != p_PID->m_OptionChainsInstalled )
	{	res = p_PID->Subscribe( _T("OptionChains"), p_PID->m_HUID_NH, p_PID->c_OCR_Downloaded, p_PID->c_OCR_Installed );
		if ( res < 0 )
		{	retVal = 11;
			goto	Exit;
		}
		p_PID->m_OptionChainsInstalled = true;
		p_PID->c_OptionChains.EnableWindow( FALSE );
	}

	if ( (p_PID->m_TBillRates == TRUE) != p_PID->m_TBillRatesInstalled )
	{	res = p_PID->Subscribe( _T("TBillRates"), p_PID->m_HUID_YCF, p_PID->c_TBR_Downloaded, p_PID->c_TBR_Installed );
		if ( res < 0 )
		{	retVal = 12;
			goto	Exit;
		}
		p_PID->m_TBillRatesInstalled = true;
		p_PID->c_TBillRates.EnableWindow( FALSE );
	}
/*
	if ( (p_PID->m_CovarAcct == TRUE) != p_PID->m_CovarAcctInstalled )
	{	res = p_PID->Subscribe( _T("CovarAcct"), p_PID->m_HUID_SM, p_PID->c_VAR_Downloaded, p_PID->c_VAR_Installed );
		if ( res < 0 )
		{	retVal = 13;
			goto	Exit;
		}
		p_PID->m_CovarAcctInstalled = true;
		p_PID->c_CovarAcct.EnableWindow( FALSE );
	}
*/
/*		// can recover the DeviceId from the license key now
		// put the deviceID into the device's registry (should know the deviceId at this point)
//	short sres = p_PID->RegisterKeyValuePair( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\PocketNumerix\\DeviceId"), REG_BINARY,
//											sizeof(p_PID->m_DeviceID), (void*)&(p_PID->m_DeviceID) );
	if ( sres < 0 )	p_PID->c_StatusResult.SetWindowTextW( _T("Failed to register device ID") );
*/
Exit:
		// Indicate that the subscriptions have been activated
	if (	subscriptionsToActivate
//		&&	(p_PID->m_CovarAcct == TRUE)    == p_PID->m_CovarAcctInstalled				// XXX need new PpcInstall
		&&  (p_PID->m_OptionChains == TRUE) == p_PID->m_OptionChainsInstalled
		&&	(p_PID->m_TBillRates == TRUE)   == p_PID->m_TBillRatesInstalled		)
	{	p_PID->c_SubscriptionsActivatedLabel.SetWindowText( _T("Subscriptions Activated") );
		p_PID->c_SubscriptionsActivatedLabel.EnableWindow( TRUE );
		p_PID->c_SubscriptionsActivated.SetCheck( TRUE );
		p_PID->c_SubscriptionsActivated.ShowWindow( SW_SHOWNOACTIVATE );
	}
		// restore the installer's icon in the dialog and enable the Install button if there's more to install on the BOM
	p_PID->c_AppImage.SetIcon( theApp.LoadIcon( IDR_MAINFRAME ) );

//	p_PID->c_FetchButton.EnableWindow( p_PID->m_ProductsMask );
	p_PID->fetchInProgress = false;
	p_PID->UpdateMemoryStatus();		// enables the Fetch/Install Button if appropriate
	return	retVal;
}		// GetBinary()
//------------------------------------------------------------------------------------
		// What do we know?
		// p_PID->m_DeskTempPath		m_DeviceTempPath
		// p_PID->lpFileName			urlRequest
		// p_PID->m_UUIDresult			m_UUIDout (buffer of the entire file)
		// p_PID->m_DeviceID
		// p_PID->m_RanPpc
		// p_PID->m_BondManager			m_RanBM		m_BMmemReq
		// p_PID->m_NillaHedge			m_RanNH		m_NHmemReq
		// p_PID->m_OptionChains
		// p_PID->m_StrategyExplorer	m_RanSX		m_SXmemReq
		// p_PID->m_TBillRates
		// p_PID->m_YieldCurveFitter	m_RanYCF	m_YCFmemReq
