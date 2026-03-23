#pragma once

#include "StrategyExplorerDlg.h"

// CStrategyExplorerHiResDlg dialog

class CStrategyExplorerHiResDlg : public CStrategyExplorerDlg
{
	DECLARE_DYNAMIC(CStrategyExplorerHiResDlg)

public:
	CStrategyExplorerHiResDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStrategyExplorerHiResDlg();

// Dialog Data
	enum { IDD = IDD_StrategyExplorerHiResDlg };
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
