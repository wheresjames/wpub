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
// DlgGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgGeneral.h"

#include "DlgTreeFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SHORTCUT_FILENAME	"WheresJames Webcam Publisher Pro.lnk"
#define SHORTCUT_NAME		"WheresJames Webcam Publisher Pro"

#define STARTUP_HKEY		HKEY_CURRENT_USER
#define STARTUP_SZKEY		"Software\\Microsoft\\Windows\\CurrentVersion\\Run"

/////////////////////////////////////////////////////////////////////////////
// CDlgGeneral dialog


CDlgGeneral::CDlgGeneral(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGeneral::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgGeneral)
	m_bStartup = FALSE;
	m_bTrayIcon = FALSE;
	m_nAction = 0;
	m_bToolbar = FALSE;
	m_bAutoUpdate = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgGeneral::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgGeneral)
	DDX_Control(pDX, IDC_CLOSEICON, m_imgClose);
	DDX_Check(pDX, IDC_STARTUP, m_bStartup);
	DDX_Check(pDX, IDC_TRAYICON, m_bTrayIcon);
	DDX_Radio(pDX, IDC_ACTION, m_nAction);
	DDX_Check(pDX, IDC_TOOLBAR, m_bToolbar);
	DDX_Check(pDX, IDC_AUTOUPDATE, m_bAutoUpdate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgGeneral, CDialog)
	//{{AFX_MSG_MAP(CDlgGeneral)
	ON_BN_CLICKED(IDC_ACTION, OnAction)
	ON_BN_CLICKED(IDC_ACTION2, OnAction2)
	ON_BN_CLICKED(IDC_ACTION3, OnAction3)
	ON_BN_CLICKED(IDC_STARTUP, OnStartup)
	ON_BN_CLICKED(IDC_TRAYICON, OnTrayicon)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_SAVE, OnSave )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgGeneral message handlers

void CDlgGeneral::OnCancel() 
{
}

void CDlgGeneral::OnOK() 
{
}

BOOL CDlgGeneral::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_imgClose.SetInfo( "WINDOW", WP_CLOSEBUTTON, CBS_NORMAL, 
						CGrDC::LoadIcon( IDI_CLOSE ) );

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgGeneral::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	{ // Check startup
		BOOL ok = FALSE;
		HKEY hKey;
		if ( RegOpenKeyEx(	STARTUP_HKEY, STARTUP_SZKEY,
							0, KEY_ALL_ACCESS | KEY_READ, &hKey ) == ERROR_SUCCESS )
		{
			DWORD len = CWF_STRSIZE;
			char path[ CWF_STRSIZE ];

			// Attempt to query entry
			if ( RegQueryValueEx(	hKey, SHORTCUT_NAME, 
									0, NULL, (LPBYTE)path, &len ) == ERROR_SUCCESS )
			{
				char exe[ CWF_STRSIZE ];

				// Remove quotes from path
				CWinFile::Unquote( path );

				// Verify this points to us
				if (	GetModuleFileName( NULL, exe, CWF_STRSIZE ) &&
						!strcmpi( path, exe ) )
				{
					ok = TRUE;
				} // end if
			} // end if

			RegCloseKey( hKey );
		} // end if

		m_bStartup = ok;
	} // end check startup


	DWORD onclose = 0;
	CFG().GetValue( "Settings", "OnCloseAction", &onclose );
	m_nAction = onclose;
	if ( m_nAction > 2 ) m_nAction = 0;

	// Restore settings
	m_bTrayIcon = CFG().GetDword( "Settings", "NoMinimizeToTray", 0 ) == 0;
	m_bToolbar = CFG().GetDword( "Settings", "RestoreToolbars", 0 ) != 0;
	m_bAutoUpdate = CFG().GetDword( "Settings", "AutoUpdateCheck", 1 ) != 0;
	
	UpdateData( FALSE );
	return 0;
}

LRESULT CDlgGeneral::OnSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );

	if ( m_nAction > 2 || m_nAction < 0 ) m_nAction = 0;
	CFG().SetValue( "Settings", "OnCloseAction", (DWORD)m_nAction );

	if ( CFG().GetDword( "Settings", "AutoUpdateCheck", 1 ) != 0 )

	// Minimize to tray
	CFG().SetValue( "Settings", "NoMinimizeToTray", (DWORD)( m_bTrayIcon ? 0 : 1 ) );	

	// Restore toolbars
	CFG().SetValue( "Settings", "RestoreToolbars", (DWORD)( m_bToolbar ? 1 : 0 ) );	

	// Restore toolbars
	CFG().SetValue( "Settings", "AutoUpdateCheck", (DWORD)( m_bAutoUpdate ? 1 : 0 ) );	

	if ( m_bStartup )
	{
		char	path[ CWF_STRSIZE ];
		HKEY	hKey;
		if (	GetModuleFileName( NULL, path, sizeof( path ) ) &&
				RegCreateKey( STARTUP_HKEY, STARTUP_SZKEY, &hKey ) == ERROR_SUCCESS )
		{
			// Put the path in quotes
			CWinFile::Quote( path );

			// Add to registry
			RegSetValueEx( hKey, SHORTCUT_NAME,
							0, REG_SZ, (const BYTE *)path, strlen( path ) );

			// Close the key
			RegCloseKey( hKey );
		} // end if
	} // end if
	// Oh well, remove us from the windows startup files
	else
	{
		HKEY hKey;
		if ( RegOpenKeyEx(	STARTUP_HKEY, STARTUP_SZKEY,
							0, KEY_ALL_ACCESS, &hKey ) == ERROR_SUCCESS )
		{
			// Attempt to remove our entry
			RegDeleteValue( hKey, SHORTCUT_NAME );
			RegCloseKey( hKey );
		} // end if
	} // end else

	UpdateData( FALSE );
	return 0;
}

void CDlgGeneral::OnAction() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}

void CDlgGeneral::OnAction2() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}

void CDlgGeneral::OnAction3() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}

void CDlgGeneral::OnChangeFramerate() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}

void CDlgGeneral::OnStartup() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}

void CDlgGeneral::OnTrayicon() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}
