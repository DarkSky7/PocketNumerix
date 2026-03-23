// mystring.h

#ifndef __MyString_H__
#define __MyString_H__
 
#include <string>
using namespace std;

class mystring : public string
{
public:
		// creates a new mystring
				mystring( void );
				mystring( string& str );
				mystring( string* str );
				mystring( const char* str );
	char*		getTokenWtermSet( char* termChars, int startPos );			// set of single character token terminators
	char*		getTokenWtermStr( char* termStr, int startPos = 0 );		// multi-character token terminator
	mystring*	left( size_t cnt );						// creates a new mystring instance on heap
	mystring*	mid( size_t start, size_t cnt = 0 );	// creates a new mystring instance on heap
	mystring*	right( size_t cnt );					// creates a new mystring instance on heap

		// modifies this
	mystring&	operator=( mystring* str )		{	return	operator=(*str);	}
	mystring&	operator=( mystring& str );
	mystring&	operator=( const char* str );

	mystring&	operator+( mystring* str )		{	return	operator+(*str);	}
	mystring&	operator+( mystring& str );
	mystring&	operator+( const char* str );
	mystring&	operator+( char ch );

	mystring&	operator+=( mystring* str )		{	return	operator+=(*str);	}
	mystring&	operator+=( mystring& str );
	mystring&	operator+=( const char* str );
	mystring&	operator+=( char ch );

	bool		operator==( mystring* str )		{	return	compare(str->c_str()) == 0;	}
	bool		operator==( mystring& str )		{	return	compare(str.c_str()) == 0;	}
	bool		operator==( const char* str )	{	return	compare(str) == 0;			}

	bool		operator!=( mystring* str )		{	return	compare(str->c_str()) != 0;	}
	bool		operator!=( mystring& str )		{	return	compare(str.c_str()) != 0;	}
	bool		operator!=( const char* str )	{	return	compare(str) != 0;			}
};
#endif