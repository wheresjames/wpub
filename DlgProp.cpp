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
// DlgProp.cpp : implementation file
//

#include "stdafx.h"
#include "Vp.h"
#include "DlgProp.h"

#include "MainFrm.h"

#include "VpDoc.h"
#include "VpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgProp dialog


CDlgProp::CDlgProp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_item = NULL;
}


void CDlgProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProp)
	DDX_Control(pDX, IDC_COMBO, m_combo);
	DDX_Control(pDX, IDC_LIST, m_proplist);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProp, CDialog)
	//{{AFX_MSG_MAP(CDlgProp)
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_COMBO, OnSelchangeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgProp message handlers

BOOL CDlgProp::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_proplist.GetListCtrl().HorzLines( TRUE );
	m_proplist.GetListCtrl().VertLines( TRUE );

	// Lose previous list
	m_proplist.EmptyList();

	Size();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgProp::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	Size();	
}

void CDlgProp::Size()
{
	if ( !IsWindow( GetSafeHwnd() ) ) return;

	RECT rect;
	GetClientRect( &rect );

	if ( IsWindow( m_combo.GetSafeHwnd() ) ) 
	{
		RECT combo;
		m_combo.GetClientRect( &combo );
		combo.left = rect.left;
		combo.right = rect.right;

		m_combo.SetWindowPos(	NULL, rect.left + 2, rect.top + 2,
								RW( combo ) - 8, RH( combo ),
								SWP_NOZORDER | SWP_NOACTIVATE );

		rect.top += ( combo.bottom - combo.top ) + 4;

	} // end if

	if ( IsWindow( m_proplist.GetSafeHwnd() ) ) 
	{
		m_proplist.SetWindowPos(	NULL, rect.left, rect.top,
									RW( rect ), RH( rect ),
									SWP_NOZORDER | SWP_NOACTIVATE );
		m_proplist.Size();

	} // end if

	RedrawWindow();
}

BOOL CDlgProp::Show( LPFX pFx )
{
	if ( !IsWindow( GetSafeHwnd() ) ) return FALSE;

	FillCombo();

	return m_proplist.Show( pFx );
}

void CDlgProp::OnSelchangeCombo() 
{
	int n = m_combo.GetCurSel();
	if ( n == CB_ERR ) return;

	LPFX pfx = (LPFX)m_combo.GetItemData( n );
	if ( pfx == NULL ) return;

	EDIT().SetEditFx( pfx );
}

void CDlgProp::FillCombo( )
{
	m_combo.ResetContent();

	CFx *Fx = APP()->GetActiveFx();
	if ( Fx == NULL ) return;

	LPFX pedit = EDIT().GetEditFx();

	DWORD	item = 0;
	LPFX	pfx = NULL;
	int		insert = CB_ERR, sel = -1;

	while ( ( pfx = Fx->GetPrev( pfx ) ) != NULL )
	{
		char str[ CWF_STRSIZE ];

		Fx->GetFxString( pfx, str );

		// Insert string
		insert = m_combo.AddString( str );
		if ( insert != LB_ERR )
			m_combo.SetItemData( insert, (DWORD)pfx );

		// Was this the last seletion?
		if ( pedit == pfx ) sel = insert;

	} // end while

	// Select active item
	if ( sel != -1 ) m_combo.SetCurSel( sel );
}

void CDlgProp::Refresh()
{
	m_proplist.Update();
}
