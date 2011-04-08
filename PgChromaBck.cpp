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
// PgChromaBck.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgChromaBck.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgChromaBck dialog


CPgChromaBck::CPgChromaBck(CWnd* pParent /*=NULL*/)
	: CDialog(CPgChromaBck::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgChromaBck)
	m_nBckType = 0;
	m_sImage = _T("");
	//}}AFX_DATA_INIT
}


void CPgChromaBck::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgChromaBck)
	DDX_Control(pDX, IDC_CHOOSE_IMAGE, m_chooseimage);
	DDX_Control(pDX, IDC_VIDEO, m_comboVideo);
	DDX_Control(pDX, IDC_PANEL, m_comboPanel);
	DDX_Control(pDX, IDC_IMAGE, m_comboImage);
	DDX_Radio(pDX, IDC_BCKTYPE, m_nBckType);
	DDX_CBString(pDX, IDC_IMAGE, m_sImage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgChromaBck, CDialog)
	//{{AFX_MSG_MAP(CPgChromaBck)
	ON_BN_CLICKED(IDC_BCKTYPE, OnBcktype)
	ON_BN_CLICKED(IDC_BCKTYPE2, OnBcktype2)
	ON_BN_CLICKED(IDC_BCKTYPE3, OnBcktype3)
	ON_CBN_SELCHANGE(IDC_IMAGE, OnSelchangeImage)
	ON_CBN_SELCHANGE(IDC_PANEL, OnSelchangePanel)
	ON_CBN_SELCHANGE(IDC_VIDEO, OnSelchangeVideo)
	ON_BN_CLICKED(IDC_CHOOSE_IMAGE, OnChooseImage)
	ON_CBN_EDITCHANGE(IDC_IMAGE, OnEditchangeImage)
	ON_CBN_DROPDOWN(IDC_IMAGE, OnDropdownImage)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgChromaBck message handlers

BOOL CPgChromaBck::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Set button icons
	m_chooseimage.SetIcon( CGrDC::LoadIcon( IDI_DIR_CLOSED ) );

	FillCombos();

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPgChromaBck::DoEnable()
{
	UpdateData( TRUE );

	CWnd *pCtrl = GetDlgItem( IDC_IMAGE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_nBckType == 0 );
	pCtrl = GetDlgItem( IDC_CHOOSE_IMAGE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_nBckType == 0 );

	pCtrl = GetDlgItem( IDC_PANEL );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_nBckType == 1 );

	pCtrl = GetDlgItem( IDC_VIDEO );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_nBckType == 2 );

}

LRESULT CPgChromaBck::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) { DoEnable(); return FALSE; }

//	m_sImage = "";
//	m_comboPanel.SetCurSel( -1 );
//	m_comboVideo.SetCurSel( -1 );

	// Initialize variables
	if ( ( ppii->f1 & PIIF1_KEYIMG ) != 0 ) 
	{	m_nBckType = 0; m_sImage = ppii->keyimg; m_comboImage.SetWindowText( m_sImage ); }
	else if ( ( ppii->f1 & PIIF1_KEYDYNAMIC ) != 0 ) 
	{	m_nBckType = 1; m_comboPanel.SelectString( -1, ppii->keyimg ); }
	else if ( ( ppii->f1 & PIIF1_KEYVIDEO ) != 0 ) 
	{	m_nBckType = 2; CComCtrl::ComboSelectItemByData( m_comboVideo.GetSafeHwnd(), ppii->keydriver ); }
	else m_nBckType = -1;
	
	UpdateData( FALSE );

	DoEnable();

	return 1;
}

void CPgChromaBck::OnBcktype() 
{	DoEnable();

	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;
	
	ppii->f1 &= ~( PIIF1_KEYIMG | PIIF1_KEYDYNAMIC | PIIF1_KEYVIDEO );
	ppii->f1 |= PIIF1_KEYIMG;
	strcpy( ppii->keyimg, m_sImage );

	APP()->ReGen();
}

void CPgChromaBck::OnBcktype2() 
{	DoEnable();

	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;
	
	ppii->f1 &= ~( PIIF1_KEYIMG | PIIF1_KEYDYNAMIC | PIIF1_KEYVIDEO );
	ppii->f1 |= PIIF1_KEYDYNAMIC;

	int sel = m_comboPanel.GetCurSel();
	if ( sel == CB_ERR ) return;

	CString str;
	m_comboPanel.GetLBText( sel, str );

	strcpy( ppii->keyimg, str );

	APP()->ReGen();
}

void CPgChromaBck::OnBcktype3() 
{	DoEnable();

	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;
	
	ppii->f1 &= ~( PIIF1_KEYIMG | PIIF1_KEYDYNAMIC | PIIF1_KEYVIDEO );
	ppii->f1 |= PIIF1_KEYVIDEO;

	int sel = m_comboVideo.GetCurSel();
	if ( sel == CB_ERR ) return;

	CString str;
	m_comboVideo.GetLBText( sel, str );
	
	ppii->keydriver = m_comboVideo.GetItemData( sel );
	strcpy( ppii->keyimg, str );

	APP()->ReGen();
}

void CPgChromaBck::FillCombos()
{

	// Add dynamic video data
	m_comboPanel.ResetContent();
	LPPUBIMGINFO ppii = NULL;
	while( ( ppii = (LPPUBIMGINFO)IMGLIST().GetNext( ppii ) ) != NULL )
		if ( ppii != IMGLIST().GetSelImage() )
			m_comboPanel.AddString( ppii->name );

	m_comboVideo.ResetContent();

	CCapture	cap;
	DWORD		drivers = cap.GetNumDrivers();

	// For each driver
	for ( DWORD i = 0; i < drivers; i++ )
	{
		char				show[ CWF_STRSIZE ];
		CAPTUREDRIVERINFO	ci;

		if ( cap.GetDriverInfo( i, &ci ) )
		{
			sprintf( show, "%s (%s)", ci.szName, ci.szVer );

			int index = m_comboVideo.AddString( show );
			if ( index != LB_ERR ) m_comboVideo.SetItemData( index, ci.index );
		} // end if

	} // end for	

}

void CPgChromaBck::OnSelchangeImage() 
{
	int sel = m_comboImage.GetCurSel();
	if ( sel == CB_ERR ) return;

	// Get the new text
	m_comboImage.GetLBText( sel, m_sImage );

	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	char path[ CWF_STRSIZE ];
	CWinFile::GetPathFromFileName( ppii->keyimg, path );

	DWORD attrib = GetFileAttributes( path );
	if ( *path == 0 || attrib == MAXDWORD )
	{	CWinFile::GetExePath( path, sizeof( path ) );
		CWinFile::BuildPath( path, path, "Overlay Images" );
	} // end if

	CWinFile::BuildPath( path, path, m_sImage );
	
	// Get file attributes
	attrib = GetFileAttributes( path );
	if ( attrib == MAXDWORD ) return;

	// Is it a directory?
	if ( ( attrib & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) return;

	// Same file?
	if ( !strcmpi( ppii->keyimg, path ) ) return;

	// Build path to file
	strcpy( ppii->keyimg, path );

	// Update
	APP()->ReGen( TRUE );

	// Update display
	m_sImage = path;	
	m_comboImage.SetCurSel( -1 );
	m_comboImage.SetWindowText( m_sImage );

	if ( m_comboImage.SelectString( -1, m_sImage ) == CB_ERR )
		m_comboImage.SetCurSel( m_comboImage.AddString( m_sImage ) );

}

void CPgChromaBck::OnSelchangePanel() 
{
	int sel = m_comboPanel.GetCurSel();
	if ( sel == CB_ERR ) return;

	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	// Get the new text
	CString str;
	m_comboPanel.GetLBText( sel, str );

	strcpy( ppii->keyimg, str );

	APP()->ReGen();
}

void CPgChromaBck::OnSelchangeVideo() 
{
	int sel = m_comboVideo.GetCurSel();
	if ( sel == CB_ERR ) return;

	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	// Get the new text
	CString str;
	m_comboVideo.GetLBText( sel, str );

	// Set data to object
	ppii->keydriver = m_comboVideo.GetItemData( sel );
	strcpy( ppii->keyimg, str );

	APP()->ReGen();
}

void CPgChromaBck::FillFileCombo()
{
	m_comboImage.ResetContent();

	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	// Are we using images?
	if ( ( ppii->f1 & PIIF1_KEYIMG ) == 0 ) return;

	char path[ CWF_STRSIZE ];

	// Get base directory
	DWORD attrib = GetFileAttributes( ppii->keyimg );
	if ( *ppii->keyimg == 0 || attrib == MAXDWORD )
	{	CWinFile::GetExePath( path, sizeof( path ) );
		CWinFile::BuildPath( path, path, "Overlay Images" );
	} // end if
	else
	{
		if( ( attrib & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
			CWinFile::GetPathFromFileName( ppii->keyimg, path );
		else strcpy( path, ppii->keyimg );
	} // end else		
	
	WIN32_FIND_DATA		wfd;
	char				search[ CWF_STRSIZE ];
	HANDLE				hFind;

	// Get first file
	CWinFile::BuildPath( search, path, "*.*" );

	// Punt if empty
	hFind = FindFirstFile( search, &wfd );
	if ( hFind == INVALID_HANDLE_VALUE ) return;
	
	do
	{
		// Do we need to copy this file?
		if (	*wfd.cFileName != NULL &&
				strcmp( wfd.cFileName, "." ) &&
				strcmp( wfd.cFileName, ".." ) && 
				( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
		{
			DWORD type = CWinImg::GetFileType( wfd.cFileName );

			// Add this string if recognized image type
			if ( type != MAXDWORD )
				m_comboImage.AddString( wfd.cFileName );

		} // end if

	// Get the next file or directory
	} while ( FindNextFile( hFind, &wfd ) );
	
	// Close the find handle
	FindClose( hFind );

	// Set the filename
	m_comboImage.SetWindowText( m_sImage );

	// Select current file
//	if ( *cur ) m_comboFiles.SelectString( -1, cur );
}

void CPgChromaBck::OnChooseImage() 
{
	char filename[ CWF_STRSIZE ] = { 0 };

	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	if ( *ppii->keyimg == 0 )
	{	CWinFile::GetExePath( filename, sizeof( filename ) );
		CWinFile::BuildPath( filename, filename, "Overlay Images" );
	} // end if

	// Start with old file
	else strcpy( filename, ppii->keyimg );

	// Get user filename
	if ( !CWinFile::GetOpenFileName(	filename, "Image File",
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
	m_sImage = filename;
	strcpy_sz( ppii->keyimg, filename );
	
	UpdateData( FALSE );

	APP()->ReGen();
}

void CPgChromaBck::OnEditchangeImage() 
{
	m_comboImage.GetWindowText( m_sImage );

	// Punt if no filename
	if ( m_sImage.IsEmpty() ) return;
	
	// Get file attributes
	DWORD attrib = GetFileAttributes( m_sImage );
	if ( attrib == MAXDWORD ) return;

	// Is it a directory?
	if ( ( attrib & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) return;

	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	// Update image name
	strcpy( ppii->keyimg, m_sImage );

	APP()->ReGen();
}

void CPgChromaBck::OnDropdownImage() 
{
	FillFileCombo();
}
