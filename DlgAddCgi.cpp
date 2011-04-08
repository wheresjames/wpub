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
// DlgAddCgi.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgAddCgi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAddCgi dialog


CDlgAddCgi::CDlgAddCgi(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAddCgi::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAddCgi)
	m_sExe = _T("");
	m_sExt = _T("");
	m_sName = _T("");
	m_sParams = _T("");
	//}}AFX_DATA_INIT
}


void CDlgAddCgi::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAddCgi)
	DDX_Text(pDX, IDC_EXE, m_sExe);
	DDX_Text(pDX, IDC_EXT, m_sExt);
	DDX_Text(pDX, IDC_NAME, m_sName);
	DDX_Text(pDX, IDC_PARAMS, m_sParams);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgAddCgi, CDialog)
	//{{AFX_MSG_MAP(CDlgAddCgi)
	ON_BN_CLICKED(IDC_CHOOSE_EXE, OnChooseExe)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAddCgi message handlers

void CDlgAddCgi::OnChooseExe() 
{
	UpdateData( TRUE );

	// Get the file name
	char filename[ CWF_STRSIZE ] = { 0 };
	if ( !CWinFile::GetOpenFileName(	filename, "Choose CGI Executable",
										"Executable Files (*.exe;*.com)\x0*.exe;*.com\x0"
										"All Files (*.*)\x0*.*\x0",
										"exe",
										 GetSafeHwnd() ) ) return;

	m_sExe = filename;
	UpdateData( FALSE );
}
