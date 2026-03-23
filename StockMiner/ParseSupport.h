// ParseSupport.h

#include <stdio.h>

bool	ReadPast( FILE* fp, const char* terminator );
char*	GetXMLPayload( FILE* fp, char* tagName );
bool	GetToken( FILE* fp, const char* terminator, char*& token );
bool	isMember( char example, const char* set );
char*	gettoken( char*& haystack, const char* delim );

/*	adopted a more powerful tokenizer above
char*	GetToken( char* src, int& start, char term = ' ' );
*/
