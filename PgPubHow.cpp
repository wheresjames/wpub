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
// PgPubHow.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgPubHow.h"

#include "PubThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgPubHow dialog

static char *g_timestamp[] =
{
	"12.00", "%h.%m",
	"12.00.00", "%h.%m.%s",
	"00.00.00", "%H.%m.%s",
	"12.00 AM", "%h.%m %A",
	"12-25-03", "%c-%d-%y",
	"12-25-2003", "%c-%d-%Y",
	"Dec 25, 2003", "%b %d, %Y",
	"Thu, Dec 25, 2003", "%w, %b %d, %Y",
	"December 25, 2003", "%B %d, %Y",
	"Thursday, December 25, 2003", "%W, %B %d, %Y",
	"12.00 AM 12-25-03", "%h.%m %A %c-%d-%y",
	"12.00 AM Dec 25, 2003", "%h.%m %A %b %d, %Y",
	0, 0
};


CPgPubHow::CPgPubHow(CWnd* pParent /*=NULL*/)
	: CDialog(CPgPubHow::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgPubHow)
	m_bRotate = FALSE;
	m_sFile = _T("");
	m_sCurrentIndex = _T("");
	m_sStartIndex = _T("");
	m_sStopIndex = _T("");
	m_bTimestamp = FALSE;
	m_sTemplate = _T("");
	m_sFinal = _T("");
	//}}AFX_DATA_INIT
}


void CPgPubHow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgPubHow)
	DDX_Control(pDX, IDC_FORMAT, m_comboFormat);
	DDX_Check(pDX, IDC_ROTATE, m_bRotate);
	DDX_Text(pDX, IDC_FILE, m_sFile);
	DDX_Text(pDX, IDC_CURRENTINDEX, m_sCurrentIndex);
	DDX_Text(pDX, IDC_STARTINDEX, m_sStartIndex);
	DDX_Text(pDX, IDC_STOPINDEX, m_sStopIndex);
	DDX_Check(pDX, IDC_TIMESTAMP, m_bTimestamp);
	DDX_Text(pDX, IDC_TEMPLATE, m_sTemplate);
	DDX_Text(pDX, IDC_FINAL, m_sFinal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgPubHow, CDialog)
	//{{AFX_MSG_MAP(CPgPubHow)
	ON_BN_CLICKED(IDC_ROTATE, OnRotate)
	ON_BN_CLICKED(IDC_CHOOSE_FILE, OnChooseFile)
	ON_EN_CHANGE(IDC_FILE, OnChangeFile)
	ON_EN_CHANGE(IDC_STARTINDEX, OnChangeStartindex)
	ON_EN_CHANGE(IDC_STOPINDEX, OnChangeStopindex)
	ON_EN_CHANGE(IDC_CURRENTINDEX, OnChangeCurrentindex)
	ON_BN_CLICKED(IDC_TIMESTAMP, OnTimestamp)
	ON_EN_CHANGE(IDC_TEMPLATE, OnChangeTemplate)
	ON_CBN_SELCHANGE(IDC_FORMAT, OnSelchangeFormat)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_SAVE, OnSave )
	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgPubHow message handlers

BOOL CPgPubHow::OnInitDialog() 
{
	CDialog::OnInitDialog();

	FillCombo();

	OnRefresh( 0, 0L );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgPubHow::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	m_sFile = "webcam.jpg";

	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return -1;

	// Get the file name
	m_sFile = pReg->GetSz( "PUBINFO", "Fname", "webcam.jpg" );
	pReg->Set( "PUBINFO", "Fname", m_sFile );

	m_bRotate = ( ( pReg->GetDword( "PUBINFO", "F1" ) & PUBF1_ROTATEFNAME ) != 0 );

	m_bTimestamp = ( ( pReg->GetDword( "PUBINFO", "F1" ) & PUBF1_TIMESTAMP ) != 0 );

	VerifyIndex();

	m_sStartIndex = pReg->GetSz( "PUBINFO", "StartIndex", "0" );
	m_sStopIndex = pReg->GetSz( "PUBINFO", "StopIndex", "10" );
	m_sCurrentIndex = pReg->GetSz( "PUBINFO", "CurrentIndex", "0" );

	m_sTemplate = pReg->GetSz( "PUBINFO", "Template" );
	SelectTemplate( m_sTemplate );

	PreviewFilename();

	UpdateData( FALSE );

	DoEnable();

	return 0;
}

LRESULT CPgPubHow::OnSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );

	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return -1;
	
	DWORD f1 = pReg->GetDword( "PUBINFO", "F1" );
	if ( m_bRotate ) f1 |= PUBF1_ROTATEFNAME;
	else f1 &= ~PUBF1_ROTATEFNAME;

	if ( m_bTimestamp ) f1 |= PUBF1_TIMESTAMP;
	else f1 &= ~PUBF1_TIMESTAMP;
	pReg->Set( "PUBINFO", "F1", f1 );

	VerifyIndex();
	pReg->Set( "PUBINFO", "StartIndex", strtoul( m_sStartIndex, NULL, 10 ) );
	pReg->Set( "PUBINFO", "StopIndex", strtoul( m_sStopIndex, NULL, 10 ) );
	pReg->Set( "PUBINFO", "CurrentIndex", strtoul( m_sCurrentIndex, NULL, 10 ) );

	pReg->Set( "PUBINFO", "Template", m_sTemplate );
	SelectTemplate( m_sTemplate );

	PreviewFilename();

	UpdateData( FALSE );

	DoEnable();

	return 0;
}

void CPgPubHow::DoEnable()
{
	UpdateData( TRUE );

	CWnd *pWnd = GetDlgItem( IDC_STARTINDEX );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bRotate );
	pWnd = GetDlgItem( IDC_STOPINDEX );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bRotate );
	pWnd = GetDlgItem( IDC_CURRENTINDEX );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bRotate );

	pWnd = GetDlgItem( IDC_TXT_FORMAT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bTimestamp );
	pWnd = GetDlgItem( IDC_FORMAT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bTimestamp );
	pWnd = GetDlgItem( IDC_TEMPLATE );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bTimestamp );


}

void CPgPubHow::OnRotate() 
{	OnSave(	0, 0L );
}

void CPgPubHow::OnChooseFile() 
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return;
	
	// Get the file name
	char filename[ CWF_STRSIZE ] = { 0 };
	CWinFile::BuildPath( filename, pReg->GetSz( "PUBINFO", "Path" ), pReg->GetSz( "PUBINFO", "Fname" ) );

	if ( !CWinFile::GetSaveFileName(	filename, "Save Image As",
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

	CWinFile::GetFileNameFromPath( filename, filename );
	pReg->Set( "PUBINFO", "Fname", filename );
	m_sFile = filename;

	UpdateData( FALSE );
}

void CPgPubHow::OnChangeFile() 
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return;

	UpdateData( TRUE );
	pReg->Set( "PUBINFO", "Fname", m_sFile ); 
	PreviewFilename();
	UpdateData( FALSE );
}

void CPgPubHow::VerifyIndex()
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return;

	DWORD startindex = pReg->GetDword( "PUBINFO", "StartIndex" );
	DWORD stopindex = pReg->GetDword( "PUBINFO", "StopIndex" );
	DWORD curindex = pReg->GetDword( "PUBINFO", "CurrentIndex" );

	if ( startindex == 0 && stopindex == 0 ) startindex = 1, stopindex = 10;	
	if ( stopindex < startindex ) stopindex = startindex;
	if ( curindex > stopindex ) curindex = stopindex;	 	
	if ( curindex < startindex ) curindex = startindex;

	pReg->Set( "PUBINFO", "StartIndex", startindex );
	pReg->Set( "PUBINFO", "StopIndex", stopindex );
	pReg->Set( "PUBINFO", "CurrentIndex", curindex );
}

void CPgPubHow::OnChangeStartindex() 
{	OnSave(	0, 0L );
}

void CPgPubHow::OnChangeStopindex() 
{	OnSave(	0, 0L );
}

void CPgPubHow::OnChangeCurrentindex() 
{	OnSave(	0, 0L );
}

void CPgPubHow::FillCombo()
{
	m_comboFormat.ResetContent();

	for ( DWORD i = 0; g_timestamp[ i ] != 0; i += 2 )
	{	int n = m_comboFormat.AddString( g_timestamp[ i ] );
		if ( n != CB_ERR ) m_comboFormat.SetItemData( n, i );
	} // end for

}

void CPgPubHow::OnTimestamp() 
{
	OnSave( 0, 0L );
}

void CPgPubHow::SelectTemplate(LPCTSTR tmpl)
{
	// Search for this template
	for ( DWORD i = 0; g_timestamp[ i ] != 0; i += 2 )
		if ( !strcmp( g_timestamp[ i + 1 ], tmpl ) )
		{	m_comboFormat.SelectString( -1, g_timestamp[ i ] );
			return;
		} // end if

	// No selection
	m_comboFormat.SetCurSel( -1 );
}

void CPgPubHow::OnChangeTemplate() 
{
	OnSave( 0, 0L );
}

void CPgPubHow::OnSelchangeFormat() 
{
	UpdateData( TRUE );

	int n = m_comboFormat.GetCurSel();
	if ( n == CB_ERR ) return;

	DWORD i = m_comboFormat.GetItemData( n );
	m_sTemplate = g_timestamp[ i + 1 ];

	UpdateData( FALSE );

	OnSave( 0, 0L );
}

void CPgPubHow::PreviewFilename()
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return;

	PUBINFO pi;
	ZeroMemory( &pi, sizeof( pi ) );

	// Init data
	pi.f1 = pReg->GetDword( "PUBINFO", "F1" );
	pReg->Get( "PUBINFO", "Fname", pi.fname, sizeof( pi.fname ) );
	pReg->Get( "PUBINFO", "Template", pi.tmpl, sizeof( pi.tmpl ) );
	pi.startindex = pReg->GetDword( "PUBINFO", "StartIndex" );
	pi.stopindex = pReg->GetDword( "PUBINFO", "StopIndex" );
	pi.curindex = pReg->GetDword( "PUBINFO", "CurrentIndex" );

	// Preview the filename
	CPubThread::GetFileName( &pi, TRUE ); 

	// Load the result
	m_sFinal = pi.pub_fname;
}
