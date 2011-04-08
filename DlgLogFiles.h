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
#if !defined(AFX_DLGLOGFILES_H__D93036BD_7E55_4BDE_A50E_FFC549E0A6B4__INCLUDED_)
#define AFX_DLGLOGFILES_H__D93036BD_7E55_4BDE_A50E_FFC549E0A6B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLogFiles.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgLogFiles dialog

class CDlgLogFiles : public CDialog
{
// Construction
public:
	void DoEnable();
	LRESULT OnSave( WPARAM wParam, LPARAM lParam );
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CDlgLogFiles(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgLogFiles)
	enum { IDD = IDD_LOGFILES };
	BOOL	m_bFtp;
	CString	m_sFtpLog;
	BOOL	m_bSmtp;
	CString	m_sSmtpLog;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLogFiles)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CDlgLogFiles)
	virtual BOOL OnInitDialog();
	afx_msg void OnFtp();
	afx_msg void OnSmtp();
	afx_msg void OnOpenftp();
	afx_msg void OnOpensmtp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLOGFILES_H__D93036BD_7E55_4BDE_A50E_FFC549E0A6B4__INCLUDED_)
