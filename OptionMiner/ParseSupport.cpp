// ParseSupport.cpp

#include "StdAfx.h"
#include "ParseSupport.h"
#include "mystring.h"
//#include <string>				// std::string
//#include "ParseSupport.h"		// GetThinString()
#include "utils.h"					// GetThinString()

bool	isMember( char example, const char* set )
{
	while ( *set )
		if ( *set++ == example )
			return	true;
	return	false;
}			// isMember( char example, char* set )
//--------------------------------------------------------------------------------------
void	rtrim( char* str, const char* outChars )
{		// eliminate trailing outChars from str
		// ex1: rtrim( "stuff ", " \t\r\n" ) --> str = "stuff"
		// ii=          012345
		// ex2: rtrim( "stuff", " \t\r\n" ) --> str = "stuff"
	size_t ii = strlen(str) - 1;
	while ( ii >= 0  &&  isMember(*(str+ii), outChars) )
		ii--;
		// ex1: ii=4; ex2: ii=4
	*(str+ii+1) = '\0';							// works whether ii has moved or not
}			// rtrim()
//--------------------------------------------------------------------------------------
void	ltrim( char* str, const char* outChars )
{	
	int	ii = 0;
	while ( *(str+ii)  &&  isMember(*(str+ii), outChars) )
		ii++;
	if ( ii > 0 )
	{	int	jj = ii;
		while ( *(str+jj) )
			*(str+jj-ii) = *(str+jj++);
		*(str+jj-ii) = '\0';					// terminate it in the new position
	}
}			// ltrim()
//--------------------------------------------------------------------------------------
int		GetFnames( mystring& args, char*& yhsp, char*& cboe )
{
	cboe = yhsp = NULL;
	size_t	cboe_i = args.find( "cboe" );
	bool have_cboe = cboe_i != std::string::npos;

	size_t	yhsp_i = args.find( "yhsp" );
	bool have_yhsp = yhsp_i != std::string::npos;
	if ( !have_cboe  &&  !have_yhsp )
		return	0;										// no files to open

	size_t cboe_sz = have_cboe  ?  (args.length() - (cboe_i + 5))  :  0;		// works when cboe arg is missing or last
	size_t	yhsp_sz = have_yhsp  ?  (args.length() - (yhsp_i + 5))  :  0;		// works when yhsp arg is missing or last
	int	retVal = 1;										// one file to open
	if ( have_cboe  &&  have_yhsp )
	{		// both args present - recalc the size of the first filename
		if ( cboe_i < yhsp_i )
				//				   1         2
				//       01234567890123456789012345
				// e.g.  progName cboe =... yhsp =...
				// cboe_i=9, yhsp=19
			cboe_sz = yhsp_i - (cboe_i + 5) - 1;		// kill the separating space too
		else	// cboe_i >= yhsp_i
				// e.g.  yhsp =... cboe =...
			yhsp_sz = cboe_i - (yhsp_i + 5) - 1;		// kill the separating space too
		retVal = 2;										// two files to open
	}

	if ( cboe_sz )
	{	cboe = new char[cboe_sz+1];
		strncpy( cboe, args.c_str()+cboe_i+5, cboe_sz );
		*(cboe+cboe_sz) = '\0';
		ltrim( cboe, " =\t" );
		rtrim( cboe );
	}
	if ( yhsp_sz )
	{	yhsp = new char[yhsp_sz+1];
		strncpy( yhsp, args.c_str()+yhsp_i+5, yhsp_sz );
		*(yhsp+yhsp_sz) = '\0';
		ltrim( yhsp, " =\t" );
		rtrim( yhsp );
	}
	return	retVal;
}			// GetFnames()
//--------------------------------------------------------------------------------------
void	CatArgs( int argc, TCHAR* argv[], mystring& args )
{		// catenate a wide char argument list into an ANSI string
	for ( short ii = 0; ii < argc; ii++ )
	{	char* thinString = GetThinString( *(argv+ii) );
		args += thinString;
		delete [] thinString;
		args += " ";
	}
}			// CatArgs()
//--------------------------------------------------------------------------------------
bool		ParseCBOEsymbolCompany( const char* str, char*& symbol, char*& company )
{		// from the first line of a CBOE option prices file
		//											  1		    2		  3
		//									0123456789012345678901234567890
		// expecting a string of the form: "INTC (INTEL CORP),23.84,-0.15,"
		//										 ^			^
		//							   openParen=5			closeParen=16
	mystring mystr = str;
	size_t	 openParen = mystr.find( '(' );
	if ( openParen < 0 )
		return	false;
	size_t	 closeParen = mystr.find( ')', openParen+1 );
	if ( closeParen < 0 )
		return	false;
		// get the symbol
	symbol= new char[openParen+1];						// has room for terminating null
	strncpy( symbol, str, openParen );					// openParen stops short of openParen
	*(symbol+openParen) = '\0';
	rtrim( symbol, " " );
		// get the company name
	int	diff = closeParen - openParen;					// e.g. diff=11
	company = new char[diff];							// with room for terminating null
	strncpy( company, str+openParen+1, diff-1 );
	*(company+diff-1) = '\0';
	return	true;
}			// ParseCBOEsymbolCompany()
//--------------------------------------------------------------------------------------
bool		ParseFnameCompanySymbol( const char* str, char*& company, char*& symbol )
{		// expecting ws = _T("drive:/path/CompanyName (symbol) ignored description.ext")
		// allocating space for and returning symbol and companyName
		// find last '/', first '(', and first ')'
	mystring mystr = str;
	size_t lastSlash = mystr.find( '/' );
	if ( lastSlash < 0 )
		lastSlash = -1;									// assume a relative path
	size_t openParen = mystr.find( '(', lastSlash+1 );
	if ( openParen < 0 )
		return	false;
	size_t closeParen = mystr.find( ')', openParen+1 );
	if ( closeParen < 0 )
		return	false;
		//                           1         2         3         4         5
		//                 0123456789012345678901234567890123456789012345678901
		// in the example "C:/path/CompanyName (symbol) ignored description.ext", we'd get
		//                        ^            ^      ^
		//              lastSlash=7            |      |
		//                          openParen=20      |
		//                                closeParen=27
		// we're still here - lets grab some substrings
		// get the company name
	size_t diff = openParen - lastSlash;				// e.g., diff=13
	company = new char[diff];							// has room for terminating null
	strncpy( company, str+lastSlash+1, diff-1 );		// diff-1 stops short of openParen
	*(company+diff-1) = '\0';
	rtrim( company, " " );
		// get the symbol
	diff = closeParen - openParen;						// e.g. diff=7
	symbol = new char[diff];							// with room for terminating null
	strncpy( symbol, str+openParen+1, diff-1 );
	*(symbol+diff-1) = '\0';
	return	true;
}			// ParseFnameCompanySymbol()
//--------------------------------------------------------------------------------------
wchar_t		tolower( wchar_t cc )
{		// a wide character version
	const	int		lowDiff = _T('a') - _T('A');				// 'A' < 'a', so lowDiff > 0
	return ( cc >= _T('A')  &&  cc <= _T('Z') )  ?  cc + lowDiff  :  cc;
}			// tolower()
//--------------------------------------------------------------------------------------
wchar_t		toupper( wchar_t cc )
{		// a wide character version
	const	int		lowDiff = _T('a') - _T('A');				// 'A' < 'a', so lowDiff > 0
	return ( cc >= _T('a')  &&  cc <= _T('z') )  ?  cc - lowDiff  :  cc;
}			// toupper()
//--------------------------------------------------------------------------------------
void	strtolower( char* ss )
{		// an ANSI version
	while ( *ss )
		*ss++ = tolower( *ss );		// compiler doesn't advance ss properly in: *ss = tolower( *ss++ );
}			// strtolower()
//--------------------------------------------------------------------------------------
void	strtolower( wchar_t* ss )
{		// a wide character version
	while ( *ss )
		*ss++ = tolower( *ss );		// compiler doesn't advance ss properly in: *ss = tolower( *ss++ );
}			// strtolower()
//--------------------------------------------------------------------------------------
void	strtoupper( char* ss )
{		// an ANSI version
	while ( *ss )
		*ss++ = toupper( *ss );		// compiler doesn't advance ss properly in: *ss = toupper( *ss++ );
}			// strtoupper()
//--------------------------------------------------------------------------------------
void	strtoupper( wchar_t* ss )
{		// a wide character version
	while ( *ss )
		*ss++ = toupper( *ss );		// compiler doesn't advance ss properly in: *ss = toupper( *ss++ );
}			// strtoupper()
//--------------------------------------------------------------------------------------
char*	CreateUpString( const char* str )
{
	char*	upStr = NULL;
	size_t	slen = strlen( str );
	if ( slen > 0 )
	{	upStr = new char[slen+1];
		if ( upStr )
		{	strcpy( upStr, str );
			strtoupper( upStr );
		}
	}
	return	upStr;
}			// CreateUpString()
//--------------------------------------------------------------------------------------
char*	CreateDownString( const char* str )
{
	char*	downStr = NULL;
	size_t	slen = strlen( str );
	if ( slen > 0 )
	{	downStr = new char[slen+1];
		if ( downStr )
		{	strcpy( downStr, str );
			strtolower( downStr );
		}
	}
	return	downStr;
}			// CreateDownString()
//--------------------------------------------------------------------------------------
bool		ReadPast( CFile& fp, const char* terminator )
{		// side effect: file position moves after the terminator
	char	buf[ 64 ];
	short	termLen = strlen( terminator );
	short	te = 0;												// index within terminator

	long	startPos = (long)fp.GetPosition();
	int	bytesRead = fp.Read( buf, sizeof(buf) );
	if ( bytesRead <= 0 )
		return	false;

	long	bu = 0;												// index within buf
	char	matchChar;
	bool	match = false;
	while ( bu < bytesRead  &&  te < termLen )
	{	matchChar = *(terminator + te);
		te = ( *(buf + bu++) == matchChar ) ? (te + 1) : 0;		// bu always increments, te can reset
		match = te >= termLen;
		if ( match )
		{		// move the file pointer to just after the
				// *(buf + bu) we compared above (bu has already moved)
			fp.Seek( startPos + bu + 1, CFile::begin );			// the +1 includes the high unicode byte (which is second) 
			break;												// matched, so we're done
		}
		if ( bu >= bytesRead )										// bu has already moved up
		{	startPos += bytesRead;									// bufLen will be obliterated next
			bytesRead = fp.Read( buf, sizeof(buf) );						// get a new buffer from the file
			if ( bytesRead <= 0 )
				return	false;
			bu = 0;												// reset bu
		}
	}
	return	match;
}			// ReadPast()
//--------------------------------------------------------------------------------------
bool		GetToken( CFile& fp, const char* terminator, char*& token )
{		// we want the stuff between the initial file position and the beginning of the terminator
		// side effects: the file position moves after the terminator
		// if no terminator is found before EOF, the rest of the file is returned
	size_t startPos = fp.GetPosition();
	bool termFound = ReadPast( fp, terminator );			// ignore the return value
		// whether or not we found the terminator...
	size_t currentPos = fp.GetPosition();
	fp.Seek( startPos, CFile::begin );
	int	termLen = (int)strlen( terminator );
	size_t	nChars = currentPos - termLen - startPos;
	if ( nChars > 0 )
	{		// caller should provide an unused char* to receive token
		if ( token )
			delete [] token;									// ensure that we don't leak tokens
		token = new char[ nChars+1 ];
		if ( nChars == fp.Read(token, 1) )
		{		// at this point, termFound is true - can terminate the token and return
			*(token + nChars) = '\0';
			if ( termFound )
				fp.Seek( termLen, CFile::current );				// pass up terminator
		}
		else													// Read() failed
		{	delete [] token;
			token = NULL;		// NOTE: token can be NULL even though termFound is true
		}
	}
	return	nChars > 0;
}			// GetToken()
//--------------------------------------------------------------------------------------
char*	gettoken( char*& haystack, const char* delim )
{		// copy token starting at haystack and ending at terminator
		// side effect:  move haystack pointer past the token and the terminator
	if ( !haystack  ||  *haystack == '\0' )
		return	NULL;
	while ( isMember( *haystack, delim ) )
		haystack++;									// ignore leading delimiters
	size_t	tloc = strcspn( haystack, delim );
	char*	token = new char[ tloc+1 ];
	strncpy( token, haystack, tloc );
	*(token+tloc) = '\0';
	haystack += tloc;								// bypass the token
	return	token;									// caller responsible for deleting token
}			// gettoken( char* haystack, char terminator )
//--------------------------------------------------------------------------------------
/* now using more powerful tokenizer above
char*	GetToken( char* src, int& start, char term )
{	int	idx = start;
		// skip initial whitespace
	while ( *(src+idx) && isspace( *(src+idx) ) )
		idx++;

	start = idx;
	while ( *(src+idx)  &&  *(src+idx) != term )
		idx++;
		// did we find it ?
	if ( *(src+idx) != term )
		return	NULL;

	int  nChars = idx - start;
	char* dest = new char[ nChars + 1 ];
	strncpy( dest, src+start, nChars );
	*(dest+nChars) = '\0';
	start += nChars;
	return	dest;
}		// GetToken()
*/
//--------------------------------------------------------------------------------------
/*
char*	ParseDateTime( const char* src )
{		// line is expected to look something like the following quoted string:
		//            1         2         3
		//   123456789 123456789 123456789 123456789   byte offset
		// "Apr 23 2010 @ 13:37 ET,Bid,23.83,Ask,23.84,Size,214x259,Vol,36256404,"
		// "2010-Apr-23 13:37 ET" is the associated return value
		//  123456789 123456789 123456789 123456789    string length
		//           1         2         3
	if ( strlen(src) < 22 )
		return	NULL;
	char* dest = new char[24];						// max length 23, time zone starting at 17
	if ( dest )
	{	strncpy( dest, src+7, 4 );					// four digit year 
		*(dest+4) = '-';
		char month[4];								// three character month
		strncpy( month, src+0, 3 );
		*(month+3) = '\0';
		int monIdx = GetMonIdx( month );			// zero-based month index
		char monIdxSt[8];							// hopefully we'll only use three low order bytes
		sprintf( monIdxSt, "%02d", monIdx+1 );		// one-based month index as a string
		strncpy( dest+5, monIdxSt, 2 );				// two digit 1-based month
		*(dest+7) = '-';
		strncpy( dest+8, src+4, 2 );				// two digit day of month
		*(dest+10) = ' ';
		strncpy( dest+11, src+14, 5 );				// a five character 24 hour time 
		*(dest+16) = ' ';
		mystring str = src;
		int comma = str.find( ',', 20 );			// expecting 22
		int zoneLen = comma - 20;
		if ( zoneLen > sizeof(dest)-1 - 17 )
			zoneLen = sizeof(dest)-1 - 17;
		strncpy( dest+17, src+20, zoneLen );		// the time zone (up to three characters long)
		*(dest+17+zoneLen) = '\0';
	}
	return	dest;
}			// ParseDateTime()
*/
//--------------------------------------------------------------------------------------
