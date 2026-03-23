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
#include "TimeDecay.h"
#include "VolatilityExplorerState.h"

class CRegistryManager  
{
public:
	CRegistryManager();
	virtual ~CRegistryManager();

		// Risk Free Rate
	bool		GetRiskFreeRate( float& rfr )
				{	return	ReadRegFloat( HKEY_CURRENT_USER, 
								_T("SOFTWARE\\PocketNumerix\\RiskFreeRate"), rfr );
				}
	bool		SetRiskFreeRate( float rfr )
				{	return	WriteRegFloat( HKEY_CURRENT_USER, 
								_T("SOFTWARE\\PocketNumerix\\RiskFreeRate"), rfr );
				}

		// SaveVerifyPrefs
	bool		GetSaveVerifyPrefs( short& prefs )
				{	return	ReadRegShort( HKEY_CURRENT_USER, 
								_T("SOFTWARE\\PocketNumerix\\NillaHedge\\SaveVerifyPrefs"), prefs );
				}
	bool		SetSaveVerifyPrefs( short prefs )
				{	return	WriteRegShort( HKEY_CURRENT_USER, 
								_T("SOFTWARE\\PocketNumerix\\NillaHedge\\SaveVerifyPrefs"), prefs );
				}

		// DeltaYTM
/*
	bool		GetDeltaYTM( float& deltaYTM )
				{	return	ReadRegFloat( 
								_T("SOFTWARE\\PocketNumerix\\NillaHedge\\DeltaYTM"), deltaYTM );
				}
	bool		SetDeltaYTM( float deltaYTM )
				{	return	WriteRegFloat( 
								_T("SOFTWARE\\PocketNumerix\\NillaHedge\\DeltaYTM"), deltaYTM );
				}
*/
		// Socket Send and Recv Wait Times
	bool		GetSocketRecvWait( unsigned long& milliseconds );
	bool		GetSocketSendWait( unsigned long& milliseconds );
	bool		GetOCRServer( char*& serverName );

		// HedgeExplorer State
	bool		GetHedge( OneStk3OptHedge& os3oh );
	bool		SetHedge( OneStk3OptHedge& os3oh );

		// RateSensitivityExplorer State
	bool		GetOptionRateSensitivity( OptionRateSensitivity& rateSens );
	bool		SetOptionRateSensitivity( OptionRateSensitivity& rateSens );

		// TimeDecayExplorer State
	bool		GetTimeDecay( TimeDecay& timeDecay );
	bool		SetTimeDecay( TimeDecay& timeDecay );

		// VolatilityExplorer State
	bool		GetVolatilityExplorerState( VolatilityExplorerState& volExpState );
	bool		SetVolatilityExplorerState( VolatilityExplorerState& volExpState );

		// GUI preferences ...
		// stored in the registry under <HKEY_CURRENT_USER...\\ColumnVisibility
	bool		GetPosListColVisibility( long& areVisible );				// failure --> false
	bool		SetPosListColVisibility( long areVisible );					// failure --> false

		// stored in the registry under <HKEY_CURRENT_USER...\\<colName>\\Width
	bool		GetPosListColWidth( LPCTSTR colName, long& pixels );		// failure --> false
	bool		SetPosListColWidth( LPCTSTR colName, long pixels );			// failure --> false

		// PositionsDialog initial instrument (radio button selection criteria)
		// stored in the registry under <HKEY_CURRENT_USER...\\PosListInitialInstrument
	bool		GetRecentInstrument( AssetType& at )					// failure --> false
				{	return	ReadRegShort( HKEY_CURRENT_USER, 
								_T("SOFTWARE\\PocketNumerix\\NillaHedge\\RecentInstrument"),
								(short&)at );
				}
	bool		SetRecentInstrument( AssetType at )						// failure --> false
				{	return	WriteRegShort( HKEY_CURRENT_USER, 
								_T("SOFTWARE\\PocketNumerix\\NillaHedge\\RecentInstrument"),
								(short)at );
				}

		// PositionsDialog, Definitions dialogs, Analyzer dialogs, 
/*
	bool		GetRecentBond( long& def_ID )							// failure --> false
				{	return	ReadRegLong( 
								_T("SOFTWARE\\PocketNumerix\\NillaHedge\\RecentBond"),
								def_ID );
				}
	bool		SetRecentBond( long def_ID )							// failure --> false
				{	return	WriteRegLong( 
								_T("SOFTWARE\\PocketNumerix\\NillaHedge\\RecentBond"),
								def_ID );
				}
*/
	bool		GetRecentOption( long& def_ID )							// failure --> false
				{	return	ReadRegLong( HKEY_CURRENT_USER, 
								_T("SOFTWARE\\PocketNumerix\\NillaHedge\\RecentOption"),
								def_ID );
				}
	bool		SetRecentOption( long def_ID )							// failure --> false
				{	return	WriteRegLong( HKEY_CURRENT_USER, 
								_T("SOFTWARE\\PocketNumerix\\NillaHedge\\RecentOption"),
								def_ID );
				}
	bool		GetRecentStock( long& def_ID )							// failure --> false
				{	return	ReadRegLong( HKEY_CURRENT_USER, 
								_T("SOFTWARE\\PocketNumerix\\NillaHedge\\RecentStock"),
								def_ID );
				}
	bool		SetRecentStock( long def_ID )							// failure --> false
				{	return	WriteRegLong( HKEY_CURRENT_USER, 
								_T("SOFTWARE\\PocketNumerix\\NillaHedge\\RecentStock"),
								def_ID );
				}

		// Workhorse Registry access functions
	bool		ReadRegBinary( HKEY hKey, LPCTSTR pcszKey, short nBytes, void* inBuf );
	bool		WriteRegBinary( HKEY hKey, LPCTSTR pcszKey, short nBytes, void* outBuf );

	bool		ReadRegLong( HKEY hKey, LPCTSTR pcszKey, long& dwValue );
	bool		WriteRegLong( HKEY hKey, LPCTSTR pcszKey, long dwValue );

	bool		ReadRegFloat( HKEY hKey, LPCTSTR pcszKey, float& fValue );
	bool		WriteRegFloat( HKEY hKey, LPCTSTR pcszKey, float fValue );

	bool		ReadRegShort( HKEY hKey, LPCTSTR pcszKey, short& sValue );
	bool		WriteRegShort( HKEY hKey, LPCTSTR pcszKey, short sValue );
/*
	bool		ReadRegString( LPCTSTR pcszKey, CString pcszValue );
	bool		WriteRegString( LPCTSTR pcszKey, CString pcszValue );

	bool		ReadRegDouble( LPCTSTR pcszKey, double& qwValue );
	bool		WriteRegDouble( LPCTSTR pcszKey, double qwValue );
*/
};

#endif // !defined(AFX_REGISTRYMANAGER_H__9B7F52A8_EA9C_43DA_9B21_6AFC121AEF0E__INCLUDED_)
