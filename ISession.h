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
// ISession.h: interface for the CISession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISESSION_H__2EEE032F_586D_43C3_8B15_A96F45E0800B__INCLUDED_)
#define AFX_ISESSION_H__2EEE032F_586D_43C3_8B15_A96F45E0800B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct tagISESSIONINFO : LLISTINFO
{
	CAsyncWinSock		*ws;

} ISESSIONINFO, *LPISESSIONINFO; // end typedef struct

typedef CAsyncWinSock* (*CISessionList_CreateSession)();
typedef void (*CISessionList_DeleteSession)( CAsyncWinSock *ptr );

class CISession : 
	public CThread, 
	public CLList  
{
public:
	void ReleaseEvents();
	BOOL UpdateEventList();
	void Destroy();

	virtual void DeleteObject( void *node );
	virtual DWORD GetObjSize() { return sizeof( ISESSIONINFO ); }

	CAsyncWinSock* NewSocket();
	void Cleanup();
	
	BOOL EndThread( LPVOID pData );
	virtual BOOL DoThread( LPVOID pData );
	virtual BOOL InitThread( LPVOID pData );

	CISession();
	virtual ~CISession();

	// Set create/delete functions
	void SetCreateDelete(	CISessionList_CreateSession fCreate,
							CISessionList_DeleteSession fDelete )
	{	m_fCreate = fCreate; m_fDelete = fDelete; }

	BOOL Update() { return SetEvent( m_hUpdate ); }

private:
	CAsyncWinSock* Add();

	DWORD							m_dwEvents;
	LPHANDLE						m_phEvents;
	LPISESSIONINFO					*m_pIsi;

	HANDLE							m_hUpdate;
	HANDLE							m_hAdd;
	HANDLE							m_hAdded;
	CAsyncWinSock					*m_pAws;
	
	CISessionList_CreateSession		m_fCreate;
	CISessionList_DeleteSession		m_fDelete;

};

#endif // !defined(AFX_ISESSION_H__2EEE032F_586D_43C3_8B15_A96F45E0800B__INCLUDED_)
