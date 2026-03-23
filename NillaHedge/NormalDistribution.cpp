#include "StdAfx.h"
#include "NormalDistribution.h"

double	normCDF(
	double			xx	)
{		// An approximation to the cumulative normal distribution function
		// think this was from __Paul Wilmott Introduces Quantitative Finance__
		// this function is slightly more accurate than the phi/erf pair of routines.
		// In 100 samples (from -2 .. 2), the sum squared error against Matlab's erf
		// was 8.2e-10 smaller.  The actual SSE = 8.224400049883229e-010
	const double	a1 =  0.319381530;
	const double	a2 = -0.356563782;
	const double	a3 =  1.781477937;
	const double	a4 = -1.821255978;
	const double	a5 =  1.330274429;
	double			LL, KK, ww;

	LL = fabs( xx );
	KK = 1.0 / (1.0 + 0.2316419 * LL);

		//	original formulation
//	ww = 1.0 - exp( -0.5 * LL * LL ) / sqrt2PI
//	   * ( a1*KK + a2*KK*KK + a3*pow(KK,3.0) + a4*pow(KK,4.0) + a5*pow(KK,5.0) );

		// my revamped formulation
	ww = 1.0 - exp( -0.5*LL*LL ) / sqrt2PI
	   * ( KK*(a1 + KK*(a2 + KK*(a3 + KK*(a4 + KK*a5)))));

	return	(xx < 0.0) ? 1.0 - ww : ww;
}				// normCDF()
//----------------------------------------------------------------------------------------------------
