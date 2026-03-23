// utils.cpp
#include "StdAfx.h"
#include <ctype.h>				// tolower()
#include "mystring.h"			// strlen()
#include <time.h>

int		GetMonIdx( const char* mon )
{
	char*	months[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
						   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	int	monIdx = 0;
	while ( monIdx < 12  &&  strcmp(mon,months[monIdx]) != 0 )
		monIdx++;
	if ( monIdx > 11 )
	{	fprintf( stderr, "ThirdFriday: no months match '%s'\n", mon ); 
		monIdx = -1;
	}
	return	monIdx;
}			// GetMonIdx()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int		ThirdFriday( const int monIdx, int yr )
{		// compute the day of the month for the third Friday
		// yr is a two digit year based at 2000
		// monIdx is a 0-based month
		// re: http://www.cplusplus.com/reference/clibrary/ctime/mktime/
	time_t rawtime;
	time( &rawtime );
	struct tm* timeinfo = localtime( &rawtime );
	timeinfo->tm_year = 100 + yr;							// cast two digit year in 21st century (20xx) against base year 1900
	timeinfo->tm_mon = monIdx;								// 0-based month index
	timeinfo->tm_mday = 20;
	mktime( timeinfo );										// sets tm_wday
		// move mid-month date to 3rd Friday using tm_wday
	int	deltaFriday = timeinfo->tm_mday - timeinfo->tm_wday + 5;
	int thirdFriday = 14 + deltaFriday % 7;
	if ( thirdFriday < 15 )
		thirdFriday += 7;
	return	thirdFriday;							// the 1-based tm_mday of the 3rd Friday
}			// ThirdFriday()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int		ParseCBOEdateTime( const char* src, char* dest, int maxSize )
{		// src is expected to look something like the following quoted string:
		// "Apr 23 2010 @ 13:37 ET,Bid,23.83,Ask,23.84,Size,214x259,Vol,36256404,"
		// result -> "2010-Apr-23 13:37", without a time zone
		// (would have to declare dateTime as a TIMESTAMP WITH TIME ZONE in the <stksym>_option_prices table)
	if ( strlen(src) < 22 )
	{	fprintf( stderr, "ParseCBOEdateTime: insufficient input: '%s'\n", src );
		return	-1;
	}
	char		month[4], timeZone[4];
	struct tm	stm;
	stm.tm_sec = 30;
	stm.tm_isdst = -1;										// Daylight Savings status to be computed
	sscanf( src, "%3s %d %d @ %d:%d %3s",
		&month, &(stm.tm_mday), &(stm.tm_year), &(stm.tm_hour), &(stm.tm_min), &timeZone );
	stm.tm_mon = GetMonIdx( month );						// both zero-based
	stm.tm_year -= 1900;
	strftime( dest, maxSize, "%Y-%b-%d %H:%M", &stm );		// e.g. "2010-Apr-23 13:37"
	return	0;
}			// ParseCBOEdateTime()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
char*	GetThinString( const wchar_t* wstr )
{
	if ( ! wstr )
		return	NULL;
	size_t	ss = 0, slen = wcslen( wstr );
	char*	res = new char[ slen + 1 ];
	while ( ss < slen )
		*(res + ss++) = (char)wstr[ss];
	*(res + slen) = '\0';
	return	res;
}			// GetThinString()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
wchar_t*	GetWideString( const char* str )
{
	if ( ! str )
		return	NULL;
	size_t		ss = 0, slen = strlen( str );
	wchar_t*	res = new wchar_t[ slen + 1 ];
	while ( ss < slen )
		*(res + ss++) = (wchar_t)str[ss];
	*(res + slen) = _T('\0');
	return	res;
}			// GetWideString()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool	mygetline( CFile& fpin, char*& buf, int& blen )
{		// get a fresh line of inpu, return value indicates valid output
	char	cc;
	int		nRead;
		// read newlines here...
	while ( (nRead = fpin.Read(&cc, 1)) > 0  &&  (cc == 0x0A  ||  cc == 0x0D) )
		;
	ULONGLONG start = fpin.GetPosition();
	if ( cc != 0x0A  &&  cc != 0x0D )
		start--;								// back up to capture the opening non-newline character

		// read all but newlines here...
	while ( (nRead = fpin.Read(&cc, 1)) > 0  &&  cc != 0x0A  &&  cc != 0x0D )
		;
	ULONGLONG stop = fpin.GetPosition();
	if ( cc == 0x0A  ||  cc == 0x0D )
		stop--;									// back up to exclude the terminating newline

		// should be able to use ungetc( cc, fpin ) to put back whatever was not a newline, but it's not working right
	blen = stop - start + 1;
	if ( blen < 2 )
		return	false;
	if ( buf )
		delete [] buf;
	buf = new char[ blen + 1 ];
	if ( buf )
	{	fpin.Seek( start, CFile::begin );
			// unfortunately in VS8 fread() is locked in binary mode, so we can't use
			// nRead = fread( line, 1, linelen, fpin );
		nRead = fpin.Read( buf, blen );
		*(buf+blen) = '\0';
	}
#ifdef DEBUG
	fprintf( stderr, "\nmygetline: %s\n", buf );
#endif
	return	buf  ?  true  :  false;
}			// mygetline()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool	mygetline( FILE* fpin, char*& buf, int& blen )
{		// get a fresh line of input
		// return value indicates valid output
	int	cc;
	size_t start = ftell(fpin);
		// scan forward from pos to the first '\n'
	while ( (cc = fgetc(fpin)) >= 0  &&  cc != 0x0A  &&  cc != 0x0D )		// read all but newlines here
		;
	size_t stop = ftell(fpin);
	if ( cc > 0 )
		stop--;																// exclude the newline
	while ( (cc = fgetc(fpin)) >= 0  &&  (cc == 0x0A  ||  cc == 0x0D) )		// eat newlines here
		;
	size_t bypass = ftell( fpin );
	if ( cc > 0)
		bypass--;															// exclude the non-newline character next time
	blen = (int)(stop - start);
	if ( blen <= 0 )
		return	false;														// our feof() indicator

		// should be able to use ungetc( cc, fpin ) to put back whatever was not a newline, but it's not working right
	buf = new char[ blen + 1 ];
	if ( buf )
	{	fseek( fpin, (long)start, SEEK_SET );
			// unfortunately in VS8 fread() is locked in binary mode, so we can't use
			// nRead = fread( line, 1, linelen, fpin );
		char* bb = buf;
		for ( int ii = 0; ii < blen; ii++ )
			*bb++ = (char)fgetc( fpin );
		*(buf+blen) = '\0';
	}
	fseek( fpin, (long)bypass, SEEK_SET );									// exclude the non-newline character next time
#ifdef DEBUG
	fprintf( stderr, "mygetline: %s\n", buf );
#endif
	return	true;
}			// mygetline()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool		Eof( CFile& fp )
{
	return	fp.GetLength() == fp.GetPosition();
}			// Eof()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
char*	CString2char( CString cs )
{
	long	slen = cs.GetLength();
	char*	res = new char[ slen+1 ];
	while ( ss < slen )
	{	wchar_t us = cs.GetAt( ss );
		*(res + ss++) = (char)us;
	}
	*(res + slen) = '\0';
	return	res;
}			// CString2char()
*/
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
