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
// PgText.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgText.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgText dialog


CPgText::CPgText(CWnd* pParent /*=NULL*/)
	: CDialog(CPgText::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgText)
	m_sText = _T("");
	m_sEscapement = _T("");
	m_sFontWidth = _T("");
	m_sFontStr = _T("");
	m_sShadow = _T("");
	m_bAutoSize = FALSE;
	//}}AFX_DATA_INIT
}


void CPgText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgText)
	DDX_Control(pDX, IDC_COLORBOX, m_colorbox);
	DDX_Control(pDX, IDC_FONT, m_sFont);
	DDX_Text(pDX, IDC_TEXT, m_sText);
	DDX_Text(pDX, IDC_ESCAPEMENT, m_sEscapement);
	DDX_Text(pDX, IDC_FONTWIDTH, m_sFontWidth);
	DDX_Text(pDX, IDC_FONTSTR, m_sFontStr);
	DDX_Text(pDX, IDC_SHADOW, m_sShadow);
	DDX_Check(pDX, IDC_AUTOSIZE, m_bAutoSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgText, CDialog)
	//{{AFX_MSG_MAP(CPgText)
	ON_EN_CHANGE(IDC_TEXT, OnChangeText)
	ON_BN_CLICKED(IDC_FONT, OnFont)
	ON_BN_CLICKED(IDC_COLOR, OnColor)
	ON_EN_CHANGE(IDC_ESCAPEMENT, OnChangeEscapement)
	ON_BN_CLICKED(IDC_CHARMAP, OnCharmap)
	ON_EN_CHANGE(IDC_FONTWIDTH, OnChangeFontwidth)
	ON_EN_CHANGE(IDC_SHADOW, OnChangeShadow)
	ON_BN_CLICKED(IDC_AUTOSIZE, OnAutosize)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgText message handlers

BOOL CPgText::OnInitDialog() 
{
	CDialog::OnInitDialog();

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPgText::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	m_sText = "";
	m_sEscapement = "";
	m_sFontWidth = "";
	m_sFontStr = "";

	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return -1; }

	if ( pfx->type != FXTYPE_TEXT )
	{	CWnd *pText = GetDlgItem( IDC_TEXT );
		if ( pText != NULL ) pText->EnableWindow( FALSE );
	} // end if

	ShowFont( pfx );

	char num[ 64 ];

	wsprintf( num, "%li", pfx->lf.lfEscapement );
	m_sEscapement = num;

//	wsprintf( num, "%li", pfx->lf.lfHeight );
//	m_sSize = num;

	wsprintf( num, "%li", pfx->lf.lfWidth );
	m_sFontWidth = num;

	wsprintf( num, "%li", pfx->shadow );
	m_sShadow = num;

	m_sText = pfx->data;
	
	m_bAutoSize = ( pfx->f1 & FXF1_NOAUTOSIZE ) == 0;

	UpdateData( FALSE );

	m_colorbox.SetColor( pfx->rgb );

	return 0;
}

void CPgText::OnChangeText() 
{
	UpdateData( TRUE );

	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	strcpy_sz( pfx->data, m_sText );

	CFx::AdjustRect( pfx );
	APP()->ReGen();
}

BOOL CPgText::ShowFont( LPFX pfx )
{
	char show[ 256 ];
	wsprintf( show, "   %s", pfx->lf.lfFaceName );
	m_sFontStr = show;
	return TRUE;
}

void CPgText::OnFont() 
{
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	long escapement = pfx->lf.lfEscapement;
//	pfx->lf.lfHeight = 24;
//	pfx->lf.lfEscapement = 0;

	LOGFONT	lf;
	memcpy( &lf, &pfx->lf, sizeof( lf ) );

	CHOOSEFONT	cf;
	ZeroMemory( &cf, sizeof( cf ) );

	cf.lStructSize = sizeof( cf );
	cf.hwndOwner = GetSafeHwnd();
	cf.lpLogFont = &lf;
	cf.Flags =	CF_EFFECTS |  
				CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
	cf.rgbColors = pfx->rgb;

	// Choose the font
	if ( !ChooseFont( &cf ) ) return;

	// Copy the font
	memcpy( &pfx->lf, &lf, sizeof( lf ) );

	pfx->lf.lfEscapement = escapement;
	if ( cf.rgbColors != RGB( 0, 0, 0 ) ) pfx->rgb = cf.rgbColors;
	m_colorbox.SetColor( pfx->rgb );

	ShowFont( pfx );

	APP()->ReGen();
}

void CPgText::OnColor() 
{
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;

	// Choose a new color
	CWinFile::ChooseColor( &pfx->rgb, GetSafeHwnd() );	

	m_colorbox.SetColor( pfx->rgb );

	APP()->ReGen();
}

void CPgText::OnChangeEscapement() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;
	pfx->lf.lfEscapement = strtol( m_sEscapement, NULL, 10 );
	APP()->ReGen();
}

void CPgText::OnCharmap() 
{
	ShellExecute(	NULL, 
					"open", 
					"charmap.exe",
					NULL,
					NULL, 
					SW_SHOWNORMAL );  	
}
/*
void CPgText::OnChangeFontsize() 
{
	UpdateData( TRUE );
	LPFX pfx = OVERLAY().GetEditFx();
	if ( pfx == NULL ) return;
	pfx->lf.lfHeight = strtol( m_sSize, NULL, 10 );
	OVERLAY().ReGen();
}
*/
void CPgText::OnChangeFontwidth() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;
	pfx->lf.lfWidth = strtol( m_sFontWidth, NULL, 10 );
	APP()->ReGen();
}

void CPgText::OnChangeShadow() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) return;
	pfx->shadow = strtol( m_sShadow, NULL, 10 );
	if ( pfx->shadow > 200 )
	{	pfx->shadow = 200;
		m_sShadow = "200";
		UpdateData( FALSE );
	} // end if
	else if ( pfx->shadow < -200 )
	{	pfx->shadow = -200;
		m_sShadow = "-200";
		UpdateData( FALSE );
	} // end if
	APP()->ReGen();
}

void CPgText::OnAutosize() 
{
	UpdateData( TRUE );
	LPFX pfx = EDIT().GetEditFx();
	if ( pfx == NULL ) { UpdateData( FALSE ); return; }
	if ( m_bAutoSize ) pfx->f1 &= ~FXF1_NOAUTOSIZE;
	else pfx->f1 |= FXF1_NOAUTOSIZE;
	APP()->ReGen();
}

void CPgText::OnOK() 
{
}
void CPgText::OnCancel() 
{
}

