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
// ItemList.cpp: implementation of the CItemList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemList::CItemList()
{
	m_dwNoDraw = 0;

	m_dwPtr = 0;
	m_dwSize = NULL;
	m_pIndex = NULL;

	m_dwDefaultHeight = 18;

	m_bHorzLines = FALSE;
	m_bVertLines = FALSE;

	m_rgbHorzLines = GetSysColor( COLOR_BTNTEXT );
	m_rgbVertLines = GetSysColor( COLOR_BTNTEXT );

	m_bMultiSelect = TRUE;

	m_dwLine = 0;

	m_lHScroll = 0;

	m_phii = NULL;	

	m_bGroups = FALSE;
	m_bInGroup = FALSE;

	m_bAutoDefaultColors = TRUE;

}

void CItemList::DefaultColors()
{
	SetBckCol(	GetSysColor( COLOR_3DFACE ), GetSysColor( COLOR_3DFACE ) );
	SetSelCol(	CGrDC::ScaleColor( GetSysColor( COLOR_HIGHLIGHT ), 40 ),
				CGrDC::ScaleColor( GetSysColor( COLOR_HIGHLIGHT ), -40 ) );
	SetTextCol(	GetSysColor( COLOR_BTNTEXT ) );
	SetSelTextCol(	GetSysColor( COLOR_HIGHLIGHTTEXT ) );
	SetBarCol(	CGrDC::ScaleColor( GetSysColor( COLOR_INACTIVECAPTION ), 40 ),
				CGrDC::ScaleColor( GetSysColor( COLOR_INACTIVECAPTION ), -40 ) );
}

CItemList::~CItemList()
{
	Destroy();
}

void CItemList::Destroy()
{
	m_phii = NULL;

	// Delete the list
	if ( m_pIndex != NULL )
	{
		for ( DWORD i = 0; i < m_dwPtr; i++ )
		{
			if ( m_pIndex[ i ] != NULL )
			{
				DeleteSubItems( i );
				delete m_pIndex[ i ];
				m_pIndex[ i ] = NULL;
			} // end if

		} // end for

		delete[] m_pIndex;
		m_pIndex = NULL;

	} // end if

	m_dwSize = 0;
	m_dwPtr = 0;

	m_dwLine = 0;
	m_lHScroll = 0;

	m_bGroups = FALSE;
	m_bInGroup = FALSE;
}

BOOL CItemList::AllocateIndex(DWORD size)
{
	if ( m_pIndex == NULL )
	{
		m_dwSize = 0;

		// Allocate memory
		m_pIndex = new LPLISTITEMINFO[ 16 ];
		if ( m_pIndex == NULL ) return FALSE;

		// Save size
		m_dwSize = 16;

		return TRUE;

	} // end if

	// Do we already have enough room
	while ( size > m_dwSize )
	{
		// Allocate more memory
		LPLISTITEMINFO	*list = new LPLISTITEMINFO[ m_dwSize << 1 ];
		if ( list == NULL ) return FALSE;

		// Copy the pointers
		memcpy( list, m_pIndex, ( m_dwSize * sizeof( DWORD ) ) );

		// Use the new list
		delete [] m_pIndex;
		m_pIndex = list;
		m_dwSize <<= 1;

	} // end while

	return TRUE;
}

DWORD CItemList::AddItem( DWORD flags )
{
	// Is it a group?
	if ( ( flags & LIF_GROUP ) != 0 ) m_bInGroup = TRUE;
	if ( m_bInGroup ) flags |= LIF_INGROUP;
	else flags &= ~LIF_INGROUP;

	// Set group flag
	if ( m_bInGroup ) m_bGroups = TRUE;

	// Allocate memory
	LPLISTITEMINFO	plii = new LISTITEMINFO;
	if ( plii == NULL ) return MAXDWORD;

	// Initialize memory to zero
	ZeroMemory( plii, sizeof( LISTITEMINFO ) );

	// Save the index pointer
	plii->index = m_dwPtr;
	plii->height = m_dwDefaultHeight;
	plii->flags = flags;
	plii->subitems = 0;
	
	// Allocate memory for index
	AllocateIndex( m_dwPtr + 1 );

	// Add this block to the index
	m_pIndex[ m_dwPtr ] = plii;

	// Count a block
	m_dwPtr++;

	// Return the index
	return plii->index;
}


BOOL CItemList::AddSubItem(DWORD index, LPVOID data, HICON icon, DWORD type, DWORD flags, COLORREF rgbtext, COLORREF rgbbck, LPLOGFONT plf )
{
	// Sanity check
	if ( index >= m_dwPtr || m_pIndex[ index ] == NULL ) 
		return FALSE;

	// Allocate memory
	LPLISTSUBITEMINFO	sitem = new LISTSUBITEMINFO;
	if ( sitem == NULL ) return FALSE;

	// Zero memory
	ZeroMemory( sitem, sizeof( LISTSUBITEMINFO ) );

	// Save data
	sitem->index = m_pIndex[ index ]->subitems;
	sitem->type = type;
	sitem->flags = flags;
	sitem->icon = icon;
	sitem->rgbtext = rgbtext;
	sitem->rgbbck = rgbbck;

	// Copy font if any
	sitem->pfont = plf;

	switch( type )
	{
		case SIT_TEXT :
			sitem->data = new char[ strlen( (char*)data ) + 2 ];
			if ( data == NULL )
			{
				delete sitem;
				return FALSE;
			} // end if
			strcpy( (char*)sitem->data, (char*)data );
			break;

		default:
			sitem->data = data;
			break;

	} // end switch

	// Add to item list
	// Add this node to the list
	sitem->pNext = NULL;
	sitem->pPrev = m_pIndex[ index ]->tail;
	if ( m_pIndex[ index ]->tail == NULL ) m_pIndex[ index ]->head = sitem;
	else m_pIndex[ index ]->tail->pNext = sitem;
	m_pIndex[ index ]->tail = sitem;
	m_pIndex[ index ]->subitems++;		

	return TRUE;
}

void CItemList::DeleteSubItemData(LPLISTSUBITEMINFO psii)
{
	if ( psii == NULL ) return;

	switch( psii->type )
	{
		case SIT_TEXT :
			if ( psii->data != NULL )
			{
				delete [] psii->data;
				psii->data = NULL;
			} // end if
			break;

		default : break;

	} // end switch

	// Delete the font
//	if ( psii->pfont != NULL ) 
//	{	delete psii->pfont;
//		psii->pfont = NULL;
//	} // end if
}

void CItemList::DeleteSubItems(DWORD index)
{
	// Sanity check
	if ( index > m_dwPtr ) return;

	// Delete sub items
	while ( m_pIndex[ index ]->head != NULL )
	{
		LPLISTSUBITEMINFO	next = m_pIndex[ index ]->head->pNext;
		DeleteSubItemData( m_pIndex[ index ]->head );
		delete m_pIndex[ index ]->head;
		m_pIndex[ index ]->head = next;
	} // end while
	m_pIndex[ index ]->tail = NULL;
	m_pIndex[ index ]->subitems = 0;
}

BOOL CItemList::ModifySubItem(DWORD index, DWORD subitem, LPVOID data, HICON icon, DWORD type, DWORD flags )
{
	LPLISTSUBITEMINFO	sitem = GetSubItem( index, subitem );
	if ( sitem == NULL ) return FALSE;

	// Remove old data
	DeleteSubItemData( sitem );

	// Zero memory
//	ZeroMemory( sitem, sizeof( LISTSUBITEMINFO ) );

	// Save data
	sitem->type = type;
	sitem->flags = flags;
	sitem->icon = icon;

	switch( type )
	{
		case SIT_TEXT :
			sitem->data = new char[ strlen( (char*)data ) + 2 ];
			if ( sitem->data != NULL )
				strcpy( (char*)sitem->data, (char*)data );
			break;

		default:
			sitem->data = data;
			break;

	} // end switch

	return TRUE;
}

LPLISTSUBITEMINFO CItemList::GetSubItem(DWORD index, DWORD subitem)
{
	// Sanity check
	if ( index >= m_dwPtr || m_pIndex[ index ] == NULL || m_pIndex[ index ]->subitems <= subitem ) 
		return FALSE;

	LPLISTSUBITEMINFO	sitem = m_pIndex[ index ]->head;

	// Get the correct sub-item
	while ( sitem != NULL && sitem->index != subitem )
		sitem = sitem->pNext;		

	return sitem;
}

BOOL CItemList::SetSubItemFlags(DWORD index, DWORD subitem, DWORD flags )
{
	LPLISTSUBITEMINFO	plsii = GetSubItem( index, subitem );
	if ( plsii == NULL ) return FALSE;

	// Save data
	plsii->flags = flags;

	return TRUE;
}

BOOL CItemList::SetSubItemIcon(DWORD index, DWORD subitem, HICON icon)
{
	LPLISTSUBITEMINFO	plsii = GetSubItem( index, subitem );
	if ( plsii == NULL ) return FALSE;

	// Save data
	plsii->icon = icon;

	return TRUE;
}

#define GROUPOFFSET 16

DWORD CItemList::GetCurLine() 
{
	if ( !m_bGroups ) return m_dwLine; 

	DWORD i = 0, s = 0;
	BOOL bCollapsed = FALSE;
	while ( i < m_dwLine && i < m_dwPtr )
	{	
		if ( m_pIndex[ i ] != NULL ) 
		{	
			if ( ( m_pIndex[ i ]->flags & LIF_GROUP ) != 0 )
			{	s++;
				if ( ( m_pIndex[ i ]->flags & LIF_COLLAPSED ) != 0 ) bCollapsed = TRUE;
				else bCollapsed = FALSE;
			} // end if
			else if ( !bCollapsed ) s++;
		} // end if
		i++;
	} // end while

	return s;
}

void CItemList::SetCurLine( DWORD l )
{
	if ( !m_bGroups )
	{
		if ( l <= m_dwPtr ) m_dwLine = l; 
		else m_dwLine = m_dwPtr; 
	} // end if

	else
	{
		// Sanity check
		if ( l >= m_dwPtr ) { m_dwLine = m_dwPtr; return; }

		m_dwLine = 0;
		DWORD i = 0, s = 0;
		BOOL bCollapsed = FALSE;
		while ( s < l && i < m_dwPtr )
		{	
			if ( m_pIndex[ i ] != NULL ) 
			{	
				if ( ( m_pIndex[ i ]->flags & LIF_GROUP ) != 0 )
				{	s++;
					if ( ( m_pIndex[ i ]->flags & LIF_COLLAPSED ) != 0 ) bCollapsed = TRUE;
					else bCollapsed = FALSE;
				} // end if
				else if ( !bCollapsed ) s++;
			} // end if
			i++;
		} // end while

		if ( i < m_dwPtr ) m_dwLine = i;
		else m_dwLine = m_dwPtr;

	} // end else
}

	
BOOL CItemList::Draw(HDC hDC, LPRECT pRect, HWND hWnd )
{
	// Reset default colors if needed
	if ( m_bAutoDefaultColors ) DefaultColors();

	// Check for no draw
	if ( m_dwNoDraw ) { m_dwNoDraw--; return TRUE; }

	RECT draw;
	CopyRect( &draw, pRect );

	// Fill in the back ground
	CGrDC::VertGradientFill( hDC, &draw, m_rgbBckTop, m_rgbBckBottom );

	// Draw side bar
	if ( m_bGroups )
	{	draw.right = draw.left + GROUPOFFSET;
		CGrDC::VertGradientFill( hDC, &draw, m_rgbBarTop, m_rgbBarBottom );
		CopyRect( &draw, pRect );
	} // end if

	// Draw the header
	m_header.Draw( hDC, &draw, m_lHScroll, m_bGroups ? GROUPOFFSET : 0 );

	// Draw line under header
	CGrDC::Line(	hDC, m_rgbHorzLines, 1,
					draw.left, draw.top + m_header.GetHeight(), 
					draw.right, draw.top + m_header.GetHeight() );


	// Offset header for groups if needed
	if ( m_bGroups ) draw.left += GROUPOFFSET;

	// Offset the rect below the header
	draw.top += m_header.GetHeight();
	if ( draw.top >= draw.bottom ) return TRUE;

	// Item rect
	RECT ir;				
	ir.left = draw.left;
	ir.top = draw.top;
	ir.right = draw.right;
	ir.bottom = draw.top;

	BOOL bCollapsed = FALSE;

	DWORD i = 0;

	if ( !m_bGroups ) i = m_dwLine;
	else while ( i < m_dwLine && i < m_dwPtr )
	{	
		if ( m_pIndex[ i ] != NULL ) 
		{	
			if ( ( m_pIndex[ i ]->flags & LIF_GROUP ) != 0 )
			{	if ( ( m_pIndex[ i ]->flags & LIF_COLLAPSED ) != 0 ) bCollapsed = TRUE;
				else bCollapsed = FALSE;
			} // end if
		} // end if
		i++;
	} // end while

	while ( i < m_dwPtr && ir.bottom < pRect->bottom )
	{
		LPLISTITEMINFO	plii = m_pIndex[ i ];

		// Skip collapsed items
		if ( bCollapsed ) 
			while ( plii != NULL && ( plii->flags & LIF_GROUP ) == 0 )
				if ( ++i < m_dwPtr ) plii = m_pIndex[ i ]; else plii = NULL;

		if ( plii != NULL )
		{
			// Calculate list item height
			ir.bottom = ir.top + plii->height;

			if ( m_bGroups && ( plii->flags & LIF_GROUP ) == 0 )
			{
				CGrDC::FillSolidRect( hDC, &ir, GetSysColor( COLOR_WINDOW ) );

			} // end if			

//			if ( ir.bottom < pRect->bottom )
			{
				// Draw tab
				if ( m_bGroups && ( plii->flags & LIF_GROUP ) != 0 )
				{
					RECT tab;
					CopyRect( &tab, &ir );
					tab.left = pRect->left; 
					tab.right = ir.left - 1;
					tab.bottom = tab.top + RW( tab );
					InflateRect( &tab, -2, -2 );

					CGrDC::Center( &tab, &ir, FALSE, TRUE ); 

					// Are we using XP themes?
//					if ( FALSE )
					if ( CXpTheme::IsWindowThemed( hWnd ) )
					{
						CXpTheme xpthm( hWnd, "TREEVIEW" );
						if ( ( plii->flags & LIF_COLLAPSED ) != 0 )
							xpthm.DrawThemeBackground( hDC, TVP_GLYPH, GLPS_CLOSED, &tab, NULL );
						else xpthm.DrawThemeBackground( hDC, TVP_GLYPH, GLPS_OPENED, &tab, NULL );

					} // end if

					else
					{
						CGrDC::FillSolidRect( hDC, &tab, m_rgbBckTop );
						CGrDC::Box3d( hDC, &tab, 1, RGB( 0, 0, 0 ), RGB( 0, 0, 0 ) );

						long mh = tab.top + ( RH( tab ) / 2 );
						long mw = tab.left + ( RW( tab ) / 2 );

						CGrDC::Line(	hDC, RGB( 0, 0, 0 ), 1,
										tab.left + 2, mh, tab.right - 2, mh );
						if ( ( plii->flags & LIF_COLLAPSED ) != 0 )
							CGrDC::Line(	hDC, RGB( 0, 0, 0 ), 1,
											mw, tab.top + 2, mw, tab.bottom - 2 );

					} // end else

				} // end if

				// Draw this list item
				DrawItem( i, hDC, &ir );

				if ( m_bHorzLines )
				{
					CGrDC::Line(	hDC, m_rgbHorzLines, 1,
									ir.left, ir.top, ir.right, ir.top );

					if ( m_bGroups && ( plii->flags & LIF_GROUP ) != 0 &&
						 ( plii->flags & LIF_COLLAPSED ) != 0 )
					{	
						HPEN hPen = CreatePen( PS_SOLID, 1, m_rgbSelTop );
						HPEN hOldPen = (HPEN)::SelectObject( hDC, hPen );
						COLORREF oldcolor = SetBkColor( hDC, m_rgbBckTop );

						MoveToEx( hDC, ir.left, ir.bottom - 1, NULL );
						LineTo( hDC, ir.right, ir.bottom - 1 );

						::SelectObject( hDC, hOldPen );
						::DeleteObject( hPen );
						hPen = CreatePen( PS_SOLID, 1, RGB( 220, 220, 220 ) );
						hOldPen = (HPEN)::SelectObject( hDC, hPen );
						MoveToEx( hDC, ir.left, ir.bottom - 2, NULL );
						LineTo( hDC, ir.right, ir.bottom - 2 );

						
						// Release drawing objects
						SetBkColor( hDC, oldcolor );
						::SelectObject( hDC, hOldPen );
						::DeleteObject( hPen );

					} // end else


//					CGrDC::Box3d( hDC, &ir, 1, m_rgbHorzLines, m_rgbHorzLines );

				} // end if

				if ( m_bVertLines && ( plii->flags & LIF_GROUP ) == 0 )
				{
					long x = ir.left;

					CGrDC::Line(	hDC, m_rgbHorzLines, 1,
									x, ir.top, x, ir.bottom );

					x--;
					LPHEADERITEMINFO phii = NULL;
					while( ( phii = GetHeader().GetNext( phii ) ) != NULL &&
							GetHeader().GetNext( phii ) != NULL )
					{
						// Add width
						x += phii->width;

						// Draw vertical line
						CGrDC::Line(	hDC, m_rgbHorzLines, 1,
										x, ir.top, x, ir.bottom );

					} // end while

					CGrDC::Line(	hDC, m_rgbHorzLines, 1,
									ir.right - 1, ir.top, 
									ir.right - 1, ir.bottom );


				} // end if

			} // end if

			// Next item position
			ir.top = ir.bottom;

			// Are we collapsing?
			if ( ( plii->flags & LIF_GROUP ) != 0 )
			{	if ( ( plii->flags & LIF_COLLAPSED ) != 0 ) bCollapsed = TRUE;
				else bCollapsed = FALSE;
			} // end if

		} // end if

		// Next item
		i++;

	} // end while

	// Draw last horz line
	if ( m_bHorzLines )
		CGrDC::Line(	hDC, m_rgbHorzLines, 1,
						ir.left, ir.bottom, ir.right, ir.bottom );

/*	if ( m_bVertLines )
	{
		long x = draw.left;

		CGrDC::Line(	hDC, m_rgbHorzLines, 1,
						x, draw.top, x, ir.bottom );

		x--;
		LPHEADERITEMINFO phii = NULL;
		while( ( phii = GetHeader().GetNext( phii ) ) != NULL )
		{
			// Add width
			x += phii->width;

			// Draw vertical line
			CGrDC::Line(	hDC, m_rgbHorzLines, 1,
							x, draw.top, x, ir.bottom );

		} // end while
	} // end if
*/
	
	return TRUE;
}

BOOL CItemList::DrawItem( DWORD i, HDC hDC, LPRECT pRect)
{
	CText				text;
	RECT				rect;
	LPLISTSUBITEMINFO	plsii = m_pIndex[ i ]->head;
	LPHEADERITEMINFO	phii = NULL;

	text.SetFlags( DT_SINGLELINE | DT_VCENTER );
	text.SetColor( m_rgbText );

	// Use bold text for group names
	if ( ( m_pIndex[ i ]->flags & LIF_GROUP ) != 0 )
		text.SetWeight( FW_BOLD );

	// Rect
	CopyRect( &rect, pRect ); InflateRect( &rect, 0, -1 );

	if ( ( m_pIndex[ i ]->flags & LIF_SELECTED ) != 0 &&
		 ( m_pIndex[ i ]->flags & LIF_GROUP ) == 0 )
	{
		CGrDC::VertGradientFill( hDC, &rect, m_rgbSelTop, m_rgbSelBottom );
	} // end if

	// Adjust for horz scroll
	rect.left -= m_lHScroll;

//	rect.top += 1;

	BOOL bSpecialFont = TRUE;

	// Draw all items
	while ( ( phii = m_header.GetNext( phii ) ) != NULL && 
			plsii != NULL && rect.left < pRect->right )
	{
		// Set special font
		if ( plsii->pfont != NULL ) { bSpecialFont = TRUE; text.SetFont( plsii->pfont ); }

		// Set default font
		else if ( bSpecialFont ) { bSpecialFont = FALSE; text.SetFont( 16, "Arial" ); }

		if ( m_bGroups && ( m_pIndex[ i ]->flags & LIF_GROUP ) != 0 )
			rect.right = pRect->right;

		else
		{	// Calculate item width
			rect.right = rect.left + phii->width;
			if ( rect.right > pRect->right ) rect.right = pRect->right;
		} // end else

		if ( rect.right > pRect->left )
		{
			// Fill in bck if needed
			if ( ( m_pIndex[ i ]->flags & LIF_SELECTED ) != 0 &&
				 ( m_pIndex[ i ]->flags & LIF_GROUP ) == 0 )
			{
				if ( plsii->rgbbck != MAXDWORD )
				{		
					RECT b;
					CopyRect( &b, &rect );
					InflateRect( &b, -2, -1 ); b.top++; b.right--;
					CGrDC::FillSolidRect( hDC, &b, plsii->rgbbck );
//					COLORREF bottom = CGrDC::ScaleColor( plsii->rgbbck, -50 );
//					CGrDC::GradientFill( hDC, &rect, plsii->rgbbck, bottom );
				} // end if
			} // end if
			else
			{
				if ( plsii->rgbbck != MAXDWORD )
				{
					RECT b;
					CopyRect( &b, &rect );
					InflateRect( &b, -2, -1 ); b.top++; b.right--;
					CGrDC::FillSolidRect( hDC, &b, plsii->rgbbck );
//					CGrDC::GradientFill( hDC, &rect, plsii->rgbbck, m_rgbSelBottom );
				} // end if
			} // end else

			// Draw icon if needed
			if ( plsii->icon != NULL )
			{
				DrawIconEx( hDC, rect.left + 2, rect.top + 2, plsii->icon, 
							16, 16, 0, NULL, DI_NORMAL );
				rect.left += 17;
			} // end if

			if ( plsii->type == SIT_TEXT )
			{
				rect.left += 3;

				// Did the user specify a color?
				if ( plsii->rgbtext != MAXDWORD ) text.SetColor( plsii->rgbtext );

				// Is Item selected?
				else if (	( m_pIndex[ i ]->flags & LIF_SELECTED ) != 0 &&
							( m_pIndex[ i ]->flags & LIF_GROUP ) == 0 )

					text.SetColor( m_rgbSelText );

				// Set default text color
				else text.SetColor( m_rgbText );


				text.DrawText( hDC, (char*)plsii->data, &rect );			
			} // end if

		} // end if

		// Shift rect
		rect.left = rect.right;

		// Next sub-item
		plsii = plsii->pNext; 

	} // end while

	return TRUE;
}

BOOL CItemList::SelectItem(DWORD index, BOOL bSelect, BOOL bToggle, BOOL bMulti, BOOL bRange)
{
	DWORD r = MAXDWORD;

	if ( m_bMultiSelect && bRange )
	{
		for ( DWORD i = 0; r == MAXDWORD && i < m_dwPtr; i++ )
			if ( ( m_pIndex[ i ]->flags & LIF_SELECTED ) != 0 )
				r = i;
	} // end if

	// Sanity check
	if ( index < m_dwPtr && m_pIndex[ index ] != NULL ) 
	{
		// Select / Deselect item
		if ( bToggle ) m_pIndex[ index ]->flags ^= LIF_SELECTED;
		else
		{	if ( bSelect ) m_pIndex[ index ]->flags |= LIF_SELECTED;
			else m_pIndex[ index ]->flags &= ~LIF_SELECTED;
		} // end if
	} // end if

	// If not multi select
	if ( !m_bMultiSelect || !bMulti ) 
		for ( DWORD i = 0; i < m_dwPtr; i++ )

			// Deselect items
			if ( i != index ) m_pIndex[ i ]->flags &= ~LIF_SELECTED;

	// Select range if needed
	if ( m_bMultiSelect && bRange && r != MAXDWORD )
	{
		DWORD s = ( index < r ) ? s = index : s = r;
		DWORD e = ( index > r ) ? e = index : e = r;

		for ( DWORD i = s; i < e; i++ )
			if ( m_pIndex[ i ] != NULL )
				m_pIndex[ i ]->flags |= LIF_SELECTED;
	} // end if

	if ( index < m_dwPtr && m_pIndex[ index ] != NULL ) 
	{
		if ( ( m_pIndex[ index ]->flags & LIF_GROUP ) != 0 )
		{
			if ( ( m_pIndex[ index ]->flags & LIF_COLLAPSED ) == 0 )
				m_pIndex[ index ]->flags |= LIF_COLLAPSED;
			else m_pIndex[ index ]->flags &= ~LIF_COLLAPSED;

		} // end if
	} // end if

	return TRUE;
}

DWORD CItemList::GetItem(long x, long y, LPRECT pRect)
{
	// Sanity check
	if ( pRect == NULL ) return MAXDWORD;
	if ( x < pRect->left || x > pRect->right ) return MAXDWORD;
	if ( y < pRect->top || y > pRect->bottom ) return MAXDWORD;
	
	RECT draw;
	CopyRect( &draw, pRect );
	
	// Offset the rect below the header
	draw.top += m_header.GetHeight();
	if ( draw.top >= draw.bottom ) return MAXDWORD;

	// Item rect
	RECT ir;				
	ir.left = draw.left;
	ir.top = draw.top;
	ir.right = draw.right;
	ir.bottom = draw.top;

	POINT	pt;	
	pt.x = x;
	pt.y = y;

	BOOL bCollapsed = FALSE;

	DWORD i = 0;
	while ( i < m_dwLine && i < m_dwPtr )
	{	if ( m_pIndex[ i ] != NULL ) 
			if ( ( m_pIndex[ i ]->flags & LIF_GROUP ) != 0 )
			{	if ( ( m_pIndex[ i ]->flags & LIF_COLLAPSED ) != 0 ) bCollapsed = TRUE;
				else bCollapsed = FALSE;
			} // end if
		i++;
	} // end while

	while ( i < m_dwPtr && ir.bottom < pRect->bottom )
	{
		LPLISTITEMINFO	plii = m_pIndex[ i ];

		// Skip collapsed items
		if ( bCollapsed ) 
			while ( plii != NULL && ( plii->flags & LIF_GROUP ) == 0 )
				if ( ++i < m_dwPtr ) plii = m_pIndex[ i ]; else plii = NULL;

		if ( plii != NULL )
		{
			// Calculate list item height
			ir.bottom = ir.top + plii->height;

			// Is it visible
//			if ( ir.bottom < pRect->bottom )
			{
				// Return true if this is our point
				if ( PtInRect( &ir, pt ) ) return i;

			} // end if

			// Next item position
			ir.top = ir.bottom;

			// Are we collapsing?
			if ( ( plii->flags & LIF_GROUP ) != 0 )
			{	if ( ( plii->flags & LIF_COLLAPSED ) != 0 ) bCollapsed = TRUE;
				else bCollapsed = FALSE;
			} // end if

		} // end if

		// Next item
		i++;

	} // end while

	return MAXDWORD;
}

BOOL CItemList::OnLButtonDown(UINT flags, LPPOINT point, LPRECT prect )
{
	DWORD i = GetItem( point, prect );

	// Select items
	SelectItem( i, TRUE, TRUE, ( GetKeyState( VK_CONTROL ) & 0x8000 ), 
				( GetKeyState( VK_SHIFT ) & 0x8000 ) != 0 );

	// Set scroll information
	if (	i < m_dwPtr && m_pIndex[ i ] != NULL && 
			( m_pIndex[ i ]->flags & LIF_GROUP ) != 0 )
	{
		// Validate max scroll line
		DWORD slm = GetScrollLineMax( prect );
		DWORD cl = GetCurLine();
		if ( cl > slm ) SetCurLine( slm );
	} // end if

	// Punt if item clicked
	if ( i != MAXDWORD )return TRUE;

	// Are we clicking a header resize area?
	m_phii = GetHeader().GetHeaderResizeColumn( prect, point, m_lHScroll );
	if ( m_phii != NULL )
	{
		m_phiiwidth = m_phii->width;
	} // end if

	// Save click location
	m_lbuttondown.x = point->x; m_lbuttondown.y = point->y;

	return FALSE;
}

BOOL CItemList::OnMouseMove(UINT flags, LPPOINT point, LPRECT prect)
{
	// Clear if button is let up
	if ( ( flags & MK_LBUTTON ) == 0 ) m_phii = NULL;

	if ( m_phii != NULL )
	{
		long nw = m_phiiwidth - ( m_lbuttondown.x - point->x );
		if ( nw > 10 ) m_phii->width = nw;
		else m_phii->width = 10;
		
		return TRUE;	

	} // end if

	return FALSE;
}

BOOL CItemList::OnLButtonUp()
{
	m_phii = NULL;

	return FALSE;
}

HCURSOR CItemList::GetCursor( LPRECT pRect, LPPOINT pt )
{
	// Are over a header resize area?
	if (	m_phii != NULL || 
			GetHeader().GetHeaderResizeColumn( pRect, pt, m_lHScroll ) != NULL )
		return LoadCursor( NULL, IDC_SIZEWE );

	// Use default cursor
	return LoadCursor( NULL, IDC_ARROW );
}

DWORD CItemList::GetScrollLineMax(LPRECT pRect)
{
	DWORD lpp = GetLinesPerPage( pRect );
	DWORD ucl = GetUncollapsedLines();

	// Will all the lines fit on the page?
	if ( lpp >= ucl ) return 0;

	// Return number of lines that don't fit on the page
	return ucl - lpp;
}


DWORD CItemList::GetLinesPerPage(LPRECT pRect)
{
	// Sanity check
	if ( pRect == NULL || m_pIndex == NULL ) return FALSE;

	// Get first list item
	LPLISTITEMINFO plii = m_pIndex[ 0 ];
	if ( plii == NULL ) return FALSE;

	long h = PRH( pRect );
	long th = plii->height;
	
	// valid per page data?
	if ( h == 0 || th == 0 ) return 0;

	// Calculate lines per page
	long lpp = ( h / th ) - 1;
	if ( lpp < 0 ) lpp = 0;

	return (DWORD)lpp;

}


DWORD CItemList::GetFirstSelectedItem()
{

	for( DWORD i = 0; i < m_dwPtr; i++ )
	{
		LPLISTITEMINFO	plii = m_pIndex[ i ];

		// Valid index?
		if ( plii != NULL )
			if ( ( plii->flags & LIF_SELECTED ) != 0 )
				return i;

	} // end while

	return MAXDWORD;
}

BOOL CItemList::GetItemRect(DWORD item, LPRECT pRect)
{
	// Sanity check
	if ( pRect == NULL ) return FALSE;
	if ( item < m_dwLine ) return FALSE;
	if ( item >= m_dwPtr ) return FALSE;
	
	// Item rect
	RECT ir;				
	ir.left = pRect->left;
	ir.top = pRect->top + m_header.GetHeight();
	ir.right = pRect->right;
	ir.bottom = pRect->top;

	if ( m_bGroups ) ir.left += GROUPOFFSET;
	
	// Verify space for items
	if ( ir.top >= pRect->bottom ) return MAXDWORD;

	BOOL bCollapsed = FALSE;

	DWORD i = 0;
	while ( i < m_dwLine && i < m_dwPtr )
	{	if ( m_pIndex[ i ] != NULL ) 
			if ( ( m_pIndex[ i ]->flags & LIF_GROUP ) != 0 )
			{	if ( ( m_pIndex[ i ]->flags & LIF_COLLAPSED ) != 0 ) bCollapsed = TRUE;
				else bCollapsed = FALSE;
			} // end if
		i++;
	} // end while

	while ( i < m_dwPtr && ir.bottom < pRect->bottom )
	{
		LPLISTITEMINFO	plii = m_pIndex[ i ];

		// Skip collapsed items
		if ( bCollapsed ) 
			while ( plii != NULL && ( plii->flags & LIF_GROUP ) == 0 )
				if ( ++i < m_dwPtr ) plii = m_pIndex[ i ]; else plii = NULL;

		if ( plii != NULL )
		{
			// Calculate list item height
			ir.bottom = ir.top + plii->height;

			// Is it the item we want?
			if ( i == item )
			{
				CopyRect( pRect, &ir );
				return TRUE;
			} // end if

			// Next item position
			ir.top = ir.bottom;

			// Are we collapsing?
			if ( ( plii->flags & LIF_GROUP ) != 0 )
			{	if ( ( plii->flags & LIF_COLLAPSED ) != 0 ) bCollapsed = TRUE;
				else bCollapsed = FALSE;
			} // end if

		} // end if

		// Next item
		i++;

	} // end while

	return MAXDWORD;
}

DWORD CItemList::FindString(LPCTSTR pCol, LPCTSTR pStr)
{
	// Get column index
	DWORD i = GetHeader().FindCol( pCol );
	if ( i == MAXDWORD ) return i;

	return FindString( i, pStr );
}

DWORD CItemList::FindString(DWORD dwCol, LPCTSTR pStr)
{
	for( DWORD i = 0; i < m_dwPtr; i++ )
	{
		LPLISTSUBITEMINFO plsii = GetSubItem( i, dwCol );
		if (	plsii != NULL && plsii->type == SIT_TEXT &&
				!strcmpi( (LPCTSTR)plsii->data, pStr ) ) return i;
	} // end for

	return MAXDWORD;
}

BOOL CItemList::GetSubItemText(DWORD item, DWORD subitem, LPSTR buf, DWORD size)
{
	LPLISTSUBITEMINFO plsii = GetSubItem( item, subitem );
	if ( plsii == NULL ) return FALSE;

	// Is it in fact text?
	if ( plsii->type != SIT_TEXT ) return FALSE;

	// Copy text string
	strncpy( buf, (char*)plsii->data, size );

	// NULL terminate to be sure
	buf[ size - 1 ] = NULL;

	return TRUE;
}

long CItemList::GetTotalWidth()
{
	long width = 0;
	LPHEADERITEMINFO phii = NULL;

	while ( ( phii = GetHeader().GetNext( phii ) ) != NULL )
		width += phii->width;

	return width;
}


BOOL CItemList::SetSubItemTextColor(DWORD index, DWORD subitem, COLORREF rgb)
{
	LPLISTSUBITEMINFO	plsii = GetSubItem( index, subitem );
	if ( plsii == NULL ) return FALSE;

	// Save data
	plsii->rgbtext = rgb;

	return TRUE;
}

BOOL CItemList::SetSubItemBckColor(DWORD index, DWORD subitem, COLORREF rgb)
{
	LPLISTSUBITEMINFO	plsii = GetSubItem( index, subitem );
	if ( plsii == NULL ) return FALSE;

	// Save data
	plsii->rgbbck = rgb;

	return TRUE;
}

BOOL CItemList::SetItemData(DWORD item, DWORD data)
{
	if ( item >= m_dwPtr || m_pIndex[ item ] == NULL ) 
		return FALSE;

	m_pIndex[ item ]->user = data;

	return TRUE;
}

DWORD CItemList::GetItemData(DWORD item)
{
	if ( item >= m_dwPtr || m_pIndex[ item ] == NULL ) 
		return 0;

	return m_pIndex[ item ]->user;
}

BOOL CItemList::SetSubItemText(DWORD index, DWORD subitem, LPCTSTR pText)
{
	return ModifySubItem( index, subitem, (void*)pText );
}

DWORD CItemList::GetUncollapsedLines()
{
	BOOL bCollapsed = FALSE;
	DWORD lines = 0;
	DWORD i = 0;
	while ( i < m_dwPtr )
	{	
		if ( m_pIndex[ i ] != NULL ) 
		{
			// Is it a group?
			if ( ( m_pIndex[ i ]->flags & LIF_GROUP ) != 0 )
			{	lines++;
				if ( ( m_pIndex[ i ]->flags & LIF_COLLAPSED ) != 0 ) bCollapsed = TRUE;
				else bCollapsed = FALSE;
			} // end if
			else if ( !bCollapsed ) lines++;

		} // end if

		i++;

	} // end while

	return lines;
}

BOOL CItemList::GetSubItemData(DWORD item, DWORD subitem, LPDWORD pdwData)
{
	// Sanity check
	if ( pdwData == NULL ) return FALSE;

	// Get sub item
	LPLISTSUBITEMINFO plsii = GetSubItem( item, subitem );
	if ( plsii == NULL ) return FALSE;

	*pdwData = plsii->user;

	return TRUE;
}

BOOL CItemList::SetSubItemData(DWORD item, DWORD subitem, DWORD data)
{
	// Get sub item
	LPLISTSUBITEMINFO plsii = GetSubItem( item, subitem );
	if ( plsii == NULL ) return FALSE;

	plsii->user = data;

	return TRUE;
}
