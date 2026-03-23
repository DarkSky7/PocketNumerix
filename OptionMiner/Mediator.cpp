#include "Stock.h"
#include "Option.h"						// gets you Stock.h too?

#include "StocksDatabase.h"
#include "OptionsDatabase.h"			// gets you StocksDatabase.h too?

CStock*		GetStock( StocksDatabase& db, const char* stkSym )
{	int			stock_id;
	CStock*		stk = NULL;
	if ( db.GetStockID( stkSym, stock_id ) >= 0 )
	{	stk = new CStock( stkSym );
		stk->stock_id = stock_id;
	}
	return	stk;
}	// GetStock

