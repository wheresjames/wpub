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
// WzFtpPg2.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "WzFtpPg2.h"

#include "WzFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWzFtpPg2 dialog


CWzFtpPg2::CWzFtpPg2(CWnd* pParent /*=NULL*/)
	: CDialog(CWzFtpPg2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWzFtpPg2)
	m_bPassive = FALSE;
	m_sPassword = _T("");
	m_bRename = FALSE;
	m_sUsername = _T("");
	//}}AFX_DATA_INIT
}


void CWzFtpPg2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWzFtpPg2)
	DDX_Check(pDX, IDC_PASSIVE, m_bPassive);
	DDX_Text(pDX, IDC_PASSWORD, m_sPassword);
	DDX_Check(pDX, IDC_RENAME, m_bRename);
	DDX_Text(pDX, IDC_USERNAME, m_sUsername);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWzFtpPg2, CDialog)
	//{{AFX_MSG_MAP(CWzFtpPg2)
	//}}AFX_MSG_MAP

	ON_MESSAGE( CWZ_PAGECLOSE, OnPageClose )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWzFtpPg2 message handlers

BOOL CWzFtpPg2::OnInitDialog() 
{
	CDialog::OnInitDialog();

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CWzFtpPg2::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	CRKey *pRk = (CRKey*)CWzFrame::GetData( GetSafeHwnd() );
	if ( pRk == NULL ) return -1;

	m_sUsername = pRk->GetSz( "Username" );
	m_sPassword = pRk->GetSz( "Password" );
	m_bPassive = pRk->GetDword( "Passive", 1 ) != 0;
	m_bRename = pRk->GetDword( "Rename" ) != 0;

	UpdateData( FALSE );

	return 0;
}

LRESULT CWzFtpPg2::OnPageClose(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );

	CRKey *pRk = (CRKey*)CWzFrame::GetData( GetSafeHwnd() );
	if ( pRk == NULL ) return 0;

	pRk->Set( "Username", m_sUsername );	
	pRk->Set( "Password", m_sPassword );	
	pRk->Set( "Passive", (DWORD)( m_bPassive ? 1 : 0 ) );	
	pRk->Set( "Rename", (DWORD)( m_bRename ? 1 : 0 ) );	

	return 0;
}
