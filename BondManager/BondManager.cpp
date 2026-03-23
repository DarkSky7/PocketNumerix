// BondManager.cpp: implementation of the CBondManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BondManagerApp.h"
#include "BondManager.h"
#include "DBFile.h"
//#include "BOS.h"
#include "Bond.h"
#include "utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBondManager::CBondManager( CString subDir ) : CDataManager(subDir)
{
}
//----------------------------------------------------------------------------
CBondManager::~CBondManager()
{
}
//----------------------------------------------------------------------------
CBond*		CBondManager::GetIndexedBond( long zbi )
{
	CDBFile*	posIndxFile = GetPosIndxFile();
	if ( posIndxFile == NULL )
		return	NULL;
	long		def_ID = GetIndexedDef_ID( zbi );
	return ( def_ID >= 0 )  ?  ReadBond( def_ID )  :  NULL;
}			// GetIndexedBond()
//----------------------------------------------------------------------------
CBond*		CBondManager::GetBond( CString symbol )
{		// fetch bond definition from offset bytes into the bondDefFile
	long	offset = AssetExists( symbol );
	if ( offset < 0 )
		return	NULL;

	CBond* bnd = ReadBond( offset );
	bnd->setSymbol( symbol );
	return	bnd;
}			// GetBond()
//----------------------------------------------------------------------------
CBond*		CBondManager::ReadBond( long offset )
{		// fetch bond definition from offset bytes into the bondDefFile
	float	parValue;
	float	couponRate;
	char	couponsPerYear;
	char	dayCounting;
	long	packedIncep;				// only 3 bytes of data here
	long	packedMaturity;				// only 3 bytes of data here

	CDBFile*	defFile = GetDefFile();
	if ( defFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("BondManager::ReadBond:  Error(-1) getting the bondDef file.\n") );
#endif
		return	NULL;
	}

		// read the bond definition
		// start with openPosOffset, symbol_ID, desc_ID & mktPrice
	CBOS*	bos = ReadBOS( offset );
	if (	 bos					== NULL																//  0-19
		 ||  sizeof(parValue)		!= defFile->Read( &parValue,		   sizeof(parValue)		  )		// 20-23
		 ||  sizeof(couponRate)		!= defFile->Read( &couponRate,		   sizeof(couponRate)	  )		// 24-27
		 ||  sizeof(couponsPerYear)	!= defFile->Read( &couponsPerYear,	   sizeof(couponsPerYear) )		// 28
		 ||  1						!= defFile->Read( &dayCounting,		   1					  )		// 29
		 ||  3						!= defFile->Read( &packedIncep,		   3					  )		// 30-32
		 ||  3						!= defFile->Read( &packedMaturity,	   3					  )	)	// 33-35
	{
#ifdef _DEBUG
		TRACE( _T("BondManager::ReadBond:  Error (-2) reading the bondDef file.\n") );
#endif
		delete	bos;
		return	NULL;
	}

		// build an in-memory CBond for the stored data
	CBond*	theBond = new CBond( *bos );
	delete	bos;
	theBond->setParValue( parValue );
	theBond->setCouponRate( couponRate );
	theBond->setCouponsPerYear( couponsPerYear );
	theBond->setDayCounting( (DayCountMethod)(dayCounting & 0xFF) );	// clean up the Read
	theBond->setIncepDate( UnpackOleDate( packedIncep ) );
	theBond->setMaturityDate( UnpackOleDate( packedMaturity ) );
	return	theBond;
}			// ReadBond()
//----------------------------------------------------------------------------
short		CBondManager::WriteBond( CBond* theBond )
{		// see if we can open the defs file
	CDBFile*	defFile = GetDefFile();
	if ( defFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("BondManager::WriteBond: Error(-1) getting the bondDef file.\n") );
#endif
		return	-1;
	}
		// write the bond definition back to the file system (object store)
		// start with openPosOffset, symbol_ID, desc_ID, mktPrice & posIndx_ID
	short res = WriteBOS( (CBOS*)theBond );														//  0-19
	if ( res != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("BondManager::WriteBond: Error(-2) writing BOS content to the bondDef file, res = %d\n"), res );
#endif
		return	-2;
	}

		// now for the rest of the fields
	long	packedIncep		= PackOleDate( theBond->getIncepDate() );
	long	packedMaturity	= PackOleDate( theBond->getMaturityDate() );
//	long	packedToday		= PackOleDate( COleDateTime::GetCurrentTime() );
	float	parValue		= theBond->getParValue();
	float	cpnRate			= theBond->getCouponRate();
	char	cpy				= (char)theBond->getCouponsPerYear();
	char	dayCntg			= (char)theBond->getDayCounting();
#if ( SEH_EXCEPTIONS )
	__try
#else
	try
#endif
	{	defFile->Write( &parValue,		 sizeof(parValue)		);	// 20-23
		defFile->Write( &cpnRate,		 sizeof(cpnRate)		);	// 24-27
		defFile->Write( &cpy,			 sizeof(cpy)			);	// 28
		defFile->Write( &dayCntg,		 sizeof(dayCntg)		);	// 29
		defFile->Write( &packedIncep,	 3						);	// 30-32
		defFile->Write( &packedMaturity, 3						);	// 33-35
//		defFile->Write( &packedToday,	 3						);	// 36-38	 last modified date
	}
#if ( SEH_EXCEPTIONS )
	__except ( EXCEPTION_EXECUTE_HANDLER )
#else
	catch ( CException& ex )
#endif
	{
#ifdef _DEBUG
		TRACE( _T("BondManager::WriteBond: Error(-3) writing to the bondDef file.\n"));
#endif
		return	-3;
	}
	defFile->Flush();
	return	0;
}			// WriteBond()
//----------------------------------------------------------------------------
