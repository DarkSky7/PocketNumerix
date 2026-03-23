#include "StdAfx.h"
//#include "Afx.h"				// CString's require all of MFC

//#include "Winsock2.h"
#include "HttpGetFile.h"
#include "PNMX_InstallerDlg.h"
#include "PNMX_Installer.h"

extern	CPNMX_InstallerApp	theApp;

//------------------------------------------------------------------------------------
// DownloadURL uses:  ctx->progName, ctx->serverName, ctx->urlRequest, ctx->c_StatusResult, ctx->fileName, ctx->fileSize
DWORD	WINAPI	DownloadURL( void* pThreadCtx )
{	FILE*	fp;												// closed at CloseFile
	SOCKET	aSocket;										// closed at CloseSocket
	unsigned long	recvWaitTime =  30000;					// thirty seconds default
	unsigned long	sendWaitTime =  30000;					// thirty seconds default
	DownloadCtx_T* ctx = (DownloadCtx_T*)pThreadCtx;
	int		retVal = 0;
	theApp.DoWaitCursor(1);									// reset(-1) at Cleanup

		// this version was originally based on: <http://www.sockaddr.com/ExampleSourceCode.html>
		// see also:  <http://www.codeproject.com/internet/winsockintro02.asp>
		// WSA init phase
	WORD wVersionRequested = MAKEWORD( 1, 1 );
	WSADATA wsaData;

		// Initialize WinSock.dll
	size_t	nRet = WSAStartup( wVersionRequested, &wsaData );
	if ( nRet )
	{	wchar_t	msg[ 40 ];
		switch ( nRet )
		{	case WSASYSNOTREADY:
				wcscpy_s( msg, 40, _T("WSA SYS NOT READY") );
				break;
			case WSAVERNOTSUPPORTED:
				wcscpy_s( msg, 40, _T("WSA VER 1.1 NOT SUPPORTED") );
				break;
			case WSAEINVAL:
				wcscpy_s( msg, 40, _T("WSA E INVAL") );
				break;
			default:
				swprintf_s( msg, 40, _T("WSA Startup error %d"), nRet );
		}
		wchar_t   msg2[ 80 ];
		swprintf_s( msg2, 80, _T(" %s: %s"), ctx->progName, msg );
		ctx->c_StatusResult->SetWindowText( msg2 );
#ifdef _DEBUG
		TRACE( _T("DownloadURL(-1): WSAStartup() --> %d\n"), nRet );
#endif
		retVal = -1;
		goto	Cleanup;
	}

		// Check WinSock version
	if ( wsaData.wVersion != wVersionRequested )
	{
#ifdef _DEBUG
		TRACE( _T("DownloadURL(-2): Winsock v1.1 not supported\n") );
#endif
		retVal = -2;
		goto	Cleanup;
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// Set the port number for the HTTP service on TCP
	SOCKADDR_IN		saServer;
	saServer.sin_port = htons( 80 );
	saServer.sin_family = AF_INET;

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// Use inet_addr() to determine if we're
		// dealing with a name or an address
	LPHOSTENT		lpHostEntry;
	unsigned int	addr;
	addr = inet_addr( ctx->serverName );
	if ( addr != INADDR_NONE )
	{		// ServerName was a dotted IPV4 address
			// finish setting up saServer (for use in connect)
		IN_ADDR	inadr;
		inadr.S_un.S_addr = addr;
		saServer.sin_addr = inadr;
	}
	else	// ServerName was not an IPV4 address string
	{		// maybe it was an FQDN like OptimalPortfolio.net
		wchar_t msg[ 40 ];
		lpHostEntry = gethostbyname( ctx->serverName );		// causes broadcast (therefore deprecated)
		if ( lpHostEntry != NULL )
		{		// finish setting up saServer (for use in connect)
			saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
		}
		else
		{
			int	err = WSAGetLastError();
			switch ( err )
			{	case WSATRY_AGAIN:
					wcscpy_s( msg, 40, _T("WSA TRY AGAIN") );
					break;
				case WSANOTINITIALISED:
					wcscpy_s( msg, 40, _T("WSA NOT INITIALIZED") );	// WSAStartup failed
					break;
				case WSAENETDOWN:
					wcscpy_s( msg, 40, _T("WSA E NET DOWN") );		// Network subsystem has failed
					break;
				case WSAHOST_NOT_FOUND:
					wcscpy_s( msg, 40, _T("WSA HOST NOT FOUND") );	// Authoritative answer host not found
					break;
				case WSANO_RECOVERY:
					wcscpy_s( msg, 40, _T("WSA NO RECOVERY") );		// A nonrecoverable error occurred
					break;
				case WSANO_DATA:
					wcscpy_s( msg, 40, _T("WSA NO DATA") );			// Valid name, no data record of requested type
					break;
				case WSAEINPROGRESS:
					wcscpy_s( msg, 40, _T("WSA E IN PROGRESS") );	// A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function
					break;
				case WSAEAFNOSUPPORT:
					wcscpy_s( msg, 40, _T("WSA E AF NO SUPPORT") );	// The type specified is not supported by the Windows Sockets implementation
					break;
				case WSAEFAULT:
					wcscpy_s( msg, 40, _T("WSA E FAULT") );			// The addr parameter is not a valid part of the user address space, or the len parameter is too small
					break;
				case WSAEINTR:
					wcscpy_s( msg, 40, _T("WSA E INTR") );			// A blocking Windows Socket 1.1 call was cancelled through WSACancelBlockingCall
					break;
				default:
					swprintf_s( msg, 40, _T("Host error %d"), err ); 
			}		
			wchar_t		msg2[ 80 ];
			swprintf_s( msg2, 80, _T(" %s: %s"), ctx->progName, msg );
			ctx->c_StatusResult->SetWindowText( msg2 );
			retVal = -3;
			goto	Cleanup;
		}
	}
		// report the sin_addr we came up with
#ifdef _DEBUG
	TRACE( _T("DownloadURL: sin_addr = %d\n"), saServer.sin_addr );
#endif

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// Create a TCP/IP stream socket
	aSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( aSocket == INVALID_SOCKET )
	{
#ifdef _DEBUG
		TRACE( _T("DownloadURL(-4): socket() --> INVALID_SOCKET\n") ); 
#endif
		retVal = -4;
		goto	Cleanup;				// no socket to close
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// put the socket into asynchronous mode
	unsigned long	arg;
	arg = 1;						// anything but 0 will put the socket in non-blocking mode.
	nRet = ioctlsocket( aSocket, FIONBIO, &arg );
	if ( nRet == SOCKET_ERROR )
	{
#ifdef _DEBUG
		TRACE( _T("DownloadURL(-5): ioctlsocket() --> SOCKET_ERROR = %d\n"), nRet );
#endif
		retVal = -5;
		goto	CloseSocket;
	}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// See if we connect() with the asynchronous socket
	nRet = connect( aSocket, (LPSOCKADDR)&saServer, sizeof(SOCKADDR_IN) );
	if ( nRet != 0 )
	{	wchar_t msg[ 40 ];
		nRet = WSAGetLastError();
		switch ( nRet )
		{	case WSAEWOULDBLOCK:
				wcscpy_s( msg, 40, _T("WSA E BLOCKED (waiting...)") );
				break;
			case WSAECONNREFUSED:
				wcscpy_s( msg, 40, _T("WSA E CONN REFUSED") );
				break;
			case WSAENETUNREACH:
				wcscpy_s( msg, 40, _T("WSA E NET UNREACH") );
				break;
			case WSAETIMEDOUT:
				wcscpy_s( msg, 40, _T("WSA E TIMED OUT") );
				break;
			case WSAEISCONN:
				wcscpy_s( msg, 40, _T("WSA E IS CONN") );
				break;
			case WSAEINVAL:
				wcscpy_s( msg, 40, _T("WSA E INVAL") );
				break;
			case WSAEALREADY:
				wcscpy_s( msg, 40, _T("WSA E ALREADY") );
				break;
			default:
				swprintf_s( msg, 40, _T("Connect error %d"), nRet ); 
		}
		wchar_t	msg2[ 80 ];
		swprintf_s( msg2, 80, _T(" %s: %s"), ctx->progName, msg );
		ctx->c_StatusResult->SetWindowText( msg2 );
#ifdef _DEBUG
		TRACE( _T("DownloadURL: connect() --> %s\n"), msg2 );
#endif
	}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// set up to run select() with aSocket
	fd_set fds;
	ZeroMemory( &fds, sizeof(fd_set) );
	FD_SET( aSocket, &fds );
		// stuff the sendWaitTime into a timeval struct
	timeval	timeout;
	timeout.tv_sec = sendWaitTime / 1000;					// milliseconds to seconds
	timeout.tv_usec = sendWaitTime * 1000 - timeout.tv_sec * 1000000;
		// first arg is nfds - only used in Berkeley sockets
		// second arg is readfds - a pointer to a set of sockets to be checked for readabiity
		// third arg is writefds - a pointer to a set of sockets to be checked for wrtiabiity
		// fourth arg is exceptfds - a pointer to a set of sockets to be checked for errors
		// fifth arg - timeval structure specifying max timeout for whatever
	nRet = select( 0, NULL, &fds, NULL, &timeout );
	if ( nRet == 0 )
	{		// time limit expired
		wchar_t	msg[ 40 ];
		swprintf_s( msg, 40, _T(" %s: SEND Socket Timeout"), ctx->progName );
		ctx->c_StatusResult->SetWindowText( msg );
#ifdef _DEBUG
		TRACE( _T("DownloadURL(-6):%s\n"), msg );
#endif
		retVal = -6;
		goto	CloseSocket;
	}
		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// Format a GET request
	char	szBuffer[ 4096 ];			// server is currently closing the connection after receiving 339 bytes of PpcInstall
	sprintf_s( szBuffer, 4096, "GET %s\n", ctx->urlRequest );
#ifdef _DEBUG
	{	wchar_t* ws = MakeWideString( szBuffer );
		TRACE( _T("Sending: %s\n"), ws );
		delete [] ws;
	}
#endif

		// send the GET request
	nRet = send( aSocket, szBuffer, (int)strlen(szBuffer), 0 );
#ifdef _DEBUG
	TRACE( _T("DownloadURL: send() --> %d\n"), nRet );
#endif

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// check result of send()
	if ( nRet == SOCKET_ERROR )
	{		// nRet = sock_errno();
#ifdef _DEBUG
		TRACE( _T("DownloadURL(-7)\n") );
#endif
		wchar_t msg[ 80 ];
		swprintf_s( msg, 80, _T(" %s: SEND error %d"), ctx->progName, nRet );
		ctx->c_StatusResult->SetWindowText( msg );
		retVal = -7;
		goto	CloseSocket;
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// open a temporary file and save the downloaded page to
	errno_t err = _wfopen_s( &fp, ctx->fileName, _T("wb") );
	if ( err != 0  ||  fp == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("DownloadURL(-8): Can't open '%s' for writing\n"), ctx->fileName );
#endif
		wchar_t msg[ 300 ];
		swprintf( msg, 300, _T(" %s: Can't open(w) %s"), ctx->progName, ctx->fileName );
		ctx->c_StatusResult->SetWindowText( msg );
		retVal = -8;
		goto	CloseSocket;
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// switch to recvWaitTime as the timeout value
	timeout.tv_sec = recvWaitTime / 1000;						// milliseconds to seconds
	timeout.tv_usec = recvWaitTime * 1000 - timeout.tv_sec * 1000000;
		// download the page
	int		bytesRead;
	bytesRead = 0;
	while( 1 )
	{		// Wait to receive, nRet = NumberOfBytesReceived
			// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
			// wait on the ability to read from the socket
			// first arg is nfds - only used in Berkeley sockets
			// second arg is readfds - a pointer to a set of sockets to be checked for readabiity
			// third arg is writefds - a pointer to a set of sockets to be checked for wrtiabiity
			// fourth arg is exceptfds - a pointer to a set of sockets to be checked for errors
			// fifth arg - timeval structure specifying max timeout for whatever
		nRet = select( 0, &fds, NULL, NULL, &timeout );
		if ( nRet == 0 )
		{		// time limit expired
			wchar_t msg[ 40 ];
			swprintf_s( msg, 40, _T(" %s: RECV Timeout/Complete"), ctx->progName );
			ctx->c_StatusResult->SetWindowText( msg );
			retVal = -9;
			goto	CloseFile;
		}
		nRet = recv( aSocket, szBuffer, sizeof(szBuffer), 0 );
#ifdef _DEBUG
//		TRACE( _T("DownloadURL: recv() --> %d\n"), nRet );
#endif
		if ( nRet == SOCKET_ERROR )
		{
#ifdef _DEBUG
		    TRACE( _T("DownloadURL: recv() --> SOCKET_ERROR = %d\n"), nRet );
#endif
			wchar_t msg[ 40 ];
			swprintf_s( msg, 40, _T(" %s: RECV Socket Error"), ctx->progName );
			ctx->c_StatusResult->SetWindowText( msg );
			break;
		}
			// Did the server close the connection?
		if ( nRet == 0 )
		{
#ifdef _DEBUG
			TRACE( _T("DownloadURL: %s connection closed\n"), ctx->progName );
			TRACE( _T("   %d of %d bytes downloaded --> %s\n"),
						bytesRead, ctx->expectedFileSize, ctx->fileName );
#endif
			break;
		}
		bytesRead += (int)nRet;
			// update the GUI status line
		wchar_t msg[ 56 ];
		swprintf_s( msg, 56, _T(" %s: %d bytes downloaded"), ctx->progName, bytesRead );
		ctx->c_StatusResult->SetWindowText( msg );

			// Write the buffer to fp
		fwrite( szBuffer, nRet, 1, fp );
	}
	if ( ctx->expectedFileSize > 0 )
	{	wchar_t msg[ 80 ];
		swprintf_s( msg, 80, _T(" %s: expected %d bytes; received %d"),
						ctx->progName, ctx->expectedFileSize, bytesRead );
		ctx->c_StatusResult->SetWindowText( msg );
		if ( bytesRead != ctx->expectedFileSize )
			retVal = -10;				// fall into CloseFile
	}
//	if ( retVal >= 0 )		// download probably timed out
//		ctx->p_PID->c_FetchButton.EnableWindow( TRUE );
CloseFile:
	if ( fp )
		fclose( fp );
CloseSocket:
	if ( aSocket )
		closesocket( aSocket );
Cleanup:
	WSACleanup();
	theApp.DoWaitCursor(-1);

	return	retVal;
}			// DownloadURL()
//-------------------------------------------------------------------------------------------------
/*
DWORD	WINAPI	DownloadURL( void* threadCtx )
{
	CYieldCurveFitterDlg* p_PID = (CYieldCurveFitterDlg*)threadCtx;
//	p_PID->c_Fetch.EnableWindow( FALSE );
	p_PID->c_Fetch.SetWindowText( _T("Cancel") );
	p_PID->wininetIsActive = true;

	theApp.DoWaitCursor(1);
		// enable the following code to download html files for parsing...

// ASSERT_VALID( p_PID );
		// inspired by the diagram on p.194 of Nick Grattan and Marshall Brain's
		//									   Windows CE 3.0 Applications Programming
		// which tracks with code on p. 198-199 as well as being the skeleton of
		// Aaron Skonnard's approach in his
		//		Essential Wininet: Developing Applications Using the Windows Internet API
		//						   with RAS, ISAPI, ASP, & COM
	if ( ! p_PID->hInetSession )
	{
#ifdef _DEBUG
		TRACE( _T("DownloadURL(-1): hInetSession not initialized\n") );
#endif
//		p_PID->c_Fetch.EnableWindow( TRUE );
		p_PID->c_Fetch.SetWindowText( _T("Fetch") );
		theApp.DoWaitCursor(-1);
		p_PID->wininetIsActive = false;
		return	-1;
	}

		// open an HTTP session on the url
		// 1st NULL for _T("GET"), 2nd NULL for HTTP version 1.1,
		// 3rd NULL for Referrer, 4th NULL indicates AcceptTypes are limited to text,
		// 5th NULL is for a callback context)
	p_PID->c_StatusResult.SetWindowText( _T("Opening request...") );
	HINTERNET	hInetUrl = HttpOpenRequest( p_PID->hInetSession, NULL,
											p_PID->urlRequest, NULL, NULL, NULL,
			// Nick Grattan uses 0 for the flags
		INTERNET_FLAG_RELOAD	  |  INTERNET_FLAG_NO_UI	  |
		INTERNET_FLAG_NO_COOKIES  |  INTERNET_FLAG_NEED_FILE,	 NULL );
	if ( hInetUrl == NULL )
	{
#ifdef _DEBUG
		unsigned short	msg[64];
		int	err = GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, NULL, msg, 63, NULL );
		TRACE( _T("DownloadURL(-2): can't open a request on %s, error = %d --> %s\n"),
			p_PID->urlRequest, err, msg );
#endif
		MessageBeep( MB_ICONEXCLAMATION );		// 0xFFFFFFFF is the system default
		p_PID->c_StatusResult.SetWindowText( _T("OpenRequest failed!") );
//		p_PID->c_Fetch.EnableWindow( TRUE );
		p_PID->c_Fetch.SetWindowText( _T("Fetch") );
		theApp.DoWaitCursor(-1);
		p_PID->wininetIsActive = false;
		return	-2;
	}

		// send the request.  1st NULL is for headers, 2nd NULL is for additional POST/PUT info
		// we'd really like to be able to abort HttpSendRequest(), but it would seem the
		// only option is TerminateThread, which would likely result in dangling references
	p_PID->c_StatusResult.SetWindowText( _T("Sending request...") );
	BOOL   res = HttpSendRequest( hInetUrl, NULL, 0, NULL, 0 );
	if ( ! res )
	{
#ifdef _DEBUG
		unsigned short	msg[64];
		int	err = GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, NULL, msg, 63, NULL );
		TRACE( _T("DownloadURL(-3): can't send the request on %s, error = %d --> %s\n"),
			p_PID->urlRequest, err, msg );
#endif
		MessageBeep( MB_ICONEXCLAMATION );		// 0xFFFFFFFF is the system default
		p_PID->c_StatusResult.SetWindowText( _T("SendRequest failed!") );
//		p_PID->c_Fetch.EnableWindow( TRUE );
		p_PID->c_Fetch.SetWindowText( _T("Fetch") );
		theApp.DoWaitCursor(-1);
		p_PID->wininetIsActive = false;
		return	-3;
	}

		// keep the downloaded file in the DB directory
	FILE*	fp = fopen( p_PID->lpFileName, "w");
	if ( !fp )
	{
#ifdef _DEBUG
		TRACE( _T("DownloadURL(-4): Can't open '%s' for writing\n"), p_PID->lpFileName );
#endif
		InternetCloseHandle( hInetUrl );

		wchar_t		wbuf[160];
		swprintf( wbuf, _T("Can't open('%s','w')"), p_PID->lpFileName );
		p_PID->c_StatusResult.SetWindowText( wbuf );

//		p_PID->c_Fetch.EnableWindow( TRUE );
		p_PID->c_Fetch.SetWindowText( _T("Fetch") );
		theApp.DoWaitCursor(-1);
		p_PID->wininetIsActive = false;
		return	-4;
	}

		// iteratively write the receive buffer to a temporary file
	p_PID->c_StatusResult.SetWindowText( _T("Attempting download...") );
	unsigned long	totalBytesRead = 0;			// totalBytesRead = 0;
//	unsigned long	bytesRead;
	char			rcvBuf[1024];

	INTERNET_BUFFERSA	IbufStruct;
	IbufStruct.dwStructSize = sizeof( IbufStruct );
	IbufStruct.dwBufferLength = sizeof( rcvBuf );
	IbufStruct.lpvBuffer = rcvBuf;
	IbufStruct.Next = NULL;
	IbufStruct.lpcszHeader = NULL;
//	IbufStruct.;

	BOOL	done = FALSE;
	bool	doneOnce = false;
	while ( ! done )
	{
//		done = InternetReadFile( hInetUrl, rcvBuf, sizeof(rcvBuf), &bytesRead );
		done = InternetReadFileExA( hInetUrl, &IbufStruct, IRF_NO_WAIT, NULL );
		if ( ! doneOnce )
		{	doneOnce = true;
			p_PID->c_StatusResult.SetWindowText( _T("Downloading...") );
		}
			// stuff the received bytes into the temporary file
		fwrite( rcvBuf, 1, IbufStruct.dwBufferLength, fp );
		totalBytesRead += IbufStruct.dwBufferLength;

		done  =  done  &&  ( IbufStruct.dwBufferLength == 0 );
	}
		// cleanup wininet
	InternetCloseHandle( hInetUrl );

#ifdef _DEBUG
	TRACE( _T("DownloadURL: totalBytesRead=%d\n"), totalBytesRead );
#endif

		// flush the file and rewind it so it's ready for parsing
	fclose( fp );							// the download/writing phase is hereby concluded
	int	itemsRead = 0;
	if ( totalBytesRead > 0 )
	{	fp = fopen( p_PID->lpFileName, "r" );
		if ( ! fp )
		{
#ifdef _DEBUG
		TRACE( _T("DownloadURL(-5): Can't open '%s' for reading\n"), p_PID->lpFileName );
#endif
			wchar_t		wbuf[160];
			swprintf( wbuf, _T("Can't open('%s','r')"), p_PID->lpFileName );
			p_PID->c_StatusResult.SetWindowText( wbuf );
			p_PID->c_Fetch.SetWindowText( _T("Fetch") );
//			p_PID->c_Fetch.EnableWindow( TRUE );
			theApp.DoWaitCursor(-1);
			p_PID->wininetIsActive = false;
			return	-5;
		}

		ServerRates	SR;
			// 01-Mar-2001
		int	yr, mo, da;		// can't read directly into a (byte) 
		itemsRead = fscanf( fp, "%4d-%2d-%2d %f %f %f %f %f %f %f",
							&yr, &mo, &da,
							&SR.ff[0], &SR.ff[1], &SR.ff[2],
							&SR.ff[3], &SR.ff[4], &SR.ff[5], &SR.ff[6] );
		fclose( fp );
		if ( itemsRead < 10 )
		{
#ifdef _DEBUG
			TRACE( _T("DownloadURL(-6): found %d items in %s\n"),
					 itemsRead, p_PID->lpFileName );
#endif
			if ( itemsRead == 3  &&  yr == 0  &&  mo == 0  &&  da == 0 )
				p_PID->c_StatusResult.SetWindowText( _T("Authentication failed.") );
			else
				p_PID->c_StatusResult.SetWindowText( _T("Server is down.") );

			p_PID->c_Fetch.SetWindowText( _T("Fetch") );
//			p_PID->c_Fetch.EnableWindow( TRUE );
			theApp.DoWaitCursor(-1);
			p_PID->wininetIsActive = false;
			return	-6;
		}
		SR.da = (byte)da;
		SR.mo = (byte)mo;
		SR.yr = (unsigned short)yr;

			// save the ServerRates struct to the registry
		bool b1 = SetServerRates( SR );
#ifdef _DEBUG
		if ( ! b1 )
			TRACE( _T("DownloadURL: SetServerRates() failed\n") );
#endif

			// update the rateDate on the dialog
		wchar_t	dateStr[ 16 ];
		swprintf( dateStr, _T("%02d%3s%04d"), SR.da, months[SR.mo-1], SR.yr );
		p_PID->c_RateDate.SetWindowText( dateStr );

			// update the rates on the dialog
		p_PID->RestoreServerRates( SR );
		p_PID->UpdateRates();
	}

		// for now, we'll leave the downloaded file lying around
//	p_PID->c_Fetch.EnableWindow( TRUE );
	p_PID->c_Fetch.SetWindowText( _T("Fetch") );
	p_PID->c_StatusResult.SetWindowText( _T("") );
	theApp.DoWaitCursor(-1);
	p_PID->wininetIsActive = false;
	return	itemsRead;
}			// DownloadUrl()
	// InternetOpen/InternetConnect/InternetOpenRequest/InternetSendRequest/InternetReadFile version
*/
//-------------------------------------------------------------------------------------------------
