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
// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__A6FDE549_FD6C_48B4_97B2_432472A631F9__INCLUDED_)
#define AFX_LOG_H__A6FDE549_FD6C_48B4_97B2_432472A631F9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#define MAXLOGSIZE		1000

#ifdef _DEBUG

#define _Log( flags, title, msg )								\
	{															\
		try {													\
		char deb[ 512 + MAX_PATH ];								\
		wsprintf( deb,	"File :  %s \r\n"						\
						"Line :  %lu \r\n"						\
						"Compiled :  %s \r\n"					\
						"Modified :  %s \r\n",					\
						(LPCTSTR)__FILE__,						\
						(ULONG)__LINE__,						\
						(LPCTSTR)__DATE__ " " __TIME__,			\
						(LPCTSTR)__TIMESTAMP__ );				\
		CLog::AddMessage( title, msg, deb, flags );				\
	} catch (... ) {} }

#else

#define _Log( flags, title, msg )								\
	{ try { CLog::AddMessage( title, msg, NULL, flags ); } catch( ... ) {} }

#endif

#define _ShowLog CLog::ShowLog

#define _SetLogFile CLog::SetLogFile

typedef struct tagLOGMSGINFO
{
	DWORD			dwFlags;
	char			szTitle[ 1024 ];
	char			szMessage[ 1024 ];
	char			szLocation[ 1024 ];
	char			szTime[ 1024 ];

	tagLOGMSGINFO	*pNext;

} LOGMSGINFO; // end typedef struct
typedef LOGMSGINFO* LPLOGMSGINFO;

class CDlgMsgView;
class CWinFile;

class CLog  
{
public:
	static BOOL SaveToFile( CWinFile *pFile, LPLOGMSGINFO pLmi );
	static BOOL SetLogFile( LPCTSTR pFile );
	static void ShowLog();
	static BOOL AddMessage( LPCTSTR pTitle, LPCTSTR pMessage, LPCTSTR pLocation, DWORD dwFlags = MB_ICONINFORMATION );
	void Destroy();
	LPLOGMSGINFO InsertMessage( LPCTSTR pTitle, LPCTSTR pMessage, LPCTSTR pLocation, DWORD dwFlags = MB_ICONINFORMATION );
	CLog();
	virtual ~CLog();

	static LPCTSTR __cdecl Format( LPSTR pFormat, ... );
	static LPCTSTR vFormat( LPSTR pFormat, va_list argptr );

	BOOL IsValidMsgInfo( LPLOGMSGINFO ptr );

	DWORD Size() { return m_dwSize; }
	DWORD GetNumEntries() { return Size(); }
	LPLOGMSGINFO GetNext( LPLOGMSGINFO ptr )
	{	if ( ptr != NULL ) return ptr->pNext; return m_pHead; }

private:

	static CLog			m_log;
	CDlgMsgView			*m_dlg;
	CWinFile			*m_file;

	static char			m_msgbuf[ 4096 ];

	DWORD				m_dwSize;
	LPLOGMSGINFO		m_pHead;
	LPLOGMSGINFO		m_pTail;

	static char			m_lastTitle[ 4096 ];
	static char			m_lastMessage[ 4096 ];
	static char			m_lastLocation[ 4096 ];
	static DWORD		m_lastFlags;
	static DWORD		m_lastTime;
	static CTlLock		m_lock;

};

#endif // !defined(AFX_LOG_H__A6FDE549_FD6C_48B4_97B2_432472A631F9__INCLUDED_)
