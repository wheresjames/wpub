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
// DlgServerMap.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgServerMap.h"

#include "DlgTreeFrame.h"

#include "DlgMapImage.h"
#include "DlgMapFolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgServerMap dialog


CDlgServerMap::CDlgServerMap(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgServerMap::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgServerMap)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDlgServerMap::~CDlgServerMap()
{
	m_images.Detach();
	m_imagesN.Detach();
	m_dirimages.DeleteImageList();
}

void CDlgServerMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgServerMap)
	DDX_Control(pDX, IDC_MAPLIST, m_maplist);
	DDX_Control(pDX, IDC_IMGLIST, m_imglist);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgServerMap, CDialog)
	//{{AFX_MSG_MAP(CDlgServerMap)
	ON_BN_CLICKED(IDC_ADDIMG, OnAddimg)
	ON_BN_CLICKED(IDC_REMOVEIMG, OnRemoveimg)
	ON_BN_CLICKED(IDC_ADDMAP, OnAddmap)
	ON_BN_CLICKED(IDC_REMOVEMAP, OnRemovemap)
	ON_NOTIFY(NM_DBLCLK, IDC_IMGLIST, OnDblclkImglist)
	ON_NOTIFY(NM_DBLCLK, IDC_MAPLIST, OnDblclkMaplist)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_SAVE, OnSave )
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgServerMap message handlers

BOOL CDlgServerMap::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Testing...
//	WEBSETTINGS().SetValue( "ImageMap", "webcam.jpg", (DWORD)1 );

	RECT rect;
	m_imglist.GetClientRect( &rect );
	long w = ( rect.right - rect.left ) - GetSystemMetrics( SM_CXVSCROLL ) - 4;
	m_imglist.InsertColumn( 0, "Virtual File Name", LVCFMT_LEFT, w >> 1 );
	m_imglist.InsertColumn( 1, "Image Name", LVCFMT_LEFT, w >> 1 );

	ListCtrl_SetExtendedListViewStyle( m_imglist.GetSafeHwnd(), LVS_EX_FULLROWSELECT );
	
	m_maplist.GetClientRect( &rect );
	w = ( rect.right - rect.left ) - GetSystemMetrics( SM_CXVSCROLL ) - 4;
	m_maplist.InsertColumn( 0, "Virtual Folder Name", LVCFMT_LEFT, w >> 1 );
	m_maplist.InsertColumn( 1, "Disk Folder", LVCFMT_LEFT, w >> 1 );
	
	m_dirimages.Create( 16, 16, ILC_MASK | ILC_COLOR24, 0, 32 );
	m_dirimages.Add( AfxGetApp()->LoadIcon( IDI_FOLDER ) );
		
	m_maplist.SetImageList( &m_dirimages, LVSIL_NORMAL );
	m_maplist.SetImageList( &m_dirimages, LVSIL_SMALL );

	ListCtrl_SetExtendedListViewStyle( m_maplist.GetSafeHwnd(), LVS_EX_FULLROWSELECT );

	OnRefresh( 0, 0L );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgServerMap::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	FillImgList();
	FillMapList();
	return 0;
}

LRESULT CDlgServerMap::OnSave(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void CDlgServerMap::OnAddimg() 
{
	CDlgMapImage	dlg;
	if ( dlg.DoModal() != IDOK ) return;

	char fname[ CWF_STRSIZE ];
	char txt[ CWF_STRSIZE ];
	strcpy_sz( txt, dlg.m_sMapTo );
	DWORD i = 0;
	while ( i < CWF_STRSIZE && txt[ i ] != 0 &&
			( txt[ i ] == '\\' || txt[ i ] == '/' ) ) i++;
	strcpy_sz( fname, &txt[ i ] );

	// Set new image map
	WEBSETTINGS().Set( "ImageMap", fname, dlg.m_sImage );

	FillImgList();
}

void CDlgServerMap::OnRemoveimg() 
{
	int sel = CComCtrl::ListGetFirstSelectedItem( m_imglist.GetSafeHwnd() );
	if ( sel == LB_ERR ) return;

	// Remove the selected item from the list
	CString str = m_imglist.GetItemText( sel, 0 );
	WEBSETTINGS().DeleteValue( "ImageMap", str );
	FillImgList();
}

void CDlgServerMap::OnAddmap() 
{
	CDlgMapFolder	dlg;
	if ( dlg.DoModal() != IDOK ) return;

	char fname[ CWF_STRSIZE ];
	char txt[ CWF_STRSIZE ];
	strcpy_sz( txt, dlg.m_sVirtualFolder );
	DWORD i = 0;
	while ( i < CWF_STRSIZE && txt[ i ] != 0 &&
			( txt[ i ] == '\\' || txt[ i ] == '/' ) ) i++;
	strcpy_sz( fname, &txt[ i ] );

	WEBSETTINGS().Set( "FolderMap", fname, dlg.m_sFolder );
	
	FillMapList();
}

void CDlgServerMap::OnRemovemap() 
{
	int sel = CComCtrl::ListGetFirstSelectedItem( m_maplist.GetSafeHwnd() );
	if ( sel == LB_ERR ) return;

	// Remove the selected item from the list
	CString str = m_maplist.GetItemText( sel, 0 );
	WEBSETTINGS().DeleteValue( "FolderMap", str );
	FillMapList();
}

void CDlgServerMap::FillImgList()
{
	m_imglist.DeleteAllItems();

	CRKey *pRk = WEBSETTINGS().GetKey( "ImageMap" );
	if ( pRk == NULL ) return;

	LPREGVALUE	prv = NULL;
	while ( ( prv = (LPREGVALUE)pRk->GetNext( prv ) ) != NULL )
	{
		// Add the item to the list
		int i = m_imglist.InsertItem(	m_imglist.GetItemCount(), (LPCTSTR)prv->cpkey, 
										GetFileIcon( (LPCTSTR)prv->cpkey ) );
		// Set the file name
		m_imglist.SetItemText( i, 1, pRk->GetSz( prv ) );

	} // end while
}

DWORD CDlgServerMap::GetFileIcon(LPCTSTR pFile)
{
	if ( pFile == NULL ) return 0;

	SHFILEINFO	sfi;

	char filetype[ CWF_STRSIZE ];
	const char *ext =CWinFile::GetExtension( pFile );
	if ( ext == NULL ) return 0;
	strcpy( filetype, "file." );
	strcat( filetype, ext );

	HIMAGELIST hImgList;
	// Get large icon for this file type
	hImgList = 
		(HIMAGELIST)SHGetFileInfo(	pFile, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof( sfi ),
									SHGFI_ICON | 
									SHGFI_SYSICONINDEX | SHGFI_LARGEICON );
	if ( hImgList == NULL ) hImgList = 
		(HIMAGELIST)SHGetFileInfo(	filetype, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof( sfi ),
									SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | 
									SHGFI_SYSICONINDEX | SHGFI_LARGEICON );
	if ( hImgList != NULL )
	{
		m_imagesN.Detach();
		m_imagesN.Attach( hImgList );
		m_imglist.SetImageList( &m_imagesN, LVSIL_NORMAL );
	} // end if

	// Get small icon for this file type
	hImgList = 
		(HIMAGELIST)SHGetFileInfo(	pFile, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof( sfi ),
									SHGFI_ICON | 
									SHGFI_SYSICONINDEX | SHGFI_SMALLICON );
	if ( hImgList == NULL ) hImgList = 
		(HIMAGELIST)SHGetFileInfo(	filetype, FILE_ATTRIBUTE_NORMAL, &sfi, sizeof( sfi ),
									SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | 
									SHGFI_SYSICONINDEX | SHGFI_SMALLICON );
	if ( hImgList != NULL )
	{
		m_images.Detach();
		m_images.Attach( hImgList );
		m_imglist.SetImageList( &m_images, LVSIL_SMALL );
	} // end if

	return sfi.iIcon;
}

void CDlgServerMap::FillMapList()
{
	m_maplist.DeleteAllItems();

	CRKey *pRk = WEBSETTINGS().GetKey( "FolderMap" );
	if ( pRk == NULL ) return;

	LPREGVALUE	prv = NULL;
	while ( ( prv = (LPREGVALUE)pRk->GetNext( prv ) ) != NULL )
	{
		int i = 0;

		if (	*prv->cpkey == 0 || !strcmp( prv->cpkey, "/" ) ||
				!strcmp( prv->cpkey, "\\" ) )

			// Add the item to the list
			i = m_maplist.InsertItem( m_imglist.GetItemCount(), "<root>", 0 );

		// Add the item to the list
		else i = m_maplist.InsertItem( m_imglist.GetItemCount(), prv->cpkey, 0 );

		// Set the file name
		m_maplist.SetItemText( i, 1, pRk->GetSz( prv ) );

	} // end while
}


BOOL CDlgServerMap::DestroyWindow() 
{
	m_images.Detach();
	m_imagesN.Detach();
	m_dirimages.DeleteImageList();
	
	return CDialog::DestroyWindow();
}

void CDlgServerMap::OnDblclkImglist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int sel = CComCtrl::ListGetFirstSelectedItem( m_imglist.GetSafeHwnd() );
	if ( sel == LB_ERR ) return;

	// Initialize data
	CDlgMapImage	dlg;
	dlg.m_sMapTo = m_imglist.GetItemText( sel, 0 );
	dlg.m_sImage = m_imglist.GetItemText( sel, 1 );

	CString sOld = dlg.m_sMapTo;

	if ( dlg.DoModal() != IDOK ) return;

	char fname[ CWF_STRSIZE ];
	char txt[ CWF_STRSIZE ];
	strcpy_sz( txt, dlg.m_sMapTo );
	DWORD i = 0;
	while ( i < CWF_STRSIZE && txt[ i ] != 0 &&
			( txt[ i ] == '\\' || txt[ i ] == '/' ) ) i++;
	strcpy_sz( fname, &txt[ i ] );

	// Set new image map
	WEBSETTINGS().DeleteValue( "ImageMap", sOld );
	WEBSETTINGS().Set( "ImageMap", fname, dlg.m_sImage );

	FillImgList();
	
	*pResult = 0;
}

void CDlgServerMap::OnDblclkMaplist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int sel = CComCtrl::ListGetFirstSelectedItem( m_maplist.GetSafeHwnd() );
	if ( sel == LB_ERR ) return;

	CDlgMapFolder	dlg;
	dlg.m_sVirtualFolder = m_maplist.GetItemText( sel, 0 );
	dlg.m_sFolder = m_maplist.GetItemText( sel, 1 );

	CString sOld = dlg.m_sVirtualFolder;

	// Check for root folder
	if ( !strcmpi( dlg.m_sVirtualFolder, "<root>" ) ) dlg.m_sVirtualFolder = "/";

	if ( dlg.DoModal() != IDOK ) return;

	char fname[ CWF_STRSIZE ];
	char txt[ CWF_STRSIZE ];
	strcpy_sz( txt, dlg.m_sVirtualFolder );
	DWORD i = 0;
	while ( i < CWF_STRSIZE && txt[ i ] != 0 &&
			( txt[ i ] == '\\' || txt[ i ] == '/' ) ) i++;
	strcpy_sz( fname, &txt[ i ] );
	CWinFile::CleanChars( fname );

	WEBSETTINGS().DeleteValue( "FolderMap", sOld );
	WEBSETTINGS().Set( "FolderMap", fname, dlg.m_sFolder );
	
	FillMapList();

	*pResult = 0;
}

void CDlgServerMap::OnClose() 
{

	CDialog::OnClose();
}
