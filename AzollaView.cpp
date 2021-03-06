// AzollaView.cpp : implementation of the CAzollaView class
//


#include "stdafx.h"
#include "Azolla.h"

#include "AzollaDoc.h"
#include "AzollaView.h"

#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ZOOM_IN_10_PERCENT	1.10
#define ZOOM_OUT_10_PERCENT	1.00/1.10

// CAzollaView

IMPLEMENT_DYNCREATE(CAzollaView, CView)

BEGIN_MESSAGE_MAP(CAzollaView, CView)
	// Standard mouse events
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	// Screen
	ON_WM_ERASEBKGND()
	//UI update
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_RUN, OnUpdateRunButton)
	ON_UPDATE_COMMAND_UI(ID_SIMULATION_STOP, UnUpdateStopButton)
	ON_UPDATE_COMMAND_UI(ID_ROBOT_SHOWTRAIL, OnUpdateDrawTrajectory)
	ON_UPDATE_COMMAND_UI(ID_WORLD_SHOWGRID, OnUpdateShowGrid)
	ON_UPDATE_COMMAND_UI(ID_BTLINE, OnUpdateLineButton)
	ON_UPDATE_COMMAND_UI(ID_BTCIRC, OnUpdateCircleButton)
	ON_UPDATE_COMMAND_UI(ID_BTRECT, OnUpdateRectangleButton)
	ON_UPDATE_COMMAND_UI(ID_BTSELECT, OnUpdateSelectButton)
	ON_UPDATE_COMMAND_UI(ID_BTDEL, OnUpdateDeleteButton)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMIN, OnUpdateZoomInButton)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT, OnUpdateZoomOutButton)
	ON_UPDATE_COMMAND_UI(ID_ROBOT_SETPROPERTIES, OnUpdateSetPropertiesButton)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpenButton)
	
	// General
	ON_COMMAND(ID_VIEW_ROBOTPROPERTIES, CAzollaView::OnViewProperty)
	ON_COMMAND(ID_VIEW_EDITOR, CAzollaView::OnViewEditor)
	ON_COMMAND(ID_VIEW_ZOOMIN, CAzollaView::OnZoomIn)
	ON_COMMAND(ID_VIEW_ZOOMOUT, CAzollaView::OnZoomOut)
	ON_COMMAND(ID_ROBOT_SETPROPERTIES, CAzollaView::OnViewSetProperty)
	ON_COMMAND(ID_SIMULATION_RUN, CAzollaView::OnRun)
	ON_COMMAND(ID_SIMULATION_STOP, CAzollaView::OnStop)
	ON_COMMAND(ID_ROBOT_RESETPOSITION, CAzollaView::OnResetRobot)
	ON_COMMAND(ID_WORLD_CLEARWORLD, CAzollaView::OnClearWorld)
	ON_COMMAND(ID_ROBOT_SHOWTRAIL, CAzollaView::OnDrawTrail)
	ON_COMMAND(ID_WORLD_SHOWGRID, CAzollaView::OnShowGrid)
	ON_COMMAND(ID_ROBOT_ADDROBOT, CAzollaView::OnAddRobot)
	ON_COMMAND(ID_ROBOT_REMOVEROBOT, CAzollaView::OnRemoveRobot)
	ON_COMMAND(ID_ROBOT_ADDMARK, CAzollaView::OnAddMark)
	ON_COMMAND(ID_ROBOT_REMOVEMARK, CAzollaView::OnRemoveMark)
	// Buttin for drawing objects
	ON_COMMAND(ID_BTRECT, CAzollaView::OnBtRect)
	ON_COMMAND(ID_BTCIRC, CAzollaView::OnBtCirc)
	ON_COMMAND(ID_BTLINE, CAzollaView::OnBtLine)
	ON_COMMAND(ID_BTDEL, CAzollaView::OnBtDel)
	ON_COMMAND(ID_BTSELECTALL, CAzollaView::OnBtSelectAll)
	ON_COMMAND(ID_BTSELECT, CAzollaView::OnBtSelect)
END_MESSAGE_MAP()

// CAzollaView construction/destruction
CAzollaView::CAzollaView()
{
	// Init robot
	m_propertyDlg = NULL;
	m_editorDlg = NULL;
	m_evaluate = NULL;
	m_bManuallyMoved = false;
	m_bActiveMarkMoved = false;
	m_bRunning = false;
	m_isShowGrid = false;
	m_bAddRobot = false;
	m_bAddMark = false;
	// Prepare dynamic variables
	m_evaluate = new CEvaluate;

	// Set canvas address for the robot so the robot knows where the obstacles are
	m_robot.SetCanvasAddr(&m_canvas);
}

CAzollaView::~CAzollaView()
{
	OnStop();

	// Destroy window & clean up memory
	// Clean m_propertyDlg
	if (m_propertyDlg)
    {
		if (::IsWindow(m_propertyDlg->GetSafeHwnd()))
            m_propertyDlg->EndDialog(IDCANCEL);
		m_propertyDlg->DestroyWindow();
		delete m_propertyDlg;
    }

	// Clean m_editorDlg
	if (m_editorDlg){
        if (::IsWindow(m_editorDlg->GetSafeHwnd()))
            m_editorDlg->EndDialog(IDCANCEL);
		m_editorDlg-DestroyWindow();
		delete m_editorDlg;
    }

	if (m_evaluate)
		delete m_evaluate;
}

BOOL CAzollaView::PreCreateWindow(CREATESTRUCT& cs)
{
	m_canvas.SetSnapToGrid(true, 10, 10);
	return CView::PreCreateWindow(cs);
}

// CAzollaView drawing
void CAzollaView::OnDraw(CDC* dc)
{
	CMemDC pDC(dc);

	CAzollaDoc* pDoc = GetDocument();
	pDoc->m_canvas = &m_canvas;
	ASSERT_VALID(pDoc);

	if (m_isShowGrid)
		m_canvas.DrawGrid(&pDC, this->GetSafeHwnd());
	m_canvas.OnDraw(&pDC);
	m_robot.DrawRobot(&pDC);

	m_canvas.SetMessage(m_evaluate->getTextLineMsg());
}

BOOL CAzollaView::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}

BOOL CAzollaView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_bAddRobot || m_bAddMark){
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
		return TRUE;
	}
	return CView::OnSetCursor(pWnd, nHitTest, message);
}

// CAzollaView diagnostics
#ifdef _DEBUG
void CAzollaView::AssertValid() const
{
	CView::AssertValid();
}

void CAzollaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAzollaDoc* CAzollaView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAzollaDoc)));
	return (CAzollaDoc*)m_pDocument;
}
#endif //_DEBUG


// CAzollaView message handlers
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CAzollaView::OnMouseMove(UINT nFlags, CPoint point)
{
	CView::OnMouseMove(nFlags, point);

	if (!m_bRunning){
		// Drag robot manually
		if (m_bManuallyMoved){
			m_robot.SetPosition(point);
			if (m_propertyDlg)	
				m_propertyDlg->update();
			Invalidate();
		}
		else if (m_bActiveMarkMoved){
			m_robot.SetLocationMarPosition(m_iActiveMark, point);
			Invalidate();
		}
		else
			m_canvas.OnMouseMove(point);
	}

	if (GetCapture() == this)
		Invalidate();
}

void CAzollaView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CView::OnLButtonDown(nFlags, point);

	// Drag robot manually BUT don't move if simulation is running
	if (!m_bRunning) {
		m_iActiveMark = m_robot.IsLocationMarkDetected(point);

		if (m_robot.IsRobotDetected(point)){
			m_bManuallyMoved = true;
			m_robot.SetPosition(point);
		}

		else if (m_iActiveMark != -1){
			m_bActiveMarkMoved = true;
			m_robot.SetLocationMarPosition(m_iActiveMark, point);
		}

		else
			m_canvas.OnLButtonDown(point);

		SetCapture();
	}

	if (m_bAddRobot){
		m_robot.AddRobot(point);
		m_bAddRobot = false;	
	}

	if (m_bAddMark){
		m_robot.AddLocationMark(point);
		m_bAddMark = false;
	}

	Invalidate();
}

void CAzollaView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CView::OnLButtonUp(nFlags, point);

	// Drag robot manually, skip if simulation is running and send event to CCanvas
	if (!m_bRunning){
		if (!m_bManuallyMoved && !m_bActiveMarkMoved)
			m_canvas.OnLButtonUp(point);
		else{
			m_bManuallyMoved = false;
			m_bActiveMarkMoved = false;
		}
	}

	m_robot.UpdateSensorValue(5,true);

	if (GetCapture() == this){
		ReleaseCapture();
		Invalidate();
	}
}

void CAzollaView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CView::OnLButtonDblClk(nFlags, point);

	// When robot is double clicked, show set proerty dialog
	if (!m_bRunning) {
		if (m_robot.IsRobotDetected(point)){
			OnViewSetProperty();
		}
	}
}

DWORD WINAPI CAzollaView::ScreenThread(LPVOID param)
{
	CAzollaView *obj = (CAzollaView *)param;
	obj->ScreenThreadWorker();
	return 0;
}
DWORD WINAPI CAzollaView::SensorThread(LPVOID param)
{
	CAzollaView *obj = (CAzollaView *)param;
	obj->SensorThreadWorker();
	return 0;
}

void CAzollaView::SensorThreadWorker()
{
	m_robot.PrepareToRun();
	while(m_bRunning /*&& !m_evaluate->isItDone()*/){
		if (m_robot.UpdateSensorValue())
			if (m_propertyDlg) // Update robot property values on proerty dialog box
				m_propertyDlg->update();

		Sleep(1);
	}
}

void CAzollaView::ScreenThreadWorker()
{
	while(m_bRunning && !m_evaluate->isItDone()){
		// Screen thread will refresh the canvas every 10 ms
		Invalidate();
		Sleep(20);
	}

	m_bRunning = false;
	theApp.OutputMessage(m_evaluate->getEvalMsg());
	theApp.OutputMessage("Simulation stopped");
}


void CAzollaView::OnRun()
{
	// Load code from editor to buffer then run it
	if(m_editorDlg && m_bRunning == false){
		// Retrieve code
		CString codes;
		m_editorDlg->GetText(codes);

		// Handle empty scripts
		if (codes.IsEmpty()){
			m_canvas.SetMessage("Code editor is empty\n");
			Invalidate();
			return;
		}

		// Prepare to run
		int ret = m_evaluate->init(codes, &m_robot);

		if (ret == 0){
			// Init and start a new thread for simulation
			theApp.OutputMessage("Simulation running");
			m_bRunning = true;

			DWORD thid;
			CreateThread(NULL, 0, SensorThread, this, 0, &thid);
			CreateThread(NULL, 0, ScreenThread, this, 0, &thid);
		}
		else
			// Init failed, get error message
			theApp.OutputMessage(m_evaluate->getEvalMsg());
	}

	else
		m_canvas.SetMessage("Open the editor with CTRL + E\n");

	Invalidate();
}

void CAzollaView::OnStop()
{
		m_evaluate->setQuitFlag();
}

void CAzollaView::OnViewProperty()
{
	if (!m_propertyDlg)
		m_propertyDlg = new CPropertyDlg;

	m_propertyDlg->m_robot = &m_robot;

	if (!::IsWindow(m_propertyDlg->GetSafeHwnd()))
		m_propertyDlg->Create(IDD_SHOWPROPERTIES);

	m_propertyDlg->ShowWindow(SW_SHOW);
}

void CAzollaView::OnViewEditor()
{
	if (!m_editorDlg)
		m_editorDlg = new CEditorDlg;

	if (!::IsWindow(m_editorDlg->GetSafeHwnd()))
		m_editorDlg->Create(IDD_EDITOR, GetDesktopWindow());

	m_editorDlg->ShowWindow(SW_SHOW);
	m_editorDlg->SetActiveWindow();
}

void CAzollaView::OnViewSetProperty()
{
	CSetPropertyDlg setPropertyDlg;
	setPropertyDlg.m_robot = &m_robot;
	setPropertyDlg.DoModal();
	Invalidate();
}


void CAzollaView::OnBtCirc()
{
	m_canvas.SetTool(TOOL_CIRCLE);
}

void CAzollaView::OnBtRect()
{
	m_canvas.SetTool(TOOL_RECTANGLE);
}

void CAzollaView::OnBtLine()
{
	m_canvas.SetTool(TOOL_LINE);
}

void CAzollaView::OnBtDel()
{
	m_canvas.SetTool(TOOL_DELETE);
}

void CAzollaView::OnBtSelectAll()
{
	m_canvas.SelectAll();
	Invalidate();
}

void CAzollaView::OnBtSelect()
{
	m_canvas.SetTool(TOOL_SELECT);
}

void CAzollaView::OnResetRobot()
{
	// Stop simulation if running
	OnStop();
	// Init robot with default values. 
	// TODO: read from configuration file!
	m_robot.ResetPosition();
	
	// Refresh screen
	Invalidate();
}

void CAzollaView::OnDrawTrail()
{
	m_robot.SetDrawTrajectoryStatus(!m_robot.GetDrawTrajectoryStatus());
	Invalidate();
}

void CAzollaView::OnShowGrid()
{
	m_isShowGrid = !m_isShowGrid;
	Invalidate();
}

void CAzollaView::OnZoomIn()
{
	m_canvas.Zoom(ZOOM_IN_10_PERCENT);
	m_robot.Zoom(ZOOM_IN_10_PERCENT);
	Invalidate();
}

void CAzollaView::OnZoomOut()
{
	m_canvas.Zoom(ZOOM_OUT_10_PERCENT);
	m_robot.Zoom(ZOOM_OUT_10_PERCENT);
	Invalidate();
}

void CAzollaView::OnClearWorld()
{
	m_canvas.ClearAll();
	Invalidate();
}

void CAzollaView::OnAddRobot()
{
	if (!m_bRunning)
		m_bAddRobot = true;
	// No need to invalidate, it has been handled by mouse event!
}

void CAzollaView::OnAddMark()
{
	if (!m_bRunning)
		m_bAddMark = true;
}

void CAzollaView::OnRemoveMark()
{
	if (!m_bRunning){
		m_robot.RemoveLocationMark();
		Invalidate();
	}
}

void CAzollaView::OnRemoveRobot()
{
	if (!m_bRunning){
		m_robot.RemoveRobot();
		Invalidate();
	}
}

// CAzollaView UI update handlers
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CAzollaView::OnUpdateRunButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bRunning);
}

void CAzollaView::UnUpdateStopButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bRunning);
}

void CAzollaView::OnUpdateDrawTrajectory(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((int) m_robot.GetDrawTrajectoryStatus());
}

void CAzollaView::OnUpdateShowGrid(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((int) m_isShowGrid);
}

void CAzollaView::OnUpdateRectangleButton(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((int) m_canvas.GetTool() == TOOL_RECTANGLE);
}

void CAzollaView::OnUpdateCircleButton(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((int) m_canvas.GetTool() == TOOL_CIRCLE);
}

void CAzollaView::OnUpdateLineButton(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((int) m_canvas.GetTool() == TOOL_LINE);
}

void CAzollaView::OnUpdateSelectButton(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((int) m_canvas.GetTool() == TOOL_SELECT);
}

void CAzollaView::OnUpdateDeleteButton(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck((int) m_canvas.GetTool() == TOOL_DELETE);
}

void CAzollaView::OnUpdateZoomInButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bRunning);
}

void CAzollaView::OnUpdateZoomOutButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bRunning);
}

void CAzollaView::OnUpdateSetPropertiesButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bRunning);
}

void CAzollaView::OnUpdateFileOpenButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bRunning);
}