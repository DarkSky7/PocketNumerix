// StockManager.h: interface for the CStockManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STOCKMANAGER_H__97E87BE0_70FA_4CD4_8FF5_5C7AAB11A018__INCLUDED_)
#define AFX_STOCKMANAGER_H__97E87BE0_70FA_4CD4_8FF5_5C7AAB11A018__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataManager.h"
#include "Stock.h"

class CStockManager : public CDataManager
{
public:
	CStockManager( CString portfolioDir );
	virtual ~CStockManager();

	CStock*		GetStock( CString symbol );					// NULL := failure
	CStock*		ReadStock( long offset );					// NULL := failure
	short		WriteStock( CStock* stk );					// 0 := success
	CStock*		GetPosIndexedStock( long zbi );				// NULL := failure

};

#endif // !defined(AFX_STOCKMANAGER_H__97E87BE0_70FA_4CD4_8FF5_5C7AAB11A018__INCLUDED_)
