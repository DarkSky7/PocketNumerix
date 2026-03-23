// PlotOptionsDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "PlotOptionsDialog.h"
#include "DataManager.h"
//#include "OptionManager.h"
//#include "Option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlotOptionsDialog dialog

CPlotOptionsDialog::CPlotOptionsDialog( int IDD, CWnd* pParent /*=NULL*/ )
	: CPlotDialog( IDD, pParent)
{
	//{{AFX_DATA_INIT(CPlotOptionsDialog)
	m_Symbol1 = _T("");
	m_Symbol2 = _T("");
	m_Symbol3 = _T("");
	m_Symbol4 = _T("");
	//}}AFX_DATA_INIT

	for ( short jj = 0; jj < nBOS; jj++ )
		*(pBOS+jj) = NULL;
}
//----------------------------------------------------------------------------
CPlotOptionsDialog::~CPlotOptionsDialog( void )
{
	DeleteBOSs();
}
//----------------------------------------------------------------------------
void	CPlotOptionsDialog::DeleteBOSs( void )
{
	for ( short ii = 0; ii < nBOS; ii++ )
	{	if ( *(pBOS+ii) )
		{	delete	*(pBOS+ii);
			*(pBOS+ii) = NULL;
		}
	}
}			// DeleteBOSs()
//----------------------------------------------------------------------------
void CPlotOptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CPlotDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlotOptionsDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPlotOptionsDialog, CPlotDialog)
	//{{AFX_MSG_MAP(CPlotOptionsDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlotOptionsDialog message handlers

void	CPlotOptionsDialog::LoadSymbolsCore( CMapStringToPtr* theMap )
{		// unload any existing entries in c_Symbol
	c_Symbol1.Clear();
	c_Symbol2.Clear();
	c_Symbol3.Clear();
	c_Symbol4.Clear();

	int	nItems = 0;								// number of symbols in the symbol table
	if ( theMap )
	{	nItems = theMap->GetCount();			// get the real nItems count
		if ( nItems > 0 )
		{	c_Symbol1.LoadMap( theMap );
			c_Symbol2.LoadMap( theMap );
			c_Symbol3.LoadMap( theMap );
			c_Symbol4.LoadMap( theMap );
		}
	}
		// disable empty ComboBoxes
	c_Symbol1.EnableWindow( nItems > 0 );
	c_Symbol2.EnableWindow( nItems > 0 );
	c_Symbol3.EnableWindow( nItems > 0 );
	c_Symbol4.EnableWindow( nItems > 0 );

		// display symbols for the currently 'selected' BOSs
		// obviously LoadBOSs should ba called before LoadSymbols
	CString	sym;
	if ( *(pBOS+0) )
	{	sym = (*(pBOS+0))->getSymbol();
		int	idx = c_Symbol1.FindStringExact( 0, sym );
		if ( idx != CB_ERR )
			c_Symbol1.SetCurSel( idx );
	}
	if ( *(pBOS+1) )
	{	sym = (*(pBOS+1))->getSymbol();
		int	idx = c_Symbol2.FindStringExact( 0, sym );
		if ( idx != CB_ERR )
			c_Symbol2.SetCurSel( idx );
	}
	if ( *(pBOS+2) )
	{	sym = (*(pBOS+2))->getSymbol();
		int	idx = c_Symbol3.FindStringExact( 0, sym );
		if ( idx != CB_ERR )
			c_Symbol3.SetCurSel( idx );
	}
	if ( *(pBOS+3) )
	{	sym = (*(pBOS+3))->getSymbol();
		int	idx = c_Symbol4.FindStringExact( 0, sym );
		if ( idx != CB_ERR )
			c_Symbol4.SetCurSel( idx );
	}
}			// LoadSymbolsCore()
//----------------------------------------------------------------------------
void	CPlotOptionsDialog::SetCheckButtons( bool* chkBtns ) 
{
	c_Check1.SetCheck( *(chkBtns+0)  ?  BST_CHECKED  :  BST_UNCHECKED );
	c_Check2.SetCheck( *(chkBtns+1)  ?  BST_CHECKED  :  BST_UNCHECKED );
	c_Check3.SetCheck( *(chkBtns+2)  ?  BST_CHECKED  :  BST_UNCHECKED );
	c_Check4.SetCheck( *(chkBtns+3)  ?  BST_CHECKED  :  BST_UNCHECKED );
}			// SetCheckButtons()
//----------------------------------------------------------------------------
/*
void	CPlotOptionsDialog::KillFocusSymbolWork( short ii, CString sym, long* def_ID )
{		// ii is the zero-based index of the option

		// kill any Option at this index
	if ( *(pBOS+ii) )
		delete	*(pBOS+ii);

	*(pBOS+ii) = theApp.optMgr->GetOption( sym );
		
		// update the dialog's persistent state
	*(def_ID+ii) = *(pBOS+ii) == NULL  ?  -1  :  (*(pBOS+ii))->getDef_ID();

		// if the check button is checked, a repaint is called for
	if (	( ii == 0  &&  c_Check1.GetCheck() == BST_CHECKED )
		 || ( ii == 1  &&  c_Check2.GetCheck() == BST_CHECKED )
		 || ( ii == 2  &&  c_Check3.GetCheck() == BST_CHECKED )
		 || ( ii == 3  &&  c_Check4.GetCheck() == BST_CHECKED ) )
		Plot();
}			// KillFocusSymbolWork()
*/
//----------------------------------------------------------------------------
