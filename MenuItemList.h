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
// MenuItemList.h: interface for the CMenuItemList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENUITEMLIST_H__CE7F4DC6_DDF9_46C8_B85D_BAF7CE2D08EA__INCLUDED_)
#define AFX_MENUITEMLIST_H__CE7F4DC6_DDF9_46C8_B85D_BAF7CE2D08EA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CWinMenu;
class CWndToolbar;
typedef struct tagMITEMINFO : LLISTINFO
{
	char			name[ 256 ];
	char			tip[ 256 ];
	DWORD			cmd;
	HICON			icon;
	HBITMAP			bmp;
	HCURSOR			cursor;
	LPBOOL			b;

	char			rtext[ 256 ];
	HICON			ricon[ 16 ];
	
	CWinMenu		*submenu;
	CWndToolbar		*toolbar;
	
	tagMITEMINFO	*pNext;
	tagMITEMINFO	*pPrev;

} MITEMINFO; // end typedef struct

typedef MITEMINFO*	LPMITEMINFO;

class CMenuItemList : public CLList  
{
public:
	
	CMenuItemList();
	virtual ~CMenuItemList();

	virtual DWORD GetObjSize() { return sizeof( MITEMINFO ); }
	virtual void DeleteObject( void *node );

};

#endif // !defined(AFX_MENUITEMLIST_H__CE7F4DC6_DDF9_46C8_B85D_BAF7CE2D08EA__INCLUDED_)
