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
// PgTrans.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgTrans.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_POSITIVE		( WM_USER + 101 )
#define WM_NEGATIVE		( WM_USER + 102 )

/////////////////////////////////////////////////////////////////////////////
// CPgTrans dialog


CPgTrans::CPgTrans(CWnd* pParent /*=NULL*/)
	: CDialog(CPgTrans::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgTrans)
	m_sPositive = _T("");
	m_sNegative = _T("");
	m_bTransparency = FALSE;
	m_bNoSmoothing = FALSE;
	//}}AFX_DATA_INIT
}


void CPgTrans::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgTrans)
	DDX_Control(pDX, IDC_COLORBOX, m_colorbox);
	DDX_Control(pDX, IDC_SLIDENEGATIVE, m_slideNegative);
	DDX_Control(pDX, IDC_SLIDEPOSITIVE, m_slidePositive);
	DDX_Text(pDX, IDC_POSITIVE, m_sPositive);
	DDX_Text(pDX, IDC_NEGATIVE, m_sNegative);
	DDX_Check(pDX, IDC_TRANSPARENCY, m_bTransparency);
	DDX_Check(pDX, IDC_NOSMOOTHING, m_bNoSmoothing);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgTrans, CDialog)
	//{{AFX_MSG_MAP(CPgTrans)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHOOSE_COLOR, OnChooseColor)
	ON_BN_CLICKED(IDC_TRANSPARENCY, OnTransparency)
	ON_EN_CHANGE(IDC_NEGATIVE, OnChangeNegative)
	ON_EN_CHANGE(IDC_POSITIVE, OnChangePositive)
	ON_BN_CLICKED(IDC_NOSMOOTHING, OnNosmoothing)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_POSITIVE, OnPositive )
	ON_MESSAGE( WM_NEGATIVE, OnNegative )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgTrans message handlers

BOOL CPgTrans::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_slidePositive.SetMessageTarget( GetSafeHwnd(), WM_POSITIVE );
	m_slideNegative.SetMessageTarget( GetSafeHwnd(), WM_NEGATIVE );

	m_slidePositive.SetRange( 0, 255 );
	m_slideNegative.SetRange( 0, 255 );

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgTrans::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return -1; }

	char num[ 64 ];

	wsprintf( num, "%lu", pfx->cpos );
	m_sPositive = num;
	m_slidePositive.SetPos( pfx->cpos );

	wsprintf( num, "%lu", pfx->cneg );
	m_sNegative = num;
	m_slideNegative.SetPos( pfx->cneg );
	
	m_bTransparency = ( pfx->f1 & FXF1_TRANSPARENCY ) != 0;

	m_bNoSmoothing = ( pfx->f1 & FXF1_NOSMOOTHING ) != 0;

	m_colorbox.SetColor( pfx->trans );

	UpdateData( FALSE );

	DoEnable();

	return 0;
}

LRESULT CPgTrans::OnPositive(WPARAM wParam, LPARAM lParam)
{
/*	UpdateData( TRUE );
	LPFX pfx = OVERLAY().GetEditFx();
	if ( pfx == NULL ) return;

	m_sPositive = m_slidePositive.m_sText;
	pfx->cpos = strtol( m_sPositive, NULL, 10 );

	OVERLAY().ReGen();
*/
	return 0;
}

LRESULT CPgTrans::OnNegative(WPARAM wParam, LPARAM lParam)
{
/*	UpdateData( TRUE );
	LPFX pfx = OVERLAY().GetEditFx();
	if ( pfx == NULL ) return;

	m_sNegative = m_slideNegative.m_sText;
	pfx->cneg = strtol( m_sNegative, NULL, 10 );

	OVERLAY().ReGen();
*/
	return 0;
}

void CPgTrans::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	long pos = pScrollBar->GetScrollPos();

	int min = 0, max = 100;
//	pScrollBar->GetScrollRange( &min, &max );

	switch( nSBCode )
	{
		case SB_LEFT : pos = min;							break;
		case SB_ENDSCROLL : 
			break;
		case SB_LINELEFT : pos -= 1;						break;
		case SB_LINERIGHT : pos += 1;						break;
		case SB_PAGELEFT : pos -= ( max - min ) / 10;		break;
		case SB_PAGERIGHT : pos += ( max - min ) / 10;		break;
		case SB_RIGHT : pos = max;							break;
		case SB_THUMBPOSITION : pos = long( nPos );			break;
		case SB_THUMBTRACK : pos = long( nPos );			break;
		default : return; break;

	} // end switch
	
	if ( (CSlider*)pScrollBar == &m_slidePositive )
	{
		if ( nSBCode == SB_ENDSCROLL )
			pos = m_slidePositive.GetPos();
		else m_slidePositive.SetPos( pos );

		LPFX pfx = EDIT().GetEditFx();
		if ( pfx == NULL ) return;

		char num[ 64 ];
		wsprintf( num, "%lu", pos );
		m_sPositive = num;

		UpdateData( FALSE );

		if ( nSBCode == SB_ENDSCROLL )
		{			
			pfx->cpos = pos;
			APP()->ReGen();
		} // end if
	} // end if

	else if ( (CSlider*)pScrollBar == &m_slideNegative )
	{
		if ( nSBCode == SB_ENDSCROLL )
			pos = m_slideNegative.GetPos();
		else m_slideNegative.SetPos( pos );

		LPFX pfx = EDIT().GetEditFx();
		if ( pfx == NULL ) return;

		char num[ 64 ];
		wsprintf( num, "%lu", pos );
		m_sNegative = num;

		UpdateData( FALSE );

		if ( nSBCode == SB_ENDSCROLL )
		{			
			pfx->cneg = pos;
			APP()->ReGen();
		} // end if
	} // end if

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPgTrans::OnChooseColor() 
{
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	// Choose a new color
	CWinFile::ChooseColor( &pfx->trans, GetSafeHwnd() );	

	m_colorbox.SetColor( pfx->trans );

	APP()->ReGen();
}

void CPgTrans::OnTransparency() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	if ( m_bTransparency ) pfx->f1 |= FXF1_TRANSPARENCY;
	else pfx->f1 &= ~FXF1_TRANSPARENCY;

	DoEnable();

	APP()->ReGen();
}

void CPgTrans::OnChangeNegative() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	pfx->cneg = strtoul( m_sNegative, NULL, 10 );
	m_slideNegative.SetPos( pfx->cneg );

	APP()->ReGen();
}

void CPgTrans::OnChangePositive() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	pfx->cpos = strtoul( m_sPositive, NULL, 10 );
	m_slidePositive.SetPos( pfx->cpos );

	APP()->ReGen();
}

void CPgTrans::OnNosmoothing() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	if ( m_bNoSmoothing ) pfx->f1 |= FXF1_NOSMOOTHING;
	else pfx->f1 &= ~FXF1_NOSMOOTHING;

	APP()->ReGen();
}

void CPgTrans::DoEnable()
{
	UpdateData( TRUE );

	CWnd *pCtrl = GetDlgItem( IDC_NOSMOOTHING );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bTransparency );

	pCtrl = GetDlgItem( IDC_NEGATIVE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bTransparency );
	pCtrl = GetDlgItem( IDC_SLIDENEGATIVE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bTransparency );

	pCtrl = GetDlgItem( IDC_POSITIVE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bTransparency );
	pCtrl = GetDlgItem( IDC_SLIDEPOSITIVE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bTransparency );

	pCtrl = GetDlgItem( IDC_COLORBOX );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bTransparency );
	pCtrl = GetDlgItem( IDC_CHOOSE_COLOR );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bTransparency );

	pCtrl = GetDlgItem( IDC_TXT_POSITIVE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bTransparency );
	pCtrl = GetDlgItem( IDC_TXT_NEGATIVE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bTransparency );
}
