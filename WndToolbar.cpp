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
// WndToolbar.cpp : implementation file
//

#include "stdafx.h"
#include "WndToolbar.h"
#include "WinMenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWndToolbar

CWndToolbar::CWndToolbar()
{
	m_lWidth = 0;
	m_lHeight = 26;
	m_pmiover = NULL;
	m_bClick = FALSE;
	m_clickitem = NULL;

	m_hWnd = NULL;
	m_uMsg = 0;

	m_rgbBck = GetSysColor( COLOR_3DFACE );

	ZeroMemory( &m_rect, sizeof( m_rect ) );
}

CWndToolbar::~CWndToolbar()
{	
}

long CWndToolbar::GetWidth()
{
	LPMITEMINFO pmi = NULL;

	m_lWidth = 0;
	while ( ( pmi = (LPMITEMINFO)m_mitems.GetNext( pmi ) ) != NULL )
	{
		if ( pmi->icon == NULL ) m_lWidth += 2;
		else m_lWidth += m_lHeight + 2;

	} // end while

	// Minimum width
	if ( m_lWidth < m_lHeight ) m_lWidth = m_lHeight;

	return m_lWidth; 
}

LPMITEMINFO CWndToolbar::AddItem(LPCTSTR pName, DWORD dwCmd, HICON hIcon, HCURSOR hCursor, LPBOOL pBool, LPCTSTR pToolTip)
{
	LPMITEMINFO	node = (LPMITEMINFO)m_mitems.New( NULL, 0, pName );
	if ( node == NULL ) return NULL;

	// Save item information
	if ( pName != NULL ) { strcpy_sz( node->name, pName ); }
	node->cmd = dwCmd;
	node->icon = hIcon;
	node->b = pBool;
	if ( pToolTip != NULL ) { strcpy_sz( node->tip, pToolTip ); }

	// Is this a sub menu?
	if ( dwCmd == MAXDWORD ) node->submenu = new CWinMenu();
	else node->submenu = NULL;

	return node;
}

BOOL CWndToolbar::Draw( HWND hWnd, HDC hDC, LPRECT pRect)
{
	// Sanity check
	if ( hDC == NULL || pRect == NULL ) return FALSE;

	// Create tooltips
	if ( !m_tooltip.IsTip() ) CreateToolTips( hWnd, pRect );

	// Fill in the background
	CGrDC::FillSolidRect( hDC, pRect, GetSysColor( COLOR_3DFACE ) );

	RECT rect;
	CopyRect( &rect, pRect );
	InflateRect( &rect, -4, -2 );	

	long cy = GetHeight();
	long cx = cy;

	LPMITEMINFO	pmi = NULL;

	// Remember our last rect
	CopyRect( &m_rect, &rect );

	// Horizontal or vertical?
	if ( RW( rect ) > RH( rect ) )
	{
		long y = rect.top;
		for ( long x = rect.left; x < rect.right; x += cx + 2 )
		{
			// Any more items?
			pmi = (LPMITEMINFO)m_mitems.GetNext( pmi );
			if ( pmi == NULL ) return TRUE;

			// Is it a separator?
			if ( pmi->icon == NULL ) cx += 2;

			// Draw the button
			else
			{
				// Where to draw button
				RECT button;
				SetRect( &button, x, y, x + cx, y + cy );

				CGrDC::Center( &button, &rect, FALSE );
				
				DrawButton( hWnd, hDC, &button, pmi );

			} // end else

		} // end for

	} // end if

	else
	{
		long x = rect.left;
		for ( long y = rect.top; y < rect.bottom; y += cy + 2 )
		{
			// Any more items?
			pmi = (LPMITEMINFO)m_mitems.GetNext( pmi );
			if ( pmi == NULL ) return TRUE;

			// Where to draw button
			RECT button;
			SetRect( &button, x, y, x + cx, y + cy );

			// Check for separator
			if ( pmi->icon == NULL ) cy += 2;

			// Draw the button
			else DrawButton( hWnd, hDC, &button, pmi );

		} // end for
	} // end else

	return TRUE;
}

BOOL CWndToolbar::DrawButton( HWND hWnd, HDC hDC, LPRECT pRect, LPMITEMINFO pmi)
{
	if ( CXpTheme::IsWindowThemed( hWnd ) )
	{
		CXpTheme xpthm( hWnd, "TOOLBAR" );

		int state = TS_NORMAL;

		// Determine button state
		if ( pmi->b != NULL && *pmi->b ) state = TS_PRESSED;
		else if ( pmi == m_pmiover ) 
		{	if ( m_bClick ) { if ( pmi == m_clickitem ) state = TS_PRESSED; }
			else state = TS_HOT;
		} // end if

		// Draw the button
		xpthm.DrawThemeBackground( hDC, TP_BUTTON, state, pRect, NULL );

		RECT rect;
		CopyRect( &rect, pRect );
		if ( pmi == m_pmiover && m_bClick ) OffsetRect( &rect, 1, 1 );

		CGrDC::DrawIcon( hDC, &rect, pmi->icon );

	} // end if
	else
	{
		UINT state = 0;

		// Determine button state
		if ( pmi->b != NULL && *pmi->b ) state = 1;
		if ( pmi == m_pmiover ) 
		{	if ( m_bClick ) { if ( pmi == m_clickitem ) state = 1; }
			else state = 2;
		} // end if		

		// Draw the button
		if ( state == 0 );
//			CGrDC::Box3d(	hDC, pRect, 2, 
//							CGrDC::ScaleColor( m_rgbBck, 120 ),
//							CGrDC::ScaleColor( m_rgbBck, -120 ),
//							FALSE );

		else if ( state == 1 ) CGrDC::Box3d(	hDC, pRect, 2, 
												CGrDC::ScaleColor( m_rgbBck, 120 ),
												CGrDC::ScaleColor( m_rgbBck, -120 ),
												TRUE );

		else if ( state == 2 ) CGrDC::Box3d(	hDC, pRect, 2, 
//												CGrDC::ScaleColor( m_rgbBck, 120 ),
//												CGrDC::ScaleColor( m_rgbBck, -120 ),
												CGrDC::ScaleColor( GetSysColor( COLOR_ACTIVECAPTION ), 160 ),
												CGrDC::ScaleColor( GetSysColor( COLOR_ACTIVECAPTION ), -160 ),
												FALSE );
		RECT rect;
		CopyRect( &rect, pRect );
		if ( pmi == m_pmiover && m_bClick ) OffsetRect( &rect, 1, 1 );

		CGrDC::DrawIcon( hDC, &rect, pmi->icon );

	} // end else

	return TRUE;
}

LPMITEMINFO CWndToolbar::HitTest(LPPOINT pt)
{
	long			cy = GetHeight();
	long			cx = cy;
	LPMITEMINFO		pmi = NULL;
	LPRECT			pRect = &m_rect;

	// Horizontal or vertical?
	if ( PRW( pRect ) > PRH( pRect ) )
	{
		long y = pRect->top;
		for ( long x = pRect->left; x < pRect->right; x += cx + 2 )
		{
			// Any more items?
			pmi = (LPMITEMINFO)m_mitems.GetNext( pmi );
			if ( pmi == NULL ) return NULL;

			// Where to draw button
			RECT button;
			SetRect( &button, x, y, x + cx, y + cy );

			// Is this item hot
			if ( pmi->icon != NULL && PtInRect( &button, *pt ) )
				return pmi;

		} // end for

	} // end if

	else
	{
		long x = pRect->left;
		for ( long y = pRect->top; y < pRect->bottom; y += cy + 2 )
		{
			// Any more items?
			pmi = (LPMITEMINFO)m_mitems.GetNext( pmi );
			if ( pmi == NULL ) return NULL;

			// Where to draw button
			RECT button;
			SetRect( &button, x, y, x + cx, y + cy );

			// Is this item hot
			if ( pmi->icon != NULL && PtInRect( &button, *pt ) )
				return pmi;

		} // end for
	} // end else
	return NULL;
}


BOOL CWndToolbar::OnMouseMove(UINT nFlags, LPPOINT pt)
{
	BOOL bRedraw = FALSE;
	LPMITEMINFO pmi = HitTest( pt );

	// Redraw if clicked state changed
	if ( ( ( nFlags & MK_LBUTTON ) != 0 ) != m_bClick )
		bRedraw = TRUE;
	
	// Save click state
	m_bClick = ( ( nFlags & MK_LBUTTON ) != 0 );
	if ( !m_bClick ) m_clickitem = NULL;

	if ( pmi != m_pmiover )
	{	m_pmiover = pmi;
		return TRUE;
	} // end if

	return bRedraw;
}

BOOL CWndToolbar::OnLButtonDown(UINT nFlags, LPPOINT pt)
{
	m_clickitem = HitTest( pt );
	if ( m_clickitem != NULL ) m_bClick = TRUE;

	return ( m_clickitem != NULL );
}

BOOL CWndToolbar::OnLButtonUp( UINT nFlags, LPPOINT pt)
{
	m_bClick = FALSE;
	
	// Process click
	LPMITEMINFO pmi = HitTest( pt );
	if ( pmi != NULL && pmi == m_clickitem )
	{
		// Toggle bool value if needed
		if ( pmi->b != NULL ) *pmi->b = !( *pmi->b );

		// Send command
		if ( pmi->cmd != 0 ) SendCommand( pmi->cmd, 0 );

	} // end if	

	// No more click item
	m_clickitem = NULL;

	return TRUE;
}

BOOL CWndToolbar::CreateToolTips( HWND hWnd, LPRECT pRect )
{
	long cy = GetHeight();
	long cx = cy;

	// Create tool tip window
	m_tooltip.Create( hWnd, CWin32::GetInstanceHandle() );

	LPMITEMINFO	pmi = NULL;

	// Horizontal or vertical?
	if ( PRW( pRect ) > PRH( pRect ) )
	{
		long y = pRect->top;
		for ( long x = pRect->left; x < pRect->right; x += cx + 2 )
		{
			// Any more items?
			pmi = (LPMITEMINFO)m_mitems.GetNext( pmi );
			if ( pmi == NULL ) return TRUE;

			// Where to draw button
			RECT button;
			SetRect( &button, x, y, x + cx, y + cy );

			// Is there a tip?
			if ( pmi->icon != NULL && *pmi->tip ) 
				m_tooltip.Add( &button, pmi->tip );

		} // end for

	} // end if

	else
	{
		long x = pRect->left;
		for ( long y = pRect->top; y < pRect->bottom; y += cy + 2 )
		{
			// Any more items?
			pmi = (LPMITEMINFO)m_mitems.GetNext( pmi );
			if ( pmi == NULL ) return TRUE;

			// Where to draw button
			RECT button;
			SetRect( &button, x, y, x + cx, y + cy );

			// Is there a tip?
			if ( pmi->icon != NULL && *pmi->tip ) 
				m_tooltip.Add( &button, pmi->tip );

		} // end for
	} // end else

	return TRUE;
}
