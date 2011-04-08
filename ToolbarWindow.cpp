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
// ToolbarWindow.cpp : implementation file
//

#include "stdafx.h"
#include "ToolbarWindow.h"

#include "Winuser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MOUSELEAVE			101

/////////////////////////////////////////////////////////////////////////////
// CToolbarWindow

CToolbarWindow::CToolbarWindow()
{
}

CToolbarWindow::~CToolbarWindow()
{
}


BEGIN_MESSAGE_MAP(CToolbarWindow, CStatic)
	//{{AFX_MSG_MAP(CToolbarWindow)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_MOUSELEAVE, OnNotifyMouseLeave )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolbarWindow message handlers

BOOL CToolbarWindow::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}

void CToolbarWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	RECT rect;
	GetClientRect( &rect );

	// Create offscreen buffer if we can
	HDC hDC = dc.GetSafeHdc();
	if ( m_offscreen.Create( hDC, &rect ) )
		hDC = m_offscreen.GetSafeHdc();

	// Draw the toolbar
	m_wndtoolbar.Draw( GetSafeHwnd(), hDC, &rect );

	// Blt to screen
	if ( m_offscreen.GetSafeHdc() != NULL )
		m_offscreen.Draw( dc.GetSafeHdc(), &rect );
	
}

void CToolbarWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ( m_wndtoolbar.OnLButtonDown( nFlags, &point ) )
		RedrawWindow();
	
	CStatic::OnLButtonDown(nFlags, point);
}

void CToolbarWindow::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ( m_wndtoolbar.OnLButtonUp( nFlags, &point ) )
		RedrawWindow();
	
	CStatic::OnLButtonUp(nFlags, point);
}

void CToolbarWindow::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( m_wndtoolbar.OnMouseMove( nFlags, &point ) )
		RedrawWindow();

//  This function sucks, don't ever use it!!!
//	TRACKMOUSEEVENT	tme;
//	tme.cbSize = sizeof( tme );
//	tme.dwFlags = TME_LEAVE;
//	tme.hwndTrack = GetSafeHwnd();
//	_TrackMouseEvent( &tme );

	// Watch for mouse to leave
	m_uTimerId = SetTimer( MOUSELEAVE, 250, NULL );
	
	CStatic::OnMouseMove(nFlags, point);
}

LRESULT CToolbarWindow::OnNotifyMouseLeave( WPARAM wParam, LPARAM lParam )
{
	// Clear effects
	m_wndtoolbar.ClearMouseOver();
	RedrawWindow();

	return 0;
}

void CToolbarWindow::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	RedrawWindow();	
}

void CToolbarWindow::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent == MOUSELEAVE )
	{
		POINT pt;
		RECT rect;
		GetCursorPos( &pt );
		GetWindowRect( &rect );
		if ( !PtInRect( &rect, pt ) )
		{
			OnNotifyMouseLeave( 0, 0L );
			if ( m_uTimerId != 0 )
			{	KillTimer( m_uTimerId );
				m_uTimerId = 0;
			} // end if
		} // end if

	} // end if
	
	CStatic::OnTimer(nIDEvent);
}
