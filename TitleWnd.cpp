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
// TitleWnd.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "TitleWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTitleWnd

CTitleWnd::CTitleWnd()
{
	m_bCenter = FALSE;
	*m_szTitle = 0;

	m_dwFontSize = 26;
	strcpy( m_szFont, "Arial" );

	m_rgbWindow = GetSysColor( COLOR_3DFACE );
	m_rgbText = MAXDWORD;
}

CTitleWnd::~CTitleWnd()
{
}


BEGIN_MESSAGE_MAP(CTitleWnd, CStatic)
	//{{AFX_MSG_MAP(CTitleWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTitleWnd message handlers

BOOL CTitleWnd::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}

void CTitleWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	RECT rect;
	GetClientRect( &rect );

	COLORREF bck = CGrDC::ScaleColor( GetSysColor( COLOR_3DFACE ), 20 );
//	COLORREF bck = RGB( 255, 255, 255 );

//	dc.FillSolidRect( &rect, RGB( 255, 255, 255 ) );
	dc.FillSolidRect( &rect, bck );

//	dc.FillSolidRect( &rect, m_rgbWindow );

	if( *m_szTitle )
	{
		CText text;

		text.SetWeight( FW_BOLD );
		text.SetFont( m_dwFontSize, m_szFont );

		DWORD flags = DT_VCENTER | DT_SINGLELINE;
		if ( m_bCenter ) flags |= DT_CENTER;
		text.SetFlags( flags );

		rect.left += 10;

		// Draw shadow
		OffsetRect( &rect, 1, 1 );
		text.SetColor( CGrDC::ScaleColor( bck, -30 ) );
		text.DrawText( dc.GetSafeHdc(), m_szTitle, &rect );

		// Draw for real
		OffsetRect( &rect, -2, -2 );
		if ( m_rgbText != MAXDWORD ) text.SetColor( m_rgbText );
		else text.SetColor( CGrDC::ScaleColor( GetSysColor( COLOR_ACTIVECAPTION ), -20 ) );
		text.DrawText( dc.GetSafeHdc(), m_szTitle, &rect );


/*		CText text;

		text.SetWeight( FW_BOLD );
		text.SetFont( m_dwFontSize, m_szFont );

		DWORD flags = DT_VCENTER | DT_SINGLELINE;
		if ( m_bCenter ) flags |= DT_CENTER;
		text.SetFlags( flags );

		rect.left += 10;

		// Draw shadow
		OffsetRect( &rect, 1, 1 );
		text.SetColor( RGB( 180, 180, 180 ) );
		text.DrawText( dc.GetSafeHdc(), m_szTitle, &rect );

		// Draw for real
		OffsetRect( &rect, -2, -2 );
		text.SetColor( RGB( 0, 0, 180 ) );
		text.DrawText( dc.GetSafeHdc(), m_szTitle, &rect );
*/
	} // end if
}

BOOL CTitleWnd::GetTextSize(LPRECT pRect)
{
	// Sanity check
	if ( pRect == NULL ) return FALSE;

	RECT rect;
	rect.left = 0; rect.right = 10;
	rect.top = 0; rect.bottom = 10;

	HDC hDC = ::GetDC( GetSafeHwnd() );
	if ( hDC == NULL ) return FALSE;

	CText text;
	text.SetWeight( FW_BOLD );
	text.SetFont( m_dwFontSize, m_szFont );

	DWORD flags = DT_VCENTER | DT_SINGLELINE;
	if ( m_bCenter ) flags |= DT_CENTER;
	text.SetFlags( flags );

	// Calculate text size
	text.CalcRect( hDC, m_szTitle, &rect );

	// Release the DC
	::ReleaseDC( GetSafeHwnd(), hDC );

	// Let the caller in on the good news
	CopyRect( pRect, &rect );

	return TRUE;
}
