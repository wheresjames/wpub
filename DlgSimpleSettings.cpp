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
// DlgSimpleSettings.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgSimpleSettings.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSimpleSettings dialog


CDlgSimpleSettings::CDlgSimpleSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSimpleSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSimpleSettings)
	m_bEmail = FALSE;
	m_bFtp = FALSE;
	m_sFtpFilename = _T("");
	m_bPrint = FALSE;
	m_bSetup = FALSE;
	m_bSave = FALSE;
	m_xMax = _T("800");
	m_yMax = _T("600");
	//}}AFX_DATA_INIT
}


void CDlgSimpleSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSimpleSettings)
	DDX_Control(pDX, IDC_IMAGE, m_comboImage);
	DDX_Control(pDX, IDC_FTPSERVER, m_comboFtpServer);
	DDX_Check(pDX, IDC_EMAIL, m_bEmail);
	DDX_Check(pDX, IDC_FTP, m_bFtp);
	DDX_Text(pDX, IDC_FTPFILENAME, m_sFtpFilename);
	DDX_Check(pDX, IDC_PRINT, m_bPrint);
	DDX_Check(pDX, IDC_SETUP, m_bSetup);
	DDX_Check(pDX, IDC_SAVE, m_bSave);
	DDX_Text(pDX, IDC_XMAX, m_xMax);
	DDX_Text(pDX, IDC_YMAX, m_yMax);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSimpleSettings, CDialog)
	//{{AFX_MSG_MAP(CDlgSimpleSettings)
	ON_BN_CLICKED(IDC_FTP, OnFtp)
	ON_BN_CLICKED(IDC_FTPEDIT, OnFtpedit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSimpleSettings message handlers

BOOL CDlgSimpleSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Update flags
	m_bEmail = CFG().GetDword( "CMSettings", "EmailBtn", (DWORD)1 ) != 0;
	m_bPrint = CFG().GetDword( "CMSettings", "PrintBtn", (DWORD)1 ) != 0;
	m_bFtp = CFG().GetDword( "CMSettings", "FtpBtn", (DWORD)1 ) != 0;
	m_bSave = CFG().GetDword( "CMSettings", "SaveBtn", (DWORD)1 ) != 0;
	m_bSetup = CFG().GetDword( "CMSettings", "SetupBtn", (DWORD)1 ) != 0;
	m_sFtpFilename = CFG().GetSz( "CMSettings", "FtpFilename", "webcam.jpg" );
	m_xMax = CFG().GetSz( "CMSettings", "XMax", "800" );
	m_yMax = CFG().GetSz( "CMSettings", "YMax", "600" );

	UpdateData( FALSE );

	FillImageCombo();
	FillFtpCombo();

	DoEnable();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSimpleSettings::DoEnable()
{
	CWnd *pCtrl = GetDlgItem( IDC_TXT_FTPSERVER );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bFtp );

	pCtrl = GetDlgItem( IDC_FTPSERVER );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bFtp );

	pCtrl = GetDlgItem( IDC_FTPEDIT );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bFtp );

	pCtrl = GetDlgItem( IDC_TXT_FTPFILENAME );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bFtp );

	pCtrl = GetDlgItem( IDC_FTPFILENAME );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bFtp );
}

void CDlgSimpleSettings::OnFtp() 
{
	UpdateData( TRUE ); DoEnable();
}


void CDlgSimpleSettings::FillImageCombo()
{
	m_comboImage.ResetContent();

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

			int index = m_comboImage.AddString( show );
			if ( index != LB_ERR ) m_comboImage.SetItemData( index, ci.index );
		} // end if

	} // end for

	// Select first driver
	m_comboImage.SetCurSel( CFG().GetDword( "CMSettings", "Driver", (DWORD)0 ) );
}

void CDlgSimpleSettings::FillFtpCombo()
{
	m_comboFtpServer.ResetContent();

	HGROUP				hGroup = NULL;
	while ( ( hGroup = FTPSERVERS().GetNext( hGroup ) ) != NULL )
		m_comboFtpServer.AddString( hGroup->name );

	// Select FTP server
	m_comboFtpServer.SelectString( -1, CFG().GetSz( "CMSettings", "FtpServer", "" ) );
}

void CDlgSimpleSettings::OnOK() 
{
	UpdateData( TRUE );

	CFG().SetValue( "CMSettings", "EmailBtn", (DWORD)( m_bEmail ? 1 : 0 ) );
	CFG().SetValue( "CMSettings", "PrintBtn", (DWORD)( m_bPrint ? 1 : 0 ) );
	CFG().SetValue( "CMSettings", "FtpBtn", (DWORD)( m_bFtp ? 1 : 0 ) );
	CFG().SetValue( "CMSettings", "SaveBtn", (DWORD)( m_bFtp ? 1 : 0 ) );
	CFG().SetValue( "CMSettings", "SetupBtn", (DWORD)( m_bSetup ? 1 : 0 ) );

	CFG().SetValue( "CMSettings", "XMax", (DWORD)strtoul( m_xMax, NULL, 10 ) );
	CFG().SetValue( "CMSettings", "YMax", (DWORD)strtoul( m_yMax, NULL, 10 ) );

	int sel = m_comboImage.GetCurSel();
	if ( sel != -1 )
	{	DWORD driver = m_comboImage.GetItemData( sel );
		CFG().SetValue( "CMSettings", "Driver", driver );
	} // end if

	sel = m_comboFtpServer.GetCurSel();
	if ( sel != -1 )
	{	char txt[ CWF_STRSIZE ];
		m_comboFtpServer.GetLBText( sel, txt );
		CFG().SetValue( "CMSettings", "FtpServer", txt );	
	} // end if

	CDialog::OnOK();
}

void CDlgSimpleSettings::OnFtpedit() 
{	UpdateData( TRUE );
	FRAME()->ShowSettings( 8 );
	FillFtpCombo();
}
