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
// Header.h: interface for the CHeader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEADER_H__49A61F7D_AC0F_11D4_821A_0050DA205C15__INCLUDED_)
#define AFX_HEADER_H__49A61F7D_AC0F_11D4_821A_0050DA205C15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define HIF_SORT		0x00000001
#define HIF_3D			0x00000002
#define HIF_SHADOWTEXT	0x00000004

typedef struct tagHEADERITEMINFO
{
	char				title[ 256 ];
	HICON				icon;
	DWORD				flags;
	COLORREF			rgbtext;
	COLORREF			rgbbck;
	long				width;

	tagHEADERITEMINFO	*pPrev;
	tagHEADERITEMINFO	*pNext;

} HEADERITEMINFO; // end typedef struct
typedef HEADERITEMINFO*	LPHEADERITEMINFO;

class CHeader  
{
public:
	BOOL SetHeaderItemWidths( long w );
	DWORD FindCol( LPCTSTR str );
	LPHEADERITEMINFO GetHeaderItem( DWORD i );
	long GetHeaderItemWidth( DWORD i );
	BOOL SetHeaderItemWidths( long min, HDC hDC );
	LPHEADERITEMINFO GetHeaderResizeColumn( LPRECT pRect, LPPOINT pt, long lHScroll );
	BOOL Draw( HDC hDC, LPRECT pRect, long lHScroll = 0, long lSkip = 0 );
	BOOL AddHeaderItem( long width, LPCTSTR pText, DWORD flags = HIF_3D, HICON icon = NULL, COLORREF rgbtext = GetSysColor( COLOR_CAPTIONTEXT ), COLORREF rgbbck = GetSysColor( COLOR_ACTIVECAPTION ) );
	void Destroy();
	CHeader();
	virtual ~CHeader();

	DWORD Size() { return m_dwSize; }
	DWORD GetNumHeaderItems() { return Size(); }

	void SetHeight( long h ) { m_lHeight = h; }
	long GetHeight() { return m_lHeight; }

	LPHEADERITEMINFO GetNext( LPHEADERITEMINFO ptr )
	{	if ( ptr != NULL ) return ptr->pNext; return m_pHead; }

	void Autosize( BOOL b ) { m_bAutosize = b; }
	BOOL IsAutosizing() { return m_bAutosize; }

private:

	DWORD				m_dwSize;
	LPHEADERITEMINFO	m_pHead;
	LPHEADERITEMINFO	m_pTail;

	long				m_lHeight;

	BOOL				m_bAutosize;
};

#endif // !defined(AFX_HEADER_H__49A61F7D_AC0F_11D4_821A_0050DA205C15__INCLUDED_)
