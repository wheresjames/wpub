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
// DrawSlantToolbar.cpp: implementation of the CDrawSlantToolbar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DrawSlantToolbar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawSlantToolbar::CDrawSlantToolbar()
{
	m_pClicked = NULL;
	m_pMouseOver = NULL;
	ZeroMemory( &m_rect, sizeof( m_rect ) );

	m_hWnd = NULL;
	m_uMsgCmd = 0;
	m_uMsgUpdate = 0;

	m_bTextArea = TRUE;

	*m_szTitle = 0;

	m_lMinTextArea = 20;

	m_bHiLight = FALSE;
	m_bHiLightBtn = FALSE;

	m_lButtonSlant = 8;
	m_bForceTooltips = FALSE;

	m_bCaptionMode = FALSE;

	m_screenx = 0; m_screeny = 0;
}

CDrawSlantToolbar::~CDrawSlantToolbar()
{

}

BOOL CDrawSlantToolbar::Add(UINT uCmd, HICON hIcon, LPCTSTR pText, LPCTSTR pTip, LPBOOL pBool, HICON hPressed, DWORD flags)
{
	// Allocate memory
	LPSLANTTOOLBARINFO psti = (LPSLANTTOOLBARINFO)New();
	if ( psti == NULL ) return FALSE;

	// Copy information
	psti->cmd = uCmd;
	psti->icon = hIcon;
	psti->pressed = hPressed;
	psti->b = pBool;
	psti->f1 = flags;
	if ( pTip == NULL ) *psti->tip = 0;
	else strcpy_sz( psti->tip, pTip );
	if ( pText == NULL ) *psti->txt = 0;
	else strcpy_sz( psti->txt, pText );

	return TRUE;
}

LPSLANTTOOLBARINFO CDrawSlantToolbar::HitTest(LPRECT pRect, LPPOINT pt)
{
	// Sanity check
	if ( pRect == NULL ) return NULL;

	BOOL bVert = RW( m_rect ) < RH( m_rect );

	RECT btn;
	long w = 40, h = 24, iw = 16;
	long x = pRect->left + 4;
	long y = pRect->top + 4;
	long rx = pRect->right - 4;
	long ry = pRect->bottom - 4;

	LPSLANTTOOLBARINFO psti = NULL;

	HICON hFirstIcon = NULL;
	while ( hFirstIcon == NULL &&  ( psti = (LPSLANTTOOLBARINFO)GetNext( psti ) ) != NULL )
		hFirstIcon = psti->icon;
	if ( hFirstIcon != NULL )
	{	RECT icon;
		CGrDC::GetIconRect( hFirstIcon, &icon );

		if ( !bVert )
		{	h = RH( icon ) + 6;
			w = RW( icon ) + ( m_lButtonSlant * 2 ) + 4;
		} // end if
		else
		{	w = RW( icon ) + 6;
			h = RH( icon ) + ( m_lButtonSlant * 2 ) + 4;
		} // end else
	} // end if

	long yextra = PRH( pRect );
	if ( yextra < h ) yextra = 0; else yextra = ( yextra - h ) >> 1;
	long top = pRect->top + yextra;
	long bottom = pRect->bottom - yextra;

	long xextra = PRW( pRect );
	if ( xextra < h ) xextra = 0; else xextra = ( xextra - w ) >> 1;
	long left = pRect->left + xextra;
	long right = pRect->right - xextra;

	POINT	pts[ 4 ];

	// For each button
	psti = NULL;
	while ( ( psti = (LPSLANTTOOLBARINFO)GetNext( psti ) ) != NULL )
	{
		// Check for separator
		if ( psti->icon == NULL )
		{
			// Next button
			if ( !bVert ) x += 4; else y += 4;

		} // end if

		else
		{
			long tw = 0;

			if ( !bVert && *psti->txt != 0 )
			{
				CText	text;
				RECT	trect;

				// Init text rect
				SetRect( &trect, 0, 0, 10, 10 );

				// What font does the user want for menu's?
				NONCLIENTMETRICS info;
				info.cbSize = sizeof(info);			
				SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( info ), &info, 0 );

	//			text.SetWeight( FW_BOLD );
				text.SetFont( &info.lfMenuFont );

				HDC hDC = GetDC( NULL );
				if ( hDC != NULL )
				{
					// Calc text width
					text.CalcRect( hDC, psti->txt, &trect );

					// Release the scren dc
					ReleaseDC( NULL, hDC );

				} // end if

				// Add for text
				tw = RW( trect ) + 6;

			} // end if

			if ( ( psti->f1 & STIF1_RIGHTBTN ) == 0 )
			{
				// Set button rect
				if ( !bVert ) SetRect( &btn, x, top, x + w + tw, bottom );
				else SetRect( &btn, left, y, right, y + h );
			} // end if
			else
			{
				// Set button rect
				if ( !bVert ) SetRect( &btn, rx - w - tw, top, rx, bottom );
				else SetRect( &btn, left, ry - y, right, ry );
			} // end else

			if ( !bVert )
			{	pts[ 0 ].x = btn.left; pts[ 0 ].y = btn.bottom;
				pts[ 1 ].x = btn.left + m_lButtonSlant; pts[ 1 ].y = btn.top;
				pts[ 2 ].x = btn.right; pts[ 2 ].y = btn.top;
				pts[ 3 ].x = btn.right - m_lButtonSlant; pts[ 3 ].y = btn.bottom;
			} // end if
			else
			{	pts[ 0 ].x = btn.left; pts[ 0 ].y = btn.bottom - m_lButtonSlant;
				pts[ 1 ].x = btn.left; pts[ 1 ].y = btn.top;
				pts[ 2 ].x = btn.right; pts[ 2 ].y = btn.top + m_lButtonSlant;
				pts[ 3 ].x = btn.right; pts[ 3 ].y = btn.bottom;
			} // else

			// If there's room
			if (	( !bVert && rx > ( x + RW( btn ) ) ) ||
					( bVert && ry > ( y + RH( btn ) ) )	)
			{

				// Check if point is in button
				HRGN hRgn = CreatePolygonRgn( pts, 4, WINDING );
				BOOL bPtInRgn = PtInRegion( hRgn, pt->x, pt->y );
				::DeleteObject( hRgn );

				// Did user click this button?
				if ( bPtInRgn ) return psti;

			} // end if

			if ( ( psti->f1 & STIF1_RIGHTBTN ) == 0 )
			{
				// Next button
				if ( !bVert ) x += ( w + tw ) - ( m_lButtonSlant - 2 );
				else y += h - ( m_lButtonSlant - 2 );
			} // end if
			else
			{
				// Next button
				if ( !bVert ) rx -= ( w + tw ) - ( m_lButtonSlant - 2 );
				else ry -= h - ( m_lButtonSlant - 2 );
			} // end else

		} // end else

	} // end while

	return NULL;
}

BOOL CDrawSlantToolbar::Draw( HWND hWnd, HDC hDC, LPRECT pRect)
{
	// Sanity check
	if ( hDC == NULL || pRect == NULL ) return FALSE;

	// Kill tips if needed
	if ( !EqualRect( &m_rect, pRect ) ) m_tooltip.Destroy();

	// Save drawing rect
	CopyRect( &m_rect, pRect );

	// Horizontal or vertical?
	BOOL bVert = RW( m_rect ) < RH( m_rect );

	// Do we need to create tips?
	BOOL bCreateTips = ( bVert || !m_bTextArea || m_bForceTooltips ) && hWnd != NULL && !m_tooltip.IsTip();

	// Create tip window
	if ( bCreateTips ) if ( !m_tooltip.Create( hWnd, CWin32::GetInstanceHandle() ) ) bCreateTips = FALSE;

	// Create background if needed
	if ( m_bck.GetSafeHdc() == NULL || m_bck.GetHeight() != PRH( pRect ) )
	{	
		if ( !bVert )
		{
			if ( !m_bck.Create( hDC, 0, 0, 64, PRH( pRect ) ) ) return FALSE;
			CGrDC::VertGradientFill(	m_bck, m_bck, 
										CGrDC::ScaleColor( GetSysColor( COLOR_3DFACE ), 30 ),
										CGrDC::ScaleColor( GetSysColor( COLOR_3DFACE ), -30 ) );
		} // end if
		else 
		{
			if ( !m_bck.Create( hDC, 0, 0, PRW( pRect ), 64 ) ) return FALSE;
			CGrDC::HorzGradientFill(	m_bck, m_bck, 
										CGrDC::ScaleColor( GetSysColor( COLOR_3DFACE ), 30 ),
										CGrDC::ScaleColor( GetSysColor( COLOR_3DFACE ), -30 ) );
		} // end else
//		m_bck.Texturize( 10 );
		m_bck.Aluminize( 5 );
	} // end if

	// Fill in the background
	m_bck.Tile( hDC, pRect );

	// Box it in
	CGrDC::Box3d( hDC, pRect, 1, GetSysColor( COLOR_3DHILIGHT ), GetSysColor( COLOR_3DSHADOW ) );

	RECT btn;
	long w = 40, h = 24, iw = 16;
	long x = pRect->left + 4;
	long y = pRect->top + 4;
	long rx = pRect->right - 4;
	long ry = pRect->bottom - 4;
	LPSLANTTOOLBARINFO psti = NULL;

	HICON hFirstIcon = NULL;
	while ( hFirstIcon == NULL &&  ( psti = (LPSLANTTOOLBARINFO)GetNext( psti ) ) != NULL )
		hFirstIcon = psti->icon;
	if ( hFirstIcon != NULL )
	{	RECT icon;
		CGrDC::GetIconRect( hFirstIcon, &icon );

		if ( !bVert )
		{	h = RH( icon ) + 4;
			w = RW( icon ) + ( m_lButtonSlant * 2 ) + 4;
		} // end if
		else
		{	w = RW( icon ) + 6;
			h = RH( icon ) + ( m_lButtonSlant * 2 ) + 4;
		} // end else
	} // end if

	long yextra = PRH( pRect );
	if ( yextra < h ) yextra = 0; else yextra = ( yextra - h ) >> 1;
	long top = pRect->top + yextra;
	long bottom = pRect->bottom - yextra;

	long xextra = PRW( pRect );
	if ( xextra < w ) xextra = 0; else xextra = ( xextra - w ) >> 1;
	long left = pRect->left + xextra;
	long right = pRect->right - xextra;

	// For each button
	psti = NULL;
	while ( ( psti = (LPSLANTTOOLBARINFO)GetNext( psti ) ) != NULL )
	{
		// Check for separator
		if ( psti->icon == NULL )
		{
			// If there's room
			if (	( !bVert && rx > ( x + RW( btn ) ) ) ||
					( bVert && ry > ( y + RH( btn ) ) )	)
			{
				CGrDC dc( hDC, FALSE );
				CGrPen lt( PS_SOLID, 1, GetSysColor( COLOR_3DHILIGHT ) );
				CGrPen dk( PS_SOLID, 1, GetSysColor( COLOR_3DSHADOW ) );

				long dx = x; if ( ( psti->f1 & STIF1_RIGHTBTN ) != 0 ) dx = rx;
				long dy = y; if ( ( psti->f1 & STIF1_RIGHTBTN ) != 0 ) dy = ry;

				if ( !bVert )
				{
					dc.SelectObject( dk );
					dc.MoveTo( dx + 1, bottom );
					dc.LineTo( dx + 1 + m_lButtonSlant, top );
					dc.SelectObject( lt );
					dc.MoveTo( dx + 2, bottom );
					dc.LineTo( dx + 2 + m_lButtonSlant, top );

				} // end if

				else
				{
					dc.SelectObject( dk );
					dc.MoveTo( left, dy + 1 );
					dc.LineTo( right, dy + 1 + m_lButtonSlant );
					dc.SelectObject( lt );
					dc.MoveTo( left, dy + 2 );
					dc.LineTo( right, dy + 2 + m_lButtonSlant );

				} // end if

			} // end if

			// Next button
			if ( !bVert ) x += 4; else y += 4;

		} // end if

		else
		{	
			long tw = 0;

			if ( !bVert && *psti->txt != 0 )
			{
				CText	text;
				RECT	trect;

				// Init text rect
				SetRect( &trect, 0, 0, 10, 10 );

				// What font does the user want for menu's?
				NONCLIENTMETRICS info;
				info.cbSize = sizeof(info);			
				SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( info ), &info, 0 );

	//			text.SetWeight( FW_BOLD );
				text.SetFont( &info.lfMenuFont );

				// Calc text width
				text.CalcRect( hDC, psti->txt, &trect );

				// Add for text
				tw = RW( trect ) + 6;

			} // end if

			if ( ( psti->f1 & STIF1_RIGHTBTN ) == 0 )
			{
				// Set button rect
				if ( !bVert ) SetRect( &btn, x, top, x + w + tw, bottom );
				else SetRect( &btn, left, y, right, y + h );
			} // end if
			else
			{
				// Set button rect
				if ( !bVert ) SetRect( &btn, rx - w - tw, top, rx, bottom );
				else SetRect( &btn, left, ry - y, right, ry );
			} // end else

			if ( ( psti->f1 & STIF1_DISABLED ) == 0 )
			{
				// Draw this button
				if ( m_pClicked == NULL && psti == m_pMouseOver ) psti->f1 |= STIF1_HILIGHT;
				else psti->f1 &= ~STIF1_HILIGHT;

				if ( psti == m_pClicked && psti == m_pMouseOver ) psti->f1 |= STIF1_PRESSED;
				else psti->f1 &= ~STIF1_PRESSED;

			} // end if

			if ( psti->b != NULL ) 
			{	if ( *psti->b ) psti->f1 |= STIF1_CHECKED;
				else psti->f1 &= ~STIF1_CHECKED;
			} // end if

			// Create tooltip if needed
			if ( bCreateTips && *psti->tip != 0 ) 
			{
				RECT tip;
				CopyRect( &tip, &btn );
				if ( bVert ) tip.top += m_lButtonSlant, tip.bottom -= m_lButtonSlant;
				else tip.left += m_lButtonSlant, tip.right -= m_lButtonSlant;

				m_tooltip.Add( &tip, psti->tip );
			} // end if

			// If there's room
			if (	( !bVert && rx > ( x + RW( btn ) ) ) ||
					( bVert && ry > ( y + RH( btn ) ) )	)
			{
				// Save button position
				CopyRect( &psti->rect, &btn );

				// Draw the button
				Draw( psti, hDC, &btn, bVert );

			} // end draw

			if ( ( psti->f1 & STIF1_RIGHTBTN ) == 0 )
			{
				// Next button
				if ( !bVert ) x += ( w + tw ) - ( m_lButtonSlant - 2 );
				else y += h - ( m_lButtonSlant - 2 );
			} // end if
			else
			{
				// Next button
				if ( !bVert ) rx -= ( w + tw ) - ( m_lButtonSlant - 2 );
				else ry -= h - ( m_lButtonSlant - 2 );
			} // end else

		} // end if

	} // end while

	// Do we want a text area?
	if ( !m_bTextArea ) return TRUE;
	if ( m_lMinTextArea <= 0 ) m_lMinTextArea = 20;
	if ( bVert && y > ry - m_lMinTextArea ) return TRUE;
	if ( !bVert && x > rx - m_lMinTextArea ) return TRUE;

	if ( !bVert )
	{
		COLORREF rgbCaption = CGrDC::ScaleColor(	GetSysColor( COLOR_3DFACE ),
													GetSysColor( COLOR_ACTIVECAPTION ), 200 );

		if ( m_bHiLight &&	( m_bCaptionMode || *m_szTitle != 0 || 
							  ( m_pMouseOver != NULL && *m_pMouseOver->tip != 0 ) ) )
		{
			POINT	pts[ 4 ];
			pts[ 0 ].x = x + 4; pts[ 0 ].y = bottom;
			pts[ 1 ].x = x + 4 + m_lButtonSlant; pts[ 1 ].y = top;
			pts[ 2 ].x = rx - 4; pts[ 2 ].y = top;
			pts[ 3 ].x = rx - m_lButtonSlant - 4; pts[ 3 ].y = bottom;

			HRGN hRgn = CreatePolygonRgn( pts, 4, WINDING );

			CWinDc cap;
			cap.Create( NULL, 0, 0, 64, bottom );
			CGrDC::VertGradientFill( cap, cap,	CGrDC::ScaleColor( rgbCaption, -30 ), 
												CGrDC::ScaleColor( rgbCaption, 30 ) );

			cap.Texturize( 10 );
//			cap.Aluminize( 5 );

			HBRUSH hBrush = CreatePatternBrush( cap );
			FillRgn( hDC, hRgn, hBrush );

			::DeleteObject( hBrush );
			::DeleteObject( hRgn );

		} // end if

		CGrDC dc( hDC, FALSE );
		CGrPen lt( PS_SOLID, 1, GetSysColor( COLOR_3DHILIGHT ) );
		CGrPen dk( PS_SOLID, 1, GetSysColor( COLOR_3DSHADOW ) );

		dc.SelectObject( dk );
		dc.MoveTo( x + 4, bottom );
		dc.LineTo( x + 4 + m_lButtonSlant, top );
		dc.LineTo( rx - 4, top );

		dc.SelectObject( lt );
		dc.MoveTo( x + 4, bottom );
		dc.LineTo( rx - m_lButtonSlant - 4, bottom );
		dc.LineTo( rx - 4, top );
	
		if ( *m_szTitle != 0 || ( !m_bForceTooltips && m_pMouseOver != NULL && *m_pMouseOver->tip != 0 ) )
		{
			CText	text;
			RECT	trect;

			text.SetFlags( DT_VCENTER | DT_SINGLELINE );

			// Init text rect
			SetRect( &trect, x + m_lButtonSlant + 12, top, rx - m_lButtonSlant - 6, bottom );

			if ( RW( trect ) > 20 )
			{
				LPCTSTR pText = m_szTitle;
				if ( *m_szTitle == 0 && ( !m_bForceTooltips && m_pMouseOver != NULL ) )
					pText = m_pMouseOver->tip;

				// What font does the user want for menu's?
				NONCLIENTMETRICS info;
				info.cbSize = sizeof(info);			
				SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( info ), &info, 0 );

				// Use bold font
				info.lfMenuFont.lfWeight = FW_BOLD;
				text.SetFont( &info.lfMenuFont );

				// Use end ellipsis
				text.SetFlags( text.GetFlags() | DT_END_ELLIPSIS );
				
				OffsetRect( &trect, 1, 1 );
				text.SetColor( GetSysColor( COLOR_3DHILIGHT ) );
				text.DrawText( hDC, pText, &trect );

				OffsetRect( &trect, -1, -1 );
//				text.SetColor( GetSysColor( COLOR_BTNTEXT ) );
				text.SetColor( CGrDC::ScaleColor( rgbCaption, -180 ) );
				text.DrawText( hDC, pText, &trect );

			} // end if

		} // end if

	} // end if

	else
	{
		CGrDC dc( hDC, FALSE );
		CGrPen lt( PS_SOLID, 1, GetSysColor( COLOR_3DHILIGHT ) );
		CGrPen dk( PS_SOLID, 1, GetSysColor( COLOR_3DSHADOW ) );

		dc.SelectObject( dk );
		dc.MoveTo( left, pRect->bottom - 4 );
		dc.LineTo( left, y + 4 );
		dc.LineTo( right, y + 4 + m_lButtonSlant );

		dc.SelectObject( lt );
		dc.MoveTo( right, y + 4 + m_lButtonSlant );
		dc.LineTo( right, pRect->bottom - 4 );
		dc.LineTo( left, pRect->bottom - 4 );

	} // end else

	return TRUE;
}

BOOL CDrawSlantToolbar::Draw(LPSLANTTOOLBARINFO psti, HDC hDC, LPRECT pRect, BOOL bVert)
{
	// Let user update
	SendUpdate( psti->cmd, (WPARAM)psti );

	// Highlight background if needed
	if ( ( m_bHiLightBtn && ( psti->f1 & STIF1_HILIGHT ) != 0 ) ||
		 ( psti->f1 & STIF1_HILIGHTALWAYS ) != 0 )
	{
		POINT	pts[ 4 ];

		if ( !bVert )
		{	pts[ 0 ].x = pRect->left; pts[ 0 ].y = pRect->bottom;
			pts[ 1 ].x = pRect->left + m_lButtonSlant; pts[ 1 ].y = pRect->top;
			pts[ 2 ].x = pRect->right; pts[ 2 ].y = pRect->top;
			pts[ 3 ].x = pRect->right - m_lButtonSlant; pts[ 3 ].y = pRect->bottom;
		} // end if

		HRGN hRgn = CreatePolygonRgn( pts, 4, WINDING );

		CWinDc cap;
		cap.Create( NULL, 0, 0, 64, 64 );
		CGrDC::FillSolidRect( cap, cap, CGrDC::ScaleColor(	GetSysColor( COLOR_3DFACE ),
															GetSysColor( COLOR_ACTIVECAPTION ),
															100 ) );
		cap.Texturize( 20 );

		HBRUSH hBrush = CreatePatternBrush( cap );
		FillRgn( hDC, hRgn, hBrush );

		::DeleteObject( hBrush );
		::DeleteObject( hRgn );

	} // end if

	CGrDC dc( hDC, FALSE );
	CGrPen lt( PS_SOLID, 1, GetSysColor( COLOR_3DHILIGHT ) );
	CGrPen dk( PS_SOLID, 1, GetSysColor( COLOR_3DSHADOW ) );
	
	if ( psti->icon != NULL )
	{
		long ix = 0, iy = 0;
		if ( bVert || *psti->txt == 0 ) ix = pRect->left + ( PRW( pRect ) >> 1 ) - ( 16 >> 1 );
		else ix = pRect->left + m_lButtonSlant + 2;
		iy = pRect->top + ( PRH( pRect ) >> 1 ) - ( 16 >> 1 );

		if ( ( psti->f1 & STIF1_NOCLICK ) == 0 && ( psti->f1 & STIF1_PRESSED ) != 0 ) ix++, iy++;

		// Save the icon rect
		SetRect( &psti->ricon, ix, iy, ix + 16, iy + 16 );

		if ( ( psti->f1 & STIF1_MOUSEROTATE ) == 0 )
		{
			// Draw the icon
			if ( ( psti->f1 & STIF1_CHECKED ) != 0 && psti->pressed != NULL )
					CGrDC::DrawIcon( hDC, NULL, psti->pressed, ix, iy, ( psti->f1 & STIF1_DISABLED ) != 0 );
			else CGrDC::DrawIcon( hDC, NULL, psti->icon, ix, iy, ( psti->f1 & STIF1_DISABLED ) != 0 );

		} // end if

		else DrawMouseRotateIcon( hDC, pRect, psti );

	} // end if

	if ( !bVert && *psti->txt != NULL )
	{
		CText	text;
		RECT	trect;

		// Init text rect
		SetRect( &trect, pRect->left + PRH( pRect ), pRect->top, pRect->right, pRect->bottom );

		// What font does the user want for menu's?
		NONCLIENTMETRICS info;
		info.cbSize = sizeof(info);			
		SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( info ), &info, 0 );

//		text.SetWeight( FW_BOLD );
		text.SetFont( &info.lfMenuFont );

		if ( ( psti->f1 & STIF1_NOCLICK ) == 0 && ( psti->f1 & STIF1_PRESSED ) != 0 )
			OffsetRect( &trect, 1, 1 );

		OffsetRect( &trect, 1, 1 );
		text.SetColor( GetSysColor( COLOR_3DHILIGHT ) );
		text.DrawText( hDC, psti->txt, &trect );

		OffsetRect( &trect, -1, -1 );
		text.SetColor( GetSysColor( COLOR_BTNTEXT ) );
		text.DrawText( hDC, psti->txt, &trect );

		// Calc text width
//		text.DrawText( hDC, psti->txt, &trect );

	} // end if

	if ( ( psti->f1 & STIF1_NOCLICK ) == 0 || ( psti->f1 & STIF1_HILIGHTALWAYS ) != 0 )
		if ( ( psti->f1 & ( STIF1_PRESSED | STIF1_CHECKED | STIF1_HILIGHT | STIF1_HILIGHTALWAYS ) ) != 0 )
		{
			if ( ( psti->f1 & ( STIF1_PRESSED | STIF1_CHECKED | STIF1_HILIGHTALWAYS ) ) != 0 ) 
				dc.SelectObject( dk );
			else dc.SelectObject( lt );

			if ( !bVert )
			{	dc.MoveTo( pRect->left, pRect->bottom );
				dc.LineTo( pRect->left + m_lButtonSlant, pRect->top );
				dc.LineTo( pRect->right, pRect->top );
			} // end if
			else
			{	dc.MoveTo( pRect->left, pRect->bottom - m_lButtonSlant );
				dc.LineTo( pRect->left, pRect->top );
				dc.LineTo( pRect->right, pRect->top + m_lButtonSlant );
			} // end if

			if ( ( psti->f1 & ( STIF1_PRESSED | STIF1_CHECKED | STIF1_HILIGHTALWAYS ) ) != 0 ) 
				dc.SelectObject( lt );
			else dc.SelectObject( dk );
			if ( !bVert )
			{	dc.MoveTo( pRect->left, pRect->bottom );
				dc.LineTo( pRect->right - m_lButtonSlant, pRect->bottom );
				dc.LineTo( pRect->right, pRect->top );
			} // end if
			else
			{	dc.MoveTo( pRect->right, pRect->top + m_lButtonSlant );
				dc.LineTo( pRect->right, pRect->bottom );
				dc.LineTo( pRect->left, pRect->bottom - m_lButtonSlant );
			} // end if

		} // end if

	return TRUE;
}

BOOL CDrawSlantToolbar::OnMouseMove(UINT nFlags, LPPOINT pt)
{
	LPSLANTTOOLBARINFO psti = HitTest( &m_rect, pt );

	// Undo click
	if ( ( nFlags & MK_LBUTTON ) == 0 ) m_pClicked = FALSE;		

	if ( psti != m_pMouseOver )
	{	m_pMouseOver = psti;
		return TRUE;
	} // end if

	return FALSE;
}

BOOL CDrawSlantToolbar::OnLButtonDown(UINT nFlags, LPPOINT pt)
{
	// Get click item
	m_pClicked = HitTest( &m_rect, pt );	

	// Ensure item is not disabled
	if ( m_pClicked != NULL && ( m_pClicked->f1 & STIF1_DISABLED ) != 0 )
		m_pClicked = NULL;

	return ( m_pClicked != NULL );
}

BOOL CDrawSlantToolbar::OnLButtonUp(UINT nFlags, LPPOINT pt)
{
	// Ensure item is not disabled
	if ( m_pClicked != NULL && 
		 ( m_pClicked->f1 & STIF1_DISABLED ) != 0 )
		m_pClicked = NULL;

	BOOL bRedraw = ( m_pClicked != NULL );

	// Verify user let go on the button
	if ( m_pClicked != NULL && HitTest( &m_rect, pt ) == m_pClicked )
	{
		// Toggle bool flag if any
		if ( m_pClicked->b != NULL ) *m_pClicked->b = !( *m_pClicked->b );

		// Send command
		SendCommand( m_pClicked->cmd, (WPARAM)m_pClicked );

	} // end if

	m_pClicked = NULL;

	return TRUE;
}


BOOL CDrawSlantToolbar::SetTextByCmd(UINT cmd, LPCTSTR text)
{
	LPSLANTTOOLBARINFO psti = NULL;
	while ( ( psti = (LPSLANTTOOLBARINFO)GetNext( psti ) ) != NULL )
		if ( psti->cmd == cmd )
		{	strcpy_sz( psti->txt, text );
			return TRUE;
		} // end if

	return FALSE;

}

BOOL CDrawSlantToolbar::DrawMouseRotateIcon(HDC hDC, LPRECT pRect, LPSLANTTOOLBARINFO psti)
{
	long cx = m_screenx + pRect->left + ( PRW( pRect ) >> 1 );
	long cy = m_screeny + pRect->top + ( PRH( pRect ) >> 1 );
	
	POINT pt = { 0, 0 };
	GetCursorPos( &pt );
	float angle = (float)180 + (float)360 - (float)CGrDC::GetAngle( cx, cy, pt.x, pt.y );
	if ( angle > 360 ) angle -= 360;
	HICON hRIcon = CGrDC::RotateIcon( psti->icon, angle );
	if ( hRIcon == NULL ) return FALSE;

	// Draw the icon
	CGrDC::DrawIcon( hDC, NULL, hRIcon, psti->ricon.left, psti->ricon.top );

	// Delete the icon
	::DeleteObject( hRIcon );

	return TRUE;
}
