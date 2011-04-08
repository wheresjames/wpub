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
// ShowImg.cpp : implementation file
//

#include "stdafx.h"
#include "ShowImg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UPDATETIMER			101

/////////////////////////////////////////////////////////////////////////////
// CShowImg

CShowImg::CShowImg()
{
	m_showdc = NULL;
}

CShowImg::~CShowImg()
{
}


BEGIN_MESSAGE_MAP(CShowImg, CStatic)
	//{{AFX_MSG_MAP(CShowImg)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShowImg message handlers

BOOL CShowImg::Load(LPCTSTR pRes, LPCTSTR pResType, LPCTSTR pType )
{
	DWORD	size = 0;
	
	// Load the license
	if ( !CWinFile::LoadResource( pRes, NULL, &size, pResType, NULL ) && size > 0 )
		return FALSE;

	// Allocate memory
	TMem< BYTE > buf;
	if ( !buf.allocate( size + 1 ) ) return FALSE;

	// Load resource
	if ( !CWinFile::LoadResource(	pRes, buf, &size, pResType, NULL ) )
		return FALSE;
	
	// Decode the image
	if ( pType == NULL ) pType = "png";
	if ( !m_img.Decode( buf, size, pType ) ) return FALSE;

	// Refresh the image
	if ( ::IsWindow( GetSafeHwnd() ) ) RedrawWindow();

	return TRUE;
}

BOOL CShowImg::OnEraseBkgnd(CDC* pDC) 
{
	if ( pDC == NULL ) return FALSE;

	RECT rect;
	GetClientRect( &rect );

//	m_offscreen.Draw( pDC->GetSafeHdc(), &rect );

	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}

void CShowImg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	RECT rect;
	GetClientRect( &rect );

	if ( m_showdc != NULL )
	{	m_showdc->Draw( dc.GetSafeHdc(), &rect, FALSE );
		return;
	} // end if

	if ( m_img.IsValid() ) m_img.Draw( dc.GetSafeHdc(), &rect );
	else CGrDC::FillSolidRect( dc.GetSafeHdc(), &rect, GetSysColor( COLOR_3DFACE ) );

/*	// Create offscreen buffer
	if ( !m_offscreen.Create( NULL, &rect ) )
		return;

	HDC hDC = m_offscreen;

	// Fill in the background
	CGrDC::FillSolidRect( hDC, &rect, GetSysColor( COLOR_3DFACE ) );

	// Draw the image
	if ( m_img.IsValid() )
	{
		RECT draw;
		CopyRect( &draw, &rect );
		CGrDC::AspectCorrect( &draw, m_img );
		InflateRect( &draw, -4, -4 );

		if ( RW( draw ) == m_img.GetWidth() && RH( draw ) == m_img.GetHeight() )
			m_img.Draw( hDC, &draw );
		else
		{
			CWinImg sized;
			sized.Copy( &m_img );
			sized.Resample( &draw, 0 );
			sized.Draw( hDC, &draw, FALSE );
			
		} // end else
	
		COLORREF rgb =  GetSysColor( COLOR_APPWORKSPACE );
		CGrDC::Box3d( hDC, &draw, 2,	CGrDC::ScaleColor( rgb, -100 ),
										CGrDC::ScaleColor( rgb, 100 ) );

	} // end if

	// Draw to the screen
	m_offscreen.Draw( dc.GetSafeHdc(), &rect );
*/
/*
	RECT rect;
	GetClientRect( &rect );

	// Draw the image
	if ( m_img.IsValid() ) m_img.Draw( dc.GetSafeHdc(), rect );

	// Fill solid gray if no image
	else dc.FillSolidRect( &rect, RGB( 80, 80, 80 ) );
*/
}

BOOL CShowImg::DestroyWindow() 
{
	m_img.Destroy();
	
	return CStatic::DestroyWindow();
}

BOOL CShowImg::LoadFile(LPCTSTR pFile)
{
	if ( !m_img.Load( pFile ) ) return FALSE;

	Regen();

	return TRUE;
}

void CShowImg::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);

	RedrawWindow();
}

BOOL CShowImg::Regen()
{	
	// Redraw window if any
	if ( ::IsWindow( GetSafeHwnd() ) ) 
		RedrawWindow();

	return TRUE;
}

void CShowImg::OnSysColorChange() 
{
	CStatic::OnSysColorChange();
}


void CShowImg::CaptureShowDc()
{
	if ( m_showdc == NULL ) return;
	m_img.CreateFromHBITMAP( *m_showdc );
	m_showdc = NULL;
}
