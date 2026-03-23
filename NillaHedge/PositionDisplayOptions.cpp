// PositionDisplayOptions.cpp: implementation of the CPositionDisplayOptions class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "NillaHedge.h"
#include "PositionDisplayOptions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL( CPositionDisplayOptions, CObject, VERSIONABLE_SCHEMA | 1 )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPositionDisplayOptions::CPositionDisplayOptions()
{	aggregateIncome = TRUE;
	annualizedNetGain = TRUE;
	capitalGain = TRUE;
	exerciseValue = TRUE;
	initialCost = TRUE;
	marketValue = TRUE;
	netGain = TRUE;
	note = TRUE;
	numUnits = TRUE;
	purchaseDate = TRUE;
}

CPositionDisplayOptions::~CPositionDisplayOptions()
{

}

void	CPositionDisplayOptions::Serialize( CArchive& ar )
{	if ( ar.IsStoring() )
	{	ar << aggregateIncome;
		ar << annualizedNetGain;
		ar << capitalGain;
		ar << exerciseValue;
		ar << initialCost;	
		ar << marketValue;
		ar << netGain;	
		ar << note;
		ar << numUnits;
		ar << purchaseDate;

		ar << aggregateIncomePixels;
		ar << annualizedNetGainPixels;
		ar << capitalGainPixels;
		ar << exerciseValuePixels;
		ar << initialCostPixels;	
		ar << marketValuePixels;
		ar << netGainPixels;
		ar << notePixels;
		ar << numUnitsPixels;
		ar << purchaseDatePixels;
	}
	else		// loading
	{	ar >> aggregateIncome;
		ar >> annualizedNetGain;
		ar >> capitalGain;
		ar >> exerciseValue;
		ar >> initialCost;	
		ar >> marketValue;
		ar >> netGain;	
		ar >> note;
		ar >> numUnits;
		ar >> purchaseDate;

		ar >> aggregateIncomePixels;
		ar >> annualizedNetGainPixels;
		ar >> capitalGainPixels;
		ar >> exerciseValuePixels;
		ar >> initialCostPixels;	
		ar >> marketValuePixels;
		ar >> netGainPixels;
		ar >> notePixels;
		ar >> numUnitsPixels;
		ar >> purchaseDatePixels;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPositionDisplayOptions diagnostics

#ifdef _DEBUG
void CPositionDisplayOptions::AssertValid() const
{
	CObject::AssertValid();
}

void CPositionDisplayOptions::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
