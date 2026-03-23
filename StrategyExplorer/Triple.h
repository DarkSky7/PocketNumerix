#pragma once

template <typename Qty_T, typename BuySell_T, typename Cost_T>
class Triple
{
public:
	Triple( void )
	{}
	Triple( Qty_T qt, BuySell_T bs, Cost_T cst )
		: qty(qt), buySell(bs), cost(cst)
	{}
	Triple( Triple& trp )
		: qty(trp.qty), buySell(trp.buySell), cost(trp.cost)
	{}
	virtual ~Triple( void )
	{}

//	Qty_T		GetQty( void )				{	return	qty;		}
//	BuySell_T	GetBuySell( void )			{	return	buySell;	}
//	Cost_T		GetCost( void )				{	return	cost;		}

//	void		SetQty( Qty_T qt )			{	qty = qt;			}
//	void		SetBuySell( BuySell_T bs )	{	buySell = bs;		}
//	void		SetCost( Cost_T cst )		{	cost = cst;			}

	Qty_T		qty;
	BuySell_T	buySell;
	Cost_T		cost;
};
