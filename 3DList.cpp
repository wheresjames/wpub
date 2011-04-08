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
// 3DList.cpp : implementation file
//

#include "stdafx.h"
#include "3DList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_DBLCLICKED	WM_USER + 502

/////////////////////////////////////////////////////////////////////////////
// C3DList

C3DList::C3DList()
{
	m_bInitialize = TRUE;

	m_hContextMenu = NULL;
}

C3DList::~C3DList()
{
	m_bInitialize = TRUE;
}


BEGIN_MESSAGE_MAP(C3DList, CStatic)
	//{{AFX_MSG_MAP(C3DList)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3DList message handlers

BOOL C3DList::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}

void C3DList::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	UpdateVScrollBar();
	UpdateHScrollBar();

	RECT	rect;
	GetClientRect( &rect );
	rect.right -= GetSystemMetrics( SM_CXVSCROLL );

	// No bottom scroll bar if autosizing header
	if ( !GetListCtrl().GetHeader().IsAutosizing() )
		rect.bottom -= GetSystemMetrics( SM_CYHSCROLL );

	// Get a dc in which to draw
	HDC hDC = dc.GetSafeHdc();
	if ( m_offscreen.Create( hDC, &rect ) )
		hDC = m_offscreen.GetSafeHdc();
		
	// Draw the list
	if ( !m_list.Draw( hDC, &rect, GetSafeHwnd() ) )
		CGrDC::FillSolidRect( hDC, &rect, RGB( 128, 128, 128 ) );

	// Draw to the screen
	m_offscreen.Draw( dc.GetSafeHdc(), &rect );
}


void C3DList::Initialize()
{
	m_bInitialize = FALSE;

	// Set clip children style
	DWORD style = GetWindowLong( GetSafeHwnd(), GWL_STYLE );
	style |= WS_CLIPCHILDREN;
	SetWindowLong( GetSafeHwnd(), GWL_STYLE, style );

	// Create the scroll bar
	RECT rect;
	GetClientRect( &rect );
	rect.left = rect.right - GetSystemMetrics( SM_CXVSCROLL );
	m_scrollbar.Create(	SBS_RIGHTALIGN | SBS_VERT,
//						SBS_SIZEBOXBOTTOMRIGHTALIGN | SBS_SIZEBOX,
						rect, this, 0 );
	m_scrollbar.ShowWindow( SW_SHOWNORMAL );

	// Create the horizontal scroll bar
	GetClientRect( &rect );
	rect.top = rect.bottom - GetSystemMetrics( SM_CYHSCROLL );
	rect.right -= GetSystemMetrics( SM_CXVSCROLL );
	m_hscroll.Create(	SBS_RIGHTALIGN | SBS_HORZ,
//							SBS_SIZEBOXBOTTOMRIGHTALIGN | SBS_SIZEBOX,
						rect, this, 0 );

	// Hide or show scroll bar
	if ( !GetListCtrl().GetHeader().IsAutosizing() )
		m_hscroll.ShowWindow( SW_SHOWNORMAL );
	else m_hscroll.ShowWindow( SW_HIDE );

	Size();

}


void C3DList::UpdateVScrollBar()
{
	if ( m_bInitialize ) Initialize();
	if ( !IsWindow( m_scrollbar.GetSafeHwnd() ) ) return;

	RECT	rect;
	GetClientRect( &rect );

	// Get scroll range info
	DWORD mr = m_list.GetScrollLineMax( &rect );
	DWORD pg = m_list.GetLinesPerPage( &rect );
	DWORD cl = m_list.GetCurLine();
	DWORD sz = m_list.GetUncollapsedLines();
	if ( sz ) sz--;

	if ( mr > 0 )
	{
		// Reset current line if needed
		if ( cl > mr ) m_list.SetCurLine( cl = mr );

		SCROLLINFO info; ZeroMemory( &info, sizeof( info ) );
		info.cbSize = sizeof( SCROLLINFO );
		info.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
		info.nMin = 0;
		info.nMax = sz;
		info.nPage = pg; //( sz > mr ) ? sz - mr : 0;
		info.nPos = cl;

		// Set the scroll bar info
		m_scrollbar.SetScrollInfo( &info );

	} // end if

	// Enable / disable window
	m_scrollbar.EnableWindow( ( mr != 0 ) );
}

void C3DList::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();

	RECT rect, hs, vs;
	GetClientRect( &rect );

	m_scrollbar.GetClientRect( &vs );
	rect.right -= RW( vs );

	// If horizontal scroll bar
	if ( !GetListCtrl().GetHeader().IsAutosizing() )
	{
		m_hscroll.GetClientRect( &hs );
		rect.bottom -= RH( hs );
	} // end if

	// Let list handle button press
	if ( m_list.OnLButtonDown( nFlags, &point, &rect ) )
	{
		RECT rect;
		GetClientRect( &rect );
		rect.right -= GetSystemMetrics( SM_CXVSCROLL );			
		rect.bottom -= GetSystemMetrics( SM_CYHSCROLL );
		RedrawWindow( &rect );
		UpdateVScrollBar();
	} // end if
	
	CStatic::OnLButtonDown(nFlags, point);
}

void C3DList::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();

	m_list.OnLButtonUp();  

	UpdateHScrollBar();
	
	CStatic::OnLButtonUp(nFlags, point);
}

void C3DList::OnMouseMove(UINT nFlags, CPoint point) 
{
	RECT rect;
	GetClientRect( &rect );

	// Let list handle mouse move
	if ( m_list.OnMouseMove( nFlags, &point, &rect ) )
	{
		RECT rect;
		GetClientRect( &rect );
		rect.right -= GetSystemMetrics( SM_CXVSCROLL );			
		rect.bottom -= GetSystemMetrics( SM_CYHSCROLL );
		RedrawWindow( &rect );
	} // end if
	
	// Set the correct cursor
	SetCursor( m_list.GetCursor( &rect, &point ) );

//	CStatic::OnMouseMove(nFlags, point);
}

void C3DList::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	HWND hParent = ::GetParent( GetSafeHwnd() );
	if ( hParent != NULL ) ::SendMessage( hParent, WM_DBLCLICKED, 0, 0L );

	CStatic::OnLButtonDblClk(nFlags, point);
}

void C3DList::Size()
{
	// Must have window
	if ( !IsWindow( GetSafeHwnd() ) ) return;

	RECT	rect;
	GetClientRect( &rect );

	UpdateVScrollBar();
	UpdateHScrollBar();

	// Set scroll bar position
	rect.left = rect.right - GetSystemMetrics( SM_CXVSCROLL );
	if ( IsWindow( m_scrollbar.GetSafeHwnd() ) )
	{
		m_scrollbar.SetWindowPos(	NULL, rect.left, rect.top,
									rect.right - rect.left,
									rect.bottom - rect.top,
									SWP_NOZORDER | SWP_NOACTIVATE );		

		m_scrollbar.RedrawWindow();

		// Validate vscroll
		ValidateRect( &rect );

	} // end if

	// Set scroll bar position
	GetClientRect( &rect );
	rect.top = rect.bottom - GetSystemMetrics( SM_CYHSCROLL );
	rect.right -= GetSystemMetrics( SM_CXVSCROLL );
	if ( IsWindow( m_hscroll.GetSafeHwnd() ) )
	{
		m_hscroll.SetWindowPos(	NULL, rect.left, rect.top,
								rect.right - rect.left,
								rect.bottom - rect.top,
								SWP_NOZORDER | SWP_NOACTIVATE );		

		m_hscroll.RedrawWindow();

		// Hide or show scroll bar
		if ( !GetListCtrl().GetHeader().IsAutosizing() )
		{
			// Ensure visible
			m_hscroll.ShowWindow( SW_SHOWNORMAL );

			// Validate hscroll
			ValidateRect( &rect );

		} // end if
		else m_hscroll.ShowWindow( SW_HIDE );

		
	} // end if

	RedrawWindow();
}

void C3DList::OnDestroy() 
{
	CStatic::OnDestroy();
	
	m_bInitialize = TRUE;
	
}


void C3DList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if ( pScrollBar != NULL )
	{
		RECT rect;
		GetClientRect( &rect );
		DWORD lpp = m_list.GetLinesPerPage( &rect );
		int pos = pScrollBar->GetScrollPos();
		int min, max;
		pScrollBar->GetScrollRange( &min, &max );

		switch( nSBCode )
		{
			case SB_BOTTOM :
				pScrollBar->SetScrollPos( max );
				break;

			case SB_LINEDOWN :
				pScrollBar->SetScrollPos( pos + 1 );
				break;

			case SB_LINEUP :
				pScrollBar->SetScrollPos( pos - 1 );
				break;

			case SB_PAGEDOWN :
				pScrollBar->SetScrollPos( pos + lpp );
				break;

			case SB_PAGEUP :
				pScrollBar->SetScrollPos( pos - lpp );
				break;

			case SB_THUMBPOSITION :
				pScrollBar->SetScrollPos( nPos );
				break;

			case SB_THUMBTRACK :
				pScrollBar->SetScrollPos( nPos );
				break;

			case SB_TOP :
				pScrollBar->SetScrollPos( min );
				break;

		} // end switch

		if ( pScrollBar == &m_scrollbar )
		{
			m_list.SetCurLine( pScrollBar->GetScrollPos() );

			RECT rect;
			GetClientRect( &rect );
			rect.right -= GetSystemMetrics( SM_CXVSCROLL );			
			RedrawWindow( &rect );
		} // end if

	} // end if	
	
	CStatic::OnVScroll(nSBCode, nPos, pScrollBar);
}

void C3DList::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if ( pScrollBar != NULL )
	{
		RECT rect;
		GetClientRect( &rect );
		int pos = pScrollBar->GetScrollPos();
		int min, max;
		pScrollBar->GetScrollRange( &min, &max );

		switch( nSBCode )
		{
			case SB_BOTTOM :
				pScrollBar->SetScrollPos( max );
				break;

			case SB_LINEDOWN :
				pScrollBar->SetScrollPos( pos + 1 );
				break;

			case SB_LINEUP :
				pScrollBar->SetScrollPos( pos - 1 );
				break;

			case SB_PAGEDOWN :
				pScrollBar->SetScrollPos( pos + ( rect.right - rect.left ) );
				break;

			case SB_PAGEUP :
				pScrollBar->SetScrollPos( pos - ( rect.right - rect.left ) );
				break;

			case SB_THUMBPOSITION :
				pScrollBar->SetScrollPos( nPos );
				break;

			case SB_THUMBTRACK :
				pScrollBar->SetScrollPos( nPos );
				break;

			case SB_TOP :
				pScrollBar->SetScrollPos( min );
				break;

		} // end switch

		if ( pScrollBar == &m_hscroll )
		{
			m_list.SetHScroll( pScrollBar->GetScrollPos() );

			RECT rect;
			GetClientRect( &rect );
			rect.right -= GetSystemMetrics( SM_CXVSCROLL );			
			rect.bottom -= GetSystemMetrics( SM_CYHSCROLL );
			RedrawWindow( &rect );

		} // end if

	} // end if	
	
	CStatic::OnHScroll(nSBCode, nPos, pScrollBar);
}

void C3DList::UpdateHScrollBar()
{
	if ( !IsWindow( m_hscroll.GetSafeHwnd() ) ) return;

	RECT	rect;
	GetClientRect( &rect );

	long	width = rect.right - rect.left - GetSystemMetrics( SM_CXVSCROLL );
	long	ms = m_list.GetTotalWidth();
	long	cs = m_list.GetHScroll();

	// Calc max scroll
	if ( width > ms ) ms = 0; else ms -= width;

	// Reset current line if needed
	if ( cs > ms )
	{
		m_list.SetHScroll( cs = ms );
		RedrawWindow();
	} // end if

	// Set scroll range
//	m_hscroll.SetScrollRange( 0, ms, FALSE );
//	m_hscroll.SetScrollPos( cs, TRUE );

	if ( ms > 0 ) 
	{

		SCROLLINFO info; ZeroMemory( &info, sizeof( info ) );
		info.cbSize = sizeof( SCROLLINFO );
		info.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
		info.nMin = 0;
		info.nMax = ms;
		info.nPage = width; //( sz > mr ) ? sz - mr : 0;
		info.nPos = cs;

		// Set the scroll bar info
		m_hscroll.SetScrollInfo( &info );

	} // end if


	// Enable/disable scroll bar
	m_hscroll.EnableWindow( ( ms != 0 ) );
}

void C3DList::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);

	Size();	
}


void C3DList::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// Send right click message if needed
	if ( m_hContextMenu != NULL )
		::PostMessage(	m_hContextMenu, WM_RBUTTONDOWN, (WPARAM)nFlags, 
						(LPARAM)MAKELONG( point.x, point.y ) );
	
	CStatic::OnRButtonDown(nFlags, point);
}
