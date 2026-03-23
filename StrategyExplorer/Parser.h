// Parser.h

char*		GetThinString( CString cs );
bool		ReadPast( CFile& fp, char* terminator );
wchar_t*	GetStratBuf( CFile& fp, char* term );				// a logical 'line' of input
bool		CrackStratBuf( wchar_t* stratBuf, CString& cs, char*& ansiSuffix );
char*		GetToken( char*& lp, char* pre, char* post );
char*		Uni2Ansi( wchar_t* unic, int nBytes );
int			ScanFloat( CString cs, float& ff );
char		StopAtAny( char*& lp, const char* set );
CString		CStringFromFloat( float ff );
char		EatWhatever( char*& lp, const char* whatever );

#define 	EatWhite( arg )			EatWhatever( (arg), " \t\n\r" )

