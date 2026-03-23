// RGMpricer.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#define DEBUG		1
#define LINUX		0
#if LINUX
#define  _TCHAR		char
#define	 _tmain		main
#include <stdlib.h>
#endif

#include <string.h>
#include <iostream>
#include <iomanip>
#include <ostream>
#include <functional>						// defines:  std::less
#include <utility>							// make_pair
#include <map>
#include <set>
#include "mrl_pair.h"
#include "MyTokens.h"
#include "Position.h"
#include "Triple.h"
#include "AvlTree.h"

using namespace std;

const unsigned int MAX_LINE_LENGTH = 255;
const char EOS = '\0';
const char SPACE = ' ';
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void	EatSeparators( char*& lp, char separator )
{
	while ( *lp == separator  &&  *lp != EOS )
		*lp++;
}			// EatSeparators()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
char*	GetToken( char*& lp, char term )
{	// PURPOSE: eat leading terminators, scan for next one,
	//			allocate new char*, fill & return it
	// SIDE EFFECT: *lp advances
	// Allocating tokens on heap is not as efficient as stuffing a stack
	// based token, but then we'd have to check size, take action, ...
		// eat leading terminators (spaces?)
	EatSeparators( lp, term );
	if ( *lp == EOS )
		return	NULL;

		// found a non-term character, look for end of token
	char *beg = lp;
	while ( *lp != term  && *lp != EOS )
		lp++;

		// allocate a new char*, fill it, and return it
	long	cnt = (long)(lp - beg);
	char	*ret = new char[ cnt+1 ];
	strncpy( ret, beg, cnt );
	*(ret+cnt) = EOS;
	return	ret;
}			// GetToken()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef MyTokens<char*,char,char*,char,char*,char*>		TokenSet_T;
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TokenSet_T*		ParseLine( char* lpBeg )
{	char	*lp = lpBeg,			// for local manipulation
			*ts = NULL,
			ot,
			*oid = NULL,
			sid = '?',
			*prSt = NULL,
			*szSt = NULL;

		// get local timeStamp 
	if ( (ts = GetToken( lp, SPACE )) == NULL )
	{	std::cerr << "No timestamp in: '" << lpBeg << "'" << std::endl;
		goto	ErrorExit;					// no timeStamp in *lp
	}

		// get local orderType
	EatSeparators( lp, SPACE );
	if ( *lp == EOS )
	{	std::cerr << "No order type found in '" << lpBeg << "'" << std::endl;
		goto	ErrorExit;					// short line exception
	}
	ot = *lp++;
	if ( ot != 'A'  &&  ot != 'R' )
	{	std::cerr << "Unsupported order type: '" << lpBeg << "'" << std::endl;
		goto	ErrorExit;					// unsupported orderType
	}

		// get local orderId
	if ( (oid = GetToken( lp, SPACE )) == NULL )
	{	std::cerr << "No order-id found in: '" << lpBeg << "'" << std::endl;
		goto	ErrorExit;
	}

		// get local side, price
	if ( ot == 'A' )
	{		// get local side
		EatSeparators( lp, SPACE );
		if ( *lp == EOS )
		{	std::cerr << "No side type found in '" << lpBeg << "'" << std::endl;
			goto	ErrorExit;					// short line exception
		}
		sid = *lp++;
		if ( sid != 'B'  &&  sid != 'S' )
		{	std::cerr << "Unsupported side type: '" << lpBeg << "'" << std::endl;
			goto	ErrorExit;					// unsupported side type
		}

			// get local price
		if ( (prSt = GetToken( lp, SPACE )) == NULL )
		{	std::cerr << "No price found in '" << lpBeg << "'" << std::endl;
			goto	ErrorExit;					// short line exception
		}
	}

		// get local size
	if ( (szSt = GetToken( lp, SPACE )) == NULL )
	{	std::cerr << "No size found in: '" << lpBeg << "'" << std::endl;
		goto	ErrorExit;
	}

		// set arguments for success
	return	new TokenSet_T( ts, ot, oid, sid, prSt, szSt );

ErrorExit:
		// error cleanup
	if ( ts )	delete [] ts;
	if ( oid )	delete [] oid;
	if ( prSt )	delete [] prSt;
	if ( szSt )	delete [] szSt;
	return	NULL;
}			// ParseLine()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void	EchoLine( TokenSet_T* tks, bool toCerr = true )
{
	if ( !tks )
		throw "EchoLine: Bad token set";
	if ( toCerr )
	{	std::cerr << tks->timeStamp << " " << tks->orderType << " " << tks->GetOrderId();
		if ( tks->side != '?' )
			std::cerr << " " << tks->side;
		if ( tks->priceSt )
			std::cerr << " " << tks->priceSt;
		if ( tks->sizeSt )
			std::cerr << " " << tks->sizeSt;		// w/o an EOL
	}
	else
	{	std::cout << tks->timeStamp << " " << tks->orderType << " " << tks->GetOrderId() << " " << tks->side;
		if ( tks->side != '?' )
			std::cout << " " << tks->side;
		if ( tks->priceSt )
			std::cout << " " << tks->priceSt;
		if ( tks->sizeSt )
			std::cout << " " << tks->sizeSt;		// w/o an EOL
	}
}				// EchoLine()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	const char*		OrderId_T;
typedef	float			Price_T;
typedef	unsigned int	Qty_T;			// a size type
typedef char			Side_T;
typedef unsigned long	Total_T;		// a size type at least as capacious as Qty_T

struct ltOID
{	bool operator()(OrderId_T s1, OrderId_T s2) const
	{	return	strcmp(s1, s2) < 0;
	}
};
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	// handy typedefs
typedef Triple<char,Price_T,Qty_T>						OrderDetails_T;					// everything but the orderId
typedef	mrl_pair<Price_T,Qty_T>							PriceQtyPair_T;
typedef map<OrderId_T,OrderDetails_T*,ltOID>			MapOrderId2OrderDetails_T;		// Map(OrderId) -> (Side,Price,Qty) triple
typedef	set<OrderId_T,ltOID>							OrderSet_T;						// Collection of Orders (at some price)
typedef	Position<Price_T,Qty_T,OrderSet_T*>									PriceQtyOrderSet_T;
typedef	AvlTree<PriceQtyOrderSet_T,Price_T,std::less<PriceQtyOrderSet_T> >	AvlPQOS_T;
typedef map<Price_T,OrderSet_T*>						AvlRepairSet_T;
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void	DumpTree( AvlPQOS_T* node, short deep = 0 )
{
	PriceQtyOrderSet_T* pqos = node->GetPayload();
	if ( !pqos )
	{	std::cerr << "Empty tree" << std::endl;
		return;
	}
	OrderSet_T*	os = pqos->GetOrders();
	if ( !os )
		throw "Dumptree AvlPQOS node has no orderset";
	std::cerr << pqos->GetKey() << " " << node->GetHeight() << " " << node->GetBalance() << " ";

	OrderSet_T::iterator osit = os->begin();
	if ( osit != os->end() )
	{	while ( osit != os->end() )
		{	std::cerr << *osit++;
			if ( osit != os->end() )
				std::cerr << ",";
		}
	}
	std::cerr << std::endl;

	AvlPQOS_T* left = node->GetLeft();
	if ( left )
	{	for ( short ii = 0; ii < deep; ii++ )
			std::cerr << " ";
		std::cerr << " L: "; 
		DumpTree( left, deep+1 );
	}
	AvlPQOS_T* right = node->GetRight();
	if ( right )
	{	for ( short ii = 0; ii < deep; ii++ )
			std::cerr << " ";
		std::cerr << " R: ";
		DumpTree( right, deep+1 );
	}
}		// DumpTree()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void	OrdersCleanup(
	MapOrderId2OrderDetails_T&	ordersMap,
	AvlPQOS_T*&					ordersByPrice,
	AvlRepairSet_T&				repairSet )
{	AvlPQOS_T*					aNode;
		// traversal complete, okay to get deadwood out of the AVL tree
	AvlRepairSet_T::iterator rsi = repairSet.begin();
	while ( rsi != repairSet.end() )
	{	Price_T	nodePrice = rsi->first;
		ordersByPrice->Find( nodePrice, aNode );
		OrderSet_T* aNode_orders = aNode->GetPayload()->GetOrders();
		OrderSet_T* rsi_os = rsi->second;
		OrderSet_T::iterator rsi_osit = rsi_os->begin();
		while ( rsi_osit != rsi_os->end() )
		{	ordersMap.erase( *rsi_osit );					// remove this order from the orders map
			aNode_orders->erase( *rsi_osit++ );			// remove this order from the orders at price AVL node
		}
		if ( aNode_orders->empty() )
			aNode->Delete( nodePrice, ordersByPrice );
		rsi++;
	}
	repairSet.clear();
}			// OrdersCleanup()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void		AddOrder(
	MapOrderId2OrderDetails_T&	ordersMap,
	AvlPQOS_T*&					ordersByPrice,
	Total_T&					sideTotalShares,
	TokenSet_T*					tks					)
{	OrderDetails_T*				orderDetails;
	Qty_T		size = (Qty_T)atoi( tks->sizeSt );
	Price_T		price;
#if DEBUG
	Qty_T		b4Qty = 0;
#endif
		// do we have an entry in the ordersMap?
	MapOrderId2OrderDetails_T::iterator		mapit = ordersMap.find( tks->GetOrderId() );
	if ( mapit != ordersMap.end() )
	{		// existing order
		orderDetails = mapit->second;
#if DEBUG
		b4Qty = orderDetails->GetQty();
#endif
		orderDetails->AddQty( size );
		price = orderDetails->GetPrice();
	}
	else	// no existing order
	{	price = (Price_T)atof( tks->priceSt );
		orderDetails = new OrderDetails_T( tks->side, price, size );
		ordersMap.insert( make_pair( tks->UseOrderId(), orderDetails ) );	// map(orderId) -> (side,price,size)
	}
	sideTotalShares += size;					// update total side shares

#if DEBUG
	EchoLine( tks );
	std::cerr << (mapit != ordersMap.end() ? " : Add to existing "  :  " : New ");
	std::cerr << (tks->side == 'B' ? "bid" : "ask");
	if ( mapit != ordersMap.end() )
		std::cerr << ", b4Qty=" << b4Qty;
	std::cerr << ", curQty=" << orderDetails->GetQty();
	std::cerr << ", " << (tks->side == 'B' ? "Buy" : "Sell");
	std::cerr << " side total=" << sideTotalShares << std::endl;
#endif

		// update {shares available & orderSet} at this price (on this side)
	PriceQtyOrderSet_T*		pqos;
	AvlPQOS_T*				aNode;
	if ( ordersByPrice->Find( price, aNode ) )
	{		// add to existing node
		pqos = aNode->GetPayload();
		if ( !pqos )
			throw "found an AVL node with no payload";
		OrderSet_T* nodeOrderSet = pqos->GetOrders();
		if ( !nodeOrderSet )
			throw "NULL nodeOrderSet encountered";
		OrderSet_T::iterator osit = nodeOrderSet->find( tks->GetOrderId() );
		if ( osit == nodeOrderSet->end() )
			nodeOrderSet->insert( tks->UseOrderId() );
		pqos->AddQty( size );				// shares available at price (on this side)
	}
	else	// no price totals node in the AVL tree - create one
	{	if ( mapit != ordersMap.end() )		// existing order
			throw	"Add to existing order with no AVL node at price";
		OrderSet_T* nodeOrderSet = new OrderSet_T;
		nodeOrderSet->insert( tks->UseOrderId() );
		pqos = new PriceQtyOrderSet_T( price, size, nodeOrderSet );
		ordersByPrice->Insert( pqos, price, ordersByPrice );
	}
}			// AddOrder()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// the largest chunk of code that can (symmetrically) be pulled out of the main() 'R'educe handler
void		ReduceOrder(
	MapOrderId2OrderDetails_T&	ordersMap,
	AvlPQOS_T*&					ordersByPrice,
	Total_T&					sideTotalShares,
	TokenSet_T*					tks,
	Side_T						side,
	MapOrderId2OrderDetails_T::iterator& mapit,
	OrderDetails_T*				orderDetails		)
{	AvlPQOS_T*					aNode;
	PriceQtyOrderSet_T*			pqos;
	Qty_T	size = (Qty_T)atoi( tks->sizeSt );
	Price_T	price = orderDetails->GetPrice();
#if DEBUG
	Qty_T	b4Qty = orderDetails->GetQty();
#endif
	Qty_T	decr = size, remainQty = orderDetails->ReduceQty( decr );
	bool deadOrder = remainQty <= 0;
	if ( deadOrder )
		ordersMap.erase( mapit );					// nothing left, drop the order
	sideTotalShares -= decr;

#if DEBUG
	EchoLine( tks );
	std::cerr << " : Reduce " << (side == 'B' ? "bid" : "ask");
	std::cerr << ", b4qty=" << b4Qty;
	std::cerr << ", curQty=" << remainQty;
	std::cerr << ", " << (side == 'B' ? "Buy" : "Sell");
	std::cerr << " side total=" << sideTotalShares << std::endl;
#endif

		// update {shares available & orderSet} at this price (on this side)
	if ( ordersByPrice->Find( price, aNode ) )
	{	pqos = aNode->GetPayload();
		OrderSet_T* nodeOrderSet = pqos->GetOrders();
		if ( !nodeOrderSet )
			throw "NULL OrderSet encountered";
		Qty_T remainQtyAtPrice = pqos->ReduceQty( decr );
		if ( deadOrder )
			nodeOrderSet->erase( tks->GetOrderId() );
		bool noMoreOrdersAtPrice = nodeOrderSet->size() < 1;
		bool noMoreSharesAtPrice = remainQtyAtPrice <= 0;
		if ( noMoreSharesAtPrice  &&  noMoreOrdersAtPrice )
			aNode->Delete( price, ordersByPrice );
		else if ( noMoreOrdersAtPrice != noMoreSharesAtPrice )
			throw "noMoreOrders disagrees with noMoreShares";
	}
	else
		throw	"Reduce order found but no AVL node at price";
}			// ReduceOrder()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int		TakeAction(
	MapOrderId2OrderDetails_T&	ordersMap,
	AvlPQOS_T*&					ordersByPrice,
	Total_T&					sideTotalShares,
	TokenSet_T*					tks,
	Side_T						side,
	bool						wantNAs				)
{	MapOrderId2OrderDetails_T::iterator  mapit;
	AvlRepairSet_T				nodesToRepair;
	int							nOrdersHit = 0;
	AvlPQOS_T*					aNode;
	extern	Total_T				targetSize;
	Total_T						sharesToGo = targetSize;

		// could have an empty ordersByPrice tree
	if ( !ordersByPrice->IsEmpty()  &&  sideTotalShares >= targetSize )
	{		// assumptions:  (side == 'B'  ||  side == 'S')
		double	aggDollars = 0.0;
			//	buy low from Sell side (start at minNode) : sell high to Buy side (start at maxNode)
		aNode =	side == 'S'  ?  ordersByPrice->MinNode()  :  ordersByPrice->MaxNode();
		while ( aNode )
		{	PriceQtyOrderSet_T*		pqos = aNode->GetPayload();
			if ( !pqos )
				throw "NULL payload encountered";
			Price_T		nodePrice = pqos->GetKey();
			OrderSet_T*			nodeOrderSet = pqos->GetOrders();
			if ( !nodeOrderSet )
				throw "NULL OrderSet encountered";
			OrderSet_T::iterator	osIter = nodeOrderSet->begin();
			while ( osIter != nodeOrderSet->end() )
			{	OrderId_T	oid = *osIter++;
				mapit = ordersMap.find( oid );						// reuse mapit: orderId --> (side,price,size)
				OrderDetails_T*	orderDetails = mapit->second;
					// grab shares available, reduce side & node totals
				Qty_T	decr = sharesToGo, remainQty = orderDetails->ReduceQty( decr );		// hit the bid/ask
				Total_T remainQtyAtPrice = pqos->ReduceQty( decr );		// reduce shares available @ price (on this side)
				aggDollars += decr * nodePrice;							// income(expense) for this sale(buy)
				sideTotalShares -= decr;								// side total
				sharesToGo -= decr;										// loop sentinal
				nOrdersHit++;
#if DEBUG
				std::cerr << "Hit " << (side == 'B' ? "bid '" : "ask '");
				std::cerr << oid << "' for " << decr << " * ";
				std::cerr << nodePrice << " = " << (decr*nodePrice);
				std::cerr << "; " << remainQty << " shares left" << std::endl;
#endif
				if ( remainQty <= 0 )
				{		// AVL tree and orders map ToDo list (to address after the traversal)
					AvlRepairSet_T::iterator arsi = nodesToRepair.find( nodePrice );
					if ( arsi == nodesToRepair.end() )
					{	OrderSet_T*	os = new OrderSet_T;
						os->insert( oid );
						nodesToRepair.insert( make_pair(nodePrice,os) );	
					}
					else
						arsi->second->insert( oid );
#if DEBUG
					std::cerr << "'" << oid << "', " << nodePrice;
					std::cerr << " scheduled for repair" << std::endl;
#endif
				}
				if ( sharesToGo <= 0 )
				{	std::cout << tks->timeStamp << (side == 'S' ? " B " : " S ") << aggDollars << std::endl;
#if DEBUG
					std::cerr << "cout: " << tks->timeStamp << (side == 'S' ? " B " : " S ") << aggDollars << std::endl;
#endif
					goto	LoopDeLoopExit;
				}
			}			// while ( osIter != nodeOrderSet->end() )
				// buy low <-- Sell side (traverse higher) : sell high --> Buy side (traverse lower)
			aNode =		    side == 'S'  ?  aNode->Succ()  :  aNode->Pred();
		}			// while ( aNode )
	}			// 	if ( !ordersByPrice->IsEmpty()  &&  sideTotalShares >= targetSize )
		// the second reason for being called - possible NA output
LoopDeLoopExit:
	if ( sharesToGo > 0  &&  wantNAs )
	{	std::cout << tks->timeStamp << (side == 'S' ? " B" : " S") << " NA" << std::endl;
#if DEBUG
		std::cerr << "cout: " << tks->timeStamp << (side == 'S' ? " B" : " S") << " NA" << std::endl;
#endif
	}
		// get the deadwood out of the main orders map and the orders-by-price AvlTree we're using
	OrdersCleanup( ordersMap, ordersByPrice, nodesToRepair );
	return	nOrdersHit;
}			// TakeAction()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// globals
Total_T		targetSize;
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int _tmain( int argc, _TCHAR* argv[] )
{	char	line[ MAX_LINE_LENGTH + 1 ];
	if ( argc < 2 )
		std::cerr << "Usage: " << *argv << " target-size (an unsigned integer), e.g.: " << *argv << " 200" << std::endl;
#if LINUX
	targetSize = atoi( *(argv+1) );	
#else
	targetSize = _wtoi( *(argv+1) );	
#endif
	if ( targetSize <= 0 )
	{	std::cerr << "Target-size parameter must be positive." << std::endl;
		exit( -1 );
	}
	MapOrderId2OrderDetails_T	orders;				// Map(orderId) -> (price,qty)
	AvlPQOS_T		*buyOrders = new AvlPQOS_T,
					*sellOrders = new AvlPQOS_T,
					**ordersByPrice;				// INDEX(price), total shares @ price, orderSet @ price)
	Total_T			buySideTotalShares = 0, sellSideTotalShares = 0, *sideTotalShares;
	cout.setf( ios::fixed, ios::floatfield );		cerr.setf( ios::fixed, ios::floatfield );
	cout.setf( ios::showpoint );					cerr.setf( ios::showpoint );
	std::cout << setprecision( 2 );						std::cerr << setprecision( 2 );
	bool	wantSellNAs = false, wantBuyNAs = false;
	while ( cin.good() )
	{	char*	lp = line;
		cin.getline( lp, MAX_LINE_LENGTH );
		TokenSet_T*	tks;
		if ( (tks = ParseLine( lp )) )
		{		// have tokens from a line of input
			Side_T	side = '?';					// to be overwritten in the 'A'dd and 'R'educe handlers
			if ( tks->orderType == 'A' )									// 'A'dd Order
			{	if ( tks->side != 'B'  &&  tks->side != 'S' )
					throw "Bad side here?";
				side = tks->side;				// tks->side has already been filtered to 'B' or 'S'
				sideTotalShares = side == 'B'  ?  &buySideTotalShares  :  &sellSideTotalShares;
				ordersByPrice	= side == 'B'  ?		   &buyOrders  :  &sellOrders;
				AddOrder( orders, *ordersByPrice, *sideTotalShares, tks );
			}
			else										 					// 'R'educe Order
			{		// side is unavailable in the input
				MapOrderId2OrderDetails_T::iterator		mapit = orders.find( tks->GetOrderId() );
				if ( mapit != orders.end() )
				{	OrderDetails_T*	orderDetails = mapit->second;
					if ( !orderDetails )
						throw "Order with no orderDetails encountered";
					side = orderDetails->GetQualifier();					// side on a pre-existing order
					if ( side != 'B'  &&  side != 'S' )
						throw "Bad side retrived from orders map";			// very strange
					sideTotalShares = side == 'B'  ?  &buySideTotalShares  :  &sellSideTotalShares;
					ordersByPrice	= side == 'B'  ?		   &buyOrders  :  &sellOrders;
						// a function call here: 1) to better expose main flow of execution
						//					 and 2) for symmetry w/ AddOrder() above
						//							 (which obviously has more reasonable coupling w/main)
					ReduceOrder( orders, *ordersByPrice, *sideTotalShares, tks, side, mapit, orderDetails );
				}
				else		// 'R'educe order without a matching orderId in the orders map
				{		// side stays with '?', so cout will NOT get a NA message (no side information available)
					EchoLine( tks );
					std::cerr << " : No existing orderId '" << tks->GetOrderId() << "'; skipped" << std::endl;
				}
			}
				// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
				// can we take action on this side?
			if ( side == 'B'  ||  side == 'S' )								// side could be '?'
			{	bool	wantNAs = side == 'B'  ?  wantSellNAs  :  wantBuyNAs;
					// note that sideTotalShares & ordersByPrice are already pointing at the right addresses
				int  nOrdersHit = TakeAction( orders, *ordersByPrice, *sideTotalShares, tks, side, wantNAs );
				if ( nOrdersHit > 0 )
				{	
#if DEBUG
					if ( !wantBuyNAs  &&  side == 'S' )
						std::cerr << tks->timeStamp << " : enabled buyNAs" << std::endl;
					if ( !wantSellNAs  &&  side == 'B' )
						std::cerr << tks->timeStamp << " : enabled sellNAs" << std::endl;
#endif
					wantBuyNAs  = wantBuyNAs  || side == 'S';
					wantSellNAs = wantSellNAs || side == 'B';
				}
			}
				// always kill timeStamp, priceSt, sizeSt, conditionally kill orderId
			if ( tks )
				delete	tks;
		}			// if ( ParseLine( lp, timeStamp, orderType, orderId, side, priceSt, sizeSt ) )
#if DEBUG
		std::cerr << std::endl;					// for readability in the error file during DEBUG
#endif
	}			// while ( infile.good() )

		// cleanup
	if ( sellOrders )	delete	sellOrders;
	if ( buyOrders )	delete	buyOrders;
	return	0;
}			// _tmain()
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
