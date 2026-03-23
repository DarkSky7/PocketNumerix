// YieldCurveFitter.h : main header file for the YIELDCURVEFITTER application
//

#if !defined(AFX_YIELDCURVEFITTER_H__65CB994D_BC38_4505_A8D5_9C498608360A__INCLUDED_)
#define AFX_YIELDCURVEFITTER_H__65CB994D_BC38_4505_A8D5_9C498608360A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "ShortRateCalibration.h"
#include "ServerRates.h"

/////////////////////////////////////////////////////////////////////////////
// CYieldCurveFitterApp:
// See YieldCurveFitter.cpp for the implementation of this class
//

class CYieldCurveFitterApp : public CWinApp
{
public:
	CYieldCurveFitterApp();
	~CYieldCurveFitterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CYieldCurveFitterApp)
	public:
	virtual BOOL	InitInstance( void );
	virtual int		ExitInstance( void );
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CYieldCurveFitterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	unsigned long	dv;			// RAPI DeviceID
	char*			na;			// licensed user '<first name> <last name>'
	char*			em;			// licensed user email
	BYTE*			uu;			// device uuid
	BYTE*			hu;			// hashed uuid on WM5, zero elsewhere
	char*			lpServerName;
	wchar_t*		lpFileName;
	char*			urlRequest;

protected:
	void				ExitPrep( short code );
	HANDLE				hMutex;
//	NOTIFYICONDATA		nid;
};

const LRESULT	WM_YIELDCURVEFITTER_ALREADY_RUNNING
                 = RegisterWindowMessage( _T("Is YieldCurveFitter already running?") );
const LRESULT	WM_YIELDCURVEFITTER_PARSE_DATA
                 = RegisterWindowMessage( _T("YieldCurveFitter Parse Data") );

//wchar_t*		MakeWideString( const char* src );

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_YIELDCURVEFITTER_H__65CB994D_BC38_4505_A8D5_9C498608360A__INCLUDED_)
/*
CString			EuroFormat( COleDateTime aDate );
COleDateTime	unpackOleDate( long	packedOleDate );
long			packOleDate( COleDateTime aDate );

char*			GetThinString( CString cs );
CString		GetSerialNumber( void );
CString			GetOsVersion( void );
char*			MakeHexString( BYTE* uuid, DWORD& len );
char 			AnsiCodeToHexChar( BYTE code );
*/
