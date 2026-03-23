// NillaHedgeView.cpp : implementation of the CNillaHedgeView class
//

#include "StdAfx.h"
#include "NillaHedge.h"

#include "NillaHedgeDoc.h"
#include "NillaHedgeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNillaHedgeView

IMPLEMENT_DYNCREATE(CNillaHedgeView, CView)

BEGIN_MESSAGE_MAP(CNillaHedgeView, CView)
	//{{AFX_MSG_MAP(CNillaHedgeView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNillaHedgeView construction/destruction

CNillaHedgeView::CNillaHedgeView()
{
	// TODO: add construction code here

}

CNillaHedgeView::~CNillaHedgeView()
{
}

BOOL	CNillaHedgeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CNillaHedgeView drawing

void CNillaHedgeView::OnDraw(CDC* pDC)
{
//	CNillaHedgeDoc* pDoc = GetDocument();
//	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CNillaHedgeView diagnostics

#ifdef _DEBUG
void CNillaHedgeView::AssertValid() const
{
	CView::AssertValid();
}
//--------------------------------------------------------------------------
/*
void CNillaHedgeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
*/
//--------------------------------------------------------------------------
CNillaHedgeDoc* CNillaHedgeView::GetDocument() // non-debug version is inline
{
	ASSERT( m_pDocument->IsKindOf(RUNTIME_CLASS(CNillaHedgeDoc)) );
	return (CNillaHedgeDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CNillaHedgeView message handlers
int CNillaHedgeView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	int	iResult = CView::OnCreate(lpCreateStruct);
//	if ( iResult == 0 )
//		ShowDoneButton(TRUE);

	return iResult;
}			// OnCreate()
//--------------------------------------------------------------------------
void CNillaHedgeView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);
	//::SHSipPreference(m_hWnd, SIP_UP);
}			// OnSetFocus()
//--------------------------------------------------------------------------
void CNillaHedgeView::OnDestroy()
{
	//::SHSipPreference(m_hWnd, SIP_FORCEDOWN);
	CView::OnDestroy();
}			// OnDestroy()
//--------------------------------------------------------------------------
BOOL	CNillaHedgeView::Create(
	LPCTSTR			lpszClassName,
	LPCTSTR			lpszWindowName,
	DWORD			dwStyle,
	const			RECT& rect,
	CWnd*			pParentWnd,
	UINT			nID,
	CCreateContext*	pContext		)
{
	return CWnd::Create( lpszClassName, lpszWindowName,
						 dwStyle, rect, pParentWnd, nID, pContext );
}			// Create()
//--------------------------------------------------------------------------
