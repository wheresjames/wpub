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
// ImgThread.h: interface for the CImgThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMGTHREAD_H__09738A49_A344_488E_B7D5_F7E01EA9EA2A__INCLUDED_)
#define AFX_IMGTHREAD_H__09738A49_A344_488E_B7D5_F7E01EA9EA2A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Thread.h"

class CImgThread : public CThread  
{
public:
	BOOL EnumImages( LPCTSTR pDir, HWND hWnd, UINT uMsg );
	virtual BOOL EndThread( LPVOID pData );
	virtual BOOL InitThread( LPVOID pData );
	virtual BOOL DoThread( LPVOID pData );
	CImgThread();
	virtual ~CImgThread();

private:

	char				m_szDir[ CWF_STRSIZE ];
	HWND				m_hWnd;
	UINT				m_uMsg;

	HANDLE				m_hFind;
	WIN32_FIND_DATA		m_wfd;

};

#endif // !defined(AFX_IMGTHREAD_H__09738A49_A344_488E_B7D5_F7E01EA9EA2A__INCLUDED_)
