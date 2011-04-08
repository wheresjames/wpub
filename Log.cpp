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
// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vp.h"
#include "Log.h"

#include "DlgMsgView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

char CLog::m_msgbuf[ 4096 ] = { 0 };

CLog CLog::m_log;

char		CLog::m_lastTitle[ 4096 ] = { 0 };
char		CLog::m_lastMessage[ 4096 ] = { 0 };
char		CLog::m_lastLocation[ 4096 ] = { 0 };
DWORD		CLog::m_lastFlags = 0;
DWORD		CLog::m_lastTime = 0;
CTlLock		CLog::m_lock;

LPCTSTR __cdecl CLog::Format( LPSTR pFormat, ... )
{
	return vFormat( pFormat, (char*) ( &pFormat + 1 ) );
}

LPCTSTR CLog::vFormat( LPSTR pFormat, va_list argptr )
{
	try
	{
		// Sanity Check
		if ( pFormat == NULL || pFormat[ 0 ] == 0x0 ) return FALSE;

		// Create the string
		vsprintf( m_msgbuf, pFormat, argptr );

		// Add CRLF
		strcat( m_msgbuf, "\r\n" );

	} // end try
	catch( ... )
	{
		strcpy_sz( m_msgbuf, "Exception\r\n" );
	} // end catch

	return m_msgbuf;
}

CLog::CLog()
{
	m_dwSize = 0;
	m_pHead = NULL;
	m_pTail = NULL;

	m_dlg = new CDlgMsgView();
	if ( m_dlg != NULL ) m_dlg->SetLog( this );

	m_file = NULL;
}

CLog::~CLog()
{
	if ( m_dlg != NULL )
	{	delete m_dlg;
		m_dlg = NULL;
	} // end if

	if ( m_file != NULL )
	{	m_file->Close();
		delete m_file;
		m_file = NULL;
	} // end if

	Destroy();
}

void CLog::Destroy()
{
	if ( m_pHead != NULL )
	{
		while ( m_pHead != NULL )
		{
			LPLOGMSGINFO next = m_pHead->pNext;
			delete m_pHead;
			m_pHead = next;
		} // end while

	} // end if

	m_pTail = NULL;
	m_dwSize = 0;
}

LPLOGMSGINFO CLog::InsertMessage( LPCTSTR pTitle, LPCTSTR pMessage, LPCTSTR pLocation, DWORD dwFlags )
{
	LPLOGMSGINFO node = new LOGMSGINFO;
	if ( node == NULL ) return NULL;

	node->dwFlags = dwFlags;
	if ( pTitle == NULL || *pTitle == NULL ) strcpy( node->szTitle, "Message" );
	else
	{
		strncpy( node->szTitle, pTitle, sizeof( node->szTitle ) - 1 );
		node->szTitle[ sizeof( node->szTitle ) - 1 ] = NULL;
	} // end else
	if ( pMessage == NULL || *pMessage == NULL ) strcpy( node->szMessage, "< NULL >" );
	else
	{
		strncpy( node->szMessage, pMessage, sizeof( node->szMessage ) - 1 );
		node->szMessage[ sizeof( node->szMessage ) - 1 ] = NULL;
	} // end else
	if ( pLocation == NULL || *pLocation == NULL ) strcpy( node->szLocation, "" );
	else
	{
		strncpy( node->szLocation, pLocation, sizeof( node->szLocation ) - 1 );
		node->szLocation[ sizeof( node->szLocation ) - 1 ] = NULL;
	} // end else
	
	{ // Save Date and time
		SYSTEMTIME	st;
		GetLocalTime( &st );
		wsprintf( node->szTime, "%02lu/%02lu/%lu - %02lu:%02lu:%02lu",
								ULONG( st.wMonth ),
								ULONG( st.wDay ),
								ULONG( st.wYear ),
								ULONG( st.wHour ),
								ULONG( st.wMinute ),
								ULONG( st.wSecond ),
								ULONG( st.wMilliseconds ) );		
	} // end Save Date And Time

	// This will be the last one
	node->pNext = NULL;

	// Add this one into the list
	if ( m_pTail == NULL ) m_pHead = node;
	else m_pTail->pNext = node;
	m_pTail = node;

	// Count a node
	m_dwSize++;

	return node;
}


BOOL CLog::IsValidMsgInfo(LPLOGMSGINFO ptr)
{
	if ( ptr == NULL ) return FALSE;

	LPLOGMSGINFO next = m_pHead;

	while ( next != NULL )
	{
		if ( next == ptr ) return TRUE;
		next = next->pNext;
	} // end while

	return FALSE;
}

BOOL CLog::AddMessage(LPCTSTR pTitle, LPCTSTR pMessage, LPCTSTR pLocation, DWORD dwFlags )
{
//	BOOL bShow =	( dwFlags & 0x000000F0L ) != MB_ICONINFORMATION &&
//					( dwFlags & 0x000000F0L ) != MB_ICONWARNING;

	CTlLocalLock ll( &m_lock );
	if ( !ll.IsLocked() ) return FALSE; 

	// Don't show the same error more than once every three seconds
	if ( ( m_lastTime + 3000 ) > GetTickCount() )
	{
		// Verify message has changed
		if ( dwFlags == m_lastFlags &&
			 ( pTitle == NULL || !strcmp( pTitle, m_lastTitle ) ) &&
			 ( pMessage == NULL || !strcmp( pMessage, m_lastMessage ) ) &&
			 ( pLocation == NULL || !strcmp( pLocation, m_lastLocation ) ) )
			return TRUE;

	} // end if

	// Save message info
	if ( pTitle == NULL ) *m_lastTitle = 0; else strcpy_sz( m_lastTitle, pTitle ); 
	if ( pMessage == NULL ) *m_lastMessage = 0; else strcpy_sz( m_lastMessage, pMessage );
	if ( pLocation == NULL ) *m_lastLocation = 0; else strcpy_sz( m_lastLocation, pLocation );
	m_lastFlags = dwFlags;

	// Save time
	m_lastTime = GetTickCount();

	BOOL bShow		= FALSE;
	BOOL bError		= ( dwFlags & 0x000000F0L ) == MB_ICONERROR;
	BOOL bWarning	= ( dwFlags & 0x000000F0L ) == MB_ICONWARNING;
	BOOL bInfo		= !bError && !bWarning;

	// Do we want to popup on errors?
	if ( bError )
	{	BOOL popup = AfxGetApp()->GetProfileInt( "MsgView", "Popup", 1 ) ? 1 : 0;
		if ( popup ) bShow = TRUE;
	} // end if

	// Do we want to popup on errors?
	if ( bWarning )
	{	BOOL popupwarning = AfxGetApp()->GetProfileInt( "MsgView", "PopupWarnings", 1 ) ? 1 : 0;
		if ( popupwarning ) bShow = TRUE;
	} // end if

	if ( m_log.m_dlg == NULL ) 
	{	m_log.m_dlg = new CDlgMsgView();
		if ( m_log.m_dlg != NULL ) m_log.m_dlg->SetLog( &m_log );
	} // end if
	
	if ( m_log.m_dlg != NULL )
	{
		// Create if needed
		if ( !IsWindow( m_log.m_dlg->GetSafeHwnd() ) )
		{
			m_log.m_dlg->m_bSave = FALSE;
			if ( !m_log.m_dlg->Create( CDlgMsgView::IDD, CWnd::FromHandle( GetDesktopWindow() ) ) ) return FALSE;
			long x = AfxGetApp()->GetProfileInt( "MsgView", "MsgBoxPos_X", 0 );
			long y = AfxGetApp()->GetProfileInt( "MsgView", "MsgBoxPos_Y", 0 );
			long cx = AfxGetApp()->GetProfileInt( "MsgView", "MsgBoxSize_X", 0 );
			long cy = AfxGetApp()->GetProfileInt( "MsgView", "MsgBoxSize_Y", 0 );

			if (	x > 0 && x < GetSystemMetrics( SM_CXSCREEN ) && 
					y > 0 && y < GetSystemMetrics( SM_CYSCREEN ) &&
					cx > 0 && cy > 0 )
				m_log.m_dlg->SetWindowPos(	NULL, x, y, cx, cy,
											SWP_NOZORDER | SWP_NOACTIVATE );
			m_log.m_dlg->m_bSave = TRUE;
		} // end if
		
		if ( bShow )
		{	m_log.m_dlg->ShowWindow( SW_SHOWNORMAL );
			m_log.m_dlg->SetWindowPos(	&CWnd::wndTopMost, 0, 0, 0, 0,
										SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE );
		} // end if

	} // end if

	if ( m_log.Size() > MAXLOGSIZE )
	{
		// Lose all log entries
		m_log.Destroy();

		// Truncate disk log
		if ( m_log.m_file != NULL )
			if ( m_log.m_file->IsOpen() )
			{	char fname[ CWF_STRSIZE ];
				strcpy_sz( fname, m_log.m_file->GetPath() );
				m_log.m_file->OpenNew( fname, GENERIC_WRITE );
			} // end if

		// Refresh list
		if ( m_log.m_dlg != NULL && IsWindow( m_log.m_dlg->GetSafeHwnd() ) )
			m_log.m_dlg->PostMessage( WM_UPDATELIST, 0, 0L );
	
	} // end if


	// Add the message
	LPLOGMSGINFO plmi = m_log.InsertMessage( pTitle, pMessage, pLocation, dwFlags );
	if ( plmi == NULL ) return FALSE;

	// Alert the user
	if ( bShow ) ::MessageBeep( dwFlags & 0x000000F0L );

	// Update the list
	if ( m_log.m_dlg != NULL && IsWindow( m_log.m_dlg->GetSafeHwnd() ) )
		m_log.m_dlg->PostMessage( WM_INSERTITEM, 0, (LPARAM)plmi );

	// Log to file
	if ( m_log.m_file != NULL )
		if ( m_log.m_file->IsOpen() )
			SaveToFile( m_log.m_file, plmi );

	return TRUE;
}

void CLog::ShowLog()
{
	if ( m_log.m_dlg != NULL && IsWindow( m_log.m_dlg->GetSafeHwnd() ) )
		m_log.m_dlg->ShowWindow( SW_SHOWNORMAL );
}

BOOL CLog::SetLogFile(LPCTSTR pFile)
{
	if ( m_log.m_file == NULL )
	{	m_log.m_file = new CWinFile();
		if ( m_log.m_file == NULL ) return FALSE;
	} // end if

	return m_log.m_file->OpenNew( pFile );
}

#define FILE_WRITE( file, str ) file->Write( str, strlen( str ) )
BOOL CLog::SaveToFile(CWinFile * pFile, LPLOGMSGINFO pLmi)
{
	if ( pFile == NULL || pLmi == NULL ) return FALSE;

	FILE_WRITE( pFile, "\r\nTime : " );
	FILE_WRITE( pFile, pLmi->szTime );
	FILE_WRITE( pFile, "\r\nTitle : " );
	FILE_WRITE( pFile, pLmi->szTitle );
	FILE_WRITE( pFile, "\r\nMessage : " );
	FILE_WRITE( pFile, pLmi->szMessage );
	FILE_WRITE( pFile, "\r\nLocation : " );
	FILE_WRITE( pFile, pLmi->szLocation );
	FILE_WRITE( pFile, "\r\n" );

	return TRUE;
}
