// PNMX_Installer.h : main header file for the PROJECT_NAME application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "StartRapi.h"

const LRESULT	WM_PNMX_INSTALLER_ALREADY_RUNNING
                 = RegisterWindowMessage( _T("Is PNMX_Installer already running?") );

// CPNMX_InstallerApp:
// See PNMX_Installer.cpp for the implementation of this class
//

class CPNMX_InstallerApp : public CWinApp
{
public:
	CPNMX_InstallerApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int	 ExitInstance();

// Implementation
	DECLARE_MESSAGE_MAP()

		// prevent multiple instances
	HANDLE	hMutex;
public:
};
//-----------------------------------------------------------------------------------------------
	// GetToken skips initial white
	// it stops only at a terminator if one is supplied or
	// if no terminator is supplied, it stops at the first whitespace
char*		GetToken( char* buf, int& ii, char term = '\0' );
char*		GetThinString( CString cs );
wchar_t*	MakeWideString( const char* src );
void		FormatMemStr( int byteCount, wchar_t* wbuf, DWORD maxChars );
//short		LoadBOMcache( wchar_t* deskTempPath, char*& cache );				// moved into the dialog
char*		NumToBase62( unsigned int num );				// caller must delete return value
char*		MakeRanName( void );							// caller must delete return value

extern		CPNMX_InstallerApp theApp;
//-----------------------------------------------------------------------------------------------
inline bool	iswhite( char ch )
{	return	ch == '\n'  ||  ch == '\r'  ||  ch == '\t'  ||  ch == ' ';
}			// iswhite()
//-----------------------------------------------------------------------------------------------
