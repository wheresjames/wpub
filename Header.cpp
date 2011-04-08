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
// Header.cpp: implementation of the CHeader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Header.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHeader::CHeader()
{
	m_dwSize = 0;
	m_pHead = NULL;
	m_pTail = NULL;

	m_lHeight = 20;

	m_bAutosize = FALSE;

}

CHeader::~CHeader()
{
	Destroy();
}

void CHeader::Destroy()
{
	while ( m_pHead != NULL )
	{
		LPHEADERITEMINFO next = m_pHead->pNext;
		delete m_pHead;
		m_pHead = next;
	} // end while

	m_dwSize = 0;
	m_pTail = NULL;
}

BOOL CHeader::AddHeaderItem(long width, LPCTSTR pText, DWORD flags, HICON icon, COLORREF rgbtext, COLORREF rgbbck)
{
	// Allocate memory
	LPHEADERITEMINFO node = new HEADERITEMINFO;
	if ( node == NULL ) return FALSE;

	// Save header item info
	if ( pText != NULL ) strcpy( node->title, pText );
	else *node->title = 0x0;
	node->width = width;
	node->icon = icon;
	node->flags = flags;
	node->rgbtext = rgbtext;
	node->rgbbck = rgbbck;

	// Add this node to the list
	node->pNext = NULL;
	node->pPrev = m_pTail;
	if ( m_pTail == NULL ) m_pHead = node;
	else m_pTail->pNext = node;
	m_pTail = node;
	m_dwSize++;		

	return TRUE;
}


BOOL CHeader::Draw(HDC hDC, LPRECT pRect, long lHScroll, long lSkip )
{
	// Sanity check
	if ( hDC == NULL || pRect == NULL ) return FALSE;

	// Size if needed
	if ( m_bAutosize )
	{	long hi = Size();
		if ( hi > 0 )
		{	long w = ( PRW( pRect ) - lSkip ) / hi;
			SetHeaderItemWidths( w );
		} // end if
	} // end if

	// Real header size
	RECT ir;
	CopyRect( &ir, pRect );

	// Adjust size
	if ( ir.bottom > ( ir.top + m_lHeight ) )
		ir.bottom = ir.top + m_lHeight;

	// Fill in bck
//	CGrDC::FillSolidRect( hDC, &ir, RGB( 128, 128, 128 ) );
//	CGrDC::GradientFill(	hDC, &ir, 
//							RGB( 128, 128, 128 ), 
//							RGB( 200, 200, 200 ) );
	CGrDC::VertGradientFill(	hDC, &ir, 
								CGrDC::ScaleColor( GetSysColor( COLOR_APPWORKSPACE ), -40 ), 
								CGrDC::ScaleColor( GetSysColor( COLOR_APPWORKSPACE ), 40 ) );

	// Are there any columns?
	if ( m_dwSize == 0 ) return TRUE;

	// Skip if needed
	if ( lSkip ) 
	{
		RECT skip;
		CopyRect( &skip, &ir );

		ir.left += lSkip;
		skip.right = skip.left + lSkip;

		COLORREF top = CGrDC::ScaleColor( GetSysColor( COLOR_MENU ), 40 );
		COLORREF bottom = CGrDC::ScaleColor( GetSysColor( COLOR_MENU ), -40 );

		// Fill in background
		CGrDC	dc( hDC );
		CGrPen	dkgraypen( PS_SOLID, 1, RGB( 100, 100, 100 ) );

		dc.VertGradientFill( &skip, top, bottom );
		
		dc.SelectObject( dkgraypen );
		dc.MoveTo( skip.right - 1, skip.top );
		dc.LineTo( skip.right - 1, skip.bottom );

		dc.Restore();
		dc.Detach();


	} // end if

	CText text;

	// What font does the user want for menu's?
	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);			
	SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( info ), &info, 0 );

	text.SetWeight( FW_BOLD );
	text.SetFont( &info.lfMenuFont );

//	text.SetFont( 18, "Arial" );
	text.SetFlags( DT_VCENTER | DT_SINGLELINE );

	LPHEADERITEMINFO phii = NULL;

	// Offset for HScroll
	ir.left -= lHScroll;

	while ( ( phii = GetNext( phii ) ) != NULL &&
			ir.left < pRect->right )
	{
		if ( m_bAutosize && GetNext( phii ) == NULL )
			ir.right = pRect->right;

		// Get column rect
		else ir.right = ir.left + phii->width;

		// Is it scrolled onto the screen?
		if ( ir.right > pRect->left )
		{
			{ // draw button

				CGrDC	dc( hDC );
				CGrPen	whitepen( PS_SOLID, 1, RGB( 255, 255, 255 ) );
				CGrPen	dkgraypen( PS_SOLID, 1, RGB( 100, 100, 100 ) );

				if ( ( phii->flags & HIF_3D ) != 0 )
				{
					// Get gradient colors
//					COLORREF top = dc.ScaleColor( phii->rgbbck, 40 );
//					COLORREF bottom = dc.ScaleColor( phii->rgbbck, -40 );
					COLORREF top = CGrDC::ScaleColor( GetSysColor( COLOR_MENU ), 40 );
					COLORREF bottom = CGrDC::ScaleColor( GetSysColor( COLOR_MENU ), -40 );

					// Fill in background
					dc.VertGradientFill( &ir, top, bottom );
				} // end if

				else dc.FillSolidRect( &ir, phii->rgbbck );
				
				dc.SelectObject( whitepen );

				dc.MoveTo( ir.right - 1, ir.top );
				dc.LineTo( ir.left, ir.top );
				dc.LineTo( ir.left, ir.bottom );
				
				dc.SelectObject( dkgraypen );

				dc.LineTo( ir.right - 1, ir.bottom );
				dc.LineTo( ir.right - 1, ir.top );

				dc.Restore();
				dc.Detach();
				
			} // end draw button

			RECT tr;
			CopyRect( &tr, &ir );

			// Draw icon if needed
			if ( phii->icon != NULL )
			{
				DrawIconEx( hDC, tr.left + 2, tr.top + 2, phii->icon, 
							16, 16, 0, NULL, DI_NORMAL );
				tr.left += 17;
			} // end if

			// Offset Text
			tr.left += 5;

			// Draw shadow
			if ( ( phii->flags & HIF_SHADOWTEXT ) != 0 )
			{
				text.SetColor( RGB( 0, 0, 0 ) );
				text.DrawText( hDC, phii->title, &tr );
				OffsetRect( &tr, 1, 1 );
			} // end if

			// Draw header text
//			text.SetColor( phii->rgbtext );
			text.SetColor( GetSysColor( COLOR_MENUTEXT ) );
			text.DrawText( hDC, phii->title, &tr );

		} // end if

		// Next item
		ir.left += phii->width;
	
	} // end while	

	return TRUE;
}


LPHEADERITEMINFO CHeader::GetHeaderResizeColumn(LPRECT pRect, LPPOINT pt, long lHScroll )
{
	// Sanity check
	if ( pRect == NULL || pt == NULL ) return FALSE;

	// Is it in the y bounds?
	if ( pt->y < pRect->top || pt->y > ( pRect->top + GetHeight() ) )
		return FALSE;

	// Check each item
	long xpos = -lHScroll;
	LPHEADERITEMINFO phii = NULL;
	while( ( phii = GetNext( phii ) ) != NULL )
	{
		// Add header item width
		xpos += phii->width;

		// Are we resizing this item?
		if ( pt->x >= ( xpos - 4 ) && pt->x <= ( xpos + 4 ) )
			return phii;

		// Keep going?
		if ( pt->x <= xpos ) return FALSE;

	} // end while

	return FALSE;

}

BOOL CHeader::SetHeaderItemWidths(long min, HDC hDC)
{
	RECT	tr;
	CText text;
	text.SetFlags( DT_VCENTER | DT_SINGLELINE );

	LPHEADERITEMINFO phii = NULL;
	while ( ( phii = GetNext( phii ) ) != NULL )
	{
		// Start with margin
		long w = 10;

		// Add for icon
		if ( phii->icon != NULL ) w += 17;

		SetRect( &tr, 0, 0, 10000, 10000 );

		text.CalcRect( hDC, phii->title, &tr );

		w += tr.right - tr.left;

		phii->width = ( w > min ) ? w : min;

	} // end while

	return TRUE;
}

BOOL CHeader::SetHeaderItemWidths(long w)
{
	LPHEADERITEMINFO phii = NULL;
	while ( ( phii = GetNext( phii ) ) != NULL )
		phii->width = w;

	return TRUE;
}

long CHeader::GetHeaderItemWidth(DWORD i)
{	
	LPHEADERITEMINFO phii = GetHeaderItem( i );
	if ( phii == NULL ) return 0;

	return phii->width;
}

LPHEADERITEMINFO CHeader::GetHeaderItem(DWORD i)
{
	LPHEADERITEMINFO	phii = GetNext( NULL );

	while( i )
	{
		phii = GetNext( phii );
		if ( phii == NULL ) return NULL;

		i--;

	} // end while

	return phii;
}

DWORD CHeader::FindCol(LPCTSTR str)
{
	LPHEADERITEMINFO phii = NULL;

	DWORD i = 0;
	while ( ( phii = GetNext( phii ) ) != NULL )
	{	if ( !strcmpi( phii->title, str ) ) return i;
		i++;
	} // end while

	return MAXDWORD;
}

