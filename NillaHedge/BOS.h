#if !defined(AFX_BOS_H__ED90EA59_8510_4670_8EBE_136E159FC32C__INCLUDED_)
#define AFX_BOS_H__ED90EA59_8510_4670_8EBE_136E159FC32C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BOS.h : header file

#include "AssetType.h"
//#include "BOSDatabase.h"
//#include "OleDateTimeEx.h"

class	CPosition;
class	CDataManager;

/////////////////////////////////////////////////////////////////////////////
// CBOS document

enum DefinitionStatus
{	UnknownDefinitionStatus = 0,
	ActiveDefinition		= 1,
	DeletedDefinition		= 2
};

typedef		float	MarketPrice_t;

class CBOS : public CObject
{

public:
	CBOS();					// protected constructor used by dynamic creation
	virtual ~CBOS();

	DECLARE_SERIAL(CBOS)

protected:
	CBOS( CString sym );

// Attributes
public:

// Operations
public:
		// the following allows code sharing among COption & CStock,
		// but have to test the subclass object for type
	virtual	AssetType	getAssetType( void )			{	return UnknownAssetType;	}
	CString				getDesc( void );
	long				getDef_ID( void );
	MarketPrice_t		getMktPrice( void )				{	return	mktPrice;			}
	CMapPtrToPtr*		getPositionList( void );
	CString				getSymbol( void );

	void				setDef_ID( long offset )				{	def_ID = offset;			}
	void				setMktPrice( MarketPrice_t	mktPrc )	{	mktPrice = mktPrc;			}
	void				setSymbol( CString sym )				{	symbol = sym;				}
	void				setDesc( CString csDesc );
	short				saveDesc( void );					// returns an error code

	short				addPosition( CPosition* pos );
	short				deletePosition( CPosition* pos );
	short				deletePositionList( void );

	int					hasPositions( void )
						{	return	positions ? positions->GetCount() : 0;	}

		// accepting the default AssetType will generate an exception in the database

protected:
	CDataManager*		getDataManager( void );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBOS)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	//}}AFX_VIRTUAL

// Implementation

// Data Members
public:
		// non-user data
	long		desc_ID;			// offset into the <inst>Descs file
	long		symbol_ID;			// offset into the <inst>Symbols file
	long		openPosOffset;		// offset of 'first' position
	long		def_ID;				// in memory use only (saves having to load a symbol table to find it)
	long		posIndx_ID;			// offset of this def_ID in the <inst>PosIndx file
//	COleDateTime		lastModifiedDate;	// when persistent storage was last modified (need a subclass)
	DefinitionStatus	status;

protected:
	MarketPrice_t	mktPrice;			// dollars				(8 bytes, an int would be 4 bytes)
	CString			desc;
	CString			symbol;
	bool			descHasChanged;

		// not serialized with the definition - stored in a separate file
	CMapPtrToPtr*	positions;

#ifdef _DEBUG
	virtual void AssertValid() const;
//	virtual void Dump(CDumpContext& dc) const;
#endif
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_BOS_H__ED90EA59_8510_4670_8EBE_136E159FC32C__INCLUDED_)
