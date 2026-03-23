#if !defined(AFX_BOSLISTCTRL_H__C6763927_944D_4A3D_B2E4_D8D1D435495A__INCLUDED_)
#define AFX_BOSLISTCTRL_H__C6763927_944D_4A3D_B2E4_D8D1D435495A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BOSListCtrl.h : header file

#include "RedBlackListCtrl.h"

//class	CPositionDisplayOptions;

/////////////////////////////////////////////////////////////////////////////
// CBOSListCtrl window

class CBOSListCtrl : public CRedBlackListCtrl
{
	DECLARE_DYNAMIC( CBOSListCtrl )

// Construction
public:
	CBOSListCtrl();

// Operations
public:

protected:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBOSListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBOSListCtrl();
	void	DeleteTotalRow( void );

		// overrides
	BOOL	DeleteItem( int iItem );
	void	DeleteAllColumns( void );
	void	DeleteAllItems( void );

		// Generated message map functions
protected:
	//{{AFX_MSG(CBOSListCtrl)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Attributes
protected:
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_BOSLISTCTRL_H__C6763927_944D_4A3D_B2E4_D8D1D435495A__INCLUDED_)
