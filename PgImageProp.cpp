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
// PgImageProp.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgImageProp.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgImageProp dialog


CPgImageProp::CPgImageProp(CWnd* pParent /*=NULL*/)
	: CDialog(CPgImageProp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgImageProp)
	m_bCustomSize = FALSE;
	m_sHeight = _T("");
	m_sName = _T("");
	m_sWidth = _T("");
	//}}AFX_DATA_INIT
}


void CPgImageProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgImageProp)
	DDX_Check(pDX, IDC_CUSTOMSIZE, m_bCustomSize);
	DDX_Text(pDX, IDC_HEIGHT, m_sHeight);
	DDX_Text(pDX, IDC_NAME, m_sName);
	DDX_Text(pDX, IDC_WIDTH, m_sWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgImageProp, CDialog)
	//{{AFX_MSG_MAP(CPgImageProp)
	ON_BN_CLICKED(IDC_CUSTOMSIZE, OnCustomsize)
	ON_EN_CHANGE(IDC_HEIGHT, OnChangeHeight)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_EN_CHANGE(IDC_WIDTH, OnChangeWidth)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgImageProp message handlers

BOOL CPgImageProp::OnInitDialog() 
{
	CDialog::OnInitDialog();

	OnRefresh( 0, 0L );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgImageProp::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return FALSE;

	m_sName = ppii->name;

	m_bCustomSize = ( ppii->f1 & PIIF1_CUSTOMSIZE ) != 0;

	long w = ppii->rect.right - ppii->rect.left;
	long h = ppii->rect.bottom - ppii->rect.top;

	char num[ 256 ];

	wsprintf( num, "%lu", w );
	m_sWidth = num;

	wsprintf( num, "%lu", h );
	m_sHeight = num;

	UpdateData( FALSE );

	DoEnable();	

	return 0;
}

void CPgImageProp::DoEnable()
{
	UpdateData( TRUE );

	CWnd *pWnd = GetDlgItem( IDC_WIDTH );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bCustomSize );
	pWnd = GetDlgItem( IDC_HEIGHT );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bCustomSize );
}

void CPgImageProp::OnCustomsize() 
{
	DoEnable();

	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;
	
	// Toggle custom size mode
	if ( m_bCustomSize ) ppii->f1 |= PIIF1_CUSTOMSIZE;
	else ppii->f1 &= ~PIIF1_CUSTOMSIZE;

	APP()->ReGen();

	EDIT().Size( NULL );

	OnRefresh( 0, 0L );
}

void CPgImageProp::OnChangeHeight() 
{
	UpdateData( TRUE );

	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	long h = strtoul( m_sHeight, NULL, 10 );
	if ( h <= 8 || h > 2600 ) h = 320;

	ppii->rect.bottom = ppii->rect.top + h;

	EDIT().Size( NULL );

	// Regen everything
	FRAME()->SetUpdateTimer();
}

void CPgImageProp::OnChangeWidth() 
{
	UpdateData( TRUE );

	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	long w = strtoul( m_sWidth, NULL, 10 );
	if ( w <= 8 || w > 2600 ) w = 320;

	ppii->rect.right = ppii->rect.left + w;

	EDIT().Size( NULL );

	// Regen everything
	FRAME()->SetUpdateTimer();
}

void CPgImageProp::OnChangeName() 
{
	UpdateData( TRUE );

	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	// Verify non-NULL non-preexisting name
	if ( !m_sName.IsEmpty() && IMGLIST().FindByName( m_sName ) == NULL )
		strcpy( ppii->name, m_sName );

	FRAME()->SetUpdateTimer();
}

