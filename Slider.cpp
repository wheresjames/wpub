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
// Slider.cpp : implementation file
//

#include "stdafx.h"
#include "Slider.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSlider

CSlider::CSlider()
{
	m_hMsgWnd = NULL;
	m_uMsg = 0;
}

CSlider::~CSlider()
{
}


BEGIN_MESSAGE_MAP(CSlider, CSliderCtrl)
	//{{AFX_MSG_MAP(CSlider)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlider message handlers

void CSlider::SetDword(DWORD dwPos)
{
	char text[ 256 ];

	SetPos( dwPos );

	sprintf( text, "%lu", (ULONG) dwPos );

	m_sText = text;
}

void CSlider::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateText();
	CSliderCtrl::OnHScroll(nSBCode, nPos, pScrollBar);

	if ( ::IsWindow( m_hMsgWnd ) )
		::SendMessage( m_hMsgWnd, m_uMsg, GetDlgCtrlID(), nPos );
}

void CSlider::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateText();
	CSliderCtrl::OnVScroll(nSBCode, nPos, pScrollBar);

	if ( ::IsWindow( m_hMsgWnd ) )
		::SendMessage( m_hMsgWnd, m_uMsg, GetDlgCtrlID(), nPos );
}

void CSlider::UpdateText()
{
	char text[ 256 ];
	sprintf( text, "%lu", (ULONG) GetPos() );
	m_sText = text;	
}
