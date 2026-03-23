#include "StdAfx.h"
#include "StockManagerApp.h"
#include "HttpGetFile.h"
//#include "OptionChainDialog.h"
#include "RegistryManager.h"
#include "hwnds.h"

extern	CStockManagerApp	theApp;
extern	CString	months[12];
//------------------------------------------------------------------------------------
DWORD	WINAPI	DownloadURL( void* threadCtx )
{	FILE*	fp;												// closed at CloseFile
	SOCKET	aSocket;										// closed at CloseSocket
//	COptionChainDialog* p_OCR = (COptionChainDialog*)threadCtx;
	int		retVal = 0;
	unsigned long	recvWaitTime = 1;						// 1 millisecond default causes failure
	unsigned long	sendWaitTime = 1;						// 1 millisecond default causes failure
	bool success;

		// some handy window handles
	hwnds*	zz = (hwnds*)threadCtx;
	HWND	c_StatusResult = zz->statusResult;
	theApp.DoWaitCursor( 1 );								// reset with -1 at Cleanup

		// this version was originally based on: <http://www.sockaddr.com/ExampleSourceCode.html>
		// see also:  <http://www.codeproject.com/internet/winsockintro02.asp>
		// WSA init phase
	WORD wVersionRequested = MAKEWORD( 1, 1 );
	WSADATA wsaData;

		// Initialize WinSock.dll
	int	nRet = WSAStartup( wVersionRequested, &wsaData );
	if ( nRet )
	{	CString	msg = _T("");
		switch ( nRet )
		{	case WSASYSNOTREADY:
				msg = _T("WSA SYS NOT READY");
				break;
			case WSAVERNOTSUPPORTED:
				msg = _T("WSA VER 1.1 NOT SUPPORTED");
				break;
			case WSAEINVAL:
				msg = _T("WSA E INVAL");
				break;
			default:
			{	wchar_t  buf[40];
				swprintf( buf, _T("WSA Startup error %d"), nRet ); 
				msg = buf;
			}
		}
			// wait for the message text to be updated
		SendMessage( c_StatusResult, WM_SETTEXT, 0, (LPARAM)msg.GetBuffer( msg.GetLength() ));
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
	addr = inet_addr( theApp.lpServerName );
	if ( addr != INADDR_NONE )
	{		// ServerName was a dotted IPV4 address
			// finish setting up saServer (for use in connect)
		IN_ADDR	inadr;
		inadr.S_un.S_addr = addr;
		saServer.sin_addr = inadr;
	}
	else	// ServerName was not an IPV4 address string
	{		// maybe it was an FQDN like OptimalPortfolio.net
		lpHostEntry = gethostbyname( theApp.lpServerName );
		if ( lpHostEntry != NULL )
		{		// finish setting up saServer (for use in connect)
			saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
		}
		else
		{	CString	msg( _T("") );
			int	err = WSAGetLastError();
			switch ( err )
			{
				case WSATRY_AGAIN:
					msg = _T("WSA TRY AGAIN");
					break;
				case WSANOTINITIALISED:
					msg = _T("WSA NOT INITIALIZED");	// WSAStartup failed
					break;
				case WSAENETDOWN:
					msg = _T("WSA E NET DOWN");			// Network subsystem has failed
					break;
				case WSAHOST_NOT_FOUND:
					msg = _T("WSA HOST NOT FOUND");		// Authoritative answer host not found
					break;
				case WSANO_RECOVERY:
					msg = _T("WSA NO RECOVERY");		// A nonrecoverable error occurred
					break;
				case WSANO_DATA:
					msg = _T("WSA NO DATA");			// Valid name, no data record of requested type
					break;
				case WSAEINPROGRESS:
					msg = _T("WSA E IN PROGRESS");		// A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function
					break;
				case WSAEAFNOSUPPORT:
					msg = _T("WSA E AF NO SUPPORT");	// The type specified is not supported by the Windows Sockets implementation
					break;
				case WSAEFAULT:
					msg = _T("WSA E FAULT");			// The addr parameter is not a valid part of the user address space, or the len parameter is too small
					break;
				case WSAEINTR:
					msg = _T("WSA E INTR");				// A blocking Windows Socket 1.1 call was cancelled through WSACancelBlockingCall
					break;
				default:
				{	wchar_t  buf[40];
					swprintf( buf, _T("Host error %d"), err ); 
					msg = buf;
				}
			}
			SendMessage( c_StatusResult, WM_SETTEXT, 0, (LPARAM)msg.GetBuffer( msg.GetLength() ));
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
		// get the socket send/recv wait time before starting any communications
	success = GetSocketSendWait( &sendWaitTime );
#ifdef _DEBUG
	if ( success )
		TRACE( _T("DownloadURL: SocketSendWait = %d\n"), sendWaitTime );
	else
		TRACE( _T("DownloadURL: Couldn't get SocketSendWait from the registry.\n") );
#endif

	success = GetSocketRecvWait( &recvWaitTime );
#ifdef _DEBUG
	if ( success )
		TRACE( _T("DownloadURL: SocketRecvWait = %d\n"), recvWaitTime );
	else
		TRACE( _T("DownloadURL: Couldn't get SocketRecvWait from the registry.\n") );
#endif

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// See if we connect() with the asynchronous socket
	nRet = connect( aSocket, (LPSOCKADDR)&saServer, sizeof(SOCKADDR_IN) );
	if ( nRet != 0 )
	{	CString	msg( _T("") );
		nRet = WSAGetLastError();
		switch ( nRet )
		{	case WSAEWOULDBLOCK:
				msg = _T("WSA E WOULD BLOCK");
				break;
			case WSAECONNREFUSED:
				msg = _T("WSA E CONN REFUSED");
				break;
			case WSAENETUNREACH:
				msg =  _T("WSA E NET UNREACH");
				break;
			case WSAETIMEDOUT:
				msg = _T("WSA E TIMED OUT");
				break;
			case WSAEISCONN:
				msg = _T("WSA E IS CONN");
				break;
			case WSAEINVAL:
				msg = _T("WSA E INVAL");
				break;
			case WSAEALREADY:
				msg = _T("WSA E ALREADY");
				break;
			default:
			{	wchar_t  buf[40];
				swprintf( buf, _T("Connect error %d"), nRet ); 
				msg = buf;
			}
		}
		SendMessage( c_StatusResult, WM_SETTEXT, 0, (LPARAM)msg.GetBuffer( msg.GetLength() ));
#ifdef _DEBUG
		TRACE( _T("DownloadURL: connect() -> %s\n"), msg );
#endif
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// set up to run select() with aSocket
	fd_set fds;
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
		SendMessage( c_StatusResult, WM_SETTEXT, 0, (LPARAM)_T("SEND Socket Timeout") );
#ifdef _DEBUG
		TRACE( _T("DownloadURL(-6): %s\n"), _T("SEND Socket Timeout") );
#endif
		retVal = -6;
		goto	CloseSocket;
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// Format a GET request
	char szBuffer[ 512 ];
	sprintf( szBuffer, "GET %s\n", zz->urlRequest );
#ifdef _DEBUG
	{	CString cs = szBuffer;
		TRACE( _T("Sending: %s\n"), cs );
	}
#endif

		// send the GET request
	nRet = send( aSocket, szBuffer, strlen(szBuffer), 0 );
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
		wchar_t	wbuf[ 120 ];
		swprintf( wbuf, _T("SEND error %d"), nRet );
		SendMessage( c_StatusResult, WM_SETTEXT, 0, (LPARAM)wbuf );
		retVal = -7;
		goto	CloseSocket;
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// open a temporary file and save the downloaded page to
	fp = _wfopen( zz->lpFileName, _T("wb") );
	if ( !fp )
	{
#ifdef _DEBUG
		CString	msg = zz->lpFileName;
		TRACE( _T("DownloadURL(-8): Can't open '%s' for writing\n"), msg );
#endif
		wchar_t	wbuf[120];
		swprintf( wbuf, _T("Can't open(w) %s"), zz->lpFileName ); 
		SendMessage( c_StatusResult, WM_SETTEXT, 0, (LPARAM)wbuf );

		retVal = -8;
		goto	CloseSocket;
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// switch to recvWaitTime as the timeout value
	timeout.tv_sec = recvWaitTime / 1000;				// milliseconds to seconds
	timeout.tv_usec = recvWaitTime * 1000 - timeout.tv_sec * 1000000;
		// download the page
	int		bytesRead;
	bytesRead = 0;
	SendMessage( c_StatusResult, WM_SETTEXT, 0, (LPARAM)_T("Waiting/Receiving Data...") );
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
			SendMessage( c_StatusResult, WM_SETTEXT, 0, (LPARAM)_T("RECV Timeout/Complete") );		// this may not be an error
			retVal = -9;
			goto	CloseFile;
		}
		nRet = recv( aSocket, szBuffer, sizeof(szBuffer), 0 );
#ifdef _DEBUG
		TRACE( _T("DownloadURL: recv() --> %d\n"), nRet );
#endif
		if ( nRet == SOCKET_ERROR )
		{
#ifdef _DEBUG
		    TRACE( _T("DownloadURL: recv() --> SOCKET_ERROR = %d\n"), nRet );
#endif
			SendMessage( c_StatusResult, WM_SETTEXT, 0, (LPARAM)_T("RECV Socket Error") );
			break;
		}
			// Did the server close the connection?
		if ( nRet == 0 )
		{
#ifdef _DEBUG
		    TRACE( _T("DownloadURL: Connection closed\n") );
#endif
			SendMessage( c_StatusResult, WM_SETTEXT, 0, (LPARAM)_T("Connection Closed") );
			break;
		}
		bytesRead += nRet;

			// Write to fp
		fwrite( szBuffer, nRet, 1, fp );
	}

CloseFile:
	if ( fp )
		fclose( fp );
CloseSocket:
	if ( aSocket )
		closesocket( aSocket );
Cleanup:
	WSACleanup();
	theApp.DoWaitCursor( -1 );						// stop the wait cursor

#ifdef _DEBUG
	TRACE( _T("DownloadURL: bytesRead=%d\n"), bytesRead );
#endif
	if ( retVal == 0 )
		SendMessage( c_StatusResult, WM_SETTEXT, 0, (LPARAM)_T("") );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// the post download task
	if ( bytesRead > 0 )
	{	if ( SendMessage( zz->dialog, zz->PostDownloadWinMessage, 0, 0 ) != 0 )			// that's bad
		{
#ifdef _DEBUG
			TRACE( _T("DownloadURL: %s --> %d\n"), zz->PostDownloadErrorText, nRet );
#endif
			retVal = -10;
		}			
	}
	delete [] zz->lpFileName;
	delete [] zz->urlRequest;
	delete	zz;							// release the hwnds_t struct
	return	retVal;
}			// DownloadURL()
//-------------------------------------------------------------------------------------------------
