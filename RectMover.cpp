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
// RectMover.cpp: implementation of the CRectMover class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vp.h"
#include "RectMover.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRectMover::CRectMover()
{
	m_pEditRect = NULL;
	m_dwCursor = MAXDWORD;
}

CRectMover::~CRectMover()
{
	Abort();
}

BOOL CRectMover::Start(LPRECT pRect, LPRECT pRef, LPPOINT pt)
{
	// Quit last edit
	if ( m_pEditRect != NULL ) Abort();
	
	// Update cursor
	Move( pt, pRect );

	// Save pointer to edit rect
	if ( pRect == NULL ) m_pEditRect = &m_rect;
	else m_pEditRect = pRect;

	// Update cursor
	Move( pt, pRect );

	// Copy reference rect if any
	if ( pRef != NULL ) CopyRect( &m_ref, pRef );
	else ZeroMemory( &m_ref, sizeof( m_ref ) );

	return TRUE;
}

void CRectMover::Abort()
{
	m_pEditRect = NULL;
	m_dwCursor = MAXDWORD;
}


void CRectMover::End()
{
	m_pEditRect = NULL;
	m_dwCursor = MAXDWORD;
}


BOOL CRectMover::Move(LPPOINT point, LPRECT pEdit)
{
	POINT pt;
	pt.x = point->x; pt.y = point->y;

	RECT rect;
	CopyRect( &rect, &m_ref );
	

	// 0 = Outside box
	// 1 = Move
	// 2 = size left
	// 3 = size right
	// 4 = size top
	// 5 = size bottom
	// 6 = size top left
	// 7 = size top right
	// 8 = size bottom left
	// 9 = size bottom right

	if ( m_pEditRect == NULL || m_dwCursor == MAXDWORD )
	{
		if ( pEdit == NULL ) m_dwCursor = 0;

		else
		{
			DWORD	cursor = 0;

			RECT edit;
			CopyRect( &edit, pEdit );

			// Choose a cursor
			if ( PtInRect( &edit, pt ) )
			{
				if ( pt.x <= ( edit.left + 5 ) ) cursor = 2;
				else if ( pt.x >= ( edit.right - 5 ) ) cursor = 3;
				
				if ( pt.y <= ( edit.top + 5 ) ) 
				{
					if ( cursor == 2 ) cursor = 6;
					else if ( cursor == 3 ) cursor = 7;
					else cursor = 4;
				} // end if

				else if ( pt.y >= ( edit.bottom - 5 ) )
				{
					if ( cursor == 2 ) cursor = 8;
					else if ( cursor == 3 ) cursor = 9;
					else cursor = 5;
				} // end else if

				if ( cursor == 0 ) cursor = 1;
			} // end else

			// Set cursor
			m_dwCursor = cursor;

		} // end if

	} // end if

	// Do we need to move the rect?
	if ( m_pEditRect != NULL && m_dwCursor != 0 )
	{
		long offx = pt.x - m_ptMove.x;
		long offy = pt.y - m_ptMove.y;

		// Convert to virtual coords
		CopyRect( &rect, m_pEditRect );

		// Save the point the first time through
		if ( m_ptMove.x == 0 && m_ptMove.y == 0 )
		{
			CopyRect( &m_rectEditRef, m_pEditRect );
			memcpy( &m_ptMove, &pt, sizeof( POINT ) );
			return ( m_dwCursor != 0 );
		} // end if

		RECT	compare, e;
		CopyRect( &compare, m_pEditRect );
		
		// Reset back to reference
		CopyRect( m_pEditRect, &m_rectEditRef );
		CopyRect( &e, m_pEditRect );


		// Book if not moving
		if ( offx == 0 && offy == 0 ) return ( m_dwCursor != 0 );

		else if ( ( offx != 0 || offy != 0 ) && m_dwCursor == 1 )
			OffsetRect( &e, offx, offy );

		else if ( m_dwCursor == 2 ) 
		{ if ( e.left + offx < e.right ) e.left += offx; }

		else if ( m_dwCursor == 3 ) 
		{ if ( e.right + offx > e.left ) e.right += offx; }

		else if ( m_dwCursor == 4 ) 
		{ if ( e.top + offy < e.bottom ) e.top += offy; }

		else if ( m_dwCursor == 5 ) 
		{ if ( e.bottom + offy > e.top ) e.bottom += offy; }

		else if ( m_dwCursor == 6 ) 
		{
			{ if ( e.left + offx < e.right ) e.left += offx; }
			{ if ( e.top + offy < e.bottom ) e.top += offy; }
		} // end else if

		else if ( m_dwCursor == 7 ) 
		{
			{ if ( e.right + offx > e.left ) e.right += offx; }
			{ if ( e.top + offy < e.bottom ) e.top += offy; }
		} // end else if

		else if ( m_dwCursor == 8 ) 
		{
			{ if ( e.left + offx < e.right ) e.left += offx; }
			{ if ( e.bottom + offy > e.top ) e.bottom += offy; }
		} // end else if

		else if ( m_dwCursor == 9 ) 
		{
			{ if ( e.right + offx > e.left ) e.right += offx; }
			{ if ( e.bottom + offy > e.top ) e.bottom += offy; }
		} // end else if

		// Minimum size
		if ( ( e.right - e.left ) < 15 ) e.right = e.left + 15;
		if ( ( e.bottom - e.top ) < 15 ) e.bottom = e.top + 15;

		// Maximum size
		if ( ( e.right - e.left ) > ( m_ref.right - m_ref.left ) )
			e.right = e.left + ( m_ref.right - m_ref.left );
		if ( ( e.bottom - e.top ) > ( m_ref.bottom - m_ref.top ) ) 
			e.bottom = e.top + ( m_ref.bottom - m_ref.top );

		// Don't let it go off the window
		long ox = 0, oy = 0;
		if ( e.right < m_ref.left + 5 ) ox = ( m_ref.left + 5 ) - e.right;
		else if ( e.left > m_ref.right - 5 ) ox = ( m_ref.right - 5 ) - e.left;
		if ( e.bottom < m_ref.top + 5 ) oy = ( m_ref.top + 5 ) - e.bottom;
		else if ( e.top > m_ref.bottom - 5 ) oy = ( m_ref.bottom - 5 ) - e.top;
		if ( ox != 0 || oy != 0 ) OffsetRect( &e, ox, oy );


		// Save the new rect
		CopyRect( m_pEditRect, &e );

		// Have things changed?
		if ( !EqualRect( &compare, m_pEditRect ) )
			return TRUE;

	} // end if

	// Reset move point
	else { m_ptMove.x = 0; m_ptMove.y = 0; }

	return FALSE;
}

HCURSOR CRectMover::GetCursor()
{
	// Switch cursors if needed
	HCURSOR	hCursor = NULL;

	// What cursor should we use?
	switch( m_dwCursor )
	{
		case 1: hCursor = LoadCursor( NULL, IDC_SIZEALL ); break;
		case 2: case 3: hCursor = LoadCursor( NULL, IDC_SIZEWE ); break;
		case 4: case 5: hCursor = LoadCursor( NULL, IDC_SIZENS ); break;
		case 7: case 8: hCursor = LoadCursor( NULL, IDC_SIZENESW ); break;
		case 6: case 9: hCursor = LoadCursor( NULL, IDC_SIZENWSE ); break;

		default: hCursor = LoadCursor( NULL, IDC_ARROW ); break;

	} // end switch

	return hCursor;
}

#define TIC		8
#define HTIC	( TIC >> 1 )
BOOL CRectMover::Draw(HDC hDC, LPRECT pRect, LPRECT pEdit, BOOL bTicks )
{
	// Punt if no edit box
	if ( hDC == NULL || pRect == NULL || pEdit == NULL ) return FALSE;

	RECT rect;
	CopyRect( &rect, pEdit );
	rect.left--; rect.top--;

//	CGrDC::DottedBox( hDC, &rect, RGB( 0, 0, 0 ), RGB( 255, 255, 255 ) );

	{	// Draw dotted box

		DrawFocusRect( hDC, &rect );

/*		HPEN hPen = CreatePen( PS_DOT, 1, RGB( 0, 0, 0 ) );
		HPEN hOldPen = (HPEN)::SelectObject( hDC, hPen );
		HBRUSH hOldBrush = (HBRUSH)::SelectObject( hDC, (HBRUSH)GetStockObject( NULL_BRUSH ) );

		MoveToEx( hDC, rect.left, rect.top, NULL );
		LineTo( hDC, rect.right, rect.top );
		LineTo( hDC, rect.right, rect.bottom );

		MoveToEx( hDC, rect.left, rect.top, NULL );
		LineTo( hDC, rect.left, rect.bottom );
		LineTo( hDC, rect.right, rect.bottom );

		::SelectObject( hDC, hOldBrush );
		::SelectObject( hDC, hOldPen );
		::DeleteObject( hPen );
*/
	} // End draw dotted box

	if ( bTicks )
	{
		HBRUSH hBrush = CreateSolidBrush( GetSysColor( COLOR_ACTIVECAPTION ) );
		HBRUSH hOldBrush = (HBRUSH)::SelectObject( hDC, hBrush );

		// Draw corner ticks
		Rectangle( hDC, rect.left, rect.top, rect.left + TIC, rect.top + TIC );
		Rectangle( hDC, rect.right - TIC, rect.top, rect.right, rect.top + TIC );
		Rectangle( hDC, rect.left, rect.bottom - TIC, rect.left + TIC, rect.bottom );
		Rectangle( hDC, rect.right - TIC, rect.bottom - TIC, rect.right, rect.bottom );

		long hw = rect.left + ( ( rect.right - rect.left ) >> 1 );
		long hh = rect.top + ( ( rect.bottom - rect.top ) >> 1 );

		// Draw edge ticks
		Rectangle( hDC, hw - HTIC, rect.top, hw + HTIC, rect.top + TIC );
		Rectangle( hDC, hw - HTIC, rect.bottom - TIC, hw + HTIC, rect.bottom );
		Rectangle( hDC, rect.left, hh - HTIC, rect.left + TIC, hh + HTIC );
		Rectangle( hDC, rect.right - TIC, hh - HTIC, rect.right, hh + HTIC );

		::SelectObject( hDC, hOldBrush );
		::DeleteObject( hBrush );
	} // end if

	return TRUE;
}
