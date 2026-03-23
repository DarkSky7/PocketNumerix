// PlotDialog.cpp : implementation file
//
#include "StdAfx.h"
#include "resource.h"
#include "PlotDialog.h"
#include "BondManagerApp.h"								// for Discretize()

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const	COLORREF	bkgndColor = RGB(233,233,188);		// background fill color (manilla 228,228,184)
const	COLORREF	gridColor  = RGB(191,191,191);		// most grid lines (light Gray)
const	COLORREF	axisColor  = RGB(159,159,159);		// X-axis (light-mid Gray)
const	COLORREF	textColor  = RGB( 63, 63, 63);		// labels (dark Gray)

/////////////////////////////////////////////////////////////////////////////
// CPlotDialog dialog


CPlotDialog::CPlotDialog( int IDD, CWnd* pParent /*=NULL*/ )
	: CNillaDialog( IDD, pParent )
{
	//{{AFX_DATA_INIT(CPlotDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void	CPlotDialog::DoDataExchange(CDataExchange* pDX)
{
	CNillaDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlotDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( CPlotDialog, CNillaDialog )
	//{{AFX_MSG_MAP(CPlotDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------------------
BOOL CPlotDialog::OnInitDialog() 
{
	CNillaDialog::OnInitDialog();

		// set the extents of plot region
	int cx = GetSystemMetrics( SM_CXSCREEN );			// screen width 240 okay
	int cy = GetSystemMetrics( SM_CYSCREEN ) - 52;		// screen height 320 minus menubarheight -> 268
	plotExtents = CRect( 0, 0, cx, cy );
	return	TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}			// OnInitDialog()
//----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////
//						CPlotDialog GUI extensions							//
//////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------
void	CPlotDialog::DrawPolyline( int ii, CPaintDC& dc, CFPoint* boFpts, int nPoints )
{		// paint a Polyline given nTimePts in boFpts
	CPen		boPen;
	COLORREF	boColor;
	if ( ii < 3 )
			// 0 -->  255, 127, 127;	1 --> 127, 255, 127;	2 --> 127, 127, 255
		boColor = RGB( ii == 0 ? 255 : 127,
					   ii == 1 ? 255 : 127,
					   ii == 2 ? 255 : 127 );		// creates Red, Green, or Blue
	else if ( ii == 3 )
		boColor = RGB( 232, 0, 232 );				// purple
	else
		boColor = RGB( 0, 0, 0 );					// black

	boPen.CreatePen( PS_SOLID, 2, boColor );
	CPen*	pOldPen = dc.SelectObject( &boPen );

	CPoint*	pts = new CPoint[ nPoints ];
	for ( short jj = 0; jj < nPoints; jj++ )
	{	short clipped = MapToPlot( *(boFpts+jj), *(pts+jj) );
#ifdef _DEBUG
		if ( clipped )
			TRACE( _T("CPlotDialog::DrawPolyline: ii=%d, jj=%d, clipped=%d, (boFpts+jj)=%g, *(pts+jj)=%g\n"),
			ii, jj, clipped, *(boFpts+jj), *(pts+jj) );
#endif
	}
	BOOL	res = dc.Polyline( pts, nPoints );
	delete [] pts;

#ifdef _DEBUG
	if ( res == 0 )
	{	TRACE( _T("CPlotDialog::DrawOption: res=%d (should be nonzero)\n"), res );
		AfxDebugBreak();
	}
#endif
	dc.SelectObject( pOldPen );
}			// DrawPolyline()
//----------------------------------------------------------------------------
void	CPlotDialog::DrawGrid( CPaintDC& dc ) 
{		// Drawthe plot background
	CBrush		brushBkgnd( bkgndColor );					// bkgndColor is a CStrategyExplorer const
	CBrush*		pOldBrush = dc.SelectObject( &brushBkgnd );	// for graphics (e.g. Rect) purposes

		// Create and select a thin gray pen.
	CPen		gridPen;
	gridPen.CreatePen( PS_SOLID, 1, gridColor );			// gridColor is a CStrategyExplorer const
	CPen*		pOldPen = dc.SelectObject( &gridPen );

		// Draw a thin gray rectangle filled with Manilla
		// CE mapping mode is always MM_TEXT, so +y is down
	dc.Rectangle( plotExtents );

	COLORREF	oldBkColor = dc.SetBkColor( bkgndColor );		// gridColor is a CStrategyExplorer const
	COLORREF	oldTextColor = dc.SetTextColor( gridColor );	// switch to grid color for the copyright notice
	float		incY = (float)Discretize( (topLeft.y - bottomRight.y) / 8.0 );

		// Display 'copyright' notice
	int cpmyy = MapToPlotY( (float)(0.5 * incY) );
	dc.ExtTextOut( 72, cpmyy - 8, ETO_OPAQUE, NULL, _T("All rights reserved."), NULL );
	cpmyy = MapToPlotY( (float)(-0.5 * incY) );
	dc.ExtTextOut( 72, cpmyy - 8, ETO_OPAQUE, NULL, _T("PocketNumerix, Inc."), NULL );
	dc.SetTextColor( textColor );

	CPen	axisPen;
	axisPen.CreatePen( PS_SOLID, 2, axisColor );

		// Draw horizontal grid lines
	float	fyy = incY * (float)ceil( bottomRight.y / incY );
	while ( topLeft.y > fyy  &&  fyy > bottomRight.y )
	{	bool	itsTheXaxis = ( fabs(fyy) < 1e-15 );
		short	yy = MapToPlotY( (float)fyy );
#ifdef _DEBUG
//		TRACE( _T("DrawGrid: fyy=%g, yy=%d\n"), fyy, yy );
#endif
			// if we're painting the X-axis, switch to a broader pen
		if ( itsTheXaxis )
			dc.SelectObject( &axisPen );
		dc.MoveTo( plotExtents.left, yy );
		dc.LineTo( plotExtents.right, yy );
#if ( _WIN32_WCE >= 0x420 )
			// when the if statement is present in EVT3,
			// the compiler will optimize the SelectObject() out of the executable
		if ( itsTheXaxis )
#endif
			dc.SelectObject( &gridPen );		// put the gridPen back
		
			// next pass
		fyy += incY;
	}

		// Draw vertical grid lines
	float	incX = (float)Discretize( (bottomRight.x - topLeft.x) / 14.0 );
	float	fxx = incX * (float)ceil( topLeft.x / incX );
	while ( fxx < bottomRight.x )
	{	short	xx = MapToPlotX( (float)fxx );
#ifdef _DEBUG
//		TRACE( _T("DrawGrid: fxx=%g, xx=%d\n"), fxx, xx );
#endif
		dc.MoveTo( xx, plotExtents.top	  );
		dc.LineTo( xx, plotExtents.bottom );
			// next pass
		fxx += incX;
	}

		// Put back the old objects.
	dc.SelectObject( pOldBrush );
	dc.SelectObject( pOldPen );
}			// DrawGrid()
//----------------------------------------------------------------------------
void	CPlotDialog::DrawGridLabels( CPaintDC& dc ) 
{		// for text purposes
	COLORREF	oldBkColor = dc.SetBkColor( bkgndColor );		// gridColor is a CStrategyExplorer const
	COLORREF	oldTextColor = dc.SetTextColor( textColor );

		// Draw labels for horizontal grid lines
	float	incY = (float)Discretize( (topLeft.y - bottomRight.y) / 8.0 );
	float	fyy = incY * (float)floor( topLeft.y / incY );
	CPoint	horLabelBR;						// the bottom right corner of the text extent rectangle
	while ( fyy > bottomRight.y )
	{	if ( fabs(fyy) < 1e-15 )
			fyy = 0.0;
		short	yy = MapToPlotY( (float)fyy );
#ifdef _DEBUG
//		TRACE( _T("DrawGridLabels: fyy=%g, yy=%d\n"), fyy, yy );
#endif
		if ( plotExtents.top + 7 <= yy  &&  yy <= plotExtents.bottom - 7 )
		{	wchar_t buf[16];
			swprintf( buf, _T("%g"), fabs(fyy) < 1e-4 ? 0.0 : fyy );
			CString	str = buf;
			CSize	cs = dc.GetTextExtent( str );
			CPoint	txtOrigin( plotExtents.left + 6, yy - 7 );
			horLabelBR = txtOrigin + cs;
			dc.ExtTextOut( txtOrigin.x, txtOrigin.y, ETO_OPAQUE, NULL, buf, NULL );
		}		
			// next pass
		fyy -= incY;
	}

		// Draw labels for vertical grid lines
	float	incX = (float)Discretize( (bottomRight.x - topLeft.x) / 14.0 );
	float	fxx = incX * (float)ceil( topLeft.x / incX );
#ifdef _DEBUG
//	TRACE( _T("DrawGridLabels: horLabelBR=(%d,%d), fxx=%g, incX=%g\n"),
//			horLabelBR.x, horLabelBR.y, fxx, incX );
#endif
	int	verLabelX = 14;
	while ( fxx < bottomRight.x )
	{	short	xx = (short)(plotExtents.left + (fxx - topLeft.x) * xScale);
#ifdef _DEBUG
//		TRACE( _T("DrawGridLabels: fxx=%g, xx=%d\n"), fxx, xx );
#endif
			// skip the first vertical line label to avoid conflicts with Y-axis labels
		wchar_t buf[16];
		swprintf( buf, _T("%g"), fxx );
		CString	str = buf;
		CSize cs = dc.GetTextExtent( str );
		CPoint	txtOrigin = CPoint( xx - cs.cx / 2, plotExtents.bottom - 16 );
#ifdef _DEBUG
//		TRACE( _T("DrawGridLabels: fxx=%g, xx=%d, verLabelX=%d cs=(%d,%d), txtOrigin=(%d,%d)\n"),
//				fxx, xx, verLabelX, cs.cx, cs.cy, txtOrigin.x, txtOrigin.y );
#endif
		if (	    txtOrigin.x - 8 > verLabelX
			  &&  ( txtOrigin.x     > horLabelBR.x  ||  txtOrigin.y - 2 > horLabelBR.y ) )
		{	dc.ExtTextOut( txtOrigin.x, txtOrigin.y, ETO_OPAQUE, NULL, buf, NULL );
			verLabelX = txtOrigin.x + cs.cx;
		}
			// next pass
		fxx += incX;
	}

	// Put back the old objects.
	dc.SetTextColor( oldTextColor );
	dc.SetBkColor( oldBkColor );
}			// DrawGridLabels()
//----------------------------------------------------------------------------
void	CPlotDialog::DrawPlotBackground( CPaintDC& dc ) 
{		// only called by OnPaint(), but identical to code within DrawGrid()
		// slightly lighter Manilla than in the PortfolioNavigator
	CBrush		brushBkgnd( bkgndColor );					// bkgndColor is a CStrategyExplorer const
	CBrush*		pOldBrush = dc.SelectObject( &brushBkgnd );	// for graphics (e.g. Rect) purposes

//	CPen		gridPen;
//	gridPen.CreatePen( PS_SOLID, 1, gridColor );			// gridColor is a CStrategyExplorer const
//	CPen*		pOldPen = dc.SelectObject( &gridPen );

		// Draw a thin gray rectangle filled with Manilla
		// CE mapping mode is always MM_TEXT, so +y is down
	dc.Rectangle( plotExtents );
	dc.SelectObject( pOldBrush );
//	dc.SelectObject( pOldPen );
}			// DrawPlotBackground()
//----------------------------------------------------------------------------
short	CPlotDialog::MapToPlot( CFPoint ptIn, CPoint& ptOut )
{		// uses dialog globals:  origin, xScale, yScale
	short	clipping = 0;
	ptOut.x = plotExtents.left + (short)((ptIn.x - topLeft.x) * xScale);
		// recall that +y is down in MM_TEXT mode
	ptOut.y = plotExtents.top + (short)((topLeft.y - ptIn.y) * yScale);
	if ( ptOut.x < plotExtents.left )
	{
#ifdef _DEBUG
		TRACE( _T("PlotDialog::MapToPlot(1): clipped x=%d to %d\n"),
			ptOut.x, plotExtents.left );
#endif
		ptOut.x = plotExtents.left;
		clipping = 1;
	}

	if ( ptOut.x > plotExtents.right )
	{
#ifdef _DEBUG
		TRACE( _T("PlotDialog::MapToPlot(2): clipped x=%d to %d\n"),
			ptOut.x, plotExtents.right );
#endif
		ptOut.x = plotExtents.right;
		clipping = 2;
	}

	if ( ptOut.y < plotExtents.top )
	{
#ifdef _DEBUG
		TRACE( _T("PlotDialog::MapToPlot(3): clipped y=%d to %d\n"),
			ptOut.y, plotExtents.top );
#endif
		ptOut.y = plotExtents.top;
		clipping = 3;
	}

	if ( ptOut.y > plotExtents.bottom )
	{
#ifdef _DEBUG
		TRACE( _T("PlotDialog::MapToPlot(4): clipped y=%d to %d\n"),
			ptOut.y, plotExtents.bottom );
#endif
		ptOut.y = plotExtents.bottom;
		clipping = 4;
	}
	return	clipping;
}			// MapToPlot()
//----------------------------------------------------------------------------
void	CPlotDialog::Plot( void )
{		// trigger a repaint
	InvalidateRect( plotExtents, FALSE );
//	UpdateWindow();						// this forces UI events ahead in the queue
}			// Plot()
//----------------------------------------------------------------------------
