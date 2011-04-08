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
// DlgServer.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgServer.h"
#include "MainFrm.h"

#include "DlgTreeFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgServer dialog


CDlgServer::CDlgServer(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgServer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgServer)
	m_sMaxUsers = _T("");
	m_sPort = _T("");
	m_sAddress = _T("");
	m_sRemotePage = _T("");
	m_sRemotePassword = _T("");
	m_sHttpPassword = _T("");
	m_bRemote = FALSE;
	m_bHttp = FALSE;
	m_bHttpEnable = FALSE;
	//}}AFX_DATA_INIT

}


void CDlgServer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgServer)
	DDX_Control(pDX, IDC_PORT, m_comboPort);
	DDX_Text(pDX, IDC_MAXUSERS, m_sMaxUsers);
	DDX_CBString(pDX, IDC_PORT, m_sPort);
	DDX_Text(pDX, IDC_ADDRESS, m_sAddress);
	DDX_Text(pDX, IDC_REMOTE_PAGE, m_sRemotePage);
	DDX_Text(pDX, IDC_REMOTE_PASSWORD, m_sRemotePassword);
	DDX_Text(pDX, IDC_HTTP_PASSWORD, m_sHttpPassword);
	DDX_Check(pDX, IDC_REMOTE, m_bRemote);
	DDX_Check(pDX, IDC_HTTP, m_bHttp);
	DDX_Check(pDX, IDC_ENABLEHTTP, m_bHttpEnable);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgServer, CDialog)
	//{{AFX_MSG_MAP(CDlgServer)
	ON_EN_CHANGE(IDC_MAXUSERS, OnChangeMaxusers)
	ON_CBN_EDITCHANGE(IDC_PORT, OnEditchangePort)
	ON_CBN_SELCHANGE(IDC_PORT, OnSelchangePort)
	ON_BN_CLICKED(IDC_HTTP, OnHttp)
	ON_BN_CLICKED(IDC_REMOTE, OnRemote)
	ON_EN_CHANGE(IDC_HTTP_PASSWORD, OnChangeHttpPassword)
	ON_BN_CLICKED(IDC_ENABLEHTTP, OnEnablehttp)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_SAVE, OnSave )
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgServer message handlers

BOOL CDlgServer::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_comboPort.AddString( "80" );

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgServer::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	char	sz[ 256 ];
	DWORD	dw = 0;

	dw = 80;
	CFG().GetValue( "Settings", "WebServerPort", &dw );
	if ( dw > 65535 ) dw = 65535;
	wsprintf( sz, "%lu", dw );
	m_sPort = sz;	

	dw = 16;
	CFG().GetValue( "Settings", "WebServerMaxUsers", &dw );
	if ( dw > 65535 ) dw = 65535;
	wsprintf( sz, "%lu", dw );
	m_sMaxUsers = sz;	

	dw = 0;
	CFG().GetValue( "Settings", "WebServerRemote", &dw );
	m_bRemote = ( dw != 0 );

	*sz = 0;
	CFG().GetValue( "Settings", "WebServerRemotePassword", sz, sizeof( sz ) );
	m_sRemotePassword = sz;

	*sz = 0;
	CFG().GetValue( "Settings", "WebServerRemotePage", sz, sizeof( sz ) );
	if ( *sz == 0 ) strcpy( sz, "admin" );
	m_sRemotePage = sz;

	dw = 1;
	CFG().GetValue( "Settings", "WebServerHttpEnable", &dw );
	m_bHttpEnable = ( dw != 0 );

	dw = 0;
	CFG().GetValue( "Settings", "WebServerHttp", &dw );
	m_bHttp = ( dw != 0 );

	*sz = 0;
	CFG().GetValue( "Settings", "WebServerHttpPassword", sz, sizeof( sz ) );
	m_sHttpPassword = sz;
	
	UpdateData( FALSE );

	ShowAddress();

	DoEnable();
	return 0;
}

LRESULT CDlgServer::OnSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );

	// Stop the server if it's running
	BOOL bRestart = FALSE;
	if ( FRAME()->m_iserver.IsRunning() )
	{	bRestart = TRUE; FRAME()->StopServer(); }

	DWORD	dw;

	dw = strtoul( m_sPort, NULL, 10 );
	CFG().SetValue( "Settings", "WebServerPort", dw );

	dw = strtoul( m_sMaxUsers, NULL, 10 );
	CFG().SetValue( "Settings", "WebServerMaxUsers", dw );

	CFG().SetValue( "Settings", "WebServerRemote", (DWORD)( m_bRemote ? 1 : 0 ) );
	CFG().SetValue( "Settings", "WebServerRemotePassword", m_sRemotePassword );
	CFG().SetValue( "Settings", "WebServerRemotePage", m_sRemotePage );

	CFG().SetValue( "Settings", "WebServerHttpEnable", (DWORD)( m_bHttpEnable ? 1 : 0 ) );
	CFG().SetValue( "Settings", "WebServerHttp", (DWORD)( m_bHttp ? 1 : 0 ) );
	CFG().SetValue( "Settings", "WebServerHttpPassword", m_sHttpPassword );

	// Restart server if needed
	if ( bRestart ) FRAME()->StartServer();

	return 0;
}


void CDlgServer::OnChangeMaxusers() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}

void CDlgServer::OnEditchangePort() 
{	ShowAddress();
	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}

void CDlgServer::OnSelchangePort() 
{
	// Update selection
	int sel = m_comboPort.GetCurSel();
	if ( sel == CB_ERR ) return;
	m_comboPort.GetLBText( sel, m_sPort );
	m_comboPort.SetWindowText( m_sPort );

	ShowAddress();
	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}

void CDlgServer::ShowAddress()
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


void CDlgServer::DoEnable()
{
	UpdateData( TRUE );

	CWnd *pWnd = GetDlgItem( IDC_REMOTE_PASSWORD );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bRemote );
	pWnd = GetDlgItem( IDC_REMOTE_PAGE );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bRemote );
	pWnd = GetDlgItem( IDC_REMOTE_PASSWORD_TXT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bRemote );
	pWnd = GetDlgItem( IDC_PAGE_NAME_TXT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bRemote );

	pWnd = GetDlgItem( IDC_HTTP );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bHttpEnable );
	pWnd = GetDlgItem( IDC_HTTP_PASSWORD );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bHttpEnable && m_bHttp );
	pWnd = GetDlgItem( IDC_HTTP_PASSWORD_TXT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bHttpEnable && m_bHttp );

}

void CDlgServer::OnHttp() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
	DoEnable();
}

void CDlgServer::OnRemote() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
	DoEnable();
}

void CDlgServer::OnChangeHttpPassword() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}

void CDlgServer::OnEnablehttp() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
	DoEnable();
}
