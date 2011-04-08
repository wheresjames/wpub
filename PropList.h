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
// PropList.h: interface for the CPropList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROPLIST_H__51122450_E2FB_4CD8_B1BE_2F202ED556A3__INCLUDED_)
#define AFX_PROPLIST_H__51122450_E2FB_4CD8_B1BE_2F202ED556A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define PROPTYPE_BIN			0
#define PROPTYPE_BOOL			1
#define PROPTYPE_DWORD			2
#define PROPTYPE_LONG			3
#define PROPTYPE_DOUBLE			4
#define PROPTYPE_RGB			5
#define PROPTYPE_STR			6
#define PROPTYPE_FILE			7
#define PROPTYPE_DIR			8
#define PROPTYPE_FLAG			9
#define PROPTYPE_FONT			10
#define PROPTYPE_RGBSTR			11
#define PROPTYPE_CUSTOM			12

#define PROPTYPE_GROUP			0xffff

#define PROPEDIT_NONE			0
#define PROPEDIT_BUTTON			1
#define PROPEDIT_SLIDER			2
#define PROPEDIT_CHECK			3
#define PROPEDIT_COMBOEDIT		4
#define PROPEDIT_COMBO			5
#define PROPEDIT_EDIT			6
#define PROPEDIT_SCROLLEDIT		7
#define PROPEDIT_RGB			8
#define PROPEDIT_FILE			9
#define PROPEDIT_DIR			10
#define PROPEDIT_FONT			11

typedef struct tagPROPINFO : LLISTINFO
{
	DWORD		type;
	DWORD		user;
	char		name[ 256 ];

	DWORD		edit;

	long		min;
	long		max;

	DWORD		mask;
	char		on[ 256 ];
	char		off[ 256 ];

	DWORD		size;
	LPVOID		data;

	BOOL		fullredraw;

} PROPINFO, *LPPROPINFO; // end typedef struct


class CPropList : public CLList  
{
public:

	static void SetStr( LPPROPINFO ppi, LPCTSTR str );
	static void SetNum( LPPROPINFO ppi, double num );	
	static BOOL GetStr( LPPROPINFO ppi, LPSTR str, DWORD size );
	static double GetNum( LPPROPINFO ppi );

	LPPROPINFO Add( DWORD type, DWORD edit, 
					DWORD user, LPCTSTR name, 
					LPVOID data, DWORD size,
					long min = 0, long max = 0,
					DWORD mask = 0, LPCTSTR on = NULL, LPCTSTR off = NULL,
					BOOL fullredraw = FALSE );

	virtual void DeleteObject( void *node );
	virtual DWORD GetObjSize() { return sizeof( PROPINFO ); }

	CPropList();
	virtual ~CPropList();

};

#endif // !defined(AFX_PROPLIST_H__51122450_E2FB_4CD8_B1BE_2F202ED556A3__INCLUDED_)
