// StockManager.cpp: implementation of the CStockManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "NillaHedge.h"
#include "StockManager.h"
#include "DBFile.h"

#if ( ! SEH_EXCEPTIONS )
#include <exception>
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStockManager::CStockManager( CString subDir ) : CDataManager(subDir)
{

}
//----------------------------------------------------------------------------
CStockManager::~CStockManager()
{

}
//----------------------------------------------------------------------------
CStock*		CStockManager::GetPosIndexedStock( long zbi )
{
	CDBFile*	posIndxFile = GetPosIndxFile();
	if ( posIndxFile == NULL )
		return	NULL;
	long		def_ID = GetPosIndexedDef_ID( zbi );
	return ( def_ID >= 0 )  ?  ReadStock( def_ID )  :  NULL;
}			// GetPosIndexedStock()
//----------------------------------------------------------------------------
CStock*		CStockManager::GetStock( CString symbol )
{		// fetch stock definition from offset bytes into the stockDefFile
	long	offset = AssetExists( symbol );
	if ( offset < 0 )
		return	NULL;

		// mruStock updated in readStock() 
	CStock* stk = ReadStock( offset );
	stk->setSymbol( symbol );
	return	stk;
}			// GetStock()
//----------------------------------------------------------------------------
CStock*		CStockManager::ReadStock( long offset )
{		// fetch stock definition from offset bytes into the stockDefFile	
	float					volatility;
	float					dividends[4];
	register unsigned long	divDates0_3;
	unsigned short			divDates4;		// actually, only the low order byte is worth reading/writing
//	long					packedModifiedDate;
	CDBFile*	defFile = GetDefFile();
	if ( defFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("StockManager::writeOption:  Error(-1) getting the stockDef file.\n") );
#endif
		return	NULL;
	}
		// read the stock definition
		// start with openPosOffset, symbol_ID, desc_ID & mktPrice
	CBOS*	 bos = ReadBOS( offset );																//  0-19
	if (	 bos				  == NULL
		 ||  sizeof(volatility)   != defFile->Read( &volatility,		 sizeof(volatility)   )		// 20-23
		 ||  sizeof(dividends[0]) != defFile->Read( &dividends[0],		 sizeof(dividends[0]) )		// 24-27
		 ||  sizeof(dividends[1]) != defFile->Read( &dividends[1],		 sizeof(dividends[1]) )		// 28-31
		 ||  sizeof(dividends[2]) != defFile->Read( &dividends[2],		 sizeof(dividends[2]) )		// 32-35
		 ||  sizeof(dividends[3]) != defFile->Read( &dividends[3],		 sizeof(dividends[3]) )		// 36-39
		 ||  sizeof(divDates0_3)  != defFile->Read( &divDates0_3,		 sizeof(divDates0_3)  )		// 40-43
		 ||  1					  != defFile->Read( &divDates4,			 1					  ) )	// 44
//		 ||  3					  != defFile->Read( &packedModifiedDate, 3					  ) )	// 45-47
	{
#ifdef _DEBUG
	TRACE( _T("StockManager::ReadStock: Error(-2) reading the stockDef file.\n") );
#endif
		delete	bos;
		return	NULL;
	}

	divDates4 &= 255;			// kill off the upper byte

		// build an in-memory CStock for the stored data
	CStock*	theStock = new CStock( *bos );
	delete	bos;
	theStock->volatility = volatility;

		// divDates are organized as:
		// 4          3         2         1         0	Arabic (R->L) representation of file contents
		//  98765432 10987654321098765432109876543210	bit numbers
		//        Q4         Q3        Q2        Q1
		//  mmmmdddd dBmmmmdddddBmmmmdddddBmmmmdddddB	divDate data
		//     4         3        2        1        0	byte numbers
		//           high shift    . . .    low shift
		// decompress the ex-dividend dates and their associated booleans from the 40 divDates bits
	int		ii, days, months;
	int		yr = COleDateTime::GetCurrentTime().GetYear();
	for ( ii = 0; ii < 3; ii++ )
	{	theStock->dividendsArePaid[ii] = (divDates0_3 & 1) == 1;

		divDates0_3 >>= 1;
		days = divDates0_3  &  31;

		divDates0_3 >>= 5;
		months = divDates0_3  &  15;
		theStock->exDividendDates[ii].SetDate( yr, months, days );

		divDates0_3 >>= 4;
	}
	theStock->dividendsArePaid[3] = (divDates0_3 & 1) == 1;
	divDates0_3 >>= 1;									// now just have 1 bit of Q4 days left

	days = (divDates4 & 15)  |  divDates0_3;
	divDates4 >>= 4;

	months = divDates4  &  15;
	theStock->exDividendDates[3].SetDate( yr, months, days );

		// now for the dividend amounts
	for ( ii = 0; ii < 4; ii++ )
	{	theStock->dividends[ii] = dividends[ii];
	}
//	theStock->lastModifiedDate = unpackOleDate( packedModifiedDate );		// non-user data
	return	theStock;
}			// ReadStock()
//----------------------------------------------------------------------------
short		CStockManager::WriteStock( CStock* theStock )
{		// see if we can open the defs file
	CDBFile*	defFile = GetDefFile();
	if ( defFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("StockManager::WriteStock: Error(-1) getting the stockDef file.\n") );
#endif
		return	-1;
	}
		// save only the day and month from each of the ex-dividend COleDateTimes
		// represent day with five bits and month with four,
		// represent dividendsArePaid with one bit for the bool (1=true)
		// concatenate all four dates as 40 bits (5 bytes) of persistent data
		// organized as:  Q1 has dividends, Q1 divDate, Q2 has dividends, etc.
		// 4          3         2         1         0	Arabic (R->L) representation of disk contents
		//  98765432 10987654321098765432109876543210	bit numbers
		//        Q4         Q3        Q2        Q1
		//  mmmmdddd dBmmmmdddddBmmmmdddddBmmmmdddddB	divDate data
		//     4         3        2        1        0	byte numbers
		//           high shift    . . .    low shift
		// compress ex-dividend dates and their associated booleans into 40 bits
	unsigned short	day3 = theStock->exDividendDates[3].GetDay();
	unsigned short	divDates4 = (theStock->exDividendDates[3].GetMonth() << 4)		// 4 bits for month
							  | (day3 >> 1);										// 4 of 5 bits of day3
	register unsigned long	divDates0_3 = (day3 << 1)
									   | (theStock->dividendsArePaid[3] ? 1 : 0);
	for ( short ii = 2; ii >= 0; ii-- )
	{	divDates0_3 <<= 4;												// make room for months
		divDates0_3  |= theStock->exDividendDates[ii].GetMonth();
		divDates0_3 <<= 5;												// make room for days
		divDates0_3  |= theStock->exDividendDates[ii].GetDay();
		divDates0_3 <<= 1;												// make room for a boolean
		divDates0_3  |= theStock->dividendsArePaid[ii] ? 1 : 0;
	}

		// write the stock definition back to the file system (object store)
		// start with openPosOffset, symbol_ID, desc_ID, mktPrice & posIndx_ID
	short res = WriteBOS( (CBOS*)theStock );										//  0-19
	if ( res != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("StockManager::WriteStock: Error(-2) writing BOS content to the stockDef file, res = %d\n"), res );
#endif
		return	-2;
	}

		// now for the rest of the fields
//	long	packedToday = packOleDate( COleDateTime::GetCurrentTime() );
#if ( SEH_EXCEPTIONS )
	__try
#else
	try
#endif
	{	defFile->Write( &theStock->volatility, sizeof(theStock->volatility) );
		for ( unsigned short ii = 0; ii < 4; ii++ )
			defFile->Write( &theStock->dividends[ii], sizeof(theStock->dividends[ii]) );
		defFile->Write( &divDates0_3, sizeof(divDates0_3) );	// the low 4 bytes of the 40 bits of compressed divDates
		defFile->Write( &divDates4, 1 );						// the high byte of the 40 bits of compressed divDates
//		defFile->Write( &packedToday, 3 );						// last modified date
	}
#if ( SEH_EXCEPTIONS )
	__except ( EXCEPTION_EXECUTE_HANDLER )
#else
	catch ( std::exception& ex )
#endif
	{
#ifdef _DEBUG
		TRACE( _T("StockManager::WriteStock: Error(-3) writing to the stock defs file.\n") );
#endif
		return	-3;
	}
	defFile->Flush();
	return	0;
}			// WriteStock()
//----------------------------------------------------------------------------
