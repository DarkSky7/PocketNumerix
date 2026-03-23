// OptionManager.cpp: implementation of the COptionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "NillaHedge.h"
#include "OptionManager.h"
#include "DBFile.h"
#include "Option.h"

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

COptionManager::COptionManager( CString subDir ) : CDataManager(subDir)
{

}
//----------------------------------------------------------------------------
COptionManager::~COptionManager()
{

}
//----------------------------------------------------------------------------
COption*		COptionManager::GetPosIndexedOption( long zbi )
{
	CDBFile*	posIndxFile = GetPosIndxFile();
	if ( posIndxFile == NULL )
		return	NULL;
	long		def_ID = GetPosIndexedDef_ID( zbi );
	return ( def_ID >= 0 )  ?  ReadOption( def_ID )  :  NULL;
}			// GetPosIndexedOption()
//----------------------------------------------------------------------------
COption*		COptionManager::GetStkIndexedOption( CString stkSym, long zbi )
{		// working here
	CDBFile*	stkOptIndxFile = GetStkOptIndxFile( stkSym );
	return		GetStkIndexedOption( stkOptIndxFile, zbi );
}			// GetStkIndexedOption()
//----------------------------------------------------------------------------
COption*		COptionManager::GetStkIndexedOption( CDBFile* stkOptIndxFile, long zbi )
{		// zbi is an integer scaled up to file position within GetStkIndexedOptDef_ID()
	if ( stkOptIndxFile == NULL )
		return	NULL;
	long		def_ID = GetStkIndexedOptDef_ID( stkOptIndxFile, zbi );
	return ( def_ID >= 0 )  ?  ReadOption( def_ID )  :  NULL;
}			// GetStkIndexedOption()
//----------------------------------------------------------------------------
COption*		COptionManager::GetOption( CString symbol )
{		// fetch option definition from offset bytes into the optionDefFile
	long	offset = AssetExists( symbol );
	if ( offset < 0 )
		return	NULL;

		// mruOption updated in readOption()
	COption* opt = ReadOption( offset );
	opt->setSymbol( symbol );
	return	opt;
}			// getOption()
//----------------------------------------------------------------------------
COption*	COptionManager::ReadOption( long offset )
{		// fetch option definition from offset bytes into the optionDefFile
	long	stockSymbol_ID;
	float	strikePrice;
	char	putCall;
	long	packedExpiry;
//	long	packedModifiedDate;

	CDBFile*	defFile = GetDefFile();
	if ( defFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("OptionManager::ReadOption: Error(-1) getting the optionDef file.\n") );
#endif
		return	NULL;
	}
		// read the option definition
		// start with openPosOffset, symbol_ID, desc_ID & mktPrice
	CBOS*	 bos = ReadBOS( offset );																	//  0-19
	if (	 bos					== NULL
		 ||	 sizeof(stockSymbol_ID) != defFile->Read( &stockSymbol_ID,	   sizeof(stockSymbol_ID) )		// 20-23
		 ||  sizeof(strikePrice)	!= defFile->Read( &strikePrice,		   sizeof(strikePrice)	  )		// 24-27
		 ||  1						!= defFile->Read( &putCall,			   1					  )		// 28
		 ||  3						!= defFile->Read( &packedExpiry,	   3					  )	)	// 29-31
	{
#ifdef _DEBUG
		TRACE( _T("OptionManager::ReadOption: Error(-2) reading the optionDef file.\n") );
#endif
		delete	bos;
		return	NULL;
	}
		// build an in-memory COption for the stored data
	COption* theOption = new COption( *bos );
	delete	bos;
	theOption->stockSymbol_ID = stockSymbol_ID;				// non-user data
	theOption->strikePrice = strikePrice;
	theOption->setPutCall( putCall );
	theOption->setExpiry( unpackOleDate( packedExpiry ) );
//	theOption->lastModifiedDate = unpackOleDate( packedModifiedDate );
	return	theOption;
}			// ReadOption()
//----------------------------------------------------------------------------
short		COptionManager::WriteOption( COption* theOption )
{		// see if we can open the defs file
	CDBFile*	defFile = GetDefFile();
	if ( defFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("OptionManager::WriteOption:  Error(-1) getting the optionDef file.\n") );
#endif
		return	-1;
	}
		// write the option definition back to the file system (object store)
		// start with openPosOffset, symbol_ID, desc_ID, mktPrice & posIndx_ID
	short res = WriteBOS( (CBOS*)theOption );												//  0-19
	if ( res != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("OptionManager::WriteOption:  Error(-2) writing BOS content to the optionDef file, res = %d\n"), res );
#endif
		return	-2;
	}
		// now for the rest of the fields
	char	putCall = (char)theOption->getPutCall();
	long	packedExpiry = packOleDate( theOption->getExpiry() );
//	long	packedToday = packOleDate( COleDateTime::GetCurrentTime() );
#if ( SEH_EXCEPTIONS )
	__try
#else
	try
#endif
	{
#ifdef _DEBUG
		TRACE( _T("OptionManager::WriteOption theOption->stockSymbol_ID=%d\n"), theOption->stockSymbol_ID );
#endif
		defFile->Write( &theOption->stockSymbol_ID,	sizeof(theOption->stockSymbol_ID) );	// 20-23
		defFile->Write( &theOption->strikePrice,	sizeof(theOption->strikePrice)	  );	// 24-27
		defFile->Write( &putCall,					1								  );	// 28
		defFile->Write( &packedExpiry,				3								  );	// 29-31
//		defFile->Write( &packedToday,				3								  );	// 32-34	last modified date
	}
#if ( SEH_EXCEPTIONS )
	__except ( EXCEPTION_EXECUTE_HANDLER )
#else
	catch ( std::exception& ex )
#endif
	{
#ifdef _DEBUG
		TRACE( _T("OptionManager::writeOption:  Error(-3) writing to the optionDef file.\n"));
#endif
		return	-3;
	}
	defFile->Flush();
	return	0;
}			// WriteOption()
//----------------------------------------------------------------------------
