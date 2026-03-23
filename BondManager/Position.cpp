// Position.cpp: implementation of the CPosition class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BondManagerApp.h"
#include "BOS.h"
#include "Position.h"
#include "DataManager.h"
#include "BondManager.h"
//#include "OptionManager.h"
//#include "StockManager.h"
//#include "BOSDatabase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CBondManagerApp	theApp;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPosition::CPosition()
{	inst = UnknownAssetType;
	pos_ID = -1;
	def_ID = -1;
	prevPos_ID = -1;
	note_ID = -1;
	status = OpenPosition;
	noteHasChanged = false;
}

CPosition::CPosition( CPosition& pos )
{	inst = pos.inst;					// core
	def_ID = pos.def_ID;				// core
	nUnits = pos.nUnits;				// core
	totalCost = pos.totalCost;			// core
	purchaseDate = pos.purchaseDate;	// core
	note = pos.note;
	note_ID = pos.note_ID;
	noteHasChanged = pos.noteHasChanged;
	pos_ID = pos.pos_ID;
	prevPos_ID = pos.prevPos_ID;
	status = pos.status;
}

CPosition::~CPosition()
{
}

IMPLEMENT_SERIAL( CPosition, CObject, VERSIONABLE_SCHEMA | 1 )

void	CPosition::Serialize( CArchive& ar )
{	if ( ar.IsStoring() )
	{	ar << note;
		ar << nUnits;
		ar << purchaseDate;
		ar << totalCost;
	}
	else	// loading
	{	ar >> note;
		ar >> nUnits;
		ar >> purchaseDate;
		ar >> totalCost;
	}
}
//--------------------------------------------------------------------
CDataManager*		CPosition::getDataManager( void )
{		// update the database
	return	theApp.bndMgr;
}			// getDataManager()
//---------------------------------------------------------------------------
CString		CPosition::getNote( void )
{
	if ( note != _T("") )
		return note;			// got it right here!

	if ( note_ID < 0 )
		return _T("");			// nothing to look up

		// it's an existing position whose Note hasn't been loaded yet
		// get the database to do the real work
	CDataManager*	dm = getDataManager();
	return	dm->ReadPosNote( note_ID );
}			// getNote()
//--------------------------------------------------------------------
void		CPosition::setNote( CString csNote )
{	if ( csNote != note )
	{	note = csNote;
		noteHasChanged = true;
	}
}			// setNote()
//--------------------------------------------------------------------
short		CPosition::saveNote( void )
{		// the database does the real work
	short res = 0;
	if ( noteHasChanged )
	{	CDataManager*	dm = getDataManager();
		res = dm->WritePosNote( note, note_ID );
	}
	return	res;
}			// saveNote()
//--------------------------------------------------------------------
CString		CPosition::getSymbol( void )
{	CBOS*	theAsset = getBOS();
	CString sym = theAsset->getSymbol();
	delete	theAsset;
	return	sym;
}			// getSymbol()
//---------------------------------------------------------------------------
CBOS*		CPosition::getBOS( void )
{
	CDataManager*	dm = getDataManager();
	CBOS*	theAsset = (CBOS*)((CBondManager*)dm)->ReadBond( def_ID );
	return	theAsset;
}			// getBOS()
//---------------------------------------------------------------------------
void		CPosition::setStatus( short stat )
{
	if		( stat == 1 )
		status = OpenPosition;
	else if ( stat == 2 )
		status = ClosedPosition;
	else if ( stat == 4 )
		status = DeletedPosition;
	else
		status = UnknownPositionStatus;
}			// setStatus()
//---------------------------------------------------------------------------
