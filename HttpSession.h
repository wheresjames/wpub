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
// HttpSession.h: interface for the CHttpSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPSESSION_H__3E0A5FD3_A03E_475C_80D8_8567DFCBD06E__INCLUDED_)
#define AFX_HTTPSESSION_H__3E0A5FD3_A03E_475C_80D8_8567DFCBD06E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define HRTYPE_GET		0x00000001
#define HRTYPE_POST		0x00000002
#define HRTYPE_HEAD		0x00000004

#define HRDOCTYPE_HTTP	0x00000001

#define HTTPMSG_NONE	( CWINSOCK_STATUS_USER | 0 )
#define HTTPMSG_ERROR	( CWINSOCK_STATUS_USER | 1 )
#define HTTPMSG_URL		( CWINSOCK_STATUS_USER | 2 )
#define HTTPMSG_CLOSE	( CWINSOCK_STATUS_USER | 3 )

#define HTTPERR_OK				200
#define HTTPERR_FORBIDDEN		403
#define HTTPERR_NOT_FOUND		404
#define HTTPERR_SERVER_ERROR	500

typedef struct tagHTTPREQUEST
{
	DWORD	type;
	char	doc[ 4096 ];
	char	file[ 4096 ];
	DWORD	doctype;
	DWORD	ver;
	DWORD	len;
	DWORD	ip;
	DWORD	port;

	CVar	params;
	CVar	var;

} HTTPREQUEST, *LPHTTPREQUEST; // end typedef struct


#define SESSIONBUFFERSIZE		4096


class CHttpSession : public CAsyncWinSock  
{
public:
	
	static DWORD WriteGallery( LPSTR out, DWORD op, DWORD omax, CVar *params );
	static DWORD WriteChat( LPSTR out, DWORD op, DWORD omax, CVar *params);

	BOOL DoRead();
	BOOL DoChat( LPHTTPREQUEST phr );
	BOOL SendHttpFile( LPHTTPREQUEST phr );
	BOOL LocateFile( LPHTTPREQUEST phr );
	BOOL DoCgi( LPHTTPREQUEST phr, LPCTSTR pCgi, LPCTSTR pParams );
	BOOL CheckPassword( LPHTTPREQUEST phr );
	BOOL Extract( LPCTSTR pResource, LPCTSTR pFile );
	BOOL RunAdmin( LPHTTPREQUEST phr );
	BOOL SendFile( LPCTSTR pFile, LPBYTE buf = NULL, DWORD size = 0 );
	BOOL SendErrorMsg( LPCTSTR pMsg );
	BOOL TranslatePath( LPCTSTR pReq, LPSTR pFolder );
	void ZeroHTTPREQUEST( LPHTTPREQUEST phr );
	BOOL Log( DWORD status, LPHTTPREQUEST phr );
	BOOL SendImage( LPCTSTR pImg, LPHTTPREQUEST phr );
	BOOL SendFile( LPHTTPREQUEST phr );
	static LPCTSTR GetErrString( DWORD err );
	BOOL SendHeader();
	BOOL SendError( DWORD err );
	BOOL SendContentType( LPCTSTR pExt );
	BOOL ProcessRequest( LPHTTPREQUEST phr );
	BOOL Parse( LPBYTE buf, DWORD size, LPHTTPREQUEST phr );
	BOOL OnRead( int nErr );
	CHttpSession();
	virtual ~CHttpSession();

	static CAsyncWinSock* sCreate();
	static void sDelete( CAsyncWinSock *ptr );

private:

	BYTE		m_pData[ SESSIONBUFFERSIZE ];
	DWORD		m_dwData;

	CWinImg		m_img;

};

#endif // !defined(AFX_HTTPSESSION_H__3E0A5FD3_A03E_475C_80D8_8567DFCBD06E__INCLUDED_)
