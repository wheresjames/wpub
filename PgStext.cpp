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
// PgStext.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgStext.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgStext dialog


CPgStext::CPgStext(CWnd* pParent /*=NULL*/)
	: CDialog(CPgStext::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgStext)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPgStext::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgStext)
	DDX_Control(pDX, IDC_TEXT, m_comboText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgStext, CDialog)
	//{{AFX_MSG_MAP(CPgStext)
	ON_CBN_SELCHANGE(IDC_TEXT, OnSelchangeText)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgStext message handlers

BOOL CPgStext::OnInitDialog() 
{
	CDialog::OnInitDialog();

	FillCombo();

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPgStext::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return; }
	
	// Select current item
	int sel = CComCtrl::ComboGetItemByData( m_comboText.GetSafeHwnd(), pfx->driver );
	if ( sel != CB_ERR ) m_comboText.SetCurSel( sel );

}

void CPgStext::FillCombo()
{
	m_comboText.ResetContent();

	DWORD	i = 0;
	LPCTSTR	pname;
	while ( ( pname = CFx::GetSpecialTextName( i ) ) != NULL )
	{	int n = m_comboText.AddString( pname );
		if ( n != CB_ERR ) m_comboText.SetItemData( n, i );
		i++;
	} // end while
}

void CPgStext::OnSelchangeText() 
{
	int sel = m_comboText.GetCurSel();
	if ( sel == CB_ERR ) return;

	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	pfx->driver = m_comboText.GetItemData( sel );

	APP()->ReGen();
	
}
