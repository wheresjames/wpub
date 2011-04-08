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
// UserList.h: interface for the CUserList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERLIST_H__DBE75717_4C3B_4865_B1B5_BD4CC5421913__INCLUDED_)
#define AFX_USERLIST_H__DBE75717_4C3B_4865_B1B5_BD4CC5421913__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


typedef struct tagUSERINFO : LLISTINFO
{
	DWORD			access;
	DWORD			time;

} USERINFO, *LPUSERINFO; // end typedef sturct


class CUserList : public CLList  
{
public:

	virtual DWORD GetObjSize() { return sizeof( USERINFO ); }

	BOOL RefreshUser( DWORD ip );
	BOOL Expire( DWORD timeout );
	DWORD GetUserAccess( DWORD ip, BOOL refresh = FALSE );
	BOOL Add( DWORD ip, DWORD access = 1 );
	CUserList();
	virtual ~CUserList();

};

#endif // !defined(AFX_USERLIST_H__DBE75717_4C3B_4865_B1B5_BD4CC5421913__INCLUDED_)
