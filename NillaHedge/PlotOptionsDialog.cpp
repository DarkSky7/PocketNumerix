// PlotOptionsDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "PlotOptionsDialog.h"
#include "DataManager.h"
#include "OptionManager.h"
#include "Option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPlotOptionsDialog dialog

CPlotOptionsDialog::CPlotOptionsDialog( int IDD, CWnd* pParent /*=NULL*/ )
	: CPlotDialog( IDD, pParent)
	, m_activeComboBox( NULL )
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

CPlotOptionsDialog::~CPlotOptionsDialog( void )
{
	DeleteBOSs();
}

void CPlotOptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CPlotDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlotOptionsDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPlotOptionsDialog, CPlotDialog)
	//{{AFX_MSG_MAP(CPlotOptionsDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlotOptionsDialog message handlers

/*
BOOL CPlotOptionsDialog::OnInitDialog() 
{
	CPlotDialog::OnInitDialog();
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
*/
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
void	CPlotOptionsDialog::FillSlots( long* def_ID ) 
{
	POSITION pos = theApp.optMgr->GetDefIterCtx();
	while ( pos )
	{	short	firstSlot = -1;
		for ( short jj = 0; jj < nBOS; jj++ )
		{	if ( *(def_ID+jj) < 0 )
			{	firstSlot = jj;
				break;
			}
		}
		if ( firstSlot < 0 )
			break;
		CString	sym;			// don't really care
		long	offset = theApp.optMgr->GetNextDef_ID( pos, sym );
		if (	offset >= 0
			&&  offset != *(def_ID+0)
			&&  offset != *(def_ID+1)
			&&  offset != *(def_ID+2)
			&&  offset != *(def_ID+3) )
			*(def_ID+firstSlot) = offset;
	}
}			// FillSlots()
//----------------------------------------------------------------------------
bool	CPlotOptionsDialog::InstValid( int jj, bool* chkBtns )
{		// jj is 0-based
	COption*	opt = (COption*)*(pBOS+jj);
	if ( ! *(chkBtns+jj)  ||  opt == NULL )				// no longer bailing out when ( opt->getMktPrice() < 0.001 )
		return	false;									// nothing to do

	bool	res = opt->getVolatility() > 0.01
			  &&  opt->getStockPrice() > 0.001
			  &&  opt->strikePrice	   > 0.001;
	return	res;
}			// InstValid()
//----------------------------------------------------------------------------
void	CPlotOptionsDialog::LoadBOSs( long* def_ID ) 
{	unsigned short	jj;
	DeleteBOSs();				// delete any existing options

		// fill any open slots (containing -1) in the selected def_ID
	FillSlots( def_ID );

		// load pOption's corresponding to the def_IDs
	for ( jj = 0; jj < nBOS; jj++ )
	{	if ( *(def_ID+jj) >= 0 )
			*(pBOS+jj) = (CBOS*)theApp.optMgr->ReadOption( *(def_ID+jj) );
	}
}			// LoadBOSs()
//----------------------------------------------------------------------------
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
void	CPlotOptionsDialog::LoadSymbols( void )
{		// load the symbols associated with the current instrument type
	CMapStringToPtr*  theMap = theApp.GetSymbolTable( Option );
	LoadSymbolsCore ( theMap );
}			// LoadSymbols()
//----------------------------------------------------------------------------
void	CPlotOptionsDialog::SetCheckButtons( bool* chkBtns ) 
{
	c_Check1.SetCheck( *(chkBtns+0)  ?  BST_CHECKED  :  BST_UNCHECKED );
	c_Check2.SetCheck( *(chkBtns+1)  ?  BST_CHECKED  :  BST_UNCHECKED );
	c_Check3.SetCheck( *(chkBtns+2)  ?  BST_CHECKED  :  BST_UNCHECKED );
	c_Check4.SetCheck( *(chkBtns+3)  ?  BST_CHECKED  :  BST_UNCHECKED );
}			// SetCheckButtons()
//----------------------------------------------------------------------------
