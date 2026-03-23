// Bond.h: interface for the CBond class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOND_H__25D39E94_CD1A_4080_A322_F68BABBFA4BC__INCLUDED_)
#define AFX_BOND_H__25D39E94_CD1A_4080_A322_F68BABBFA4BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include <afxcoll.h>

//#undef AFX_API
//#define AFX_API AFX_EXT_CLASS

#include "BOS.h"
//#include "NillaHedge.h"				// DayCountMethod, calcYears
//#include "OleDateTimeEx.h"


enum DayCountMethod
{	UnknownDayCountMethod	=  0,
	ActualActual			=  1,
	Actual360				=  2,
	Thirty360				=  3
};

typedef struct
{	bool			yrsToMaturityIsValid;
	bool			ytmIsValid;
	bool			macDurIsValid;
	bool			cvxtyIsValid;
	COleDateTime	fromDate;
	double			yrsToMaturity;
	double			ytm;
	double			ytmErr;
	double			macDur;
	double			cvxty;
} BondEvaluationContext;

class CBond : public CBOS
{
	DECLARE_SERIAL( CBond )

public:
						CBond();
						CBond( CString sym );
						CBond( CBond& bnd );
						CBond( CBOS& bos );
	virtual				~CBond( void );
//	void				Invalidate( void );

//	void				Serialize( CArchive& ar );

	static	double		GetErrTol( void )			{	return	0.005;				}

		// Accessors - getters
	AssetType			getAssetType( void )		{	return	Bond;				}
	unsigned char		getCouponsPerYear( void )	{	return	couponsPerYear;		}
	float				getCouponRate( void )		{	return	couponRate;			}
	DayCountMethod		getDayCounting( void )		{	return	dayCounting;		}
	COleDateTime		getIncepDate( void )		{	return	incepDate;			}
	COleDateTime		getMaturityDate( void )		{	return	maturityDate;		}
	float				getParValue( void )			{	return	parValue;			}
		// look like accessors
	int					GetCouponIntervals( COleDateTime aDate );
	COleDateTime		getCouponDate( int couponIntervals );
		// yield calculations
	double				getCoupon( void )
						{	return ( couponsPerYear < 1 )
										?  0.0
										:  couponRate * parValue / couponsPerYear;
						}
	double				currentYield( void )
						{	return	( mktPrice == 0.0  ||  couponsPerYear < 1 )
										?  0.0
										:  couponRate * parValue / mktPrice;
						}

		// Accessors - setters
	void	setCouponsPerYear( unsigned char cpy );
	void	setCouponRate( float cpr );
	void	setDayCounting( DayCountMethod dcm );
	void	setIncepDate( COleDateTime incDt );
	void	setMaturityDate( COleDateTime matDt );
	void	setParValue( float parVal );

		// time shifts
	COleDateTime		SkipCpnPeriods( COleDateTime theDate, short nPeriods );

		// yieldToMaturity, duration, ...
	double	DailyRate( double annualRate, long yr );
	double	YieldToMaturity( BondEvaluationContext& evalCtx );
	double	YieldToMaturity( double& err, COleDateTime evalDate = COleDateTime::GetCurrentTime() );
	double	MacaulayDuration( BondEvaluationContext& evalCtx );
	double	MacaulayDuration( COleDateTime evalDate = COleDateTime::GetCurrentTime() );
	double	ModifiedDuration( BondEvaluationContext& evalCtx );
	double	ModifiedDuration( double& macDur, COleDateTime evalDate = COleDateTime::GetCurrentTime() );
	double	Convexity( BondEvaluationContext& evalCtx );
	double	Convexity( COleDateTime evalDate = COleDateTime::GetCurrentTime() );
	double	DeltaPdivP( BondEvaluationContext& evalCtx, double deltaYTM );
	double	DeltaPdivP( double deltaYTM );

		// discounting, present value, ...
	double	PresentValue( double ytm, COleDateTime evalDate = COleDateTime::GetCurrentTime() )
			{	return  PresentValuePar( evalDate, ytm )
					+	PresentValueCoupons( evalDate, maturityDate, ytm );
			}

	double	PresentValue( double yrs, char dummy, double ytm )
			{	return  PresentValuePar( yrs, 6, ytm )
					+	PresentValueCoupons( yrs, 6, ytm );
			}
	double	PresentValueCoupons( double yrs, char dummy, double ytm = 0.0 );

		 // calls Discount() to get the present value of the coupons
	double	PresentValueCoupons( COleDateTime fromDate, COleDateTime toDate, double ytm = 0.0 );
	double	PresentValuePar( COleDateTime evalDate, double ytm = 0.0 );
		// the following overload has 'char dummy' as the second argument because the compiler
		// was erroneously mapping calls to PresentValuePar( double, double )
		//								 to PresentValuePar( COleDateTime, double )
	double	PresentValuePar( double yrs, char dummy, double ytm = 0.0 );

protected:			//attributes
	float				couponRate;			// a fraction			(4 bytes)
	unsigned char		couponsPerYear;		//						(1 byte)
	DayCountMethod		dayCounting;		//						(1 byte)
	COleDateTime		incepDate;			//						(3 bytes compressed)
	COleDateTime		maturityDate;		//						(3 bytes compressed)
	float				parValue;			// dollars				(4 bytes)

private:
	double	ConvexityCore( double yrs, double ytm );
	double	ValueCouponsCore( double nCpnPeriods, double periodicYield, bool discount = true );
	double	MacaulayDurationCore( double yrs, double ytm );
	double	YieldToMaturityCore( double& err, double yrs );
};
		// year counting (a global function)
	double	calcYears( COleDateTime fromDate, COleDateTime toDate, DayCountMethod method, long& spDays );

//#undef AFX_API
//#define AFX_API
#endif // !defined(AFX_BOND_H__25D39E94_CD1A_4080_A322_F68BABBFA4BC__INCLUDED_)
/*
		// Discount() uses calcYears-like functionality, possibly not debugged
		// hence the version now in use
	double	PresentValuePar( COleDateTime aDate, double ytm = 0.0 )
			{	COleDateTime today = COleDateTime::GetCurrentTime();
				bool	reverse = ( (maturityDate < today) != 0 );
				return	parValue * Discount( aDate, maturityDate, discRate, reverse );
			}
//	double	PresentValueCoupons( BondEvaluationContext& evalCtx, double	ytm );
//	double	PresentValueCouponsToMaturity( double yrsToMaturity, double ytm );
//	double	Discount( COleDateTime fromDate, COleDateTime toDate,
//					  double discRate = 0.0, bool reverse = false );
*/
