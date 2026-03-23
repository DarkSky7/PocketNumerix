// ClosedPosition.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "ClosedPosition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClosedPosition

IMPLEMENT_SERIAL( CClosedPosition, CPosition, VERSIONABLE_SCHEMA | 1  )

CClosedPosition::CClosedPosition( CPosition& pos )
{
	inst = pos.inst;					// core
	def_ID = pos.def_ID;				// core
	nUnits = pos.nUnits;				// core
	totalCost = pos.totalCost;			// core
	purchaseDate = pos.purchaseDate;	// core
	note = pos.getNote();
	note_ID = pos.note_ID;
	noteHasChanged = pos.noteHasChanged;
	pos_ID = pos.pos_ID;
	prevPos_ID = pos.prevPos_ID;
	status = pos.status;
}

CClosedPosition::CClosedPosition()
{
}

CClosedPosition::~CClosedPosition()
{
}
/////////////////////////////////////////////////////////////////////////////
// CClosedPosition diagnostics

#ifdef _DEBUG
void CClosedPosition::AssertValid() const
{
	CPosition::AssertValid();
}

/*
void CClosedPosition::Dump(CDumpContext& dc) const
{
	CPosition::Dump(dc);
}
*/
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CClosedPosition serialization

void CClosedPosition::Serialize(CArchive& ar)
{	CPosition::Serialize( ar );

	if (ar.IsStoring())
	{	ar << dateClosed;
		ar << netProceeds;
	}
	else
	{	ar >> dateClosed;
		ar >> netProceeds;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CClosedPosition commands
