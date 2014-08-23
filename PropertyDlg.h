#pragma once

#include "afxcmn.h"
#include "robot.h"

// Timer ID constants
const UINT ID_TIMER_PROPERTYBOX = 0x2001;
/////////////////////////////////////////////////////////////////////////////////////////

// CPropertyDlg dialog
class CPropertyDlg : public CDialog
{
	DECLARE_DYNAMIC(CPropertyDlg)

public:
	CPropertyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPropertyDlg();

	void update();
	void initPropertyList();

// Dialog Data
	enum { IDD = IDD_SHOWPROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listProperty;
	CRobot *m_robot;	// this points to m_robot on AzollaView.h
private:
	int m_updateInterval;
	int m_sensorNum;
};
