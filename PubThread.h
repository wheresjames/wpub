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
// PubThread.h: interface for the CPubThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PUBTHREAD_H__EBE9852E_A5C4_4E90_9BAB_47DB5FBABBD4__INCLUDED_)
#define AFX_PUBTHREAD_H__EBE9852E_A5C4_4E90_9BAB_47DB5FBABBD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PubList.h"
#include "PubImgList.h"

class CFtp;
class CPubThread : public CThread  
{
public:
	BOOL WriteAviFrame( LPPUBINFO ppi, CWinAvi *pavi, LPPUBIMGINFO ppii, LPCTSTR pFilename );
	BOOL GetThumbnailRect( LPPUBINFO ppi, LPRECT pRect );
	static BOOL OnNfCallback( DWORD dwUser, WPARAM wParam, LPARAM lParam );
	BOOL FtpImage( LPCTSTR pServer, CNetFile *pNf, LPBYTE buf, DWORD size, LPCTSTR pPath );
	BOOL FtpImage( LPCTSTR pServer, CFtp *pFtp, LPBYTE buf, DWORD size, LPCTSTR pPath );
	BOOL IsMotion( LPPUBINFO ppi );

	BOOL GetThumbnail( LPPUBINFO ppi, CWinImg *pImg, long iw, long ih );
	
	static BOOL AddTimestamp( LPSTR str, LPCTSTR tmpl );
	static BOOL AddIndex( LPSTR pFile, LPCTSTR pTemplate, DWORD index );
	static BOOL GetFileName( LPPUBINFO ppi, BOOL bPreview = FALSE, LPSTR fname = NULL, LPSTR tfname = NULL, DWORD index = MAXDWORD );

	BOOL Disk( LPPUBINFO ppi );
	BOOL Email( LPPUBINFO ppi );
	BOOL Ftp( LPPUBINFO ppi );
	virtual BOOL EndThread( LPVOID pData );
	virtual BOOL DoThread( LPVOID pData );
	virtual BOOL InitThread( LPVOID pData );
	CPubThread();
	virtual ~CPubThread();

private:

	BOOL			m_bReset;

	DWORD			m_dwMotionFrame;

};

#endif // !defined(AFX_PUBTHREAD_H__EBE9852E_A5C4_4E90_9BAB_47DB5FBABBD4__INCLUDED_)
