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
#if !defined(AFX_WZFTPPG2_H__C350D46C_D808_4192_B735_054DBFDC078C__INCLUDED_)
#define AFX_WZFTPPG2_H__C350D46C_D808_4192_B735_054DBFDC078C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WzFtpPg2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWzFtpPg2 dialog

class CWzFtpPg2 : public CDialog
{
// Construction
public:
	LRESULT OnPageClose( WPARAM wParam, LPARAM lParam );
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CWzFtpPg2(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWzFtpPg2)
	enum { IDD = IDD_WZFTP_PG2 };
	BOOL	m_bPassive;
	CString	m_sPassword;
	BOOL	m_bRename;
	CString	m_sUsername;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWzFtpPg2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CWzFtpPg2)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WZFTPPG2_H__C350D46C_D808_4192_B735_054DBFDC078C__INCLUDED_)
