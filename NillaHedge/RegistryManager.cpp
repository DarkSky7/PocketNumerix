// RegistryManager.cpp: implementation of the CRegistryManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
//#include "BondManagerApp.h"
#include "RegistryManager.h"
//#include "BondRateSensitivity.h"
#include "utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

bool	GetOCRPage( BYTE** pageName, DWORD* nBytes )
{
	const wchar_t* key = _T("SOFTWARE\\PocketNumerix\\OptionChains\\Page");
	return	ReadRegString( HKEY_LOCAL_MACHINE, key, pageName, nBytes );
}			// GetOCRPage()
//----------------------------------------------------------------------------------------
bool	GetOCRServer( BYTE** serverName, DWORD* nBytes )
{
	const wchar_t* key = _T("SOFTWARE\\PocketNumerix\\OptionChains\\Server");
	return	ReadRegString( HKEY_LOCAL_MACHINE, key, serverName, nBytes );
}			// GetOCRServer()
//----------------------------------------------------------------------------------------
bool	GetSocketRecvWait( DWORD* milliseconds )
{
	const wchar_t* key = _T("SOFTWARE\\PocketNumerix\\OptionChains\\RecvWait");
	return	ReadRegDWORD( HKEY_LOCAL_MACHINE, key, milliseconds );
}			// GetSocketRecvWait()
//----------------------------------------------------------------------------------------
bool	GetSocketSendWait( DWORD* milliseconds )
{
	const wchar_t* key = _T("SOFTWARE\\PocketNumerix\\OptionChains\\SendWait");
	return	ReadRegDWORD( HKEY_LOCAL_MACHINE, key, milliseconds );
}			// GetSocketSendWait()
//----------------------------------------------------------------------------------------
bool	GetRecentInstrument( AssetType* at )					// failure --> false
{
	const wchar_t* key = _T("SOFTWARE\\PocketNumerix\\NillaHedge\\RecentInstrument");
	return	ReadRegShort( HKEY_CURRENT_USER, key, (short*)at );
}
//----------------------------------------------------------------------------
bool	SetRecentInstrument( AssetType at )						// failure --> false
{
	const wchar_t* key = _T("SOFTWARE\\PocketNumerix\\NillaHedge\\RecentInstrument");
	return	WriteRegShort( HKEY_CURRENT_USER, key, (short)at );
}
//----------------------------------------------------------------------------
bool	GetHedge( OneStk3OptHedge* os3oh )
{
	unsigned char	flags = 0;							// BuySell status storage

		// read the quantities from the registry
	wchar_t* key = _T("Software\\PocketNumerix\\NillaHedge\\HedgeQtys");
	bool	b0 = ReadRegBinary( HKEY_CURRENT_USER, key, (BYTE*)&(os3oh->qty), 4*sizeof(float) );

		// read the packed flags from the registry
	key = _T("Software\\PocketNumerix\\NillaHedge\\HedgeFlags");
	bool	b1 = ReadRegBinary( HKEY_CURRENT_USER, key, (BYTE*)&flags, sizeof(char) );

		// read the def_IDs from the registry
	key = _T("Software\\PocketNumerix\\NillaHedge\\HedgeDefIDs");
	bool	b2 = ReadRegBinary( HKEY_CURRENT_USER, key, (BYTE*)&(os3oh->def_ID), 4*sizeof(long) );

		// unpack the 'booleans' (really tri-state)
	for ( short ii = 0; ii < 4; ii++ )
	{	os3oh->buySell[ii] = (BuySellStatus)(flags & 0x3);	// BuySellStatus is tristate -> 2 bits
		flags >>= 2;
	}

	return	b0  &&  b1  &&  b2;
}			// GetHedge()
//----------------------------------------------------------------------------
bool	SetHedge( OneStk3OptHedge os3oh )
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
	wchar_t* key = _T("Software\\PocketNumerix\\NillaHedge\\HedgeQtys");
	bool	b0 = WriteRegBinary( HKEY_CURRENT_USER, key, (BYTE*)&(os3oh.qty), 4*sizeof(float) );

		// save the flags to the registry
	key = _T("Software\\PocketNumerix\\NillaHedge\\HedgeFlags");
	bool	b1 = WriteRegBinary( HKEY_CURRENT_USER, key, (BYTE*)&flags, sizeof(char) );

		// save the def_IDs to the registry
	key = _T("Software\\PocketNumerix\\NillaHedge\\HedgeDefIDs");
	bool	b2 = WriteRegBinary( HKEY_CURRENT_USER, key, (BYTE*)&(os3oh.def_ID), 4*sizeof(long) );

	return	b0  &&  b1  &&  b2;
}			// SetHedge()
//----------------------------------------------------------------------------
bool	GetOptionRateSensitivity( OptionRateSensitivity* rateSens )
{	short	jj;
	long	storage[5];
		// read the quantities from the registry
	wchar_t* key = _T("Software\\PocketNumerix\\NillaHedge\\OptionRateSensitivity\\DefsAndSwitches");
	bool	b0 = ReadRegBinary( HKEY_CURRENT_USER, key, (BYTE*)&storage, 4*sizeof(long)+sizeof(char) );
	if ( b0 )
	{		// copy the def_IDs
		for ( jj = 0; jj < 4; jj++ )
			rateSens->optDef_ID[jj] = *(storage+jj);

			// copy the button (and instrument) states
		long	pkdBtns = *(storage+4) & 0x1F;				// mask the low order 5 bits

			// unpack the button states
		for ( jj = 0; jj < 4; jj++ )
		{	rateSens->optChk[jj] = pkdBtns & 0x1;
			pkdBtns >>= 1;
		}
		rateSens->optPortf = pkdBtns & 0x1;
	}

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// copy the floats
	float	deltas[2];
	key = _T("Software\\PocketNumerix\\NillaHedge\\OptionRateSensitivity\\Deltas");
	bool	b1 = ReadRegBinary( HKEY_CURRENT_USER, key, (BYTE*)deltas, 2*sizeof(float) );
	if ( b1 )
	{	rateSens->deltaLow  = (float)*(deltas+0);
		rateSens->deltaHigh = (float)*(deltas+1);
	}
	return	b0  &&  b1;
}			// GetOptionRateSensitivity()
//----------------------------------------------------------------------------
bool	SetOptionRateSensitivity( OptionRateSensitivity rateSens )
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
	wchar_t* key = _T("Software\\PocketNumerix\\NillaHedge\\OptionRateSensitivity\\DefsAndSwitches");
	bool	b0 = WriteRegBinary( HKEY_CURRENT_USER, key, (BYTE*)&storage, 4*sizeof(long)+sizeof(char) );

		// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		// copy and save the floats
	float	deltas[2];
	*(deltas+0) = rateSens.deltaLow;
	*(deltas+1) = rateSens.deltaHigh;
	key = _T("Software\\PocketNumerix\\NillaHedge\\OptionRateSensitivity\\Deltas");
	bool	b1 = WriteRegBinary( HKEY_CURRENT_USER, key, (BYTE*)&deltas, 2*sizeof(float) );
	return	b0  &&  b1;
}			// SetOptionRateSensitivity()
//----------------------------------------------------------------------------
bool	GetTimeDecay( TimeDecay* timeDecay )
{	short	jj;
	long	storage[6];

		// read the quantities from the registry
	const wchar_t* key = _T("Software\\PocketNumerix\\NillaHedge\\TimeDecay");
	bool	b0 = ReadRegBinary( HKEY_CURRENT_USER, key, (BYTE*)storage, 5*sizeof(long)+sizeof(char) );
	if ( b0 )
	{		// copy the def_IDs
		for ( jj = 0; jj < 4; jj++ )
			timeDecay->optDef_ID[jj] = *(storage+jj);

			// unpack the endDate
		timeDecay->endDate = UnpackOleDate( *(storage+4) );

			// copy the button (and instrument) states
		char	pkdBtns = (char) *(storage+5) & 0xFF;				// mask the low order 8 bits

			// unpack the button states
		for ( jj = 0; jj < 4; jj++ )
		{	timeDecay->optChk[jj] = pkdBtns & 0x1;
			pkdBtns >>= 1;
		}
		timeDecay->optPortf = pkdBtns & 0x1;
		pkdBtns >>= 1;
		timeDecay->pureBS = pkdBtns & 0x1;
	}
	return	b0;
}			// GetTimeDecay()
//----------------------------------------------------------------------------
bool	SetTimeDecay( TimeDecay timeDecay )
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
	*(storage+4) = PackOleDate( timeDecay.endDate );

		// store the packed check button states
	*(storage+5) = (long)pkdBtns;				// low order byte has the lowest address

		// save 34 bytes worth of 'long' to the registry
	const wchar_t* key = _T("Software\\PocketNumerix\\NillaHedge\\TimeDecay");
	bool	b0 = WriteRegBinary( HKEY_CURRENT_USER, key, (BYTE*)storage, 5*sizeof(long)+sizeof(char) );
	return	b0;
}			// SetTimeDecay()
//----------------------------------------------------------------------------
bool	GetVolatilityExplorerState( VolatilityExplorerState* volExpState )
{	short	jj;
	long	storage[6];

		// read the quantities from the registry
	const wchar_t* key = _T("Software\\PocketNumerix\\NillaHedge\\VolatilityExplorer");
	bool	b0 = ReadRegBinary( HKEY_CURRENT_USER, key, (BYTE*)&storage, 5*sizeof(long)+sizeof(char) );
	if ( b0 )
	{		// copy the def_IDs
		for ( jj = 0; jj < 4; jj++ )
			volExpState->optDef_ID[jj] = *(storage+jj);

			// unpack the endDate
		volExpState->evalDate = UnpackOleDate( *(storage+4) );

			// copy the button (and instrument) states
		char	pkdBtns = (char) *(storage+5) & 0xFF;				// mask the low order 8 bits

			// unpack the button states
		for ( jj = 0; jj < 4; jj++ )
		{	volExpState->optChk[jj] = pkdBtns & 0x1;
			pkdBtns >>= 1;
		}
		volExpState->optPortf = pkdBtns & 0x1;
		pkdBtns >>= 1;
		volExpState->pureBS = pkdBtns & 0x1;
	}
	return	b0;
}			// GetVolatilityExplorerState()
//----------------------------------------------------------------------------
bool	SetVolatilityExplorerState( VolatilityExplorerState volExpState )
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
	*(storage+4) = PackOleDate( volExpState.evalDate );

		// store the packed check button states
	*(storage+5) = (long)pkdBtns;				// low order byte has the lowest address

		// save 34 bytes worth of 'long' to the registry
	const wchar_t* key = _T("Software\\PocketNumerix\\NillaHedge\\VolatilityExplorer");
	bool	b0 = WriteRegBinary( HKEY_CURRENT_USER, key, (BYTE*)storage, 5*sizeof(long)+sizeof(char) );
	return	b0;
}			// SetVolatilityExplorerState()
//----------------------------------------------------------------------------
bool	GetRecentOption( long* def_ID )							// failure --> false
{
	const wchar_t* key = _T("SOFTWARE\\PocketNumerix\\NillaHedge\\RecentOption");
	return	ReadRegLong( HKEY_CURRENT_USER, key, def_ID );
}
//-----------------------------------------------------------------------------------------
bool	SetRecentOption( long def_ID )							// failure --> false
{
	const wchar_t* key = _T("SOFTWARE\\PocketNumerix\\NillaHedge\\RecentOption");
	return	WriteRegLong( HKEY_CURRENT_USER, key, def_ID );
}
//-----------------------------------------------------------------------------------------
bool	GetRecentStock( long* def_ID )							// failure --> false
{
	const wchar_t* key = _T("SOFTWARE\\PocketNumerix\\NillaHedge\\RecentStock");
	return	ReadRegLong( HKEY_CURRENT_USER, key, def_ID );
}
//-----------------------------------------------------------------------------------------
bool	SetRecentStock( long def_ID )							// failure --> false
{
	const wchar_t* key = _T("SOFTWARE\\PocketNumerix\\NillaHedge\\RecentStock");
	return	WriteRegLong( HKEY_CURRENT_USER, key, def_ID );
}
//-----------------------------------------------------------------------------------------
bool	GetRiskFreeRate( float* rfr )
{
	const wchar_t* key = _T("SOFTWARE\\PocketNumerix\\RiskFreeRate");
	return	ReadRegFloat( HKEY_CURRENT_USER, key, rfr );
}			// GetRiskFreeRate()
//-----------------------------------------------------------------------------------------
bool	SetRiskFreeRate( float rfr )
{
	const wchar_t* key = _T("SOFTWARE\\PocketNumerix\\RiskFreeRate");
	return	WriteRegFloat( HKEY_CURRENT_USER, key, rfr );
}			// SetRiskFreeRate()
//-----------------------------------------------------------------------------------------

bool	GetSaveVerifyPrefs( short* prefs )							// was short&
{
	const wchar_t* key = _T("SOFTWARE\\PocketNumerix\\NillaHedge\\SaveVerifyPrefs");
	return	ReadRegShort( HKEY_CURRENT_USER, key, prefs );
}
//-----------------------------------------------------------------------------------------
bool	SetSaveVerifyPrefs( short prefs )
{
	const wchar_t* key = _T("SOFTWARE\\PocketNumerix\\NillaHedge\\SaveVerifyPrefs");
	return	WriteRegShort( HKEY_CURRENT_USER, key, prefs );
}
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
bool	GetPosListColVisibility( long* areVisible )
{
	const wchar_t* key = _T("SOFTWARE\\PocketNumerix\\NillaHedge\\ColumnVisibility");
	return	ReadRegLong( HKEY_CURRENT_USER, key, areVisible );
}			// GetPosListColVisibility()
//-----------------------------------------------------------------------------------------
bool	SetPosListColVisibility( long areVisible )
{
	const wchar_t* key = _T("SOFTWARE\\PocketNumerix\\NillaHedge\\ColumnVisibility");
	return	WriteRegLong( HKEY_CURRENT_USER, key, areVisible );
}			// SetPosListColVisibility()
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
inline bool	ReadRegShort( HKEY hKey, const wchar_t* pcszKey, short* value )
{
	return	ReadReg( hKey, pcszKey, REG_BINARY, (BYTE*)value, sizeof(short) );
}			// ReadRegShort()
//-----------------------------------------------------------------------------------------
inline bool	WriteRegShort( HKEY hKey, const wchar_t* pcszKey, short value )
{
	return	WriteReg( hKey, pcszKey, REG_BINARY, (BYTE*)&value, sizeof(short) );
}			// ReadRegShort()
//-----------------------------------------------------------------------------------------
inline bool	ReadRegLong( HKEY hKey, const wchar_t* pcszKey, long* value )
{
	return	ReadReg( hKey, pcszKey, REG_BINARY, (BYTE*)value, sizeof(long) );
}			// ReadRegLong()
//-----------------------------------------------------------------------------------------
inline bool	WriteRegLong( HKEY hKey, const wchar_t* pcszKey, long value )
{
	return	WriteReg( hKey, pcszKey, REG_BINARY, (BYTE*)&value, sizeof(long) );
}			// ReadRegLong()
//-----------------------------------------------------------------------------------------
inline bool	ReadRegDWORD( HKEY hKey, const wchar_t* pcszKey, DWORD* value )
{
	return	ReadReg( hKey, pcszKey, REG_DWORD, (BYTE*)value, sizeof(long) );
}			// ReadRegDWORD()
//-----------------------------------------------------------------------------------------
inline bool	WriteRegDWORD( HKEY hKey, const wchar_t* pcszKey, DWORD value )
{
	return	WriteReg( hKey, pcszKey, REG_DWORD, (BYTE*)&value, sizeof(long) );
}			// WriteRegDWORD()
//-----------------------------------------------------------------------------------------
inline bool	ReadRegFloat( HKEY hKey, const wchar_t* pcszKey, float* value )
{
	return	ReadReg( hKey, pcszKey, REG_BINARY, (BYTE*)value, sizeof(float) );
}			// ReadRegFloat()
//-----------------------------------------------------------------------------------------
inline bool	WriteRegFloat( HKEY hKey, const wchar_t* pcszKey, float value )
{
	return	WriteReg( hKey, pcszKey, REG_BINARY, (BYTE*)&value, sizeof(float) );
}			// ReadRegFloat()
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
bool	ReadRegBinary( HKEY hKey, const wchar_t* pcszKey, BYTE** value, DWORD* nBytes )
{		// allocates space for value
	return	ReadReg( hKey, pcszKey, REG_BINARY, value, nBytes );
}			// ReadRegBinary()
//-----------------------------------------------------------------------------------------
bool	ReadRegBinary( HKEY hKey, const wchar_t* pcszKey, BYTE* value, DWORD nBytes )
{		// expects value to already be allocated
	return	ReadReg( hKey, pcszKey, REG_BINARY, value, nBytes );
}			// ReadRegBinary()
//-----------------------------------------------------------------------------------------
bool	WriteRegBinary( HKEY hKey, const wchar_t* pcszKey, BYTE* value, DWORD nBytes )
{		// expects space for value to be allocated already
	return	WriteReg( hKey, pcszKey, REG_BINARY, value, nBytes );
}			// WriteRegBinary()
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
bool	ReadRegString( HKEY hKey, const wchar_t* pcszKey, BYTE** value, DWORD* nBytes )
{		// allocates space for value
	return	ReadReg( hKey, pcszKey, REG_SZ, value, nBytes );
}			// ReadRegString()
//-----------------------------------------------------------------------------------------
bool	ReadRegString( HKEY hKey, const wchar_t* pcszKey, BYTE* value, DWORD nBytes )
{		// expects value to already be allocated
	return	ReadReg( hKey, pcszKey, REG_SZ, value, nBytes );
}			// ReadRegString()
//-----------------------------------------------------------------------------------------
bool	WriteRegString( HKEY hKey, const wchar_t* pcszKey, BYTE* value, DWORD nBytes )
{		// expects space for value to be allocated already
	return	WriteReg( hKey, pcszKey, REG_SZ, value, nBytes );
}			// WriteRegString()
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
bool	ReadReg( HKEY hKey, const wchar_t* pcszKey, DWORD type, BYTE** value, DWORD* nBytes )
{		// this ReadReg() allocates the space required for value
	ASSERT ( AfxIsValidString(pcszKey) );
#ifdef _DEBUG
	TRACE( _T("ReadReg: REG_SZ=%d, type=%d, key=%s\n"), REG_SZ, type, pcszKey );
#endif
	bool	retVal = false;
	bool	isString = ( REG_SZ == type );
	HKEY	subkey = NULL;

		// break the key name into prefix and suffix
	CString	cs = pcszKey;
	int		lastBackslash = cs.ReverseFind( _T('\\') );
	CString prefix = cs.Left( lastBackslash );		// everything to the left of the last backslash
	CString suffix = cs.Mid( lastBackslash + 1 );	// everything to the right of the last backslash

	LONG	err = RegOpenKeyEx( hKey, prefix, 0, 0, &subkey );
#ifdef _DEBUG
	TRACE( _T("ReadReg: RegOpenKeyEx(%s): err=%d (should be zero).\n"), pcszKey, err );
#endif
	if ( err != ERROR_SUCCESS )
		goto	Exit;

	err = RegQueryInfoKey( subkey, NULL, NULL, NULL, NULL, NULL, NULL,
								   NULL, NULL, nBytes, NULL, NULL );
#ifdef _DEBUG
	TRACE( _T("ReadReg: RegQueryInfoKey(%s): err=%d (should be zero), *nBytes=%d\n"), pcszKey, err, *nBytes );
#endif
	if ( err != ERROR_SUCCESS )
		goto	Exit;

	*value = new BYTE[ *nBytes ];
		//				    HKEY  LPCWSTR  0  DWORD*  BYTE*  DWORD*
	err = RegQueryValueEx( subkey, suffix, 0, &type, *value, nBytes );
#ifdef _DEBUG
	if ( err != ERROR_SUCCESS )
	{		// this frequently happens with err = 87, meaning 'Operation completed successfully'
		wchar_t	buf[384];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, buf, 384, NULL );
		TRACE( _T("ReadReg: RegQueryValueEx(%s): %d --> %s.\n"), pcszKey, err, buf );
	}
#endif
	retVal = ( err == ERROR_SUCCESS );
Exit:
	RegCloseKey( subkey );
	RegCloseKey( hKey );
	return	retVal;	
}			// ReadReg()
//-----------------------------------------------------------------------------------------
bool	ReadReg( HKEY hKey, const wchar_t* pcszKey, DWORD type, BYTE* value, DWORD nBytes )
{		// this ReadReg() assumes that nBytes of space for value is already allocated
	ASSERT ( AfxIsValidString(pcszKey) );
	bool	retVal = false;
//	bool	isString = ( REG_SZ == type );
	HKEY	subkey = NULL;

		// break the key name into prefix and suffix
	CString	cs = pcszKey;
	int		lastBackslash = cs.ReverseFind( _T('\\') );
	CString prefix = cs.Left( lastBackslash );		// everything to the left of the last backslash
	CString suffix = cs.Mid( lastBackslash + 1 );	// everything to the right of the last backslash

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
bool	WriteReg( HKEY hKey, const wchar_t* pcszKey, DWORD type, BYTE* value, DWORD nBytes )
{	HKEY	subkey = NULL;
	ASSERT ( AfxIsValidString(pcszKey) );
	//	RegOpenKeyEx( HKEY_CURRENT_USER, pcszKey, 0, KEY_SET_VALUE, &hKey );
//	LPWSTR	cls = _T("float");
	bool	retVal = false;
	DWORD	disp;
	CString	cs = pcszKey;
	int	lastBackslash = cs.ReverseFind( _T('\\') );
	CString prefix = cs.Left( lastBackslash );				// everything to the left of the last backslash
	CString suffix = cs.Mid( lastBackslash+1 );				// everything to the right of the last backslash

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
/*
bool	ReadRegBinary( HKEY hKey, LPCTSTR pcszKey, short nBytes, void* outBuf )
{
	ASSERT ( AfxIsValidString(pcszKey) );
	LONG	err = RegOpenKeyEx( hKey, pcszKey, 0, KEY_QUERY_VALUE, &hKey );
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
bool	WriteRegBinary( HKEY hKey, LPCTSTR pcszKey, short nBytes, void* inBuf )
{
	ASSERT ( AfxIsValidString(pcszKey) );;
//	RegOpenKeyEx( hKey, pcszKey, 0, KEY_SET_VALUE, &hKey );
	LPWSTR	cls = _T("binary");
	DWORD	disp;

	LONG	err = RegCreateKeyEx( hKey, pcszKey, 0, cls, 0, 0, 0, &hKey, &disp );
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
bool	ReadRegLong( HKEY hKey, LPCTSTR pcszKey, long& dwValue )
{
	ASSERT ( AfxIsValidString(pcszKey) );
	LONG	err = RegOpenKeyEx( hKey, pcszKey, 0, KEY_QUERY_VALUE, &hKey );
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
bool	WriteRegLong( HKEY hKey, LPCTSTR pcszKey, long dwValue )
{
	ASSERT ( AfxIsValidString(pcszKey) );
//	RegOpenKeyEx( hKey, pcszKey, 0, KEY_SET_VALUE, &hKey );
	LPWSTR	cls = _T("long");
	DWORD	disp;

	LONG	err = RegCreateKeyEx( hKey, pcszKey, 0, cls, 0, 0, 0, &hKey, &disp );
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
bool	ReadRegFloat( HKEY hKey, LPCTSTR pcszKey, float& fValue )
{
	ASSERT ( AfxIsValidString(pcszKey) );
	LONG	err = RegOpenKeyEx( hKey, pcszKey, 0, KEY_QUERY_VALUE, &hKey );
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
bool	WriteRegFloat( HKEY hKey, LPCTSTR pcszKey, float fValue )
{
	ASSERT ( AfxIsValidString(pcszKey) );
//	RegOpenKeyEx( hKey, pcszKey, 0, KEY_SET_VALUE, &hKey );
	LPWSTR	cls = _T("float");
	DWORD	disp;

	LONG	err = RegCreateKeyEx( hKey, pcszKey, 0, cls, 0, 0, 0, &hKey, &disp );
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
bool	ReadRegShort( HKEY hKey, LPCTSTR pcszKey, short& sValue )
{
	ASSERT ( AfxIsValidString(pcszKey) );
	LONG	err = RegOpenKeyEx( hKey, pcszKey, 0, KEY_QUERY_VALUE, &hKey );
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
bool	WriteRegShort( HKEY hKey, LPCTSTR pcszKey, short sValue )
{
	ASSERT ( AfxIsValidString(pcszKey) );
//	RegOpenKeyEx( hKey, pcszKey, 0, KEY_SET_VALUE, &hKey );
	LPWSTR	cls = _T("short");
	DWORD	disp;

	LONG	err = RegCreateKeyEx( hKey, pcszKey, 0, cls, 0, 0, 0, &hKey, &disp );
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
*/
//-----------------------------------------------------------------------------------------------
