// OneStk4OptStrategy.h

#if !defined(AFX_OneStk4OptStrategy_H__BF98493C_AE1F_4C52_95B7_A4A03AA87931C4F2__INCLUDED_)
#define      AFX_OneStk4OptStrategy_H__BF98493C_AE1F_4C52_95B7_A4A03AA87931C4F2__INCLUDED_

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
{	BuySellStatus	buySell[5];			// stock buySell is in buySell[3]
	float			cost[5];			// stock price is in cost[3]
	float			qty[5];
	float			strike[4];
	OptionType		putCall[4];
	// enable the following to support calendar spreads and BlackScholes plots
//	float			volatility;			// the stock's volatility
//	long			exp[3];				// compressed date of the options' expiration
//	long			evalDate;			// compressed date of option evaluation
} OneStk4OptStrategy;

	// the following is used in the HedgeAnalyzer (definite links to the database)
typedef struct
{	long			def_ID[5];			// stockDef_ID at index 4
	float			qty[5];				// stockQty at index 4
	BuySellStatus	buySell[5];			// stock buySell at index 4	
} OneStk3OptHedge;
//---------------------------------------------------------------------------
#endif