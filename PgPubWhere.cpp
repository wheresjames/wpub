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
// PgPubWhere.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgPubWhere.h"

#include "DlgComposeMail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgPubWhere dialog


CPgPubWhere::CPgPubWhere(CWnd* pParent /*=NULL*/)
	: CDialog(CPgPubWhere::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgPubWhere)
	m_nType = -1;
	m_sPath = _T("");
	//}}AFX_DATA_INIT
}


void CPgPubWhere::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgPubWhere)
	DDX_Control(pDX, IDC_FTP, m_comboFtp);
	DDX_Control(pDX, IDC_EMAIL, m_comboEmail);
	DDX_Radio(pDX, IDC_TYPE, m_nType);
	DDX_Text(pDX, IDC_PATH, m_sPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgPubWhere, CDialog)
	//{{AFX_MSG_MAP(CPgPubWhere)
	ON_BN_CLICKED(IDC_TYPE, OnType)
	ON_BN_CLICKED(IDC_TYPE2, OnType2)
	ON_BN_CLICKED(IDC_TYPE3, OnType3)
	ON_CBN_SELCHANGE(IDC_FTP, OnSelchangeFtp)
	ON_CBN_SELCHANGE(IDC_EMAIL, OnSelchangeEmail)
	ON_BN_CLICKED(IDC_EMAILMSG, OnEmailmsg)
	ON_BN_CLICKED(IDC_CHOOSE_PATH, OnChoosePath)
	//}}AFX_MSG_MAP
	
	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgPubWhere message handlers

BOOL CPgPubWhere::OnInitDialog() 
{
	CDialog::OnInitDialog();

	FillCombo();

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgPubWhere::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	m_nType = -1;

	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return -1;

	// Get the file name
	m_sPath = pReg->GetSz( "PUBINFO", "Path" );

	DWORD type = pReg->GetDword( "PUBINFO", "Type" );
	// Get further information
	if ( type == PUBTYPE_FTP )
		m_nType = 0, m_comboFtp.SelectString( -1, pReg->GetSz( "PUBINFO", "Str" ) );
	else if ( type == PUBTYPE_EMAIL )
		m_nType = 1, m_comboEmail.SelectString( -1, pReg->GetSz( "PUBINFO", "Str" ) );
	else if ( type == PUBTYPE_DISK )
		m_nType = 2; 

	UpdateData( FALSE );

	DoEnable();

	return 0;
}

void CPgPubWhere::FillCombo()
{
	// Fill in FTP servers
	m_comboFtp.ResetContent();
	HGROUP hGroup = NULL;
	while ( ( hGroup = FTPSERVERS().GetNext( hGroup ) ) != NULL )
		int item = m_comboFtp.AddString( hGroup->name );		

	// Fill in Email servers
	m_comboEmail.ResetContent();
	hGroup = NULL;
	while ( ( hGroup = EMAILSERVERS().GetNext( hGroup ) ) != NULL )
		int item = m_comboEmail.AddString( hGroup->name );		

}

void CPgPubWhere::DoEnable()
{
	UpdateData( TRUE );

	CWnd *pWnd = GetDlgItem( IDC_FTP );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nType == 0 );

	pWnd = GetDlgItem( IDC_EMAIL );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nType == 1 );
	pWnd = GetDlgItem( IDC_EMAILMSG );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nType == 1 );

	pWnd = GetDlgItem( IDC_CHOOSE_PATH );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nType == 2 );

	pWnd = GetDlgItem( IDC_PATH );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nType == 2 );
}

void CPgPubWhere::OnType() 
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg != NULL ) pReg->Set( "PUBINFO", "Type", (DWORD)PUBTYPE_FTP );

	DoEnable();
	
	OnSelchangeFtp();
}

void CPgPubWhere::OnType2() 
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg != NULL ) pReg->Set( "PUBINFO", "Type", (DWORD)PUBTYPE_EMAIL );

	DoEnable();

	OnSelchangeEmail();
}

void CPgPubWhere::OnType3() 
{
	DoEnable();

	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg != NULL ) pReg->Set( "PUBINFO", "Type", (DWORD)PUBTYPE_DISK );

	UpdateData( FALSE );
}

void CPgPubWhere::OnSelchangeFtp() 
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return;

	char str[ CWF_STRSIZE ];
	int sel = m_comboFtp.GetCurSel();	
	if ( sel == CB_ERR ) *str = 0;
	else m_comboFtp.GetLBText( sel, str );	
	pReg->Set( "PUBINFO", "Str", str );

}

void CPgPubWhere::OnSelchangeEmail() 
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return;

	char str[ CWF_STRSIZE ];
	int sel = m_comboEmail.GetCurSel();	
	if ( sel == CB_ERR ) *str = 0;
	else m_comboEmail.GetLBText( sel, str );	
	pReg->Set( "PUBINFO", "Str", str );

}

void CPgPubWhere::OnEmailmsg() 
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return;
	
	CDlgComposeMail	dlg;
	
	dlg.m_sTo = pReg->GetSz( "PUBINFO", "To" );
	dlg.m_sFrom = pReg->GetSz( "PUBINFO", "From" );
	dlg.m_sSubject = pReg->GetSz( "PUBINFO", "Subject" );
	dlg.m_sMsg = pReg->GetSz( "PUBINFO", "Msg" );

	if ( dlg.DoModal() != IDOK ) return;

	pReg->Set( "PUBINFO", "To", dlg.m_sTo );
	pReg->Set( "PUBINFO", "From", dlg.m_sFrom );
	pReg->Set( "PUBINFO", "Subject", dlg.m_sSubject );
	pReg->Set( "PUBINFO", "Msg", dlg.m_sMsg );

}

void CPgPubWhere::OnChoosePath() 
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return;
	
	// Get the file name
	char path[ CWF_STRSIZE ] = { 0 };
	pReg->Get( "PUBINFO", "Path", path, sizeof( path ) );

	// Get user path
	if ( !CWinFile::GetBrowseDir( GetSafeHwnd(), "File Path", path, path ) )
		return;

	// Save the path
	pReg->Set( "PUBINFO", "Path", path );
	m_sPath = path;

	UpdateData( FALSE );
}
