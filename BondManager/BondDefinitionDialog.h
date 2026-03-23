#if !defined(AFX_BONDDEFINITIONDIALOG_H__D5FC9FAB_8EA5_4CF1_8137_49BDCF1D9169__INCLUDED_)
#define AFX_BONDDEFINITIONDIALOG_H__D5FC9FAB_8EA5_4CF1_8137_49BDCF1D9169__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "NillaDialog.h"
#include "Bond.h"			// where DayCountMethod is defined
#include "SeekComboBox.h"
#include "NumEdit.h"
#include "resource.h"
//#include "OleDateTimeEx.h"

/////////////////////////////////////////////////////////////////////////////
// CBondDefinitionDialog dialog

class CBondDefinitionDialog : public CNillaDialog
{
	DECLARE_DYNAMIC( CBondDefinitionDialog )

// Construction
public:
	CBondDefinitionDialog( CWnd* pParent = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBondDefinitionDialog)
	enum { IDD = IDD_BondDefinitionDialog };
	CDateTimeCtrl	c_IncepDate;
	CButton			c_RadioActualActual;
	CButton			c_Radio30_360;
	CButton			c_RadioActual_360;
	CStatic			c_YieldToMaturityResult;
	CNumEdit		c_ParValue;
	CDateTimeCtrl	c_MaturityDate;
	CNumEdit		c_MarketPrice;
	CStatic			c_MacaulayDurationResult;
	CEdit			c_Desc;
	CStatic			c_CurrentYieldResult;
	CListBox		c_CouponsPerYear;
	CNumEdit		c_CouponRate;
	CStatic			c_ConvexityResult;
	CSeekComboBox	c_BondSymbol;
	CString			m_BondSymbol;
	COleDateTime	m_MaturityDate;
	CString			m_Desc;
	int				m_CouponsPerYear;
	float			m_ParValue;
	MarketPrice_t	m_MarketPrice;
	float			m_CouponRate;
	COleDateTime	m_IncepDate;
	//}}AFX_DATA
	DayCountMethod	dayCounting;
private:
	BondEvaluationContext	evalCtx;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBondDefinitionDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CBondDefinitionDialog)
	virtual BOOL	OnInitDialog();
	afx_msg void	OnThirty360Clicked();
	afx_msg void	OnActual360Clicked();
	afx_msg void	OnActualActualClicked();
	afx_msg void	OnKillFocusParValue();
	afx_msg void	OnKillFocusCouponRate();
	afx_msg void	OnKillFocusCouponsPerYear();								// ListBoxes don't need to catch HotKeys
	afx_msg void	OnKillFocusDesc();
	afx_msg void	OnKillFocusMarketPrice();
	afx_msg void	OnKillFocusMaturityDate( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void	OnSelChangeCouponsPerYear();
	afx_msg void	OnSetFocusMarketPrice();
	afx_msg void	OnSetFocusDesc();
	afx_msg void	OnSetFocusCouponRate();
	afx_msg void	OnSetFocusParValue();
	afx_msg void	OnKillFocusIncepDate( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void	OnSelChangeBondSymbol();
	afx_msg void	OnDeleteDefinition();
	afx_msg void	OnCloseUpIncepDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnSetFocusMaturityDate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnCloseUpMaturityDate(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnSetFocusIncepDate(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void	OnKillFocusBondSymbol();									// HotKeys +
	afx_msg void	OnSetFocusBondSymbol();										// HotKeys
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);		// HotKeys
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);						// HotKeys

// Implementation
public:
	void		SaveIfChanged( bool updateGUI = false );	// MainFrame uses the default
	void		SaveBond( CBond* aBond );
	DWORD		m_activeComboBox;												// HotKeys

protected:
	void		killFocusIncepDateWork( NMHDR* pNMHDR, LRESULT* pResult );
	void		killFocusMaturityDateWork( NMHDR* pNMHDR, LRESULT* pResult );
	void		KillFocusBondSymbolWork( CString sym );
	void		updateStatics( void );
private:
	bool		criticalRegion;

public:
	afx_msg void OnSetFocusCouponsPerYear();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
//	void		clearStatics( void );
//	void		enableDialogItems( bool enDis );
//	void		resetControls( void );

#endif // !defined(AFX_BONDDEFINITIONDIALOG_H__D5FC9FAB_8EA5_4CF1_8137_49BDCF1D9169__INCLUDED_)
