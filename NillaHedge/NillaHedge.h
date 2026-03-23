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

//#include "Option.h"

//#include "OleDateTimeEx.h"

//class	CStock;
//class	CPosition;
//class	CBond;
//class	COption;
//class	CBOSDatabase;
class	CDataManager;
//class	CBondManager;
class	COptionManager;
class	CStockManager;
class	CRegistryManager;
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
const double	sqrt2 = sqrt( 2.0 );
const double	sqrt3 = sqrt( 3.0 );

#define					WM_ICON_NOTIFY		(WM_USER + 102)		// BM has +101
const LRESULT	WM_NILLAHEDGE_ALREADY_RUNNING
                 = RegisterWindowMessage( _T("Is NillaHedge already running?") );
const LRESULT	WM_NILLAHEDGE_PARSE_DATA
                 = RegisterWindowMessage( _T("NillaHedge Parse Data") );

//typedef		CMap<long,long,CString,CString&>	CMapLongToString;
//typedef		CMap<CString,CString,long,long>		CMapStringToLong;

/////////////////////////////////////////////////////////////////////////////
// CNillaHedgeApp:
// See NillaHedge.cpp for the implementation of this class
//

class CNillaHedgeApp : public CWinApp
{
public:
	CNillaHedgeApp();
	~CNillaHedgeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNillaHedgeApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
//	void	NotFileOpenEnabled( bool enabled );

	//{{AFX_MSG(CNillaHedgeApp)
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

	COptionManager*		optMgr;
	CStockManager*		stkMgr;
	unsigned long		dv;					// RAPI DeviceID
	char*				na;					// licensed user '<first name> <last name>'
	char*				em;					// licensed user email
	BYTE*				uu;					// device uuid
	BYTE*				hu;					// hashed uuid on WM5, zero elsewhere
	char*				lpServerName;
	char*				lpPageName;
	wchar_t*			lpFileName;
	char*				urlRequest;
	DWORD				m_osVer;

protected:
	void				ExitPrep( short code );

	NOTIFYICONDATA		nid;		// never figured out how to ShowWindow after clicking on the tray icon
	CString				dbDir;
	HANDLE				hMutex;
};
//----------------------------------------------------------------------------------------------------
//	GLOBALS

	// year counting (a global function)
char*			CString2char( CString str );						// allocates a char* on the heap
void			EatWhitespace( char* str );
char*			ExtractToken( char* szCache, long tokLen );			// allocates heap space
long			FindSubStr( char* str, long skipCnt, char* sub );	// works like CString::Find()
CString			EjectChar( CString src, wchar_t ch );
void			EjectChar( char* str, char ch );
//CString			GetSerialNumber( void );						// moved to GetDeviceInfo.*
double			Discretize( double	dd );
//short			MakeShortMonYr( CString monYr );

inline double	sign( double xx )					{  return ( xx < 0.0 ) ? -1.0 : 1.0;			}
inline void		swap( double& xx, double& yy )		{  double tmp = xx;	xx = yy;	yy = tmp;		}
inline void		swap( CString& xx, CString& yy )	{  CString tmp = xx;	xx = yy;	yy = tmp;	}
//inline double	NRSign( double aa, double bb )		{  return ( bb >= 0.0 ) ? fabs(aa) : -fabs(aa);	}
//inline double	Max( double aa, double bb) {	return (aa > bb) ? aa : bb; }
//inline long	Max( long aa, long bb) {	return (aa > bb) ? aa : bb; }
//void			CSquickSort( CString aa[], short ll, short rr );
//int			CScompareFunc( CString& st1, CString& st2 );


double	normCDF( double xx );									// the univarNormIntegral()

/*			the following functions support Roll-Geske-Whaley
double	bivarNormIntegral(  double aa,    double bb,    double rho );
double	trivarNormIntegral( double hh1,   double hh2,   double hh3,
						    double rho12, double rho13, double rho23 );

double	ShoveltonQuadrature( double (*fcn)(double), double aa, double bb );

double	pntGnd( double ba, double bb, double bc,
			    double ra, double rb, double sr, double rr );
double	sincos2( double xx, double& cs2 );
double	pfInt( double xx );

double	biNormDist( double x1, double rho, double x2 );
double	triNormDist( double x1, double rho12, double x2, double rho23, double x3, double rho13 );
double	findXgivenY( double (*blackBox)( double xx ), double startX, unsigned short& loopLimit,
					 double (*targetFcn)( double zz ) = NULL, double errorTolerance	= 0.01 );
*/

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NILLAHEDGE_H__BF98493C_AE1F_4C52_95B7_A4A03AA8C4F2__INCLUDED_)

