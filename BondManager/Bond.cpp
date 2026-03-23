// Bond.cpp: implementation of the CBond class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "BondManagerApp.h"
#include "Bond.h"
#include <math.h>				// floor(), pow(), exp()
#include "Position.h"
//#include "BOSDatabase.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBond::CBond() : CBOS( _T("") )
{
	dayCounting = UnknownDayCountMethod;
	status = ActiveDefinition;
}

CBond::CBond( CString sym ) : CBOS(sym)
{
	couponRate = 0.0;
	dayCounting = UnknownDayCountMethod;
	status = ActiveDefinition;
}

CBond::CBond( CBOS& bos )
{
	openPosOffset = bos.openPosOffset;
	symbol_ID = bos.symbol_ID;
	desc_ID = bos.desc_ID;
	mktPrice = bos.getMktPrice();
	posIndx_ID = bos.posIndx_ID;
	def_ID = bos.def_ID;
	couponRate = 0.0;
	dayCounting = UnknownDayCountMethod;
	status = ActiveDefinition;
}

CBond::CBond( CBond& bnd )
{
	openPosOffset = bnd.openPosOffset;
	symbol_ID = bnd.symbol_ID;
	desc_ID = bnd.desc_ID;
	mktPrice = bnd.mktPrice;

	desc = bnd.desc;
	couponsPerYear = bnd.couponsPerYear;
	couponRate = bnd.couponRate;
	dayCounting = bnd.dayCounting;
	incepDate = bnd.incepDate;
	maturityDate = bnd.maturityDate;
	positions = bnd.positions;
	symbol = bnd.symbol;
	parValue = bnd.parValue;
	status = bnd.status;
}

CBond::~CBond()
{
}

IMPLEMENT_SERIAL( CBond, CBOS, VERSIONABLE_SCHEMA | 1 )

//----------------------------------------------------------------------------------------------
void	CBond::setCouponRate( float cpr )
{
	couponRate = cpr;
}			// setCouponRate()
//----------------------------------------------------------------------------------------------
void	CBond::setCouponsPerYear( unsigned char cpy )
{
	couponsPerYear = cpy;
}			// setCouponRate()
//----------------------------------------------------------------------------------------------
void	CBond::setDayCounting( DayCountMethod dcm )
{
	dayCounting = dcm;
}			// setDayCounting( short& dcm )
//----------------------------------------------------------------------------------------------
void	CBond::setIncepDate( COleDateTime incDt )
{
	incepDate = incDt;
}			// setIncepDate()
//----------------------------------------------------------------------------------------------
void	CBond::setMaturityDate( COleDateTime matDt )
{
	maturityDate = matDt;
}			// setMaturityDate()
//----------------------------------------------------------------------------------------------
void	CBond::setParValue( float parVal )
{	
	parValue = parVal;
}			// setParValue()
//----------------------------------------------------------------------------------------------
double	CBond::YieldToMaturity( BondEvaluationContext& evalCtx )
{
	if ( evalCtx.ytmIsValid )
		return	evalCtx.ytm;

	if ( ! evalCtx.yrsToMaturityIsValid )
	{	long	spDays;
			// if this is a resale Bond, evalDate will be after the incepDate...
			// we want to avoid counting coupons before the bond's incepDate
		COleDateTime	startDate = evalCtx.fromDate;
		if ( startDate < incepDate )
			startDate = incepDate;

		evalCtx.yrsToMaturity = calcYears( startDate, maturityDate, dayCounting, spDays );
		evalCtx.yrsToMaturityIsValid = true;
	}

	evalCtx.ytm = YieldToMaturityCore( evalCtx.ytmErr, evalCtx.yrsToMaturity );
	evalCtx.ytmIsValid = ( evalCtx.ytmErr < GetErrTol() );
	return	evalCtx.ytm;
}			// YieldToMaturity()
//----------------------------------------------------------------------------------------------
double		CBond::YieldToMaturity( double& minErr, COleDateTime evalDate /* = COleDateTime::GetCurrentTime() */ )
{		// if this is a resale Bond, evalDate will be after the incepDate...
		// we want to avoid counting coupons before the bond's incepDate
	long	spDays;
	COleDateTime	startDate = evalDate;
	if ( startDate < incepDate )
		startDate = incepDate;

	double	yrs	= calcYears( startDate, maturityDate, dayCounting, spDays );
	if ( yrs <= 1e-5 )
		return	0.0;

#ifdef _DEBUG
//	TRACE( _T("Bond::YieldToMaturity2: startDate=%s, yrs=%g\n"),
//		startDate.Format(VAR_DATEVALUEONLY), yrs );
#endif

	return	YieldToMaturityCore( minErr, yrs );
}			// YieldToMaturity()
//------------------------------------------------------------------------------------------------
double	CBond::YieldToMaturityCore( double& err, double yrs )
{
	if ( mktPrice < 0.01  ||  yrs < 1e-5 )
		return	0.0;

		// the zero coupon approach to YTM
		// FV = PV * exp( r * t )		where r is a forward (evalDate -> maturity) rate
		// (par + coupons) = marketPrice * exp( r * t )
		// r = log( (par + coupons) / marketPrice ) / t
	double	topLine = parValue + getCoupon();
	double	r_1 = log( topLine / mktPrice ) / yrs;
	if ( couponRate <= 0.0  ||  couponsPerYear < 1 )
		return	r_1;								// zero-coupon bonds bail out now

	double	mv2, mv1 = PresentValue( yrs, 6, r_1 );
	double	er2, er1 = mv1 - mktPrice;
		// recall the inverse relationship between market price and interest rate
	double	slope, bestR, r_2 = r_1 + (er1 > 0.0)  ?  0.01 : -0.01;

	const double	errTol = GetErrTol();
	double	minErr = 1e9;
		// use the secant method to find the root (min) of the error function
	unsigned short	loops = 0, loopLimit = 20;		// count number of tuning loops
	while ( loops++ < loopLimit )
	{
		mv2 = PresentValue( yrs, 6, r_2 );
		er2 = mv2 - mktPrice;

			// track the best solution
		if ( fabs(er2) < fabs(minErr) )
		{	minErr = er2;
			bestR = r_2;
		}

		if ( fabs(minErr) < errTol )
			break;

			// prepare for next pass
		double	deltaErr = ( er2 - er1 );
		if ( fabs(deltaErr) < 0.5 * errTol )
			break;
		slope = ( r_2 - r_1 ) / deltaErr;
		er1 = er2;
		mv1 = mv2;
		r_1 = r_2;
		r_2 -= er2 * slope * 0.85;
	}
	return	bestR;
}			// YieldToMaturityCore()
//----------------------------------------------------------------------------------------------
double	CBond::PresentValueCoupons( COleDateTime fromDate, COleDateTime toDate, double ytm /* = 0.0 */ )
{		// this PresentValueCoupons() overload compounds coupons from the fromDate to today
		// and discounts coupons from the toDate to the maturityDate
	if ( couponsPerYear < 1  ||  toDate < fromDate )
		return	0.0;								// there are no coupons or the date range is infeasible

	if ( fromDate < incepDate )
		fromDate = incepDate;

	if ( toDate > maturityDate )
		toDate = maturityDate;

	long	spDays;
	double	tmp;			// either a simple coupon count or a present value prior to scaling by the coupon amount
	double	nTodayCpnPeriods;
	double	nFromDateCpnPeriods = calcYears( fromDate, maturityDate, dayCounting, spDays ) * couponsPerYear;
	double	nToDateCpnPeriods   = calcYears(   toDate, maturityDate, dayCounting, spDays ) * couponsPerYear;
#ifdef _DEBUG
//	TRACE( _T("Bond::PresentValueCoupons: nFromDateCpnPeriods=%g, nToDateCpnPeriods=%g\n"),
//			nFromDateCpnPeriods, nToDateCpnPeriods );
#endif
	if ( ytm == 0.0 )
	{		// no discounting ...  just count coupons and multiply by the coupon amount (below)
			// first carve off fractional part of the long period,
			// reduce by the entire short period, then cut that fraction off
			// add one to account for the fact that there is
			// always one more coupon than the number of coupon periods
		long	nCpns = (long)( (long)nFromDateCpnPeriods - nToDateCpnPeriods );
		double	fracPeriods = nToDateCpnPeriods - (long)nToDateCpnPeriods;
		if ( fracPeriods > 0.0 )
			nCpns++;												// toDate is a coupon date
		tmp = nCpns;
#ifdef _DEBUG
//		TRACE( _T("Bond::PresentValueCoupons: tmp=%g fromDate=%s toDate=%s\n"),
//			tmp, fromDate.Format(VAR_DATEVALUEONLY), toDate.Format(VAR_DATEVALUEONLY) );
#endif
	}
	else
	{		// want present value of the coupons on the given range
		double	periodicYield = ytm / couponsPerYear;
		COleDateTime today = COleDateTime::GetCurrentTime();
#ifdef _DEBUG
//		TRACE( _T("Bond::PresentValueCoupons: today=%s, fromDate=%s\n   toDate=%s, maturity=%s\n"),
//			today.Format(VAR_DATEVALUEONLY), fromDate.Format(VAR_DATEVALUEONLY),
//			toDate.Format(VAR_DATEVALUEONLY), maturityDate.Format(VAR_DATEVALUEONLY) );
#endif
		if ( today <= fromDate )
		{		// case 1 - all coupons are in the future (a bond pricing calculation)
				// i.e.:  incepDate  ..  today  ..  fromDate  ..  toDate  ..  maturityDate
				//			|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	coupon dates
				//						   |--------------------------------------->|	todayCpns
				//									   |--------------------------->|	fromDateCpns
				//													 |------------->|	toDateCpns
				//
				// calculate the value of fromDate coupons and remove the value of toDate coupons
				// then discount to today.  PresentValueCouponsCore() complements the exponent internally
			double	pvToDateCpns   = ValueCouponsCore( nToDateCpnPeriods,   periodicYield, true );
			double	pvFromDateCpns = ValueCouponsCore( nFromDateCpnPeriods, periodicYield, true );
			double	netPvCpns = pvFromDateCpns - pvToDateCpns;
			COleDateTimeSpan	ts = today - fromDate;
			if ( fabs(ts.GetTotalDays()) < 1.0 )
				nTodayCpnPeriods = nFromDateCpnPeriods;
			else
				nTodayCpnPeriods = calcYears( today, maturityDate, dayCounting, spDays ) * couponsPerYear;

				// now discount from the fromDate to today...
			tmp = netPvCpns
				* pow( 1.0 + periodicYield, nFromDateCpnPeriods - nTodayCpnPeriods );
		}
		else if ( toDate <= today )
		{		// case 2 - all coupons are in the past (income calculation or a user experiment in the BondAnalyzer)
				// i.e.:  incepDate  ..  fromDate  ..  toDate  ..  today  ..  maturityDate
				//			|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	coupon dates
				//						   |--------------------------------------->|	fromDateCpns
				//										 |------------------------->|	toDateCpns
				//													 |------------->|	todayCpns
				//
				// calculate the value of fromDate coupons and remove the value of toDate coupons
				// then compound to today.  We must complement the exponent sent to PresentValueCouponsCore()
			COleDateTimeSpan	ts = today - toDate;
			if ( fabs(ts.GetTotalDays()) < 1.0 )
				nTodayCpnPeriods = nToDateCpnPeriods;
			else
				nTodayCpnPeriods = calcYears( today, maturityDate, dayCounting, spDays ) * couponsPerYear;

				// reduce            exact coupon periods     by today's coupon periods, so the results are non-integral again
			nFromDateCpnPeriods = (long)nFromDateCpnPeriods - nTodayCpnPeriods;
			nToDateCpnPeriods   = (long)nToDateCpnPeriods   - nTodayCpnPeriods;
				// now:   incepDate  ..  fromDate  ..  toDate  ..  today  ..  maturityDate
				//			|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	coupon dates
				//							|<-----------------------|				|	fromDateCpns
				//											|<-------|				|	toDateCpns
				//													 |------------->|	todayCpns
				//
				// compute future coupon value on today's date
				// false prevents flipping the sign of nCpnPeriods, so
				// we're compounding (false) instead of discounting (default = true)
			double	fvToDateCpns   = ValueCouponsCore( nToDateCpnPeriods,   periodicYield, false );
			double	fvFromDateCpns = ValueCouponsCore( nFromDateCpnPeriods, periodicYield, false );
			tmp = fvFromDateCpns - fvToDateCpns;
		}
		else
		{		// case 3 - some coupons are in the past and some are in the future (user experiment in the BondAnalyzer)
				// i.e.:  incepDate  ..  fromDate  ..  today  ..  toDate  ..  maturityDate
				//			|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	coupon dates
				//						   |--------------------------------------->|	fromDateCpns
				//										 |------------------------->|	todayCpns
				//													 |------------->|	toDateCpns
				//
				// try to avoid calculating years if today is within a day of toDate or fromDate
				// examine the fromDate
			COleDateTimeSpan	ts = today - fromDate;
			if ( fabs(ts.GetTotalDays()) < 1.0 )
				nTodayCpnPeriods = nFromDateCpnPeriods;
			else
			{		// examine the toDate
				ts = today - toDate;
				if ( fabs(ts.GetTotalDays()) < 1.0 )
					nTodayCpnPeriods = nToDateCpnPeriods;
				else		// give up and compute years from today to maturity
					nTodayCpnPeriods = calcYears( today, maturityDate, dayCounting, spDays ) * couponsPerYear;
			}

				//
				// now:   incepDate  ..  fromDate  ..  today  ..  toDate  ..  maturityDate
				//			|	|	|	|	|	|	|	|	|	|	|	|	|	|	|	coupon dates
				//						    |<-----------|							|	fromDateCpns
				//										 |------------------------->|	todayCpns
				//													 |------------->|	toDateCpns
				//
				// toDate coupons are in the future (so are discounted)
				// fromDate coupons are in the past (so are compounded)

				// compute the present value of future coupons through toDate on today's date
			double	pvToDateCpns = ValueCouponsCore( nToDateCpnPeriods, periodicYield, true );
			double	pvTodayCpns = ( nToDateCpnPeriods == nTodayCpnPeriods )
								?  pvToDateCpns
								:  ValueCouponsCore( nTodayCpnPeriods,  periodicYield, true );
			double	netPvCpns = pvTodayCpns - pvToDateCpns;							// should be positive

				// compute the compound value of past coupons from fromDate to today's date
				// fromDate coupon periods must be aligned with a coupon date originating at today
			nFromDateCpnPeriods = (long)nFromDateCpnPeriods - nTodayCpnPeriods;
			if ( nFromDateCpnPeriods < 0.0 )
				nFromDateCpnPeriods = 0.0;
			double	fvFromDateCpns = ValueCouponsCore( nFromDateCpnPeriods, periodicYield, false );
			tmp = netPvCpns + fvFromDateCpns;
		}
	}
	return	getCoupon() * tmp;
}			// PresentValueCoupons()
//------------------------------------------------------------------------------------------------
double	CBond::ValueCouponsCore( double nCpnPeriods, double periodicYield, bool discount /* = true */ )
{		// if discount is true, the present value of coupons is given by the following expression times
		// the coupon amount.  The caller is responsible for scaling by the coupon amount.
		// when discount is false, coupons are compounded to create the future value
		//
		// Maple says:  sum( 1 / (1+y)^n, n=1..N ) is:
		//
		//    1 - (1+y)^(-N)					   (1+y)^(N+1) - 1
		// ----------------- for discounting, or ------------------- - 1 for compounding
		//			y									 y
		//
	if ( periodicYield == 0.0  ||  nCpnPeriods == 0.0 )
		return	0.0;

	long	wholeCpns = (long)nCpnPeriods;			// we're truncating here
	double	fracPeriods = nCpnPeriods - wholeCpns;
	double	sumOfSeries = 0.0;
	if ( wholeCpns != 0.0 )
	{	double	exponent = discount  ?  -wholeCpns  :  (wholeCpns + 1.0);
		double	powRes = pow( 1.0 + periodicYield, exponent );
		double	numer = discount  ?  (1.0 - powRes)  :  (powRes - 1.0);
		sumOfSeries = numer / periodicYield;
		if ( ! discount )
			sumOfSeries -= 1.0;
	}

		// now add in the nearest coupon and discount by the fractional period
		// if we're exactly on a coupon date (fracPeriods==0.0), we don't count the coupon, but
		// if we have any fractional periods, we count one more coupon
	if ( fracPeriods != 0.0 )
	{	sumOfSeries++;				// account for the the nearest coupon
			// now for the discounting
		sumOfSeries *= pow( 1.0 + periodicYield, discount  ?  -fracPeriods  :  fracPeriods );
	}

#ifdef _DEBUG
//	TRACE( _T("Bond::ValueCouponsCore: sumOfSeries=%g, nCpnPeriods=%g, periodicYield=%g\n"),
//		sumOfSeries, nCpnPeriods, periodicYield );
#endif

	return	sumOfSeries;
}			// ValueCouponsCore()
//------------------------------------------------------------------------------------------------
double	CBond::PresentValueCoupons( double yrs, char dummy, double ytm /* = 0.0 */ )
{		// this overload assumes that yrs represents time to maturity (from today),
		// so there are no past coupons to consider
	if ( couponsPerYear < 1  ||  couponRate <= 0.0 )
		return	0.0;

	double	periodicYield = ytm / couponsPerYear;
	double	nCpnPeriods = yrs * couponsPerYear;			// PresentValueCouponsCore adds the maturityDate coupon
	return	getCoupon() * ValueCouponsCore( nCpnPeriods, periodicYield );
}			// PresentValueCoupons()
//------------------------------------------------------------------------------------------------
double	CBond::PresentValuePar( double yrs, char dummy, double ytm /* = 0.0 */ )
{		// this overload has 'char dummy' as the second argument because the compiler
		// was erroneously mapping calls of PresentValuePar( double, double )
		//								 to PresentValuePar( COleDateTime, double )
	double	cpy = ( couponsPerYear < 1  ||  couponRate <= 0.0 )  ?  1.0  :  couponsPerYear;
	double	periodicYield = ytm / cpy;
	double	nCpnPeriods = yrs * cpy;

#ifdef _DEBUG
//	double	pvPar = parValue * pow( 1.0 + periodicYield, -nCpnPeriods );
//	double	expDisc = exp( - ytm * yrs );
//	TRACE( _T("Bond::PresentValuePar: pvPar=%g, yrs=%g, ytm=%g, perYield=%g, nCpnPeriods=%g, expDisc=%g\n"),
//		pvPar, yrs, ytm, periodicYield, nCpnPeriods, expDisc );
#endif

	return	parValue * pow( 1.0 + periodicYield, -nCpnPeriods );
}			// PresentValuePar()
//------------------------------------------------------------------------------------------------
double	CBond::PresentValuePar( COleDateTime evalDate, double ytm /* = 0.0 */ )
{	long	spDays;
	if ( evalDate < incepDate )
		evalDate = incepDate;

	double	yrs = calcYears( evalDate, maturityDate, dayCounting, spDays );
	return	PresentValuePar( yrs, 6, ytm );
}			// PresentValuePar()
//------------------------------------------------------------------------------------------------
double	CBond::MacaulayDuration( BondEvaluationContext& evalCtx )
{		// the coupon portion of macDur is given by
		// Maple says:  sum( n / (1+y)^n, n=1..N) is:
		//
		//   (1+y)		  ((N+1)y+1) * (1/(1+y))^N		
		// --------  -  ----------------------------
		//    y^2				  y^2
		// 
		// will still need to:
		// add in the effect of accrued interest up to the first coupon
		// upscale by the coupon amount
		// add in the par value portion
		// downscale everything by the market price
		//
	if ( evalCtx.macDurIsValid )
		return	evalCtx.macDur;

		// gating variables
	if ( mktPrice < 0.01 )
		return	0.0;

	if ( ! evalCtx.yrsToMaturityIsValid )
	{	long	spDays;
		COleDateTime	startDate = evalCtx.fromDate;
		if ( startDate < incepDate )
			startDate = incepDate;
		evalCtx.yrsToMaturity = calcYears( startDate, maturityDate, dayCounting, spDays );
		evalCtx.yrsToMaturityIsValid = true;
	}
	evalCtx.macDur = MacaulayDurationCore( evalCtx.yrsToMaturity, YieldToMaturity( evalCtx ) );
	evalCtx.macDurIsValid = true;
	return	evalCtx.macDur;
}			// MacaulayDuration()
//------------------------------------------------------------------------------------------------
double		CBond::MacaulayDurationCore( double yrs, double ytm )
{		// this is the per period Macaulay duration
	if ( ytm == 0.0  ||  yrs < 0.0 )
		return	0.0;

	if ( couponRate <= 0.0  ||  couponsPerYear < 1 )
		return	yrs;

	double	periodicYield = ytm / couponsPerYear;
	double	nPeriods = yrs * couponsPerYear;
	double	nP1 = nPeriods + 1.0;
	double	onePy = 1.0 + periodicYield;
	double	powFactor = pow( onePy, -nPeriods );

		// Maple magic here ...
	double	cpnPart = onePy - (nP1 * periodicYield + 1.0) * powFactor;
	cpnPart *= getCoupon() / (periodicYield * periodicYield);

	double	parPart = parValue * nPeriods * powFactor;
#ifdef _DEBUG
//	TRACE( _T("Bond::MacaulayDurationCore: parPart=%g, cpnPart=%g\n"),
//		parPart, cpnPart );
#endif
		// by dividing by the market price and couponsPerYear, we have
		// the annulized 'dollar duration'
	return ( cpnPart + parPart ) / ( mktPrice * couponsPerYear);
}			// MacaulayDurationCore()
//------------------------------------------------------------------------------------------------
double		CBond::MacaulayDuration( COleDateTime evalDate /* = COleDateTime::GetCurrentTime() */ )
{		// gating variables
	if ( mktPrice < 0.01 )
		return	0.0;

		// final gate - verify that the bond is still alive
	if ( evalDate > maturityDate )
		return	0.0;

	if ( evalDate < incepDate )
		evalDate = incepDate;

		// if this is a resale Bond, evalDate will be after the incepDate...
		// we want to avoid counting coupons before the bond's incepDate
	long	spDays;
	double	yrs = calcYears( evalDate, maturityDate, dayCounting, spDays );

	double	err = GetErrTol();
	double	ytm = YieldToMaturity( err, evalDate );
	if ( err > GetErrTol() )
	{
#ifdef _DEBUG
		TRACE( _T("Bond::MacaulayDuration: returning 0.0 ... ytm=%g, err=%g > GetErrTol()=%g\n"),
			ytm, err, GetErrTol() );
#endif
		return	0.0;				// couldn't converge on a yield
	}
	return	MacaulayDurationCore( yrs, ytm );
/*
		// we're replacing the scan and discount method below with the direct calculation above
	long	spDays;
	double	yrs = calcYears( fromDate, maturityDate, dayCounting, spDays ); 

		// get the duration associated with the par value & last coupon
	double	err = GetErrTol();
	double	ytm = YieldToMaturity( err );
#ifdef _DEBUG
	if ( err > GetErrTol() )
		TRACE( _T("Bond::MacaulayDuration: ytm=%g, err=%g > GetErrTol()=%g\n"),
			ytm, err, GetErrTol() );
#endif
	double	disc = exp( -ytm * yrs );
//	double	disc = Discount( later, maturityDate, ytm );
	double	cpn = getCoupon();
	double	dur = yrs * (parValue + cpn) / disc;

		// add in the duration associated with the remaining coupons
	if ( couponsPerYear > 0  &&  cpn > 0.0 )
	{	double	cpnPeriod = 1.0 / (double)couponsPerYear;

			// the continuous approach
		while ( yrs > 0.0 )
		{		// to use Discount(), we'd need to construct coupon dates
			disc = exp( -ytm * yrs );
			dur += yrs * cpn / disc;
			yrs -= cpnPeriod;
		}
*/
/*
			// the discrete approach
		int	cpnPeriods = 1;			// start at the first coupon before the maturityDate
		COleDateTime	cpnDate = getCouponDate( cpnPeriods );
		while ( cpnDate > fromDate )
		{		// this coupon's date is within range
			yrs -= cpnPeriod;					// estimate years rather than calling calcYears()
			disc = Discount( cpnDate, maturityDate, ytm );
			dur += yrs * cpn / disc;
				// loop control is the exact coupon date
			cpnDate = getCouponDate( ++cpnPeriods );
		}
	}
	return	dur / mktPrice;
*/
}			// MacaulayDuration()
//----------------------------------------------------------------------------------------------
double		CBond::Convexity( BondEvaluationContext& evalCtx )
{
	if ( evalCtx.cvxtyIsValid )
		return	evalCtx.cvxty;

	if ( ! evalCtx.yrsToMaturityIsValid )
	{	long	spDays;
		COleDateTime	startDate = evalCtx.fromDate;
		if ( startDate < incepDate )
			startDate = incepDate;
		evalCtx.yrsToMaturity = calcYears( startDate, maturityDate, dayCounting, spDays );
		evalCtx.yrsToMaturityIsValid = true;
	}
	evalCtx.cvxty = ConvexityCore( evalCtx.yrsToMaturity, YieldToMaturity(evalCtx) );
	evalCtx.cvxtyIsValid = true;
	return	evalCtx.cvxty;
}			// Convexity()
//------------------------------------------------------------------------------------------------
double		CBond::ConvexityCore( double yrs, double ytm )
{		// this is the per period Convexity
		// Maple says:  sum( t * (t+1) / (1+y)^(t+2), t=1..N) is:
		// I replaced  (1/(1+y))^(N+1) / (1+y)  with  (1+y)^(-N-2)
		//
		//    2		    (2 + y*(N+2)  *  (y*(N+1) + 2 ))  *  (1+y)^(-N-2)
		// -------  -  -----------------------------------------------------
		//   y^3							y^3
		// 
		// will still need to:
		// add in the accrued interest
		// upscale by the coupon amount
		// add in the effect of par value = N(N+1) * parValue * (1+y)^(-N-2)
		// 
	if ( couponRate <= 0.0  ||  couponsPerYear < 1 )
		return	0.0;

	double	nPeriods = yrs * couponsPerYear;
	double	periodicYield = ytm / couponsPerYear;

		// Maple solution follows ...
	double	np1 = nPeriods + 1.0;
	double	np2 = nPeriods + 2.0;

		// Maple's (1+y) in the denominator is absorbed by the pow exponent
	double	right = 2.0 + periodicYield * np2  *  (periodicYield * np1 + 2.0);
	right *= pow( 1.0 + periodicYield, -np2 );

		// flip the denominator into the numerator by complementing the exponent
	double	cpnPart = getCoupon() * ( 2.0  -  right ) * pow( periodicYield, -3.0 );
	double	parPart = parValue * nPeriods * np1 * pow( 1.0 + periodicYield, -np2 );

#ifdef _DEBUG
//	TRACE( _T("Bond::ConvexityCore: parPart=%g, cpnPart=%g\n"), parPart, cpnPart );
#endif

		// return the annualized 'dollar convexity'
	return	( parPart + cpnPart ) / ( mktPrice * couponsPerYear * couponsPerYear );
}			// ConvexityCore()
//------------------------------------------------------------------------------------------------
// convexity - the second derivative of the bond valuation equation w.r.t. the yield
// ref:  p.75 of Frank Fabozzi's Bond Markets, Analysis, and Sstrategies, 5th Edition 
// ref:  Timothy R. Mays Bond Valuation (Fin 4600-Ch 12) - slide 39, 40 is wrong
// ref:  James Farrell's Portfolio Management is also wrong
double		CBond::Convexity( COleDateTime evalDate /* = COleDateTime::GetCurrentTime() */ )
{		// gating variables
	if ( mktPrice < 0.01 )
		return	0.0;

		// verify that the bond is still alive
	if ( evalDate > maturityDate )
		return	0.0;

		// if this is a resale Bond, evalDate will be after the incepDate...
		// we want to avoid counting coupons before the bond's incepDate
	COleDateTime	startDate = evalDate;
	if ( startDate < incepDate )
		startDate = incepDate;

	long	spDays;
	double	yrs = calcYears( startDate, maturityDate, dayCounting, spDays );

		// get the convexity associated with the par value & last coupon
	double	err = CBond::GetErrTol();
	double	ytm = YieldToMaturity( err );			// what if err > CBond::GetErrTol()
	return	ConvexityCore( yrs, ytm );

//	double	disc = exp( -ytm * yrs );									// use  x^y = exp(-yearlyRate*yrs)
/*
		// laterMinus2Periods accounts for the +2 in the exponent of Fabozzi's Convexity formula
		// calling SkipCpnPeriods with a negative shift causes it to expand the date range, from
		// our adjusted value for evalDate (our 'later') to the coupon or maturity date
	COleDateTime	laterMinus2Periods = SkipCpnPeriods( later, -2 );
	double	disc = Discount( laterMinus2Periods, maturityDate, ytm );	// use  x^y = pow(1-dailyRate, days)
	double	t2pt = yrs * yrs + yrs;
	double	cpn = getCoupon();
	double	cvx = (parValue + cpn) * t2pt / disc;

		// add in the duration associated with the remaining coupons
	if ( couponsPerYear > 0  &&  cpn > 0.0 )
	{	double	cpnPeriod = 1.0 / (double)couponsPerYear;

			// the continuous approach
		yrs -= cpnPeriod;
		while ( yrs > 0.0 )
		{	t2pt = yrs * yrs + yrs;
			disc = exp( -ytm * yrs );
			cvx += cpn * t2pt / disc;
			yrs -= cpnPeriod;
		}
*/
/*
			// the discrete approach
		int		cpnPeriods = 1;
		COleDateTime	cpnDate = getCouponDate( cpnPeriods );
		while ( cpnDate > later )
		{	yrs -= cpnPeriod;
			t2pt = yrs * yrs + yrs;
			disc = Discount( laterMinus2Periods, cpnDate, ytm );
			cvx += cpn * t2pt / disc;
			cpnDate = getCouponDate( ++cpnPeriods );
		}
	}
		// rescale the series
//	double	cpy = ( couponRate == 0.0  ||  couponsPerYear < 1 )  ?  1  :  couponsPerYear;
//	double	resc = 1.0 + ytm / cpy;
//	return	cvx / (resc * resc * mktPrice);
	return	cvx / mktPrice;
*/
}			// Convexity()
//----------------------------------------------------------------------------------------------
double	CBond::DeltaPdivP( BondEvaluationContext& evalCtx, double deltaYTM )
{		// return the fractional change in market price at given deltaYTM
	double	modDur = ModifiedDuration( evalCtx );			// modDur is not stored in evalCtx
	Convexity( evalCtx );
	double	deltaPdivP_ModDur = - modDur * deltaYTM;
	double	deltaPdivP_Cvxty = 0.5 * evalCtx.cvxty * deltaYTM * deltaYTM;
	double	deltaPdivP = deltaPdivP_ModDur + deltaPdivP_Cvxty;
#ifdef _DEBUG
//	TRACE( _T("Bond::DeltaPdivP(%s): deltaPdivP_ModDur=%g, deltaPdivP_Cvxty=%g\n"),
//		getSymbol(), deltaPdivP, deltaPdivP_ModDur, deltaPdivP_Cvxty );
#endif
	return	deltaPdivP;				// fractional change in market price
}			// DeltaPdivP()
//----------------------------------------------------------------------------------------------
double	CBond::ModifiedDuration( BondEvaluationContext& evalCtx )
{
	MacaulayDuration( evalCtx );

	double	cpy = (couponRate == 0.0  ||  couponsPerYear < 1)  ?  1.0  :  couponsPerYear;
	return	evalCtx.macDur / ( 1.0 + YieldToMaturity(evalCtx) / cpy );
}			// ModifiedDuration()
//----------------------------------------------------------------------------------------------
double	CBond::ModifiedDuration( double& macDur, COleDateTime evalDate /* = COleDateTime::GetCurrentTime() */ )
{		// return MacaulayDuration as a side effect
	double	err = GetErrTol();
	double	ytm = YieldToMaturity( err, evalDate );	// what if err > GetErrTol() ???
	macDur = MacaulayDuration( evalDate );
	double	cpy = (couponRate == 0.0  ||  couponsPerYear < 1)  ?  1.0  :  couponsPerYear;
	return	macDur / ( 1.0 + ytm / cpy );
}			// ModifiedDuration()
//----------------------------------------------------------------------------------------------
double	CBond::DeltaPdivP( double deltaYTM )
{		// return the fractional change in market price at given deltaYTM
	double	macDur, modDur = ModifiedDuration( macDur );
	double	cvxty = Convexity();
	double	deltaPdivP_ModDur = - modDur * deltaYTM;
	double	deltaPdivP_Cvxty = 0.5 * cvxty * deltaYTM * deltaYTM;
	double	deltaPdivP = deltaPdivP_ModDur + deltaPdivP_Cvxty;
#ifdef _DEBUG
//	TRACE( _T("Bond::DeltaPdivP(%s): deltaPdivP_ModDur=%g, deltaPdivP_Cvxty=%g\n"),
//		getSymbol(), deltaPdivP, deltaPdivP_ModDur, deltaPdivP_Cvxty );
#endif
	return	deltaPdivP;				// fractional change in market price
}			// DeltaPdivP()
//----------------------------------------------------------------------------------------------
double		CBond::DailyRate( double annualRate, long yr )
{	short	days;
	if ( dayCounting == ActualActual )
		days = isLeapYr( yr )  ?  366  :  365;
	else										// Actual360 or Thirty360
		days = 360;
#ifdef _DEBUG
//	TRACE( _T("Bond::DailyRate: annualRate=%f, yr=%d, days=%d, dailyRate=%f\n"),
//		annualRate, yr, days, annualRate / (double)days );
#endif
	return	annualRate / (double)days;
}			// DailyRate()
//------------------------------------------------------------------------------------------------
int		CBond::GetCouponIntervals( COleDateTime evalDate )
{		// return the first coupon date before aDate
	if ( couponsPerYear < 1 )
		return	-1;						// indicates no possible coupon date

	if ( evalDate > maturityDate )	evalDate = maturityDate;
	if ( evalDate < incepDate )		evalDate = incepDate;
#ifdef _DEBUG
//	TRACE( _T("Bond::GetCouponIntervals: to %s\n"), evalDate.Format(VAR_DATEVALUEONLY) );
#endif

		// get the number of coupon intervals from Maturity to aDate
	long	spDays;
	double fYrs = calcYears( evalDate, maturityDate, dayCounting, spDays );
		// NOTE:  zero intervals means you get only the final coupon on the maturityDate
	int	intervals = (int)( fYrs * couponsPerYear );

#ifdef _DEBUG
//	TRACE( _T("Bond::GetCouponIntervals: intervals = %d\n"), intervals );
#endif
	return	intervals;
}			// GetCouponIntervals()
//----------------------------------------------------------------------------------------------
COleDateTime	CBond::getCouponDate( int couponIntervals )
{		// compute a coupon date couponIntervals prior to maturityDate
	if ( couponsPerYear < 1  ||  couponIntervals < 1 )
		return	maturityDate;
	
	int	couponIntervalMonths = 12 / couponsPerYear;
	int	matYr = maturityDate.GetYear();
	int	matMo = maturityDate.GetMonth();
	int	intervalYrs = couponIntervals / couponsPerYear;
	int	intervalMonths = couponIntervals * couponIntervalMonths - 12 * intervalYrs;
	int	cpnYr = matYr - intervalYrs;
	int	cpnMo = matMo - intervalMonths;
	int	cpnDa = maturityDate.GetDay();
	if ( cpnMo < 1 )
	{	cpnMo += 12;
		cpnYr--;
	}
		// if cpnDa > number of days in cpnMo, cut back to the max for that month
	if ( cpnDa > 28 )
	{	switch ( cpnMo )
		{		// Jan 31     Feb 28/29  Mar 31
				// Apr 30     May 30     Jun 30
				// Jul 31     Aug 30     Sep 30
				// Oct 31     Nov 30     Dec 31
			case 2:
				if ( cpnDa > 29 )
					cpnDa = 29;
				if ( cpnDa > 28  &&  ! isLeapYr(cpnYr) )
					cpnDa = 28;
				break;
			case  4:
			case  5:
			case  6:
			case  8:
			case  9:
			case 11:
				if ( cpnDa > 30 )
					cpnDa = 30;
				break;
			default:
				break;
		}
	}
	COleDateTime cpnDate( cpnYr, cpnMo, cpnDa, 0, 0, 0 );
	return	cpnDate;
}			// getCouponDate()
//----------------------------------------------------------------------------------------------
COleDateTime	CBond::SkipCpnPeriods( COleDateTime theDate, short nPeriods )
{		// calculate the time shift for nPeriods worth of coupons
	short	monthsPerCoupon = couponsPerYear / 12;		// the only bond specific thing here
	short	monthShift = monthsPerCoupon * nPeriods;
	short	yearShift = monthShift / 12;				// integer division
	monthShift -= yearShift * 12;						// what's left after the yearShift portion

		// return a date which is nPeriods after theDate
	int	yr = theDate.GetYear() + yearShift;
	int	mo = theDate.GetMonth() + monthShift;

		// ensure that mo & yr are in legal range
	while ( mo > 12 )
	{	mo -= 12;
		yr++;
	}
	while ( mo < 1 )
	{	mo += 12;
		yr--;
	}

		// fix the day of the month
	int	da = theDate.GetDay();
	if ( da > 28  &&  mo == 2 )
		da = (yr % 4 == 0) ? 29 : 28;
	else if ( da > 30  &&  (mo == 4  ||  mo ==  6  ||  mo ==  9  ||  mo == 11 ) )
		da = 30;		// April, June, September, November

	COleDateTime	ret;									// the return value
	ret.SetDate( yr, mo, da );
	return	ret;
}			// SkipCpnPeriods()
//--------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////
//										GLOBAL functions										//
//////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------
// NOTE:  this is a global function!
double	calcYears( COleDateTime fromDate, COleDateTime toDate, DayCountMethod method, long& spDays )
{
#ifdef _DEBUG
	if ( fromDate.GetStatus() != COleDateTime::valid )
	{	TRACE( _T("calcYears: fromDate is invalid or null\n") );
		AfxDebugBreak();
	}
	if ( toDate.GetStatus() != COleDateTime::valid )
	{	TRACE( _T("calcYears: toDate is invalid or null\n") );
		AfxDebugBreak();
	}
#endif
	if ( fromDate == toDate )
		return	0.0;

	double	yrs = 0.0;												// the return value
	bool	reverse = ( fromDate > toDate ) != 0;
	if ( reverse )
	{		// ensure that fromDate is really before the toDate
		COleDateTime	tmp(toDate);
		toDate = fromDate;
		fromDate = tmp;
	}
		// whole years
	int	fromYr = fromDate.GetYear();
	int	toYr   = toDate.GetYear();
	yrs = toYr - fromYr - 1;
#ifdef _DEBUG
//	TRACE( _T("calcYears: from %s to %s, wholeYrs=%d\n"),
//		fromDate.Format(VAR_DATEVALUEONLY), toDate.Format(VAR_DATEVALUEONLY), wholeYrs );
#endif
	if ( yrs < 0 )
		yrs = 0;

	int	wholeYrs = (int)yrs;
	switch ( method )
	{	case Thirty360:												// XXX there is a serious bug in Thirty360
		{	int	toDay = toDate.GetDay();
			int	fromDay = fromDate.GetDay();

			int	fromMon = fromDate.GetMonth();
			int	toMon = toDate.GetMonth();

				// treat initial year and final year seperately
			if ( fromYr == toYr )
			{	int	months = toMon - fromMon - 1;					// full months only
				if ( months < 0 )
					months = 0;

				spDays = 30 * months +  toDay - fromDay;
				if ( fromMon != toMon )
					spDays += 30;
			}
			else													// different years
			{		// initial (from) year
				spDays = 30 * (12 - fromMon)  +  30 - fromDay;

					// final (to) year
				spDays += 30 * (toMon - 1)   +  toDay;
			}
			yrs += (double)spDays / 360.0;
			spDays += wholeYrs * 360;
			break;
		}
		case Actual360:
		case ActualActual:
		default:
		{	int	fromDay = fromDate.GetDayOfYear();					// XXX does GetDayOfYear() account for leap years ???
			int	toDay = toDate.GetDayOfYear();			
			int daysInFromYr = isLeapYr(fromYr)  ?  366  :  365;

			if ( fromYr == toYr )
			{	spDays = toDay - fromDay;
				yrs += (double)spDays / (double)daysInFromYr;
			}
			else
			{	int	fromYrSpDays = daysInFromYr - fromDay;
				yrs += (double)fromYrSpDays / (double)daysInFromYr;
				yrs += (double)toDay / (double)(isLeapYr(toYr)  ?  366  :  365);
				spDays = fromYrSpDays + toDay;
			}
			spDays += wholeYrs * 365 + wholeYrs / 4;				// an approximation
			break;
		}
	}
	if ( reverse )
		yrs = -yrs;
#ifdef _DEBUG
//	TRACE( _T("calcYears: from %s to %s, yrs=%.3f, spDays=%d\n"),
//		fromDate.Format(VAR_DATEVALUEONLY), toDate.Format(VAR_DATEVALUEONLY), yrs, spDays );
#endif
	return	yrs;
}			// calcYears()
//------------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////
//											outtakes											//
//////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------
/*
double	CBond::PresentValueCouponsToMaturity( double ytm, double yrsToMaturity )
{		// Assumes that yrsToMaturity runs to the maturityDate
		//
	if ( couponRate <= 0.0  ||  couponsPerYear < 1 )
		return	0.0;

	double	periodicYield = ytm / couponsPerYear;
	double	nCpns = yrsToMaturity * couponsPerYear;
	return	PresentValueCouponsCore( periodicYield, nCpns );
}			// PresentValueCouponsToMaturity()
*/
//--------------------------------------------------------------------------------------
/*
double	calcYears( COleDateTime fromDate, COleDateTime toDate, DayCountMethod dayCounting )
{	if ( fromDate > toDate )
	{		// ensure that fromDate is really before the toDate
		COleDateTime	tmp(toDate);
		toDate = fromDate;
		fromDate = tmp;
	}
	int		toYr  = toDate.GetYear();
		// lastLeap behavior:  {1997, 1998, 1999, 2000} -> 1996
	//int		lastLeap = toYr - toYr%4;

	int		fromYr  = fromDate.GetYear();
		// firstLeap behavior:  {1997, 1998, 1999, 2000} -> 2000
	//int		firstLeap = fromYr + ((fromYr%4 > 0) ? 4 - fromYr%4 : 0);

	bool	fromLeap = FALSE;
	double	spDays, spYrs = 0.0;										// return value
	int		toDay, fromDay;
	COleDateTimeSpan  timeSpan;
	//int		numLeaps = (lastLeap >= firstLeap) ? 1 + (lastLeap - firstLeap) / 4 : 0;
	switch ( dayCounting )
	{	case Thirty360:
				// corporate bonds, U.S. Agency bonds, and all mortgage backed securities
			fromDay = fromDate.GetDay();
			toDay   = toDate.GetDay();
			if ( fromDay == 31 )
				fromDay = 30;										// fromDay :== D1
			if ( toDay   == 31  &&  fromDay > 29 )
				toDay = 30;		// toDay   :== D2
			fromLeap = ( fromYr % 4 == 0 );							// fromDate is in a leap year
			if ( fromDate.GetMonth() == 2 
				&&  ( ( fromDay == 28  &&  !fromLeap )
				  ||  ( fromDay == 29  &&   fromLeap ) ) )
				fromDay = 30;
			spDays = 360 * (toDate.GetYear()  - fromDate.GetYear())		// years
				   +  30 * (toDate.GetMonth() - fromDate.GetMonth())	// months
				   +	   toDay - fromDay;								// days
			spYrs = spDays / 360.0;
			break;
		case Actual360:
				// bank deposits and in calculating rates pegged to some indices, such as LIBOR
		case ActualActual:
				// Treasury bonds and notes
		default:								// UnknownDayCountMethod treated as ActualActual
			toDay = toDate.GetDayOfYear();
			fromDay = fromDate.GetDayOfYear();
			if ( fromYr == toYr )				// workaround for bug in timeSpan.operator-()
			{	spDays = toDay - fromDay;		
			}
			else
			{	timeSpan = toDate - fromDate;	// produces 0 when difference is 1 day
				spDays = (int)floor( timeSpan.GetTotalDays() );
			}
			spYrs = spDays / 365.0;				// GetTotalDays() doesn't count Leap days!
			break;
	}
	return spYrs;
}			// calcYears()
*/
//------------------------------------------------------------------------------------------------
/*
double		CBond::compoundRate( double simpleRate )
{	int	cpy = ( couponRate > 0.0 ) ? couponsPerYear : 0;
	return ( cpy < 2 ) ? simpleRate
					   : pow(1.0 + simpleRate/(double)cpy, (double)cpy) - 1.0;
}			// compoundRate()
*/
//----------------------------------------------------------------------------------------------
/*
int		CBond::couponsRemaining( COleDateTime aDate, bool ignoreCouponValue )
{		// ignoreCouponValue - default is false
	if ( !ignoreCouponValue  &&  (getCoupon() == 0.0)  ||  (aDate > maturityDate) ) return 0;
	if ( aDate == maturityDate )
		return 1;
//	int		spDays;
	double	spYrs = calcYears( aDate, maturityDate, dayCounting );
		// slice off the fractional part of a coupon period, but
		// account for one coupon occuring with zero years to maturity
	return (int)floor( spYrs * (double)couponsPerYear ) + 1;
}			// couponsRemaining()
*/
//----------------------------------------------------------------------------------------------
/*		Doesn't check out (probably due to calling couponsRemaining) - not currently used...
COleDateTime	CBond::getNextCpnDate( COleDateTime fromDate )
{	COleDateTime	result = maturityDate + COleDateTimeSpan(1);
	if ( fromDate >= maturityDate )
		return result;
		// find the date of a coupon which is cpnPeriods before the maturityDate
		// start the day after fromDate
	int	cpns = couponsRemaining( fromDate + COleDateTimeSpan(1) );
	if ( cpns == 1 )  return maturityDate;
	return getPriorCpnDate( cpns-1 );
}			// getNextCpnDate()
*/
//----------------------------------------------------------------------------------------------
/*		Think this checked out - not currently used...
COleDateTime	CBond::getPriorCpnDate( int cpnPeriods )
{		// return the date of a coupon which is cpnPeriods before the maturityDate
	COleDateTime	result = COleDateTime::GetCurrentTime() - COleDateTimeSpan(1);
	if ( getCoupon() == 0.0 )
		return result;

	if ( cpnPeriods == 0 )
		return maturityDate;

	int		yr = maturityDate.GetYear();
	int		mo = maturityDate.GetMonth();
	int		da = maturityDate.GetDay();				// of month
	switch ( couponsPerYear )
	{	case 4:			// find N quarters before maturity
			yr -=      cpnPeriods / 4;
			mo -= 3 * (cpnPeriods % 4);
			break;
		case 2:			// find N/2 years before maturity
			yr -=      cpnPeriods / 2;
			mo -= 6 * (cpnPeriods % 2);
			break;
		case 1:
				// only need to modify 'year'
			yr -=      cpnPeriods;
			break;
		default:
			break;
	}
		// in the event that we landed on an fictitious day of the month,
		// fix up the date components
	if ( mo < 1 )
	{	yr -= 1;
		mo += 12;
	}
	if ( mo == 2 )
	{		// Feb-28,29
		if ( isLeapYr(yr) )					// its a leap year
		{	if ( da > 29 )
			da = 29;
		}
		else if ( da > 28 )
			da = 28;						// its not a leap year
	}
	else		// Apr-30, June-30, Sep-30, Nov-30
	{	if ( da > 30  &&  (mo == 4  ||  mo == 6  ||  mo == 9  ||  mo == 11) )
			da = 30;
	}
		// there are no changes for Jan-31, Mar-31, May-31, July-31, Aug-31, Oct-31, Dec-31
	result.SetDate( yr, mo, da );
	return	result;
}			// getPriorCpnDate()
*/
//----------------------------------------------------------------------------------------------
/*		Don't remember if this was checked out - not currently used...
int		CBond::countDays( COleDateTime fromDate, COleDateTime toDate )
{	long	spDays;

	int		fromDay = fromDate.GetDay();
	int		fromYr = fromDate.GetYear();

	int		toDay = toDate.GetDay();
	int		toYr = toDate.GetYear();

	switch ( dayCounting )
	{	case Thirty360:
		{		// corporate bonds, U.S. Agency bonds, and all mortgage backed securities
			if ( fromDay == 31 )
				fromDay = 30;									// fromDay :== D1
			if ( toDay   == 31  &&  fromDay > 29 )
				toDay = 30;	// toDay   :== D2
			int		fromLeap = ( fromYr % 4 == 0 );				// fromDate is in a leap year
			if ( fromDate.GetMonth() == 2 
				&&  ( ( fromDay == 28  &&  !fromLeap )
				  ||  ( fromDay == 29  &&   fromLeap ) ) )
				fromDay = 30;
			spDays = 360 * (toDate.GetYear()  - fromDate.GetYear())		// years
				   +  30 * (toDate.GetMonth() - fromDate.GetMonth())	// months
				   +	    toDay - fromDay;							// days
			break;
		}
		case Actual360:
				// bank deposits and rates pegged to indices, such as LIBOR
		case ActualActual:
				// Treasury bonds and notes
		default:							// UnknownDayCountMethod treated as ActualActual
		{	if ( fromYr == toYr )			// workaround for bug in timeSpan.operator-()
			{	spDays = toDay - fromDay;		
			}
			else	// COleDateTimeSpans don't account for leap years!
			{	COleDateTimeSpan timeSpan = toDate - fromDate;		// produces 0 when difference is 1 day
				spDays = (int)floor( timeSpan.GetTotalDays() );
			}
				// add leap days
			int leapDays = numLeapDays( fromDate, toDate );
			spDays += ( spDays >= 0 ) ? leapDays : -leapDays;
			break;
		}
	}
	return	spDays;
}			// countDays()
*/
//----------------------------------------------------------------------------------------------
/*
void	CBond::setDayCounting( CArchive& ar )
{	short  tmp;
	ar >> tmp;
	setDayCounting( tmp );
}			// setDayCounting( CArchive& ar )
*/
//----------------------------------------------------------------------------------------------
/*
void	CBond::Serialize( CArchive& ar )
{	if ( ar.IsStoring() )
	{	ar << couponsPerYear;
		ar << couponRate;
		ar << getDayCounting();
		ar << desc;
		ar << parValue;
		ar << maturityDate;
		ar << mktPrice;
		ar << symbol;
	}
	else	// loading
	{	ar >> couponsPerYear;
		ar >> couponRate;
		setDayCounting( ar );
		ar >> desc;
		ar >> parValue;
		ar >> mktPrice;
		ar >> maturityDate;
		ar >> symbol;
	}
	positions->Serialize( ar );
}			// Serialize()
*/
//----------------------------------------------------------------------------------------------
/*
double	CBond::Discount( COleDateTime fromDate, COleDateTime toDate,
								 double discRate = 0.0, bool reverse = false )
{		// reverse = true --> 1.0 + the discount rate in the pow() calculation
		// reverse = false --> 1.0 - the discount rate in the pow() calculation
	if ( fromDate == toDate )
		return	1.0;								// unity discount with zero time between dates

		// discount a cash flow on the toDate back to today (the fromDate), using the given dayCounting method 
		// first ensure that the dates are in temporal order
	if ( fromDate > toDate )
	{		// ensure that the fromDate is really before the toDate
		COleDateTime	tmp(toDate);
		toDate = fromDate;
		fromDate = tmp;
	}

		// whole years
	int	fromYr = fromDate.GetYear();
	int	toYr   = toDate.GetYear();
	int	wholeYrs = toYr - fromYr - 1;					// must be separated by 2 to get a whole year in between
	if ( wholeYrs < 0 )
		wholeYrs = 0;									// whole years only
	double	wholeYrBase = 1.0 + (reverse  ?  discRate  :  -discRate );
	double	dI, dF, dW  = pow( wholeYrBase, wholeYrs );

	double	initialRate = DailyRate( discRate, fromYr );					// daily version of the discRate
	double	finalBase, initialBase = 1.0 + (reverse ? initialRate :  -initialRate);

	int	months;
	switch ( dayCounting )
	{	case Thirty360:
		{	int	toDay = toDate.GetDay();
			int	fromDay = fromDate.GetDay();

			int	fromMon = fromDate.GetMonth();
			int	toMon = toDate.GetMonth();

				// treat initial year and final year seperately
			if ( fromYr == toYr )
			{	months = toMon - fromMon - 1;			// full months only
				if ( months < 0 )
					months = 0;
				int days = toDay - fromDay;
				if ( fromMon != toMon )
					days += 30;
				days += 30 * months;
				dI = pow( initialBase, (double)days );
				dF = 1.0;								// fromYr handles it all
			}
			else										// different years
			{		// initial (from) year
				int initialDays = 30 * (12 - fromMon)  +  30 - fromDay;
				dI = pow( initialBase, (double)initialDays );

					// final (to) year
				int finalDays = 30 * (toMon - 1)   +  toDay;
					// get a daily version of discRate for the final year
				double	finalRate = DailyRate( discRate, toYr );
				finalBase = 1.0 + (reverse  ?  finalRate  :  -finalRate );
				dF = pow( initialBase, (double)finalDays );
			}
			break;
		}
		case Actual360:
		case ActualActual:
		default:
		{	int	fromDay = fromDate.GetDayOfYear();		// XXX does GetDayOfYear() account for leap years ???
			int	toDay = toDate.GetDayOfYear();			

			if ( fromYr == toYr )
			{	int days = toDay - fromDay;
				dI = pow( initialBase, (double)days );
				dF = 1.0;								// fromYr handles it all
			}
			else
			{	bool	fromIsLeap = isLeapYr( fromYr );
				int daysInFromYr = fromIsLeap ? 366 : 365;
				int initialDays = daysInFromYr - fromDay;
				dI = pow( initialBase, (double)initialDays );

				int finalDays = toDate.GetDayOfYear();
					// get a daily version of discRate for the final year
				double	finalRate = DailyRate( discRate, toYr );
				double	finalBase = 1.0 + (reverse  ?  finalRate  :  -finalRate );
				dF = pow( finalBase, (double)finalDays );
			}
			break;
		}
	}
		// multiply the cash flow by the discount to get the cash flow's present value
	return	dI * dW * dF;
}			// Discount()
*/
//------------------------------------------------------------------------------------------------
