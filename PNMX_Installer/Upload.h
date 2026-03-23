// Upload.h
#include "StdAfx.h"
#include "PNMX_InstallerDlg.h"

//DWORD	WINAPI	UploadInstallCleanup( void* pThreadCtx );
short	DeviceUpload( void* ctx, wchar_t* progName, wchar_t* deskFileName, 
					  unsigned char* mime, unsigned long fileSize,
					  wchar_t* deviceFileName, CStatic* statusResult, FILETIME* creationTime );

#define		UpldCtxProgNameSize			 16
#define		UpldCtxDeskFileNameSize		256
#define		UpldCtxDeviceFileNameSize	256
#define		UpldCtxTestPathSize			256

typedef struct UploadCtx
{
	CPNMX_InstallerDlg*		p_PID;
	wchar_t		progName[ UpldCtxProgNameSize ];
	wchar_t		deskFileName[ UpldCtxDeskFileNameSize ];			// deskFile
	wchar_t		deviceFileName[ UpldCtxDeviceFileNameSize ];		// deviceFile
	wchar_t		testPath[ UpldCtxTestPathSize ];
	CStatic*	c_StatusResult;								// status 
//	CButton*	c_Button;									// completion check button (not using this anymore)
} UploadCtx_T;
