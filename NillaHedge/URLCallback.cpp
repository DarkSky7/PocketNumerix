//////////////////////////////////////////////////////////////////////
//
// URLCallback.cpp: implementation of the IBindStatusCallback class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "URLCallback.h"
//#include <shlwapi.h>					// for StrFormatByteSize()

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CURLCallback::CURLCallback( CProgressCtrl* prgCtrl ) : m_pProgCtrl(prgCtrl)
{
	m_bAbort = false;
}

CURLCallback::~CURLCallback()
{
}
//---------------------------------------------------------------------------------
HRESULT CURLCallback::OnProgress( ULONG ulProgress,   ULONG ulProgressMax,
								  ULONG ulStatusCode, LPCWSTR wszStatusText )
{		// Did the user hit the Stop button?
	if ( m_bAbort )
		return	E_ABORT;

		// Report progress to the CProgressCtrl
	if ( m_pProgCtrl != NULL )
	{		// condition incoming ProgressMax
		if ( ulProgressMax == 0 )
			ulProgressMax = 0xFFFFFFFF;						// max int
		double	ratioProgress = (double)ulProgress / (double)ulProgressMax;

			// modify the progress control's position
		int	minPos, maxPos;
		m_pProgCtrl->GetRange( minPos, maxPos );
		m_pProgCtrl->SetPos( minPos + (int)(ratioProgress * maxPos) );
	}
	return	S_OK;
}
//---------------------------------------------------------------------------------
