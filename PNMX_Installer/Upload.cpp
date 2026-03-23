#include "StdAfx.h"
#include "Upload.h"
#include "MimeEncodeDecode.h"
#include "PNMX_InstallerDlg.h"

short	DeviceUpload( void* ctx, wchar_t* progName, wchar_t* deskFileName, 
					  unsigned char* mime, unsigned long fileSize,
					  wchar_t* deviceFileName, CStatic* statusResult, FILETIME* creationTime )
{	const unsigned short MSG_BUF_SIZE = 128;
	wchar_t	msgBuf[ MSG_BUF_SIZE ];
	CPNMX_InstallerDlg* p_PID = (CPNMX_InstallerDlg*)ctx;
	FILE*	fpIn  = NULL;
	HANDLE	h_Out = NULL;
	short	retVal = 0;

		// open the deviceFileName
		// filename, access, sharedMode, security, creation, flags, template
	h_Out = CeCreateFile( deviceFileName, GENERIC_WRITE, FILE_SHARE_WRITE,
						  NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL );
	if ( h_Out == INVALID_HANDLE_VALUE )
	{	swprintf_s( msgBuf, MSG_BUF_SIZE, _T(" %s: unable to open(w) %s "), progName, deviceFileName );
		statusResult->SetWindowTextW( msgBuf );
		if ( fpIn  ) fclose( fpIn  );
		return	-1;
	}

		// open the deskFileName
	errno_t err = _wfopen_s( &fpIn, deskFileName, _T("rb") );
	if ( err != 0 )
	{	swprintf_s( msgBuf, MSG_BUF_SIZE, _T(" %s: unnable to open(r) %s "), progName, deskFileName );
		statusResult->SetWindowTextW( msgBuf );
		return	-2;
	}

		// set up decryption
	unsigned long mimelen = (unsigned long)strlen( (char*)mime );
	unsigned char* ckiv = new unsigned char[ mimelen * 3 / 4 ];
	MimeDecode( mime, mimelen, ckiv );			// decode mime into ckiv
	unsigned long rjrk[ nRjRoundKeys ];			// 120 round keys in Rijndael256
	rjKeyExpansion( ckiv, (unsigned int*)rjrk );				// create RoundKeys for the target executable

		//	start moving bytes...
	long	bytesUploaded = 0;
	if ( fpIn  &&  h_Out )
	{	const unsigned short XFER_BUF_SIZE = 128 * AES_BLOCK_SIZE;		// a multiple AES_BLOCK_SIZE, here = 4096
		unsigned char	buf[ XFER_BUF_SIZE ];
		unsigned long	bytesAvail = 1;
#if DECRYPT_BOM
		unsigned char	cryptBuf[ XFER_BUF_SIZE ];
		do
		{	bytesAvail = (unsigned long)fread( cryptBuf, 1, XFER_BUF_SIZE, fpIn );
			if ( bytesAvail <= 0 )
				break;

				// cfb_decrypt( unsigned char *iv, unsigned char *ibuf, unsigned long iBytes,
				//				unsigned char *obuf, const unsigned long *keySchedule );
			rjDecrypt( ckiv+32, cryptBuf, bytesAvail, buf, (unsigned int *)rjrk );
#else
		do
		{	bytesAvail = fread( buf, 1, XFER_BUF_SIZE, fpIn );
			if ( bytesAvail <= 0 )
				break;
#endif
				// zero indicates failure
			DWORD	bytesWritten;
			BOOL	B1 = CeWriteFile( h_Out, buf, bytesAvail, &bytesWritten, NULL );
			if ( B1 == 0 )
			{	retVal = -3;
				goto	Exit;
			}
			bytesUploaded += bytesWritten;
			swprintf_s( msgBuf, MSG_BUF_SIZE, _T(" %s: %d bytes uploaded"), progName, bytesUploaded );
			statusResult->SetWindowTextW( msgBuf );
		} while ( 1 );
	}
	swprintf_s( msgBuf, MSG_BUF_SIZE, _T(" %s: %d of %d bytes uploaded"), progName, bytesUploaded, fileSize );
	statusResult->SetWindowTextW( msgBuf );
	if ( bytesUploaded != fileSize )
	{	retVal = -4;
		goto	Exit;
	}
	if ( p_PID->m_OsMajVerNum < 5 )
	{	BOOL B1 = CeCloseHandle( h_Out );
		if ( B1 == 0 )
		{	DWORD	err = CeGetLastError();
			const unsigned short	MSG_SIZE = 80;
			wchar_t	msg[MSG_SIZE];
			FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL, err, 0, (LPWSTR)&msg, MSG_SIZE, NULL );
			swprintf_s( msgBuf, MSG_BUF_SIZE, _T(" %s: CeCloseHandle -> %d, %s"), progName, err, msg );
			statusResult->SetWindowTextW( msgBuf );
			retVal = -5;			// creation time is invalid
			goto	Exit;
		}
		h_Out = CeCreateFile( deviceFileName, GENERIC_READ, FILE_SHARE_READ,
						  NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	}
	FILETIME lastAccessTime, lastWriteTime;
	BOOL B1 = CeGetFileTime( h_Out, creationTime, &lastAccessTime, &lastWriteTime );
	if ( B1 == 0 )
	{	DWORD	err = CeGetLastError();
		const unsigned short	MSG_SIZE = 80;
		wchar_t	msg[MSG_SIZE];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL, err, 0, (LPWSTR)&msg, MSG_SIZE, NULL );
		swprintf_s( msgBuf, MSG_BUF_SIZE, _T(" %s: CeGetFileTime -> %d, %s"), progName, err, msg );
		statusResult->SetWindowTextW( msgBuf );
		retVal = -6;			// creation time is invalid
	}

Exit:
	if ( ckiv ) delete [] ckiv;
	if ( h_Out ) CeCloseHandle( h_Out );
	if ( fpIn  ) fclose( fpIn  );
	return	retVal;							// we think of 0 as success
}			// DeviceUpload()
//------------------------------------------------------------------------------------
/*
DWORD	WINAPI	UploadInstallCleanup( void* pThreadCtx )
{	const unsigned short MSG_BUF_SIZE = 256;
	wchar_t	wMsgBuf[ MSG_BUF_SIZE ];
	DWORD	retVal = 0;
	UploadCtx_T* uploadCtx = (UploadCtx_T*)pThreadCtx;

	short	res = DeviceUpload( pThreadCtx );
	if ( res < 0 )
	{	swprintf_s( wMsgBuf, MSG_BUF_SIZE, _T(" %s: upload failed -> %d"), uploadCtx->progName, res );
		uploadCtx->c_StatusResult->SetWindowTextW( wMsgBuf );
		retVal = -1;
		goto	Exit;
	}
	swprintf_s( wMsgBuf, MSG_BUF_SIZE, _T(" %s uploaded"), uploadCtx->progName );
	uploadCtx->c_StatusResult->SetWindowTextW( wMsgBuf );
	uploadCtx->c_Button->SetCheck( BST_CHECKED );
	uploadCtx->c_Button->ShowWindow( SW_SHOWNOACTIVATE );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// we wouldn't be here unless upload was successful, so we can cleanup the DeskTemp file
		// XXX - we could be verifying that the device file is present first
	BOOL B1 = DeleteFile( uploadCtx->deskFileName );			// 0 ==> failure
	if ( ! B1 )
	{
#ifdef _DEBUG
		wchar_t	MsgBuf[128];
		DWORD msgID = GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			0, msgID, 0, (LPTSTR)&MsgBuf, 128, NULL );
		TRACE( _T("PNMX_InstallerDlg::DeleteDesktopFile: %d --> '%s'\n"), msgID, MsgBuf );
#endif
		swprintf_s( wMsgBuf, MSG_BUF_SIZE, _T(" %s deskTemp cleanup failed"), uploadCtx->progName );
		uploadCtx->c_StatusResult->SetWindowTextW( wMsgBuf );
		retVal = -2;
		goto	Exit;
	}
	swprintf_s( wMsgBuf, MSG_BUF_SIZE, _T(" %s deskTemp cleanup OK"), uploadCtx->progName );
	uploadCtx->c_StatusResult->SetWindowTextW( wMsgBuf );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// install progName using wceload.exe
		// second argument contains command line options
		//  3) process attributes NULL
		//  4) thread attributes NULL
		//  5) inherit handles FALSE
		//  6) creation flags - there are possible values, but we don't need anything special
		//  7) environment NULL
		//  8) current directory NULL
		//  9) startup info NULL
		// 10) process info structure pointer
	PROCESS_INFORMATION	procInfo;
	swprintf_s( wMsgBuf, MSG_BUF_SIZE, _T("/noui %s"), uploadCtx->deviceFileName );
	B1 = CeCreateProcess( _T("wceload.exe"), wMsgBuf, NULL, NULL, FALSE,		// 1: appName; 2:commandLine
								  0, NULL, NULL, NULL, &procInfo );
	if ( ! B1 )
	{	swprintf_s( wMsgBuf, MSG_BUF_SIZE, _T(" %s install failed"), uploadCtx->progName );
		uploadCtx->c_StatusResult->SetWindowTextW( wMsgBuf );
		retVal = -3;
		goto	Exit;
	}

		// Verify that <testPath> is present
		// if the installed exe exists, and the RanString.CAB is still in /Temp/PocketNumerix
		// then, delete the CAB from /Temp/PocketNumerix
	HANDLE handle = CeCreateFile( uploadCtx->testPath, GENERIC_READ, FILE_SHARE_READ, NULL,
							      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( handle == INVALID_HANDLE_VALUE )
	{
		DWORD lastError = CeGetLastError();
		bool fileExists = ( lastError == 0  ||  ( lastError != ERROR_FILE_NOT_FOUND  &&  lastError != ERROR_PATH_NOT_FOUND ));
		if ( fileExists )
		{		// cleanup the device resident CAB file
			BOOL B1 = CeDeleteFile( uploadCtx->deviceFileName );		// nonzero indicates success
			if ( B1 )
			{	swprintf_s( wMsgBuf, MSG_BUF_SIZE, _T(" %s installed"), uploadCtx->progName );
				uploadCtx->c_StatusResult->SetWindowTextW( wMsgBuf );
				uploadCtx->c_Button->SetCheck( BST_CHECKED );
				uploadCtx->c_Button->EnableWindow( SH_SHOWWINDOW );

			}
		}
		// license it ...
		// need to know:  ranString, progName(?), uuid, 
	}

Exit:
	delete	uploadCtx;			// cleanup heap
	return	res;
}			// UploadInstallCleanup()
*/
//------------------------------------------------------------------------------------
