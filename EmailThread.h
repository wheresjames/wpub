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
// EmailThread.h: interface for the CEmailThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EMAILTHREAD_H__DA8F400D_8C45_4F90_BEBE_608EF60D3D0C__INCLUDED_)
#define AFX_EMAILTHREAD_H__DA8F400D_8C45_4F90_BEBE_608EF60D3D0C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Thread.h"
#include "SmtpClient.h"




class CEmailThread : public CThread  
{
public:
	static BOOL SocketCallback( CAsyncWinSock *pSocket, DWORD dwUser, WPARAM wParam, LPARAM lParam );
	BOOL Email( LPCTSTR pImg, LPCTSTR pDiskFile, LPCTSTR pTo, LPCTSTR pFrom, LPCTSTR pSubject, LPCTSTR pFile,  LPCTSTR pServer, LPCTSTR pUsername, LPCTSTR pPassword, LPCTSTR pBody, DWORD dwLogin, DWORD dwAuthDetect, DWORD dwAuthType );
	BOOL GetImage( LPCTSTR pFile, LPBYTE *buf, LPDWORD size );
	void Email();
	BOOL SendEmail();
	CEmailThread();
	virtual ~CEmailThread();

	virtual BOOL InitThread( LPVOID pData );
	virtual BOOL DoThread( LPVOID pData );
	virtual BOOL EndThread( LPVOID pData );

private:

	CSmtpClient		m_smtp;
	HANDLE			m_hEmail;

	// Message data
	CStr			m_to;
	CStr			m_from;
	CStr			m_subject;
	CStr			m_fname;
	CStr			m_body;

	// Server data
	CStr			m_server;
	CStr			m_username;
	CStr			m_password;

	DWORD			m_login;
	DWORD			m_authdetect;
	DWORD			m_authtype;

	// Image
	CStr			m_sImg;
	TMem< BYTE >	m_mem;

};

#endif // !defined(AFX_EMAILTHREAD_H__DA8F400D_8C45_4F90_BEBE_608EF60D3D0C__INCLUDED_)
