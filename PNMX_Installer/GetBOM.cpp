#include "StdAfx.h"
#include <stdio.h>
#include "PNMX_Installer.h"
#include "PNMX_InstallerDlg.h"
#include "HttpGetFile.h"

extern CPNMX_InstallerApp theApp;
//------------------------------------------------------------------------------------
// assumes that theApp.m_DeskTempPath and theApp.m_DeviceTempPath are already defined
DWORD WINAPI	GetBOM( void* pThreadCtx )
{		// GetBOM also runs PpcInstall and parses the output file
	short	retVal = 0;
	short	sres;
	CString	cs = _T("");
	char*	uuidBuf = NULL;

	DownloadCtx_T*	dwnLdCtx = (DownloadCtx_T*)pThreadCtx;
	CPNMX_InstallerDlg*	p_PID = dwnLdCtx->p_PID;
	dwnLdCtx->c_StatusResult = &p_PID->c_DownloadStatusResult;
	dwnLdCtx->expectedFileSize = 0;								// don't know how big the file will be
	strncpy_s( dwnLdCtx->serverName, DwnldCtxServerNameSize, p_PID->lpServerName, DwnldCtxServerNameSize );

		// get the BOM
	DWORD res = DownloadURL( pThreadCtx );
	if ( res != 0 )
	{	retVal = -1;
		dwnLdCtx->p_PID->c_FetchButton.EnableWindow( FALSE );
		goto	Exit;
	}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	const unsigned short WBUF_SIZE = 256;
	wchar_t		wbuf[ WBUF_SIZE];
	retVal = p_PID->ParseBOM();		// get m_RanDir, m_RanPpc (among other things)
	if ( retVal < 0 )
	{	swprintf_s( wbuf, WBUF_SIZE, _T(" ParseBOM failed -> %d"), retVal );
		p_PID->c_StatusResult.SetWindowTextW( wbuf );
		retVal = -2;
		goto	Exit;
	}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// look for PpcInstall - only upload it if it's not already there
	swprintf_s( wbuf, WBUF_SIZE, _T("%s\\PpcInstall.exe"), p_PID->m_DeviceTempPath );	// wbuf is the testPath
	CE_FIND_DATA	findData;
	int	sz = sizeof( findData );
	ZeroMemory( &findData, sz );
	findData.dwFileAttributes  = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_NORMAL;
	HANDLE hndl = CeFindFirstFile( wbuf, &findData );
	if ( hndl == INVALID_HANDLE_VALUE )
	{		// PpcInstall is not present
			// Download and upload PpcInstall
//		p_PID->c_AppImage.SetIcon( theApp.LoadIcon( IDR_MAINFRAME ) );					// IDR_MAINFRAME is already the default image on the dialog
//		short sres = p_PID->DownUpInstall( p_PID->m_RanPpc, _T("PpcInstall"), "0", wbuf, p_PID->m_PpcInstallMemReq,
		sres = p_PID->DownUp( _T("PpcInstall"), "0", wbuf, p_PID->m_PpcInstallMemReq,
								p_PID->c_Ppc_Downloaded, p_PID->c_Ppc_Uploaded,
								p_PID->c_Ppc_Installed, NULL, p_PID->m_PPCckiv, wbuf );
		if ( sres < 0 )
		{	retVal = -3;
			goto	Exit;
		}
	}
	else
	{
		BOOL B1 = CeFindClose( hndl );
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// run PpcIstall in UUID snag mode ( Do we need MSVCR80, MFC80, ATL80 DLLs on the device too? )
		// we must wait for DownUpInstall( PpcInstall ) to complete
	Sleep( 300 );											// ensure that the CAB file Install has completed
	PROCESS_INFORMATION	procInfo;							// don't care
		// PpcInstall u <tempPath>\UUIDfile
	wchar_t	args[ MAX_PATH ];
	swprintf_s( args, MAX_PATH, _T("u %s\\UUID"), p_PID->m_DeviceTempPath );
	sres = p_PID->Execute( wbuf, args, &procInfo );
	if ( sres < 0 )
	{	wchar_t	wbuf[ WBUF_SIZE ];
		swprintf_s( wbuf, WBUF_SIZE, _T(" PpcInstall: launch failed" ), sres ); 
		p_PID->c_StatusResult.SetWindowTextW( wbuf );
		retVal = -4;
		goto	Exit;
	}
	p_PID->c_StatusResult.SetWindowTextW( _T(" PpcInstall: launch succeeded") );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// read UUID on the device after PpcInstall has completed
	swprintf_s( wbuf, WBUF_SIZE, _T("%s\\UUID"), p_PID->m_DeviceTempPath );
	Sleep( 300 );						// ensure that PpcInstall has enough time to run
	HANDLE	h_UUIDout = CeCreateFile( wbuf, GENERIC_READ, FILE_SHARE_READ, NULL,
									  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( h_UUIDout == INVALID_HANDLE_VALUE )
	{	p_PID->c_StatusResult.SetWindowTextW( _T(" Couldn't open UUID") );
		retVal = -5;
		goto	Exit;
	}

	DWORD	flen;
	DWORD	bytesRead = CeGetFileSize( h_UUIDout, &flen );		// reuse bytesRead as an error result
	if ( bytesRead <= 0 )										// > 0 is the file size, flen seems to be always 0
	{	p_PID->c_StatusResult.SetWindowTextW( _T(" Couldn't get UUID result size") );
		retVal = -6;
		goto	Exit;
	}
	flen = bytesRead;		// flen always turns out to be 0 (???)

	bytesRead = 0;
	uuidBuf = new char[ flen + 1 ];								// entire output of the UUIDsnagger
	BOOL B1 = CeReadFile( h_UUIDout, uuidBuf, flen, &bytesRead, NULL );
	if ( B1 == 0 )
	{	DWORD msgID = CeGetLastError();
#ifdef _DEBUG
		wchar_t	MsgBuf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			0, msgID, 0, (LPTSTR)&MsgBuf, 127, NULL );
		TRACE( _T("PNMX_InstallerDlg::Execute: %d --> '%s'\n"), msgID, MsgBuf );
#endif
		p_PID->c_StatusResult.SetWindowTextW( _T(" Can't read UUID") );
		retVal = -7;
		goto	Exit;
	}
	CeCloseHandle( h_UUIDout );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// remove the UUID file
	sres = p_PID->DeleteDeviceFile( _T("PpcInstall"), wbuf );		// still has the full path to the UUID file in it
	if ( sres < 0 )
	{	p_PID->c_StatusResult.SetWindowTextW( _T(" Unable to cleanup UUID file") );
		retVal = -8;
		goto	Exit;												// should we bail out just for this???
	}
/*
		// XXX - maybe put this back after getting the new PpcInstall signed
		// delete PpcInstall.exe from the deviceTempPath
	swprintf_s( wbuf, WBUF_SIZE, _T("%s\\%s"), p_PID->m_DeviceTempPath, _T("PpcInstall.exe") );
	sres = p_PID->DeleteDeviceFile( _T("PpcInstall"), wbuf );
	if ( sres < 0 )
	{	p_PID->c_StatusResult.SetWindowTextW( _T(" Unable to cleanup PpcInstall.exe") );
		retVal = -9;
		goto	Exit;												// should we bail out just for this???
	}
*/
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// parse uuidBuf
	int	ii = 0;
	if ( p_PID->m_UUID ) delete [] p_PID->m_UUID;
	p_PID->m_UUID = GetToken( uuidBuf, ii );
	if ( p_PID->m_UUID == NULL )
	{	p_PID->c_StatusResult.SetWindowTextW( _T(" UUID memory request failed") );
		retVal = -10;
		goto	Exit;
	}
	cs = p_PID->m_UUID;									// ANSI to CString conversion
	p_PID->c_UUIDresult.SetWindowText( cs );			// the hardware UUID reported on the dialog

		// GetDeviceUniqueID (which produces hashed UUIDs) is only available on WM5 and later
	if ( p_PID->m_OsMajVerNum >= 5 )
	{		// BM's hashed UUID
		if ( p_PID->m_HUID_BM )
			delete [] p_PID->m_HUID_BM;
		p_PID->m_HUID_BM = GetToken( uuidBuf, ii );
		if ( p_PID->m_HUID_BM == NULL )
		{	p_PID->c_StatusResult.SetWindowTextW( _T(" HUID_BM memory request failed") );
			retVal = -11;
			goto	Exit;
		}

			// NH's hashed UUID
		if ( p_PID->m_HUID_NH )
			delete [] p_PID->m_HUID_NH;
		p_PID->m_HUID_NH = GetToken( uuidBuf, ii );
		if ( p_PID->m_HUID_NH == NULL )
		{	p_PID->c_StatusResult.SetWindowTextW( _T(" HUID_NH memory request failed") );
			retVal = -12;
			goto	Exit;
		}
/*
			// XXX need new PpcInstall that collects SM_HUIDs before enabling this piece
			// SM's hashed UUID
		if ( p_PID->m_HUID_SM )
			delete [] p_PID->m_HUID_SM;
		p_PID->m_HUID_SM = GetToken( uuidBuf, ii );
		if ( p_PID->m_HUID_SM == NULL )
		{	p_PID->c_StatusResult.SetWindowTextW( _T(" HUID_SM memory request failed") );
			retVal = -13;
			goto	Exit;
		}
*/
			// SX's hashed UUID
		if ( p_PID->m_HUID_SX )
			delete [] p_PID->m_HUID_SX;
		p_PID->m_HUID_SX = GetToken( uuidBuf, ii );
		if ( p_PID->m_HUID_SX == NULL )
		{	p_PID->c_StatusResult.SetWindowTextW( _T(" HUID_SX memory request failed") );
			retVal = -14;
			goto	Exit;
		}

			// YCF's hashed UUID
		if ( p_PID->m_HUID_YCF )
			delete [] p_PID->m_HUID_YCF;
		p_PID->m_HUID_YCF = GetToken( uuidBuf, ii );
		if ( p_PID->m_HUID_YCF == NULL )
		{	p_PID->c_StatusResult.SetWindowTextW( _T(" HUID_YCF memory request failed") );
			retVal = -15;
			goto	Exit;
		}
	}

		// return control to the user
	p_PID->c_FetchButton.SetWindowTextW( _T("Install") );
	p_PID->fetchInProgress = false;
	p_PID->UpdateMemoryStatus();				// no longer enables the Fetch/Install Button
	p_PID->c_StatusResult.SetWindowTextW( _T(" Ready to install") );

Exit:
	if ( uuidBuf ) delete [] uuidBuf;
	delete	dwnLdCtx;
	return	(DWORD)retVal;
}			// GetBOM()
//------------------------------------------------------------------------------------
