// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include "cooldialogbar.h"
#include "infodlg.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CToolBar	m_objToolBar;

// Generated message map functions
protected:
	CInfoDlg		m_cDialog;
	CCoolDialogBar	m_wndDialogBar;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewInfoDlg();
	DECLARE_MESSAGE_MAP()
};


