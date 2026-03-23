// NillaHedgeView.h : interface of the CNillaHedgeView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_NILLAHEDGEVIEW_H__0F434193_096C_4879_9492_10F2FAFBACDD__INCLUDED_)
#define AFX_NILLAHEDGEVIEW_H__0F434193_096C_4879_9492_10F2FAFBACDD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CNillaHedgeView : public CView
{
protected: // create from serialization only
	CNillaHedgeView();
	DECLARE_DYNCREATE(CNillaHedgeView)

// Attributes
public:
	CNillaHedgeDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNillaHedgeView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNillaHedgeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
//	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CNillaHedgeView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnSetFocus( CWnd* pOldWnd );
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in NillaHedgeView.cpp
inline CNillaHedgeDoc* CNillaHedgeView::GetDocument()
   { return (CNillaHedgeDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NILLAHEDGEVIEW_H__0F434193_096C_4879_9492_10F2FAFBACDD__INCLUDED_)
