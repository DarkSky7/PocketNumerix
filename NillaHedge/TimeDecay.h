// TimeDecay.h

#if !defined(AFX_TimeDecay_H__BF98493C_AE1F_4C52_95B7_A4A03AA87931C4F2__INCLUDED_)
#define      AFX_TimeDecay_H__BF98493C_AE1F_4C52_95B7_A4A03AA87931C4F2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'StdAfx.h' before including this file for PCH
#endif

	// used to store the state of the TimeDecayExplorer to the Registry
typedef struct
{	bool			pureBS;	
	bool			optChk[4];			// 4 bits (check box states)
	long			optDef_ID[4];
	bool			optPortf;			// 1 bit (portfolio check box state)
	COleDateTime	endDate;
} TimeDecay;
//---------------------------------------------------------------------------
#endif		// !defined(AFX_TimeDecay_H__BF98493C_AE1F_4C52_95B7_A4A03AA87931C4F2__INCLUDED_)