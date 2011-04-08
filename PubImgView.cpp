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
// PubImgView.cpp : implementation file
//

#include "stdafx.h"
#include "Vp.h"
#include "PubImgView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPubImgView

CPubImgView::CPubImgView()
{
	m_pImgList = NULL;

	m_hWndCallback = NULL;
	m_uMsgCallback = NULL;
}

CPubImgView::~CPubImgView()
{
}


BEGIN_MESSAGE_MAP(CPubImgView, CWnd)
	//{{AFX_MSG_MAP(CPubImgView)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPubImgView message handlers

BOOL CPubImgView::OnEraseBkgnd(CDC* pDC) 
{	
	if ( pDC == NULL ) return FALSE;

	RECT rect;
	GetClientRect( &rect );

	HDC hDC = pDC->GetSafeHdc();

	// Create offscreen DC
	if ( m_offscreen.Create( hDC, &rect ) )
		hDC = m_offscreen;

	// Fill rect
	CGrDC::FillSolidRect( hDC, &rect, GetSysColor( COLOR_3DFACE ) );

	// Regen video if needed
	if ( m_video.GetSafeHdc() == NULL ) Regen();

	// Draw to screen
	m_video.Draw( hDC, &rect, FALSE );

	DWORD sel = m_pImgList->GetCurSel();

	if ( sel < m_pImgList->Size() )
	{
		RECT selbox;
		CopyRect( &selbox, &rect );

		long w = RW( rect );
		long h = RH( rect );

		// Where will the selection box be?
		if ( w > h ) { selbox.left += sel * h; selbox.right = selbox.left + h; }
		else { selbox.top += sel * w; selbox.bottom = selbox.top + w; }

		InflateRect( &selbox, -4, -4 );

		// Draw the selection box
		CGrDC::Box3d(	hDC, &selbox, 3,
						GetSysColor( COLOR_HIGHLIGHT ), 
						GetSysColor( COLOR_HIGHLIGHT ), TRUE );	
	} // end if

	// Update the screen
	m_offscreen.Draw( pDC->GetSafeHdc(), &rect );

	return TRUE;
//	return CWnd::OnEraseBkgnd(pDC);
}

BOOL CPubImgView::Regen( DWORD i )
{
	RECT rect;
	GetClientRect( &rect );

	// Punt if no image list
	if ( m_pImgList == NULL || m_pImgList->Size() == 0 )
	{	m_video.Destroy(); return TRUE; }

	long w = RW( rect );
	long h = RH( rect );

	// Just allocate what we need
	if ( w > h ) rect.right = rect.left + ( h * m_pImgList->Size() );
	else rect.bottom = rect.top + ( w + m_pImgList->Size() );

	// Create offscreen buffer
	if ( !m_video.Create( NULL, &rect ) ) return FALSE;

	// Fill in the background
	CGrDC::FillSolidRect( m_video, &rect, GetSysColor( COLOR_3DFACE ) );
	
	// Draw the images
	if ( !Draw( m_video, &rect ) ) return FALSE;

	// Show the changes
	Update();

	return TRUE;
}

BOOL CPubImgView::Init(HWND hParent, LPRECT pRect )
{
	// Verify window
	if ( !::IsWindow( hParent ) ) return FALSE;

	// Lose old window if any
	if ( ::IsWindow( GetSafeHwnd() ) ) DestroyWindow();

	// Use whole rect if not specified
	RECT rect;
	if ( pRect == NULL )
	{	pRect = &rect;
		::GetClientRect( hParent, &rect );		
	} // end if

	// Calc window metrics
	long width = pRect->right - pRect->left;
	long height = pRect->bottom - pRect->top;

	// Create video window
	CreateEx(	0, AfxRegisterWndClass(	0, LoadCursor( NULL, IDC_ARROW ),
										(HBRUSH)GetStockObject( BLACK_BRUSH ) ),
					NULL, WS_CHILD | WS_VISIBLE, 
					pRect->top, pRect->bottom, width, height,
					hParent, NULL );	

	DragAcceptFiles();

	return TRUE;
}

BOOL CPubImgView::Size( LPRECT pRect )
{
	if ( pRect == NULL ) return FALSE;
	if ( !::IsWindow( GetSafeHwnd() ) ) return FALSE;

	// Calc window metrics
	long width = pRect->right - pRect->left;
	long height = pRect->bottom - pRect->top;

	// Set the new position
	SetWindowPos( NULL, pRect->left, pRect->top, width, height,
						SWP_NOZORDER | SWP_NOACTIVATE );

	Update();

	return TRUE;
}

#define BORDERWIDTH		5

BOOL CPubImgView::Draw(HDC hDC, LPRECT pRect)
{
	// Sanity checks
	if ( m_pImgList == NULL ) return TRUE;
	if ( hDC == NULL || pRect == NULL ) return FALSE;

	long width = pRect->right - pRect->left;
	long height = pRect->bottom - pRect->top;

	RECT	rect;
	DWORD	i = 0;
	long	off = 0;

	// Vertical list
	if ( width < height )
	{
		while ( i < m_pImgList->Size() && ( off + width ) <= pRect->bottom )
		{
			// Create image bounding rect
			SetRect( &rect, pRect->left + BORDERWIDTH, off + BORDERWIDTH, 
							pRect->right - BORDERWIDTH, off + width - BORDERWIDTH );

			// Draw the image
			DrawImage( hDC, &rect, i++, FALSE );

			// Next pane
			off += width;

		} // end while

	} // end if

	// Horizontal list
	else
	{
		while ( i < m_pImgList->Size() && ( off + height ) <= pRect->right )
		{
			// Create image bounding rect
			SetRect( &rect, off + BORDERWIDTH, pRect->top + BORDERWIDTH, 
							off + height - BORDERWIDTH, pRect->bottom - BORDERWIDTH );

			// Draw the image
			DrawImage( hDC, &rect, i++, TRUE );

			// Next pane
			off += height;

		} // end while


	} // end if

	return TRUE;
}

BOOL CPubImgView::DrawImage(HDC hDC, LPRECT pRect, DWORD i, BOOL /*bHorz*/ )
{
	if ( m_pImgList == NULL ) return FALSE;

	RECT rect;
	CopyRect( &rect, pRect );

	// Make room for letters
//	rect.bottom -= ( rect.bottom - rect.top ) >> 2;

//	if ( sel == i )
//		Box(	hDC, &rect, 3,
//				GetSysColor( COLOR_HIGHLIGHT ), 
//				GetSysColor( COLOR_HIGHLIGHT ) );	

	CGrDC::Box3d(	hDC, &rect, 1,
					GetSysColor( COLOR_3DHILIGHT ), 
					GetSysColor( COLOR_3DDKSHADOW ), TRUE );	

	// Scale the rectangle
	m_pImgList->Scale( i, &rect );

	// Center the rect
	CGrDC::Center( &rect, pRect );

	// Draw the image
	m_pImgList->Draw( hDC, &rect, i );

	return TRUE;
}

void CPubImgView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDown(nFlags, point);
	
	RECT rect;
	GetClientRect( &rect );

	POINT pt;
	pt.x = point.x; pt.y = point.y;

	// Do callback
	DWORD i = HitTest( &rect, &pt );
	if ( i != MAXDWORD ) 
	{	
		if ( m_pImgList != NULL ) m_pImgList->SetCurSel( i );
		Update();
		Callback( 1, 0L );
	} // end if
	
}

DWORD CPubImgView::HitTest(LPRECT pRect, LPPOINT pt)
{
	// Sanity checks
	if ( m_pImgList == NULL ) return MAXDWORD;

	long width = pRect->right - pRect->left;
	long height = pRect->bottom - pRect->top;

	RECT	rect;
	DWORD	i = 0;
	long	off = 0;

	// Vertical list
	if ( width < height )
	{
		while ( i < m_pImgList->Size() && ( off + width ) < pRect->bottom )
		{
			// Create image bounding rect
			SetRect( &rect, pRect->left + BORDERWIDTH, off + BORDERWIDTH, 
							pRect->right - BORDERWIDTH, off + width - BORDERWIDTH );

			// Is this image being clicked?
			if ( PtInRect( &rect, *pt ) ) return i; else i++;

			// Next pane
			off += width;

		} // end while

	} // end if

	// Horizontal list
	else
	{
		while ( i < m_pImgList->Size() && ( off + height ) < pRect->right )
		{
			// Create image bounding rect
			SetRect( &rect, off + BORDERWIDTH, pRect->top + BORDERWIDTH, 
							off + height - BORDERWIDTH, pRect->bottom - BORDERWIDTH );

			// Is this image being clicked?
			if ( PtInRect( &rect, *pt ) ) return i; else i++;

			// Next pane
			off += height;

		} // end while


	} // end if

	return MAXDWORD;
}


