// PositionStatus.h

#if !defined(AFX_PositionStatus_H__BF98493C_AE1F_4C52_95B7_A4A03AA87931C4F2__INCLUDED_)
#define      AFX_PositionStatus_H__BF98493C_AE1F_4C52_95B7_A4A03AA87931C4F2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'StdAfx.h' before including this file for PCH
#endif

enum PositionStatus
{	UnknownPositionStatus	=  0,
	OpenPosition			=  1,
	ClosedPosition			=  2,
	DeletedPosition			=  3
};

#endif