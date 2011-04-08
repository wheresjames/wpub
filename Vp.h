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
// Vp.h : main header file for the VP application
//

#if !defined(AFX_VP_H__88F3AF63_CD7C_427D_8995_013754964E0D__INCLUDED_)
#define AFX_VP_H__88F3AF63_CD7C_427D_8995_013754964E0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


#include "resource.h"       // main symbols

#include "DlgTabFrame.h"
#include "Fx.h"
#include "LogFile.h"
#include "UserList.h"
#include "PubList.h"
#include "DlgImgFxEditor.h"
#include "SndThread.h"

/////////////////////////////////////////////////////////////////////////////
// CVpApp:
// See Vp.cpp for the implementation of this class
//

class CVpApp : public CWinApp
{
public:
	void SaveSettings();
	CVpApp();
	~CVpApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVpApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CVpApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:


public:
	void KillProperties();
	void SmtpLog( BOOL bRx, LPCTSTR pServer, LPCTSTR pData, DWORD len = 0 );
	void Log( CWinFile *f, BOOL bRx, LPCTSTR pPre, LPCTSTR pData, DWORD len = 0 );
	void FtpLog( BOOL bRx, LPCTSTR pServer, LPCTSTR pData, DWORD len = 0 );
	BOOL RegistrationDlg();
	BOOL Restart();
	BOOL Register(void);
	void LoadSettings();
	CFx* GetActiveFx();
	void ReGen( BOOL bAll = FALSE );
	void RefreshProperties();
	void ShowProperties( DWORD i );
	void AddRecent( LPCTSTR pFile );
	void ExpectDelays( BOOL b ) { m_bExpectDelays = b; }

	BOOL				m_bRestart;

	CDlgTabFrame		m_dlgproperties;

	CCfgFile			m_cfg;

	LPFX				m_fxProperties;

	HANDLE				m_hMutex;

	CLogFile			m_serverlog;

	CReg				m_websettings;

	CReg				m_htmlvars;

	CWinDc				m_screenshot;

	CUserList			m_webusers;

	CCfgFile			m_ftpservers;

	CCfgFile			m_emailservers;

	CReg				m_cgi;

	CReg				m_videosettings;

	CReg				m_chat;
					
	CPubList			m_publist;

	CDlgImgFxEditor		m_dlgimgfxeditor;

	CUserList			m_authusers;
	CUserList			m_authadmin;

	CWinFile			m_ftplog;
	CWinFile			m_smtplog;

	CSndThread			m_sndthread;

	BOOL				m_bExpectDelays;

//	CWinImg				m_testimg;

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VP_H__88F3AF63_CD7C_427D_8995_013754964E0D__INCLUDED_)
