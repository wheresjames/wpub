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
// WndButton.cpp : implementation file
//

#include "stdafx.h"
#include "WndButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MOUSELEAVE		101

/////////////////////////////////////////////////////////////////////////////
// CWndButton

CWndButton::CWndButton()
{
}

CWndButton::~CWndButton()
{
}


BEGIN_MESSAGE_MAP(CWndButton, CButton)
	//{{AFX_MSG_MAP(CWndButton)
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	
	ON_MESSAGE( WM_MOUSELEAVE, OnMouseLeave )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWndButton message handlers
/*
BOOL CWndButton::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
//	return CButton::OnEraseBkgnd(pDC);
}

void CWndButton::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

/*	RECT rect;
	GetClientRect( &rect );

	char txt[ 256 ];
	GetWindowText( txt, sizeof( txt ) );
	m_winbutton.SetText( txt );

	// Set selected state
	m_winbutton.SetSelected( GetState() & 0x0008 );

	// Draw the button
	m_winbutton.Draw( GetSafeHwnd(), dc.GetSafeHdc(), &rect );

}	
*/

void CWndButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if ( lpDrawItemStruct == NULL ) return;

	// Set button text
	char txt[ 256 ];
	GetWindowText( txt, sizeof( txt ) );
	m_winbutton.SetText( txt );

	// Set check state
//	m_winbutton.SetCheck( lpDrawItemStruct->itemState & ODS_CHECKED );
	m_winbutton.SetDisabled( lpDrawItemStruct->itemState & ODS_DISABLED );
	m_winbutton.SetDefaulted( lpDrawItemStruct->itemState & ODS_DEFAULT );
	m_winbutton.SetPress( lpDrawItemStruct->itemState & ODS_SELECTED );
	m_winbutton.SetFocus( lpDrawItemStruct->itemState & ODS_FOCUS );

	// Set selected state
	m_winbutton.SetSelected( GetState() & 0x0008 );

	m_winbutton.Draw(	lpDrawItemStruct->hwndItem, 
						lpDrawItemStruct->hDC, 
						&lpDrawItemStruct->rcItem );	
}


void CWndButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( m_winbutton.OnMouseMove( nFlags, &point ) )
		RedrawWindow();
	
	// Track mouse leave
//	TRACKMOUSEEVENT	tme;
//	tme.cbSize = sizeof( tme );
//	tme.dwFlags = TME_LEAVE;
//	tme.hwndTrack = GetSafeHwnd();
//	_TrackMouseEvent( &tme );

	// Watch for mouse to leave
	m_uTimerId = SetTimer( MOUSELEAVE, 250, NULL );

	CButton::OnMouseMove(nFlags, point);
}

LRESULT CWndButton::OnMouseLeave( WPARAM wParam, LPARAM lParam )
{
	m_winbutton.SetHot( FALSE );
	RedrawWindow();
	return 0;
}


BOOL CWndButton::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
//	return CButton::OnEraseBkgnd(pDC);
}

void CWndButton::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent == MOUSELEAVE )
	{
		POINT pt;
		RECT rect;
		GetCursorPos( &pt );
		GetWindowRect( &rect );
		if ( !PtInRect( &rect, pt ) )
		{
			OnMouseLeave( 0, 0L );
			if ( m_uTimerId != 0 )
			{	KillTimer( m_uTimerId );
				m_uTimerId = 0;
			} // end if
		} // end if

	} // end if
	
	CButton::OnTimer(nIDEvent);
}
