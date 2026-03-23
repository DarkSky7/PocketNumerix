// RegistryManager.cpp: implementation of the CRegistryManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "nillahedge.h"
#include "RegistryManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegistryManager::CRegistryManager()
{
}

CRegistryManager::~CRegistryManager()
{
}
//----------------------------------------------------------------------------
bool		CRegistryManager::GetVolatilityExplorerState( VolatilityExplorerState& volExpState )
{	short	jj;
	long	storage[6];

		// read the quantities from the registry
	CString	key = _T("Software\\PocketNumerix\\NillaHedge\\VolatilityExplorer");
	bool	b0 = ReadRegBinary( HKEY_CURRENT_USER, key, 5*sizeof(long)+sizeof(char), (void*)&storage );
	if ( b0 )
	{		// copy the def_IDs
		for ( jj = 0; jj < 4; jj++ )
			volExpState.optDef_ID[jj] = *(storage+jj);

			// unpack the endDate
		volExpState.evalDate = unpackOleDate( *(storage+4) );

			// copy the button (and instrument) states
		char	pkdBtns = (char) *(storage+5) & 0xFF;				// mask the low order 8 bits

			// unpack the button states
		for ( jj = 0; jj < 4; jj++ )
		{	volExpState.optChk[jj] = pkdBtns & 0x1;
			pkdBtns >>= 1;
		}
		volExpState.optPortf = pkdBtns & 0x1;
		pkdBtns >>= 1;
		volExpState.pureBS = pkdBtns & 0x1;
	}
	return	b0;
}			// GetVolatilityExplorerState()
//----------------------------------------------------------------------------
bool		CRegistryManager::SetVolatilityExplorerState( VolatilityExplorerState& volExpState )
{		// pack five buttons in five bits
	short	jj;
	char	pkdBtns = ( volExpState.pureBS  ?  1  :  0 );
	pkdBtns <<= 1;
	pkdBtns |= ( volExpState.optPortf  ?  1  :  0 );
	for ( jj = 3; jj >= 0; jj-- )
	{	pkdBtns <<= 1;
#ifdef _DEBUG
		if ( jj < 0  || jj > 3 )
			AfxDebugBreak();
#endif
		pkdBtns |= ( volExpState.optChk[jj]  ?  1  :  0 );
	}

		// copy the def_IDs
	long	storage[6];
	for ( jj = 0; jj < 4; jj++ )
		*(storage+jj) = volExpState.optDef_ID[jj];

		// store the packed endDate
	*(storage+4) = packOleDate( volExpState.evalDate );

		// store the packed check button states
	*(storage+5) = (long)pkdBtns;				// low order byte has the lowest address


		// save 34 bytes worth of 'long' to the registry
	CString	key = _T("Software\\PocketNumerix\\NillaHedge\\VolatilityExplorer");
	bool	b0 = WriteRegBinary( HKEY_CURRENT_USER, key, 5*sizeof(long)+sizeof(char), (void*)&storage );
	return	b0;
}			// SetVolatilityExplorerState()
//----------------------------------------------------------------------------
bool		CRegistryManager::GetTimeDecay( TimeDecay& timeDecay )
{	short	jj;
	long	storage[6];

		// read the quantities from the registry
	CString	key = _T("Software\\PocketNumerix\\NillaHedge\\TimeDecay");
	bool	b0 = ReadRegBinary( HKEY_CURRENT_USER, key, 5*sizeof(long)+sizeof(char), (void*)&storage );
	if ( b0 )
	{		// copy the def_IDs
		for ( jj = 0; jj < 4; jj++ )
			timeDecay.optDef_ID[jj] = *(storage+jj);

			// unpack the endDate
		timeDecay.endDate = unpackOleDate( *(storage+4) );

			// copy the button (and instrument) states
		char	pkdBtns = (char) *(storage+5) & 0xFF;				// mask the low order 8 bits

			// unpack the button states
		for ( jj = 0; jj < 4; jj++ )
		{	timeDecay.optChk[jj] = pkdBtns & 0x1;
			pkdBtns >>= 1;
		}
		timeDecay.optPortf = pkdBtns & 0x1;
		pkdBtns >>= 1;
		timeDecay.pureBS = pkdBtns & 0x1;
	}
	return	b0;
}			// GetTimeDecay()
//----------------------------------------------------------------------------
bool		CRegistryManager::SetTimeDecay( TimeDecay& timeDecay )
{		// pack five buttons in five bits
	short	jj;
	char	pkdBtns = ( timeDecay.pureBS  ?  1  :  0 );
	pkdBtns <<= 1;
	pkdBtns |= ( timeDecay.optPortf  ?  1  :  0  );
	for ( jj = 3; jj >= 0; jj-- )
	{	pkdBtns <<= 1;
#ifdef _DEBUG
		if ( jj < 0  || jj > 3 )
			AfxDebugBreak();
#endif
		pkdBtns |= ( timeDecay.optChk[jj]  ?  1  :  0 );
	}

		// copy the def_IDs
	long	storage[6];
	for ( jj = 0; jj < 4; jj++ )
		*(storage+jj) = timeDecay.optDef_ID[jj];

		// store the packed endDate
	*(storage+4) = packOleDate( timeDecay.endDate );

		// store the packed check button states
	*(storage+5) = (long)pkdBtns;				// low order byte has the lowest address


		// save 34 bytes worth of 'long' to the registry
	CString	key = _T("Software\\PocketNumerix\\NillaHedge\\TimeDecay");
	bool	b0 = WriteRegBinary( HKEY_CURRENT_USER, key, 5*sizeof(long)+sizeof(char), (void*)&storage );
	return	b0;
}			// SetTimeDecay()
//----------------------------------------------------------------------------
bool		CRegistryManager::GetOptionRateSensitivity( OptionRateSensitivity& rateSens )
{	short	jj;
	long	storage[5];
		// read the quantities from the registry
	CString	key = _T("Software\\PocketNumerix\\NillaHedge\\OptionRateSensitivity\\DefsAndSwitches");
	bool	b0 = ReadRegBinary( HKEY_CURRENT_USER, key, 4*sizeof(long)+sizeof(char), (void*)&storage );
	if ( b0 )
	{		// copy the def_IDs
		for ( jj = 0; jj < 4; jj++ )
			rateSens.optDef_ID[jj] = *(storage+jj);

			// copy the button (and instrument) states
		long	pkdBtns = *(storage+4) & 0x1F;				// mask the low order 5 bits

			// unpack the button states
		for ( jj = 0; jj < 4; jj++ )
		{	rateSens.optChk[jj] = pkdBtns & 0x1;
			pkdBtns >>= 1;
		}
		rateSens.optPortf = pkdBtns & 0x1;
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// copy the floats
	float	deltas[2];
	key = _T("Software\\PocketNumerix\\NillaHedge\\OptionRateSensitivity\\Deltas");
	bool	b1 = ReadRegBinary( HKEY_CURRENT_USER, key, 2*sizeof(float), (void*)&deltas );
	if ( b1 )
	{	rateSens.deltaLow  = (float)*(deltas+0);
		rateSens.deltaHigh = (float)*(deltas+1);
	}
	return	b0  &&  b1;
}			// GetOptionRateSensitivity()
//----------------------------------------------------------------------------
bool		CRegistryManager::SetOptionRateSensitivity( OptionRateSensitivity& rateSens )
{		// pack ten buttons & instrument in twelve bits
	short	jj;
	short	pkdBtns = ( rateSens.optPortf  ?  1  :  0 );
	for ( jj = 3; jj >= 0; jj-- )
	{	pkdBtns <<= 1;
#ifdef _DEBUG
		if ( jj < 0  || jj > 3 )
			AfxDebugBreak();
#endif
		pkdBtns |= ( rateSens.optChk[jj]  ?  1  :  0 );
	}

		// copy the def_IDs
	long	storage[5];
	for ( jj = 0; jj < 4; jj++ )
	{
		*(storage+jj) = rateSens.optDef_ID[jj];
	}
	*(storage+4) = pkdBtns;					// low order bytes have lowest addresses

		// a short, four longs, and two floats
		// save 17 bytes worth of 'long' to the registry
	CString	key = _T("Software\\PocketNumerix\\NillaHedge\\OptionRateSensitivity\\DefsAndSwitches");
	bool	b0 = WriteRegBinary( HKEY_CURRENT_USER, key, 4*sizeof(long)+sizeof(char), (void*)&storage );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// copy and save the floats
	float	deltas[2];
	*(deltas+0) = rateSens.deltaLow;
	*(deltas+1) = rateSens.deltaHigh;
	key = _T("Software\\PocketNumerix\\NillaHedge\\OptionRateSensitivity\\Deltas");
	bool	b1 = WriteRegBinary( HKEY_CURRENT_USER, key, 2*sizeof(float), (void*)&deltas );
	return	b0  &&  b1;
}			// SetOptionRateSensitivity()
//----------------------------------------------------------------------------
bool		CRegistryManager::GetHedge( OneStk3OptHedge& os3oh )
{
	unsigned char	flags = 0;							// BuySell status storage

		// read the quantities from the registry
	CString	floatsKey = _T("Software\\PocketNumerix\\NillaHedge\\HedgeQtys");
	bool	b0 = ReadRegBinary( HKEY_CURRENT_USER, floatsKey, 4*sizeof(float), (void*)&(os3oh.qty) );

		// read the packed flags from the registry
	CString	flagsKey = _T("Software\\PocketNumerix\\NillaHedge\\HedgeFlags");
	bool	b1 = ReadRegBinary( HKEY_CURRENT_USER, flagsKey, 1, (void*)&flags );

		// read the def_IDs from the registry
	CString	longsKey = _T("Software\\PocketNumerix\\NillaHedge\\HedgeDefIDs");
	bool	b2 = ReadRegBinary( HKEY_CURRENT_USER, longsKey, 4*sizeof(long), (void*)&(os3oh.def_ID) );

		// unpack the 'booleans' (really tri-state)
	for ( short ii = 0; ii < 4; ii++ )
	{	os3oh.buySell[ii] = (BuySellStatus)(flags & 0x3);	// BuySellStatus is tristate -> 2 bits
		flags >>= 2;
	}

	return	b0  &&  b1  &&  b2;
}			// GetHedge()
//----------------------------------------------------------------------------
bool		CRegistryManager::SetHedge( OneStk3OptHedge& os3oh )
{	short	ii;
		// pack the 'booleans' (really tri-state)
	unsigned char	flags = 0;							// BuySell status storage
	for ( ii = 3; ii >= 0; ii-- )
	{		// pack the BuySell flags
#ifdef _DEBUG
		if ( ii < 0  || ii > 3 )
			AfxDebugBreak();
#endif
		flags |= (unsigned char)(os3oh.buySell[ii]);	// the high order bits
		if ( ii != 0 )									// don't shift the last time through the loop
			flags <<= 2;
	}

		// save the quantities to the registry
	CString	floatsKey = _T("Software\\PocketNumerix\\NillaHedge\\HedgeQtys");
	bool	b0 = WriteRegBinary( HKEY_CURRENT_USER, floatsKey, 4*sizeof(float), (void*)&(os3oh.qty) );

		// save the flags to the registry
	CString	flagsKey = _T("Software\\PocketNumerix\\NillaHedge\\HedgeFlags");
	bool	b1 = WriteRegBinary( HKEY_CURRENT_USER, flagsKey, 1, (void*)&flags );

		// save the def_IDs to the registry
	CString	longsKey = _T("Software\\PocketNumerix\\NillaHedge\\HedgeDefIDs");
	bool	b2 = WriteRegBinary( HKEY_CURRENT_USER, longsKey, 4*sizeof(long), (void*)&(os3oh.def_ID) );

	return	b0  &&  b1  &&  b2;
}			// SetHedge()
//----------------------------------------------------------------------------
bool		CRegistryManager::GetSocketRecvWait( unsigned long& milliseconds )
{	CString	keyName = _T("SOFTWARE\\PocketNumerix\\SocketRecvWait");
	return	ReadRegBinary( HKEY_CURRENT_USER, keyName, sizeof(milliseconds), (void*)&milliseconds );
}			// GetSocketRecvWait()
//----------------------------------------------------------------------------------------
bool		CRegistryManager::GetSocketSendWait( unsigned long& milliseconds )
{	CString	keyName = _T("SOFTWARE\\PocketNumerix\\SocketSendWait");
	return	ReadRegBinary( HKEY_CURRENT_USER, keyName, sizeof(milliseconds), (void*)&milliseconds );
}			// GetSocketSendWait()
//----------------------------------------------------------------------------------------
bool		CRegistryManager::GetPosListColVisibility( long& areVisible )
{	CString	keyName = _T("SOFTWARE\\PocketNumerix\\PositionManager\\ColumnVisibility");
	return	ReadRegLong( HKEY_CURRENT_USER, keyName, areVisible );
}			// GetPosListColVisibility()
//----------------------------------------------------------------------------
bool		CRegistryManager::SetPosListColVisibility( long areVisible )
{	CString	keyName = _T("SOFTWARE\\PocketNumerix\\PositionManager\\ColumnVisibility");
	return	WriteRegLong( HKEY_CURRENT_USER, keyName, areVisible );
}			// SetPosListColVisibility()
//----------------------------------------------------------------------------
bool		CRegistryManager::GetOCRServer( char*& serverName )
{	char	buf[256];
	bool	b0 = ReadRegBinary( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\PocketNumerix\\OCRServer"), sizeof(buf), buf );
	if ( b0 )
	{	short	slen = strlen( buf );
		serverName = new char[ slen+1 ];
		strncpy( serverName, buf, slen+1 );
	}
	return	b0;									// caller responsible for deleting serverName
}			// GetOCRServer()
//----------------------------------------------------------------------------
bool		CRegistryManager::ReadRegBinary( HKEY hKey, LPCTSTR pcszKey, short nBytes, void* outBuf )
{	ASSERT ( AfxIsValidString(pcszKey) );
	LONG	err = RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_QUERY_VALUE, &hKey );
	if ( err != ERROR_SUCCESS )
	{
#ifdef _DEBUG
		wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("RegistryManager::ReadRegBinary(open %s): %s.\n"), pcszKey, buf );
#endif
		return	false;
	}
	DWORD	dwType = (DWORD)REG_BINARY;
	DWORD	cbData = (DWORD)nBytes;
		//				   HKEY  LPCWSTR  0  LPDWORD   LPBYTE		 LPDWORD
	err = RegQueryValueEx( hKey, pcszKey, 0, &dwType, (PBYTE)outBuf, &cbData );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{	wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("RegistryManager::ReadRegBinary(query %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			// ReadRegBinary()
//-----------------------------------------------------------------------------------------------
bool		CRegistryManager::WriteRegBinary( HKEY hKey, LPCTSTR pcszKey, short nBytes, void* inBuf )
{	ASSERT ( AfxIsValidString(pcszKey) );
//	RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_SET_VALUE, &hKey );
	LPWSTR	cls = _T("binary");
	DWORD	disp;

	LONG	err = RegCreateKeyEx( HKEY_CURRENT_USER, pcszKey, 0, cls, 0, 0, 0, &hKey, &disp );
#ifdef _DEBUG
	TRACE( _T("RegistryManager::WriteRegBinary: RegCreateKeyEx(%s %s): err=%d (should be zero).\n"),
		(disp == REG_CREATED_NEW_KEY) ? _T("create") : _T("open"), pcszKey, err );
#endif
	if ( err != ERROR_SUCCESS )
		return	false;

		//				 HKEY  LPCWSTR  0  DWORD        BYTE*		  DWORD
	err = RegSetValueEx( hKey, pcszKey, 0, REG_BINARY, (PBYTE)inBuf, nBytes );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{	wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("RegistryManager::WriteRegBinary(set %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			//WriteRegBinary()
//-----------------------------------------------------------------------------------------------
bool		CRegistryManager::ReadRegLong( HKEY hKey, LPCTSTR pcszKey, long& dwValue )
{	ASSERT ( AfxIsValidString(pcszKey) );
	SetLastError( 0 );
	LONG	err = RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_QUERY_VALUE, &hKey );
	if ( err != ERROR_SUCCESS )
	{
#ifdef _DEBUG
		wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("RegistryManager::ReadRegLong(open %s): %s.\n"), pcszKey, buf );
#endif
		return false;
	}
	DWORD	dwType = (long)REG_DWORD;
	DWORD	cbData = sizeof(DWORD);
		//				   HKEY  LPCWSTR  0  LPDWORD  LPBYTE          LPDWORD
	err = RegQueryValueEx( hKey, pcszKey, 0, &dwType, (PBYTE)&dwValue, &cbData );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{	wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("RegistryManager::ReadRegLong(query %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			// ReadRegLong()
//-----------------------------------------------------------------------------------------------
bool		CRegistryManager::WriteRegLong( HKEY hKey, LPCTSTR pcszKey, long dwValue )
{	ASSERT ( AfxIsValidString(pcszKey) );
//	RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_SET_VALUE, &hKey );
	LPWSTR	cls = _T("long");
	DWORD	disp;

	LONG	err = RegCreateKeyEx( HKEY_CURRENT_USER, pcszKey, 0, cls, 0, 0, 0, &hKey, &disp );
#ifdef _DEBUG
	TRACE( _T("RegistryManager::WriteRegLong: RegCreateKeyEx(%s %s): err=%d (should be zero).\n"),
		(disp == REG_CREATED_NEW_KEY) ? _T("create") : _T("open"), pcszKey, err );
#endif
	if ( err != ERROR_SUCCESS )
		return false;

		//				 HKEY  LPCWSTR  0  DWORD       BYTE*           DWORD
	err = RegSetValueEx( hKey, pcszKey, 0, REG_DWORD, (PBYTE)&dwValue, sizeof(DWORD) );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{	wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("RegistryManager::WriteRegLong(set %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			//WriteRegLong()
//-----------------------------------------------------------------------------------------------
bool		CRegistryManager::ReadRegFloat( HKEY hKey, LPCTSTR pcszKey, float& fValue )
{	ASSERT ( AfxIsValidString(pcszKey) );
	LONG	err = RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_QUERY_VALUE, &hKey );
	if ( err != ERROR_SUCCESS )
	{
#ifdef _DEBUG
		wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("RegistryManager::ReadRegFloat(open %s): %s.\n"), pcszKey, buf );
#endif
		return	false;
	}
	DWORD	dwType = (DWORD)REG_DWORD;
	DWORD	cbData = sizeof(float);
		//				   HKEY  LPCWSTR  0  LPDWORD  LPBYTE          LPDWORD
	err = RegQueryValueEx( hKey, pcszKey, 0, &dwType, (PBYTE)&fValue, &cbData );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{	wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("RegistryManager::ReadRegFloat(query %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			// ReadRegFloat()
//-----------------------------------------------------------------------------------------------
bool		CRegistryManager::WriteRegFloat( HKEY hKey, LPCTSTR pcszKey, float fValue )
{	ASSERT ( AfxIsValidString(pcszKey) );
//	RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_SET_VALUE, &hKey );
	LPWSTR	cls = _T("float");
	DWORD	disp;

	LONG	err = RegCreateKeyEx( HKEY_CURRENT_USER, pcszKey, 0, cls, 0, 0, 0, &hKey, &disp );
#ifdef _DEBUG
	TRACE( _T("RegistryManager::WriteRegFloat: RegCreateKeyEx(%s %s): err=%d (should be zero).\n"),
		(disp == REG_CREATED_NEW_KEY) ? _T("create") : _T("open"), pcszKey, err );
#endif
	if ( err != ERROR_SUCCESS )
		return	false;

		//				 HKEY  LPCWSTR  0  DWORD       BYTE*           DWORD
	err = RegSetValueEx( hKey, pcszKey, 0, REG_DWORD, (PBYTE)&fValue, sizeof(float) );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{	wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("RegistryManager::WriteRegFloat(set %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			//WriteRegFloat()
//-----------------------------------------------------------------------------------------------
bool		CRegistryManager::ReadRegShort( HKEY hKey, LPCTSTR pcszKey, short& sValue )
{	ASSERT ( AfxIsValidString(pcszKey) );
	LONG	err = RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_QUERY_VALUE, &hKey );
	if ( err != ERROR_SUCCESS )
	{
#ifdef _DEBUG
		wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("RegistryManager::ReadRegShort(open %s): %s.\n"), pcszKey, buf );
#endif
		return	false;
	}
	DWORD	dwType = (DWORD)REG_BINARY;
	DWORD	cbData = sizeof(short);
		//				   HKEY  LPCWSTR  0  LPDWORD  LPBYTE          LPDWORD
	err = RegQueryValueEx( hKey, pcszKey, 0, &dwType, (PBYTE)&sValue, &cbData );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{	wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("RegistryManager::ReadRegShort(query %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			// ReadRegShort()
//-----------------------------------------------------------------------------------------------
bool		CRegistryManager::WriteRegShort( HKEY hKey, LPCTSTR pcszKey, short sValue )
{	ASSERT ( AfxIsValidString(pcszKey) );
//	RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_SET_VALUE, &hKey );
	LPWSTR	cls = _T("short");
	DWORD	disp;

	LONG	err = RegCreateKeyEx( HKEY_CURRENT_USER, pcszKey, 0, cls, 0, 0, 0, &hKey, &disp );
#ifdef _DEBUG
	TRACE( _T("RegistryManager::WriteRegShort: RegCreateKeyEx(%s %s): err=%d (should be zero).\n"),
		(disp == REG_CREATED_NEW_KEY) ? _T("create") : _T("open"), pcszKey, err );
#endif
	if ( err != ERROR_SUCCESS )
		return	false;

		//				 HKEY  LPCWSTR  0  DWORD       BYTE*           DWORD
	err = RegSetValueEx( hKey, pcszKey, 0, REG_BINARY, (PBYTE)&sValue, sizeof(short) );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{	wchar_t	buf[128];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 127, NULL );
		TRACE( _T("RegistryManager::WriteRegShort(set %s): %s.\n"), pcszKey, buf );
	}
#endif
	RegCloseKey( hKey );
	return	err == ERROR_SUCCESS;
}			//WriteRegShort()
//-----------------------------------------------------------------------------------------------
