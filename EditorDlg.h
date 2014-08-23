#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class CMyToolBar : public CToolBar
{
public:
	CEdit m_edTextToFind;
};

//=================================================================================
//=================================================================================

// CEditorDlg dialog
class CEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(CEditorDlg)

public:
	CEditorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditorDlg();

// Dialog Data
	enum { IDD = IDD_EDITOR };

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual void SetText(LPCSTR szText);
	virtual LPSTR GetText();
	virtual void GetText(CString &strText);
	virtual BOOL LoadFile(LPCSTR szFile);
	virtual BOOL SaveFile(LPCSTR szFile);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implemetation
protected:
	afx_msg BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnEditCut();
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnSaveFile();
	afx_msg void OnOpenFile();
	afx_msg void OnFindText();
	//afx_msg void OnUpdateLine(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()

	CMyToolBar m_wndToolBar;
	CStatusBar m_bar;

private:
	// Initializes the Scintilla editor
	void InitialiseEditor();
	// Sends a message to the Scintilla editor
	LRESULT SendEditor(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0);
	// Sets a Scintilla style
	void SetAStyle(int style, COLORREF fore, COLORREF back = RGB(255, 255, 255), int size = -1, const char *face = 0);
	// Positions child windows
	BOOL ScintillaSize();
	// Create status bar
	BOOL SetStatusBar();
	BOOL EmbedEditControl();
	// Line numbering
	int GetLinenumberChars();
	int GetLinenumberWidth();
	void SetDisplayLinenumbers(BOOL bFlag);
	void OnCharAdded();
	void DoBraceMatching();
	// Handle to Scintilla editor window
	HWND m_hwndEditor;
	int m_indent;

public:
};
