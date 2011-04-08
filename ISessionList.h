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
// ISessionList.h: interface for the CISessionList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISESSIONLIST_H__6C062DCF_CDA3_4481_B601_44BF38FEE815__INCLUDED_)
#define AFX_ISESSIONLIST_H__6C062DCF_CDA3_4481_B601_44BF38FEE815__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


typedef struct tagISESSIONINFO : LLISTINFO
{
	CAsyncWinSock		*ws;


} ISESSIONINFO, *LPISESSIONINFO; // end typedef struct

typedef CAsyncWinSock* (*CISessionList_CreateSession)();
typedef void (*CISessionList_DeleteSession)( CAsyncWinSock *ptr );

class CISessionList : public CLList  
{
public:
	void Cleanup();
	CAsyncWinSock* Add();

	void DeleteObject( void *node );
	DWORD GetObjSize() { return sizeof( ISESSIONINFO ); }

	CISessionList();
	virtual ~CISessionList();

	// Set create/delete functions
	void SetCreateDelete(	CISessionList_CreateSession fCreate,
							CISessionList_DeleteSession fDelete )
	{	m_fCreate = fCreate; m_fDelete = fDelete; }

private:

	CISessionList_CreateSession		m_fCreate;
	CISessionList_DeleteSession		m_fDelete;

};

#endif // !defined(AFX_ISESSIONLIST_H__6C062DCF_CDA3_4481_B601_44BF38FEE815__INCLUDED_)
