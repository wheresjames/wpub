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
// PgTime.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgTime.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgTime dialog


CPgTime::CPgTime(CWnd* pParent /*=NULL*/)
	: CDialog(CPgTime::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgTime)
	m_nGmt = -1;
	m_nStyle = -1;
	m_sFormatStr = _T("");
	//}}AFX_DATA_INIT
}


void CPgTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgTime)
	DDX_Radio(pDX, IDC_GMT, m_nGmt);
	DDX_Radio(pDX, IDC_STYLE, m_nStyle);
	DDX_Text(pDX, IDC_FORMATSTR, m_sFormatStr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgTime, CDialog)
	//{{AFX_MSG_MAP(CPgTime)
	ON_BN_CLICKED(IDC_GMT, OnGmt)
	ON_BN_CLICKED(IDC_GMT2, OnGmt2)
	ON_BN_CLICKED(IDC_STYLE, OnStyle)
	ON_BN_CLICKED(IDC_STYLE2, OnStyle2)
	ON_EN_CHANGE(IDC_FORMATSTR, OnChangeFormatstr)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgTime message handlers

BOOL CPgTime::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgTime::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return -1; }


	if ( ( pfx->f1 & FXF1_GMT ) != 0 ) m_nGmt = 1;
	else m_nGmt = 0;

	if ( ( pfx->f1 & FXF1_ANALOG ) != 0 ) m_nStyle = 1;
	else m_nStyle = 0;

	// Force default time format
	if ( *pfx->sep == 0 )
	{
		// Ultra backward compatible
		if ( ( pfx->f1 & FXF1_24HR ) != 0 ) strcpy( pfx->sep, "%g:%m:%s %A" );
		else strcpy( pfx->sep, "%h:%m:%s %A" );
		
	} // end if

	// Clear old flag
	pfx->f1 &= ~FXF1_24HR;

	// Set the time format
	m_sFormatStr = pfx->sep;

	UpdateData( FALSE );

	DoEnable();

	return 0;
}


void CPgTime::OnGmt() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return; }
	pfx->f1 &= ~FXF1_GMT;
	APP()->ReGen();
}

void CPgTime::OnGmt2() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return; }
	pfx->f1 |= FXF1_GMT;
	APP()->ReGen();
}

void CPgTime::OnOK() 
{
}
void CPgTime::OnCancel() 
{
}

void CPgTime::OnStyle() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return; }
	pfx->f1 &= ~FXF1_ANALOG;
	DoEnable();
	APP()->ReGen();
}

void CPgTime::OnStyle2() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return; }
	pfx->f1 |= FXF1_ANALOG;
	DoEnable();
	APP()->ReGen();
}

void CPgTime::OnChangeFormatstr() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return; }
	strcpy( pfx->sep, m_sFormatStr );
	APP()->ReGen();
}

void CPgTime::DoEnable()
{
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	CWnd *pCtrl = GetDlgItem( IDC_FORMATSTR );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( ( pfx->f1 & FXF1_ANALOG ) == 0 );

}
