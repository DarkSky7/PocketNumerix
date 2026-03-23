// Profile.h

#include <string>

// PROFILE entries (one per line):
// UnderlyingSymbol 							// all combinations, including calendar w/o date constraints
// UnderlyingSymbol & d1 d2 					// all combinations, including calendar w/date constraints
// UnderlyingSymbol * MarketOutlook 			// all spreads w/in specified market outlook
// UnderlyingSymbol * MarketOutlook & d1 d2		// all spreads w/in specified market outlook w/date constraints
// UnderlyingSymbol # SpreadName 				// specific spread with date constraints
// UnderlyingSymbol # SpreadName & d1 d2		// specific spread with date constraints w/date constraints
//	 '&' precedes a date pair constraint
//	 '#' precedes a spread name or code
//	 '*' precedes a market outlook name, signifying all spreads in that market outlook
// Threshold XX.X%								// a float
// 
// Dates specified in number of days from present

/* example profile
AMAT
GD  * bear
IBM * neutral
NYT * volatile
XOM * bull
*/

class Profile
{	string	stkSym;
	string	mktOutlook;					// 
	short	minExpiry;					// in days from present
	short	maxExpiry;					// in days from present

}