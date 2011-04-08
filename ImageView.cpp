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
// ImageView.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "ImageView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageView

CImageView::CImageView()
{
	m_pvi = NULL;
	m_pos = NULL;
	m_imgindex = MAXDWORD;
	m_bDrawMotion = TRUE;
}

CImageView::~CImageView()
{
}


BEGIN_MESSAGE_MAP(CImageView, CStatic)
	//{{AFX_MSG_MAP(CImageView)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageView message handlers

BOOL CImageView::OnEraseBkgnd(CDC* pDC) 
{
	if ( pDC == NULL ) return FALSE;

	RECT rect;
	GetClientRect( &rect );

	// Draw from dc if any
	if ( m_pos != NULL ) 
	{
		HDC hDC = pDC->GetSafeHdc();

		// Create offscreen buffer
		if ( m_offscreen.Create( pDC->GetSafeHdc(), &rect ) )
			hDC = m_offscreen;

		// Fill in bck
		CGrDC::FillSolidRect( hDC, &rect, GetSysColor( COLOR_APPWORKSPACE ) );

		if (	RW( rect ) < PRW( m_pos->GetRect() ) ||
				RH( rect ) < PRH( m_pos->GetRect() ) )
		
			// Correct aspect ratio
			CGrDC::AspectCorrect( &rect, *m_pos );

		else
		{
			RECT win;
			CopyRect( &win, &rect );

			CopyRect( &rect, *m_pos );
			CGrDC::Center( &rect, &win );
		} // end else

	
		// Draw the video image
		m_pos->Draw( hDC, &rect );

		// Draw motion if needed
		if ( m_bDrawMotion && m_pvi != NULL && m_pvi->motion.bEnable )
			CWinDc::DrawMotion( &m_pvi->motion, hDC, &rect );

		// Draw to screen
		m_offscreen.Draw( pDC->GetSafeHdc(), m_offscreen );

	} // end if

	// Do we have an image index?
	else if ( m_imgindex != MAXDWORD )
	{
		HDC hDC = pDC->GetSafeHdc();

		// Create offscreen buffer
		if ( m_offscreen.Create( pDC->GetSafeHdc(), &rect ) )
			hDC = m_offscreen;

		// Fill in background
		CGrDC::FillSolidRect( hDC, &rect, GetSysColor( COLOR_APPWORKSPACE ) );

		// Get image object
		LPPUBIMGINFO ppii = IMGLIST().GetImage( m_imgindex );
		if ( ppii == NULL || ppii->os == NULL ) return TRUE;

		// Correct aspect ratio
		CGrDC::AspectCorrect( &rect, &ppii->rect );

		// Show a little cuff
		InflateRect( &rect, -4, -4 );

		// Draw box
		CGrDC::Box3d(	hDC, &rect, 1, 
						CGrDC::ScaleColor( GetSysColor( COLOR_APPWORKSPACE ), 100 ),
						CGrDC::ScaleColor( GetSysColor( COLOR_APPWORKSPACE ), -100 ),
						TRUE );

		InflateRect( &rect, -1, -1 );

		// Draw the image
		ppii->os->Draw( hDC, &rect );

		// Draw to screen
		m_offscreen.Draw( pDC->GetSafeHdc(), m_offscreen );

	} // end if

	// Fill in background
	else CGrDC::FillSolidRect( pDC->GetSafeHdc(), &rect, GetSysColor( COLOR_APPWORKSPACE ) );

	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}

void CImageView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

}

void CImageView::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);

	RedrawWindow();
}
