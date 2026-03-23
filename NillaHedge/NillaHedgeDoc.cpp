// NillaHedgeDoc.cpp : implementation of the CNillaHedgeDoc class
//
#include "StdAfx.h"
#include "NillaHedge.h"
#include "NillaHedgeDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNillaHedgeDoc

IMPLEMENT_SERIAL( CNillaHedgeDoc, CDocument, VERSIONABLE_SCHEMA | 1 )

BEGIN_MESSAGE_MAP( CNillaHedgeDoc, CDocument )
	//{{AFX_MSG_MAP(CNillaHedgeDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNillaHedgeDoc construction/destruction

CNillaHedgeDoc::CNillaHedgeDoc()
{
	// TODO: add one-time construction code here
}

CNillaHedgeDoc::~CNillaHedgeDoc()
{
}

BOOL	CNillaHedgeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

		// this is where we defaulted the risk free rate (in the pDoc approach)
//	shortRateBasis.riskFreeRate = 0.03f;		// a small interest rate (3.0%)

	// (SDI documents will reuse this document)

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CNillaHedgeDoc serialization

void	CNillaHedgeDoc::Serialize( CArchive& ar )
{
//	bonds.Serialize( ar );
//	options.Serialize( ar );
//	portfolioNavigatorOptions.Serialize( ar );
//	positionListOptions.Serialize( ar );
//	shortRateBasis.Serialize( ar );
//	stocks.Serialize( ar );	
}
/////////////////////////////////////////////////////////////////////////////
// CNillaHedgeDoc diagnostics

#ifdef _DEBUG
void CNillaHedgeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

/*
void CNillaHedgeDoc::Dump( CDumpContext& dc ) const
{
	CDocument::Dump(dc);
}
*/
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNillaHedgeDoc commands
