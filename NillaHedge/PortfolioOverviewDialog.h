#if !defined(AFX_PORTFOLIOOVERVIEWDIALOG_H__B7352E9Z_868F_4CC1_A00C_210DC4F94D1D__INCLUDED_)
#define AFX_PORTFOLIOOVERVIEWDIALOG_H__B7352E9Z_868F_4CC1_A00C_210DC4F94D1D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PortfolioOverviewDialog.h : header file
//

#include "NillaDialog.h"
#include "PortfolioCommon.h"
#include "BOS.h"
#include "NillaHedge.h"
#include "TreeListCtrl.h"
const	short	nCols = 11;

/////////////////////////////////////////////////////////////////////////////
// CPortfolioOverviewDialog dialog

class CPortfolioOverviewDialog : public CNillaDialog
{
public:
	DECLARE_DYNAMIC( CPortfolioOverviewDialog )

// Construction
public:
	CPortfolioOverviewDialog( CWnd* pParent = NULL );   // standard constructor
	~CPortfolioOverviewDialog( void );

// Dialog Data
	//{{AFX_DATA(CPortfolioOverviewDialog)
	enum { IDD = IDD_PortfolioNavigatorDialog };
	CTreeListCtrl	c_PositionsList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPortfolioOverviewDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
		// Generated message map functions
	//{{AFX_MSG(CPortfolioOverviewDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetDispInfoPositionsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	afx_msg void OnCustomDrawPositionsList( NMHDR* pNMHDR, LRESULT* pResult );
	DECLARE_MESSAGE_MAP()

protected:
	void						InitListCtrl( void );
	void						InsertColumns( void );
	void						LoadRows( void );
	PortfolioListItemInfo*		LoadPositions( CBOS* inst );
	CString						GetPositionItemText( int rr, int cc );

private:
	short		MapSubitem( short subitem )	{	return colMap[subitem];   }

		// lastSortKey is a 1-based column number (so its sign can be flipped)
		// if positive --> the last sort was ascending;
		// if negative --> the last sort was descending
	short		lastSortKey;		// track most recent column sort
		// the order of optns is:
		//		purchaseDate, numUnits, initialCost, marketValue, ...
		// if its an option,     exerciseValue                     appears here
		//		capitalGain, aggIncome, netGain, annNetGain, note
	short		colMap[nCols];
	CPtrArray	rowData;
	bool		initiallyShowPositions;
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_PORTFOLIOOVERVIEWDIALOG_H__B7352E9F_868F_4CC1_A00C_210DC4F94D1D__INCLUDED_)
