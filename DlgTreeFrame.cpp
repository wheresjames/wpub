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
// DlgTreeFrame.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgTreeFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTreeFrame dialog


CDlgTreeFrame::CDlgTreeFrame(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTreeFrame::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTreeFrame)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	// No list so far
	m_pg.dwSize = 0;
	m_pg.pHead = NULL;
	m_pg.pTail = NULL;
	m_pg.pParent = NULL;

	m_dwSize = 0;
	m_dwIndex = 0;

	m_dwStartPage = 0;	  

	m_dwIcon = 0;

	m_hpg = NULL;

	m_bApply = FALSE;

	m_dwLastPage = 0;
}

CDlgTreeFrame::~CDlgTreeFrame()
{
	Destroy();
}

void CDlgTreeFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTreeFrame)
	DDX_Control(pDX, IDC_TREE, m_tree);
	DDX_Control(pDX, IDC_BLANK, m_blank);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTreeFrame, CDialog)
	//{{AFX_MSG_MAP(CDlgTreeFrame)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	ON_BN_CLICKED(IDC_PLEASEHELP, OnPleasehelp)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE, OnItemexpandedTree)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_ENAPPLY, OnEnableApply )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTreeFrame message handlers

BOOL CDlgTreeFrame::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Set tree icons if any
	if ( m_dwIcon > 0 ) 
		m_tree.SetImageList( &m_images, TVSIL_NORMAL );
	
	// Fill in the tree
	FillTree();

	// Set to first page
	SetPageByIndex( m_dwStartPage );

	CWnd *pApply = GetDlgItem( IDC_APPLY );
	if ( pApply != NULL ) pApply->EnableWindow( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgTreeFrame::Destroy()
{
	m_pg.dwSize = 0;
	m_pg.pTail = NULL;
	DeleteTreePageList( m_pg.pHead );
 	m_pg.pHead = NULL;

	m_dwSize = 0;
	m_dwIndex = 0;
	m_dwStartPage = 0;

	// Reset image list
	m_dwIcon = 0;
	m_images.DeleteImageList();
	
}

void CDlgTreeFrame::DeleteTreePageList(LPTREEPAGEINFO node)
{
	while ( node != NULL )
	{	LPTREEPAGEINFO	next = node->pNext;
		
		// Delete sub pages if any
		if ( node->pHead != NULL ) 
		{	DeleteTreePageList( node->pHead );
			node->pHead = NULL;
		} // end if

		// Delete the page
		if ( node->pg != NULL )
		{	delete node->pg;
			node->pg = NULL;
		} // end if

		delete node;
		node = next;

		// Strike one page
		if ( m_dwSize ) m_dwSize--;

	} // end while
}

HTPAGE CDlgTreeFrame::AddPage(HTPAGE hParent, HICON hIcon, CDialog * pg, DWORD idd, LPCTSTR name, LPCTSTR help, LPCTSTR hlink)
{
	// Sanity checks
	if ( name == NULL ) return FALSE;

	// Use top of list if none specified
	if ( hParent == NULL ) hParent = &m_pg;

	// Allocate memory
	LPTREEPAGEINFO node = new TREEPAGEINFO;
	if ( node == NULL ) return FALSE;
	ZeroMemory( node, sizeof( TREEPAGEINFO ) );

	// Save page information
	node->index = m_dwIndex++;
	node->icon = hIcon;
	node->pg = pg;
	node->idd = idd;
	if ( name != NULL ) strcpy_sz( node->name, name );
	if ( help != NULL ) strcpy_sz( node->help, help );
	if ( hlink != NULL ) strcpy_sz( node->hlink, hlink );

	// Add this node to the list
	node->pParent = hParent;
	node->pNext = NULL;
	node->pPrev = hParent->pTail;
	if ( hParent->pTail == NULL ) hParent->pHead = node;
	else hParent->pTail->pNext = node;
	hParent->pTail = node;
	hParent->dwSize++;	
	
	m_dwSize++;

	if ( hIcon != NULL )
	{
		// Add icon to image list
		if ( m_images.GetSafeHandle() == NULL )
			m_images.Create( 16, 16, ILC_MASK | ILC_COLOR24, 32, 32 );

		// Add this icon
		m_images.Add( hIcon );

		// Count one icon
		node->iindex = m_dwIcon++;

	} // end if

	// No icon
	else node->iindex = MAXDWORD;

	return node;
}


BOOL CDlgTreeFrame::RemovePage(HTPAGE hParent, HTPAGE node)
{
	// Sanity check
	if ( node == NULL ) return FALSE;

	// Use top of list if none specified
	if ( hParent == NULL ) hParent = &m_pg;

	// Remove this node from the list
	if ( node->pPrev != NULL ) node->pPrev->pNext = node->pNext;
	if ( node->pNext != NULL ) node->pNext->pPrev = node->pPrev;
	if ( hParent->pHead == node ) hParent->pHead = node->pNext;			
	if ( hParent->pTail == node ) hParent->pTail = node->pPrev;		
	
	// Delete this node
	if ( node->pHead != NULL ) 
	{	DeleteTreePageList( node->pHead );
		node->pHead = NULL;
	} // end if
	delete node;

	// Decrement the count
	if ( m_dwSize ) m_dwSize--;

	return TRUE;
}


BOOL CDlgTreeFrame::SetPage(HTPAGE hpg)
{
	// Sanity checks
	if ( hpg == NULL ) return FALSE;

	// Lose current page
	if (	m_hpg != NULL && m_hpg->pg != NULL &&
			IsWindow( m_hpg->pg->GetSafeHwnd() ) )
	{
		// Hide the page
		m_hpg->pg->ShowWindow( SW_HIDE );

	} // end if

	// Get a pointer to the help button
	CWnd *pHelp = GetDlgItem( IDC_PLEASEHELP );

	// Enable / Disable help button
	if ( pHelp != NULL )
	{
		// Show help button if help file
		if ( hpg != NULL && hpg->help[ 0 ] != NULL )
		{
			pHelp->ShowWindow( SW_SHOWNORMAL );

			// Disable button if file not available
			char link[ CWF_STRSIZE ];
			CWinFile::BuildPath( link, m_helproot, hpg->help );
			pHelp->EnableWindow( CWinFile::DoesExist( link ) );
		} // end if

		// Is there a link
		else if ( hpg != NULL && hpg->hlink[ 0 ] != NULL )
			pHelp->EnableWindow( TRUE );

		// Hide help button if no help
		else pHelp->ShowWindow( SW_HIDE );

	} // end if

	if ( hpg != NULL && hpg->pg != NULL )
	{
		// Create dialog if needed
		if ( !::IsWindow( hpg->pg->GetSafeHwnd() ) )
			hpg->pg->Create( hpg->idd, &m_blank );

	} // end if

	// Save current page
	m_hpg = hpg;

	m_dwLastPage = m_hpg->index;

	// Size everything
	Size();

	return TRUE;
}

BOOL CDlgTreeFrame::Size()
{
	CWnd	*pBlank = GetDlgItem( IDC_BLANK );

	RECT rect;
	SetRect( &rect, 0, 0, 0, 0 );

	// Set page parent window position
	if ( pBlank != NULL )
		m_blank.GetClientRect( &rect );
//		InflateRect( &rect, -4, -4 );

	// Set page window position
	if (	m_hpg != NULL && m_hpg->pg != NULL && 
			::IsWindow( m_hpg->pg->GetSafeHwnd() ) )
	{
		// Show the blank
		if ( pBlank != NULL ) pBlank->ShowWindow( SW_SHOWNORMAL );

		// Set the page position
		m_hpg->pg->ShowWindow( SW_SHOWNORMAL );
		m_hpg->pg->SetWindowPos(	NULL, rect.left, rect.top,
									rect.right - rect.left,
									rect.bottom - rect.top,
									SWP_NOZORDER | SWP_NOACTIVATE );
		m_hpg->pg->RedrawWindow();

	} // end if
	else if ( pBlank != NULL ) pBlank->ShowWindow( SW_HIDE );

	return TRUE;

}

BOOL CDlgTreeFrame::SetPageByIndex( DWORD i, HTPAGE hParent )
{
	LPTREEPAGEINFO	ptpi = NULL;

	// Search each page
	while ( ( ptpi = GetNext( hParent, ptpi ) ) != NULL )
		if ( ptpi->index == i ) return SetPage( ptpi );
		else if ( ptpi->pHead != NULL ) if ( SetPageByIndex( i, ptpi ) )
			return TRUE;

	return FALSE;
}

BOOL CDlgTreeFrame::FillTree(HTPAGE hParent, HTREEITEM hItemParent)
{
	DWORD added = 0;

	// Reset tree contents
	if ( hParent == NULL && hItemParent == NULL )
		m_tree.DeleteAllItems();

	LPTREEPAGEINFO	ptpi = NULL;

	// Search each page
	while ( ( ptpi = GetNext( hParent, ptpi ) ) != NULL )
	{	added++;
		HTREEITEM hItem = MakeTreeEntry( ptpi->name, hItemParent, ptpi->iindex );
		m_tree.SetItemData( hItem, (DWORD)ptpi );
		if ( ptpi->index == m_dwStartPage ) m_tree.Select( hItem, TVGN_CARET );
		if ( ptpi->pHead != NULL ) added += FillTree( ptpi, hItem );		
	} // end while

	return added;
}

HTREEITEM CDlgTreeFrame::MakeTreeEntry(LPCTSTR pText, HTREEITEM hParent, DWORD dwImage)
{
	HTREEITEM			hItem;
	TV_INSERTSTRUCT		tv;

	char pszTemp[256];

	strcpy_sz( pszTemp, pText );

	tv.item.pszText = pszTemp;
	tv.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tv.item.state = 0;	
	tv.item.iImage = dwImage;
	tv.item.iSelectedImage = dwImage;
	tv.hInsertAfter = TVI_LAST;
	
	if ( hParent != NULL ) tv.hParent = hParent;
	else tv.hParent = TVI_ROOT;
	
	hItem = m_tree.InsertItem( &tv );

	// Start expanded
	if ( hParent != NULL )
		m_tree.Expand( hParent, TVE_EXPAND );

	return hItem;
}

void CDlgTreeFrame::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	HTREEITEM hItem = m_tree.GetSelectedItem();
	if ( hItem == NULL ) return;

	LPTREEPAGEINFO ptpi = (LPTREEPAGEINFO)m_tree.GetItemData( hItem );
	if ( ptpi == NULL ) return;

	// Make this the current page
	SetPage( ptpi );
}

void CDlgTreeFrame::OnOK() 
{
	// Tell pages to save data
	SendPageMessage( WM_PG_SAVE );
	
	CDialog::OnOK();
}

void CDlgTreeFrame::OnCancel() 
{
	// Tell pages to restore data
	SendPageMessage( WM_PG_RESTORE );
	
	CDialog::OnCancel();
}

void CDlgTreeFrame::SendPageMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LPTREEPAGEINFO pParent)
{
	LPTREEPAGEINFO	ptpi = NULL;

	// Search each page
	while ( ( ptpi = GetNext( pParent, ptpi ) ) != NULL )
	{
		// Send message to this page if valid
		if ( ptpi->pg != NULL && ::IsWindow( ptpi->pg->GetSafeHwnd() ) )
			ptpi->pg->SendMessage( uMsg, wParam, lParam );

		// Send message to sub pages if any
		if ( ptpi->pHead != NULL ) SendPageMessage( uMsg, wParam, lParam, ptpi );

	} // end while
}


void CDlgTreeFrame::EnableApply(HWND hWnd)
{
	if ( !::IsWindow( hWnd ) ) return;
	hWnd = ::GetParent( hWnd );
	if ( !::IsWindow( hWnd ) ) return;
	hWnd = ::GetParent( hWnd );
	if ( !::IsWindow( hWnd ) ) return;
	::SendMessage( hWnd, WM_PG_ENAPPLY, 0, 0L );
}

LRESULT CDlgTreeFrame::OnEnableApply(WPARAM wParam, LPARAM lParam)
{
	m_bApply = TRUE;
	CWnd *pApply = GetDlgItem( IDC_APPLY );
	if ( pApply != NULL ) pApply->EnableWindow( TRUE );
	return 0;
}

void CDlgTreeFrame::OnApply() 
{
	// Tell pages to save data
	SendPageMessage( WM_PG_SAVE );

	m_bApply = FALSE;
	CWnd *pApply = GetDlgItem( IDC_APPLY );
	if ( pApply != NULL ) pApply->EnableWindow( FALSE );
}

void CDlgTreeFrame::OnPleasehelp() 
{
	if ( m_hpg == NULL ) return;

	// Open help if any
	if ( m_hpg->help[ 0 ] == NULL ) return;

	// Open the help file
	char link[ CWF_STRSIZE ];
	CWinFile::BuildPath( link, m_helproot, m_hpg->help );
	ShellExecute( NULL, "open", link, NULL, NULL, SW_SHOWNORMAL );  		
}

void CDlgTreeFrame::OnItemexpandedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

DWORD CDlgTreeFrame::GetCurPageIndex()
{
	LPTREEPAGEINFO	ptpi = NULL;

	// Search each page
	while ( ( ptpi = GetNext( NULL, ptpi ) ) != NULL )
		if ( ptpi == m_hpg ) return ptpi->index;

	return 0;
}
