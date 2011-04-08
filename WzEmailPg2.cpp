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
// WzEmailPg2.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "WzEmailPg2.h"

#include "WzFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWzEmailPg2 dialog


CWzEmailPg2::CWzEmailPg2(CWnd* pParent /*=NULL*/)
	: CDialog(CWzEmailPg2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWzEmailPg2)
	m_bLogin = FALSE;
	m_sPassword = _T("");
	m_sUsername = _T("");
	m_bAuthDetect = FALSE;
	//}}AFX_DATA_INIT
}


void CWzEmailPg2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWzEmailPg2)
	DDX_Control(pDX, IDC_AUTHTYPE, m_comboAuthType);
	DDX_Check(pDX, IDC_LOGIN, m_bLogin);
	DDX_Text(pDX, IDC_PASSWORD, m_sPassword);
	DDX_Text(pDX, IDC_USERNAME, m_sUsername);
	DDX_Check(pDX, IDC_AUTHDETECT, m_bAuthDetect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWzEmailPg2, CDialog)
	//{{AFX_MSG_MAP(CWzEmailPg2)
	ON_BN_CLICKED(IDC_LOGIN, OnLogin)
	ON_BN_CLICKED(IDC_AUTHDETECT, OnAuthdetect)
	//}}AFX_MSG_MAP

	ON_MESSAGE( CWZ_PAGECLOSE, OnPageClose )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWzEmailPg2 message handlers

BOOL CWzEmailPg2::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	FillCombo();

	OnRefresh( 0, 0L );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CWzEmailPg2::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	CRKey *pRk = (CRKey*)CWzFrame::GetData( GetSafeHwnd() );
	if ( pRk == NULL ) return -1;

	m_bAuthDetect = pRk->GetDword( "AuthDetect", 1 ) != 0;
	m_bLogin = pRk->GetDword( "Login", 1 ) != 0;
	m_dwAuthType = pRk->GetDword( "AuthType" );

	m_sUsername = pRk->GetSz( "Username" );
	m_sPassword = pRk->GetSz( "Password" );
	
	UpdateData( FALSE );

	FillCombo();
	DoEnable();

	return 1;
}

void CWzEmailPg2::FillCombo()
{
	// Empty list
	m_comboAuthType.ResetContent();

	// Setup combo
	int i = m_comboAuthType.AddString( "Plain Text (PLAIN)" );
	if ( i != CB_ERR )
	{	m_comboAuthType.SetItemData( i, SMTPAUTH_PLAIN );
		if ( m_dwAuthType == SMTPAUTH_PLAIN ) m_comboAuthType.SetCurSel( i );
	} // end if
	i = m_comboAuthType.AddString( "Base64 (LOGIN)" );
	if ( i != CB_ERR )
	{	m_comboAuthType.SetItemData( i, SMTPAUTH_LOGIN );
		if ( m_dwAuthType == SMTPAUTH_LOGIN ) m_comboAuthType.SetCurSel( i );
	} // end if
	i = m_comboAuthType.AddString( "Cram-MD5 (CRAM-MD5)" );
	if ( i != CB_ERR )
	{	m_comboAuthType.SetItemData( i, SMTPAUTH_CRAMMD5 );
		if ( m_dwAuthType == SMTPAUTH_CRAMMD5 ) m_comboAuthType.SetCurSel( i );
	} // end if
}

void CWzEmailPg2::DoEnable()
{
	CWnd *pWnd = GetDlgItem( IDC_USERNAME );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bLogin );
	pWnd = GetDlgItem( IDC_PASSWORD );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bLogin );

	pWnd = GetDlgItem( IDC_AUTHDETECT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bLogin );
	pWnd = GetDlgItem( IDC_AUTHTYPE );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bLogin && !m_bAuthDetect );

}

void CWzEmailPg2::OnLogin() 
{	UpdateData( TRUE ); DoEnable();
}

void CWzEmailPg2::OnAuthdetect() 
{	UpdateData( TRUE ); DoEnable();
}


LRESULT CWzEmailPg2::OnPageClose(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );

	CRKey *pRk = (CRKey*)CWzFrame::GetData( GetSafeHwnd() );
	if ( pRk == NULL ) return -1;

	int sel = m_comboAuthType.GetCurSel();
	if ( sel != CB_ERR ) m_dwAuthType = m_comboAuthType.GetItemData( sel );

	pRk->Set( "AuthDetect", (DWORD)m_bAuthDetect ? 1 : 0 );
	pRk->Set( "Login", (DWORD)m_bLogin ? 1 : 0 );
	pRk->Set( "AuthType", m_dwAuthType );
	pRk->Set( "Username", m_sUsername );
	pRk->Set( "Password", m_sPassword );

	return 0;
}
