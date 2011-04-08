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
// DlgMapFolder.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgMapFolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMapFolder dialog


CDlgMapFolder::CDlgMapFolder(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMapFolder::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMapFolder)
	m_sFolder = _T("");
	m_sVirtualFolder = _T("");
	//}}AFX_DATA_INIT
}


void CDlgMapFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMapFolder)
	DDX_Text(pDX, IDC_FOLDER, m_sFolder);
	DDX_Text(pDX, IDC_VIRTUALFOLDER, m_sVirtualFolder);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMapFolder, CDialog)
	//{{AFX_MSG_MAP(CDlgMapFolder)
	ON_BN_CLICKED(IDC_CHOOSE_FOLDER, OnChooseFolder)
	ON_EN_CHANGE(IDC_FOLDER, OnChangeFolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMapFolder message handlers

BOOL CDlgMapFolder::OnInitDialog() 
{
	CDialog::OnInitDialog();

	FixFolder();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMapFolder::OnChooseFolder() 
{
	UpdateData( TRUE );

	char dir[ CWF_STRSIZE ];

	// Copy current folder name
	strcpy_sz( dir, m_sFolder );

	// Get browse directory
	if ( !CWinFile::GetBrowseDir( GetSafeHwnd(), "Choose Folder", dir, dir ) )
		return;
	
	char oldfolder[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( m_sFolder, oldfolder );

	m_sFolder = dir;	

	char newfolder[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( m_sFolder, newfolder );

	// Set new folder name if needed
	if( m_sVirtualFolder.IsEmpty() || !strcmpi( oldfolder, m_sVirtualFolder ) )
		m_sVirtualFolder = newfolder;

	UpdateData( FALSE );

}

void CDlgMapFolder::FixFolder()
{
	char oldfolder[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( m_sFolder, oldfolder );

	UpdateData( TRUE );
	
	char newfolder[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( m_sFolder, newfolder );

	// Set new folder name if needed
	if( m_sVirtualFolder.IsEmpty() || !strcmpi( oldfolder, m_sVirtualFolder ) )
	{	m_sVirtualFolder = newfolder;
		UpdateData( FALSE );
	} // end if
}

void CDlgMapFolder::OnChangeFolder() 
{
	FixFolder();
}
