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
// WinMenu.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "WinMenu.h"

#include "WndToolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIMERID			101

#define TFONTSIZE		20
#define TFONTTYPE		"Arial"

#define WM_MENUCMD		( WM_USER + 101 )

#define EMPTY_STR		"(Empty)"

/////////////////////////////////////////////////////////////////////////////
// CWinMenu

CWinMenu::CWinMenu()
{
	m_bFirstDraw = TRUE;

	m_hWndMsgTarget = NULL;

	m_menuitem = NULL;

	m_hDropWnd = NULL;

	m_dwDestroyDelay = 0;

	m_itemover = NULL;

	m_hLastWnd = NULL;

	m_hParentMenu = NULL;

	m_tbclick = NULL;

	m_dwEmptyLimit = 0;

	m_dwShading = 2;

	m_dwLine = 0;

	m_hTick = NULL;
	m_hCheck = NULL;
	m_hUncheck = NULL;
	m_hHotCheck = NULL;
	m_hHotUncheck = NULL;

	m_bSystemColors = TRUE;
	m_rgbMenu = GetSysColor( COLOR_MENU );
	m_rgbMenuText = GetSysColor( COLOR_MENUTEXT );
	m_rgbSel = GetSysColor( COLOR_HIGHLIGHT );
	m_rgbSelText = GetSysColor( COLOR_HIGHLIGHTTEXT );

	m_bShowRight = TRUE;
	m_bShowUp = TRUE;
}

CWinMenu::~CWinMenu()
{
	Destroy();
}

void CWinMenu::Destroy()
{
/*	while ( m_pHead != NULL )
	{
		LPMITEMINFO next = m_pHead->pNext;

		// Delete submenu if any
		if ( m_pHead->submenu != NULL )
		{	delete m_pHead->submenu;
			m_pHead->submenu = NULL;
		} // end if

		// Delete this node
		delete m_pHead;
		m_pHead = next;

	} // end while
	m_dwSize = 0;
	m_pTail = NULL;
*/
	m_mitems.Destroy();
	m_bck.Destroy();
	m_offscreen.Destroy();

	m_hCheck = NULL;
	m_hUncheck = NULL;
	m_hHotCheck = NULL;
	m_hHotUncheck = NULL;

	RedrawLastWindow();
}

BEGIN_MESSAGE_MAP(CWinMenu, CWnd)
	//{{AFX_MSG_MAP(CWinMenu)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_KEYDOWN()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_MENUCMD, OnMenuCommand )

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWinMenu message handlers

void CWinMenu::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	RECT rect;
	HDC hDC = dc.GetSafeHdc();
	GetClientRect( &rect );

	// Where will we draw?
	HDC hDraw = hDC;
	if ( m_offscreen.Create( hDC, &rect ) ) 
		hDraw = m_offscreen.GetSafeHdc();

	// Fill in menu window
	COLORREF rgbBck = m_rgbMenu;
	if ( m_bSystemColors ) rgbBck = GetSysColor( COLOR_MENU );
	
	COLORREF	rgbBckLt = ScaleColor( rgbBck, 40 );
	COLORREF	rgbBckDk = ScaleColor( rgbBck, -40 );

//	CGrDC::GradientFill( hDraw, &rect,	RGB( 255, 0, 0 ), RGB( 0, 255, 0 ),
//										RGB( 255, 0, 255 ), RGB( 0, 255, 255 ) );

	if (	m_bck.GetSafeHdc() == NULL || 
			RW( rect ) != m_bck.GetWidth() ||
			RH( rect ) != m_bck.GetHeight() )
	{
		// Create the background
		m_bck.Create( NULL, &rect );

		if ( m_dwShading == 0 )
			CGrDC::FillSolidRect( hDraw, &rect, rgbBck );
		else if ( m_dwShading == 1 )
			CGrDC::VertGradientFill( hDraw, &rect,	rgbBckLt, rgbBckDk );
		else if ( m_dwShading == 3 )
		{
			CGrDC::VertGradientFill(	m_bck, m_bck, 
										CGrDC::ScaleColor( GetSysColor( COLOR_3DFACE ), 50 ),
										CGrDC::ScaleColor( GetSysColor( COLOR_3DFACE ), -50 ) );
			m_bck.Aluminize( 5 );

		} // end else if
		else if ( m_dwShading == 4 )
		{
			CGrDC::VertGradientFill(	m_bck, m_bck, 
										CGrDC::ScaleColor( GetSysColor( COLOR_3DFACE ), 50 ),
										CGrDC::ScaleColor( GetSysColor( COLOR_3DFACE ), -50 ) );
			m_bck.Texturize( 10 );

		} // end else if
		else
		{	// Gradient fill
			COLORREF rgbCorner = m_rgbSel;
			if ( m_bSystemColors ) rgbCorner = GetSysColor( COLOR_ACTIVECAPTION );
			CGrDC::GradientFill( hDraw, &rect,	rgbBckLt, rgbBckDk, 
												rgbBckLt, rgbCorner );
		} // end else

		RECT brect;
		CopyRect( &brect, m_bck );

		HPEN whitepen = CreatePen( PS_SOLID, 3, ScaleColor( rgbBck, 120 ) );
		HPEN dkgraypen = CreatePen( PS_SOLID, 3, ScaleColor( rgbBck, -120 ) );
		HPEN oldpen = (HPEN)SelectObject( m_bck, whitepen );

		// Draw white line
		MoveToEx( m_bck, brect.right, brect.top, NULL );
		LineTo( m_bck, brect.left, brect.top );
		LineTo( m_bck, brect.left, brect.bottom );

		// Draw dark gray line
		SelectObject( m_bck, dkgraypen );
		MoveToEx( m_bck, brect.left, brect.bottom, NULL );
		LineTo( m_bck, brect.right, brect.bottom );
		LineTo( m_bck, brect.right, brect.top );

		SelectObject( m_bck, oldpen );
		DeleteObject( whitepen );
		DeleteObject( dkgraypen );

	} // end if

	// Draw background
	m_bck.Draw( hDraw, &rect );

	// Draw the menu items
	DrawMenuItems( hDraw, &rect );

	// Copy from offscreen
	if ( m_offscreen.GetSafeHdc() ) m_offscreen.Draw( hDC, &rect );
}


BOOL CWinMenu::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
//	return CWnd::OnEraseBkgnd(pDC);
}

#define MARGIN		15
BOOL CWinMenu::GetMenuRect(long x, long y, LPRECT pRect)
{
	if ( pRect == NULL ) return FALSE;

	long	w = 0;
	long	h = 8;
	RECT	rect;
	CText	text;
	HDC		hDC = ::GetDC( NULL );

	// Punt if no dc
	if ( hDC == NULL ) return FALSE;

	m_tw = w;
	m_th = 20;

	// What font does the user want for menu's?
	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);			
	SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( info ), &info, 0 );

	text.SetWeight( FW_BOLD );
	text.SetFont( &info.lfMenuFont );

	// Handle NULL menu
	if ( m_mitems.Size() == 0 )
	{
		// Get size of empty string
		SetRect( &rect, 0, 0, 10, 10 );
//		text.SetFlags( DT_SINGLELINE | DT_CENTER | DT_VCENTER );
		text.SetFlags( 0 );
		text.CalcRect( hDC, EMPTY_STR, &rect );

		// Save text params
		w = ( rect.right - rect.left );
		if ( rect.bottom - rect.top > 20 )
		{
			m_th = rect.bottom - rect.top;
			h += m_th;
		} else h += 20;

	} // end if

	LPMITEMINFO	pmi = NULL;
	while(	( pmi = (LPMITEMINFO)m_mitems.GetNext( pmi ) ) != NULL )
	{
		if ( *pmi->name != NULL )
		{
			// Acc. width
			SetRect( &rect, 0, 0, 10, 10 );
//			text.SetFlags( DT_SINGLELINE | DT_CENTER | DT_VCENTER );
			text.SetFlags( DT_SINGLELINE | DT_VCENTER );
			text.CalcRect( hDC, pmi->name, &rect );
			
			RECT rrect;
			if ( *pmi->rtext != 0 )
			{	SetRect( &rect, 0, 0, 10, 10 );
				text.CalcRect( hDC, pmi->rtext, &rrect );
			} // end if
			else ZeroMemory( &rrect, sizeof( rrect ) );

			long cw = ( ( rect.right - rect.left ) + MARGIN );
			
			// Add toolbar size
			if ( pmi->toolbar != NULL ) cw += pmi->toolbar->GetWidth();

			// Track width
			if ( cw > w ) w = cw;

			if ( ( rect.bottom - rect.top ) > 20 )
			{	m_th = rect.bottom - rect.top;
				h += m_th;
			} // end if
			else h += 20;

		} // end if
		else h += 6;

	} // end while

	// Correct width
	w += ( m_th * 3 );
	if ( w < 80 ) w = 80;

	m_tw = w;

	::ReleaseDC( NULL, hDC );

	pRect->left = x;
	pRect->right = x + w;
	pRect->top = y;
	pRect->bottom = y + h;

	// Correct if drifting offscreen
	long ox = 0, oy = 0;
	if ( pRect->left < 10 ) ox = 10 - pRect->left;
	if ( pRect->top < 10 ) oy = 10 - pRect->top;

	// Offset rect if needed
	if ( ox || oy ) OffsetRect( pRect, ox, oy );
		
	return TRUE;
}

LPMITEMINFO CWinMenu::AddMenuItem(	LPCTSTR pName, DWORD dwCmd, HICON hIcon, HCURSOR hCursor, LPBOOL pBool,
									LPCTSTR pRText, HICON *pRIcons, DWORD dwRIcons )
{
//	LPMITEMINFO	node = new MITEMINFO;
//	if ( node == NULL ) return NULL;

	LPMITEMINFO	node = (LPMITEMINFO)m_mitems.New( NULL, 0, pName );
	if ( node == NULL ) return NULL;

	// Save item information
	if ( pName != NULL ) { strcpy_sz( node->name, pName ); }
	else *node->name = 0x0;
	node->cmd = dwCmd;
	node->icon = hIcon;
	node->bmp = NULL;
	node->cursor = hCursor;
	node->b = pBool;
	if ( pRText != NULL ) { strcpy_sz( node->rtext, pRText ); }
	else *node->rtext = 0x0;
	if ( pRIcons != NULL && dwRIcons > 0 )
		for ( DWORD i = 0; i < dwRIcons && i < 16; i++ )
			node->ricon[ i ] = pRIcons[ i ];

	// Is this a sub menu?
	if ( dwCmd == MAXDWORD ) node->submenu = new CWinMenu();
	else node->submenu = NULL;

	return node;
}

BOOL CWinMenu::RemoveMenuItem(LPMITEMINFO node)
{
	m_mitems.Delete( node );
	return TRUE;
}

BOOL CWinMenu::DrawMenuItems(HDC hDC, LPRECT pRect)
{
	if ( hDC == NULL || pRect == NULL ) return FALSE;

	BOOL bFirst = m_bFirstDraw;
	m_bFirstDraw = FALSE;

	CText		text;
	long		x = pRect->left + 4;
	long		y = pRect->top + 4;
	long		w = pRect->right - pRect->left - 8;

	long		bx = x;
	long		by = y;

	// Set colors
	COLORREF rgbMenu, rgbMenuText, rgbSel, rgbSelText;

	if ( m_bSystemColors )
	{	rgbMenu = GetSysColor( COLOR_MENU );
		rgbMenuText = GetSysColor( COLOR_MENUTEXT );
		rgbSel = GetSysColor( COLOR_HIGHLIGHT );
		rgbSelText = GetSysColor( COLOR_HIGHLIGHTTEXT );
	} // end if
	else
	{	rgbMenu = m_rgbMenu;
		rgbMenuText = m_rgbMenuText;
		rgbSel = m_rgbSel;
		rgbSelText = m_rgbSelText;
	} // end else


	COLORREF	rgbLightPen = ScaleColor( rgbMenu, 100 );
	COLORREF	rgbDarkPen = ScaleColor( rgbMenu, -150 );

	COLORREF	rgbBck = rgbMenu;
	COLORREF	rgbBckLt = ScaleColor( rgbBck, 40 );
	COLORREF	rgbBckDk = ScaleColor( rgbBck, -40 );
	COLORREF	rgbSelLt = ScaleColor( rgbSel, 80 );
	COLORREF	rgbSelDk = ScaleColor( rgbSel, -120 );
	COLORREF	rgbSelTextLt = ScaleColor( rgbSelText, 80 );
	COLORREF	rgbSelTextDk = ScaleColor( rgbSelText, -150 );
	COLORREF	rgbBumpLt = ScaleColor( rgbBck, 120 );
	COLORREF	rgbBumpDk = ScaleColor( rgbBck, -120 );

	COLORREF	rgbText = rgbMenuText;
	COLORREF	rgbTextLt = ScaleColor( rgbBck, 100 );
	COLORREF	rgbTextDk = ScaleColor( rgbBck, -100 );

	if ( GetColorAvg( rgbSel ) > 128 )
	{	COLORREF swap = rgbSelTextLt;
		rgbSelTextLt = rgbSelTextDk; rgbSelTextDk = swap;
	} // end if


	// What font does the user want for menu's?
	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);			
	SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( info ), &info, 0 );

	// Check for empty menu	
	if ( m_mitems.Size() == 0 )
	{
		COLORREF rgbEmptyLt = ScaleColor( rgbBck, 120 );
		COLORREF rgbEmptyDk = ScaleColor( rgbBck, -120 );

		if ( GetColorAvg( rgbBck ) < 128 ) 
		{	COLORREF swap = rgbEmptyLt;
			rgbEmptyLt = rgbEmptyDk; rgbEmptyDk = swap;
		} // end if

		RECT t;
		CopyRect( &t, pRect );

		text.SetWeight( FW_BOLD );
		text.SetFont( &info.lfMenuFont );
		text.SetFlags( DT_SINGLELINE | DT_CENTER | DT_VCENTER );

		// Make text look recessed
		OffsetRect( &t, -1, -1 );
		text.SetColor( rgbEmptyDk );
		text.DrawText( hDC, EMPTY_STR, &t );

		OffsetRect( &t, 1, 1 );
		text.SetColor( rgbEmptyLt );
		text.DrawText( hDC, EMPTY_STR, &t );

	} // end if
	
	
	// This kinda bites, but we need to know how far to offset the text
	BOOL islots = 0;
	LPMITEMINFO	pmi = NULL;
	while(	islots < 2 && ( pmi = (LPMITEMINFO)m_mitems.GetNext( pmi ) ) != NULL )
	{
		// Do we need both icon slots?
		if ( pmi->icon != NULL && pmi->b != NULL ) islots = 2;

		// Do we have one islot already?
		else if ( islots < 1 )
		{
			if ( pmi->icon != NULL ) islots = 1;
			
			else if ( pmi->b != NULL && m_hCheck == NULL && m_hUncheck == NULL )
				islots = 1;
		} // end else if

	} // end while

	
	// The icon size
	long iconsize = m_th - 4;

	// Draw each item
	pmi = NULL;
	while(	( pmi = (LPMITEMINFO)m_mitems.GetNext( pmi ) ) != NULL &&
			x < pRect->right && y < pRect->bottom )
	{
		// Set base coords
		bx = x; by = y;

		if ( *pmi->name != NULL )
		{
			POINT	pt;
			GetCursorPos( &pt );
			ScreenToClient( &pt );

			// Is this item being selected?
			BOOL bSelected = FALSE;
			RECT hl;
			hl.left = pRect->left + 3;
			hl.right = pRect->right - 4;
			hl.top = by;
			hl.bottom = by + m_th;
//			if (	pt.x > hl.left && pt.x < hl.right &&
//					pt.y > hl.top && pt.y < hl.bottom )

			// Add for toolbar
			if ( pmi->toolbar != NULL && pmi->toolbar->GetNumButtons() )
				hl.left += pmi->toolbar->GetWidth();


			if ( pmi == m_itemover )
			{
				bSelected = TRUE;
//				HPEN whitepen = CreatePen( PS_SOLID, 1, RGB( 255, 255, 255 ) );
//				HPEN dkgraypen = CreatePen( PS_SOLID, 1, RGB( 50, 50, 50 ) );
				HPEN whitepen = CreatePen( PS_SOLID, 1, rgbLightPen );
				HPEN dkgraypen = CreatePen( PS_SOLID, 1, rgbDarkPen );
				HPEN oldpen = (HPEN)SelectObject( hDC, whitepen );

				// Draw white line
				MoveToEx( hDC, hl.right, hl.top, NULL );
				LineTo( hDC, hl.right, hl.bottom  );
				LineTo( hDC, hl.left, hl.bottom  );

				// Draw dark gray line
				SelectObject( hDC, dkgraypen );
				MoveToEx( hDC, hl.right, hl.top, NULL );
				LineTo( hDC, hl.left, hl.top  );
				LineTo( hDC, hl.left, hl.bottom  );

				SelectObject( hDC, oldpen );
				DeleteObject( whitepen );
				DeleteObject( dkgraypen );

//				InflateRect( &hl, -1, -1 );
				hl.left += 1;
				hl.top += 1;

//				GradientFill( hDC, &hl, RGB( 170, 170, 190 ), RGB( 110, 110, 110 ) );
				CGrDC::VertGradientFill( hDC, &hl, rgbSelLt, rgbSelDk );
/*				HBRUSH brush = CreateSolidBrush( RGB( 120, 120, 120 ) );
				FillRect( hDC, &hl, brush );
				DeleteObject( brush );
*/

			} // end if

			// Draw toolbar
			if ( pmi->toolbar != NULL && pmi->toolbar->GetNumButtons() )
			{	pmi->toolbar->SetHeight( m_th );

				RECT tbar;
				SetRect( &tbar, bx, by, bx + pmi->toolbar->GetWidth(), 
									by + pmi->toolbar->GetHeight() );

				if ( bFirst ) pmi->toolbar->CreateToolTips( GetSafeHwnd(), &tbar );
				pmi->toolbar->SetMessageTarget( GetSafeHwnd(), WM_MENUCMD );
				pmi->toolbar->Draw( GetSafeHwnd(), hDC, &tbar );
			
				bx += pmi->toolbar->GetWidth();
			} // end if

			RECT t;

			// Calc text box
			t.left = bx + 4;
			t.left += islots * iconsize;
			t.right = pRect->right;
			t.top = by;
			t.bottom = by + m_th;

			// Draw Check
			if ( pmi->b != NULL )
			{
				RECT icon;
				SetRect( &icon, bx + 2, by + 2, bx + 2 + iconsize, by + 2 + iconsize );

				if ( *pmi->b != FALSE )
				{
					if ( bSelected && m_hHotCheck != NULL )
						CGrDC::DrawIcon( hDC, &icon, m_hHotCheck );
//						DrawIconEx( hDC, bx + 2, by + 2, m_hHotCheck, iconsize, iconsize, 0, 0, DI_NORMAL );

					else if ( m_hCheck != NULL )
						CGrDC::DrawIcon( hDC, &icon, m_hCheck );
//						DrawIconEx( hDC, bx + 2, by + 2, m_hCheck, iconsize, iconsize, 0, 0, DI_NORMAL );
				} // end if
				else 
				{
					if ( bSelected && m_hHotUncheck != NULL )
						CGrDC::DrawIcon( hDC, &icon, m_hHotUncheck );
//						DrawIconEx( hDC, bx + 2, by + 2, m_hHotUncheck, iconsize, iconsize, 0, 0, DI_NORMAL );

					else if ( m_hUncheck != NULL )
						CGrDC::DrawIcon( hDC, &icon, m_hUncheck );
//						DrawIconEx( hDC, bx + 2, by + 2, m_hUncheck, iconsize, iconsize, 0, 0, DI_NORMAL );
				} // end else
			} // end if

			// Draw icon
			if ( pmi->icon != NULL ) 
			{
				long xoff = 2;
				xoff += iconsize * ( islots - 1 ) + 1;


//				DrawIconEx( hDC, x + xoff, y + 2, pmi->icon,
//							16, 16, 0, 0, DI_NORMAL );

				DrawIconEx( hDC, bx + xoff, by + 2, pmi->icon,
							iconsize, iconsize, 0, 0, DI_NORMAL );



			} // end if

			// Setup the text object
			text.SetFlags( DT_SINGLELINE | DT_VCENTER );

			if ( bSelected )
			{
				text.SetWeight( FW_BOLD );
				text.SetFont( &info.lfMenuFont );
	
				// Make text look recessed
				OffsetRect( &t, -1, -1 );
//				text.SetColor( RGB( 0, 0, 0 ) );
				text.SetColor( rgbSelTextDk );
				text.DrawText( hDC, pmi->name, &t );

				OffsetRect( &t, 1, 1 );
				text.SetColor( rgbSelTextLt );
			} // end if
			else
			{
//				text.SetItalic( FALSE );
				text.SetWeight( FW_BOLD );
				text.SetFont( &info.lfMenuFont );
//				text.SetFont( TFONTSIZE, TFONTTYPE );
//				text.SetColor( RGB( 0, 50, 128 ) );
				text.SetColor( rgbText );
			} // end else
//			else text.SetColor( rgbText );
//			text.SetColor( RGB( 0, 50, 128 ) );
			text.DrawText( hDC, pmi->name, &t );

			// Draw sub menu indicator
			if ( pmi->submenu != NULL )
			{
				// Do we have a tick mark icon?
				if ( m_hTick != NULL )
				{
					RECT ic;
					SetRect( &ic, t.right - 18, t.top + 5, t.right - 8, t.bottom - 7 );

					// Colorize tick icon
					HICON hMono = NULL;
					if ( !pmi->submenu->IsEmpty() ) 
						hMono = CGrDC::CreateMonoChromeIcon( m_hTick, GetSysColor( COLOR_ACTIVECAPTION ) );
					else hMono = CGrDC::CreateMonoChromeIcon( m_hTick, GetSysColor( COLOR_INACTIVECAPTION ) );
//						hMono = CGrDC::CreateMonoChromeIcon( m_hTick, rgbSel );
//					else hMono = CGrDC::CreateMonoChromeIcon( m_hTick, rgbBck );

					// Draw the icon
					if ( hMono != NULL )
					{	
						CGrDC::DrawIcon( hDC, &ic, hMono );
						DestroyIcon( hMono );
					} // end if

				} // end if
				else
				{
					HPEN whitepen = CreatePen( PS_SOLID, 1, rgbLightPen );
					HPEN dkgraypen = CreatePen( PS_SOLID, 1, ScaleColor( rgbBck, -10 ) );
					HPEN blackpen = CreatePen( PS_SOLID, 1, ScaleColor( rgbBck, -40 ) );
					HPEN oldpen = (HPEN)SelectObject( hDC, whitepen );				
					HBRUSH mbrush;
					if ( !pmi->submenu->IsEmpty() ) mbrush = CreateSolidBrush( rgbBck );
					else mbrush = CreateSolidBrush( ScaleColor( rgbBck, -80 ) );
					HBRUSH oldbrush = (HBRUSH)SelectObject( hDC, mbrush );

					POINT	pts[ 3 ];

					pts[ 0 ].x = t.right - 14;
					pts[ 0 ].y = t.top + 5;
					pts[ 1 ].x = t.right - 14;
					pts[ 1 ].y = t.bottom - 7;
					pts[ 2 ].x = t.right - 8;
					pts[ 2 ].y = t.top + ( ( t.bottom - t.top ) / 2 );

					// Draw the shape
					Polygon( hDC, pts, sizeof( pts ) / sizeof( POINT ) );

					// Draw border
					MoveToEx( hDC, pts[ 0 ].x, pts[ 0 ].y, NULL );
					LineTo( hDC, pts[ 1 ].x, pts[ 1 ].y );
					SelectObject( hDC, blackpen );
					LineTo( hDC, pts[ 2 ].x, pts[ 2 ].y );
					SelectObject( hDC, dkgraypen );
					LineTo( hDC, pts[ 0 ].x, pts[ 0 ].y ); 


					// Release drawing objects
					SelectObject( hDC, oldpen );
					DeleteObject( whitepen );
					DeleteObject( dkgraypen );
					DeleteObject( blackpen );
					SelectObject( hDC, oldbrush );
					DeleteObject( mbrush );

				} // end if

			} // end if

			// Next menu item position
			y += m_th;
		} // end if

		else // separator
		{
//			HPEN whitepen = CreatePen( PS_SOLID, 1, RGB( 255, 255, 255 ) );
//			HPEN dkgraypen = CreatePen( PS_SOLID, 1, RGB( 50, 50, 50 ) );
			HPEN whitepen = CreatePen( PS_SOLID, 1, rgbBumpLt );
			HPEN dkgraypen = CreatePen( PS_SOLID, 1, rgbBumpDk );
			HPEN oldpen = (HPEN)SelectObject( hDC, whitepen );

			// Draw white line
			MoveToEx( hDC, bx + 2, by + 2, NULL );
			LineTo( hDC, ( pRect->right - pRect->left ) - 2, by + 2 );

			// Draw dark gray line
			SelectObject( hDC, dkgraypen );
			MoveToEx( hDC, bx + 2, by + 3, NULL );
			LineTo( hDC, ( pRect->right - pRect->left ) - 2, by + 3 );

			SelectObject( hDC, oldpen );
			DeleteObject( whitepen );
			DeleteObject( dkgraypen );

			y += 6;

		} // end else

	} // end while

	return TRUE;
}

void CWinMenu::OnMouseMove(UINT nFlags, CPoint point) 
{
	LPMITEMINFO	item = GetMenuItem( &point );

	BOOL bRedraw = FALSE;

	// Redraw window if needed by toolbar
	if (	item != NULL && item->toolbar != NULL && 
			item->toolbar->OnMouseMove( nFlags, &point ) )
		bRedraw = TRUE;

	if ( ( nFlags & MK_LBUTTON ) == 0 )
	{	

		if ( item != m_itemover )
		{
			// Clear mouse over
			if ( m_itemover != NULL && m_itemover->toolbar != NULL )
				m_itemover->toolbar->ClearMouseOver();

			// Save new item
			m_itemover = item;

			// Popup sub menu if needed
			NewSubMenu( item );

			// Redraw our window
			bRedraw = TRUE;

		} // end if

	} // end if

	else
	{
		RECT c;
		GetWindowRect( &c );

		if ( !PtInRect( &c, point ) )
		{
			HWND last = m_hDropWnd;

			POINT pt;
			pt.x = point.x;
			pt.y = point.y;
			ClientToScreen( &pt );

			// Which window was choosen
			HWND hParent = ::WindowFromPoint( pt );
			if ( m_bAllowChildren )
			{
				if ( hParent != NULL )
				{
					// Check for child windows
					::ScreenToClient( hParent, &pt );
					HWND hChild = ::ChildWindowFromPoint( hParent, pt );
					
					// Which window to use
					if ( hChild != NULL ) m_hDropWnd = hChild;
					else m_hDropWnd = hParent;
						
				} // end if
			} // end if
			else
			{
				// Get topmost window
				while ( hParent != NULL )
				{
					m_hDropWnd = hParent;
					hParent = ::GetParent( hParent );
				} // end while

			} // end else

			// Border window
			if ( last != m_hDropWnd ) 
			{
				// Undo our mess
				RedrawLastWindow();

				if ( m_hDropWnd != GetSafeHwnd() && ::IsWindow( m_hDropWnd ) )
				{
					HDC hDC = ::GetWindowDC( m_hDropWnd );

					if ( hDC != NULL )
					{
						// Save last window we screwed up
						m_hLastWnd = m_hDropWnd;

						RECT rect;
						::GetWindowRect( m_hDropWnd, &rect );
						OffsetRect( &rect, -rect.left, -rect.top );

						HBRUSH oldbrush = (HBRUSH)SelectObject( hDC, GetStockObject( NULL_BRUSH ) );
						HPEN red = CreatePen( PS_DASH, 2, RGB( 255, 0, 0 ) );
						HPEN white = CreatePen( PS_DASH, 2, RGB( 80, 80, 80 ) );
						HPEN blue = CreatePen( PS_DASH, 2, RGB( 0, 0, 255 ) );
						HPEN oldpen = (HPEN)SelectObject( hDC, red );

						// Draw the rectangle
//						Rectangle( hDC, rect.left, rect.top, rect.right, rect.bottom );

						InflateRect( &rect, -2, -2 );
						SelectObject( hDC, red );
						Rectangle( hDC, rect.left, rect.top, rect.right, rect.bottom );

						InflateRect( &rect, -2, -2 );
						SelectObject( hDC, blue );
						Rectangle( hDC, rect.left, rect.top, rect.right, rect.bottom );

						// Restore the DC
						SelectObject( hDC, oldpen );
						SelectObject( hDC, oldbrush );
						DeleteObject( red );
						DeleteObject( white );
						DeleteObject( blue );
						::ReleaseDC( m_hDropWnd, hDC );
					} // end if
				} // end if
			} // end if

		} // end if

	} // end else if
	
	// Redraw window if needed
	if ( bRedraw ) RedrawWindow();

	CWnd::OnMouseMove(nFlags, point);
}



void CWinMenu::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);

	// Ignore refocus
	if ( pNewWnd == this ) return;

	else if ( pNewWnd != NULL )
	{
		// Is our parent getting the focus?
		if ( pNewWnd->GetSafeHwnd() == m_hParentMenu )
			return;

		// Is it one of our child windows getting the focus?
		LPMITEMINFO		pmi = NULL;
		while ( ( pmi = (LPMITEMINFO)m_mitems.GetNext( pmi ) ) != NULL )
			if ( pmi->submenu != NULL )
				if ( pmi->submenu->GetSafeHwnd() == pNewWnd->GetSafeHwnd() )
					return;

	} // end else if

	// Let the parent do it
	if ( m_hParentMenu != NULL )			
		::PostMessage( m_hParentMenu, WM_MENUCMD, 0, 0L );
	else DestroyWindow();
}

void CWinMenu::OnLButtonDown(UINT nFlags, CPoint point) 
{
	BOOL bCmdArea = FALSE;
	LPMITEMINFO	pmi = GetMenuItem( &point, &bCmdArea );

	// Check with toolbar
	if (	pmi != NULL && pmi->toolbar != NULL && 
			pmi->toolbar->OnLButtonDown( nFlags, &point ) ) 
	{	m_tbclick = pmi; pmi = NULL; RedrawWindow(); }
	else m_tbclick = NULL;

	// Clear if not in command area
	if ( !bCmdArea ) pmi = NULL;

	// Process if object clicked
	if ( pmi != NULL && *pmi->name != NULL )
	{
		if ( pmi->cursor != NULL )
		{
			m_menuitem = pmi;
			SetCapture();
			SetCursor( pmi->cursor );
		} // end if

		// Let the parent window handle it if any
		else if ( m_hParentMenu != NULL )
		{
			// Do they expect us to toggle the flag?
			if ( pmi->cmd == 0 && pmi->b != NULL )
				*pmi->b = !(*pmi->b);

			// Let the parent send the command
			::PostMessage( m_hParentMenu, WM_MENUCMD, (WPARAM)pmi->cmd, (LPARAM)m_hWndMsgTarget );

		} // end else if

		// Ignore clicks on sub menu
		else if ( pmi->cmd == MAXDWORD );

		// Send message if there is a target
		else if ( pmi->cmd != 0 && m_hWndMsgTarget != NULL && IsWindow( m_hWndMsgTarget ) )
		{
			::PostMessage( m_hWndMsgTarget, WM_COMMAND, pmi->cmd, NULL );
//			DestroyWindow();
			PostMessage( WM_CLOSE, 0, 0L );

		} // end if

//		else DestroyWindow();

	} // end if
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CWinMenu::OnLButtonUp(UINT nFlags, CPoint point) 
{
	BOOL bCmdArea = FALSE;
	LPMITEMINFO	pmi = GetMenuItem( &point, &bCmdArea );
	if ( pmi == NULL ) return;

	// Call up on toolbar
	if ( m_tbclick != NULL ) 
	{
		if ( m_tbclick->toolbar->OnLButtonUp( nFlags, &point ) )
			RedrawWindow(); 
	} // end if

	// Punt if not in command area
	if ( !bCmdArea ) return;

	// Release capture
	ReleaseCapture();
	
	RedrawLastWindow();
	
	// Send message if needed
	if ( m_menuitem != NULL )
	{
		POINT pt;
		pt.x = point.x;
		pt.y = point.y;
		ClientToScreen( &pt );

		// Which window was choosen
		HWND hParent = ::WindowFromPoint( pt );
		if ( m_bAllowChildren )
		{
			if ( hParent != NULL )
			{
				// Check for child windows
				::ScreenToClient( hParent, &pt );
				HWND hChild = ::ChildWindowFromPoint( hParent, pt );
				
				// Which window to use
				if ( hChild != NULL ) m_hDropWnd = hChild;
				else m_hDropWnd = hParent;
					
			} // end if
		} // end if
		else
		{
			// Get topmost window
			while ( hParent != NULL )
			{
				m_hDropWnd = hParent;
				hParent = ::GetParent( hParent );
			} // end while

		} // end else

		// Send message if there is a target
		if ( m_menuitem->cmd != 0 && m_hWndMsgTarget != NULL )
		{
			::PostMessage( m_hWndMsgTarget, WM_COMMAND, m_menuitem->cmd, 0L );
		} // end if
		m_menuitem = NULL;
		DestroyWindow();
	} // end if

	CWnd::OnLButtonUp(nFlags, point);
}

LPMITEMINFO CWinMenu::GetMenuItem(LPPOINT pPt, LPBOOL bCmdArea)
{
	RECT rect;
	GetClientRect( &rect );

	long	x = rect.left + 4;
	long	y = rect.top + 4;

	LPMITEMINFO	pmi = NULL;
	while(	( pmi = (LPMITEMINFO)m_mitems.GetNext( pmi ) ) != NULL )
	{
		RECT hl;
		hl.left = rect.left + 2;
		hl.right = rect.right - 2;
		hl.top = y;

		if ( *pmi->name != NULL ) hl.bottom = y + 20;
		else hl.bottom = y + 6;

		// Is this item being selected
		if (	pPt->x >= hl.left && pPt->x <= hl.right &&
				pPt->y >= hl.top && pPt->y <= hl.bottom )
		{
			if ( bCmdArea != NULL )
			{	if ( pmi->toolbar == NULL ) *bCmdArea = TRUE;
				else if ( pPt->x > ( hl.left + 2 + pmi->toolbar->GetWidth() ) )
					*bCmdArea = TRUE;				
			} // end if

			return pmi;
		} // end if

		if ( *pmi->name != NULL ) y += m_th;
		else y += 6;
		
	} // end while

	return NULL;
}

BOOL CWinMenu::GetItemRect(LPMITEMINFO item, LPRECT pRect)
{
	// Sanity check
	if ( item == NULL || pRect == NULL ) return FALSE;

	RECT rect;
	GetClientRect( &rect );

	long	x = rect.left + 4;
	long	y = rect.top + 4;

	LPMITEMINFO	pmi = NULL;
	while(	( pmi = (LPMITEMINFO)m_mitems.GetNext( pmi ) ) != NULL )
	{
		RECT hl;
		hl.left = rect.left + 2;
		hl.right = rect.right - 2;
		hl.top = y;
		if ( *pmi->name != NULL ) hl.bottom = y + 20;
		else hl.bottom = y + 6;

		// Is this our rect?
		if ( pmi == item )
		{	CopyRect( pRect, &hl );
			return TRUE;
		} // end if

		if ( *pmi->name != NULL ) y += m_th;
		else y += 6;
		
	} // end while

	return FALSE;
}


int CWinMenu::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Set a timer
	m_itemover = NULL;
	m_uTimerID = SetTimer( TIMERID, 100, NULL );
	m_dwDestroyDelay = 0;
	
	return 0;
}

void CWinMenu::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent == TIMERID )
	{
		POINT	pt;
		RECT	rect;
		GetCursorPos( &pt );
		GetWindowRect( &rect );

		if (	pt.x < rect.left || pt.x > rect.right ||
				pt.y < rect.top || pt.y > rect.bottom )
		{
			if ( m_dwDestroyDelay > 3000 )
				DestroyWindow();
			else m_dwDestroyDelay++;
		} // end if
		else m_dwDestroyDelay = 0;


	} // end if	

	CWnd::OnTimer(nIDEvent);
}

BOOL CWinMenu::DestroyWindow() 
{
	RedrawLastWindow();
	m_itemover = NULL;
	KillTimer( m_uTimerID );	
	KillSubMenus();

	// Lose the offscreen buffer
	m_offscreen.Destroy();

	return CWnd::DestroyWindow();
}

void CWinMenu::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnRButtonDown(nFlags, point);

	BOOL bCmdArea = FALSE;
	LPMITEMINFO	pmi = GetMenuItem( &point, &bCmdArea );
	if ( !bCmdArea ) pmi = NULL;

	// RButton sends command without killing the menu
	if ( pmi != NULL && pmi->cursor == NULL )
	{	
		if ( pmi->cmd != 0 && m_hWndMsgTarget != NULL )
		{
			// If bool we must wait to see if it changes
			// Callee must not create dialogs etc...
			if ( pmi->b != NULL )
			{	::SendMessage( m_hWndMsgTarget, WM_COMMAND, pmi->cmd, 0L );
				if ( ::IsWindow( GetSafeHwnd() ) ) RedrawWindow( NULL );
			} // end if

			// Just post the message
			else
			{	/*DestroyWindow();*/
				::PostMessage( m_hWndMsgTarget, WM_COMMAND, pmi->cmd, 0L );
			} // end else

		} // end if

		// Do they expect us to toggle the flag?
		else if ( pmi->b != NULL )
		{	*pmi->b = !(*pmi->b);
			if ( ::IsWindow( GetSafeHwnd() ) ) RedrawWindow( NULL );
		} // end else if

	} // end if

}

void CWinMenu::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CWnd::OnRButtonUp(nFlags, point);
}

void CWinMenu::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);

	m_menuitem = NULL;
	DestroyWindow();
}

void CWinMenu::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);

//	m_menuitem = NULL;
//	DestroyWindow();
}

COLORREF CWinMenu::ScaleColor(COLORREF col, long scale)
{
	// Are we scaling?
	if ( scale == 0 ) return col;

	// Get color components
	long r = GetRValue( col );
	long g = GetGValue( col );
	long b = GetBValue( col );

	// Fix dark problem
	if ( scale > 0 )
		if ( r < scale && g < scale && b < scale )
			r += scale, g += scale, b += scale;

	// Scale values
	r += ( scale * r ) >> 8;
	g += ( scale * g ) >> 8;
	b += ( scale * b ) >> 8;


	// Bounds check
	if ( r > 255 ) r = 255; if ( r < 0 ) r = 0;
	if ( g > 255 ) g = 255; if ( g < 0 ) g = 0;
	if ( b > 255 ) b = 255; if ( b < 0 ) b = 0;

	// Cool effect
//	r &= 0xff;
//	g &= 0xff;
//	b &= 0xff;

	return ( RGB( r, g, b ) );
}

void CWinMenu::RedrawLastWindow()
{
	// Redraw last window
	if ( m_hLastWnd != NULL && ::IsWindow( m_hLastWnd ) )
		::RedrawWindow( m_hLastWnd, NULL, NULL, RDW_UPDATENOW | RDW_INVALIDATE | 
						RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN );

	m_hLastWnd = NULL;
}

void CWinMenu::KillSubMenus()
{
	LPMITEMINFO		pmi = NULL;

	while ( ( pmi = (LPMITEMINFO)m_mitems.GetNext( pmi ) ) != NULL )
	{
		// Lose sub menu if any
		if ( pmi->submenu != NULL )
		{
			// Kill the menu
			if ( ::IsWindow( pmi->submenu->GetSafeHwnd() ) )
				pmi->submenu->DestroyWindow();

			// Lose the offscreen buffer
			pmi->submenu->m_offscreen.Destroy();


		} // end if
	} // end while
}

BOOL CWinMenu::NewSubMenu(LPMITEMINFO pmi)
{
	// Lose previous popups
	KillSubMenus();

	// Punt if no sub menu
	if ( pmi == NULL || pmi->submenu == NULL ) return FALSE;

	// Get item rect
	RECT avoid;
	GetItemRect( pmi, &avoid );
	ClientToScreen( &avoid );

	// Where do we want the menu?
	RECT rect;
	pmi->submenu->GetMenuPosition( &avoid, &rect );

	// Set us as the parent menu
	pmi->submenu->SetParentMenu( GetSafeHwnd() );

	pmi->submenu->m_bFirstDraw = TRUE;

	// Create sub menu
	if ( !::IsWindow( pmi->submenu->GetSafeHwnd() ) )
		pmi->submenu->CreateEx(	WS_EX_TOPMOST,
								NULL, 
								"Menu", 
								WS_POPUP | WS_CHILD | WS_CLIPCHILDREN,
								rect.left, rect.top,
								rect.right - rect.left, 
								rect.bottom - rect.top,
								GetSafeHwnd(),
								NULL, NULL );

	// Set message target for sub menu
	pmi->submenu->SetMessageTarget( m_hWndMsgTarget );

	// Create tooltips
//	CreateToolTips( pmi->submenu, &rect );

	// Set colors
	pmi->submenu->SetShading( m_dwShading );
	pmi->submenu->SetColors( m_rgbMenu, m_rgbMenuText, m_rgbSel, m_rgbSelText );

	// Show the menu
	pmi->submenu->ShowWindow( SW_SHOWNORMAL );
	pmi->submenu->SetForegroundWindow();
	pmi->submenu->SetFocus();

	return TRUE;

/*	// We want commands
	m_menu.SetMessageTarget( GetSafeHwnd() );

	// Show the menu
	m_menu.ShowWindow( SW_SHOWNORMAL );
	m_menu.SetForegroundWindow();
	m_menu.SetFocus();
*/
}


LRESULT CWinMenu::OnMenuCommand(WPARAM wParam, LPARAM lParam)
{
	// Relay to parent if any
	if ( m_hParentMenu != NULL )
		::PostMessage( m_hParentMenu, WM_MENUCMD, wParam, lParam );

	// Is it a valid command?
	else if ( wParam != 0 && lParam != NULL )
	{
		::PostMessage( (HWND)lParam, WM_COMMAND, wParam, 0L );
		DestroyWindow();
	} // end else

	// Send to our parent?
	else if ( wParam != 0 )
	{
		::PostMessage( m_hWndMsgTarget, WM_COMMAND, wParam, 0L );
		DestroyWindow();
	} // end else if

	else DestroyWindow();

	return 1;
}

BOOL CWinMenu::GetMenuPosition(LPRECT pAvoid, LPRECT pRect)
{
	// Sanity check
	if ( pAvoid == NULL || pRect == NULL ) return FALSE;
	
	// Get screen coords
	long mx = 10;
	long my = 10;
	long sx = GetSystemMetrics( SM_CXSCREEN ) - 10;
	long sy = GetSystemMetrics( SM_CYSCREEN ) - 10;

	// Get sub menu size
	RECT	rect;
	GetMenuRect( 0, 0, &rect );

	// Offset from cursor
	long ox = pAvoid->right == pAvoid->left ? 0 : 1;
	long oy = pAvoid->top == pAvoid->bottom ? 0 : 1;

	// Where do we want the menu ?
	long x, y;

	// right or left
	if ( m_bShowRight ) x = pAvoid->right - ox;
	else x = pAvoid->right - RW( rect ) + ox;

	// up or down?
	if ( m_bShowUp ) y = pAvoid->bottom - ( rect.bottom - rect.top ) + oy;
	else y = pAvoid->bottom - oy;

	// Check for overhang
	if ( ( x + ( rect.right - rect.left ) ) > sx )
		x = pAvoid->left - ( rect.right - rect.left ) + ox;
	if ( y < 0 ) y = pAvoid->top - oy;

	// Get the final position
	GetMenuRect( x, y, pRect );

	long offx = 0, offy = 0;

	// Overhangging bottom?
	if ( pRect->right > sx ) offx = sx - pRect->right;
	if ( pRect->bottom > sy ) offy = sy - pRect->bottom;

	// Overhangging top?
	if ( ( pRect->left + offx ) < mx ) offx += mx - ( pRect->left + offx );
	if ( ( pRect->top + offy ) < mx ) offy += mx - ( pRect->top + offy );

	// Correct menu position if needed
	if ( offx != 0 || offy != 0 ) OffsetRect( pRect, offx, offy );

	return TRUE;
}

BOOL CWinMenu::ShowMenu(HWND hParent, long x, long y)
{
	RECT	rect;
	GetMenuPosition( x, y, &rect );

	m_bFirstDraw = TRUE;

	// Create the window
	if ( !::IsWindow( GetSafeHwnd() ) )
		CreateEx(	WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
					NULL, 
					"Menu", 
					WS_POPUP | WS_CHILD | WS_CLIPCHILDREN,
					rect.left, rect.top,
					rect.right - rect.left, 
					rect.bottom - rect.top,
					GetSafeHwnd(),
					NULL, NULL );

	// Did we get a window?
	if ( !::IsWindow( GetSafeHwnd() ) ) return FALSE;

	// We're want commands
	SetMessageTarget( hParent );

	// Create tooltips
//	CreateToolTips( NULL, &rect );

	// Show the menu
	ShowWindow( SW_SHOWNORMAL );
	SetForegroundWindow();
	SetFocus();

	return TRUE;
}


void CWinMenu::Sort()
{
	m_mitems.QuickSort();
}

CWndToolbar* CWinMenu::GetItemToolbar( LPMITEMINFO ptr )
{
	if ( ptr != NULL && ptr->toolbar != NULL ) return ptr->toolbar; 
		else return ( ptr->toolbar = new CWndToolbar ); 
}

/*
BOOL CWinMenu::CreateToolTips( CWinMenu *pMenu, LPRECT pRect )
{
	// Get menu pointer
	if ( pMenu == NULL ) pMenu = this;

	RECT rect;
	CopyRect( &rect, pRect );
	OffsetRect( &rect, -rect.left, -rect.top );

	LPMITEMINFO	pmi = NULL;
	while(	( pmi = (LPMITEMINFO)pMenu->m_mitems.GetNext( pmi ) ) != NULL )
		if ( pmi != NULL && pmi->toolbar != NULL ) 
			pmi->toolbar->CreateToolTips( pMenu->GetSafeHwnd(), &rect );

	return TRUE;
}
*/

BOOL CWinMenu::IsMenuRight( long x, long y )
{
	RECT rect;
	GetMenuPosition( x, y, &rect );
	return ( rect.left >= x );
}

void CWinMenu::OnClose() 
{
	RedrawLastWindow();
	m_itemover = NULL;
	KillTimer( m_uTimerID );	
	KillSubMenus();

	// Lose the offscreen buffer
	m_offscreen.Destroy();
	
	CWnd::OnClose();
}
