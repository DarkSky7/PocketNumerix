// DataManager.h: interface for the CDataManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAMANAGER_H__7DE112C6_E940_4988_8363_2DD9AA2EB0C7__INCLUDED_)
#define AFX_DATAMANAGER_H__7DE112C6_E940_4988_8363_2DD9AA2EB0C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "AssetType.h"
#include "Position.h"			// for PositionIterationContext
class	CDBFile;
class	CBOS;
class	CPosition;
class	CClosedPosition;

class CDataManager : public CObject
{
public:
	CDataManager( CString subDir );
	virtual ~CDataManager( void );

		// existence
	bool	DBFileExists( CString fName );			// returns false there is no file <fName> in the DataManager's subdirectory
	virtual	long	AssetExists( CString symbol );							// check

		// BOSs
//	virtual CBOS*	GetBOS( long offset );			// deleted, use ReadBOS
	CBOS*		ReadBOS( unsigned long offset );	// NULL := failure
	short		WriteBOS( CBOS* bos );				// 0 := success
	short		DeleteBOS( CBOS* bos );				// 0 := success

	short		WriteBOScore( CBOS* bos );			// 0 := success

		// symbols & descriptions
	long		GetSymbol_ID( CString symbol );								// reverse lookup
	CString		ReadSymbol( long symbol_ID );								// check
	CString		GetSymbol( long def_ID );
	short		WriteSymbol( CString sym, long def_ID, long& symbol_ID );	// check

	CString		ReadDesc( long desc_ID );									// check
	short		WriteDesc( CString desc, long& desc_ID );					// check

		// Position Notes
	CString		ReadPosNote( long note_ID );								// check
	short		WritePosNote( CString csNote, long& note_ID );				// check

		// database facilities
	CDBFile*	GetSymbolFile( void );										// check
	CDBFile*	GetDefFile( void );											// check
	CDBFile*	GetDescFile( void );										// check
	CDBFile*	GetPosFile( short yr = 0 );									// check
	CDBFile*	GetPosNoteFile( void );										// check
	CDBFile*	GetPosIndxFile( void );										// check
	CString		GetDBdir( void )		{	return	dbDir;	}

	long	GetDefFileLen();

		// Symbol table support
	bool	SymbolFileExists( void );					// returns false if an index file has never been created
	CMapStringToPtr*	GetSymbolTable( void );			// maps symbols to instrument defs
	int					GetSymbolTableSize( void );							// check
	POSITION	GetDefIterCtx( void );							// returns a definition iteration context
	long		GetNextDef_ID( POSITION& pos, CString& key );	// returns key (symbol) & offset for the next symTab entry

public:
		// indexing issues with Positions
	long	GetIndexedDef_ID( long zbi );			// returns the def_ID located at 4*zbi in the posIndxFile
	long	NumIndexedIssues( void );				// returns the number of offsets in the posIndxFile
protected:
	short	IndexIssueForPositions( CBOS* bos );							// check
	short	DelistIssueForPositions( CBOS* bos );							// check

		// free list related
	short	SelectOffset( CDBFile* dbFile, long& offset );						// manages the free list for Writes
	short	UpdateFreeList( CDBFile* dbFile, long offset, long& freeListHead );	// updates the free list for Deletes

		// Position Iteration Contexts
public:
	CPosition*					GetNextPosition( PositionIterationContext*& ctx );	// NULL := failure; when yr != 0, return is really a CClosedPosition*
	PositionIterationContext*	GetPositionIterationContext( short yr );

		// Positions
public:
	short	DeletePosition( CPosition& pos );									// 0 := success
	short	WritePosition( CPosition* pos, long& offset );						// 0 := success

	CPosition*	ReadOpenPosition( long offset );								// NULL := failure
	short		WriteOpenPosition( CPosition& pos );							// 0 := success

	CClosedPosition*	ReadClosedPosition( CDBFile* posFile, long offset );	// NULL := failure
	short				WriteClosedPosition( CClosedPosition* pos );			// 0 := success
protected:
	short	ReadPositionCore( CDBFile* posFile, CPosition& pos );				// 0 := success
	short	WritePositionCore( CDBFile* posFile, CPosition& pos );				// 0 := success

		// PStrings
protected:
	short				DeletePString( CDBFile* strFile, long offset );			// 0 := success
	wchar_t*			ReadPString( CDBFile* fp, long offset );				// caller deletes the return value
	short				WritePString( CString cs, CDBFile* fp, long& offset );	// offset is a read/write argument

		// low level I/O
	short	ReadLong( CDBFile* posFile, long goTo, long& data );
	short	WriteLong( CDBFile* posFile, long goTo, long data );

		// destructor support
public:
	void	DeleteSymbolTables( void );
protected:
	void	CloseDBFiles( void );
	void	NullifyDBFiles( void );

		// symbols, definitions, descriptions, position index file
	CMapStringToPtr*	symbols;			// maps symbol names to definition offsets
	CDBFile*	symbolFile;					// symbols.pnh
	CDBFile*	defsFile;					// defs.pnh
	CDBFile*	descFile;					// desc.pnh
	CDBFile*	posIndxFile;

		// positions
	CDBFile*	openPosFile;				// OpenPositions.pnh
	CDBFile*	openPosNoteFile;			// OpenPosNotes.pnh
		// the closedPositions file is opened when a position is closed and remains open
		// until the app closes and forces its closure.  However, if the calendar year
		// should increment while the closedPositions file is open the positions file is
		// closed and one with the current year suffix is opened for write.  Therefore,
		// the date/time MUST be examined before every write to a ClosedPositions<YYYY> file.
	CDBFile*	closedPosFile;				// named ClosedPositions<YYYY>.pnh
	short		closedPosFileYear;

		// home directory
	CString				dbDir;				// path to the portfolio directory
};
#endif // !defined(AFX_DATAMANAGER_H__7DE112C6_E940_4988_8363_2DD9AA2EB0C7__INCLUDED_)
