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
// CustomToolbar.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "CustomToolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomToolbar

CCustomToolbar::CCustomToolbar()
{
}

CCustomToolbar::~CCustomToolbar()
{
}


BEGIN_MESSAGE_MAP(CCustomToolbar, CStatic)
	//{{AFX_MSG_MAP(CCustomToolbar)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomToolbar message handlers

void CCustomToolbar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	RECT rect;
	GetClientRect( &rect );
	m_tb.Draw( GetSafeHwnd(), dc.GetSafeHdc(), &rect );
}

BOOL CCustomToolbar::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}

void CCustomToolbar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if ( m_tb.OnLButtonDown( nFlags, &point ) )
		RedrawWindow();
	
	CStatic::OnLButtonDown(nFlags, point);
}

void CCustomToolbar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ( m_tb.OnLButtonUp( nFlags, &point ) )
		RedrawWindow();
	
	CStatic::OnLButtonUp(nFlags, point);
}

void CCustomToolbar::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( m_tb.OnMouseMove( nFlags, &point ) )
		RedrawWindow();
	
	CStatic::OnMouseMove(nFlags, point);
}
