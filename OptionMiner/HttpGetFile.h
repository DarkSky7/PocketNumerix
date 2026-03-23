#ifndef HttpGetFile_H
#define HttpGetFile_H
 
#include <stdio.h>
#include "mystring.h"

#ifdef WIN32
//	#include <io.h>
	#include <winsock.h>
		// Helper macro for displaying errors
	#define PRINTERROR(s)	\
				fprintf( stderr,"\n%: %d\n", s, WSAGetLastError() )
#else
	// <linux/socket.h> is my idea
	//#include <linux/socket.h>
	// the link below is gone...
	// http://tangentsoft.net/wskfaq/articles/bsd-communuity.html
	// the following are needed instead of <winsock.h>
	#include <sys/types.h>
//	#include <sys/socket.h>			// used to define sock_errno()
//	#include <mysql/my_global.h>	// sock_errno(), creates problems reading my_config.h, my_dbug.h
//	#include <netinet/in.h>
//	#include <arpa/inet.h>
//	#include <netdb.h>
	#include <fcntl.h>
/*
	#define	IN_ADDR		in_addr
	#define LPIN_ADDR	in_addr*
	#define LPHOSTENT	hostent*
	#define	SOCKADDR	sockaddr
	#define	LPSOCKADDR	sockaddr*
	#define SOCKADDR_IN	sockaddr_in
	#define	SOCKET		int
	#define INVALID_SOCKET	-1
	#define LPSERVENT	servent*
	#define closesocket(sock)	shutdown((sock), SHUT_RDWR)
*/
#endif

#ifndef LPCSTR
#define	LPCSTR		char*
#endif

//short	HttpGetFile( LPCSTR lpServerName, LPCSTR lpFileName, FILE* fp );
DWORD	WINAPI	DownloadURL( void* threadCtx );

/*
typedef struct hostent {
  char FAR* h_name;
  char FAR  FAR** h_aliases;
  short h_addrtype;
  short h_length;
  char FAR  FAR** h_addr_list;
} hostent;

struct sockaddr_in { 
    short sin_family;
    unsigned short sin_port;
    struct   in_addr sin_addr;
    char sin_zero[8]; 
};
*/

#endif	// #ifndef  HttpGetFile_H
