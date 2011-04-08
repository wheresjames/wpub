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
// DlgHtmlBuild.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgHtmlBuild.h"

#include "HtmlBuilder.h"

#include "DlgPublishHtml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_REFRESHHTML		( WM_USER + 101 )

/////////////////////////////////////////////////////////////////////////////
// CDlgHtmlBuild dialog


CDlgHtmlBuild::CDlgHtmlBuild(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgHtmlBuild::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgHtmlBuild)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	// Set minimum buffer size
	m_reg.SetMinSize( 1024 );

	*m_szTemplate = 0;
	*m_szPath = 0;

	m_bSize = FALSE;
	m_dwSplit = 400;
}


void CDlgHtmlBuild::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgHtmlBuild)
	DDX_Control(pDX, IDC_WARNING, m_warning);
	DDX_Control(pDX, IDC_PROPERTIES, m_prop);
	DDX_Control(pDX, IDC_TREE, m_tree);
	DDX_Control(pDX, IDC_WEB, m_web);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgHtmlBuild, CDialog)
	//{{AFX_MSG_MAP(CDlgHtmlBuild)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(NM_CLICK, IDC_TREE, OnClickTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_DEFAULTS, OnDefaults)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_REFRESHHTML, OnRefreshHtml )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgHtmlBuild message handlers

void CDlgHtmlBuild::Size()
{
	// Must have valid window
	if ( !::IsWindow( GetSafeHwnd() ) ) return;

	RECT rect, ctrl;
	GetClientRect( &rect );
	long lw = ( ( RW( rect ) * m_dwSplit ) / 1000 ) | 1;

	// Set control rect
	CopyRect( &ctrl, &rect );
	ctrl.right = ctrl.left + lw;
	ctrl.left += 2;
	ctrl.top += 2;

	CWnd *pCtrl = GetDlgItem( IDC_TREE );
	if ( pCtrl != NULL )
	{
		ctrl.bottom = ctrl.top + ( RH( ctrl ) / 3 );

		pCtrl->SetWindowPos(	NULL, ctrl.left, ctrl.top, 
								RW( ctrl ), RH( ctrl ),
								SWP_NOZORDER | SWP_NOACTIVATE );
	} // end if

	pCtrl = GetDlgItem( IDC_PROPERTIES );
	if ( pCtrl != NULL )
	{
		ctrl.top = ctrl.bottom + 2;
		ctrl.bottom = rect.bottom - 48;
		
		pCtrl->SetWindowPos(	NULL, ctrl.left, ctrl.top, 
								RW( ctrl ), RH( ctrl ),
								SWP_NOZORDER | SWP_NOACTIVATE );
	
		m_prop.Size();

	} // end if

	ctrl.top = ctrl.bottom + 2;
	ctrl.bottom = rect.bottom - 2;

	pCtrl = GetDlgItem( IDC_FRM_NEXT );
	if ( pCtrl != NULL )
		pCtrl->SetWindowPos(	NULL, ctrl.left, ctrl.top, 
								RW( ctrl ), RH( ctrl ),
								SWP_NOZORDER | SWP_NOACTIVATE );
	RECT button;
	button.top = 0; button.bottom = 25;
	CGrDC::Center( &button, &ctrl, FALSE );
	button.right = ctrl.right - 2;
	button.left = ctrl.right - ( RW( ctrl ) / 3 ) + 2;

	pCtrl = GetDlgItem( IDOK );
	if ( pCtrl != NULL )
	{
		
		pCtrl->SetWindowPos(	NULL, button.left, button.top, 
								RW( button ), RH( button ),
								SWP_NOZORDER | SWP_NOACTIVATE );
		pCtrl->RedrawWindow();

	} // end if

	button.right = button.left - 2;
	button.left = button.right - ( RW( ctrl ) / 3 ) + 2;

	pCtrl = GetDlgItem( IDC_DEFAULTS );
	if ( pCtrl != NULL )
	{
		
		pCtrl->SetWindowPos(	NULL, button.left, button.top, 
								RW( button ), RH( button ),
								SWP_NOZORDER | SWP_NOACTIVATE );
		pCtrl->RedrawWindow();

	} // end if

	button.right = button.left - 2;
	button.left = button.right - ( RW( ctrl ) / 3 ) + 2;

	pCtrl = GetDlgItem( IDCANCEL );
	if ( pCtrl != NULL )
	{
		
		pCtrl->SetWindowPos(	NULL, button.left, button.top, 
								RW( button ), RH( button ),
								SWP_NOZORDER | SWP_NOACTIVATE );
		pCtrl->RedrawWindow();

	} // end if

	pCtrl = GetDlgItem( IDC_WARNING );
	if ( pCtrl != NULL )
	{
		CopyRect( &ctrl, &rect );
		InflateRect( &ctrl, -2, -2 );
		ctrl.left += lw + 3;
		ctrl.bottom = ctrl.top + 28;

		pCtrl->SetWindowPos(	NULL, ctrl.left, ctrl.top, 
								RW( ctrl ), RH( ctrl ),
								SWP_NOZORDER | SWP_NOACTIVATE );
	} // end if

	pCtrl = GetDlgItem( IDC_WEB );
	if ( pCtrl != NULL )
	{
		ctrl.top = ctrl.bottom + 2;
		ctrl.bottom = rect.bottom - 2;

		pCtrl->SetWindowPos(	NULL, ctrl.left, ctrl.top, 
								RW( ctrl ), RH( ctrl ),
								SWP_NOZORDER | SWP_NOACTIVATE );
	} // end if

}

void CDlgHtmlBuild::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	Size();
	
}

BOOL CDlgHtmlBuild::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetIcon( AfxGetApp()->LoadIcon( IDI_WORLD ), TRUE );
	SetIcon( AfxGetApp()->LoadIcon( IDI_WORLD ), FALSE );

	m_warning.SetTitleColor( RGB( 255, 0, 0 ) );
	m_warning.SetFont( 18, "Arial" );
	m_warning.SetTitle( "Pages may not display completely until published!" );


	// Restore window position
	CFG().RestoreWindowPos( GetSafeHwnd(), "Positions", "HtmlBuilder" );
	CFG().GetValue( "Settings", "HtmlSplit", &m_dwSplit );

	// Ensure valid split
	if ( m_dwSplit < 100 || m_dwSplit > 900 )
		m_dwSplit = 400;

	*m_szTemplate = 0;
	*m_szPath = 0;

	char path[ CWF_STRSIZE ];
	CWinFile::GetExePath( path );

	m_images.Create( 16, 16, ILC_MASK | ILC_COLOR24, 0, 32 );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_FOLDER ) );		
	m_images.Add( AfxGetApp()->LoadIcon( IDI_FILE ) );		
	m_images.Add( AfxGetApp()->LoadIcon( IDI_WORLD ) );		
	m_tree.SetImageList( &m_images, TVSIL_NORMAL );

	// Setup to refresht the page
	m_prop.SetRefreshCallback( GetSafeHwnd(), WM_REFRESHHTML );

	Size();

	FillTree();
	
	m_prop.GetListCtrl().HorzLines( TRUE );
	m_prop.GetListCtrl().VertLines( TRUE );

	DoEnable();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgHtmlBuild::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize.x = 500;
	lpMMI->ptMinTrackSize.y = 375;
	
	CDialog::OnGetMinMaxInfo(lpMMI);
}

BOOL CDlgHtmlBuild::FillTree()
{
	m_tree.DeleteAllItems();	
	
	char path[ CWF_STRSIZE ];

	// Get template directory
	if ( !CFG().GetValue( "Settings", "HTMLTemplatesDir", path, sizeof( path ) ) )
	{	CWinFile::GetExePath( path );
		CWinFile::BuildPath( path, path, "HTML Templates" );
	} // end if

	HTREEITEM hRoot = CComCtrl::TreeInsertItem( m_tree.GetSafeHwnd(), NULL, "HTML Schemes", 2 );

	FillTree( hRoot, path );

	m_tree.Expand( hRoot, TVE_EXPAND );

	return TRUE;
}

BOOL CDlgHtmlBuild::FillTree(HTREEITEM hRoot, LPCTSTR pDir)
{
	WIN32_FIND_DATA		wfd;
	char				path[ CWF_STRSIZE ] = { 0 };
	char				search[ CWF_STRSIZE ];
	HANDLE				hFind;

	// Build search string
	if ( pDir != NULL ) strcpy_sz( path, pDir );
	CWinFile::BuildPath( search, path, "*.*" );

	// Get first file
	hFind = FindFirstFile( search, &wfd );
	if ( hFind == INVALID_HANDLE_VALUE ) return FALSE;
	
	do
	{
		// Do we need to copy this file?
		if (	*wfd.cFileName != NULL &&
				strcmp( wfd.cFileName, "." ) &&
				strcmp( wfd.cFileName, ".." ) )
		{
			char full[ CWF_STRSIZE ];
			CWinFile::BuildPath( full, path, wfd.cFileName );

			// If a directory
			if ( ( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
				FillTree( CComCtrl::TreeInsertItem(	m_tree.GetSafeHwnd(), hRoot, 
													wfd.cFileName, 0 ), full );

			// Just add the one item
			else if ( CWinFile::CmpExt( wfd.cFileName, "htp" ) )
				CComCtrl::TreeInsertItem( m_tree.GetSafeHwnd(), hRoot, wfd.cFileName, 1 );

		} // end if

	// Get the next file or directory
	} while ( FindNextFile( hFind, &wfd ) );
	
	// Close the find handle
	FindClose( hFind );

	return TRUE;
}

void CDlgHtmlBuild::OnClickTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	TVHITTESTINFO ht;
//	if ( !GetCursorPos( &ht.pt ) ) return;
//	m_tree.ScreenToClient( &ht.pt );
//	HTREEITEM hItem = m_tree.HitTest( &ht );
//	if ( hItem == NULL ) return;
//	if ( ( ht.flags & TVHT_ONITEM ) == 0 ) return;

//	HTREEITEM hItem = m_tree.GetSelectedItem();
//	if ( hItem == NULL ) return;

	*pResult = 0;
}

void CDlgHtmlBuild::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if ( pNMTreeView == NULL ) return;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if ( hItem == NULL ) return;

	// Get item path/name
	char name[ CWF_STRSIZE ];
	if ( !CComCtrl::TreeGetItemPath( m_tree.GetSafeHwnd(), hItem, name, 1 ) )
		return;

	// Build filename
	char path[ CWF_STRSIZE ];
	// Get template directory
	if ( !CFG().GetValue( "Settings", "HTMLTemplatesDir", path, sizeof( path ) ) )
	{	CWinFile::GetExePath( path );
		CWinFile::BuildPath( path, path, "HTML Templates" );
	} // end if

	// Build path to file
	CWinFile::BuildPath( path, path, name );

	// Ensure file
	if ( !CWinFile::IsFile( path ) ) return;

	ShowTemplate( path );

	DoEnable();

	*pResult = 0;
}

BOOL CDlgHtmlBuild::ShowTemplate(LPCTSTR pTemplate, BOOL bDefaults)
{
	// Lose the list
	m_prop.EndEdit();
	m_prop.EmptyList();

	char path[ CWF_STRSIZE ];
	CWinFile::GetExePath( path );
	CWinFile::BuildPath( path, path, "Sample" );

	// Save for later
	strcpy_sz( m_szTemplate, pTemplate );
	strcpy_sz( m_szPath, path );

	// Reload the gfile
	if ( !m_reg.LoadRegFile( pTemplate ) ) return FALSE;

	// Update vars
	UpdateVars();

	// Load user values
	LoadVars( bDefaults );

	// Show variables
	ShowVars( &m_reg );

	// Create a sample
	if ( !CHtmlBuilder::CreatePage( pTemplate, path, &m_reg ) )
		return FALSE;

	// Build path to file
	CWinFile::BuildPath( path, path, m_reg.GetSz( "settings", "main" ) );
	
	// Ensure file
	if ( !CWinFile::IsFile( path ) ) return FALSE;

	// Show the thing
	m_web.Navigate( path, NULL, NULL, NULL, NULL );

/*
	if ( pTemplate == NULL ) return FALSE;

	char path[ CWF_STRSIZE ];
	CWinFile::GetPathFromFileName( pTemplate, path );

	CReg reg;
	if ( !reg.LoadRegFile( pTemplate ) ) return FALSE;

	CRKey *pRk = reg.GetKey( "files" );
	if ( pRk != NULL )
	{
		LPREGVALUE prv = NULL;
		while ( ( prv = (LPREGVALUE)pRk->GetNext( prv ) ) != NULL )
		{
			int x = 0; 
		}

	} // end if

	// Build path to file
	CWinFile::BuildPath( path, path, reg.GetSz( "settings", "main" ) );
	
	// Ensure file
	if ( !CWinFile::IsFile( path ) ) return FALSE;

	// Show the thing
	m_web.Navigate( path, NULL, NULL, NULL, NULL );
*/

	return TRUE;
}

BOOL CDlgHtmlBuild::ShowVars(CReg *pReg)
{						
	m_prop.Show( pReg );

	return TRUE;
}

LRESULT CDlgHtmlBuild::OnRefreshHtml(WPARAM wParam, LPARAM lParam)
{
	// Ensure we have a window
	if ( !::IsWindow( GetSafeHwnd() ) || !::IsWindow( m_web.GetSafeHwnd() ) )
		return 0;

//	UpdateVars();

	// Create a sample
	if ( !CHtmlBuilder::CreatePage( m_szTemplate, m_szPath, &m_reg ) )
		return 0;

	char path[ CWF_STRSIZE ];

	// Build path to file
	CWinFile::BuildPath( path, m_szPath, m_reg.GetSz( "settings", "main" ) );
	
	// Ensure file
	if ( !CWinFile::IsFile( path ) ) return FALSE;

	// Show the thing
	m_web.Navigate( path, NULL, NULL, NULL, NULL );

	// Save variable values
	SaveVars();

	return 1;
}

BOOL CDlgHtmlBuild::UpdateVars()
{

//	WEBSETTINGS().GetValue( "ImageMap"

	return TRUE;
}

void CDlgHtmlBuild::OnMouseMove(UINT nFlags, CPoint point) 
{
	CDialog::OnMouseMove(nFlags, point);

	RECT rect;
	GetClientRect( &rect );
	long lw = ( RW( rect ) * m_dwSplit ) / 1000;

	// Are we sizing?
	if ( m_bSize )
	{
		// Set the cursor
		SetCursor( LoadCursor( NULL, IDC_SIZEWE ) );

		// Set the new split point
		m_dwSplit = ( ( point.x * 1000 ) / RW( rect ) ) & ~1;

		// Resize
		Size();

		// Update everything
		RedrawWindow();

		// Redraw the web portal
		m_web.RedrawWindow();

	} // end if

	// Set size cursor if needed
	else if ( point.x > ( lw - 2 ) && point.x < ( lw + 2 ) )
		SetCursor( LoadCursor( NULL, IDC_SIZEWE ) );

}

void CDlgHtmlBuild::OnLButtonDown(UINT nFlags, CPoint point) 
{
	RECT rect;
	GetClientRect( &rect );
	long lw = ( RW( rect ) * m_dwSplit ) / 1000;

	// Set size cursor if needed
	if ( point.x > ( lw - 2 ) && point.x < ( lw + 2 ) )
	{	SetCapture();
		SetCursor( LoadCursor( NULL, IDC_SIZEWE ) );
		m_bSize = TRUE;
	} // end if	

	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgHtmlBuild::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ( m_bSize )
	{	m_bSize = FALSE;
		ReleaseCapture();
	} // end if
	
	CDialog::OnLButtonUp(nFlags, point);
}

BOOL CDlgHtmlBuild::DestroyWindow() 
{
	CFG().SaveWindowPos( GetSafeHwnd(), "Positions", "HtmlBuilder" );
	CFG().SetValue( "Settings", "HtmlSplit", m_dwSplit );
	
	return CDialog::DestroyWindow();
}

BOOL CDlgHtmlBuild::SaveVars()
{
	// Save global vars
	CRKey *pRk = m_reg.FindKey( "vars" );
	if ( pRk != NULL )
	{
		CRKey *pSave = HTMLVARS().GetKey( "vars" );
		if ( pSave != NULL ) pSave->Copy( pRk );
	} // end if

	// Save local vars
	char tmpl[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( m_szTemplate, tmpl );
	pRk = m_reg.FindKey( "lvars" );
	if ( pRk != NULL )
	{	CWinFile::BuildPath( tmpl, "lvars", tmpl );
		CRKey *pSave = HTMLVARS().GetKey( tmpl );
		if ( pSave != NULL ) pSave->Copy( pRk );
	} // end if

	return TRUE;
}

BOOL CDlgHtmlBuild::LoadVars( BOOL bDefaults )
{
	// Load global vars
	CRKey *pRk = m_reg.GetKey( "vars" );
	if ( pRk != NULL )
	{
		CRKey *pSave = HTMLVARS().FindKey( "vars" );
		if ( pSave != NULL ) pRk->Update( pSave );
	} // end if

	// Want default locals?
	if ( bDefaults ) return TRUE;

	// Load local vars
	char tmpl[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( m_szTemplate, tmpl );
	pRk = m_reg.GetKey( "lvars" );
	if ( pRk != NULL )
	{	CWinFile::BuildPath( tmpl, "lvars", tmpl );
		CRKey *pSave = HTMLVARS().FindKey( tmpl );
		if ( pSave != NULL ) pRk->Update( pSave );
	} // end if

	return TRUE;
}

void CDlgHtmlBuild::OnDefaults() 
{
	// Show using defaults
	ShowTemplate( m_szTemplate, TRUE );	

	// Save the defaults
	SaveVars();
}

void CDlgHtmlBuild::OnOK() 
{
	// Build path to file
	char path[ CWF_STRSIZE ];
	CWinFile::BuildPath( path, m_szPath, m_reg.GetSz( "settings", "main" ) );

	// Ensure file exists
	if ( !CWinFile::DoesExist( path ) )
	{	AfxMessageBox( "HTML page does not exist" ); return; }

	CDialog::OnOK();

	// Publishing object
	CDlgPublishHtml dlg;

	// Set path to main file
	dlg.m_sMain = path;

	// Run the publishing dialog box
	dlg.DoModal();
}

void CDlgHtmlBuild::DoEnable()
{
	UpdateData( TRUE );

	// Build path to file
	char path[ CWF_STRSIZE ];
	CWinFile::BuildPath( path, m_szPath, m_reg.GetSz( "settings", "main" ) );

	CWnd *pCtrl = GetDlgItem( IDOK );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( CWinFile::DoesExist( path ) );

}
