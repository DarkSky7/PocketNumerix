#if !defined(AFX_POSITIONSDIALOG_H__9BF113A8_4B47_4504_AC0F_6266F577F0B5__INCLUDED_)
#define AFX_POSITIONSDIALOG_H__9BF113A8_4B47_4504_AC0F_6266F577F0B5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PositionsDialog.h : header file
//
#include "NillaDialog.h"
#include "BOSListCtrl.h"
#include "NumEdit.h"
#include "SeekComboBox.h"
#include "Position.h"				// includes PositionStatus definition
//#include "OleDateTimeEx.h"

class	CBOS;

//class CPositionDisplayOptions;

/////////////////////////////////////////////////////////////////////////////
// CPositionsDialog dialog

int	CALLBACK	BOSPositionCompareFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort );

	// the following struct supports the third LPARAM
	// value in the CompareFunc when sorting Bond/Option/Stock PositionLists
typedef struct
{	CObject*		aBOS;
	short			sortKey;
} SortInfo;

	// the following struct is used by the global BOSPositionCompareFunc()
	// so it cannot be scoped by the PositionsDialog class
typedef struct
{
	CPosition*		thePosition;
	double			income;			// a Stock's divIncome, a Bond's cpnIncome
	double			pctYield;		// Stocks & Bonds only
	double			mktValue;		// new 3
	double			capGain;		// new 4
} BOSPositionListItemInfo;

const short	maxCols = 10;			// includes 9 working columns plus a blank column on the right

class CPositionsDialog : public CNillaDialog
{
	DECLARE_DYNAMIC( CPositionsDialog )

// Construction
public:
	CPositionsDialog(CWnd* pParent = NULL);   // standard constructor
	~CPositionsDialog( void );

// Dialog Data
	//{{AFX_DATA(CPositionsDialog)
	enum { IDD = IDD_PositionsDialog };
	CButton			c_Bond;
	CButton			c_Option;
	CButton			c_Stock;
	CButton			c_Enter;
	CButton			c_Modify;
	CButton			c_Close;
	CButton			c_Delete;
	CSeekComboBox	c_Symbol;
	CDateTimeCtrl	c_PurchaseDate;
	CNumEdit		c_Units;
	CNumEdit		c_TotalCost;
	CEdit			c_Note;
	CBOSListCtrl	c_PositionsList;
	double			m_TotalCost;
	CString			m_Symbol;
	COleDateTime	m_PurchaseDate;
	CString			m_Note;
	double			m_Units;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPositionsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);				// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	CPosition*		AddPosition( CPosition*	pos = NULL);

protected:
	void			EnableEnterButton( void );
	void			EnablePositionButtons( void );
	CMapPtrToPtr*	GetPositionList( bool create = false );
	CString			GetPositionItemText( int rr, int cc );
	void			LoadSymbols( void );
	void			LoadTheAsset( void );
	int				LoadOnePosition( CPosition* pos );
	int				LoadColumnTotals( void );
	void			ShowTotalCost( void );
		// Open --> Edit it;	Deleted --> delete it;	Closed --> close it
	void			DeleteOrModifyPosition( int iItem, PositionStatus stat );

		// BOSPositionList shared functions...
	void			EditPosition( CPosition* thePosition );
	void			InitListCtrl( void );
	void			InsertColumns( void );
	short			MapSubitem( short subitem )	{	return colMap[subitem];   }
	void			KillFocusSymbolWork( CString sym );
	void			KillFocusPurchaseDateWork( NMHDR* pNMHDR, LRESULT* pResult );

	// Generated message map functions
	//{{AFX_MSG(CPositionsDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnBond();
	afx_msg void OnOption();
	afx_msg void OnStock();
	afx_msg void OnKillFocusPurchaseDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusUnits();
	afx_msg void OnKillFocusTotalCost();
	afx_msg void OnEnter();
	afx_msg void OnDelete();
	afx_msg void OnGetDispInfoPositionsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblClickPositionsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangingPositionsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnClickPositionsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	afx_msg void OnModify();
	afx_msg void OnClickPositionsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusPositionsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetFocusUnits();
	afx_msg void OnSetFocusTotalCost();
	afx_msg void OnSetFocusNote();
	afx_msg void OnSetFocusPositionsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChangeSymbol();
	afx_msg void OnKillFocusNote();
	afx_msg void OnDeleteDefinition();
	afx_msg void OnCloseUpPurchaseDate(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnKillFocusSymbol();											// HotKeys +
	afx_msg void OnSetFocusSymbol();											// HotKeys
	//}}AFX_MSG
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);		// HotKeys
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);						// HotKeys

	afx_msg void OnCustomDrawPositionsList( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg LRESULT OnMarqueeEnd( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()

public:
//	AssetType	instrument;
	CBOS*		theAsset;
	CObArray*	itsPositions;

protected:
		// lastSortKey is a 1-based column number (so its sign can be flipped)
		// if positive --> the last sort was ascending;
		// if negative --> the last sort was descending
	DWORD		m_activeComboBox;												// HotKeys
	short		lastSortKey;		// track most recent column sort
		// the order of optns is:
		//		purchaseDate, numUnits, initialCost, marketValue, ...
		// if its an option,     exerciseValue                     appears here
		//		capitalGain, aggIncome, netGain, annNetGain, note
	short		colMap[maxCols];
	double		aggUnits;
	double		aggCost;
	double		aggIncome;
	double		wtdYield;
public:
	afx_msg void OnSetFocusPurchaseDate(NMHDR *pNMHDR, LRESULT *pResult);
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSITIONSDIALOG_H__9BF113A8_4B47_4504_AC0F_6266F577F0B5__INCLUDED_)
//	void		DeleteSelectedPositions( void );
//	void		enableDialogItems( bool enDis );	// not needed if Stock is the default
