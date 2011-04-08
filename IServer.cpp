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
// IServer.cpp: implementation of the  CIServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vp.h"
#include "IServer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIServer::CIServer()
{
	m_pSl = NULL;

	m_hWndSession = NULL;
	m_uMsgSession = 0;
}

CIServer::~CIServer()
{

}

BOOL CIServer::Accept(CAsyncWinSock *pSocket)
{
	if ( pSocket == NULL ) return FALSE;

	// Attempt to accept connection
	if ( !CAsyncWinSock::Accept( pSocket ) ) return FALSE;

	// Enable callbacks
	pSocket->EnableCallbacks();
	pSocket->StartCallbacks( pSocket->GetEnabledCallbacks(), FALSE );								

	// Update the processing list
	if ( m_pSl != NULL ) m_pSl->Update();

	// Enable the thread callbacks
//	pSocket->EnableCallbacks();
//	pSocket->StartCallbacks();

	// Start the thread
//	pSocket->StartThread();

	return TRUE;
}


BOOL CIServer::OnAccept(int nErr)
{
	// Ensure we have a session list
	if ( m_pSl == NULL ) return FALSE;

	// Add a socket to handle the connection
	CAsyncWinSock *ws = m_pSl->NewSocket();
	if ( ws == NULL ) return FALSE;

	// Set the message target
	ws->SetMessageTarget( m_hWndSession, m_uMsgSession );

	// Connect the client
	return Accept( ws );
}


BOOL CIServer::Start(HWND hWnd, UINT uMsg, DWORD dwPort, DWORD dwMax)
{
	Destroy();
	
	// Open the socket
	if (	!OpenSocket() ||

			// Bind to TCP port
			!Bind( dwPort ) ||

			// Listen for connections
			!Listen( dwMax ) ||

			// Async operation
			!SetMessageTarget( hWnd, uMsg ) ||

			// Start callback thread
			!StartCallbacks() )
	{
		Destroy();
		return FALSE;
	} // end if

	return TRUE;
}



