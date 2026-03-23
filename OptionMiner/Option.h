#pragma once
#include "Stock.h"

class COption
{
public:
	COption(void);
	virtual ~COption(void);

	double	COption::RollGeskeWhaleyAmerCall(
					CStock* theStock, double stockPrice,
					double	sigma, double riskFreeRate, double yrsToExpiry );


};
