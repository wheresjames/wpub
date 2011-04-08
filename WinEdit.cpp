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
// WinEdit.cpp : implementation file
//

#include "stdafx.h"
#include "Vp.h"
#include "WinEdit.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinEdit

CWinEdit::CWinEdit()
{
	m_pImgList = NULL;
	m_pEditRect = NULL;
	m_dwInsert = 0;
	m_pFlybyRect = NULL;
	m_fxEdit = NULL;
	m_bInserting = FALSE;
}

CWinEdit::~CWinEdit()
{
}


BEGIN_MESSAGE_MAP(CWinEdit, CWnd)
	//{{AFX_MSG_MAP(CWinEdit)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_POPUP_PROPERTIES, OnPopupProperties)
	ON_COMMAND(ID_POPUP_COPY, OnPopupCopy)
	ON_COMMAND(ID_POPUP_CUT, OnPopupCut)
	ON_COMMAND(ID_POPUP_DELETE, OnPopupDelete)
	ON_COMMAND(ID_POPUP_DUPLICATE, OnPopupDuplicate)
	ON_COMMAND(ID_POPUP_PASTE, OnPopupPaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CWinEdit message handlers

BOOL CWinEdit::Init(HWND hParent, LPRECT pRect)
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
	CreateEx(	0, AfxRegisterWndClass(	0, NULL,
										(HBRUSH)GetStockObject( BLACK_BRUSH ) ),
					NULL, WS_CHILD | WS_VISIBLE, 
					pRect->top, pRect->bottom, width, height,
					hParent, NULL );	

	DragAcceptFiles();

	return TRUE;
}

BOOL CWinEdit::Size(LPRECT pRect)
{
	// Sanity check
	if ( !::IsWindow( GetSafeHwnd() ) ) return FALSE;

	HWND hParent = ::GetParent( GetSafeHwnd() );

	// Get rect if not specified
	RECT parentrect;
	if ( pRect == NULL )
	{
		if ( !::IsWindow( hParent ) ) return FALSE;

		::GetClientRect( hParent, &parentrect );
		pRect = &parentrect;

	} // end if

	// Calc window metrics
	long width = pRect->right - pRect->left;
	long height = pRect->bottom - pRect->top;

	RECT rect;	
	SetRect( &rect, 0, 0, 320, 240 );

	if ( m_pImgList != NULL )
	{
		// Get image rect
		LPPUBIMGINFO ppii = m_pImgList->GetSelImage();
		if ( ppii != NULL ) CopyRect( &rect, &ppii->rect );

	} // end if

	// Center rect
	OffsetRect( &rect,	( width - ( rect.right - rect.left ) ) >> 1, 
						( height - ( rect.bottom - rect.top ) ) >> 1 );

	// Set the new position
	SetWindowPos( NULL, rect.left, rect.top,
						rect.right - rect.left,
						rect.bottom - rect.top,
						SWP_NOZORDER | SWP_NOACTIVATE );

	Update();

	if ( ::IsWindow( hParent ) ) ::RedrawWindow(	hParent, NULL, NULL, 
													RDW_UPDATENOW | RDW_INVALIDATE |
													RDW_ERASE | RDW_ERASENOW );

	return TRUE;
}

BOOL CWinEdit::OnEraseBkgnd(CDC* pDC) 
{
	// Get HDC
	if ( pDC == NULL ) return FALSE;
	HDC hDC = pDC->GetSafeHdc();

	RECT rect;
	GetClientRect( &rect );

	// Create offscreen buffer
	if ( m_offscreen.Create( hDC, &rect ) )
		hDC = m_offscreen;

	{ // Draw the image

		// Get image object
		LPPUBIMGINFO ppii = NULL;
		if ( m_pImgList != NULL ) ppii = m_pImgList->GetSelImage();

		// Fill solid color if no image
		if ( ppii == NULL || ppii->os == NULL )
			CGrDC::FillSolidRect( hDC, &rect, GetSysColor( COLOR_APPWORKSPACE ) );

		// Draw the image
		else 
		{
			// Lock the image
			CTlLocalLock ll( ppii->lock, 3000 );
			if ( ll.IsLocked() ) ppii->os->Draw( hDC, &rect );

		} // end else


	} // end draw image

	// Draw the edit rect
	if ( m_pEditRect != NULL )
		m_rmover.Draw( hDC, &rect, m_pEditRect );

	// Draw mouser over box
	else if ( m_pFlybyRect != NULL )
		m_rmover.Draw( hDC, &rect, m_pFlybyRect, FALSE );
	
	// Draw to screen
	m_offscreen.Draw( pDC->GetSafeHdc(), &rect );

	return TRUE;
//	return CWnd::OnEraseBkgnd(pDC);
}

void CWinEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();

	BOOL bRedraw = TRUE;

	RECT rect;
	GetClientRect( &rect );

	POINT pt;
	pt.x = point.x; pt.y = point.y;

	HCURSOR hCursor = CFx::GetInsertCursor( m_dwInsert );

	if( hCursor != NULL )
	{
		// Insert the fx
		InsertFx( m_dwInsert, &pt );
		ClearInsert();
		m_bInserting = TRUE;

	} // end if

	else
	{
		// Click test
		LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
		if ( ppii != NULL ) SetEditFx( ppii->fx->HitTest( &pt ) );
		else SetEditFx( NULL );

		// Move to top of the Z-order if needed
		if ( m_fxEdit != NULL )
		{
			DWORD dwval = 0;
			CFG().GetValue( "Settings", "FixedDrawingOrder", &dwval );
			if ( dwval == 0 )
			{	
				// Get current fx object
				CFx *Fx = APP()->GetActiveFx();
				if ( Fx != NULL )
				{
					// Move to top of drawing order
					if ( Fx->MoveToBottom( m_fxEdit ) ) 
					{	APP()->ReGen(); bRedraw = FALSE; }

				} // end if
			} // end if
		} // end if
	
	} // end if

	// Start the editing
	if ( m_pEditRect == NULL ) m_rmover.Abort();
	else m_rmover.Start( m_pEditRect, &rect, &pt );

	// Set the cursor
	SetCursor( m_rmover.GetCursor() );

	// Only update if needed
	if ( !bRedraw ) Update();

	CWnd::OnLButtonDown(nFlags, point);
}

void CWinEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonUp(nFlags, point);

	ReleaseCapture();

	// End edit
	m_rmover.End();

	if ( m_bInserting )
	{	m_bInserting = FALSE;
		APP()->ShowProperties( 0 );
	} // end if

	// Draw everything
	APP()->ReGen();

}

void CWinEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	POINT pt;
	pt.x = point.x; pt.y = point.y;

	HCURSOR hCursor = CFx::GetInsertCursor( m_dwInsert );
	
	// Assume nothing
	LPRECT pover = m_pFlybyRect;
	m_pFlybyRect = NULL;

	if( hCursor != NULL )
	{
		// Clear the edit rectangle
		SetEditFx( NULL );

		// Set the insert cursor
		SetCursor( hCursor );

	} // end if
	else
	{
		if ( m_pEditRect != NULL )
		{
			// Move the rect
			if ( m_rmover.Move( &pt, m_pEditRect ) )
			{	IMGLIST().Update(); Update(); }

			// Set the cursor
			SetCursor( m_rmover.GetCursor() );

		} // end if

		else
		{
			LPFX pfx = NULL;
			LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
			if ( ppii != NULL ) pfx = ppii->fx->HitTest( &pt );
			if ( pfx != NULL )
			{	if ( m_pFlybyRect != &pfx->rect )
				{	m_pFlybyRect = &pfx->rect;
					Update();
				} // end if
			} // end if
			else if ( pover != NULL ) Update();


		} // end else

	} // end if
	
	CWnd::OnMouseMove(nFlags, point);
}

BOOL CWinEdit::SetEditFx(LPFX pFx)
{
	if ( pFx == m_fxEdit ) 
	{	if ( pFx == NULL ) m_pEditRect = NULL;
		return FALSE;
	} // end if

	// Save Fx
	m_fxEdit = pFx;

	if ( pFx != NULL )
		
		// Set edit rect
		SetEditRect( &pFx->rect );

	// Clear the edit rect
	else SetEditRect( NULL );

	// Update the properties dialog box
	APP()->ShowProperties( 0 );

	return TRUE;

}

BOOL CWinEdit::InsertFx(DWORD type, LPPOINT pt)
{
	// Clear edit fx
	SetEditFx( NULL );

	// Get fx class
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii == NULL ) return FALSE;

	FX fx;
	ZeroMemory( &fx, sizeof( fx ) );

	// Set rect at click point
	fx.rect.left = fx.rect.right = pt->x;
	fx.rect.top = fx.rect.bottom = pt->y;
	fx.rect.right += 100;
	fx.rect.bottom += 25;
	fx.bck = GetSysColor( COLOR_BTNFACE );
	fx.rgb = GetSysColor( COLOR_BTNTEXT );

	LPFX pFx = NULL;

	switch( type )
	{
		case FXTYPE_TEXT :
			fx.type = type;
			fx.shadow = 1;
			fx.rgb = RGB( 255, 255, 255 );
			strcpy( fx.lf.lfFaceName, "Arial" );
			fx.flags = DT_CENTER | DT_VCENTER | EDT_MULTICENTER | DT_NOCLIP | EDT_ESCAPEMENTCENTER;
			pFx = ppii->fx->AddFx( &fx );
			break;

		case FXTYPE_TIME :
			fx.type = type;
			fx.shadow = 1;
			fx.rgb = RGB( 255, 255, 255 );
			strcpy( fx.lf.lfFaceName, "Arial" );
			fx.flags = DT_CENTER | DT_VCENTER | DT_SINGLELINE | EDT_MULTICENTER | DT_NOCLIP | EDT_ESCAPEMENTCENTER;
			pFx = ppii->fx->AddFx( &fx );
			break;

		case FXTYPE_DATE :
			fx.type = type;
			fx.shadow = 1;
			strcpy( fx.sep, "%w, %b %D, %Y" );
			fx.f1 = FXF1_DATEFORM1;
			fx.rgb = RGB( 255, 255, 255 );
			strcpy( fx.lf.lfFaceName, "Arial" );
			fx.flags = DT_CENTER | DT_VCENTER | DT_SINGLELINE | EDT_MULTICENTER | DT_NOCLIP | EDT_ESCAPEMENTCENTER;
			pFx = ppii->fx->AddFx( &fx );
			break;

		case FXTYPE_RECT :
			fx.type = type;
			pFx = ppii->fx->AddFx( &fx );
			break;

		case FXTYPE_WATERMARK :
			fx.translucency = 7500;
			{ // Set directory
				char path[ CWF_STRSIZE ];
				if ( GetModuleFileName( NULL, path, sizeof( path ) ) )
				{	CWinFile::GetPathFromFileName( path, fx.data );
					CWinFile::BuildPath( fx.data, fx.data, "Watermark Images" );
				} // end if
			} // end set file
			fx.type = type;
			pFx = ppii->fx->AddFx( &fx );
			break;

		case FXTYPE_IMAGE :
			{ // Set directory
				char path[ CWF_STRSIZE ];
				if ( GetModuleFileName( NULL, path, sizeof( path ) ) )
				{	CWinFile::GetPathFromFileName( path, fx.data );
					CWinFile::BuildPath( fx.data, fx.data, "Overlay Images" );
				} // end if
			} // end set file
			fx.type = type;
			pFx = ppii->fx->AddFx( &fx );
			break;

		case FXTYPE_VIDEO :
			fx.type = type;
			fx.driver = 0;
			fx.rect.right = fx.rect.left + 64;
			fx.rect.bottom = fx.rect.top + 48;
			pFx = ppii->fx->AddFx( &fx );
			break;

		case FXTYPE_URL :
			fx.type = type;
			fx.rect.right = fx.rect.left + 64;
			fx.rect.bottom = fx.rect.top + 48;
			pFx = ppii->fx->AddFx( &fx );
			break;

		case FXTYPE_SCREENSHOT :
			fx.type = type;
			fx.rect.right = fx.rect.left + 64;
			fx.rect.bottom = fx.rect.top + 48;
			pFx = ppii->fx->AddFx( &fx );
			break;

		case FXTYPE_STEXT :
			fx.type = type;
			fx.shadow = 1;
			fx.rgb = RGB( 255, 255, 255 );
			strcpy( fx.lf.lfFaceName, "Arial" );
			fx.flags = DT_CENTER | DT_VCENTER | EDT_MULTICENTER | DT_NOCLIP | EDT_ESCAPEMENTCENTER;
			pFx = ppii->fx->AddFx( &fx );
			break;

	} // end switch

	SetEditFx( pFx );

	return ( pFx != NULL );
}

void CWinEdit::ToggleInsert(DWORD type)
{
	// Ensure FX object
	if ( m_pImgList == NULL || m_pImgList->Size() == 0 )
	{	m_dwInsert = 0;
		return;
	} // end if

	// Toggle insert mode
	if ( m_dwInsert == type ) m_dwInsert = 0;
	else m_dwInsert = type;
}

void CWinEdit::ClearInsert()
{
	m_dwInsert = 0; 
	FRAME()->RedrawOverlayBar(); 
}

BOOL CWinEdit::ShowMenu(long x, long y)
{
	HMENU hMenu, hSub;
	BOOL bOnTop = FALSE;

	// Load the template
	hMenu = LoadMenu( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDM_POPUP ) );
	if ( hMenu == NULL ) return FALSE;
	hSub = GetSubMenu( hMenu, 0 );
	if ( hSub == NULL ) return FALSE;

	if ( m_pEditRect == NULL )
	{	EnableMenuItem( hSub, ID_POPUP_CUT, MF_GRAYED );
		EnableMenuItem( hSub, ID_POPUP_COPY, MF_GRAYED );
		EnableMenuItem( hSub, ID_POPUP_DELETE, MF_GRAYED );
		EnableMenuItem( hSub, ID_POPUP_DUPLICATE, MF_GRAYED );
	} // end if

	// Check for clipboard data
	{
		UINT uFormat = RegisterClipboardFormat( "WheresJames Webcam Publisher Ovleray Element" );
		BOOL enable =	IsClipboardFormatAvailable( uFormat ) ||
						IsClipboardFormatAvailable( CF_TEXT );
		if ( !enable ) EnableMenuItem( hSub, ID_POPUP_PASTE, MF_GRAYED );
	} 

	// We must be the foreground window for this to work correctly
	SetForegroundWindow();

	// Save menu point
	m_ptMenu.x = x; m_ptMenu.y = y;

	// Show the popup menu
	::TrackPopupMenu( hSub, TPM_RIGHTBUTTON, x, y, 0, GetSafeHwnd(), NULL );

	// Unload the menu
	DestroyMenu( hMenu );

	return TRUE;
}

void CWinEdit::OnRButtonDown(UINT nFlags, CPoint point) 
{
	RECT rect;
	GetClientRect( &rect );

	POINT pt;
	pt.x = point.x; pt.y = point.y;

	ClearInsert();
	m_bInserting = FALSE;

	// Click test
	LPPUBIMGINFO ppii = IMGLIST().GetSelImage();
	if ( ppii != NULL ) SetEditFx( ppii->fx->HitTest( &pt ) );
	else SetEditFx( NULL );

	ClientToScreen( &pt );
	ShowMenu( pt.x, pt.y );
	
	CWnd::OnRButtonDown(nFlags, point);
}


void CWinEdit::OnPopupCopy()
{	

	LPFX pfx = GetEditFx();
	if ( pfx == NULL ) return;

	UINT uFormat = RegisterClipboardFormat( "WheresJames Webcam Publisher Ovleray Element" );
	if ( uFormat == 0 ) return;

	HGLOBAL hData = GlobalAlloc( GMEM_MOVEABLE, sizeof( FX ) );
	if ( hData == NULL ) return;

	if ( ::OpenClipboard( GetSafeHwnd() ) )
	{
		// Lose current clipboard contents
		EmptyClipboard();

		LPFX pcbfx = (LPFX)GlobalLock( hData );

		if ( pcbfx != NULL )
		{
			// Copy data
			memcpy( pcbfx, pfx, sizeof( FX ) );

			// Set to clipboard
			GlobalUnlock( hData );
			SetClipboardData( uFormat, hData );

		} // end if

		// Set plain text type
		if ( strlen( pfx->data ) > 0 )
		{
			hData = GlobalAlloc( GMEM_MOVEABLE, strlen( pfx->data ) + 1 );
			if ( hData != NULL )
			{
				char *text = (char*)GlobalLock( hData );
				if ( text != NULL )
				{
					strcpy( text, pfx->data );
					GlobalUnlock( hData );
					SetClipboardData( CF_TEXT, hData );
				} // end if

			} // end if
								  
		} // end if
		
		::CloseClipboard();

	} // end if
}

void CWinEdit::OnPopupCut()
{	

	OnPopupCopy();
	OnPopupDelete();	
}

void CWinEdit::OnPopupDelete()
{	

	// Get current fx object
	CFx *Fx = APP()->GetActiveFx();
	if ( Fx == NULL ) return;

	LPFX pfx = GetEditFx();
	if ( pfx != NULL )
	{	SetEditFx( NULL );
		Fx->RemoveFx( pfx );
		APP()->ReGen();
		APP()->RefreshProperties();
	} // end if	
}

void CWinEdit::OnPopupDuplicate()
{	

	// Get current fx object
	CFx *Fx = APP()->GetActiveFx();
	if ( Fx == NULL ) return;

	LPFX pfx = GetEditFx();
	if ( pfx != NULL )
	{	SetEditFx( NULL );
		LPFX n = Fx->AddFx( pfx );
		if ( n != NULL )
		{
			OffsetRect( &n->rect, 5, 5 );
			SetEditFx( n );
			APP()->ReGen();
			APP()->RefreshProperties();
		} // end if
	} // end if	
}

void CWinEdit::OnPopupPaste()
{	

	// Get current fx object
	CFx *Fx = APP()->GetActiveFx();
	if ( Fx == NULL ) return;

	POINT pt;
	memcpy( &pt, &m_ptMenu, sizeof( pt ) );
	if ( m_ptMenu.x != 0 && m_ptMenu.y != 0 )
		ScreenToClient( &pt );
	else pt.x = 20, pt.y = 20;

	UINT uFormat = RegisterClipboardFormat( "WheresJames Webcam Publisher Ovleray Element" );

	if (	uFormat != 0 && IsClipboardFormatAvailable( uFormat ) &&
			::OpenClipboard( GetSafeHwnd() ) )
	{
		HANDLE hData = GetClipboardData( uFormat );

		LPFX pcbfx = (LPFX)GlobalLock( hData );
		if ( pcbfx != NULL )
		{
			FX fx;
			memcpy( &fx, pcbfx, sizeof( FX ) );
			SetRect( &fx.rect,	pt.x, pt.y, 
								pt.x + ( pcbfx->rect.right - pcbfx->rect.left ),
								pt.y + ( pcbfx->rect.bottom - pcbfx->rect.top ) );
			
			SetEditFx( Fx->AddFx( &fx ) );
			GlobalUnlock( hData );

		} // end if

		::CloseClipboard();

		APP()->ReGen();

	}

	else if (	IsClipboardFormatAvailable( CF_TEXT ) &&
				::OpenClipboard( GetSafeHwnd() ) )
	{
		HANDLE hData = GetClipboardData( CF_TEXT );
		if ( hData != NULL )
		{
			char* text = (char*)GlobalLock( hData );

			if ( text != NULL )
			{
				FX fx;
				ZeroMemory( &fx, sizeof( fx ) );

				// Copy text
				DWORD i = 0;
				for ( i = 0; text[ i ] != NULL && i < 64; i++ )
					fx.data[ i ] = text[ i ];
				fx.data[ i ] = 0;
				fx.type = FXTYPE_TEXT;
				fx.shadow = 2;
				fx.rgb = RGB( 255, 255, 255 );
				strcpy( fx.lf.lfFaceName, "Arial" );
				fx.flags = DT_CENTER | DT_VCENTER | DT_SINGLELINE | EDT_MULTICENTER | DT_NOCLIP | EDT_ESCAPEMENTCENTER;

				SetRect( &fx.rect, pt.x, pt.y, pt.x + 1000, pt.y + 1000 );
				CText txt;
				txt.SetFlags( fx.flags );
				txt.SetFont( &fx.lf );
				if ( !txt.CalcRect( m_offscreen, fx.data, &fx.rect ) )
					SetRect( &fx.rect, pt.x, pt.y, pt.x + 100, pt.y + 25 );
				else InflateRect( &fx.rect, 16, 4 );

				SetEditFx( Fx->AddFx( &fx ) );
				GlobalUnlock( hData );
			} // end if
		} // end if

		::CloseClipboard();

		APP()->ReGen();

	} // end if
}

void CWinEdit::OnPopupProperties()
{
	APP()->ShowProperties( 1 );
}




void CWinEdit::ClearFlyby()
{
	if ( m_pFlybyRect != NULL )
	{	m_pFlybyRect = NULL;
		Update();
	} // end if
}



void CWinEdit::FreeMemory()
{
	m_offscreen.Destroy();
}
