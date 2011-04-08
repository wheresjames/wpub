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
// PubThread.cpp: implementation of the CPubThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vp.h"
#include "PubThread.h"
#include "MainFrm.h"

#include "Ftp.h"
#include "EmailThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPubThread::CPubThread()
{
	m_bReset = TRUE;
	m_dwMotionFrame = 0;
}

CPubThread::~CPubThread()
{

}

BOOL CPubThread::InitThread(LPVOID pData)
{
	return TRUE;
}

BOOL CPubThread::DoThread(LPVOID pData)
{
	DWORD enablepublishing = 0;
	CFG().GetValue( "Settings", "EnablePublishing", &enablepublishing );

	// Punt if publishing is disabled
	if ( !enablepublishing )
	{	m_bReset = TRUE;
		Sleep( 1000 ); return TRUE;
	} // end if

	DWORD tickcount = GetTickCount();
	
	SYSTEMTIME	st;
	GetSystemTime( &st );

	// Calculate seconds offset
	DWORD seconds = ( st.wHour * 60 * 60 ) + ( st.wMinute * 60 ) + st.wSecond;

	// Process each job
	LPPUBINFO ppi = NULL;
	while ( ( ppi = (LPPUBINFO)PUBLIST().GetNext( ppi ) ) != NULL )
	{
		try
		{
			// Is publishing on hold?
			if ( ppi->bHold ) continue;

			// Image information
			LPPUBIMGINFO ppii = NULL;

			// Are we doing any avi capturing?
			if ( ( ppi->f1 & ( PUBF1_AVI | PUBF1_THMAVI ) ) != 0 )
			{
				// Update AVI's
				if ( ( ppi->f1 & PUBF1_AVICAPMOTION ) == 0 || IsMotion( ppi ) )
				{
					// Check for avi
					if ( ( ppi->f1 & PUBF1_AVI ) != 0 )
					{
						// Time to capture?
						if (	( ppi->f1 & PUBF1_AVICAPMOTION ) != 0 || 
								ppi->avitimeout < tickcount )
						{
							ppii = IMGLIST().FindByName( ppi->img );
							if ( ppii != NULL )
							{
								// Refresh the image
								IMGLIST().Update( ppii, TRUE );

								// Wait for next frame
								if ( ppi->capframes < 1 ) ppi->capframes = 1;
								if ( ppi->capseconds < 1 ) ppi->capseconds = 1;
								long delay = ( ppi->capseconds * 1000 ) / ppi->capframes;
								ppi->avitimeout = tickcount + delay;

								// Write out a frame of the avi
								WriteAviFrame( ppi, ppi->avi, ppii, ppi->pub_fname );
							
							} // end if

						} // end if

					} // end if

					// Check for thumbnail avi
					if ( ( ppi->f1 & PUBF1_THMAVI ) != 0 )
					{
						// Time to capture?
						if (	( ppi->f1 & PUBF1_AVICAPMOTION ) != 0 || 
								ppi->thmavitimeout < tickcount )
						{
							// Get image if we don't already have it
							if ( ppii == NULL )
							{	ppii = IMGLIST().FindByName( ppi->img );
								IMGLIST().Update( ppii, TRUE );
							} // end if

							if ( ppii != NULL )
							{
								// Wait for next frame
								if ( ppi->capframes < 1 ) ppi->capframes = 1;
								if ( ppi->capseconds < 1 ) ppi->capseconds = 1;
								long delay = ( ppi->capseconds * 1000 ) / ppi->capframes;
								ppi->thmavitimeout = tickcount + delay;

								// Write out a frame of the avi
								WriteAviFrame( ppi, ppi->thmavi, ppii, ppi->pub_tfname );

							} // end if

						} // end if

					} // end if

				} // end if

			} // end if

			// Are we detecting motion?
			if ( ( ppi->f1 & PUBF1_MOTION ) != 0 )
			{
				if ( IsMotion( ppi ) )
				{
					// Save motion time
					if ( ppi->motioninterval == 0 ) ppi->motioninterval = 30;
					ppi->nextmotion = GetTickCount() + ( ppi->motioninterval * 1000 );

					// Get current file name
					GetFileName( ppi );

					// Refresh the image
					if ( ppii == NULL ) IMGLIST().Update( ppi->img, TRUE );

					// Handle avi
					if ( ( ppi->f1 & PUBF1_AVI ) != 0 )
					{
						if ( ppi->avi->IsOpen() )
						{
							// Save avi filename
							strcpy( ppi->avicachefile, ppi->avi->GetFileName() );
							ppi->avi->Close();

							// Save thumbnail avi filename
							strcpy( ppi->thmavicachefile, ppi->thmavi->GetFileName() );
							ppi->thmavi->Close();

						} // end if

					} // end if

					// Handle thumbnail avi
					if ( ( ppi->f1 & PUBF1_THMAVI ) != 0 )
					{
						if ( ppi->thmavi->IsOpen() )
						{
							// Save avi filename
							strcpy( ppi->thmavicachefile, ppi->thmavi->GetFileName() );
							ppi->thmavi->Close();

							// Save thumbnail avi filename
							strcpy( ppi->thmavicachefile, ppi->thmavi->GetFileName() );
							ppi->thmavi->Close();

						} // end if

					} // end if

					BOOL bPublished = FALSE;

					// Check for FTP
					if ( ppi->type == PUBTYPE_FTP ) bPublished = Ftp( ppi );

					// Check for Email
					else if ( ppi->type == PUBTYPE_EMAIL ) bPublished = Email( ppi );

					// Check for Disk
					else if ( ppi->type == PUBTYPE_DISK ) bPublished = Disk( ppi );

					if ( bPublished )
					{
						// Inform FRAME
						if ( ppi->type == PUBTYPE_FTP ) FRAME()->SetEvent( 2 );
						else if ( ppi->type == PUBTYPE_EMAIL ) FRAME()->SetEvent( 3 );
						else if ( ppi->type == PUBTYPE_DISK ) FRAME()->SetEvent( 4 );

						// Play sound if needed
						if ( ( ppi->f1 & PUBF1_PLAYSOUND ) != 0 )
						{	if ( *ppi->snd ) PLAYSOUND( ppi->snd );
							else PLAYSOUND( IDW_CAMERA );
						} // end if

					} // end if

					// Ensure cache files are gone
					if ( *ppi->avicachefile != 0 )
					{	CWinFile::Delete( ppi->avicachefile );
						*ppi->avicachefile = 0;
					} // end if
					if ( *ppi->thmavicachefile != 0 )
					{	CWinFile::Delete( ppi->thmavicachefile );
						*ppi->thmavicachefile = 0;
					} // end if

				} // end if

			} // end if

			// Has an interval been specified?
			else if ( ppi->interval != 0 )
			{
				// Set interval first time
				if ( m_bReset || ppi->timeout == 0 ) 
					ppi->timeout = tickcount + ( ppi->interval * 1000 );

				// Have we timed out?			
				BOOL publish = ppi->timeout < tickcount;

				// Do we want to sync to the system clock?
				if ( !publish && ( ppi->f1 & PUBF1_SYNCTOCLOCK ) != 0 ) 
				{
					// Is it a new second?
					if ( seconds != ppi->lasttime )
					{
						// Record last pub time
						ppi->lasttime = seconds;

						// Is it time to publish?
						if ( !( seconds % ppi->interval ) ) publish = TRUE;

					} // end if

				} // end if

				if ( publish )
				{
					// Record next timeout interval
					ppi->timeout = tickcount + ( ppi->interval * 1000 );

					// Record last pub time
					ppi->lasttime = seconds;
					
					// Get current file name
					GetFileName( ppi );

					// Refresh the image
					if ( ppii == NULL ) IMGLIST().Update( ppi->img, TRUE );

					// Handle avi
					if ( ( ppi->f1 & PUBF1_AVI ) != 0 )
					{
						if ( ppi->avi->IsOpen() )
						{
							// Save avi filename
							strcpy( ppi->avicachefile, ppi->avi->GetFileName() );
							ppi->avi->Close();

							// Save thumbnail avi filename
							strcpy( ppi->thmavicachefile, ppi->thmavi->GetFileName() );
							ppi->thmavi->Close();

						} // end if

						// Punt if no avi
						else return TRUE;

					} // end if

					// Handle thumbnail avi
					if ( ( ppi->f1 & PUBF1_THMAVI ) != 0 )
					{
						if ( ppi->thmavi->IsOpen() )
						{
							// Save avi filename
							strcpy( ppi->thmavicachefile, ppi->thmavi->GetFileName() );
							ppi->thmavi->Close();

							// Save thumbnail avi filename
							strcpy( ppi->thmavicachefile, ppi->thmavi->GetFileName() );
							ppi->thmavi->Close();

						} // end if

					} // end if

					BOOL bPublished = FALSE;

					// Check for FTP
					if ( ppi->type == PUBTYPE_FTP ) bPublished = Ftp( ppi );

					// Check for Email
					else if ( ppi->type == PUBTYPE_EMAIL ) bPublished = Email( ppi );

					// Check for Disk
					else if ( ppi->type == PUBTYPE_DISK ) bPublished = Disk( ppi );

					if ( bPublished )
					{
						// Inform FRAME
						if ( ppi->type == PUBTYPE_FTP ) FRAME()->SetEvent( 2 );
						else if ( ppi->type == PUBTYPE_EMAIL ) FRAME()->SetEvent( 3 );
						else if ( ppi->type == PUBTYPE_DISK ) FRAME()->SetEvent( 4 );

						// Play sound if needed
						if ( ( ppi->f1 & PUBF1_PLAYSOUND ) != 0 )
						{	if ( *ppi->snd ) PLAYSOUND( ppi->snd );
							else PLAYSOUND( IDW_CAMERA );
						} // end if

					} // end if

					// Ensure cache files are gone
					if ( *ppi->avicachefile != 0 )
					{	CWinFile::Delete( ppi->avicachefile );
						*ppi->avicachefile = 0;
					} // end if
					if ( *ppi->thmavicachefile != 0 )
					{	CWinFile::Delete( ppi->thmavicachefile );
						*ppi->thmavicachefile = 0;
					} // end if

				} // end if

			} // end if

		} // end try

		// Try to return to a normal life if we can...
		catch( ... ) 
		{	_Log( MB_ICONERROR, "PublishThread()", "Assert" );	
			ASSERT( 0 ); 
		}
	
	} // end while

	m_bReset = FALSE;

	Sleep( 100 );

	return TRUE;
}

BOOL CPubThread::EndThread(LPVOID pData)
{

	return TRUE;
}

BOOL CPubThread::Ftp( LPPUBINFO ppi )
{
	if ( ppi == NULL ) return FALSE;

	// Ensure ftp server
	HGROUP hGroup = FTPSERVERS().FindGroup( ppi->str );
	if ( hGroup == NULL )
	{	_Log( MB_ICONERROR, ppi->str, "FTP Server information not found" );	
		return FALSE;
	} // end if

	BOOL bPublished = FALSE;
	LPBYTE buf = NULL;
	DWORD size = 0;
	TMem< BYTE > temp;
	CWinImg img;

	// Is it an avi file
	if ( *ppi->avicachefile != 0 )
	{
		CWinFile wf;
		if ( wf.OpenExisting( ppi->avicachefile ) )
		{	size = wf.Size();
			if ( size && temp.allocate( size ) && wf.Read( temp.ptr(), temp.size() ) )
				buf = (LPBYTE)temp.ptr();
		} // end if
		
	} // end if
	else
	{
		// Get the image
		if ( !IMGLIST().GetImage( ppi->img, &img ) )
		{	_Log( MB_ICONERROR, ppi->img, "Image not found" );	
			return FALSE;
		} // end if

		// Set jpeg quality
		FRAME()->SetQuality( &img );

		// Encode the file
		if ( !img.Encode( &buf, &size, ppi->pub_fname ) ) 
		{	_Log( MB_ICONERROR, ppi->img, img.GetLastError() );	
			return FALSE;
		} // end if

	} // end else

	// Want Win32 interface?
	if ( FTPSERVERS().GetDword( hGroup, "Rename", FALSE ) )
	{
		// Create FTP object if needed
		if ( ppi->pftp == NULL ) ppi->pftp = new CFtp();
		if ( ppi->pftp == NULL )
		{	_Log( MB_ICONERROR, "Ftp()", "Out of memory" );	
			return FALSE;
		} // end if

		// Is the FTP already working?
		if ( !ppi->pftp->IsConnecting() && !ppi->pftp->IsConnected() )
		{

			// Copy the memory
			if ( ppi->mem == NULL || !ppi->mem->put( buf, size ) )
			{	_Log( MB_ICONERROR, "Ftp()", "Memory error" );	
				return FALSE;
			} // end if

			// Upload the data
			bPublished = FtpImage( ppi->str, ppi->pftp, *ppi->mem, ppi->mem->size(), ppi->pub_fname );
		} // end if

	} // end if

	else 
	{
		// Create FTP object if needed
		if ( ppi->pw32ftp == NULL ) ppi->pw32ftp = new CNetFile();
		if ( ppi->pw32ftp == NULL )
		{	_Log( MB_ICONERROR, "Ftp()", "Out of memory" );	
			return FALSE;
		} // end if

		// Use Windows interface
		bPublished = FtpImage( ppi->str, ppi->pw32ftp, buf, size, ppi->pub_fname );

	} // end else

	// Punt if no thumbnail
	if ( ( ppi->f1 & PUBF1_THUMBNAIL ) == 0 ) return bPublished;

	// Load image if we haven't already
	if ( !img.IsValid() )
	{
		// Get the image
		if ( !IMGLIST().GetImage( ppi->img, &img ) )
		{	_Log( MB_ICONERROR, ppi->img, "Image not found" );	
			return FALSE;
		} // end if

		// Set jpeg quality
		FRAME()->SetQuality( &img );

		// Encode the file
		if ( !img.Encode( &buf, &size, ppi->pub_fname ) ) 
		{	_Log( MB_ICONERROR, ppi->img, img.GetLastError() );	
			return FALSE;
		} // end if

	} // end if

	// Get the thumbnail image
	if ( !GetThumbnail( ppi, &img, img.GetWidth(), img.GetHeight() ) )
		return FALSE;

	// Set jpeg quality
	FRAME()->SetQuality( &img );

	// Encode the thumbnail file
	if ( !img.Encode( &buf, &size, ppi->pub_tfname ) ) return FALSE;

	// Want Win32 interface?
	if ( FTPSERVERS().GetDword( hGroup, "Rename", FALSE ) )
	{
		// Create FTP object if needed
		if ( ppi->pftpthm == NULL ) ppi->pftpthm = new CFtp();
		if ( ppi->pftpthm == NULL )
		{	_Log( MB_ICONERROR, "Ftp()", "Out of memory" );	
			return FALSE;
		} // end if

		// Is the FTP already working?
		if ( !ppi->pftpthm->IsConnecting() && !ppi->pftpthm->IsConnected() )
		{
			// Copy the memory
			if ( ppi->tnmem == NULL || !ppi->tnmem->put( buf, size ) )
			{	_Log( MB_ICONERROR, "Ftp()", "Memory error" );
				return FALSE;
			} // end if

			// Upload the data
			FtpImage( ppi->str, ppi->pftpthm, *ppi->tnmem, ppi->tnmem->size(), ppi->pub_tfname );

		} // end if

	} // end if

	else 
	{
		// Create FTP object if needed
		if ( ppi->pw32ftpthm == NULL ) ppi->pw32ftp = new CNetFile();
		if ( ppi->pw32ftpthm == NULL )
		{	_Log( MB_ICONERROR, "Ftp()", "Memory error" );
			return FALSE;
		} // end if

		// Use Windows interface
		FtpImage( ppi->str, ppi->pw32ftpthm, buf, size, ppi->pub_tfname );

	} // end else

	return bPublished;
}

BOOL CPubThread::FtpImage(LPCTSTR pServer, CFtp *pFtp, LPBYTE buf, DWORD size, LPCTSTR pPath)
{
	// Sanity checks
	if ( pServer == NULL || pFtp == NULL || buf == NULL || size == 0 || pPath == NULL )
		return FALSE;

	// Is the FTP already working?
	if ( pFtp->IsConnecting() || pFtp->IsConnected() )
		return FALSE;

	// Reset the FTP object
	pFtp->Destroy();
	
	// Ensure we have server information
	HGROUP hGroup = FTPSERVERS().FindGroup( pServer );
	if ( hGroup == NULL )
	{	_Log( MB_ICONERROR, pServer, "FTP server information not found" );
		return FALSE;
	} // end if

	char user[ CWF_STRSIZE ];
	char password[ CWF_STRSIZE ];
	char addr[ CWF_STRSIZE ];
	char folder[ CWF_STRSIZE ];
	DWORD port = 21;
	DWORD passive = 1;

	strcpy( addr, FTPSERVERS().GetSz( hGroup, "Address" ) );
	strcpy( folder, FTPSERVERS().GetSz( hGroup, "Folder" ) );
	strcpy( user, FTPSERVERS().GetSz( hGroup, "Username" ) );
	strcpy( password, FTPSERVERS().GetSz( hGroup, "Password" ) );
	port = FTPSERVERS().GetDword( hGroup, "Port", 21 );
	passive = FTPSERVERS().GetDword( hGroup, "Passive", 1 );

	// Set passive FTP mode
	pFtp->SetPassiveMode( passive != 0 );

	// Connect to server
	pFtp->SetUsernamePassword( user, password );
	if ( !pFtp->Connect( addr, port ) ) return FALSE;

	// Build full path to remote file
	CWinFile::WebBuildPath( folder, folder, pPath );

	// Upload file
	pFtp->AutoClose( TRUE );
	if ( !pFtp->Upload( buf, size, folder ) )
	{	_Log( MB_ICONERROR, addr, "Upload error" );
		return FALSE;
	} // end if

	// Inform user
	char msg[ CWF_STRSIZE ];
	wsprintf( msg, "Uploading %s to %s", folder, addr );
	FRAME()->SetStatus( msg );

	return TRUE;
}

BOOL CPubThread::FtpImage(LPCTSTR pServer, CNetFile *pNf, LPBYTE buf, DWORD size, LPCTSTR pPath)
{
	// Sanity checks
	if ( pServer == NULL || pNf == NULL || buf == NULL || size == 0 || pPath == NULL )
		return FALSE;

	// Is the FTP already working?
	if ( pNf->IsWorking() ) return FALSE;
	pNf->Destroy();
	
	// Ensure we have server information
	HGROUP hGroup = FTPSERVERS().FindGroup( pServer );
	if ( hGroup == NULL ) return FALSE;

	char user[ CWF_STRSIZE ];
	char password[ CWF_STRSIZE ];
	char addr[ CWF_STRSIZE ];
	char folder[ CWF_STRSIZE ];
	DWORD port = 21;
	DWORD passive = 1;

	// Get server information
	strcpy( addr, FTPSERVERS().GetSz( hGroup, "Address" ) );
	strcpy( folder, FTPSERVERS().GetSz( hGroup, "Folder" ) );
	strcpy( user, FTPSERVERS().GetSz( hGroup, "Username" ) );
	strcpy( password, FTPSERVERS().GetSz( hGroup, "Password" ) );
	port = FTPSERVERS().GetDword( hGroup, "Port", 21 );
	passive = FTPSERVERS().GetDword( hGroup, "Passive", 1 );

//	FTPSERVERS().GetValue( hGroup, "Address", addr, sizeof( addr ) );
//	FTPSERVERS().GetValue( hGroup, "Folder", folder, sizeof( folder ) );
//	FTPSERVERS().GetValue( hGroup, "Port", &port );
//	FTPSERVERS().GetValue( hGroup, "Username", user, sizeof( user ) );
//	FTPSERVERS().GetValue( hGroup, "Password", password, sizeof( password ) );
//	FTPSERVERS().GetValue( hGroup, "Passive", &passive );

	char connect[ CWF_STRSIZE * 2 ];
	strcpy( connect, "ftp://" );
	strcat( connect, addr );
	CWinFile::WebBuildPath( connect, connect, folder );
	CWinFile::WebBuildPath( connect, connect, pPath );

	// Connect to server
	pNf->SetUsernamePassword( user, password );

	// Set callback
	pNf->SetCallback( CPubThread::OnNfCallback, (DWORD)this );

	// Upload the data
	if ( !pNf->Upload( connect, buf, size, passive != 0 ) )
	{	_Log( MB_ICONERROR, connect, "Upload error" );
		return FALSE;
	} // end if

	// Inform user
	char msg[ CWF_STRSIZE ];
	wsprintf( msg, "Uploading %s", connect );
	FRAME()->SetStatus( msg );

	return TRUE;
}


BOOL CPubThread::Email(LPPUBINFO ppi)
{
	if ( ppi == NULL || ppi->pemi == NULL ) return FALSE;

	// Inform user
	char msg[ CWF_STRSIZE ];
	wsprintf( msg, "Emailing %s to %s", ppi->pub_fname, ppi->pemi->to );
	FRAME()->SetStatus( msg );

	// Get FTP object
	CEmailThread *pEmail = ppi->pemail;
	if ( pEmail == NULL )
	{	_Log( MB_ICONERROR, ppi->str, "Invalid object." );
		return FALSE;
	} // end if

	HGROUP hGroup = EMAILSERVERS().FindGroup( ppi->str );
	if ( hGroup == NULL ) 
	{	_Log( MB_ICONERROR, ppi->str, "Email Server information not found." );
		return FALSE;
	} // end if

	CString server = (LPCTSTR)EMAILSERVERS().GetValuePtr( hGroup, "Address" );
	CString username = (LPCTSTR)EMAILSERVERS().GetValuePtr( hGroup, "Username" );
	CString password = (LPCTSTR)EMAILSERVERS().GetValuePtr( hGroup, "Password" );

	DWORD login = 0;
	EMAILSERVERS().GetValue( hGroup, "Login", &login );

	DWORD authdetect = 1;
	EMAILSERVERS().GetValue( hGroup, "AuthDetect", &authdetect );

	DWORD authtype = 0;
	EMAILSERVERS().GetValue( hGroup, "AuthType", &authtype );

	// Is it an avi file
	if ( *ppi->avicachefile != 0 )

		// E-mail the AVI
		return pEmail->Email(	NULL, ppi->avicachefile,
								ppi->pemi->to, ppi->pemi->from,
								ppi->pemi->subject, ppi->pub_fname,
								server, username, password, 
								ppi->pemi->body, login, 
								authdetect, authtype );


	// E-mail image
	return pEmail->Email(	ppi->img, NULL,
							ppi->pemi->to, ppi->pemi->from,
							ppi->pemi->subject, ppi->pub_fname,
							server, username, password, 
							ppi->pemi->body, login, 
							authdetect, authtype );

}

BOOL CPubThread::Disk(LPPUBINFO ppi)
{
	if ( ppi == NULL ) return FALSE;

	// Is it an avi file
	if ( *ppi->avicachefile != 0 )
	{
		// Build target filename
		char fname[ CWF_STRSIZE ];
		CWinFile::BuildPath( fname, ppi->path, ppi->pub_fname );

		// Copy the file
		if ( !MoveFile( ppi->avicachefile, fname ) )
			CopyFile( ppi->avicachefile, fname, FALSE );

	} // end if

	else
	{
		CWinImg img;

		// Get the image
		if ( !IMGLIST().GetImage( ppi->img, &img ) )
		{	_Log( MB_ICONERROR, ppi->img, "Invalid image." );
			return FALSE;
		} // end if

		// Inform user
		char msg[ CWF_STRSIZE ];
		wsprintf( msg, "Saving %s to %s", ppi->pub_fname, ppi->path );
		FRAME()->SetStatus( msg );

		char fname[ CWF_STRSIZE ];
		CWinFile::BuildPath( fname, ppi->path, ppi->pub_fname );
		
		// Save the file
		if ( !img.Save( fname ) )
		{	_Log( MB_ICONERROR, ppi->img, img.GetLastError() );
		} // end if

	} // end else

	// Punt if no thumbnail
	if ( ( ppi->f1 & PUBF1_THUMBNAIL ) == 0 ) return TRUE;

	// Is it an avi file
	if ( *ppi->thmavicachefile != 0 )
	{
		// Build target filename
		char fname[ CWF_STRSIZE ];
		CWinFile::BuildPath( fname, ppi->path, ppi->pub_tfname );

		// Copy the file
		if ( !MoveFile( ppi->thmavicachefile, fname ) )
			CopyFile( ppi->thmavicachefile, fname, FALSE );

	} // end if

	else
	{
		CWinImg img;

		// Get thumbnail
		GetThumbnail( ppi, &img, img.GetWidth(), img.GetHeight() );

		// Build thumbnail path
		char fname[ CWF_STRSIZE ];
		CWinFile::BuildPath( fname, ppi->path, ppi->pub_tfname );
		
		// Save thumbnail
		if ( !img.Save( fname ) )
		{	_Log( MB_ICONERROR, ppi->img, img.GetLastError() );
		} // end if

	} // end else

	return TRUE;
}

BOOL CPubThread::GetFileName(LPPUBINFO ppi, BOOL bPreview, LPSTR fname, LPSTR tfname, DWORD index )
{
	if ( ppi == NULL ) return FALSE;

	// Ensure buffer
	if ( fname == NULL ) fname = ppi->pub_fname;
	if ( tfname == NULL ) tfname = ppi->pub_tfname;
	if ( index == MAXDWORD ) index = ppi->curindex;

	// Add index to file name
	if ( ( ppi->f1 & PUBF1_ROTATEFNAME ) != 0 )
		AddIndex( fname, ppi->fname, index );
	else strcpy( fname, ppi->fname );

	if ( ( ppi->f1 & PUBF1_TIMESTAMP ) != 0 )
		AddTimestamp( fname, ppi->tmpl );
	
	CWinFile::CleanChars( fname );

	// Add index to thumbnail
	if ( ( ppi->f1 & PUBF1_ROTATEFNAME ) != 0 && ( ppi->f1 & PUBF1_TNROTATE ) != 0 )
		AddIndex( tfname, ppi->tfname, index );
	else strcpy( tfname, ppi->tfname );

	if ( ( ppi->f1 & PUBF1_TIMESTAMP ) != 0 && ( ppi->f1 & PUBF1_TNTIMESTAMP ) != 0 )
		AddTimestamp( tfname, ppi->tmpl );

	CWinFile::CleanChars( tfname );

	// Roll over index
	if ( !bPreview && ( ppi->f1 & PUBF1_ROTATEFNAME ) != 0 )
	{
		ppi->curindex++;
		if ( ppi->curindex > ppi->stopindex ) ppi->curindex = ppi->startindex;
		if ( ppi->curindex < ppi->startindex ) ppi->curindex = ppi->startindex;
	} // end if

	return TRUE;
}

BOOL CPubThread::AddIndex(LPSTR pFile, LPCTSTR pTemplate, DWORD index)
{
	DWORD i = 0, x = 0;
	while ( i < CWF_STRSIZE && pTemplate[ i ] != 0 &&
			pTemplate[ i ] != '#' ) pFile[ x++ ] = pTemplate[ i++ ];

	// Did we find the replace character?
	if ( pTemplate[ i ] == '#' )
	{
		// Add index
		wsprintf( &pFile[ x ], "%lu", index );

		// Finish off file name
		i++; x = strlen( pFile );
		while ( i < CWF_STRSIZE && pTemplate[ i ] != 0 )
			pFile[ x++ ] = pTemplate[ i++ ];
		pFile[ x ] = 0;

	} // end if

	else
	{
		strcpy( pFile, pTemplate );
		char *pext = (LPSTR)CWinFile::GetExtension( pFile );
		char *oext = (LPSTR)CWinFile::GetExtension( pTemplate );

		// Backup one char if extension
		if ( pext != pFile ) pext--;

		// Create new filename
		wsprintf( pext, " (%lu).%s", index, oext );

	} // end else

	return TRUE;
}

BOOL CPubThread::GetThumbnail(LPPUBINFO ppi, CWinImg *pImg, long iw, long ih)
{
	RECT rect;
	SetRect( &rect, 0, 0, iw, ih );

	// Get thumbnail size
	if ( !GetThumbnailRect( ppi, &rect ) ) return FALSE;

	// Get the image
	return IMGLIST().GetImage( ppi->img, pImg, RW( rect ), RH( rect ) );
}

BOOL CPubThread::AddTimestamp(LPSTR str, LPCTSTR tmpl)
{
	if ( str == NULL || tmpl == NULL ) return FALSE;

	DWORD i = 0, x = 0;
	char dst[ CWF_STRSIZE ];

	CWinTime	time;

	// Find timestamp point
	DWORD p = MAXDWORD, f;
	for ( f = 0; str[ f ] != 0 && str[ f ] != '$'; f++ )
		if ( str[ f ] == '.' ) p = f;

	// Will we append?
	BOOL bAppend = ( str[ f ] != '$' );
	if ( bAppend && p == MAXDWORD ) p = strlen( str );

	while ( str[ i ] != 0 )
	{
		if ( str[ i ] == '$' || ( bAppend && i == p ) )
		{
			if ( bAppend ) dst[ x++ ] = ' ';
			if ( bAppend ) dst[ x++ ] = '(';

			// Insert timestamp
			time.GetString( &dst[ x ], tmpl );

			// adjust to new length
			x += strlen( &dst[ x ] );					

			if ( bAppend ) { dst[ x++ ] = ')'; dst[ x++ ] = '.'; }

		} // end if

		// Just copy the character
		else dst[ x++ ] = str[ i ];

		// Next character
		i++;

	} // end while

	// NULL terminate
	dst[ x ] = 0;

	// Copy the thing
	strcpy( str, dst );

	return TRUE;
}

BOOL CPubThread::IsMotion(LPPUBINFO ppi)
{
	// Get video driver
	LPVIDEOINFO pvi = VIDEOLIST().FindByIndex( ppi->motion );
	if ( pvi == NULL ) return FALSE;

	// Check for motion
	BOOL bMotion = ( pvi->motion.dwMotionFrame != ppi->lastmotionframe );
	ppi->lastmotionframe = pvi->motion.dwMotionFrame;

	// Time for motion
	if ( ppi->nextmotion != 0 && ppi->nextmotion > GetTickCount() )
		return FALSE;

	return bMotion;
}



BOOL CPubThread::OnNfCallback(DWORD dwUser, WPARAM wParam, LPARAM lParam)
{
	CNetFile *pNf = (CNetFile*)lParam;

	switch( wParam )
	{
		case NETFILE_DS_ERROR :
		{	
			if ( pNf != NULL ) 
			{	_Log( MB_ICONERROR, pNf->GetUrl(), "Cannot connect to FTP server" ); }
			else { _Log( MB_ICONERROR, "FTP", "Cannot connect to FTP server" ); }

		} break;

	} // end switch

	return TRUE;
}

BOOL CPubThread::GetThumbnailRect(LPPUBINFO ppi, LPRECT pRect)
{
	long iw = PRW( pRect );
	long ih = PRH( pRect );

	// Absolute width?
	if ( ( ppi->f1 & PUBF1_TNSIZING ) ) 
		SetRect( pRect, 0, 0, ppi->tnwidth, ppi->tnheight );

	// Percentage
	else SetRect( pRect, 0, 0, ( iw * ppi->tnpwidth ) / 100, ( ih * ppi->tnpheight ) / 100 );

	if ( ( ppi->f1 & PUBF1_FIXEDRATIO ) )
	{
		// Correct aspect ratio
		CGrDC::AspectCorrect( pRect, iw, ih );

	} // end if

	return TRUE;
}

BOOL CPubThread::WriteAviFrame(LPPUBINFO ppi, CWinAvi *pavi, LPPUBIMGINFO ppii, LPCTSTR pFilename)
{
	// Sanity checks
	if ( pavi == NULL || ppii == NULL ) return FALSE;

	// Open the avi if needed
	if ( !pavi->IsOpen() )
	{
		// Get current file name
		GetFileName( ppi, TRUE );

		char fname[ CWF_STRSIZE ];
		CWinFile::BuildPath( fname, ppi->path, pFilename );

		char path[ CWF_STRSIZE ];
		CWinFile::GetExePath( path );
		CWinFile::BuildPath( path, path, "Cache" );
		CWinFile::CreateDirectory( path );
		CWinFile::GetCacheFileName( fname, path, path );

		// Don't send code if not compressed
		if ( ( ppi->f1 & PUBF1_COMPRESSVIDEO ) == 0 ) ppi->fourCC = 0;

		// Open the avi
		pavi->OpenNew(	path, *ppii->os, 
						ppi->pbframes, ppi->pbseconds, 
						ppi->fourCC, 
						ppi->codecdata != NULL ? ppi->codecdata->ptr() : NULL,
						ppi->codecdata != NULL ? ppi->codecdata->size() : 0,
						FALSE );

	} // end if

	// Save frame if avi is open
	if ( pavi->IsOpen() )
	{					
		CTlLocalLock ll( ppii->lock, 3000 );
		if ( ll.IsLocked() ) pavi->AddFrame( ppii->os );

	} // end if

	return TRUE;
}
