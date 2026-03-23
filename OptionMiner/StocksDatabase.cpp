// StocksDatabase.cpp

#include "StdAfx.h"
#include "ParseSupport.h"
#include "StocksDatabase.h"
#include "ParseSupport.h"			// strtoupper()
#include "utils.h"					// mygetline()
#include "mystring.h"

StocksDatabase::StocksDatabase(void)
{
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StocksDatabase::~StocksDatabase(void)
{
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
char*	StocksDatabase::GetStockSymbol( int stock_id )
{	char		buf[128];
	mystring	sql;
	sprintf( buf, "SELECT symbol from stocks WHERE id=%d", stock_id );
	sql = buf;
	return	GetCstringFromSQL( sql );
}			// GetStockSymbol()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int		StocksDatabase::InsertStock( const char* symbol, const char* company )
{	mystring	sql;
	int			retVal = 0;
		// symbol assumed to be upcase
	sql = "INSERT INTO stocks ( company_name, symbol ) VALUES ('";
	sql += company;
	sql += "', '";
	sql += symbol;
	sql += "')";
	if ( ! RunUpdate( sql ) )
	{	fprintf( stderr, "StocksDatabase::InsertStock: RunUpdate(\"%s\") failed (duplicate definition)\n", sql.c_str() );
		retVal -= 1;									// INSERT INTO stocks failed
	}
	if ( DefineStockPricesTable( symbol ) < 0 )
	{	fprintf( stderr, "StocksDatabase::InsertStock: DefineStockPricesTable(\"%s\") failed (duplicate definition)\n", symbol );
		retVal -= 2;									//
	}
EXIT:
	return	retVal;
}			// InsertStock()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int		StocksDatabase::StuffYahooStockPrices( const char* fname )
{	char		insertSQL[192];
	mystring	sql;
	FILE		*fp;
	char		*company = NULL, *symbol = NULL, *upSymbol = NULL;
	int			linesRead = 0, retVal = 0, errCnt = 0;
	if ( ! ParseFnameCompanySymbol( fname, company, symbol ) )
	{	fprintf( stderr, "StocksDatabase::StuffYahooStockPrices: GetStockIDFromFileName(%s) failed\n", fname );
		retVal = -1;
		goto	EXIT;
	}
	upSymbol = CreateUpString( symbol );
	if ( ! upSymbol )
	{	retVal = -2;
		goto	EXIT;
	}
		// ensure that upSymbol is in the stocks table & a prices table has been created
	InsertStock( upSymbol, company );		// will often get errors for attempting to create a duplicate table definition

	sprintf( insertSQL, "INSERT INTO %s_prices ( date, open, high, low, close, volume, adj_close ) VALUES ( ", symbol );
	fp = fopen( fname, "rb" );
	while ( !feof(fp) )
	{	int		blen = -1;
		char*	line = NULL;
		while ( mygetline( fp, line, blen ) )
		{	char			date[12];
			float			open, high, low, close, adjClose;
			unsigned int	volume;
			if ( ++linesRead > 1 )
			{		// First line: "Date,Open,High,Low,Close,Volume,Adj Close" discarded
				int  nItems = sscanf( line, "%10s,%f,%f,%f,%f,%d,%f",
									&date, &open, &high, &low, &close, &volume, &adjClose );
				if ( nItems == 7 )
				{	char	buf[192];
					sprintf( buf, "'%s', %g, %g, %g, %g, %d, %g )",
							date, open, high, low, close, volume, adjClose );
					sql = insertSQL;
					sql += buf;
					if ( ! RunUpdate( sql ) )
					{		// allow three errors to accumulate
						if ( ++errCnt > 2 )
						{	fprintf( stderr, "StocksDatabase::StuffYahooStockPrices: three RunUpdate(INSERT) errors, giving up\n" );
							return	-4;
						}
					}
				}
			}
			delete [] line;
		}
	}
	fclose( fp );
EXIT:
	if ( company )
		delete [] company;
	if ( symbol )
		delete [] symbol;
	if ( upSymbol )
		delete [] upSymbol;
	return	retVal;
}			// StuffYahooStockPrices()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int		StocksDatabase::DefineStockPricesTable( const char* upSymbol )
{	mystring	sql;
	char	table[64];
	int		cnt;
	sprintf( table, "%s_prices", upSymbol );
	if ( CountTables(table, cnt)  &&  cnt > 0 )
		return	0;										// already have a prices table for this stock symbol
	sql = "id SERIAL NOT NULL,";
	sql += " date DATE,";
	sql += " open FLOAT(24),";
	sql += " high FLOAT(24),";
	sql += " low FLOAT(24),";
	sql += " close FLOAT(24),";
	sql += " volume BIGINT,";
	sql += " adj_close FLOAT(24),";
		// don't store volatility?
		// Could be GJR-GARCH, Asymmetric Power ARCH, ...
		// Would need fitted model params for each volatility model in play
//	sql += " volatility FLOAT,";
	sql += " UNIQUE (date)";
	return	CreateTable( table, sql )  ?  1  :  -2;
}			// DefineStockPricesTable()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int		StocksDatabase::GetStockID( const char* symbol, int& stock_id )
{		// returns -2 if postgre hasn't indexed the symbol
	mystring	sql;
	int			retVal = 0;
	size_t		slen = strlen( symbol );
	char* upSymbol = CreateUpString( symbol );
	if ( ! upSymbol )
	{	fprintf( stderr, "StocksDatabase::GetStockID: CreateUpString(\"%s\") failed\n", symbol );
		retVal = -1;
		goto	EXIT;
	}
	sql = "SELECT id FROM stocks WHERE symbol ='";
	sql += upSymbol;
	sql += "'";
	retVal = GetIntFromSQL( sql, stock_id )  ?  0  :  -2;
EXIT:
	if ( upSymbol )
		delete [] upSymbol;
	return	retVal;
}			// GetStockID()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int		StocksDatabase::GetStockID( const char* company, const char* symbol, int& stock_id )
{		// company string is only used (as input) if postgre hasn't indexed the symbol
	mystring	sql1, sql2;
	int			retVal = 0;
	sql1 = "SELECT id FROM stocks WHERE symbol ='";
	size_t	slen = strlen( symbol );
	char* upSymbol = CreateUpString( symbol );
	if ( ! upSymbol )
	{	fprintf( stderr, "StocksDatabase::GetStockID: CreateUpString(\"%s\") failed\n", symbol );
		retVal = -1;
		goto	EXIT;
	}
	sql1 += upSymbol;
	sql1 += "'";
	if ( GetIntFromSQL( sql1, stock_id ) )
		goto	EXIT;									// fetched the stock_id successfully, retVal = 0

	sql2 = "INSERT INTO stocks ( company_name, symbol ) VALUES ('";
	sql2 += company;
	sql2 += "', '";
	sql2 += upSymbol;
	sql2 += "')";
	if ( ! RunUpdate( sql2 ) )
	{	fprintf( stderr, "StocksDatabase::GetStockID: RunUpdate(\"%s\") failed\n", sql2.c_str() );
		retVal = -2;									// INSERT failed
		goto	EXIT;
	}
		// inserted company & symbol successfully
	if ( ! GetIntFromSQL( sql1, stock_id ) )
	{	fprintf( stderr, "StocksDatabase::GetStockID: GetIntFromSQL(\"%s\") failed\n", sql1.c_str() );
		retVal = -3;									// SELECT after INSERT failed
		goto	EXIT;
	}
	retVal = 1;											// INSERTed company & symbol, SELECTed stock_id
EXIT:
	if ( upSymbol )
		delete [] upSymbol;
	return	retVal;
}			// GetStockID()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
PGresult*	StocksDatabase::GetStocksInPortfolio( int portfolio_id )
{	char		buf[32];
	mystring	sql;
	sql = "SELECT id FROM stock_positions WHERE portfolio_id =";
	_itoa( portfolio_id, buf, 10 );
	sql += buf;
	return	GetQueryResult( sql );
}			// GetStocksInPortfolio()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool		StocksDatabase::GetRecentStockPrice( int stock_id, double& res )
{		// use the day's closing price as the 'recent stock price'
	char		buf[32];
	mystring	sql;
	sql = "SELECT close FROM historical_stock_prices WHERE stock_id=";
	_itoa( stock_id, buf, 10 );
	sql += buf;
	sql += " ORDER BY date DESC LIMIT 1";
	return	GetDoubleFromSQL( sql, res );
}			// GetRecentStockPrice()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool	StocksDatabase::SumDividendsBetweenDates(
	int			stock_id,
	mystring&	purchDateSt,
	mystring&	saleDateSt,
	double&		res			)
{	mystring	sql;
	char		buf[32];
	sql = "SELECT SUM(dividend) FROM dividends WHERE stock_id=";
	_itoa( stock_id, buf, 10 );
	sql += buf;
	if ( !purchDateSt.empty() )
	{	sql += " AND '";
		sql += purchDateSt;
		sql += "' <= date";
	}
	if ( !saleDateSt.empty() )
	{	sql += " AND date <= '";
		sql += saleDateSt;
		sql += "'";
	}
	return	GetDoubleFromSQL( sql, res );
}			// SumDividendsBetweenDates()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void		StocksDatabase::DefineTables( void )
{		// setting up three tables, so successful return mask is 0x7
	int			cnt;
	char*		table;
	short		mask = 0;
	mystring	sql;

	table = "stocks";
	if ( CountTables(table, cnt)  &&  cnt > 0 )
		mask |= 0x1;
	else
	{	sql = "id SERIAL NOT NULL,";
		sql += " company_name CHARACTER VARYING(63),";		// really max 31 characters
		sql += " symbol CHARACTER VARYING(15) NOT NULL,";
		sql += " PRIMARY KEY (id),";
		sql += " UNIQUE (symbol)";
		if ( CreateTable( table, sql ) )
			mask |= 0x1;
	}
	table = "dividends";
	if ( CountTables(table, cnt)  &&  cnt > 0 )
		mask |= 0x2;
	else
	{	sql = "id SERIAL NOT NULL,";
		sql += " stock_id INTEGER,";
		sql += " date DATE,";
		sql += " dividend FLOAT(24),";
		sql += " FOREIGN KEY (stock_id) REFERENCES stocks (id),";
		sql += " UNIQUE (stock_id, date)";
		if ( CreateTable( table, sql ) )
			mask |= 0x2;
	}
/*
	table = "portfolios";
	if ( CountTables(table, cnt)  &&  cnt > 0 )
		mask |= 0x8;
	else
	{	sql = "id SERIAL NOT NULL,";
		sql += " account_name CHARACTER VARYING(63) NOT NULL,";
		sql += " PRIMARY KEY (id),";
		sql += " UNIQUE (account_name)";
		if ( CreateTable( table, sql ) )
			mask |= 0x8;
	}
	table = "stock_positions";
	if ( CountTables(table, cnt)  &&  cnt > 0 )
		mask |= 0x10;
	else
	{	sql = "id SERIAL NOT NULL,";
		sql += " portfolio_id INTEGER,";
		sql += " stock_id INTEGER,";
		sql += " num_units DOUBLE PRECISION,";
		sql += " purchase_date TIMESTAMP,";
		sql += " cost_basis DOUBLE PRECISION,";
		sql += " sale_date TIMESTAMP,";
		sql += " net_sale DOUBLE PRECISION,";
		sql += " note CHARACTER VARYING(255),";
		sql += " PRIMARY KEY (id),";
		sql += " FOREIGN KEY (portfolio_id) REFERENCES portfolios (id),";
		sql += " FOREIGN KEY (stock_id) REFERENCES stocks (id)";
		if ( CreateTable( table, sql ) )
			mask |= 0x10;
	}
	table = "string_registry";
	if ( CountTables(table, cnt)  &&  cnt > 0 )
		mask |= 0x20;
	else
	{	sql = "id SERIAL NOT NULL,";
		sql += " name CHARACTER VARYING(63),";
		sql += " value CHARACTER VARYING(255),";
		sql += " PRIMARY KEY (id),";
		sql += " UNIQUE (name)";
		if ( CreateTable( table, sql ) )
			mask |= 0x20;
	}
	table = "cross_correlation";
	if ( CountTables(table, cnt)  &&  cnt > 0 )
		mask |= 0x40;
	else
	{	sql = "id SERIAL NOT NULL,";
		sql += " stock_id1 INTEGER,";
		sql += " stock_id2 INTEGER,";
		sql += " date DATE, cross-correlation REAL,";
		sql += " FOREIGN KEY (stock_id) REFERENCES stocks (id),";
		sql += " UNIQUE (stock_id, date)";
		if ( CreateTable( table, sql ) )
			mask |= 0x40;
	}
	table = "profile";
	if ( CountTables(table, cnt)  &&  cnt > 0 )
		mask |= 0x80;
	else
	{	sql = "id SERIAL NOT NULL,";
		sql += " stock_id INTEGER,";
		sql += " FOREIGN KEY (stock_id) REFERENCES stocks (id),";
		sql += " UNIQUE (stock_id, date)";
		if ( CreateTable( table, sql ) )
			mask |= 0x80;						// the eighth bit
	}
*/
		// setting up three tables; success mask is 0x7
	if ( mask != 0x3 )
	{	fprintf( stderr, "StocksDatabase::DefineStockTables: mask=0x%x, should be 0x3\n", mask );
		throw "Didn't successfully Init stock database tables";
	}
}			// DefineStockTables()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
mystring*	StocksDatabase::GetRegisteredValue( const char* name )
{	mystring	sql;
	sql = "SELECT value FROM string_registry WHERE name = '";
	sql += name;
	sql += "'";
	return	GetMystringFromSQL( sql );
}			// GetRegisteredValue()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
bool	StocksDatabase::SetRegisteredValue( const char* name, const char* value )
{	
	mystring	sql, *val = GetRegisteredValue( name );
	if ( val->empty() )
	{	sql = "INSERT INTO string_registry (name, value) VALUES ('";
		sql += name;
		sql += "', '";
		sql += value;
		sql += "')";
	}
	else
	{	sql = "UPDATE string_registry SET value = '";
		sql += value;
		sql += "' WHERE name = '";
		sql += name;
		sql += "'";
	}
	if ( val )
		delete	val;
	return	RunUpdate( sql );
}			// SetRegisteredValue()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/*
int		StocksDatabase::GetStockIDFromFilename( const char* fname, int& stock_id )
{		// fname in the form of drive:/path/CompanyName (symbol) ignored description.ext
	char *company = NULL, *symbol = NULL;					// have to delete these
	int	retVal = 0;
	if ( ! ParseFnameCompanySymbol( fname, company, symbol ) )
	{	fprintf( stderr,
			"StocksDatabase::GetStockIDFromFilename: couldn't parse company & symbol from '%s'\n",
				fname );
		retVal = -1;
		goto	EXIT;
	}
		// make sure there's a record for company/symbol in the stocks table
	if ( GetStockID( company, symbol, stock_id ) < 0 )
	{	fprintf( stderr,
			"StocksDatabase::GetStockIDFromFilename: couldn't get stock_id for symbol='%s'\n",
				symbol );
		retVal = -2;
		goto	EXIT;
	}
EXIT:
	if ( symbol )
		delete [] symbol;
	if ( company )
		delete [] company;
	return	retVal;
}			// GetStockIDFromFilename()
*/
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
