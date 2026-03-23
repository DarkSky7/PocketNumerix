// RapiCtx.h
#pragma once

typedef struct RapiCtx
{
	CButton*			pFetchButton;
	unsigned short*		pButtonState;
	BOOL*				pDeviceConnected;
	CStatic*			pStatusResult;
	RAPIINIT*			pRapiStruct;
}	RapiCtx_T;
