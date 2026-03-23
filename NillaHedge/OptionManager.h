// OptionManager.h: interface for the COptionManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPTIONMANAGER_H__AE7EA9FC_4B8A_4B31_AAA7_53C2FF65D367__INCLUDED_)
#define AFX_OPTIONMANAGER_H__AE7EA9FC_4B8A_4B31_AAA7_53C2FF65D367__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataManager.h"
class	COption;

class COptionManager : public CDataManager
{
public:
	COptionManager( Cstring portfolioDir );
	virtual ~COptionManager();

	COption*	GetOption( Cstring symbol );									// NULL := failure
	COption*	ReadOption( long offset );										// NULL := failure
	short		WriteOption( COption* opt );									// 0 := success
	COption*	GetPosIndexedOption( long zbi );								// NULL := failure
	COption*	GetStkIndexedOption( Cstring stkSym, long zbi );				// NULL := failure
	COption*	GetStkIndexedOption( CDBFile* stkOptIndxFile, long zbi );		// NULL := failure
};
#endif // !defined(AFX_OPTIONMANAGER_H__AE7EA9FC_4B8A_4B31_AAA7_53C2FF65D367__INCLUDED_)
