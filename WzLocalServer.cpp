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
// WzLocalServer.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "WzLocalServer.h"

#include "WzFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWzLocalServer dialog


CWzLocalServer::CWzLocalServer(CWnd* pParent /*=NULL*/)
	: CDialog(CWzLocalServer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWzLocalServer)
	m_sPort = _T("");
	m_sHttpPassword = _T("");
	m_sFilename = _T("");
	m_sRootDir = _T("");
	m_sAddress = _T("");
	//}}AFX_DATA_INIT
}


void CWzLocalServer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWzLocalServer)
	DDX_Control(pDX, IDC_CHOOSE_ROOTDIR, m_chooserootdir);
	DDX_Control(pDX, IDC_PORT, m_comboPort);
	DDX_CBString(pDX, IDC_PORT, m_sPort);
	DDX_Text(pDX, IDC_HTTPPASSWORD, m_sHttpPassword);
	DDX_Text(pDX, IDC_FILENAME, m_sFilename);
	DDX_Text(pDX, IDC_ROOTDIR, m_sRootDir);
	DDX_Text(pDX, IDC_ADDRESS, m_sAddress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWzLocalServer, CDialog)
	//{{AFX_MSG_MAP(CWzLocalServer)
	ON_BN_CLICKED(IDC_CHOOSE_ROOTDIR, OnChooseRootdir)
	ON_CBN_EDITCHANGE(IDC_PORT, OnEditchangePort)
	ON_CBN_SELCHANGE(IDC_PORT, OnSelchangePort)
	//}}AFX_MSG_MAP

	ON_MESSAGE( CWZ_PAGECLOSE, OnSave )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWzLocalServer message handlers

BOOL CWzLocalServer::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_chooserootdir.SetIcon( CGrDC::LoadIcon( IDI_FOLDER ) );

	m_comboPort.AddString( "80" );

	OnRefresh( 0, 0L );	

	ShowAddress();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CWzLocalServer::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	// Get pointer to reg
	CReg *pReg = (CReg*)CWzFrame::GetData( GetSafeHwnd() );

	char	sz[ 256 ];
	DWORD	dw = 0;
	
	if ( pReg != NULL ) m_sRootDir = pReg->GetSz( "CWzLocalServer", "RootDir" );
	if ( m_sRootDir.IsEmpty() )
	{
		// Get the root directory
		m_sRootDir.Empty();
		CRKey *pRk = WEBSETTINGS().GetKey( "FolderMap" );
		if ( pRk != NULL )
		{	LPREGVALUE	prv = NULL;
			while ( m_sRootDir.IsEmpty() &&
					( prv = (LPREGVALUE)pRk->GetNext( prv ) ) != NULL )
				if (	*prv->cpkey == 0 || !strcmp( prv->cpkey, "/" ) ||
						!strcmp( prv->cpkey, "\\" ) ) m_sRootDir = (char*)prv->data;
		} // end if

		// Set default web site directory
		if ( m_sRootDir.IsEmpty() ) m_sRootDir = "C:\\MyWebSite";

	} // end if

	dw = 80;
	if ( pReg == NULL || !pReg->Get( "CWzLocalServer", "WebServerPort", &dw ) )	
		CFG().GetValue( "Settings", "WebServerPort", &dw );

	if ( dw > 65535 ) dw = 65535;
	wsprintf( sz, "%lu", dw );
	m_sPort = sz;	

	*sz = 0;
	if ( pReg == NULL || !pReg->Get( "CWzLocalServer", "WebServerHttpPassword", sz, sizeof( sz ) ) )
		CFG().GetValue( "Settings", "WebServerHttpPassword", sz, sizeof( sz ) );
	m_sHttpPassword = sz;

	if ( pReg != NULL )
	{	
		m_sFilename = pReg->GetSz( "CWzLocalServer", "Filename" );
		if ( m_sFilename.IsEmpty() )
		{
			char text[ CWF_STRSIZE ] = { 0 };
			
			if ( pReg->Get( "CWzCam", "Name", text, sizeof( text ) ) )
			{
				char fname[ CWF_STRSIZE ] = { 0 };

				DWORD x = 0;
				for ( DWORD i = 0; text[ i ] != 0 && i < sizeof( fname ); i++ )
					if ( text[ i ] > ' ' && text[ i ] <= '~' )
						fname[ x++ ] = text[ i ];

				fname[ x ] = 0;
				strcat( fname, ".jpg" );
				CWinFile::CleanChars( fname );

				// Set the filename
				m_sFilename = fname;

			} // end if

			else m_sFilename += "webcam.jpg";

		} // end if

	} // end if

	UpdateData( FALSE );

	return 0;
}

LRESULT CWzLocalServer::OnSave(WPARAM wParam, LPARAM lParam)
{
	// Get pointer to reg
	CReg *pReg = (CReg*)CWzFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return 0;
	UpdateData( TRUE );

	// Save data
	pReg->Set( "CWzLocalServer", "RootDir", (LPCTSTR)m_sRootDir );
	pReg->Set( "CWzLocalServer", "WebServerPort", strtoul( m_sPort, NULL, 10 ) );
	pReg->Set( "CWzLocalServer", "WebServerHttpPassword", m_sHttpPassword );
	pReg->Set( "CWzLocalServer", "Filename", m_sFilename );

	return 0;
}

void CWzLocalServer::OnChooseRootdir() 
{
	UpdateData( TRUE );

	char dir[ CWF_STRSIZE ];
	strcpy( dir, m_sRootDir );

	// Choose a folder
	if ( !CWinFile::GetBrowseDir(	GetSafeHwnd(), "Choose Root Folder",
									dir, dir ) ) return;

	m_sRootDir = dir;

	UpdateData( FALSE );

}

void CWzLocalServer::OnEditchangePort() 
{
	ShowAddress();
}

void CWzLocalServer::OnSelchangePort() 
{
	// Update selection
	int sel = m_comboPort.GetCurSel();
	if ( sel == CB_ERR ) return;
	m_comboPort.GetLBText( sel, m_sPort );
	m_comboPort.SetWindowText( m_sPort );

	ShowAddress();
}

void CWzLocalServer::ShowAddress()
{
	DWORD	port = 80;
	char	strport[ 256 ] = { 0 };

	CComboBox *pCombo = (CComboBox*)GetDlgItem( IDC_PORT );
	if ( pCombo != NULL )
	{
		// Get the text
		pCombo->GetWindowText( m_sPort );

		// Get port address
		port = strtoul( m_sPort, NULL, 10 );
		if ( port > 65535 ) port = 65535;
		wsprintf( strport, "%lu", port );

	} // end if

	char name[ CWF_STRSIZE ] = { '~', 0x0 };
	if ( !gethostname( name, CWF_STRSIZE - 1 ) )
	{
		m_sAddress = "http://";
		m_sAddress += name;
		if ( port != 80 ) m_sAddress += ":", m_sAddress += strport;
		m_sAddress += "/";

		LPHOSTENT he = gethostbyname( name );
		if ( he != NULL )
		{
			m_sAddress += "\r\nhttp://";
			m_sAddress += inet_ntoa( *(LPIN_ADDR) *( he->h_addr_list ) );
			if ( port != 80 ) m_sAddress += ":", m_sAddress += strport;
			m_sAddress += "/";
		} // end if

	} // end if

	// Update the window
	CWnd *pWnd = GetDlgItem( IDC_ADDRESS );
	if ( pWnd != NULL )
	{	pWnd->SetWindowText( m_sAddress );
		pWnd->RedrawWindow();
	} // end if
}

