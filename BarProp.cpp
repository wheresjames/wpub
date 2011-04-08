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
// BarProp.cpp : implementation file
//

#include "stdafx.h"
#include "Vp.h"
#include "BarProp.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBarProp dialog


CBarProp::CBarProp(CWnd* pParent /*=NULL*/)
{
	//{{AFX_DATA_INIT(CBarProp)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bHorz = TRUE;
	m_dwMode = 0;
	m_view = NULL;
	ZeroMemory( &m_viewrect, sizeof( m_viewrect ) );
}


void CBarProp::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBarProp)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBarProp, CDialogBar)
	//{{AFX_MSG_MAP(CBarProp)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBarProp message handlers

BOOL CBarProp::Create( CWnd *pParent, DWORD style, UINT idd, UINT idview)
{
	style &= ~CBRS_SIZE_FIXED;
	style |= CBRS_SIZE_DYNAMIC;

//	style |= CCS_NOPARENTALIGN | CCS_NOMOVEY| CCS_NODIVIDER | CCS_NORESIZE;

	// Create the dialog bar
//	if ( !CDialogBar::Create( pParent, CG_IDD_PUBIMGVIEW, style, CG_ID_VIEW_PUBIMGVIEW ) )
//		return FALSE;

	if ( !CDialogBar::Create( pParent, idd, style, idview ) )
		return FALSE;

	m_bChangeDockedSize = TRUE;
	m_sizeFloating = m_sizeDocked = m_sizeDefault;
	
	style = GetBarStyle();

	style &= ~CBRS_SIZE_FIXED;
	style |= CBRS_SIZE_DYNAMIC;

	SetBarStyle( style );

	return TRUE;
}

BOOL CBarProp::OnEraseBkgnd(CDC* pDC) 
{
	CDialogBar::OnEraseBkgnd(pDC);

	if ( !IsFloating() )
	{
		RECT rect;
		GetClientRect( &rect );

		CGrDC	dc( pDC->GetSafeHdc(), FALSE );
//		CGrPen	dkpen( PS_SOLID, 1, GetSysColor( COLOR_3DDKSHADOW ) );
//		CGrPen	ltpen( PS_SOLID, 1, GetSysColor( COLOR_3DHILIGHT ) );

		CGrPen	dkpen( PS_SOLID, 1, CGrDC::ScaleColor( GetSysColor( COLOR_3DFACE ), -60 ) );
		CGrPen	ltpen( PS_SOLID, 1, CGrDC::ScaleColor( GetSysColor( COLOR_3DFACE ), 40 ) );

		long off = 10;

		if ( !m_bHorz )
		{
			dc.SelectObject( ltpen );
			dc.MoveTo( rect.left, off - 4 );
			dc.LineTo( rect.right, off - 4 );
			dc.MoveTo( rect.left, off - 3 );
			dc.LineTo( rect.right, off - 3 );

			
			dc.MoveTo( rect.left, off + 1 );
			dc.LineTo( rect.right, off + 1 );
			dc.MoveTo( rect.left, off + 2 );
			dc.LineTo( rect.right, off + 2 );

			dc.SelectObject( dkpen );
			dc.MoveTo( rect.left, off - 2 );
			dc.LineTo( rect.right, off - 2 );
			dc.MoveTo( rect.left, off + 3 );
			dc.LineTo( rect.right, off + 3 );

		} // end if
		else
		{
			dc.SelectObject( ltpen );
			dc.MoveTo( off - 4, rect.top );
			dc.LineTo( off - 4, rect.bottom );
			dc.MoveTo( off - 3, rect.top );
			dc.LineTo( off - 3, rect.bottom );
			dc.MoveTo( off + 1, rect.top );
			dc.LineTo( off + 1, rect.bottom );
			dc.MoveTo( off + 2, rect.top );
			dc.LineTo( off + 2, rect.bottom );

			dc.SelectObject( dkpen );
			dc.MoveTo( off - 2, rect.top );
			dc.LineTo( off - 2, rect.bottom );
			dc.MoveTo( off + 3, rect.top );
			dc.LineTo( off + 3, rect.bottom );

		} // end if

	} // end if
	
	if ( m_view == NULL )
		CGrDC::FillSolidRect(	pDC->GetSafeHdc(), &m_viewrect, 
								GetSysColor( COLOR_APPWORKSPACE ) );

	return TRUE;
}

CSize CBarProp::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	// Save docking mode
	m_dwMode = dwMode;

	// Track horz or vert
	if ( dwMode & LM_VERTDOCK ) m_bHorz = FALSE;
	else if ( dwMode & LM_HORZDOCK ) m_bHorz = TRUE;

	// Return default if it is being docked or floated
	if ( ( dwMode & LM_VERTDOCK) || ( dwMode & LM_HORZDOCK ) )
	{
		CFrameWnd *pFrame = GetDockingFrame();

		if ( pFrame != NULL && ::IsWindow( pFrame->GetSafeHwnd() ) )
		{
			CWnd *pWnd = VIEW();
//			CWnd *pWnd = pFrame;
			if ( pWnd == NULL ) pWnd = pFrame;

			if ( dwMode & LM_HORZDOCK )
			{
				RECT rect, org;
				::GetClientRect( pWnd->GetSafeHwnd(), &org );
				CopyRect( &rect, &org );
//				InflateRect( &rect, -4, -4 );

				rect.top = 4;
				rect.bottom = m_sizeDocked.cy - 4;
				if ( !IsFloating() ) rect.left += 18;

				CopyRect( &m_viewrect, &rect );
				UpdateViewPos();

				if ( !IsFloating() ) rect.left -= 18;
				
				return CSize( ( org.right - org.left ), m_sizeDocked.cy );

			} // end if

			else if ( dwMode & LM_VERTDOCK )
			{
				RECT rect, org;
				::GetClientRect( pWnd->GetSafeHwnd(), &org );
				CopyRect( &rect, &org );
//				InflateRect( &rect, -4, -4 );

				rect.left = 4;
				rect.right = m_sizeDocked.cx - 4;
				if ( !IsFloating() ) rect.top += 18;

				CopyRect( &m_viewrect, &rect );
				UpdateViewPos();

				if ( !IsFloating() ) rect.top -= 18;

				return CSize( m_sizeDocked.cx, ( org.bottom - org.top ) );

			} // end else

		} // end if

		if (dwMode & LM_STRETCH) // if not docked stretch to fit
			return CSize((dwMode & LM_HORZ) ? 32767 : m_sizeDocked.cx,
							(dwMode & LM_HORZ) ? m_sizeDocked.cy : 32767);

		else return m_sizeDocked;

	}

	if (dwMode & LM_MRUWIDTH)
	   return m_sizeFloating;

	// In all other cases, accept the dynamic length
	if (dwMode & LM_LENGTHY)
	   return CSize(m_sizeFloating.cx, (m_bChangeDockedSize) ?
					m_sizeFloating.cy = m_sizeDocked.cy = nLength :
					m_sizeFloating.cy = nLength);
	else
	   return CSize((m_bChangeDockedSize) ?
					m_sizeFloating.cx = m_sizeDocked.cx = nLength  :
					m_sizeFloating.cx = nLength, m_sizeFloating.cy);
  
} 

BOOL CBarProp::UpdateViewPos()
{
	if ( m_view == NULL ) return FALSE;
	if ( !::IsWindow( m_view->GetSafeHwnd() ) ) return FALSE;

	// Move window
	m_view->SetWindowPos( NULL,	m_viewrect.left, m_viewrect.top,
								RW( m_viewrect ), RH( m_viewrect ),
								SWP_NOZORDER | SWP_NOACTIVATE );

	// Ensure window is visible
	m_view->ShowWindow( SW_SHOWNORMAL );

	// Redraw the window
	m_view->RedrawWindow();

	return TRUE;
}

void CBarProp::SetView( CWnd *pView ) 
{
	m_view = pView; 

	if ( pView == NULL ) return;
	if ( !::IsWindow( pView->GetSafeHwnd() ) ) return;
	
//	DWORD style = GetWindowLong( pView->GetSafeHwnd(), GWL_STYLE );
//	style |= WS_CHILD;
//	SetWindowLong( pView->GetSafeHwnd(), GWL_STYLE, style );

	UpdateViewPos(); 
}


void CBarProp::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBar::OnSize(nType, cx, cy);

	Size();
}

void CBarProp::Size()
{
	if ( !IsWindow( GetSafeHwnd() ) ) return;
	if ( !IsWindow( FRAME()->GetSafeHwnd() ) ) return;

	RECT rect;
	GetClientRect( &rect );

	if ( !IsFloating() && VIEW() != NULL && IsWindow( VIEW()->GetSafeHwnd() ) )
	{
		RECT frame;
		VIEW()->GetClientRect( &frame );
		if ( !m_bHorz )
		{
			rect.top = frame.top;
			rect.bottom = frame.bottom;
		} // end if
		else
		{
			rect.left = frame.left;
			rect.right = frame.right;
		} // end if

	} // end if

	if ( !m_bHorz )
	{
		rect.left = 4;
		rect.right = m_sizeDocked.cx - 4;
		if ( !IsFloating() ) rect.top += 18;
	} // end if
	else
	{
		rect.top = 4;
		rect.bottom = m_sizeDocked.cy - 4;
		if ( !IsFloating() ) rect.left += 18;
	} // end else

	CopyRect( &m_viewrect, &rect );

	// Redraw the window
	RedrawWindow();

	// Update the view
	UpdateViewPos();	
}


void CBarProp::OnMouseMove(UINT nFlags, CPoint point) 
{
	RECT rect;
	GetClientRect( &rect );

	if ( m_dwMode & LM_HORZDOCK )
	{
		if ( point.y < 4 )
		{
			SetCursor( LoadCursor( NULL, IDC_SIZENS ) );
		} // end if
	} // end if
	else if ( m_dwMode & LM_VERTDOCK )
	{
		if ( point.x < 4 )
		{
			SetCursor( LoadCursor( NULL, IDC_SIZEWE ) );

			if ( m_pt.x != 0 && ( nFlags & MK_LBUTTON ) != 0 )
			{
				long nx = point.x - m_pt.x;
				SetWindowPos(	NULL, nx, 0, RW( m_rect ) + nx, RH( m_rect ), 
								SWP_NOACTIVATE | SWP_NOZORDER );


			} // end if

		} // end if
	} // end else if
	
	CDialogBar::OnMouseMove(nFlags, point);
}

void CBarProp::OnLButtonDown(UINT nFlags, CPoint point) 
{
	RECT rect;
	GetClientRect( &rect );

	if ( m_dwMode & LM_HORZDOCK )
	{
		if ( point.y < 4 )
		{	
			GetClientRect( &m_rect );
			m_pt.x = point.x; m_pt.y = point.y;

			SetCapture();
			SetCursor( LoadCursor( NULL, IDC_SIZENS ) );

			return;
		} // end if
	} // end if
	else if ( m_dwMode & LM_VERTDOCK )
	{
		if ( point.x < 4 )
		{
			GetClientRect( &m_rect );
			m_pt.x = point.x; m_pt.y = point.y;

			SetCapture();
			SetCursor( LoadCursor( NULL, IDC_SIZEWE ) );

			return;
		} // end if
	} // end else if

	CDialogBar::OnLButtonDown(nFlags, point);
}

void CBarProp::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if ( m_pt.x != 0 || m_pt.y != 0 )
	{	m_pt.x = 0; m_pt.y = 0; ReleaseCapture(); }
	
	CDialogBar::OnLButtonUp(nFlags, point);
}
