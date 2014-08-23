// AzollaView.h : interface of the CAzollaView class
//


#pragma once

#include "robot.h"
#include "Evaluate.h"
#include "PropertyDlg.h"
#include "SetPropertyDlg.h"
#include "EditorDlg.h"

#include "Canvas.h"


/////////////////////////////////////////////////////////////////////////////////////////

class CAzollaView : public CView
{
protected: // create from serialization only
	CAzollaView();
	DECLARE_DYNCREATE(CAzollaView)

// Attributes
public:
	CAzollaDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:

// Implementation
public:
	virtual ~CAzollaView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	static DWORD WINAPI ScreenThread(LPVOID param);
	static DWORD WINAPI SensorThread(LPVOID param);
	void ScreenThreadWorker();
	void SensorThreadWorker();

protected:

private:
	CRobot m_robot;
	CEvaluate *m_evaluate;

	bool m_bManuallyMoved;
	bool m_bActiveMarkMoved;

	CPropertyDlg *m_propertyDlg;
	CEditorDlg *m_editorDlg;

	CCanvas m_canvas;
	bool m_isShowGrid;

	bool m_bRunning;
	bool m_bAddRobot;
	bool m_bAddMark;
	int m_iActiveMark;


// Generated message map functions
protected:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message );
	afx_msg void OnUpdateRunButton(CCmdUI* pCmdUI);
	afx_msg void UnUpdateStopButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDrawTrajectory(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowGrid(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSelectButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeleteButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRectangleButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCircleButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLineButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomInButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateZoomOutButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSetPropertiesButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileOpenButton(CCmdUI* pCmdUI);

	afx_msg void OnAddRobot();
	afx_msg void OnRemoveRobot();
	afx_msg void OnViewProperty();
	afx_msg void OnViewEditor();
	afx_msg void OnViewSetProperty();
	afx_msg void OnResetRobot();
	afx_msg void OnClearWorld();
	afx_msg void OnDrawTrail();
	afx_msg void OnShowGrid();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnRun();
	afx_msg void OnStop();
	afx_msg void OnBtRect();
	afx_msg void OnBtCirc();
	afx_msg void OnBtLine();
	afx_msg void OnBtDel();
	afx_msg void OnBtSelectAll();
	afx_msg void OnBtSelect();
	afx_msg void OnAddMark();
	afx_msg void OnRemoveMark();

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AzollaView.cpp
inline CAzollaDoc* CAzollaView::GetDocument() const
   { return reinterpret_cast<CAzollaDoc*>(m_pDocument); }
#endif

