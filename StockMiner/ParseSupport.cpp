// ParseSupport.cpp

#include "StdAfx.h"
#include "ParseSupport.h"

bool	ReadPast( FILE* fp, const char* terminator )
{		// side effect: file position moves after the terminator
	char	buf[ 64 ];
	size_t	termLen = strlen( terminator );
	unsigned short	te = 0;										// index within terminator
	long startPos = ftell( fp );								// e.g. fpos_t startPos;  fgetpos( fp, &startPos );

	size_t	bufLen = fread( buf, 1, sizeof(buf), fp );
	unsigned long	bu = 0;										// index within buf
	char	matchChar;
	bool	match = false;
	while ( bu < bufLen  &&  te < termLen )
	{	matchChar = *(terminator + te);
		te = ( *(buf + bu++) == matchChar ) ? (te + 1) : 0;		// bu always increments, te can reset
		match = te >= termLen;
		if ( match )
		{		// move the file pointer to just after the
				// *(buf + bu) we compared above (bu has already moved)
			fseek( fp, (long)(startPos + bu), SEEK_SET );
			break;												// matched, so we're done
		}
		if ( bu >= bufLen )										// bu has already moved up
		{	startPos += bufLen;									// bufLen will be obliterated next
			bufLen = fread( buf, 1, sizeof(buf), fp );			// get a new buffer from the file
			bu = 0;												// reset bu
		}
	}
	return	match;
}			// ReadPast()
//--------------------------------------------------------------------------------------
char*	GetXMLPayload( FILE* fp, char* tagName )
{		// NOTE:  we are not rewinding before searching for the specified tag, hoping to save I/O time
	size_t slen = strlen( tagName );
	char* tag = new char[ slen + 3 ];
	sprintf( tag, "<%s>", tagName );
	bool okay = ReadPast( fp, tag );
	delete [] tag;
	if ( !okay )
		return	NULL;

	long startPos = ftell( fp );								// e.g. fpos_t startPos;  fgetpos( fp, &startPos );
	tag = new char[ slen + 4 ];
	sprintf( tag, "</%s>", tagName );
	okay = ReadPast( fp, tag );
	delete [] tag;
	if ( !okay )
		return	NULL;

	long endPos = ftell( fp );									// e.g. fpos_t endPos;	fgetpos( fp, &endPos );
	endPos -= slen + 3;											// don't include the end tag in the payload
	int nChars = (int)(endPos - startPos);
	char* payload = new char[ nChars + 1 ];
	fseek( fp, startPos, SEEK_SET );
	fread( payload, 1, nChars, fp );
	*(payload+nChars) = '\0';
	fseek( fp, (long)(slen+3), SEEK_CUR );
	return	payload;
}			// GetXMLPayload()
//--------------------------------------------------------------------------------------
bool		GetToken( FILE* fp, const char* terminator, char*& token )
{		// we want the stuff between the initial file position and the beginning of the terminator
		// side effects: the file position moves after the terminator
		// if no terminator is found before EOF, the rest of the file is returned
	int	startPos = ftell( fp );
	bool termFound = ReadPast( fp, terminator );			// ignore the return value
		// whether or not we found the terminator...
	int currentPos = ftell( fp );
	fseek( fp, startPos, SEEK_SET );
	short termLen = strlen( terminator );
	unsigned long nChars = currentPos - termLen - startPos;
	if ( nChars > 0 )
	{		// caller should provide an unused char* to receive token
		if ( token )
			delete [] token;									// ensure that we don't leak tokens
		token = new char[ nChars+1 ];
		if ( nChars == fread( token, 1, nChars, fp ) )
		{		// at this point, termFound is true - can terminate the token and return
			*(token + nChars) = '\0';
			if ( termFound )
				fseek( fp, termLen, SEEK_CUR );					// pass up terminator
		}
		else													// Read() failed
		{	delete [] token;
			token = NULL;		// NOTE: token can be NULL even though termFound is true
		}
	}
	return	nChars > 0;
}			// GetToken()
//--------------------------------------------------------------------------------------
bool	isMember( char example, const char* set )
{
	while ( *set )
		if ( *set++ == example )
			return	true;
	return	false;
}			// isMember( char example, char* set )
//--------------------------------------------------------------------------------------
char*	gettoken( char*& haystack, const char* delim )
{		// copy token starting at haystack and ending at terminator
		// side effect:  move haystack pointer past the token and the terminator
	if ( !haystack  ||  *haystack == '\0' )
		return	NULL;
	while ( isMember( *haystack, delim ) )
		haystack++;								// ignore leading delimiters
	int tloc = strcspn( haystack, delim );
	char* token = new char[ tloc+1 ];
	strncpy( token, haystack, tloc );
	*(token+tloc) = '\0';
	haystack += tloc;							// bypass the token
	return	token;								// caller responsible for deleting token
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
