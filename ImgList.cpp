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
// ImgList.cpp : implementation file
//

#include "stdafx.h"
#include "Vp.h"
#include "ImgList.h"

#include "VpDoc.h"
#include "VpView.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define UPDATETIMER			101

#define WM_DROPMSG		( WM_USER + 101 )

/////////////////////////////////////////////////////////////////////////////
// CImgList

CImgList::CImgList()
{
	m_pImgList = NULL;

	m_lAnimate = MAXLONG;

	m_bInit = TRUE;
	m_pSelected = NULL;
	m_hWinIcon = ::LoadIcon( NULL, IDI_WINLOGO );
	m_hHtml = CWinFile::GetIconForFile( "file.htm" );
	m_hHtmlSmall = CWinFile::GetIconForFile( "file.htm", FALSE );

	m_uTimerId = 0;
	m_dwUpdateTimer = 0;
}

CImgList::~CImgList()
{
	if ( m_hHtml != NULL ) 
	{	DeleteObject( m_hHtml );
		m_hHtml = NULL;
	} // end if
}


BEGIN_MESSAGE_MAP(CImgList, CStatic)
	//{{AFX_MSG_MAP(CImgList)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_DROPFILES()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_TIMER()
	ON_COMMAND(ID_IMAGELIST_NEWIMAGE, OnImagelistNewimage)
	ON_COMMAND(ID_IMAGELIST_DELETEIMAGE, OnImagelistDeleteimage)
	ON_COMMAND(ID_IMAGELIST_PROPERTIES, OnImagelistProperties)
	ON_COMMAND(ID_IMAGELIST_VIEW, OnImagelistView)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_DROPMSG, OnDropMsg )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImgList message handlers

BOOL CImgList::OnEraseBkgnd(CDC* pDC) 
{

	return TRUE;
//	return CStatic::OnEraseBkgnd(pDC);
}

void CImgList::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// Initialize if needed
//	if ( m_bInit ) { m_bInit = FALSE; Init(); }

	RECT rect;
	GetClientRect( &rect );
	if ( m_scrollbar.IsWindowVisible() )
	{	if ( m_bVertScroll ) rect.right -= GetSystemMetrics( SM_CXVSCROLL );
		else rect.bottom -= GetSystemMetrics( SM_CXHSCROLL );
	} // end if

	if ( m_offscreen.GetSafeHdc() == NULL ) Regen();
	m_offscreen.Draw( dc.GetSafeHdc(), &rect );

	// Forget the buffer
//	else Draw( dc.GetSafeHdc(), &rect );

	// Are we animating?
	if ( m_lAnimate != MAXLONG )
	{	if ( m_scrollbar.IsWindowVisible() )
		{	SCROLLINFO si;
			m_scrollbar.GetScrollInfo( &si, SIF_POS );
			long dif = ( si.nPos - m_lAnimate );
			if ( dif > 256 ) dif = 256; else if ( dif < -256 ) dif = -256;
			if ( dif > -2 && dif < 2 ) m_lAnimate = MAXLONG;
			else m_lAnimate += ( dif / 2 );
		} // end if
		else m_lAnimate = MAXLONG;

		Regen();

	} // end if
}

void CImgList::Init( HWND hParent )
{				 
	// Verify window
	if ( !::IsWindow( hParent ) ) return;

	// Lose old window if any
	if ( ::IsWindow( GetSafeHwnd() ) ) DestroyWindow();

	// Use whole rect if not specified
	RECT rect, *pRect = &rect;
	::GetClientRect( hParent, &rect );		

	// Calc window metrics
	long width = pRect->right - pRect->left;
	long height = pRect->bottom - pRect->top;

	// Create video window
	CreateEx(	0, AfxRegisterWndClass(	0, LoadCursor( NULL, IDC_ARROW ), NULL ),
					NULL, WS_CHILD | WS_VISIBLE, 
					pRect->top, pRect->bottom, width, height,
					hParent, NULL );	

//	Create( NULL, WS_CHILD | WS_VISIBLE, *pRect, CWnd::FromHandle( hParent ) );


	// Set up for drag-drop operations
	DragAcceptFiles();
	m_droptarget.Register( GetSafeHwnd(), WM_DROPMSG );

	// Set clip children style
	DWORD style = GetWindowLong( GetSafeHwnd(), GWL_STYLE );
	style |= WS_CLIPCHILDREN;
	SetWindowLong( GetSafeHwnd(), GWL_STYLE, style );

	// Create the scroll bar
	GetClientRect( &rect );
	rect.left = rect.right - GetSystemMetrics( SM_CXVSCROLL );
	m_scrollbar.Create(	SBS_RIGHTALIGN | SBS_VERT,
//						SBS_SIZEBOXBOTTOMRIGHTALIGN | SBS_SIZEBOX,
						rect, this, 0 );

	// Upate the scrollbar
	UpdateScrollbar();

	// Set update timer
	m_uTimerId = SetTimer( UPDATETIMER, 250, NULL );
}

void CImgList::OnDropFiles(HDROP hDropInfo) 
{
	// Get the number of dropped files
	DWORD	dwFiles = DragQueryFile( hDropInfo, 0xffffffff, NULL, 0 );
	if ( dwFiles == 0 ) return;
	
	// Get the drop group
	POINT	pt;
	DragQueryPoint( hDropInfo, &pt );

	// For each dropped file
	char fname[ CWF_STRSIZE ];
	for ( DWORD i = 0; i < dwFiles; i++ )
	{	DragQueryFile( hDropInfo, i, fname, sizeof( fname ) - 1 );
		if ( fname[ 0 ] != 0x0 ) Add( fname );
	} // end for
	
	// Redraw
	UpdateScrollbar();
	RedrawWindow();

	CStatic::OnDropFiles(hDropInfo);
}

LRESULT CImgList::OnDropMsg(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR str = (LPCTSTR)lParam;
	if ( wParam != CF_TEXT || str == NULL || *str == 0 )
		return 0;
	
	Add( str );

	// Redraw
	UpdateScrollbar();
	RedrawWindow();

	return 0;
}

BOOL CImgList::Add(LPCTSTR pUrl)
{
	if ( m_pImgList == NULL ) return FALSE;
	if ( pUrl == NULL ) return FALSE;

	// Add an image
	m_pImgList->Add( pUrl );

	return TRUE;
}

#define XMARGIN		12
#define YMARGIN		12

#define XTILE		80
#define YTILE		80

BOOL CImgList::Draw(HDC hDC, LPRECT pRect, LPPOINT pt, LPPUBIMGINFO *pPii, LPDWORD index)
{
	// Sanity checks
	if ( m_pImgList == NULL ) return FALSE;
	if ( pRect == NULL ) return FALSE;
	if ( index != NULL ) *index = MAXDWORD;

	// Update the selected image
	m_pSelected = m_pImgList->GetSelImage();

	// Set us as the message target
//	m_pImgList->SetMessageTarget( GetSafeHwnd(), WM_IMGMSG );
//	m_pImgList->IncFrame();

	long soff = 0;
	RECT rect;
	CopyRect( &rect, pRect );
	
	// Get scroll offset
	if ( m_lAnimate != MAXLONG ) soff = m_lAnimate;
	else if ( m_scrollbar.IsWindowVisible() )
	{	SCROLLINFO si;
		m_scrollbar.GetScrollInfo( &si, SIF_POS );
		soff = si.nPos;
	} // endif

	if ( hDC != NULL )
	{
		// Create background if needed
		if ( m_bck.GetSafeHdc() == NULL )
		{	m_bck.Create( hDC, 0, 0, 64, 64 );
//			CGrDC::FillSolidRect( m_bck, m_bck, GetSysColor( COLOR_APPWORKSPACE ) );
//			CGrDC::FillSolidRect( m_bck, m_bck, GetSysColor( COLOR_3DFACE ) );
//			CGrDC::FillSolidRect( m_bck, m_bck, CGrDC::ScaleColor(	GetSysColor( COLOR_APPWORKSPACE ),
//																	GetSysColor( COLOR_ACTIVECAPTION ),
//																	100 ) );
			CGrDC::FillSolidRect( m_bck, m_bck, GetSysColor( COLOR_APPWORKSPACE ) );
			m_bck.Texturize( 20 );
//			m_bck.Aluminize( 5 );
		} // end if

		// Tile in the bck
		m_bck.Tile( hDC, &rect );

	} // end if

	// Any images to draw?
	DWORD listsize = m_pImgList->Size();
	if ( listsize == 0 ) return TRUE;

	// Create margin
	InflateRect( &rect, -4, -4 );

	long w = XTILE;
	long h = YTILE;
	long rw = RW( rect );
	long rh = RH( rect );

	if ( m_bVertScroll )
	{
		// Check for vertical squeeze
		if ( rh <= ( h + YMARGIN ) )
		{	h = RH( rect ) - YMARGIN - 2;
			if ( h < 32 ) h = 32; w = h;
		} // end if
	} // end if
	else
	{
		// Check for vertical squeeze
		if ( rw <= ( w + XMARGIN ) )
		{	w = RW( rect ) - XMARGIN - 2;
			if ( w < 32 ) w = 32; h = w;
		} // end if
	} // end else

	// How many tiles
	long xt = rw / ( w + XMARGIN );
	long yt = rh / ( h + YMARGIN );

	DWORD i = 0;
	if ( m_bVertScroll )
	{
		// Center images on the x axis
		if ( rw <= ( w + XMARGIN ) ) 
		{
			rect.left += 2; 
			w = RW( rect ) - XMARGIN - 2; 
			if ( w < 32 ) w = 32; h = w; 
			xt = rw / ( w + XMARGIN );
		}
		else
		{	if ( xt > (long)listsize ) xt = listsize;
			rect.left += ( rw - ( ( w + XMARGIN ) * xt ) ) >> 1;
		} // end else

		long x = rect.left, y = rect.top - soff;
		LPPUBIMGINFO ppii = NULL;
		while (	y < rh &&
				( ppii = (LPPUBIMGINFO)m_pImgList->GetNext( ppii ) ) != NULL )
		{

			// Can this image be seen?
			if ( ( y + h ) > rect.top )
			{
				RECT rImg;
				SetRect( &rImg, x + ( XMARGIN >> 1 ), y + ( YMARGIN >> 1 ), 
								x + ( XMARGIN >> 1 ) + w, y + ( YMARGIN >> 1 ) + h );

				try
				{
					// Draw the image
					if ( hDC != NULL ) DrawImage( hDC, &rImg, ppii );

				} catch( ... ) { ASSERT( 0 ); }

				// Are we doing a hit test?
				if ( pt != NULL && pPii != NULL )
					if ( PtInRect( &rImg, *pt ) )
					{ 
						if ( index != NULL ) *index = i;
						*pPii = ppii; return TRUE; 
					} 

				i++;

			} // end if

			// Next image location
			x += w + XMARGIN;
			if ( ( x + w ) > rw ) x = rect.left, y += h + YMARGIN;

		} // end while

	} // end if
	else
	{
		// Center images on the x axis
		if ( rh <= ( h + YMARGIN ) ) 
		{
			rect.top += 2; 
			h = RH( rect ) - YMARGIN - 2; 
			if ( h < 32 ) h = 32; w = h; 
			yt = rh / ( h + YMARGIN );
		}
		else
		{	if ( yt > (long)listsize ) yt = listsize;
			rect.top += ( rh - ( ( h + YMARGIN ) * yt ) ) >> 1;
		} // end else

		long x = rect.left - soff, y = rect.top;
		LPPUBIMGINFO ppii = NULL;
		while (	x < rw &&
				( ppii = (LPPUBIMGINFO)m_pImgList->GetNext( ppii ) ) != NULL )
		{
			// Can this image be seen?
			if ( ( x + w ) > rect.left )
			{
				RECT rImg;
				SetRect( &rImg, x + ( XMARGIN >> 1 ), y + ( YMARGIN >> 1 ), 
								x + ( XMARGIN >> 1 ) + w, y + ( YMARGIN >> 1 ) + h );

				try
				{
					// Draw the image
					if ( hDC != NULL ) DrawImage( hDC, &rImg, ppii );

				} catch( ... ) { ASSERT( 0 ); }

				// Are we doing a hit test?
				if ( pt != NULL && pPii != NULL )
					if ( PtInRect( &rImg, *pt ) )
					if ( PtInRect( &rImg, *pt ) )
					{ 
						if ( index != NULL ) *index = i;
						*pPii = ppii; return TRUE; 
					}

				i++;

			} // end if

			// Next image location
			y += h + YMARGIN;
			if ( ( y + h ) > rh ) y = rect.top, x += w + XMARGIN;

		} // end while

	} // end else

	// Check for hit test
	if ( pt != NULL && pPii != NULL ) return FALSE;

	return TRUE;
}

BOOL CImgList::DrawImage(HDC hDC, LPRECT pRect, LPPUBIMGINFO ppii)
{
	// Sanity check
	if ( hDC == NULL || pRect == NULL || ppii == NULL ) 
		return FALSE;

	RECT rTxt;
	{ // Draw the image text

		char fname[ CWF_STRSIZE ];
		strcpy( fname, ppii->name );

		// Get the users preferred font
		CText text;
		NONCLIENTMETRICS info;
		info.cbSize = sizeof(info);			
		SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( info ), &info, 0 );
		text.SetFont( &info.lfMenuFont );

		// Set text flags
		text.SetFlags( DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_PATH_ELLIPSIS | DT_END_ELLIPSIS );

		// Calc text size
		SetRect( &rTxt, 0, 0, 10, 10 );
		text.CalcRect( hDC, fname, &rTxt );

		// Should we left align?
		if ( ( RW( rTxt ) + 2 ) >= PRW( pRect ) )
			text.SetFlags( text.GetFlags() & ~DT_CENTER ),
			text.SetFlags( text.GetFlags() | DT_LEFT );

		// Move to correct location
		OffsetRect( &rTxt, 0, pRect->bottom - RH( rTxt ) );
		rTxt.left = pRect->left; rTxt.right = pRect->right;
		rTxt.bottom += 6; rTxt.right += 1;

		if ( ppii == m_pSelected )
		{
			CWinDc dc( NULL, &rTxt );
			COLORREF rgb = CGrDC::ScaleColor(	GetSysColor( COLOR_3DFACE ), 
												GetSysColor( COLOR_ACTIVECAPTION ), 
												500 );

			CGrDC::VertGradientFill( dc, dc,	CGrDC::ScaleColor( rgb, -40 ),
												CGrDC::ScaleColor( rgb, 40 ) );
//			dc.Texturize( 20 );
			dc.Aluminize( 8 );

			dc.Draw( hDC, &rTxt );

			CGrDC::Box3d( hDC, &rTxt, 1,	CGrDC::ScaleColor( rgb, 120 ),
											CGrDC::ScaleColor( rgb, -120 ), FALSE );

		} // end if

//		OffsetRect( &rTxt, 1, 1 );
//		text.SetColor( GetSysColor( COLOR_3DHILIGHT ) );
//		text.DrawText( hDC, fname, &rTxt );

//		OffsetRect( &rTxt, -1, -1 );
		text.SetColor( GetSysColor( COLOR_HIGHLIGHTTEXT ) );
		text.DrawText( hDC, fname, &rTxt );

	} // end draw image text

	RECT rImg;
	CopyRect( &rImg, pRect );
	rImg.bottom = rTxt.top - 4;

	// Draw the image if any
	CWinDc *pdc = ppii->os;
	if ( pdc != NULL && pdc->GetSafeHdc() != NULL )
	{
		// Correct the aspect ratio
		CGrDC::AspectCorrect( &rImg, *pdc );

		// Draw the image
		pdc->Draw( hDC, &rImg );

		// Put border around image
		RECT border;
		CopyRect( &border, &rImg );
		InflateRect( &border, 1, 1 );
		CGrDC::Box3d( hDC, &border, 1, RGB( 0, 0, 0 ), RGB( 0, 0, 0 ) );

	} // end if

	else
	{
//		DrawFocusRect( hDC, &rImg ); 

		HPEN hPen = CreatePen( PS_DOT, 1, RGB( 180, 0, 0 ) );
		HPEN hOldPen = (HPEN)::SelectObject( hDC, hPen );
		HBRUSH hOldBrush = (HBRUSH)::SelectObject( hDC, (HBRUSH)GetStockObject( NULL_BRUSH ) );

		MoveToEx( hDC, rImg.left, rImg.top, NULL );
		LineTo( hDC, rImg.right, rImg.top );
		LineTo( hDC, rImg.right, rImg.bottom );

		MoveToEx( hDC, rImg.left, rImg.top, NULL );
		LineTo( hDC, rImg.left, rImg.bottom );
		LineTo( hDC, rImg.right, rImg.bottom );

		::SelectObject( hDC, hOldBrush );
		::SelectObject( hDC, hOldPen );
		::DeleteObject( hPen );

		// Adjust rect
		rImg.right += 1; rImg.bottom += 1;

	} // end else

	if ( ppii == m_pSelected )
	{
		COLORREF rgb = CGrDC::ScaleColor(	GetSysColor( COLOR_3DFACE ), 
											GetSysColor( COLOR_ACTIVECAPTION ), 
											500 );

		InflateRect( &rImg, 2, 2 );

		CGrDC::Box3d( hDC, &rImg, 2,	CGrDC::ScaleColor( rgb, 120 ),
										CGrDC::ScaleColor( rgb, -120 ), TRUE );

	} // end if


	return TRUE;
}

void CImgList::Size()
{
	if ( !::IsWindow( GetSafeHwnd() ) ) return;

	// Position the scrollbar
	if ( ::IsWindow( m_scrollbar.GetSafeHwnd() ) )
	{
		RECT rect;
		GetClientRect( &rect );
		if ( m_bVertScroll ) rect.left = rect.right - GetSystemMetrics( SM_CXVSCROLL );
		else rect.top = rect.bottom - GetSystemMetrics( SM_CXHSCROLL );
		m_scrollbar.MoveWindow( &rect );
		UpdateScrollbar();
	} // end if
}

void CImgList::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);

	Size();	

	SetUpdateTimer();
}

long CImgList::GetMaxScrollHeight( BOOL bScrollbar )
{
	// Punt if no images
	if ( m_pImgList == NULL || m_pImgList->Size() == 0 ) return 0;
	if ( !::IsWindow( GetSafeHwnd() ) ) return 0;
	DWORD listsize = m_pImgList->Size();

	RECT rect;
	GetClientRect( &rect );
	if ( bScrollbar )
	{	if ( m_bVertScroll ) rect.right -= GetSystemMetrics( SM_CXVSCROLL );
		else rect.bottom -= GetSystemMetrics( SM_CXHSCROLL );
	} // end if
	InflateRect( &rect, -4, -4 );

	long w = XTILE;
	long h = YTILE;
	long rw = RW( rect );
	long rh = RH( rect );

	if ( m_bVertScroll )
	{
		// Check for vertical squeeze
		if ( rh <= ( h + YMARGIN ) )
		{	h = RH( rect ) - YMARGIN - 2;
			if ( h < 32 ) h = 32; w = h;
		} // end if
	} // end if
	else
	{
		// Check for vertical squeeze
		if ( rw <= ( w + XMARGIN ) )
		{	w = RW( rect ) - XMARGIN - 2;
			if ( w < 32 ) w = 32; h = w;
		} // end if
	} // end else

	// How many tiles
	long xt = rw / ( w + XMARGIN );
	long yt = rh / ( h + YMARGIN );

	if ( m_bVertScroll )
	{
		// Center images on the x axis
		if ( rw <= ( w + XMARGIN ) ) 
		{
			rect.left += 2; 
			w = RW( rect ) - XMARGIN - 2; 
			if ( w < 32 ) w = 32; h = w; 
			xt = rw / ( w + XMARGIN );
		}
		else
		{	if ( xt > (long)listsize ) xt = listsize;
			rect.left += ( rw - ( ( w + XMARGIN ) * xt ) ) >> 1;
		} // end else

		// How many vertical tiles?
		if ( xt < 1 ) xt = 1;
		long vtiles = ( listsize / xt );
		if ( vtiles == 0 ) vtiles = 1;
		if ( listsize % xt ) vtiles++;
		
		// How many pixels high are the tiles?
		return ( vtiles * ( h + YMARGIN ) ) + YMARGIN;

	} // end if
	else
	{
		// Center images on the x axis
		if ( rh <= ( h + YMARGIN ) ) 
		{
			rect.top += 2; 
			h = RH( rect ) - YMARGIN - 2; 
			if ( h < 32 ) h = 32; w = h; 
			yt = rh / ( h + YMARGIN );
		}
		else
		{	if ( yt > (long)listsize ) yt = listsize;
			rect.top += ( rh - ( ( h + YMARGIN ) * yt ) ) >> 1;
		} // end else
	
		// How many vertical tiles?
		if ( yt < 1 ) yt = 1;
		long htiles = ( listsize / yt );
		if ( htiles == 0 ) htiles = 1;
		if ( listsize % yt ) htiles++;
		
		// How many pixels high are the tiles?
		return ( htiles * ( w + XMARGIN ) ) + XMARGIN;

	} // end else

}

void CImgList::UpdateScrollbar()
{
	RECT rect;
	GetClientRect( &rect );

	// Get scrollbar information
	SCROLLINFO info; ZeroMemory( &info, sizeof( info ) );
	info.cbSize = sizeof( SCROLLINFO );
	m_scrollbar.GetScrollInfo( &info );

	// Horizontal or vertical?
//	m_bVertScroll = RW( rect ) < RH( rect );
	m_bVertScroll = RW( rect ) < RH( rect ) || RH( rect ) > 320;
	if ( ( ( m_scrollbar.GetStyle() & SBS_VERT ) != 0 ) != m_bVertScroll )
	{
		// Lose the window
		m_scrollbar.DestroyWindow();

		if ( m_bVertScroll ) 
		{	rect.left = rect.right - GetSystemMetrics( SM_CXVSCROLL );
			m_scrollbar.Create( WS_VISIBLE | SBS_RIGHTALIGN | SBS_VERT, rect, this, 0 ); 
		} // end if
		else
		{	rect.top = rect.bottom - GetSystemMetrics( SM_CXHSCROLL );
			m_scrollbar.Create( WS_VISIBLE | SBS_RIGHTALIGN | SBS_HORZ, rect, this, 0 ); 
		} // end else

		// Restore information
		m_scrollbar.SetScrollInfo( &info );

	} // end if

	long ns_maxscroll = GetMaxScrollHeight( FALSE );
	if (	( m_bVertScroll && ns_maxscroll > RH( rect ) ) ||
			( !m_bVertScroll && ns_maxscroll > RW( rect ) ) )
	{
		long ws_maxscroll = GetMaxScrollHeight( TRUE );

		info.fMask = SIF_ALL;
		info.nMin = 0;
		info.nMax = ws_maxscroll;
		if ( info.nPos > info.nMax ) info.nPos = info.nMax;
		if ( m_bVertScroll ) info.nPage = RH( rect );
		else info.nPage = RW( rect );

		// Set the scroll bar info
		m_scrollbar.SetScrollInfo( &info );

		// Make scroll bar visible if needed
		if ( !m_scrollbar.IsWindowVisible() )
			m_scrollbar.ShowWindow( SW_SHOWNORMAL );

	} // end if

	// Hide the window if we don't need it
	else if ( m_scrollbar.IsWindowVisible() )
		m_scrollbar.ShowWindow( SW_HIDE );
}

void CImgList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	long pos = MAXLONG;

	long oldpos = MAXLONG;
	if ( m_scrollbar.IsWindowVisible() )
	{	SCROLLINFO si;
		m_scrollbar.GetScrollInfo( &si, SIF_POS );
		oldpos = si.nPos;
	} // endif
	
	// Update the scrollbar
	if ( pScrollBar != NULL ) 
		pos = CComCtrl::OnScroll( pScrollBar->GetSafeHwnd(), nSBCode );

	// Update list if needed
	if ( pos != MAXLONG && pScrollBar == &m_scrollbar )
	{
		// Animate position if needed
		if (	pos != oldpos && 
				( nSBCode == SB_PAGEUP || nSBCode == SB_PAGEDOWN ) )
			m_lAnimate = oldpos;
		else m_lAnimate = MAXLONG;
		
		RECT rect;
		GetClientRect( &rect );
		rect.right -= GetSystemMetrics( SM_CXVSCROLL );					
		Regen( &rect );

	} // end if

	CStatic::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CImgList::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	long pos = MAXLONG;

	long oldpos = MAXLONG;
	if ( m_scrollbar.IsWindowVisible() )
	{	SCROLLINFO si;
		m_scrollbar.GetScrollInfo( &si, SIF_POS );
		oldpos = si.nPos;
	} // endif

	if ( pScrollBar != NULL ) 
		pos = CComCtrl::OnScroll( pScrollBar->GetSafeHwnd(), nSBCode );

	if ( pos != MAXLONG && pScrollBar == &m_scrollbar )
	{
		// Animate position if needed
		if (	pos != oldpos && 
				( nSBCode == SB_PAGEUP || nSBCode == SB_PAGEDOWN ) )
			m_lAnimate = oldpos;
		else m_lAnimate = MAXLONG;

		RECT rect;
		GetClientRect( &rect );
		if ( m_bVertScroll ) rect.right -= GetSystemMetrics( SM_CXVSCROLL );
		else rect.bottom -= GetSystemMetrics( SM_CXHSCROLL );
		Regen( &rect );
	} // end if
	
	CStatic::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CImgList::OnLButtonDown(UINT nFlags, CPoint point) 
{
	RECT rect; GetClientRect( &rect );
	LPPUBIMGINFO pOld = m_pSelected;

	// Get new selection
	DWORD i = 0;
	m_pSelected = HitTest( &rect, &point, &i );
	if ( m_pSelected == pOld && m_pSelected != NULL ) m_pSelected = NULL;

	// Redraw if selection changed
	if ( m_pSelected != pOld ) 
	{	if ( m_pImgList != NULL && i != MAXDWORD ) 
			m_pImgList->SetCurSel( i );
		Regen();
	} // end if

	// Send click message
	SendWMMessage( 1, (LPARAM)m_pSelected );

	CStatic::OnLButtonDown(nFlags, point);
}

void CImgList::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	RECT rect; GetClientRect( &rect );
	LPPUBIMGINFO pOld = m_pSelected;

	// Get new selection
	DWORD i = 0;
	m_pSelected = HitTest( &rect, &point, &i );
	if ( m_pSelected == pOld && m_pSelected != NULL ) m_pSelected = NULL;

	// Redraw if selection changed
	if ( m_pSelected != pOld )
	{	if ( m_pImgList != NULL && i != MAXDWORD ) 
			m_pImgList->SetCurSel( i );
		Regen();
	} // end if

	// Send click message
	SendWMMessage( 2, (LPARAM)m_pSelected );
	
	CStatic::OnLButtonDblClk(nFlags, point);
}


void CImgList::OnLButtonUp(UINT nFlags, CPoint point) 
{


	CStatic::OnLButtonUp(nFlags, point);
}

BOOL CImgList::ShowMenu(long x, long y)
{
	HMENU hMenu, hSub;
	BOOL bOnTop = FALSE;

	// Load the template
	hMenu = LoadMenu( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDM_IMAGELIST ) );
	if ( hMenu == NULL ) return FALSE;
	hSub = GetSubMenu( hMenu, 0 );
	if ( hSub == NULL ) return FALSE;

	// Check for clipboard data
	{
		BOOL enable = IsClipboardFormatAvailable( CF_TEXT );
		if ( !enable ) EnableMenuItem( hSub, ID_IMGLIST_FROMCLIPBOARD, MF_GRAYED );
	} 

	// We must be the foreground window for this to work correctly
	SetForegroundWindow();

	// Show the popup menu
	::TrackPopupMenu( hSub, TPM_RIGHTBUTTON, x, y, 0, GetSafeHwnd(), NULL );

	// Unload the menu
	DestroyMenu( hMenu );

	return TRUE;
}


void CImgList::OnRButtonDown(UINT nFlags, CPoint point) 
{
	RECT rect; GetClientRect( &rect );
	LPPUBIMGINFO pOld = m_pSelected;

	// Get new selection
	DWORD i = 0;
	m_pSelected = HitTest( &rect, &point, &i );
	if ( m_pSelected == pOld && m_pSelected != NULL ) m_pSelected = NULL;

	// Redraw if selection changed
	if ( m_pSelected != pOld ) 
	{	if ( m_pImgList != NULL && i != MAXDWORD ) 
			m_pImgList->SetCurSel( i );
		Regen();
	} // end if

	// Send click message
	SendWMMessage( 1, (LPARAM)m_pSelected );

	APP()->ReGen();

	POINT pt;
	pt.x = point.x; pt.y = point.y;

	ClientToScreen( &pt );
	ShowMenu( pt.x, pt.y );
	
	CStatic::OnRButtonDown(nFlags, point);
}


BOOL CImgList::Regen( LPRECT pRect )
{
	if ( !::IsWindow( GetSafeHwnd() ) ) return FALSE;
	if ( !::IsWindow( m_scrollbar.GetSafeHwnd() ) ) return FALSE;

	RECT rect;
	GetClientRect( &rect );

	// Create offscreen
	if ( !m_offscreen.Create( NULL, &rect ) )
		return FALSE;

	// Draw to offscreen buffer
	Draw( m_offscreen, m_offscreen );

	CGrDC::Box3d(	m_offscreen, m_offscreen, 1,
					CGrDC::ScaleColor( GetSysColor( COLOR_APPWORKSPACE ), -120 ),
					CGrDC::ScaleColor( GetSysColor( COLOR_APPWORKSPACE ), 120 ) );

	RedrawWindow( pRect );

	return TRUE;
}

void CImgList::OnSysColorChange() 
{
	CStatic::OnSysColorChange();
	
	m_bck.Destroy();	
	Regen();
}

void CImgList::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent == UPDATETIMER )
	{
		// Do we need to update?
		if ( m_dwUpdateTimer != 0 && m_dwUpdateTimer < GetTickCount() )
		{	m_dwUpdateTimer = 0;
			Regen();
		} // end if

	} // end if	
	
	CStatic::OnTimer(nIDEvent);
}

void CImgList::OnImagelistNewimage() 
{	FRAME()->OnNewimage();
}

void CImgList::OnImagelistDeleteimage() 
{	FRAME()->OnDeleteimage();
}

void CImgList::OnImagelistProperties() 
{	APP()->ShowProperties( 1 );
}

void CImgList::OnImagelistView() 
{	FRAME()->OnViewimage();
}
