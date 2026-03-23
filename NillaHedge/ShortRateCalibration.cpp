// ShortRateCalibration.cpp: implementation of the CShortRateCalibration class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ShortRateCalibration.h"
#include "NillaHedge.h"
#include "Matrix.h"
#include "Vector.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern CNillaHedgeApp theApp;

CShortRateCalibration::CShortRateCalibration()
{	r3mo  = 0.0;
	r6mo  = 0.0;
	r1yr  = 0.0;
	r2yr  = 0.0;
	r3yr  = 0.0;
	r5yr  = 0.0;
	r7yr  = 0.0;
	r10yr = 0.0;
	r15yr = 0.0;
	r20yr = 0.0;
	r25yr = 0.0;
	r30yr = 0.0;
//	riskFreeRate = 0.0;
}

CShortRateCalibration::~CShortRateCalibration()
{
}

IMPLEMENT_SERIAL( CShortRateCalibration, CObject, VERSIONABLE_SCHEMA | 1 )

//--------------------------------------------------------------------
void		CShortRateCalibration::Serialize( CArchive& ar )
{	if ( ar.IsStoring() )
	{	ar << r3mo;
		ar << r6mo;
		ar << r1yr;
		ar << r2yr;
		ar << r3yr;
		ar << r5yr;
		ar << r7yr;
		ar << r10yr;
		ar << r15yr;
		ar << r20yr;
		ar << r25yr;
		ar << r30yr;
//		ar << riskFreeRate;		// user supplied or extrapolated
	}
	else	// loading
	{	ar >> r3mo;
		ar >> r6mo;
		ar >> r1yr;
		ar >> r2yr;
		ar >> r3yr;
		ar >> r5yr;
		ar >> r7yr;
		ar >> r10yr;
		ar >> r15yr;
		ar >> r20yr;
		ar >> r25yr;
		ar >> r30yr;
//		ar >> riskFreeRate;		// user supplied or extrapolated
	}
}			// Serialize()
//--------------------------------------------------------------------
double		CShortRateCalibration::evalFunc( double xx, CVector* fitVec )
{		// compute a rate from a term
		// apply coefficients to basis functions evaluated at xx
	const double	shft = 0.48;
	return	fitVec->elems[0]
		  + fitVec->elems[1] * exp( -(xx+shft) )
		  + fitVec->elems[2] /       (xx+shft)
		  + fitVec->elems[3] *   pow( xx+shft, -2.0 )
		  + fitVec->elems[4] *   pow( xx+shft, -3.0 );
}			// evalFunc()
//--------------------------------------------------------------------
void	CShortRateCalibration::computeRates( void )
{
//	SetCursor( theApp.LoadCursor(IDC_WAIT) );
	unsigned short nzr = (r3mo  > 0.0 ? 1 : 0)
					   + (r6mo  > 0.0 ? 1 : 0)
					   + (r1yr  > 0.0 ? 1 : 0)
					   + (r2yr  > 0.0 ? 1 : 0)
					   + (r3yr  > 0.0 ? 1 : 0)
					   + (r5yr  > 0.0 ? 1 : 0)
					   + (r7yr  > 0.0 ? 1 : 0)
					   + (r10yr > 0.0 ? 1 : 0)
					   + (r15yr > 0.0 ? 1 : 0)
					   + (r20yr > 0.0 ? 1 : 0)
					   + (r25yr > 0.0 ? 1 : 0)
					   + (r30yr > 0.0 ? 1 : 0);

		// initialize sample data (and std. dev.)  Vectors 
		// from the CEdit box values previously captured
	CVector		xx( ++nzr );				// +1 for the 45day (1/8 yr) rate
	double*		xxEl = xx.elems;
	CVector		bb( nzr );
	double*		bbEl = bb.elems;
	const double	mt[13] = { 0.125, 0.25,  0.5,  1.0,  2.0,  3.0,  5.0,
							          7.0,  10.0, 15.0, 20.0, 25.0, 30.0  };
	unsigned short	cc = 0;
						   *(xxEl+cc) = mt[ 0];  *(bbEl+cc++) = (double) r45da;
	if (  r3mo > 0.0f ) {  *(xxEl+cc) = mt[ 1];  *(bbEl+cc++) = (double) r3mo;  }
	if (  r6mo > 0.0f ) {  *(xxEl+cc) = mt[ 2];  *(bbEl+cc++) = (double) r6mo;  }
	if (  r1yr > 0.0f ) {  *(xxEl+cc) = mt[ 3];  *(bbEl+cc++) = (double) r1yr;  }
	if (  r2yr > 0.0f ) {  *(xxEl+cc) = mt[ 4];  *(bbEl+cc++) = (double) r2yr;  }
	if (  r3yr > 0.0f ) {  *(xxEl+cc) = mt[ 5];  *(bbEl+cc++) = (double) r3yr;  }
	if (  r5yr > 0.0f ) {  *(xxEl+cc) = mt[ 6];  *(bbEl+cc++) = (double) r5yr;  }
	if (  r7yr > 0.0f ) {  *(xxEl+cc) = mt[ 7];  *(bbEl+cc++) = (double) r7yr;  }
	if ( r10yr > 0.0f ) {  *(xxEl+cc) = mt[ 8];  *(bbEl+cc++) = (double)r10yr;  }
	if ( r15yr > 0.0f ) {  *(xxEl+cc) = mt[ 9];  *(bbEl+cc++) = (double)r15yr;  }
	if ( r20yr > 0.0f ) {  *(xxEl+cc) = mt[10];  *(bbEl+cc++) = (double)r20yr;  }
	if ( r25yr > 0.0f ) {  *(xxEl+cc) = mt[11];  *(bbEl+cc++) = (double)r25yr;  }
	if ( r30yr > 0.0f ) {  *(xxEl+cc) = mt[12];  *(bbEl+cc  ) = (double)r30yr;  }

		// shft = 0.48 seems to work well with the following US Treasury rates
		// (current on 27-Dec-03).  Short rate should be something like:  0.61  
		// 0.7600    0.9100    1.7900    2.2500    3.1400    4.1400    4.9600
	const double	shft = 0.48;
	CMatrix		aa( nzr, 5 );
	double**    aaEl = aa.elems;
		// the following for loop is the equivalent of calling evalFunc() with
		// a fitVec with all 1's as coefficients
	for ( unsigned short rr = 0; rr < nzr; rr++ )
	{	aaEl[rr][0] = 1.0;
		aaEl[rr][1] = exp( -(*(xxEl+rr)+shft));
		aaEl[rr][2] = 1.0 / (*(xxEl+rr)+shft);
		aaEl[rr][3] = pow(  (*(xxEl+rr)+shft), -2.0);
		aaEl[rr][4] = pow(  (*(xxEl+rr)+shft), -3.0);
	}
		// compute the Moore-Penrose pseudoinverse
	CMatrix*	aT = aa.transpose();
	CMatrix*	aTa = *aT * aa;
	CMatrix*	aTaI = aTa->inverse();
	if ( aTaI == NULL )
		return;							// no inverse (A'A)^(-1), so no solution

	CMatrix*	pinv = *aTaI * aT;		// this is the Moore-Penrose pseudoInverse
	CVector*	objFunc = *pinv * bb;	// coefficients of basis functions above

	if (  r3mo <= 0.0f ) {   r3mo = (float)evalFunc( mt[ 0], objFunc );  }
	if (  r6mo <= 0.0f ) {   r6mo = (float)evalFunc( mt[ 1], objFunc );  }
	if (  r1yr <= 0.0f ) {   r1yr = (float)evalFunc( mt[ 2], objFunc );  }
	if (  r2yr <= 0.0f ) {   r2yr = (float)evalFunc( mt[ 3], objFunc );  }
	if (  r3yr <= 0.0f ) {   r3yr = (float)evalFunc( mt[ 4], objFunc );  }
	if (  r5yr <= 0.0f ) {   r5yr = (float)evalFunc( mt[ 5], objFunc );  }
	if (  r7yr <= 0.0f ) {   r7yr = (float)evalFunc( mt[ 6], objFunc );  }
	if ( r10yr <= 0.0f ) {  r10yr = (float)evalFunc( mt[ 7], objFunc );  }
	if ( r15yr <= 0.0f ) {  r15yr = (float)evalFunc( mt[ 8], objFunc );  }
	if ( r20yr <= 0.0f ) {  r20yr = (float)evalFunc( mt[ 9], objFunc );  }
	if ( r25yr <= 0.0f ) {  r25yr = (float)evalFunc( mt[10], objFunc );  }
	if ( r30yr <= 0.0f ) {  r30yr = (float)evalFunc( mt[11], objFunc );  }

	delete pinv, objFunc;
//	SetCursor( theApp.LoadCursor(IDC_ARROW) );

	//	double*		ofEl = objFunc->elems;
		//	fitSpotRate =	ofEl[0]	* 1.0				// * 1.0
		//				+ ofEl[1] * exp(-shft)			// * exp(-(xx + shft))
		//				+ ofEl[2] / shft				// * 1 / (xx + shft)
		//				+ ofEl[3] * pow(shft,-2.0)		// * 1 / (xx + shft)^2
		//				+ ofEl[4] * pow(shft,-3.0);		// * 1 / (xx + shft)^3
		// (float)evalFunc( 0.0, objFunc );
	return;		
}			// computeRates()
//--------------------------------------------------------------------
