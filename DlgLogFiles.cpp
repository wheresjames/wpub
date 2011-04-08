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
// DlgLogFiles.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgLogFiles.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLogFiles dialog


CDlgLogFiles::CDlgLogFiles(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogFiles::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLogFiles)
	m_bFtp = FALSE;
	m_sFtpLog = _T("");
	m_bSmtp = FALSE;
	m_sSmtpLog = _T("");
	//}}AFX_DATA_INIT
}


void CDlgLogFiles::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLogFiles)
	DDX_Check(pDX, IDC_FTP, m_bFtp);
	DDX_Text(pDX, IDC_FTPLOG, m_sFtpLog);
	DDX_Check(pDX, IDC_SMTP, m_bSmtp);
	DDX_Text(pDX, IDC_SMTPLOG, m_sSmtpLog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLogFiles, CDialog)
	//{{AFX_MSG_MAP(CDlgLogFiles)
	ON_BN_CLICKED(IDC_FTP, OnFtp)
	ON_BN_CLICKED(IDC_SMTP, OnSmtp)
	ON_BN_CLICKED(IDC_OPENFTP, OnOpenftp)
	ON_BN_CLICKED(IDC_OPENSMTP, OnOpensmtp)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_SAVE, OnSave )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLogFiles message handlers

BOOL CDlgLogFiles::OnInitDialog() 
{
	CDialog::OnInitDialog();

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgLogFiles::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	char str[ CWF_STRSIZE ];

	DWORD dw = 0;
	CFG().GetValue( "Settings", "EnableFtpLog", &dw );
	m_bFtp = ( dw != 0 ) ? TRUE : FALSE;

	strcpy( str, "FtpLog.txt" );
	CFG().GetValue( "Settings", "FtpLogFile", str, sizeof( str ) );
	m_sFtpLog = str;

	dw = 0;
	CFG().GetValue( "Settings", "EnableSmtpLog", &dw );
	m_bSmtp = ( dw != 0 ) ? TRUE : FALSE;

	strcpy( str, "SmtpLog.txt" );
	CFG().GetValue( "Settings", "SmtpLogFile", str, sizeof( str ) );
	m_sSmtpLog = str;

	UpdateData( FALSE );

	DoEnable();

	return 0;
}

LRESULT CDlgLogFiles::OnSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );

	CFG().SetValue( "Settings", "EnableFtpLog", (DWORD)( m_bFtp ? 1 : 0 ) );
	CFG().SetValue( "Settings", "FtpLogFile", m_sFtpLog );

	CFG().SetValue( "Settings", "EnableSmtpLog", (DWORD)( m_bSmtp ? 1 : 0 ) );
	CFG().SetValue( "Settings", "SmtpLogFile", m_sSmtpLog );

	return 0;
}

void CDlgLogFiles::OnFtp() 
{
	DoEnable();
}

void CDlgLogFiles::OnSmtp() 
{
	DoEnable();
}

void CDlgLogFiles::DoEnable()
{
	UpdateData( TRUE );

	CWnd *pWnd = GetDlgItem( IDC_FTPLOG );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bFtp );

	pWnd = GetDlgItem( IDC_SMTPLOG );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bSmtp );

	char path[ CWF_STRSIZE ];

	CWinFile::GetExePath( path, sizeof( path ) );
	CWinFile::BuildPath( path, path, m_sFtpLog );
	pWnd = GetDlgItem( IDC_OPENFTP );
	if ( pWnd != NULL ) pWnd->EnableWindow( CWinFile::DoesExist( path ) );

	
	CWinFile::GetExePath( path, sizeof( path ) );
	CWinFile::BuildPath( path, path, m_sSmtpLog );
	pWnd = GetDlgItem( IDC_OPENSMTP );
	if ( pWnd != NULL ) pWnd->EnableWindow( CWinFile::DoesExist( path ) );


}

void CDlgLogFiles::OnOpenftp() 
{
	UpdateData( TRUE );

	char path[ CWF_STRSIZE ];
	CWinFile::GetExePath( path, sizeof( path ) );
	CWinFile::BuildPath( path, path, m_sFtpLog );

	ShellExecute( NULL, "open", path, NULL, NULL, SW_SHOWNORMAL );
}

void CDlgLogFiles::OnOpensmtp() 
{
	UpdateData( TRUE );

	char path[ CWF_STRSIZE ];

	CWinFile::GetExePath( path, sizeof( path ) );
	CWinFile::BuildPath( path, path, m_sSmtpLog );

	ShellExecute( NULL, "open", path, NULL, NULL, SW_SHOWNORMAL );
}
