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
// CaptureThread.cpp: implementation of the CCaptureThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vp.h"
#include "CaptureThread.h"

#include "DlgInform.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCaptureThread::CCaptureThread()
{
	m_hCapture = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_hCaptureComplete = CreateEvent( NULL, TRUE, FALSE, NULL );

	m_dwFrameRate = 0;
	m_dwTickCount = 0;
}

CCaptureThread::~CCaptureThread()
{
	CloseHandle( m_hCapture );
	CloseHandle( m_hCaptureComplete );
}

BOOL CCaptureThread::DoThread( LPVOID pData )
{
	DWORD enablerefresh = ( m_dwFrameRate != 0 );

	DWORD wait = 250;
	if ( !enablerefresh ) wait = 1000;
	else
	{
		// How long should we wait?
		if ( m_dwFrameRate > 60 ) m_dwFrameRate = 60;
		if ( m_dwFrameRate != 0 ) wait = 1000 / m_dwFrameRate;

		DWORD tickcount = GetTickCount();
		if ( tickcount > m_dwTickCount )
		{	tickcount -= m_dwTickCount;
			if ( tickcount < wait ) 
				wait -= tickcount;
		} // end if

	} // end else

	HANDLE	phCmd[ 3 ];

	phCmd[ 0 ] = m_hStop;
	phCmd[ 1 ] = m_hCapture;

	// Wait
	if ( wait > 1000 ) wait = 1000;
	DWORD ret = WaitForMultipleObjects( 2, phCmd, FALSE, wait );

	// Verify range
	if ( ret < WAIT_OBJECT_0 ) return TRUE;

	// Get event index
	ret -= WAIT_OBJECT_0;

	// Check for stop
	if ( ret == 0 ) return FALSE;

	// Punt if refresh disabled
	if ( ret == WAIT_TIMEOUT && !enablerefresh ) return TRUE;

	// Save tick count
	m_dwTickCount = GetTickCount();

	// Refresh the video
	if ( IsConnected() ) 
	{	
		try
		{
			CCapture::GrabFrame();		
			CCapture::WaitGrab();
		} 
		catch( ... )
		{	_Log( MB_ICONERROR, "Capture Driver Error", "Capture Driver Crashed : Restarting" );
			CCapture::Reconnect();
		} // end catch

	} // end if

	// Capture should not take more than
	if ( ( GetTickCount() - m_dwTickCount ) > 2000 )
	{
		// Should we be expecting delayed video?
		if ( !APP()->m_bExpectDelays )
		{
			_Log( MB_ICONERROR, "Capture Driver Error", "Capture Driver Timeout : Restarting" );
			CCapture::Reconnect();
		} // end if

	} // end if

	// Reset capture event
	ResetEvent( m_hCapture );
	SetEvent( m_hCaptureComplete );

	return TRUE;
}

BOOL CCaptureThread::GrabFrame()
{	ResetEvent( m_hCaptureComplete ); return SetEvent( m_hCapture );
}

BOOL CCaptureThread::WaitCapture( DWORD timeout )
{	if ( IsInCallback() ) return FALSE;
	return ( WaitForSingleObject( m_hCaptureComplete, timeout ) != WAIT_TIMEOUT ); 
}
