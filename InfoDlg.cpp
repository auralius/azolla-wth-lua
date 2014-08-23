// InfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Azolla.h"
#include "InfoDlg.h"
#include "time.h"


// CInfoDlg dialog

IMPLEMENT_DYNAMIC(CInfoDlg, CDialog)

CInfoDlg::CInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInfoDlg::IDD, pParent)
{
	m_bInit = FALSE;
}

CInfoDlg::~CInfoDlg()
{
}

void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDOUTPUT, m_edOutput);
}

BOOL CInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect sz;
	GetClientRect(sz);
	sz.DeflateRect(5,5);
	
	m_bInit = TRUE;

	theApp.m_output = &m_edOutput;

	m_edOutput.DeleteAllItems();
	m_edOutput.SetExtendedStyle(LVS_EX_GRIDLINES);
	m_edOutput.InsertColumn(0, "Time", LVCFMT_LEFT, 80);
	m_edOutput.InsertColumn(1, "Output", LVCFMT_LEFT, 1000);

	return TRUE;
}

void CInfoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if(!m_bInit)
		return;

	CRect sz;
	GetClientRect(sz);
	sz.DeflateRect(5,5);

	m_edOutput.MoveWindow(sz);
}

void CInfoDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// Load the desired menu
	CMenu mnuPopupOutput;

	mnuPopupOutput.LoadMenu(IDR_OUTPUT_POPUP);

	// Find the rectangle around the control
	CRect rectListCtrl;

	m_edOutput.GetWindowRect(&rectListCtrl);

	// Get a pointer to the first item of the menu
	CMenu *mnuPopupMenu = mnuPopupOutput.GetSubMenu(0);
	ASSERT(mnuPopupMenu);
	
	// Find out if the user right-clicked a button
	if( rectListCtrl.PtInRect(point) ) // Since the user right-clicked a button, display its context menu
		mnuPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

}

void CInfoDlg::OnPopupClear()
{
	m_edOutput.DeleteAllItems();
}

BEGIN_MESSAGE_MAP(CInfoDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_POPUP_CLEAR, OnPopupClear)
END_MESSAGE_MAP()


// CInfoDlg message handlers
