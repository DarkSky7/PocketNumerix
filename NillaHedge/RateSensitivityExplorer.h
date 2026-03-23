#if !defined(AFX_RATESENSITIVITYEXPLORER_H__E7A866D3_B313_4C7D_9F5E_87E761420EF7__INCLUDED_)
#define AFX_RATESENSITIVITYEXPLORER_H__E7A866D3_B313_4C7D_9F5E_87E761420EF7__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RateSensitivityExplorer.h : header file
//
#include "AssetType.h"
#include "PlotOptionsDialog.h"
#include "SeekComboBox.h"
#include "NumEdit.h"
#include "RateSensitivity.h"
#include "Option.h"

class	CBOS;
//class	CBond;
//class	COption;
class	CFPoint;
class	CDataManager;

const	short	nInstRatePts = 12;			// the number of evaluations for each option & portfolio
const	short	nFolderRatePts = 12;		// the number of evaluations for the options positions

/////////////////////////////////////////////////////////////////////////////
// CRateSensitivityExplorer dialog

class CRateSensitivityExplorer : public CPlotOptionsDialog
{

// Construction
public:
	CRateSensitivityExplorer(CWnd* pParent = NULL);		// standard constructor
	~CRateSensitivityExplorer( void );					// standard destructor

// Dialog Data
	//{{AFX_DATA(CRateSensitivityExplorer)
	enum { IDD = IDD_RateSensitiityExplorer };
	CStatic			c_RiskFreeRateLabel;
	CNumEdit		c_DeltaHigh;
	CNumEdit		c_DeltaLow;
	CNumEdit		c_RiskFreeRate;
//	CButton			c_Bond;
//	CButton			c_Option;
	CButton			c_Portfolio;
	float			m_DeltaHigh;
	float			m_DeltaLow;
	float			m_RiskFreeRate;
	//}}AFX_DATA
	OptionRateSensitivity		rateSens;
	OptionEvaluationContext		evcs;
//	BondEvaluationContext		bondEvalCtx;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRateSensitivityExplorer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	void		KillFocusSymbolWork( CSeekComboBox* scb, short ii, CString& optSym );
	void		SelChangeSymbolWork( CSeekComboBox* scb, short ii, CString& optSym );
	void		LoadBOSs( void );			// def_IDs --> symbols and pBOSs
	void		LoadSymbols( void );		// selects a symbol table for LoadMapSymbolsCore()
	void		SetCheckButtons( void );
	void		CheckWork( CButton* chkButton, unsigned short ii );

		// the stuff that needs to get cleaned up by the destructor
	int		nIndexedIssues;					// saved decision point for enabling c_Option
	DWORD	m_activeComboBox;

		// plot related
	void		RecalcAll( void );
	void		CalcInstFpts( unsigned short ii );
	void		CalcFolderFpts( void );
	void		SymbolWorkCore( short ii, CString sym, long* def_ID );
	CFPoint		instFpts[ nBOS * nInstRatePts ];	// row-major order, portfolio at [nBOS*nRatePts]
	bool		instFptsValid[ nBOS ];				// portfolio points not included
	float		instMinY[ nBOS ];
	float		instMaxY[ nBOS ];
	float		folderMinY;
	float		folderMaxY;
	CFPoint		folderFpts[ nFolderRatePts ];
	bool		folderFptsValid;

//	CFPoint		instFpts[ nBOS * nInstRatePts ];	// row-major order, portfolio at [nBOS*nRatePts]
//	bool		instFptsValid[ nBOS ];				// portfolio points not included
//	float		instMinY[ nBOS ];
//	float		instMaxY[ nBOS ];
//	float		folderMinY;
//	float		folderMaxY;
//	CFPoint		optionFolderPts[ nFolderRatePts ];	// this was here
//	bool		optionPtsAreValid;					// so was this...
//	bool		bPortfolio;

	// Generated message map functions
	//{{AFX_MSG(CRateSensitivityExplorer)
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnCheck3();
	afx_msg void OnCheck4();
	afx_msg void OnKillFocusDeltaHigh();
	afx_msg void OnSetFocusDeltaHigh();
	afx_msg void OnKillFocusDeltaLow();
	afx_msg void OnSetFocusDeltaLow();
	afx_msg void OnKillFocusRiskFreeRate();
	afx_msg void OnSetFocusRiskFreeRate();
	afx_msg void OnPortfolio();
	afx_msg void OnPaint();
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
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RATESENSITIVITYEXPLORER_H__E7A866D3_B313_4C7D_9F5E_87E761420EF7__INCLUDED_)
