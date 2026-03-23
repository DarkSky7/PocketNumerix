// mystring.h

#include "StdAfx.h"
#include "mystring.h"
#include "ParseSupport.h"

mystring::mystring( void )
	: string("")
{
}
// - - - - - - - - - - - - - - - - - - - - - - - - - 
mystring::mystring( string& str )
	: string(str)
{
}
// - - - - - - - - - - - - - - - - - - - - - - - - - 
mystring::mystring( string* str )
	: string(*str)
{
}
// - - - - - - - - - - - - - - - - - - - - - - - - - 
mystring::mystring( const char* str )
	: string(str)
{
}
// - - - - - - - - - - - - - - - - - - - - - - - - - 
// - - - - - - - - - - - - - - - - - - - - - - - - - 
char*	mystring::getTokenWtermSet( char* termChars, int startPos )
{		// termChars - a set of single character token terminators
		// no side effects on the underlying mystring buffer
	UINT	ii = startPos;
	const char*	buf = c_str();
	size_t	len = length();
	while ( ii < len )
	{	if ( isMember( *(buf+ii), termChars ) )
			break;
		ii++;
	}
	char* ret = NULL;
	if ( ii > 0 )
	{	ret = new char[ ii+1 ];
		if ( ret )
		{	strncpy( ret, buf, ii );
				// just in case strncpy() didn't terminate the target
			*(ret+ii) = '\0';
		}
	}
	return	ret;
}			// gettokenWtermSet()
// - - - - - - - - - - - - - - - - - - - - - - - - - 
char*	mystring::getTokenWtermStr( char* termStr, int startPos /* =0 */ )
{		// termStr - a multi-character token terminator
		// no side effects on the underlying mystring buffer
	const char*	buf = c_str();						// mystring's internal buffer
	size_t	bufLen = length();
	size_t	termLen = strlen( termStr );
	UINT	bu = 0;								// index within buf
	UINT	te = 0;								// index within termSt
	bool	match = false;
	while ( bu < bufLen  &&  te < termLen )
	{	char	matchChar = *(termStr + te);
		te = ( *(buf + bu++) == matchChar ) ? (te + 1) : 0;		// bu always increments, te can reset
		match = te >= termLen;
		if ( match )
			break;												// matched, so we're done
	}
	char* ret = NULL;
	if ( match  &&  bu > 0 )
	{	ret = new char[ bu+1 ];
		if ( ret )
		{	strncpy( ret, buf, bu );
				// just in case strncpy() didn't terminate the target
			*(ret+bu) = '\0';
		}
	}
	return	ret;
}			// gettokenWtermStr()
// - - - - - - - - - - - - - - - - - - - - - - - - - 
mystring*	mystring::left( size_t cnt )
{
	size_t	len = length();
	if ( cnt >= len )
		cnt = len-1;
	return	new mystring( substr(0,cnt) );				// on heap
}			// left()
// - - - - - - - - - - - - - - - - - - - - - - - - - 
mystring*	mystring::mid( size_t start, size_t cnt /* =0 */ )
{
	size_t	len = length();
	if ( start >= len )
		start = len-1;
	if ( cnt == 0 )
		cnt = len;
	if ( start+cnt >= len )
		cnt = len - start;
	return	new mystring( substr(start,cnt) );			// on heap
}			// mid()
// - - - - - - - - - - - - - - - - - - - - - - - - - 
mystring*	mystring::right( size_t cnt )
{
	size_t len = length();
	size_t beg = len - cnt;
	if ( beg < 0 )
		beg = 0;
	return	new mystring( substr( beg, len-beg ) );		// on heap
}			// right()
// - - - - - - - - - - - - - - - - - - - - - - - - - 
// - - - - - - - - - - - - - - - - - - - - - - - - - 
mystring&	mystring::operator=( mystring& str )
{
	return	(mystring&)assign( str );
}
// - - - - - - - - - - - - - - - - - - - - - - - - - 
mystring&	mystring::operator=( const char* str )
{
	return	(mystring&)assign( str );
}
// - - - - - - - - - - - - - - - - - - - - - - - - - 
// - - - - - - - - - - - - - - - - - - - - - - - - - 
mystring&	mystring::operator+( mystring& str )
{
	mystring xx(this);
	return	(mystring&)xx.append( str );
}
// - - - - - - - - - - - - - - - - - - - - - - - - - 
mystring&	mystring::operator+( const char* str )
{
	mystring xx(this);
	return	(mystring&)xx.append( str );
}
// - - - - - - - - - - - - - - - - - - - - - - - - - 
mystring&	mystring::operator+( char ch )
{
	mystring xx(this);
	return	(mystring&)xx.append( 1, ch );
}
// - - - - - - - - - - - - - - - - - - - - - - - - - 
// - - - - - - - - - - - - - - - - - - - - - - - - - 
mystring&	mystring::operator+=( mystring& str )
{
	return	(mystring&)append( str );
}
// - - - - - - - - - - - - - - - - - - - - - - - - - 
mystring&	mystring::operator+=( const char* str )
{
	return	(mystring&)append( str );
}
// - - - - - - - - - - - - - - - - - - - - - - - - - 
mystring&	mystring::operator+=( char ch )
{
	return	(mystring&)append( 1, ch );
}
// - - - - - - - - - - - - - - - - - - - - - - - - - 
