// Parser.cpp
#include "StdAfx.h"
#include "Parser.h"

const unsigned int MAX_LINE_LENGTH = 511;
const char EOS = '\0';
//----------------------------------------------------------------------------
char*		GetThinString( CString cs )
{	int	len = cs.GetLength();
	char*	res = new char[ len+1 ];

	for ( unsigned short ii = 0; ii < len; ii++ )
	{
		wchar_t us = cs.GetAt( ii );
		*(res+ii) = (char)us;
	}
	*(res+len) = '\0';
	return	res;
}			// GetThinString()
//----------------------------------------------------------------------------------------
int		ScanFloat( CString cs, float& ff )
{
/*
	USES_CONVERSION;			// won't compile here...
	int			len = cs.GetLength();
	wchar_t*	wbuf = cs.GetBuffer( len+1 );
	char*		buf = W2CA( wbuf );
*/
	char*	buf = GetThinString( cs );
	int		itemsScanned = sscanf( buf, "%f", &ff );
	delete [] buf;

	return	itemsScanned;
}			// ScanFloat()
//---------------------------------------------------------------------------
bool	isMember( char example, const char* set )
{
	while ( *set )
		if ( *set++ == example )
			return	true;
	return	false;
}			// isMember( char example, char* set )
//--------------------------------------------------------------------------------------
char	EatWhatever( char*& lp, const char* set )
{
	bool	match = false;
	while ( *lp != EOS  &&  isMember( *lp, set ) )
		lp++;									// ignore leading set members
	return	*lp;
}			// EatWhatever()
//----------------------------------------------------------------------------
char	StopAtAny( char*& lp, const char* set )
{
	bool	match = false;
	while ( *lp != EOS  &&  !isMember( *lp, set ) )
		lp++;									// ignore stuff that isn't a set member
	return	*lp;
}			// StopAtAny()
//----------------------------------------------------------------------------
CString		CStringFromFloat( float ff )
{	char	buf[16];
	sprintf( buf, "%g", ff );
	CString cs = buf;
	return	cs;
}			// CStringFromFloat()
//----------------------------------------------------------------------------
bool		ReadPast( CFile& fp, char* terminator )
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
char*		Uni2Ansi( wchar_t* unic, int nBytes )
{
	int		len = nBytes / 2;
	char	*ansi = new char[ len + 1];
	char	*aa = ansi;
	wchar_t	*uu = unic, *nn = unic + len;
	while ( uu < nn )
		*aa++ = (char)*uu++;				// drop Unicode's high bit
	*aa = '\0';
	return	ansi;
}			// Uni2Ansi()
//----------------------------------------------------------------------------------------
bool	CrackStratBuf( wchar_t* stratBuf, CString& cs, char*& ansiSuffix )
{		// lp is unicode, suffix is ANSI
#if _DEBUG
//	TRACE( _T("CrackStratBuf: %s\n"), stratBuf );
#endif
	cs = stratBuf;
	int len = cs.GetLength();
	TCHAR delim = _T('\'');
	int ii = cs.Find( delim );				// opening delimiter
	if ( ii < 0 )
	{	delim = _T('\"');					// try double quote
		ii = cs.Find( delim );				// opening delimiter
	}
	ii++;									// bypass the opening delimiter
	int jj = cs.Find( delim, ii );			// closing delimiter
	if ( jj < 0 )
	{	cs = "";
		return	false;
	}
	cs = cs.Mid( ii, jj-ii );
	cs.Trim();
	jj++;									// bypass the closing delimiter
	ansiSuffix = Uni2Ansi( stratBuf+jj, 2*(len-jj) );
#if _DEBUG
//	{	CString tmp = ansiSuffix;
//		TRACE( _T("CrackStratBuf: %s --> stratName=%s, ansiSuffix=%s\n"), stratBuf, cs, tmp );
//	}
#endif
	return	ansiSuffix  ?  true  :  false;
}			// CrackStratBuf()
//--------------------------------------------------------------------------------------
wchar_t*	GetStratBuf( CFile& fp, char* term )
{		// read unicode at byte level, cast to wide string upon return
		// ReadPast() read past an opening '\'' (which we're going to keep)
	long startPos = (long)fp.GetPosition();
	startPos -= 2;
	if ( startPos < 0 )
		startPos = 0;
	fp.Seek( startPos, CFile::begin );
	char*		unic = NULL;					// Unicode in an ANSI string !
	wchar_t*	wunic = NULL;
	int nBytes = 0;
	if ( ReadPast( fp, term ) )
	{	long afterPos = (long)fp.GetPosition();
		long slen = afterPos - startPos;
		unic = new char[ slen + 2 ];			// +2 for a wide terminator
		fp.Seek( startPos, CFile::begin );
		nBytes = fp.Read( unic, slen );
#if _DEBUG
//		TRACE( _T("GetStratBuf: unic=%s, slen=%d, nBytes=%d\n"), unic, slen, nBytes );
#endif
		if ( nBytes < 2  ||  nBytes % 2 > 0 )
		{	delete [] unic;
			return	NULL;
		}
		wunic = (wchar_t*)unic;
		*(wunic + nBytes/2) = _T('\0');
	}
	return	wunic;
}			// GetStratBuf()
//--------------------------------------------------------------------------------------
char*	GetToken( char*& lp, char* pre, char* post )
{		// PURPOSE: eat leading terminators, scan for next one,
		//			allocate new char*, fill & return it
		// SIDE EFFECT: *lp advances
		// Allocating tokens on heap is not as efficient as stuffing a stack
		// based token, but then we'd have to check size, take action, ...
		// eat leading terminators (spaces?)

		// set of token separators we can eat
	if ( EOS == EatWhatever( lp, pre ) )
		return	NULL;

	if ( EOS == EatWhite( lp ) )
		return	NULL;

		// found a non-term character, look for end of token
	char *beg = lp;
	StopAtAny( lp, post );
	
		// allocate a new char*, fill it, and return it
	long	cnt = (long)(lp - beg);
	char	*ret = new char[ cnt+1 ];
	strncpy( ret, beg, cnt );
	*(ret+cnt) = EOS;
	return	ret;
}			// GetToken()
//----------------------------------------------------------------------------
/*
char*		GetThinString( CString cs )
{	int	len = cs.GetLength();
	char*	res = new char[ len+1 ];

	for ( unsigned short ii = 0; ii < len; ii++ )
	{
		wchar_t us = cs.GetAt( ii );
		*(res+ii) = (char)us;
	}
	*(res+len) = '\0';
	return	res;
}			// GetThinString()
*/
//----------------------------------------------------------------------------------------
