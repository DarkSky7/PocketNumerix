// Registry.h
#include "ServerRates.h"

	// save state support
//bool	RegisterKeyValuePair( HKEY nKey, wchar_t* subkey, DWORD type, void* value, unsigned short nBytes );

bool		ReadReg( HKEY hKey, wchar_t* pcszKey, DWORD type, BYTE* value, DWORD nBytes );
bool		ReadReg( HKEY hKey, wchar_t* pcszKey, DWORD type, BYTE** value, DWORD* nByes );	// allocates space
bool		ReadRegBinary( HKEY hKey, wchar_t* pcszKey, BYTE* value, DWORD nBytes );
bool		ReadRegBinary( HKEY hKey, wchar_t* pcszKey, BYTE** value, DWORD* nBytes );		// allocates space
bool		ReadRegString( HKEY hKey, wchar_t* pcszKey, BYTE** value, DWORD* nBytes );					// allocates space
bool		ReadRegDWORD( HKEY hKey, wchar_t* pcszKey, DWORD* value );
//bool		ReadRegFloat( HKEY hKey, wchar_t* pcszKey, float& value );
//------------------------------------------------------
bool		WriteReg( HKEY hKey, wchar_t* pcszKey, DWORD type, BYTE* value, DWORD nBytes );
bool		WriteRegBinary( HKEY hKey, wchar_t* pcszKey, BYTE* value, DWORD nBytes );
bool		WriteRegString( HKEY hKey, wchar_t* pcszKey, BYTE* value );
bool		WriteRegFloat( HKEY hKey, wchar_t* pcszKey, float value );
//------------------------------------------------------
bool		GetServerRates( ServerRates* sr );
bool		SetServerRates( ServerRates sr );
//------------------------------------------------------
bool		GetSocketRecvWait( DWORD* milliseconds );
bool		GetSocketSendWait( DWORD* milliseconds );
//------------------------------------------------------
bool		GetTBRServer( BYTE** serverName, DWORD* slen );
bool		GetTBRPage( BYTE** pageName, DWORD* plen );

