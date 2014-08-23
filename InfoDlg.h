#pragma once


// CInfoDlg dialog

class CInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CInfoDlg)

public:
	CInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInfoDlg();

// Dialog Data
	enum { IDD = IDD_INFO };

// Implemetation
protected:
	BOOL m_bInit;
	CListCtrl  m_edOutput;

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint point);
	afx_msg void OnPopupClear();
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
