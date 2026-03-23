// ParseSupport.h
#ifndef __ParseSupport_h__
#define	__ParseSupport_h__		1

#include <stdio.h>
#include "mystring.h"

void	CatArgs( int argc, TCHAR* argv[], mystring& args );
int		GetFnames( mystring& args, char*& yhsp, char*& cboe );

bool	ParseCBOEsymbolCompany( const char* str, char*& company, char*& symbol );
bool	ParseFnameCompanySymbol( const char* str, char*& symbol,  char*& company );

bool	GetToken( FILE* fp, const char* terminator, char*& token );
bool	ReadPast( CFile& fp, const char* terminator );

char*	gettoken( char*& haystack, const char* delim );
bool	isMember( char example, const char* set );
void	rtrim( char* str, const char* outChars = " \t\r\n" );
void	ltrim( char* str, const char* outChars = " \t\r\n" );

void	strtoupper( char* str );
void	strtoupper( wchar_t* str );

void	strtolower( char* str );
void	strtolower( wchar_t* str );

wchar_t	tolower( wchar_t cc );
wchar_t	toupper( wchar_t cc );

char*	CreateUpString( const char* str );
char*	CreateDownString( const char* str );

/*	adopted a more powerful tokenizer above
char*	GetToken( char* src, int& start, char term = ' ' );
*/
#endif
