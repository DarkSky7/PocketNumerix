// OneStk3OptStrategy.h

#if !defined(AFX_OneStk3OptStrategy_H__BF98493C_AE1F_4C52_95B7_A4A03AA87931C4F2__INCLUDED_)
#define      AFX_OneStk3OptStrategy_H__BF98493C_AE1F_4C52_95B7_A4A03AA87931C4F2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'StdAfx.h' before including this file for PCH
#endif

#include "OptionType.h"

enum	BuySellStatus
{	UnknownBuySellStatus	=  0,
	Buy						=  1,
	Sell					=  2
};

	// the following is used in the Strategy Explorer (no real links to the rest of NillaHedge)
typedef struct
{	BuySellStatus	buySell[4];			// stock buySell is in buySell[3]
	float			cost[4];			// stock price is in cost[3]
	float			qty[4];
	float			strike[3];
	OptionType		putCall[3];
	// enable the following to support calendar spreads and BlackScholes plots
//	float			volatility;			// the stock's volatility
//	long			exp[3];				// compressed date of the options' expiration
//	long			evalDate;			// compressed date of option evaluation
} OneStk3OptStrategy;

	// the following is used in the HedgeAnalyzer (definite links to the database)
typedef struct
{	long			def_ID[4];			// stockDef_ID at index 3
	float			qty[4];				// stockQty at index 3
	BuySellStatus	buySell[4];			// stock buySell at index 3	
} OneStk3OptHedge;
//---------------------------------------------------------------------------
#endif