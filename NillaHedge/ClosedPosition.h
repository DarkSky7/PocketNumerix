#if !defined(AFX_CLOSEDPOSITION_H__4EAA30DB_6196_4A50_A365_DCECBFB7A57E__INCLUDED_)
#define AFX_CLOSEDPOSITION_H__4EAA30DB_6196_4A50_A365_DCECBFB7A57E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ClosedPosition.h : header file
//
#include "Position.h"
//#include "OleDateTimeEx.h"

/////////////////////////////////////////////////////////////////////////////
// CClosedPosition document

class CClosedPosition : public CPosition
{
public:
	CClosedPosition( CPosition& pos );
	CClosedPosition();           // protected constructor used by dynamic creation

		// protected constructor used by dynamic creation
protected:
	DECLARE_SERIAL( CClosedPosition )

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClosedPosition)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CClosedPosition();
#ifdef _DEBUG
	virtual void AssertValid() const;
//	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	COleDateTime	dateClosed;
	double			netProceeds;
	CString			closingNote;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLOSEDPOSITION_H__4EAA30DB_6196_4A50_A365_DCECBFB7A57E__INCLUDED_)
