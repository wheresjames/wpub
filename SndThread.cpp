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
// SndThread.cpp: implementation of the CSndThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vp.h"
#include "SndThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSndThread::CSndThread()
{
	// Create a semephore
	m_hPlay = CreateEvent( NULL, TRUE, FALSE, NULL );

	m_pSnd = NULL;
	*m_szSnd = 0;

}

CSndThread::~CSndThread()
{
	if ( m_hPlay != NULL ) 
	{	CloseHandle( m_hPlay );
		m_hPlay = NULL;
	} // end if	 
}

BOOL CSndThread::DoThread(LPVOID pData)
{
	HANDLE	phCmd[ 3 ];

	phCmd[ 0 ] = m_hStop;
	phCmd[ 1 ] = m_hPlay;

	// Wait
	DWORD ret = WaitForMultipleObjects( 2, phCmd, FALSE, INFINITE );

	// Verify range
	if ( ret < WAIT_OBJECT_0 ) return TRUE;

	// Get event index
	ret -= WAIT_OBJECT_0;

	// Check for stop
	if ( ret == 0 ) return FALSE;

	// Skip if busy
	if ( m_audioout.IsPlaying() ) { Sleep( 30 ); return TRUE; }

	// Attempt to play the wav file
	if ( m_pSnd != NULL )
	{	if ( m_wf.LoadFromResource( m_pSnd, "WAVE" ) )
			m_audioout.Play( &m_wf );
	} // end if

	// Play sound file
	else if ( m_wf.Load( m_szSnd ) ) m_audioout.Play( &m_wf );

	// Reset play event
	ResetEvent( m_hPlay );

	return TRUE;
}

BOOL CSndThread::Play(UINT snd)
{
	// Save sound
	SetSound( snd );

	// Set thread
	if ( !IsRunning() ) StartThread();

	// Play the sound
	SetEvent( m_hPlay );

	return TRUE;
}

BOOL CSndThread::Play(LPCTSTR pSnd)
{
	// Save sound
	SetSound( pSnd );

	// Set thread
	if ( !IsRunning() ) StartThread();

	// Play the sound
	SetEvent( m_hPlay );

	return TRUE;
}

BOOL CSndThread::InitThread(LPVOID pData)
{
	m_wf.LoadFromResource( m_pSnd, "WAVE" );
	m_audioout.WAOOpen();

	return TRUE;
}

BOOL CSndThread::EndThread(LPVOID pData)
{
	m_audioout.WAOClose();

	return TRUE;
}

