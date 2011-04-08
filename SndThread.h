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
// SndThread.h: interface for the CSndThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SNDTHREAD_H__3AD2D1D4_0264_4EA1_B75E_8B0B9229C744__INCLUDED_)
#define AFX_SNDTHREAD_H__3AD2D1D4_0264_4EA1_B75E_8B0B9229C744__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Thread.h"
#include "WavFile.h"
#include "AudioOut.h"

class CSndThread : public CThread  
{
public:
	BOOL Play( LPCTSTR pSnd );
	virtual BOOL EndThread( LPVOID pData );
	virtual BOOL InitThread( LPVOID pData );
	BOOL Play( UINT snd );
	BOOL DoThread( LPVOID pData );
	CSndThread();
	virtual ~CSndThread();

	void SetSound( UINT snd ) 
	{	*m_szSnd = 0; m_pSnd = MAKEINTRESOURCE( snd ); }

	void SetSound( LPCTSTR pSnd ) 
	{	m_pSnd = NULL; strcpy( m_szSnd, pSnd ); }


private:

	HANDLE		m_hPlay;

	LPCTSTR		m_pSnd;
	char		m_szSnd[ CWF_STRSIZE ];

	CWavFile	m_wf;
	CAudioOut	m_audioout;

};

#endif // !defined(AFX_SNDTHREAD_H__3AD2D1D4_0264_4EA1_B75E_8B0B9229C744__INCLUDED_)
