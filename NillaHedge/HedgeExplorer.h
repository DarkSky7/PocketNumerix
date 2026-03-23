#if !defined(AFX_HEDGEEXPLORER_H__F8FECD93_A9D8_4A0E_9BF3_0FEE9FC72FF9__INCLUDED_)
#define AFX_HEDGEEXPLORER_H__F8FECD93_A9D8_4A0E_9BF3_0FEE9FC72FF9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// HedgeExplorer.h : header file
//
#include "PlotDialog.h"
#include "SeekComboBox.h"
#include "OneStk3OptStrategy.h"					// for BuySellStatus too
#include "FPoint.h"
#include "BOS.h"								// for MarketPrice_t
#include "NumEdit.h"

class	COption;
class	CStock;
class	CPolyline;

const	short	nHedgePts = 24;		// # of evaluations for each option, stock & the portfolio
const	short	nHedgeBOS = 4;
const	short	nHedgeOpts  = nHedgeBOS - 1;	// nBOS defined in PlotOptionsDialog.h
const	short	stkIdx		= nHedgeBOS - 1;	// i.e. 3

/////////////////////////////////////////////////////////////////////////////
// CHedgeExplorer dialog

class CHedgeExplorer : public CPlotDialog
{
// Construction
public:
	CHedgeExplorer(CWnd* pParent = NULL);	// standard constructor
	~CHedgeExplorer( void );				// standard destructor
	void	SaveIfChanged( bool updateGUI = false );

// Dialog Data
	//{{AFX_DATA(CHedgeExplorer)
	enum { IDD = IDD_HedgeExplorer };
	CSeekComboBox	c_StockQty;
	CSeekComboBox	c_Option1Qty;
	CSeekComboBox	c_Option2Qty;
	CSeekComboBox	c_Option3Qty;
	CNumEdit		c_RiskFreeRate;
	CDateTimeCtrl	c_EvalDate;
	CSeekComboBox	c_StockSymbol;
	CSeekComboBox	c_Option1Symbol;
	CSeekComboBox	c_Option2Symbol;
	CSeekComboBox	c_Option3Symbol;
	CString			m_StockSymbol;
	CString			m_Option1Symbol;
	CString			m_Option2Symbol;
	CString			m_Option3Symbol;
	COleDateTime	m_EvalDate;
	CButton			c_SellStock;
	CButton			c_SellOption1;
	CButton			c_SellOption2;
	CButton			c_SellOption3;
	CButton			c_BuyStock;
	CButton			c_BuyOption1;
	CButton			c_BuyOption2;
	CButton			c_BuyOption3;
	float			m_Volatility;
	MarketPrice_t	m_StockPrice;
	float			m_RiskFreeRate;
	//}}AFX_DATA

	OneStk3OptHedge		os3oh;
	COption*			pOption[ nHedgeBOS ];
	CStock*				pStock;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHedgeExplorer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void	DrawOption( short idx, CPaintDC& dc, CFPoint* optFpts );
	short	DrawStock( CPaintDC& dc, CPolyline& pl );
	void	DrawSum( CPaintDC& dc, CFPoint* sumFpts );
	bool	InstComplete( short ii );			// idx is 0-based, (nBOS-1) -> stock
	void	KillFocusEvalDateWork( void );

	void	KillFocusStockDisplay( void );
	void	KillFocusStockResetOptions( void );
//	void	KillFocusStockQtyWork( void );
	int		LoadOptionSymbols( void );		// depends on pStock, return indicates how many options exist
	DWORD	m_activeComboBox;

private:
	void	KillFocusOptionSymbolCore( CSeekComboBox* scb, short ii, CString optSym );
	void	KillFocusOptionSymbolWork( CSeekComboBox* scb, short ii, CString& optSym );
	void	SelChangeOptionSymbolWork( CSeekComboBox* scb, short ii, CString& optSym );
	void	KillFocusStockSymbolWork( void );
	void	KillFocusQtyWork( CSeekComboBox* scb, float* qty );
	void	SelChangeQtyWork( CSeekComboBox* scb, float* qty );
	void	KillFocusEditBoxWork( int ctrlID, float* storedFloat );
	void	BuySellWork( CButton* pBuyButton, CButton* pSellButton,
						 BuySellStatus* pBuySellMemory, BuySellStatus bsVal );
	void	SetThreeOptions( CString opt1Sym );
	bool	criticalRegion;

	// Generated message map functions
	//{{AFX_MSG(CHedgeExplorer)
	afx_msg void OnKillFocusStockPrice();
	afx_msg void OnKillFocusVolatility();
	afx_msg void OnBuyOption1();
	afx_msg void OnBuyOption2();
	afx_msg void OnBuyOption3();
	afx_msg void OnBuyStock();
	afx_msg void OnSellOption1();
	afx_msg void OnSellOption2();
	afx_msg void OnSellOption3();
	afx_msg void OnSellStock();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnKillFocusRiskFreeRate();
	afx_msg void OnCloseUpEvalDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusEvalDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnSelChangeOption1Qty();
	afx_msg void OnSelChangeOption2Qty();
	afx_msg void OnSelChangeOption3Qty();
	afx_msg void OnSelChangeStockQty();
	afx_msg void OnSelChangeStockSymbol();
	afx_msg void OnSelChangeOption1Symbol();
	afx_msg void OnSelChangeOption2Symbol();
	afx_msg void OnSelChangeOption3Symbol();

	afx_msg void OnKillFocusOption1Qty();
	afx_msg void OnKillFocusOption2Qty();										// HotKeys +
	afx_msg void OnKillFocusOption3Qty();										// HotKeys +
	afx_msg void OnKillFocusStockQty();											// HotKeys +
	afx_msg void OnKillFocusStockSymbol();										// HotKeys +
	afx_msg void OnKillFocusOption1Symbol();									// HotKeys +
	afx_msg void OnKillFocusOption2Symbol();									// HotKeys +
	afx_msg void OnKillFocusOption3Symbol();									// HotKeys +

	afx_msg void OnSetFocusOption1Qty();										// HotKeys
	afx_msg void OnSetFocusOption2Qty();										// HotKeys
	afx_msg void OnSetFocusOption3Qty();										// HotKeys
	afx_msg void OnSetFocusStockQty();											// HotKeys
	afx_msg void OnSetFocusStockSymbol();										// HotKeys
	afx_msg void OnSetFocusOption1Symbol();										// HotKeys
	afx_msg void OnSetFocusOption2Symbol();										// HotKeys
	afx_msg void OnSetFocusOption3Symbol();										// HotKeys
	//}}AFX_MSG
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);		// HotKeys
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);						// HotKeys

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSetFocusStockPrice();
	afx_msg void OnSetFocusVolatility();
	afx_msg void OnSetFocusRiskFreeRate();
	afx_msg void OnSetFocusEvalDate(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HEDGEEXPLORER_H__F8FECD93_A9D8_4A0E_9BF3_0FEE9FC72FF9__INCLUDED_)


/*
		// graphics support - moved to CPlotDialog
	void		DrawPlotBackground( CPaintDC& dc );
	void		DrawGrid( CPaintDC& dc );
	void		DrawGridLabels( CPaintDC& dc );
	void		MapToPlot( CFPoint ptIn, CPoint& ptOut );
	short		MapToPlotY( float fyy )
				{	return	plotExtents.top + (short)((topLeft.y - fyy) * yScale);	};
	short		MapToPlotX( float fxx )
				{	return	plotExtents.left + (short)((fxx - topLeft.x) * xScale);	};

	CFPoint		topLeft;		// domain point set by EvalStrategy
	CFPoint		bottomRight;	// domain point used by Draw... routines
	float		yScale;			// pixels per dollar
	float		xScale;			// scale up yscale by height/width ratio
	CRect		plotExtents;
*/
