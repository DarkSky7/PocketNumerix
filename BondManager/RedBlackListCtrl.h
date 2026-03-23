#if !defined(AFX_REDBLACKLISTCTRL_H__AAE43800_D175_4969_BDC1_7F006D32D5F8__INCLUDED_)
#define AFX_REDBLACKLISTCTRL_H__AAE43800_D175_4969_BDC1_7F006D32D5F8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RedBlackListCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRedBlackListCtrl window

	// notifications sent to parent
#define WM_MarqueeEnd ( WM_USER + 1 )			// OnMouseMove() sends this

class CRedBlackListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC( CRedBlackListCtrl )

		// Construction
public:
	CRedBlackListCtrl();

		// Attributes


		// Operations
protected:

		// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRedBlackListCtrl)
	//}}AFX_VIRTUAL

		// Implementation
public:
	virtual ~CRedBlackListCtrl();
	int		AddItem( LPARAM pItem, int iItemNo );
    void	OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult ); 

		// Generated message map functions
protected:
	bool	m_bTracking;
	int		startItem;
	int		recentItem;

	//{{AFX_MSG(CRedBlackListCtrl)
	//}}AFX_MSG
	afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
//	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
	DECLARE_MESSAGE_MAP()

public:
/*
	class CSortHeaderCtrl : public CHeaderCtrl
	{
	public:		
		CSortHeaderCtrl();
		virtual ~CSortHeaderCtrl()					{}
		void	SetSortedColumn( int nCol )			{  m_iSortColumn = nCol;			}
		void	SetSortAscending( bool bAscending ) {  m_bSortAscending = bAscending;	}
		void	ComplementAscending( void )
					{  m_bSortAscending = m_bSortAscending ? false : true;				}
		bool	IsSortAscending() const				{  return m_bSortAscending;			}
		int		GetSortedColumn() const				{  return m_iSortColumn;			}
		void	UpdateSortArrow();

	protected:
		void	DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
		int		m_iSortColumn;
		bool	m_bSortAscending;
	};
*/
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REDBLACKLISTCTRL_H__AAE43800_D175_4969_BDC1_7F006D32D5F8__INCLUDED_)
