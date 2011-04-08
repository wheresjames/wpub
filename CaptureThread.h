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
// CaptureThread.h: interface for the CCaptureThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAPTURETHREAD_H__2AB4600E_C603_4502_834B_910A91C406CE__INCLUDED_)
#define AFX_CAPTURETHREAD_H__2AB4600E_C603_4502_834B_910A91C406CE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CCaptureThread : public CThread, public CCapture
{
public:
	BOOL WaitCapture( DWORD timeout = 3000 );
	virtual BOOL GrabFrame();
	CCaptureThread();
	virtual ~CCaptureThread();

	virtual BOOL DoThread( LPVOID pData );
	void SetFrameRate( DWORD fr ) 
	{ 	m_dwFrameRate = fr; 
		if ( !IsRunning() ) StartThread();		
	}
	DWORD GetFrameRate() { return m_dwFrameRate; }

private:

	HANDLE		m_hCapture;
	HANDLE		m_hCaptureComplete;

	DWORD		m_dwFrameRate;
	DWORD		m_dwTickCount;

};

#endif // !defined(AFX_CAPTURETHREAD_H__2AB4600E_C603_4502_834B_910A91C406CE__INCLUDED_)
