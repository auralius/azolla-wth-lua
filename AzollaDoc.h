// AzollaDoc.h : interface of the CAzollaDoc class
//


#pragma once

#include "Canvas.h"

class CAzollaDoc : public CDocument
{
protected: // create from serialization only
	CAzollaDoc();
	DECLARE_DYNCREATE(CAzollaDoc)

// Attributes
public:
	CCanvas *m_canvas;

// Operations
public:

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

// Implementation
public:
	virtual ~CAzollaDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


