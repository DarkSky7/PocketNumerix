#if !defined(AFX_TREELISTCTRL_H__36C21A6B_051D_11D3_96E7_000000000000__INCLUDED_)
#define AFX_TREELISTCTRL_H__36C21A6B_051D_11D3_96E7_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreeListCtrl.h : header file
//

#include "NillaHedge.h"
#include "PortfolioCommon.h"
//#include "RedBlackListCtrl.h"

class	CBOS;

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrl window

class CTreeListCtrl: public CListCtrl
{
	// Construction
public:
			CTreeListCtrl();
	BOOL	Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID );

	// Attributes
public:

	// Operations
public:
	int		AddItem( PortfolioListItemInfo* plii );			// stores plii in the TreeListCtrl
					
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeListCtrl)
	protected:
	virtual void	PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual		~CTreeListCtrl();
    void		OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult ); 

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeListCtrl)
	afx_msg void	OnClick( NMHDR* pNMHDR, LRESULT* pResult );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	int		InsertItem( PortfolioListItemInfo* plii, int iItem );	// displays plii in the ListCtrl
	void	Collapse( int prdi, int selIdx );
	int		ExpandContract( int prdi, int pIdx, bool expand );

protected:
	CImageList		m_ilState;
	CPtrArray		m_RowData;			// all possible ListCtrl entries
//	CPtrArray		m_Bonds;			// RowData indices of Bond Summaries
	CPtrArray		m_Options;			// RowData indices of Option Summaries
	CPtrArray		m_Stocks;			// RowData indices of Stock Summaries
	CPtrArray		m_Portfolio;		// RowData indices of Instrument Summaries
	int				m_ItemCount;		// bumped up by AddItem, ignored by InsertItem
//	bool			m_SuppressPositions;
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
//	CMapPtrToPtr	m_ItemsNotDisplayed;
//	PortfolioListItemInfo*		PrevItem( int iItem, bool& visible );	// stores plii in the TreeListCtrl

#endif // !defined(AFX_TREELISTCTRL_H__36C21A6B_051D_11D3_96E7_000000000000__INCLUDED_)
