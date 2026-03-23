#if !defined(AFX_OptionType_H__BF98493C_AE1F_4C52_95B7_A4A03AKQXZA87931C4F2__INCLUDED_)
#define      AFX_OptionType_H__BF98493C_AE1F_4C52_95B7_A4A03AKQXZA87931C4F2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'StdAfx.h' before including this file for PCH
#endif


enum OptionType					// used by:  AbstractHedge, Option
{	UnknownOptionType	= 0,
	Put					= 1,
	Call				= 2
};
#endif // !defined(AFX_OptionType_H__BF98493C_AE1F_4C52_95B7_A4A03AKQXZA87931C4F2__INCLUDED_)