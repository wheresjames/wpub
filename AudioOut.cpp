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
// AudioOut.cpp: implementation of the CAudioOut class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AudioOut.h"
#include "WavFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAudioOut::CAudioOut() : CThread()
{
	// Create events for audio
	mwao_hEvent[ 0 ] = CreateEvent( NULL, FALSE, FALSE, NULL ); 
	mwao_hEvent[ 1 ] = CreateEvent( NULL, FALSE, FALSE, NULL ); 
	mwao_hEvent[ 2 ] = CreateEvent( NULL, FALSE, FALSE, NULL ); 

	StartThread();

	mwao_hAudio = NULL;

	m_bPlaying = FALSE;
	mwao_bOpen = FALSE;
	mwao_bDone = FALSE;

	mwao_dwHeadBlock = 0;
	mwao_dwTailBlock = 0;

	// Initialize the headers
	for ( DWORD i = 0; i < CAUDIOOUT_NUMBUFFERS; i++ )
	{
		ZeroMemory( (LPVOID)&mwao_pWH, sizeof( WAVEHDR ) );
		mwao_pucBuffer[ i ].allocate( CAUDIOOUT_BUFFERSIZE );
	} // end for

	m_dwLength = 0;

	ZeroMemory( &m_wf, sizeof( m_wf ) );
}

CAudioOut::~CAudioOut()
{
	StopThread();
	
	// Close any open audio streams
	WAOClose();

	CloseHandle( mwao_hEvent[ 0 ] ); 
	CloseHandle( mwao_hEvent[ 1 ] ); 
	CloseHandle( mwao_hEvent[ 2 ] ); 

}

//////////////////////////////////////////////////////////////////////
// Wave Messaging
//////////////////////////////////////////////////////////////////////
BOOL CAudioOut::InitThread( LPVOID pData )
{
	// We need to check for a stop command
	mwao_hEvent[ 3 ] = CThread::GetStopEvent();

	return TRUE;
}

BOOL CAudioOut::DoThread( LPVOID pData )
{
	// Wait for something to happen
	DWORD ret = WaitForMultipleObjects( 4, mwao_hEvent, FALSE, INFINITE );

	// Ignore the stop command
	if ( ret >= WAIT_OBJECT_0 && ret < ( WAIT_OBJECT_0 + 3 ) )
	{
		switch( ret - WAIT_OBJECT_0 )
		{
			case 0 :
				WOMOpen ( mwao_hAudio );
				break;
			case 1 :
				WOMClose( mwao_hAudio );
				break;
			case 2 :
				WOMDone( mwao_hAudio, mwao_pwh );
				break;
		} // end switch
	} // end if

	// We always want more time
	return TRUE;
}

void CALLBACK CAudioOut::WAOWaveCallback(	HWAVEOUT hwo,   
											UINT uMsg, 
											DWORD dwInstance, 
											DWORD dwParam1, 
											DWORD dwParam2 )
{
	if ( dwInstance == NULL ) return;

	// Get a pointer to the class instace
	CAudioOut* pAudioOut = (CAudioOut*)dwInstance;

	// Call the correct wave instance
	pAudioOut->WAOWaveMessage( hwo, uMsg, dwParam1, dwParam2 );
}

// Map the messages where they need to go 
void CAudioOut::WAOWaveMessage(	HWAVEOUT hWave, UINT uMsg, 
								DWORD dwParam1, DWORD dwParam2 )
{
	if ( hWave == NULL ) return;
	switch ( uMsg )
	{
 		case WOM_OPEN :
			SetEvent( mwao_hEvent[ 0 ] );
			break;
		case WOM_CLOSE :
			SetEvent( mwao_hEvent[ 1 ] );
			break;
		case WOM_DONE :
			m_bPlaying = FALSE;
			mwao_bDone = TRUE;
			SetEvent( mwao_hEvent[ 2 ] );
			if ( m_dwLength )
			{	waveOutUnprepareHeader( mwao_hAudio, &mwao_pWH[ mwao_dwTailBlock ], sizeof( WAVEHDR ) );
				m_dwLength = 0;
			} // end if
			mwao_wh = (WAVEHDR*)dwParam1;
			break;
	} // end switch
}

void CAudioOut::WOMOpen( HWAVEOUT hWave )
{
	mwao_bOpen = TRUE;
}

void CAudioOut::WOMClose( HWAVEOUT hWave )
{
	mwao_bOpen = FALSE;
}

void CAudioOut::WOMDone( HWAVEOUT hWave, WAVEHDR* lpWH )
{
	// Release the audio buffers
	while ( ( mwao_dwTailBlock != mwao_dwHeadBlock ) &&
			( mwao_pWH[ mwao_dwTailBlock ].dwFlags & WHDR_DONE ) )
	{
		// Remove from the queue
		waveOutUnprepareHeader( hWave, &mwao_pWH[ mwao_dwTailBlock ], sizeof( WAVEHDR ) );

		if ( ++mwao_dwTailBlock > CAUDIOOUT_NUMBUFFERS )
			mwao_dwTailBlock = 0;
	} // end while	
}


//////////////////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////////////////
BOOL CAudioOut::WAOOpen( UINT uDeviceID, DWORD sps, DWORD ch, DWORD bps )
{
	// Close any open audio out stream
	WAOClose();

	// Punt if audio out is not supported
	if ( !waveOutGetNumDevs() ) return FALSE;

	// Reset length
	m_dwLength = 0;

	// Start the buffers over
	m_bPlaying = FALSE;
	mwao_bDone = FALSE;
	mwao_dwHeadBlock = 0;
	mwao_dwTailBlock = 0;

	// Set up the wave form structure
	ZeroMemory( &m_wf, sizeof( m_wf ) );
	m_wf.wFormatTag = WAVE_FORMAT_PCM;	// Pulse Code Modulation
	m_wf.nChannels = (WORD)ch;			// channels
	m_wf.nSamplesPerSec = sps;			// sampling rate
	m_wf.nAvgBytesPerSec = sps;			// 
	m_wf.nBlockAlign = 1;				//
	m_wf.wBitsPerSample =	(WORD)bps;	// 
	m_wf.cbSize = 0;					// N/A

	if ( waveOutOpen(	&mwao_hAudio,
						uDeviceID,
						&m_wf,
						(DWORD)CAudioOut::WAOWaveCallback,
						(DWORD)this,
						CALLBACK_FUNCTION ) != MMSYSERR_NOERROR )
	{
		mwao_hAudio = NULL;
		return FALSE;
	} // end if

	return TRUE;
}

void CAudioOut::WAOClose()
{
	mwao_bOpen = FALSE;

	if ( mwao_hAudio != NULL )
	{
		// Reset the audio
		waveOutReset( mwao_hAudio );

		// Close the wave handle
		waveOutClose( mwao_hAudio );

		DWORD timeout = GetTickCount() + 3000;
		while ( WAOIsOpen() && timeout > GetTickCount() );

		ZeroMemory( &m_wf, sizeof( m_wf ) );

		// Once is enough, don't you think?
		mwao_hAudio = NULL;

		// Not playing
		m_bPlaying = FALSE;

	} // end if
}

BOOL CAudioOut::WAOPlay(BYTE * pBuffer, DWORD dwLength)
{
	// Sanity check
	if ( pBuffer == NULL || dwLength == 0 ) return FALSE;

	DWORD head = mwao_dwHeadBlock;

	// Check for an open buffer
	if ( ++head >= CAUDIOOUT_NUMBUFFERS ) head = 0;
	if ( head == mwao_dwTailBlock ) return FALSE;
	if ( ++head >= CAUDIOOUT_NUMBUFFERS ) head = 0;
	if ( head == mwao_dwTailBlock ) return FALSE;

	head = mwao_dwHeadBlock;

	// How much data can we write out
	dwLength = ( dwLength < CAUDIOOUT_BUFFERSIZE ?
				dwLength : CAUDIOOUT_BUFFERSIZE );

	// Copy the data into our buffer
	memcpy( (LPVOID)(BYTE*)mwao_pucBuffer[ head ], (LPVOID)pBuffer, dwLength );

	// Clear out any old junk
	ZeroMemory( (LPVOID)&mwao_wh, sizeof( WAVEHDR ) );

	// Fill in the wave block info
	mwao_pWH[ head ].lpData =  (LPSTR)(BYTE*)mwao_pucBuffer[ head ];
	mwao_pWH[ head ].dwBufferLength = dwLength;
	mwao_pWH[ head ].dwBytesRecorded = 0;
	mwao_pWH[ head ].dwUser = 0;
	mwao_pWH[ head ].dwFlags = 0;
	mwao_pWH[ head ].dwLoops = 0;
	//wh.wavehdr_tag = NULL;
	//wh.reserved = 0;

	// Prepare the buffer for playing
	if ( waveOutPrepareHeader( mwao_hAudio, &mwao_pWH[ head ], sizeof( WAVEHDR ) ) 
		!= MMSYSERR_NOERROR ) 
	{
		return FALSE;
	} 

	// Attempt to play
	m_bPlaying = TRUE;
	if ( waveOutWrite( mwao_hAudio, &mwao_pWH[ head ], sizeof( WAVEHDR ) ) 
				!= MMSYSERR_NOERROR )
	{	m_bPlaying = FALSE;
		return FALSE;
	} // end if

	// Point to the next buffer
	if ( ++mwao_dwHeadBlock >= CAUDIOOUT_NUMBUFFERS ) 
		mwao_dwHeadBlock = 0;

	return TRUE;
}

BOOL CAudioOut::WAOPause()
{
	return ( waveOutPause( mwao_hAudio ) == MMSYSERR_NOERROR );
}

BOOL CAudioOut::WAORestart()
{
	return ( waveOutRestart( mwao_hAudio ) == MMSYSERR_NOERROR );
}

BOOL CAudioOut::WAOReset()
{
	return ( waveOutReset( mwao_hAudio ) == MMSYSERR_NOERROR );
}

BOOL CAudioOut::Play(CWavFile *pWf)
{
	// Reopen if settings are different
	if (	pWf->GetFormat()->nSamplesPerSec != m_wf.nSamplesPerSec ||
			pWf->GetFormat()->nChannels != m_wf.nChannels ||
			pWf->GetFormat()->wBitsPerSample != m_wf.wBitsPerSample )
	{
		// Set correct format
		if ( !WAOOpen(	WAVE_MAPPER, 
						pWf->GetFormat()->nSamplesPerSec, 
						pWf->GetFormat()->nChannels,
						pWf->GetFormat()->wBitsPerSample ) ) return FALSE;
	} // end if

	// Sanity check
	if ( pWf->GetBuffer() == NULL || pWf->GetBufferSize() == 0 ) 
		return FALSE;

	// Fill in the wave block info
	mwao_pWH[ 0 ].lpData = (char*)pWf->GetBuffer();
	mwao_pWH[ 0 ].dwBufferLength = pWf->GetBufferSize();
	mwao_pWH[ 0 ].dwBytesRecorded = 0;
	mwao_pWH[ 0 ].dwUser = 0;
	mwao_pWH[ 0 ].dwFlags = 0;
	mwao_pWH[ 0 ].dwLoops = 0;

	// Prepare the buffer for playing
	if ( waveOutPrepareHeader( mwao_hAudio, &mwao_pWH[ 0 ], sizeof( WAVEHDR ) ) 
		!= MMSYSERR_NOERROR ) return FALSE;

	// Attempt to play
	m_bPlaying = TRUE;
	if ( waveOutWrite( mwao_hAudio, &mwao_pWH[ 0 ], sizeof( WAVEHDR ) ) 
				!= MMSYSERR_NOERROR )
	{	m_bPlaying = FALSE;
		return FALSE;
	} // end if

	// Save total length
	m_dwLength = pWf->GetBufferSize();

	return TRUE;
}

BOOL CAudioOut::WAOIsPlaying()
{
	// Is there a valid handle
	if ( mwao_hAudio == NULL ) return FALSE; 

	MMTIME mt;
	ZeroMemory( &mt, sizeof( mt ) );

	// Get position in bytes
	mt.wType = TIME_BYTES;

	// Get the play position
	if ( waveOutGetPosition( mwao_hAudio, &mt, sizeof( mt ) ) 
			!= MMSYSERR_NOERROR ) return FALSE;

	// Is it reset or at the end
	if ( mt.u.cb == 0 || mt.u.cb >= m_dwLength ) return FALSE;

	// Must be still playing
	return TRUE;
}
