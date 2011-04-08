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
// PgTextFlags.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgTextFlags.h"

#include "DlgTabFrame.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgTextFlags dialog


CPgTextFlags::CPgTextFlags(CWnd* pParent /*=NULL*/)
	: CDialog(CPgTextFlags::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgTextFlags)
	m_bEditControl = FALSE;
	m_bEndEllipsis = FALSE;
	m_bEscapementCenter = FALSE;
	m_bExpandTabs = FALSE;
	m_bExternalLeading = FALSE;
	m_bMultiCenter = FALSE;
	m_bNoClip = FALSE;
	m_bNoPrefix = FALSE;
	m_bPathEllipsis = FALSE;
	m_bRtlReading = FALSE;
	m_bSingleLine = FALSE;
	m_bWordBreak = FALSE;
	//}}AFX_DATA_INIT
}


void CPgTextFlags::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgTextFlags)
	DDX_Control(pDX, IDC_HALIGN, m_comboHAlign);
	DDX_Control(pDX, IDC_VALIGN, m_comboVAlign);
	DDX_Check(pDX, IDC_EDITCONTROL, m_bEditControl);
	DDX_Check(pDX, IDC_END_ELLIPSIS, m_bEndEllipsis);
	DDX_Check(pDX, IDC_ESCAPEMENTCENTER, m_bEscapementCenter);
	DDX_Check(pDX, IDC_EXPANDTABS, m_bExpandTabs);
	DDX_Check(pDX, IDC_EXTERNALLEADING, m_bExternalLeading);
	DDX_Check(pDX, IDC_MULTICENTER, m_bMultiCenter);
	DDX_Check(pDX, IDC_NOCLIP, m_bNoClip);
	DDX_Check(pDX, IDC_NOPREFIX, m_bNoPrefix);
	DDX_Check(pDX, IDC_PATH_ELLIPSIS, m_bPathEllipsis);
	DDX_Check(pDX, IDC_RTLREADING, m_bRtlReading);
	DDX_Check(pDX, IDC_SINGLELINE, m_bSingleLine);
	DDX_Check(pDX, IDC_WORDBREAK, m_bWordBreak);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgTextFlags, CDialog)
	//{{AFX_MSG_MAP(CPgTextFlags)
	ON_BN_CLICKED(IDC_EDITCONTROL, OnEditcontrol)
	ON_BN_CLICKED(IDC_END_ELLIPSIS, OnEndEllipsis)
	ON_BN_CLICKED(IDC_ESCAPEMENTCENTER, OnEscapementcenter)
	ON_BN_CLICKED(IDC_EXPANDTABS, OnExpandtabs)
	ON_BN_CLICKED(IDC_EXTERNALLEADING, OnExternalleading)
	ON_BN_CLICKED(IDC_MULTICENTER, OnMulticenter)
	ON_BN_CLICKED(IDC_NOCLIP, OnNoclip)
	ON_BN_CLICKED(IDC_NOPREFIX, OnNoprefix)
	ON_BN_CLICKED(IDC_PATH_ELLIPSIS, OnPathEllipsis)
	ON_BN_CLICKED(IDC_RTLREADING, OnRtlreading)
	ON_BN_CLICKED(IDC_SINGLELINE, OnSingleline)
	ON_BN_CLICKED(IDC_WORDBREAK, OnWordbreak)
	ON_CBN_SELCHANGE(IDC_HALIGN, OnSelchangeHalign)
	ON_CBN_SELCHANGE(IDC_VALIGN, OnSelchangeValign)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgTextFlags message handlers

BOOL CPgTextFlags::OnInitDialog() 
{
	CDialog::OnInitDialog();

	FillHAlign();
	FillVAlign();

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgTextFlags::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return -1;

	// Update switches
	m_bEditControl = ( pfx->flags & DT_EDITCONTROL ) != 0;
	m_bEndEllipsis = ( pfx->flags & DT_END_ELLIPSIS ) != 0;
	m_bEscapementCenter = ( pfx->flags & EDT_ESCAPEMENTCENTER ) != 0;
	m_bExpandTabs = ( pfx->flags & DT_EXPANDTABS ) != 0;
	m_bExternalLeading = ( pfx->flags & DT_EXTERNALLEADING ) != 0;
	m_bMultiCenter = ( pfx->flags & EDT_MULTICENTER ) != 0;
	m_bNoClip = ( pfx->flags & DT_NOCLIP ) != 0;
	m_bNoPrefix = ( pfx->flags & DT_NOPREFIX ) != 0;
	m_bPathEllipsis = ( pfx->flags & DT_PATH_ELLIPSIS ) != 0;
	m_bRtlReading = ( pfx->flags & DT_RTLREADING ) != 0;
	m_bSingleLine = ( pfx->flags & DT_SINGLELINE ) != 0;
	m_bWordBreak = ( pfx->flags & DT_WORDBREAK ) != 0;

	if ( ( pfx->flags & DT_CENTER ) != 0 ) m_comboHAlign.SetCurSel( 1 );
	else if ( ( pfx->flags & DT_RIGHT ) != 0 ) m_comboHAlign.SetCurSel( 2 );
	else
	{
		pfx->flags |= DT_LEFT;
		m_comboHAlign.SetCurSel( 0 );
	} // end else if

	if ( ( pfx->flags & DT_VCENTER ) != 0 ) m_comboVAlign.SetCurSel( 1 );
	else if ( ( pfx->flags & DT_BOTTOM ) != 0 ) m_comboVAlign.SetCurSel( 2 );
	else
	{
		pfx->flags |= DT_TOP;
		m_comboVAlign.SetCurSel( 0 );
	} // end else if

	// Enable / disable controls
	if ( m_bMultiCenter )
	{
		CWnd* pCtrl = GetDlgItem( IDC_ESCAPEMENTCENTER );
		if ( pCtrl != NULL ) pCtrl->EnableWindow( TRUE );
		pCtrl = GetDlgItem( IDC_NOCLIP );
		if ( pCtrl != NULL ) pCtrl->EnableWindow( FALSE );
		m_bNoClip = TRUE;
		pfx->flags |= DT_NOCLIP;
//		m_comboHAlign.EnableWindow( FALSE );
		m_comboVAlign.EnableWindow( FALSE );
	} // end if
	else
	{
		CWnd* pCtrl = GetDlgItem( IDC_ESCAPEMENTCENTER );
		if ( pCtrl != NULL ) pCtrl->EnableWindow( FALSE );
		pCtrl = GetDlgItem( IDC_NOCLIP );
		if ( pCtrl != NULL ) pCtrl->EnableWindow( TRUE );
//		m_comboHAlign.EnableWindow( TRUE );
		if ( m_bSingleLine ) m_comboVAlign.EnableWindow( TRUE );
		else m_comboVAlign.EnableWindow( FALSE );
		pfx->flags &= ~EDT_ESCAPEMENTCENTER;
		m_bEscapementCenter = FALSE;
	} // end else

	UpdateData( FALSE );

	return 0;
}

#define GETSWITCH( var, mask )		\
	if ( var ) pfx->flags |= mask;	\
	else pfx->flags &= ~mask;

void CPgTextFlags::UpdateSwitches()
{
	UpdateData( TRUE );

	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	GETSWITCH( m_bEditControl, DT_EDITCONTROL );
	GETSWITCH( m_bEndEllipsis, DT_END_ELLIPSIS );
	GETSWITCH( m_bEscapementCenter, EDT_ESCAPEMENTCENTER );
	GETSWITCH( m_bExpandTabs, DT_EXPANDTABS );
	GETSWITCH( m_bExternalLeading, DT_EXTERNALLEADING );
	GETSWITCH( m_bMultiCenter, EDT_MULTICENTER );
	GETSWITCH( m_bNoClip, DT_NOCLIP );
	GETSWITCH( m_bNoPrefix, DT_NOPREFIX );
	GETSWITCH( m_bPathEllipsis, DT_PATH_ELLIPSIS );
	GETSWITCH( m_bRtlReading, DT_RTLREADING );
	GETSWITCH( m_bSingleLine, DT_SINGLELINE );
	GETSWITCH( m_bWordBreak, DT_WORDBREAK );

	// Enable / disable controls
	if ( m_bMultiCenter )
	{
		CWnd* pCtrl = GetDlgItem( IDC_ESCAPEMENTCENTER );
		if ( pCtrl != NULL ) pCtrl->EnableWindow( TRUE );
		pCtrl = GetDlgItem( IDC_NOCLIP );
		if ( pCtrl != NULL ) pCtrl->EnableWindow( FALSE );
		m_bNoClip = TRUE;
		pfx->flags |= DT_NOCLIP;
//		m_comboHAlign.EnableWindow( FALSE );
		m_comboVAlign.EnableWindow( FALSE );
	} // end if
	else
	{
		CWnd* pCtrl = GetDlgItem( IDC_ESCAPEMENTCENTER );
		if ( pCtrl != NULL ) pCtrl->EnableWindow( FALSE );
		pCtrl = GetDlgItem( IDC_NOCLIP );
		if ( pCtrl != NULL ) pCtrl->EnableWindow( TRUE );
//		m_comboHAlign.EnableWindow( TRUE );
		if ( m_bSingleLine ) m_comboVAlign.EnableWindow( TRUE );
		else m_comboVAlign.EnableWindow( FALSE );
		pfx->flags &= ~EDT_ESCAPEMENTCENTER;
		m_bEscapementCenter = FALSE;
	} // end else

	APP()->ReGen();

	UpdateData( FALSE );
}

void CPgTextFlags::OnEditcontrol() 
{	UpdateSwitches(); }

void CPgTextFlags::OnEndEllipsis() 
{	UpdateSwitches(); }

void CPgTextFlags::OnEscapementcenter() 
{	UpdateSwitches(); }

void CPgTextFlags::OnExpandtabs() 
{	UpdateSwitches(); }

void CPgTextFlags::OnExternalleading() 
{	UpdateSwitches(); }

void CPgTextFlags::OnMulticenter() 
{	UpdateSwitches(); }

void CPgTextFlags::OnNoclip() 
{	UpdateSwitches(); }

void CPgTextFlags::OnNoprefix() 
{	UpdateSwitches(); }

void CPgTextFlags::OnPathEllipsis() 
{	UpdateSwitches(); }

void CPgTextFlags::OnRtlreading() 
{	UpdateSwitches(); }

void CPgTextFlags::OnSingleline() 
{	UpdateSwitches(); }

void CPgTextFlags::OnWordbreak() 
{	UpdateSwitches(); }

void CPgTextFlags::OnSelchangeHalign() 
{
	int sel = m_comboHAlign.GetCurSel();
	if ( sel == CB_ERR ) return;
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	pfx->flags &= ~( DT_LEFT | DT_CENTER | DT_RIGHT );
	pfx->flags |= m_comboHAlign.GetItemData( sel );	

	APP()->ReGen();
}

void CPgTextFlags::OnSelchangeValign() 
{
	int sel = m_comboVAlign.GetCurSel();
	if ( sel == CB_ERR ) return;
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	pfx->flags &= ~( DT_TOP | DT_VCENTER | DT_BOTTOM );
	pfx->flags |= m_comboVAlign.GetItemData( sel );	

	APP()->ReGen();
}

void CPgTextFlags::FillVAlign()
{
	m_comboVAlign.ResetContent();

	int x;
	x = m_comboVAlign.AddString( "Top" );
	m_comboVAlign.SetItemData( x, DT_TOP );
	x = m_comboVAlign.AddString( "Center" );
	m_comboVAlign.SetItemData( x, DT_VCENTER );
	x = m_comboVAlign.AddString( "Bottom" );
	m_comboVAlign.SetItemData( x, DT_BOTTOM );
}

void CPgTextFlags::FillHAlign()
{
	m_comboHAlign.ResetContent();

	int x;
	x = m_comboHAlign.AddString( "Left" );
	m_comboHAlign.SetItemData( x, DT_LEFT );
	x = m_comboHAlign.AddString( "Center" );
	m_comboHAlign.SetItemData( x, DT_CENTER );
	x = m_comboHAlign.AddString( "Right" );
	m_comboHAlign.SetItemData( x, DT_RIGHT );
}
void CPgTextFlags::OnOK() 
{
}
void CPgTextFlags::OnCancel() 
{
}
