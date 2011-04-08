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
// DlgProductUpdate.cpp : implementation file
//

#include "stdafx.h"
#include "Vp.h"
#include "DlgProductUpdate.h"

#include "DlgMsgBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//#define UPDATELINK			"http://127.0.0.1/version.inf"

#define WM_NETFILE			( WM_USER + 101 )

/////////////////////////////////////////////////////////////////////////////
// CDlgProductUpdate dialog


CDlgProductUpdate::CDlgProductUpdate(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProductUpdate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProductUpdate)
	m_sStatus = _T("");
	//}}AFX_DATA_INIT

	*m_szLink = 0;
	m_dVer = 0;
	m_bSilent = FALSE;
}


void CDlgProductUpdate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProductUpdate)
	DDX_Text(pDX, IDC_STATUS, m_sStatus);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CDlgProductUpdate, CDialog)
	//{{AFX_MSG_MAP(CDlgProductUpdate)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_NETFILE, OnNetFile )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgProductUpdate message handlers

BOOL CDlgProductUpdate::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetStatus( "Wait..." );

	// Setup for callbacks
	m_nf.SetMessageTarget( GetSafeHwnd(), WM_NETFILE );
	m_nf.EnableCache( FALSE );

	// Start the download
	m_nf.Download( m_szLink );		
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgProductUpdate::OnCancel() 
{
	m_nf.Destroy();

	DestroyWindow();

//	CDialog::OnCancel();
}

void CDlgProductUpdate::OnOK() 
{
	// Evaluate the version
	Evaluate( GetSafeHwnd(), m_dVer );

	m_nf.Destroy();

	DestroyWindow();

//	CDialog::OnOK();
}

void CDlgProductUpdate::SetStatus(LPCTSTR pStatus)
{
	if ( pStatus == NULL ) return;

	m_sStatus = pStatus;

	if ( !::IsWindow( GetSafeHwnd() ) ) return;

	CWnd *pWnd = GetDlgItem( IDC_STATUS );
	if ( pWnd != NULL ) pWnd->SetWindowText( pStatus );
}

LRESULT CDlgProductUpdate::OnNetFile(WPARAM wParam, LPARAM lParam)
{
	switch( wParam )
	{
		case NETFILE_DS_INITIALIZING :
			SetStatus( "Initializing..." );
			break;

		case NETFILE_DS_CONNECTING :
			SetStatus( "Connecting to server..." );
			break;

		case NETFILE_DS_DOWNLOADING :
			SetStatus( "Getting update information..." );
			break;

		case NETFILE_DS_DONE :
			SetStatus( "Reading product information..." );
			OnOK();
			break;

		case NETFILE_DS_ERROR :

			char err[ 512 ];
			CAsyncWinSock::GetErrorMsg( err, m_nf.GetTransferError() );

			SetStatus( err );

			// Show user if needed
			if ( !m_bSilent ) 
				::MessageBox( GetSafeHwnd(), err, "Unable to access server", MB_OK );

			OnCancel();
			break;

	} // end switch
	return 0;
}

void CDlgProductUpdate::Evaluate( HWND hWnd, double ver )
{
	CCfgFile	cfg;

	// Load the data
	cfg.LoadFromMem( m_nf.GetMem(), m_nf.GetDataRead() );

	double avail = 0;

	cfg.GetValue( "Product Information", "Last Version", &avail );
	if ( avail == 0 ) 
	{
		SetStatus( "Product information unavailable." );
		return;
	} // end if

	if ( avail > ver )
	{
		SetStatus( "Update available" );

		// Does the user want to get the update?
		CDlgMsgBox dlg( this );
		if ( dlg.ShowMessage(	"Update Available!", 
								"An update is available!\r\n\r\n"
								"Would you like to go there now?",
								LoadIcon( NULL, IDI_QUESTION ), 
								MB_YESNO ) != IDOK ) return;


		char url[ CWF_STRSIZE ] = { 0 };
		if ( !cfg.GetValue( "Product Information", "URL", url, sizeof( url ) ) || *url == 0 )
			strcpy( url, "http://www.wheresjames.com/" );

		ShellExecute( NULL, "open", url, NULL, NULL, SW_SHOWNORMAL );

	} // end if

	else if ( !m_bSilent )
	{
		SetStatus( "Update not available" );

		::MessageBox( hWnd,	"You have the latest version",
							"Update Not Available.", MB_OK );

	} // end else

}

BOOL CDlgProductUpdate::DoUpdateCheck(CWnd *pParent, LPCTSTR pLink, double dVer, BOOL bSilent)
{
	// Lose any current window
	m_nf.Destroy();
	if ( ::IsWindow( GetSafeHwnd() ) ) DestroyWindow();

	// Copy data
	strcpy_sz( m_szLink, pLink );
	m_dVer = dVer;
	m_bSilent = bSilent;

	// Create the dialog box
	if ( !Create( CDlgProductUpdate::IDD, pParent ) )
		return FALSE;

	// Show or hide
	if ( !bSilent ) ShowWindow( SW_SHOWNORMAL );
	else ShowWindow( SW_HIDE );

	return TRUE;
}
