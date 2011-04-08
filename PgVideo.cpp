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
// PgVideo.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgVideo.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgVideo dialog


CPgVideo::CPgVideo(CWnd* pParent /*=NULL*/)
	: CDialog(CPgVideo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgVideo)
	m_bAspect = FALSE;
	//}}AFX_DATA_INIT
}


void CPgVideo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgVideo)
	DDX_Control(pDX, IDC_DRIVERS, m_comboDrivers);
	DDX_Check(pDX, IDC_ASPECT, m_bAspect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgVideo, CDialog)
	//{{AFX_MSG_MAP(CPgVideo)
	ON_CBN_SELCHANGE(IDC_DRIVERS, OnSelchangeDrivers)
	ON_BN_CLICKED(IDC_ASPECT, OnAspect)
	//}}AFX_MSG_MAP
	
	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgVideo message handlers

BOOL CPgVideo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDrivers();

	OnRefresh( 0, 0L );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgVideo::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return -1; }

	// Select driver
	SelDriver( pfx->driver );

	m_bAspect = ( pfx->f1 & FXF1_ASPECT ) != 0;

	UpdateData( FALSE );

	return 0;
}

void CPgVideo::GetDrivers()
{
	m_comboDrivers.ResetContent();

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

			int index = m_comboDrivers.AddString( show );
			if ( index != LB_ERR ) m_comboDrivers.SetItemData( index, ci.index );
		} // end if

	} // end for

//	int index = m_comboDrivers.AddString( "Fake Driver" );
//	if ( index != LB_ERR ) m_comboDrivers.SetItemData( index, 1 );


}

void CPgVideo::SelDriver(DWORD index)
{
	int n = m_comboDrivers.GetCount();
	if ( n == 0 ) return;

	// Select driver by index
	for ( int i = 0; i < n; i++ )
		if ( m_comboDrivers.GetItemData( i ) == index )
		{	m_comboDrivers.SetCurSel( i ); return; }

}

void CPgVideo::OnSelchangeDrivers() 
{
	int sel = m_comboDrivers.GetCurSel();
	if ( sel == CB_ERR ) return;
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	// Select new driver
	pfx->driver = m_comboDrivers.GetItemData( sel );

	// Redraw the images
	APP()->ReGen();

	// Reset the video frame rates
	FRAME()->SetFrameRates();
}

void CPgVideo::OnAspect() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	// Toggle bit
	if ( m_bAspect ) pfx->f1 |= FXF1_ASPECT;
	else pfx->f1 &= ~FXF1_ASPECT;

	APP()->ReGen();
}
