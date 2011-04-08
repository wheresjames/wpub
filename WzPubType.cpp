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
// WzPubType.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "WzPubType.h"

#include "WzFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWzPubType dialog


CWzPubType::CWzPubType(CWnd* pParent /*=NULL*/)
	: CDialog(CWzPubType::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWzPubType)
	m_nPubType = 0;
	//}}AFX_DATA_INIT
}


void CWzPubType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWzPubType)
	DDX_Radio(pDX, IDC_PUBTYPE, m_nPubType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWzPubType, CDialog)
	//{{AFX_MSG_MAP(CWzPubType)
	ON_BN_CLICKED(IDC_PUBTYPE, OnPubtype)
	ON_BN_CLICKED(IDC_PUBTYPE2, OnPubtype2)
	ON_BN_CLICKED(IDC_PUBTYPE3, OnPubtype3)
	ON_BN_CLICKED(IDC_PUBTYPE4, OnPubtype4)
	ON_BN_CLICKED(IDC_PUBTYPE5, OnPubtype5)
	//}}AFX_MSG_MAP

	ON_MESSAGE( CWZ_PAGECLOSE, OnSave )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWzPubType message handlers

BOOL CWzPubType::OnInitDialog() 
{
	CDialog::OnInitDialog();

	OnRefresh( 0, 0L );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CWzPubType::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	// Get pointer to reg
	CReg *pReg = (CReg*)CWzFrame::GetData( GetSafeHwnd() );
	
	// Init type
	if ( pReg != NULL ) m_nPubType = pReg->GetDword( "CWzPubType", "PubType", 0 );

	UpdateData( FALSE );

	UpdatePubType( m_nPubType );

	return 0;
}

LRESULT CWzPubType::OnSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );

	return 0;
}

void CWzPubType::UpdatePubType(DWORD type)
{

	if ( type == 1 ) 
		CWzFrame::SetNextPage( GetSafeHwnd(), 
			CWzFrame::GetCurrentPage( GetSafeHwnd() ) + 1 );

	else if ( type == 2 ) 
		CWzFrame::SetNextPage( GetSafeHwnd(), 
			CWzFrame::GetCurrentPage( GetSafeHwnd() ) + 2 );

	else if ( type == 3 ) 
		CWzFrame::SetNextPage( GetSafeHwnd(), 
			CWzFrame::GetCurrentPage( GetSafeHwnd() ) + 3 );

	else if ( type == 4 ) 
		CWzFrame::SetNextPage( GetSafeHwnd(), 
			CWzFrame::GetCurrentPage( GetSafeHwnd() ) + 5 );

	else CWzFrame::SetNextPage( GetSafeHwnd(), MAXDWORD );

	// Get pointer to reg
	CReg *pReg = (CReg*)CWzFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return;

	// Init type
	pReg->Set( "CWzPubType", "PubType", (DWORD)type );

}

void CWzPubType::OnPubtype() 
{	UpdatePubType( 0 );
}

void CWzPubType::OnPubtype2() 
{	UpdatePubType( 1 );
}

void CWzPubType::OnPubtype3() 
{	UpdatePubType( 2 );
}

void CWzPubType::OnPubtype4() 
{	UpdatePubType( 3 );
}

void CWzPubType::OnPubtype5() 
{	UpdatePubType( 4 );
}
