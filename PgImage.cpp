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
// PgImage.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgImage.h"
#include "ImgEffects.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgImage dialog


CPgImage::CPgImage(CWnd* pParent /*=NULL*/)
	: CDialog(CPgImage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgImage)
	m_sTranslucency = _T("");
	m_bRealSize = FALSE;
	m_sFilename = _T("");
	m_bAspect = FALSE;
	//}}AFX_DATA_INIT
}


void CPgImage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgImage)
	DDX_Control(pDX, IDC_FILES, m_comboFiles);
	DDX_Text(pDX, IDC_TRANSLUCENCY, m_sTranslucency);
	DDX_Check(pDX, IDC_REALSIZE, m_bRealSize);
	DDX_CBString(pDX, IDC_FILES, m_sFilename);
	DDX_Check(pDX, IDC_ASPECT, m_bAspect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgImage, CDialog)
	//{{AFX_MSG_MAP(CPgImage)
	ON_BN_CLICKED(IDC_CHOOSE_FILENAME, OnChooseFilename)
	ON_EN_CHANGE(IDC_TRANSLUCENCY, OnChangeTranslucency)
	ON_CBN_SELCHANGE(IDC_FILES, OnSelchangeFiles)
	ON_BN_CLICKED(IDC_REALSIZE, OnRealsize)
	ON_BN_CLICKED(IDC_EFFECTS, OnEffects)
	ON_CBN_EDITCHANGE(IDC_FILES, OnEditchangeFiles)
	ON_BN_CLICKED(IDC_ASPECT, OnAspect)
	ON_CBN_DROPDOWN(IDC_FILES, OnDropdownFiles)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgImage message handlers

BOOL CPgImage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgImage::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	m_sFilename = "";
	m_comboFiles.ResetContent();

	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return -1; }

	char num[ 64 ];

	sprintf( num, "%.2f", ( (double)pfx->translucency ) / ( (double)100 ) );
	m_sTranslucency = num;

//	wsprintf( num, "%lu", pfx->terr );
//	m_sColorErr = num;

	m_sFilename = pfx->data;	

	FillCombo();

	if ( ( pfx->f1 & FXF1_REALSIZE ) != 0 ) m_bRealSize = TRUE;
	else m_bRealSize = FALSE;

	m_bAspect = ( pfx->f1 & FXF1_ASPECT ) != 0;

	UpdateData( FALSE );

	return 0;
}

void CPgImage::OnChooseFilename() 
{
	char filename[ CWF_STRSIZE ] = { 0 };

	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	if ( *pfx->data == 0 )
	{	CWinFile::GetExePath( filename, sizeof( filename ) );
		CWinFile::BuildPath( filename, filename, "Overlay Images" );
	} // end if

	// Start with old file
	else strcpy( filename, pfx->data );

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
	m_sFilename = filename;
	strcpy_sz( pfx->data, filename );
	
	UpdateData( FALSE );

	FillCombo();

	APP()->ReGen();
}

/*
void CPgImage::OnChangeFilename() 
{
	UpdateData( TRUE );

	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	// Ensure valid file
	if ( !m_sFilename.IsEmpty() )
		if ( !CWinFile::DoesExist( m_sFilename ) ) return;

	// Save filename
	strcpy_sz( pfx->data, m_sFilename );

	char fname[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( m_sFilename, fname );
	if ( *fname ) m_comboFiles.SelectString( -1, fname );

	APP()->ReGen();
}
*/

void CPgImage::OnChangeTranslucency() 
{
	UpdateData( TRUE );

	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	pfx->translucency = (DWORD)( strtod( m_sTranslucency, NULL ) * 100 );
	if ( pfx->translucency > 10000 ) 
	{
		pfx->translucency = 10000;
		m_sTranslucency = "100.00";
		UpdateData( FALSE );
	} // end if

	APP()->ReGen();
}


void CPgImage::OnSelchangeFiles() 
{
	int sel = m_comboFiles.GetCurSel();
	if ( sel == CB_ERR ) return;

	// Get current file name
	char fname[ CWF_STRSIZE ];
	if ( m_comboFiles.GetLBText( sel, fname ) == CB_ERR )
		return;

	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	char path[ CWF_STRSIZE ];
	CWinFile::GetPathFromFileName( pfx->data, path );
	CWinFile::BuildPath( path, path, fname );
	
	// Get file attributes
	DWORD attrib = GetFileAttributes( path );
	if ( attrib == MAXDWORD ) return;

	// Is it a directory?
	if ( ( attrib & FILE_ATTRIBUTE_DIRECTORY ) != 0 ) return;

	// Same file?
	if ( !strcmpi( pfx->data, path ) ) return;

	// Build path to file
	strcpy( pfx->data, path );

	// Update display
	m_sFilename = path;	
	m_comboFiles.SetWindowText( m_sFilename );

	// Update
	APP()->ReGen( TRUE );
}


BOOL CPgImage::FillCombo()
{
	m_comboFiles.ResetContent();

	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return FALSE;

	char path[ CWF_STRSIZE ];

	// Get base directory
	DWORD attrib = GetFileAttributes( pfx->data );
	if ( *pfx->data == 0 || attrib == MAXDWORD )
	{	CWinFile::GetExePath( path, sizeof( path ) );
		CWinFile::BuildPath( path, path, "Overlay Images" );
	} // end if
	else
	{
		if( ( attrib & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
			CWinFile::GetPathFromFileName( pfx->data, path );
		else strcpy( path, pfx->data );
	} // end else		
	
	WIN32_FIND_DATA		wfd;
	char				search[ CWF_STRSIZE ];
	HANDLE				hFind;

	// Get first file
	CWinFile::BuildPath( search, path, "*.*" );

	// Punt if empty
	hFind = FindFirstFile( search, &wfd );
	if ( hFind == INVALID_HANDLE_VALUE ) return FALSE;
	
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
				m_comboFiles.AddString( wfd.cFileName );

		} // end if

	// Get the next file or directory
	} while ( FindNextFile( hFind, &wfd ) );
	
	// Close the find handle
	FindClose( hFind );

	// Set the filename
	m_comboFiles.SetWindowText( m_sFilename );

	// Select current file
//	if ( *cur ) m_comboFiles.SelectString( -1, cur );

	return TRUE;
}

void CPgImage::OnRealsize() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return; }
	if ( m_bRealSize ) pfx->f1 |= FXF1_REALSIZE;
	else pfx->f1 &= ~FXF1_REALSIZE;
	APP()->ReGen();
}

void CPgImage::OnEffects() 
{
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	// Create fx object if needed
	if ( pfx->imgfx == NULL ) pfx->imgfx = new CImgEffects();
	if ( pfx->imgfx == NULL ) return;


	char fname[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( pfx->data, fname );

	// Set the image params
	pfx->imgfx->SetFx( fname, RW( pfx->rect ), RH( pfx->rect ) );
		
	// Edit this effect list
//	pfx->imgfx->SetFx( pfx );
	pfx->imgfx->Edit( NULL );
}

void CPgImage::OnEditchangeFiles() 
{
	CComboBox *pCombo = (CComboBox*)GetDlgItem( IDC_FILES );
	if ( pCombo == NULL ) return;
	pCombo->GetWindowText( m_sFilename );

	// Punt if no filename
	if ( m_sFilename.IsEmpty() ) return;
	
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	char path[ CWF_STRSIZE ];
	strcpy_sz( path, m_sFilename );

	// Build path
	if ( !CWinFile::DoesExist( path ) )
	{	CWinFile::GetPathFromFileName( pfx->data, path );
		CWinFile::BuildPath( path, path, m_sFilename );
		if ( !CWinFile::DoesExist( path ) ) return;
	} // end if

	// Save filename
	strcpy_sz( pfx->data, path );

/*	char fname[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( m_sFilename, fname );
	if ( *fname ) m_comboFiles.SelectString( -1, fname );
*/
	APP()->ReGen();
}


void CPgImage::OnAspect() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	// Toggle bit
	if ( m_bAspect ) pfx->f1 |= FXF1_ASPECT;
	else pfx->f1 &= ~FXF1_ASPECT;

	APP()->ReGen();
}

void CPgImage::OnDropdownFiles() 
{
	FillCombo();
}
