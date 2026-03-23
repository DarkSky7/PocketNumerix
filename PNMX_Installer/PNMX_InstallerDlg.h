// PNMX_InstallerDlg.h : header file
//
#pragma once
#include <afxwin.h>
#include <afxdtctl.h>
#include <atlcomtime.h>
#include <ATLComTime.h>
#include <queue>
//#include "RapiCtx.h"
#include "resource.h"
#include "crypt.h"			// for the constants that determine the size of the KeySchedule
#define DECRYPT_BOM 1		// (true) - we're going to encrypt it from now on

	// the cipher key comes in the invoice header now
//const unsigned long ck[ nKeys ] = {	0x83DE9C41, 0x28AED2A6, 0x09Cf4f3C, 0x731FE385,
//										0x2B7E1516, 0xAB710588, 0x9B7E1839, 0x36C0F31D	};
// CPNMX_InstallerDlg dialog
class CPNMX_InstallerDlg : public CDialog
{
// Construction
public:
	CPNMX_InstallerDlg( CWnd* pParent = NULL );		// standard constructor
	~CPNMX_InstallerDlg();							// standard destructor

// Dialog Data
	enum { IDD = IDD_PNMX_INSTALLER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
	HICON m_hSmallIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnPNMX_InstallerAlreadyRunning( WPARAM, LPARAM );
	afx_msg LRESULT OnPNMX_InstallerDeviceConnected( WPARAM, LPARAM );
	afx_msg void OnBnClickedFetch();
//	afx_msg void OnEnChangeFirstName();
//	afx_msg void OnEnChangeLastName();
//	afx_msg void OnEnChangeEmailAddress();
//	afx_msg void OnEnChangeInvoicNnumber();
	afx_msg void OnBnClickedBondManager();
	afx_msg void OnBnClickedNillaHedge();
	afx_msg void OnBnClickedStockManager();
	afx_msg void OnBnClickedOptionChains();
	afx_msg void OnBnClickedStrategyExplorer();
	afx_msg void OnBnClickedTBillRates();
	afx_msg void OnBnClickedYieldCurveFitter();
	DECLARE_MESSAGE_MAP()
	void	EnableFetchButton( CString cs, unsigned short bb );

public:
	CButton		c_DeviceConnected;
	CButton		c_FetchButton;
	CButton		c_BOM_Found;
	CButton		c_InstallComplete;

	CDateTimeCtrl	c_InvoiceDate;
	CEdit			c_FirstName;
	CEdit			c_LastName;
	CEdit			c_EmailAddress;
	CEdit			c_InvoiceNumber;

	CStatic		c_MemoryRequiredResult;
	CStatic		c_MemoryAvailResult;
	CStatic		c_OsVersionResult;
	CStatic		c_StatusResult;
	CStatic		c_DeviceIdResult;
	CStatic		c_DeviceConnectedLabel;
	CStatic		c_BOM_Label;
	CStatic		c_PlatformIdResult;
	CStatic		c_UUIDresult;
	CString		m_FirstName;
	CString		m_LastName;
	CString		m_EmailAddress;
	CString		m_InvoiceNumber;
	char*		lpServerName;			// 192.168.1.101 for now, later, pnmx.com
	char*		urlRequest;				// the server filename
	BOOL		m_DeviceConnected;

		// 0 - no data; 1 - have osVer, 2 - have devID, 4 - have internet
	unsigned short	m_ButtonState;

		//  0 - no data
		//  1 - have FirstName
		//  2 - have LastName
		//  4 - have email address
		//  8 - have Invoice Number
	unsigned short	m_InvoiceFields;

		// desktop info
	void	GetInvoice( void );
		// RAPI info
	BOOL	GetOsVersion( void );		// --> osVersion
	bool	GetDeviceID( void );		// --> deviceID
	bool	GetMemoryAvail( void );		// --> XXX - ???

		// Script suppport
	short	DownUp( wchar_t* progNsme, char* hashUUID,
					wchar_t* testPath, unsigned long expectedFileSize,
							CButton& downloadedButton, CButton& uploadedButton,	CButton& installedButton,
							DWORD memReq, unsigned char* mimeSig, wchar_t* devPath );
	short	DownUpInstall( wchar_t* progNsme, char* hashUUID,
							wchar_t* testPath, unsigned long expectedFileSize,
							CButton& downloadedButton, CButton& uploadedButton,	CButton& installedButton,
							DWORD memReq, unsigned char* mimeSig, wchar_t* cabName,
							wchar_t* devPath = NULL );			// if NULL, create from the ranString
	short	Subscribe( wchar_t* progName, char*	hashUUID, CButton& downloadedButton, CButton& installedButton );
	short	License( wchar_t* progName, char* hashUUID, CButton& licensedButton );
	short	DeleteDesktopFile( wchar_t* progName, wchar_t* T_path );
	short	DeleteDeviceFile( wchar_t* progName, wchar_t* fullPathName );
	short	Execute( wchar_t* T_path, wchar_t* T_cmdLine, PROCESS_INFORMATION* procInfo );
	void	LoadApp( wchar_t* progName, wchar_t* cabName, wchar_t* deviceFileName );
	long	UnloadApp( wchar_t* progName, wchar_t* cabName );
	short	RegisterKeyValuePair( HKEY nKey, wchar_t* subkey, DWORD type, unsigned short nBytes, void* value );
	void	FetchBOM( void );
	short	Install( void );
	short	ParseBOM( void );
	void	GetInstalledDLLSizes( DWORD* atl, DWORD* mfc, DWORD* vcr );	// size of three MFC DLLs
	DWORD	GetFileSize( wchar_t* dirName, wchar_t* pnmxFilename );		// just the part after \\<dirName>\\PocketNumerix
	DWORD	GetPNMXfileSize( wchar_t* pnmxFilename );					// just the part after \\Program Files\\PocketNumerix
	DWORD	GetTempFileSize( wchar_t* pnmxFilename );					// just the part after \\Temp\\PocketNumerix
	void	UpdateMemoryStatus( void );

	long			m_MemoryAvail;
	DWORD			m_PpcInstallMemReq;
	char*			m_UUID;					// the hardware UUID
	char*			m_HUID_BM;				// BondManager's hashed UUID
	char*			m_HUID_NH;				// NillaHedge's hashed UUID
	char*			m_HUID_SM;				// StockManager's hashed UUID
	char*			m_HUID_SX;				// StrategyExplorer's hashed UUID
	char*			m_HUID_YCF;				// YieldCurveFitter's hashed UUID
	unsigned char*	m_BMckiv;				// MFC and PpcInstall use the invoice header ckiv
	unsigned char*	m_NHckiv;
	unsigned char*	m_SMckiv;
	unsigned char*	m_SXckiv;
	unsigned char*	m_YCFckiv;
	unsigned char*	m_MFCckiv;
	unsigned char*	m_PPCckiv;
	unsigned char*	m_OCRckiv;
	unsigned char*	m_VARckiv;				// CovarAcct
	unsigned char*	m_TBRckiv;

	wchar_t*		lpFileName;				// local temp file for downloads
	wchar_t*		m_DeskTempPath;
	wchar_t*		m_DeviceTempPath;
	long			m_MemoryRequired;
	short			m_ProductsMask;			// where BM, NH, SM, OCR, SX, TBillRates, YCF get a bit each
	short			m_NumProdsRequiringMFClibs;
											// as in YTSONB, so BM is in the least significant bit
	wchar_t			m_progFilesDir[ MAX_PATH ];
	COleDateTime	m_InvoiceDate;
	CString			m_OsVersion;			// a string ???
	DWORD			m_DeviceID;				// 128 bits --> 16 bytes

private:
	HANDLE			m_ThreadHandle;			// do we need to saved the thread id too?
	BOOL			m_BOM_Found;

public:
	CButton		c_LicensesInstalled;
	CStatic		c_LicensesInstalledLabel;

	CButton		c_BondManager;
	CButton		c_NillaHedge;
	CButton		c_StockManager;
	CButton		c_StrategyExplorer;
	CButton		c_YieldCurveFitter;
	CButton		c_OptionChains;
	CButton		c_TBillRates;
	CButton		c_CovarAcct;

	DWORD		m_BMmemReq;
	DWORD		m_NHmemReq;
	DWORD		m_SMmemReq;
	DWORD		m_SXmemReq;
	DWORD		m_YCFmemReq;
	DWORD		m_MFCmemReq;

	long		m_OsMajVerNum;			// an integral representation of the major version

	BOOL		m_BondManager;
	BOOL		m_NillaHedge;
	BOOL		m_StockManager;
	BOOL		m_StrategyExplorer;
	BOOL		m_YieldCurveFitter;
	BOOL		m_OptionChains;
	BOOL		m_TBillRates;
	BOOL		m_CovarAcct;			// goes with m_CovarAcctDownloaded, m_CovarAcctInstalled

	bool		m_DLLsInstalled;

	bool		m_BondManagerInstalled;
	bool		m_BondManagerLicensed;

	bool		m_NillaHedgeInstalled;
	bool		m_NillaHedgeLicensed;

	bool		m_StockManagerInstalled;
	bool		m_StockManagerLicensed;

	bool		m_StrategyExplorerInstalled;
	bool		m_StrategyExplorerLicensed;

	bool		m_YieldCurveFitterInstalled;
	bool		m_YieldCurveFitterLicensed;

	bool		m_OptionChainsInstalled;
	bool		m_TBillRatesInstalled;
	bool		m_CovarAcctInstalled;

public:
	bool	fetchInProgress;
	CButton c_Ppc_Downloaded;
	CButton c_MFC_Downloaded;
	CButton c_BM_Downloaded;
	CButton c_NH_Downloaded;
	CButton c_SM_Downloaded;
	CButton c_SX_Downloaded;
	CButton c_YCF_Downloaded;
	CButton c_OCR_Downloaded;
	CButton c_TBR_Downloaded;
	CButton c_VAR_Downloaded;				// CA downloaded button

	CButton c_Ppc_Uploaded;
	CButton c_MFC_Uploaded;
	CButton c_BM_Uploaded;
	CButton c_NH_Uploaded;
	CButton c_SM_Uploaded;
	CButton c_SX_Uploaded;
	CButton c_YCF_Uploaded;

	CButton c_Ppc_Installed;
	CButton c_MFC_Installed;
	CButton c_BM_Installed;
	CButton c_NH_Installed;
	CButton c_SM_Installed;
	CButton c_SX_Installed;
	CButton c_YCF_Installed;
	CButton c_OCR_Installed;
	CButton c_TBR_Installed;
	CButton c_VAR_Installed;				// CA installed button

	CButton c_BM_Licensed;
	CButton c_NH_Licensed;
	CButton c_SM_Licensed;
	CButton c_SX_Licensed;
	CButton c_YCF_Licensed;

	CButton c_SubscriptionsActivated;
	CButton c_SubscriptionsActivatedLabel;
	CButton c_BinariesInstalled;
	CStatic c_BinariesInstalledLabel;

	CStatic c_DownloadStatusResult;
	CStatic c_UploadStatusResult;
	CStatic c_InstallStatusResult;

	CStatic c_AppImage;

	RAPIINIT	rapiInit;

private:
	short	LoadBOMcache( wchar_t* deskTempPath, char*& cache );

};
//	short	WceloadPrepEpilogue( wchar_t* progName, bool capture );		// true for Prep, false for Epilogue
