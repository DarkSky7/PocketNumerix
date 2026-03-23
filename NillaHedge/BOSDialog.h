#if !defined(AFX_BOSDIALOG_H__014AAF93_4578_4548_AA0D_428965FF99F4__INCLUDED_)
#define AFX_BOSDIALOG_H__014AAF93_4578_4548_AA0D_428965FF99F4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BOSDialog.h : header file
//
#include "NillaDialog.h"
#include "NillaHedge.h"
//#include "BOSDatabase.h"

//class CBond;
class COption;
class CStock;
class CPosition;

extern	CNillaHedgeApp	theApp;

/////////////////////////////////////////////////////////////////////////////
// CBOSDialog dialog

class CBOSDialog : public CNillaDialog
{
	DECLARE_DYNAMIC( CBOSDialog )

		// Construction
public:
	CBOSDialog( int idd, CWnd* pParent = NULL );

// Dialog Data

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBOSDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_MSG(CBOSDialog)
	//}}AFX_MSG
//DECLARE_MESSAGE_MAP()

// Implementation
protected:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BOSDIALOG_H__014AAF93_4578_4548_AA0D_428965FF99F4__INCLUDED_)
