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
// DlgUpload.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgUpload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_FTP		( WM_USER + 101 )

/////////////////////////////////////////////////////////////////////////////
// CDlgUpload dialog


CDlgUpload::CDlgUpload(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUpload::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgUpload)
	m_sStatus = _T("");
	m_sStatusBox = _T("");
	//}}AFX_DATA_INIT

	m_hFind = NULL;
	*m_szRoot = 0;
//	*m_szTemp = 0;

	m_ptr = 0;
	m_buf = NULL;
	m_size = NULL;
	m_fname = NULL;
}


void CDlgUpload::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgUpload)
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
	DDX_Text(pDX, IDC_STATUS, m_sStatus);
	DDX_Text(pDX, IDC_STATUSBOX, m_sStatusBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgUpload, CDialog)
	//{{AFX_MSG_MAP(CDlgUpload)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_FTP, OnFtp )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgUpload message handlers

BOOL CDlgUpload::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Create temporary filename
//	CWinFile::CreateTemp( m_szTemp );

	// Setup progress bar
	m_dwPos = 0;
	m_progress.SetRange( 0, (short)m_dwFileCount );
	m_progress.SetPos( 0 );

	// Setup for message callbacks
	m_ftp.SetMessageTarget( GetSafeHwnd(), WM_FTP );

	char username[ CWF_STRSIZE ] = { 0 };
	char password[ CWF_STRSIZE ] = { 0 };

	if ( FTPSERVERS().GetValue( m_sFtp, "Username", username, sizeof( username ) ) )
	{
		FTPSERVERS().GetValue( m_sFtp, "Password", password, sizeof( password ) );

		// Set username / password
		m_ftp.SetUsernamePassword( username, password );

	} // end if

	// Attempt to connect to the domain
	char domain[ CWF_STRSIZE ] = { 0 };
	if ( FTPSERVERS().GetValue( m_sFtp, "Address", domain, sizeof( domain ) ) )
	{
		m_ftp.Connect( domain );
		SetStatus( "Connecting ..." );
	} // end if

	// Lost cause
	else 
	{	AfxMessageBox( "No FTP information" );
		OnCancel();
	} // end else

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgUpload::Upload( LPCTSTR pPath, LPCTSTR pFtp, CWnd *pwnd, BOOL bModeless)
{
	m_ptr = 0;
	m_buf = NULL;
	m_size = NULL;
	m_fname = NULL;

	// Sanity checks
	if ( pPath == NULL || pFtp == NULL ) return FALSE;
	if ( FTPSERVERS().FindGroup( pFtp ) == NULL ) return FALSE;

	// Copy ftp server tag name
	m_sFtp = pFtp;

	// Is window
	if ( ::IsWindow( GetSafeHwnd() ) )
	{	ShowWindow( SW_SHOWNORMAL );
		return FALSE;
	} // end if

	// How many files?
	m_dwFileCount = CWinFile::GetFileCount( pPath );
	if ( m_dwFileCount == 0 ) return FALSE;

	char search[ MAX_PATH ];
	
	// Verify file
	DWORD fa = GetFileAttributes( pPath );
	if ( fa == MAXDWORD ) return FALSE;

	if ( ( fa & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
	{	CWinFile::BuildPath( search, pPath, "*.*" );
		strcpy_sz( m_szRoot, pPath );
	} // end if
	else
	{	strcpy_sz( search, pPath );
		CWinFile::GetPathFromFileName( pPath, m_szRoot );
	} // end else

	// Find first file
	m_hFind = FindFirstFile( search, &m_fd );
	if ( m_hFind == INVALID_HANDLE_VALUE ) return FALSE;

	if ( bModeless )
	{
		// Create the dialog box
		if ( GetSafeHwnd() != NULL ) DestroyWindow();
		Create( IDD, pwnd );
		ShowWindow( SW_SHOWNORMAL );
		CenterWindow();
	} // end if

	return TRUE;
}

BOOL CDlgUpload::UploadFile()
{
	char path[ FTP_STRSIZE ];
	char upload[ FTP_STRSIZE ] = { 0 };
	char root[ FTP_STRSIZE ];

	// Get the upload directory
	FTPSERVERS().GetValue( m_sFtp, "Folder", root, sizeof( root ) );

	do
	{
		// Any more files?
		if ( m_hFind == NULL )
		{	SendMessage( WM_CLOSE, 0, 0L );
			return FALSE;
		} // end if

		// Get a real file
		while (	!strcmp( m_fd.cFileName, "." ) ||
				!strcmp( m_fd.cFileName, ".." ) ||
				( m_fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
			if ( !FindNextFile( m_hFind, &m_fd ) )
			{	SendMessage( WM_CLOSE, 0, 0L );
				return FALSE;
			} // end if

		// Build a full path to the file
		CWinFile::BuildPath( path, m_szRoot, m_fd.cFileName );

		// Build the upload path
		CWinFile::WebBuildPath( upload, root, m_fd.cFileName );

/*		LPCTSTR ext = CWinFile::GetExtension( m_fd.cFileName );
		if ( !strcmpi( ext, "htm" ) || !strcmpi( ext, "html" ) )
		{
//			SubstituteStrings( path, m_szTemp );
//			strcpy_sz( path, m_szTemp );
		} // end if
*/

		{ // Update display
			m_sStatus = m_fd.cFileName;
			m_progress.SetPos( ++m_dwPos );	
			UpdateData( FALSE );

			CWnd *pWnd = GetDlgItem( IDC_STATUS );
			if ( pWnd != NULL ) pWnd->RedrawWindow();
			pWnd = GetDlgItem( IDC_PROGRESS );
			if ( pWnd != NULL ) pWnd->RedrawWindow();

		} // end update display


		// Get the next file
		if ( !FindNextFile( m_hFind, &m_fd ) ) 
		{	FindClose( m_hFind ); m_hFind = NULL; }

	// Upload it
	} while ( !m_ftp.Upload( path, upload ) );

	return TRUE;
}

BOOL CDlgUpload::SubstituteStrings(LPCTSTR pFile, LPCTSTR pSub)
{
	CWinFile f1, f2;

	// Open files
	if ( !f1.OpenExisting( pFile ) || !f2.OpenNew( pSub ) )
		return FALSE;

	DWORD size = f1.Size();
	if ( size == 0 ) return FALSE;

	LPBYTE buf = new BYTE[ size + 1 ];
	if ( buf == NULL ) return FALSE;

	if ( !f1.Read( buf, size ) )
	{	delete [] buf; return FALSE; }

	// Sub strings

	f2.Write( buf, size );

	delete [] buf;

	return TRUE;
}

/*
void CDlgUpload::OnNetFile(WPARAM wParam, LPARAM lParam)
{
	
	if ( ( wParam & NETFILE_DS_INITIALIZING ) != 0 )
		SetStatus( "Initializing ..." );
		
	else if ( ( wParam & NETFILE_DS_CONNECTING ) != 0 )
		SetStatus( "Connecting ..." );

	else if ( ( wParam & NETFILE_DS_DOWNLOADING ) != 0 )
	{
		CNetFile *pnf = (CNetFile*)lParam;
		if ( pnf != NULL )
		{
			char msg[ 256 ];
			char num[ 256 ];
			CWinFile::CreateSizeString( num, pnf->GetDataRead() );
			strcpy( msg, "Downloading - " );
			strcat( msg, num );
			SetStatus( msg );

		} // end if
		else SetStatus( "Downloading ..." );

	} // end if

	else if ( ( wParam & NETFILE_DS_UPLOADING ) != 0 )
	{
		CNetFile *pnf = (CNetFile*)lParam;
		if ( pnf != NULL )
		{
			char msg[ 256 ];
			char num[ 256 ];
			CWinFile::CreateSizeString( num, pnf->GetDataWritten() );
			strcpy( msg, "Uploading - " );
			strcat( msg, num );
			SetStatus( msg );

		} // end if
		else SetStatus( "Uploading ..." );

	} // end if

	else if ( ( wParam & NETFILE_DS_DONE ) != 0 )
	{
		if ( ( wParam & NETFILE_DS_ERROR ) != 0 ) SetStatus( "Error" );
		else SetStatus( "Success" );

		// Call next file
		if ( !UploadFile() )
		{	CWinFile::Delete( m_szTemp );
			DestroyWindow();
		} // end if

	} // end if
}
*/
void CDlgUpload::OnCancel() 
{
//	m_netfile.Destroy();
	m_ftp.Destroy();
	if ( m_hFind != NULL )
	{	FindClose( m_hFind ); m_hFind = NULL; }
	
	CDialog::OnCancel();
}

void CDlgUpload::SetStatus(LPCTSTR pStatus)
{
	m_sStatusBox = pStatus;

	CWnd *pSB = GetDlgItem( IDC_STATUSBOX );
	if ( pSB != NULL ) 
	{
		pSB->SetWindowText( pStatus );
		pSB->RedrawWindow();
	} // end if
}

LRESULT CDlgUpload::OnFtp(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == FTPMSG_CMDCOMPLETE ||
		( wParam == FTPMSG_STATUS && lParam == FTP_LOGGEDON ) )
	{
		m_sStatusBox = "Uploading...";
		UpdateData( FALSE );

		if ( m_buf == NULL ) UploadFile();
		else if ( m_ptr >= m_dwFileCount )
			SendMessage( WM_CLOSE, 0, 0L );
		else
		{
			{ // Update display
				m_sStatus = m_fname[ m_ptr ];
				m_progress.SetPos( ++m_dwPos );	
				UpdateData( FALSE );

				CWnd *pWnd = GetDlgItem( IDC_STATUS );
				if ( pWnd != NULL ) pWnd->RedrawWindow();
				pWnd = GetDlgItem( IDC_PROGRESS );
				if ( pWnd != NULL ) pWnd->RedrawWindow();

			} // end update display

			// Upload FTP
			m_ftp.Upload( (LPBYTE)m_buf[ m_ptr ], m_size[ m_ptr ], m_fname[ m_ptr ] );

			// Next file
			m_ptr++;

		} // end else
	} // end if

	return 0;
}

BOOL CDlgUpload::Upload(char* *buf, LPDWORD size, char* *fname, CWnd * pwnd, BOOL bModeless)
{
	if ( buf == NULL || size == NULL || fname == NULL ) 
		return FALSE;

	// Is window
	if ( ::IsWindow( GetSafeHwnd() ) )
	{	ShowWindow( SW_SHOWNORMAL );
		return FALSE;
	} // end if

	m_dwFileCount = 0;
	while ( buf[ m_dwFileCount ] != NULL ) m_dwFileCount++;
	if ( m_dwFileCount == 0 ) return FALSE;

	// Save data pointers
	m_buf = buf;
	m_size = size;
	m_fname = fname;

	// Start with first file
	m_ptr = 0;

	if ( bModeless )
	{
		// Create the dialog box
		if ( GetSafeHwnd() != NULL ) DestroyWindow();
		Create( IDD, pwnd );
		ShowWindow( SW_SHOWNORMAL );
		CenterWindow();
	} // end if

	return TRUE;
}
