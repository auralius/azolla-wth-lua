// AzollaDoc.cpp : implementation of the CAzollaDoc class
//

#include "stdafx.h"
#include "Azolla.h"

#include "AzollaDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAzollaDoc

IMPLEMENT_DYNCREATE(CAzollaDoc, CDocument)

BEGIN_MESSAGE_MAP(CAzollaDoc, CDocument)
END_MESSAGE_MAP()


// CAzollaDoc construction/destruction

CAzollaDoc::CAzollaDoc()
{
	// TODO: add one-time construction code here
}

CAzollaDoc::~CAzollaDoc()
{
}

BOOL CAzollaDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CAzollaDoc serialization

void CAzollaDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CAzollaDoc diagnostics

#ifdef _DEBUG
void CAzollaDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAzollaDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CAzollaDoc commands
BOOL CAzollaDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	CString fileName(lpszPathName);
	bool retVal = m_canvas->SaveFile(fileName.GetBuffer(0));
	if (retVal) return TRUE;
	return FALSE;
}

BOOL CAzollaDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
{
	CString newName = lpszPathName;

	newName = m_strPathName;

	if (newName.IsEmpty()){
		if (!theApp.PromptForFileName(newName, 
			bReplace ? AFX_IDS_SAVEFILE : AFX_IDS_SAVEFILECOPY,
			OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, SAVE_WLD_FILE))
		{
			return FALSE;       // don't even try to save
		}
	}

	BeginWaitCursor();

	if (!OnSaveDocument(newName)){
		if (lpszPathName == NULL){
			// be sure to delete the file
			TRY 
			{
				CFile::Remove(newName);
			}
			CATCH_ALL(e)
			{
				TRACE0("Warning: failed to delete file after failed SaveAs\n");
			}
			END_CATCH_ALL
		}
		EndWaitCursor();
		return FALSE;
	}

	EndWaitCursor();
	
	// Reset the title and change the document name
	SetPathName(newName, TRUE);
	ASSERT(m_strPathName == newName); // must be set

	return TRUE;
}

BOOL CAzollaDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CString fileName(lpszPathName);
	if (m_canvas->LoadFile(fileName.GetBuffer(0)))
		return TRUE;
	return FALSE;
}
