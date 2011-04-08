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
// DrawGraph.cpp: implementation of the CDrawGraph class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DrawGraph.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDrawGraph::CDrawGraph()
{
	m_dwTime = 0;
	m_dwTimePerPixel = 50;
	m_dwGraphSize = 2048;
	m_dwLastWindow = 0;
}

CDrawGraph::~CDrawGraph()
{	Destroy();
}

BOOL CDrawGraph::Draw(HDC hDC, LPRECT pRect)
{
	// Sanity check
	if ( hDC == NULL || pRect == NULL ) return FALSE;

	// Create background bitmap
	if ( m_bck.GetSafeHdc() == NULL || m_bck.GetHeight() != PRH( pRect ) )
	{	if ( !m_bck.Create( hDC, 0, 0, 64, PRH( pRect ) ) ) return FALSE;
//		CGrDC::VertGradientFill( m_bck, m_bck, RGB( 80, 80, 120 ), RGB( 0, 0, 0 ) );
		CGrDC::VertGradientFill(	m_bck, m_bck, 
									CGrDC::ScaleColor( GetSysColor( COLOR_ACTIVECAPTION ), -50 ), 
									RGB( 0, 0, 0 ) );
	} // end if

	// Draw the background
	m_bck.Tile( hDC, pRect );

	// Get tick
	if ( m_dwTimePerPixel == 0 ) m_dwTimePerPixel = 1000;
	DWORD tick = m_dwTime / m_dwTimePerPixel;

	// One second on chart
	DWORD onesecond = 1000 / m_dwTimePerPixel;

	// Draw grid
//	CGrDC::DrawGrid( hDC, pRect, RGB( 0, 120, 0 ), onesecond, 10, tick % onesecond );
	CGrDC::DrawGrid(	hDC, pRect, 
						CGrDC::ScaleColor( GetSysColor( COLOR_INACTIVECAPTION ), -100 ), 
						onesecond, 10, tick % onesecond );

	// Do we have a valid time?
	if ( m_dwTime == 0 ) return FALSE;

	// Get width and height
	long w = PRW( pRect );
	long h = PRH( pRect );
	if ( w <= 0 || h <= 0 ) return FALSE;

	// Calc time values
/*	long maxtime = m_dwTime;
	long timescale = m_dwTimePerPixel * w;
	long mintime = 0;
	if ( (DWORD)timescale < m_dwTime ) mintime = m_dwTime - timescale;
	long timespan = maxtime - mintime;
*/
	DWORD tmrange = m_dwTimePerPixel * w;
	DWORD tmmax = m_dwTime;
	DWORD tmmin = ( m_dwTime > (DWORD)tmrange ) ? m_dwTime - tmrange : 0;
	m_dwLastWindow = (DWORD)tmrange;

	long icons[ 32 ];
	ZeroMemory( icons, sizeof( icons ) );

	// For each graph element
	BOOL bMove = TRUE;
	LPGRAPHELEMENTINFO pgei = NULL;
	while ( ( pgei = (LPGRAPHELEMENTINFO)GetNext( pgei ) ) != NULL )
	{
		if ( pgei->type == 1 )
		{
			// Get ready to draw
			CGrDC dc( hDC, FALSE );
			CPen pen( PS_SOLID, 1, pgei->rgbline );
			dc.SelectObject( pen );

			// For each point
			for ( DWORD i = 0; i < pgei->validpts; i++ )
				if ( pgei->ptimes[ i ] >= tmmin && pgei->ptimes[ i ] <= tmmax )
				{
					POINT pt;
					pt.x = ( ( pgei->ptimes[ i ] - tmmin ) * w ) / tmrange;
					pt.y = ( pgei->ppts[ i ] * h ) / 1000;
					pt.x = w - pt.x; pt.y = h - pt.y;

					if ( pt.y > ( pRect->bottom - 1 ) ) pt.y = pRect->bottom - 1;
					else if ( pt.y < ( pRect->top + 1 ) ) pt.y = pRect->top + 1;

					// Create a line
					if ( bMove ) { bMove = FALSE; dc.MoveTo( pt.x, pt.y ); }
					else dc.LineTo( pt.x, pt.y );

				} // end for

		} // end if

		else if ( pgei->type == 2 )
		{
			POINT pt;
			pt.x = ( ( pgei->time - tmmin ) * w ) / tmrange;
			pt.x = w - pt.x;

			pt.y = pRect->top + 4;
			DWORD where = 0;
			while ( pt.y < pRect->bottom && where < 32 && 
					icons[ where ] != 0 && icons[ where ] < pt.x + 16 )
			{	where++; pt.y += 16; }
			if ( where < 32 )
			{
				// Save icon location
				icons[ where ] = pt.x;

				// Is it in the rect?
				if ( PtInRect( pRect, pt ) && pt.x < pRect->right - 16 && pt.y < pRect->bottom - 16 )
	
					// Draw the icon
					if ( pgei->icon != NULL ) CGrDC::DrawIcon( hDC, NULL, pgei->icon, pt.x, pt.y );

			} // end if

		} // end else if 

	} // end while

	return TRUE;
}

void CDrawGraph::DeleteObject(void *node)
{
	LPGRAPHELEMENTINFO pgei = (LPGRAPHELEMENTINFO)node;
	if ( pgei == NULL ) return;

	ReleaseMemory( pgei );

	CHList::DeleteObject( node );
}

BOOL CDrawGraph::Add(LPCTSTR pName, DWORD dwTime, DWORD dwValue, COLORREF rgbLine, HICON hIcon)
{
	// Use current time if not supplied
	if ( dwTime == 0 ) dwTime = GetTickCount();

	// Get the memory
	LPGRAPHELEMENTINFO pgei = (LPGRAPHELEMENTINFO)Find( pName );
	if ( pgei == NULL )
	{
		// Create new list entry
		pgei = (LPGRAPHELEMENTINFO)New( NULL, 0, pName );
		if ( pgei == NULL ) return TRUE;

	} // end if

	if ( hIcon == NULL )
	{
		pgei->type = 1;

		// Allocate memory
		if ( !AllocateMemory( pgei, m_dwGraphSize ) ) 
		{	Delete( pgei ); return FALSE; }

		// Don't overflow
		if ( pgei->validpts > pgei->pts ) pgei->validpts = pgei->pts;

		// Make some room
		if ( pgei->validpts > 0 )
			for ( DWORD i = pgei->validpts - 1; i > 0; i-- )
			{	pgei->ppts[ i ] = pgei->ppts[ i - 1 ];
				pgei->ptimes[ i ] = pgei->ptimes[ i - 1 ];
			} // end for
				
		// Save data
		pgei->ppts[ 0 ] = dwValue;
		pgei->ptimes[ 0 ] = dwTime;
		pgei->rgbline = rgbLine;

		// Increment valid point counter
		if ( pgei->validpts < pgei->pts ) pgei->validpts++;

	} // end if

	else
	{
		pgei->type = 2;

		// Don't need any point memory
		ReleaseMemory( pgei );

		// Save icon and time
		pgei->icon = hIcon;
		pgei->time = dwTime;

	} // end else


	return TRUE;
}

BOOL CDrawGraph::Tick(DWORD dwTick)
{
	// Save tick count
	if ( dwTick == 0 ) m_dwTime = GetTickCount();
	else m_dwTime = dwTick;

	// Clean up the list
	Clean();

	return TRUE;
}

BOOL CDrawGraph::AllocateMemory(LPGRAPHELEMENTINFO pgei, DWORD size)
{
	// Sanity check
	if ( pgei == NULL ) return FALSE;

	// Want any memory?
	if ( size == 0 ) { ReleaseMemory( pgei ); return TRUE; }

	// Do we already have what they want?
	if ( pgei->pts == size && pgei->ppts != NULL && pgei->ptimes != NULL ) 
		return TRUE;

	// Save current data pointers if any
	long *data = NULL;
	DWORD *times = NULL;
	if ( pgei->ppts != NULL ) data = pgei->ppts;
	if ( pgei->ptimes != NULL ) times = pgei->ptimes;

	// Allocate memory
	pgei->ppts = new long[ size ];
	pgei->ptimes = new DWORD[ size ];

	// Ensure we got the memory
	if ( pgei->ppts == NULL || pgei->ptimes == NULL )
	{	DeleteObject( pgei ); return FALSE; }

	// Save size
	pgei->pts = size;

	// Copy old values if any
	if ( pgei->validpts > 0 )
	{
		// Bounds check size
		if ( pgei->validpts > size ) pgei->validpts = size;

		// Copy old data values
		if ( data != NULL ) memcpy( pgei->ppts, data, pgei->validpts );		

		// Copy old time values
		if ( times != NULL ) memcpy( pgei->ptimes, times, pgei->validpts );
		
	} // end if

	// Delete pointer / init memory
	if ( data != NULL ) delete [] data;

	// Delete pointer / init memory
	if ( times != NULL ) delete [] times;

	return TRUE;
}

void CDrawGraph::ReleaseMemory(LPGRAPHELEMENTINFO pgei)
{
	if ( pgei->ppts != NULL )
	{	delete [] pgei->ppts;
		pgei->ppts = NULL;
	} // end if

	if ( pgei->ptimes != NULL )
	{	delete [] pgei->ptimes;
		pgei->ptimes = NULL;
	} // end if

	pgei->pts = 0;
	pgei->validpts = 0;
}

BOOL CDrawGraph::Clean()
{
	// Verify window
	if ( m_dwLastWindow == 0 || m_dwLastWindow > m_dwTime ) return FALSE;

	// For each graph element
	LPGRAPHELEMENTINFO pgei = NULL;
	while ( ( pgei = (LPGRAPHELEMENTINFO)GetNext( pgei ) ) != NULL )
	{
		if ( pgei->type == 1 )
		{	if (	pgei->validpts == 0 || 
					pgei->ptimes == NULL || 
					pgei->ptimes[ 0 ] < ( m_dwTime - m_dwLastWindow ) )
			{	LPGRAPHELEMENTINFO prev = (LPGRAPHELEMENTINFO)pgei->pPrev;
				Delete( pgei ); pgei = prev;
			} // end if
		} // end if
		else if ( pgei->type == 2 )
		{	if ( pgei->time < ( m_dwTime - m_dwLastWindow ) )
			{	LPGRAPHELEMENTINFO prev = (LPGRAPHELEMENTINFO)pgei->pPrev;
				Delete( pgei ); pgei = prev;
			} // end if
		} // end else


	} // end while


	return TRUE;
}
