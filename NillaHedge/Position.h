// Position.h: interface for the CPosition class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_POSITION_H__DEE0CD15_EDFF_4A7D_B06A_A7A4690FDC0B__INCLUDED_)
#define AFX_POSITION_H__DEE0CD15_EDFF_4A7D_B06A_A7A4690FDC0B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AssetType.h"
#include "PositionStatus.h"

class	CDataManager;
//#include "NillaHedge.h"
//#include "BOSDatabase.h"
//#include "OleDateTimeEx.h"

class	CBOS;
class	CDBFile;

typedef struct
{	CDBFile*		posFile;
	short			yr;
	long			offset;
	long			eof;
} PositionIterationContext;

class CPosition : public CObject
{
	DECLARE_SERIAL( CPosition )

public:
					CPosition();
					CPosition( CPosition& pos );
	virtual			~CPosition();
	void			Serialize( CArchive& ar );

		// accessors
	CDataManager*	getDataManager( void );
	CString			getNote( void );
	void			setNote( CString csNote );
	short			saveNote( void );

	CString			getSymbol( void );
	CBOS*			getBOS( void );

	void			setStatus( short stat );

public:
		// attributes
	double				nUnits;
	COleDateTime		purchaseDate;
	double				totalCost;

		// non-user data
	AssetType			inst;				// Option or Stock
	long				pos_ID;				// offset into the positions file
	long				def_ID;
	long				prevPos_ID;
	long				note_ID;
	PositionStatus		status;				// { Open, Closed, or Deleted }
// protected:
		// want noteHasChanged to be protected,
		// but the compiler doesn't like accessing pos.noteHasChanged
	bool				noteHasChanged;

protected:
	CString				note;
};
#endif // !defined(AFX_POSITION_H__DEE0CD15_EDFF_4A7D_B06A_A7A4690FDC0B__INCLUDED_)
