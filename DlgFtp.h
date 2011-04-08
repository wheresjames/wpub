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
#if !defined(AFX_DLGFTP_H__6ACBE866_536C_488A_B75A_978E88FC700A__INCLUDED_)
#define AFX_DLGFTP_H__6ACBE866_536C_488A_B75A_978E88FC700A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFtp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgFtp dialog

class CDlgFtp : public CDialog
{
// Construction
public:
	void UpdateHttpPath();
	void DoEnable();
	void UpdateControls();
	void FillListBox();
	LRESULT OnSave( WPARAM wParam, LPARAM lParam );
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CDlgFtp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgFtp)
	enum { IDD = IDD_FTP };
	CListCtrl	m_list;
	CString	m_sAddress;
	CString	m_sFolder;
	BOOL	m_bPassive;
	CString	m_sPassword;
	CString	m_sUsername;
	CString	m_sName;
	CString	m_sHttpPath;
	BOOL	m_bRename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFtp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFtp)
	virtual void OnCancel(){}
	virtual void OnOK(){}
	virtual BOOL OnInitDialog();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeName();
	afx_msg void OnChangeAddress();
	afx_msg void OnChangeFolder();
	afx_msg void OnChangeHttppath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	CImageList		m_images;

	BOOL			m_bUpdateHttpPath;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFTP_H__6ACBE866_536C_488A_B75A_978E88FC700A__INCLUDED_)
