#include "StdAfx.h"
#include "Stock.h"

CStock::CStock( void ) : stock_id(0), symbol(NULL)
{	
}

CStock::CStock( const char* stkSym ) : stock_id(0)
{		// 
	size_t slen = strlen( stkSym );
	symbol = ( slen > 0 )  ?  new char[slen+1]  :  NULL;
	if ( symbol )
		strcpy( symbol, stkSym );
}

CStock::~CStock( void )
{
	if ( symbol )
		delete [] symbol;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int		CStock::GetStockID( StocksDatabase& db )
{	
	if ( stock_id )
		return	stock_id;
	return	db.GetStockID( symbol, stock_id );
}			// GetStockID()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
