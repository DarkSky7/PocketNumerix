// DataManager.cpp: implementation of the CDataManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "NillaHedge.h"
#include "DateSupport.h"
//#include "DBFile.h"
#include "DataManager.h"
#include "ClosedPosition.h"
#include "DBFile.h"
#include "BOS.h"

#if ( ! SEH_EXCEPTIONS )
#include <exception>
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern	CNillaHedgeApp	theApp;

const CString	symbolFileName = _T("Symbols.pnh");
const CString	defFileName = _T("Defs.pnh");
const CString	descFileName = _T("Descs.pnh");
const CString	openPosFileName = _T("OpenPositions.pnh");
const CString	openPosNoteFileName = _T("OpenPosNotes.pnh");
const CString	posIndxFileName = _T("PosIndx.pnh");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataManager::CDataManager( CString subDir )
{
/*
	CString	appName;
	appName.LoadString( IDS_AppName );						// "???"
	CString	keyName;
	keyName.LoadString( IDS_DatabaseDirectory );			// "DatabaseDirectory"
	CString	defDir;
	defDir.LoadString( AFXCE_IDS_INTL_DIRMYDOCUMENTS );		// "My Documents"
	CString DBsubDir;
	DBsubDir.LoadString( IDS_DBSubDir );					// "NillaHedgeDB"

	defDir += _T("\\");
	defDir += DBsubDir;

//	CNillaHedgeApp	theApp = AfxGetApp();
//	dbDir = theApp->GetProfileString( appName, keyName, defDir );
*/
		// make sure the parent directory exists
	dbDir = theApp.GetDBdir();					// assumed to have backslash in front
	if ( CreateDirectory(dbDir, NULL) == 0 )
	{		// create failure, maybe the directory already exists
#ifdef _DEBUG
		TRACE( _T("DataManager::CDataManager: Error(-1) creating %s - last error = %d.\n"),
				dbDir, GetLastError() );
#endif
	}
		// make sure the DataManager's subdirectory exists
	dbDir += subDir;							// assumed to have backslash in front
	if ( CreateDirectory(dbDir, NULL) == 0 )
	{		// create failure, maybe the directory already exists
#ifdef _DEBUG
		TRACE( _T("DataManager::CDataManager: Error(-1) creating %s - last error = %d.\n"),
				dbDir, GetLastError() );
#endif
	}
	NullifyDBFiles();
}			// CDataManager()
//----------------------------------------------------------------------------
void	CDataManager::NullifyDBFiles( void )
{
	symbols = NULL;

	symbolFile = NULL;
	defsFile = NULL;
	descFile = NULL;

	posIndxFile = NULL;

	openPosFile = NULL;
	openPosNoteFile = NULL;
	closedPosFile = NULL;
	closedPosFileYear = 0;
}			// NullifyDBFiles()
//----------------------------------------------------------------------------
CDataManager::~CDataManager()
{
	DeleteSymbolTables();
	CloseDBFiles();
}			// ~CDataManager()
//----------------------------------------------------------------------------
void	CDataManager::DeleteSymbolTables()
{
	if ( symbols )
	{	symbols->RemoveAll();
		delete	symbols;
		symbols = NULL;
	}
}			// DeleteSymbolTables()
//----------------------------------------------------------------------------
void	CDataManager::CloseDBFiles( void )
{
		// symbol file
	if ( symbolFile != NULL )
	{	symbolFile->Close();
		delete	symbolFile;
		symbolFile = NULL;
	}

		// def file
	if ( defsFile != NULL )
	{	defsFile->Close();
		delete	defsFile;
		defsFile = NULL;
	}

		// desc file
	if ( descFile != NULL )
	{	descFile->Close();
		delete	descFile;
		descFile = NULL;
	}

		// posIndx file
	if ( posIndxFile != NULL )
	{	posIndxFile->Close();
		delete	posIndxFile;
		posIndxFile = NULL;
	}

		// open position file
	if ( openPosFile != NULL )
	{	openPosFile->Close();
		delete	openPosFile;
		openPosFile = NULL;
	}

		// position note file
	if ( openPosNoteFile != NULL )
	{	openPosNoteFile->Close();
		delete	openPosNoteFile;
		openPosNoteFile = NULL;
	}

		// closed position file
	if ( closedPosFile != NULL )
	{	closedPosFile->Close();
		delete	closedPosFile;
		closedPosFile = NULL;
	}
}			// CloseDBFiles()
//----------------------------------------------------------------------------
bool	CDataManager::DBFileExists( CString fName )
{	WIN32_FIND_DATA		findFileData;
	CString	pathName = dbDir + _T("\\");
	CString csPosIndxFile = pathName + fName;
	HANDLE	hSearch = FindFirstFile( csPosIndxFile, &findFileData );
	return	hSearch != INVALID_HANDLE_VALUE;
}			// PosIndxFileExists()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Symbol Tables								//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
int		CDataManager::GetSymbolTableSize( void )
{
	if ( ! DBFileExists(symbolFileName) )
		return	0;

	CMapStringToPtr*	symTab = GetSymbolTable();
	return	symTab  ?  symTab->GetCount()  :  0;
}			// GetSymbolTableSize()
//----------------------------------------------------------------------------
CMapStringToPtr*	CDataManager::GetSymbolTable( void )
{
	if ( symbols != NULL )
		return	symbols;

	CDBFile*	symFile = GetSymbolFile();
	if ( symFile == NULL )
		return	NULL;

	DWORD flen = (unsigned long)symFile->GetLength();		// see GetLength for TRY/CATCH example
	symFile->SeekToBegin();

		// scan through the symbol file, saving the symbols found
	symbols = new CMapStringToPtr;
	DWORD fpos = 0;
	while ( fpos < flen )
	{		// read a string from the symFile
		wchar_t*	buf = ReadPString( symFile, fpos );
		if ( buf == NULL )
			goto	ReadError;

			// read 4-byte offset --> the <inst>Def offset
		long	offset;
		if ( sizeof(offset) != symFile->Read(&offset, sizeof(offset)) )
			goto	ReadError;

		symbols->SetAt( buf, (void*&)offset );
//		delete	buf;							// XXX is this is a memory leak???

			// loop exit data
		fpos = (unsigned long)symFile->GetPosition();
	}
		// that's it, return the resulting symbol table
	goto	ReturnLabel;

ReadError:
#ifdef _DEBUG
	TRACE( _T("DataManager::GetSymbolTable: Error reading the symbol file.\n") );
#endif
	symbols->RemoveAll();
	delete	symbols;
	symbols = NULL;

ReturnLabel:
	return	symbols;			// already TRACEd at the source
}			// getSymbolTable()
//----------------------------------------------------------------------------
POSITION	CDataManager::GetDefIterCtx( void )
{
	CMapStringToPtr*	symTab = GetSymbolTable();
	return	symTab  ?  symTab->GetStartPosition()  :  NULL;
}			// GetDefIterCtx()
//----------------------------------------------------------------------------
long	CDataManager::GetNextDef_ID( POSITION& pos, CString& key )
{	long	offset = -1;			// a sentinal 'return' value
	CMapStringToPtr*	symTab = GetSymbolTable();
	if ( symTab == NULL )
		return	-1;

	symTab->GetNextAssoc( pos, key, (void*&)offset );
	return	offset;
}			// GetNextDef_ID()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//									Symbol file								//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
long		CDataManager::AssetExists( CString symbol )
{		// return value is the Asset's offset.  If it doesn't exist --> -1
	CMapStringToPtr*	symTab = GetSymbolTable();
	if ( symTab == NULL )
		return	-1;

	long	offset;
	BOOL res = symTab->Lookup( symbol, (void*&)offset );
	return ( res == 0 )  ?  -1  :  offset;
}			// AssetExists()
//----------------------------------------------------------------------------
CDBFile*	CDataManager::GetSymbolFile( void )
{
	if ( symbolFile != NULL )
		return	symbolFile;

	CString		fName = dbDir + _T("\\");
	fName += symbolFileName;

	symbolFile = new CDBFile;
	CFileException	fe;
	if ( ! symbolFile->Open( fName, &fe ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::GetSymbolFile: couldn't open %s, cause = %d.\n"),
			fName, fe.m_cause );
#endif
		delete	symbolFile;
		symbolFile = NULL;
		exit( -1 );
	}
	return	symbolFile;
}			// GetSymbolFile()
//----------------------------------------------------------------------------
CString		CDataManager::ReadSymbol( long symbol_ID )
{
	CDBFile*	symFile = GetSymbolFile();
	if ( symFile == NULL )
		return	_T("");

	wchar_t*	buf = ReadPString( symFile, symbol_ID );
	if ( buf == NULL )
		return _T("");

	CString cs(buf);
	delete [] buf;
	return	cs;
}			// readSymbol()
//----------------------------------------------------------------------------
CString		CDataManager::GetSymbol( long def_ID )
{
	CBOS* bos = ReadBOS( def_ID );
	if ( bos == NULL )
		return	_T("");

	CString	sym = bos->getSymbol();
	delete	bos;

	return	sym;
}			// GetSymbol()
//----------------------------------------------------------------------------
short		CDataManager::WriteSymbol( CString sym, long def_ID, long& symbol_ID )
{
	CDBFile*	symFile = GetSymbolFile();
	if ( symFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WriteSymbol: Error(-1) opening a symbol file.\n"));
#endif
		return	-1;
	}
	long	offset = symbol_ID;
	short	res = WritePString( sym, symFile, offset );
	if ( res == 0 )
	{	symbol_ID = offset;							// write offset back to the Asset
#if ( SEH_EXCEPTIONS )
		__try
#else
		try
#endif
		{	symFile->Write( &def_ID, sizeof(def_ID) );
		}
#if ( SEH_EXCEPTIONS )
		__except ( EXCEPTION_EXECUTE_HANDLER )		// ( CFileException& ex 
#else
		catch ( std::exception& ex )
#endif
		{
#ifdef _DEBUG
			TRACE( _T("BOSDatabase::WriteSymbol: Error(-2) writing def_ID to a symbol file.\n"));
#endif
			return	-2;
		}
	}
	symFile->Flush();
	return	res;
}			// WriteSymbol()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//							Instrument Definition file						//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
long		CDataManager::GetSymbol_ID( CString symbol )
{		// return value is the offset of the Asset's symbol (in the symbols file).
		// all three instrument types store the symbol_ID as
		// the first element of the instument definition on disk
		// If the asset doesn't exist --> -1
	long	offset = AssetExists( symbol );
	if ( offset < 0 )
		return	offset;

		// offset of the instrument definition will lead us to its symbol_ID
	CDBFile*	defFile = GetDefFile();
	if ( defFile == NULL )
		return	-1;

			// position for reading the inst definition
	if ( offset != (long)(unsigned long)defFile->Seek( offset, CFile::begin ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::GetSymbol_ID: Seek to offset=%d in %s defs file failed.\n"), offset );
#endif
		return	-1;
	}
		// get the location of the asset's symbol
	long	symbol_ID;
	if ( sizeof(symbol_ID) != defFile->Read( &symbol_ID, sizeof(symbol_ID) ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::GetSymbol_ID: Reading defs file for a symbol_ID at offset=%d failed.\n"),
			offset );
#endif
		return	-1;
	}
	return	symbol_ID;
}			// GetSymbol_ID()
//----------------------------------------------------------------------------
CDBFile*	CDataManager::GetDefFile( void )
{
	if ( defsFile != NULL )
		return	defsFile;
	
	CString		fName = dbDir + _T("\\");
	fName += defFileName;

	defsFile = new CDBFile;
	CFileException	fe;
	if ( ! defsFile->Open( fName, &fe ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::GetDefFile: Couldn't open %s, cause = %d.\n"),
			fName, fe.m_cause );
#endif
		delete	defsFile;
		defsFile = NULL;
		exit( -1 );
	}
	return	defsFile;
}			// GetDefFile()
//----------------------------------------------------------------------------
long	CDataManager::GetDefFileLen( void )
{		// get the defFile
	CDBFile*	defFile = GetDefFile();
	if ( defFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::GetDefFileLen: Error(-1) getting the defFile.\n") );
#endif
		return	-1;
	}

		// find out how long it is
	return	(unsigned long)defFile->Seek( 0, CFile::end );
}			// GetDefFileLen()
//----------------------------------------------------------------------------
short	CDataManager::SelectOffset( CDBFile* dbFile, long& offset )
{		// Two issues:	1) file could have zero length (and therefore needs to have a free list created)
		//				2) a free list entry may exist
		// seek to the write offset requested
	UINT	beginEnd = CFile::begin;					// for offset > 0
	if ( offset < 0 )
	{		// this is a new position, so it goes at EOF or replaces a Deleted Position on the free list
			// see if there are any deleted Positions in the free list

			// only need to consider the free list if the file is non-empty
		unsigned long	flen = (unsigned long)dbFile->GetLength();
		if ( flen < 4 )			// could use sizeof(freeListSentinel) or similar if sizeof() weren't unsigned
		{		// this is the first position in the open positions file, create a free list head
			short	res = WriteLong( dbFile, 0, -1 );
			if ( res != 0 )
			{
#ifdef _DEBUG
				TRACE( _T("DataManager::SelectOffset: Error(-1) writing freeListHead=-1.\n") );
#endif
				return	-1;
			}

				// now we have a positive offset from BOF, so ...
				// beginEnd = CFile::begin;					  ... still holds sway
			offset = 4;			// would use sizeof(long) if sizeof weren't unsigned
			// a seek is not required, we're already at offset = 4;
		}
		else	// get the free list head (if it's non-negative, the new position will go there)
		{		// in here, the default is the end of the file
				// check for unused positions within the file ...
			long	freeListHead;
			short	res = ReadLong( dbFile, 0, freeListHead );
			if ( res != 0 )
			{
#ifdef _DEBUG
				TRACE( _T("DataManager::SelectOffset: Error(-2) reading the free list head.\n") );
#endif
				return	-2;
			}

			if ( freeListHead < 0 )
			{		// no free list, just use EOF
				offset = 0;
				beginEnd = CFile::end;
				// in general, a seek is still required
			}
			else		// fix up the free list ...
			{		// go get the next entry in the free list (it will become the new free list head)
				long	freeListEntry;
				res = ReadLong( dbFile, freeListHead, freeListEntry );			// Bad data (a zero) fetched
				if ( res != 0 )
				{
#ifdef _DEBUG
					TRACE( _T("DataManager::SelectOffset: Error(-3) reading a free list entry at %d.\n"),
						freeListHead );
#endif
					return	-3;
				}
					// put the contents of the freeListEntry into the freeListHead
				res = WriteLong( dbFile, 0, freeListEntry );
				if ( res != 0 )
				{
#ifdef _DEBUG
					TRACE( _T("DataManager::SelectOffset: Error(-4) reading a free list entry at %d.\n"),
						freeListHead );
#endif
					return	-4;
				}
					// we found a slot within the file, replace the freeListHead
				offset = freeListHead;
				// beginEnd = CFile::begin;		// we're already using CFile::begin
			}
		}
	}

	long	res = (unsigned long)dbFile->Seek( offset, beginEnd );
	if ( beginEnd == CFile::begin  &&  res != offset )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::SelectOffset: Error(-5) seeking offset=%d in the positions file.\n"),
			offset );
#endif
		return	-5;
	}

		// update offset (in case we sought 0 from EOF)
	offset = res;										// relative to CFile::begin

	return	0;											// the okay signal
}			// SelectOffset()
//----------------------------------------------------------------------------
CBOS*		CDataManager::ReadBOS( unsigned long offset )
{		// position for reading a BOS definition
	CDBFile*	defFile = GetDefFile();
	if ( defFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::ReadBOS: Error(-1) getting the defFile.\n") );
#endif
		return	NULL;
	}

	if ( offset != (unsigned long)defFile->Seek( offset, CFile::begin ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::ReadBOS: Error(-2) seeking to offset=%d in a Def file failed.\n"),
			offset );
#endif
		return	NULL;
	}
	long			symbol_ID;
	long			openPosOffset;
	long			desc_ID;
	char			status;
	MarketPrice_t	mktPrice;
	long			posIndx_ID;
		// NOTE:  getSymbol_ID() assumes symbol_ID is first
	if (	 sizeof(symbol_ID)	   != defFile->Read( &symbol_ID,	 sizeof(symbol_ID)	   )	//  0-3
		 ||  1					   != defFile->Read( &status,		 1					   )	//	4
		 ||  sizeof(openPosOffset) != defFile->Read( &openPosOffset, sizeof(openPosOffset) )	//  5-8
		 ||  sizeof(desc_ID)	   != defFile->Read( &desc_ID,		 sizeof(desc_ID)	   )	//  9-12
		 ||  sizeof(mktPrice)	   != defFile->Read( &mktPrice,		 sizeof(mktPrice)	   )	// 13-16
		 ||  sizeof(posIndx_ID)	   != defFile->Read( &posIndx_ID,	 sizeof(posIndx_ID)	   ) )	// 17-20 (21 bytes total)
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::ReadBOS: Error(-3) reading a Def file.\n") );
		AfxDebugBreak();
#endif
		return	NULL;
	}
		// prevent return of a deleted position
		// not deleted, keep going
	CBOS*	bos = new CBOS;
	bos->symbol_ID		= symbol_ID;
	bos->status			= (DefinitionStatus)status;
	if ( bos->status == DeletedDefinition )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::ReadBOS(-4): Just fetched a deleted definition at offset %d, status=%d\n"),
			offset, status );
		AfxDebugBreak();
#endif
		delete	bos;
		return	NULL;
	}
	bos->desc_ID		= desc_ID;
	bos->openPosOffset  = openPosOffset;
	bos->setMktPrice( mktPrice );
	bos->posIndx_ID		= posIndx_ID;
	bos->setDef_ID( offset );
	return	bos;
}			// ReadBOS( CDBFile* defFile, long offset )
//----------------------------------------------------------------------------
short		CDataManager::WriteBOS( CBOS* bos )
{
		// get the appropriate defFile
	CDBFile*	defFile = GetDefFile();
	if ( defFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WriteBOS: Error(-1) getting the def file.\n") );
#endif
		return	-1;
	}

		// SelectOffset() not only manages the freelist, it also performs a Seek to the resulting offset
	long	offset = bos->getDef_ID();						// do we already know where this BOS goes?
	short	res = SelectOffset( defFile, offset );			// verify or select a more appropriate offset and Seek() there
	if ( res < 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WriteBOS: Error(-2) getting a freelist managed offset=%d in a positions file.\n"),
			offset );
#endif
		return	-2;
	}


		// if it's a new BOS, fix up the symbol table
	CString	sym = bos->getSymbol();							// when caller is DelistIssueForPositions, bos doesn't know it's type (Stock or Option)
	if ( bos->symbol_ID < 0  &&  sym != _T("") )
	{		// this is a new BOS definition
		ASSERT( offset >= 0 );								// verify SelectOffset() --> a valid offset
		bos->setDef_ID( offset );

		WriteSymbol( sym, offset, bos->symbol_ID );			// last arg is read/write
		CMapStringToPtr*	symTab = GetSymbolTable();
		if ( ! symTab )
			return	-3;
		symTab->SetAt( sym, (void*&)offset );				// tell the symbol table about it
	}


		// if necessary, fix up the posIndx file
	if ( bos->openPosOffset >= 0  &&  bos->posIndx_ID < 0 )
	{	res = IndexIssueForPositions( bos );				// updates bos->posIndx_ID
		if ( res != 0 )
		{
#ifdef _DEBUG
			TRACE( _T("DataManager::WriteBOS: Error(-3) indexing '%s' for positions.\n"),
					sym );
			AfxDebugBreak();
#endif
		}
	}
	else if ( bos->openPosOffset < 0  &&  bos->posIndx_ID >= 0 )
	{	res = DelistIssueForPositions( bos );
		if ( res != 0 )
		{
#ifdef _DEBUG
			TRACE( _T("DataManager::WriteBOS: Error(-4) delisting'%s' for positions.\n"),
					sym );
			AfxDebugBreak();
#endif
		}
	}
#ifdef _DEBUG
	if ( bos->status == DeletedDefinition )
	{	TRACE( _T("DataManager::WriteBOS: Warning(-5) ... about to write a deleted definition.\n") );
		AfxDebugBreak();
	}
#endif

	res = WriteBOScore( bos );
	if ( res < 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WriteBOS: Error(-6) writing to the Def file.\n"));
#endif
		return	-4;
	}

	return	0;
}			// WriteBOS( CBOS* bos )
//----------------------------------------------------------------------------
short		CDataManager::WriteBOScore( CBOS* bos )
{		// note that we ignore the possibility that the index may need to be updated
		// write bos at the current offset in the defFile
	CDBFile*	defFile = GetDefFile();
	if ( defFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WriteBOScore: Error(-1) getting the def file.\n") );
#endif
		return	-1;
	}

		// seek to bos' offset
#ifdef _DEBUG
	ASSERT( bos->getDef_ID() >= 0 );
#endif
	long	res = (unsigned long)defFile->Seek( bos->def_ID, CFile::begin );
	if ( res != bos->getDef_ID() )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WriteBOScore: Error(-2) seeking to %d\n"), bos->def_ID );
#endif
		return	-2;
	}
		
		// write the bos to 'disc'
	MarketPrice_t	mktPrice = bos->getMktPrice();
#if ( SEH_EXCEPTIONS )
	__try
#else
	try
#endif
	{		// NOTE:  getSymbol_ID() assumes that symbol_ID is first
		defFile->Write( &bos->symbol_ID,	 sizeof(bos->symbol_ID)		);		//  0-3
		defFile->Write( &bos->status,		 1							);		//	4
		defFile->Write( &bos->openPosOffset, sizeof(bos->openPosOffset) );		//  5-8
		defFile->Write( &bos->desc_ID,		 sizeof(bos->desc_ID)		);		//  9-12
		defFile->Write( &mktPrice,			 sizeof(mktPrice)			);		// 13-16
		defFile->Write( &bos->posIndx_ID,	 sizeof(bos->posIndx_ID)	);		// 17-20 (21 bytes total)
	}
#if ( SEH_EXCEPTIONS )
	__except ( EXCEPTION_EXECUTE_HANDLER )
#else
	catch ( std::exception& ex )
#endif
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WriteBOScore: Error(-3) writing to a Def file.\n"));
#endif
		return	-3;
	}

		// in the event that the BOS is the terminal class
		// i.e. not part of a writeOption, or writeStock call
	defFile->Flush();
	return	0;						// the okay signal
}			// WriteBOScore()
//----------------------------------------------------------------------------
short		CDataManager::DeleteBOS( CBOS* bos )
{	short	res;
	ASSERT ( bos->def_ID >= 4 );
	CString	sym = bos->getSymbol();
	ASSERT( sym != _T("") );

		// mark the BOS's description Deleted
	if ( bos->desc_ID >= 0 )
	{	CDBFile*	dscFile = GetDescFile();
		ASSERT( dscFile != NULL );
		res = DeletePString( dscFile, bos->desc_ID );
		if ( res != 0 )
		{
#ifdef _DEBUG
			TRACE( _T("DataManager::DeleteBOS: Error(-1) deleting '%s's description ('%s'), res=%d.\n"),
				sym, bos->getDesc(), res );
				// the dscFile could be corrupted 
			AfxDebugBreak();
#endif
		}
		dscFile->Flush();
		bos->desc_ID = -1;
	}


		// delete all of bos' positions (on disk)
	res = bos->deletePositionList();		// performs the flush (after each position is deleted)
	if ( res != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::DeleteBOS: Error(-2) deleting the position list on disk, res=%d\n"), res );
		AfxDebugBreak();
#endif
	}


		// take the BOS's symbol out of the (in memory) symbol table
	CMapStringToPtr*	symTab = GetSymbolTable();
	ASSERT( symTab );
	if ( symTab->RemoveKey(sym) == 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::DeleteBOS: Error(-3) removing '%s' from the symbol table.\n"), sym );
		AfxDebugBreak();
#endif
	}
		// mark the BOS's (on disk) symbol Deleted
	if ( bos->symbol_ID >= 0 )
	{	CDBFile*	symFile = GetSymbolFile();
		ASSERT( symFile != NULL );
		res = DeletePString( symFile, bos->symbol_ID );
		if ( res != 0 )
		{
#ifdef _DEBUG
			TRACE( _T("DataManager::DeleteBOS: Error(-4) deleting '%s' from the symbol file, res=%d.\n"),
				sym, res );
				// the symbolFile could be corrupted 
			AfxDebugBreak();
#endif
		}
		symFile->Flush();
		bos->symbol_ID = -1;
	}


		// put this definition on the free list
	CDBFile*	defFile = GetDefFile();
	if ( defFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::DeleteBOS: Error(-5) getting the defFile.\n") );
		AfxDebugBreak();
#endif
	}
	long	freeListHead;								// UpdateFreeList() fetches this for us
	res = UpdateFreeList( defFile, bos->def_ID, freeListHead );
	if ( res != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::DeleteBOS: Error(-6) updating the free list in a definitions file, res=%d\n"), res );
		AfxDebugBreak();
#endif
	}


		// the deleted position becomes the head of the free list
		// so it needs to point to the former freeListHead (now next in the chain)
		// reuse the symbol_ID as a freeList pointer (all freeList entries are < 0)
	bos->symbol_ID = freeListHead;				// got the freeListHead from UpdateFreeList()
	bos->setSymbol( _T("") );
	bos->status = DeletedDefinition;			// important for subsequent defFile insertions
	res = WriteBOS( bos );						// essentially just to Write the symbol_ID & status to disk
	if ( res != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::DeleteBOS: Error(-7) overwriting the Deleted BOS, res=%d\n"), res );
		AfxDebugBreak();
#endif
	}
	defFile->Flush();
	return	0;									// the okay signal... 
}			// DeleteBOS( CBOS* bos )
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//						Instrument Description files						//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
CDBFile*	CDataManager::GetDescFile( void )
{
	if ( descFile != NULL )
		return	descFile;

	CString		fName = dbDir + _T("\\");
	fName += descFileName;

	descFile = new CDBFile;
	CFileException	fe;
	if ( ! descFile->Open( fName, &fe ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::GetDescFile: Couldn't open %s, cause = %d.\n"),
			fName, fe.m_cause );
#endif
		delete	descFile;
		descFile = NULL;
		exit( -1 );
	}
	return	descFile;
}			// GetDescFile()
//----------------------------------------------------------------------------
CString		CDataManager::ReadDesc( long desc_ID )
{
	CDBFile*	dscFile = GetDescFile();
	if ( dscFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::ReadDesc: Unable to get a desc file.\n") );
#endif
		return	_T("");
	}
	wchar_t* buf = ReadPString( dscFile, desc_ID );
	if ( buf != NULL )
	{	CString	cs(buf);
		delete [] buf;
		return	cs;
	}
	return	_T("");
}			// ReadDesc()
//----------------------------------------------------------------------------
short		CDataManager::WriteDesc( CString csDesc, long& desc_ID )
{
	CDBFile*	dscFile = GetDescFile();
	if ( dscFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WriteDesc: (-1) Unable to get a desc file.\n") );
#endif
		return	-1;
	}
	long	offset = desc_ID;
	short	res = WritePString( csDesc, dscFile, offset );
	if ( res == 0 )
		desc_ID = offset;
	dscFile->Flush();
	return	res;
}			// WriteDesc()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Open Position Notes							//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
CDBFile*		CDataManager::GetPosNoteFile( void )
{
		// request is for the openPosNotes file
	if ( openPosNoteFile != NULL )
		return openPosNoteFile;

	CString		fname = dbDir + _T("\\");
	fname += openPosNoteFileName;

	openPosNoteFile = new CDBFile;
	CFileException	fe;
	if ( ! openPosNoteFile->Open( fname, &fe ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::GetPosNoteFile: couldn't open %s, cause = %d.\n"),
			fname, fe.m_cause );
#endif
		delete	openPosNoteFile;
		openPosNoteFile = NULL;
		exit( -1 );
	}
	return	openPosNoteFile;
}			// GetPosNoteFile()
//----------------------------------------------------------------------------
CString		CDataManager::ReadPosNote( long offset )
{
	ASSERT( offset >= 0 );

	CDBFile*	noteFile = GetPosNoteFile();
	if ( noteFile == NULL )
		return	_T("");

	wchar_t*	buf = ReadPString( noteFile, offset );
	if ( buf != NULL )
	{	CString cs = CString( buf );
		delete [] buf;
		return	cs;
	}
	else
		return	_T("");
}			// ReadPosNote()
//----------------------------------------------------------------------------
short		CDataManager::WritePosNote( CString csNote, long& offset )
{
	CDBFile*	noteFile = GetPosNoteFile();
	if ( noteFile == NULL )
		return	-1;

	short	res = WritePString( csNote, noteFile, offset );
	noteFile->Flush();
	return	res;
}			// WritePosNote()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//							Position Index file								//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
CDBFile*		CDataManager::GetPosIndxFile( void )
{		// used by position list traversals
	if ( posIndxFile != NULL )
		return	posIndxFile;

	CString		fname = dbDir + _T("\\");
	fname += posIndxFileName;

	posIndxFile = new CDBFile;
	CFileException	fe;
	if ( ! posIndxFile->Open( fname, &fe ) )
	{
#ifdef _DEBUG
		TRACE( _T("CDataManager::GetPosIndxFile: Couldn't open %s, cause = %d.\n"),
			fname, fe.m_cause );
#endif
		delete	posIndxFile;
		posIndxFile = NULL;
		exit( -1 );
	}
		// replace the NULL with a pointer to the new DBFile
	return	posIndxFile;
}			// GetPosIndxFile()
//----------------------------------------------------------------------------
CDBFile*		CDataManager::GetStkOptIndxFile( CString stkSym )
{		// used by option list traversals for a given stock (by stock symbol)
	CString		fname = dbDir + _T("\\");
	fname += stkSym;

	CDBFile* stkOptIndxFile = new CDBFile;
	CFileException	fe;
	if ( ! stkOptIndxFile->Open( fname, &fe ) )
	{
#ifdef _DEBUG
		TRACE( _T("CDataManager::GetStkIndxFile: Couldn't open %s, cause = %d.\n"),
			fname, fe.m_cause );
#endif
		delete	stkOptIndxFile;
		stkOptIndxFile = NULL;
		exit( -1 );
	}
		// replace the NULL with a pointer to the new DBFile
	return	posIndxFile;
}			// GetStkOptIndxFile()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Position Indexing							//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
long	CDataManager::GetPosIndexedDef_ID( long zbi )
{		// returns the def_ID located at 4*zbi in the posIndxFile
	ASSERT( zbi >= 0 );
	unsigned long		fileLen = (unsigned long)posIndxFile->GetLength();
	unsigned long		offset = 4 * zbi;
	if ( offset > fileLen-4 )
		return	-1;				// not really an error, more of an EOF indicator

		// seek to the def_ID
	unsigned long	res = (unsigned long)posIndxFile->Seek( offset, CFile::begin );
	if ( res != offset )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::GetPosIndexedDef_ID: Error(-2) seeking to %d.\n"), offset );
#endif
		return	-2;
	}
		// read the def_ID
	long		def_ID;
	if ( sizeof(def_ID) != posIndxFile->Read( &def_ID, sizeof(def_ID) ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::GetPosIndexedDef_ID: Error(-3) reading a def_ID from the posIndx file.\n") );
#endif
		return	-3;
	}
	return	def_ID;
}			// GetPosIndexedDef_ID()
//----------------------------------------------------------------------------
long	CDataManager::GetStkIndexedOptDef_ID( CDBFile* stkOptIndxFile, long zbi )
{		// returns the def_ID located at 4*zbi in stkSym's OptIndxFile
		// XXX - working here
	ASSERT( zbi >= 0 );
	unsigned long		fileLen = (unsigned long)stkOptIndxFile->GetLength();
	unsigned long		offset = 4 * zbi;
	if ( offset > fileLen-4 )
		return	-1;				// not really an error, more of an EOF indicator

		// seek to the def_ID
	unsigned long	res = (unsigned long)stkOptIndxFile->Seek( offset, CFile::begin );
	if ( res != offset )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::GetPosIndexedDef_ID: Error(-2) seeking to %d.\n"), offset );
#endif
		return	-2;
	}
		// read the def_ID
	long		def_ID;
	if ( sizeof(def_ID) != stkOptIndxFile->Read( &def_ID, sizeof(def_ID) ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::GetPosIndexedDef_ID: Error(-3) reading a def_ID from the posIndx file.\n") );
#endif
		return	-3;
	}
	return	def_ID;
}			// GetStkIndexedDef_ID()
//----------------------------------------------------------------------------
long	CDataManager::NumIndexedIssues( void )
{	
	if ( ! DBFileExists( posIndxFileName ) )
		return	0;

	CDBFile*	posIndxFile = GetPosIndxFile();
	if ( posIndxFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::NumIndexedIssues: Error(-1) getting a posIndx file\n") );
#endif
		return	-1;
	}
	return	(unsigned long)posIndxFile->GetLength() / sizeof(long);
}			// NumIndexedIssues()
//----------------------------------------------------------------------------
short			CDataManager::IndexIssueForPositions( CBOS* bos )
{		// see if the BOS has already been indexed
	if ( bos->posIndx_ID >= 0 )
		return	0;

	CDBFile*	posIndxFile = GetPosIndxFile();
	if ( posIndxFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::IndexIssueForPositions: Error(-1) getting a posIndx file\n") );
#endif
		return	-1;
	}

		// Seek EOF
	unsigned long	res = (unsigned long)posIndxFile->Seek( 0, CFile::end );
	if ( res != (unsigned long)posIndxFile->GetLength() )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::IndexIssueForPositions: Error(-2) seeking EOF in a posIndx file, res=%d\n"), res );
#endif
		return	-2;
	}

		// indicate that the asset has been indexed
	bos->posIndx_ID = (unsigned long)posIndxFile->GetPosition();

		// Write the def_ID into the posIndx file
#if ( SEH_EXCEPTIONS )
	__try
#else
	try
#endif
	{
		posIndxFile->Write( &bos->def_ID, sizeof(bos->def_ID) );
	}
#if ( SEH_EXCEPTIONS )
	__except ( EXCEPTION_EXECUTE_HANDLER )
#else
	catch ( std::exception& ex )
#endif
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::IndexIssueForPositions: Error(-3) writing to a posIndx file.\n") );
#endif
		return	-3;
	}
	posIndxFile->Flush();
	return	0;
}			// IndexIssueForPositions()
//----------------------------------------------------------------------------
short			CDataManager::DelistIssueForPositions( CBOS* bos )
{		// get the index file
	CDBFile*	posIndxFile = GetPosIndxFile();
	if ( posIndxFile == NULL )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::DelistIssueForPositions: Error(-1) getting a posIndx file.\n") );
#endif
		return	-1;
	}
		// ensure that there's at least one indexed instrument
	unsigned long	fileLen = (unsigned long)posIndxFile->GetLength();
	long	lastDef_ID;
	ASSERT( fileLen >= sizeof(lastDef_ID) );

		// Seek -4 from EOF (offset of the last def_ID indexed
	unsigned long		res = (unsigned long)posIndxFile->Seek( 0 - sizeof(lastDef_ID), CFile::end );

		// read the last def_ID from the posIndxFile
	if ( sizeof(lastDef_ID) != posIndxFile->Read( &lastDef_ID, sizeof(lastDef_ID) ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::DelistIssueForPositions: Error(-2) reading last def_ID.\n") );
#endif
		return	-2;
	}
		// are there additional indexed issues besides this one?
	if ( lastDef_ID != bos->def_ID )
	{		// Yes.  We have to write lastDef_ID into the bos's slot (bos->posIndx_ID)
			// seek to the position of the delisted issue
		if ( bos->posIndx_ID != (long)(unsigned long)posIndxFile->Seek( bos->posIndx_ID, CFile::begin ) )
		{
#ifdef _DEBUG
			TRACE( _T("DataManager::DelistIssueForPositions: Error(-3) seeking to delisted's posIndx_ID.\n") );
#endif
			return	-3;
		}

			// write the lastDef_ID into the slot for the delisted issue
#if ( SEH_EXCEPTIONS )
		__try
#else
		try
#endif
		{
			posIndxFile->Write( &lastDef_ID, sizeof(lastDef_ID) );
		}
#if ( SEH_EXCEPTIONS )
		__except ( EXCEPTION_EXECUTE_HANDLER )
#else
		catch ( std::exception& ex )
#endif
		{
#ifdef _DEBUG
			TRACE( _T("DataManager::DelistIssueForPositions: Error(-4) writing to a posIndx file.\n") );
#endif
			return	-4;
		}
		posIndxFile->Flush();

			// update the issue that used to be the last issue def_ID in the index file
		CBOS*	lastBos = ReadBOS( lastDef_ID );		// NOTE: this BOS doesn't know it's AssetType
		lastBos->posIndx_ID = bos->posIndx_ID;			// so, it can't lookup its symbol because
		short	res = WriteBOScore( lastBos );			// it won't be able to find its DataManager
#ifdef _DEBUG
		if ( res != 0 )
			TRACE( _T("DataManager::DelistIssueForPositions: WriteBOScore failed, res=%d\n"), res );
#endif
		delete	lastBos;
	}
		// trim duplicate lastDef_ID out of the file
	posIndxFile->SetLength( fileLen - sizeof(lastDef_ID) );

		// indicate that the asset has been delisted
	bos->posIndx_ID = -1;		// depend on caller to write bos and Flush the defFile
	return	0;
}			// DelistIssueForPositions()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//							Position Iteration Contexts						//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
CPosition*		CDataManager::GetNextPosition( PositionIterationContext*& ctx )
{		// facilitates serially traversing positions in a Positions File
	CPosition* pos;				// the return value
		// the gate
	if ( ctx == NULL )
		return	NULL;

		// the epilogue
	if ( ctx->offset >= ctx->eof )
	{	delete	ctx;
		ctx = NULL;
		return	NULL;
	}

		// the work ...
	if ( ctx->yr == 0 )
	{		// we're looking for active (open) Positions
		pos = ReadOpenPosition( ctx->offset );
	}
	else
	{		// we're looking for Closed Positions
		pos = (CPosition*)ReadClosedPosition( ctx->posFile, ctx->offset );
	}
		// set up for next pass;
	ctx->offset = (unsigned long)ctx->posFile->GetPosition();
	return	pos;
}			// GetNextPosition()
//----------------------------------------------------------------------------
PositionIterationContext*	CDataManager::GetPositionIterationContext( short yr )
{
	PositionIterationContext*	ctx = new PositionIterationContext;
	ctx->yr = yr;
	ctx->posFile = GetPosFile( yr );
		// in active positions file, the first position is located at offset = 4, but
		// in closed positions files...  there's no free list, so
		//								 the first position is located at offset = 0
	ctx->offset = ( yr == 0 ) ? 4 : 0;
	ctx->eof = ( ctx->posFile ) ? (unsigned long)ctx->posFile->GetLength() : -1;
	return	ctx;
}			// GetPositionIterationContext()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//									Position file							//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
CDBFile*		CDataManager::GetPosFile( short yr /* = 0 */ )
{
	CString		fName = dbDir + _T("\\");
	if ( yr == 0 )							// request is for the openPosFile
	{		// filename: OpenPositions.pnh
		if ( openPosFile != NULL )
			return	openPosFile;
		fName += openPosFileName;
	}
	else									// request is for a closedPosFile
	{		// filename: ClosedPositions<YYYY>.pnh
		if ( closedPosFile )
		{	if ( closedPosFileYear == yr )
				return	closedPosFile;
		
			closedPosFile->Close();
			delete	closedPosFile;
			closedPosFile = NULL;
		}
		fName += _T("ClosedPositions");
		wchar_t		buf[8];
		swprintf( buf, _T("%4d"), yr );
		fName += buf;
		fName += _T(".pnh");
	}

		// open the positions file requested
	CDBFile*		posFile = new CDBFile;
	CFileException	fe;
	if ( ! posFile->Open( fName, &fe ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::GetPosFile(: couldn't open %s, cause = %d.\n"),
			fName, fe.m_cause );
#endif
		delete	posFile;
		posFile = NULL;
		exit( -1 );
	}

		// success ... track the open file
	if ( yr == 0 )
		openPosFile = posFile;
	else
	{	closedPosFile = posFile;
		closedPosFileYear = yr;
	}
	return	posFile;
}			// GetPosFile()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Position Delete/Write						//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
short	CDataManager::UpdateFreeList( CDBFile* dbFile, long offset, long& freeListHead )
{		// Manage the free list
	ASSERT( dbFile );
	unsigned long	fileLen = (unsigned long)dbFile->GetLength();
	ASSERT ( fileLen > 4 );

		// goto BOF and read the free list head
	short	res = ReadLong( dbFile, 0, freeListHead );
	if ( res != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::UpdateFreeList: Error(-1) getting the free list head.\n") );
#endif
		return	-1;
	}

		// point the free list head at the deleted Definition or Position
	res = WriteLong( dbFile, 0, offset );			// offset = pos.pos_ID for positions
	if ( res != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::UpdateFreeList: Error(-2) writing the free list head.\n") );
#endif
		return	-2;
	}

		// seek the position's offset
	long seekResult = (unsigned long)dbFile->Seek( offset, CFile::begin );
	if ( seekResult != offset )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::UpdateFreeList: Error(-3) seeking the Position offset.\n") );
#endif
		return	-3;
	}

	return	0;								// the okay signal
}			// UpdateFreeList()	
//----------------------------------------------------------------------------
short		CDataManager::DeletePosition( CPosition& pos )
{		// 
	ASSERT ( pos.pos_ID >= 4 );
	pos.status = DeletedPosition;		// this is important for subsequent (serial) posFile insertions

		// mark the position's note Deleted
	if ( pos.note_ID >= 0 )
	{	CDBFile*	posNoteFile = GetPosNoteFile();
		ASSERT( posNoteFile != NULL );
		short	err = DeletePString( posNoteFile, pos.note_ID );
		if ( err != 0 )
		{
#ifdef _DEBUG
			TRACE( _T("DataManager::DeletePosition: Error(-1) deleting the note in the posNotes file, err=%d.\n"),
				err );
#endif
			// consider this failure to be non-fatal (at least in the immediate sense), keep going...
			// the posNoteFile could well be corrupted 
		}
		posNoteFile->Flush();
	}

		// put this position on the free list
	CDBFile* posFile = GetPosFile();
	ASSERT( posFile );
	long	freeListHead;								// UpdateFreeList() provides this for us
	short	res = UpdateFreeList( posFile, pos.pos_ID, freeListHead );
	if ( res != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::DeletePosition: Error(-2) updating the free list in a positions file.\n") );
#endif
		return	-2;
	}

		// the deleted position is now the head of the free list
		// so it needs to point to the former freeListHead (now next in the chain)
		// reuse the def_ID as a freeList pointer (all freeList entries are < 0)
	pos.def_ID = freeListHead;						// got the freeListHead from UpdateFreeList()
	res = WritePositionCore( posFile, pos );
	if ( res != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::DeletePosition: Error(-3) overwriting the Deleted position.\n") );
#endif
		return	-3;
	}
	posFile->Flush();
	return	0;
}			// DeletePosition()
//----------------------------------------------------------------------------
short		CDataManager::WritePosition( CPosition* pos, long& offset )
{		// ensure that pos->status is properly set before calling writePosition()
	ASSERT( pos->status != UnknownPositionStatus );		
	short	yr = (pos->status == ClosedPosition) ? COleDateTime::GetCurrentTime().GetYear() : 0;
	CDBFile* posFile = GetPosFile( yr );
	ASSERT( posFile );

		// SelectOffset() not only manages the freelist, it also performs a Seek to the resulting offset
	short	res = SelectOffset( posFile, offset );
	if ( res < 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WritePosition: Error(-1) getting a freelist managed offset=%d in a positions file.\n"),
			offset );
#endif
		return	-1;
	}
	return	WriteOpenPosition( *pos );					// handles the Flush() too
}			// WritePosition()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Position Cores								//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
short	CDataManager::ReadPositionCore( CDBFile* posFile, CPosition& pos )
{		// Assumes posFile is already correctly positioned for the read
		// reads the core data (24 bytes) of a CPosition (or CClosedPosition)
		// into a CPosition (or CClosedPosition) created by the caller
	wchar_t	pckdInstStat;		// { Option, Stock } & { Closed, Deleted, Open }
	long	def_ID;				// offset into <option>Defs.pnh
	long	pckdPurchDate;
	double	nUnits;
	double	totalCost;
	if (	 sizeof(pos.def_ID)	   != posFile->Read( &def_ID,		 sizeof(pos.def_ID)	   )	//  1-4
		 ||	 1					   != posFile->Read( &pckdInstStat,  1					   )	//  5
		 ||  sizeof(pos.nUnits)	   != posFile->Read( &nUnits,		 sizeof(pos.nUnits)	   )	//  6-13
		 ||  sizeof(pos.totalCost) != posFile->Read( &totalCost,	 sizeof(pos.totalCost) )	// 14-21
		 ||  3					   != posFile->Read( &pckdPurchDate, 3					   ) )	// 22-24
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::ReadPositionCore:  Error(-1) reading from the positions file.\n") );
#endif
		return	-1;
	}
	pos.status = (PositionStatus)(pckdInstStat & 0xF);
	pos.inst   = (AssetType)((pckdInstStat >> 4) & 0xF);
	pos.def_ID = def_ID;
	pos.nUnits = nUnits;
	pos.totalCost = totalCost;
	pos.purchaseDate = unpackOleDate( pckdPurchDate );
	return	0;
}			// ReadPositionCore()
//----------------------------------------------------------------------------
short		CDataManager::WritePositionCore( CDBFile* posFile, CPosition& pos )
{		// Assumes posFile is already correctly positioned for the write
		// write the core data (24 bytes) of a CPosition (or CClosedPosition) to the file system
	ASSERT( pos.status != UnknownPositionStatus );
	wchar_t		pckdInstStat = (pos.inst << 4)  |  pos.status;
	long		pckdPurchDate = packOleDate( pos.purchaseDate );
#if ( SEH_EXCEPTIONS )
	__try
#else
	try
#endif
	{		// no longer storing the status byte for Open Positions
		posFile->Write( &pos.def_ID,	sizeof(pos.def_ID)	  );
		posFile->Write( &pckdInstStat,	1					  );	// { Option, Stock } & { Closed, Deleted, Open }
		posFile->Write( &pos.nUnits,	sizeof(pos.nUnits)	  );
		posFile->Write( &pos.totalCost,	sizeof(pos.totalCost) );
		posFile->Write( &pckdPurchDate,	3					  );
	}
#if ( SEH_EXCEPTIONS )
	__except ( EXCEPTION_EXECUTE_HANDLER )
#else
	catch ( std::exception& ex )
#endif
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WritePositionCore: Error(-1) writing to the positions file.\n") );
#endif
		return	-1;
	}
	return	0;
}			// WritePositionCore()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Open Positions								//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
CPosition*		CDataManager::ReadOpenPosition( long offset )
{
	ASSERT( offset >= 0 );
	CDBFile* posFile = GetPosFile();
	if ( posFile == NULL )
		return	NULL;

		// seek to the read position (passing right by the freelist pointer)
	if ( offset != (long)(unsigned long)posFile->Seek( offset, CFile::begin ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::ReadOpenPosition: Error(-1) seeking offset=%d in the positions file.\n"),
			offset );
#endif
		return	NULL;
	}

		// read the position core...
	CPosition	stackPos;
	short	res = ReadPositionCore( posFile, stackPos );
	if ( res != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::ReadOpenPosition: Error(-2) reading the Position core.\n") );
#endif
		return	NULL;
	}

	long		note_ID = -1;
	long		prevPos_ID = -1;
	if ( stackPos.status == OpenPosition )
	{		// read the rest of the open position
		if (	 sizeof(note_ID)	 != posFile->Read( &note_ID,	sizeof(note_ID)	   )	// 25-28
			 ||  sizeof(prevPos_ID)	 != posFile->Read( &prevPos_ID, sizeof(prevPos_ID) ) )	// 29-32
		{
#ifdef _DEBUG
			TRACE( _T("DataManager::readOpenPosition: Error(-3) reading supplementary fields from the Positions file.\n") );
#endif
			return	NULL;
		}
	}
	else
	{		// simulate reading the rest of the Open position
		posFile->Seek( sizeof(note_ID) + sizeof(prevPos_ID), CFile::current );
	}

		// create the return CPosition
	CPosition* pos = new CPosition( stackPos );
	pos->pos_ID = offset;						// remember where we found this position
	pos->note_ID = note_ID;
	pos->prevPos_ID = prevPos_ID;
	return	pos;
}			// ReadOpenPosition()
//----------------------------------------------------------------------------
short		CDataManager::WriteOpenPosition( CPosition& pos )
{		// subordinate to WritePosition(),
		// offset gets fixed up there, files flushed & closed, etc.
	CDBFile* 	posFile = GetPosFile();
	if ( posFile == NULL )
		return	-1;

	short	res = WritePositionCore( posFile, pos );
	if ( res != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WriteOpenPosition: Error(-2) writing to the positions file.\n") );
#endif
		return	-2;
	}

		// now for the rest of the Open Position
#if ( SEH_EXCEPTIONS )
	__try
#else
	try
#endif
	{	posFile->Write( &pos.note_ID,		 sizeof(pos.note_ID)	 );		// 25-28
		posFile->Write( &pos.prevPos_ID,	 sizeof(pos.prevPos_ID)  );		// 29-32
	}
#if ( SEH_EXCEPTIONS )
	__except ( EXCEPTION_EXECUTE_HANDLER )
#else
	catch ( std::exception& ex )
#endif
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WriteOpenPosition: Error(-3) writing supplementary fields to the Positions file.\n") );
#endif
		return	-3;
	}

	posFile->Flush();
	return	0;
}			// WriteOpenPosition()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//								Closed Positions							//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
CClosedPosition*	CDataManager::ReadClosedPosition( CDBFile* posFile, long offset )
{		// possibly a superfluous Seek(), but we can't be sure what the caller did previously
	ASSERT( posFile != NULL );
	long	ret = (unsigned long)posFile->Seek( offset, CFile::begin );
	if ( ret != offset )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::ReadClosedPosition: Error(-1) seeking offset=%d in a Positions file, ret=%d.\n"),
			offset, ret );
#endif
		return	NULL;
	}
		// read the CPosition core
	CPosition	stackPos;
	short	res = ReadPositionCore( posFile, stackPos );
	if ( res < 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::ReadClosedPosition: Error(-2) reading the Position core.\n") );
#endif
		return	NULL;
	}

		// read the fixed width types
	double		netProceeds;
	long		packedClosingDate;
	if (	 sizeof(netProceeds) != posFile->Read( &netProceeds,		sizeof(netProceeds) )		// 25-32
		 ||  3					 != posFile->Read( &packedClosingDate,	3					) )		// 33-35
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::ReadClosedPosition: Error(-3) reading supplementary fields from the Positions file.\n") );
#endif
		return	NULL;
	}

		// get the initial note
	CString		initialNote;
	wchar_t*	buf = ReadPString( posFile, (unsigned long)posFile->GetPosition() );
	if ( buf == NULL )
		return	NULL;
	else
	{	initialNote = buf;
		delete [] buf;
	}
		// get the closing note
	CString		closingNote;
	buf = ReadPString( posFile, (unsigned long)posFile->GetPosition() );
	if ( buf == NULL )
		return	NULL;
	else
	{	closingNote = buf;
		delete [] buf;
	}

		// all reads were successful ...
	CClosedPosition*	pos = new CClosedPosition( stackPos );
	pos->netProceeds = netProceeds;
	pos->dateClosed = unpackOleDate( packedClosingDate );
	pos->setNote( initialNote );							// opening note
	pos->closingNote = closingNote;
	pos->pos_ID = offset;
	pos->setStatus( ClosedPosition );
	return	pos;
}			// ReadClosedPosition()
//----------------------------------------------------------------------------
short		CDataManager::WriteClosedPosition( CClosedPosition* pos )
{		// write a closed CPosition back to the file system (object store)
	int			yr = pos->dateClosed.GetYear();
	CDBFile*	posFile = GetPosFile( yr );
	if ( posFile == NULL )
		return	-1;

	unsigned long	positionOffset = (unsigned long)posFile->Seek( 0, CFile::end );

		// write the Position core data
	short	res = WritePositionCore( posFile, (CPosition)*pos );
	if ( res != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WriteClosedPosition: Error(-2) writing Position core.\n") );
#endif
		return	-2;
	}
		// continue writing the remainder of the ClosedPosition data,
		// specifically:  dateClosed, netProceeds, initial & closing note
	long	packedClosingDate = packOleDate( pos->dateClosed );
#if ( SEH_EXCEPTIONS )
	__try
#else
	try
#endif
	{
		posFile->Write( &pos->netProceeds,	 sizeof(pos->netProceeds) );	// closed positions only
		posFile->Write( &packedClosingDate,	 3						  );	// closed positions only
	}
#if ( SEH_EXCEPTIONS )
	__except ( EXCEPTION_EXECUTE_HANDLER )			//  ( CFileException& ex )
#else
	catch ( std::exception& ex )
#endif
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WriteClosedPosition: Error(-3) writing supplementary fields to the positions file.\n") );
#endif
		posFile->SetLength( positionOffset );		// truncate the incomplete closedPosition
		return	-3;
	}

	long	initialNoteOffset = -1;					// signifies EOF
	res = WritePString( pos->getNote(), posFile, initialNoteOffset );
	if ( res != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WriteClosedPosition: Error(-4) writing opening note to the positions file.\n") );
#endif
		posFile->SetLength( positionOffset );		// truncate the incomplete closedPosition
		return	-4;
	}

	long	closingNoteOffset = -1;					// signifies EOF
	res = WritePString( pos->closingNote, posFile, closingNoteOffset );
	if ( res != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WriteClosedPosition: Error(-5) writing closing note to the positions file.\n") );
#endif
		posFile->SetLength( positionOffset );		// truncate the incomplete closedPosition
		return	-5;
	}

	posFile->Flush();
	return	0;
}			// WriteClosedPosition()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//									PStrings								//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
short	CDataManager::DeletePString( CDBFile* strFile, long offset )
{	ASSERT( strFile != NULL );
	if ( offset != (long)(unsigned long)strFile->Seek( offset, CFile::begin ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::DeletePString: Error(-1) seeking offset=%d in a PString file.\n"),
				offset );
#endif
		return	-1;
	}
	unsigned short		fsLen;
	if (   1 != strFile->Read( &fsLen, 1 ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::DeletePString: Error(-2) reading string length from a PString file.\n") );
#endif
		return	-2;
	}
	fsLen &= 0xFF;
	if ( offset != (long)(unsigned long)strFile->Seek( -1, CFile::current ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::DeletePString: Error(-3) seeking offset=%d in a PString file.\n"), offset );
#endif
		return	-3;
	}
		// we need to zero out 1 byte (the length), plus the next 2 * sLen bytes
		// in longs, that would be (2*sLen + 1)/4 
	short	nBytes = sizeof(wchar_t) * fsLen + 1;
		// + 1 accounts for integer division rounding down,
		// remember we'll only write the actual number of bytes
	short	nLongs = nBytes / sizeof(unsigned long) + 1;
	long	junk[128];
	for ( short ii = 0; ii < nLongs; ii++ )
		*(junk+ii) = 0;		// clear out the buffer, then write the exact number of bytes to disk
#if ( SEH_EXCEPTIONS )
	__try
#else
	try
#endif
	{	strFile->Write( &junk, nBytes );
	}
#if ( SEH_EXCEPTIONS )
	__except ( EXCEPTION_EXECUTE_HANDLER )
#else
	catch ( std::exception& ex )
#endif
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::DeletePString: Error(-4) overwriting the PString with NULLs.\n") );
#endif
		return	-4;
	}
	return	0;
}			// DeletePString()
//----------------------------------------------------------------------------
wchar_t*		CDataManager::ReadPString( CDBFile* fp, long offset )
{
	ASSERT( fp != NULL  &&  offset >= 0 );
		// position for reading a PString
	if ( offset != (long)(unsigned long)fp->Seek( offset, CFile::begin ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::ReadPString: Seek error (-1) to offset=%d.\n"), offset );
#endif
		return	NULL;
	}
		// read 1 byte string length
	unsigned char	sLen;
	if ( 1 != fp->Read(&sLen,1) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::ReadPString: Read error (-2) at offset=%d.\n"), offset );
#endif
		return	NULL;
	}
		// read <sLen> characters --> the buffer
	wchar_t*	buf = new wchar_t[sLen+1];
	unsigned short	bytesToRead = sLen * sizeof(wchar_t);
	if ( bytesToRead != fp->Read(buf, bytesToRead) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::ReadPString: Read (-3) error at offset=%d.\n"), offset+1 );
#endif
		delete	buf;
		return	NULL;
	}
	buf[sLen] = '\0';							// terminate the string
	return	buf;
}			// ReadPString()
//----------------------------------------------------------------------------
short		CDataManager::WritePString( CString cs, CDBFile* strFile, long& offset )
{		// strFile is assumed to be open (but not positioned) for read/write
	ASSERT( strFile != NULL );
	cs.TrimLeft();
	cs.TrimRight();
	short	bytesToWrite;
	short	zeroFill = 0;				// number of unsigned shorts to NULL out after writing cs
	short	fsLen, csLen = cs.GetLength();					// space needed
	if ( offset >= 0 )
	{		// we hope to replace an existing PString stored at offset
		long	res = (unsigned long)strFile->Seek( offset, CFile::begin );
		if ( res != offset )
		{
#ifdef _DEBUG
			TRACE( _T("DataManager::WritePString: (-1)Seek Failure.\n") );
#endif
			return	-1;
		}
		if ( 1 != strFile->Read( &fsLen, 1 ) )					// length of existing PString
		{
#ifdef _DEBUG
			TRACE( _T("DataManager::WritePString: (-2)Read Failure.\n") );
#endif
			return	-2;
		}
		fsLen &= 0xFF;					// only the low byte is from the file, the rest is junk

			// Can we overwrite the existing PString?
		zeroFill = fsLen - csLen;		// the difference between the before and after string lengths
		if ( zeroFill == 0 )
		{		// the new string is exactly the same length as the stored one
			goto WriteContents;			// the leader and trailer are already correct
		}
			// else, the replacement string is either longer or shorter
			// seek back over the length byte
		res = (unsigned long)strFile->Seek( -1, CFile::current );
		if ( res != offset )
		{
#ifdef _DEBUG
			TRACE( _T("DataManager::WritePString: (-3)Seek Failure.\n") );
#endif
			return	-3;
		}
			// we're now positioned to overwrite length & contents and zero fill the rest
		short	origBytes = sizeof(wchar_t) * fsLen + 1;
		long	flen = (unsigned long)strFile->GetLength();
		if ( zeroFill > 0  ||  flen == (offset + origBytes) )
		{		// this is either the last PString in the file (the flen test), or
				// the new PString is shorter than the stored one (zeroFill is positive)
			goto WriteLeader;									// offset will be unchanged
		}

		zeroFill = -zeroFill;
			// The new PString is longer than the stored PString but
			// the stored PString is not the last one in the file, so we have some modifications to do . . .
			// Skip past the existing PString.
			// The following Seek() can't fail, because seeking beyond EOF
			// (which shouldn't happen here), just extends the file
		res = (unsigned long)strFile->Seek( origBytes, CFile::current );
		UINT	bytesAvailable = flen - res;
		UINT	bytesWanted = sizeof(wchar_t) * zeroFill;
		if ( bytesAvailable >= bytesWanted )
		{		// read the characters after the stored string - if they're all NULLs, write the PString here
			long	buf[128];
			if ( bytesWanted != strFile->Read( &buf, bytesWanted ) )
			{
#ifdef _DEBUG
				TRACE( _T("DataManager::WritePString: (-4)Error reading PString extension.\n") );
#endif
				return	-4;
			}
			short	ii, nBytesTotal = bytesAvailable - bytesWanted;
			short	nLongs = nBytesTotal / sizeof(unsigned long);		// truncated by integer division
			bool	allNulls;
			for ( ii = 0; ii < nLongs; ii++ )
			{	allNulls = *(buf+ii++) == 0;
				if ( ! allNulls )
					break;
			}
			if ( allNulls )
			{	short	nBytes = nBytesTotal - nLongs * sizeof(unsigned long);
				if ( nBytes > 0 )
				{	long	lastLong = *(buf+nLongs);
						// low offset bytes reside at low bit positions
					for ( ii = 0; ii < nBytes; ii++ )
					{	unsigned char lastByte = (unsigned char)(lastLong >> (ii * sizeof(char)));
						allNulls = ( lastByte == 0 );
						if ( ! allNulls )
							break;
					}
				}
				if ( allNulls )
				{		// it's okay to write the longer string right where the shorter one was
					strFile->Seek( offset, CFile::begin );	// offset will be unchanged
					goto	WriteLeader;					// start with the leader and go from there
				}
			}
		}
		// else, the longer string doesn't fit here

			// zero out the existing PString
		short err = DeletePString( strFile, offset );
#ifdef _DEBUG
		if ( err != 0 )
			TRACE( _T("DataManager::WritePString: (-5)Error deleting PString, err=%d\n"), err );
			// consider this to be a non-fatal error, keep going...
#endif
			// now we can fall into SeekEOF to write the new string (where offset is modified)
	}
	// else ( offset < 0 ) --> just fall into SeekEOF and start writing the new string there

		// SeekEOF:
	strFile->Seek( 0, CFile::end );
	offset = (unsigned long)strFile->GetPosition();				// fix up the offset

WriteLeader:
#if ( SEH_EXCEPTIONS )
	__try
#else
	try
#endif
	{	strFile->Write( &csLen, 1 );
	}
#if ( SEH_EXCEPTIONS )
	__except ( EXCEPTION_EXECUTE_HANDLER )
#else
	catch ( std::exception& ex )
#endif
	{	goto WriteException;
	}

WriteContents:
	bytesToWrite = csLen * sizeof(wchar_t);
		// cs was cs.GetBuffer(csLen)
#if ( SEH_EXCEPTIONS )
	__try
#else
	try
#endif
	{	strFile->Write( cs, bytesToWrite );
	}
#if ( SEH_EXCEPTIONS )
	__except ( EXCEPTION_EXECUTE_HANDLER )
#else
	catch ( std::exception& ex )
#endif
	{	goto WriteException;
	}

		// zero-fill the rest
	if ( zeroFill > 0 )
	{		// zero fill the remainder of the original string
		bytesToWrite = zeroFill * sizeof(wchar_t);
		wchar_t	buf[255];
		for ( short ii = 0; ii < zeroFill; ii++ )
			*(buf+ii) = 0;
#if ( SEH_EXCEPTIONS )
		__try
#else
		try
#endif
		{	strFile->Write( &buf, bytesToWrite );
		}
#if ( SEH_EXCEPTIONS )
		__except ( EXCEPTION_EXECUTE_HANDLER )
#else
		catch ( std::exception& ex )
#endif
		{	goto WriteException;
		}
	}
	return	0;		// a successful return

WriteException:
#ifdef _DEBUG
	TRACE( _T("CDataManager::WritePString: Error(-6) writing PString.\n") );
#endif
	return	-6;
}			// WritePString()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//									Low Level I/O							//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
short		CDataManager::ReadLong( CDBFile* posFile, long goTo, long& data )
{		// prep for the initial seek
	UINT	beginEnd = CFile::begin;
		// if goTo is negative, we want EOF
	if ( goTo < 0 )
	{	beginEnd = CFile::end;
		goTo = 0;
	}
		// seek the read location
	posFile->Seek( goTo, beginEnd );

		// read the data
	if ( sizeof(data) != posFile->Read( &data, sizeof(data) ) )
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::ReadLong: Error(-1) reading a long.\n") );
#endif
		return	-1;
	}
	return	0;
}			// ReadLong()
//----------------------------------------------------------------------------
short		CDataManager::WriteLong( CDBFile* posFile, long goTo, long data )
{		// prep for the initial seek
	UINT	beginEnd = CFile::begin;
		// if goTo is negative, we want EOF
	if ( goTo < 0 )
	{	beginEnd = CFile::end;
		goTo = 0;
	}
		// seek the read location
	posFile->Seek( goTo, beginEnd );

		// write the data
#if ( SEH_EXCEPTIONS )
	__try
#else
	try
#endif
	{	posFile->Write( &data, sizeof(data) );
	}
#if ( SEH_EXCEPTIONS )
	__except ( EXCEPTION_EXECUTE_HANDLER )
#else
	catch ( std::exception& ex )
#endif
	{
#ifdef _DEBUG
		TRACE( _T("DataManager::WriteLong: Error(-1) writing the data to the Positions file.\n") );
#endif
		return	-1;
	}
	return	0;
}			// WriteLong()
//----------------------------------------------------------------------------
