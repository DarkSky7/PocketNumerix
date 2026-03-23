// PositionDisplayOptions.h: interface for the CPositionDisplayOptions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POSITIONDISPLAYOPTIONS_H__D8B30379_89C0_4DBD_93BC_6053C72F9A65__INCLUDED_)
#define AFX_POSITIONDISPLAYOPTIONS_H__D8B30379_89C0_4DBD_93BC_6053C72F9A65__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPositionDisplayOptions : CObject
{
protected:
	DECLARE_SERIAL( CPositionDisplayOptions )

public:
	CPositionDisplayOptions();

public:
		// these values are updated by the PositionList Display Preferences and
		// the PortfolioNavigator Display Preferences dialogs
	BOOL	aggregateIncome;
	BOOL	annualizedNetGain;
	BOOL	capitalGain;
	BOOL	exerciseValue;			// only valid for options
	BOOL	initialCost;
	BOOL	marketValue;
	BOOL	netGain;
	BOOL	note;
	BOOL	numUnits;
	BOOL	purchaseDate;

		// these values are updated by the
		// InitialOptionPosition & InitialStockPosition
	BYTE	aggregateIncomePixels;
	BYTE	annualizedNetGainPixels;
	BYTE	capitalGainPixels;
	BYTE	exerciseValuePixels;	// only valid for options
	BYTE	initialCostPixels;
	BYTE	marketValuePixels;
	BYTE	netGainPixels;
	BYTE	notePixels;
	BYTE	numUnitsPixels;
	BYTE	purchaseDatePixels;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPositionDisplayOptions)
public:
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

public:
	virtual ~CPositionDisplayOptions();
#ifdef _DEBUG
	virtual void AssertValid() const;
//	virtual void Dump(CDumpContext& dc) const;
#endif
};
#endif // !defined(AFX_POSITIONDISPLAYOPTIONS_H__D8B30379_89C0_4DBD_93BC_6053C72F9A65__INCLUDED_)
