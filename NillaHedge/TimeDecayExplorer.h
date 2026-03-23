#if !defined(AFX_TIMEDECAYEXPLORER_H__D1A8E6C8_F42E_4D77_8280_03B1E0A0C884__INCLUDED_)
#define AFX_TIMEDECAYEXPLORER_H__D1A8E6C8_F42E_4D77_8280_03B1E0A0C884__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TimeDecayExplorer.h : header file
//
#include "PlotOptionsDialog.h"
#include "SeekComboBox.h"
#include "NumEdit.h"
#include "TimeDecay.h"
#include "Option.h"
//#include "ColorDefinitions.h"

//class	COption;

const	short	nInstTimePts = 60;		// the number of evaluations for each option
const	short	nFolderTimePts = 60;	// the number of evaluations for option positions
//const	short	nTimeBOS = 4;			// the number of options possible

/////////////////////////////////////////////////////////////////////////////
// CTimeDecayExplorer dialog

class CTimeDecayExplorer : public CPlotOptionsDialog
{
// Construction
public:
	CTimeDecayExplorer(CWnd* pParent = NULL);	// standard constructor
	~CTimeDecayExplorer();						// standard deconstructor

// Dialog Data
	//{{AFX_DATA(CTimeDecayExplorer)
	enum { IDD = IDD_TimeDecayExplorer };
	CButton			c_PureBS;
	CButton			c_Portfolio;
	CNumEdit		c_RiskFreeRate;
	CDateTimeCtrl	c_EndDate;
	COleDateTime	m_EndDate;
	float			m_RiskFreeRate;
	//}}AFX_DATA
	TimeDecay		timeDecay;					// contains the persistent states
//	OptionEvaluationContext	evcs;				// a Black-Scholes evaluation context

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeDecayExplorer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void	KillFocusSymbolWork( CSeekComboBox* scb, short ii, CString& optSym );
	void	SelChangeSymbolWork( CSeekComboBox* scb, short ii, CString& optSym );
	void	KillFocusEndDateWork( NMHDR* pNMHDR, LRESULT* pResult );
	void	SetCheckButtons( void );
	void	CheckWork( CButton* chkButton, unsigned short ii );
	long	nIndexedIssues;
	DWORD	m_activeComboBox;

		// plot related
	void		CalcInstFpts( unsigned short ii );
	void		CalcFolderFpts( void );
	void		SymbolWorkCore( short ii, CString sym, long* def_ID );
	void		RecalcAll( void );
	CFPoint		instFpts[ nBOS * nInstTimePts ];	// row-major order, portfolio at [nBOS*nRatePts]
	bool		instFptsValid[ nBOS ];				// portfolio points not included
	float		instMinY[ nBOS ];
	float		instMaxY[ nBOS ];
	float		folderMinY;
	float		folderMaxY;
	CFPoint		folderFpts[ nFolderTimePts ];
	bool		folderFptsValid;

	// Generated message map functions
	//{{AFX_MSG(CTimeDecayExplorer)
	afx_msg void OnPortfolio();
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnCheck3();
	afx_msg void OnCheck4();
	afx_msg void OnKillFocusRiskFreeRate();
	afx_msg void OnPaint();
	afx_msg void OnKillFocusEndDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCloseUpEndDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPureBS();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnSelChangeSymbol1();
	afx_msg void OnSelChangeSymbol2();
	afx_msg void OnSelChangeSymbol3();
	afx_msg void OnSelChangeSymbol4();
	afx_msg void OnKillFocusSymbol1();											// HotKeys +
	afx_msg void OnKillFocusSymbol2();											// HotKeys +
	afx_msg void OnKillFocusSymbol3();											// HotKeys +
	afx_msg void OnKillFocusSymbol4();											// HotKeys +
	afx_msg void OnSetFocusSymbol1();											// HotKeys
	afx_msg void OnSetFocusSymbol2();											// HotKeys
	afx_msg void OnSetFocusSymbol3();											// HotKeys
	afx_msg void OnSetFocusSymbol4();											// HotKeys
	//}}AFX_MSG
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);		// HotKeys
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);						// HotKeys

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSetFocusEndDate(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnSetFocusRiskFreeRate();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEDECAYEXPLORER_H__D1A8E6C8_F42E_4D77_8280_03B1E0A0C884__INCLUDED_)
