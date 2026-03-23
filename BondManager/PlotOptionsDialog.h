#if !defined(AFX_PLOTOPTIONSDIALOG_H__35BDA557_65A3_4455_BCB3_3FFC402F428D__INCLUDED_)
#define AFX_PLOTOPTIONSDIALOG_H__35BDA557_65A3_4455_BCB3_3FFC402F428D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PlotOptionsDialog.h : header file
//

#include "PlotDialog.h"
#include "SeekComboBox.h"
#include "Bond.h"

class	CDataManager;

const	short	nBOS = 4;

/////////////////////////////////////////////////////////////////////////////
// CPlotOptionsDialog dialog

class CPlotOptionsDialog : public CPlotDialog
{
// Construction
public:
	CPlotOptionsDialog( int IDD, CWnd* pParent = NULL);	// standard constructor
	~CPlotOptionsDialog( void );						// standard destructor

// Dialog Data
	//{{AFX_DATA(CPlotOptionsDialog)
	enum { IDD = 1 };
	CSeekComboBox	c_Symbol1;
	CSeekComboBox	c_Symbol2;
	CSeekComboBox	c_Symbol3;
	CSeekComboBox	c_Symbol4;
	CString			m_Symbol1;
	CString			m_Symbol2;
	CString			m_Symbol3;
	CString			m_Symbol4;
	CButton			c_Check1;
	CButton			c_Check2;
	CButton			c_Check3;
	CButton			c_Check4;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlotOptionsDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
//	void		FillSlots( long* def_ID );									// fills *(def_ID) 
//	void		killFocusSymbolWork( short ii, CString sym, long* def_ID );
//	bool		InstValid( int jj, bool* chkBtns );
//	void		LoadBOSs( long* def_ID );									// fills pBOS[]
//	void		LoadSymbols( void );
	void		LoadSymbolsCore( CMapStringToPtr* theMap );
//	void		KillFocusSymbolWork( short ii, CString sym, long* def_ID );
	void		SetCheckButtons( bool* chkBtns );
	void		DeleteBOSs( void );

	CBond*		pBOS[ nBOS ];

	// Generated message map functions
	//{{AFX_MSG(CPlotOptionsDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLOTOPTIONSDIALOG_H__35BDA557_65A3_4455_BCB3_3FFC402F428D__INCLUDED_)
