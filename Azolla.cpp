// Azolla.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Azolla.h"
#include "MainFrm.h"

#include "AzollaDoc.h"
#include "AzollaView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern bool Scintilla_RegisterClasses( void *hInstance );
extern bool Scintilla_ReleaseResources();


// CAzollaApp

BEGIN_MESSAGE_MAP(CAzollaApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, CAzollaApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_OPEN, CAzollaApp::OnFileOpen)
END_MESSAGE_MAP()


// CAzollaApp construction

CAzollaApp::CAzollaApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_output = NULL;
}


// The one and only CAzollaApp object

CAzollaApp theApp;


// CAzollaApp initialization

BOOL CAzollaApp::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	// Initialize the editor
	if ( !Scintilla_RegisterClasses( AfxGetApp()->m_hInstance ) )
	{	
		AfxMessageBox( "Scintilla failed to initiailze" );
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAzollaDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CAzollaView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// Prompt for file name - used for open and save as
//    static function called from app
//////////////////////////////////////////////////////////////////////////////
BOOL CAzollaApp::PromptForFileName(CString& fileName, UINT /*nIDSTitle*/, 
	DWORD dwFlags, int nDialogType)
{
	CString sFilt, sExtension;
	bool bOpenFileDialog;

	if (nDialogType == SAVE_BMP_FILE){ 
		sFilt = "bitmap files(*.bmp)|*.bmp||";
		sExtension = "*.bmp";
		bOpenFileDialog = false;
	}
	else if (nDialogType == SAVE_WLD_FILE){ 
		sFilt = "world files(*.wld)|*.wld||";
		sExtension = "*.wld";
		bOpenFileDialog = false;
	}
	else if (nDialogType == OPEN_WLD_FILE){ 
		sFilt = "world files(*.wld)|*.wld||";
		sExtension = "*.wld";
		bOpenFileDialog = true;
	}

	CFileDialog dlgFile(bOpenFileDialog, sExtension, sExtension, NULL, sFilt);

	CString title;
	if (bOpenFileDialog) title = _T("Open file"; else title = "Save file");

	dlgFile.m_ofn.Flags |= dwFlags;
	dlgFile.m_ofn.lpstrTitle = title;
	dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);

	BOOL bRet = (dlgFile.DoModal() == IDOK) ? TRUE : FALSE;
	fileName.ReleaseBuffer();

	return bRet;
}

void CAzollaApp::OutputMessage(char *msg)
{
	if (!m_output)
		return;

	// Get current time
	char time_str[10];
	time_t epoch_time;
    struct tm *tm_p;
    epoch_time = time( NULL );
    tm_p = localtime(&epoch_time);
	sprintf(time_str, "%.2d:%.2d:%.2d", tm_p->tm_hour, tm_p->tm_min, tm_p->tm_sec);

	
	int index = m_output->GetItemCount();
	m_output->InsertItem(index, time_str);
	m_output->SetItemText(index, 1,msg);
}

// CAzollaApp message handlers
void CAzollaApp::OnFileOpen() 
{
	// prompt the user (with all document templates)
	CString newName;

	if (!PromptForFileName(newName, AFX_IDS_OPENFILE,
	  OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, OPEN_WLD_FILE))
		return; // open cancelled
	
	OpenDocumentFile(newName);
}


//////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CAzollaApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
