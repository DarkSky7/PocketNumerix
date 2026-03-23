#pragma once
#include "mystring.h"
#include "StocksDatabase.h"
#include <time.h>

class CStock
{
public:
				CStock( void );
				CStock( const char* stkSym );
	virtual		~CStock( void );
	int			GetStockID( StocksDatabase& db );

private:
	int			stock_id;
	char*		symbol;
	float		price;
	float		volatility;
	float		dividend;
	time_t		divDate;
};