#if !defined(AFX_PORTFOLIOCOMMON_H__B7352E9Z_868F_4CC1_A00C_210ZHGDC4F94D1D__INCLUDED_)
#define AFX_PORTFOLIOCOMMON_H__B7352E9Z_868F_4CC1_A00C_210ZHGDC4F94D1D__INCLUDED_


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "AssetType.h"

// for PortfolioOverviewDialog and TreeListCtrl use...

enum	OverviewItemType
{	Portfolio	= 0x0,		// indent level 0
	Instruments = 0x1,
	Issues		= 0x2,
	Positions	= 0x3
};

// PortfolioOverviewDialog list items could be any of the following
//	0) a portfolio (grand total) row
//	1) an instrument subtotal row
//	2) an issue subtotal row
//	3) a position row

// Portfolio List Item Info
typedef struct
{	AssetType				inst;			// GrandTotal uses UnknownAssetType
	bool					visible;
	short					rowDataIdx;
	OverviewItemType		level;			// Positions/Issues/Instruments/Portfolio --> LVITEM.iIndent
	CString					dateName;		// positions have purchase dates, instruments have names, then there's total row content
	double					nUnits;
	double					totalCost;
	double					mktValue;
	//						capGain			// can figure this out
	double					exerIncome;		// options have exerVal, stocks have aggIncome
	//						netGain			// can figure this out 
	float					pctYield;		// Stocks only
	CString					noteDesc;		// positions have notes, instruments have descriptions
} PortfolioListItemInfo;
//------------------------------------------------------------------------------------------
#endif