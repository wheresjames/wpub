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
// AudioOut.h: interface for the CAudioOut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIOOUT_H__E2943713_429D_11D2_83AA_00104B2C9CFA__INCLUDED_)
#define AFX_AUDIOOUT_H__E2943713_429D_11D2_83AA_00104B2C9CFA__INCLUDED_

#include "mmsystem.h"

//#include "Instance.h"

#include "Thread.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define CAUDIOOUT_NUMBUFFERS	5
#define CAUDIOOUT_BUFFERSIZE	10480

class CWavFile;
class CAudioOut : public CThread
{
	// Wave messages
public:
	void WAOWaveMessage(	HWAVEOUT hWave, UINT uMsg, 
							DWORD dwParam1, DWORD dwParam2 );

	virtual void WOMClose( HWAVEOUT hWave );
	virtual void WOMDone( HWAVEOUT hWave, WAVEHDR* lpWH );
	virtual void WOMOpen( HWAVEOUT hWave );

public:
	BOOL IsPlaying() { return m_bPlaying; }
	BOOL WAOIsPlaying();
	BOOL Play( CWavFile *pWf );
	BOOL WAOReset();
	BOOL WAORestart();
	BOOL WAOPause();
	BOOL WAOPlay( BYTE *pBuffer, DWORD dwLength );
	void WAOClose();
	BOOL WAOOpen( UINT uDeviceID = WAVE_MAPPER, DWORD sps = 44100, DWORD ch = 2, DWORD bps = 16 );
	CAudioOut();
	virtual ~CAudioOut();

	BOOL WAOIsOpen() { return ( mwao_hAudio != NULL && mwao_bOpen ); }

	UINT WAOGetDeviceID() { return (UINT) mwao_hAudio; }
	void WAOSetHandle( HWAVEOUT h ) { WAOClose(); mwao_hAudio = h; mwao_bOpen = TRUE; }
	void WAODetach() { mwao_hAudio = NULL; mwao_bOpen = FALSE; }

	virtual BOOL InitThread( LPVOID pData );
	virtual BOOL DoThread( LPVOID pData );

	BOOL IsDone() { return mwao_bDone; }

private:
	volatile BOOL		mwao_bDone;
	volatile BOOL		mwao_bOpen;
	HWAVEOUT			mwao_hAudio;

	HANDLE				mwao_hEvent[ 4 ];
	WAVEHDR*			mwao_wh;

	DWORD				mwao_dwHeadBlock;
	DWORD				mwao_dwTailBlock;
	WAVEHDR 			mwao_pWH[ CAUDIOOUT_NUMBUFFERS ];
	WAVEHDR 			mwao_pwh[ CAUDIOOUT_NUMBUFFERS ];
	TMem < BYTE >		mwao_pucBuffer[ CAUDIOOUT_NUMBUFFERS ];

	WAVEFORMATEX		m_wf;

	DWORD				m_dwLength;

	BOOL				m_bPlaying;

private:
	// Callback for wave messages
	static void CALLBACK WAOWaveCallback(	HWAVEOUT hwo,   
											UINT uMsg, 
											DWORD dwInstance, 
											DWORD dwParam1, 
											DWORD dwParam2 );

};

#endif // !defined(AFX_AUDIOOUT_H__E2943713_429D_11D2_83AA_00104B2C9CFA__INCLUDED_)
