// StrategyExplorerDlg.h : header file
//

#if !defined(AFX_STRATEGYEXPLORERDLG_H__3B8EB950_A9B5_4A83_BD22_0A3D87636A4D__INCLUDED_)
#define AFX_STRATEGYEXPLORERDLG_H__3B8EB950_A9B5_4A83_BD22_0A3D87636A4D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OneStk4OptStrategy.h"
#include "SeekComboBox.h"
#include "Polyline.h"
#include "NillaDialog.h"
#include "Quint.h"			// gets you Triple.h too
#include <utility>			// for pair
#include <map>
#include <vector>			// <list> can be used w/o other changes (faster inserts, but more overhead)
using namespace std;

typedef Triple<float,char,float>				StockPos_T;
typedef Quint<float,char,float,char,float>		OptionPos_T;
typedef vector<OptionPos_T*>					Spread_T;
//typedef	map<CString,Spread_T*>					Spreads_T;
typedef pair<StockPos_T*,Spread_T*>				Hedge_T;
typedef	map<CString,Hedge_T*>					Hedges_T;

/////////////////////////////////////////////////////////////////////////////
// CStrategyExplorerDlg dialog

const unsigned short	stkIdx = 4;

class CStrategyExplorerDlg : public CNillaDialog
{
	// Construction
public:
	CStrategyExplorerDlg( CWnd* pParent = NULL );		// standard constructor

	BuySellStatus	m_BuySell[stkIdx+1];	// Stock at index 4
	float			m_Cost[stkIdx+1];		// ditto
	float			m_Qty[stkIdx+1];		// ditto
	OptionType		m_PutCall[stkIdx];		// options at indices 0..3
	float			m_Strike[stkIdx];

// Dialog Data
	//{{AFX_DATA(CStrategyExplorerDlg)
	enum { IDD = IDD_StrategyExplorerHiResDlg };
	CSeekComboBox	c_StockQty;				// Stock
	CSeekComboBox	c_Option1Qty;
	CSeekComboBox	c_Option2Qty;
	CSeekComboBox	c_Option3Qty;
	CSeekComboBox	c_Option4Qty;
	CButton			c_SellStock;			// Stock
	CButton			c_SellOption1;
	CButton			c_SellOption2;
	CButton			c_SellOption3;
	CButton			c_SellOption4;
	CButton			c_BuyStock;				// Stock
	CButton			c_BuyOption1;
	CButton			c_BuyOption2;
	CButton			c_BuyOption3;
	CButton			c_BuyOption4;
	CSeekComboBox	c_Strategy;
	CString			m_Strategy;
	//}}AFX_DATA

protected:
	void		DrawPlotBackground( CPaintDC& dc );
	void		DrawChart( CPaintDC& dc );
	void		DrawGrid( CPaintDC& dc );
	void		DrawGridLabels( CPaintDC& dc );
	short		DrawStock( CPaintDC& dc, CPolyline& pl );
	short		DrawOption( short idx, CPaintDC& dc, CPolyline& pl );
	void		EvalStrategy( void );									// owns the Profit Polyline
	float		OptionY( short ii, float fxx, CFPoint& strikePt );		// returns fyy

		// graphics support
	bool		CalcDomainExtents( void );
	void		MapToPlot( CFPoint ptIn, CPoint& ptOut );
	short		MapToPlotY( float fyy )
				{	return	(short)(plotExtents.top + (topLeft.y - fyy) * yScale);	};
	short		MapToPlotX( float fxx )
				{	return	(short)(plotExtents.left + (fxx - topLeft.x) * xScale);	};

	bool		InstComplete( short ii );		// idx is 0-based, 4 -> stock
	void		killFocusStrategyWork( void );
	void		killFocusStockQtyWork( void );
	void		SelChangeQtyWork( CSeekComboBox* scb, float* qty );
	void		SetFocusComboBoxWork( CSeekComboBox* scb, DWORD ctrlID );
	void		KillFocusComboBoxWork( void );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStrategyExplorer)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	bool		inhibitEval;
	bool		vga;
	CRect		plotExtents;
	CFPoint		topLeft;		// domain point set by EvalStrategy
	CFPoint		bottomRight;	// domain point used by Draw... routines
	float		yScale;			// pixels per dollar
	float		xScale;			// scale up yscale by height/width ratio

private:
//	void	RegisterHotKeys( void );
	bool	InTheMoney( int ii, double xx );
	double	Slope( int ii, double xx );
	double	SlopeCept( int ii, double xx, double& yCept );		// returns slope
//	double	LeftSlope( int ii, double xx );
//	double	RightSlope( int ii, double xx );
//	bool	ProfitX( double strike, bool hiSide, bool complete[], double& xCept );

	void	GetStrategies( void );
	void	UpdateStrategy( void );
	void	killFocusQtyWork( CSeekComboBox* scb, float* qty );
	void	killFocusEditBoxWork( int ctrlID, float* storedFloat );
	void	buySellWork( CButton* pBuyButton, CButton* pSellButton,
						 BuySellStatus* pBuySellMemory, BuySellStatus bsVal );
	void	putCallWork( int putButtonID, int callButtonID,
						 OptionType* putCallMemory, OptionType pcVal );
	short	unicodeHeader;

public:
	void		SaveStrategies( void );
	Hedges_T	hedges;			// InitInstance will save spreads & hedges out to "Strategies.txt"

	// Generated message map functions
	//{{AFX_MSG(CStrategyExplorerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeStrategy();
	afx_msg void OnBuyOption1();
	afx_msg void OnBuyOption2();
	afx_msg void OnBuyOption3();
	afx_msg void OnBuyOption4();
	afx_msg void OnBuyStock();

	afx_msg void OnSellOption1();
	afx_msg void OnSellOption2();
	afx_msg void OnSellOption3();
	afx_msg void OnSellOption4();
	afx_msg void OnSellStock();

	afx_msg void OnOption1Call();
	afx_msg void OnOption2Call();
	afx_msg void OnOption3Call();
	afx_msg void OnOption4Call();

	afx_msg void OnOption1Put();
	afx_msg void OnOption2Put();
	afx_msg void OnOption3Put();
	afx_msg void OnOption4Put();

	afx_msg void OnKillFocusOption1Cost();
	afx_msg void OnKillFocusOption2Cost();
	afx_msg void OnKillFocusOption3Cost();
	afx_msg void OnKillFocusOption4Cost();
	afx_msg void OnKillFocusStockPrice();

	afx_msg void OnKillFocusOption1Strike();
	afx_msg void OnKillFocusOption2Strike();
	afx_msg void OnKillFocusOption3Strike();
	afx_msg void OnKillFocusOption4Strike();

	afx_msg void OnSetFocusOption1Cost();
	afx_msg void OnSetFocusOption2Cost();
	afx_msg void OnSetFocusOption3Cost();
	afx_msg void OnSetFocusOption4Cost();
	afx_msg void OnSetFocusStockPrice();

	afx_msg void OnSetFocusOption1Strike();
	afx_msg void OnSetFocusOption2Strike();
	afx_msg void OnSetFocusOption3Strike();
	afx_msg void OnSetFocusOption4Strike();

	afx_msg void OnPaint();

	afx_msg void OnSelChangeOption1Qty();
	afx_msg void OnSelChangeOption2Qty();
	afx_msg void OnSelChangeOption3Qty();
	afx_msg void OnSelChangeOption4Qty();
	afx_msg void OnSelChangeStockQty();

	afx_msg void OnKillFocusStrategy();
	afx_msg void OnKillFocusOption1Qty();
	afx_msg void OnKillFocusOption2Qty();
	afx_msg void OnKillFocusOption3Qty();
	afx_msg void OnKillFocusOption4Qty();
	afx_msg void OnKillFocusStockQty();

	afx_msg void OnSetFocusStrategy();
	afx_msg void OnSetFocusOption1Qty();
	afx_msg void OnSetFocusOption2Qty();
	afx_msg void OnSetFocusOption3Qty();
	afx_msg void OnSetFocusOption4Qty();
	afx_msg void OnSetFocusStockQty();

	//}}AFX_MSG

//	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
//	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg LRESULT OnStrategyExplorerAlreadyRunning( WPARAM, LPARAM );
	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);

		// Implementation
//	afx_msg	LRESULT	OnNotifyIcon( WPARAM wParam, LPARAM lParam );
//	HICON		m_hIcon;
	CBrush		m_hbWhite;
	CBrush		m_hbGray;
	DWORD		m_activeComboBox;
protected:
//	bool		LeftSlope( int ii, float xx, float& slope );
//	bool		RightSlope( int ii, float xx, float& slope );
	void		RegisterHotKeys( void );
//	void		UnregisterHotKeys( void );

	short		nStrategies;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STRATEGYEXPLORERDLG_H__3B8EB950_A9B5_4A83_BD22_0A3D87636A4D__INCLUDED_)
