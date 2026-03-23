// Registry.cpp (borrowed functions most from YCF, added ReadRegShort and WriteRegShort)

#include "StdAfx.h"
#include "Registry.h"

bool	ReadRegShort( HKEY hKey, wchar_t* pcszKey, short* value )
{
	return	ReadReg( hKey, pcszKey, REG_BINARY, (BYTE*)value, sizeof(short) );
}			// ReadRegDWORD()
//-----------------------------------------------------------------------------------------
bool	WriteRegShort( HKEY hKey, wchar_t* pcszKey, short value )
{
	return	WriteReg( hKey, pcszKey, REG_BINARY, (BYTE*)&value, sizeof(short) );
}			// ReadRegDWORD()
//-----------------------------------------------------------------------------------------
bool	ReadRegBinary( HKEY hKey, wchar_t* pcszKey, BYTE** value, DWORD* nBytes )
{		// allocates space for value
	return	ReadReg( hKey, pcszKey, REG_BINARY, value, nBytes );
}			// ReadRegBinary()
//-----------------------------------------------------------------------------------------
bool	ReadRegBinary( HKEY hKey, wchar_t* pcszKey, BYTE* value, DWORD nBytes )
{		// expects value to already be allocated
	return	ReadReg( hKey, pcszKey, REG_BINARY, value, nBytes );
}			// ReadRegBinary()
//-----------------------------------------------------------------------------------------
bool	WriteRegBinary( HKEY hKey, wchar_t* pcszKey, BYTE* value, DWORD nBytes )
{		// expects space for value to be allocated already
	return	WriteReg( hKey, pcszKey, REG_BINARY, value, nBytes );
}			// WriteRegBinary()
//-----------------------------------------------------------------------------------------
bool		ReadReg( HKEY hKey, wchar_t* pcszKey, DWORD type, BYTE** value, DWORD* nBytes )
{		// this ReadReg() allocates the space required for value
	ASSERT ( AfxIsValidString(pcszKey) );
	bool	retVal = false;
	HKEY	subkey = NULL;

		// break the key name into prefix and suffix
	CString	cs = pcszKey;
	int		lastBackslash = cs.ReverseFind( _T('\\') );
	CString prefix = cs.Left( lastBackslash );				// everything to the left of the last backslash
	CString suffix = cs.Mid( lastBackslash + 1 );			// everything to the right of the last backslash

	LONG	err = RegOpenKeyEx( hKey, prefix, 0, 0, &subkey );
#ifdef _DEBUG
	TRACE( _T("ReadReg: RegOpenKeyEx(%s): err=%d (should be zero).\n"), pcszKey, err );
#endif
	if ( err != ERROR_SUCCESS )
		goto	Exit;

	err = RegQueryInfoKey( subkey, NULL, NULL, NULL, NULL, NULL, NULL,
								   NULL, NULL, nBytes, NULL, NULL );
#ifdef _DEBUG
	TRACE( _T("ReadReg: RegQueryInfoKey(%s): err=%d (should be zero).\n"), pcszKey, err );
#endif
	if ( err != ERROR_SUCCESS )
		goto	Exit;

	*value = new BYTE[ (type == REG_SZ)  ?  *nBytes + 1  :  *nBytes ];
		//				    HKEY  LPCWSTR  0  DWORD*  BYTE*  DWORD*
	err = RegQueryValueEx( subkey, suffix, 0, &type, *value, nBytes );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{		// this frequently happens with err = 87, meaning 'Operation completed successfully'
		wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("ReadReg: RegQueryValueEx(%s): %d --> %s.\n"), pcszKey, err, buf );
	}
#endif
	if ( type == REG_SZ )
		*(*value + *nBytes) = NULL;					// terminator for strings only
	retVal = ( err == ERROR_SUCCESS );
Exit:
	RegCloseKey( subkey );
	RegCloseKey( hKey );
	return	retVal;	
}			// ReadReg()
//-----------------------------------------------------------------------------------------
bool	ReadReg( HKEY hKey, wchar_t* pcszKey, DWORD type, BYTE* value, DWORD nBytes )
{		// this ReadReg() assumes that nBytes of space for value is already allocated
	ASSERT ( AfxIsValidString(pcszKey) );
	bool	retVal = false;
	HKEY	subkey = NULL;

		// break the key name into prefix and suffix
	CString	cs = pcszKey;
	int		lastBackslash = cs.ReverseFind( _T('\\') );
	CString prefix = cs.Left( lastBackslash );			// everything to the left of the last backslash
	CString suffix = cs.Mid( lastBackslash + 1 );		// everything to the right of the last backslash

	LONG	err = RegOpenKeyEx( hKey, prefix, 0, 0, &subkey );
#ifdef _DEBUG
	TRACE( _T("ReadReg: RegOpenKeyEx(%s): err=%d (should be zero).\n"), pcszKey, err );
#endif
	if ( err != ERROR_SUCCESS )
		goto	Exit;

		//				    HKEY  LPCWSTR  0  DWORD* BYTE*   DWORD*
	err = RegQueryValueEx( subkey, suffix, 0, &type, value, &nBytes );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{		// this frequently happens with err = 87 or 234, meaning 'Operation completed successfully'
		wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("ReadReg: RegQueryValueEx(%s): %d --> %s.\n"), pcszKey, err, buf );
	}
#endif
	retVal = ( err == ERROR_SUCCESS );
Exit:
	RegCloseKey( subkey );
	RegCloseKey( hKey );
	return	retVal;
}			// ReadReg()
//-----------------------------------------------------------------------------------------------
bool		WriteReg( HKEY hKey, wchar_t* pcszKey, DWORD type, BYTE* value, DWORD nBytes )
{	HKEY	subkey = NULL;
	ASSERT ( AfxIsValidString(pcszKey) );
//	RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_SET_VALUE, &hKey );
//	LPWSTR	cls = _T("float");
	bool	retVal = false;
	DWORD	disp;
	CString	cs = pcszKey;
	int	lastBackslash = cs.ReverseFind( _T('\\') );
	CString prefix = cs.Left( lastBackslash );			// everything to the left of the last backslash
	CString suffix = cs.Mid( lastBackslash+1 );			// everything to the right of the last backslash

	long	err = RegCreateKeyEx( hKey, prefix, 0, 0, 0, 0, 0, &subkey, &disp );
#ifdef _DEBUG
	TRACE( _T("ReadReg: RegCreateKeyEx(%s %s): err=%d (should be zero).\n"),
		(disp == REG_CREATED_NEW_KEY) ? _T("create") : _T("open"), prefix, err );
#endif
	if ( err != ERROR_SUCCESS )
		goto	Exit;

		//			     HKEY  LPCWSTR  0  DWORD  BYTE*  DWORD
	err = RegSetValueEx( subkey, suffix, 0, type, value, nBytes );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{		// this frequently happens with err = 87, meaning 'Operation completed successfully'
		wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("WriteReg: RegSetValueEx(%s): %d --> %s.\n"), pcszKey, err, buf );
	}
#endif
	retVal = ( err == ERROR_SUCCESS );
Exit:
	RegCloseKey( subkey );
	RegCloseKey( hKey );
	return	retVal;
}			// WriteReg()
//-----------------------------------------------------------------------------------------
