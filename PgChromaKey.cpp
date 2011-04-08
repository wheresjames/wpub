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
// PgChromaKey.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgChromaKey.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgChromaKey dialog


CPgChromaKey::CPgChromaKey(CWnd* pParent /*=NULL*/)
	: CDialog(CPgChromaKey::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgChromaKey)
	m_bEnable = FALSE;
	m_sNegative = _T("");
	m_sPositive = _T("");
	m_bNoSmoothing = FALSE;
	//}}AFX_DATA_INIT
}


void CPgChromaKey::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgChromaKey)
	DDX_Control(pDX, IDC_COLORBOX, m_colorbox);
	DDX_Control(pDX, IDC_SLIDEPOSITIVE, m_slidePositive);
	DDX_Control(pDX, IDC_SLIDENEGATIVE, m_slideNegative);
	DDX_Check(pDX, IDC_ENABLE, m_bEnable);
	DDX_Text(pDX, IDC_NEGATIVE, m_sNegative);
	DDX_Text(pDX, IDC_POSITIVE, m_sPositive);
	DDX_Check(pDX, IDC_NOSMOOTHING, m_bNoSmoothing);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgChromaKey, CDialog)
	//{{AFX_MSG_MAP(CPgChromaKey)
	ON_BN_CLICKED(IDC_ENABLE, OnEnable)
	ON_BN_CLICKED(IDC_CHOOSE_COLOR, OnChooseColor)
	ON_EN_CHANGE(IDC_NEGATIVE, OnChangeNegative)
	ON_EN_CHANGE(IDC_POSITIVE, OnChangePositive)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_NOSMOOTHING, OnNosmoothing)
	//}}AFX_MSG_MAP
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgChromaKey message handlers

BOOL CPgChromaKey::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_slidePositive.SetRange( 0, 255 );
	m_slideNegative.SetRange( 0, 255 );

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgChromaKey::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) { DoEnable(); return FALSE; }

	char num[ 256 ];

	wsprintf( num, "%lu", ppii->cpos );
	m_sPositive = num;
	m_slidePositive.SetPos( ppii->cpos );

	wsprintf( num, "%lu", ppii->cneg );
	m_sNegative = num;
	m_slideNegative.SetPos( ppii->cneg );

	m_colorbox.SetColor( ppii->rgbkey );

	// Set the flags value
	m_bEnable = ( ( ppii->f1 & PIIF1_CHROMAKEY ) != 0 ) ? TRUE : FALSE;	
	m_bNoSmoothing = ( ( ppii->f1 & PIIF1_NOSMOOTHING ) != 0 ) ? TRUE : FALSE;	

	UpdateData( FALSE );

	DoEnable();

	return 0;
}

void CPgChromaKey::DoEnable()
{
	UpdateData( TRUE );

	CWnd *pCtrl = GetDlgItem( IDC_NOSMOOTHING );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnable );

	pCtrl = GetDlgItem( IDC_NEGATIVE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnable );
	pCtrl = GetDlgItem( IDC_SLIDENEGATIVE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnable );

	pCtrl = GetDlgItem( IDC_POSITIVE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnable );
	pCtrl = GetDlgItem( IDC_SLIDEPOSITIVE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnable );

	pCtrl = GetDlgItem( IDC_COLORBOX );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnable );
	pCtrl = GetDlgItem( IDC_CHOOSE_COLOR );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnable );

	pCtrl = GetDlgItem( IDC_BOX );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnable );
	pCtrl = GetDlgItem( IDC_TXT_POSITIVE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnable );
	pCtrl = GetDlgItem( IDC_TXT_NEGATIVE );
	if ( pCtrl != NULL ) pCtrl->EnableWindow( m_bEnable );
}

void CPgChromaKey::OnEnable() 
{	DoEnable();

	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	// Set/clear flag
	if ( m_bEnable ) ppii->f1 |= PIIF1_CHROMAKEY;
	else ppii->f1 &= ~PIIF1_CHROMAKEY;

	APP()->ReGen();
}

void CPgChromaKey::OnChooseColor() 
{
	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	// Choose a new color
	CWinFile::ChooseColor( &ppii->rgbkey, GetSafeHwnd() );	

	// Set color to box
	m_colorbox.SetColor( ppii->rgbkey );

	APP()->ReGen();
}

void CPgChromaKey::OnChangeNegative() 
{
	UpdateData( TRUE );

	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	// Set neg value
	ppii->cneg = strtoul( m_sNegative, NULL, 10 );
	m_slideNegative.SetPos( ppii->cneg );

	APP()->ReGen();
}

void CPgChromaKey::OnChangePositive() 
{
	UpdateData( TRUE );

	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	// Set neg value
	ppii->cpos = strtoul( m_sPositive, NULL, 10 );
	m_slidePositive.SetPos( ppii->cpos );

	APP()->ReGen();
}

void CPgChromaKey::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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

		char num[ 64 ];
		wsprintf( num, "%lu", pos );
		m_sPositive = num;

		UpdateData( FALSE );

		if ( nSBCode == SB_ENDSCROLL )
		{			
			// Get the current image
			LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
			if ( ppii == NULL ) return;

			ppii->cpos = pos;
			APP()->ReGen();
		} // end if
	} // end if

	else if ( (CSlider*)pScrollBar == &m_slideNegative )
	{
		if ( nSBCode == SB_ENDSCROLL )
			pos = m_slideNegative.GetPos();
		else m_slideNegative.SetPos( pos );

		char num[ 64 ];
		wsprintf( num, "%lu", pos );
		m_sNegative = num;

		UpdateData( FALSE );

		if ( nSBCode == SB_ENDSCROLL )
		{			
			// Get the current image
			LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
			if ( ppii == NULL ) return;

			ppii->cneg = pos;
			APP()->ReGen();
		} // end if
	} // end if
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPgChromaKey::OnNosmoothing() 
{
	UpdateData( TRUE );

	// Get the current image
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return;

	// Set/clear flag
	if ( m_bNoSmoothing ) ppii->f1 |= PIIF1_NOSMOOTHING;
	else ppii->f1 &= ~PIIF1_NOSMOOTHING;

	APP()->ReGen();
}
