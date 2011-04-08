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
// IServer.h: interface for the CIServer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISERVER_H__FC03179E_5259_4D52_B4BA_3E7900345D63__INCLUDED_)
#define AFX_ISERVER_H__FC03179E_5259_4D52_B4BA_3E7900345D63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ISession.h"

class CIServer : public CAsyncWinSock  
{
public:
	BOOL Start( HWND hWnd, UINT uMsg, DWORD dwPort, DWORD dwMax = 30 );
	virtual BOOL OnAccept( int nErr );
	virtual BOOL Accept( CAsyncWinSock *pSocket );
	CIServer();
	virtual ~CIServer();

	void SetSessionList( CISession *pSl ) { m_pSl = pSl; }
	CISession* GetSessionList() { return m_pSl; }

	void SetSessionMessageTarget( HWND hWnd, UINT uMsg )
	{	m_hWndSession = hWnd; m_uMsgSession = uMsg; }

private:

	CISession			*m_pSl;

	HWND				m_hWndSession;
	UINT				m_uMsgSession;

};

#endif // !defined(AFX_CISERVER_H__FC03179E_5259_4D52_B4BA_3E7900345D63__INCLUDED_)
