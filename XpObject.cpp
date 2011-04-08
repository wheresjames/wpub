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
// XpObject.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "XpObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXpObject

CXpObject::CXpObject()
{
	*m_szClass = 0;
	m_iPartId = 0;
	m_iStateId = 0;
	m_hIcon = NULL;
}

CXpObject::~CXpObject()
{
}


BEGIN_MESSAGE_MAP(CXpObject, CStatic)
	//{{AFX_MSG_MAP(CXpObject)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXpObject message handlers

BOOL CXpObject::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}

void CXpObject::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	RECT rect;
	GetClientRect( &rect );

	CWinDc	wdc;
	wdc.Create( dc.GetSafeHdc(), &rect );

	// Fill in bck color
	COLORREF bck = GetSysColor( COLOR_3DFACE );
	CGrDC::FillSolidRect( wdc, wdc, bck );

	// Are we using XP themes?
	if ( *m_szClass && CXpTheme::IsWindowThemed( GetSafeHwnd() ) )
	{
		// Draw the xp control
		CXpTheme xpthm( GetSafeHwnd(), m_szClass );
		xpthm.DrawThemeBackground( wdc, m_iPartId, m_iStateId, wdc, NULL );

	} // end if

	// Draw icon if supplied
	else if ( m_hIcon != NULL ) CGrDC::DrawIcon( wdc, wdc, m_hIcon );

	// Draw to screen
	wdc.Draw( dc.GetSafeHdc(), &rect );
}
