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
// DlgJpegSettings.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgJpegSettings.h"

#include "DlgTreeFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgJpegSettings dialog


CDlgJpegSettings::CDlgJpegSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgJpegSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgJpegSettings)
	m_sQuality = _T("");
	//}}AFX_DATA_INIT
}


void CDlgJpegSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgJpegSettings)
	DDX_Control(pDX, IDC_SLIDEQUALITY, m_slideQuality);
	DDX_Text(pDX, IDC_QUALITY, m_sQuality);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgJpegSettings, CDialog)
	//{{AFX_MSG_MAP(CDlgJpegSettings)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_QUALITY, OnChangeQuality)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_SAVE, OnSave )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgJpegSettings message handlers

void CDlgJpegSettings::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
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
	
	if ( (CSliderCtrl*)pScrollBar == &m_slideQuality )
	{
		if ( nSBCode == SB_ENDSCROLL )
			pos = m_slideQuality.GetPos();
		else m_slideQuality.SetPos( pos );

		char num[ 64 ];
		wsprintf( num, "%lu", ( pos * 100 ) / 255 );
		m_sQuality = num;

		UpdateData( FALSE );

		if ( nSBCode == SB_ENDSCROLL )
		{
			CDlgTreeFrame::EnableApply( GetSafeHwnd() );
		} // end if
	} // end if

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgJpegSettings::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	char szval[ 256 ];
	DWORD dwval = 0;
	CFG().GetValue( "Settings", "JpegQuality", &dwval );
	if ( dwval == 0 ) dwval = DEFJPEGQUALITY;
	wsprintf( szval, "%lu", ( dwval * 100 ) / 255 );
	m_sQuality = szval;
	m_slideQuality.SetPos( dwval );

	UpdateData( FALSE );
}

void CDlgJpegSettings::OnChangeQuality() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
	UpdateData( TRUE );

	DWORD dwval = ( strtoul( m_sQuality, NULL, 10 ) * 255 ) / 100;
	if ( dwval > 255 ) dwval = 255;
	m_slideQuality.SetPos( dwval );

}

BOOL CDlgJpegSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_slideQuality.SetRange( 0, 255 );

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgJpegSettings::OnSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );
	
	DWORD dwval = ( strtoul( m_sQuality, NULL, 10 ) * 255 ) / 100;
	if ( dwval > 255 ) dwval = 255;
	else if ( dwval == 0 ) dwval = DEFJPEGQUALITY;
	CFG().SetValue( "Settings", "JpegQuality", dwval );

	return 0;
}
