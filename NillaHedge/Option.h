// Option.h: interface for the COption class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPTION_H__D23B06D9_69A1_4D60_806B_D187A52E91F1__INCLUDED_)
#define AFX_OPTION_H__D23B06D9_69A1_4D60_806B_D187A52E91F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BOS.h"
#include "OptionType.h"
#include "Stock.h"
#include "DateSupport.h"			// for calcYears()

//#include "OleDateTimeEx.h"
//#include <afxcoll.h>

class	CStock;

typedef struct
{	bool			calcPvDivs;
	bool			calcYrsToExpiry;

	COleDateTime	fromDate;				// x changing this
	double			riskFreeRate;			// x doesn't change frequently
	double			sigma;					// x fixed per option
	double			stockPrice;				//

	double			pvDivs;
	double			yrsToExpiry;
} OptionEvaluationContext;

class COption : public CBOS
{
	DECLARE_SERIAL( COption )

public:
					COption();
					COption( CBOS& bos );
					COption( CString sym );
					COption( COption& anOption );
	virtual			~COption();
	void			Serialize( CArchive& ar );
	AssetType		getAssetType( void ) {	return Option;	};

	CStock*			getUnderlying();
	float			getVolatility( void );
	double			getStockPrice( void );
	short			getPutCall( void )					{	return	putCall;	}
	void			setPutCall( short pc );
	void			setPutCall( OptionType pc )			{	putCall = pc;		}
	void			setPutCall( CArchive& ar );
	COleDateTime	getExpiry( void )					{	return	expiry;		}
	void			setExpiry( COleDateTime exp );
	bool		CanComputeBlackScholes( OptionEvaluationContext& evcs );
	bool		CanComputeBlackScholes( double stockPrice, double sigma );
	bool		ExerVal( double& exerVal );
	bool		ExerVal( OptionEvaluationContext& evcs, double& exerVal );
	bool		Delta( OptionEvaluationContext& evcs, double& delta );
	bool		Gamma( OptionEvaluationContext& evcs, double& gamma );		// gamma is valid if true, else can't be evaluated
	bool		RhoD ( OptionEvaluationContext& evcs, double& rhoD );
	bool		RhoR ( OptionEvaluationContext& evcs, double& rhoR );
	bool		Theta( OptionEvaluationContext& evcs, double& theta );
	bool		Vega ( OptionEvaluationContext& evcs, double& vega );
	double		DivYield( void )
				{	return	underlying  ?  underlying->curYield()  :  0.0;		}
	double		YrsToExpiry( COleDateTime evalDate )
				{	long	spDays;
					return	calcYears( evalDate, expiry, spDays );				}
	bool		pITM( OptionEvaluationContext& evcs, double& pITM );
	bool		ImpliedVolatility( OptionEvaluationContext& evcs, double& minErr );
	bool		EuroValue( OptionEvaluationContext& evcs, double& euroVal );
	bool		EuroValue( double yrsToExpiry, double riskFreeRate, double divYield, double& euroVal );

private:
		// the core of Black=Scholes value is computed vy BSdt1()
		// most of its functionality is also present in EuroValue()
	bool	BSdt1( OptionEvaluationContext& evcs, double& sigSqT, double& dt1 );
	void	VettEvalCtx( OptionEvaluationContext& evcs );
	void	VettExpiry( OptionEvaluationContext& evcs );
	double	GetSeDt( OptionEvaluationContext& evcs, bool& usedWilmott );	// sets usedWilmott
	bool	BSd1( OptionEvaluationContext& evcs, double& sigSqT, double& d1 );
	bool	BSd2( OptionEvaluationContext& evcs, double& sigSqT, double& d2 );
	bool	BSd2d1( OptionEvaluationContext& evcs, double& sigSqT, double& d1, double& d2 );

public:					// attributes
	CStock*				underlying;			// in memory use only
	OptionType			putCall;
	float				strikePrice;		// dollars
		// non-user data
	long				stockSymbol_ID;		// offset into StockDefs.pnh

private:
	COleDateTime		expiry;

};
#endif // !defined(AFX_OPTION_H__D23B06D9_69A1_4D60_806B_D187A52E91F1__INCLUDED_)
/*
		// the original euroValue(), the new one was euroValue2()
	double				euroValue( 
							   double stockPrice, double sigma, double yrsToExpiry, double riskFreeRate, double divYield = 0.0 );
		// the original impliedVolatility(), the new one was impliedVolatility3()
	double				impliedVolatility( double& minErr,
							   double stockPrice, double yrsToExpiry, double riskFreeRate, double divYield = 0.0 );
	double				impliedVolatility2( double& minErr,
							   double stockPrice, double yrsToExpiry, double riskFreeRate );
		// the original vega(), the new one was vega2()
	double				vega ( double stockPrice, double sigma, double yrsToExpiry, double riskFreeRate, double divYield = 0.0 );
*/