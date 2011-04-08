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
// LogThread.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "LogThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogThread

IMPLEMENT_DYNCREATE(CLogThread, CWinThread)

CLogThread::CLogThread()
{
}

CLogThread::~CLogThread()
{
}

BOOL CLogThread::InitInstance()
{
	// Create the window
	if ( !::IsWindow( m_dlglogtarget.GetSafeHwnd() ) )
		m_dlglogtarget.Create( CDlgLogTarget::IDD, NULL );
	m_dlglogtarget.ShowWindow( SW_HIDE );
	
	return TRUE;
}

int CLogThread::ExitInstance()
{
	if ( ::IsWindow( m_dlglogtarget.GetSafeHwnd() ) )
		m_dlglogtarget.DestroyWindow();
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CLogThread, CWinThread)
	//{{AFX_MSG_MAP(CLogThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogThread message handlers
