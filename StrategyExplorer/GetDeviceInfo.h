// GetSerialNumber.cpp
#include <Afx.h>					// CString
#if _WIN32_WCE > 0x500
#include <GetDeviceUniqueId.h>		// for GetDeviceUniqueID() on WM5 (to obtain HUIDs)
#endif

CString	GetOsVersion( void );
CString	GetSerialNumber( void );
char*	MakeHexString( BYTE* uuid, DWORD& len );
short	GetDeviceInfo( unsigned long& dv, unsigned char*& uu, unsigned char*& hu, char*& na, char*& em	);

/*
		// usage:
		// UUID...
	CString	serialNo_CSt = GetSerialNumber();
	char*	thinSerialNo = GetThinString( serialNo_CSt );		// ANSI UUID

		// HUID...

		// the pdoduct hash keys used as in GetDeviceUniqueID() below
	pktNumerixBondManager
	pcktNumerixNillaHedge
	pknmxStrategyExplorer
	pknmxYieldCurveFitter

	BYTE	uuid[48];
	DWORD	lenOut = ( tokLen < 47 )  ?  tokLen  :  47;
	HRESULT hr = GetDeviceUniqueID( hashKey, lenOut, 1, uuid, &lenOut );

	if ( uuidHex != NULL ) delete [] uuidHex;		// try not to clobber uuidHex
		// HEX ANSI  <--  BYTE* (actually a very long integer)
	uuidHex = MakeHexString( uuid, lenOut );		// uuidHex is little-endian reversed


*/