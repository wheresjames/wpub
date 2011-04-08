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
// PgUrl.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgUrl.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgUrl dialog


CPgUrl::CPgUrl(CWnd* pParent /*=NULL*/)
	: CDialog(CPgUrl::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgUrl)
	m_bAspect = FALSE;
	m_sUrl = _T("");
	m_sType = _T("");
	//}}AFX_DATA_INIT
}


void CPgUrl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgUrl)
	DDX_Control(pDX, IDC_TYPE, m_comboType);
	DDX_Control(pDX, IDC_REFRESH, m_btnRefresh);
	DDX_Check(pDX, IDC_ASPECT, m_bAspect);
	DDX_Text(pDX, IDC_URL, m_sUrl);
	DDX_CBString(pDX, IDC_TYPE, m_sType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgUrl, CDialog)
	//{{AFX_MSG_MAP(CPgUrl)
	ON_BN_CLICKED(IDC_ASPECT, OnAspect)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	ON_EN_CHANGE(IDC_URL, OnChangeUrl)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgUrl message handlers

BOOL CPgUrl::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Set button icon
	m_btnRefresh.SetIcon( CGrDC::LoadIcon( IDI_REFRESH ) );

	// Add supported image types
	m_comboType.AddString( "JPG" );
	m_comboType.AddString( "PNG" );
	m_comboType.AddString( "BMP" );
	m_comboType.AddString( "TGA" );
	m_comboType.AddString( "TIF" );
	m_comboType.AddString( "PCX" );
	m_comboType.AddString( "ICO" );

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgUrl::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return 0; }

	// Get the current string
	m_sUrl = pfx->data;
	m_sType = pfx->sep;

	m_bAspect = ( pfx->f1 & FXF1_ASPECT ) != 0;

	UpdateData( FALSE );

	// Attempt to select the string
	int item = m_comboType.FindString( -1, m_sType );
	if ( item != CB_ERR ) m_comboType.SetCurSel( item );

	return 1;
}

void CPgUrl::OnAspect() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	// Toggle bit
	if ( m_bAspect ) pfx->f1 |= FXF1_ASPECT;
	else pfx->f1 &= ~FXF1_ASPECT;

	APP()->ReGen();
}

void CPgUrl::OnRefresh() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	// Get the new URL
	strcpy_sz( pfx->data, m_sUrl );
	strcpy_sz( pfx->sep, m_sType );
	*pfx->preloaded = 0;
	pfx->loadtimeout = 0;
	
	// Refresh the display
	APP()->ReGen();
}

void CPgUrl::OnChangeUrl() 
{
	UpdateData( TRUE );

	// Get Extension
	LPCTSTR pExt = CWinFile::GetExtension( m_sUrl );
	if ( pExt == NULL || *pExt == 0 ) return;

	// Attempt to select this type
	int item = m_comboType.FindString( -1, pExt );
	if ( item != CB_ERR ) m_comboType.SetCurSel( item );

}
