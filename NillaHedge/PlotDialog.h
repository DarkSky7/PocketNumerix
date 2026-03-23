#if !defined(AFX_PLOTDIALOG_H__56E0CC9B_1B2D_4508_8E2E_09C642FABA88__INCLUDED_)
#define AFX_PLOTDIALOG_H__56E0CC9B_1B2D_4508_8E2E_09C642FABA88__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PlotDialog.h : header file
//

#include "NillaDialog.h"
#include "FPoint.h"

/////////////////////////////////////////////////////////////////////////////
// CPlotDialog dialog

class CPlotDialog : public CNillaDialog
{
// Construction
public:
	CPlotDialog( int IDD, CWnd* pParent = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPlotDialog)
	enum { IDD = 1 };			// a placeholder value that must be overriden
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlotDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void		DrawPolyline( int ii, CPaintDC& dc, CFPoint* boFpts, int nPoints );
	void		DrawPlotBackground( CPaintDC& dc );
	void		DrawGrid( CPaintDC& dc );
	void		DrawGridLabels( CPaintDC& dc );
	short		MapToPlot( CFPoint ptIn, CPoint& ptOut );	// 0 indicates no clipping
	short		MapToPlotY( float fyy )
				{	return	(short)(plotExtents.top + (topLeft.y - fyy) * yScale);	};
	short		MapToPlotX( float fxx )
				{	return	(short)(plotExtents.left + (fxx - topLeft.x) * xScale);	};

protected:
	void		Plot( void );					// invalidate the plot region and request an update

	CFPoint		topLeft;		// domain point set by EvalStrategy
	CFPoint		bottomRight;	// domain point used by Draw... routines
	float		yScale;			// pixels per dollar
	float		xScale;			// scale up yscale by height/width ratio
	CRect		plotExtents;
	char		copyrightOffset[2];		// half grids from the axis [0] for AllRightsReserved, [1] for PocketNumerix
	bool		copyrightAxisRelative;		// if true, place Copyright notices above and below the X-axis

	// Generated message map functions
	//{{AFX_MSG(CPlotDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLOTDIALOG_H__56E0CC9B_1B2D_4508_8E2E_09C642FABA88__INCLUDED_)
