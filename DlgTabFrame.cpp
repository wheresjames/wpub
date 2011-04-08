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
// DlgTabFrame.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgTabFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTabFrame dialog


CDlgTabFrame::CDlgTabFrame(UINT uIDD, CWnd* pParent /*=NULL*/)
	: CDialog(uIDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTabFrame)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_dwPages = 0;
	m_dwPg = 0;
	ZeroMemory( &m_pg, sizeof( m_pg ) );

	*m_helproot = NULL;
	ZeroMemory( &m_help, sizeof( m_help ) );

	*m_szRestore = 0;
	m_bSize = FALSE;

	m_pData = NULL;
}

CDlgTabFrame::~CDlgTabFrame()
{
	Destroy();
}

void CDlgTabFrame::Destroy()
{
	// Lose all pages
	for ( DWORD i = 0; i < TABDLG_MAXPAGES; i++ )
		if ( m_pg[ i ] != NULL )
		{
			if( ::IsWindow( m_pg[ i ]->GetSafeHwnd() ) )
				m_pg[ i ]->DestroyWindow();
			delete m_pg[ i ];
			m_pg[ i ] = NULL;
		} // end if

	// Reinit variables
	ZeroMemory( &m_pg, sizeof( m_pg ) );
	ZeroMemory( &m_help, sizeof( m_help ) );
	m_dwPg = 0;
	m_dwPages = 0;
}

void CDlgTabFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTabFrame)
	DDX_Control(pDX, IDC_BLANK, m_blank);
	DDX_Control(pDX, IDC_TAB, m_tab);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTabFrame, CDialog)
	//{{AFX_MSG_MAP(CDlgTabFrame)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnSelchangeTab)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_HELPBTN, OnHelpbtn)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_GETDATA, OnGetData )
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTabFrame message handlers

BOOL CDlgTabFrame::OnInitDialog() 
{
	CDialog::OnInitDialog();

	AddPages();

	// Start on the first page
	SetPage( m_dwPg );
	m_tab.SetCurSel( m_dwPg );

	// Restore position
	if ( *m_szRestore != NULL )
		CFG().RestoreWindowPos( GetSafeHwnd(), "Positions", m_szRestore, FALSE );

	// Size everything correctly
	Size();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTabFrame::Size()
{
	CWnd	*pTab = GetDlgItem( IDC_TAB );
	CWnd	*pBlank = GetDlgItem( IDC_BLANK );

	RECT rect;

	if ( m_bSize )
	{
		GetClientRect( &rect );	
		InflateRect( &rect, -4, -4 );

		// Set tab control position
		if ( pTab != NULL )
		{
			pTab->SetWindowPos(	NULL, rect.left, rect.top,
								rect.right - rect.left,
								rect.bottom - rect.top,
								SWP_NOZORDER | SWP_NOACTIVATE );
		} // end if

	} // end if
	
	// Adjust window to tab control
	if ( pTab != NULL && ::IsWindow( pTab->GetSafeHwnd() ) )
	{
		if ( !m_bSize ) 
		{	pTab->GetWindowRect( &rect );
			ScreenToClient( &rect );
		} // end if

		m_tab.AdjustRect( FALSE, &rect );
		rect.bottom -= 2;

		// Set page parent window position
		if ( pBlank != NULL )
		{
			// Set page parent position
			pBlank->SetWindowPos(	NULL, rect.left, rect.top,
									rect.right - rect.left,
									rect.bottom - rect.top,
									SWP_NOZORDER | SWP_NOACTIVATE );
			m_blank.GetClientRect( &rect );
	//		m_blank.GetWindowRect( &rect );
	//		ScreenToClient( &rect );

		} // end if

		// Set page window position
		if ( m_pg[ m_dwPg ] != NULL && ::IsWindow( m_pg[ m_dwPg ]->GetSafeHwnd() ) )
		{
			// Set the page position
			m_pg[ m_dwPg ]->SetWindowPos(	NULL, rect.left, rect.top,
											rect.right - rect.left,
											rect.bottom - rect.top,
											SWP_NOZORDER | SWP_NOACTIVATE );
			m_pg[ m_dwPg ]->RedrawWindow();
		} // end if

	} // end if
}


BOOL CDlgTabFrame::SetPage(DWORD pg)
{
	// Do we need to add the pages?
	if ( m_dwPages == 0 ) AddPages();

	// Is page within reason?
	if ( pg >= TABDLG_MAXPAGES || m_pg[ pg ] == NULL )
		return FALSE;

	// Lose current page
	if (	m_pg[ m_dwPg ] != NULL && 
			IsWindow( m_pg[ m_dwPg ]->GetSafeHwnd() ) )
	{
		// Current page must approve page switches
		if ( m_pg[ m_dwPg ]->SendMessage( WM_PG_CHANGE, 1, 0L ) ) 
			return FALSE;

		// Lose the page
		m_pg[ m_dwPg ]->DestroyWindow();

	} // end if
	
	CWnd *pHelp = GetDlgItem( IDC_HELPBTN );

	// Enable / Disable help button
	if ( pHelp != NULL )
	{
		// Show help button if link
		if ( m_help[ pg ][ 0 ] != 0 )
		{
			pHelp->ShowWindow( SW_SHOWNORMAL );

			char link[ CWF_STRSIZE * 2 ];
			CWinFile::BuildPath( link, m_helproot, m_help[ pg ] );
			pHelp->EnableWindow( CWinFile::DoesExist( link ) );
		} // end if

		// Hide help button if no help
		else pHelp->ShowWindow( SW_HIDE );

	} // end if

	// Set title text
//	m_title.SetTitle( m_name[ pg ] );
//	m_title.RedrawWindow();

	// Create dialog
	if ( !IsWindow( m_pg[ pg ]->GetSafeHwnd() ) )
		m_pg[ pg ]->Create( m_idd[ pg ], &m_blank );

	// Save current page
	m_dwPg = pg;

	Size();

	// Show the dialog box
	m_pg[ pg ]->ShowWindow( SW_SHOWNA );
	m_pg[ pg ]->RedrawWindow();

	return TRUE;
}

void CDlgTabFrame::RefreshPages()
{
	// Refresh all pages
	for ( DWORD i = 0; i < TABDLG_MAXPAGES; i++ )
		if ( m_pg[ i ] != NULL && ::IsWindow( m_pg[ i ]->GetSafeHwnd() ) )
			m_pg[ i ]->SendMessage( WM_PG_REFRESH, 0, (LPARAM)this );
}

void CDlgTabFrame::AddPages()
{
	TC_ITEM		tcitem;	
	tcitem.mask = TCIF_TEXT;

	// Lose all items in the tab control
	m_tab.DeleteAllItems();

	// Add pages
	for ( DWORD i = 0; i < TABDLG_MAXPAGES && m_pg[ i ] != NULL; i++ )
	{
		tcitem.pszText = m_name[ i ];
		m_tab.InsertItem( i, &tcitem );	
		m_dwPages++;
	} // end for
}

void CDlgTabFrame::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Select new view
	SetPage( m_tab.GetCurSel() );
	
	*pResult = 0;
}

void CDlgTabFrame::OnClose() 
{
	SavePosition();
	CDialog::OnClose();
//	if ( ::IsWindow( GetSafeHwnd() ) ) DestroyWindow();	
}

BOOL CDlgTabFrame::DestroyWindow() 
{
	SavePosition();
	return CDialog::DestroyWindow();
}


void CDlgTabFrame::SavePosition()
{
	if ( *m_szRestore != NULL )
		CFG().SaveWindowPos( GetSafeHwnd(), "Positions", m_szRestore );
}

LRESULT CDlgTabFrame::OnGetData(WPARAM wParam, LPARAM lParam)
{	
	return (LRESULT)m_pData;
}

void* CDlgTabFrame::GetData( HWND hPage )
{		
	void *ptr = NULL;

	// Get the blank
	HWND hWnd = ::GetParent( hPage );

	// Get the dialog
	if ( ::IsWindow( hWnd ) ) hWnd = ::GetParent( hWnd );

	// Send message
	if ( ::IsWindow( hWnd ) )
	{	ptr = (void*)::SendMessage( hWnd, WM_PG_GETDATA, 0, 0 );
		if ( (DWORD)ptr < 256 ) ptr = NULL;
	} // end if

	return ptr;
}

void CDlgTabFrame::OnHelpbtn() 
{
	// Open help if any
	if ( m_help[ m_dwPg ][ 0 ] == NULL ) return;

	// Open the help file
	char link[ CWF_STRSIZE ];
	CWinFile::BuildPath( link, m_helproot, m_help[ m_dwPg ] );
	ShellExecute( NULL, "open", link, NULL, NULL, SW_SHOWNORMAL );  		
}

