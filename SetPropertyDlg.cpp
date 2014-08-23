// SetPropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Azolla.h"
#include "SetPropertyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CSetPropertyDlg, CDialog)
	//{{AFX_MSG_MAP(CSetPropertyDlg)
	ON_CBN_SELENDOK( IDC_CB_ROBOTINDEX, OnComboSelEndOk )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSetPropertyDlg dialog


CSetPropertyDlg::CSetPropertyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetPropertyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetPropertyDlg)

	//}}AFX_DATA_INIT
}


void CSetPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetPropertyDlg)
	// Handle edit control
	DDX_Control(pDX, IDC_EDSENSORNUM, m_ctrlSensorNum);
	DDX_Control(pDX, IDC_EDDIAMETER, m_ctrlDiameter);
	DDX_Control(pDX, IDC_EDANGLE, m_ctrlAngle);
	DDX_Control(pDX, IDC_EDRADIATION, m_ctrlRadiationCone);
	DDX_Control(pDX, IDC_EDTIMESTEP, m_ctrlTimeStep);
	DDX_Control(pDX, IDC_EDMINDISTANCE, m_ctrlMinDistance);
	DDX_Control(pDX, IDC_EDMAXDISTANCE, m_ctrlMaxDistance);
	DDX_Control(pDX, IDC_EDGAUSSMEAN, m_ctrlMeanGauss);
	DDX_Control(pDX, IDC_EDGAUSSDEV, m_ctrlDevGauss);
	DDX_Control(pDX, IDC_EDSONARRATE, m_ctrlSonarRate);

	// Handle spin control
	DDX_Control(pDX, IDC_SPINDIAMETER, m_spinDiameter);
	DDX_Control(pDX, IDC_SPINSENSORNUM, m_spinSensorNum);
	DDX_Control(pDX, IDC_SPINANGLE, m_spinAngle);
	DDX_Control(pDX, IDC_SPINTIMESTEP, m_spinTimeStep);
	DDX_Control(pDX, IDC_SPINRADIATION, m_spinRadiationCone);
	DDX_Control(pDX, IDC_SPINMINDISTANCE, m_spinMinDistance);
	DDX_Control(pDX, IDC_SPINMAXDISTANCE, m_spinMaxDistance);
	DDX_Control(pDX, IDC_SPINSONARRATE, m_spinSonarRate);

	DDX_Control(pDX, IDC_SPINGAUSSMEAN, m_spinMeanGauss);
	DDX_Control(pDX, IDC_SPINGAUSSDEV, m_spinDevGauss);

	DDX_Control(pDX, IDC_CB_ROBOTINDEX, m_cbRobotIndex);

	DDX_Control(pDX, IDCOLORPICKER, m_clrRobot);

	COLORREF ThrowAwayColor;
	DDX_ColorButton(pDX, IDCOLORPICKER, ThrowAwayColor);

	//}}AFX_DATA_MAP
}

BOOL CSetPropertyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Spin control configuration
	m_spinDiameter.SetRangeAndDelta(20.0, 50.0, 1.0);
	m_spinSensorNum.SetRangeAndDelta(1.0, 12.0, 1.0);
	m_spinAngle.SetRangeAndDelta(0.0, 360.0, 1.0);
	m_spinRadiationCone.SetRangeAndDelta(0.0,60.0, 1.0);
	m_spinMinDistance.SetRangeAndDelta(10.0, 200.0, 1.0);
	m_spinMaxDistance.SetRangeAndDelta(10.0, 200.0, 1.0);
	m_spinMeanGauss.SetRangeAndDelta(0.0, 200.0, 0.1);
	m_spinDevGauss.SetRangeAndDelta(0.0, 200.0, 0.1);
	m_spinTimeStep.SetRangeAndDelta(0.0, 1.0, 0.01);


	CString strNum; // Place for converting float to CString

	// Fill up combo box for existing robot list
	for (UINT count = 0; count < m_robot->GetNumberOfRobot(); count++){
		strNum.Format("%i", count);
		m_cbRobotIndex.AddString((LPCTSTR)strNum);
	}

	// Set combo box content and color picker to show index and color of current active robot
	m_cbRobotIndex.SetCurSel(m_robot->GetIndexOfActiveRobot());
	m_clrRobot.SetColor(m_robot->GetRobotColor(m_robot->GetIndexOfActiveRobot()));

	// Load current value
	// ->Diameter
	strNum.Format("%4.2f", m_robot->PixelToCm(m_robot->GetSize()));
	m_ctrlDiameter.SetWindowText(strNum);

	// ->Angle
	strNum.Format("%4.2f", m_robot->GetAngle()/M_PI*180); // To degrees
	m_ctrlAngle.SetWindowText(strNum);

	// ->Number of sensors
	strNum.Format("%i", m_robot->GetSensorNum());
	m_ctrlSensorNum.SetWindowText(strNum);

	// ->Sensor radiation cone
	strNum.Format("%4.2f", m_robot->GetRadiationCone() / M_PI * 180); // in degrees
	m_ctrlRadiationCone.SetWindowText(strNum);

	// ->Sensor minimum value
	strNum.Format("%4.2f",m_robot->PixelToCm(m_robot->GetSensorRange(MIN_VAL)));
	m_ctrlMinDistance.SetWindowText(strNum);

	// ->Sensor maximum value
	strNum.Format("%4.2f", m_robot->PixelToCm(m_robot->GetSensorRange(MAX_VAL)));
	m_ctrlMaxDistance.SetWindowText(strNum);

	// ->Sensor noise properties
	float mean;
	float dev;
	m_robot->GetNoiseProperties(mean, dev);
	strNum.Format("%4.2f", mean);
	m_ctrlMeanGauss.SetWindowText(strNum);
	strNum.Format("%4.2f", dev);
	m_ctrlDevGauss.SetWindowText(strNum);

	//->Simulation time step
	strNum.Format("%4.2f", m_robot->GetSimulationTimeStep());
	m_ctrlTimeStep.SetWindowText(strNum);

	//->Simulation time step
	strNum.Format("%4.2f", m_robot->GetSenorSamplingRate());
	m_ctrlSonarRate.SetWindowText(strNum);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CSetPropertyDlg message handlers

void CSetPropertyDlg::OnOK() 
{
	char strNum[8], strNum2[8];

	// Set diameter
	m_ctrlDiameter.GetWindowText(strNum, sizeof(strNum));
	if (atof(strNum) < 5){
		MessageBox("Too small, the minimum size is 5.", "Error found", MB_ICONWARNING);
		return;
	}
	m_robot->SetSize(m_robot->CmToPixel(atof(strNum)));

	// Set number of sensors
	m_ctrlSensorNum.GetWindowText(strNum, sizeof(strNum));
	if (atoi(strNum) > 30){
		MessageBox("Allowed maximum sensor is 30.", "Error found", MB_ICONWARNING);
		return;
	}
	m_robot->SetSensorNum(atoi(strNum));

	// Set robot angle
	m_ctrlAngle.GetWindowText(strNum, sizeof(strNum));
	m_robot->SetAngle(atof(strNum)/180*M_PI); // To degrees

	// Set angular width of sensor radiation cone
	m_ctrlRadiationCone.GetWindowText(strNum, sizeof(strNum));
	if (atof(strNum) > 60.0){
		MessageBox("Allowed maximum sensor radiation cone is 60 degrees.", "Error found", MB_ICONWARNING);
		return;
	}
	m_robot->SetRadiationCone(atof(strNum)*M_PI/180); // To radians

	// Set minimum and maximum distance
	m_ctrlMinDistance.GetWindowText(strNum, sizeof(strNum));
	m_ctrlMaxDistance.GetWindowText(strNum2, sizeof(strNum2));
	m_robot->SetSensorRange(m_robot->CmToPixel(atof(strNum)), m_robot->CmToPixel(atof(strNum2)));

	// Set noise properties
	m_ctrlMeanGauss.GetWindowText(strNum, sizeof(strNum));
	m_ctrlDevGauss.GetWindowText(strNum2, sizeof(strNum2));
	m_robot->SetNoiseProperties(atof(strNum), atof(strNum2));

	//->Simulation time step
	m_ctrlTimeStep.GetWindowText(strNum, sizeof(strNum));
	m_robot->SetSimulationTimeStep(atof(strNum));

	//-> Set sonar sampling rate
	m_ctrlSonarRate.GetWindowText(strNum, sizeof(strNum));
	m_robot->SetSensorSamplingRate(atof(strNum));

	//-> Set robot color
	if (m_clrRobot.GetColor() == OBSTACLE){
		MessageBox("BLACK is not allowed.\nPlease select other color.", "Error found", MB_ICONWARNING);
		return;
	}
		m_robot->SetActiveRobotColor(m_clrRobot.GetColor());

	CDialog::OnOK();
}

void CSetPropertyDlg::OnComboSelEndOk()
{
	UINT index = m_cbRobotIndex.GetCurSel();
	
	m_robot->SetActiveRobot(index);
	m_clrRobot.SetColor(m_robot->GetRobotColor(index));

	CString strNum;

	// ->Diameter
	strNum.Format("%4.2f", m_robot->PixelToCm(m_robot->GetSize()));
	m_ctrlDiameter.SetWindowText(strNum);

	// ->Angle
	strNum.Format("%4.2f", m_robot->GetAngle()/M_PI*180); // To degrees
	m_ctrlAngle.SetWindowText(strNum);
}