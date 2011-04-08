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
// HttpSession.cpp: implementation of the CHttpSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vp.h"
#include "HttpSession.h"

#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHttpSession::CHttpSession()
{

}

CHttpSession::~CHttpSession()
{
	Destroy();
	m_img.Destroy();
}

CAsyncWinSock* CHttpSession::sCreate()
{
	return new CHttpSession();
}

void CHttpSession::sDelete( CAsyncWinSock *ptr )
{
	CHttpSession *http = (CHttpSession*)ptr;
	if ( http != NULL ) delete http;
}

BOOL CHttpSession::OnRead(int nErr)
{
	DoRead();

	ThreadShutdown();
//	ThreadCloseSocket();

	return TRUE;
}


BOOL CHttpSession::DoRead()
{
	// Read data from socket
	if ( !Recv( m_pData, SESSIONBUFFERSIZE - 1, &m_dwData ) ) return FALSE;

	// NULL terminate data
	m_pData[ m_dwData ] = NULL;

	// Parse the data
	HTTPREQUEST	hr;
	if ( !Parse( m_pData, m_dwData, &hr ) ) return FALSE;

	// Get address and port info
	GetConnectionInfo( &hr.ip, &hr.port );

	// Punt if shutting down
	if ( IsStopping() ) return FALSE;

		// Show requested URL
		if ( m_hWnd != NULL )
			if ( ::SendMessage( m_hWnd, m_uMsg, (WPARAM)HTTPMSG_URL, (LPARAM)&hr ) != 0 )
			{	SendError( HTTPERR_FORBIDDEN );
				Log( HTTPERR_FORBIDDEN, &hr );
				SendErrorMsg( "Error 403: Access Denied." );
				return FALSE;
			} // end if
		
	// Process the request
	ProcessRequest( &hr );

	return TRUE;
}


BOOL CHttpSession::Parse(LPBYTE buf, DWORD size, LPHTTPREQUEST phr)
{
	DWORD i = 0;

	// Sanity check
	if ( buf == NULL || size < 4 || phr == NULL ) 
		return FALSE;

	// Zero structure
	ZeroHTTPREQUEST( phr );

	// GET/POST type
	if ( !strnicmp( (char*)&buf[ i ], "GET", 3 ) ) phr->type |= HRTYPE_GET;
	else if ( !strnicmp( (char*)&buf[ i ], "HEAD", 4 ) ) phr->type |= HRTYPE_HEAD;
	else if ( !strnicmp( (char*)&buf[ i ], "POST", 4 ) ) phr->type |= HRTYPE_POST;
	else return FALSE;

	// Search for next token
	while ( buf[ i ] != ' ' && i < size ) i++;
	if ( i >= size ) return FALSE;
	i++;

	// Read in the document name
	DWORD t = 0;
	char tbuf[ 1024 ];
	while ( t < ( sizeof( tbuf ) - 1 ) && buf[ i ] > ' ' && buf[ i ] <= '~' )
		tbuf[ t++ ] = buf[ i++ ];
	tbuf[ t ] = 0x0;

	// replace escape characters
	DWORD out = CCfgFile::GetMinDeCanonicalizeBufferSize( strlen( tbuf ) );
	if ( out > 0 )
	{
		TMem< char > decon;
		if ( decon.allocate( out + 1 ) )
		{
			DWORD sz = strlen( tbuf );
			if ( CCfgFile::DeCanonicalizeBuffer( tbuf, (LPBYTE)decon.ptr(), decon.size(), &sz ) )
				strcpy_sz( phr->doc, decon.ptr() );

		} // end if

	} // end if

	DWORD p = MAXDWORD;
	for ( DWORD k = 0; p == MAXDWORD && phr->doc[ k ] != 0; k++ )
		if ( phr->doc[ k ] == '?' ) p = k + 1;

	i++;

	// Read params if any
	if ( p != MAXDWORD )
	{	phr->params.ReadInline( (LPBYTE)&phr->doc[ p ], t - p );
		if ( p && phr->doc[ p - 1 ] == '?' ) phr->doc[ p - 1 ] = 0;
	} // end if

	// Save request type
	phr->var.AddVar( "Doc", phr->doc );

	// Crack the url if needed
	CRACKURLINFO	cui;
	if ( CrackUrl( phr->doc, &cui ) )
		strcpy( phr->doc, cui.szUrlPath );

	// Check for type
	if ( !strnicmp( (char*)&buf[ i ], "HTTP", 4 ) ) phr->doctype |= HRDOCTYPE_HTTP;

	// Search for next token
	while ( buf[ i ] != '/' && i < size ) i++;
	if ( i >= size ) return FALSE;
	i++;

	// Read in the version
	char *end;
	phr->ver = ( strtoul( (char*)&buf[ i ], &end, 10 ) ) << 16;
	if ( *end == '.' ) end++;
	phr->ver += strtoul( end, &end, 10 );

	// Find end of line
	while ( i < size && buf[ i ] != 0 &&
			buf[ i ] != '\r' && buf[ i ] != '\n' ) i++;
	if ( i >= size || buf[ i ] == 0 ) return TRUE;

	// Search for end of vars
	BOOL post = FALSE;
	DWORD e = i;

	while ( !post && e < size && buf[ e ] != 0 )
	{
		// Check for end of data
		if ( ( size - e ) > 4 && !strncmp( (char*)&buf[ e ], "\r\n\r\n", 4 ) ) 
			post = TRUE;

		// Increment e
		e++;
	} // end while

	// Read in other variables
	if ( e > i ) phr->var.ReadMIME( &buf[ i ], e - i );

	// Read in post data
	if ( post && phr->type == HRTYPE_POST ) 
	{
		e += 3; 
		DWORD out = CCfgFile::GetMinDeCanonicalizeBufferSize( size - e );
		if ( out > 0 )
		{
			TMem< BYTE > decon;
			if ( decon.allocate( out + 1 ) )
			{
				for ( DWORD k = e; k < size; k++ )
					if ( buf[ k ] == '+' ) buf[ k ] = ' ';

				DWORD sz = size - e;
				CCfgFile::DeCanonicalizeBuffer( (LPCTSTR)&buf[ e ], decon.ptr(), decon.size(), &sz );
				phr->params.ReadInline( decon, sz );
			} // end if

		} // end if
		

	} // end if

	return TRUE;
}

BOOL CHttpSession::ProcessRequest(LPHTTPREQUEST phr)
{
	if ( ( phr->type & ( HRTYPE_POST | HRTYPE_GET | HRTYPE_HEAD ) ) == 0 )
	{
		SendError( HTTPERR_SERVER_ERROR );
		Log( HTTPERR_SERVER_ERROR, phr );
		SendErrorMsg( "Error 500: Invalid Request." );
		return FALSE;
	} // end if

/*	if ( strlen( phr->doc ) < 2 && *phr->doc == '/' )
	{
		strcpy( doc, "C:\\Web\\Default.htm" );
	} // end if
*/

	// Check for administration page
	DWORD	admin = 0;
	CFG().GetValue( "Settings", "WebServerRemote", &admin );
	if ( admin )
	{
		// Get admin page name
		char page[ CWF_STRSIZE ] = "admin";
		if (	!CFG().GetValue( "Settings", "WebServerRemotePage", page, sizeof( page ) ) ||
				*page == 0 )
			strcpy( page, "admin" );

		// Skip '/'
		DWORD i = 0;
		while(	phr->doc[ i ] != 0 && 
				( phr->doc[ i ] == '/' || phr->doc[ i ] == '\\' ) ) i++;

		// Did they ask for the admin page?
		if ( !strcmpi( &phr->doc[ i ], page ) )
		{	FRAME()->SetEvent( 5 );
			return RunAdmin( phr );
		} // end if

	} // end if

	// Verify password
	DWORD	password = 0;
	CFG().GetValue( "Settings", "WebServerHttp", &password );
	if ( password ) { if ( !CheckPassword( phr ) ) return FALSE; }

	DWORD i = 0;
	while ( phr->doc[ i ] != 0 && 
			( phr->doc[ i ] == '/' || phr->doc[ i ] == '\\' ) ) i++;

	// Update chat
	DoChat( phr );

	// Do they want an image?
	LPPUBIMGINFO	ppii = NULL;
	char img[ CWF_STRSIZE ];
	if (	phr->doc[ i ] != 0 &&
			WEBSETTINGS().Get( "ImageMap", &phr->doc[ i ], img, sizeof( img ) ) &&
			*img != 0 )
	{	FRAME()->SetEvent( 5 );
		return SendImage( img, phr );
	} // end if

	// Locate the file
	if ( !LocateFile( phr ) ) 
	{	
		char fname[ CWF_STRSIZE ];
		CWinFile::GetFileNameFromPath( phr->file, fname );

		// If their looking for an index, just send them an image
		if ( strcmpi( fname, "index.htm" ) && strcmpi( fname, "index.html" ) )
		{	FRAME()->SetEvent( 6 );
			SendError( HTTPERR_NOT_FOUND );
			Log( HTTPERR_NOT_FOUND, phr );
			SendErrorMsg( "Error 404: Document not found." );
			return FALSE;
		} // end if

		// Send first image in mapping
		LPREGVALUE prv = NULL;
		CRKey *pRk = WEBSETTINGS().FindKey( "ImageMap" );
		if ( pRk != NULL ) prv = (LPREGVALUE)pRk->GetNext( NULL );
		if ( prv != NULL ) 
		{	FRAME()->SetEvent( 5 );
			return SendImage( prv->pcdata, phr );
		} // end if
		
		// Send the first image
		LPPUBIMGINFO ppii = (LPPUBIMGINFO)IMGLIST().GetNext( NULL );
		if ( ppii != NULL ) 
		{	FRAME()->SetEvent( 5 );
			return SendImage( ppii->name, phr );
		} // end if

		FRAME()->SetEvent( 6 );
		SendError( HTTPERR_NOT_FOUND );
		Log( HTTPERR_NOT_FOUND, phr );
		SendErrorMsg( "Error 404: Document not found." );
		
		return FALSE;

	} // end if

	// Check for cgi
	LPCTSTR pExt = CWinFile::GetExtension( phr->file );
	if ( pExt != NULL )
	{
		HREGKEY				hRk = NULL;
		while (		( hRk = (HREGKEY)CGI().GetNext( hRk ) ) != NULL &&
					hRk->key != NULL )
		{
			CRKey *pRk = hRk->key;
			if ( !strcmpi( pExt, pRk->GetSz( "Ext" ) ) )
			{	FRAME()->SetEvent( 5 );
				return DoCgi( phr, pRk->GetSz( "Exe" ), pRk->GetSz( "Params" ) );	
			} // end if
	
		} // end while

		// Check for HTTP document
		if ( !strcmpi( pExt, "htm" ) || !strcmpi( pExt, "html" ) )
		{	FRAME()->SetEvent( 5 );
			return SendHttpFile( phr );
		} // end if

	} // end if	

	// Send the file
	FRAME()->SetEvent( 5 );

	return SendFile( phr );
}

BOOL CHttpSession::SendContentType(LPCTSTR pExt)
{
	if ( pExt == NULL ) return FALSE;

	char type[ 256 ];

	if ( !CMime::GetContentType( pExt, type ) )
		strcpy( type, "text/html" );
			
	char snd[ 256 ];
	wsprintf( snd, "Content-type: %s\r\n", type );

	return Send( snd );
}

BOOL CHttpSession::SendFile(LPHTTPREQUEST phr)
{
	CWinFile file;
	
	if ( !file.OpenExisting( phr->file, GENERIC_READ ) ) 
	{
		SendError( HTTPERR_NOT_FOUND );
		Log( HTTPERR_NOT_FOUND, phr );
		SendErrorMsg( "Error 404: Document not found." );
		return FALSE;
	} // end if

	// Save file size
	phr->len = file.Size();

	{
		// Send headers
		char msg[ 256 ];

		// Send Ok code
		SendError( HTTPERR_OK );
		Log( HTTPERR_OK, phr );

		// Send header info
		SendHeader();

		// Get last modified time
		FILETIME ft;
		SYSTEMTIME st;
		GetFileTime( file, NULL, NULL, &ft );
		FileTimeToSystemTime( &ft, &st );

		// Send the file date
		CWinTime time( &st );
		char tstr[ 256 ];
		time.GetString( tstr, "%w %b %D %g:%m:%s %Y" );
		wsprintf( msg, "Last-modified: %s\r\n", tstr );
		Send( msg );

		// Send the file size
		wsprintf( msg, "Content-length: %lu\r\n", file.Size() );
		Send( msg );

		// Send the content type
		SendContentType( CWinFile::GetExtension( phr->doc ) );

		// End of the headers
		Send( "\r\n" );

	} // end send headers

	// Is this all they wanted?
	if ( ( phr->type & HRTYPE_HEAD ) != 0 ) return TRUE;

	DWORD read;
//	BYTE buf[ 1024 ];
/*	while ( file.Read( buf, sizeof( buf ), &read ) && read > 0 )
	{
		// Send the data
		Send( buf, read );
	} // end while
*/
	TMem< char > b;
	if ( !b.allocate( file.Size() + 1 ) )
	{	SendError( HTTPERR_SERVER_ERROR );
		Log( HTTPERR_SERVER_ERROR, phr );
		SendErrorMsg( "Error 500: Server is out of memory." );
		return FALSE;
	} // end if

	file.Read( b, file.Size(), &read );
	Send( b, read );

	file.Close();

	return TRUE;
}

BOOL CHttpSession::SendHttpFile(LPHTTPREQUEST phr)
{
	CWinFile file;
	
	if ( !file.OpenExisting( phr->file, GENERIC_READ ) ) 
	{
		SendError( HTTPERR_NOT_FOUND );
		Log( HTTPERR_NOT_FOUND, phr );
		SendErrorMsg( "Error 404: Document not found." );
		return FALSE;
	} // end if

	// Save file size
	phr->len = file.Size();

	TMem< char > in;
	TMem< char > out;

	// Allocate memory
	if ( !in.allocate( file.Size() ) || !out.allocate( ( file.Size() * 2 ) + 256000 ) )
	{	SendError( HTTPERR_SERVER_ERROR );
		Log( HTTPERR_SERVER_ERROR, phr );
		SendErrorMsg( "Error 500: Out Of Memory." );
		return FALSE;
	} // end if

	// Read file data
	if ( !file.Read( in, file.Size() ) )
	{	SendError( HTTPERR_SERVER_ERROR );
		Log( HTTPERR_SERVER_ERROR, phr );
		SendErrorMsg( "Error 500: Error reading file." );
		return FALSE;
	} // end if
	file.Close();

	CVar var;

	// Add break for chat
	var.AddVar( "chat", VAR_VOID, NULL, 0 );
	var.AddVar( "gallery", VAR_VOID, NULL, 0 );

	// Run replace function
	BOOL	bModified = FALSE;
	DWORD	op = 0, i = 0;
	char	token[ 256 ];
	CVar	params;
	while ( var.Replace(	out, &op, out.size(), in, in.size(), 
							"<!--&&", "-->", token, &params, &i ) )
	{
		// Mark as changed
		bModified = TRUE;

		// Check for chat data
		if ( !strcmpi( token, "chat" ) ) 
			op = WriteChat( out, op, out.size(), &params );

		// Check for image gallery
		else if ( !strcmpi( token, "gallery" ) ) 
			op = WriteGallery( out, op, out.size(), &params );

	} // end while

	{
		// Send headers
		char msg[ 256 ];

		// Send Ok code
		SendError( HTTPERR_OK );
		Log( HTTPERR_OK, phr );

		// Send header info
		SendHeader();
		
		// Send the file date
		// Wdy Mon DD HH:MM:SS YYYY
		// Fri Nov 19 16:14:55 1982
		CWinTime time( NULL );
		char tstr[ 256 ];

		if ( bModified ) time.GetSystemTime();
		else
		{	FILETIME ft;
			SYSTEMTIME st;
			GetFileTime( file, NULL, NULL, &ft );
			FileTimeToSystemTime( &ft, &st );
			time.SetTime( &st );
		} // end else

		time.GetString( tstr, "%w %b %D %g:%m:%s %Y" );
		wsprintf( msg, "Last-modified: %s\r\n", tstr );
		Send( msg );

		// Send the file size
		wsprintf( msg, "Content-length: %lu\r\n", op );
		Send( msg );

		// Send the content type
		SendContentType( CWinFile::GetExtension( phr->doc ) );

		// End of the headers
		Send( "\r\n" );

	} // end send headers

	// Is this all they wanted?
	if ( ( phr->type & HRTYPE_HEAD ) != 0 ) return TRUE;

	// Send data
	Send( (LPBYTE)out.ptr(), op );

	return TRUE;
}

BOOL CHttpSession::SendFile(LPCTSTR pFile, LPBYTE buf, DWORD size)
{
	CWinFile file;

	if ( buf == NULL )
	{
		if ( !file.OpenExisting( pFile, GENERIC_READ ) ) 
		{
			SendError( HTTPERR_NOT_FOUND );
			SendErrorMsg( "Error 404: Document not found." );
			return FALSE;
		} // end if

		// Save file size
		size = file.Size();

	} // end if

	{
		// Send headers
		char msg[ 256 ];

		// Send Ok code
		SendError( HTTPERR_OK );

		// Send header info
		SendHeader();

		// Send the file date
		CWinTime time( NULL );
		char tstr[ 256 ];
		time.GetSystemTime();
		time.GetString( tstr, "%w %b %D %g:%m:%s %Y" );
		wsprintf( msg, "Last-modified: %s\r\n", tstr );
		Send( msg );

		// Send the file size
		wsprintf( msg, "Content-length: %lu\r\n", size );
		Send( msg );

		// Send the content type
		SendContentType( CWinFile::GetExtension( pFile ) );

		// End of the headers
		Send( "\r\n" );

	} // end send headers

	// Was data supplied?
	if ( buf != NULL )
		Send( buf, size );

	else
	{
		DWORD read;
		TMem< char > b;

		if ( !b.allocate( size + 1 ) )
		{	SendError( HTTPERR_SERVER_ERROR );
			SendErrorMsg( "Error 500: Server is out of memory." );
			return FALSE;
		} // end if
		
		file.Read( b, size, &read );
		Send( b, read );

		file.Close();
	} // end if

	return TRUE;
}


BOOL CHttpSession::SendImage(LPCTSTR pImg, LPHTTPREQUEST phr)
{
	// Get a pointer to the current image
	LPPUBIMGINFO ppii = IMGLIST().FindByName( pImg );
	if ( ppii == NULL || ppii->os == NULL || ppii->os->GetSafeHdc() == NULL )
	{	SendError( HTTPERR_SERVER_ERROR );
		Log( HTTPERR_SERVER_ERROR, phr );
		SendErrorMsg( "Error 500: Invalid Image Object." );
		return FALSE;
	} // end if

	// Refresh the image
	IMGLIST().Update( ppii, TRUE );

	LPBYTE buf = NULL;
	DWORD size = 0;

	// Create the image object
	if ( !m_img.CreateFromHBITMAP( *ppii->os ) )
	{	SendError( HTTPERR_SERVER_ERROR );
		Log( HTTPERR_SERVER_ERROR, phr );
		SendErrorMsg( "Error 500: Unable to create image." );
		return FALSE;
	} // end if
	
	// Set image quality
	FRAME()->SetQuality( &m_img );

	// Encode the image
	BOOL ret = FALSE;
	DWORD itype = m_img.GetFileType( phr->doc );
	if ( itype != MAXDWORD ) ret = m_img.Encode( &buf, &size, phr->doc );
	else ret = m_img.Encode( &buf, &size, "jpg" );
	if ( !ret )
	{	SendError( HTTPERR_SERVER_ERROR );
		Log( HTTPERR_SERVER_ERROR, phr );
		SendErrorMsg( "Error 500: Error encoding image." );
		return FALSE;
	} // end if

	// Save file size
	phr->len = size;

	{
		// Send headers
		char msg[ 256 ];

		// Send Ok code
		SendError( HTTPERR_OK );
		Log( HTTPERR_OK, phr );

		// Send header info
		SendHeader();

		// Send the file date
		Send( "Last-modified: \r\n" );

		// Send the file size
		wsprintf( msg, "Content-length: %lu\r\n", size );
		Send( msg );

		// Send the content type
		if ( itype != MAXDWORD ) SendContentType( CWinFile::GetExtension( phr->doc ) );
		else SendContentType( CWinFile::GetExtension( "file.jpg" ) );

		// End of the headers
		Send( "\r\n" );

	} // end send headers

	// Is this all they wanted?
	if ( ( phr->type & HRTYPE_HEAD ) != 0 ) return TRUE;

	// Send the image
	Send( buf, size );

	return TRUE;
}


BOOL CHttpSession::SendError(DWORD err)
{
	Send( (LPVOID)GetErrString( err ) );

	// Notify parent window
	if ( m_hWnd != NULL )
		PostMessage( m_hWnd, m_uMsg, (WPARAM)HTTPMSG_ERROR, (LPARAM)err );

//	Send( "\r\n" );

	return TRUE;
}

BOOL CHttpSession::SendHeader()
{
	char		msg[ 256 ];
	char		str[ 256 ];
	time_t		t;

	// Send server information
	Send( "Server: WheresJames Webcam Server " VERSTR "\r\n" );

	// Send time string
	time( &t );
	strftime( str, 256, "%A, %d-%b-%y %H:%M:%S GMT", gmtime( &t ) );	
	wsprintf( msg, "Date: %s\r\n", str );
	Send( msg );

	return TRUE;
}

LPCTSTR CHttpSession::GetErrString(DWORD err)
{
	switch( err )
	{
		case HTTPERR_OK :
			return "HTTP/1.0 200 OK\r\n";
			break;

		case HTTPERR_FORBIDDEN :
			return "HTTP/1.0 403 Forbidden\r\n";
			break;
		
		case HTTPERR_NOT_FOUND :
			return "HTTP/1.0 404 Document Not Found\r\n";
			break;

		case HTTPERR_SERVER_ERROR :
		default :
			return "HTTP/1.0 500 Server Error\r\n";
			break;
	}

	return "HTTP/1.0 500 Server Error\r\n";
}



BOOL CHttpSession::Log(DWORD status, LPHTTPREQUEST phr)
{

	try
	{
		DWORD type = LOGTYPE_INFO;
		if ( status == HTTPERR_FORBIDDEN ) type = LOGTYPE_ERROR;
		else if ( status == HTTPERR_NOT_FOUND ) type = LOGTYPE_ERROR;
		else if ( status == HTTPERR_SERVER_ERROR ) type = LOGTYPE_ERROR;

		// Get ip address
		DWORD ip = 0, port = 0;
		GetConnectionInfo( &ip, &port );

		// Get the username
		char username[ 256 ] = { '-', 0 };
		if ( *m_szUserName != 0 ) strcpy( username, m_szUserName );

		char timestr[ 256 ];
		{ // Get time string

			SYSTEMTIME st;
			GetLocalTime( &st );

			// Get time-zone information
			TIME_ZONE_INFORMATION tzi;
			ZeroMemory( &tzi, sizeof( tzi ) );
			GetTimeZoneInformation( &tzi );

			// Change to correct format
			long hour = tzi.Bias / 60;
			if ( tzi.Bias < 0 ) tzi.Bias = -tzi.Bias;
			long min = tzi.Bias % 60;
			if ( hour <= 12 ) hour = -hour;
			else if ( hour > 12 ) hour = 24 - hour;

			char tzone[ MIME_STRSIZE ] = { 0 };
			if ( hour < 0 ) wsprintf( tzone, "%03li%02lu", hour, min );
			else wsprintf( tzone, "+%02li%02lu", hour, min );

			// Create time string
			time_t t;
			time( &t );
			strftime( timestr, 256, "%d/%b/%Y:%H:%M:%S ", gmtime( &t ) );	
			strcat( timestr, tzone );
		
		}

		char referer[ 1024 ] = { '-', 0 };
		phr->var.GetVar( "Referer", referer, sizeof( referer ) );

		char useragent[ 1024 ] = { '-', 0 };
		phr->var.GetVar( "User-Agent", useragent, sizeof( useragent ) );
		
		// Add to log
		SERVERLOG().Log( type,	"%lu.%lu.%lu.%lu - %s [%s] \"%s\" %lu %lu \"%s\" \"%s\"",
								(DWORD)ip & 0x000000ff,
								(DWORD)( ip & 0x0000ff00 ) >> 8,
								(DWORD)( ip & 0x00ff0000 ) >> 16,
								(DWORD)( ip & 0xff000000 ) >> 24,
								username,
								timestr,
								phr->doc,
								status,
								phr->len,
								referer,
								useragent );
	} // end try
	
	catch( ... )
	{	ASSERT( 0 );
		return FALSE;
	} // end catch

	return TRUE;
}

void CHttpSession::ZeroHTTPREQUEST(LPHTTPREQUEST phr)
{
	if ( phr == NULL ) return;
	
	phr->type = 0;
	*phr->doc = 0;
	phr->doctype = 0;
	phr->ver = 0;
	phr->len = 0;
	phr->var.Destroy();
}

BOOL CHttpSession::TranslatePath(LPCTSTR pReq, LPSTR pFile)
{
	if ( pReq == NULL || pFile == NULL ) return FALSE;

	DWORD i = 0;
	char doc[ CWF_STRSIZE ];

	// Remove foreward \/
	while ( pReq[ i ] != 0 && ( pReq[ i ] == '/' || pReq[ i ] == '\\' ) ) i++;
	strcpy_sz( doc, &pReq[ i ] );

	// Remove trailing \/
	i = strlen( doc ); if ( i ) i--;
	while ( i && ( doc[ i ] == '/' || doc[ i ]== '\\' ) ) i--;
	if ( doc[ i ] == '/' || doc[ i ]== '\\' ) doc[ i ] = 0;


	// Check for root
//	if ( *path == 0 ) strcpy( path, "" );

	CRKey *pRk = WEBSETTINGS().GetKey( "FolderMap" );
	if ( pRk == NULL ) return FALSE;

	char path[ CWF_STRSIZE ];
	LPREGVALUE	prv = NULL;
	while ( ( prv = (LPREGVALUE)pRk->GetNext( prv ) ) != NULL )
	{
		DWORD i = 0;
		while ( CWinFile::GetRootPath( i, doc, path ) )
		{
			if ( !strnicmp( path, prv->cpkey, strlen( path ) ) )
			{
				CWinFile::GetSubPath( i, doc, path );
				CWinFile::BuildPath( pFile, pRk->GetSz( prv ), path );
				return TRUE;
			} // end if		

			// Next root
			i++;

		} // end while

	} // end while
		
	// Just use the root path
	if ( WEBSETTINGS().Get( "FolderMap", "", path, sizeof( path ) ) )
	{	CWinFile::BuildPath( path, path, doc );
		strcpy( pFile, path );
		return TRUE;
	} // end if

/*
	char fname[ CWF_STRSIZE ];
	char path[ CWF_STRSIZE ];
	CWinFile::GetFileNameFromPath( doc, fname );
	CWinFile::GetPathFromFileName( doc, path );

	char folder[ CWF_STRSIZE ];
	if ( WEBSETTINGS().GetValue( "FolderMap", path, folder, sizeof( folder ) ) )
	{	CWinFile::BuildPath( pFile, folder, fname );
		return TRUE;
	} // end if

	if ( WEBSETTINGS().GetValue( "FolderMap", doc, folder, sizeof( folder ) ) )
	{	strcpy( pFile, folder );
		return TRUE;
	} // end if

	// Just use the root path
	if ( WEBSETTINGS().GetValue( "FolderMap", "", folder, sizeof( folder ) ) )
	{	CWinFile::BuildPath( folder, folder, doc );
		strcpy( pFile, folder );
		return TRUE;
	} // end if
*/

	return FALSE;

}


BOOL CHttpSession::SendErrorMsg(LPCTSTR pMsg)
{

	// Send an error document
	char msg[ 256 ];
	char errdoc[ 1024 ];
	strcpy( errdoc, "<HTML> <BODY><p><big><big>" );
	strcat( errdoc, pMsg );
	strcat( errdoc, "</big></big></p> </BODY></HTML>" );
	
	SendHeader();

	Send( "Last-modified: \r\n" );
	wsprintf( msg, "Content-length: %lu\r\n", strlen( errdoc ) );
	Send( msg );
	SendContentType( "htm" );
	Send( "\r\n" );
	Send( errdoc );

	return TRUE;
}


BOOL CHttpSession::RunAdmin( LPHTTPREQUEST phr )
{
	// Make modifications if needed
	char input[ 256 ] = { 0 }, password[ 256 ] = { 0 };	
	CFG().GetValue( "Settings", "WebServerRemotePassword", password, sizeof( password ) );

	if (	*password != 0 &&
			phr->params.GetVar( "restart", input, sizeof( input ) ) &&
			!strcmp( input, password ) )
	{
		APP()->Restart();
	} // end if			

	if (	*password != 0 &&
			phr->params.GetVar( "reboot", input, sizeof( input ) ) &&
			!strcmp( input, password ) )
	{
		// Reboot the computer
		CWin32::Reboot();
	} // end if			


	if ( *password != 0 && phr->params.GetVar( "password", input, sizeof( input ) ) )
	{
		if ( strcmp( input, password ) )
		{	SendError( HTTPERR_SERVER_ERROR );
			SendErrorMsg( "Incorrect Password" );
			return FALSE;	
		} // end if

		char str[ 1024 ];

		// guest_access
		if ( phr->params.GetVar( "guest_access", str, sizeof( str ) ) )
			CFG().SetValue( "Settings", "WebServerHttpEnable", 
							(DWORD)( !strcmpi( str, "on" ) ? 1 : 0 ) );

		// require_password
		if ( phr->params.GetVar( "require_password", str, sizeof( str ) ) )
			CFG().SetValue( "Settings", "WebServerHttp", 
							(DWORD)( !strcmpi( str, "on" ) ? 1 : 0 ) );

		// tcp_port
		if ( phr->params.GetVar( "tcp_port", str, sizeof( str ) ) )
		{	DWORD dw = strtoul( str, NULL, 10 );
			CFG().SetValue( "Settings", "WebServerPort", dw );
		} // end if

		// max_users
		if ( phr->params.GetVar( "max_users", str, sizeof( str ) ) )
		{	DWORD dw = strtoul( str, NULL, 10 );
			CFG().SetValue( "Settings", "WebServerMaxUsers", dw );
		} // end if
	
	} // end if
	
	
	char adminfile[ CWF_STRSIZE ];
	CWinFile::BuildPath( adminfile, CWinFile::GetExePath( adminfile ), "web\\admin.htm" );

	if ( !Extract( MAKEINTRESOURCE( IDH_ADMIN ), adminfile ) )
	{	SendError( HTTPERR_NOT_FOUND );
		Log( HTTPERR_NOT_FOUND, phr );
		SendErrorMsg( "Error 404: Error extracting administration page." );
		return FALSE;
	} // end if

	CWinFile	file;
	if ( !file.OpenExisting( adminfile, GENERIC_READ ) )
	{	SendError( HTTPERR_NOT_FOUND );
		Log( HTTPERR_NOT_FOUND, phr );
		SendErrorMsg( "Error 404: Administration page not found." );
		return FALSE;
	} // end if

	TMem< char > in;
	TMem< char > out;

	// Allocate memory
	if ( !in.allocate( file.Size() ) || !out.allocate( file.Size() * 2 ) )
	{	SendError( HTTPERR_SERVER_ERROR );
		Log( HTTPERR_SERVER_ERROR, phr );
		SendErrorMsg( "Error 500: Out Of Memory." );
		return FALSE;
	} // end if

	// Read file data
	if ( !file.Read( in, file.Size() ) )
	{	SendError( HTTPERR_SERVER_ERROR );
		Log( HTTPERR_SERVER_ERROR, phr );
		SendErrorMsg( "Error 500: Error reading file." );
		return FALSE;
	} // end if

	file.Close();

	CVar var;

	{ // formaction
		// Get admin page name
		char page[ CWF_STRSIZE ] = "admin";
		if (	!CFG().GetValue( "Settings", "WebServerRemotePage", page, sizeof( page ) ) ||
				*page == 0 )
			strcpy( page, "admin" );		

		var.AddVar( "formaction", page );
	} // end


	{ // guest_access
		DWORD	dw = 0;
		CFG().GetValue( "Settings", "WebServerHttpEnable", &dw );
		var.AddVar( "guest_access_state", dw ? "checked" : "" );
	} // end

	{ // require_password_state
		DWORD	password = 0;
		CFG().GetValue( "Settings", "WebServerHttp", &password );
		var.AddVar( "require_password_state", password ? "checked" : "" );
	} // end

	{ // users_online
		char num[ 256 ];
		wsprintf( num, "%lu", (DWORD)WEBUSERS().Size() );
		var.AddVar( "users_online", num );
	}

	{ // total_hits
		char num[ 256 ];
		wsprintf( num, "%lu", (DWORD)FRAME()->GetTotalHits() );
		var.AddVar( "total_hits", num );
	}

	{ // tcp_port
		char num[ 256 ];
		DWORD dw = 80;
		CFG().GetValue( "Settings", "WebServerPort", &dw );
		wsprintf( num, "%lu", dw );
		var.AddVar( "tcp_port", num );
	}

	{ // max_users
		char num[ 256 ];
		DWORD dw = 80;
		CFG().GetValue( "Settings", "WebServerMaxUsers", &dw );
		wsprintf( num, "%lu", dw );
		var.AddVar( "max_users", num );
	}

	// Run replace function
	DWORD	op = 0, i = 0;
	char	token[ 256 ];
	while ( var.Replace(	out, &op, out.size(), in, in.size(), 
							NULL, NULL, token, NULL, &i ) )
	{

	} // end while

	// Send data
	SendFile( "admin.htm", (LPBYTE)out.ptr(), op );

	return TRUE;
}

BOOL CHttpSession::Extract(LPCTSTR pResource, LPCTSTR pFile)
{
	// Punt if file exists
	if ( CWinFile::DoesExist( pFile ) ) return TRUE;

	// Ensure directory exists
	char dir[ CWF_STRSIZE ];
	CWinFile::GetPathFromFileName( pFile, dir );
	CWinFile::CreateDirectory( dir );

	return CWinFile::ExtractResource( pFile, pResource, RT_HTML );
}

BOOL CHttpSession::CheckPassword( LPHTTPREQUEST phr )
{
	// Expire users
	APP()->m_authusers.Expire( 60000 * 5 );

	// Is user logged on?
	if ( APP()->m_authusers.GetUserAccess( phr->ip, TRUE ) ) return TRUE;

	char pwdfile[ CWF_STRSIZE ];
	CWinFile::BuildPath( pwdfile, CWinFile::GetExePath( pwdfile ), "web\\pwd.htm" );

	if ( !Extract( MAKEINTRESOURCE( IDH_PWD ), pwdfile ) )
	{	SendError( HTTPERR_NOT_FOUND );
		Log( HTTPERR_NOT_FOUND, phr );
		SendErrorMsg( "Error 404: Error extracting administration page." );
		return FALSE;
	} // end if

	DWORD i = 0;
	while(	phr->doc[ i ] != 0 && 
			( phr->doc[ i ] == '/' || phr->doc[ i ] == '\\' ) ) i++;

	if ( !strcmpi( &phr->doc[ i ], "login.act" ) )
	{
		char input[ 256 ] = { 0 }, password[ 256 ] = { 0 };

		// Get the password
		CFG().GetValue( "Settings", "WebServerHttpPassword", password, sizeof( password ) );

		// Did the user get the password correct?
		if (	phr->params.GetVar( "password", input, sizeof( input ) ) &&
				!strcmp( input, password ) )
		{
			// Add user
			APP()->m_authusers.Add( phr->ip );

			// Point to root document
			strcpy( phr->doc, "/" );

			return TRUE;

		} // end if

	} // end if

	// Send the password file
	SendFile( pwdfile );

	return FALSE;
}

BOOL CHttpSession::DoCgi(LPHTTPREQUEST phr, LPCTSTR pCgi, LPCTSTR pParams)
{
	// Sanity checks
	if ( phr == NULL || pCgi == NULL || !CWinFile::DoesExist( pCgi ) ) 
		return FALSE;

	char fname[ CWF_STRSIZE ] = { 0 };
	CWinFile::GetFileNameFromPath( phr->doc, fname );
	if ( *fname == 0 ) CWinFile::GetFileNameFromPath( pCgi, fname );
	if ( *fname == 0 ) strcpy( fname, "WheresJames CGI" );

	DWORD csz = CWF_STRSIZE + ( ( pParams != NULL ) ? strlen( pParams ) : 0 );
	char *cmdline = new char[ csz ];
	if ( cmdline != NULL )
	{	strcpy( cmdline, pCgi );
		strcat( cmdline, " " );
		strcat( cmdline, phr->file );
		if ( pParams != NULL && *pParams != 0 ) 
			strcat( cmdline, " " ), strcat( cmdline, pParams );
	} // end if

	char dir[ CWF_STRSIZE ];
	CWinFile::GetPathFromFileName( pCgi, dir );

	PROCESS_INFORMATION pi;
	ZeroMemory( &pi, sizeof( pi ) );

	// Create security structure
	SECURITY_ATTRIBUTES sa; 
	ZeroMemory( &sa, sizeof( sa ) );
	sa.nLength = sizeof( sa ); 
	sa.bInheritHandle = TRUE; 
	sa.lpSecurityDescriptor = NULL; 

	// Create pipes
	HANDLE hStdInputRd, hStdInputWr;
	HANDLE hStdOutputRd, hStdOutputWr;
	HANDLE hStdErrorRd, hStdErrorWr;
	CreatePipe( &hStdInputRd, &hStdInputWr, &sa, 0 );
	CreatePipe( &hStdOutputRd, &hStdOutputWr, &sa, 0 );
	CreatePipe( &hStdErrorRd, &hStdErrorWr, &sa, 0 );

	STARTUPINFO si;
	ZeroMemory( &si, sizeof( si ) );

	si.cb = sizeof( si );
	si.lpTitle = fname;
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_FORCEOFFFEEDBACK;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = hStdInputRd;
	si.hStdOutput = hStdOutputWr;
	si.hStdError = hStdErrorWr;

	// Create the cgi process
	BOOL res = CreateProcess(	pCgi, cmdline, NULL, NULL, TRUE, 
								DETACHED_PROCESS | NORMAL_PRIORITY_CLASS, 
								NULL, dir, 
								&si, &pi );

	if ( res == 0 )
	{
		char show[ CWF_STRSIZE ];
		wsprintf( show, "Error 500 : CGI Error : %lu", GetLastError() );

		SendError( HTTPERR_SERVER_ERROR );
		Log( HTTPERR_SERVER_ERROR, phr );
		SendErrorMsg( show );
		return FALSE;
	} // end if

	// While the process is running
	DWORD dwTotalBytes = 0;
	BOOL error = FALSE;
	BYTE buf[ CWF_STRSIZE ];
	DWORD dwTimeout = GetTickCount() + 30000;
	while ( !error && dwTimeout > GetTickCount() &&
			WaitForSingleObject( pi.hProcess, 0 ) == WAIT_TIMEOUT )
	{
//		if ( WaitForSingleObject( hStdOutputRd, 100 ) != WAIT_TIMEOUT )
		{
			DWORD read = 0;

			// Read data from the process
			if ( !ReadFile( hStdOutputRd, buf, CWF_STRSIZE, &read, NULL ) ) error = TRUE;

			// Did we get anything?
			if ( read == 0 ) error = TRUE;

			// Pass it on
			Send( buf, read );

			// Tally bytes
			dwTotalBytes += read;

		} // end if

		Sleep( 15 );

	} // end while

	// Force close the thing if needed
	BOOL bTerminated = FALSE;
	if ( WaitForSingleObject( pi.hProcess, 0 ) == WAIT_TIMEOUT )
	{	TerminateProcess( pi.hProcess, 0 ); bTerminated = TRUE; }

	if ( dwTotalBytes == 0 )
	{	SendError( HTTPERR_SERVER_ERROR );
		Log( HTTPERR_SERVER_ERROR, phr );
		SendErrorMsg( "Error 500 : CGI Failed to produce any data" );
		return FALSE;
	} // end if

	else if ( bTerminated )
	{	SendError( HTTPERR_SERVER_ERROR );
		Log( HTTPERR_SERVER_ERROR, phr );
		SendErrorMsg( "Error 500 : CGI Timed out" );
		return FALSE;
	} // end if

	return TRUE;
}

BOOL CHttpSession::LocateFile(LPHTTPREQUEST phr)
{
	// Build default document name
	if ( *phr->doc != 0 ) 
	{
		if (	phr->doc[ strlen( phr->doc ) - 1 ] == '\\' ||
				phr->doc[ strlen( phr->doc ) - 1 ] == '/' )
				CWinFile::BuildPath( phr->file, phr->doc, "index.htm" );
	
		// Copy the document name
		else strcpy( phr->file, phr->doc );

	} // end if

	// Translate the path
	if ( !TranslatePath( phr->file, phr->file ) )
	{
		SendError( HTTPERR_NOT_FOUND );
		Log( HTTPERR_NOT_FOUND, phr );
		SendErrorMsg( "Error 404: Document not found." );
		return FALSE;
	} // end if

	// Convert for Windows 95
	for ( DWORD i = 0; i < sizeof( phr->file ) && phr->file[ i ] != 0; i++ )
			if ( phr->file[ i ] == '/' ) phr->file[ i ] = '\\';

	// Send error
	DWORD attrib = ::GetFileAttributes( phr->file );
	if ( attrib == MAXDWORD ) 
	{
		return FALSE;
	} // end if

	// Check for directory
	else if ( ( attrib & FILE_ATTRIBUTE_DIRECTORY ) != 0 )
		CWinFile::BuildPath( phr->file, phr->file, "index.htm" );

	return TRUE;
}


BOOL CHttpSession::DoChat(LPHTTPREQUEST phr)
{
	// Do we want chat?
	BOOL bEnableChat = CFG().GetDword( "Settings", "EnableChat", 1 ) != 0;
	if ( !bEnableChat ) return FALSE;

	// Chat params
	BOOL bShowAddr = CFG().GetDword( "Settings", "ChatShowAddr", 1 ) != 0;
	BOOL bShowUsername = CFG().GetDword( "Settings", "ChatShowUsername", 1 ) != 0;
	BOOL bAllowHtml = CFG().GetDword( "Settings", "ChatAllowHtml", 0 ) != 0;
	DWORD dwMaxLines = CFG().GetDword( "Settings", "ChatMaxLines", 8 );
	DWORD dwMaxMsgLen = CFG().GetDword( "Settings", "ChatMaxMsgLen", 80 );
	if ( dwMaxMsgLen > 1000 ) dwMaxMsgLen = 1000;
	if ( dwMaxLines > 100000 ) dwMaxLines = 100000;

	// Check for chat message
	char msg[ 4096 ] = { 0 };
	{
		// Get the raw text message
		char temp[ 1024 ];
		if ( !phr->params.GetVar( "chatmsg", temp, sizeof( temp ) ) || *temp == 0 )
			return FALSE;
		temp[ dwMaxMsgLen ] = 0;

		// User is living on the edge
		if ( bAllowHtml ) strcpy( msg, temp );
		
		// Convert characters
		else CText::HtmlText( msg, sizeof( msg ), temp, strlen( temp ), TRUE );
	}

	// Get chat name
	char name[ 256 ] = { 0 };
	if ( bShowUsername )
	{	if ( !phr->params.GetVar( "chatname", name, sizeof( name ) ) )
			*name = 0;
		name[ 13 ] = 0;
	} // end if

	// Get ip address
	char addr[ 256 ] = { 0 };
	if ( bShowAddr )
	{	DWORD ip = 0, port = 0;
		GetConnectionInfo( &ip, &port );
		wsprintf( addr, "%lu.%lu.%lu.%lu",	(DWORD)ip & 0x000000ff,
											(DWORD)( ip & 0x0000ff00 ) >> 8,
											(DWORD)( ip & 0x00ff0000 ) >> 16,
											(DWORD)( ip & 0xff000000 ) >> 24 );
	} // end if

	// Build string
	char show[ 1024 ] = { 0 };
	
	if ( *name && *addr ) 
		wsprintf( show, "<b>%s</b> <font size=\"2\">(%s)</font><b>:</b> %s <br>\r\n", name, addr, msg );
	else if ( *name ) wsprintf( show, "<b>%s:</b> %s <br>\r\n", name, msg );
	else if ( *addr ) wsprintf( show, "<b>%s:</b> %s <br>\r\n", addr, msg );
	else wsprintf( show, "%s <br>\r\n", addr, msg );

	// Get chat group
	char group[ 256 ] = { 0 };
	if ( !phr->params.GetVar( "chatgroup", group, sizeof( group ) ) )
		*group = 0;
	
	CRKey *pRk = CHAT().GetKey( group );
	if ( pRk != NULL ) 
	{
		CWinTime	time;
		char		tstr[ CWF_STRSIZE ];
		time.GetString( tstr, "%c.%d.%y.%h.%m.%s" );

		// Only keep the last dwMaxLines
		while ( pRk->Size() > dwMaxLines ) pRk->Delete( pRk->GetNext( NULL ) );

		// Add the chat string
		pRk->Set( tstr, show );

	} // end if	

	return TRUE;
}


DWORD CHttpSession::WriteChat( LPSTR out, DWORD op, DWORD omax, CVar *params)
{
	CString str( "No Messages<br>\r\n" );

	// Get the group name
	char group[ 256 ] = { 0 };
	if ( params == NULL || !params->GetVar( "chatgroup", group, sizeof( group ) ) )
		*group = 0;

	// Build chat string if we can
	CRKey *pRk = CHAT().FindKey( group );
	if ( pRk == NULL ) return op;

	str.Empty();

	// Build chat dialog
	LPREGVALUE prv = NULL;
	while ( ( prv = (LPREGVALUE)pRk->GetNext( prv ) ) != NULL )
		str += pRk->GetSz( prv );

	// Write out the chat string
	return CText::WriteBuffer( out, op, omax, str, strlen( str ) );
}

DWORD CHttpSession::WriteGallery(LPSTR out, DWORD op, DWORD omax, CVar *params)
{
	// Get the group name
	DWORD index = MAXDWORD;
	if ( params == NULL || !params->GetVar( "index", &index ) )
		index = MAXDWORD;
	
	// Get the image name
	char image[ 256 ] = { 0 };
	if ( params == NULL || !params->GetVar( "image", image, sizeof( image ) ) )
		*image = 0;

	BOOL bThumbnail = ( params->FindVar( "thumbnail" ) != 0 );

	LPPUBINFO ppi = NULL;
	while ( ( ppi = (LPPUBINFO)PUBLIST().GetNext( ppi ) ) != NULL )
	{
		// Check for gallery requirements
		if (	ppi->type == PUBTYPE_DISK && 
				( ( ppi->f1 & PUBF1_ROTATEFNAME ) != 0 || ( ppi->f1 & PUBF1_TNROTATE ) != 0 ) &&
				( ppi->f1 & PUBF1_TIMESTAMP ) == 0 &&
				( ppi->f1 & PUBF1_TNTIMESTAMP ) == 0 &&
				( *image == 0 || !strcmpi( image, ppi->img ) ) )
		{
			// Create a copy of the pub info
			DWORD curindex = ppi->curindex;
			char fname[ CWF_STRSIZE ];
			char tfname[ CWF_STRSIZE ];
			
			// Thumbnail available?
			if ( 	( ppi->f1 & PUBF1_THUMBNAIL ) == 0 ||
					( ppi->f1 & PUBF1_TNROTATE ) == 0 ) bThumbnail = FALSE;

			DWORD width = bThumbnail ? 4 : 2;
			if ( params == NULL || !params->GetVar( "width", &width ) )
				width = bThumbnail ? 4 : 2;


			// Do they want a particular index
			if ( index < 1024 && index < ( ppi->stopindex - ppi->startindex ) )
			{
				// Count backward
				do 
				{	if ( curindex > ppi->startindex ) curindex--; 
					else curindex = ppi->stopindex;
					if ( index ) index--;
				} while( index );

				// Create filenames
				CPubThread::GetFileName( ppi, TRUE, fname, tfname, curindex );

				// Write out the file name
				if ( bThumbnail && ( ppi->f1 & PUBF1_TNROTATE ) != 0 )
					strcpy( &out[ op ], tfname ), op += strlen( tfname );
				else strcpy( &out[ op ], fname ), op += strlen( fname );

				return op;

			} // end if	
			
			// Check for index error
			else if ( index != MAXDWORD ) return op;
			
			// Write out the whole gallery
			else
			{
				DWORD max = 64;
				DWORD w = 0;
				DWORD orgindex = curindex;

				// Start table row
				op = CText::WriteBuffer( out, op, omax, "\r\n<tr>\r\n", 8 );

				do 
				{
					// Break row if needed
					if ( w >= width )
					{	w = 1;
						op = CText::WriteBuffer( out, op, omax, "</tr><tr>\r\n", 11 );
					} // end if
					else w++;

					// Get next index
					if ( curindex > ppi->startindex ) curindex--; 
					else curindex = ppi->stopindex;

					// Create filenames
					CPubThread::GetFileName( ppi, TRUE, fname, tfname, curindex );
					
					char show[ 4096 ] = { 0 };
					if ( bThumbnail && ( ppi->f1 & PUBF1_TNROTATE ) != 0 )
						wsprintf( show, "<td><img src=\"%s\"></td>\r\n", tfname );
					else wsprintf( show, "<td><img src=\"%s\"></td>\r\n", fname );

					// Write data
					op = CText::WriteBuffer( out, op, omax, show, strlen( show ) );

					// Be reasonable
					max--;

				} while( max && curindex != orgindex );

				// End the table row
				op = CText::WriteBuffer( out, op, omax, "</tr>\r\n", 8 );
			
				return op;

			} // end else

		} // end if

	} // end while

	return op;
}
