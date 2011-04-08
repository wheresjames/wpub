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
// SmtpClient.cpp: implementation of the CSmtpClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SmtpClient.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SMTP_BUFSIZE		4096

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSmtpClient::CSmtpClient()
{
	m_pTo = NULL;
	m_pFrom = NULL;
	m_pBody = NULL;
	m_dwBody = 0;
	m_dwBodyPtr = 0;

	*m_szFrom = 0;
	m_dwPort = 25;

	*m_szError = 0;

	m_hSmtpWnd = NULL;
	m_uSmtpMsg = 0;

	*m_szUsername = 0;
	*m_szPassword = 0;
	m_bAuthenticate = FALSE;
	m_dwAuthType = SMTPAUTH_CRAMMD5;
	m_bAutoType = TRUE;
	m_bTypeDetected = FALSE;
	m_dwState = SMTP_INIT;

	m_bDestroy = FALSE;
}

CSmtpClient::~CSmtpClient()
{
	Destroy();
}

void CSmtpClient::Destroy()
{
	ReleaseMemory();

	// Lose the socket connection
	CloseSocket();
}

void CSmtpClient::ReleaseMemory()
{
	if ( m_pTo != NULL )
	{	delete [] m_pTo;
		m_pTo = NULL;
	} // end if

	if ( m_pFrom != NULL )
	{	delete [] m_pFrom;
		m_pFrom = NULL;
	} // end if

	m_dwBody = 0;
	if ( m_pBody != NULL )
	{	delete [] m_pBody;
		m_pBody = NULL;
	} // end if
}

BOOL CSmtpClient::OnConnect( int nErr )
{
	// Verify there was no error
	if ( nErr ) return FALSE;

	return TRUE;
}

BOOL CSmtpClient::OnWrite( int nErr )
{
	// Finish message
	if ( m_dwState == SMTP_MSG ) SendMsg();

	return TRUE;

} // end if

#define RCVBUFSIZE	4096
BOOL CSmtpClient::OnRead( int nErr )
{
	DWORD bytes = 0;

	// How many bytes are waiting?
	if ( !GetRecvBytesWaiting( &bytes ) )
		return FALSE;

	// Allocate buffer
//	char *buf = new char[ bytes + 1 ];
	TMem< char > buf;
	if ( !buf.allocate( bytes + 1 ) ) return FALSE;

//	char buf[ SMTP_BUFSIZE + 1 ];

	// Read data
	if ( !Recv( buf, bytes, &bytes ) || bytes == 0 )
		return FALSE;
	buf[ bytes ] = 0;

/*	char *buf = new char[ RCVBUFSIZE + 1 ];
	if ( buf == NULL ) return FALSE;

	// Read in data
	DWORD read = 0;
	while(	bytes < RCVBUFSIZE && 
			Recv( &buf[ bytes ], RCVBUFSIZE - bytes, &read ) && 
			read > 0 ) bytes += read, read = 0;
	bytes += read;
	if ( bytes == 0 ) { delete [] buf; return FALSE; }

	// NULL terminate
	buf[ bytes ] = 0;
*/
	// Process the reply
	ProcessReply( buf, bytes );

	if ( m_bDestroy ) { SignalStop(); m_bDestroy = FALSE; }


	return TRUE;
}

BOOL CSmtpClient::Connect(LPCTSTR pAddr, DWORD dwPort, HWND hWnd, UINT uMsg)
{
	// Reset state
	m_dwState = SMTP_INIT;

	// Set the message target
	EnableCallbacks();
	SetMessageTarget( hWnd, uMsg );

	// Attempt to connect
	return CAsyncWinSock::Connect( pAddr, dwPort );
}

BOOL CSmtpClient::ProcessReply(LPCTSTR buf, DWORD size)
{
	// Get error code
	DWORD code = strtoul( buf, NULL, 10 );

	// Check for error
	if ( code >= 400 )
	{
		// Save error
		strcpy( m_szError, buf );

		// Are we authenticating
		if (	m_dwState >= SMTP_EHLO && m_dwState <= SMTP_AUTHENTICATED &&
				m_bAuthenticate )
		{	
			if ( m_bAutoType )
			{
				// Try each type of authentication
				if ( m_dwAuthType > 0 )
				{
					// Backup until we are supported
					while( m_dwAuthType && 
							(	!m_bAuthSupported[ m_dwAuthType ] || 
								m_dwAuthType == SMTPAUTH_DIGESTMD5 ) )
						m_dwAuthType--;
					
					if ( !m_dwAuthType ) m_bAuthenticate = FALSE;
				} // end if
				else m_bAuthenticate = FALSE;

				// Skip this type
				if ( m_dwAuthType == SMTPAUTH_DIGESTMD5 )
					m_dwAuthType--;

				// Start over
				m_dwState = SMTP_INIT;
			} // end if
			else
			{
				// Send message
				SendSmtpMsg( SMTP_FAILED );

				_Log( MB_ICONERROR, "SMTP (E-mail)", buf );

				// Lose the socket
				m_bDestroy = TRUE;

				return FALSE;
			} // end else


		} // end if

		else if ( m_dwState == SMTP_TO )
		{
			// Ignore invalid user

		} // end else if

		// Unrecoverable errors
		else
		{
			// Send message
			SendSmtpMsg( SMTP_FAILED );

			_Log( MB_ICONERROR, "SMTP (E-mail)", buf );

			// Lose the socket
			m_bDestroy = TRUE;

			return FALSE;

		} // end if

	} // end if

	// Not supporting this one for now
	if ( m_dwAuthType == SMTPAUTH_DIGESTMD5 ) 
		m_dwAuthType = SMTPAUTH_CRAMMD5;

	// Auto detect authentication types
	if ( !m_bTypeDetected &&
		 ( m_dwState == SMTP_EHLO || m_dwState == SMTP_AUTH1 ) )
	{
		// Check for supported types
		CheckAuth( buf, size );

		// Skip if type returned
		if ( m_dwState == SMTP_EHLO && m_bTypeDetected )
			m_dwState = SMTP_AUTH1;

	} // end if

	// Send Helo
	if ( m_dwState == SMTP_INIT )
	{	
		SendSmtpMsg( SMTP_INITIALIZING );
		
		// Start communications
		if ( m_bAuthenticate && m_dwAuthType > 0 ) 
		{	m_dwState = SMTP_EHLO;
			Ehlo( m_szFrom );
		} // end if
		else
		{	m_dwState = SMTP_HELO;
			Helo( m_szFrom );
		} // end else

	} // end if

	else if ( m_dwState == SMTP_EHLO )
	{	m_dwState = SMTP_AUTH1;
	
		if ( IsCommandComplete( buf, size ) )
		{	m_dwState = SMTP_AUTH2;
			
			if ( m_dwAuthType == SMTPAUTH_PLAIN ) AuthPlain();

			else if ( m_dwAuthType == SMTPAUTH_LOGIN ) AuthLogin();

			else AuthCramMd5();

		} // end if

	} // end else if

	else if ( m_dwState == SMTP_AUTH1 )
	{	m_dwState = SMTP_AUTH2;

		if ( m_dwAuthType == SMTPAUTH_PLAIN ) AuthPlain();

		else if ( m_dwAuthType == SMTPAUTH_LOGIN ) AuthLogin();

		else AuthCramMd5();

	} // end else if

	else if ( m_dwState == SMTP_AUTH2 )
	{	m_dwState = SMTP_AUTH3;

		if ( m_dwAuthType == SMTPAUTH_PLAIN ) AuthPlainUsername();

		else if ( m_dwAuthType == SMTPAUTH_LOGIN ) AuthLoginUsername();

		else 
		{ 	m_dwState = SMTP_AUTHENTICATED; 
			AuthCramMd5Challenge( buf, size ); 
		} // end else

	} // end else if

	else if ( m_dwState == SMTP_AUTH3 )
	{	m_dwState = SMTP_AUTHENTICATED;

		if ( m_dwAuthType == SMTPAUTH_PLAIN ) AuthPlainPassword();

		else AuthLoginPassword();

	} // end else if

	else if ( m_dwState == SMTP_HELO || m_dwState == SMTP_AUTHENTICATED )
	{	m_dwState = SMTP_FROM;
		MailFrom( m_pFrom );
		m_dwToPtr = 0;
	} // end if

	else if ( m_dwState == SMTP_FROM || m_dwState == SMTP_TO )
	{
		// Continue if no more addresses
		if ( !SendNextEmail() )	m_dwState = SMTP_ENDTO;
		else SMTP_TO;

	} // end if

	if ( m_dwState == SMTP_ENDTO )
	{	m_dwState = SMTP_DATA;
		Data();
	} // end if

	else if ( m_dwState == SMTP_DATA )
	{	m_dwState = SMTP_MSG;
		SendSmtpMsg( SMTP_SENDINGMSG );
		m_bSentEnd = 0;
		SendMsg();
	} // end if

	else if ( m_dwState == SMTP_MSG )
	{	m_dwState = SMTP_QUIT;
		Quit();
	} // end else if

	else if ( m_dwState == SMTP_QUIT )
	{	m_dwState = SMTP_INIT;
		SendSmtpMsg( SMTP_COMPLETE );
		m_bDestroy = TRUE;
	} // end else if

//	FRAME()->SetStatus( buf );

	return TRUE;
}


BOOL CSmtpClient::Helo(LPCTSTR pDomain)
{
	// Verify that we are connected
	if ( !IsConnected() ) return FALSE;
	
	char msg[ SMTP_STRSIZE ];
	wsprintf( msg, "HELO %s\r\n", pDomain );
	Send( msg );	

	return TRUE;
}

BOOL CSmtpClient::Ehlo(LPCTSTR pDomain)
{
	// Verify that we are connected
	if ( !IsConnected() ) return FALSE;
	
	char msg[ SMTP_STRSIZE ];
	wsprintf( msg, "EHLO %s\r\n", pDomain );
	Send( msg );	

	return TRUE;
}

BOOL CSmtpClient::MailFrom(LPCTSTR pUser)
{
	// Verify that we are connected
	if ( !IsConnected() ) return FALSE;
	if ( pUser == NULL ) return FALSE;
	
	char msg[ SMTP_STRSIZE ];
	wsprintf( msg, "MAIL FROM:%s\r\n", pUser );
	Send( msg );	

	return TRUE;
}

BOOL CSmtpClient::RcptTo(LPCTSTR pUser)
{
	// Verify that we are connected
	if ( !IsConnected() ) return FALSE;
	if ( pUser == NULL ) return FALSE;
	
	char msg[ SMTP_STRSIZE ];
	wsprintf( msg, "RCPT TO:%s\r\n", pUser );
	Send( msg );	

	return TRUE;
}


BOOL CSmtpClient::Data()
{
	// Verify that we are connected
	if ( !IsConnected() ) return FALSE;
	
	Send( "DATA\r\n" );	

	return TRUE;
}

BOOL CSmtpClient::Quit()
{
	// Verify that we are connected
	if ( !IsConnected() ) return FALSE;
	
	Send( "QUIT\r\n" );	

	return TRUE;
}

BOOL CSmtpClient::InitMail(CMime *pMime, LPCTSTR pFrom)
{
	if ( IsConnected() ) return FALSE;
	Destroy();

	// Don't destroy yet
	m_bDestroy = FALSE;

	// Init state
	m_dwState = SMTP_INIT;

	// Clear errors
	*m_szError = 0;

	// Ensure valid message object
	if ( pMime == NULL ) return FALSE;

	// Lose old memory
	ReleaseMemory();

	// Get the mime header
	LPMIMEBLOCK header = pMime->GetHeader();
	if ( header == NULL ) return FALSE;

	// Where is it going
	LPVAR v = header->var.FindVar( "To" );
	if ( v == NULL || v->val == NULL ) return FALSE;
	m_pTo = new char[ strlen( (char*)v->val ) + 1 ];
	strcpy( m_pTo, (char*)v->val );

	// Who is it from?
	v = header->var.FindVar( "From" );
	if ( v == NULL || v->val == NULL ) return FALSE;
	m_pFrom = new char[ strlen( (char*)v->val ) + 1 ];
	strcpy( m_pFrom, (char*)v->val );

	m_dwBody = 0;
	m_dwBodyPtr = 0;
	
	// Get the message size
	DWORD msgsize = pMime->SaveToMem( NULL, 0 );
	if ( msgsize > 0 )
	{
		// Allocate memory
		char *msgbuf = new char[ msgsize + 1 ];
		if ( msgbuf != NULL )
		{
			// Create the message
			msgsize = pMime->SaveToMem( (LPBYTE)msgbuf, msgsize );		
			if ( msgsize != 0 ) 
			{	
				DWORD i = 0;

				// Must add extra character for dots at beginning of lines
				m_dwBody = msgsize;
				for ( i = 0; i < ( msgsize - 3 ) && msgbuf[ i ] != 0; i++ )
					if ( !strnicmp( &msgbuf[ i ], "\r\n.", 3 ) ) m_dwBody++;

				// Allocate memory
				m_pBody = new char[ m_dwBody + 1 ];

				// Copy the message
				DWORD c = 0;
				for ( i = 0; i < msgsize && msgbuf[ i ] != 0; i++ )
				{
					char ch = msgbuf[ i ];
					
					// Ignore non 7-bit characters
					if ( ch <= 0 );

					// Check for end flag (replace .'s at start of line with ..)
					else if ( !strnicmp( &msgbuf[ i ], "\r\n.", 3 ) )
					{	i += 2;
						m_pBody[ c++ ] = '\r';
						m_pBody[ c++ ] = '\n';
						m_pBody[ c++ ] = '.';
						m_pBody[ c++ ] = '.';
					} // end else if

					// Just save the character
					else m_pBody[ c++ ] = ch;

					// Save the new size
					m_dwBody = c;

				} // end for				

			} // ebd uf

			delete [] msgbuf;
		} // end if
	} // end if

	// Save the domain name
	if ( pFrom != NULL ) strcpy( m_szFrom, pFrom );
	else 
	{	char domain[ MIME_STRSIZE ] = { 0 };
		if ( CMime::GetEmailComponents( m_pFrom, NULL, NULL, domain ) )
			strcpy( m_szFrom, domain );
		else *m_szFrom = 0;
	} // end else

	// Ensure we got everything we need
	if ( m_pTo == NULL || m_pFrom == NULL || m_pBody == NULL )
	{	ReleaseMemory();
		return FALSE;
	} // end if

	return TRUE;
}

BOOL CSmtpClient::Mail(CMime * pMime, LPCTSTR pFrom, LPCTSTR pServer, HWND hWnd, UINT uMsg )
{
	// Init message
	if ( !InitMail( pMime, pFrom ) )
	{	_Log( MB_ICONERROR, pServer,	"Cannot create email message. "
										"Missing address or subject" );
		return FALSE;
	} // end if

	// Connect to server to get the ball rolling
	if ( !Connect( pServer, m_dwPort, hWnd, uMsg ) )
	{	ReleaseMemory();
		_Log( MB_ICONERROR, pServer, "Cannot connect to server." );		
		return FALSE;
	} // end if

	// Reset email pointer
	m_dwToPtr = 0;

	// Will we be authenticating?
	m_bAuthenticate = *m_szUsername != 0;

	// Assume all supported
	m_bTypeDetected = FALSE;
	memset( m_bAuthSupported, 0xff, sizeof( m_bAuthSupported ) );

	// Auto type?
	if ( m_bAutoType ) m_dwAuthType = SMTPAUTH_CRAMMD5;

	SendSmtpMsg( SMTP_CONNECTING );

	return TRUE;
}

void CSmtpClient::SendMsg()
{
	// Any more data?
	if ( m_pBody == NULL ) return;
	if ( m_dwBodyPtr >= m_dwBody )
	{	if ( !m_bSentEnd ) 
		{	if ( Send( "\r\n.\r\n" ) ) m_bSentEnd; }
		return;
	} // end if

	m_bSentEnd = 0;
	DWORD sent = 0, send = 0;
	while ( m_dwBodyPtr < m_dwBody && sent == send )
	{
		// How many bytes to send?
		send = m_dwBody - m_dwBodyPtr;
		if ( send > SMTP_BUFSIZE ) send = SMTP_BUFSIZE;

		// Send the next block of data
		sent = 0;
		if ( !Send( &m_pBody[ m_dwBodyPtr ], send, &sent ) )
			sent = 0;

		// Add number of bytes sent
		m_dwBodyPtr += sent;

	} // end while

	// Is the message over?
	if ( m_dwBodyPtr >= m_dwBody ) 
	{	if ( Send( "\r\n.\r\n" ) ) m_bSentEnd; }

	// Update caller
	SendSmtpMsg( SMTP_SENDINGMSG );
}

BOOL CSmtpClient::SendNextEmail()
{
	char email[ MIME_STRSIZE ];

	if ( m_pTo == NULL ) return FALSE;

	// Get the next email in the list
	DWORD skip = CMime::GetNextEmail( NULL, email, &m_pTo[ m_dwToPtr ] );
	if ( skip == 0 ) return FALSE;

	// Skip this address
	m_dwToPtr += skip;		

	// Send this email address
	RcptTo( email );

	return TRUE;
}

BOOL CSmtpClient::Auth(LPCTSTR pType)
{
	// Verify that we are connected
	if ( !IsConnected() ) return FALSE;
	if ( pType == NULL ) return FALSE;
	
	char msg[ SMTP_STRSIZE ];
	wsprintf( msg, "AUTH %s\r\n", pType );
	Send( msg );	

	return TRUE;
}

// PLAIN LOGIN CRAM-MD5 DIGEST-MD5

BOOL CSmtpClient::AuthPlain()
{
	return Auth( "PLAIN" );
}

BOOL CSmtpClient::AuthPlainUsername()
{
	char msg[ SMTP_STRSIZE ];
	wsprintf( msg, "%s\r\n", m_szUsername );

	return Send( msg );
}

BOOL CSmtpClient::AuthPlainPassword()
{
	char msg[ SMTP_STRSIZE ];
	wsprintf( msg, "%s\r\n", m_szPassword );
	
	return Send( msg );
}

BOOL CSmtpClient::AuthLogin()
{
	return Auth( "LOGIN" );
}

BOOL CSmtpClient::AuthLoginUsername()
{
	char msg[ SMTP_STRSIZE ];

	// Create base64 string from username
	msg[ CBase64::Encode( msg, sizeof( msg ), 
			(LPBYTE)m_szUsername, strlen( m_szUsername ) ) ] = 0;
	strcat( msg, "\r\n" );
	
	return Send( msg );
}

BOOL CSmtpClient::AuthLoginPassword()
{
	char msg[ SMTP_STRSIZE ];

	// Create base64 string from username
	msg[ CBase64::Encode( msg, sizeof( msg ),
			(LPBYTE)m_szPassword, strlen( m_szPassword ) ) ] = 0;
	strcat( msg, "\r\n" );
	
	return Send( msg );
}

BOOL CSmtpClient::AuthCramMd5()
{
	return Auth( "CRAM-MD5" );
}

//	Example in RFC2195
//	pUsername "tim"
//	pPassword "tanstaaftanstaaf"
//	pText "<1896.697170952@postoffice.reston.mci.net>"
//  pCramMd5 = "dGltIGI5MTNhNjAyYzdlZGE3YTQ5NWI0ZTZlNzMzNGQzODkw"
//  *Unencoded = "tim b913a602c7eda7a495b4e6e7334d3890"

BOOL CSmtpClient::GetCramMd5(LPSTR pCramMd5, LPCTSTR pUsername, LPCTSTR pPassword, LPCTSTR pText)
{
	// Sanity checks
	if (	pCramMd5 == NULL || pUsername == NULL ||
			pPassword == NULL || pText == NULL ) return FALSE;

	CMD5	md5;
	DWORD	h1[ 4 ];
	char	ki[ SMTP_STRSIZE ], ko[ SMTP_STRSIZE ];

	// Copy info
	ZeroMemory( ki, sizeof( ki ) );
	ZeroMemory( ko, sizeof( ko ) );

	// Use key if less than 64 bytes in length
	if ( strlen( pPassword ) <= 64 )
	{	strcpy( ki, pPassword );
		strcpy( ko, pPassword );
	} // end if

	// Use md5 hash if more than 64 bytes
	else
	{	md5.Init();
		md5.Transform( (LPVOID)pPassword, strlen( pPassword ) );
		memcpy( ki, md5.m_hash, sizeof( md5.m_hash ) );
		memcpy( ko, md5.m_hash, sizeof( md5.m_hash ) );
	} // end else
	
	// Create k1 and k2
	DWORD i;
	for ( i = 0; i < 64; i++ ) 
		ki[ i ] ^= 0x36, ko[ i ] ^= 0x5c;
	
	// Gen first hash
	md5.Init();
	md5.Transform( ki, 64 );
	md5.Transform( (LPVOID)pText, strlen( pText ) );
	md5.End();
	memcpy( h1, md5.m_hash, sizeof( h1 ) );

	// Gen second hash
	md5.Init();
	md5.Transform( ko, 64 );
	md5.Transform( h1, sizeof( h1 ) );
	md5.End();

	// Create string
	char str[ SMTP_STRSIZE ];
	wsprintf( str, "%s %08lx%08lx%08lx%08lx", pUsername,
				md5.m_digest[ 0 ], md5.m_digest[ 1 ],
				md5.m_digest[ 2 ], md5.m_digest[ 3 ] );

	// Do base64 encoding
	pCramMd5[ CBase64::Encode( pCramMd5, SMTP_STRSIZE, (LPBYTE)str, strlen( str ) ) ] = 0;

	return TRUE;
}


BOOL CSmtpClient::AuthCramMd5Challenge(LPCTSTR buf, DWORD size)
{
	DWORD i = 0;
	
	// Skip code
	while ( i < size && buf[ i ] >= '0' && buf[ i ] <= '9' ) i++;

	// Skip to hash
	while ( i < size && ( buf[ i ] <= ' ' || buf[ i ] > '~' ) ) i++;

	// Anything left?
	if ( i >= size ) return FALSE;

	char text[ SMTP_STRSIZE ];
	DWORD srcsize = strlen( &buf[ i ] ), dstsize = sizeof( text );
	CBase64::Decode( &buf[ i ], &srcsize, (LPBYTE)text, &dstsize, NULL );

	// Some servers seem to add garbage to the end
	DWORD x = 0;
	while ( x < SMTP_STRSIZE && text[ x ] != 0 && text[ x ] != '>' ) x++;
	if ( text[ x ] == '>' ) text[ ++x ] = 0;
	x = 0;
	while ( x < SMTP_STRSIZE && text[ x ] != 0 && text[ x ] != '<' ) x++;

	// Get the encrypted string
	char crammd5[ SMTP_STRSIZE ];
	if ( !GetCramMd5( crammd5, m_szUsername, m_szPassword, &text[ x ] ) )
		return FALSE;

	// Terminate command
	strcat( crammd5, "\r\n" );

	// Send it
	return Send( crammd5 );
}

static char *g_authtypes[] =
{
	"NONE",
	"PLAIN",
	"LOGIN",
	"DIGEST-MD5",
	"CRAM-MD5",
	0
};

BOOL CSmtpClient::CheckAuth(LPCTSTR buf, DWORD size)
{
	if ( buf == NULL ) return FALSE;

	BOOL bDetected = FALSE;

	ZeroMemory( m_bAuthSupported, sizeof( m_bAuthSupported ) );

	// See if types are supported
	for ( DWORD i = 0; i < size && buf[ i ] != 0; i++ )
	{
		if ( !strnicmp( &buf[ i ], "AUTH", 4 ) )
			bDetected = TRUE;

		else
		{
			// For each type
			for (	DWORD x = 0; 
					x < SMTPAUTH_TYPES && g_authtypes[ x ] != NULL; 
					x++ )

				// Check for type
				if ( !strnicmp( &buf[ i ], g_authtypes[ x ], strlen( g_authtypes[ x ] ) ) )
				{	m_bAuthSupported[ x ] = TRUE; x = SMTPAUTH_TYPES; }

		} // end else

	} // end for

	if ( m_bAutoType )
	{
		// Use most prefered login type
		m_dwAuthType = SMTPAUTH_TYPES - 1;

		// Backup until we are supported
		while( m_dwAuthType && 
				(	!m_bAuthSupported[ m_dwAuthType ] || 
					m_dwAuthType == SMTPAUTH_DIGESTMD5 ) )
			m_dwAuthType--;

		// If nothing supported, assume there was a mistake
		if ( !m_dwAuthType ) m_dwAuthType = SMTPAUTH_TYPES - 1;
	} // end if

	// Set flag if detected type
	if ( bDetected ) m_bTypeDetected = TRUE;

	return bDetected;
}


BOOL CSmtpClient::IsCommandComplete(LPCTSTR buf, DWORD size)
{
	DWORD i = 0;

	while( i < size && buf[ i ] != 0 )
	{
		// Skip CRLF's
		while ( i < size && ( buf[ i ] == '\r' || buf[ i ] == '\n' ) ) i++;

		// Skip number
		while ( i < size && ( buf[ i ] >= '0' && buf[ i ] <= '9' ) ) i++;

		// Does the command end here?
		if ( i < size && buf[ i ] != '-' ) return TRUE;

		// Find the end of the line
		while ( i < size && buf[ i ] != '\r' && buf[ i ] != '\n' ) i++;

	} // end while

	return FALSE;
}
