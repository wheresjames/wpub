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
// DlgPublishHtml.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgPublishHtml.h"

#include "DlgUpload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgPublishHtml dialog


CDlgPublishHtml::CDlgPublishHtml(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPublishHtml::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgPublishHtml)
	m_sDir = _T("");
	m_nPublish = 0;
	m_sStatus = _T("");
	m_sFilename = _T("");
	//}}AFX_DATA_INIT
}


void CDlgPublishHtml::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgPublishHtml)
	DDX_Control(pDX, IDC_FILENAME, m_comboFilename);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_CHOOSE_DIR, m_choose_dir);
	DDX_Control(pDX, IDC_FTPSERVER, m_comboFtpServer);
	DDX_Control(pDX, IDC_FOLDER, m_comboFolder);
	DDX_Text(pDX, IDC_DIR, m_sDir);
	DDX_Radio(pDX, IDC_PUBLISH1, m_nPublish);
	DDX_Text(pDX, IDC_STATUS, m_sStatus);
	DDX_CBString(pDX, IDC_FILENAME, m_sFilename);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgPublishHtml, CDialog)
	//{{AFX_MSG_MAP(CDlgPublishHtml)
	ON_BN_CLICKED(IDC_PUBLISH1, OnPublish1)
	ON_BN_CLICKED(IDC_PUBLISH2, OnPublish2)
	ON_BN_CLICKED(IDC_PUBLISH3, OnPublish3)
	ON_BN_CLICKED(IDC_CHOOSE_DIR, OnChooseDir)
	ON_EN_CHANGE(IDC_DIR, OnChangeDir)
	ON_EN_CHANGE(IDC_FILENAME, OnChangeFilename)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgPublishHtml message handlers

BOOL CDlgPublishHtml::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetIcon( AfxGetApp()->LoadIcon( IDI_FTP ), TRUE );
	SetIcon( AfxGetApp()->LoadIcon( IDI_FTP ), FALSE );

	// Set the icon
	m_choose_dir.SetIcon( CGrDC::LoadIcon( IDI_DIR_CLOSED ) );

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgPublishHtml::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	// Get the target path
	CWinFile::GetPathFromFileName( m_sMain, m_szPath );
	
	// Get filename
	char fname[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( m_sMain, fname );

	// Get filename
	m_sFilename = CFG().GetSz( "Settings", "HtmlPubFilename" );
	if ( m_sFilename.IsEmpty() ) m_sFilename = fname;

//	m_sFilename = fname;

	// Add a few choice filenames
	m_comboFilename.AddString( "Index.htm" );
	m_comboFilename.AddString( fname );
	if ( strcmpi( m_sFilename, fname ) && strcmpi( m_sFilename, "Index.htm" ) )
		m_comboFilename.AddString( m_sFilename );

	// Initialize folder
	m_sDir = CFG().GetSz( "Settings", "HtmlPubDir" );

	UpdateData( FALSE );

	FillCombo();

	DoEnable();

	return 1;
}

void CDlgPublishHtml::OnOK() 
{
	UpdateData( TRUE );

	// Ensure source
	if ( m_sMain.IsEmpty() ) return;
	if ( !CWinFile::DoesExist( m_szPath ) ) return;

	// Filename buffers
	char /*src[ CWF_STRSIZE ],*/ dst[ CWF_STRSIZE ];

	// Set main filename
	if ( !CWinFile::DoesExist( m_sMain ) ) return;
	CWinFile::BuildPath( dst, m_szPath, m_sFilename );
	if ( strcmpi( m_sMain, dst ) )
	{	MoveFile( m_sMain, dst );
		m_sMain = dst;
	} // en dif

	// Save filename
	CFG().SetValue( "Settings", "HtmlPubFilename", (LPCTSTR)m_sFilename );

	if ( m_nPublish == 0 )
	{
		int n = m_comboFolder.GetCurSel();
		if ( n == CB_ERR ) return;
		m_comboFolder.GetLBText( n, dst );
		
		// Create the destination folder
		if ( !CWinFile::CreateDirectory( dst ) ) return;

		m_sStatus = "Publishing files to ";
		m_sStatus += dst;
		m_progress.SetRange( 0, 100 );
		m_progress.SetPos( 100 );
		UpdateData( FALSE );

		// Copy the files
		CWinFile::CopyFolder( m_szPath, dst, TRUE );

		m_progress.SetPos( 0 );
		m_sStatus = "Publishing complete";
		UpdateData( FALSE );

	} // end if

	else if ( m_nPublish == 1 )
	{
		int n = m_comboFolder.GetCurSel();
		if ( n == CB_ERR ) return;
		m_comboFtpServer.GetLBText( n, dst );

		CDlgUpload dlg( this );
		dlg.Upload( m_szPath, dst, NULL, FALSE );
		dlg.DoModal();
	} // end else

	else if ( m_nPublish == 2 )
	{
		// Create the destination folder
		if ( !CWinFile::CreateDirectory( m_sDir ) ) return;

		m_sStatus = "Publishing files to ";
		m_sStatus += m_sDir;
		m_progress.SetRange( 0, 100 );
		m_progress.SetPos( 100 );
		UpdateData( FALSE );

		// Copy the files
		CWinFile::CopyFolder( m_szPath, m_sDir, TRUE );

		m_progress.SetPos( 0 );
		m_sStatus = "Publishing complete";
		UpdateData( FALSE );

	} // end else if

	::MessageBox(	GetSafeHwnd(), "Publishing complete!", "Publish HTML", 
					MB_OK | MB_ICONINFORMATION );
	
//	CDialog::OnOK();
}

void CDlgPublishHtml::DoEnable()
{
	UpdateData( TRUE );

	CWnd *pCtrl = GetDlgItem( IDC_FOLDER );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_nPublish == 0 );

	pCtrl = GetDlgItem( IDC_FTPSERVER );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_nPublish == 1 );

	pCtrl = GetDlgItem( IDC_DIR );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_nPublish == 2 );
	pCtrl = GetDlgItem( IDC_CHOOSE_DIR );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_nPublish == 2 );

}

void CDlgPublishHtml::OnPublish1() 
{	DoEnable();
}
void CDlgPublishHtml::OnPublish2() 
{	DoEnable();
}
void CDlgPublishHtml::OnPublish3() 
{	DoEnable();
}

void CDlgPublishHtml::OnChooseDir() 
{
	char dir[ CWF_STRSIZE ] = { 0 };
	strcpy_sz( dir, m_sDir );

	// Get folder name
	if ( !CWinFile::GetBrowseDir(	GetSafeHwnd(), "Choose Publish Folder", 
									dir, dir ) ) return;

	// Save publish folder
	m_sDir = dir;
	CFG().SetValue( "Settings", "HtmlPubDir", (LPCTSTR)m_sDir );

	UpdateData( FALSE );	
}

BOOL CDlgPublishHtml::FillCombo()
{
	// Fill in FTP servers
	m_comboFtpServer.ResetContent();
	HGROUP hGroup = NULL;
	while ( ( hGroup = FTPSERVERS().GetNext( hGroup ) ) != NULL )
		m_comboFtpServer.AddString( hGroup->name );

	// Fill in local server paths
	m_comboFolder.ResetContent();
	CRKey *pRk = WEBSETTINGS().GetKey( "FolderMap" );
	if ( pRk != NULL )
	{	LPREGVALUE	prv = NULL;
		while ( ( prv = (LPREGVALUE)pRk->GetNext( prv ) ) != NULL )
		{
			// Add this path
			int n = m_comboFolder.AddString( pRk->GetSz( prv ) );

			// Select the root
			if (	*prv->cpkey == 0 || !strcmp( prv->cpkey, "/" ) ||
					!strcmp( prv->cpkey, "\\" ) )
				m_comboFolder.SetCurSel( n );

		} // end while
	} // end if

	return TRUE;
}

void CDlgPublishHtml::OnChangeDir() 
{
	UpdateData( TRUE );

	// Save publish folder
	CFG().SetValue( "Settings", "HtmlPubDir", (LPCTSTR)m_sDir );	
}

void CDlgPublishHtml::OnChangeFilename() 
{

}
