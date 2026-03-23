#include "StdAfx.h"
#include "RapiCtx.h"

/*
typedef struct RapiCtx
{
	CButton*			pFetchButton;
	CButton*			pDeviceConnectedButton;
	CStatic*			pStatusResultStatic;

	RAPIINIT*			pRapiStruct;
	unsigned short*		pButtonState;
	BOOL*				pDeviceConnectedState;
*/

DWORD	WINAPI	StartRapi( void* pThreadCtx )
{
	RapiCtx_T*	pCtx = (RapiCtx*)pThreadCtx;
	RAPIINIT*	pRapi_T = pCtx->pRapiInitStruct;
	short	siz = sizeof( RAPIINIT );
	ZeroMemory( pRapi_T, siz );
	rapiStruct.cbSize = siz;
	HRESULT	hr = CeRapiInitEx( &rapiStruct );
	if ( SUCCEEDED(hr) )
	{
		c_StatusResult.SetWindowTextW( _T("Device Connected.") );
		c_DeviceConnected.SetCheck( TRUE );
		m_DeviceConnected = TRUE;
		c_FirstName.EnableWindow( TRUE );
		c_LastName.EnableWindow( TRUE );
		c_EmailAddress.EnableWindow( TRUE );
		c_InvoiceNumber.EnableWindow( TRUE );
		c_InvoiceDate.EnableWindow( TRUE );
		m_ButtonState = 1;					// Device is connected
	}
	else
	{
		c_FetchButton.SetWindowTextW( _T("Connect") );
		c_FetchButton.EnableWindow( TRUE );
		c_StatusResult.SetWindowTextW( _T("Device not connected?") );
	}

	pRapiCtx->
}