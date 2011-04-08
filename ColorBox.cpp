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
// ColorBox.cpp : implementation file
//

#include "stdafx.h"
#include "ColorBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorBox

CColorBox::CColorBox()
{
	m_rgb = RGB( 0, 0, 0 );
}

CColorBox::~CColorBox()
{
}


BEGIN_MESSAGE_MAP(CColorBox, CStatic)
	//{{AFX_MSG_MAP(CColorBox)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorBox message handlers

BOOL CColorBox::OnEraseBkgnd(CDC* pDC) 
{
	if ( pDC == NULL ) return FALSE;

	RECT rect;
	GetClientRect( &rect );

	if ( IsWindowEnabled() )
		pDC->FillSolidRect( &rect, m_rgb );
	else pDC->FillSolidRect( &rect, GetSysColor( COLOR_BTNSHADOW ) );

	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}

void CColorBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CStatic::OnPaint() for painting messages
}

void CColorBox::OnEnable(BOOL bEnable) 
{
	CStatic::OnEnable(bEnable);
	RedrawWindow();
}
