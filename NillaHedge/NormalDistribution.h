// NormalDistribution.h

#if !defined(AFX_NormalDistribution_H__BF98493C_AE1F_4C52_95B7_A4A03AKQXZA87931C4F2__INCLUDED_)
#define      AFX_NormalDistribution_H__BF98493C_AE1F_4C52_95B7_A4A03AKQXZA87931C4F2__INCLUDED_

const double	PI =  3.141592653589793238462643383279502884197;
const double	sqrt2PI = sqrt( 2.0*PI );

double			normCDF( double xx );
inline double	Nprime( double xx )	
				{  return	exp( -0.5*xx*xx ) / sqrt2PI;		}

#endif			// AFX_NormalDistribution_H__BF98493C_AE1F_4C52_95B7_A4A03AKQXZA87931C4F2__INCLUDED_