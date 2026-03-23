// StocksDatabase.h

#pragma once
#include "Database.h"
#include <libpq-fe.h>				// from D:\Programs\PostgreSQL\8.4\include
#include "utils.h"


class StocksDatabase : public Database
{
public:
				StocksDatabase( void );
	virtual		~StocksDatabase( void );

	void		DefineTables( void );
	int			DefineStockPricesTable( const char* symbol );
	bool		GetRecentStockPrice( int stock_id, double& res );
	mystring*	GetRegisteredValue( const char *name );
	int			GetStockID( const char* symbol, int& stock_id );
	int			GetStockID( const char* company, const char* symbol, int& stock_id );
//	int			GetStockIDFromFilename( const char* fname, int& stock_id );
	PGresult*	GetStocksInPortfolio( int portfolio_id );
	char*		GetStockSymbol( int stock_id );
	int			InsertStock( const char* symbol, const char* company );
	bool		SetRegisteredValue( const char *name, const char *value );
	int			StuffYahooStockPrices( const char* fname );
	bool		SumDividendsBetweenDates( int stock_id, mystring& purchDateSt, mystring& saleDateSt, double& res );
};