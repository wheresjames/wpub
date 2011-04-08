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
#if !defined(AFX_WZFTPPG1_H__5D480640_821C_499E_8648_098FA31A7588__INCLUDED_)
#define AFX_WZFTPPG1_H__5D480640_821C_499E_8648_098FA31A7588__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WzFtpPg1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWzFtpPg1 dialog

class CWzFtpPg1 : public CDialog
{
// Construction
public:
	LRESULT OnPageChange( WPARAM wParam, LPARAM lParam );
	void UpdateHttpPath( BOOL bVerify = FALSE );
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CWzFtpPg1(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWzFtpPg1)
	enum { IDD = IDD_WZFTP_PG1 };
	CString	m_sAddr;
	CString	m_sFolder;
	CString	m_sHttpPath;
	CString	m_sName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWzFtpPg1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CWzFtpPg1)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeAddr();
	afx_msg void OnChangeFolder();
	afx_msg void OnChangeHttppath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	BOOL				m_bUpdateHttpPath;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WZFTPPG1_H__5D480640_821C_499E_8648_098FA31A7588__INCLUDED_)
