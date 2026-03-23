#if !defined(AFX_STRATEGYEXPLORER_H__19B9A7EF_2991_4D86_9EBE_B892889BA859__INCLUDED_)
#define AFX_STRATEGYEXPLORER_H__19B9A7EF_2991_4D86_9EBE_B892889BA859__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// StrategyExplorer.h : header file
//
#include "NillaDialog.h"
#include "Polyline.h"
#include "Option.h"			// compiling fails without this, but it's not clear why
#include "SeekComboBox.h"
#include "OptionType.h"

/////////////////////////////////////////////////////////////////////////////
// CStrategyExplorer dialog

class CStrategyExplorer : public CNillaDialog
{
	// Construction
public:
	CStrategyExplorer( CWnd* pParent = NULL );	// standard constructor

	BuySellStatus	m_BuySell[4];				// Stock at index 3
	float			m_Cost[4];					// ditto
	float			m_Qty[4];					// ditto
	OptionType		m_PutCall[3];				// options at indices 0..2
	float			m_Strike[3];

// Dialog Data
	//{{AFX_DATA(CStrategyExplorer)
	enum { IDD = IDD_StrategyExplorer };
	CButton			c_SellStock;
	CButton			c_SellOption1;
	CButton			c_SellOption2;
	CButton			c_SellOption3;
	CButton			c_BuyStock;
	CButton			c_BuyOption1;
	CButton			c_BuyOption2;
	CButton			c_BuyOption3;
	CSeekComboBox	c_Strategy;
	CString			m_Strategy;
	//}}AFX_DATA

		// the strategy example loaders
	void		SyntheticLongStock( void );
	void		SyntheticShortStock( void );

	void		BullCallSpread( void );
	void		BullPutSpread( void );

	void		BearCallSpread( void );
	void		BearPutSpread( void );

	void		LongStraddle( void );
	void		ShortStraddle( void );

	void		LongStrangle( void );
	void		ShortStrangle( void );

	void		ButterflyCallSpread( void );
	void		ButterflyPutSpread( void );

	void		CoveredPut( void );				// Synthetic Short Call
	void		CoveredCall( void );			// Synthetic Short Put

	void		ProtectivePut( void );			// Synthetic Long Call
	void		ProtectiveCall( void );			// Synthetic Long Put

	void		Collar( void );
	void		Fence( void );

					// options arbitrage
	void		Conversion( void );				// optionsXpress, interactiveBrokers
	void		Reversal( void );				// interactiveBrokers (optionsXpress calls this a Reversal)

//	void		RiskReversal( void );			// Wilmott's definition (p.45 red book)

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
				{	return	plotExtents.top + (short)((topLeft.y - fyy) * yScale);	};
	short		MapToPlotX( float fxx )
				{	return	plotExtents.left + (short)((fxx - topLeft.x) * xScale);	};

	bool		InstComplete( short ii );		// idx is 0-based, 3 -> stock
	void		killFocusStrategyWork( void );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStrategyExplorer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool		inhibitEval;
	CRect		plotExtents;
	CFPoint		topLeft;		// domain point set by EvalStrategy
	CFPoint		bottomRight;	// domain point used by Draw... routines
	float		yScale;			// pixels per dollar
	float		xScale;			// scale up yscale by height/width ratio


	// Generated message map functions
	//{{AFX_MSG(CStrategyExplorer)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeStrategy();
	afx_msg void OnBuyOption1();
	afx_msg void OnBuyOption2();
	afx_msg void OnBuyOption3();
	afx_msg void OnBuyStock();
	afx_msg void OnSellOption1();
	afx_msg void OnSellOption2();
	afx_msg void OnSellOption3();
	afx_msg void OnSellStock();
	afx_msg void OnOption1Call();
	afx_msg void OnOption2Call();
	afx_msg void OnOption3Call();
	afx_msg void OnOption1Put();
	afx_msg void OnOption2Put();
	afx_msg void OnOption3Put();
	afx_msg void OnKillFocusOption1Cost();
	afx_msg void OnKillFocusOption2Cost();
	afx_msg void OnKillFocusOption3Cost();
	afx_msg void OnKillFocusStockPrice();
	afx_msg void OnKillFocusOption1Qty();
	afx_msg void OnKillFocusOption2Qty();
	afx_msg void OnKillFocusOption3Qty();
	afx_msg void OnKillFocusStockQty();
	afx_msg void OnKillFocusOption1Strike();
	afx_msg void OnKillFocusOption2Strike();
	afx_msg void OnKillFocusOption3Strike();
	afx_msg void OnKillFocusStrategy();
	afx_msg void OnPaint();
	afx_msg void OnSelEndOkStrategy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STRATEGYEXPLORER_H__19B9A7EF_2991_4D86_9EBE_B892889BA859__INCLUDED_)

//	void		SyntheticShortPut( void );  // replaced by CoveredCall()
//	void		CoveredPut( void );			// replaced by SyntheticShortCall()
