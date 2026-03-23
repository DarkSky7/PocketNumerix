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
#include "BondRateSensitivity.h"
//#include "Option.h"

class	CBOS;
#include "Bond.h"
//class	COption;
class	CFPoint;
class	CDataManager;

const	short	nInstRatePts = 6;		// the number of evaluations for each bond or option & portfolio
const	short	nFolderRatePts = 6;		// the number of evaluations for the options positions

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
	CNumEdit		c_DeltaHigh;
	CNumEdit		c_DeltaLow;
	CButton			c_Portfolio;
//	float			m_DeltaHigh;
//	float			m_DeltaLow;
	//}}AFX_DATA
	BondRateSensitivity			rateSens;
	BondEvaluationContext		bondEvalCtx;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRateSensitivityExplorer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	void	KillFocusSymbolWorkCore( short ii, CString sym );
	void	KillFocusSymbolWork( CSeekComboBox* scb, short ii, CString& bndSym );
	void	SelChangeSymbolWork( CSeekComboBox* scb, short ii, CString& bndSym );
	void	LoadBOSs( void );				// def_IDs --> symbols and pBOSs
	void	LoadSymbols( void );			// selects a symbol table for LoadMapSymbolsCore()
	void	SetCheckButtons( void );
	void	FillSlots( long* def_ID );

	bool	InstValid( unsigned short ii );
	void	RecalcAll( void );
	void	CalcInstFpts( unsigned short ii );
	void	CalcFolderFpts( void );
	void	CheckWork( CButton* chkButton, unsigned short ii );
	void	SetFocusSymbolWork( DWORD ctrlID );

		// the stuff that needs to get cleaned up by the destructor

	int		nIndexedIssues;							// saved decision point for enabling c_Bond

		// true when DeltaLow, DeltaHigh or RiskFreeRate have changed
		// also before the points have been calculated the first timeer points
	CFPoint		instFpts[ nBOS * nInstRatePts ];	// row-major order, portfolio at [nBOS*nRatePts]
	bool		instFptsValid[ nBOS ];				// portfolio points not included
	float		instMinY[ nBOS ];
	float		instMaxY[ nBOS ];
	float		folderMinY;
	float		folderMaxY;
	CFPoint		folderFpts[ nFolderRatePts ];
	bool		folderFptsValid;

	bool		bPortfolio;							// based on rateSens.bndPortf & nIndexedIssues

	// Generated message map functions
	//{{AFX_MSG(CRateSensitivityExplorer)
	afx_msg void OnCheck1();
	afx_msg void OnCheck2();
	afx_msg void OnCheck3();
	afx_msg void OnCheck4();
	afx_msg void OnPortfolio();
	afx_msg void OnKillFocusDeltaHigh();
	afx_msg void OnKillFocusDeltaLow();
	afx_msg void OnSetFocusDeltaHigh();
	afx_msg void OnSetFocusDeltaLow();
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelChangeSymbol1();
	afx_msg void OnSelChangeSymbol2();
	afx_msg void OnSelChangeSymbol3();
	afx_msg void OnSelChangeSymbol4();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

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
	DWORD		m_activeComboBox;												// HotKeys
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RATESENSITIVITYEXPLORER_H__E7A866D3_B313_4C7D_9F5E_87E761420EF7__INCLUDED_)
