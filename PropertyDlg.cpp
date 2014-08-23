// PropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Azolla.h"
#include "PropertyDlg.h"


// CPropertyDlg dialog

IMPLEMENT_DYNAMIC(CPropertyDlg, CDialog)

CPropertyDlg::CPropertyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPropertyDlg::IDD, pParent)
{
	m_updateInterval = 0;
}

CPropertyDlg::~CPropertyDlg()
{
}

void CPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTPROPERTY, m_listProperty);
}


BEGIN_MESSAGE_MAP(CPropertyDlg, CDialog)
END_MESSAGE_MAP()


// CPropertyDlg message handlers
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

BOOL CPropertyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	initPropertyList();

	return 0;
}

void CPropertyDlg::update()
{
	// If number of sensor is changing, then update the list
	if (m_sensorNum != m_robot->GetSensorNum())
		initPropertyList();

	// Do not always update, it will cause flickering
	m_updateInterval++;
	if (m_updateInterval <= 10) // After 10 times
		return;
	m_updateInterval = 0;

	char buffer[12];

	int i = 0;
	for (i = 0; i< m_robot->GetSensorNum(); i++){
		sprintf(buffer, "%4.2f", m_robot->PixelToCm(m_robot->GetSensorValue(i)));
		m_listProperty.SetItemText(i, 1, buffer);
	}

	sprintf(buffer, "%4.2f", m_robot->PixelToCm(m_robot->GetSpeed(LEFT)));
	m_listProperty.SetItemText(i, 1, buffer);
	sprintf(buffer, "%4.2f", m_robot->PixelToCm(m_robot->GetSpeed(RIGHT)));
	m_listProperty.SetItemText(i+1, 1, buffer);
	sprintf(buffer, "%4.2f", m_robot->GetAngle() / M_PI * 180); // in degrees);
	m_listProperty.SetItemText(i+2, 1, buffer);
	
}

void CPropertyDlg::initPropertyList()
{
	m_listProperty.DeleteAllItems();
	
	m_listProperty.SetExtendedStyle(LVS_EX_GRIDLINES);
	m_listProperty.InsertColumn(0, "Item", LVCFMT_LEFT, 100);
	m_listProperty.InsertColumn(1, "Value", LVCFMT_LEFT, 100);

	LVITEM lvItem;
	lvItem.mask = LVIF_TEXT;
	lvItem.iSubItem = 0;

	char buf[10];

	int i = 0;
	for (i = 0; i < m_robot->GetSensorNum(); i++){ // We have maximum 12 sensors
	
		sprintf(buf, "Sensor %i", i);
		lvItem.iItem = i;
		lvItem.pszText = buf;
		m_listProperty.InsertItem(&lvItem);
	}

	lvItem.iItem = ++i;
	lvItem.pszText = "Left Speed";
	m_listProperty.InsertItem(&lvItem);

	lvItem.iItem = ++i;
	lvItem.pszText = "Right Speed";
	m_listProperty.InsertItem(&lvItem);
	
	lvItem.iItem = ++i;
	lvItem.pszText = "Orientation";
	m_listProperty.InsertItem(&lvItem);

	m_sensorNum = m_robot->GetSensorNum();

}