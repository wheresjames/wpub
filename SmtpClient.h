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
// SmtpClient.h: interface for the CSmtpClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMTPCLIENT_H__2A953A4B_53BA_44DD_93F2_313DD4C0BB01__INCLUDED_)
#define AFX_SMTPCLIENT_H__2A953A4B_53BA_44DD_93F2_313DD4C0BB01__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define SMTP_STRSIZE			1024

#define SMTP_INIT				0
#define SMTP_HELO				1
#define SMTP_EHLO				2
#define SMTP_AUTH1				3
#define SMTP_AUTH2				4
#define SMTP_AUTH3				5
#define SMTP_AUTHENTICATED		6
#define SMTP_FROM				7
#define SMTP_TO					8
#define SMTP_ENDTO				9
#define SMTP_DATA				10
#define SMTP_MSG				11
#define SMTP_QUIT				12

#define SMTP_NONE				0
#define SMTP_CONNECTING			1
#define SMTP_INITIALIZING		2
#define SMTP_SENDINGMSG			3
#define SMTP_COMPLETE			4
#define SMTP_FAILED				5

#define SMTPAUTH_NONE			0
#define SMTPAUTH_PLAIN			1
#define SMTPAUTH_LOGIN			2
#define SMTPAUTH_DIGESTMD5		3
#define SMTPAUTH_CRAMMD5		4
#define SMTPAUTH_TYPES			5

class CMime;

class CSmtpClient : public CAsyncWinSock  
{
public:
	BOOL IsCommandComplete( LPCTSTR buf, DWORD size );
	BOOL InitMail( CMime *pMime, LPCTSTR pFrom );
	BOOL CheckAuth( LPCTSTR buf, DWORD size );

	BOOL Auth( LPCTSTR pType );

	BOOL AuthPlain();
	BOOL AuthPlainUsername();
	BOOL AuthPlainPassword();

	BOOL AuthLogin();
	BOOL AuthLoginUsername();
	BOOL AuthLoginPassword();

	BOOL AuthCramMd5();
	BOOL AuthCramMd5Challenge( LPCTSTR buf, DWORD size );

	BOOL GetCramMd5( LPSTR pCramMd5, LPCTSTR pUsername, LPCTSTR pPassword, LPCTSTR pText );
	BOOL SendNextEmail();
	void SendMsg();
	void ReleaseMemory();
	BOOL Mail( CMime *pMime, LPCTSTR pFrom, LPCTSTR pServer, HWND hWnd, UINT uMsg );
	BOOL Quit();
	BOOL Data();
	BOOL RcptTo( LPCTSTR pUser );
	BOOL MailFrom( LPCTSTR pUser );
	BOOL Helo( LPCTSTR pDomain );
	BOOL Ehlo( LPCTSTR pDomain );
	void Destroy();
	CSmtpClient();
	virtual ~CSmtpClient();

	virtual BOOL OnConnect( int nErr );
	virtual BOOL OnRead( int nErr );
	virtual BOOL OnWrite( int nErr );

	
	BOOL Connect(LPCTSTR pAddr, DWORD dwPort, HWND hWnd, UINT uMsg);
	BOOL ProcessReply(LPCTSTR buf, DWORD size);

	void SetPort( DWORD p ) { m_dwPort = p; }
	DWORD GetPort() { return m_dwPort; }

	void SetSmtpMessageTarget( HWND hWnd, UINT uMsg )
	{	m_hSmtpWnd = hWnd; m_uSmtpMsg = uMsg; }

	void SendSmtpMsg( DWORD msg )
	{	if ( m_hSmtpWnd != NULL && IsWindow( m_hSmtpWnd ) )
		::SendMessage( m_hSmtpWnd, m_uSmtpMsg, (WPARAM)msg, (LPARAM)this ); }

	DWORD GetMessageSize() { return m_dwBody; }
	DWORD GetBytesSent() { return m_dwBodyPtr; }

	LPCTSTR GetLastError() { return m_szError; }

	void SetUsernamePassword( LPCTSTR user, LPCTSTR pass )
	{	if ( user == NULL ) *m_szUsername = 0;
		else strcpy( m_szUsername, user );
		if ( pass == NULL ) *m_szPassword = 0;
		else strcpy( m_szPassword, pass );
	} 

	void AutoAuthType( BOOL b ) { m_bAutoType = b; }
	void SetAuthType( DWORD type = SMTPAUTH_CRAMMD5 )
	{	m_dwAuthType = type; }


private:

	HWND	m_hSmtpWnd;
	UINT	m_uSmtpMsg;

	volatile DWORD	m_dwState;

	char	*m_pTo;
	char	*m_pFrom;
	char	*m_pBody;
	DWORD	m_dwBody;
	DWORD	m_dwBodyPtr;

	DWORD	m_dwToPtr;

	char	m_szFrom[ SMTP_STRSIZE ]; 

	char	m_szError[ SMTP_STRSIZE ];

	BOOL	m_bAuthenticate;
	DWORD	m_dwAuthType;
	BOOL	m_bAutoType;
	BOOL	m_bTypeDetected;
	BOOL	m_bAuthSupported[ SMTPAUTH_TYPES ];
	char	m_szUsername[ SMTP_STRSIZE ];
	char	m_szPassword[ SMTP_STRSIZE ];

	DWORD	m_dwPort;

	BOOL	m_bSentEnd;

	BOOL	m_bDestroy;
};

#endif // !defined(AFX_SMTPCLIENT_H__2A953A4B_53BA_44DD_93F2_313DD4C0BB01__INCLUDED_)
