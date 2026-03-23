// NumEdit.cpp : implementation file
//
#include "stdafx.h"
#include "resource.h"
#include "NumEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumEdit

CNumEdit::CNumEdit()
{		// double has a 52 bit mantissa
		// the longest decimal would have 16 digits
		// plus one for '.' and one for '-' means 18 is the longest
//	SetLimitText( 18 ); 
}

CNumEdit::~CNumEdit()
{
}

BEGIN_MESSAGE_MAP(CNumEdit, CEdit)
	//{{AFX_MSG_MAP(CNumEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNumEdit message handlers

void	CNumEdit::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{		// filter valid characters
		// would like to verify that there's only one dot, one dash
		// and that the string is well formed to parse numerically, but
		// this will have to do for now.
	bool	dotDash   = ( nChar == _T('.')  ||  nChar == _T('-') );
	bool	isNumeric = ( nChar >= _T('0')  &&  nChar <= _T('9') );
	bool	isEdit	  = ( nChar == VK_BACK  ||  nChar == VK_DELETE );
		// accept dot, dash, numeric, and backspace characters
	if ( dotDash  ||  isNumeric  ||  isEdit )
		CEdit::OnChar( nChar, nRepCnt, nFlags );
}			// OnChar()
//---------------------------------------------------------------------------
