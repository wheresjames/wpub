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
// PgRect.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgRect.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgRect dialog


CPgRect::CPgRect(CWnd* pParent /*=NULL*/)
	: CDialog(CPgRect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgRect)
	m_bBck = FALSE;
	m_sTranslucency = _T("");
	m_bBorder = FALSE;
	m_bFoundered = FALSE;
	//}}AFX_DATA_INIT
}


void CPgRect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgRect)
	DDX_Control(pDX, IDC_COLORBOX, m_colorbox);
	DDX_Check(pDX, IDC_BCK, m_bBck);
	DDX_Text(pDX, IDC_TRANSLUCENCY, m_sTranslucency);
	DDX_Check(pDX, IDC_BORDER, m_bBorder);
	DDX_Check(pDX, IDC_FOUNDERED, m_bFoundered);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgRect, CDialog)
	//{{AFX_MSG_MAP(CPgRect)
	ON_BN_CLICKED(IDC_COLOR, OnColor)
	ON_BN_CLICKED(IDC_BCK, OnBck)
	ON_EN_CHANGE(IDC_TRANSLUCENCY, OnChangeTranslucency)
	ON_BN_CLICKED(IDC_BORDER, OnBorder)
	ON_BN_CLICKED(IDC_FOUNDERED, OnFoundered)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgRect message handlers

BOOL CPgRect::OnInitDialog() 
{
	CDialog::OnInitDialog();

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgRect::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return -1;

	CWnd *pBck = GetDlgItem( IDC_BCK );
	if ( pBck != NULL )
	{
		if ( pfx->type == FXTYPE_RECT )
		{	pBck->EnableWindow( FALSE );
			pBck->ShowWindow( SW_HIDE );
		} // end if
		else
		{	pBck->EnableWindow( TRUE );
			m_bBck = ( pfx->f1 & FXF1_BCK ) != 0;
		} // end else
	} // end if
	
	CWnd *pTrans = GetDlgItem( IDC_TRANSLUCENCY );
	CWnd *pTText = GetDlgItem( IDC_TTEXT );
	{
		if (	pfx->type == FXTYPE_TEXT || 
				pfx->type == FXTYPE_TIME ||
				pfx->type == FXTYPE_DATE ||
				pfx->type == FXTYPE_SCREENSHOT ||
				pfx->type == FXTYPE_VIDEO ||
				pfx->type == FXTYPE_URL )
		{	pTrans->EnableWindow( TRUE );
			pTText->EnableWindow( TRUE );
			char num[ 64 ];
			sprintf( num, "%.2f", ( (double)pfx->translucency ) / ( (double)100 ) );
			m_sTranslucency = num;
		} // end else
		else
		{	pTrans->EnableWindow( FALSE );
			pTrans->ShowWindow( SW_HIDE );
			pTText->EnableWindow( FALSE );
			pTText->ShowWindow( SW_HIDE );
		} // end if
	} // end if
	
	m_colorbox.SetColor( pfx->bck );

	m_bBorder = ( pfx->f1 & FXF1_BORDER ) != 0;
	m_bFoundered = ( pfx->f1 & FXF1_FOUNDERED ) != 0;

	UpdateData( FALSE );

	DoEnable();

	return 0;
}

void CPgRect::OnColor() 
{
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	// Choose a new color
	CWinFile::ChooseColor( &pfx->bck, GetSafeHwnd() );	

	m_colorbox.SetColor( pfx->bck );

	APP()->ReGen();
}

void CPgRect::OnBck() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	// Toggle bit
	if ( m_bBck ) pfx->f1 |= FXF1_BCK;
	else pfx->f1 &= ~FXF1_BCK;

	APP()->ReGen();

	DoEnable();
}
void CPgRect::OnOK() 
{
}
void CPgRect::OnCancel() 
{
}

void CPgRect::OnChangeTranslucency() 
{
	UpdateData( TRUE );

	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	pfx->translucency = (DWORD)( strtod( m_sTranslucency, NULL ) * 100 );
	if ( pfx->translucency > 10000 ) 
	{
		pfx->translucency = 10000;
		m_sTranslucency = "100.00";
		UpdateData( FALSE );
	} // end if

	APP()->ReGen();
}

void CPgRect::OnBorder() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	// Toggle bit
	if ( m_bBorder ) pfx->f1 |= FXF1_BORDER;
	else pfx->f1 &= ~FXF1_BORDER;

	APP()->ReGen();

	DoEnable();
}

void CPgRect::OnFoundered() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	// Toggle bit
	if ( m_bFoundered ) pfx->f1 |= FXF1_FOUNDERED;
	else pfx->f1 &= ~FXF1_FOUNDERED;

	APP()->ReGen();
}

void CPgRect::DoEnable()
{
	UpdateData( TRUE );

	CWnd *pWnd = GetDlgItem( IDC_FOUNDERED );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bBorder );

	pWnd = GetDlgItem( IDC_CHOOSE_COLOR );
	if ( pWnd != NULL ) pWnd->EnableWindow( m_bBck );
}
