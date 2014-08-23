// Azolla.h : main header file for the Azolla application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


enum
{
	SAVE_BMP_FILE,
	SAVE_WLD_FILE,
	OPEN_WLD_FILE,
};

class CAzollaApp : public CWinApp
{
public:
	CAzollaApp();

// Operations
public:
	BOOL PromptForFileName(CString& fileName, UINT /*nIDSTitle*/, DWORD dwFlags, int nDialogType);
	void OutputMessage(char *msg);

	CListCtrl	*m_output;
	

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	DECLARE_MESSAGE_MAP()
};

extern CAzollaApp theApp;
