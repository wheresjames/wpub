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
#if !defined(AFX_DLGSERVER_H__5E3F04C9_FFED_4819_AC0F_AC0D054645A5__INCLUDED_)
#define AFX_DLGSERVER_H__5E3F04C9_FFED_4819_AC0F_AC0D054645A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgServer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgServer dialog

class CDlgServer : public CDialog
{
// Construction
public:
	void DoEnable();
	void ShowAddress();
	LRESULT OnSave( WPARAM wParam, LPARAM lParam );
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CDlgServer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgServer)
	enum { IDD = IDD_SERVER };
	CComboBox	m_comboPort;
	CString	m_sMaxUsers;
	CString	m_sPort;
	CString	m_sAddress;
	CString	m_sRemotePage;
	CString	m_sRemotePassword;
	CString	m_sHttpPassword;
	BOOL	m_bRemote;
	BOOL	m_bHttp;
	BOOL	m_bHttpEnable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgServer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgServer)
	virtual void OnCancel(){}
	virtual void OnOK(){}
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeMaxusers();
	afx_msg void OnEditchangePort();
	afx_msg void OnSelchangePort();
	afx_msg void OnHttp();
	afx_msg void OnRemote();
	afx_msg void OnChangeHttpPassword();
	afx_msg void OnEnablehttp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSERVER_H__5E3F04C9_FFED_4819_AC0F_AC0D054645A5__INCLUDED_)
