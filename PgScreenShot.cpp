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
// PgScreenShot.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgScreenShot.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgScreenShot dialog


CPgScreenShot::CPgScreenShot(CWnd* pParent /*=NULL*/)
	: CDialog(CPgScreenShot::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgScreenShot)
	m_bAspect = FALSE;
	m_sWindow = _T("");
	//}}AFX_DATA_INIT

	m_bValidWindow = FALSE;
}


void CPgScreenShot::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgScreenShot)
	DDX_Control(pDX, IDC_WINDOW, m_comboWindow);
	DDX_Check(pDX, IDC_ASPECT, m_bAspect);
	DDX_CBString(pDX, IDC_WINDOW, m_sWindow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgScreenShot, CDialog)
	//{{AFX_MSG_MAP(CPgScreenShot)
	ON_CBN_SELCHANGE(IDC_WINDOW, OnSelchangeWindow)
	ON_BN_CLICKED(IDC_ASPECT, OnAspect)
	ON_CBN_EDITCHANGE(IDC_WINDOW, OnEditchangeWindow)
	//}}AFX_MSG_MAP
	
	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgScreenShot message handlers

BOOL CPgScreenShot::OnInitDialog() 
{
	CDialog::OnInitDialog();

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgScreenShot::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	FillCombo();

	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return 0; }

	// Get window name
	if ( *pfx->data != 0 ) m_sWindow = pfx->data;
	else m_sWindow = "< desktop >";

	m_comboWindow.SelectString( -1, m_sWindow );
		
	m_bAspect = ( pfx->f1 & FXF1_ASPECT ) != 0;

	// Do we have a valid window?
	m_bValidWindow = ::IsWindow( ::FindWindow( NULL, pfx->data ) );

	UpdateData( FALSE );

	return 0;
}

void CPgScreenShot::FillCombo()
{
	m_comboWindow.ResetContent();

	// Add the desktop window
	m_comboWindow.SetCurSel( m_comboWindow.AddString( "< desktop >" ) );
	m_comboWindow.AddString( "< active window >" );

	HWND hWnd = ::GetDesktopWindow();
	if ( !::IsWindow( hWnd ) ) return;

	// Get first child window
	hWnd = ::GetWindow( hWnd, GW_CHILD );

	do
	{
		char text[ 256 ];
		if( ::GetWindowText( hWnd, text, sizeof( text ) ) && *text != 0 )
			if ( m_comboWindow.FindString( -1, text ) == CB_ERR )
				m_comboWindow.AddString( text );
				
	} while ( ::IsWindow( hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) ) );


}

void CPgScreenShot::OnSelchangeWindow() 
{
	int sel = m_comboWindow.GetCurSel();
	if ( sel == CB_ERR ) return;

	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	m_comboWindow.GetLBText( sel, m_sWindow );
	strcpy_sz( pfx->data, m_sWindow );

	// Redraw the images
	APP()->ReGen( TRUE );

	m_bValidWindow = TRUE;
}

void CPgScreenShot::OnEditchangeWindow() 
{
	CComboBox *pCombo = (CComboBox*)GetDlgItem( IDC_WINDOW );
	if ( pCombo == NULL ) return;
	pCombo->GetWindowText( m_sWindow );

	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	strcpy_sz( pfx->data, m_sWindow );

	// Do we have a valid window?
	BOOL bValidWindow = ::IsWindow( ::FindWindow( NULL, pfx->data ) );

	// Redraw if needed
	if ( bValidWindow || m_bValidWindow )
		APP()->ReGen( TRUE );

	// Save valid window state
	m_bValidWindow = bValidWindow;
}

void CPgScreenShot::OnAspect() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	// Toggle bit
	if ( m_bAspect ) pfx->f1 |= FXF1_ASPECT;
	else pfx->f1 &= ~FXF1_ASPECT;

	APP()->ReGen();
}

