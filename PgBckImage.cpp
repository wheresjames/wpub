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
// PgBckImage.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgBckImage.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgBckImage dialog


CPgBckImage::CPgBckImage(CWnd* pParent /*=NULL*/)
	: CDialog(CPgBckImage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgBckImage)
	m_nBckType = -1;
	m_sImage = _T("");
	m_bMotionOverlay = FALSE;
	//}}AFX_DATA_INIT
}


void CPgBckImage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgBckImage)
	DDX_Control(pDX, IDC_COLOR, m_bckcolor);
	DDX_Control(pDX, IDC_VIDEO, m_comboVideo);
	DDX_Control(pDX, IDC_IMAGE, m_comboImage);
	DDX_Radio(pDX, IDC_TYPE1, m_nBckType);
	DDX_CBString(pDX, IDC_IMAGE, m_sImage);
	DDX_Check(pDX, IDC_MOTIONOVERLAY, m_bMotionOverlay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgBckImage, CDialog)
	//{{AFX_MSG_MAP(CPgBckImage)
	ON_BN_CLICKED(IDC_TYPE1, OnType1)
	ON_BN_CLICKED(IDC_TYPE2, OnType2)
	ON_BN_CLICKED(IDC_TYPE3, OnType3)
	ON_CBN_EDITCHANGE(IDC_IMAGE, OnEditchangeImage)
	ON_CBN_SELCHANGE(IDC_IMAGE, OnSelchangeImage)
	ON_BN_CLICKED(IDC_CHOOSE_IMAGE, OnChooseImage)
	ON_BN_CLICKED(IDC_CHOOSE_COLOR, OnChooseColor)
	ON_BN_CLICKED(IDC_MOTIONOVERLAY, OnMotionoverlay)
	ON_CBN_DROPDOWN(IDC_IMAGE, OnDropdownImage)
	ON_CBN_SELCHANGE(IDC_VIDEO, OnSelchangeVideo)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgBckImage message handlers

BOOL CPgBckImage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDrivers();
	
	OnRefresh( 0, 0L );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgBckImage::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return FALSE;

	DWORD type = 0;
	ppii->cfg->GetValue( "Settings", "Type", &type );
	m_nBckType = type;

	// Update image name
	char bckimg[ CWF_STRSIZE ] = { 0 };
	ppii->cfg->GetValue( "Settings", "BckImage", bckimg, sizeof( bckimg ) );
	m_sImage = bckimg;

	// Set background color
	m_bckcolor.SetColor( ppii->rgbbck );

	if ( m_nBckType == 1 )
	{	DWORD driver = ppii->cfg->GetDword( "Settings", "Driver", MAXDWORD );
		if ( driver != MAXDWORD )
		{	LPVIDEOINFO pvi = VIDEOLIST().FindByIndex( driver );
			if ( pvi != NULL ) m_bMotionOverlay = ( pvi->f1 & VIF1_MOTIONOVERLAY ) != 0;
		} // end if

	} // end if

	UpdateData( FALSE );

	DoEnable();

	return 0;
}

void CPgBckImage::DoEnable()
{
	UpdateData( TRUE );

	CWnd *pWnd = GetDlgItem( IDC_COLOR );
	if ( pWnd != NULL ) 
	{	pWnd->EnableWindow( m_nBckType == 0 );
		if ( m_nBckType == 0 ) pWnd->RedrawWindow();
	} // end if
	pWnd = GetDlgItem( IDC_CHOOSE_COLOR );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nBckType == 0 );

	pWnd = GetDlgItem( IDC_VIDEO );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nBckType == 1 );

	pWnd = GetDlgItem( IDC_IMAGE );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nBckType == 2 );
	pWnd = GetDlgItem( IDC_CHOOSE_IMAGE );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nBckType == 2 );

	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	DWORD driver = MAXDWORD;
	ppii->cfg->GetValue( "Settings", "Driver", &driver );
	if ( driver == MAXDWORD ) return;

	LPVIDEOINFO pvi = VIDEOLIST().FindByIndex( driver );
	if ( pvi == NULL ) return;

	pWnd = GetDlgItem( IDC_MOTIONOVERLAY );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nBckType == 1 && pvi->motion.bEnable );

}

void CPgBckImage::FillComboBox()
{
	m_comboImage.ResetContent();

	char path[ CWF_STRSIZE ];

	// Get base directory
	DWORD attrib = GetFileAttributes( m_sImage );
	if ( m_sImage.IsEmpty() || attrib == MAXDWORD )
	{	CWinFile::GetExePath( path, sizeof( path ) );
		CWinFile::BuildPath( path, path, "Overlay Images" );
	} // end if
	else
	{
		if( ( attrib & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
			CWinFile::GetPathFromFileName( m_sImage, path );
		else strcpy( path, m_sImage );
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
	m_comboImage.SetWindowText( m_sImageFile );
}

void CPgBckImage::GetDrivers()
{
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

	// Select first driver
	m_comboVideo.SetCurSel( 0 );
}


void CPgBckImage::OnType1() 
{	DoEnable();

	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;
	ppii->cfg->SetValue( "Settings", "Type", (DWORD)0 );
	APP()->ReGen();
	VIDEOLIST().SetFrameRates( MAXDWORD );
}

void CPgBckImage::OnType2() 
{	DoEnable();

	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;
	ppii->cfg->SetValue( "Settings", "Type", (DWORD)1 );
	APP()->ReGen();
	VIDEOLIST().SetFrameRates( MAXDWORD );
}

void CPgBckImage::OnType3() 
{	DoEnable();

	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;
	ppii->cfg->SetValue( "Settings", "Type", (DWORD)2 );
	APP()->ReGen();
	VIDEOLIST().SetFrameRates( MAXDWORD );
}

void CPgBckImage::OnEditchangeImage() 
{
	CComboBox *pCombo = (CComboBox*)GetDlgItem( IDC_IMAGE );
	if ( pCombo == NULL ) return;
	pCombo->GetWindowText( m_sImage );

	// Punt if no filename
	if ( m_sImage.IsEmpty() ) return;
	
	char path[ CWF_STRSIZE ];
	strcpy_sz( path, m_sImage );

	// Build path
	if ( !CWinFile::DoesExist( path ) )
	{	CWinFile::GetPathFromFileName( m_sImageFile, path );
		CWinFile::BuildPath( path, path, m_sImage );
		if ( !CWinFile::DoesExist( path ) ) return;
	} // end if

	// Save the valid image file
	m_sImageFile = path;

	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	// Update image name
	ppii->cfg->SetValue( "Settings", "BckImage", m_sImageFile );
	ppii->bck->Destroy();

	APP()->ReGen();

}

void CPgBckImage::OnSelchangeImage() 
{
	int sel = m_comboImage.GetCurSel();
	if ( sel == CB_ERR ) return;

	// Get current file name
	char fname[ CWF_STRSIZE ];
	if ( m_comboImage.GetLBText( sel, fname ) == CB_ERR )
		return;

	char path[ CWF_STRSIZE ];
	CWinFile::GetPathFromFileName( m_sImage, path );
	CWinFile::BuildPath( path, path, fname );
	
	// Get file attributes
	DWORD attrib = GetFileAttributes( path );
	if ( attrib == MAXDWORD ) return;

	// Is it a directory?
	if ( ( attrib & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) return;

	// Same file?
	if ( !strcmpi( m_sImage, path ) ) return;

	// Build path to file
	m_sImageFile = path;

	// Update display
	m_sImage = m_sImageFile;
	m_comboImage.SetWindowText( m_sImage );
	m_comboImage.SetCurSel( m_comboImage.AddString( m_sImage ) );

	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	// Update image name
	ppii->cfg->SetValue( "Settings", "BckImage", m_sImageFile );
	ppii->bck->Destroy();

	APP()->ReGen();
}

void CPgBckImage::OnChooseImage() 
{
	// Get the file name
	char filename[ CWF_STRSIZE ] = { 0 };
	if ( !CWinFile::GetOpenFileName(	filename, "Choose Background Image",
										"All Images (*.bmp;*.jpg;*.jpeg;*.png;*.ico;*.tif;*.tiff;*.tga;*.pcx;*.wmf;*.emf;*.jp2)\x0*.bmp;*.jpg;*.jpeg;*.png;*.ico;*.tif;*.tiff;*.tga;*.pcx;*.wmf;*.emf;*.jp2\x0"
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

	m_sImage = filename;
	m_sImageFile = m_sImage;
	UpdateData( FALSE );

	FillComboBox();

	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	// Update image name
	ppii->cfg->SetValue( "Settings", "BckImage", m_sImageFile );
	ppii->bck->Destroy();

	APP()->ReGen();
}

void CPgBckImage::OnChooseColor() 
{
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	// Choose a new color
	CWinFile::ChooseColor( &ppii->rgbbck, GetSafeHwnd() );	

	m_bckcolor.SetColor( ppii->rgbbck );

	APP()->ReGen();
}

void CPgBckImage::OnMotionoverlay() 
{
	UpdateData( TRUE );
	
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	DWORD driver = MAXDWORD;
	ppii->cfg->GetValue( "Settings", "Driver", &driver );
	if ( driver == MAXDWORD ) return;

	LPVIDEOINFO pvi = VIDEOLIST().Add( driver );
	if ( pvi == NULL ) return;
	
	if ( m_bMotionOverlay ) pvi->f1 |= VIF1_MOTIONOVERLAY;
	else pvi->f1 &= ~VIF1_MOTIONOVERLAY;
				
	APP()->ReGen();
}

void CPgBckImage::OnDropdownImage() 
{	FillComboBox();
}

void CPgBckImage::OnSelchangeVideo() 
{
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	int sel = m_comboVideo.GetCurSel();
	if ( sel == -1 ) return;

	// Set new driver
	ppii->cfg->SetValue( "Settings", "Driver", (DWORD)m_comboVideo.GetItemData( sel ) );

	// Get app
	APP()->ReGen();
	VIDEOLIST().SetFrameRates( MAXDWORD );
}
