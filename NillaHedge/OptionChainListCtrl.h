// OptionChainListCtrl.h: interface for the COptionChainListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPTIONCHAINLISTCTRL_H__FC02B6BC_050E_45D6_B801_4C36B2C05A76__INCLUDED_)
#define AFX_OPTIONCHAINLISTCTRL_H__FC02B6BC_050E_45D6_B801_4C36B2C05A76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OptionChainCommon.h"
#include "RedBlackListCtrl.h"

class COptionChainListCtrl : public CRedBlackListCtrl
{
	DECLARE_DYNAMIC( COptionChainListCtrl )

public:
	COptionChainListCtrl();
	virtual ~COptionChainListCtrl();
	int		AddItem( LPARAM pItem, int iItemNo );
    void	OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult );

	// Generated message map functions
protected:

	//{{AFX_MSG(COptionChainListCtrl)
//	afx_msg void	OnClick( NMHDR* pNMHDR, LRESULT* pResult );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_OPTIONCHAINLISTCTRL_H__FC02B6BC_050E_45D6_B801_4C36B2C05A76__INCLUDED_)

//	int		AddItem( OptionChainItemInfo* ocii );	// stores ocii in the OptionChainListCtrl
//	int		InsertItem( OptionChainItemInfo* plii, int iItem );	// displays plii in the ListCtrl
