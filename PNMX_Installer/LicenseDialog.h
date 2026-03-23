#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// CLicenseDialog dialog

class CLicenseDialog : public CDialog
{
	DECLARE_DYNAMIC(CLicenseDialog)

public:
	CLicenseDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLicenseDialog();

// Dialog Data
	enum { IDD = IDD_LICENSE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit c_License;
	virtual BOOL OnInitDialog();
};
