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
// PgPubWhen.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgPubWhen.h"

#include "WzFrame.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgPubWhen dialog


CPgPubWhen::CPgPubWhen(CWnd* pParent /*=NULL*/)
	: CDialog(CPgPubWhen::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgPubWhen)
	m_sHour = _T("");
	m_sMin = _T("");
	m_sSec = _T("");
	m_bSyncToClock = FALSE;
	m_nWhen = -1;
	m_sHour2 = _T("");
	m_sMin2 = _T("");
	m_sSec2 = _T("");
	//}}AFX_DATA_INIT
}


void CPgPubWhen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgPubWhen)
	DDX_Control(pDX, IDC_DRIVER, m_comboDriver);
	DDX_Text(pDX, IDC_HOUR, m_sHour);
	DDX_Text(pDX, IDC_MIN, m_sMin);
	DDX_Text(pDX, IDC_SEC, m_sSec);
	DDX_Check(pDX, IDC_SYNCTOCLOCK, m_bSyncToClock);
	DDX_Radio(pDX, IDC_WHEN, m_nWhen);
	DDX_Text(pDX, IDC_HOUR2, m_sHour2);
	DDX_Text(pDX, IDC_MIN2, m_sMin2);
	DDX_Text(pDX, IDC_SEC2, m_sSec2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgPubWhen, CDialog)
	//{{AFX_MSG_MAP(CPgPubWhen)
	ON_EN_CHANGE(IDC_HOUR, OnChangeHour)
	ON_EN_CHANGE(IDC_MIN, OnChangeMin)
	ON_EN_CHANGE(IDC_SEC, OnChangeSec)
	ON_BN_CLICKED(IDC_WHEN, OnWhen)
	ON_BN_CLICKED(IDC_WHEN2, OnWhen2)
	ON_BN_CLICKED(IDC_SYNCTOCLOCK, OnSynctoclock)
	ON_EN_CHANGE(IDC_HOUR2, OnChangeHour2)
	ON_EN_CHANGE(IDC_MIN2, OnChangeMin2)
	ON_EN_CHANGE(IDC_SEC2, OnChangeSec2)
	ON_CBN_SELCHANGE(IDC_DRIVER, OnSelchangeDriver)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_SAVE, OnSave )
	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgPubWhen message handlers

BOOL CPgPubWhen::OnInitDialog() 
{
	CDialog::OnInitDialog();

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgPubWhen::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return -1;

	// Can't have zero interval
	DWORD interval = pReg->GetDword( "PUBINFO", "Interval", 30 );
	DWORD motioninterval = pReg->GetDword( "PUBINFO", "MotionInterval", 30 );
	pReg->Set( "PUBINFO", "Interval", interval );
	pReg->Set( "PUBINFO", "MotionInterval", motioninterval );

	{ // Get interval

		char msg[ 256 ];
		DWORD h, m, s;
		CWinTime::DivideSeconds( interval, NULL, &h, &m, &s );
		wsprintf( msg, "%lu", s ); m_sSec = msg;
		wsprintf( msg, "%lu", m ); m_sMin = msg;
		wsprintf( msg, "%lu", h ); m_sHour = msg;

		CWinTime::DivideSeconds( motioninterval, NULL, &h, &m, &s );
		wsprintf( msg, "%lu", s ); m_sSec2 = msg;
		wsprintf( msg, "%lu", m ); m_sMin2 = msg;
		wsprintf( msg, "%lu", h ); m_sHour2 = msg;

	} // end interval

	m_bSyncToClock = ( pReg->GetDword( "PUBINFO", "F1" ) & PUBF1_SYNCTOCLOCK ) != 0;

	m_nWhen = 0;
	if ( ( pReg->GetDword( "PUBINFO", "F1" ) & PUBF1_MOTION ) ) m_nWhen = 1;

	UpdateData( FALSE );

	FillCombo();

	// Select motion driver
	CComCtrl::ComboSelectItemByData(	m_comboDriver.GetSafeHwnd(), 
										pReg->GetDword( "PUBINFO", "Motion" ) );

	DoEnable();

	return 0;
}


LRESULT CPgPubWhen::OnSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );

	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return -1;

	// Calculate interval
	pReg->Set( "PUBINFO", "Interval",
		(DWORD)CWinTime::IntegrateSeconds(	0, 
											strtoul( m_sHour, NULL, 10 ),
											strtoul( m_sMin, NULL, 10 ),
											strtoul( m_sSec, NULL, 10 ) ) );

	// Calculate interval
	pReg->Set( "PUBINFO", "MotionInterval",
		(DWORD)CWinTime::IntegrateSeconds(	0, 
											strtoul( m_sHour2, NULL, 10 ),
											strtoul( m_sMin2, NULL, 10 ),
											strtoul( m_sSec2, NULL, 10 ) ) );

	DWORD f1 = pReg->GetDword( "PUBINFO", "F1" );
	if ( m_bSyncToClock ) f1 |= PUBF1_SYNCTOCLOCK;
	else f1 &= ~PUBF1_SYNCTOCLOCK;

	if ( m_nWhen == 1 ) f1 |= PUBF1_MOTION;
	else f1 &= ~PUBF1_MOTION;	
	
	pReg->Set( "PUBINFO", "F1", f1 );

	int sel = m_comboDriver.GetCurSel();
	if ( sel != -1 ) pReg->Set( "PUBINFO", "Motion", (DWORD)m_comboDriver.GetItemData( sel ) );

	return 0;
}

void CPgPubWhen::OnChangeHour() 
{	OnSave( 0, 0L );
}

void CPgPubWhen::OnChangeMin() 
{	OnSave( 0, 0L );
}

void CPgPubWhen::OnChangeSec() 
{	OnSave( 0, 0L );
}

void CPgPubWhen::DoEnable()
{
	UpdateData( TRUE );

	if ( m_nWhen == 0 ) 
		CWzFrame::SetNextPage( GetSafeHwnd(), 
			CWzFrame::GetCurrentPage( GetSafeHwnd() ) + 2 );

	else if ( m_nWhen == 1 ) 
		CWzFrame::SetNextPage( GetSafeHwnd(), 
			CWzFrame::GetCurrentPage( GetSafeHwnd() ) + 1 );

	CWnd *pWnd = GetDlgItem( IDC_HOUR );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nWhen == 0 );
	pWnd = GetDlgItem( IDC_TXT_HRS );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nWhen == 0 );
	pWnd = GetDlgItem( IDC_MIN );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nWhen == 0 );
	pWnd = GetDlgItem( IDC_TXT_MINS );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nWhen == 0 );
	pWnd = GetDlgItem( IDC_SEC );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nWhen == 0 );
	pWnd = GetDlgItem( IDC_TXT_SECONDS );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nWhen == 0 );
	pWnd = GetDlgItem( IDC_SYNCTOCLOCK );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nWhen == 0 );

	pWnd = GetDlgItem( IDC_DRIVER );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nWhen == 1 );
	pWnd = GetDlgItem( IDC_TXT_MININT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nWhen == 1 );
	pWnd = GetDlgItem( IDC_HOUR2 );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nWhen == 1 );
	pWnd = GetDlgItem( IDC_TXT_HRS2 );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nWhen == 1 );
	pWnd = GetDlgItem( IDC_MIN2 );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nWhen == 1 );
	pWnd = GetDlgItem( IDC_TXT_MINS2 );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nWhen == 1 );
	pWnd = GetDlgItem( IDC_SEC2 );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nWhen == 1 );
	pWnd = GetDlgItem( IDC_TXT_SECONDS2 );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_nWhen == 1 );

}

void CPgPubWhen::OnWhen() 
{	OnSave( 0, 0L ); DoEnable();
}

void CPgPubWhen::OnWhen2() 
{	OnSave( 0, 0L ); DoEnable();
}

void CPgPubWhen::OnSynctoclock() 
{	OnSave( 0, 0L );	
}

void CPgPubWhen::FillCombo()
{
	// Empty combo
	m_comboDriver.ResetContent();

	// Load capture drier names
	CCapture	cap;
	DWORD		drivers = cap.GetNumDrivers();
	for ( DWORD i = 0; i < drivers; i++ )
	{
		CAPTUREDRIVERINFO	ci;

		// For each driver that has been loaded
		if (	cap.GetDriverInfo( i, &ci ) )
		{
			int index = CB_ERR;
			char show[ CWF_STRSIZE ];
			char str[ 256 ];

			// Driver string
			wsprintf( str, "Driver %lu", i );		

			// Driver name
			sprintf( show, "%s (%s)", ci.szName, ci.szVer );

			if ( *show ) index = m_comboDriver.AddString( show );
			else  index = m_comboDriver.AddString( str );
			if ( index != CB_ERR ) m_comboDriver.SetItemData( index, i );
		} // end if

	} // end for
}

void CPgPubWhen::OnChangeHour2() 
{	OnSave( 0, 0L );
}

void CPgPubWhen::OnChangeMin2() 
{	OnSave( 0, 0L );
}

void CPgPubWhen::OnChangeSec2() 
{	OnSave( 0, 0L );
}

void CPgPubWhen::OnSelchangeDriver() 
{	OnSave( 0, 0L );
}
