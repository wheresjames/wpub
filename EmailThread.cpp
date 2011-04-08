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
// EmailThread.cpp: implementation of the CEmailThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vp.h"
#include "EmailThread.h"

#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEmailThread::CEmailThread()
{
	m_hEmail = CreateEvent( NULL, TRUE, FALSE, NULL );
}

CEmailThread::~CEmailThread()
{
	CloseHandle( m_hEmail );

}

BOOL CEmailThread::InitThread( LPVOID pData )
{
	return TRUE;
}

BOOL CEmailThread::DoThread( LPVOID pData )
{
	HANDLE	phCmd[ 3 ];

	phCmd[ 0 ] = m_hStop;
	phCmd[ 1 ] = m_hEmail;

	// Wait
	DWORD ret = WaitForMultipleObjects( 2, phCmd, FALSE, INFINITE );

	// Verify range
	if ( ret < WAIT_OBJECT_0 ) return TRUE;

	// Get event index
	ret -= WAIT_OBJECT_0;

	// Check for stop
	if ( ret == 0 ) return FALSE;

	// Do we want to send an e-mail?
	else if ( ret == 1 )
	{
		SendEmail();
		ResetEvent( m_hEmail );
	} // end else if

	return TRUE;
}

BOOL CEmailThread::EndThread( LPVOID pData )
{
	return TRUE;
}

BOOL CEmailThread::SendEmail()
{
	// Are we already e-mailing?
	if ( m_smtp.IsConnected() ) return FALSE;

	CMime	mime;
	if ( !mime.Create( m_to, m_from, m_subject ) ) return FALSE;

	// Add the body of the message
	mime.AddPlainText( m_body );

	// Attach file if needed
	if ( m_fname.size() )
	{
		if ( m_sImg.size() )
		{
			CWinImg		img;
			LPBYTE		buf = NULL;
			DWORD		size = 0;

			// Get the image
			if ( !IMGLIST().GetImage( m_sImg, &img ) ) return FALSE;

			// Encode the image
			FRAME()->SetQuality( &img );
			if ( !img.Encode( &buf, &size, m_fname ) || buf == NULL || size == 0 )
				return FALSE;

			// Add to the mime file
			if ( !mime.AddFile( m_fname, buf, size ) ) return FALSE;

		} // end if

		// Do we need to add a disk file
		else if ( m_mem.size() )
		{
			mime.AddFile( m_fname, m_mem.ptr(), m_mem.size() );

		} // end else if

	} // end if

	// Set username and password
	if ( m_login != 0 )
	{
		m_smtp.SetUsernamePassword( m_username, m_password );

		// Testing authtypes
		if ( m_authdetect != 0 )
		{	m_smtp.AutoAuthType( TRUE );
			m_smtp.SetAuthType( m_authtype );
		} // end if
		else m_smtp.AutoAuthType( FALSE );
	} // end if

	// Send the mail
	m_smtp.SetCallback( CEmailThread::SocketCallback, (DWORD)this );

	// Start the mailing
	return m_smtp.Mail( &mime, NULL, m_server, NULL, 0 );
}

void CEmailThread::Email()
{	SetEvent( m_hEmail );
}

BOOL CEmailThread::Email(	LPCTSTR pImg, LPCTSTR pDiskFile,
							LPCTSTR pTo, LPCTSTR pFrom, 
							LPCTSTR pSubject, LPCTSTR pFile, 
							LPCTSTR pServer, LPCTSTR pUsername, 
							LPCTSTR pPassword, LPCTSTR pBody, 
							DWORD dwLogin, DWORD dwAuthDetect, 
							DWORD dwAuthType)
{
	// Are we already e-mailing?
	if ( 	WaitForSingleObject( m_hEmail, 0 ) != WAIT_TIMEOUT ||
			m_smtp.IsConnecting() || m_smtp.IsConnected() ) return FALSE;

	// Copy message data
	m_to.copy( pTo );
	m_from.copy( pFrom );
	m_subject.copy( pSubject );
	m_fname.copy( pFile );
	m_body.copy( pBody );

	// Copy server data
	m_server.copy( pServer );
	m_username.copy( pUsername );
	m_password.copy( pPassword );

	m_login = dwLogin;
	m_authdetect = dwAuthDetect;
	m_authtype = dwAuthType;
	
	m_sImg.copy( pImg );

	// Load disk file if needed
	m_mem.destroy();
	if ( pDiskFile != NULL )
	{	CWinFile wf;
		if ( wf.OpenExisting( pDiskFile ) )
		{	DWORD size = wf.Size();
			if ( size && m_mem.allocate( size ) )
				wf.Read( m_mem.ptr(), size );
		} // end if
	} // end if

	// Validate params
	if ( m_server.size() == 0 || m_from.size() == 0 ) return FALSE;

	StartThread();

	// Signal send email
	Email();

	return TRUE;
}

BOOL CEmailThread::SocketCallback(CAsyncWinSock *pSocket, DWORD dwUser, WPARAM wParam, LPARAM lParam)
{
	CEmailThread *pEmail = (CEmailThread*)dwUser;
	if ( pEmail == NULL ) return FALSE;	

	long	lNetworkEvents = (long)wParam;
	int		*iErrorCodes = (int*)lParam;

	if ( ( lNetworkEvents & FD_CONNECT ) != 0 )
	{
		if ( iErrorCodes[ FD_CONNECT_BIT ] != 0 )
		{	char msg[ CWF_STRSIZE ];
			if ( !CAsyncWinSock::GetErrorMsg( msg, iErrorCodes[ FD_CONNECT_BIT ], FALSE ) )
				strcpy( msg, "Unknown Error" );
			_Log( MB_ICONERROR, "E-mail", msg );
			pEmail->m_smtp.ThreadCloseSocket();
		} // end if
	} // end if

	else if ( ( lNetworkEvents & FD_CLOSE ) != 0 )
	{
		if ( iErrorCodes[ FD_CONNECT_BIT ] != 0 )
		{	char msg[ CWF_STRSIZE ];
			if ( !CAsyncWinSock::GetErrorMsg( msg, iErrorCodes[ FD_CONNECT_BIT ], FALSE ) )
				strcpy( msg, "Unknown Error" );
			_Log( MB_ICONERROR, "E-mail", msg );
		} // end if
		pEmail->m_smtp.ThreadCloseSocket();
	} // end if

	return TRUE;
}
