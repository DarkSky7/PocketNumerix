#include "StdAfx.h"
#include "RegistryManager.h"		// needed in BM/NH where RegistryManager is a class
#include "GetDeviceInfo.h"
#include "RegistryManager.h"
#include "crypt.h"

// GetOsVersion() is not really about serial numbers, but needs to be in every product, so...
CString		GetOsVersion( void )
{	OSVERSIONINFO		verInfo;
	SetLastError( 0 );								// coredll.lib, winbase.h
	BOOL	B1 = GetVersionEx( &verInfo );			// req:  rapi.h, rapi.lib
#ifdef _DEBUG
	if ( ! B1 )
	{	wchar_t msgBuf[256];
		DWORD	err = GetLastError();
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, err, 0, (LPWSTR)&msgBuf, 0, NULL );
		TRACE( _T("Couldn't get the OS version: %s\n"), msgBuf );
	}
#endif
	const unsigned short WBUF_SIZE = 64;
	wchar_t	wbuf[ WBUF_SIZE ];
	DWORD	osMajVerNum = verInfo.dwMajorVersion;
	if ( osMajVerNum < 0 ) osMajVerNum = -1;

	long	minVer = verInfo.dwMinorVersion;
	long	bld = verInfo.dwBuildNumber;
	if ( minVer < 0 ) minVer = -1;
	swprintf( wbuf, _T("%li.%li.%li"), osMajVerNum, minVer, bld );

	CString	cs( wbuf );
	return	cs;
}			// GetOsVersion()
//--------------------------------------------------------------------------------------------
// #include, extern, KernelIoControl declaration, and the #define are
// from:  <http://www.pocketpcdn.com/articles/serial_number2002.html>
#include <WINIOCTL.H>
extern "C" __declspec(dllimport)

BOOL KernelIoControl( DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize,
					  LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned );

#define IOCTL_HAL_GET_DEVICEID	CTL_CODE( FILE_DEVICE_HAL, 21, METHOD_BUFFERED, FILE_ANY_ACCESS )

// the code is from http://64.41.105.202/forum/viewtopic.php?t=138&view=previous&sid=035c3fe1310d5e0f9fe80037765f71eb
CString		GetSerialNumber( void )
{	DWORD		dwOutBytes;
	const int	nBuffSize = 80;
	byte*		arrOutBuff = (byte*)LocalAlloc( LMEM_FIXED, nBuffSize );

	BOOL bRes = ::KernelIoControl( IOCTL_HAL_GET_DEVICEID, 0, 0, arrOutBuff, nBuffSize, &dwOutBytes );

		// if call fails - there seems to be no way to get it to succeed the first time
	if ( !bRes )
	{
		if ( GetLastError() == 122 )			// buffer not quite right, reallocate the buffer
		{
			UINT *pSize = (UINT*)&arrOutBuff[0];
//			char	buf[32];
//			sprintf( buf, "%d", *pSize );
//			MessageBox( CString(buf), _T("Buffer Size") );
			arrOutBuff = (byte*)LocalReAlloc( arrOutBuff, *pSize, LMEM_MOVEABLE );
			bRes = ::KernelIoControl( IOCTL_HAL_GET_DEVICEID, 0, 0, arrOutBuff, *pSize, &dwOutBytes );
		}
	}
		// if success, get the Serial Number
	if ( bRes )
	{	CString strDeviceInfo;
		for ( unsigned int ii = 0; ii < dwOutBytes; ii++ )
		{	CString		strNextChar;
			strNextChar.Format( _T("%02X"), arrOutBuff[ii] );
			strDeviceInfo += strNextChar;
		}
		CString strDeviceId = strDeviceInfo.Mid(40,16)
							+ strDeviceInfo.Mid(56,4)
							+ strDeviceInfo.Mid(64,12);
//		MessageBox( strDeviceId, _T("UUID") );
#ifdef _DEBUG
		TRACE( _T("UUID::GetSerialNumber: UUID=%s\n"), strDeviceId );
#endif
		LocalFree( arrOutBuff );
		return	strDeviceId;
	}
	return	_T("");
}			// GetSerialNumber()
//-----------------------------------------------------------------------------------------------
inline char 	AnsiCodeToHexChar( BYTE code )
{		// ANSI '0' => 48, ANSI 'A' (10) => 65
	return	(char)(code + (( code <= 9 )  ?  48  :  55 ));
}			// AnsiCodeToHexChar()
//-----------------------------------------------------------------------------------------------
char*	MakeHexString( BYTE* uuid, DWORD& len )
{		// the incoming uuid is actually a very long (little-endian) integer stored at uuid
		// i.e. since it is an int, it is stored in what appears to be reverse byte order (when
		// viewed as a string) so we have to 'unpack' it to produce a Unicode hex representation.
		// Little-endian architectures store the least significant bytes in the lowest order
		// addresses, but the compiler thinks uuid is a string, so the debugger presents the
		// string reversed in eight hex character chunks vs. an integer representation.
		// In:  len is the length of the BYTE string on the way in,
		// Out: len will be the length of the HEX string returned
	short	aa = 0;
	DWORD	nLongs = ( len + 3 ) / 4;
	DWORD*	tmp = new DWORD[ nLongs ];			// want to make sure we don't cut off any fractional bytes
	memcpy( tmp, uuid, len );					// len is in bytes
	DWORD	hexLen = len * 2;					// double wideLen to allocate narrow storage
	char*	res = new char[ hexLen + 1 ];
	for ( DWORD tt = 0; tt < nLongs; tt++ )
	{	unsigned int aLong = *(tmp+tt);			// 32-bit ints
		*(res+aa++) = AnsiCodeToHexChar( (BYTE)(aLong & 0xF) );		aLong >>= 4;
		*(res+aa++) = AnsiCodeToHexChar( (BYTE)(aLong & 0xF) );		aLong >>= 4;
		*(res+aa++) = AnsiCodeToHexChar( (BYTE)(aLong & 0xF) );		aLong >>= 4;
		*(res+aa++) = AnsiCodeToHexChar( (BYTE)(aLong & 0xF) );		aLong >>= 4;
		*(res+aa++) = AnsiCodeToHexChar( (BYTE)(aLong & 0xF) );		aLong >>= 4;
		*(res+aa++) = AnsiCodeToHexChar( (BYTE)(aLong & 0xF) );		aLong >>= 4;
		*(res+aa++) = AnsiCodeToHexChar( (BYTE)(aLong & 0xF) );		aLong >>= 4;
		*(res+aa++) = AnsiCodeToHexChar( (BYTE)(aLong & 0xF) );
	}
	*(res+hexLen) = '\0';
	len = hexLen;								// 'return' the ANSI hex string length
	return	res;
}			// MakeHexString()
//-----------------------------------------------------------------------------------------------
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
//----------------------------------------------------------------------------------------
unsigned short	ParseParamString( char* str, char marker, char*& fn, char*& ln, char*& em, char*& dv, unsigned char*& uu, unsigned char*& hu )
{		// expecting str to be of the format: "?fn=%s?ln=%s?dv=%s?uu=%s?hu=%s?em=%s?"
	// but, ... we will not assume anything about the order of appearance of the key=value pairs
	unsigned short slen = strlen( str );
	unsigned short nParams = 0;
	unsigned short jj = 0;

		// for all six params
	while ( jj < slen  &&  nParams < 6 )
	{		// skip anything that's not a param marker
		while ( jj < slen  &&  *(str+jj) != marker )
			jj++;
			// jj is now pointing at the param marker
		if ( jj > slen - 4 )
			break;							// there must be some space for a param
		jj++;								// pass up the marker character
		char paramName[3];					// all two character paramNames
		strncpy( paramName, str+jj, 2 );	// either { fn, ln, em, dv, uu, hu }
		paramName[2] = (char)NULL;

		jj += 3;							// pass the paramName and the equal sign that follows it
		unsigned short ii = jj;				// ii is the beginning of the param's value
		while ( jj < slen  &&  *(str+jj) != marker )
			jj++;
			// jj now points just past the end of the param value
		if ( jj <= ii )
			break;

		unsigned short nChars = jj - ii;
		unsigned char*	value = new unsigned char[ nChars + 1 ];
		strncpy( (char*)value, str+ii, nChars );
		*(value+nChars) = '\0';

			// assign the value to one of the return arguments
		if (	  strcmp(paramName,"fn") == 0 )
			fn = (char*)value;
		else if ( strcmp(paramName,"ln") == 0 )
			ln = (char*)value;
		else if ( strcmp(paramName,"em") == 0 )
			em = (char*)value;
		else if ( strcmp(paramName,"dv") == 0 )
			dv = (char*)value;
		else if ( strcmp(paramName,"uu") == 0 )
			uu = value;
		else if ( strcmp(paramName,"hu") == 0 )
			hu = value;
		else
		{
#ifdef	_DEBUG
			TRACE( _T("ParseParamString: no matching paramName=%s\n"), paramName );
#endif
			break;				// something has gone wrong
		}
		nParams++;			// we didn't break out, so something must have matched
	}
	return	nParams;
}			// ParseParamString()
//-----------------------------------------------------------------------------------------------
	// decrypt device info from the <productName> registry key
short	GetDeviceInfo( unsigned long& dv, unsigned char*& uu, unsigned char*& hu, char*& na, char*& em )
{	short	retVal = 0;			// success signal
	unsigned short nParams = 0;
	char* dvs = NULL;
	char* fn = NULL;
	char* ln = NULL;
	char* device = NULL;
	BYTE* ibuf = NULL;
	BYTE* obuf = NULL;
	if ( na ) delete [] na;
	if ( em ) delete [] em;
	if ( uu ) delete [] uu;
	if ( hu ) delete [] hu;
	const unsigned short nCkIvInts = 2 * AES_BLOCK_SIZE / sizeof(unsigned long);
	unsigned long ckiv[ nCkIvInts ] =
	{	0xBAE55009, 0x9728F296, 0xF7D9743A, 0x192F8FB9,
		0xBDB00F95, 0xDEBD503B, 0x38A20A9F, 0x25DFD213,
		0x50613562, 0x51A80B63, 0xB84E4383, 0xFA2563E7,
		0x48DA402A, 0x850B90FA, 0xC6BF6EC0, 0x235C3356
	};
	const wchar_t*	regKey = _T("SOFTWARE\\PocketNumerix\\BondManager");
	DWORD	nBytes;								// size of iBuf returned
	bool success = ReadRegBinary( HKEY_LOCAL_MACHINE, regKey, &ibuf, &nBytes );		// there's no terminator on ibuf
	if ( nBytes < 65 )
	{
#ifdef _DEBUG
		TRACE( _T("GetDeviceInfo: license key too short=%d\n"), nBytes );
#endif
		retVal = -1;
		goto	Exit;							// bail out
	}
		// expecting: "?fn=%s?ln=%s?em=%s?dv=%s?uu=%s?hu=%s", $firstName, $lastName, $email, $deviceid, $uuid, $huid
		// HKEY hKey, LPCTSTR pcszKey, DWORD nBytes, BYTE* value
	unsigned long rk[ nRjRoundKeys ];									// 120 round keys in Rijndael256
	rjKeySpan( (unsigned char*)ckiv, (unsigned long*)rk );				// generate ck specific RoundKeys
	obuf = new BYTE[ nBytes ];			// no terminator
	rjDecrypt( (unsigned char*)ckiv + AES_BLOCK_SIZE, ibuf, nBytes, obuf, rk );

		// parse obuf to recover dv, uu, hu (if on WM5), user name, and email
	nParams = ParseParamString( (char*)obuf, '?', fn, ln, em, dvs, uu, hu );
	if ( nParams < 6 )
	{	retVal = -2;
		goto	Exit;
	}
	if ( fn  &&  ln )
	{	unsigned short slen = strlen( fn );
		slen += strlen( ln );
		na = new char[ slen + 2 ];
		sprintf( na, "%s %s", fn, ln );
	}
	else
	{	retVal = -3;
		goto	Exit;
	}
	if ( dvs )
		sscanf( dvs, "%d", &dv );
	else
		retVal = -4;
Exit:
	if ( ibuf ) delete [] ibuf;
	if ( obuf ) delete [] obuf;
	if ( dvs  ) delete [] dvs;
	if ( fn   ) delete [] fn;
	if ( ln   ) delete [] ln;
	return	retVal;
}			// GetDeviceInfo()
//----------------------------------------------------------------------------------------
