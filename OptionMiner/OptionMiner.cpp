// OptionMiner.cpp : Defines the entry point for the console application.
#include "StdAfx.h"
#include "MarketMinerLogin.h"					// defines: mmUser, mmPass, mmDbms
#include "OptionsDatabase.h"
#include "ParseSupport.h"						// strtolower()
#include "utils.h"

//#include <stdlib.h>							// srand()
using namespace std;
const UINT	keySize = 4;
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int	_tmain( int argc, TCHAR* argv[] )
{	mystring	args;
	char		*cboe = NULL, *yhsp = NULL;
		// ensure that there's an input file to read
	if ( argc < 2 )
	{	fprintf( stderr, "Usage: %s {yhsp=<Yahoo historical stock prices file>} {cboe=<option prices file>}", *argv );
		return	-1;
	}
		// 1. Make a connection with postgreSQL
	OptionsDatabase	db;
	int retVal = db.InstallConnect( mmUser, mmPass, mmDbms );		// installs user & dbms, but no tables
	if ( retVal < 0 )
		goto	EXIT;

		// 2. Create tables for stocks & options
	db.DefineTables();

		// concatenate the argument list
	CatArgs( argc, argv, args );									// reconstructs the command line (as one string) in args

		// 3. load Yahoo historical stock prices
	int	nFiles = GetFnames( args, yhsp, cboe );
/*
	if ( yhsp )
	{	db.StuffYahooStockPrices( yhsp );							// looks good in first pass at INTC
		delete [] yhsp;
		yhsp = NULL;
	}
*/
		// 4. load CBOE option prices file
	if ( cboe )
	{	db.StuffCBOEoptionsPrices( cboe );
		delete [] cboe;
		cboe = NULL;
	}
EXIT:
	if ( yhsp )
		delete [] yhsp;
	if ( cboe )
		delete [] cboe;
	return	retVal;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
