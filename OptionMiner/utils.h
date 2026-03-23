// utils.h
#ifndef	 __Utils_h__
#define	 __Utils_h__		1

#include <ctime>					// time_t

	char*		GetThinString( const wchar_t* wstr );
	wchar_t*	GetWideString( const char* str );
	bool		mygetline( FILE* fpin, char*& buf, int& blen );
	bool		mygetline( CFile& fpin, char*& buf, int& blen );
	int			GetMonIdx( const char* mon );				// get a 0-based month for a three character string
		// compute the day of the month for the third Friday given a 0-based month and a 2-digit year (relative to 2000)
	int			ThirdFriday( const int monIdx, int yr );
	int			ParseCBOEdateTime( const char* src, char* dest, int maxSize );
	bool		Eof( CFile& fp );

#endif