// Stock.cpp: implementation of the CStock class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DateSupport.h"
//#include "NillaHedge.h"
//#include "Bond.h"
#include "Stock.h"
#include "Position.h"
//#include "OleDateTimeEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStock::CStock() : CBOS()
{		// have to have a zero parameter constructor for Serialization
	status = ActiveDefinition;
}
//---------------------------------------------------------------------
CStock::CStock( CStock& aStock ) : CBOS()
{
	symbol_ID = aStock.symbol_ID;
	desc = aStock.desc;
	for ( short ii = 0; ii < 4; ii++ )
	{	dividends[ii] = aStock.dividends[ii];
		dividendsArePaid[ii] = aStock.dividendsArePaid[ii];
		exDividendDates[ii] = aStock.exDividendDates[ii];
	}
	mktPrice = aStock.mktPrice;
	volatility = aStock.volatility;
	status = aStock.status;
}
//---------------------------------------------------------------------
CStock::CStock( CString sym ) : CBOS(sym)
{		//identical to CStock() except that symbol is an initializer
	symbol_ID = -1;
	desc = _T("");
	for ( short ii = 0; ii < 4; ii++ )
	{	dividends[ii] = 0;		// dollars
		dividendsArePaid[ii] = FALSE;
	}
	mktPrice			= 0;		// dollars
	volatility			= 0.3f;

	SYSTEMTIME    st;
	GetLocalTime(&st);		// we actually only need yr (below) when aStock is 0
	int	yr = st.wYear;

	COleDateTime tt1(yr,  2, 15, 0, 0, 0);
	exDividendDates[0]  = tt1;
	COleDateTime tt2(yr,  5, 15, 0, 0, 0);
	exDividendDates[1]  = tt2;
	COleDateTime tt3(yr,  8, 15, 0, 0, 0);
	exDividendDates[2]  = tt3;
	COleDateTime tt4(yr, 11, 15, 0, 0, 0);
	exDividendDates[3]  = tt4;
	status = ActiveDefinition;
}
//---------------------------------------------------------------------
CStock::CStock( CBOS& bos )
{
	openPosOffset = bos.openPosOffset;
	symbol_ID = bos.symbol_ID;
	desc_ID = bos.desc_ID;
	mktPrice = bos.getMktPrice();
	posIndx_ID = bos.posIndx_ID;
	def_ID = bos.def_ID;
	status = ActiveDefinition;
}
//--------------------------------------------------------------------
CStock::~CStock()
{
}

IMPLEMENT_SERIAL( CStock, CBOS, VERSIONABLE_SCHEMA | 1 )

//---------------------------------------------------------------------
void	CStock::Serialize( CArchive& ar )
{	if ( ar.IsStoring() )
	{	ar << desc;
		ar << dividends[0];
		ar << dividends[1];
		ar << dividends[2];
		ar << dividends[3];
		ar << dividendsArePaid[0];
		ar << dividendsArePaid[1];
		ar << dividendsArePaid[2];
		ar << dividendsArePaid[3];
		ar << exDividendDates[0];
		ar << exDividendDates[1];
		ar << exDividendDates[2];
		ar << exDividendDates[3];
		ar << mktPrice;
		ar << symbol;
		ar << volatility;
	}
	else	// loading
	{	ar >> desc;
		ar >> dividends[0];
		ar >> dividends[1];
		ar >> dividends[2];
		ar >> dividends[3];
		ar >> dividendsArePaid[0];
		ar >> dividendsArePaid[1];
		ar >> dividendsArePaid[2];
		ar >> dividendsArePaid[3];
		ar >> exDividendDates[0];
		ar >> exDividendDates[1];
		ar >> exDividendDates[2];
		ar >> exDividendDates[3];
		ar >> mktPrice;
		ar >> symbol;
		ar >> volatility;
	}
}
//---------------------------------------------------------------------
double	CStock::aggShares( void )
{
	double	aggShrs = 0.0;
	CMapPtrToPtr*	posLst = getPositionList();
	if ( posLst != NULL )
	{	CPosition*	pos;
		long	posOffset = openPosOffset;
		while ( posLst->Lookup( (void*)posOffset, (void*&)pos ) )
		{	aggShrs += pos->nUnits;

				// prepare for next loop pass
			posOffset = pos->prevPos_ID;
		}
	}
	return	aggShrs;
}			// aggShares()
//---------------------------------------------------------------------
double		CStock::aggDivCostSharesRet(
	double&				aggDiv,
	double&				aggCost,
	double&				aggShares,
	BOOL				incDivInReturn	)
{		// Deliberately NOT using a function like COption::calcYears() because
		// we need day level resolution to decide whether to include dividends or not
	double				wtdReturn = 0.0;			// the return value
	COleDateTime		today = COleDateTime::GetCurrentTime();
	int					nowYr  = today.GetYear();
		// lastLeap behavior:  {1997, 1998, 1999, 2000} -> 1996
		//	int					lastLeap = nowYr - nowYr%4;
	int					nowDOY = today.GetDayOfYear();
	double				aggShrYrs = 0.0;
	COleDateTimeSpan	timeSpan;

	CMapPtrToPtr*	posLst = getPositionList();
	if ( posLst == NULL )
		return	0.0;

		// across all positions ...
	CPosition*	pos;
	long		posOffset = openPosOffset;
	while ( posLst->Lookup( (void*)posOffset, (void*&)pos ) )
	{	aggCost   += pos->totalCost;	// aggregate cost of all positions
		aggShares += pos->nUnits;		// aggregate number of shares

		COleDateTime		buyDate = pos->purchaseDate;
		int		buyYr = buyDate.GetYear();
		int		buyDOY = buyDate.GetDayOfYear();
			// firstLeap behavior:  {1997, 1998, 1999, 2000} -> 2000
			//int		firstLeap = buyYr + ((buyYr%4 > 0) ? 4 - buyYr%4 : 0);
			//int		numLeaps = (lastLeap >= firstLeap) ? 1 + (lastLeap - firstLeap) / 4 : 0;
		
			// the following is a workaround for bug in timeSpan.operator-() and/or GetTotalDays()
		int		spDays;
		if ( nowYr == buyYr )
		{	spDays = nowDOY - buyDOY;
		}
		else
		{	timeSpan = today - buyDate;		// careful, this produces 0 when difference is 1 day
			spDays = (int)floor( timeSpan.GetTotalDays() );
		}
		double	numYrs = (double)spDays / 365.0;
		double	shrYrs = pos->nUnits * numYrs;
		aggShrYrs += shrYrs;
		double  curVal = mktPrice * pos->nUnits;

			// for each quarter's dividend ...
		double  div_ii = 0.0;
		for ( short jj = 0; jj <= 3; jj++ )
		{	double  yrsOfDiv = 0.0;
			if ( dividendsArePaid[jj] )
			{	int	 exDivDOY = exDividendDates[jj].GetDayOfYear();
				if (  nowYr == buyYr  &&  buyDOY < exDivDOY  &&  exDivDOY <= nowDOY )
					yrsOfDiv++;
				else
				{	if (   buyDOY <  exDivDOY ) yrsOfDiv++;
					if ( exDivDOY <= nowDOY   ) yrsOfDiv++;
					yrsOfDiv += max(0, nowYr - buyYr - 1);
				}
			}
			div_ii += yrsOfDiv * dividends[jj] * pos->nUnits;
		}
		aggDiv += div_ii;
		double  return_ii = 0.0;
		if ( pos->totalCost != 0.0  &&  numYrs != 0.0 )
		{		// the following represents a problem since we converted incDivInReturn to
				// a portfolio attribute
			return_ii = log( (curVal + (incDivInReturn ? div_ii : 0.0)) / pos->totalCost ) / numYrs;
		}
		wtdReturn += shrYrs * return_ii;

			// prepare for next pass
		posOffset = pos->prevPos_ID;
	}
	return wtdReturn / aggShrYrs;
}			// aggDivCostSharesRet()
//---------------------------------------------------------------------
double		CStock::curYield( double curStockPrice )
{	if ( curStockPrice <= 0.0  &&  mktPrice <= 0.0 )
		return 0.0;
	bool	thereAreDividends = (dividends[0] > 0.0  &&  dividendsArePaid[0])
							||  (dividends[1] > 0.0  &&  dividendsArePaid[1])
							||  (dividends[2] > 0.0  &&  dividendsArePaid[2])
							||  (dividends[3] > 0.0  &&  dividendsArePaid[3]);
	if ( !thereAreDividends )
		return 0.0;
	
	double	aggDiv = 0.0;
	for ( short jj = 0; jj <= 3; jj++ )
	{	if ( dividendsArePaid[jj] )
			aggDiv += dividends[jj];
	}
		// annual yield as a fraction (not a percentage)
	double stockPrice = ( curStockPrice > 0.0 ) ? curStockPrice : mktPrice;
		// FV = PV * exp( r * t ),  t = 1,  PV = curStkPrice,  FV = PV + divs
		// log((PV + divs) / PV) / t = r
	return log( (stockPrice + aggDiv) / stockPrice );
}			// curYield()
//---------------------------------------------------------------------
double	CStock::lastDivB4Date(
	COleDateTime	refDate,
	COleDateTime&	exDivDate		)
{	exDivDate = 0.0;
	bool	thereAreDividends = (dividends[0] > 0.0  &&  dividendsArePaid[0])
							||  (dividends[1] > 0.0  &&  dividendsArePaid[1])
							||  (dividends[2] > 0.0  &&  dividendsArePaid[2])
							||  (dividends[3] > 0.0  &&  dividendsArePaid[3]);
	if ( !thereAreDividends )
		return 0.0;

		// return the last dividend (day before its Ex-dividend) date and the amount
		// prior to the refDate
	short	lastDiv_ii = -1;					// an invalid index into exDividendDates
	int		refDay = refDate.GetDayOfYear();
	int		fewestDaysAway = 400, daysAway;
	for ( short	ii = 0; ii < 4; ii++ )
	{	if ( dividendsArePaid[ii] )
		{		// assume that the divDate is in the same year as refDate
				// don't include the exDiv date
			daysAway = refDay - exDividendDates[ii].GetDayOfYear();		// an Ex-dividend date
			if ( daysAway < 0.0 )
				daysAway += 365;						// if 0, today is an ex-div date
			if ( daysAway < fewestDaysAway )
			{	fewestDaysAway = daysAway;
				lastDiv_ii = ii;
			}
		}
	}
		// if we found an appropriate dividend, create return values
	double	lastDiv = 0.0;										// return value
	if ( lastDiv_ii >= 0 )
	{	lastDiv = dividends[ lastDiv_ii ];
		exDivDate = exDividendDates[ lastDiv_ii ];
			// refDate supplies the year of interest
		int		yr = refDate.GetYear();
		if ( daysAway > refDay )
			yr--;
		int		mo = exDivDate.GetMonth();
		int		da = exDivDate.GetDay();
			// ensure that Ex-dividend date (in ref date's year) is feasible
		if ( da == 29  &&  mo == 2  &&  yr % 4 == 0 )
			da = 28;
		exDivDate.SetDate( yr, mo, da );
	}
	return	lastDiv;
}			// lastDivB4Date()
//---------------------------------------------------------------------
double			CStock::PresentValueDivs(
	COleDateTime	fromDate,				// not inclusive
	COleDateTime	toDate,					// inclusive
	double			discRate			 )
{		// compute the discounted value of dividends between the dates given
		// starting on the day after fromDate through (inclusive) the toDate
	if ( fromDate > toDate )
		return 0.0;
	bool	thereAreDividends = (dividends[0] > 0.0  &&  dividendsArePaid[0])
							||  (dividends[1] > 0.0  &&  dividendsArePaid[1])
							||  (dividends[2] > 0.0  &&  dividendsArePaid[2])
							||  (dividends[3] > 0.0  &&  dividendsArePaid[3]);
	if ( ! thereAreDividends )
		return 0.0;

		// for each quarter's dividend (in the abstract, not from positions)
	int		fromDay = fromDate.GetDayOfYear();
	int		fromYr = fromDate.GetYear();
#ifdef _DEBUG
//	TRACE( _T("Stock::PresentValueDivs: fromDate=%s, toDate=%s\n"),
//			fromDate.Format(VAR_DATEVALUEONLY), toDate.Format(VAR_DATEVALUEONLY) );
#endif
	double	pvDivs = 0.0;							// the return value
	for ( short ii = 0; ii <= 3; ii++ )
	{	if ( dividendsArePaid[ii] )
		{	for ( int yr = fromYr; yr <= toDate.GetYear(); yr++ )
			{	COleDateTime	divDate = dayBefore( exDividendDates[ii], yr );
				if ( divDate > fromDate  &&  divDate <= toDate )
				{	long	spDays;
					double	divYrs = calcYears( fromDate, divDate, spDays );
#ifdef _DEBUG
//	TRACE( _T("Stock::PresentValueDivs: divDate=%s, dividends[%d]=%g, discRate=%g, divYrs=%g, exp(-discRate*divYrs)=%g\n"),
//		divDate.Format(VAR_DATEVALUEONLY), ii, dividends[ii], discRate, divYrs, exp(-discRate * divYrs ) );
#endif
					pvDivs += dividends[ii] * exp( -discRate * divYrs );
				}
			}
		}
	}
#ifdef _DEBUG
//	TRACE( _T("Stock::PresentValueDivs: pvDivs=%g\n"), pvDivs );
#endif
	return	pvDivs;
}			// PresentValueDivs()
//---------------------------------------------------------------------
/*
double	CStock::fracYears( COleDateTime fromDate, COleDateTime toDate )
{		// this is NOT the same functionality as in calcYears()
		// we're only interested in the fraction of a year for a dividend,
		// not the time span of ownership.

		// Force the startDate to be in the year before the toDate
	COleDateTime startDate( fromDate );
	startDate.SetDate( toDate.GetYear() - 1, fromDate.GetMonth(), fromDate.GetDay() );

	COleDateTimeSpan  timeSpan = toDate - startDate;
		// Warning (XXX) GetTotalDays() may have a bug in it.
		// it doesn't handle 1 day differences correctly
	double	spDays = fmod( timeSpan.GetTotalDays(), 365.0);
	if ( spDays < 1.0 ) spDays += 365.0;
	return	spDays / 365.0f;
}				// fracYears()
//---------------------------------------------------------------------
COleDateTime*	CStock::nearestExDivDate( COleDateTime	refDate )
{	unsigned short	nearestDiv = -1;		// invalid index into exDividendDates
	int		refDay = refDate.GetDayOfYear();
	int		fewestDaysAway = 400, daysFwd, daysBck, daysAway, dayNum;
	for ( unsigned short	ii = 0; ii < 4; ii++ )
	{	if ( dividendsArePaid[ii] )
		{		// ensure that the exDivDate is in the same year as refDate
				// to eliminate Leap year influences
			exDividendDates[ii].SetDate( refDate.GetYear(),
										 exDividendDates[ii].GetMonth(),
										 exDividendDates[ii].GetDay() );
			dayNum = exDividendDates[ii].GetDayOfYear();
			daysFwd = dayNum - refDay;
			if ( daysFwd < 0 )
				daysFwd += 365;
			daysBck = refDay - dayNum;
			if ( daysBck < 0 )
				daysBck += 365;
			daysAway = ( daysBck < daysFwd ) ? daysBck : daysFwd;
			if ( daysAway < fewestDaysAway )
			{	fewestDaysAway = daysAway;
				nearestDiv = ii;
			}
		}
	}
	return nearestDiv >= 0 ? &exDividendDates[nearestDiv] : (COleDateTime*)0;
}			// nearestExDivDate()
*/
//---------------------------------------------------------------------
/*
double		CStock::aggShares( void )
{		// computes the sum of the shares across all positions
	double  aggShares = 0.0;		// return value

	int	nPos = positions.GetSize();
	for ( int  ii = 0; ii < nPos; ii++ )
	{	CPosition* pos = (CPosition*)(positions.GetAt(ii));
		aggShares += pos->nUnits;
	}
	return	aggShares;
}
//---------------------------------------------------------------------
double		CStock::aggCost( void )
{		// computes the sum of the totalCost attribute across all positions
	double  aggCost = 0.0;		// return value

	int	nPos = positions.GetSize();
	for ( int  ii = 0; ii < nPos; ii++ )
	{	CPosition* pos = (CPosition*)(positions.GetAt(ii));
		aggCost += pos->totalCost;
	}
	return	aggCost;
}
//---------------------------------------------------------------------
void		CStock::aggCostShares(
	double&  aggCost,
	double&  aggShares				)
{		// computes the aggregate number of shares & cost across all positions
	int	nPos = positions.GetSize();
	for ( int  ii = 0; ii < nPos; ii++ )
	{	CPosition* pos = (CPosition*)(positions.GetAt(ii));
		aggCost   += pos->totalCost;
		aggShares += pos->nUnits;
	}
	return;
}
//---------------------------------------------------------------------
double		CStock::aggDivs( void )
{	double			aggDiv = 0.0;		// return value
	COleDateTime	timeNow = COleDateTime::GetCurrentTime();
	int				nowYr  = timeNow.GetYear();
	int				nowDOY = timeNow.GetDayOfYear();

		// across all positions ...
	int	nPos = positions.GetSize();
	for ( int  ii = 0; ii < nPos; ii++ )
	{	CPosition* pos = (CPosition*)(positions.GetAt(ii));
		COleDateTime buyDate = pos->purchaseDate;
		int	buyYr = buyDate.GetYear();
		int	buyDOY = buyDate.GetDayOfYear();
		double  div_ii = 0.0;

			// for each quarter's dividend ...
		for ( short jj = 0; jj <= 3; jj++ )
		{	double  yrsOfDiv = 0;
			if ( dividendsArePaid[jj] )
			{	int	 exDivDOY = exDividendDates[jj].GetDayOfYear();
				if (  nowYr == buyYr  &&  buyDOY < exDivDOY  &&  exDivDOY <= nowDOY )
					yrsOfDiv++;
				else
				{	if (   buyDOY <  exDivDOY )
						yrsOfDiv++;
					if ( exDivDOY <= nowDOY   )
						yrsOfDiv++;
					yrsOfDiv += max(0, nowYr - buyYr - 1);
				}
			}
			div_ii += yrsOfDiv * (double)dividends[jj] * pos->nUnits;
		}
		aggDiv += div_ii;
	}
	return	aggDiv;
}
//---------------------------------------------------------------------
double		CStock::aggDivCostShares(
	double&			aggCost,
	double&			aggShares		)
{	double			aggDiv = 0.0;		// return value
	COleDateTime	timeNow = COleDateTime::GetCurrentTime();
	int				nowYr  = timeNow.GetYear();
	int				nowDOY = timeNow.GetDayOfYear();

		// across all positions ...
	int	nPos = positions.GetSize();
	for ( int  ii = 0; ii < nPos; ii++ )
	{	CPosition* pos = (CPosition*)(positions.GetAt(ii));
		aggCost   += pos->totalCost;
		aggShares += pos->nUnits;
		COleDateTime buyDate = pos->purchaseDate;
		int	buyYr = buyDate.GetYear();
		int	buyDOY = buyDate.GetDayOfYear();
		double  div_ii = 0.0;

			// for each quarter's dividend ...
		for ( short jj = 0; jj <= 3; jj++ )
		{	double  yrsOfDiv = 0.0;
			if ( dividendsArePaid[jj] )
			{	int	 exDivDOY = exDividendDates[jj].GetDayOfYear();
				if (  nowYr == buyYr  &&  buyDOY < exDivDOY  &&  exDivDOY <= nowDOY )
					yrsOfDiv++;
				else
				{	if (   buyDOY <  exDivDOY )
						yrsOfDiv++;
					if ( exDivDOY <= nowDOY   )
						yrsOfDiv++;
					yrsOfDiv += max(0, nowYr - buyYr - 1);
				}
			}
			div_ii += yrsOfDiv * (double)dividends[jj] * pos->nUnits;
		}
		aggDiv += div_ii;
	}
	return	aggDiv;
}
*/
//---------------------------------------------------------------------
