// BOS.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "BOS.h"
//#include "BOSDatabase.h"
#include "BondManagerApp.h"
#include "Position.h"
#include "DataManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CBondManagerApp	theApp;

/////////////////////////////////////////////////////////////////////////////
// CBOS

IMPLEMENT_SERIAL( CBOS, CObject, VERSIONABLE_SCHEMA | 1 )

CBOS::CBOS()
{	desc_ID = -1;
	def_ID = -1;
	desc = _T("");
	mktPrice = 0.0;
	openPosOffset = -1;
	positions = NULL;
	symbol_ID = -1;
	descHasChanged = false;
	posIndx_ID = -1;
	status = UnknownDefinitionStatus;		// derived classses must override this value
}

CBOS::CBOS( CString sym ) : symbol(sym)
{	desc_ID = -1;
	def_ID = -1;
	desc = _T("");
	mktPrice = 0.0;
	openPosOffset = -1;
	positions = NULL;
	symbol_ID = -1;
	descHasChanged = false;
	posIndx_ID = -1;
	status = UnknownDefinitionStatus;		// derived classses must override this value
}

CBOS::~CBOS()
{	if ( positions != NULL )
	{	for( POSITION mapPos = positions->GetStartPosition(); mapPos != NULL; )
		{	CPosition*	pos;
			long	offset;
			positions->GetNextAssoc( mapPos, (void*&)offset, (void*&)pos );
			delete	pos;
		}
		positions->RemoveAll();
		delete	positions;
		positions = NULL;
	}
}
//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// CBOS diagnostics

#ifdef _DEBUG
void	CBOS::AssertValid() const
{
	CObject::AssertValid();
}

/*
void	CBOS::Dump( CDumpContext& dc ) const
{
	CObject::Dump(dc);
}
*/
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBOS serialization

void	CBOS::Serialize( CArchive& ar )
{
	if ( ar.IsStoring() )
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}
/////////////////////////////////////////////////////////////////////////////
// CBOS commands
CString		CBOS::getDesc( void )
{
	if ( desc != _T("") )
		return	desc;
	if ( desc_ID < 0 )
		return	_T("");			// an error placeholder

		// get the database to do the work
	CDataManager*	dm = getDataManager();
	return	dm->ReadDesc( desc_ID );
}			// getDesc()
//---------------------------------------------------------------------------
CDataManager*		CBOS::getDataManager( void )
{		// update the database
	return	(CDataManager*)theApp.bndMgr;
}			// getDataManager()
//---------------------------------------------------------------------------
short		CBOS::saveDesc( void )
{		// update the database
	if ( ! descHasChanged )
		return 0;		// everything's already copacetic

	CDataManager*	dm = getDataManager();
	short	retVal = dm->WriteDesc( desc, desc_ID );
	if ( retVal == 0 )					// success indicator
		descHasChanged = false;
	return	retVal;
}			// saveDesc()
//---------------------------------------------------------------------------
void		CBOS::setDesc( CString cs )
{		// update the database
	descHasChanged = ( cs != desc );
	if ( descHasChanged )
		desc = cs;
}			// saveDesc()
//---------------------------------------------------------------------------
short		CBOS::addPosition( CPosition* pos )
{		// push down stack implementation
#ifdef _DEBUG
	ASSERT_VALID( pos );
#endif
	pos->prevPos_ID = openPosOffset;
	pos->saveNote();													// on disk

		// should we put the DB write into a separate thread ?
	CDataManager*	dm = getDataManager();
	short	retVal = dm->WritePosition( pos, pos->pos_ID );

		// fix up the def - this position is the new head of pDB's position list
	if ( retVal != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("BOS::addPosition: Error writing the new position to disk.\n") );
#endif
		return	retVal;
	}
		// update the positions list
	if ( positions == NULL )
		positions = new CMapPtrToPtr;
	positions->SetAt( (void*)pos->pos_ID, (void*)pos );					// in memory

		// fix up the BOS's openPosOffset in the database
	openPosOffset = pos->pos_ID;
	retVal = dm->WriteBOS( this );

#ifdef _DEBUG
	if ( retVal != 0 )
		TRACE( _T("BOS::addPosition: Error updating theAsset(%s) on disk, retVal = %d\n"),
			getSymbol(), retVal );
#endif
	return	retVal;
}			// addPosition()
//---------------------------------------------------------------------------
short		CBOS::deletePosition( CPosition* pos )
{		// delete the Position on disk & fix up the positions list
	ASSERT( positions  &&  openPosOffset >= 0 );
	CDataManager*	dm = getDataManager();

		// account for head and middle/end of the positions list deletions
	if ( openPosOffset == pos->pos_ID )
	{		// pos is the head of the positions list
		openPosOffset = pos->prevPos_ID;								// link around pos
		short	retVal = dm->WriteBOS( this );
#ifdef _DEBUG
		if ( retVal != 0 )
			TRACE( _T("BOS::deletePosition: Error(-1) updating theAsset(%s) on disk, retVal = %d\n"), getSymbol(), retVal );
#endif
	}
	else	// pos is not the head of the positions list
	{		// find pos's predecessor in the positions list
		CPosition*	predPos = NULL;
		bool	found = false;
		long	posOffset = openPosOffset;
		while ( positions->Lookup( (void*)posOffset, (void*&)predPos ) )
		{		// break on Lookup failure; or success in finding pos' predecessor
			if ( predPos->prevPos_ID == pos->pos_ID )
			{	predPos->prevPos_ID = pos->prevPos_ID;					// link around pos
					// update predPos on disk
				short	res = dm->WritePosition( predPos, predPos->pos_ID );
#ifdef _DEBUG
				if ( res != 0 )
					TRACE( _T("BOS::deletePosition: Error(-2) writing pos' predecessor position, res=%d\n"), res );
#endif
				found = true;
				break;
			}
				// set up for next Lookup()
			posOffset = predPos->prevPos_ID;
		}
#ifdef _DEBUG
		if ( ! found )
			TRACE( _T("BOS::deletePosition: Error(-3) finding predecessor to given Position.\n") );
#endif
	}

		// remove pos from the positions list (Map)
	positions->RemoveKey( (void*)pos->pos_ID );							// from the positions list
	short	retVal = dm->DeletePosition( *pos );						// on disk
#ifdef _DEBUG
	if ( retVal != 0 )
		TRACE( _T("BOS::deletePosition: Error(-4) deleting the Position, res=%d\n"), retVal );
#endif
	delete	pos;														// release memory
	return	retVal;
}			// deletePosition()
//---------------------------------------------------------------------------
short		CBOS::deletePositionList( void )
{		// delete the Position on disk & fix up the positions list
	if ( positions == NULL  &&  openPosOffset < 0 )
		return	0;											// nothing to do
	CDataManager*	dm = getDataManager();

		// get the DataManager to delete all existing positions (on disk), putting them on the free list
	if ( positions != NULL )
	{	POSITION	mapPos = positions->GetStartPosition();
		long		offset;
		CPosition*	pos;
		while ( mapPos )
		{	positions->GetNextAssoc( mapPos, (void*&)offset, (void*&)pos );
			short	res = dm->DeletePosition( *pos );
			if ( res != 0 )
			{
#ifdef _DEBUG
				TRACE( _T("BOS::deletePositionList: Error(-2) deleting CPosition at offset=%d\n"),
						offset );
				AfxDebugBreak();
#endif							// consider this problem to be non-fatal (???) and keep going
			}
			delete	pos;
		}
			// kill off the entire (in-memory) positions list
		positions->RemoveAll();
		delete	positions;
		positions = NULL;
	}
	openPosOffset = -1;


		// write openPosOffset back to disk
	short	res = dm->WriteBOS( this );
	if ( res != 0 )
	{
#ifdef _DEBUG
		TRACE( _T("BOS::deletePositionList: Error(-3) writing BOS back to disk at %d\n"), def_ID );
		AfxDebugBreak();
			// consider this problem to be non-fatal (???) and keep going
#endif
	}
	return	res;
}			// deletePositionList()
//---------------------------------------------------------------------------
CMapPtrToPtr*	CBOS::getPositionList( void )
{		// if previously fetched or there are no saved positions,
		//		positions is what we want
	if ( positions != NULL )
		return	positions;

		// first request and there are some positions stored ...
		// get the database to do the work
	positions = new CMapPtrToPtr;
	if ( openPosOffset < 0 )
		return	positions;

		// read the Positions from the database

	long	posOffset = openPosOffset;

	CDataManager*	dm = getDataManager();
	while ( posOffset >= 0 )
	{	CPosition* pos = dm->ReadOpenPosition( posOffset );
		if ( pos == NULL )
			break;
		ASSERT( pos->status != DeletedPosition );
		positions->SetAt( (void*)pos->pos_ID, (void*)pos );
		posOffset = pos->prevPos_ID;
	}
	return	positions;
}			// getPositionList()
//---------------------------------------------------------------------------
CString		CBOS::getSymbol( void )
{
	if ( symbol != _T("") )
		return	symbol;

	if ( symbol_ID < 0 )
		return	_T("");					// an error placeholder

		// get the database to do the work
	CDataManager*	dm = getDataManager();
	return	dm->ReadSymbol( symbol_ID );
}			// getSymbol()
//---------------------------------------------------------------------------
long		CBOS::getDef_ID( void )
{
/*	
	CMapStringToPtr*	symTab = theApp.pDB->getSymbolTable( getAssetType() );
	ASSERT( symTab != NULL );
	long	def_ID;
	CString	sym = getSymbol();
	symTab->Lookup( sym, (void*&)def_ID );
*/
	return	def_ID;
}			// getDef_ID()
//---------------------------------------------------------------------------
/*
short		CBOS::saveSymbol( void )
{		// update the database
	return	theApp.pDB->writeSymbol( symbol, getAssetType(), symbol_ID );;
}			// saveSymbol()
*/
//---------------------------------------------------------------------------
/*
short	CBOS::savePositionList( void )
{
	if ( positions == NULL )
		return 0;			// no positions to store

		// get the database to do the work
	return	theApp.pDB->writePositionList( positions, openPosOffset );
}			// savePositionList()
*/
//---------------------------------------------------------------------------
