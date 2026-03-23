#if !defined(AFX_OPTIONCHAINDIALOG_H__07C14535_CBC3_4FC1_83F8_95005B1EC363__INCLUDED_)
#define AFX_OPTIONCHAINDIALOG_H__07C14535_CBC3_4FC1_83F8_95005B1EC363__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// OptionChainDialog.h : header file
//
#include "NillaDialog.h"
#include "SeekComboBox.h"
#include "OptionChainCommon.h"
#include "OptionChainListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// COptionChainDialog dialog

typedef struct
{	bool		optionsLoaded;	// just as it appears on yahoo finance
	short		firstIdx;		// index of the first associated option in the OptionsList
//	CString		urlTail;		// now redundant - it's a server relative url (no 'http://xxx.com')
} LinkInfo;


	// global functions
int		CALLBACK	OptionChainCompareFunc( LPARAM lp1, LPARAM lp2, LPARAM lpSort );
//void	DisplayOptionChainInetStatus( HINTERNET hInternet, unsigned long* dwContext,
//									   unsigned long dwInternetStatus, void* lpvStatusInformation,
//									   unsigned long dwStatusInformationLength					);

DWORD	WINAPI	DownloadURL( void* threadCtx );

class COptionChainDialog : public CNillaDialog
{
// Construction
public:
	COptionChainDialog(CWnd* pParent = NULL);   // standard constructor
	void	DisplayStatusLine( void );
	~COptionChainDialog();

// Dialog Data
	//{{AFX_DATA(COptionChainDialog)
	enum { IDD = IDD_OptionChainDialog };
	CStatic	c_StatusResult;
	CButton					c_Fetch;
	CSeekComboBox			c_StockSymbol;
	CString					m_StockSymbol;
	CStatic					c_StockPriceResult;
	CStatic					c_CompanyNameResult;
	CSeekComboBox			c_Expiration;
	CString					m_Expiration;
	COptionChainListCtrl	c_OptionsList;
	CButton					c_Save;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionChainDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
		// Generated message map functions
	//{{AFX_MSG(COptionChainDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetDispInfoOptionsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnClickOptionsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickOptionsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFetch();
	afx_msg void OnSave();
	afx_msg void OnEditUpdateExpiration();
	afx_msg void OnEditUpdateStockSymbol();
//	afx_msg void OnSelEndOkExpiration();				// already catching ON_CBN_SELCHANGE()

	afx_msg void OnSelChangeExpiration();
	afx_msg void OnSelChangeStockSymbol();
	afx_msg void OnKillFocusExpiration();										// HotKeys +
	afx_msg void OnKillFocusStockSymbol();										// HotKeys +
	afx_msg void OnSetFocusExpiration();										// HotKeys
	afx_msg void OnSetFocusStockSymbol();										// HotKeys
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);		// HotKeys
	afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);						// HotKeys
	afx_msg LRESULT OnParseData( WPARAM, LPARAM );
	afx_msg void OnCustomDrawOptionsList( NMHDR* pNMHDR, LRESULT* pResult );

public:												// to provide access in DownloadURL()
	int				m_nCalls;
	int				m_nPuts;
//	DWORD			ParseFile( void );				// replaced by OnParseData()
	bool			b_saveVerifyStockPriceUpdate;
//	MEMORYSTATUS	TraceMemoryStatus( short id );
//	HINTERNET		hInetSession;
//	HINTERNET		hInet;
//	char*			urlRequest;
//	char*			lpServerName;
//	wchar_t*		lpFileName;
//	char*			lpPageName;
	char*			uuid;
//	DWORD			sendWaitTime;					// no context anymore
//	DWORD			recvWaitTime;					// no context anymore
//	void			SaveIfChanged();

protected:
//	void*		LocalHeapNewHandler( size_t size );

		// return indicates that <terminator> was found
	bool		ReadPast( CFile& fp, char* terminator );
	bool		GetToken( CFile& fp, char* terminator, char*& token );		// does a ReadPast and retrieves the intervening token
	void		UpdateDB( OptionChainItemInfo* pInfo );

	CString		GetPositionItemText( int rr, int si );
	void		InsertColumns( void );
	void		PrepareForNewStockOptions( void );
	void		KillFocusExpirationWork( void );
	void		KillFocusStockSymbolWork( CString sym );
	bool		VerifyExpiration( CString& yrSt, short& monIdx );		// false if it fails to verify
	void		HandleStockPrice( char* szStockPrice );
	void		SortOptionsAndScrollTo( long idx );
	long		EnableFetch( CString expSt );		// returns -1 or the index of the first option with that expiration

		// lastSortKey is a 1-based column number (so its sign can be flipped)
		// if positive --> the last sort was ascending
		// if negative --> the last sort was descending
	short			lastSortKey;			// track most recent column sort
	CPtrArray		m_OptionChain;			// collection of OptionChainItemInfo structs
	CMapStringToPtr	m_Links;				// expiry strings mapped to URLs
	CString			m_CompanyName;
	float			m_StockPrice;
	CString			m_StockSymbolForOptionChain;	// which stock symbol was used to fetch the currently displayed option chain
	DWORD			m_activeComboBox;
	char*			osVer;
	bool			m_specificDateRequested;

		// the old parser
//	short		ImportOptionChain( CFile& fp, CString url, bool linksToo );
//	short		LoadLinks( char* szCache );									// returns number of dates added
//	short		LoadOptions( char* szCache, long expiry, bool isCall );		// returns number of options loaded
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONCHAINDIALOG_H__07C14535_CBC3_4FC1_83F8_95005B1EC363__INCLUDED_)
