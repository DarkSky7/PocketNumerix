// ShortRateCalibration.h: interface for the CShortRateCalibration class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHORTRATECALIBRATION_H__9076488A_34A8_43FA_B688_6B64B4370AD2__INCLUDED_)
#define AFX_SHORTRATECALIBRATION_H__9076488A_34A8_43FA_B688_6B64B4370AD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#undef AFX_API
//#define AFX_API AFX_EXT_CLASS
class CVector;

class CShortRateCalibration : public CObject
{
	DECLARE_SERIAL( CShortRateCalibration )

public:
	CShortRateCalibration();
	virtual ~CShortRateCalibration();

public:
		//attributes
//	float	riskFreeRate;		// either user accepts the spot rate computation or provides one
	float	r45da;				// esxtrapolated rate for an eighth of a year
	float	r3mo;
	float	r6mo;
	float	r1yr;
	float	r2yr;
	float	r3yr;
	float	r5yr;
	float	r7yr;
	float	r10yr;
	float	r15yr;
	float	r20yr;
	float	r25yr;
	float	r30yr;
//	short	userSupplied;		// present for convenient Registry storage & retrieval

		// methods
	void		ComputeRates( void );
	double		EvalFunc( double xx, CVector* fitVec );
	bool		GetShortRateBasis( CShortRateCalibration& srb );
	bool		SetShortRateBasis( CShortRateCalibration& srb );

	void	Serialize( CArchive& ar );
};
//#undef AFX_API
//#define AFX_API
#endif // !defined(AFX_SHORTRATECALIBRATION_H__9076488A_34A8_43FA_B688_6B64B4370AD2__INCLUDED_)
