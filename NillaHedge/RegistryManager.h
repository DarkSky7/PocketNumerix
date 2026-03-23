// RegistryManager.h: interface for the CRegistryManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGISTRYMANAGER_H__9B7F52A8_EA9C_43DA_9B21_6AFC121AEF0E__INCLUDED_)
#define AFX_REGISTRYMANAGER_H__9B7F52A8_EA9C_43DA_9B21_6AFC121AEF0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AssetType.h"
#include "OneStk3OptStrategy.h"
#include "RateSensitivity.h"
//#include "BondRateSensitivity.h"
#include "RegistryManager.h"
#include "TimeDecay.h"
#include "VolatilityExplorerState.h"

		// Workhorse Registry access functions
	bool	ReadRegShort( HKEY hKey, const wchar_t* pcszKey, short* sValue );	// was short&
	bool	WriteRegShort( HKEY hKey, const wchar_t* pcszKey, short sValue );

	bool	ReadRegDWORD( HKEY hKey, const wchar_t* pcszKey, DWORD* dwValue );	// was long&
	bool	WriteRegDWORD( HKEY hKey, const wchar_t* pcszKey, DWORD dwValue );

	bool	ReadRegFloat( HKEY hKey, const wchar_t* pcszKey, float* fValue );	// was float&
	bool	WriteRegFloat( HKEY hKey, const wchar_t* pcszKey, float fValue );

	bool	ReadRegLong( HKEY hKey, const wchar_t* pcszKey, long* lValue );		// was long&
	bool	WriteRegLong( HKEY hKey, const wchar_t* pcszKey, long lValue );

	bool	GetSocketSendWait( DWORD* milliseconds );
	bool	GetSocketRecvWait( DWORD* milliseconds );
	bool	GetOCRServer( BYTE** serverName, DWORD* nBytes );
	bool	GetOCRPage( BYTE** pageName, DWORD* nBytes );

		// the first ReadRegBinary only sets the value (expecting the caller to have allocated space)
	bool	ReadRegBinary( HKEY hKey, const wchar_t* pcszKey, BYTE* inBuf, DWORD nBytes );
		// the second ReadRegBinary also allocates memory for value
	bool	ReadRegBinary( HKEY hKey, const wchar_t* pcszKey, BYTE** inBuf, DWORD* nBytes );
	bool	WriteRegBinary( HKEY hKey, const wchar_t* pcszKey, BYTE* outBuf, DWORD nBytes );

		// the first ReadRegString only sets the value (expecting the caller to have allocated space)
	bool	ReadRegString( HKEY hKey, const wchar_t* pcszKey, BYTE* inBuf, DWORD nBytes );
		// the second ReadRegString also allocates memory for value
	bool	ReadRegString( HKEY hKey, const wchar_t* pcszKey, BYTE** inBuf, DWORD* nBytes );
	bool	WriteRegString( HKEY hKey, const wchar_t* pcszKey, BYTE* outBuf, DWORD nBytes );

		// the first ReadReg only sets the value (expecting the caller to have allocated space)
	bool	ReadReg( HKEY hKey, const wchar_t* pcszKey, DWORD type, BYTE* value, DWORD nBytes );
		// the second ReadReg also allocates memory for value
	bool	ReadReg( HKEY hKey, const wchar_t* pcszKey, DWORD type, BYTE** value, DWORD* nBytes );
	bool	WriteReg( HKEY hKey, const wchar_t* pcszKey, DWORD type, BYTE* value, DWORD nBytes );

	bool	GetRecentInstrument( AssetType* at );
	bool	SetRecentInstrument( AssetType at );

	bool	GetRecentOption( long* def_ID );
	bool	SetRecentOption( long def_ID );

	bool	GetRecentStock( long* def_ID );
	bool	SetRecentStock( long def_ID );

	bool	GetHedge( OneStk3OptHedge* os3oh );
	bool	SetHedge( OneStk3OptHedge os3oh );

		// RiskFreeRate
	bool	GetRiskFreeRate( float* rfr );
	bool	SetRiskFreeRate( float rfr );

		// RateSensitivityExplorer State
	bool	GetOptionRateSensitivity( OptionRateSensitivity* rateSens );
	bool	SetOptionRateSensitivity( OptionRateSensitivity rateSens );

		// TimeDecayExplorer State
	bool	GetTimeDecay( TimeDecay* timeDecay );
	bool	SetTimeDecay( TimeDecay timeDecay );

		// VolatilityExplorer
	bool	GetVolatilityExplorerState( VolatilityExplorerState* volExpState );
	bool	SetVolatilityExplorerState( VolatilityExplorerState volExpState );

		// GUI preferences ...
		// stored in the registry under <HKEY_CURRENT_USER...\\ColumnVisibility
	bool	GetPosListColVisibility( long* areVisible );				// was long& ... failure --> false
	bool	SetPosListColVisibility( long areVisible );					// failure --> false

		// stored in the registry under <HKEY_CURRENT_USER...\\<colName>\\Width
	bool	GetPosListColWidth( LPCTSTR colName, long* pixels );		// was long& ... failure --> false
	bool	SetPosListColWidth( LPCTSTR colName, long pixels );			// failure --> false

		// SaveVerifyPrefs
	bool	GetSaveVerifyPrefs( short* prefs );							// was short&
	bool	SetSaveVerifyPrefs( short prefs );

//	bool	GetDeltaYTM( float* deltaYTM );								// was float&
//	bool	SetDeltaYTM( float deltaYTM );

//	bool	GetRecentBond( long* def_ID );							// was long& ... failure --> false
//	bool	SetRecentBond( long def_ID );							// failure --> false

//	bool	ReadRegDouble( HKEY hKey, const wchar_t* pcszKey, double& qwValue );
//	bool	WriteRegDouble( HKEY hKey, const wchar_t* pcszKey, double qwValue );

		// RateSensitivityExplorer State (only in BondManager
//	bool	GetBondRateSensitivity( BondRateSensitivity* rateSens );	// was BondRateSensitivity&
//	bool	SetBondRateSensitivity( BondRateSensitivity rateSens );

#endif // !defined(AFX_REGISTRYMANAGER_H__9B7F52A8_EA9C_43DA_9B21_6AFC121AEF0E__INCLUDED_)
