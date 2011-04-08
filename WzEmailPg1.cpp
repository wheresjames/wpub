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
// WzEmailPg1.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "WzEmailPg1.h"

#include "WzFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWzEmailPg1 dialog


CWzEmailPg1::CWzEmailPg1(CWnd* pParent /*=NULL*/)
	: CDialog(CWzEmailPg1::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWzEmailPg1)
	m_sName = _T("");
	m_sAddr = _T("");
	//}}AFX_DATA_INIT
}


void CWzEmailPg1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWzEmailPg1)
	DDX_Text(pDX, IDC_NAME, m_sName);
	DDX_Text(pDX, IDC_SERVER, m_sAddr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWzEmailPg1, CDialog)
	//{{AFX_MSG_MAP(CWzEmailPg1)
	//}}AFX_MSG_MAP

	ON_MESSAGE( CWZ_PAGECHG, OnPageChange )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWzEmailPg1 message handlers

BOOL CWzEmailPg1::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CWzEmailPg1::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	CRKey *pRk = (CRKey*)CWzFrame::GetData( GetSafeHwnd() );
	if ( pRk == NULL ) return -1;

	m_sName = pRk->GetSz( "ServerName" );
	m_sAddr = pRk->GetSz( "Address" ); 

	// Create a unique group name if needed
	if ( m_sName.IsEmpty() )
	{
		DWORD i = 1;
		char name[ CWF_STRSIZE ];
		do
		{	wsprintf( name, "Email Server %lu", i++ );
		} while ( EMAILSERVERS().FindGroup( name ) != NULL );
		m_sName = name;

	} // end if

	UpdateData( FALSE );

	return 1;
}

LRESULT CWzEmailPg1::OnPageChange(WPARAM wParam, LPARAM lParam)
{
	if ( wParam != 1 ) return 0;

	UpdateData( TRUE );

	// Get reg pointer
	CRKey *pRk = (CRKey*)CWzFrame::GetData( GetSafeHwnd() );
	if ( pRk == NULL ) return 0;

	// Save data
	pRk->Set( "ServerName", m_sName );	
	pRk->Set( "Address", m_sAddr );	

	if ( m_sName.IsEmpty() )
	{	::MessageBox(	GetSafeHwnd(), "Server Name cannot be blank",
		"Blank Field", MB_OK | MB_ICONSTOP ); return 1; }

	if ( m_sAddr.IsEmpty() )
	{	::MessageBox(	GetSafeHwnd(), "Server Address cannot be blank",
		"Blank Field", MB_OK | MB_ICONSTOP ); return 1; }

	return 0;
}
