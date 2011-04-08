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
// ISession.cpp: implementation of the CISession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vp.h"
#include "ISession.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CISession::CISession()
{
	m_pIsi = NULL;
	m_phEvents = NULL;
	m_dwEvents = 0;
	m_pAws = NULL;

	m_fCreate = NULL;
	m_fDelete = NULL;

	m_hUpdate = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hAdd = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hAdded = CreateEvent( NULL, TRUE, FALSE, NULL );
}

CISession::~CISession()
{
	Destroy();

	if ( m_hUpdate != NULL )
	{	CloseHandle( m_hUpdate );
		m_hUpdate = NULL; 
	} // end if

	if ( m_hAdd != NULL )
	{	CloseHandle( m_hAdd );
		m_hAdd = NULL; 
	} // end if

	if ( m_hAdded != NULL )
	{	CloseHandle( m_hAdded );
		m_hAdded = NULL; 
	} // end if
}

void CISession::Destroy()
{
	CThread::StopThread();
}

BOOL CISession::InitThread(LPVOID pData)
{

	return TRUE;
}

BOOL CISession::DoThread(LPVOID pData)
{
	// Create event list
	if ( m_phEvents == NULL || m_pIsi == NULL || m_dwEvents == 0 )
		if ( !UpdateEventList() ) { Sleep( 1000 ); return FALSE; }

	// Ensure valid events
	if ( m_phEvents == NULL || m_pIsi == NULL || m_dwEvents == 0 )
	{	Sleep( 1000 ); return FALSE; }

	// Just to ensure these are correct
	m_phEvents[ 0 ] = CThread::GetStopEvent();
	m_phEvents[ 1 ] = m_hUpdate;
	m_phEvents[ 2 ] = m_hAdd;

	// Wait
	DWORD ret = WaitForMultipleObjects( m_dwEvents, m_phEvents, FALSE, INFINITE );

	if ( ret == WAIT_FAILED )
	{	DWORD err = GetLastError();
		char msg[ 256 ];
		CAsyncWinSock::GetErrorMsg( msg, err );
	} // end if

	// Verify range
	if ( ret < WAIT_OBJECT_0 ) return TRUE;

	// Get event index
	ret -= WAIT_OBJECT_0;

	// Check for stop
	if ( ret == 0 ) return FALSE;

	// Check for update event list
	if ( ret == 1 ) 
	{	Cleanup();
		UpdateEventList();
		ResetEvent( m_hUpdate );
		return TRUE;
	} // end if

	if ( ret == 2 ) 
	{	
		// Ensure not waiting for another thread
		if ( WaitForSingleObject( m_hAdded, 0 ) != WAIT_TIMEOUT )
			return TRUE;

		// Ack add
		ResetEvent( m_hAdd );

		// Save handle
		m_pAws = Add();

		// We've allocated the socket
		SetEvent( m_hAdded );

		return TRUE;
	} // end if

	// Out of range?
	if ( ret >= m_dwEvents ) return TRUE;

	// Reset the event
	ResetEvent( m_phEvents[ ret ] );

	// Get object pointer
	LPISESSIONINFO pisi = m_pIsi[ ret ];

	// Let the socket do it's processing
	if ( pisi != NULL && pisi->ws->IsConnected() )
	{	pisi->ws->OnNetworkEvent( pisi->ws->GetEventHandle(), pisi->ws->GetEvents() );
		if ( WaitForSingleObject( pisi->ws->GetShutdownHandle(), 0 ) != WAIT_TIMEOUT )
		{	pisi->ws->OnShutdownEvent(); Cleanup(); UpdateEventList(); }
	} // end if

	// Clean up if someone is disconnected
	else if ( pisi != NULL ) 
	{	Cleanup();
		UpdateEventList();
	} // end else if

	return TRUE;
}

BOOL CISession::EndThread(LPVOID pData)
{
	// Release event handles
	ReleaseEvents();

	// Lose objects
	CLList::Destroy();

	return TRUE;
}

void CISession::Cleanup()
{
	// Remove disconnected nodes
	LPISESSIONINFO	pisi = NULL;
	while ( ( pisi = (LPISESSIONINFO)GetNext( pisi ) ) != NULL )
		
		// Is this node connected?
		if ( pisi->ws != NULL && !pisi->ws->IsConnected() )
		{	
			// Point to next node
			LPISESSIONINFO del = pisi;
			pisi = (LPISESSIONINFO)GetNext( pisi );

			// Delete this node
			Delete( del );

		} // end if
}

CAsyncWinSock* CISession::Add()
{
	// Can we create objects?
	if ( m_fCreate == NULL ) return NULL;

	// Cleanup old connections
	Cleanup();

	// Allocate memory
	LPISESSIONINFO node = (LPISESSIONINFO)New();
	if ( node == NULL ) return NULL;

	// Create winsock 
	node->ws = m_fCreate();
	if ( node->ws == NULL ) { DeleteObject( node ); return NULL; }

	return node->ws;
}

CAsyncWinSock* CISession::NewSocket()
{
	// Request pointer
	SetEvent( m_hAdd );
	if ( WaitForSingleObject( m_hAdded, 8000 ) == WAIT_TIMEOUT )
		return NULL;

	// Save pointer
	CAsyncWinSock *pAws = m_pAws;

	// Reset event
	ResetEvent( m_hAdded );

	return pAws;
}

void CISession::DeleteObject(void *node)
{
	LPISESSIONINFO	pisi = (LPISESSIONINFO)node;
	if ( pisi == NULL ) return;

	// Delete winsock object
	if ( pisi->ws != NULL )
	{	if ( m_fDelete != NULL ) m_fDelete( pisi->ws );
		pisi->ws = NULL;
	} // end if

	// Complete the delete
	CLList::DeleteObject( node );
}


BOOL CISession::UpdateEventList()
{
	DWORD size = Size();

	// Create new events
	if ( size != m_dwEvents || m_phEvents == NULL || m_pIsi == NULL )
	{	
		// Lose old stuff
		ReleaseEvents();

		// Create event handle array
		m_phEvents = new HANDLE[ size + 3 ];
		if ( m_phEvents == NULL ) return FALSE;

		// Create pointers
		m_pIsi = new LPISESSIONINFO[ size + 3 ];
		if ( m_pIsi == NULL ) { ReleaseEvents(); return FALSE; }
		ZeroMemory( m_pIsi, ( size + 3 ) * sizeof( LPISESSIONINFO ) );

		m_dwEvents = size + 3;

	} // end if

	DWORD i = 0;

	// Set stop event
	m_phEvents[ i++ ] = CThread::GetStopEvent();
	m_phEvents[ i++ ] = m_hUpdate;
	m_phEvents[ i++ ] = m_hAdd;

	// Save event handles into array
	LPISESSIONINFO	pisi = NULL;
	while ( ( pisi = (LPISESSIONINFO)GetNext( pisi ) ) != NULL )
		if ( i < m_dwEvents )
			m_pIsi[ i ] = pisi, m_phEvents[ i++ ] = pisi->ws->GetEventHandle();			

	return TRUE;
}

void CISession::ReleaseEvents()
{
	m_dwEvents = 0;

	if ( m_phEvents != NULL ) 
	{	delete [] m_phEvents;
		m_phEvents = NULL;
	} // end if

	if ( m_pIsi != NULL )
	{	delete [] m_pIsi;
		m_pIsi = NULL;
	} // end if
}

