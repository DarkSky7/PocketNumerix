#pragma once
#include "Triple.h"

template <typename Qty_T, typename BuySell_T, typename Cost_T, typename PutCall_T, typename Strike_T>
class Quint : public Triple<Qty_T,BuySell_T,Cost_T>
{
public:
	Quint( void )
	{}
	Quint( Qty_T qt, BuySell_T bs, Cost_T cst, PutCall_T pc, Strike_T strk )
		: Triple(qt, bs, cst), putCall(pc), strike(strk)
	{}
	Quint( Quint& q5 )
		: qty(q5.qty), buySell(q5.buySell), cost(q5.cost), putCall(q5.putCall), strike(q5.strike)
	{}
	virtual ~Quint( void )
	{}

//	PutCall_T	GetPutCall( void )			{	return	putCall;	}
//	Strike_T	GetStrike( void )			{	return	strike;		}

//	void		SetPutCall( PutCall_T pc )	{	putCall= pc;		}
//	void		SetStrike( Strike_T strk )	{	strike = strk;		}

	PutCall_T	putCall;
	Strike_T	strike;
};
