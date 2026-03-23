#include "StdAfx.h"
#include "OptionsDatabase.h"
#include "ParseSupport.h"
#include "utils.h"					// mygetline()

OptionsDatabase::OptionsDatabase( void )
{
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
OptionsDatabase::~OptionsDatabase( void )
{
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int		OptionsDatabase::StuffCBOEoptionsPrices( const char* fname )
{		// get company name and stock symbol from the first few lines in the file
	mystring	myline, sql, sqlOptDefs, sqlOptPrices;
	int		nOptsRead = 0;			// number of options read
	int		errCnt = 0;
	int		retVal = 0;
	char	dateTime[18];			// no room for a time zone modifier
	CFile	fp;
	CFileException ex;
	wchar_t* w_fname = GetWideString( fname );
	if ( ! fp.Open( w_fname, CFile::modeRead, &ex ) )
	{	fprintf( stderr, "OptionsDatabase::StuffCBOEoptionsPrices: fp.Open(\"%s\") failed\n", fname );
		retVal = -1;
		goto	EXIT;
	}
	char	*line = NULL, *stkSym = NULL, *company = NULL, *downSym = NULL;
	int		stock_id, res;
		// get line with symbol & company name
	mygetline( fp, line, res );			// get first line from fp --> "INTC (INTEL CORP),23.84,-0.15,"
	if ( ! line )
	{	fprintf( stderr, "OptionsDatabase::StuffCBOEoptionsPrices: mygetline(1) failed\n" );
		retVal = -2;
		goto	EXIT;
	}
		// get stock symbol and company name from the first line in the file
	if ( ! ParseCBOEsymbolCompany( line, stkSym, company ) )
	{	fprintf( stderr,
			"OptionsDatabase::StuffCBOEoptionsPrices: ParseCBOEsymbolCompany(\"%s\",...) failed\n" );
		retVal = -3;
		goto	EXIT;
	}
	delete [] line;									// done with first line from fp
	line = NULL;
	downSym = CreateDownString( stkSym );
	mygetline( fp, line, res );			// get second line from fp --> "Apr 23 2010 @ 13:37 ET,Bid,23.83,Ask,23.84,Size,214x259,Vol,36256404,"
	if ( ! line )
	{	fprintf( stderr, "OptionsDatabase::StuffCBOEoptionsPrices: mygetline(2) failed\n" );
		retVal = -4;
		goto	EXIT;
	}
		// arrange the date and time as: "2010-Apr-23 13:37"
	if ( ParseCBOEdateTime( line, dateTime, sizeof(dateTime) ) < 0 )
	{	retVal = -5;
		goto	EXIT;
	}
	if ( ! ReadPast(fp,"Puts")  ||  ! ReadPast(fp,"Open Int,") )		// eat the next two lines
	{	fprintf( stderr,
			"OptionsDatabase::StuffCBOEoptionsPrices: ReadPast(\"Puts\") or ReadPast(\"Open Int,\") failed\n" );
		retVal = -6;
		goto	EXIT;
	}
/*		// don't need the stock_id because referential integrity is not being enforced
	res = GetStockID( company, downSym, stock_id );
	if ( res < 0 )
	{	fprintf( stderr,
			"OptionsDatabase::StuffCBOEoptionsPrices: GetStockID(\"%s\",\"%s\") failed\n",
				company, downSym );
		retVal = -7;
		goto	EXIT;
	}
*/
	if ( DefineOptionsTable( downSym ) < 0 )
	{	fprintf( stderr,
			"OptionsDatabase::StuffCBOEoptionsPrices: DefineOptionsTable(\"%s\") failed\n",
				downSym );
		retVal = -8;
		goto	EXIT;
	}
	if ( DefineOptionPricesTable( downSym ) < 0 )
	{	fprintf( stderr,
			"OptionsDatabase::StuffCBOEoptionsPrices: DefineOptionPricesTable(\"%s\") failed\n",
				downSym );
		retVal = -9;
		goto	EXIT;
	}
	char		expMon[4], optSym[32], buf[192];
	sqlOptDefs  = "INSERT INTO ";
	sqlOptDefs  += downSym;
	sqlOptDefs  += "_options ( symbol, put_call, strike, expiry ) VALUES ( ";
	sqlOptPrices = "INSERT INTO ";
	sqlOptPrices += downSym;
	sqlOptPrices += "_option_prices ( date_time, option_id, last_sale, bid, ask, volume, open_int ) VALUES ( ";
	float			strike, lastSale, net, bid, ask;			// not going to save net (price difference from yesterday's close)
	unsigned int	expYr, volume, openInt, nItems, prevExpYr = 999;
	int				option_id, thirdFriday, monIdx, prevMonIdx = -1;
	size_t			offset;
	const char		putCall[3] = "CP";							// in the order they occur in the columns of the file
	while ( !Eof(fp) )
	{	while ( mygetline(fp, line, res) )
		{	offset = 0;
			for ( short ii = 0; ii < 2; ii++ )
			{		// a Call when ii=0; a Put when ii=1
				nItems = sscanf( line+offset, "%2d %3s %f", &expYr, &expMon, &strike );
				if ( nItems != 3 )
					goto	CLEANUP;
				nOptsRead++;
				myline = line+offset;									// so we can use myline.find()
					// calculate the offset of the second column
				size_t	openParen = myline.find( '(' );
				size_t	closeParen = myline.find( ')', openParen+1 );
				int	symLen = (int)(closeParen - openParen - 1);
				strncpy( optSym, line+offset+openParen+1, symLen );
				*(optSym+symLen) = '\0';
				mystring myOptSym = optSym;
				if ( myOptSym.find( "-" ) != std::string::npos )
					goto	CLEANUP;									// we're going to ignore prices cross-listed options (on other exchanges)
				nItems = sscanf( line+offset+closeParen+2, "%f,%f,%f,%f,%d,%d",
													&lastSale, &net, &bid, &ask, &volume, &openInt );
				if ( nItems != 6 )
					goto	CLEANUP;

					// set up for the 'Put' half of the line
				if ( offset == 0 )
				{	int oP2 = myline.find( '(', closeParen+1 );
					if ( oP2 == std::string::npos )
						goto	CLEANUP;
					int prevComma = myline.rfind( ',', oP2 );			// back up to the comma before the openParen
					if ( prevComma == std::string::npos )
						goto	CLEANUP;
					offset = prevComma+1;
				}

					// only recompute thirdFriday when monIdx or expYr have changed
				monIdx = GetMonIdx( expMon );					// get 0-based index of 3-char month string
				if ( expYr != prevExpYr  ||  monIdx != prevMonIdx )
				{	thirdFriday = ThirdFriday( monIdx, expYr );
					if ( thirdFriday < 1 )
						goto	CLEANUP;
					prevMonIdx = monIdx;
					prevExpYr = expYr;
				}
					// INSERT an option definition
				sprintf( buf, "'%s', '%c', %g, '20%2d-%3s-%02d' )",
							  optSym, putCall[ii], strike, expYr, expMon, thirdFriday );
				sql = sqlOptDefs;
				sql += buf;
				RunUpdate( sql );									// don't worry about errors, they're probably the result of UNIQUE violations
				if ( GetOptionID( downSym, optSym, option_id ) < 0 )
					goto	CLEANUP;								// can't insert prices without an option_id

					// INSERT option prices
				sprintf( buf, "'%s', %d, %g, %g, %g, %d, %d )",
							  dateTime, option_id, lastSale, bid, ask, volume, openInt );
				sql = sqlOptPrices;
				sql += buf;
				RunUpdate( sql );									// don't worry about errors, they're probably the result of UNIQUE violations
			}			// for ( short ii = 0; ii < 2; ii++ )
CLEANUP:
			delete [] line;
			line = NULL;
		}			// while ( mygetline(fp, line, res) )
	}			// while ( ! feof(fp) )
EXIT:
	if ( line )
		delete [] line;
	if ( stkSym )
		delete [] stkSym;
	if ( downSym )
		delete [] downSym;
	if ( company )
		delete [] company;
	if ( w_fname )
		delete [] w_fname;
	fp.Close();
	return	retVal;
}			// StuffCBOEoptionsPrices()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int		OptionsDatabase::GetOptionID( const char* stkSym, const char* optSym, int& option_id )
{	mystring	sql;
	sql = "SELECT id FROM ";
	sql += stkSym;
	sql += "_options WHERE symbol = '";
	sql += optSym;
	sql += "'";
	return	GetIntFromSQL( sql, option_id )  ?  0  :  -1;
}			// GetOptionID()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int		OptionsDatabase::DefineOptionPricesTable( const char* symbol )
{	mystring	sql;
	char	table[64];
	int		cnt, retVal = 0;
	char*  downSym = CreateDownString( symbol );
	if ( ! downSym )
	{	retVal = -1;
		goto	EXIT;
	}
	sprintf( table, "%s_option_prices", downSym );
	if ( CountTables(table, cnt)  &&  cnt > 0 )
		return	retVal;										// already have a prices table for this stock symbol
	sql = "id SERIAL NOT NULL,";
	sql += " option_id INTEGER,";
	sql += " date_time TIMESTAMP,";
	sql += " last_sale FLOAT(24),";
	sql += " bid FLOAT(24),";
	sql += " ask FLOAT(24),";
	sql += " volume INTEGER,";
	sql += " open_int INTEGER,";
	sql += " FOREIGN KEY (option_id) REFERENCES ";
	sql += downSym;
	sql += "_options(id),";
	sql += " UNIQUE (option_id,date_time)";
	retVal = CreateTable( table, sql )  ?  1  :  -1;
EXIT:
	if ( downSym )
		delete [] downSym;
	return	retVal;
}			// DefineOptionPricesTable()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int		OptionsDatabase::DefineOptionsTable( const char* symbol )
{	mystring	sql;
	char	table[64];
	int		cnt, retVal = 0;
	char*  downSym = CreateDownString( symbol );
	if ( ! downSym )
	{	retVal = -1;
		goto	EXIT;
	}
	sprintf( table, "%s_options", downSym );
	if ( CountTables(table, cnt)  &&  cnt > 0 )
		return	retVal;											// already have a prices table for this stock symbol
	sql = "id SERIAL NOT NULL,";
	sql += " symbol CHARACTER VARYING(31) NOT NULL,";
//	sql += " stock_id INTEGER,";								// underlying issue
	sql += " put_call CHAR(1),";								// 'P' or 'C'
	sql += " strike FLOAT(24),";
	sql += " expiry DATE,";
	sql += " PRIMARY KEY (id),";
//	sql += " FOREIGN KEY (stock_id) REFERENCES stocks (id),";
	sql += " UNIQUE (symbol)";
	retVal = CreateTable( table, sql )  ?  1  :  -1;
EXIT:
	if ( downSym )
		delete [] downSym;
	return	retVal;
}			// DefineOptionPricesTable()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
PGresult*		OptionsDatabase::GetOptions( char* stkSym )
{	mystring	sql;
	sql = "SELECT * FROM ";
	sql += stkSym;
	sql += "_options";
	return	GetQueryResult( sql );
}			// GetOptions()
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/*
void		OptionsDatabase::CreateOptionsTables( int stk_id )
{		// one options_prices table for each stock id
	char		buf[32];
	mystring	sql, table;
	_itoa( stk_id, buf, 10 );	// should the qualifier be stock symbol?
	short		mask = 0;
	table = "options_";
	table += buf;
	int			cnt;
	if ( CountTables(table.c_str(), cnt)  &&  cnt < 1 )
	{	sql = "id SERIAL NOT NULL, symbol VARCHAR(15) NOT NULL,";
		sql += " is_call BOOLEAN, strike FLOAT, expiry DATE,";
		sql += " UNIQUE ( expiry, strike )";				// expiry over strike
		if ( CreateTable( table.c_str(), sql ) )
			mask |= 0x1;
	}
		// options_prices_XXX tables are great candidates for a column oriented data store
	table = "option_prices_";
	table += buf;
	if ( CountTables(table.c_str(), cnt)  &&  cnt < 1 )
	{		// intraday option prices
		sql = "id SERIAL NOT NULL, option_id INTEGER REFERENCES options_";
		sql += buf;
		sql += "(id), date_time TIMESTAMP, last FLOAT, bid FLOAT, ask FLOAT, volume BIGINT,";
		sql += " UNIQUE ( option_id, dateTime )";
		if ( CreateTable( table.c_str(), sql ) )
			mask |= 0x2;
	}
	if ( mask != 0x3 )
		fprintf( stderr, "OptionsDatabase::CreateOptionsTables: mask=0x%x, should be 0x3\n", mask );
}			// CreateOptionsTables()
*/
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/*
void		OptionsDatabase::DefineOptionTables( void )
{	int			cnt;
	char*		table;
	short		mask = 0;
	mystring	sql;

	StocksDatabase::DefineStockTables();

		// set up options tables
	table = "options";
	if ( CountTables(table, cnt)  &&  cnt > 0 )
		mask |= 0x1;
	else
	{	sql = "id SERIAL NOT NULL,";
		sql += " symbol CHARACTER VARYING(31) NOT NULL,";
		sql += " exchange CHAR(1),";
		sql += " strike FLOAT,";
		sql += " expiry DATE,";
		sql += " PRIMARY KEY (id),";
		sql += " FOREIGN KEY (stock_id) REFERENCES stocks (id),";
		sql += " UNIQUE (symbol)";
		if ( CreateTable( table, sql ) )
			mask |= 0x1;
	}

		// setting up three tables; success mask is 0x3
	if ( mask != 0x3 )
	{	fprintf( stderr, "OptionsDatabase::DefineTables: mask=0x%x, should be 0x3\n", mask );
		throw "Didn't successfully Init option database tables";
	}
}			// DefineOptionTables()
*/
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
