// OptionsDatabase.h

#pragma once
#include "stocksdatabase.h"

class OptionsDatabase :	public StocksDatabase
{
public:
				OptionsDatabase( void );
	virtual		~OptionsDatabase( void );
	int			StuffCBOEoptionsPrices( const char* fname );
	int			DefineOptionsTable( const char* symbol );
	int			DefineOptionPricesTable( const char* symbol );
	int			GetOptionID( const char* stkSym, const char* optSym, int& option_id );
	PGresult*	GetOptions( char* stkSym );		// return a query result containing all options for this stock symbol
};
