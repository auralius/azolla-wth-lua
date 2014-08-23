#pragma once

#include "robot.h"
#include "NumSpinCtrl.h"
#include "ColorButton.h"

/////////////////////////////////////////////////////////////////////////////
// CSetPropertyDlg dialog

class CSetPropertyDlg : public CDialog
{
// Construction
public:
	CSetPropertyDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetPropertyDlg)
	enum { IDD = IDD_SETPROPERTIES };
	CEdit			m_ctrlSensorNum;
	CEdit			m_ctrlDiameter;
	CEdit			m_ctrlAngle;
	CEdit			m_ctrlRadiationCone;
	CEdit			m_ctrlTimeStep;
	CEdit			m_ctrlMinDistance;
	CEdit			m_ctrlMaxDistance;
	CEdit			m_ctrlSonarRate;
	CEdit			m_ctrlMeanGauss;
	CEdit			m_ctrlDevGauss;

	CNumSpinCtrl	m_spinDiameter;
	CNumSpinCtrl	m_spinSensorNum;
	CNumSpinCtrl	m_spinAngle;
	CNumSpinCtrl	m_spinTimeStep;
	CNumSpinCtrl	m_spinRadiationCone;
	CNumSpinCtrl	m_spinMinDistance;
	CNumSpinCtrl	m_spinMaxDistance;
	CNumSpinCtrl	m_spinSonarRate;

	CNumSpinCtrl	m_spinMeanGauss;
	CNumSpinCtrl	m_spinDevGauss;

	CColorButton	m_clrRobot;

	CComboBox		m_cbRobotIndex;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetPropertyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnComboSelEndOk();
	//}}AFX_VIRTUAL

// Implementation
public:
	CRobot *m_robot;	// this points to m_robot on AzollaView.h

protected:

	// Generated message map functions
	//{{AFX_MSG(CSetPropertyDlg)
	afx_msg BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	
};
