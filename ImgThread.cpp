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
// ImgThread.cpp: implementation of the CImgThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vp.h"
#include "ImgThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImgThread::CImgThread()
{
	*m_szDir = 0;
	m_hWnd = NULL;
	m_uMsg = 0;
	m_hFind = INVALID_HANDLE_VALUE;
}

CImgThread::~CImgThread()
{

}

BOOL CImgThread::DoThread(LPVOID pData)
{
	if ( m_hFind == NULL ) return FALSE;
	if ( !IsWindow( m_hWnd ) ) return FALSE;

	// Do we need to copy this file?
	if (	*m_wfd.cFileName != NULL &&
			strcmp( m_wfd.cFileName, "." ) &&
			strcmp( m_wfd.cFileName, ".." ) && 
			( m_wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
	{
		DWORD type = CWinImg::GetFileType( m_wfd.cFileName );

		// Add this string if recognized image type
		if ( type != MAXDWORD )
		{
			CWinImg img;
			char full[ CWF_STRSIZE ];
			CWinFile::BuildPath( full, m_szDir, m_wfd.cFileName );
			
			// Add image
			if ( img.Load( full ) )
			{
				// Check for stop
				if ( IsStopping() ) return FALSE;

				// Send message
				SendMessage( m_hWnd, m_uMsg, 0, (LPARAM)&img );

			} // end if

		} // end if

	} // end if

	// Find next file
	if ( !FindNextFile( m_hFind, &m_wfd ) )
		return FALSE;

	return TRUE;
}

BOOL CImgThread::InitThread(LPVOID pData)
{
	char search[ CWF_STRSIZE ];
	CWinFile::BuildPath( search, m_szDir, "*.*" );

	// Get first file
	m_hFind = FindFirstFile( search, &m_wfd );
	if ( m_hFind == INVALID_HANDLE_VALUE ) return FALSE;
	
	return TRUE;
}

BOOL CImgThread::EndThread(LPVOID pData)
{
	// Funs over
	if ( m_hFind != INVALID_HANDLE_VALUE ) 
	{	FindClose( m_hFind ); 
		m_hFind = NULL; 
	} // end if

	return TRUE;
}

BOOL CImgThread::EnumImages(LPCTSTR pDir, HWND hWnd, UINT uMsg)
{
	if ( pDir == NULL ) return FALSE;
	if ( !IsWindow( hWnd ) ) return FALSE;

	strcpy( m_szDir, pDir );
	m_hWnd = hWnd;
	m_uMsg = uMsg;

	StartThread();

	return TRUE;
}
