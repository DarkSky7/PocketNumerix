// Stock.h: interface for the CStock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STOCK_H__FA60B975_2FF5_49AD_9F13_B86DBA0FEEA2__INCLUDED_)
#define AFX_STOCK_H__FA60B975_2FF5_49AD_9F13_B86DBA0FEEA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BOS.h"
//#include "OleDateTimeEx.h"

//#include <afxcoll.h>

class CStock : public CBOS
{
	DECLARE_SERIAL( CStock )

public:						// methods
						CStock();
						CStock( CBOS& bos );
						CStock( CString sym );
						CStock( CStock& aStock );
	virtual				~CStock();

	void				Serialize( CArchive& ar );
	AssetType			getAssetType( void ) {	return Stock;	};

	double				aggShares( void );
	void				setDiv( double	divAmt, unsigned short idx );

		// the following function may be obsolete (after the ...ValueDialogs are deleted...
	double				aggDivCostSharesRet( double& aggDiv, double& aggCost,
											 double& aggShares, BOOL incDivInReturn );

	double				curYield( double curStockPrice = 0.0 );
	double				lastDivB4Date( COleDateTime refDate, COleDateTime& exDivDate );
	double				PresentValueDivs( COleDateTime fromDate,
										  COleDateTime toDate, double discRate );
//	COleDateTime		dayBefore( COleDateTime refDate, int yr = 0 );	// when 0, uses refDate's year

public:						//attributes
	float				volatility;				// really only a couple of meaningful digits
	float				dividends[4];
	COleDateTime		exDividendDates[4];
	BOOL				dividendsArePaid[4];	// TRUE if dividends paid associated exDividendDate
};
/*
	double			fracYears( COleDateTime fromDate, COleDateTime toDate );	// not used
	COleDateTime*	nearestExDivDate( COleDateTime	fromDate );
	double			aggDivs( void );
	double			aggCost( void );				// sums across all positions
	double			aggShares( void );				// sums across all positions
	void			aggCostShares( double&  aggCost, double&  aggShares );
	double			aggDivCostShares( double& aggCost, double&  aggShares );
*/
#endif // !defined(AFX_STOCK_H__FA60B975_2FF5_49AD_9F13_B86DBA0FEEA2__INCLUDED_)
