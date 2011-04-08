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
// Ftp.h: interface for the CFtp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTP_H__3A28B48C_1748_434A_8659_ED0E750B1147__INCLUDED_)
#define AFX_FTP_H__3A28B48C_1748_434A_8659_ED0E750B1147__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Thread.h"
#include "FtpClient.h"

#define FTPCMD_NONE			0
#define FTPCMD_CONNECT		1
#define FTPCMD_UPLOAD		2
#define FTPCMD_DOWNLOAD		3

class CFtp : public CThread  
{
public:
	BOOL Upload( LPBYTE buf, DWORD size, LPCTSTR pRemote );
	BOOL StartConnect();
	BOOL Upload( LPCTSTR pLocal, LPCTSTR pRemote );
	BOOL StartCommand( DWORD cmd );
	void Destroy();
	BOOL Close();
	BOOL Connect( LPCTSTR pAddr, DWORD dwPort = 21 );
	CFtp();
	virtual ~CFtp();

	virtual BOOL InitThread( LPVOID pData );
	virtual BOOL DoThread( LPVOID pData );
	virtual BOOL EndThread( LPVOID pData );

	static BOOL SocketCallback(	CAsyncWinSock* pSocket, DWORD dwUser, 
								WPARAM wParam, LPARAM lParam );
	static BOOL FtpCallback(	CFtpClient* pFtpClient, DWORD dwUser, 
								WPARAM wParam, LPARAM lParam );										

	BOOL IsCommand() { return ( WaitForSingleObject( m_hCmd, 0 ) != WAIT_TIMEOUT ); }

	BOOL IsConnecting() { return m_bConnecting; }
	BOOL IsConnected() { return m_bConnected; }

	void SetUsernamePassword( LPCTSTR user, LPCTSTR password )
	{	m_ftp.SetUser( user ); m_ftp.SetPassword( password ); }

	void SetMessageTarget( HWND hWnd, UINT uMsg )
	{	m_ftp.SetFtpMessageTarget( hWnd, uMsg ); }

	DWORD GetPendingCommand() 
	{	if ( m_bConnecting ) return FTPCMD_CONNECT; return m_dwCmd; }

	void AutoClose( BOOL b ) { m_bAutoClose = b; }

	void SetPassiveMode( BOOL b ) { m_ftp.SetPassiveMode( b ); }

	CFtpClient		m_ftp;

private:

	BOOL			m_bConnecting;
	BOOL			m_bConnected;

	BOOL			m_bAutoClose;

	HANDLE			m_hComplete;
	HANDLE			m_hCmd;
	HANDLE			m_hConnect;
	DWORD			m_dwCmd;

	DWORD			m_dwPort;
	char			m_szAddr[ FTP_STRSIZE ];

	char			m_szLocal[ FTP_STRSIZE ];
	char			m_szRemote[ FTP_STRSIZE ];

	LPBYTE			m_pData;
	DWORD			m_dwData;

};

#endif // !defined(AFX_FTP_H__3A28B48C_1748_434A_8659_ED0E750B1147__INCLUDED_)
