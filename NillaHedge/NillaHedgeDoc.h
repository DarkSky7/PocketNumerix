// NillaHedgeDoc.h : interface of the CNillaHedgeDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_NILLAHEDGEDOC_H__6DCBC807_92B5_40E1_A611_AADBD039DB53__INCLUDED_)
#define AFX_NILLAHEDGEDOC_H__6DCBC807_92B5_40E1_A611_AADBD039DB53__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ShortRateCalibration.h"		// lives here, so class decl. isn't enough
#include "PositionDisplayOptions.h"		// lives here, so class decl. isn't enough

class CNillaHedgeDoc : public CDocument
{
protected: // create from serialization only
	CNillaHedgeDoc();
	DECLARE_SERIAL( CNillaHedgeDoc )

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNillaHedgeDoc)
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CNillaHedgeDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Implementation
public:
	virtual ~CNillaHedgeDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
//	virtual void Dump(CDumpContext& dc) const;
#endif

	// Attributes
public:
//	BOOL					incDivInReturn;			// a display option for stocks (StockValueDialogs, which will be killed)
//	BOOL					useAnalyticalValue;		// a display option for options (OptionValueDialog, which will be killed)
//	CPositionDisplayOptions	positionListOptions;
//	CPositionDisplayOptions	portfolioNavigatorOptions;
//	CMapStringToOb			bonds;					// Bond Symbol is the map key
//	CMapStringToOb			options;				// Option Symbol is the map key
//	CMapStringToOb			stocks;					// Stock Symbol is the map key
//	CShortRateCalibration	shortRateBasis;

	// Operations
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft eMbedded Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NILLAHEDGEDOC_H__6DCBC807_92B5_40E1_A611_AADBD039DB53__INCLUDED_)
