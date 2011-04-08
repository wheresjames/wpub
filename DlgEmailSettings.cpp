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
// DlgEmailSettings.cpp : implementation file
//

#include "stdafx.h"
#include "Vp.h"
#include "DlgEmailSettings.h"

#include "DlgTreeFrame.h"

#include "SmtpClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgEmailSettings dialog


CDlgEmailSettings::CDlgEmailSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEmailSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEmailSettings)
	m_sServer = _T("");
	m_sUsername = _T("");
	m_sPassword = _T("");
	m_bLogin = FALSE;
	m_bAuthDetect = TRUE;
	m_sName = _T("");
	//}}AFX_DATA_INIT
	m_images.Create( 16, 16, ILC_MASK | ILC_COLOR24, 0, 32 );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_SMALL_LETTER ) );		
}


void CDlgEmailSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEmailSettings)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_AUTHTYPE, m_comboAuthType);
	DDX_Text(pDX, IDC_SERVER, m_sServer);
	DDX_Text(pDX, IDC_USERNAME, m_sUsername);
	DDX_Text(pDX, IDC_PASSWORD, m_sPassword);
	DDX_Check(pDX, IDC_LOGIN, m_bLogin);
	DDX_Check(pDX, IDC_AUTHDETECT, m_bAuthDetect);
	DDX_Text(pDX, IDC_NAME, m_sName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEmailSettings, CDialog)
	//{{AFX_MSG_MAP(CDlgEmailSettings)
	ON_BN_CLICKED(IDC_AUTHDETECT, OnAuthdetect)
	ON_BN_CLICKED(IDC_LOGIN, OnLogin)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_SAVE, OnSave )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEmailSettings message handlers

BOOL CDlgEmailSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	RECT rect;
	m_list.GetClientRect( &rect );
	long w = ( rect.right - rect.left ) - GetSystemMetrics( SM_CXVSCROLL ) - 4;
	m_list.InsertColumn( 0, "Name", LVCFMT_LEFT, w >> 1 );
	m_list.InsertColumn( 1, "Address", LVCFMT_LEFT, w >> 1 );

	ListCtrl_SetExtendedListViewStyle( m_list.GetSafeHwnd(), LVS_EX_FULLROWSELECT );

	m_list.SetImageList( &m_images, LVSIL_NORMAL );
	m_list.SetImageList( &m_images, LVSIL_SMALL );

	FillCombo();

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgEmailSettings::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	FillListBox();
	UpdateControls();
	return 0;
/*
	UpdateData( TRUE );

	char	temp[ 1024 ];
	DWORD	dwval;

	if ( CFG().GetValue( "Settings", "EmailServer", temp, sizeof( temp ) ) )
		m_sServer = temp;

	if ( CFG().GetValue( "Settings", "EmailUsername", temp, sizeof( temp ) ) )
		m_sUsername = temp;

	if ( CFG().GetValue( "Settings", "EmailPassword", temp, sizeof( temp ) ) )
		m_sPassword = temp;
	
	if ( CFG().GetValue( "Settings", "EmailLogin", &dwval ) )
		m_bLogin = ( dwval != 0 );
	
	if ( CFG().GetValue( "Settings", "EmailAuthDetect", &dwval ) )
		m_bAuthDetect = ( dwval != 0 );
	
	CFG().GetValue( "Settings", "EmailAuthType", &m_dwAuthType );

	UpdateData( FALSE );

	FillCombo();

	DoEnable();
*/
}

LRESULT CDlgEmailSettings::OnSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );
	return 0;
/*
	int sel = m_comboAuthType.GetCurSel();
	if ( sel != CB_ERR ) m_dwAuthType = m_comboAuthType.GetItemData( sel );

	CFG().SetValue( "Settings", "EmailServer", m_sServer );
	CFG().SetValue( "Settings", "EmailUsername", m_sUsername );
	CFG().SetValue( "Settings", "EmailPassword", m_sPassword );
	CFG().SetValue( "Settings", "EmailLogin", (DWORD)( m_bLogin ? 1 : 0 ) );	
	CFG().SetValue( "Settings", "EmailAuthDetect", (DWORD)( m_bAuthDetect ? 1 : 0 ) );
	CFG().SetValue( "Settings", "EmailAuthType", m_dwAuthType );

	UpdateData( FALSE );
*/
}

void CDlgEmailSettings::DoEnable()
{
	UpdateData( TRUE );
	
	CWnd *pWnd = GetDlgItem( IDC_USERNAME );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bLogin );
	pWnd = GetDlgItem( IDC_PASSWORD );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bLogin );

	pWnd = GetDlgItem( IDC_AUTHDETECT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bLogin );
	pWnd = GetDlgItem( IDC_AUTHTYPE );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bLogin && !m_bAuthDetect );


	pWnd = GetDlgItem( IDC_NAME );
	if ( pWnd != NULL ) pWnd->GetWindowText( m_sName );
	
	int sel = CComCtrl::ListGetFirstSelectedItem( m_list.GetSafeHwnd() );
	if ( sel == -1 ) sel = LB_ERR;

	pWnd = GetDlgItem( IDC_ADD );
	CString str;
	if ( sel != LB_ERR ) str = m_list.GetItemText( sel, 0 );
	if ( !str.IsEmpty() && !strcmpi( m_sName, str ) ) 
		pWnd->SetWindowText( "Edit" );
	else pWnd->SetWindowText( "Add" );

	pWnd = GetDlgItem( IDC_REMOVE );
	if ( pWnd != NULL ) pWnd->EnableWindow( sel != LB_ERR );

	pWnd = GetDlgItem( IDC_ADD );
	if ( pWnd != NULL ) pWnd->EnableWindow( !m_sName.IsEmpty() );

}

void CDlgEmailSettings::FillCombo()
{
	// Empty list
	m_comboAuthType.ResetContent();

	// Setup combo
	int i = m_comboAuthType.AddString( "Plain Text (PLAIN)" );
	if ( i != CB_ERR )
	{	m_comboAuthType.SetItemData( i, SMTPAUTH_PLAIN );
		if ( m_dwAuthType == SMTPAUTH_PLAIN ) m_comboAuthType.SetCurSel( i );
	} // end if
	i = m_comboAuthType.AddString( "Base64 (LOGIN)" );
	if ( i != CB_ERR )
	{	m_comboAuthType.SetItemData( i, SMTPAUTH_LOGIN );
		if ( m_dwAuthType == SMTPAUTH_LOGIN ) m_comboAuthType.SetCurSel( i );
	} // end if
	i = m_comboAuthType.AddString( "Cram-MD5 (CRAM-MD5)" );
	if ( i != CB_ERR )
	{	m_comboAuthType.SetItemData( i, SMTPAUTH_CRAMMD5 );
		if ( m_dwAuthType == SMTPAUTH_CRAMMD5 ) m_comboAuthType.SetCurSel( i );
	} // end if
}

void CDlgEmailSettings::OnAuthdetect() 
{	DoEnable();
}

void CDlgEmailSettings::OnLogin() 
{	DoEnable();
}

void CDlgEmailSettings::FillListBox()
{
	m_list.DeleteAllItems();

	char				str[ 256 ];
	HGROUP				hGroup = NULL;
	while ( ( hGroup = EMAILSERVERS().GetNext( hGroup ) ) != NULL )
	{
		// Insert the item
		int item = m_list.InsertItem( m_list.GetItemCount(), hGroup->name,0 );

		// Set address text
		if ( item != LB_ERR )
			if ( EMAILSERVERS().GetValue( hGroup, "Address", str, sizeof( str ) ) )
				m_list.SetItemText( item, 1, str );

	} // end while
}

void CDlgEmailSettings::UpdateControls()
{
	// Get the selected item
	HGROUP	hGroup = NULL;
	int		sel = CComCtrl::ListGetFirstSelectedItem( m_list.GetSafeHwnd() );

	if ( sel != LB_ERR )
	{
		// Remove the server
		CString str = m_list.GetItemText( sel, 0 );

		// Find the group
		hGroup = EMAILSERVERS().FindGroup( str );

	} // end if

	if ( hGroup != NULL )
	{
		// Get server name
		m_sName = hGroup->name;

		// Read information
		m_sServer = (LPCTSTR)EMAILSERVERS().GetValuePtr( hGroup, "Address" );
		m_sUsername = (LPCTSTR)EMAILSERVERS().GetValuePtr( hGroup, "Username" );
		m_sPassword = (LPCTSTR)EMAILSERVERS().GetValuePtr( hGroup, "Password" );

		DWORD dw = 0;
		EMAILSERVERS().GetValue( hGroup, "Login", &dw );
		m_bLogin = ( dw != 0 );

		dw = 1;
		EMAILSERVERS().GetValue( hGroup, "AuthDetect", &dw );
		m_bAuthDetect = ( dw != 0 );

		m_dwAuthType = 0;
		EMAILSERVERS().GetValue( hGroup, "AuthType", &m_dwAuthType );

	} // end if
	else
	{
		m_sName = "";
		m_sServer = "";
		m_sUsername = "";
		m_sPassword = "";
		m_bLogin = FALSE;
		m_bAuthDetect = TRUE;
		m_dwAuthType = 0;

	} // end else

	UpdateData( FALSE );

	DoEnable();
}

void CDlgEmailSettings::OnAdd() 
{
	UpdateData( TRUE );

	// Ensure address
	if ( m_sServer.IsEmpty() )
	{	::MessageBox( GetSafeHwnd(), "You must specify an SMTP server address", 
						"Address not specified", MB_OK );
		return;
	} // end if

	// Use address for name if not specified
	if ( m_sName.IsEmpty() ) m_sName = m_sServer;

	// See if it already exists
	HGROUP hGroup = EMAILSERVERS().FindGroup( m_sName );
	if ( hGroup != NULL )
	{
		if ( ::MessageBox( GetSafeHwnd(),	"A server with this name already"
											" exists.\r\n\r\n"
											"Do you want to overwrite?",
											"Overwrite Server Information?",
											MB_YESNO ) != IDYES ) return;
	} // end if

	// Create group if needed
	else
	{	hGroup = EMAILSERVERS().AddGroup( m_sName );
		if ( hGroup == NULL ) return;
	} // end else

	// Save server information
	EMAILSERVERS().SetValue( hGroup, "Address", m_sServer );
	EMAILSERVERS().SetValue( hGroup, "Username", m_sUsername );
	EMAILSERVERS().SetValue( hGroup, "Password", m_sPassword );
	EMAILSERVERS().SetValue( hGroup, "Login", (DWORD)( ( m_bLogin ) ? 1 : 0 ) );
	EMAILSERVERS().SetValue( hGroup, "AuthDetect", (DWORD)( ( m_bAuthDetect ) ? 1 : 0 ) );
	EMAILSERVERS().SetValue( hGroup, "AuthType", m_dwAuthType );

	FillListBox();
}

void CDlgEmailSettings::OnRemove() 
{
	UpdateData( TRUE );

	// Get the selected item
	int sel = CComCtrl::ListGetFirstSelectedItem( m_list.GetSafeHwnd() );
	if ( sel == LB_ERR ) return;

	// Remove the server
	CString str = m_list.GetItemText( sel, 0 );
	EMAILSERVERS().RemoveGroup( str );

	// Refresh list
	FillListBox();

	// Update controls
	UpdateControls();
}

void CDlgEmailSettings::OnChangeName() 
{	DoEnable();
}

void CDlgEmailSettings::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData( TRUE );

	// Update controls
	UpdateControls();

	*pResult = 0;
}
