// HttpGetFile.cpp : Defines the entry point for the console application.
//
#define _DEBUG		1

#include "../HttpGetFile.h"

int		main( int argc, char **argv )
{		// Check arguments
		// usage:  GetHTTP www.idgbooks.com /index.html > index.html
	if ( argc < 4 )
	{
		TRACE( "\nUsage: %s <ServerName> <FullPathName> <saveFileName>\n", *argv );
		return	0;
	}

#ifdef WIN32
	WORD wVersionRequested = MAKEWORD( 1, 1 );
	WSADATA wsaData;
		// Initialize WinSock.dll
	int	nRet = WSAStartup( wVersionRequested, &wsaData );
	if ( nRet )
	{
		TRACE( "\nWSAStartup(): %d\n", nRet );
		WSACleanup();
		return	-1;
	}

		// Check WinSock version
	if ( wsaData.wVersion != wVersionRequested )
	{
		TRACE( "\nWinSock version not supported\n" );
		WSACleanup();
		return	-2;
	}
		// Set "stdout" to binary mode
		// so that redirection will work
		// for .gif and .jpg files
	_setmode( _fileno(stdout), _O_BINARY );
#endif

		// Call GetHTTP() to do all the work
	FILE*	fp = fopen( saveFileName, "w" );
	HttpGetFile( *(argv+1), *(argv+2), fp );
	fclose( fp );

		// Release WinSock
#ifdef WIN32
	WSACleanup();
#endif
	return	0;
}		// main()
//------------------------------------------------------------------------


