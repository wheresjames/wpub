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
// Ftp.cpp: implementation of the CFtp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Ftp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFtp::CFtp()
{
	m_bConnected = FALSE;

	*m_szAddr = 0;
	*m_szLocal = 0;
	*m_szRemote = 0;
	m_dwPort = 21;

	// Create command event
	m_hComplete = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hCmd = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hConnect = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_dwCmd = FTPCMD_NONE;
	m_bConnecting = FALSE;

	m_pData = NULL;
	m_dwData = 0;

	m_bAutoClose = FALSE;
}

CFtp::~CFtp()
{
	Destroy();

	CloseHandle( m_hCmd );
	CloseHandle( m_hComplete );
	CloseHandle( m_hConnect );
}

void CFtp::Destroy()
{
	m_bConnecting = FALSE;
	m_bConnected = FALSE;
	m_ftp.Destroy();
	StopThread();

	ResetEvent( m_hCmd );
	ResetEvent( m_hComplete );
	ResetEvent( m_hConnect );
}

BOOL CFtp::InitThread( LPVOID pData )
{

	return TRUE;
}

BOOL CFtp::DoThread( LPVOID pData )
{		 

	HANDLE	phCmd[ 3 ];
	HANDLE	phComplete[ 2 ];

	phCmd[ 0 ] = m_hStop;
	phCmd[ 1 ] = m_hConnect;
	phCmd[ 2 ] = m_hCmd;

	phComplete[ 0 ] = m_hStop;
	phComplete[ 1 ] = m_hComplete;

	DWORD ret = WaitForMultipleObjects( 3, phCmd, FALSE, INFINITE );
	
	// Verify range
	if ( ret < WAIT_OBJECT_0 ) return TRUE;

	// Get event index
	ret -= WAIT_OBJECT_0;

	// Check for stop
	if ( ret == 0 ) return FALSE;

	else if ( ret == 1 )
	{
		// Lose previous connection
		m_ftp.Destroy();

		// Set callback function
		m_ftp.SetFtpCallback( &CFtp::FtpCallback, (DWORD)this );

		// Connect to site
		ResetEvent( m_hComplete );
		if ( !m_ftp.Connect( m_szAddr, m_dwPort, &CFtp::SocketCallback, (DWORD)this ) )
		{	m_ftp.Destroy();
			m_bConnected = FALSE;
			m_bConnecting = FALSE;
		} // end if

		// Wait for command to complete
		else if ( WaitForMultipleObjects( 2, phComplete, FALSE, 30000 ) == WAIT_TIMEOUT ) 
		{	m_ftp.Destroy();
			_Log( MB_ICONWARNING, "FTP", "Timeout waiting for connection" );
			m_bConnected = FALSE;
			m_bConnecting = FALSE;
		} // end if
		else
		{
			// We are now connected
			m_bConnected = TRUE;
			m_bConnecting = FALSE;
		} // end else

		// Reset Connect event
		ResetEvent( m_hConnect );
	
	} // end if

	// Handle the command
	else if ( ret == 2 )
	{
		switch( m_dwCmd )
		{
			case FTPCMD_UPLOAD :

				ResetEvent( m_hComplete );

				if ( m_pData == NULL )
					m_ftp.Put( m_szLocal, m_szRemote );
				else m_ftp.Put( m_pData, m_dwData, m_szRemote );

				// Wait for command to complete
				WaitForMultipleObjects( 2, phComplete, FALSE, INFINITE );

				break;

			default : break;
		} // end switch

	} // end else


	return TRUE;
}

BOOL CFtp::EndThread( LPVOID pData )
{
	m_bConnected = FALSE;

	// Lose the ftp connection
	m_ftp.Destroy();

	return TRUE;
}

BOOL CFtp::FtpCallback(	CFtpClient* pFtpClient, DWORD dwUser, 
						WPARAM wParam, LPARAM lParam )
{
	CFtp *pFtp = (CFtp*)dwUser;
	if ( pFtp == NULL ) return FALSE;

	if ( pFtp->m_bConnecting )
	{
		// Is the connection complete?
		if (	wParam == FTPMSG_STATUS && 
				lParam == FTP_LOGGEDON )
			SetEvent( pFtp->m_hComplete );
	} // end if

	else switch( pFtp->m_dwCmd )
	{
		case FTPCMD_UPLOAD :
		case FTPCMD_DOWNLOAD :

			if ( wParam == FTPMSG_CMDCOMPLETE )
			{	
				// Signal command complete
				pFtp->m_dwCmd = FTPCMD_NONE;
				ResetEvent( pFtp->m_hCmd );
				SetEvent( pFtp->m_hComplete );

				// Check for auto close
				if ( pFtp->m_bAutoClose ) 
				{	pFtp->m_bConnecting = FALSE;
					pFtp->m_bConnected = FALSE;
					pFtp->m_ftp.ThreadCloseSocket();
					pFtp->SignalStop();
				} // end if

			} // end if

			break;

	} // end switch
	
	return TRUE;
}

BOOL CFtp::SocketCallback(	CAsyncWinSock* pFtpClient, DWORD dwUser, 
							WPARAM wParam, LPARAM lParam )
{
	CFtp *pFtp = (CFtp*)dwUser;
	if ( pFtp == NULL ) return FALSE;	

	long	lNetworkEvents = (long)wParam;
	int		*iErrorCodes = (int*)lParam;

	if ( ( lNetworkEvents & FD_CONNECT ) != 0 )
	{
		if ( iErrorCodes[ FD_CONNECT_BIT ] != 0 )
		{	char msg[ CWF_STRSIZE ];
			if ( !CAsyncWinSock::GetErrorMsg( msg, iErrorCodes[ FD_CONNECT_BIT ], FALSE ) )
				strcpy( msg, "Unknown Error" );
			_Log( MB_ICONERROR, "FTP", msg );
			pFtp->m_ftp.ThreadCloseSocket();
			pFtp->m_bConnected = FALSE;
			pFtp->m_bConnecting = FALSE;
		} // end if
	} // end if

	else if ( ( lNetworkEvents & FD_CLOSE ) != 0 )
	{
		if ( iErrorCodes[ FD_CONNECT_BIT ] != 0 )
		{	char msg[ CWF_STRSIZE ];
			if ( !CAsyncWinSock::GetErrorMsg( msg, iErrorCodes[ FD_CONNECT_BIT ], FALSE ) )
				strcpy( msg, "Unknown Error" );
			_Log( MB_ICONERROR, "FTP", msg );
		} // end if

		pFtp->m_ftp.ThreadCloseSocket();
		pFtp->m_bConnected = FALSE;
		pFtp->m_bConnecting = FALSE;
	} // end if

	return TRUE;
}

BOOL CFtp::Connect(LPCTSTR pAddr, DWORD dwPort)
{
	// Kill old connection
	Close();
	
	// Sanity checks
	if ( pAddr == NULL ) return FALSE;

	// Save connection information
	strcpy_sz( m_szAddr, pAddr );
	if ( dwPort != 0 ) m_dwPort = dwPort;

	// Signal that we are connecting
	StartConnect();

	// Start the thread
	StartThread();
	
	return TRUE;
}

BOOL CFtp::Close()
{
	StopThread();
	return TRUE;
}

BOOL CFtp::StartConnect()
{
	// Which command to execute
	m_bConnecting = TRUE;

	// Tell the thread to go
	SetEvent( m_hConnect );

	return TRUE;
}

BOOL CFtp::StartCommand(DWORD cmd)
{
	// Which command to execute
	m_dwCmd = cmd;

	// Tell the thread to go
	SetEvent( m_hCmd );

	return TRUE;
}

BOOL CFtp::Upload(LPCTSTR pLocal, LPCTSTR pRemote)
{
	// Sanity checks
	if ( pLocal == NULL || pRemote == NULL ) return FALSE;
	if ( IsCommand() ) return FALSE;

	// Save file names
	m_pData = NULL; m_dwData = 0;
	strcpy_sz( m_szLocal, pLocal );
	strcpy_sz( m_szRemote, pRemote );

	// Send the upload command
	StartCommand( FTPCMD_UPLOAD );

	return TRUE;
}


BOOL CFtp::Upload(LPBYTE buf, DWORD size, LPCTSTR pRemote)
{
	// Sanity checks
	if ( buf == NULL || pRemote == NULL ) return FALSE;
	if ( IsCommand() ) return FALSE;

	// Save file names
	m_pData = buf; m_dwData = size;
	*m_szLocal = NULL;
	strcpy_sz( m_szRemote, pRemote );

	// Send the upload command
	StartCommand( FTPCMD_UPLOAD );

	return TRUE;
}
