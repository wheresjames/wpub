/*******************************************************************
// Copyright (c) 2002, 2003, 2004, 2005, 
// Robert Umbehant
// mailto:rumbehant@wheresjames.com
//
// This program is free software; you can redistribute it and/or 
// modify it under the terms of the GNU General Public License as 
// published by the Free Software Foundation; either version 2 of 
// the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful, 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public 
// License along with this program; if not, write to the Free 
// Software Foundation, Inc., 59 Temple Place, Suite 330, 
// Boston, MA 02111-1307 USA
//
*******************************************************************/
// VpDoc.cpp : implementation of the CVpDoc class
//

#include "stdafx.h"
#include "Vp.h"

#include "VpDoc.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVpDoc

IMPLEMENT_DYNCREATE(CVpDoc, CDocument)

BEGIN_MESSAGE_MAP(CVpDoc, CDocument)
	//{{AFX_MSG_MAP(CVpDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVpDoc construction/destruction

CVpDoc::CVpDoc()
{
	// TODO: add one-time construction code here

}

CVpDoc::~CVpDoc()
{
}

BOOL CVpDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CVpDoc serialization

void CVpDoc::Serialize(CArchive& ar)
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

/////////////////////////////////////////////////////////////////////////////
// CVpDoc diagnostics

#ifdef _DEBUG
void CVpDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVpDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVpDoc commands

BOOL CVpDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
//	if (!CDocument::OnOpenDocument(lpszPathName))
//		return FALSE;

	FRAME()->LoadOverlay( lpszPathName );	
	
	return TRUE;
}

BOOL CVpDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	return FRAME()->SaveOverlay( lpszPathName );

//	return CDocument::OnSaveDocument(lpszPathName);

}
