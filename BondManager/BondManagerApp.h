// NillaHedge.h : main header file for the NILLAHEDGE application
//

#if !defined(AFX_NILLAHEDGE_H__BF98493C_AE1F_4C52_95B7_A4A03AA8C4F2__INCLUDED_)
#define      AFX_NILLAHEDGE_H__BF98493C_AE1F_4C52_95B7_A4A03AA8C4F2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'StdAfx.h' before including this file for PCH
#endif

#include <afxtempl.h>
#include "resource.h"       // main symbols
#include "AssetType.h"
#include "GetDeviceInfo.h"
#include "utils.h"

//#include "Option.h"

//#include "OleDateTimeEx.h"

//class	CStock;
//class	CPosition;
//class	CBond;
//class	COption;
//class	CBOSDatabase;
class	CDataManager;
class	CBondManager;
//class	COptionManager;
//class	CStockManager;
//class	CRegistryManager;
//class	CPositionDisplayOptions;
//------------------------------------------------------------------------------------------
// broadly applicable enums....

enum SaveVerifyPreferences
{	DefinitionClose						= 0x1,
	DefinitionChangeSymbol				= 0x2,
	AnalyzerClose						= 0x4,
	AnalyzerChangeSymbol				= 0x8,
	PositionsDelete						= 0x10,
	PositionsModify						= 0x20,
	OptionChainStockPriceUpdateVerify	= 0x40			// i.e. stock price
};

enum PositionListColumnVisibility
{	AggregateIncome			   =      0x1,
	AnnualizedNetGain		   =      0x2,
	CapitalGain				   =      0x4,
	ExerciseValue			   =      0x8,
	InitialCost				   =     0x10,
	MarketValue				   =     0x20,
	NetGain					   =     0x40,
	Note					   =     0x80,
	NumUnits				   =    0x100,
	PurchaseDate			   =    0x200,

	AggregateIncomeOverview	   =    0x400,
	AnnualizedNetGainOverview  =    0x800,
	CapitalGainOverview		   =   0x1000,
	ExerciseValueOverview	   =   0x2000,
	InitialCostOverview		   =   0x4000,
	MarketValueOverview		   =   0x8000,
	NetGainOverview			   =  0x10000,
	NoteOverview			   =  0x20000,
	NumUnitsOverview		   =  0x40000,
	PurchaseDateOverview	   =  0x80000,

	InitiallySuppressPositions = 0x100000
};
//---------------------------------------------------------------------------
	// 40 digits exceeds double resolution
const double	PI =  3.141592653589793238462643383279502884197;
const double	sqrt2PI = sqrt( 2.0*PI );
const double	sqrt2 = sqrt( 2.0 );
const double	sqrt3 = sqrt( 3.0 );

const LRESULT	WM_BONDMANAGER_ALREADY_RUNNING
                 = RegisterWindowMessage( _T("Is BondManager already running?") );
#define					WM_ICON_NOTIFY		(WM_USER + 101)
//const LRESULT	WM_ICON_NOTIFY
//				 = RegisterWindowMessage( _T("User Clicked System Tray Icon") );

//typedef		CMap<long,long,CString,CString&>	CMapLongToString;
//typedef		CMap<CString,CString,long,long>		CMapStringToLong;

/////////////////////////////////////////////////////////////////////////////
// CBondManagerApp:
// See NillaHedge.cpp for the implementation of this class
//

class CBondManagerApp : public CWinApp
{
public:
	CBondManagerApp();
	~CBondManagerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBondManagerApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
//	void	NotFileOpenEnabled( bool enabled );

	//{{AFX_MSG(CBondManagerApp)
	afx_msg void OnAppAbout();
//	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnActivate();
	afx_msg void OnClose();
	afx_msg void OnHibernate();
	// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CDataManager*		GetDataManager( AssetType inst );
	CMapStringToPtr*	GetSymbolTable( AssetType inst );
	int					GetSymbolTableSize( AssetType inst );
	CString				GetDBdir( void );
//	void				SetNID_HWND( HWND hWnd ) {	nid.hWnd = hWnd;	}

//	HWND				hWnd;		// location of the WindowEx handle
//	CBOSDatabase*		pDB;
	CBondManager*		bndMgr;
	DWORD				m_osVer;
	unsigned long		dv;			// RAPI DeviceID
	char*				na;			// licensed user '<first name> <last name>'
	char*				em;			// licensed user email
	BYTE*				uu;			// device uuid
	BYTE*				hu;			// hashed uuid on WM5, zero elsewhere

protected:
	NOTIFYICONDATA		nid;		// never figured out how to ShowWindow after clicking on the tray icon
	CString				dbDir;
	HANDLE				hMutex;
	void				ExitPrep( short code );

};
//----------------------------------------------------------------------------------------------------
//	GLOBALS

char*			CString2char( CString str );						// allocates a char* on the heap
void			EatWhitespace( char* str );
char*			ExtractToken( char* szCache, long tokLen );			// allocates heap space
long			FindSubStr( char* str, long skipCnt, char* sub );	// works like CString::Find()
CString			EjectChar( CString src, wchar_t ch );
void			EjectChar( char* str, char ch );

double			Discretize( double	dd );
//long			packOleDate( COleDateTime aDate );
//COleDateTime	unpackOleDate( long compressedOleDate );
CString			EuroFormat( COleDateTime aDate );
long			MakeLongMonYr( CString monYr );
//short			MakeShortMonYr( CString monYr );

inline double	sign( double xx )					{  return ( xx < 0.0 ) ? -1.0 : 1.0;			}
inline void		swap( double& xx, double& yy )		{  double tmp = xx;	xx = yy;	yy = tmp;		}
inline void		swap( CString& xx, CString& yy )	{  CString tmp = xx;	xx = yy;	yy = tmp;	}
inline double	Nprime( double xx )					{  return	exp( -0.5*xx*xx ) / sqrt2PI;		}
//inline double	NRSign( double aa, double bb )		{  return ( bb >= 0.0 ) ? fabs(aa) : -fabs(aa);	}
//inline double	Max( double aa, double bb) {	return (aa > bb) ? aa : bb; }
//inline long	Max( long aa, long bb) {	return (aa > bb) ? aa : bb; }
//void			CSquickSort( CString aa[], short ll, short rr );
//int			CScompareFunc( CString& st1, CString& st2 );

int				intMonthFrom3CharStr( CString cs_mo );
COleDateTime	NextMonth( COleDateTime date );
COleDateTime	dayBefore( COleDateTime refDate, int yr = 0 );
inline bool		isLeapYr( int yr )	{  return !(yr % 400)  ||  (!(yr % 4)  &&  (yr % 100));			}
int				numLeapDays( COleDateTime d1, COleDateTime d2 );

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NILLAHEDGE_H__BF98493C_AE1F_4C52_95B7_A4A03AA8C4F2__INCLUDED_)

