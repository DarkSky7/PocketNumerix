#if !defined(AFX_OPTIONCHAINCOMMON_H__BF98493C_AE1F_4C52_95B7_A4A03AA8C4F2__INCLUDED_)
#define      AFX_OPTIONCHAINCOMMON_H__BF98493C_AE1F_4C52_95B7_A4A03AA8C4F2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


// for OptionChainDialog and OptionChainListCtrl use...
typedef struct
{	bool		isCall;			// false -> Put
	float		strike;
	long		expiry;			// expiry = (yr << 4) + month
	CString		symbol;			// without the I ?
	float		last;
	float		change;
	float		bid;
	float		ask;
	long		volume;
	long		openInt;
	short		idx;			// the original row number in the OptionsList 
} OptionChainItemInfo;
//------------------------------------------------------------------------------------------
#endif	// !defined(AFX_OPTIONCHAINCOMMON_H__BF98493C_AE1F_4C52_95B7_A4A03AA8C4F2__INCLUDED_)