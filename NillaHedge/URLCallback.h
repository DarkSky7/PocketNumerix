//////////////////////////////////////////////////////////////////////
//
// URLCallback.h: interface for the IBindStatusCallback class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_URLCALLBACK_H__6B469ECE_B785_11D3_8D3B_D5CFB868D41C__INCLUDED_)
#define AFX_URLCALLBACK_H__6B469ECE_B785_11D3_8D3B_D5CFB868D41C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <Ole2.h>
#include <Rpc.h>
#include <Rpcndr.h>
#include <Urlmon.h>
#include <Windows.h>

//#pragma warning(disable:4100)   // disable warnings about unreferenced params

class CURLCallback : public IBindStatusCallback
{
public:
	CURLCallback( CProgressCtrl* prgCtrl );
	~CURLCallback();

		// IBindStatusCallback methods.  Note that the only method called by IE
		// is OnProgress(), so the others just return E_NOTIMPL.
	STDMETHOD(OnStartBinding)(
		/* [in] */ DWORD dwReserved,
		/* [in] */ IBinding *pib)
		{ return E_NOTIMPL; }

	STDMETHOD(GetPriority)(
		/* [out] */ LONG *pnPriority)
		{ return E_NOTIMPL; }

	STDMETHOD(OnLowResource)(
		/* [in] */ DWORD reserved)
		{ return E_NOTIMPL; }

	STDMETHOD(OnProgress)(
		/* [in] */ ULONG ulProgress,
		/* [in] */ ULONG ulProgressMax,
		/* [in] */ ULONG ulStatusCode,
		/* [in] */ LPCWSTR wszStatusText);

	STDMETHOD(OnStopBinding)(
		/* [in] */ HRESULT hresult,
		/* [unique][in] */ LPCWSTR szError)
		{ return E_NOTIMPL; }

	STDMETHOD(GetBindInfo)(
		/* [out] */ DWORD *grfBINDF,
		/* [unique][out][in] */ BINDINFO *pbindinfo)
		{ return E_NOTIMPL; }

	STDMETHOD(OnDataAvailable)(
		/* [in] */ DWORD grfBSCF,
		/* [in] */ DWORD dwSize,
		/* [in] */ FORMATETC *pformatetc,
		/* [in] */ STGMEDIUM *pstgmed)
		{ return E_NOTIMPL; }

	STDMETHOD(OnObjectAvailable)(
		/* [in] */ REFIID riid,
		/* [iid_is][in] */ IUnknown __RPC_FAR *punk)
		{ return E_NOTIMPL; }

		// IUnknown methods.  Note that IE never calls any of these methods,
		// because the caller owns the IBindStatusCallback interface
	STDMETHOD_(ULONG,AddRef)()
		{ return 0; }

	STDMETHOD_(ULONG,Release)()
		{ return 0; }

	STDMETHOD(QueryInterface)(
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
		{ return E_NOTIMPL; }

		// settable by caller
	bool			m_bAbort;

protected:
		// output controls
	CProgressCtrl*	m_pProgCtrl;		// graphical output
};
#pragma warning(default:4100)

#endif // !defined(AFX_URLCALLBACK_H__6B469ECE_B785_11D3_8D3B_D5CFB868D41C__INCLUDED_)
