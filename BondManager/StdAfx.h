// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C8EC58ED_6290_4DC7_85EF_A97045ADEC63__INCLUDED_)
#define AFX_STDAFX_H__C8EC58ED_6290_4DC7_85EF_A97045ADEC63__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#if defined(_AFXDLL)
#include <AfxDtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <AfxCmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <ceconfig.h>
#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#define SHELL_AYGSHELL
#endif

#ifdef SHELL_AYGSHELL
#include <aygshell.h>
#pragma comment(lib, "aygshell.lib") 
#endif // SHELL_AYGSHELL

//#include <afxsock.h>		// MFC socket extensions

//#define	_DEBUG			1
//#define _WIN32_WCE		1		// it's getting defined now...
//#ifdef _DEBUG
//#include <AfxRes.h>
//#endif

#define PPC02_TARGET	( _WIN32_WCE < 0x420 )								// might have to set these to 1 or 0 
#define PPC03_TARGET	( _WIN32_WCE >= 0x420  &&  _WIN32_WCE < 0x500 )		// ditto
#define WM5_TARGET		( _WIN32_WCE >= 0x500 )								// ditto
#define	SEH_EXCEPTIONS	( PPC02_TARGET )		// define SEH_EXCEPTIONS for EVT3 or CPP_EXCEPTIONS for VS8

#if	( WM5_TARGET )
// EVT3 always complains about not finding ATLComTime.h regardless of the value of WIN32_PLATFORM_PSPC
#include <ATLComTime.h>		// VS8's location for COleDateTime
#endif

//{{AFX_INSERT_LOCATION}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C8EC58ED_6290_4DC7_85EF_A97045ADEC63__INCLUDED_)
