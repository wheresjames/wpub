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
#if !defined(AFX_DLGUPLOAD_H__02B45AC8_DE41_4F7A_8F2A_8BBCF2C41BF8__INCLUDED_)
#define AFX_DLGUPLOAD_H__02B45AC8_DE41_4F7A_8F2A_8BBCF2C41BF8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgUpload.h : header file
//

//#include "NetFile.h"
#include "Ftp.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgUpload dialog

class CCfgFile;
class CDlgUpload : public CDialog
{
// Construction
public:
	BOOL Upload( char* *buf, LPDWORD size, char* *fname, CWnd *pwnd = NULL, BOOL bModeless = FALSE );
	LRESULT OnFtp( WPARAM wParam, LPARAM lParam );
	void SetStatus( LPCTSTR pStatus );
	void OnNetFile( WPARAM wParam, LPARAM lParam );
	BOOL SubstituteStrings( LPCTSTR pFile, LPCTSTR pSub );
	BOOL UploadFile();
	BOOL Upload( LPCTSTR pPath, LPCTSTR pFtp, CWnd* pwnd, BOOL bModeless );
	CDlgUpload(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgUpload)
	enum { IDD = IDD_UPLOAD };
	CProgressCtrl	m_progress;
	CString	m_sStatus;
	CString	m_sStatusBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgUpload)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgUpload)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	WIN32_FIND_DATA		m_fd;
	HANDLE				m_hFind;

	char*				*m_buf;
	LPDWORD				m_size;
	char*				*m_fname;
	DWORD				m_ptr;

	DWORD				m_dwFileCount;
	DWORD				m_dwPos;

	char				m_szRoot[ MAX_PATH ];
//	char				m_szTemp[ MAX_PATH ];

	CFtp				m_ftp;
//	CNetFile			m_netfile;

	CCfgFile			m_sub;

	CString				m_sFtp;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGUPLOAD_H__02B45AC8_DE41_4F7A_8F2A_8BBCF2C41BF8__INCLUDED_)
