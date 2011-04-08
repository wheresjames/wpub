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
// FtpClient.cpp: implementation of the CFtpClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vp.h"
#include "FtpClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFtpClient::CFtpClient()
{
	m_hFtpWnd = NULL;
	m_uFtpMsg = NULL;

	m_dwFtpStatus = FTP_INIT;

	m_dwGetState = FTPGET_NONE;
	m_dwPutState = FTPPUT_NONE;

	m_dwDataPort = 1150;

	m_bPassiveFtp = TRUE;

	m_pCallback = NULL;
	m_dwCallback = 0;

	m_pData = NULL;
	m_dwData = NULL;
	m_dwPtr = NULL;

	m_bChgDir = TRUE;

	*m_szUser = 0;
	*m_szPassword = 0;

	*m_szLocal = 0;

	*m_szRemote = 0;
	*m_szTempRemote = 0;
}

CFtpClient::~CFtpClient()
{
	Destroy();
}

void CFtpClient::Destroy()
{
	// Lose the socket connection
	CloseSocket();
//	CAsyncWinSock::Destroy();

	m_dwGetState = FTPGET_NONE;
	m_dwPutState = FTPPUT_NONE;
	m_dwFtpStatus = FTP_INIT;
	m_bChgDir = TRUE;
}


BOOL CFtpClient::OnConnect( int nErr )
{
	return TRUE;
}

BOOL CFtpClient::OnClose( int nErr )
{
	return TRUE;
}

BOOL CFtpClient::OnRead( int nErr )
{
	DWORD bytes = 0;

	// How many bytes are waiting?
	if ( !GetRecvBytesWaiting( &bytes ) )
		return FALSE;

	// Allocate buffer
	TMem< char > buf;
	if ( !buf.allocate( bytes + 1 ) ) return FALSE;

	// Read in data
	Recv( buf, bytes );
	buf[ bytes ] = 0;

	// Process the reply
	ProcessReply( buf, bytes );

	return TRUE;
}

BOOL CFtpClient::OnWrite( int nErr )
{
	return TRUE;
}

BOOL CFtpClient::Connect(LPCTSTR pAddr, DWORD dwPort, HWND hWnd, UINT uMsg)
{
	Destroy();

	// Reset status
	m_dwFtpStatus = FTP_INIT;

	// Set the message target
	EnableCallbacks();
	SetMessageTarget( hWnd, uMsg );

	// Attempt to connect
	if ( !CAsyncWinSock::Connect( pAddr, dwPort ) )
	{	_Log( MB_ICONERROR, pAddr, "Cannot connect to FTP server" );
		return FALSE;
	} // end if

	return TRUE;
}

BOOL CFtpClient::ProcessReply(LPCTSTR buf, DWORD size)
{
	// Get error code
	DWORD code = strtoul( buf, NULL, 10 );

	if ( code >= 400 )
	{
		// Ignore these commands
		if (	m_dwPutState != FTPPUT_MKDIR &&
				m_dwPutState != FTPPUT_DELFILE1 &&
				m_dwPutState != FTPPUT_DELFILE2 )
		{
			// Is it a critical error?
			if (	m_dwPutState == FTPPUT_CHGDIR ||
					m_dwPutState == FTPPUT_SETPORT ||
					m_dwPutState == FTPPUT_PUTFILE ||
					m_dwPutState == FTPPUT_RENAMEFROM ||
					m_dwPutState == FTPPUT_RENAMETO )
			{
				_Log( MB_ICONERROR, "FTP", buf );
				SendFtpMessage( FTPMSG_CMDCOMPLETE, 0L );
				Destroy();			
			} // end if

			// Just a warning
			else _Log( MB_ICONWARNING, "FTP", buf );
		} // end if

	} // end if

	// Wait for rest of reply
	if ( !IsEndOfReply( buf, size ) ) return FALSE;

	// Are we receiving reply to quit command
	if ( m_dwFtpStatus == FTP_LOGGEDOFF )
	{	SendFtpMessage( FTPMSG_STATUS, FTP_LOGGEDOFF );
		Destroy();
	} // end else if	

	// Do we need to log on?
	else if ( m_dwFtpStatus != FTP_LOGGEDON )
		return LogOn( code );

	// Are we processing a GET command?
	else if ( m_dwGetState != FTPGET_NONE )
		CompleteGet( buf, size );

	// Are we processing a PUT command?
	else if ( m_dwPutState != FTPPUT_NONE )
		CompletePut( buf, size );

	return TRUE;
}

BOOL CFtpClient::LogOn(DWORD code)
{
	// Time to send username?
	if ( m_dwFtpStatus == FTP_INIT )
	{	if ( code >= 300 )
		{	m_iErr[ FD_CLOSE_BIT ] = WSAECONNREFUSED;
			SendMessage( FD_CLOSE, (LPARAM)m_iErr );
			Destroy();
			return FALSE;
		} // end if
		
		char msg[ FTP_STRSIZE ];
		wsprintf( msg, "USER %s\r\n", m_szUser );
		Send( msg );
		m_dwFtpStatus = FTP_USER;
	} // end if

	// Time to send password?
	else if ( m_dwFtpStatus == FTP_USER )
	{	if ( code >= 400 )
		{	m_iErr[ FD_CLOSE_BIT ] = ERROR_INVALID_PARAMETER;
			SendMessage( FD_CLOSE, (LPARAM)m_iErr );
			Destroy();
			return FALSE;
		} // end if

		char msg[ FTP_STRSIZE ];
		wsprintf( msg, "PASS %s\r\n", m_szPassword );
		Send( msg );
		m_dwFtpStatus = FTP_PASSWORD;
	} // end if

	else if ( m_dwFtpStatus == FTP_PASSWORD )
	{	if ( code >= 300 )
		{	m_iErr[ FD_CLOSE_BIT ] = ERROR_INVALID_PASSWORD;
			SendMessage( FD_CLOSE, (LPARAM)m_iErr );
			Destroy();
			return FALSE;
		} // end if

		// Send logged on message
		m_dwFtpStatus = FTP_LOGGEDON;
		SendFtpMessage( FTPMSG_STATUS, FTP_LOGGEDON );

	} // end if

	return TRUE;
}

BOOL CFtpClient::SetDirectory(LPCTSTR pDir)
{
	// Verify we are logged in
	if ( pDir == NULL || m_dwFtpStatus != FTP_LOGGEDON )
		return FALSE;

	char msg[ FTP_STRSIZE ];
	wsprintf( msg, "CWD %s\r\n", pDir );
	Send( msg );	

	return TRUE;
}

BOOL CFtpClient::SetMode(DWORD mode)
{
	// Verify we are logged in
	if ( m_dwFtpStatus != FTP_LOGGEDON ) return FALSE;

	// Set transfer mode
	if ( mode == FTPMODE_ASCII ) Send( "TYPE A\r\n" );	
	else Send( "TYPE I\r\n" );	

	return TRUE;
}


BOOL CFtpClient::Quit()
{
	// Verify we are logged in
	if ( m_dwFtpStatus != FTP_LOGGEDON )
	{	Destroy();
		return FALSE;
	} // end if

	// Set transfer mode
	m_dwFtpStatus = FTP_LOGGEDOFF;
	Send( "QUIT\r\n" );	

	return TRUE;
}

BOOL CFtpClient::ListenCallback(CAsyncWinSock * pSocket, DWORD dwUser, WPARAM wParam, LPARAM lParam)
{
	CFtpClient	*pFtp = (CFtpClient*)dwUser;
	if ( dwUser == NULL ) return FALSE;
	if ( pSocket == NULL ) return FALSE;

	long	lEvent = (long)wParam;
	int		*piErr = (int*)lParam;

	if ( ( lEvent & FD_READ ) != 0 )
	{
	} // end if
	
	if ( ( lEvent & FD_WRITE ) != 0 )
	{
	} // end if

	if ( ( lEvent & FD_ACCEPT ) != 0 )
	{
		if ( pSocket->Accept( &pFtp->m_data ) )
			DataCallback( &pFtp->m_data, dwUser, FD_WRITE, NULL );
	} // end if

	if ( ( lEvent & FD_CONNECT ) != 0 )
	{
	} // end if

	if ( ( lEvent & FD_CLOSE ) != 0 )
	{
	} // end if

	return TRUE;
}

BOOL CFtpClient::DataCallback(CAsyncWinSock * pSocket, DWORD dwUser, WPARAM wParam, LPARAM lParam)
{
	CFtpClient	*pFtp = (CFtpClient*)dwUser;
	if ( dwUser == NULL ) return FALSE;

	long	lEvent = (long)wParam;
	int		*piErr = (int*)lParam;

	if ( ( lEvent & FD_READ ) != 0 )
	{
		{
			DWORD bytes = 0;
			
			if ( pSocket->GetRecvBytesWaiting( &bytes ) && bytes )
			{
				TMem< char > buf;

				if ( buf.allocate( bytes + 1 ) )
				{
					// Read the data and write it to a file
					if ( pSocket->Recv( buf, bytes ) )
					{
						// File or buffer?
						if ( pFtp->m_pData == NULL )
						{
							if ( pFtp->m_dwGetState == FTPGET_GETFILE )
								pFtp->m_file.Write( buf, bytes );
						} // end if
						else // Save data in buffer
						{
							DWORD write = bytes;
							if ( write > ( pFtp->m_dwData - pFtp->m_dwPtr ) )
								write = pFtp->m_dwData - pFtp->m_dwPtr;
							if ( write > 0 )
							{	memcpy( &pFtp->m_pData[ pFtp->m_dwPtr ], buf, write );
								pFtp->m_dwPtr += write;
							} // end if

						} // end else
					} // en dif

				} // end if

			} // end if

		} // end if

	} // end if
	
	if ( ( lEvent & FD_WRITE ) != 0 )
	{
		if ( 	pFtp->m_dwPutState == FTPPUT_PUTFILE ||
				pFtp->m_dwPutState == FTPPUT_CONNECTING )
		{
			DWORD read = 1;
			char buf[ FTP_STRSIZE ];

			do
			{
				
/*  !!! This stuff didn't work on Windows 95/98

				// Checking our socket
				fd_set	fs;
				FD_ZERO( &fs );
				FD_SET( pFtp->m_data.GetSocket(), &fs );

				// Return immediately
				timeval	tv;
				tv.tv_sec = 0;
				tv.tv_usec = 0;

				// See if we can read any data
				int status = select( 0, NULL, &fs, NULL, &tv );

				if ( FD_ISSET( pFtp->m_data.GetSocket(), &fs ) )
*/
				{
					// Do we have a file or buffer?
					if ( pFtp->m_pData == NULL )
					{
						DWORD fptr = pFtp->m_file.GetPtrPos();

						// Read data from file
						if ( !pFtp->m_file.Read( buf, sizeof( buf ), &read ) )
							read = 0;

						// Write data to server
						DWORD sent = 0;
						if ( read > 0 ) pSocket->Send( buf, read, &sent );

						// Ensure all data was sent
						if ( read > sent )
						{	Sleep( 50 );
							pFtp->m_file.SetPtrPosBegin( fptr + sent );
						} // end if

					} // end if
					else // Read data from buffer
					{
						DWORD sent = 0;

						read = pFtp->m_dwData - pFtp->m_dwPtr;
						if ( read > sizeof( buf ) ) read = sizeof( buf );
						if ( read > 0 ) 
						{	
							pSocket->Send( &pFtp->m_pData[ pFtp->m_dwPtr ], read, &sent );
							pFtp->m_dwPtr += sent;
							if ( sent != read ) Sleep( 50 );

						} // end if

					} // end else

				} // end if
//				else read = 1, Sleep( 15 );

			} while ( read > 0 );

			// Lose the socket
			pSocket->ThreadShutdown();			

		} // end if
	} // end if

	if ( ( lEvent & FD_ACCEPT ) != 0 )
	{
	} // end if

	if ( ( lEvent & FD_CONNECT ) != 0 )
	{
	} // end if

	if ( ( lEvent & FD_CLOSE ) != 0 )
	{
	} // end if

	return TRUE;
}

BOOL CFtpClient::Port(DWORD port)
{
	// Verify we are logged in
	if ( m_dwFtpStatus != FTP_LOGGEDON ) return FALSE;

	// Do we want passive mode?
	if ( m_bPassiveFtp )
		return Send( "PASV\r\n" );

	DWORD addr;
	if ( !GetConnectionInfo( &addr, NULL ) ) return FALSE;
	
	// Set transfer port
	char msg[ FTP_STRSIZE ];
	wsprintf( msg, "PORT %lu,%lu,%lu,%lu,%lu,%lu\r\n", 
					( addr & 0x000000ff ),
					( addr & 0x0000ff00 ) >> 8,
					( addr & 0x00ff0000 ) >> 16,
					( addr & 0xff000000 ) >> 24,
					( port & 0xff00 ) >> 8,
					( port & 0x00ff ) );	  
	Send( msg );	

	return TRUE;
}

BOOL CFtpClient::Get(LPCTSTR pLocal, LPCTSTR pRemote)
{
	// Verify we are logged in
	if ( m_dwFtpStatus != FTP_LOGGEDON ) return FALSE;

	// Create the local file
	if ( !m_file.OpenNew( pLocal ) ) return FALSE;

	// Copy strings
	m_pData = NULL; m_dwData = 0;
	strcpy_sz( m_szLocal, pLocal );
	strcpy_sz( m_szRemote, pRemote );

	// Set get status
	m_dwGetState = FTPGET_CHGDIR;

	// Get directory
	char path[ FTP_STRSIZE ];
	CWinFile::GetPathFromFileName( pRemote, path );

	// Check for root directory
	if ( *path == 0 ) strcpy( path, "/" );

	// Change directory
	return SetDirectory( path );

/*	// Set transfer port
	char msg[ FTP_STRSIZE ];
	wsprintf( msg, "GET %s\r\n", pRemote );
	Send( msg );	
*/

	return TRUE;
}

BOOL CFtpClient::CreateDirectory(LPCTSTR pDir)
{
	// Verify we are logged in
	if ( m_dwFtpStatus != FTP_LOGGEDON ) return FALSE;

	// Create directory
	char msg[ FTP_STRSIZE ];
	wsprintf( msg, "MKD %s\r\n", pDir );
	Send( msg );	

	return TRUE;
}

BOOL CFtpClient::StartGet(LPCTSTR pRemote)
{
	// Verify we are logged in
	if ( m_dwFtpStatus != FTP_LOGGEDON ) return FALSE;
	
	// Create directory
	char msg[ FTP_STRSIZE ];
	wsprintf( msg, "RETR %s\r\n", pRemote);
	Send( msg );	

	return TRUE;
}

BOOL CFtpClient::CompleteGet( LPCTSTR buf, DWORD size)
{
	// Complete GET
	if ( m_dwGetState == FTPGET_CHGDIR )
	{	m_dwGetState = FTPGET_SETPORT;
		Port( m_dwDataPort );
	} // end else
	else if ( m_dwGetState == FTPGET_SETPORT )
	{	m_dwGetState = FTPGET_GETFILE;

		if ( !m_bPassiveFtp )
		{
			// Setup the data port
			m_data.EnableCallbacks();
			m_data.SetCallback( &CFtpClient::DataCallback, (DWORD)this );

			// Setup the server port
			m_listen.EnableCallbacks();
			m_listen.SetCallback( &CFtpClient::ListenCallback, (DWORD)this );

			// Listen for connection
			if (	!m_listen.OpenSocket() ||
					!m_listen.Bind( m_dwDataPort ) ||
					!m_listen.Listen() ) return FALSE;
		} // end if
		else
		{
			char	addr[ FTP_STRSIZE ];
			DWORD	port;

			// Get passive address information
			GetPassiveAddress( buf, size, addr, &port );

			// Attempt connection
			m_data.EnableCallbacks();
			m_data.SetCallback( &CFtpClient::DataCallback, (DWORD)this );
			m_data.Connect( addr, port );

		} // end else
	
		StartGet( m_szRemote );
	} // end else
	else if ( m_dwGetState == FTPGET_GETFILE )
	{	m_dwGetState = FTPGET_CONNECTING;
	} // end else if
	else if ( m_dwGetState == FTPGET_CONNECTING )
	{	m_dwGetState = FTPGET_NONE;
		m_listen.Destroy();
		m_data.Destroy();
		SendFtpMessage( FTPMSG_CMDCOMPLETE, 0L );
	} // end else if

	return TRUE;
}

BOOL CFtpClient::GetPassiveAddress(LPCTSTR buf, DWORD size, LPSTR addr, LPDWORD port)
{
	DWORD i = 0, x = 0;

	// Sanity checks
	if ( buf == NULL || addr == NULL || port == NULL ) return FALSE;

	// Search for begining of address
	while ( i < size && buf[ i ] != '(' ) i++;

	// Save first number
	while ( i < size && ( buf[ i ] < '0' || buf[ i ] > '9' ) ) i++;
	while ( i < size && ( buf[ i ] >= '0' || buf[ i ] >= '9' ) )
		addr[ x++ ] = buf[ i++ ];
	addr[ x++ ] = '.';

	// Save second number
	while ( i < size && ( buf[ i ] < '0' || buf[ i ] > '9' ) ) i++;
	while ( i < size && ( buf[ i ] >= '0' || buf[ i ] >= '9' ) )
		addr[ x++ ] = buf[ i++ ];
	addr[ x++ ] = '.';

	// Save third number
	while ( i < size && ( buf[ i ] < '0' || buf[ i ] > '9' ) ) i++;
	while ( i < size && ( buf[ i ] >= '0' || buf[ i ] >= '9' ) )
		addr[ x++ ] = buf[ i++ ];
	addr[ x++ ] = '.';

	// Save forth number
	while ( i < size && ( buf[ i ] < '0' || buf[ i ] > '9' ) ) i++;
	while ( i < size && ( buf[ i ] >= '0' || buf[ i ] >= '9' ) )
		addr[ x++ ] = buf[ i++ ];
	addr[ x++ ] = 0;

	// Get port number
	while ( i < size && ( buf[ i ] < '0' || buf[ i ] > '9' ) ) i++;
	*port = strtoul( &buf[ i ], NULL, 10 );
	while ( i < size && ( buf[ i ] >= '0' || buf[ i ] >= '9' ) ) i++;
	while ( i < size && ( buf[ i ] < '0' || buf[ i ] > '9' ) ) i++;
	*port = ( (*port) << 8 ) + strtoul( &buf[ i ], NULL, 10 );

	return TRUE;
}

BOOL CFtpClient::Put(LPCTSTR pLocal, LPCTSTR pRemote)
{
	// Verify we are logged in
	if ( m_dwFtpStatus != FTP_LOGGEDON ) return FALSE;

	// Create the local file
	if ( !m_file.OpenExisting( pLocal ) ) return FALSE;

	// Copy strings
	m_pData = NULL; m_dwData = 0;
	strcpy_sz( m_szLocal, pLocal );

/*	char fname[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( pRemote, fname );
	CWinFile::GetPathFromFileName( pRemote, m_szTempRemote );
	CWinFile::WebBuildPath( m_szTempRemote, m_szTempRemote, "tmp_" );
	strcat( m_szTempRemote, fname );
	strcpy_sz( m_szRemote, pRemote );
*/

	char fname[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( pRemote, fname );
//	CWinFile::GetPathFromFileName( pRemote, m_szTempRemote );
//	CWinFile::WebBuildPath( m_szTempRemote, m_szTempRemote, "tmp_" );
	strcpy( m_szTempRemote, "tmp_" );
	strcat( m_szTempRemote, fname );
	strcpy_sz( m_szRemote, pRemote );
	

	// Get directory
	char path[ FTP_STRSIZE ];
	CWinFile::GetPathFromFileName( pRemote, path );

	// check for no path
	if ( *path == 0 ) m_bChgDir = FALSE;

	// Set directory only on first file
	if ( !m_bChgDir )
	{	m_dwPutState = FTPPUT_DELFILE1;
		return DeleteFile( m_szTempRemote );
	} // end if
	m_bChgDir = FALSE;

	if ( strlen( path ) > 1 )
	{	m_dwPutState = FTPPUT_MKDIR;
		return CreateDirectory( path );
	} // end if

	m_dwPutState = FTPPUT_CHGDIR;
	return SetDirectory( path );
}

BOOL CFtpClient::CompletePut(LPCTSTR buf, DWORD size)
{
	// Complete PUT
	if ( m_dwPutState == FTPPUT_MKDIR )
	{	m_dwPutState = FTPPUT_CHGDIR;
		char path[ FTP_STRSIZE ];
		CWinFile::GetPathFromFileName( m_szRemote, path );
		if ( *path == 0 ) strcpy( path, "/" );
		SetDirectory( path );
	} // end if
	else if ( m_dwPutState == FTPPUT_CHGDIR )
	{	m_dwPutState = FTPPUT_DELFILE1;
		DeleteFile( m_szTempRemote );
	} // end else
	else if ( m_dwPutState == FTPPUT_DELFILE1 )
	{	m_dwPutState = FTPPUT_SETMODE;
		SetMode( FTPMODE_BINARY );
	} // end else
	else if ( m_dwPutState == FTPPUT_SETMODE )
	{	m_dwPutState = FTPPUT_SETPORT;
		Port( m_dwDataPort );
	} // end else
	else if ( m_dwPutState == FTPPUT_SETPORT )
	{	m_dwPutState = FTPPUT_PUTFILE;

		if ( !m_bPassiveFtp )
		{
			// Setup the data port
			m_data.EnableCallbacks();
			m_data.SetCallback( &CFtpClient::DataCallback, (DWORD)this );

			// Setup the server port
			m_listen.EnableCallbacks();
			m_listen.SetCallback( &CFtpClient::ListenCallback, (DWORD)this );

			// Listen for connection
			if (	!m_listen.OpenSocket() ||
					!m_listen.Bind( m_dwDataPort ) ||
					!m_listen.Listen() ) return FALSE;
		} // end if
		else
		{
			char	addr[ FTP_STRSIZE ];
			DWORD	port;

			// Get passive address information
			GetPassiveAddress( buf, size, addr, &port );

			// Attempt connection
			m_data.EnableCallbacks();
			m_data.SetCallback( &CFtpClient::DataCallback, (DWORD)this );
			m_data.Connect( addr, port );

		} // end else
	
		StartPut( m_szTempRemote );
	} // end else
	else if ( m_dwPutState == FTPPUT_PUTFILE )
	{	m_dwPutState = FTPPUT_CONNECTING;
		
	} // end else if
	else if ( m_dwPutState == FTPPUT_CONNECTING )
	{	m_dwPutState = FTPPUT_DELFILE2;
		char fname[ FTP_STRSIZE ];
		CWinFile::GetFileNameFromPath( m_szRemote, fname );
		DeleteFile( fname );
		m_listen.Destroy();
		m_data.Destroy();
	} // end else if
	else if ( m_dwPutState == FTPPUT_DELFILE2 )
	{	m_dwPutState = FTPPUT_RENAMEFROM;
		RenameFrom( m_szTempRemote );
	} // end else
	else if ( m_dwPutState == FTPPUT_RENAMEFROM )
	{	m_dwPutState = FTPPUT_RENAMETO;
		char fname[ FTP_STRSIZE ];
		CWinFile::GetFileNameFromPath( m_szRemote, fname );
		RenameTo( fname );
	} // end else if
	else if ( m_dwPutState == FTPPUT_RENAMETO )
	{	m_dwPutState = FTPPUT_NONE;
		SendFtpMessage( FTPMSG_CMDCOMPLETE, 0L );
	} // end else if

	return TRUE;
}

BOOL CFtpClient::StartPut(LPCTSTR pRemote)
{
	// Verify we are logged in
	if ( m_dwFtpStatus != FTP_LOGGEDON ) return FALSE;
	
	// Create directory
	char msg[ FTP_STRSIZE ];
	wsprintf( msg, "STOR %s\r\n", pRemote);
	Send( msg );	

	return TRUE;
}

BOOL CFtpClient::Get(LPBYTE buf, DWORD size, LPCTSTR pRemote)
{
	// Verify we are logged in
	if ( m_dwFtpStatus != FTP_LOGGEDON ) return FALSE;

	// Copy data
	m_pData = buf; m_dwData = size; m_dwPtr = 0;
	*m_szLocal = NULL;
	strcpy_sz( m_szRemote, pRemote );

	// Set get status
	m_dwGetState = FTPGET_CHGDIR;

	// Get directory
	char path[ FTP_STRSIZE ];
	CWinFile::GetPathFromFileName( pRemote, path );

	// Check for root directory
	if ( *path == 0 ) strcpy( path, "/" );

	// Change directory
	return SetDirectory( path );

/*	// Set transfer port
	char msg[ FTP_STRSIZE ];
	wsprintf( msg, "GET %s\r\n", pRemote );
	Send( msg );	
*/

	return TRUE;
}

BOOL CFtpClient::Put(LPBYTE buf, DWORD size, LPCTSTR pRemote)
{
	// Verify we are logged in
	if ( m_dwFtpStatus != FTP_LOGGEDON ) return FALSE;

	// Copy data
	m_pData = buf; m_dwData = size; m_dwPtr = 0;
	*m_szLocal = NULL;

	char fname[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( pRemote, fname );
//	CWinFile::GetPathFromFileName( pRemote, m_szTempRemote );
//	CWinFile::WebBuildPath( m_szTempRemote, m_szTempRemote, "tmp_" );
	strcpy( m_szTempRemote, "tmp_" );
	strcat( m_szTempRemote, fname );
	strcpy_sz( m_szRemote, pRemote );

	// Get directory
	char path[ FTP_STRSIZE ];
	CWinFile::GetPathFromFileName( pRemote, path );
	if ( *path == 0 ) m_bChgDir = FALSE;

	// Set directory only on first file
	if ( !m_bChgDir )
	{	m_dwPutState = FTPPUT_DELFILE1;
		return DeleteFile( m_szTempRemote );
	} // end if
	m_bChgDir = FALSE;

	if ( strlen( path ) > 1 )
	{	m_dwPutState = FTPPUT_MKDIR;
		return CreateDirectory( path );
	} // end if

	m_dwPutState = FTPPUT_CHGDIR;
	return SetDirectory( path );
}

BOOL CFtpClient::RenameFrom(LPCTSTR pFile)
{
	// Verify we are logged in
	if ( m_dwFtpStatus != FTP_LOGGEDON ) return FALSE;

	// Create directory
	char msg[ FTP_STRSIZE ];
	wsprintf( msg, "RNFR %s\r\n", pFile );
	Send( msg );	

	return TRUE;
}

BOOL CFtpClient::RenameTo(LPCTSTR pFile)
{
	// Verify we are logged in
	if ( m_dwFtpStatus != FTP_LOGGEDON ) return FALSE;

	// Create directory
	char msg[ FTP_STRSIZE ];
	wsprintf( msg, "RNTO %s\r\n", pFile );
	Send( msg );	

	return TRUE;
}

BOOL CFtpClient::DeleteFile(LPCTSTR pFile)
{
	// Verify we are logged in
	if ( m_dwFtpStatus != FTP_LOGGEDON ) return FALSE;

	// Create directory
	char msg[ FTP_STRSIZE ];
	wsprintf( msg, "DELE %s\r\n", pFile );
	Send( msg );	

	return TRUE;
}

BOOL CFtpClient::IsEndOfReply(LPCTSTR buf, DWORD size)
{
	DWORD i = 0;
		
	while ( i < size )
	{
		// Skip white space
		while ( i < size && ( buf[ i ] <= ' ' || buf[ i ] > '~' ) ) i++;

		// Is it the number?
		if ( buf[ i ] >= '0' && buf[ i ] <= '9' )
		{
			// Skip number
			while ( buf[ i ] >= '0' && buf[ i ] <= '9' ) i++;

			// Skip spaces
			while ( i < size && ( buf[ i ] <= ' ' || buf[ i ] > '~' )
					&& buf[ i ] != '\r' && buf[ i ] != '\n' ) i++;

			// If there is no '-', then it is the end of the reply
			if ( i >= size || buf[ i ] != '-' ) return TRUE;

		} // end if

		// Skip to end of line
		while ( i < size && buf[ i ] != '\r' && buf[ i ] != '\n' ) i++;

	} // end while

	return FALSE;
}

DWORD CFtpClient::Send( LPVOID pData, DWORD len, LPDWORD sent, DWORD flags )
{
	APP()->FtpLog( FALSE, m_szHostName, (LPCTSTR)pData, len );

	return CAsyncWinSock::Send( pData, len, sent, flags );
}


DWORD CFtpClient::Recv( LPVOID pData, DWORD len, LPDWORD read, DWORD flags )
{
	BOOL ret = CAsyncWinSock::Recv( pData, len, read, flags );

	APP()->FtpLog( TRUE, m_szHostName, (LPCTSTR)pData, len );

	return ret;
}

