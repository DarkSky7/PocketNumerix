#if !defined(AFX_SEEKCOMBOBOX_H__19A141F3_B7C4_4D7E_851A_F01E8F91B156__INCLUDED_)
#define AFX_SEEKCOMBOBOX_H__19A141F3_B7C4_4D7E_851A_F01E8F91B156__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SeekComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSeekComboBox window

class CSeekComboBox : public CComboBox
{
// Construction
public:
	CSeekComboBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSeekComboBox)
	virtual BOOL PreTranslateMessage( MSG* pMsg );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual			~CSeekComboBox();
	virtual CString	GetText( void );
	virtual void	LoadMap( CMapStringToPtr* theMap );
	virtual	short	GetFloat( float& ff );				// from the EditBox
	virtual	short	GetFloat( int idx, float& ff );		// from the ListBox
	virtual	void	SetFloat( float ff );		// essentially SetWindowText( float )
	unsigned short	lastChar;

	// Generated message map functions
protected:
	//{{AFX_MSG(CSeekComboBox)
	afx_msg void OnEditChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	virtual void	SeekEntry( void );
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SEEKCOMBOBOX_H__19A141F3_B7C4_4D7E_851A_F01E8F91B156__INCLUDED_)
