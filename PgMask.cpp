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
// PgMask.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgMask.h"

#include "MainFrm.h"

#include "VpDoc.h"
#include "VpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_LOADNEXTMASKIMAGE		( WM_USER + 101 )

/////////////////////////////////////////////////////////////////////////////
// CPgMask dialog


CPgMask::CPgMask(CWnd* pParent /*=NULL*/)
	: CDialog(CPgMask::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgMask)
	m_bMask = FALSE;
	m_sMaskFile = _T("");
	m_bMaskImg = FALSE;
	//}}AFX_DATA_INIT

	m_cur = NULL;
}


void CPgMask::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgMask)
	DDX_Control(pDX, IDC_CHOOSE_MASKFILE, m_btnChooseColor);
	DDX_Control(pDX, IDC_COLORBOX, m_maskcolor);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Check(pDX, IDC_MASK, m_bMask);
	DDX_Text(pDX, IDC_MASKFILE, m_sMaskFile);
	DDX_Check(pDX, IDC_MASKIMG, m_bMaskImg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgMask, CDialog)
	//{{AFX_MSG_MAP(CPgMask)
	ON_BN_CLICKED(IDC_CHOOSE_MASKFILE, OnChooseMaskfile)
	ON_BN_CLICKED(IDC_MASK, OnMask)
	ON_BN_CLICKED(IDC_CHOOSE_COLOR, OnChooseColor)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_EN_CHANGE(IDC_MASKFILE, OnChangeMaskfile)
	ON_BN_CLICKED(IDC_MASKIMG, OnMaskimg)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )
	ON_MESSAGE( WM_LOADNEXTMASKIMAGE, OnLoadNextMaskImage )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgMask message handlers

BOOL CPgMask::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_btnChooseColor.SetIcon( CGrDC::LoadIcon( IDI_FOLDER ) );

	m_cur = NULL;
	OnRefresh( 0, 0L );
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgMask::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return FALSE;

	m_bMask = ( ppii->f1 & PIIF1_MASK ) != 0;
	m_bMaskImg = ( ppii->f1 & PIIF1_MASKIMG ) != 0;
	
	m_sMaskFile = ppii->mask;

	m_maskcolor.SetColor( ppii->rgbmask );

	UpdateData( FALSE );

	DoEnable();

	// Sort of a hack
	if ( m_cur != ppii )
	{	m_cur = ppii; FillList(); }

	return 0;
}

void CPgMask::OnChooseMaskfile() 
{
	char fname[ CWF_STRSIZE ] = { 0 };
	char path[ CWF_STRSIZE ];

	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	if ( *ppii->mask == 0 )
	{	CWinFile::GetExePath( fname, sizeof( fname ) );
		CWinFile::BuildPath( fname, fname, "Frame Images\\*" );
	} // end if

	// Start with old file
	else strcpy( fname, ppii->mask );

	// Save the path
	CWinFile::GetPathFromFileName( fname, path );

	// Get user filename
	if ( !CWinFile::GetOpenFileName(	fname, "Frame File",
										"All Images (*.bmp;*.jpg;*.jpeg;*.png;*.ico;*.tif;*.tiff;*.tga;*.pcx)\x0*.bmp;*.jpg;*.jpeg;*.png;*.ico;*.tif;*.tiff;*.tga;*.pcx\x0"
										"Windows Bitmap (*.bmp;*.dib)\x0*.bmp;*.dib\x0"
										"JPEG (*.jpg;*.jpeg)\x0*.jpg;*.jpeg\x0"
										"Portable Network Graphic (*.png)\x0*.png\x0"
										"Icon (*.ico)\x0*.ico\x0"
										"Tagged Image File (*.tif; *.tiff)\x0*.tif;*.tiff\x0"
										"Targa (*.tga)\x0*.tga\x0"
										"PC Paintbrush (*.pcx)\x0*.pcx\x0"
										"Windows Metafile (*.wmf;*.emf)\x0*.wmf;*.emf\x0"
										"JPEG 2000 (*.jp2)\x0*.jp2\x0"
										"All Files (*.*)\x0*.*\x0",
										"jpg",
										 GetSafeHwnd() ) ) return;

	// Save the filename
	m_sMaskFile = fname;
	strcpy_sz( ppii->mask, fname );

	// Set mask flag
	CWinFile::GetFileNameFromPath( fname, fname );
	if ( !strnicmp( fname, "mask_", 5 ) ) m_bMaskImg = TRUE;
	else m_bMaskImg = FALSE;

	// Update flag
	if ( m_bMaskImg ) ppii->f1 |= PIIF1_MASKIMG;
	else ppii->f1 &= ~PIIF1_MASKIMG;	

	UpdateData( FALSE );

	// Refill the list if needed
	CWinFile::GetPathFromFileName( m_sMaskFile, fname );
	if ( m_list.GetItemCount() == 0 || strcmpi( fname, path ) ) FillList();
	else
	{	CWinFile::GetFileNameFromPath( m_sMaskFile, fname );
		int sel = CComCtrl::ListFindItemByText( m_list.GetSafeHwnd(), 0, fname );
		if ( sel != -1 ) 
		{	CComCtrl::ListSelectSingleItem( m_list.GetSafeHwnd(), sel );
			m_list.EnsureVisible( sel, FALSE );
		} // end if
	} // end else

	APP()->ReGen();
}

void CPgMask::FillList()
{
	m_imgthread.StopThread();
	m_list.DeleteAllItems();
	m_list.SetImageList( NULL, LVSIL_NORMAL );
	if ( m_thm.GetSafeHandle() != NULL )
		m_thm.DeleteImageList();

	if ( !m_bMask || m_sMaskFile.IsEmpty() ) return;

	m_thm.Create( 42, 42, ILC_MASK | ILC_COLOR24, 0, 1024 );
	m_list.SetImageList( &m_thm, LVSIL_NORMAL );

	// Get directory
	char path[ CWF_STRSIZE ];
	CWinFile::GetPathFromFileName( m_sMaskFile, path );
	if ( !CWinFile::DoesExist( path ) ) return;

	m_imgthread.EnumImages( path, GetSafeHwnd(), WM_LOADNEXTMASKIMAGE );

}


void CPgMask::OnMask() 
{	UpdateData( TRUE );
	FillList();
	DoEnable();

	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	if ( m_bMask ) ppii->f1 |= PIIF1_MASK;
	else ppii->f1 &= ~PIIF1_MASK;

	APP()->ReGen();
}

void CPgMask::OnChooseColor() 
{
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	// Choose a new color
	CWinFile::ChooseColor( &ppii->rgbmask, GetSafeHwnd() );	

	m_maskcolor.SetColor( ppii->rgbmask );

	APP()->ReGen();
}

void CPgMask::DoEnable()
{
	CWnd *pCtrl = GetDlgItem( IDC_COLORBOX );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bMask && m_bMaskImg );

	pCtrl = GetDlgItem( IDC_CHOOSE_COLOR );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bMask && m_bMaskImg );

	pCtrl = GetDlgItem( IDC_MASKFILE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bMask );

	pCtrl = GetDlgItem( IDC_CHOOSE_MASKFILE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bMask );

	pCtrl = GetDlgItem( IDC_LIST );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bMask );

	pCtrl = GetDlgItem( IDC_MASKIMG );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bMask );
}

void CPgMask::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateData( TRUE );

	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	int sel = CComCtrl::ListGetFirstSelectedItem( m_list.GetSafeHwnd() );
	if ( sel == LB_ERR ) return;

	char fname[ CWF_STRSIZE ];
	m_list.GetItemText( sel, 0, fname, sizeof( fname ) );

	char path[ CWF_STRSIZE ];
	CWinFile::GetPathFromFileName( m_sMaskFile, path );
	CWinFile::BuildPath( path, path, fname );
	if ( !CWinFile::DoesExist( path ) ) return;

	m_sMaskFile = path;

	strcpy_sz( ppii->mask, path );

	// Set mask flag
	CWinFile::GetFileNameFromPath( path, path );
	if ( !strnicmp( path, "mask_", 5 ) ) m_bMaskImg = TRUE;
	else m_bMaskImg = FALSE;

	// Update flag
	if ( m_bMaskImg ) ppii->f1 |= PIIF1_MASKIMG;
	else ppii->f1 &= ~PIIF1_MASKIMG;	
	
	UpdateData( FALSE );
	
	APP()->ReGen();

	*pResult = 0;
}

void CPgMask::OnChangeMaskfile() 
{
	UpdateData( TRUE );

	// Does it exist?
	DWORD attrib = GetFileAttributes( m_sMaskFile );
	if ( attrib == MAXDWORD || ( attrib & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) 
		return;

	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	strcpy_sz( ppii->mask, m_sMaskFile );

	FillList();	

	APP()->ReGen();
}

LRESULT CPgMask::OnLoadNextMaskImage(WPARAM wParam, LPARAM lParam)
{
	CWinImg	*pImg = (CWinImg*)lParam;
	if ( pImg == NULL ) return 0;

	// Get this file name
	char fname[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( pImg->GetFileName(), fname );

	HICON hIcon = pImg->CreateIcon( 42, 42 );
	int icon = m_thm.Add( hIcon );
	int item = m_list.InsertItem( m_list.GetItemCount(), fname, icon );

	// Get current file name
	char cname[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( m_sMaskFile, cname );

	// Is this the file?
	if ( !strcmpi( cname, fname ) )
	{	m_list.SetItemState( item, LVIS_SELECTED, LVIS_SELECTED );
		m_list.EnsureVisible( item, FALSE );
	} // end if

	return 1;
}

BOOL CPgMask::DestroyWindow() 
{
	m_imgthread.StopThread();	
	return CDialog::DestroyWindow();
}

void CPgMask::OnMaskimg() 
{	UpdateData( TRUE ); DoEnable();

	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	if ( m_bMaskImg ) ppii->f1 |= PIIF1_MASKIMG;
	else ppii->f1 &= ~PIIF1_MASKIMG;

	APP()->ReGen();	
}
