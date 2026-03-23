// BondRateSensitivity.h

#if !defined(AFX_BondRateSensitivity_H__BF98493C_AE1F_4C52_95B7_A4A03AA87931C4F2__INCLUDED_)
#define      AFX_BondRateSensitivity_H__BF98493C_AE1F_4C52_95B7_A4A03AA87931C4F2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'StdAfx.h' before including this file for PCH
#endif

	// used to store the state of the RateSensitivityExplorer to the Registry
typedef struct
{	long			bndDef_ID[4];
	float			deltaHigh;
	float			deltaLow;
	bool			bndChk[4];				// 4 bits (check box states)
	bool			bndPortf;				// 1 bit (portfolio check box state)
} BondRateSensitivity;
//---------------------------------------------------------------------------
#endif		// !defined(AFX_BondRateSensitivity_H__BF98493C_AE1F_4C52_95B7_A4A03AA87931C4F2__INCLUDED_)