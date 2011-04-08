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
// DlgFtpList.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgFtpList.h"

#include "WzFrame.h"
#include "WzFtpPg1.h"
#include "WzFtpPg2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgFtpList dialog


CDlgFtpList::CDlgFtpList(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFtpList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFtpList)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_images.Create( 16, 16, ILC_MASK | ILC_COLOR24, 0, 32 );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_SERVER ) );		
}


void CDlgFtpList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFtpList)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFtpList, CDialog)
	//{{AFX_MSG_MAP(CDlgFtpList)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP

	ON_MESSAGE( CWZ_PAGECHG, OnPageChange )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFtpList message handlers

BOOL CDlgFtpList::OnInitDialog() 
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

LRESULT CDlgFtpList::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	FillList();

	return 1;
}

void CDlgFtpList::OnAdd() 
{
	CWzFrame dlg( IDD_WZFTP_FRAME );

	// Set reg pointer
	CRKey rk;
	dlg.SetData( &rk );

	char helppath[ CWF_STRSIZE ];
	GetModuleFileName( NULL, helppath, sizeof( helppath ) );
	CWinFile::GetPathFromFileName( helppath, helppath );
	CWinFile::BuildPath( helppath, helppath, "Help" );
	dlg.SetHelpRoot( helppath );

	// Add Pages to wizard
	dlg.SetPage( 0, new CWzFtpPg1(), CWzFtpPg1::IDD, "FTP Server Information", "wizard_ftp.htm" );
	dlg.SetPage( 1, new CWzFtpPg2(), CWzFtpPg2::IDD, "FTP Server Settings", "wizard_ftp.htm" );
	
	HGROUP hGroup = NULL;
	char server[ CWF_STRSIZE ];
	do
	{
		// Run the wizard
		if ( dlg.DoModal() != IDOK ) return;

		// Get server name
		if ( !rk.Get( "ServerName", server, sizeof( server ) ) || *server == 0 )
			return;
		
		// Check for existing server
		hGroup = FTPSERVERS().FindGroup( server );

	// Does it exist?
	} while ( hGroup != NULL && 
			::MessageBox(	GetSafeHwnd(), 
							"Server Name exists!\r\n\r\nOverwrite?", 
							"Overwrite Settings?",
							MB_YESNO | MB_ICONQUESTION ) != IDYES );

	// Copy new server settings
	FTPSERVERS().Copy( server, &rk );

	FillList();
}

void CDlgFtpList::OnRemove() 
{
	UpdateData( TRUE );

	// Get the selected item
	int sel = CComCtrl::ListGetFirstSelectedItem( m_list.GetSafeHwnd() );
	if ( sel == LB_ERR ) return;

	// Remove the server
	CString str = m_list.GetItemText( sel, 0 );

	// Ask user to verify
	char msg[ CWF_STRSIZE + 256 ];
	wsprintf( msg, "Delete FTP Server %s?", str );
	if ( ::MessageBox( GetSafeHwnd(), msg, "Remove FTP Server", MB_YESNO ) != IDYES )
		return;

	// Remove the server
	FTPSERVERS().RemoveGroup( str );

	// Refresh list
	FillList();
}

BOOL CDlgFtpList::FillList()
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

	return TRUE;
}

void CDlgFtpList::OnEdit() 
{
	CWzFrame dlg( IDD_WZFTP_FRAME );

	UpdateData( TRUE );

	// Get the selected item
	int sel = CComCtrl::ListGetFirstSelectedItem( m_list.GetSafeHwnd() );
	if ( sel == LB_ERR ) return;

	// Remove the server
	CString srv = m_list.GetItemText( sel, 0 );

	// Ensure group exists
	if ( FTPSERVERS().FindGroup( srv ) == NULL )
		return;

	// Set reg pointer
	CRKey rk;
	dlg.SetData( &rk );

	// Copy current server settings
	rk.Copy( &FTPSERVERS(), srv );
	rk.Set( "ServerName", srv );

	char helppath[ CWF_STRSIZE ];
	GetModuleFileName( NULL, helppath, sizeof( helppath ) );
	CWinFile::GetPathFromFileName( helppath, helppath );
	CWinFile::BuildPath( helppath, helppath, "Help" );
	dlg.SetHelpRoot( helppath );

	// Add Pages to wizard
	dlg.SetPage( 0, new CWzFtpPg1(), CWzFtpPg1::IDD, "FTP Server Information", "wizard_ftp.htm" );
	dlg.SetPage( 1, new CWzFtpPg2(), CWzFtpPg2::IDD, "FTP Server Settings", "wizard_ftp.htm" );
	
	// Run the wizard
	if ( dlg.DoModal() != IDOK ) return;

	// Get server name
	char server[ CWF_STRSIZE ];
	if ( !rk.Get( "ServerName", server, sizeof( server ) ) || *server == 0 )
		return;
	
	// Delete old server entry
	FTPSERVERS().RemoveGroup( srv );

	// Copy new server settings
	FTPSERVERS().Copy( server, &rk );

	FillList();
}

void CDlgFtpList::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdit();	
	*pResult = 0;
}


LRESULT CDlgFtpList::OnPageChange(WPARAM wParam, LPARAM lParam)
{
	if ( wParam != 1 ) return 0;

	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return 0;

	// Get the selected item
	CString srv;
	int sel = CComCtrl::ListGetFirstSelectedItem( m_list.GetSafeHwnd() );
	if ( sel != LB_ERR ) srv = m_list.GetItemText( sel, 0 );
	if ( sel == LB_ERR || srv.IsEmpty() )
	{	::MessageBox(	GetSafeHwnd(), "You must select an FTP Server",
						"No Server", MB_OK | MB_ICONSTOP ); 
		return 1; 
	} // end if

	// Save server value
	pReg->Set( "CDlgFtpList", "Server", srv );	

	return 0;
}
