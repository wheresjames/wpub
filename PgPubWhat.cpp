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
// PgPubWhat.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgPubWhat.h"

#include "DlgTabFrame.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgPubWhat dialog


CPgPubWhat::CPgPubWhat(CWnd* pParent /*=NULL*/)
	: CDialog(CPgPubWhat::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgPubWhat)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPgPubWhat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgPubWhat)
	DDX_Control(pDX, IDC_IMG, m_img);
	DDX_Control(pDX, IDC_IMAGE, m_comboImages);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgPubWhat, CDialog)
	//{{AFX_MSG_MAP(CPgPubWhat)
	ON_CBN_SELCHANGE(IDC_IMAGE, OnSelchangeImage)
	//}}AFX_MSG_MAP
	
	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgPubWhat message handlers

BOOL CPgPubWhat::OnInitDialog() 
{
	CDialog::OnInitDialog();

	FillCombo();

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgPubWhat::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return -1;

	// Select image
	m_comboImages.SelectString( -1, pReg->GetSz( "PUBINFO", "Image" ) );

	// Select default string if needed
	if ( m_comboImages.GetCurSel() == CB_ERR )
	{
		// Select the first image
		m_comboImages.SetCurSel( 0 );
	
		// Get the image name
		char img[ CWF_STRSIZE ];
		if ( m_comboImages.GetLBText( 0, img ) != CB_ERR )

			// Set selected image name
			if ( *img != 0 ) pReg->Set( "PUBINFO", "Image", img );

	} // end if


	UpdateImage();

	return 0;
}

void CPgPubWhat::FillCombo()
{
	m_comboImages.ResetContent();

	DWORD index = 0;
	LPPUBIMGINFO ppii = NULL;
	while ( ( ppii = (LPPUBIMGINFO)IMGLIST().GetNext( ppii ) ) != NULL )
	{
		int i = m_comboImages.AddString( ppii->name );
		if ( i != CB_ERR ) m_comboImages.SetItemData( i, index++ );
	} // end while

	UpdateImage();
}

void CPgPubWhat::UpdateImage()
{
	int sel = m_comboImages.GetCurSel();
	if ( sel == CB_ERR ) return;

	DWORD i = m_comboImages.GetItemData( sel );
	m_img.SetImage( i );
}

void CPgPubWhat::OnSelchangeImage() 
{
	UpdateImage();

	CReg *pReg = (CReg*)CDlgTabFrame::GetData( GetSafeHwnd() );
	if ( pReg == NULL ) return;

	int sel = m_comboImages.GetCurSel();
	if ( sel == CB_ERR ) return;

	// Get image name
	char img[ CWF_STRSIZE ];
	m_comboImages.GetLBText( sel, img );
	pReg->Set( "PUBINFO", "Image", img );
}
