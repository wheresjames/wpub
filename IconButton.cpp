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
// IconButton.cpp : implementation file
//

#include "stdafx.h"
#include "Vp.h"
#include "IconButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIconButton

CIconButton::CIconButton()
{
	m_hIcon = NULL;
	m_bPressed = FALSE;
}

CIconButton::~CIconButton()
{
}


BEGIN_MESSAGE_MAP(CIconButton, CButton)
	//{{AFX_MSG_MAP(CIconButton)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIconButton message handlers

BOOL CIconButton::OnEraseBkgnd(CDC* pDC) 
{
	BOOL bRet = CButton::OnEraseBkgnd(pDC);

	DrawIcon();

	return bRet;
}

void CIconButton::OnPaint() 
{
	CButton::OnPaint();

	DrawIcon();
}

BOOL CIconButton::SetIcon(HICON hIcon, HICON hDisabledIcon)
{
	m_hIcon = hIcon;
	m_hDisabledIcon = hDisabledIcon;

	// Redraw window if needed
//	if ( ::IsWindow( GetSafeHwnd() ) ) RedrawWindow();

	return TRUE;
}

void CIconButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bPressed = TRUE;
	
	CButton::OnLButtonDown(nFlags, point);
}

void CIconButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bPressed = FALSE;
	
	CButton::OnLButtonUp(nFlags, point);
}

void CIconButton::DrawIcon()
{
	if ( m_hIcon == NULL ) return;

	CDC *pDC = GetDC();
	if ( pDC == NULL ) return;

	RECT rect;
	GetClientRect( &rect );

	if ( m_bPressed )
	{	POINT pt;
		GetCursorPos( &pt );
		ScreenToClient( &pt );
		if ( PtInRect( &rect, pt ) ) OffsetRect( &rect, 1, 1 );
	} // end if

	// Draw disabled
	if ( !::IsWindowEnabled( GetSafeHwnd() ) )
	{
		// Did we get a disabled icon?
		if ( m_hDisabledIcon != NULL )
			CGrDC::DrawIcon( pDC->GetSafeHdc(), &rect, m_hDisabledIcon );

		// Create disabled icon if none
		else
		{	HICON hGrayIcon = CGrDC::CreateMonoChromeIcon( m_hIcon, GetSysColor( COLOR_3DFACE ) );
			CGrDC::DrawIcon( pDC->GetSafeHdc(), &rect, hGrayIcon );
			DestroyIcon( hGrayIcon );
		} // end else

	} // end if

	// Draw the icon
	else if ( m_hIcon != NULL )
		CGrDC::DrawIcon( pDC->GetSafeHdc(), &rect, m_hIcon );

	ReleaseDC( pDC );
}

void CIconButton::OnEnable(BOOL bEnable) 
{
	CButton::OnEnable(bEnable);
	
	DrawIcon();
	
}
