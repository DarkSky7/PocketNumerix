// BondManager.h: interface for the CBondManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BONDMANAGER_H__6C8B9ACB_2FCD_4F00_B9B7_36DC40AFCB8B__INCLUDED_)
#define AFX_BONDMANAGER_H__6C8B9ACB_2FCD_4F00_B9B7_36DC40AFCB8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataManager.h"
class	CBond;

class CBondManager : public CDataManager
{
public:
	CBondManager( CString portfolioDir );
	virtual ~CBondManager();

	CBond*		GetBond( CString symbol );					// NULL := failure
	CBond*		GetIndexedBond( long zbi );					// NULL := failure
	CBond*		ReadBond( long offset );					// NULL := failure
	short		WriteBond( CBond* bnd );					// 0 := success

private:
	NOTIFYICONDATA	nid;
};
#endif // !defined(AFX_BONDMANAGER_H__6C8B9ACB_2FCD_4F00_B9B7_36DC40AFCB8B__INCLUDED_)
