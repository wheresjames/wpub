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
// DlgPublishing.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgPublishing.h"

#include "MainFrm.h"

#include "DlgTabFrame.h"
#include "PgPubWhat.h"
#include "PgPubWhere.h"
#include "PgPubWhen.h"
#include "PgPubHow.h"
#include "PgPubThumbnail.h"
#include "PgPubSounds.h"
#include "PgPubExtra.h"

#include "PubImgList.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPublishing dialog


CDlgPublishing::CDlgPublishing(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPublishing::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPublishing)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgPublishing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPublishing)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPublishing, CDialog)
	//{{AFX_MSG_MAP(CDlgPublishing)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPublishing message handlers

BOOL CDlgPublishing::OnInitDialog() 
{
	CDialog::OnInitDialog();

	RECT rect;
	m_list.GetClientRect( &rect );
	long w = ( rect.right - rect.left ) - GetSystemMetrics( SM_CXVSCROLL ) - 4;
	m_list.InsertColumn( 0, "Image", LVCFMT_LEFT, w >> 1 );
	m_list.InsertColumn( 1, "Publish To", LVCFMT_LEFT, w >> 1 );

	ListCtrl_SetExtendedListViewStyle( m_list.GetSafeHwnd(), LVS_EX_FULLROWSELECT );

	m_images.Create( 16, 16, ILC_MASK | ILC_COLOR24, 0, 32 );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_FTP ) );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_SMALL_LETTER ) );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_BLUEDISK ) );
		
	m_list.SetImageList( &m_images, LVSIL_NORMAL );
	m_list.SetImageList( &m_images, LVSIL_SMALL );

	OnRefresh( 0, 0L );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPublishing::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	DoEnable();

	FillList();
}

void CDlgPublishing::OnAdd() 
{
	CDlgTabFrame dlg( IDD_PUBFRAME, this );

	PUBINFO pi; ZeroMemory( &pi, sizeof( pi ) );
	CPubList::Default( &pi );
	pi.pemi = new EMAILMSGINFO;
	if ( pi.pemi == NULL ) return;

	// Run Dialog
	if ( !PubDlg( &pi ) ) { delete pi.pemi; return; }

	// Add the image
	PUBLIST().Add( &pi );	
	
	// Lose memory
	CPubList::ReleaseObject( &pi );

	FillList();
}

void CDlgPublishing::OnRemove() 
{
	int sel = CComCtrl::ListGetFirstSelectedItem( m_list.GetSafeHwnd() );
	if ( sel == LB_ERR ) return;

	LPPUBINFO ppi = (LPPUBINFO)m_list.GetItemData( sel );
	if ( ppi == NULL ) return;

	PUBLIST().Delete( ppi );
	FillList();	
}

void CDlgPublishing::FillList()
{
	m_list.DeleteAllItems();

	LPPUBINFO ppi = NULL;

	while ( ( ppi = (LPPUBINFO)PUBLIST().GetNext( ppi ) ) != NULL )
	{
		int i = LB_ERR;

		int icon = 0;
		if ( ppi->type == PUBTYPE_EMAIL ) icon = 1;
		else if ( ppi->type == PUBTYPE_DISK ) icon = 2;

		// Add the item to the list
		i = m_list.InsertItem( m_list.GetItemCount(), ppi->img, icon );

		// Set text item
		if ( i != LB_ERR ) 
		{	m_list.SetItemData( i, (DWORD)ppi );
			if ( ppi->type == PUBTYPE_DISK )
				m_list.SetItemText( i, 1, ppi->path );
			else m_list.SetItemText( i, 1, ppi->str );
		} // end if

	} // end while
}


void CDlgPublishing::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdit();

	DoEnable();

	*pResult = 0;
}

void CDlgPublishing::DoEnable()
{
	// Get LB selection
	int sel = CComCtrl::ListGetFirstSelectedItem( m_list.GetSafeHwnd() );

	CWnd *pWnd = GetDlgItem( IDC_EDIT );
	if ( pWnd != NULL ) pWnd->EnableWindow( sel != LB_ERR );

}

void CDlgPublishing::OnEdit() 
{
	int sel = CComCtrl::ListGetFirstSelectedItem( m_list.GetSafeHwnd() );
	if ( sel == LB_ERR ) return;

	LPPUBINFO ppi = (LPPUBINFO)m_list.GetItemData( sel );
	if ( ppi == NULL ) return;
	PubDlg( ppi );
	FillList();
}

void CDlgPublishing::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DoEnable();
	
	*pResult = 0;
}

BOOL CDlgPublishing::PubDlg(LPPUBINFO ppi)
{
	if ( ppi == NULL ) return FALSE;

	CDlgTabFrame dlg( IDD_PUBFRAME, this );

	CReg reg;

	// Read settings from object
	CPubList::UpdateData( FALSE, ppi, reg.GetKey( "PUBINFO" ) );

	// Provide a pointer to data
	dlg.SetDataPtr( &reg );

	// Set help root path
	char helppath[ CWF_STRSIZE ];
	GetModuleFileName( NULL, helppath, sizeof( helppath ) );
	CWinFile::GetPathFromFileName( helppath, helppath );
	CWinFile::BuildPath( helppath, helppath, "Help" );
	dlg.SetHelpRoot( helppath );
	
	// Add pages
	dlg.SetPage( 0, new CPgPubWhat(), CPgPubWhat::IDD, "Image", "publish_task.htm" );
	dlg.SetPage( 1, new CPgPubWhere(), CPgPubWhere::IDD, "Location", "publish_task.htm" );
	dlg.SetPage( 2, new CPgPubWhen(), CPgPubWhen::IDD, "When", "publish_task.htm" );
	dlg.SetPage( 3, new CPgPubHow(), CPgPubHow::IDD, "Filename", "publish_task.htm" );
	dlg.SetPage( 4, new CPgPubThumbnail(), CPgPubThumbnail::IDD, "Thumbnail", "publish_task.htm" );
	dlg.SetPage( 5, new CPgPubExtra(), CPgPubExtra::IDD, "AVI", "publish_task.htm" );
	dlg.SetPage( 6, new CPgPubSounds(), CPgPubSounds::IDD, "Sounds", "publish_task.htm" );

	// Pub publishing on hold
	ppi->bHold = TRUE;

	try
	{
		if ( dlg.DoModal() != IDOK )
		{	ppi->bHold = FALSE; return FALSE; }
	}
	catch ( ... )
	{	ppi->bHold = FALSE;
		ASSERT( 0 ); return FALSE;
	} // end catch

	// Read settings from object
	CPubList::UpdateData( TRUE, ppi, reg.GetKey( "PUBINFO" ) );

	// Re-enable publishing
	ppi->timeout = 0;
	ppi->bHold = FALSE;
		
	return TRUE;
}

