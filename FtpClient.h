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
// FtpClient.h: interface for the CFtpClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTPCLIENT_H__B0661231_C45B_4633_9A8D_8D655BD3CF71__INCLUDED_)
#define AFX_FTPCLIENT_H__B0661231_C45B_4633_9A8D_8D655BD3CF71__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define FTP_STRSIZE				1024

#define FTP_INIT				0
#define FTP_USER				1
#define FTP_PASSWORD			2
#define FTP_LOGGEDON			3
#define FTP_LOGGEDOFF			4


#define FTPMSG_IDLE				0
#define FTPMSG_STATUS			1
#define FTPMSG_CMDCOMPLETE		2

#define FTPMODE_ASCII			0
#define FTPMODE_BINARY			1

#define FTPGET_NONE				0
#define FTPGET_CHGDIR			1
#define FTPGET_SETPORT			2
#define FTPGET_GETFILE			3
#define FTPGET_CONNECTING		4

#define FTPPUT_NONE				0
#define FTPPUT_MKDIR			1
#define FTPPUT_CHGDIR			2
#define FTPPUT_DELFILE1			5
#define FTPPUT_SETMODE			3
#define FTPPUT_SETPORT			4
#define FTPPUT_PUTFILE			6
#define FTPPUT_CONNECTING		7
#define FTPPUT_DELFILE2			8
#define FTPPUT_RENAMEFROM		9
#define FTPPUT_RENAMETO			10


// Callback function
class CFtpClient;
typedef BOOL (*FtpCallback)(	CFtpClient* pFtpClient, DWORD dwUser, 
								WPARAM wParam, LPARAM lParam );										


class CFtpClient : public CAsyncWinSock
{
public:

	virtual DWORD Send( LPVOID pData, DWORD len = 0, LPDWORD sent = NULL, DWORD flags = 0 );
	virtual DWORD Recv( LPVOID pData, DWORD len, LPDWORD read = NULL, DWORD flags = 0 );

	BOOL IsEndOfReply( LPCTSTR buf, DWORD size );
	BOOL DeleteFile( LPCTSTR pFile );
	BOOL RenameTo( LPCTSTR pFile );
	BOOL RenameFrom( LPCTSTR pFile );
	BOOL Put( LPBYTE buf, DWORD size, LPCTSTR pRemote );
	BOOL Get( LPBYTE buf, DWORD size, LPCTSTR pRemote );
	BOOL StartPut( LPCTSTR pRemote );
	BOOL CompletePut( LPCTSTR buf, DWORD size );
	BOOL Put( LPCTSTR pLocal, LPCTSTR pRemote );
	BOOL GetPassiveAddress( LPCTSTR buf, DWORD size, LPSTR addr, LPDWORD port );
	BOOL CompleteGet( LPCTSTR buf, DWORD size );
	BOOL StartGet( LPCTSTR pRemote );
	BOOL CreateDirectory( LPCTSTR pDir );
	BOOL Get( LPCTSTR pLocal, LPCTSTR pRemote );
	BOOL Port( DWORD port );
	static BOOL DataCallback( CAsyncWinSock *pSocket, DWORD dwUser, WPARAM wParam, LPARAM lParam );
	static BOOL ListenCallback( CAsyncWinSock *pSocket, DWORD dwUser, WPARAM wParam, LPARAM lParam );
	BOOL Quit();
	BOOL SetMode( DWORD mode );
	BOOL SetDirectory( LPCTSTR pDir );
	BOOL LogOn( DWORD code );
	BOOL ProcessReply( LPCTSTR buf, DWORD size );
	BOOL Connect( LPCTSTR pAddr, DWORD dwPort, HWND hWnd, UINT uMsg );
	void Destroy();
	CFtpClient();
	virtual ~CFtpClient();

	BOOL Connect( LPCTSTR pAddr, DWORD dwPort, AsyncWinSockCallback f, DWORD user )
	{	SetCallback( f, user );
		return Connect( pAddr, dwPort, (HWND)NULL, 0 );
	}


	virtual BOOL OnConnect( int nErr );
	virtual BOOL OnClose( int nErr );
	virtual BOOL OnRead( int nErr );
	virtual BOOL OnWrite( int nErr );

	void SetUser( LPCTSTR ptr ) { strcpy_sz( m_szUser, ptr ); }
	void SetPassword( LPCTSTR ptr ) { strcpy_sz( m_szPassword, ptr ); }

	void SetFtpCallback( FtpCallback f, DWORD user )
	{	m_pCallback = f; m_dwCallback = user; }
	void SetFtpMessageTarget( HWND hWnd, UINT uMsg )
	{	m_hFtpWnd = hWnd; m_uFtpMsg = uMsg; }
	void SendFtpMessage( WPARAM wParam, LPARAM lParam )
	{	if ( m_pCallback != NULL )
			m_pCallback( this, m_dwCallback, wParam, lParam );
		if ( m_hFtpWnd != NULL && ::IsWindow( m_hFtpWnd ) )
			::SendMessage( m_hFtpWnd, m_uFtpMsg, wParam, lParam );
	}

	void SetDataPort( DWORD port ) { m_dwDataPort = port; }

	void SetPassiveMode( BOOL b ) { m_bPassiveFtp = b; }

	void SetDataBuffer( LPBYTE buf, DWORD size )
	{	m_pData = buf; m_dwData = size; }

private:  

	LPBYTE			m_pData;
	DWORD			m_dwData;
	DWORD			m_dwPtr;

	FtpCallback		m_pCallback;
	DWORD			m_dwCallback;	

	BOOL			m_bPassiveFtp;

	DWORD			m_dwDataPort;	

	char			m_szLocal[ FTP_STRSIZE ];
	char			m_szRemote[ FTP_STRSIZE ];
	char			m_szTempRemote[ FTP_STRSIZE ];

	volatile DWORD	m_dwGetState;
	volatile DWORD	m_dwPutState;

	HWND			m_hFtpWnd;
	UINT			m_uFtpMsg;

	char			m_szUser[ FTP_STRSIZE ];
	char			m_szPassword[ FTP_STRSIZE ];

	DWORD			m_dwFtpStatus;

	int				m_iErr[ FD_MAX_EVENTS ];

	CWinFile		m_file;

	CAsyncWinSock	m_listen;
	CAsyncWinSock	m_data;

	BOOL			m_bChgDir;
};

#endif // !defined(AFX_FTPCLIENT_H__B0661231_C45B_4633_9A8D_8D655BD3CF71__INCLUDED_)
