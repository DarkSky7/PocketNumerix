#if !defined(AFX_VOLATILITYEXPLORER_H__20C94418_F251_4450_8B1E_38B6FE4BBD21__INCLUDED_)
#define AFX_VOLATILITYEXPLORER_H__20C94418_F251_4450_8B1E_38B6FE4BBD21__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// VolatilityExplorer.h : header file
//
#include "PlotOptionsDialog.h"
#include "SeekComboBox.h"
#include "NumEdit.h"
#include "PlotDialog.h"
#include "VolatilityExplorerState.h"
#include "Option.h"

//class	COption;

const	short	nInstVolPts = 30;		// the number of evaluations for each option
const	short	nFolderVolPts = 30;		// the number of evaluations for the options positions

/////////////////////////////////////////////////////////////////////////////
// CVolatilityExplorer dialog

class CVolatilityExplorer : public CPlotOptionsDialog
{
// Construction
public:
	CVolatilityExplorer(CWnd* pParent = NULL);	// standard constructor
	~CVolatilityExplorer();						// standard destructor

// Dialog Data
	//{{AFX_DATA(CVolatilityExplorer)
	enum { IDD = IDD_VolatilityExplorer };
	CButton	c_PureBS;
	CDateTimeCtrl	c_EvalDate;
	CButton			c_Portfolio;
	CNumEdit		c_RiskFreeRate;
	float			m_RiskFreeRate;
	//}}AFX_DATA
	VolatilityExplorerState			volExpState;
	OptionEvaluationContext			evcs;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVolatilityExplorer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void		KillFocusSymbolWork( CSeekComboBox* scb, short ii, CString& optSym );
	void		SelChangeSymbolWork( CSeekComboBox* scb, short ii, CString& optSym );
	void		KillFocusEvalDateWork( NMHDR* pNMHDR, LRESULT* pResult );
	void		SetCheckButtons( void );
	void		CheckWork( CButton* chkButton, unsigned short ii );

		// plot related
	void		RecalcAll( void );
	void		CalcInstFpts( unsigned short ii );
	void		CalcFolderFpts( void );
	void		SymbolWorkCore( short ii, CString sym, long* def_ID );
	CFPoint		instFpts[ nBOS * nInstVolPts ];	// row-major order, portfolio at [nBOS*nRatePts]
	bool		instFptsValid[ nBOS ];				// portfolio points not included
	float		instMinY[ nBOS ];
	float		instMaxY[ nBOS ];
	float		folderMinY;
	float		folderMaxY;
	CFPoint		folderFpts[ nFolderVolPts ];
	bool		folderFptsValid;

	long		nIndexedIssues;
	DWORD		m_activeComboBox;

	// Generated message map functions
	//{{AFX_MSG(CVolatilityExplorer)
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnCheck3();
	afx_msg void OnCheck4();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillFocusRiskFreeRate();
	afx_msg void OnPaint();
	afx_msg void OnPortfolio();
	afx_msg void OnCloseUpEvalDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillFocusEvalDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPureBS();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnSelChangeSymbol1();
	afx_msg void OnSelChangeSymbol2();
	afx_msg void OnSelChangeSymbol3();
	afx_msg void OnSelChangeSymbol4();
	afx_msg void OnKillFocusSymbol1();							// HotKeys +
	afx_msg void OnKillFocusSymbol2();							// HotKeys +
	afx_msg void OnKillFocusSymbol3();							// HotKeys +
	afx_msg void OnKillFocusSymbol4();							// HotKeys +
	afx_msg void OnSetFocusSymbol1();							// HotKeys
	afx_msg void OnSetFocusSymbol2();							// HotKeys
	afx_msg void OnSetFocusSymbol3();							// HotKeys
	afx_msg void OnSetFocusSymbol4();							// HotKeys
	//}}AFX_MSG
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);		// HotKeys
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);						// HotKeys

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSetFocusRiskFreeRate();
public:
	afx_msg void OnSetFocusEvalDate(NMHDR *pNMHDR, LRESULT *pResult);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOLATILITYEXPLORER_H__20C94418_F251_4450_8B1E_38B6FE4BBD21__INCLUDED_)
