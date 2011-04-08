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
// ItemList.h: interface for the CItemList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMLIST_H__49A61F7C_AC0F_11D4_821A_0050DA205C15__INCLUDED_)
#define AFX_ITEMLIST_H__49A61F7C_AC0F_11D4_821A_0050DA205C15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Header.h"

#define SIT_NONE		0
#define SIT_TEXT		1
#define SIT_COLOR		2
#define SIT_BITMAP		3
#define SIT_ICON		4

//#define SIF_

typedef struct tagLISTSUBITEMINFO
{
	DWORD				index;

	DWORD				type;
	LPVOID				data;

	DWORD				flags;

	HICON				icon;

	COLORREF			rgbtext;
	COLORREF			rgbbck;

	DWORD				user;

	LPLOGFONT			pfont;
	
	tagLISTSUBITEMINFO	*pPrev;
	tagLISTSUBITEMINFO	*pNext;

} LISTSUBITEMINFO; // end typedef struct

typedef LISTSUBITEMINFO* LPLISTSUBITEMINFO;

#define LIF_SELECTED	0x00000001
#define LIF_GROUP		0x00000002
#define LIF_INGROUP		0x00000004
#define LIF_COLLAPSED	0x00000008

typedef struct tagLISTITEMINFO
{
	DWORD				index;

	DWORD				height;

	DWORD				flags;

	DWORD				user;

	DWORD				subitems;
	LPLISTSUBITEMINFO	head;
	LPLISTSUBITEMINFO	tail;

} LISTITEMINFO; // end typedef struct
typedef LISTITEMINFO* LPLISTITEMINFO;

class CItemList  
{
public:
	HCURSOR GetCursor( LPRECT pRect, LPPOINT pt );
	BOOL OnLButtonUp();
	DWORD GetLinesPerPage( LPRECT pRect );
	DWORD GetScrollLineMax( LPRECT pRect );
	BOOL OnMouseMove( UINT flags, LPPOINT point, LPRECT prect );
	BOOL OnLButtonDown( UINT flags, LPPOINT point, LPRECT prect );
	DWORD GetItem( long x, long y, LPRECT pRect );
	BOOL SelectItem( DWORD index, BOOL bSelect, BOOL bToggle = FALSE, BOOL bMulti = FALSE, BOOL bRange = FALSE );
	BOOL DrawItem( DWORD i, HDC hDC, LPRECT pRect );
	BOOL Draw( HDC hDC, LPRECT pRect, HWND hWnd = NULL );
	BOOL SetSubItemIcon( DWORD index, DWORD subitem, HICON icon );
	BOOL SetSubItemFlags( DWORD index, DWORD subitem, DWORD flags );
	LPLISTSUBITEMINFO GetSubItem( DWORD index, DWORD subitem );
	BOOL ModifySubItem(DWORD index, DWORD subitem, LPVOID data, HICON icon = NULL, DWORD type = SIT_TEXT, DWORD flags = 0 );
	void DeleteSubItems( DWORD index );
	void DeleteSubItemData( LPLISTSUBITEMINFO psii );
	BOOL AddSubItem( DWORD index, LPVOID data, HICON icon = NULL, DWORD type = SIT_TEXT, DWORD flags = 0, COLORREF rgbtext = MAXDWORD, COLORREF rgbbck = MAXDWORD, LPLOGFONT plf = NULL );
	DWORD AddItem( DWORD flags = 0 );
	BOOL AllocateIndex( DWORD size );
	void Destroy();
	CItemList();
	virtual ~CItemList();

	DWORD Size() { return m_dwPtr; }
	DWORD GetNumItems() { return Size(); }

	CHeader& GetHeader() { return m_header; }

	DWORD GetItem( LPPOINT pt, LPRECT pRect ) 
	{	if ( pt == NULL ) return MAXDWORD; return GetItem( pt->x, pt->y, pRect ); }

	void SetCurLine( DWORD l );
	DWORD GetCurLine();

	void SetBarCol( COLORREF top, COLORREF bottom )
	{	m_rgbBarTop = top; m_rgbBarBottom = bottom; }

	void SetBckCol( COLORREF top, COLORREF bottom )
	{	m_rgbBckTop = top; m_rgbBckBottom = bottom; }

	void SetSelCol( COLORREF top, COLORREF bottom )
	{	m_rgbSelTop = top; m_rgbSelBottom = bottom; }

	void SetTextCol( COLORREF c ) { m_rgbText = c; }
	void SetSelTextCol( COLORREF c ) { m_rgbSelText = c; }

	void SetHScroll( long hs ) { m_lHScroll = hs; }
	long GetHScroll() { return m_lHScroll; }
	
	void HorzLines( BOOL b ) { m_bHorzLines = b; }
	void VertLines( BOOL b ) { m_bVertLines = b; }

	void MultiSelect( BOOL b ) { m_bMultiSelect = b; }

	void NoDraw( DWORD dw ) { m_dwNoDraw = dw; }

	void SetDefaultItemHeight( DWORD dw ) { m_dwDefaultHeight = dw; }

	void EndGroup() { m_bInGroup = FALSE; }

	LPLISTITEMINFO GetItem( DWORD i ) 
	{	if ( i >= m_dwPtr ) return NULL; return m_pIndex[ i ]; }

private:

	long				m_lHScroll;				
	DWORD				m_dwLine;

	DWORD				m_dwSize;
	DWORD				m_dwPtr;
	LPLISTITEMINFO		*m_pIndex;

	CHeader				m_header;

	long				m_phiiwidth;
	LPHEADERITEMINFO	m_phii;
	POINT				m_lbuttondown;

	BOOL				m_bHorzLines;
	BOOL				m_bVertLines;

	COLORREF			m_rgbHorzLines;
	COLORREF			m_rgbVertLines;

	BOOL				m_bMultiSelect;

	DWORD				m_dwNoDraw;

	DWORD				m_dwDefaultHeight;

	BOOL				m_bInGroup;
	BOOL				m_bGroups;

	BOOL				m_bAutoDefaultColors;

public:

	BOOL SetSubItemData( DWORD item, DWORD subitem, DWORD data );
	BOOL GetSubItemData( DWORD item, DWORD subitem, LPDWORD pdwData );
	
	void DefaultColors();
	void AutoDefaultColors( BOOL b ) { m_bAutoDefaultColors = b; }
	DWORD GetUncollapsedLines();
	BOOL SetSubItemText( DWORD index, DWORD subitem, LPCTSTR pText );
	DWORD GetItemData( DWORD item );
	BOOL SetItemData( DWORD item, DWORD data );
	BOOL SetSubItemBckColor( DWORD index, DWORD subitem, COLORREF rgb );
	BOOL SetSubItemTextColor( DWORD index, DWORD subitem, COLORREF rgb );
	long GetTotalWidth();
	BOOL GetSubItemText( DWORD item, DWORD subitem, LPSTR buf, DWORD size );
	DWORD FindString( DWORD dwCol, LPCTSTR pStr );
	DWORD FindString( LPCTSTR pCol, LPCTSTR pStr );
	BOOL GetItemRect( DWORD item, LPRECT pRect );
	DWORD GetFirstSelectedItem();

	COLORREF			m_rgbBckTop;
	COLORREF			m_rgbBckBottom;

	COLORREF			m_rgbSelTop;
	COLORREF			m_rgbSelBottom;

	COLORREF			m_rgbText;
	COLORREF			m_rgbSelText;

	COLORREF			m_rgbBarTop;
	COLORREF			m_rgbBarBottom;

};

#endif // !defined(AFX_ITEMLIST_H__49A61F7C_AC0F_11D4_821A_0050DA205C15__INCLUDED_)
