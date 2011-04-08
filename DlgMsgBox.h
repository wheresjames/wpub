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
#if !defined(AFX_DLGMSGBOX_H__3F8A246F_4A23_461E_BBB0_2DC44AC8EFE1__INCLUDED_)
#define AFX_DLGMSGBOX_H__3F8A246F_4A23_461E_BBB0_2DC44AC8EFE1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMsgBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgMsgBox dialog

class CDlgMsgBox : public CDialog
{
// Construction
public:
	BOOL ShowMessage( LPCTSTR pTitle, LPCTSTR pMsg, HICON hIcon, DWORD dwType, DWORD dwTimeout = 8 );
	CDlgMsgBox(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgMsgBox)
	enum { IDD = IDD_MESSAGEBOX };
	CStatic	m_msgicon;
	CString	m_sMsg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMsgBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMsgBox)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	DWORD				m_dwType;
	UINT				m_uTimerID;
	HICON				m_hIcon;
	CString				m_sTitle;
	DWORD				m_dwTimeout;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMSGBOX_H__3F8A246F_4A23_461E_BBB0_2DC44AC8EFE1__INCLUDED_)
