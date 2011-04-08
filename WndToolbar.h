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
#if !defined(AFX_WNDTOOLBAR_H__2F1A3F66_B325_49A2_A1E8_1D729BF77E23__INCLUDED_)
#define AFX_WNDTOOLBAR_H__2F1A3F66_B325_49A2_A1E8_1D729BF77E23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndToolbar.h : header file
//

#include "MenuItemList.h"

/////////////////////////////////////////////////////////////////////////////
// CWndToolbar window

class CWndToolbar
{
// Construction
public:
	CWndToolbar();
	virtual ~CWndToolbar();


public:
	BOOL CreateToolTips( HWND hWnd, LPRECT pRect );
	BOOL OnLButtonUp( UINT nFlags, LPPOINT pt );
	BOOL OnLButtonDown( UINT nFlags, LPPOINT pt );
	BOOL OnMouseMove( UINT nFlags, LPPOINT pt );
	BOOL DrawButton( HWND hWnd, HDC hDC, LPRECT pRect, LPMITEMINFO pmi );
	LPMITEMINFO HitTest( LPPOINT pt );
	BOOL Draw( HWND hWnd, HDC hDC, LPRECT pRect );
	LPMITEMINFO AddItem( LPCTSTR pName, DWORD dwCmd, HICON hIcon, HCURSOR hCursor, LPBOOL pBool, LPCTSTR pTip );

	long GetHeight() { return m_lHeight; }
	long GetWidth();

	void SetMessageTarget( HWND hWnd, UINT uMsg ) { m_hWnd = hWnd; m_uMsg = uMsg; }
	void SendCommand( WPARAM wParam, LPARAM lParam )
	{	if ( m_hWnd != NULL && IsWindow( m_hWnd ) )
		::SendMessage( m_hWnd, m_uMsg, wParam, lParam ); }
	void SetHeight( long h ) { m_lHeight = h; }

	DWORD GetNumButtons() { return m_mitems.Size(); }

	void ClearMouseOver() { m_pmiover = NULL; }

	void SetBckColor( COLORREF rgb ) { m_rgbBck = rgb; }

	void Destroy() { m_pmiover = NULL; m_tooltip.Destroy(); m_mitems.Destroy(); }

private:

	COLORREF		m_rgbBck;

	CMenuItemList	m_mitems;

	long			m_lWidth;
	long			m_lHeight;

	HWND			m_hWnd;
	UINT			m_uMsg;

	RECT			m_rect;

	LPMITEMINFO		m_pmiover;
	LPMITEMINFO		m_clickitem;
	BOOL			m_bClick;

	CTooltip		m_tooltip;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDTOOLBAR_H__2F1A3F66_B325_49A2_A1E8_1D729BF77E23__INCLUDED_)
