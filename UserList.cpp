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
// UserList.cpp: implementation of the CUserList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vp.h"
#include "UserList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUserList::CUserList()
{

}

CUserList::~CUserList()
{

}

BOOL CUserList::Add(DWORD ip, DWORD access)
{
	// Does user already exist?
	LPUSERINFO node = (LPUSERINFO)Find( ip );
	if ( node != NULL ) 
	{	node->access = access;
		return TRUE;
	} // end if

	// Allocate new memory
	node = (LPUSERINFO)New( NULL, ip );
	if ( node == NULL ) return FALSE;

	// Set access level
	node->access = access;
	node->time = GetTickCount();

	return TRUE;
}

DWORD CUserList::GetUserAccess(DWORD ip, BOOL refresh)
{
	// Does user exist?
	LPUSERINFO node = (LPUSERINFO)Find( ip );
	if ( node == 0 ) return 0;

	// Refresh timeout
	if ( refresh ) node->time = GetTickCount();

	// Return access level
	return node->access;	
}

BOOL CUserList::RefreshUser(DWORD ip)
{
	// Does user exist?
	LPUSERINFO node = (LPUSERINFO)Find( ip );
	if ( node == NULL ) return FALSE;

	// Refresh timeout
	node->time = GetTickCount();

	return TRUE;
}

BOOL CUserList::Expire(DWORD timeout)
{
	BOOL		bRemoved = FALSE;
	DWORD		time = GetTickCount();
	LPUSERINFO	pui = NULL;

	// Expire old users
	while ( ( pui = (LPUSERINFO)GetNext( pui ) ) != NULL )
		if ( ( pui->time + timeout ) < time ) 
		{	Delete( pui ); pui = NULL; bRemoved = TRUE; }

	return bRemoved;
}

