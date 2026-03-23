#include "StdAfx.h"
#include "Option.h"
//----------------------------------------------------------------------------------------------------
double		COption::findSstar(
	CStock*			theStock,
	double			midStkPrc,									// midrange of stock prices to be considered
	double			lastDiv,									// make the caller supply this
	double			riskFreeRate,
	double			yrsToExpiry,
	double&			err						)
{	const short		loopLimit = 20;								// essentially 80 euroVal calls
		// 4 is optimal for calls, but 12 is sometimes necessary for puts
		// start with 12, follow through with 4 on subsequent steps
	const short		testPts = 4;
	double			prcError, prevErr = 0.0;					// set up for first meaningful impError
	double			incStkPrc = (2.0 * midStkPrc) / 13.0;		// i.e. testPts = 12 in the initial pass
	double			maxStkPrc = 2.0 * midStkPrc - incStkPrc;
	double			stkPrc = incStkPrc;
	double			optVal, sStar = 0.0;
	double			impError = 2.0 * midStkPrc;
	double			minErr = impError;							// init w/ big numbers

		// even this small impError is likely to drop below the threshold before
		// exceeding the loopLimit or getting anywhere near pricing error < 0.01
	unsigned short	loops = 0;
	while ( fabs(impError) >= 1e-6  &&  loops < loopLimit  &&  fabs(minErr) >= 0.001 )
	{	for ( ; stkPrc <= maxStkPrc; stkPrc += incStkPrc )
		{		// findSstar is called by RollGeskeWhaley with a stock price discounted
				// by the amount of the last dividend, so we DON'T want to pass a dividend yield
			optVal = euroValue( stkPrc, theStock->volatility, yrsToExpiry, riskFreeRate );
			prcError = optVal - stkPrc - lastDiv + strikePrice;
			if ( fabs(prcError) < fabs(minErr) )
			{	err = prcError;
				sStar = stkPrc;
			}
		}
			// set up to evaluate a more focused set of test points
			// we're assuming sStar is a smooth function, but
			// we're NOT assuming that it has a slope which would lead to a steepest descent approach
			// in fact, it is very likely that large portions of the price range will have slope near zero.
		impError = fabs(err) - fabs(prevErr);
		prevErr = err;
		double	nISP = (2.0 * incStkPrc) / (testPts + 1);		// new incStkPrc value
		stkPrc = sStar - incStkPrc + nISP;		// new starting point for scanning across the price range
		maxStkPrc = sStar + incStkPrc - nISP;	// assumes that testPts spans the price range of interest
		incStkPrc = nISP;
		loops++;
	}					// return the stock price at which the option should
	return	sStar;		// be exercised just after the stock goes ex-dividend
}			// COption::findSstar()
//----------------------------------------------------------------------------------------------------
double		COption::RollGeskeWhaleyAmerCall(
	CStock*		theStock,
	double		stockPrice,
	double		sigma,
	double		riskFreeRate,
	double		yrsToExpiry				 	)
{	ASSERT( putCall == COption::Call );
	if ( yrsToExpiry <= 0.0 )
		return 0.0;

		// get the amount (and date) of any dividend occuring before expiry
	COleDateTime	exDivDate;
	double		lastDiv = (double)theStock->lastDivB4Date( expiry, exDivDate ) / 100.0;		// pennies -> dollars

		// we have an exDivDate before expiry, but is it after today?
	COleDateTime	today = COleDateTime::GetCurrentTime();
	double	D1ert1 = 0.0;													// PV( last dividend )
	double	yrsToLastDiv = 709.0;											// essentially infinity
//	if ( today <= exDivDate )
//	{//	int		spDays;
//		yrsToLastDiv = calcYears( today, exDivDate, dayCounting );
//		if ( yrsToLastDiv <= 0.0 )
//		{		// prep lastDiv and its accrual date to ensure valid bivariate normal probabilities
//			yrsToLastDiv = 1e-6;		// small and positive
//			lastDiv = 0.0;
//		}
//		D1ert1 = lastDiv * exp( -riskFreeRate * yrsToLastDiv );				// PV( last dividend )
//	}

		// John Hull (p.265 of __Options, Futures, & Other Derivatives__
		// says to treat S0 as the stockPrice less the Present Value (PV) of all but the last Dividend
	double	pvOtherDivs = theStock->PresentValueDivs( today, dayBefore(exDivDate), riskFreeRate );
	double	s0_D1ert1 = stockPrice - pvOtherDivs - D1ert1;					// all remaining dividends? XXX

		// Is early exercise warranted?
		//	double	erTmt1 = exp( -riskFreeRate * (yrsToExpiry - yrsToLastDiv) );
		//	if ( lastDiv <= strikePrice *(1.0 - erTmt1) )
		//	{		// early exercise is not optimal !!!
		//			// Black-Scholes (with stock price discounted for last dividend) will do it...
		//		return	euroValue( s0_D1ert1, sigma, yrsToExpiry, riskFreeRate );
		//	}

		// find an S* that satisfies:   c(S*) = S* + D1 - K,
		// where K is the strikePrice and D1 is the last dividend
	double	err, sStar = findSstar( theStock, stockPrice, lastDiv, riskFreeRate, yrsToExpiry, err );
	double	rfr_p_SigSq_o_2 = riskFreeRate + 0.5 * sigma * sigma;

		// find M1 & M2 (bivariate normal probabilities)
	double	sigSqRtT = sigma * sqrt( yrsToExpiry );
	double	aa1 = log( s0_D1ert1 / strikePrice ) + rfr_p_SigSq_o_2 * yrsToExpiry;
			aa1 /= sigSqRtT;
	double	aa2 = aa1 - sigSqRtT;

	double	sigSqRt_t1 = sigma * sqrt( yrsToLastDiv );
	double	bb1 = log( s0_D1ert1 / sStar ) + rfr_p_SigSq_o_2 * yrsToLastDiv;
			bb1 /=  sigSqRt_t1;
	double	bb2 = bb1 - sigSqRt_t1;

	double	U1 = normCDF(bb1);		// univariate normal integral
	double	U2 = normCDF(bb2);

	double	rho = -sqrt( yrsToLastDiv / yrsToExpiry );
	double	B1 = bivarNormIntegral( aa1, -bb1, rho );
	double	B2 = bivarNormIntegral( aa2, -bb2, rho );

	double	KerT = strikePrice * exp( -riskFreeRate * yrsToExpiry );
		// according to Sterk's paper, the Roll-Geske-Whaley value follows, but when there's
		// no dividend between today and expiry, erTmt1 will overwhelm the other terms
		// rgw1 = s0_D1ert1 *( U1 + B1 )  -  KerT *( U2 * erTmt1 + B2 )  -  D1ert1 * U2;	// alpha = -1.0

	double	K_Dert1 = (strikePrice - lastDiv) * exp( -riskFreeRate * yrsToLastDiv );
		// according to Financial Numerical Recipes, this should be:
		//			rgw2 = s0_D1ert1 *( U1 + B1 )  +  KerT * B2  -  K_Dert1 * U2;
	double	rgwac = s0_D1ert1 *( U1 + B1 )  -  KerT * B2  -  K_Dert1 * U2;
	return	rgwac;
}			// RollGeskeWhaleyAmerCall()
//----------------------------------------------------------------------------------------------------
double		COption::GeskeJohnsonAmerPut(
	CStock* theStock,
	double	stockPrice,
	double	sigma,
	double	riskFreeRate,
	double	yrsToExpiry					)
{	ASSERT( putCall == COption::Put );

		// get the amount (and date) of any dividend occuring before expiry
	COleDateTime	today = COleDateTime::GetCurrentTime();
	COleDateTime	exDivDate;
	double	lastDiv = (double)theStock->lastDivB4Date( expiry, exDivDate ) / 100.0;		// pennies --> dollars
		// should we use pvOtherDivs instead of just lastDiv ???
//	double	pvOtherDivs = theStock->PresentValueDivs( today, theStock->dayBefore(exDivDate), riskFreeRate );

		// calculate P1 (the Euro value at expiry)
	double	P1 = euroValue( stockPrice, sigma, yrsToExpiry, riskFreeRate, 0.0 );	// with divYield = 0.0

	const double	To2 = yrsToExpiry / 2.0;
	const double	To3 = yrsToExpiry / 3.0;

		// critical stock prices (S-bar @ T/3,  S-bar @ T/2,  S-bar @ 2*T/3)
		// NOTE:  lastDiv might be different for each of these time points ??? 						XXX
		//		  maybe it should include present value of all dividends prior to expiry ???
		// I/F:  Sstar = findSstar( theStock, midStkPrc, lastDiv, riskFreeRate, yrsToExpiry, &err )
	double	errTo3,  S_To3  = strikePrice - findSstar( theStock, stockPrice, lastDiv, riskFreeRate,     To3,  errTo3 );
	double	errTo2,  S_To2  = strikePrice - findSstar( theStock, stockPrice, lastDiv, riskFreeRate,     To2,  errTo2 );
	double	err2To3, S_2To3 = strikePrice - findSstar( theStock, stockPrice, lastDiv, riskFreeRate, 2.0*To3, err2To3 );

		// I/F:             d2 = BSd2d1( &d1,      stockPrice,  yrsToExpiry, riskFreeRate, divYield )
	double	d1S_To3,  d2S_To3  = BSd2d1( d1S_To3,  S_To3,               To3, riskFreeRate, 0.0 );	// with eqYield = 0.0 ?
	double	d1S_To2,  d2S_To2  = BSd2d1( d1S_To2,  S_To2,               To2, riskFreeRate, 0.0 );	// with eqYield = 0.0 ?
	double	d1S_2To3, d2S_2To3 = BSd2d1( d1S_2To3, S_2To3,          2.0*To3, riskFreeRate, 0.0 );	// with eqYield = 0.0 ?
	double	d1K_T,    d2K_T    = BSd2d1( d1K_T,    strikePrice, yrsToExpiry, riskFreeRate, 0.0 );	// with eqYield = 0.0 ?

		// discount the strike price to key time points
	double	KerTo3  = strikePrice * exp( -riskFreeRate *         To3 );
	double	KerTo2  = strikePrice * exp( -riskFreeRate *         To2 );
	double	Ker2To3 = strikePrice * exp( -riskFreeRate *     2.0*To3 );
	double	KerT    = strikePrice * exp( -riskFreeRate * yrsToExpiry );

		// handy constants for P2 and P3 calculations
	const double	m1oSqrt2 = -1.0 / sqrt2;
	const double	m1oSqrt3 = -1.0 / sqrt3;
	const double	m1oSqrt_2thirds = -1.0 / sqrt( 2.0 / 3.0 );

		// calculate P2
	double	bvn1 = bivarNormIntegral( d2S_To2, -d2K_T, m1oSqrt2 );
	double	bvn2 = bivarNormIntegral( d1S_To2, -d1K_T, m1oSqrt2 );
	double	  P2 =     KerTo2 * normCDF( -d2S_To2 )
		         - stockPrice * normCDF( -d1S_To2 )
			     +	     KerT * bvn1
			     - stockPrice * bvn2;

		// calculate P3
	double	bvn3 =  bivarNormIntegral( d2S_To3, -d2S_2To3, m1oSqrt2 );
	double	bvn4 =  bivarNormIntegral( d1S_To3, -d1S_2To3, m1oSqrt2 );
	double	tvn1 = trivarNormIntegral( d1S_To3,  d1S_2To3, -d1K_T, -m1oSqrt2, m1oSqrt3, m1oSqrt_2thirds );
	double	tvn2 = trivarNormIntegral( d2S_To3,  d2S_2To3, -d2K_T, -m1oSqrt2, m1oSqrt3, m1oSqrt_2thirds );
	double	  P3 =     KerTo3 * normCDF( -d2S_To3 )
			     - stockPrice * normCDF( -d1S_To3 )
			     +	  Ker2To3 * bvn3
			     - stockPrice * bvn4
			     +	     KerT * tvn1
			     - stockPrice * tvn2;
	double	gjap = P3 + 3.5*(P3-P2) - 0.5*(P2-P1);
	return	gjap;
}			// GeskeJohnsonAmerPut()
//----------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//			The following routines support the bivariate Normal integral
//----------------------------------------------------------------------------------------------------
double fxy( double xx, double yy, double aaPrime, double bbPrime, double rho )
{	double	t1 = aaPrime * (2.0 * xx - aaPrime);
	double	t2 = bbPrime * (2.0 * yy - bbPrime);
	double	t3 = 2.0 * rho * (xx - aaPrime) * (yy - bbPrime);
	return	exp( t1 + t2 + t3 );
}

double	bivarNormIntegral(
	double			aa,
	double			bb,
	double			rho				)
{		// Return M(aa,bb; rho):  the cummulative probability that
		// a first variable is less than aa, and a second variable is less than bb;
		// given that the coefficient of correlation between aa and bb is rho.
		// this the corrected form of Z. Dresner's approximation
		// Refer to p. 266, John Hull's  __Options, Futures, & Other Derivatives__
	double			aaPrime, bbPrime;
	double			M_a_b_rho = 0.0;

		// parameter conditioning ...
	if ( rho >=  1.0 )
		rho =  0.999999;
	if ( rho <= -1.0 )
		rho = -0.999999;
		// Its okay if either aa == 0 or bb == 0, but not both.
		// Can eliminate infinite recursion just by conditioning aa.
    if ( aa == 0.0 )
		aa = -1e-100;		// the sign is very important!

		// the terminal case (for recursion) - all parameters <= 0 ...
	if ( aa <= 0.0  &&  bb <= 0.0  &&  rho <= 0.0 )
	{	const double	AA[4] = { 0.3253030, 0.4211071, 0.1334425, 0.006374323 };
		const double	BB[4] = { 0.1337764, 0.6243247, 1.3425378, 2.2626645   };
		double	sqRt_OneMinusRhoSq = sqrt( 1.0 - rho * rho );
		double	sqRt_2_OneMinusRhoSq = sqrt( 2.0 ) * sqRt_OneMinusRhoSq;
		if ( sqRt_2_OneMinusRhoSq != 0.0 )
		{	aaPrime = aa / sqRt_2_OneMinusRhoSq;
			bbPrime = bb / sqRt_2_OneMinusRhoSq;
		}
		else
		{	aaPrime = aa * 9e300;
			bbPrime = bb * 9e300;
		}
		unsigned short	ii, jj;
		for ( ii = 0; ii < 4; ii++ )
		{	for ( jj = 0; jj < 4; jj++ )
			{		// use the direct method
				M_a_b_rho += AA[ii] * AA[jj]
						   * fxy( BB[ii], BB[jj], aaPrime, bbPrime, rho );
			}
		}
		M_a_b_rho *= sqRt_OneMinusRhoSq / PI;
	}
	else if ( aa * bb * rho <= 0.0 )
	{		// Three cases handled here.  Can use one of the following identities:
			// M(a,b;rho) = N(a) - M(a,-b;-rho)
			// M(a,b;rho) = N(b) - M(-a,b;-rho)
			// M(a,b;rho) = N(a) + N(b) - 1 + M(-a,-b;rho)
			// get best efficiency in one of the first two cases
		if ( aa <= 0.0 )				// using Drezner's (9)
		{	M_a_b_rho = normCDF( aa )
					  - bivarNormIntegral( aa, -bb, -rho );
		}
		else if ( bb <= 0.0 )			// using Drezner's (8)
		{	M_a_b_rho = normCDF( bb )
					  - bivarNormIntegral( -aa, bb, -rho );
		}
		else							// rho <= 0.0, using Drezner's (7)
		{	M_a_b_rho = normCDF( aa ) + normCDF( bb ) - 1.0
					  + bivarNormIntegral( -aa, -bb, rho );
		}
	}
	else								// aa * bb * rho > 0
	{		// four of eight cases are potentially handled here (doubly recursive) ...
			// all parameters are positive or one is positive and the other two are negative
			// use the identity:  M(a,b; rho) = M(a,0; rho1) + M(b,0; rho2) - delta
			// where ...
			// denom = 0 when aa == bb and rho == 1, and when rho = (a^2 + b^2) / 2ab
		double	denom = sqrt( aa*aa - 2.0*rho*aa*bb + bb*bb );
			// prevent divide by zero
		if ( denom == 0.0 )
			denom = 9e-300;
		double	rho1  = (rho*aa - bb) * sign(aa) / denom;
		double	rho2  = (rho*bb - aa) * sign(bb) / denom;
		double	delta = (1.0 - sign(aa)*sign(bb)) / 4.0;
		M_a_b_rho = bivarNormIntegral( aa, 0.0, rho1 )
				  + bivarNormIntegral( bb, 0.0, rho2 )
				  - delta;
	}
	return	M_a_b_rho;
}			// bivarNormIntegral()
//----------------------------------------------------------------------------------------------------
//			The following globals & routines support the trivariate Normal integral
//----------------------------------------------------------------------------------------------------
double			hh[3];
double			rho[3];		// rho12 [1], rho13 [2], rho23 [3]

double	trivarNormIntegral(	double aa,    double bb,    double cc,
						    double rho12, double rho13, double rho23 )
{		// Genz's version
	extern double			hh[3];
	extern double			rho[3];		// r12 [1], r13 [2], r23 [3]
	hh[1] = aa;
	hh[2] = bb;
	hh[3] = cc;
	rho[1] = rho12;
	rho[2] = rho13;
	rho[3] = rho23;

		// Compute trivariate normal probability...
		// Calls:  adonet1, bivarNormIntegral, max, normCDF, tvtmfn1
		// Algorithms developed in these papers:o
		//   R.L. Plackett, Biometrika 41(1954), pp. 351-360.
		//   Z. Drezner, Math. Comp. 62(1994), pp. 289-294.
		//   with adaptive integration from (0,0,1) to (0,0,r23) to R. 
		//
		// Calculate the probability that x(i) < h(i), for i = 1, 2, 3.	 
		//	hh	real 'array' of three upper limits for probability distribution 
		//	rho   real 'array' of three correlation coefficients, r should 
		//		  contain the lower left portion of the correlation matrix R. 
		//		  r should contain the values r21, r31, r23 in that order.
		//	epsi  real required absolute accuracy; maximum accuracy for most
		//		  computations is approximately 1e-14.

	// if we had an adaptive integration routine, we would reinstate the following line
	double	epst = 1.0e-8;		// max( 1.0e-14, epsi );

		// Sort rhos
	if ( fabs(rho12) > fabs(rho13) ) 
	{	swap( hh[2],  hh[3]  );
		swap( rho[1], rho[2] );		// rho12 [1] <--> rho13 [2]
	}
	if ( fabs(rho13) > fabs(rho23) ) 
	{	swap( hh[1],  hh[2]  );
		swap( rho[3], rho[2] );		// rho23 [3] <--> rho13 [2]
	}

		// Check for special cases
	double	tvn = 0.0;						// the function result
	if ( fabs(hh[1]) + fabs(hh[2]) + fabs(hh[3]) < epst )
	{	tvn = ( 1.0 + ( asin(rho[1]) + asin(rho[2]) + asin(rho[3]) ) / (PI/2.0) ) / 8.0;
	}
	else if ( fabs(rho[1]) + fabs(rho[2]) < epst )
	{	tvn = normCDF( hh[1] )
			* bivarNormIntegral( -hh[2], -hh[3], rho[3] );
	}
	else if ( fabs(rho[2]) + fabs(rho[3]) < epst )
	{	tvn = normCDF( hh[3] )
			* bivarNormIntegral( -hh[1], -hh[2], rho[1] );
	}
	else if ( fabs(rho[1]) + fabs(rho[3]) < epst )
	{	tvn = normCDF( hh[2] )
			* bivarNormIntegral( -hh[1], -hh[3], rho[2] );
	}
	else if ( 1.0 - rho[3] < epst )
	{	tvn = bivarNormIntegral( -hh[1], -min(hh[2], hh[3]), rho[1] );
	}
	else if ( rho[3] + 1.0 < epst ) 
	{	if  ( hh[2] > -hh[3] )
		{	tvn = bivarNormIntegral( -hh[1], -hh[2], rho[1] )
				- bivarNormIntegral( -hh[1],  hh[3], rho[1] );
		}
	}
	else
	{		// Compute singular TVNI value
		tvn = normCDF(hh[1])
			* bivarNormIntegral( -hh[2], -hh[3], rho[3] );

			// Use numerical integration to compute probability;
			// We're using a simple quadrature routine.
			// May have to replace it with an adaptive routine later.
		tvn += ShoveltonQuadrature( &pfInt, 0.0, 1.0 ) / (2.0*PI);
	}
	return	max( 0.0, min(tvn, 1.0) ) ;
}			// trivarNormIntegral()
//----------------------------------------------------------------------------------------------------
double	ShoveltonQuadrature( 
	double	(*fcn)(double),
	double	aa,
	double	bb				)
{		// Shovelton's Rule:  5h/126 * (8(f1+f11) + 35(f2+f4+f8+f10) + 15(f3+f5+f7+f9) + 36f6)
	double	hh = (bb - aa) / 10;
	return	5*hh * ( 8*(fcn(aa)      + fcn(bb))									+
				 35*(fcn(aa+  hh) + fcn(aa+3*hh) + fcn(bb-3*hh) + fcn(bb-hh))	+
				 15*(fcn(aa+2*hh) + fcn(aa+4*hh) + fcn(bb-4*hh) + fcn(bb-2*hh))	+
				 36* fcn(aa+5*hh)												  ) / 126;
}
//----------------------------------------------------------------------------------------------------
double	pfInt( double xx )
{		// set up to compute Plackett formula integrands
	double	pfi = 0.0;					// return value
		// globals ...
	extern double	hh[3], rho[3];
	double	rua = asin( rho[1] );
	double	rub = asin( rho[2] );
    double	rr2, r12 = sincos2( rua*xx, rr2 );		// r12 <-- sin(rua*xx), rr2 <-- cos^2(rua*xx)
	double	rr3, r13 = sincos2( rub*xx, rr3 );		// r13 <-- sin(rub*xx), rr3 <-- cos^2(rub*xx)
	if ( fabs(rua) > 0.0 )
		pfi += rua * pntGnd( hh[1], hh[2], hh[3], r13, rho[3], r12, rr2 );
	if ( fabs(rub) > 0.0 )
		pfi += rub * pntGnd( hh[1], hh[3], hh[2], r12, rho[3], r13, rr3 );
	return	pfi;
}				// pfInt()
//----------------------------------------------------------------------------------------------------
double	pntGnd( double ba, double bb, double bc,
			    double ra, double rb, double sr, double rr )
{		// Computes a Plackett formula integrand
	double	pg = 0.0;					// return value
	double	span = ra - rb;
	double	dt = rr*(rr - span*span - 2.0*ra*rb*(1.0 - sr) );
	if ( dt > 0.0 ) 
	{	double	bt = ( bc*rr + ba*(sr*rb - ra) + bb*(sr*ra - rb) ) / sqrt(dt);
		double	tmp = ba - sr*bb; 
		double	ft = tmp*tmp/rr + bb*bb;
		if ( bt > -10.0  &&  ft < 100.0 ) 
		{	pg = exp( -ft / 2.0 );
			if ( bt < 10.0 )
				pg *= normCDF( bt );
		}
	}
	return	pg;
}				// pntGnd()
//----------------------------------------------------------------------------------------------------
double	sincos2( double xx, double& cs2 )
{
	double	tmp = (PI/2.0) - fabs(xx);
	double	ee = tmp*tmp;
	double	sx;						// return value
	if ( ee < 5.0e-5 )
	{		// series approx. for |xx| near PI/2
		sx = ( 1.0 - ee*(1.0 - ee/12.0 ) / 2.0 )*sign( xx );
		cs2 = ee*( 1.0 - ee*(1.0 - 2.0*ee / 15.0 ) / 3.0 );
	}
	else
	{	sx = sin(xx);
		cs2 = 1.0 - sx*sx;
	}
	return	sx;
}				// sincos2()
//----------------------------------------------------------------------------------------------------
/*
COption::COption(void)
{
}
//----------------------------------------------------------------------------------------------------
COption::~COption(void)
{
}
*/
