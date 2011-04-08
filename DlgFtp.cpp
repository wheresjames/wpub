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
// DlgFtp.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgFtp.h"

#include "DlgTreeFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFtp dialog


CDlgFtp::CDlgFtp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFtp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFtp)
	m_sAddress = _T("");
	m_sFolder = _T("");
	m_bPassive = TRUE;
	m_sPassword = _T("");
	m_sUsername = _T("");
	m_sName = _T("");
	m_sHttpPath = _T("");
	m_bRename = FALSE;
	//}}AFX_DATA_INIT

	m_bUpdateHttpPath = FALSE;

	m_images.Create( 16, 16, ILC_MASK | ILC_COLOR24, 0, 32 );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_SERVER ) );		
}


void CDlgFtp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFtp)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_ADDRESS, m_sAddress);
	DDX_Text(pDX, IDC_FOLDER, m_sFolder);
	DDX_Check(pDX, IDC_PASSIVE, m_bPassive);
	DDX_Text(pDX, IDC_PASSWORD, m_sPassword);
	DDX_Text(pDX, IDC_USERNAME, m_sUsername);
	DDX_Text(pDX, IDC_NAME, m_sName);
	DDX_Text(pDX, IDC_HTTPPATH, m_sHttpPath);
	DDX_Check(pDX, IDC_RENAME, m_bRename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFtp, CDialog)
	//{{AFX_MSG_MAP(CDlgFtp)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_EN_CHANGE(IDC_ADDRESS, OnChangeAddress)
	ON_EN_CHANGE(IDC_FOLDER, OnChangeFolder)
	ON_EN_CHANGE(IDC_HTTPPATH, OnChangeHttppath)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_SAVE, OnSave )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFtp message handlers

BOOL CDlgFtp::OnInitDialog() 
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

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgFtp::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	FillListBox();
	UpdateControls();
	return 0;
}

LRESULT CDlgFtp::OnSave(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void CDlgFtp::FillListBox()
{
	m_list.DeleteAllItems();

	char				str[ 256 ];
	HGROUP				hGroup = NULL;
	while ( ( hGroup = FTPSERVERS().GetNext( hGroup ) ) != NULL )
	{
		// Insert the item
		int item = m_list.InsertItem( m_list.GetItemCount(), hGroup->name,0 );

		// Set address text
		if ( item != LB_ERR )
			if ( FTPSERVERS().GetValue( hGroup, "Address", str, sizeof( str ) ) )
				m_list.SetItemText( item, 1, str );

	} // end while

}

void CDlgFtp::OnAdd() 
{
	UpdateData( TRUE );

	// Ensure address
	if ( m_sAddress.IsEmpty() )
	{	::MessageBox( GetSafeHwnd(), "You must specify an address", 
						"Address not specified", MB_OK );
		return;
	} // end if

	// Use address for name if not specified
	if ( m_sName.IsEmpty() ) m_sName = m_sAddress;

	// See if it already exists
	HGROUP hGroup = FTPSERVERS().FindGroup( m_sName );
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
	{	hGroup = FTPSERVERS().AddGroup( m_sName );
		if ( hGroup == NULL ) return;
	} // end else

	// Save server information
	FTPSERVERS().SetValue( hGroup, "Address", m_sAddress );
	FTPSERVERS().SetValue( hGroup, "Folder", m_sFolder );
	FTPSERVERS().SetValue( hGroup, "Username", m_sUsername );
	FTPSERVERS().SetValue( hGroup, "Password", m_sPassword );
	FTPSERVERS().SetValue( hGroup, "HttpPath", m_sHttpPath );
	FTPSERVERS().SetValue( hGroup, "Passive", (DWORD)( ( m_bPassive ) ? 1 : 0 ) );
	FTPSERVERS().SetValue( hGroup, "Rename", (DWORD)( ( m_bRename ) ? 1 : 0 ) );

	FillListBox();
}

void CDlgFtp::OnRemove() 
{
	UpdateData( TRUE );

	// Get the selected item
	int sel = CComCtrl::ListGetFirstSelectedItem( m_list.GetSafeHwnd() );
	if ( sel == LB_ERR ) return;

	// Remove the server
	CString str = m_list.GetItemText( sel, 0 );
	FTPSERVERS().RemoveGroup( str );

	// Refresh list
	FillListBox();

	// Update controls
	UpdateControls();

}

void CDlgFtp::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData( TRUE );

	// Update controls
	UpdateControls();

	DoEnable();

	*pResult = 0;
}

void CDlgFtp::UpdateControls()
{
	// Get the selected item
	HGROUP	hGroup = NULL;
	int		sel = CComCtrl::ListGetFirstSelectedItem( m_list.GetSafeHwnd() );

	if ( sel != LB_ERR )
	{
		// Remove the server
		CString str = m_list.GetItemText( sel, 0 );

		// Find the group
		hGroup = FTPSERVERS().FindGroup( str );

	} // end if

	if ( hGroup != NULL )
	{
		// Get server name
		m_sName = hGroup->name;

		// Read information
		m_sAddress = (LPCTSTR)FTPSERVERS().GetValuePtr( hGroup, "Address" );
		m_sFolder = (LPCTSTR)FTPSERVERS().GetValuePtr( hGroup, "Folder" );
		m_sUsername = (LPCTSTR)FTPSERVERS().GetValuePtr( hGroup, "Username" );
		m_sPassword = (LPCTSTR)FTPSERVERS().GetValuePtr( hGroup, "Password" );
		m_sHttpPath = (LPCTSTR)FTPSERVERS().GetValuePtr( hGroup, "HttpPath" );

		DWORD dw = 1;
		FTPSERVERS().GetValue( hGroup, "Passive", &dw );
		m_bPassive = ( dw != 0 );

		dw = 0;
		FTPSERVERS().GetValue( hGroup, "Rename", &dw );
		m_bRename = ( dw != 0 );

		UpdateData( FALSE );
		
		// Update the path
		UpdateHttpPath();

	} // end if
	else
	{
		m_sName = "";
		m_sAddress = "";
		m_sFolder = "";
		m_sUsername = "";
		m_sPassword = "";
		m_sHttpPath = "";
		m_bPassive = TRUE;
		m_bRename = FALSE;

		UpdateData( FALSE );

	} // end else

	DoEnable();
}

void CDlgFtp::DoEnable()
{
	CWnd *pWnd = GetDlgItem( IDC_NAME );
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

void CDlgFtp::OnChangeName() 
{	DoEnable();
}

void CDlgFtp::OnChangeAddress() 
{	UpdateHttpPath();
}

void CDlgFtp::OnChangeFolder() 
{	UpdateHttpPath();
}

void CDlgFtp::UpdateHttpPath()
{
	UpdateData( TRUE );

	char link[ CWF_STRSIZE ], f1[ CWF_STRSIZE ];

	// Get the domain name
	CAsyncWinSock::GetDomainName( m_sAddress, f1 );
	
	// Build HTTP link
	strcpy( link, "http://www." );
	strcat( link, f1 );
	CWinFile::WebBuildPath( link, link, m_sFolder );

	// Are we updating paths?
	if ( m_sHttpPath.IsEmpty() || !strcmpi( link, m_sHttpPath ) ) 
		m_bUpdateHttpPath = TRUE;

	// Update if needed
	if ( m_bUpdateHttpPath )
	{	m_sHttpPath = link;
		UpdateData( FALSE );
	} // end if
}

void CDlgFtp::OnChangeHttppath() 
{	m_bUpdateHttpPath = FALSE;
	UpdateHttpPath();
}

