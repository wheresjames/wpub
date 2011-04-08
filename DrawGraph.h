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
// DrawGraph.h: interface for the CDrawGraph class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRAWGRAPH_H__44CF80DF_0FC5_40C4_981E_7EC425B15F5C__INCLUDED_)
#define AFX_DRAWGRAPH_H__44CF80DF_0FC5_40C4_981E_7EC425B15F5C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct tagGRAPHELEMENTINFO : LLISTINFO
{
	DWORD			type;

	// Graph
	DWORD			pts;
	long			*ppts;
	DWORD			*ptimes;
	DWORD			validpts;
	COLORREF		rgbline;

	// Event
	HICON			icon;
	DWORD			time;

} GRAPHELEMENTINFO, *LPGRAPHELEMENTINFO;


class CDrawGraph : public CHList
{

public:
	BOOL Clean();
	void ReleaseMemory( LPGRAPHELEMENTINFO pgei );
	BOOL AllocateMemory( LPGRAPHELEMENTINFO pgei, DWORD size );
	BOOL Tick( DWORD dwTick = 0 );
	BOOL Add( LPCTSTR pName, DWORD dwTime, DWORD dwValue, COLORREF rgbLine, HICON hIcon );
	virtual void DeleteObject( void *node );
	BOOL Draw( HDC hDC, LPRECT pRect );
	CDrawGraph();
	virtual ~CDrawGraph();

	virtual DWORD GetObjSize() { return sizeof( GRAPHELEMENTINFO ); }

	CWinDc			m_bck;

private:

	DWORD			m_dwTime;
	DWORD			m_dwTimePerPixel;
	DWORD			m_dwGraphSize;
	DWORD			m_dwLastWindow;

};

#endif // !defined(AFX_DRAWGRAPH_H__44CF80DF_0FC5_40C4_981E_7EC425B15F5C__INCLUDED_)
