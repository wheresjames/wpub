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
// WzCam.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "WzCam.h"

#include "WzFrame.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWzCam dialog


CWzCam::CWzCam(CWnd* pParent /*=NULL*/)
	: CDialog(CWzCam::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWzCam)
	m_sName = _T("");
	m_nBckType = 1;
	m_sImage = _T("");
	m_bCustomSize = FALSE;
	m_sHeight = _T("240");
	m_sWidth = _T("320");
	//}}AFX_DATA_INIT
}


void CWzCam::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWzCam)
	DDX_Control(pDX, IDC_CHOOSE_IMAGE, m_chooseImage);
	DDX_Control(pDX, IDC_IMAGEFILE, m_comboImage);
	DDX_Control(pDX, IDC_COMBO, m_combo);
	DDX_Text(pDX, IDC_NAME, m_sName);
	DDX_Radio(pDX, IDC_BCKTYPE, m_nBckType);
	DDX_CBString(pDX, IDC_IMAGEFILE, m_sImage);
	DDX_Check(pDX, IDC_CUSTOMSIZE, m_bCustomSize);
	DDX_Text(pDX, IDC_HEIGHT, m_sHeight);
	DDX_Text(pDX, IDC_WIDTH, m_sWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWzCam, CDialog)
	//{{AFX_MSG_MAP(CWzCam)
	ON_CBN_SELCHANGE(IDC_IMAGEFILE, OnSelchangeImagefile)
	ON_CBN_EDITCHANGE(IDC_IMAGEFILE, OnEditchangeImagefile)
	ON_BN_CLICKED(IDC_BCKTYPE, OnBcktype)
	ON_BN_CLICKED(IDC_BCKTYPE2, OnBcktype2)
	ON_BN_CLICKED(IDC_BCKTYPE3, OnBcktype3)
	ON_BN_CLICKED(IDC_CUSTOMSIZE, OnCustomsize)
	ON_BN_CLICKED(IDC_CHOOSE_IMAGE, OnChooseImage)
	ON_CBN_DROPDOWN(IDC_IMAGEFILE, OnDropdownImagefile)
	//}}AFX_MSG_MAP

	ON_MESSAGE( CWZ_PAGECLOSE, OnSave ) 

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWzCam message handlers

void CWzCam::FillCombo()
{
	m_combo.ResetContent();

	CCapture	cap;
	DWORD		drivers = cap.GetNumDrivers();

	int			sel = CB_ERR;

	// Load drivers
	for ( DWORD i = 0; i < drivers; i++ )
	{
		char				show[ MAX_PATH ];
		CAPTUREDRIVERINFO	ci;

		if ( cap.GetDriverInfo( i, &ci ) )
		{
			sprintf( show, "%s (%s)", ci.szName, ci.szVer );

			int index = m_combo.AddString( show );
			if ( index != CB_ERR )
			{	m_combo.SetItemData( index, i );
				if ( sel == CB_ERR ) sel = index;
			} // end if
		} // end if

	} // end for

	// Set a selection
	if ( sel != CB_ERR ) m_combo.SetCurSel( sel );
	else m_combo.SetCurSel( 0 );
}

BOOL CWzCam::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Set icon
	m_chooseImage.SetIcon( CGrDC::LoadIcon( IDI_FOLDER ) );

	// Get unique name
	char show[ 256 ];
	DWORD i = IMGLIST().Size() + 1;
	do { wsprintf( show, "Image %lu", i++ ); 
	} while ( IMGLIST().FindByName( show ) != NULL );
	m_sName = show;

	UpdateData( FALSE );
	
	// Set enables
	DoEnable();	

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CWzCam::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	// Get pointer to reg
	CReg *pReg = (CReg*)CWzFrame::GetData( GetSafeHwnd() );

	FillCombo();

	// Select a driver
	if ( pReg != NULL )
	{
		CComCtrl::ComboSelectItemByData( m_combo.GetSafeHwnd(), 
										 pReg->GetDword( "CWzCam", "Driver", 0 ) );

		// Restore name
		m_sName = pReg->GetSz( "CWzCam", "Name", m_sName );

		m_bCustomSize = pReg->GetDword( "CWzCam", "CustomSize" ) != 0;
		m_sWidth = pReg->GetSz( "CWzCam", "Width", m_sWidth );
		m_sHeight = pReg->GetSz( "CWzCam", "Height", m_sHeight );

		m_nBckType = pReg->GetDword( "CWzCam", "BckType", 1 );
		m_sImage = pReg->GetSz( "CWzCam", "ImageFile" );

	} // end if

	FillFileCombo();

	UpdateData( FALSE );

	return 0;
}

LRESULT CWzCam::OnSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );

	// Get pointer to reg
	CReg *pReg = (CReg*)CWzFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return 0;

	pReg->Set( "CWzCam", "Name", (LPCTSTR)m_sName );

	pReg->Set( "CWzCam", "Width", (LPCTSTR)m_sWidth );
	pReg->Set( "CWzCam", "Height", (LPCTSTR)m_sHeight );
	pReg->Set( "CWzCam", "CustomSize", (DWORD)m_bCustomSize );

	pReg->Set( "CWzCam", "BckType", (DWORD)m_nBckType );

	// Get driver selection
	int sel = m_combo.GetCurSel();
	if ( sel != CB_ERR )
	{	DWORD i = m_combo.GetItemData( sel );
		pReg->Set( "CWzCam", "Driver", i );
	} // end if

	// Save image file name
	pReg->Set( "CWzCam", "ImageFile", m_sImage );

	return 0;
}

void CWzCam::FillFileCombo()
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

void CWzCam::OnSelchangeImagefile() 
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
}

void CWzCam::OnEditchangeImagefile() 
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
}

void CWzCam::DoEnable()
{
	UpdateData( TRUE );

	CWnd *pWnd = GetDlgItem( IDC_WIDTH );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bCustomSize );
	pWnd = GetDlgItem( IDC_HEIGHT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bCustomSize );

	pWnd = GetDlgItem( IDC_COMBO );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nBckType == 1 );

	pWnd = GetDlgItem( IDC_IMAGEFILE );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nBckType == 2 );
	pWnd = GetDlgItem( IDC_CHOOSE_IMAGE );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nBckType == 2 );
}

void CWzCam::OnBcktype() 
{	DoEnable();
}

void CWzCam::OnBcktype2() 
{	DoEnable();
}

void CWzCam::OnBcktype3() 
{	DoEnable();
}

void CWzCam::OnCustomsize() 
{	DoEnable();
}

void CWzCam::OnChooseImage() 
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

	FillFileCombo();
}

void CWzCam::OnDropdownImagefile() 
{	FillFileCombo();
}
