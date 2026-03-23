// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F7AF1891_8CEA_4358_8C19_58B0E0072D97__INCLUDED_)
#define AFX_STDAFX_H__F7AF1891_8CEA_4358_8C19_58B0E0072D97__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if (_WIN32_WCE <= 200)
#error This project does not support MFCCE 2.00 or earlier, because it requires CControlBar, available only in MFCCE 2.01 or later
#endif

#if ( _WIN32_WCE <= 211 )
#error This project can not be built for H/PC Pro 2.11 or earlier platforms.
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#include <afxsock.h>		// MFC socket extensions
#include <afxext.h>			// CCommandBar ?

#if defined(_AFXDLL)
#include <AfxDtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <AfxCmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//#include <afxsock.h>		// MFC socket extensions

#define PPC02_TARGET	( _WIN32_WCE  < 0x400 )		// might have to set these to 1 or 0 
#define PPC03_TARGET	( _WIN32_WCE >= 0x400  &&  _WIN32_WCE < 0x500 )
#define WM5_TARGET		( _WIN32_WCE >= 0x500 )		// ditto
#define	SEH_EXCEPTIONS	PPC02_TARGET				// define SEH_EXCEPTIONS = nonzero for EVT3 or 0 for VS8

#if	( PPC03_TARGET  ||  WM5_TARGET )
#include <ATLComTime.h>					// VS8's location for COleDateTime
#endif

//#define	CheckLicense

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F7AF1891_8CEA_4358_8C19_58B0E0072D97__INCLUDED_)
