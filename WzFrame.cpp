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
// WzFrame.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "WzFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWzFrame dialog


CWzFrame::CWzFrame( UINT idd, CWnd* pParent /*=NULL*/)
	: CDialog(idd, pParent)
{
	//{{AFX_DATA_INIT(CWzFrame)
	//}}AFX_DATA_INIT

	// Zero memory
	ZeroMemory( &m_pg, sizeof( m_pg ) );
	m_dwPg = 0;
	m_dwLastPage = MAXDWORD;
	m_dwNextPage = MAXDWORD;

	m_bBack = FALSE;
	m_bNext = FALSE;
	m_bFinish = FALSE;

	*m_helproot = NULL;
	ZeroMemory( &m_help, sizeof( m_help ) );

	m_pData = NULL;
}

CWzFrame::~CWzFrame()
{
	Destroy();
}

void CWzFrame::Destroy()
{
	// Delete user pages
	for ( DWORD i = 0; i < CWZ_MAXPAGES; i++ )
		if ( m_pg[ i ] != NULL ) 
		{	delete m_pg[ i ];
			m_pg[ i ] = NULL;
		} // end if

	// Ensure initialized
	ZeroMemory( &m_pg, sizeof( m_pg ) );
	ZeroMemory( &m_help, sizeof( m_help ) );
	m_dwPg = 0;
}

void CWzFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWzFrame)
	DDX_Control(pDX, IDC_BLANK, m_blank);
	DDX_Control(pDX, IDC_TITLE, m_title);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWzFrame, CDialog)
	//{{AFX_MSG_MAP(CWzFrame)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_BACK, OnBack)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_HELPBTN, OnHelpbtn)
	//}}AFX_MSG_MAP

	ON_MESSAGE( CWZ_ENABLEBACK, OnEnableBack )
	ON_MESSAGE( CWZ_ENABLENEXT, OnEnableNext )
	ON_MESSAGE( CWZ_ENABLEFINISH, OnEnableFinish )
	ON_MESSAGE( CWZ_ENABLEHELP, OnEnableHelp )
	ON_MESSAGE( CWZ_BACK, OnBackMsg )
	ON_MESSAGE( CWZ_NEXT, OnNextMsg )
	ON_MESSAGE( CWZ_SETPAGE, OnSetPage )
	ON_MESSAGE( CWZ_SETNEXTPAGE, OnSetNextPage )
	ON_MESSAGE( CWZ_GETDATA, OnGetData )
	ON_MESSAGE( CWZ_SETFINISHTEXT, OnSetFinishText )
	ON_MESSAGE( CWZ_GETCURRENTPAGE, OnGetCurrentPage )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWzFrame message handlers

BOOL CWzFrame::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Default next page
	m_dwNextPage = MAXDWORD;

	// Set current page
	SetPage( m_dwPg );	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CWzFrame::SetPage(DWORD pg)
{
	if ( !::IsWindow( GetSafeHwnd() ) ) return FALSE;

	// Is page within reason?
	if ( pg >= CWZ_MAXPAGES || m_pg[ pg ] == NULL )
	{	m_bBack = FALSE;
		m_bNext = FALSE;
		return FALSE;
	} // end if

	// Save current page
	DWORD dwCurPage = m_dwPg;

	// Lose current page
	if (	m_pg[ m_dwPg ] != NULL && 
			IsWindow( m_pg[ m_dwPg ]->GetSafeHwnd() ) )
	{
		// Current page must approve page switches
		if ( m_bNext )
		{	if ( m_pg[ m_dwPg ]->SendMessage( CWZ_PAGECHG, 1, m_dwPg ) ) 
			{	m_bNext = FALSE; return FALSE; }
			m_pg[ m_dwPg ]->SendMessage( CWZ_SAVE, 0, m_dwPg );
		} // end if
		else if ( m_bBack )
		{	if ( m_pg[ m_dwPg ]->SendMessage( CWZ_PAGECHG, 2, m_dwPg ) )
			{	m_bBack = FALSE; return FALSE; }
			m_pg[ m_dwPg ]->SendMessage( CWZ_RESTORE, 0, m_dwPg );
		} // end else if

		// Warn that we are closing the page
		m_pg[ m_dwPg ]->SendMessage( CWZ_PAGECLOSE, 0, m_dwPg );

		// Lose the page
		m_pg[ m_dwPg ]->DestroyWindow();

	} // end if
	
	// What will the next page be
	if ( !m_bBack && m_dwNextPage != MAXDWORD )
	{	if ( m_dwNextPage < CWZ_MAXPAGES && m_pg[ m_dwNextPage ] != NULL )
			pg = m_dwNextPage;
	} // end if

	else if ( m_bNext && m_next[ m_dwPg ] != MAXDWORD && m_next[ m_dwPg ] != 999 )
		pg = m_next[ m_dwPg ];

	else if ( m_bBack && m_back[ m_dwPg ] != MAXDWORD )
		pg = m_back[ m_dwPg ];

	// Clear back/next flags
	m_bBack = FALSE;
	m_bNext = FALSE;
	m_bFinish = ( m_next[ pg ] == 999 );
	m_dwNextPage = MAXDWORD;

	CWnd	*pNext = GetDlgItem( IDC_NEXT );
	CWnd	*pBack = GetDlgItem( IDC_BACK );
	CWnd	*pExit = GetDlgItem( IDOK );
	CWnd	*pHelp = GetDlgItem( IDC_HELPBTN );

	// Enable/disable next button
	if ( pNext != NULL )
	{	if ( !m_bFinish && m_pg[ pg + 1 ] != NULL ) 
		{
			pNext->EnableWindow( TRUE );
			pNext->SetWindowText( "Next >" );
			if ( pExit != NULL ) pExit->SetWindowText( "Exit" );
		} // end if
		else 
		{
			if ( pExit == NULL )
			{	pNext->EnableWindow( TRUE );
				pNext->SetWindowText( "Finish" );
			} // end if
			else
			{	pNext->EnableWindow( FALSE );
				if ( pExit != NULL ) pExit->SetWindowText( "Finish" );
			} // end else
		} // end else
	} // end if

	// Enable/disable back button
	if ( pBack != NULL )
	{	if ( pg != 0 && m_pg[ pg - 1 ] != NULL ) pBack->EnableWindow( TRUE );
		else pBack->EnableWindow( FALSE );
	} // end if

	// Enable exit button
	if ( pExit != NULL ) pExit->EnableWindow( TRUE );

	// Enable / Disable help button
	if ( pHelp != NULL )
	{
		// Show help button if link
		if ( m_help[ pg ][ 0 ] != NULL )
		{
			pHelp->ShowWindow( SW_SHOWNORMAL );

			char link[ CWF_STRSIZE ];
			CWinFile::BuildPath( link, m_helproot, m_help[ pg ] );
			pHelp->EnableWindow( CWinFile::DoesExist( link ) );
		} // end if

		// Hide help button if no help
		else pHelp->ShowWindow( SW_HIDE );

	} // end if

	// Set title text
	m_title.SetTitle( m_name[ pg ] );
	if ( ::IsWindow( m_title.GetSafeHwnd() ) )
		m_title.RedrawWindow();

	// Commit to page
	m_dwPg = pg;
	m_dwLastPage = dwCurPage;

	RECT	rect;
	CWnd	*pCtrl = GetDlgItem( IDC_BLANK );
	if ( pCtrl == NULL ) return FALSE;

	// Create dialog
	if ( !IsWindow( m_pg[ pg ]->GetSafeHwnd() ) )
		m_pg[ pg ]->Create( m_idd[ pg ], pCtrl );

	// Get dialog position
//	pCtrl->GetWindowRect( &rect );
//	ScreenToClient( &rect );
	pCtrl->GetClientRect( &rect );

	// Center dialog page
	RECT pgrect;
	m_pg[ pg ]->GetWindowRect( &pgrect );
	CGrDC::Center( &pgrect, &rect );

	// Set dialog position
	m_pg[ pg ]->SetWindowPos(	NULL, pgrect.left, pgrect.top,
								RW( pgrect ), RH( pgrect ),								
								SWP_NOZORDER | SWP_NOACTIVATE );

	// Show the dialog box
	m_pg[ pg ]->ShowWindow( SW_SHOWNORMAL );

	return TRUE;
}

void CWzFrame::OnNext() 
{
	CWnd	*pExit = GetDlgItem( IDOK );
	if ( pExit == NULL )
	{
		// Cancel if more pages left
		if ( m_bFinish || m_pg[ m_dwPg + 1 ] == NULL ) 
		{
			// Warn that we are closing the page
			m_pg[ m_dwPg ]->SendMessage( CWZ_PAGECLOSE, 0, m_dwPg );

			CDialog::OnOK();

		} // end if
	} // end if

	m_bNext = TRUE;
	SetPage( m_dwPg + 1 );
}

void CWzFrame::OnBack() 
{
	if ( m_dwPg ) 
	{	m_bBack = TRUE;
		SetPage( m_dwPg - 1 );
	} // end if
}

void CWzFrame::OnCancel() 
{
	if ( ::MessageBox(	GetSafeHwnd(), 
						"Are you sure you want to quit?", 
						"Exit Wizard", 
						MB_YESNO ) == IDYES )
	{

		// Warn that we are closing the page
		m_pg[ m_dwPg ]->SendMessage( CWZ_PAGECLOSE, 1, m_dwPg );
						
		CDialog::OnCancel();	
	} // end if
}

void CWzFrame::OnOK() 
{
	// Cancel if more pages left
	if ( !m_bFinish && m_pg[ m_dwPg + 1 ] != NULL ) 
	{
		if ( ::MessageBox(	GetSafeHwnd(), 
							"Are you sure you want to quit?", 
							"Exit Wizard", 
							MB_YESNO ) == IDYES )
		{

			// Warn that we are closing the page
			m_pg[ m_dwPg ]->SendMessage( CWZ_PAGECLOSE, 1, m_dwPg );
							
			CDialog::OnCancel();	
		} // end if

		return;
	} // end if

	// Warn that we are closing the page
	m_pg[ m_dwPg ]->SendMessage( CWZ_PAGECLOSE, 0, m_dwPg );

	CDialog::OnOK();
}

void CWzFrame::OnDestroy() 
{
	// Lose current page
	if (	m_dwPg < CWZ_MAXPAGES &&
			m_pg[ m_dwPg ] != NULL && 
			IsWindow( m_pg[ m_dwPg ]->GetSafeHwnd() ) )
		m_pg[ m_dwPg ]->DestroyWindow();

	CDialog::OnDestroy();
	
}

void CWzFrame::EnableBack( HWND hPage, BOOL enable)
{
	HWND hWnd = ::GetParent( hPage );
	if ( ::IsWindow( hWnd ) ) hWnd = ::GetParent( hWnd );

	if ( ::IsWindow( hWnd ) )
		::SendMessage( hWnd, CWZ_ENABLEBACK, enable, 0L );
}

void CWzFrame::EnableNext( HWND hPage, BOOL enable)
{
	HWND hWnd = ::GetParent( hPage );
	if ( ::IsWindow( hWnd ) ) hWnd = ::GetParent( hWnd );
	if ( ::IsWindow( hWnd ) )
		::SendMessage( hWnd, CWZ_ENABLENEXT, enable, 0L );
}

void CWzFrame::EnableFinish( HWND hPage, BOOL enable)
{
	HWND hWnd = ::GetParent( hPage );
	if ( ::IsWindow( hWnd ) ) hWnd = ::GetParent( hWnd );
	if ( ::IsWindow( hWnd ) )
		::SendMessage( hWnd, CWZ_ENABLEFINISH, enable, 0L );
}

void CWzFrame::EnableHelp( HWND hPage, BOOL enable, LPCTSTR pLink)
{
	HWND hWnd = ::GetParent( hPage );
	if ( ::IsWindow( hWnd ) ) hWnd = ::GetParent( hWnd );
	if ( ::IsWindow( hWnd ) )
		::SendMessage( hWnd, CWZ_ENABLEHELP, enable, (LPARAM)pLink );
}

void CWzFrame::Back( HWND hPage)
{
	HWND hWnd = ::GetParent( hPage );
	if ( ::IsWindow( hWnd ) ) hWnd = ::GetParent( hWnd );
	if ( ::IsWindow( hWnd ) )
		::SendMessage( hWnd, CWZ_BACK, 0, 0L );
}

void CWzFrame::Next( HWND hPage)
{
	HWND hWnd = ::GetParent( hPage );
	if ( ::IsWindow( hWnd ) ) hWnd = ::GetParent( hWnd );
	if ( ::IsWindow( hWnd ) )
		::SendMessage( hWnd, CWZ_NEXT, 0, 0L );
}

void CWzFrame::SetPage( HWND hPage, DWORD page)
{
	HWND hWnd = ::GetParent( hPage );
	if ( ::IsWindow( hWnd ) ) hWnd = ::GetParent( hWnd );
	if ( ::IsWindow( hWnd ) )
		::SendMessage( hWnd, CWZ_SETPAGE, 0, page );
}

void* CWzFrame::GetData( HWND hPage )
{	
	HWND hWnd = ::GetParent( hPage );
	if ( ::IsWindow( hWnd ) ) hWnd = ::GetParent( hWnd );
	if ( ::IsWindow( hWnd ) )
		return (void*)::SendMessage( hWnd, CWZ_GETDATA, 0, 0L );
	return NULL;
}

void CWzFrame::SetFinishText( HWND hPage, LPCTSTR pText, BOOL bCancel )
{	
	HWND hWnd = ::GetParent( hPage );
	if ( ::IsWindow( hWnd ) ) hWnd = ::GetParent( hWnd );
	if ( ::IsWindow( hWnd ) )
		::SendMessage( hWnd, CWZ_SETFINISHTEXT, !bCancel, (LPARAM)pText );
}

void CWzFrame::SetNextPage( HWND hPage, DWORD pg )
{	
	HWND hWnd = ::GetParent( hPage );
	if ( ::IsWindow( hWnd ) ) hWnd = ::GetParent( hWnd );
	if ( ::IsWindow( hWnd ) )
		::SendMessage( hWnd, CWZ_SETNEXTPAGE, (WPARAM)pg, 0L );
}

DWORD CWzFrame::GetCurrentPage( HWND hPage )
{	
	HWND hWnd = ::GetParent( hPage );
	if ( ::IsWindow( hWnd ) ) hWnd = ::GetParent( hWnd );
	if ( ::IsWindow( hWnd ) )
		return ::SendMessage( hWnd, CWZ_GETCURRENTPAGE, 0, 0L );
	return MAXDWORD;
}

LRESULT CWzFrame::OnEnableBack(WPARAM wParam, LPARAM lParam)
{
	CWnd	*pCtrl = GetDlgItem( IDC_BACK );
	if ( pCtrl == NULL ) return -1;
	pCtrl->EnableWindow( wParam );
	return 0;
}

LRESULT CWzFrame::OnEnableNext(WPARAM wParam, LPARAM lParam)
{
	CWnd	*pCtrl = GetDlgItem( IDC_NEXT );
	if ( pCtrl == NULL ) return -1;
	pCtrl->EnableWindow( wParam );
	return 0;
}

LRESULT CWzFrame::OnEnableFinish(WPARAM wParam, LPARAM lParam)
{
	CWnd	*pCtrl = GetDlgItem( IDOK );
	if ( pCtrl == NULL ) return -1;
	pCtrl->EnableWindow( wParam );
	return 0;
}

LRESULT CWzFrame::OnSetFinishText(WPARAM wParam, LPARAM lParam)
{
	if ( lParam == NULL ) return -1;
	CWnd *pCtrl = GetDlgItem( IDOK );
	if ( pCtrl == NULL ) return -1;
	pCtrl->SetWindowText( (LPCTSTR)lParam );
	m_bFinish = (BOOL)wParam;
	return 0;
}

LRESULT CWzFrame::OnEnableHelp(WPARAM wParam, LPARAM lParam)
{
	CWnd	*pCtrl = GetDlgItem( IDC_HELPBTN );
	if ( pCtrl == NULL ) return -1;
	pCtrl->EnableWindow( wParam );

	// Copy help link if provided
	if ( lParam != NULL && m_dwPg < CWZ_MAXPAGES )
	{
		strcpy( m_help[ m_dwPg ], (LPCTSTR)lParam );

		CWnd *pHelp = GetDlgItem( IDC_HELPBTN );

		if ( pHelp != NULL )
		{
			// Show help button if link
			if ( m_help[ m_dwPg ][ 0 ] != NULL )
			{
				pHelp->ShowWindow( SW_SHOWNORMAL );

				char link[ CWF_STRSIZE ];
				CWinFile::BuildPath( link, m_helproot, m_help[ m_dwPg ] );
				pHelp->EnableWindow( CWinFile::DoesExist( link ) );
			} // end if

			// Hide help button if no help
			else pHelp->ShowWindow( SW_HIDE );

		} // end else
	} // end if

	return 0;
}

void CWzFrame::OnHelpbtn() 
{
	// Open help if any
	if ( m_help[ m_dwPg ][ 0 ] == NULL ) return;

	// Open the help file
	char link[ CWF_STRSIZE ];
	CWinFile::BuildPath( link, m_helproot, m_help[ m_dwPg ] );
	ShellExecute( NULL, "open", link, NULL, NULL, SW_SHOWNORMAL );  		
}

LRESULT CWzFrame::OnBackMsg(WPARAM wParam, LPARAM lParam)
{
	OnBack(); return 0;
}

LRESULT CWzFrame::OnNextMsg(WPARAM wParam, LPARAM lParam)
{
	OnNext(); return 0;
}

LRESULT CWzFrame::OnSetPage(WPARAM wParam, LPARAM lParam)
{
	SetPage( (DWORD)lParam ); return 0;
}

LRESULT CWzFrame::OnGetData(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)m_pData;
}

LRESULT CWzFrame::OnSetNextPage(WPARAM wParam, LPARAM lParam)
{
	m_dwNextPage = (DWORD)wParam;

	CWnd	*pNext = GetDlgItem( IDC_NEXT );
	CWnd	*pExit = GetDlgItem( IDOK );

	DWORD	next = ( m_dwPg < CWZ_MAXPAGES ) ? m_next[ m_dwPg ] : MAXDWORD;
	BOOL	bIsNext = ( m_dwNextPage < CWZ_MAXPAGES && m_pg[ m_dwNextPage ] != NULL );

	if ( !bIsNext )
	{	if ( next == MAXDWORD )		
			bIsNext = ( m_dwPg < ( CWZ_MAXPAGES - 1 ) && m_pg[ m_dwPg + 1 ] != NULL );
		else bIsNext = ( next < CWZ_MAXPAGES && m_pg[ m_dwPg ] != NULL );
	} // end if
	m_bFinish = !bIsNext;

	if ( bIsNext )
	{
		if ( pNext != NULL ) 
		{	pNext->EnableWindow( TRUE );
			pNext->SetWindowText( "Next >" );
		} // end if

		if ( pExit != NULL ) pExit->SetWindowText( "Exit" );

	} // end if
	else
	{	if ( pExit != NULL ) pNext->EnableWindow( FALSE ), pExit->SetWindowText( "Finish" );
		else pNext->EnableWindow( TRUE ), pNext->SetWindowText( "Finish" );
	} // end else

	return 0;
}

LRESULT CWzFrame::OnGetCurrentPage(WPARAM wParam, LPARAM lParam)
{
	return m_dwPg;
}
