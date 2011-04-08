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
// WinButton.cpp: implementation of the CWinButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WinButton.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWinButton::CWinButton()
{
	// Create button info object
	m_pwbi = new WINBUTTONINFO;
	if ( m_pwbi == NULL ) throw;

	// Init memory
	m_bRelease = TRUE;
	Init( m_pwbi );
}

CWinButton::CWinButton( LPWINBUTTONINFO pwbi, BOOL bAutoDelete )
{
	// Init memory
	m_pwbi = pwbi;
	m_bRelease = bAutoDelete;
	Init( m_pwbi );
}

CWinButton::~CWinButton()
{	
	// Release memory
	if ( m_bRelease && m_pwbi != NULL ) delete m_pwbi;
	m_bRelease = NULL; m_pwbi = NULL;
}

BOOL CWinButton::Draw( LPWINBUTTONINFO pWbi, HWND hWnd, HDC hDC, LPRECT pRect)
{
	if ( pWbi == NULL || hDC == NULL || pRect == NULL )
		return FALSE;

	// Attempt to create offscreen
	HDC hOrg = NULL; CWinDc os;
	if ( os.Create( os, pRect ) )
	{	hOrg = hDC, hDC = os;
		CGrDC::FillSolidRect( os, os, GetSysColor( COLOR_3DFACE ) );
	} // end if

	// Get window rect
	if ( ::IsWindow( hWnd ) ) ::GetClientRect( hWnd, &pWbi->win );

	if ( ( pWbi->f1 & WBIF1_NOTHEME ) == 0 && CXpTheme::IsWindowThemed( hWnd ) )
	{
		CXpTheme xpthm( hWnd, "BUTTON" );

		int state = PBS_NORMAL;
		
		// Determine button state
		if ( GetCheck( pWbi ) ) state = PBS_PRESSED;

		else if ( GetHot( pWbi ) )
		{	if ( GetPress( pWbi ) ) state = PBS_PRESSED;
			else state = PBS_HOT;
		} // end if

		// Draw defaulted
		else if ( GetDefaulted( pWbi ) || GetSelected( pWbi ) ) 
			state = PBS_DEFAULTED;

		// Draw the button
		xpthm.DrawThemeBackground( hDC, BP_PUSHBUTTON, state, pRect, NULL );

	} // end if
	else
	{
		UINT state = 0;

		// Determine button state
		if ( GetHot( pWbi ) )
		{	if ( GetPress( pWbi ) ) state = 1;
			else state = 2;
		} // end if		

		COLORREF rgbbck = pWbi->rgbbck;
		if ( rgbbck == MAXDWORD ) rgbbck = GetSysColor( COLOR_3DFACE );

		// Draw the button
//		if ( state == 0 )
			CGrDC::Box3d(	hDC, pRect, 2, 
							CGrDC::ScaleColor( rgbbck, 120 ),
							CGrDC::ScaleColor( rgbbck, -120 ),
							FALSE );

		RECT hot;
		CopyRect( &hot, pRect );
		InflateRect( &hot, -2, -2 );

		if ( state == 1 ) CGrDC::Box3d(	hDC, pRect, 2, 
												CGrDC::ScaleColor( rgbbck, 120 ),
												CGrDC::ScaleColor( rgbbck, -120 ),
												TRUE );

		else if ( state == 2 ) CGrDC::Box3d(	hDC, &hot, 2, 
												GetSysColor( COLOR_ACTIVECAPTION ),
												GetSysColor( COLOR_ACTIVECAPTION ),
												FALSE );
	} // end else

	if ( GetFocus( pWbi ) )
	{
		RECT focus;
		CopyRect( &focus, pRect );
		InflateRect( &focus, -4, -4 );
		DrawFocusRect( hDC, &focus );

	} // end if

	RECT rect;
	CopyRect( &rect, pRect );
	if ( GetPress( pWbi ) ) OffsetRect( &rect, 1, 1 );

	if ( pWbi->hIcon != NULL )
	{
		RECT icon;
		CGrDC::GetIconRect( pWbi->hIcon, &icon );

		long off = 2;
		if ( RW( rect ) > RH( rect ) ) off = ( RH( rect ) - RH( icon ) ) >> 1;
		else off = ( RW( rect ) - RW( icon ) ) >> 1;
		if ( off < 2 ) off = 2;

		OffsetRect( &icon, off, 0 );
		CGrDC::Center( &icon, &rect, FALSE );
		rect.left += icon.left + RW( icon ) + off;

		if ( GetPress( pWbi ) ) OffsetRect( &icon, 1, 0 );

		CGrDC::DrawIcon( hDC, &icon, pWbi->hIcon );

	} // end if

	if ( pWbi->pText != NULL )
	{
		CText txt;

		if ( ( pWbi->f1 & WBIF1_CUSTOMFONT ) != 0 ) 
			txt.SetWeight( FW_BOLD );

		// Set default font
		else
		{	NONCLIENTMETRICS info;
			info.cbSize = sizeof(info);			
			SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( info ), &info, 0 );
			txt.SetFont( &info.lfMenuFont );
		} // end else
		if ( pWbi->hIcon != NULL ) txt.SetFlags( DT_SINGLELINE | DT_VCENTER | DT_LEFT );
		else txt.SetFlags( DT_SINGLELINE | DT_VCENTER | DT_CENTER );

		txt.DrawText( hDC, pWbi->pText, &rect );

	} // end if

	if ( hOrg != NULL ) os.Draw( hOrg, pRect );

	return TRUE;
}

void CWinButton::ReleaseObject(LPWINBUTTONINFO pWbi)
{
	if ( pWbi == NULL ) return;

	if ( pWbi->pText != NULL )
	{	delete [] pWbi->pText;
		pWbi->pText = NULL;
	} // end if

	if ( pWbi->plf != NULL )
	{	delete pWbi->plf;
		pWbi->plf = NULL;
	} // end if

	if ( pWbi->pbckimg != NULL )
	{	delete pWbi->pbckimg;
		pWbi->pbckimg = NULL;
	} // end if
}

LPWINBUTTONINFO CWinButton::NewObject()
{
	// Create object
	LPWINBUTTONINFO node = new WINBUTTONINFO;
	if ( node == NULL ) return NULL;

	// Init memory
	Init( node );

	// Return pointer
	return node;
}

void CWinButton::Init(LPWINBUTTONINFO pWbi)
{
	// Initialize memory
	ZeroMemory( pWbi, sizeof( WINBUTTONINFO ) );

	// Init colors
	pWbi->rgbbck = MAXDWORD;
	pWbi->rgbtxt = MAXDWORD;

}

BOOL CWinButton::OnMouseMove(LPWINBUTTONINFO pwbi, UINT nFlags, LPPOINT pt)
{
	if ( pwbi == NULL ) return FALSE;
	
	BOOL bOnButton = PtInRect( &pwbi->win, *pt );

	// Do we need to redraw
	BOOL bRedraw = (	!GetHot( pwbi ) || 
						( ( nFlags & MK_LBUTTON ) != 0 ) != GetPress( pwbi ) );

	// Set hot flag
	SetHot( pwbi, TRUE );

	// Redraw if clicked state changed
	SetPress( pwbi, bOnButton && ( nFlags & MK_LBUTTON ) != 0 );

	return bRedraw;
}

BOOL CWinButton::OnLButtonDown(LPWINBUTTONINFO pwbi, UINT nFlags, LPPOINT pt)
{
	SetPress( pwbi, TRUE );
	SetHot( pwbi, TRUE );
	return TRUE;
}

BOOL CWinButton::OnLButtonUp(LPWINBUTTONINFO pwbi, UINT nFlags, LPPOINT pt)
{
	SetPress( pwbi, FALSE );
	return TRUE;
}
