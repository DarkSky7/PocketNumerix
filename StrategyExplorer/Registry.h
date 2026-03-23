// Registry.h
	bool	WriteRegShort( HKEY hKey, wchar_t* pcszKey, short  value );
	bool	ReadRegShort ( HKEY hKey, wchar_t* pcszKey, short* value );

	bool	WriteRegBinary( HKEY hKey, wchar_t* pcszKey, BYTE*  value, DWORD  nBytes );
	bool	ReadRegBinary ( HKEY hKey, wchar_t* pcszKey, BYTE*  value, DWORD  nBytes );
	bool	ReadRegBinary ( HKEY hKey, wchar_t* pcszKey, BYTE** value, DWORD* nBytes );

	bool	WriteReg( HKEY hKey, wchar_t* pcszKey, DWORD type, BYTE*  value, DWORD  nBytes );
	bool	ReadReg ( HKEY hKey, wchar_t* pcszKey, DWORD type, BYTE** value, DWORD* nBytes );
	bool	ReadReg ( HKEY hKey, wchar_t* pcszKey, DWORD type, BYTE*  value, DWORD  nBytes );
