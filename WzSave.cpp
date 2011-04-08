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
// WzSave.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "WzSave.h"

#include "WzFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWzSave dialog


CWzSave::CWzSave(CWnd* pParent /*=NULL*/)
	: CDialog(CWzSave::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWzSave)
	m_sFolder = _T("");
	//}}AFX_DATA_INIT
}


void CWzSave::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWzSave)
	DDX_Control(pDX, IDC_CHOOSE_FOLDER, m_btnChooseFolder);
	DDX_Text(pDX, IDC_FOLDER, m_sFolder);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWzSave, CDialog)
	//{{AFX_MSG_MAP(CWzSave)
	ON_BN_CLICKED(IDC_CHOOSE_FOLDER, OnChooseFolder)
	//}}AFX_MSG_MAP

	ON_MESSAGE( CWZ_PAGECHG, OnPageChange )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWzSave message handlers

BOOL CWzSave::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_btnChooseFolder.SetIcon( CGrDC::LoadIcon( IDI_FOLDER ) );

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CWzSave::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return -1;

	m_sFolder = pReg->GetSz( "PUBINFO", "Path" );
	if ( m_sFolder.IsEmpty() ) m_sFolder = "C:\\Webcam Images";

	UpdateData( FALSE );

	return 1;
}

void CWzSave::OnChooseFolder() 
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return;
	
	// Get the file name
	char path[ CWF_STRSIZE ] = { 0 };
	strcpy( path, m_sFolder );

	// Get user path
	if ( !CWinFile::GetBrowseDir( GetSafeHwnd(), "File Path", path, path ) )
		return;

	// Save the path
	pReg->Set( "PUBINFO", "Path", path );
	m_sFolder = path;

	UpdateData( FALSE );
}

LRESULT CWzSave::OnPageChange(WPARAM wParam, LPARAM lParam)
{
	if ( wParam != 1 ) return 0;

	UpdateData( TRUE );

	if ( m_sFolder.IsEmpty() )
	{	::MessageBox( GetSafeHwnd(), "You must specify a folder name",
						"No Folder", MB_OK | MB_ICONSTOP );
		return -1;
	} // end if

	// Does the folder exist
	if ( !CWinFile::DoesExist( m_sFolder ) )
	{
		// Does the user want us to create it?
		if ( ::MessageBox(	GetSafeHwnd(), 
							"Folder does not exist!\r\n\r\n"
							"Do you want to create it?",
							"Folder does not exist.",
							MB_YESNO | MB_ICONQUESTION ) != IDYES ) return -1;

		// Create the folder
		if ( !CWinFile::CreateDirectory( m_sFolder ) )
		{	CWinFile::ShowLastError( "Error creating folder" );
			return -1;
		} // end if

	} // end if

	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return 0;

	// Save the folder name
	pReg->Set( "PUBINFO", "Path", m_sFolder );

	return 0;
}
