// YieldCurveFitterDlg.h : header file
//

#if !defined(AFX_YIELDCURVEFITTERDLG_H__623EEF0C_7D9F_4E1A_8FB7_CEC3C7A7E0A0__INCLUDED_)
#define AFX_YIELDCURVEFITTERDLG_H__623EEF0C_7D9F_4E1A_8FB7_CEC3C7A7E0A0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class	CBitArray;
class	CVector;
class	CMatrix;

#include "NillaDialog.h"
//#include "YieldCurveFitter.h"
#include "Registry.h"
#include "resource.h"
#include "NumEdit.h"
#include "ServerRates.h"

/////////////////////////////////////////////////////////////////////////////
// CYieldCurveFitterDlg dialog

class CYieldCurveFitterDlg : public CNillaDialog
{
	DECLARE_DYNAMIC( CYieldCurveFitterDlg )

// Construction
public:
	CYieldCurveFitterDlg( CWnd* pParent = NULL );	// standard constructor
	~CYieldCurveFitterDlg( void );					// destructor

// Dialog Data
	//{{AFX_DATA( CYieldCurveFitterDlg )
	enum { IDD = IDD_YieldCurveFitter };
	CButton		c_Fetch;
	CStatic		c_StatusResult;
	CStatic		c_RateDate;
	float		m_3moYield;
	float		m_6moYield;
	float		m_1yrYield;
	float		m_2yrYield;
	float		m_3yrYield;
	float		m_5yrYield;
	float		m_7yrYield;
	float		m_10yrYield;
	float		m_15yrYield;
	float		m_20yrYield;
	float		m_25yrYield;
	float		m_30yrYield;
	CNumEdit	c_3moYield;
	CNumEdit	c_6moYield;
	CNumEdit	c_1yrYield;
	CNumEdit	c_2yrYield;
	CNumEdit	c_3yrYield;
	CNumEdit	c_5yrYield;
	CNumEdit	c_7yrYield;
	CNumEdit	c_10yrYield;
	CNumEdit	c_15yrYield;
	CNumEdit	c_20yrYield;
	CNumEdit	c_25yrYield;
	CNumEdit	c_30yrYield;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL( CYieldCurveFitterDlg )
	protected:
	virtual void DoDataExchange( CDataExchange* pDX );	// DDX/DDV support
	//}}AFX_VIRTUAL

public:
		// additional attributes
	float		spotRate;
	CBitArray*	userSupplied;

		// implementation
	void		UpdateRates( void );
	void		SaveSRB( void );

// Implementation

protected:
	// Generated message map functions
	//{{AFX_MSG( CYieldCurveFitterDlg )
	virtual BOOL OnInitDialog();
	afx_msg void OnKillFocus10yrYield();
	afx_msg void OnKillFocus15yrYield();
	afx_msg void OnKillFocus1yrYield();
	afx_msg void OnKillFocus20yrYield();
	afx_msg void OnKillFocus25yrYield();
	afx_msg void OnKillFocus2yrYield();
	afx_msg void OnKillFocus30yrYield();
	afx_msg void OnKillFocus3moYield();
	afx_msg void OnKillFocus3yrYield();
	afx_msg void OnKillFocus5yrYield();
	afx_msg void OnKillFocus6moYield();
	afx_msg void OnKillFocus7yrYield();
	afx_msg void OnSetFocus10yrYield();
	afx_msg void OnSetFocus15yrYield();
	afx_msg void OnSetFocus1yrYield();
	afx_msg void OnSetFocus20yrYield();
	afx_msg void OnSetFocus25yrYield();
	afx_msg void OnSetFocus2yrYield();
	afx_msg void OnSetFocus30yrYield();
	afx_msg void OnSetFocus3moYield();
	afx_msg void OnSetFocus3yrYield();
	afx_msg void OnSetFocus5yrYield();
	afx_msg void OnSetFocus6moYield();
	afx_msg void OnSetFocus7yrYield();
#ifdef ServerAccess
	afx_msg void OnFetch();
#endif
	//}}AFX_MSG
//	afx_msg LRESULT OnCtlColorDlg();
//	afx_msg LRESULT OnCtlColorDlgStatic();

	afx_msg LRESULT OnYieldCurveFitterAlreadyRunning( WPARAM, LPARAM );
	afx_msg HBRUSH	OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
#ifdef ServerAccess
	afx_msg LRESULT OnParseData( WPARAM, LPARAM );
#endif

	DECLARE_MESSAGE_MAP()

		// new stuff... background color, notify icon
//	afx_msg LRESULT		OnNotifyIcon( WPARAM wParam, LPARAM lParam );
//	HICON		m_hIcon;
	HBRUSH		m_hbGray;
	HBRUSH		m_hbWhite;

//	HBRUSH 		OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );		// MFC version never gets called
//	LRESULT		OnCtlColorDlg( WPARAM wParam, LPARAM lParam );
//	LRESULT		OnCtlColorStatic( WPARAM wParam, LPARAM lParam );

	bool		SetRiskFreeRate( float rfr )
				{	return	WriteRegFloat( HKEY_CURRENT_USER,
								_T("SOFTWARE\\PocketNumerix\\RiskFreeRate"), rfr );
				}

public:
	void		RestoreServerRates( ServerRates SR );
//	bool		wininetIsActive;

protected:
	void		KillFocusYieldWork( int ctl_ID, float* yield, short usi );
	void		SetFocusYieldWork( CNumEdit* ceb );
	void		ExitThreadPrep( short code );
	HANDLE		threadHandle;

		// rate retrieval variables ...
		// WinInet variabless
public:
//	HINTERNET		hInet;
//	HINTERNET		hInetSession;
//	DWORD			m_DeviceId;
//	char*			lpServerPage;

//	DWORD			ParseFile( void );		// pushed into HttpGetFile.cpp
		// made app variables
//	char*			lpServerName;
//	wchar_t*		lpFileName;
//	char*			urlRequest;
		// now a local variable in InitDialog
//	wchar_t*		docDir;
};

typedef struct
{
	CYieldCurveFitterDlg*		p_YCF;
} YieldCurveFitterThreadCtx;


//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_YIELDCURVEFITTERDLG_H__623EEF0C_7D9F_4E1A_8FB7_CEC3C7A7E0A0__INCLUDED_)
