#pragma once

#include "PNMX_InstallerDlg.h"

const LRESULT	WM_PNMX_INSTALLER_DEVICE_CONNECTED
                 = RegisterWindowMessage( _T("Has PNMX_Installer connected to a device through RAPI?") );

DWORD	WINAPI	InitDeviceConnectedMsg( void* pThreadCtx );

//HRESULT	InitRapi( HANDLE hExit );
