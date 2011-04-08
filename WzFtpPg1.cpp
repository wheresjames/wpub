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
// WzFtpPg1.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "WzFtpPg1.h"

#include "WzFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWzFtpPg1 dialog


CWzFtpPg1::CWzFtpPg1(CWnd* pParent /*=NULL*/)
	: CDialog(CWzFtpPg1::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWzFtpPg1)
	m_sAddr = _T("");
	m_sFolder = _T("");
	m_sHttpPath = _T("");
	m_sName = _T("");
	//}}AFX_DATA_INIT

	m_bUpdateHttpPath = FALSE;

}


void CWzFtpPg1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWzFtpPg1)
	DDX_Text(pDX, IDC_ADDR, m_sAddr);
	DDX_Text(pDX, IDC_FOLDER, m_sFolder);
	DDX_Text(pDX, IDC_HTTPPATH, m_sHttpPath);
	DDX_Text(pDX, IDC_NAME, m_sName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWzFtpPg1, CDialog)
	//{{AFX_MSG_MAP(CWzFtpPg1)
	ON_EN_CHANGE(IDC_ADDR, OnChangeAddr)
	ON_EN_CHANGE(IDC_FOLDER, OnChangeFolder)
	ON_EN_CHANGE(IDC_HTTPPATH, OnChangeHttppath)
	//}}AFX_MSG_MAP

	ON_MESSAGE( CWZ_PAGECHG, OnPageChange )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWzFtpPg1 message handlers

BOOL CWzFtpPg1::OnInitDialog() 
{
	CDialog::OnInitDialog();

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CWzFtpPg1::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	CRKey *pRk = (CRKey*)CWzFrame::GetData( GetSafeHwnd() );
	if ( pRk == NULL ) return -1;

	// Load values
	m_sName = pRk->GetSz( "ServerName" );
	m_sAddr = pRk->GetSz( "Address" ); 
	m_sFolder = pRk->GetSz( "Folder" );
	m_sHttpPath = pRk->GetSz( "HttpPath" );

	// Create a unique group name if needed
	if ( m_sName.IsEmpty() )
	{
		DWORD i = 1;
		char name[ CWF_STRSIZE ];
		do
		{	wsprintf( name, "FTP Server %lu", i++ );
		} while ( FTPSERVERS().FindGroup( name ) != NULL );
		m_sName = name;

	} // end if

	UpdateData( FALSE );

	// Verify HTTP Path
	UpdateHttpPath( TRUE );

	return 1;
}

void CWzFtpPg1::UpdateHttpPath( BOOL bVerify )
{
	UpdateData( TRUE );

	char link[ CWF_STRSIZE ], f1[ CWF_STRSIZE ];

	// Get the domain name
	CAsyncWinSock::GetDomainName( m_sAddr, f1 );
	
	// Build HTTP link
	strcpy( link, "http://www." );
	strcat( link, f1 );

	// Search for html root indicators
	BOOL skip = FALSE;
	LPCTSTR s = m_sFolder;
	DWORD i = 0;
	for (	i = 0; 
			s[ i ] != 0 && s[ i ] != '\\' && s[ i ] != '/';
			s++ )
	{
		if (	!strnicmp( &s[ i ], "html", 4 ) ||
				!strnicmp( &s[ i ], "doc", 3 ) ) skip = TRUE;
	} // end for

	// Skip dividers
	if ( skip ) while ( s[ i ] == '/' || s[ i ] == '\\' ) s++;
	else s = m_sFolder;

	// Build HTTP link
	if ( *s != 0 ) CWinFile::WebBuildPath( link, link, s );
	
	if ( bVerify ) 
	{	if ( m_sHttpPath.IsEmpty() ) 
		{	m_sHttpPath = link, m_bUpdateHttpPath = TRUE; }
		else m_bUpdateHttpPath = !strcmpi( link, m_sHttpPath ); 
		UpdateData( FALSE );
		return; 
	} // end if

	// Are we updating paths?
	if ( m_sHttpPath.IsEmpty() || !strcmpi( link, m_sHttpPath ) ) 
		m_bUpdateHttpPath = TRUE;

	// Update if needed
	if ( m_bUpdateHttpPath )
	{	m_sHttpPath = link;
		UpdateData( FALSE );
	} // end if
}

void CWzFtpPg1::OnChangeAddr() 
{	UpdateHttpPath();
}

void CWzFtpPg1::OnChangeFolder() 
{	UpdateHttpPath();
}

void CWzFtpPg1::OnChangeHttppath() 
{	m_bUpdateHttpPath = FALSE;
	UpdateHttpPath();
}

LRESULT CWzFtpPg1::OnPageChange(WPARAM wParam, LPARAM lParam)
{
	if ( wParam != 1 ) return 0;

	UpdateData( TRUE );

	CRKey *pRk = (CRKey*)CWzFrame::GetData( GetSafeHwnd() );
	if ( pRk == NULL ) return 0;

	pRk->Set( "Address", m_sAddr );	
	pRk->Set( "HttpPath", m_sHttpPath );	
	pRk->Set( "ServerName", m_sName );	
	pRk->Set( "Folder", m_sFolder );	

	if ( m_sName.IsEmpty() )
	{	::MessageBox(	GetSafeHwnd(), "Server Name cannot be blank",
		"Blank Field", MB_OK | MB_ICONSTOP ); return 1; }

	if ( m_sAddr.IsEmpty() )
	{	::MessageBox(	GetSafeHwnd(), "Server Address cannot be blank",
		"Blank Field", MB_OK | MB_ICONSTOP ); return 1; }

	if ( m_sHttpPath.IsEmpty() )
	{	::MessageBox(	GetSafeHwnd(), "HTTP Path cannot be blank",
		"Blank Field", MB_OK | MB_ICONSTOP ); return 1; }

	return 0;
}
