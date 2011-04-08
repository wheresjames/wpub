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
// DlgImgFxEditor.cpp : implementation file
//

#include "stdafx.h"
#include "Vp.h"
#include "DlgImgFxEditor.h"
#include "Params.h"
#include "ImgEffects.h"
#include "DlgEditParams.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImgFxEditor dialog


CDlgImgFxEditor::CDlgImgFxEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImgFxEditor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImgFxEditor)
	m_sTitle = _T("");
	//}}AFX_DATA_INIT

}


void CDlgImgFxEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImgFxEditor)
	DDX_Control(pDX, IDC_TREE, m_tree);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_TITLE, m_sTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImgFxEditor, CDialog)
	//{{AFX_MSG_MAP(CDlgImgFxEditor)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_BN_CLICKED(IDC_EDITITEM, OnEdititem)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_BN_CLICKED(IDC_REMOVEALL, OnRemoveall)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE, OnDblclkTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImgFxEditor message handlers

BOOL CDlgImgFxEditor::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CFG().RestoreWindowPos( GetSafeHwnd(), "Positions", "FxEditor", FALSE );

	if ( m_images.GetSafeHandle() == NULL )
	{
		// Build image list
		m_images.Create( 16, 16, ILC_MASK, 32, 32 );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_FOLDER ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_MIRROR ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_FLIP ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_GRAYSCALE ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_ROTATE ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_NEGATIVE ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_RESAMPLE ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_DITHER ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_CROP ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_NOISE ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_MEDIAN ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_GAMMA ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_SHIFT ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_THRESHOLD ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_COLORIZE ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_LIGHT ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_ERODE ) );
		m_images.Add( AfxGetApp()->LoadIcon( IDI_DILATE ) );

	} // end if

	// Set the image list
	m_list.SetImageList( &m_images, LVSIL_NORMAL );
	m_list.SetImageList( &m_images, LVSIL_SMALL );

	// Set the image list
	m_tree.SetImageList( &m_images, TVSIL_NORMAL );

	RECT rect;
	m_list.GetClientRect( &rect );

	m_list.InsertColumn( 0, "Effects", LVCFMT_LEFT, 
							( rect.right - rect.left ) - 
							GetSystemMetrics( SM_CXVSCROLL ) - 5 );

	FillTree(); 

	OnRefresh( 0, 0L );	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgImgFxEditor::OnAdd() 
{
	if ( m_params == NULL ) return;

	HTREEITEM hItem = m_tree.GetSelectedItem();
	if ( hItem == NULL ) return;

	DWORD function = m_tree.GetItemData( hItem );
	if ( function == 0 ) return;

	// Set variables
	m_params->GetVar().AddVar( "width", (double)m_w );
	m_params->GetVar().AddVar( "height", (double)m_h );

//+++
//	else
//	{	m_params->GetVar().AddVar( "width", (double)OVERLAY().m_frame.GetWidth() );
//		m_params->GetVar().AddVar( "height", (double)OVERLAY().m_frame.GetHeight() );
//	} // end if

	m_params->Add( function, CImgEffects::GetFxParams( function ) );
	
	FillList();

	// Reload the bitmap
//	if ( m_pfx != NULL && m_pfx->bmp != NULL )
//		m_pfx->bmp->Destroy();

//	else CAPTURE()->GrabFrame();

	APP()->ReGen( TRUE );
}

void CDlgImgFxEditor::OnRemove() 
{
	BOOL bRemoved = FALSE;

	if ( m_params == NULL ) return;

	// Get listbox count
	int count = m_list.GetItemCount();
	if ( count == 0 ) return;

	// Find first selected item
	int sel = 0;
	while ( sel < count )
	{
		// Remove all selected items
		if ( m_list.GetItemState( sel, LVIS_SELECTED ) == LVIS_SELECTED )
		{				   
			// Get a pointer to the data
			LPPARAMINFO ppi = (LPPARAMINFO)m_list.GetItemData( sel );
			if ( ppi != NULL )
			{
				bRemoved = TRUE;

				// Remove the item
				m_params->Delete( ppi );

			} // end if

		} // end if

		sel++;

	} // end while

	if ( !bRemoved ) return;

	FillList();

	// Reload the bitmap
//	if ( m_pfx != NULL && m_pfx->bmp != NULL )
//		m_pfx->bmp->Destroy();

//	else CAPTURE()->GrabFrame();
//	OVERLAY().ReGen();

	APP()->ReGen( TRUE );

}

void CDlgImgFxEditor::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	FillList();
	DoEnable();
}

void CDlgImgFxEditor::FillTree()
{
	m_tree.DeleteAllItems();

	HTREEITEM hItem = MakeTreeEntry( "Basic", NULL, 0 );
	HTREEITEM hFirst = hItem;
	m_tree.SetItemData( MakeTreeEntry( "Mirror", hItem, IMGFX_MIRROR ), IMGFX_MIRROR );
	m_tree.SetItemData( MakeTreeEntry( "Flip", hItem, IMGFX_FLIP ), IMGFX_FLIP );
	m_tree.SetItemData( MakeTreeEntry( "Grayscale", hItem, IMGFX_GRAYSCALE ), IMGFX_GRAYSCALE );
	m_tree.SetItemData( MakeTreeEntry( "Rotate", hItem, IMGFX_ROTATE ), IMGFX_ROTATE );
	m_tree.SetItemData( MakeTreeEntry( "Crop", hItem, IMGFX_CROP ), IMGFX_CROP );
	m_tree.Expand( hItem, TVE_EXPAND );

	hItem = MakeTreeEntry( "Color", NULL, 0 );
	m_tree.SetItemData( MakeTreeEntry( "Negative", hItem, IMGFX_NEGATIVE ), IMGFX_NEGATIVE );
	m_tree.SetItemData( MakeTreeEntry( "Dither", hItem, IMGFX_DITHER ), IMGFX_DITHER );
	m_tree.SetItemData( MakeTreeEntry( "Gamma", hItem, IMGFX_GAMMA ), IMGFX_GAMMA );
	m_tree.SetItemData( MakeTreeEntry( "Shift", hItem, IMGFX_SHIFT ), IMGFX_SHIFT );
	m_tree.SetItemData( MakeTreeEntry( "Thresh-hold", hItem, IMGFX_THRESHOLD ), IMGFX_THRESHOLD );
	m_tree.SetItemData( MakeTreeEntry( "Colorize", hItem, IMGFX_COLORIZE ), IMGFX_COLORIZE );
	m_tree.SetItemData( MakeTreeEntry( "Lighten", hItem, IMGFX_LIGHT ), IMGFX_LIGHT );
	m_tree.Expand( hItem, TVE_EXPAND );

	hItem = MakeTreeEntry( "Special", NULL, 0 );
	m_tree.SetItemData( MakeTreeEntry( "Resample", hItem, IMGFX_RESAMPLE ), IMGFX_RESAMPLE );
	m_tree.SetItemData( MakeTreeEntry( "Noise", hItem, IMGFX_NOISE ), IMGFX_NOISE );
	m_tree.SetItemData( MakeTreeEntry( "Median", hItem, IMGFX_MEDIAN ), IMGFX_MEDIAN );
	m_tree.SetItemData( MakeTreeEntry( "Erode", hItem, IMGFX_ERODE ), IMGFX_ERODE );
	m_tree.SetItemData( MakeTreeEntry( "Dilate", hItem, IMGFX_DILATE ), IMGFX_DILATE );
	m_tree.Expand( hItem, TVE_EXPAND );

	// Want the first item to be seen
	m_tree.EnsureVisible( hFirst );
}

void CDlgImgFxEditor::FillList()
{
	int sel = GetFirstListSel( &m_list );
	LPPARAMINFO selppi = NULL;

	if ( sel != LB_ERR ) selppi = (LPPARAMINFO)m_list.GetItemData( sel );

	m_list.DeleteAllItems();
	if ( m_params == NULL ) return;

	DWORD index = 0;

	char msg[ 1024 ];
	LPPARAMINFO	ppi = NULL;
	while ( ( ppi = (LPPARAMINFO)m_params->GetNext( ppi ) ) != NULL )
	{
		strcpy_sz( msg, CImgEffects::GetFxName( ppi->function ) );
		if ( ppi->n > 0 ) strcat( msg, " ( " );
		for ( DWORD i = 0; i < ppi->n; i++ )
		{
			if ( i > 0 ) strcat( msg, ", " );
			strcat( msg, ppi->param[ i ].name );
			strcat( msg, " = " );
			double v = m_params->GetValue( ppi, i );
			char num[ 256 ];
			sprintf( num, "%g", v );
			strcat( msg, num );

		} // end for
		if ( ppi->n > 0 ) strcat( msg, " )" );

		// Insert function name
		int item = m_list.InsertItem( index++, msg, ppi->function );
		m_list.SetItemData( item, (DWORD)ppi );

		if ( ppi == selppi ) 
			m_list.SetItemState( item, LVIS_SELECTED, LVIS_SELECTED );

	} // end while

	DoEnable();
}

HTREEITEM CDlgImgFxEditor::MakeTreeEntry(LPCTSTR pText, HTREEITEM hParent, DWORD dwImage)
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

void CDlgImgFxEditor::OnOK() 
{
	m_params = NULL;
	CFG().SaveWindowPos( GetSafeHwnd(), "Positions", "FxEditor" );
//	CDialog::OnOK();
	DestroyWindow();
}

void CDlgImgFxEditor::OnCancel() 
{
	m_params = NULL;
	CFG().SaveWindowPos( GetSafeHwnd(), "Positions", "FxEditor" );
//	CDialog::OnCancel();
	DestroyWindow();
}


int CDlgImgFxEditor::GetFirstListSel(CListCtrl * pList)
{
	if ( pList == NULL ) return LB_ERR;

	// Get listbox count
	int count = pList->GetItemCount();
	if ( count == 0 ) return LB_ERR;

	// Find first selected item
	int sel = 0;
	while ( sel < count && m_list.GetItemState( sel, LVIS_SELECTED ) != LVIS_SELECTED )
		sel++;

	// Is anything selected
	if ( sel == count ) return LB_ERR;

	return sel;
}

/*
void CDlgImgFxEditor::SetFx( LPFX pfx )
{
	m_pfx = pfx; 
	if ( pfx != NULL && pfx->imgfx != NULL ) 
		m_params = &pfx->imgfx->m_params;
	else m_params = NULL;
	if ( IsWindow( GetSafeHwnd() ) ) OnRefresh( 0, 0L ); 
}
*/

void CDlgImgFxEditor::OnUp() 
{
	BOOL bMoved = FALSE;

	if ( m_params == NULL ) return;

	// Get listbox count
	int count = m_list.GetItemCount();
	if ( count == 0 ) return;

	int sel = 0;
	while ( sel < count )
	{
		// If selected
		if ( m_list.GetItemState( sel, LVIS_SELECTED ) == LVIS_SELECTED )
		{				   
			// Get a pointer to the data
			LPPARAMINFO ppi = (LPPARAMINFO)m_list.GetItemData( sel );
			if ( ppi != NULL )
			{
				// Remove the item
				bMoved = m_params->MoveUp( ppi );

			} // end if

		} // end if

		sel++;

	} // end while

	if ( !bMoved ) return;

	FillList();

	// Reload the bitmap
//	if ( m_pfx != NULL && m_pfx->bmp != NULL )
//		m_pfx->bmp->Destroy();

//	else CAPTURE()->GrabFrame();
//	OVERLAY().ReGen();

	APP()->ReGen( TRUE );
}

void CDlgImgFxEditor::OnDown() 
{
	BOOL bMoved = FALSE;

	if ( m_params == NULL ) return;

	// Get listbox count
	int count = m_list.GetItemCount();
	if ( count == 0 ) return;

	int sel = count - 1;
	while ( sel >= 0 )
	{
		// If selected
		if ( m_list.GetItemState( sel, LVIS_SELECTED ) == LVIS_SELECTED )
		{				   
			// Get a pointer to the data
			LPPARAMINFO ppi = (LPPARAMINFO)m_list.GetItemData( sel );
			if ( ppi != NULL )
			{
				// Remove the item
				bMoved = m_params->MoveDown( ppi );

			} // end if

		} // end if

		sel--;

	} // end while

	if ( !bMoved ) return;

	FillList();

	// Reload the bitmap
//	if ( m_pfx != NULL && m_pfx->bmp != NULL )
//		m_pfx->bmp->Destroy();

//	else CAPTURE()->GrabFrame();
//	OVERLAY().ReGen();

	APP()->ReGen( TRUE );
}

void CDlgImgFxEditor::DoEnable()
{
	UpdateData( TRUE );

	BOOL bParams = FALSE;
	{ // Check params
		int sel = GetFirstListSel( &m_list );
		if ( sel != LB_ERR )	
		{
			// Get a pointer to the data
			LPPARAMINFO ppi = (LPPARAMINFO)m_list.GetItemData( sel );
			if ( ppi != NULL ) bParams = ( ppi->n > 0 );
		} // end if
	} // end check params

	BOOL lsel = ( GetFirstListSel( &m_list ) != LB_ERR );
	BOOL tsel = ( m_tree.GetSelectedItem() != NULL );

	CWnd *pWnd = GetDlgItem( IDC_ADD );
	if ( pWnd != NULL ) pWnd->EnableWindow( tsel );

	pWnd = GetDlgItem( IDC_REMOVE );
	if ( pWnd != NULL ) pWnd->EnableWindow( lsel );

	pWnd = GetDlgItem( IDC_UP );
	if ( pWnd != NULL ) pWnd->EnableWindow( lsel );
	pWnd = GetDlgItem( IDC_DOWN );
	if ( pWnd != NULL ) pWnd->EnableWindow( lsel );
	pWnd = GetDlgItem( IDC_EDITITEM );
	if ( pWnd != NULL ) pWnd->EnableWindow( lsel && bParams );

}

void CDlgImgFxEditor::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	DoEnable();
	
	*pResult = 0;
}

void CDlgImgFxEditor::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	DoEnable();
	
	*pResult = 0;
}

void CDlgImgFxEditor::OnEdititem() 
{
	int sel = GetFirstListSel( &m_list );
	LPPARAMINFO selppi = NULL;
	
	// Get a pointer to the data
	LPPARAMINFO ppi = (LPPARAMINFO)m_list.GetItemData( sel );
	if ( ppi == NULL ) return;

	// Does it have any paramaters
	if ( ppi->n == 0 ) return;

	// Set variables
	m_params->GetVar().AddVar( "width", (double)m_w );
	m_params->GetVar().AddVar( "height", (double)m_h );

//+++
//	else
//	{	m_params->GetVar().AddVar( "width", (double)OVERLAY().m_frame.GetWidth() );
//		m_params->GetVar().AddVar( "height", (double)OVERLAY().m_frame.GetHeight() );
//	} // end if

	CDlgEditParams	dlg;

//	if ( m_pfx != NULL ) dlg.SetEditParam( ppi );
	dlg.SetEditParam( m_params, ppi );

	dlg.DoModal();

	FillList();

	// Reload the bitmap
//	if ( m_pfx != NULL && m_pfx->bmp != NULL )
//		m_pfx->bmp->Destroy();

//	else CAPTURE()->GrabFrame();
//	OVERLAY().ReGen();

	APP()->ReGen( TRUE );
}

void CDlgImgFxEditor::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdititem();	
	*pResult = 0;
}

void CDlgImgFxEditor::OnRemoveall() 
{
	if ( m_params == NULL ) return;

	// Lose all effects
	m_params->Destroy();
	
	FillList();

	// Reload the bitmap
//	if ( m_pfx != NULL && m_pfx->bmp != NULL )
//		m_pfx->bmp->Destroy();

//	else CAPTURE()->GrabFrame();
//	OVERLAY().ReGen();

	APP()->ReGen( TRUE );

}

void CDlgImgFxEditor::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnAdd();
	
	*pResult = 0;
}


void CDlgImgFxEditor::SetTitle(LPCTSTR pTitle)
{
	if ( pTitle == NULL ) m_sTitle = "";
	else m_sTitle = pTitle;

	if ( IsWindow( GetSafeHwnd() ) ) UpdateData( FALSE );
}
