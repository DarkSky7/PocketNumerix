// StrategyExplorer.h : main header file for the STRATEGYEXPLORER application
//
#if !defined(AFX_STRATEGYEXPLORER_H__63800993_83C3_467D_AFE4_EEB625F2CC88__INCLUDED_)
#define AFX_STRATEGYEXPLORER_H__63800993_83C3_467D_AFE4_EEB625F2CC88__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"				// main symbols
#include "OneStk4OptStrategy.h"
#include <iostream>
#include <fstream>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CStrategyExplorerApp:
// See StrategyExplorer.cpp for the implementation of this class
//
const LRESULT	WM_STRATEGYEXPLORER_ALREADY_RUNNING
                 = RegisterWindowMessage( _T("Is StrategyExplorer already running?") );

class CStrategyExplorerApp : public CWinApp
{
public:
	CStrategyExplorerApp();
	~CStrategyExplorerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStrategyExplorerApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CStrategyExplorerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

		// new stuff... background color, notify icon
		// XXX - is this in the right place, say perhaps the dialog
	HBRUSH 		OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );		// MFC version never gets called
	LRESULT		OnNotifyIcon( WPARAM wParam, LPARAM lParam );
	HICON		m_hIcon;

public:
	unsigned long	dv;			// RAPI DeviceID
	char*			na;			// licensed user '<first name> <last name>'
	char*			em;			// licensed user email
	BYTE*			uu;			// device uuid
	BYTE*			hu;			// hashed uuid on WM5, zero elsewhere
	DWORD			m_osVer;

protected:
	HANDLE			hMutex;
	void			ExitPrep( short code );
};
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	bool		GetRecentStrategy( OneStk4OptStrategy& os4os );
	bool		SetRecentStrategy( OneStk4OptStrategy& os4os );

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STRATEGYEXPLORER_H__63800993_83C3_467D_AFE4_EEB625F2CC88__INCLUDED_)
