#if !defined(AFX_NILLADIALOG_H__518255E5_78A2_4945_B7D9_9769EA17488C__INCLUDED_)
#define AFX_NILLADIALOG_H__518255E5_78A2_4945_B7D9_9769EA17488C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// NillaDialog.h : header file
//
//#include "NillaHedge.h"
//#include "OleDateTimeEx.h"

//extern	CNillaHedgeApp	theApp;

//class	CBOSDatabase;

/////////////////////////////////////////////////////////////////////////////
// CNillaDialog dialog

//const unsigned short StringPoolSize = 3;

class CNillaDialog : public CDialog
{
	DECLARE_DYNAMIC( CNillaDialog )

// Construction
public:
	CNillaDialog( int IDD, CWnd* pParent = NULL );	// passthrough call for virtual overrides
//	CBOSDatabase*	GetDB( void )	{	return	(CBOSDatabase*)GetTopLevelFrame()->GetActiveDocument();		}

// Dialog Data
	//{{AFX_DATA(CNillaDialog)
	enum { IDD = 1 };								// a bogus dialog ID
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNillaDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

		// the following are required to make the List Control work properly
//	LPTSTR				AddPool( CString* cst );	// needed for Blaszczak's GetDispInfo handler
//	unsigned short		nextFreeSlot;
//	CString				cstPool[StringPoolSize];
//	LPTSTR				pBufPool[StringPoolSize];

protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_MSG(CNillaDialog)
	//}}AFX_MSG
//DECLARE_MESSAGE_MAP()

	// Implementation
public:

protected:
	virtual CString		getListSel( int dialogItemID );
	virtual void		setEditBox( char* format, double dd, int dialogItemID );
	virtual void		setEditBox( char* format, float ff, int dialogItemID );
	virtual void		setComboBox( char* format, float ff, int dialogItemID );
	virtual double		getEditBoxDouble( int dialogItemID );
	virtual float		getEditBoxFloat( int dialogItemID );
	virtual short		getComboBoxFloat( int dialogItemID, float& ff );
	virtual	void		loadMapIntoListBox( CMapStringToPtr* theMap, int dialogItemID );
	virtual void		setStatic( char* format, float ff, int dialogItemID );
	virtual void		setStatic( char* format, double dd, int dialogItemID );
	virtual void		setStatic( CString cs, int dialogItemID );

		// the following functionality has been moved into CSeekComboBox
//	virtual	void		loadMapIntoComboBox( CMapStringToPtr* theMap, int dialogItemID );	// use CSeekComboBox::LoadMap()
//	virtual CString		getComboSel( int dialogItemID );									// use CSeekComboBox::GetText()
//	virtual void		seekComboBoxEntry( int dialogItemID );								// use CSeekComboBox::SeekEntry()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
//	virtual void		setStatic( char* buf, int dialogItemID );

#endif // !defined(AFX_NILLADIALOG_H__518255E5_78A2_4945_B7D9_9769EA17488C__INCLUDED_)
//	virtual CObject*	getMapObject( CMapStringToOb& theMap, CString sym );
//	virtual CString		getEditBoxCString( int dialogItemID );
//	virtual void		getDateTimeCtrl( int dlgItemID, COleDateTime& theDate );
//	virtual void		setDateTimeCtrl( COleDateTime& theDate, int dlgItemID );
//	virtual void		setEditBox( CString cs, int dialogItemID );
//	virtual void		setEditBox( char* st, int dialogItemID );
