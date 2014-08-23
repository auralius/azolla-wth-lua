// EditorDlg.cpp : implementation file
//

#include "Stdafx.h"
#include "Azolla.h"
#include "EditorDlg.h"

#include <iostream>
#include <fstream>
#include <io.h>


BEGIN_MESSAGE_MAP(CEditorDlg, CDialog)
	ON_WM_SIZE()
	ON_COMMAND(ID_BTCOPY_EDITOR, OnEditCopy)
	ON_COMMAND(ID_BTPASTE_EDITOR, OnEditPaste)
	ON_COMMAND(ID_BTCUT_EDITOR, OnEditCut)
	ON_COMMAND(ID_BTUNDO_EDITOR, OnEditUndo)
	ON_COMMAND(ID_BTREDO_EDITOR, OnEditRedo)
	ON_COMMAND(ID_BTOPEN_EDITOR, OnOpenFile)
	ON_COMMAND(ID_BTSAVE_EDITOR, OnSaveFile)
	ON_COMMAND(ID_BTFIND_EDITOR, OnFindText)
	//ON_UPDATE_COMMAND_UI(ID_INDICATOR_LNCOL, OnUpdateLine) 
END_MESSAGE_MAP()


// LUA keywords
static const char g_luaKeyWords[] = 

	// Standard
	"azolla.main "
	"begin end "
	"if then else elseif "
	"for do while repeat until "
	"return "
	"local "
	"in "
	"function azolla "
	"true false ";

// Scintilla Colors structure
struct SScintillaColors
{	int iItem;
	COLORREF rgb;
};

// A few basic colors
const COLORREF black = RGB( 0, 0, 0 );
const COLORREF white = RGB( 255, 255, 255 );
const COLORREF green = RGB( 0, 128, 0 );
const COLORREF red = RGB( 255, 0, 0 );
const COLORREF blue = RGB( 0, 0, 255 );
const COLORREF orange = RGB( 255, 128, 0 );
const COLORREF magenta = RGB( 255, 0, 255 );
const COLORREF cyan = RGB( 0, 255, 255 );

// Default color scheme
static SScintillaColors g_rgbSyntaxLua[] = 
{
	{	SCE_LUA_COMMENT,		green },
	{	SCE_LUA_COMMENTLINE,	green },
	{	SCE_LUA_COMMENTDOC,		green },
	{	SCE_LUA_NUMBER,			magenta },
	{	SCE_LUA_STRING,			orange },
	{	SCE_LUA_CHARACTER,		orange },
	{	SCE_LUA_OPERATOR,		red },
	{	SCE_LUA_PREPROCESSOR,	cyan },
	{	SCE_LUA_WORD,			blue },
	{	-1,						0 }
};

static UINT BASED_CODE indicators[] =
{
    ID_LINE_INDICATOR,
	IDS_LINE_INDICATOR,
};

// CEditorDlg dialog
IMPLEMENT_DYNAMIC(CEditorDlg, CDialog)

CEditorDlg::CEditorDlg(CWnd* pParent /*=NULL*/)
{
	AfxInitRichEdit();
	m_hwndEditor = NULL;
	m_indent = 0;
}

CEditorDlg::~CEditorDlg()
{
}

void CEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

int CEditorDlg::GetLinenumberChars()
{
	// try to get number of lines in control
   LRESULT lLines = SendEditor(SCI_GETLINECOUNT, 0, 0);

   int nChars = 1;
   while (lLines > 0)
   {
      lLines = lLines / 10;
      nChars++;
   }
   return nChars; // allow an extra char for safety
}

int CEditorDlg::GetLinenumberWidth ()
{
	// get number of chars needed to display highest linenumber
	int nChars = GetLinenumberChars() + 1;
	// get width of character '9' in pixels
	LRESULT lWidth = SendEditor(SCI_TEXTWIDTH, STYLE_LINENUMBER, (long)_T("9"));
	return nChars * lWidth;
}

void CEditorDlg::SetDisplayLinenumbers(BOOL bFlag) //flag if we shuld display line numbers
{
	// if display is turned off we set margin 0 to 0
	if (!bFlag){
		SendEditor(SCI_SETMARGINWIDTHN, 0, 0);
	}
	// if display is turned o we set margin 0 to the calculated width
	else{
		int nWidth = GetLinenumberWidth() + 4;
		SendEditor(SCI_SETMARGINWIDTHN, 0, nWidth);
	}
}

void CEditorDlg::SetAStyle(int style, COLORREF fore, COLORREF back, int size, const char *face)
{	
	SendEditor(SCI_STYLESETFORE, style, fore);
	SendEditor(SCI_STYLESETBACK, style, back);
	if (size >= 1)
		SendEditor(SCI_STYLESETSIZE, style, size);
	if (face) 
		SendEditor(SCI_STYLESETFONT, style, reinterpret_cast<LPARAM>(face));
}

LRESULT CEditorDlg::SendEditor(UINT Msg, WPARAM wParam, LPARAM lParam) 
{	
	return ::SendMessage(m_hwndEditor, Msg, wParam, lParam); 
}

void CEditorDlg::InitialiseEditor()
{
	if ( ::IsWindow( m_hwndEditor ) ) 
		return;

	// Create editor window
	m_hwndEditor = CreateWindowEx(	0, "Scintilla", "", 
							WS_CHILD | WS_VISIBLE | WS_TABSTOP,
							0, 0, 100, 100,
							GetSafeHwnd(), NULL /*(HMENU)GuiID*/, 
							AfxGetApp()->m_hInstance, NULL );	

	// Did we get the editor window?
	if ( !::IsWindow( m_hwndEditor ) ){	
		TRACE( "Unable to create editor window\n" );
		return;
	}

	// Separator for autocompletion
	SendEditor(SCI_AUTOCSETSEPARATOR, 10, 0);
	// LUA lexer
	SendEditor( SCI_SETLEXER, SCLEX_LUA );
	// Set number of style bits to use
	SendEditor( SCI_SETSTYLEBITS, 5 );	
	// Set tab width
	SendEditor( SCI_SETTABWIDTH, 4 );
	// Use CPP keywords
	SendEditor( SCI_SETKEYWORDS, 0, (LPARAM)g_luaKeyWords );
	// Set up the global default style. These attributes are used wherever no explicit choices are made.
	SetAStyle( STYLE_DEFAULT, black, white, 10, "Courier New" );
	// Set caret foreground color (cursor)
	SendEditor( SCI_SETCARETFORE, black);
	// Set all styles
	SendEditor( SCI_STYLECLEARALL );
	// Set selection color
	SendEditor( SCI_SETSELBACK, TRUE, RGB(120, 120, 120) );
	// Set syntax colors
	for ( long i = 0; g_rgbSyntaxLua[i].iItem != -1; i++ ) 
		SetAStyle( g_rgbSyntaxLua[i].iItem, g_rgbSyntaxLua[i].rgb );

	SetDisplayLinenumbers(TRUE);
}

BOOL CEditorDlg::EmbedEditControl()
{
	// This part will embed an edit control to toolbar
	int index = 0;
    while (m_wndToolBar.GetItemID(index) != ID_HOLDERTEXTTOFIND) 
		index++;

	CRect rect;
	m_wndToolBar.SetButtonInfo(index, ID_HOLDERTEXTTOFIND, TBBS_SEPARATOR, 150); // 150 is the width of the edit control
    m_wndToolBar.GetItemRect(index, &rect);

	//rect.top+=2;
    rect.bottom += 200;

	if (!m_wndToolBar.m_edTextToFind.Create(WS_CHILD|WS_VISIBLE|WS_BORDER, 
		rect, &m_wndToolBar, IDC_EDTEXTTOFIND))
    {
        TRACE0("Failed to create edit control in toolbar\n");
        return FALSE;
    }
    m_wndToolBar.m_edTextToFind.ShowWindow(SW_SHOW);

	return TRUE;
}

BOOL CEditorDlg::SetStatusBar()
{
	// Ensure valid window
	if ( !::IsWindow( m_hwndEditor ) )
		return FALSE;

	CRect rect;
	GetClientRect(&rect);

	m_bar.SetPaneInfo(0, ID_LINE_INDICATOR, SBPS_NORMAL,rect.Width());      

	//This is where we actually draw it on the screen
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST, 0);

	return TRUE;
}

BOOL CEditorDlg::ScintillaSize()
{
	// Ensure valid window
	if ( !::IsWindow( m_hwndEditor ) )
		return FALSE;

	// Get window size
	RECT rect, ctrl;
	GetClientRect( &rect );
	CopyRect( &ctrl, &rect );

	// Position the editor window
	ctrl.top += 35;
	ctrl.bottom -= 20;
	CWnd *pWnd = CWnd::FromHandle( m_hwndEditor );
	if ( pWnd ) pWnd->MoveWindow( &ctrl );

	return TRUE;
}

BOOL CEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set icon
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_EDITOR));
	this->SetIcon(hIcon, FALSE);

	// Create the Toolbar and attach the resource
	if(!m_wndToolBar.Create(this) || !m_wndToolBar.LoadToolBar(IDR_EDITORTOOLBAR ))
	{
		TRACE0("Failed to Create Dialog Toolbar\n");
		EndDialog(IDCANCEL);
	}

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

	EmbedEditControl();

	// Create the Scintilla editor
	InitialiseEditor();
	ScintillaSize();

	// Create status bar
	m_bar.Create(this); //We create the status bar
	m_bar.SetIndicators(indicators, 1); //Set the number of panes 
	SetStatusBar();

	return 0;
}


// CEditorDlg message handlers
///////////////////////////////////////////////////////////////////////////////

BOOL CEditorDlg::PreTranslateMessage(MSG* pMsg)
{
	// Route keyboard message
	return CWnd::PreTranslateMessage(pMsg);
}

void CEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	ScintillaSize();
	SetStatusBar();
}

void CEditorDlg::OnFindText()
{
	char *ttf = new char[128];
	m_wndToolBar.m_edTextToFind.GetWindowText(ttf,strlen(ttf));
	
	// Search from the beginning to the end of the document
	int nCurrentPos = SendEditor(SCI_GETCURRENTPOS);
	int nTextLength = SendEditor(SCI_GETTEXTLENGTH);

	if (nCurrentPos >= nTextLength)
		nCurrentPos = 0;

	SendEditor(SCI_SETTARGETSTART, nCurrentPos);
	SendEditor(SCI_SETTARGETEND, nTextLength);

	int pos = SendEditor(SCI_SEARCHINTARGET,-1, (LPARAM)ttf);
	
	SendEditor(SCI_SETSEL, pos, pos+strlen(ttf));

	delete [] ttf;
}

void CEditorDlg::OnEditCopy()
{
	SendEditor(SCI_COPY, 0, 0);
}

void CEditorDlg::OnEditPaste()
{
	SendEditor(SCI_PASTE, 0, 0);
}

void CEditorDlg::OnEditCut()
{
	SendEditor(SCI_CUT, 0, 0);
}

void CEditorDlg::OnEditUndo()
{
	if (SendEditor(SCI_CANUNDO, 0, 0))
		SendEditor(SCI_UNDO, 0, 0);
}

void CEditorDlg::OnEditRedo()
{
	if (SendEditor(SCI_CANREDO, 0, 0))
		SendEditor(SCI_REDO, 0, 0);
}

void CEditorDlg::OnOpenFile()
{
	CString csInput;
	CString filt="text files(*.lua)|*.lua||";
	CFileDialog fileDlg(TRUE,"*.lua","*.lua",NULL,filt,this);
	
	fileDlg.m_ofn.Flags|=OFN_FILEMUSTEXIST;
	fileDlg.m_ofn.lpstrTitle="Open a file";
	
	if (fileDlg.DoModal()==IDOK) 
	{
		CWaitCursor bob;	
		CString fileName = fileDlg.GetPathName();
		LoadFile((LPCSTR) fileName);
	}
}

void CEditorDlg::OnSaveFile()
{
	CString csInput;
	CString filt="text files(*.lua)|*.lua||";
	CFileDialog fileDlg(FALSE,"*.lua","*.lua",NULL,filt,this);
	
	fileDlg.m_ofn.lpstrTitle="Save as a file";
	
	if (fileDlg.DoModal()==IDOK) 
	{
		CWaitCursor bob;	
		CString fileName = fileDlg.GetPathName();
		SaveFile((LPCSTR) fileName);
	}
}


void CEditorDlg::GetText (CString &strText)
{
	LPSTR szText = GetText();
	if (szText != NULL)
	{
		strText = szText;
		delete [] szText;
	}
}

LPSTR CEditorDlg::GetText ()
{
	long lLen = SendEditor(SCI_GETLENGTH, 0, 0) + 1;
	if (lLen > 0)
	{
		TCHAR *pReturn = new TCHAR[lLen];
		if (pReturn != NULL)
		{
			*pReturn = '\0';
			SendEditor(SCI_GETTEXT, lLen, (long)pReturn);
			return pReturn;
		}
	}
	return NULL;
}

void CEditorDlg::SetText (LPCSTR szText)
{
	LRESULT lResult = 0;
	if (szText != NULL)
		lResult = SendEditor(SCI_SETTEXT,0,(LPARAM)szText);
	//SetFocus();
}


BOOL CEditorDlg::LoadFile (LPCSTR szPath)
{
	// if pathname is empty do nothing
	if (szPath == NULL || *szPath == '\0')
		return FALSE;

	CStdioFile f;
	if (!f.Open(szPath, CFile::modeRead | CFile::typeText))
	{
		AfxMessageBox("Can't open that file");
		return FALSE;
	}

	CString t;
	CString csInput;
	while (f.ReadString(t))
	{
		csInput+=t;
		csInput+="\n"; // ReadString strips these
	}

	f.Close();
	// set text to control
	SetText (csInput);
	// tell scintilla that we have an unmodified document
	SendEditor(SCI_SETSAVEPOINT,0 , 0);

	return TRUE;

}

BOOL CEditorDlg::SaveFile (LPCSTR szPath)
{
	std::ofstream file;
	file.open(szPath, std::ios::out);
	if (file.fail())
	{
		return FALSE; // Fail
	}
	int buflen = SendEditor(SCI_GETLENGTH, 0, 0) + 1; //last NULL
	TCHAR *pBuffer = new TCHAR[buflen];
	if (pBuffer != NULL)
	{
		SendEditor(SCI_GETTEXT, buflen,(long)pBuffer);
		file.write(pBuffer, buflen-1);
		delete [] pBuffer;
	}
	file << std::ends;
	file.close();
	return TRUE; // Success
}

void CEditorDlg::OnCharAdded()
{
	long lPos, lLine, lCol, lStart, lEnd = 0;
	CString strLine;

	// Current cursor position
	lPos = SendEditor(SCI_GETCURRENTPOS, 0, 0);
	lLine = SendEditor(SCI_LINEFROMPOSITION, lPos, 0) + 1;
	lCol = SendEditor(SCI_GETCOLUMN, lPos, 0) + 1;
	
	// Set status bar
	strLine.Format(IDS_LINE_INDICATOR, lLine, lCol, lPos);
	m_bar.SetPaneText(0, strLine);

	// Auto completion features
	// From Scintilla MFC Wrapper by PJ Naughter (www.naughter.com)
	int nStartSel = SendEditor(SCI_GETSELECTIONSTART);
	int nEndSel = SendEditor(SCI_GETSELECTIONEND);

	
	if ((nStartSel == nEndSel) && (nStartSel >= 5))
	{
		TextRange tr;
		tr.chrg.cpMin = nStartSel - 5;
		tr.chrg.cpMax = nEndSel;
		char sText[6];
		sText[0] = '\0';
		tr.lpstrText = sText;
		SendEditor(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));

		//Display the auto completion list
		if (_strcmpi(sText, "olla:") == 0)
		{
			const char* itemList =_T("getangle\getnumofrobots\ngetspeed\ngettargetposition\nmain\nreadsensor\nsetactiverobot\nsetspeed\nstepforward\ntextline");
			SendEditor(SCI_AUTOCSHOW, 0, (LPARAM) itemList);
		}

		else if (_strcmpi(sText, "math.") == 0)
		{
			const char* itemList =_T("abs\nacos\nasin\natan\nceil\ncos\ncosh\ndeg\nexp\nfloor\nfmod\nfrexp\nhuge\nldexp\nlog\n\log10\nmax\nmin\nmodf\npi\npow\nrad\nrandom\nrandomseed\nsin\nsinh\nsqrt\ntan\ntanh");
			SendEditor(SCI_AUTOCSHOW, 0, (LPARAM) itemList);
		}

		else if (_strcmpi(sText, "ring.") == 0) /*string*/
		{
			const char* itemList =_T("byte\nchar\ndump\nfind\nformat\ngmatch\ngsub\nlen\nlower\nmatch\nrep\nreverse\nsub\nupper");
			SendEditor(SCI_AUTOCSHOW, 0, (LPARAM) itemList);
		}

		else if (_strcmpi(sText, "file.") == 0)
		{
			const char* itemList =_T("close\nflush\nlines\nread\nseek\nsetvbuf\nwrite");
			SendEditor(SCI_AUTOCSHOW, 0, (LPARAM) itemList);
		}
	}
}

void CEditorDlg::DoBraceMatching()
{
	long lStart, lEnd = 0;

	lStart = SendEditor(SCI_GETCURRENTPOS, 0, 0);
	lEnd = SendEditor(SCI_BRACEMATCH, lStart-1, 0);
	// if there is a matching brace highlight it
	if (lEnd >= 0)
		SendEditor(SCI_BRACEHIGHLIGHT, lStart-1, lEnd);
	// if there is NO matching brace erase old highlight
	else
		SendEditor(SCI_BRACEHIGHLIGHT, -1, -1);
}

// This will update our status bar
BOOL CEditorDlg::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
	NMHDR *phDR;
	phDR = (NMHDR*)lParam;
	// does notification come from my scintilla window?
	if (phDR != NULL && phDR->hwndFrom == m_hwndEditor)
	{
		SCNotification *pMsg = (SCNotification*)lParam;
		switch (phDR->code)
		{
			case SCN_UPDATEUI:		
				DoBraceMatching();
				break;
			case SCN_CHARADDED:
				OnCharAdded();
				break;
		}
		return TRUE;
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}
