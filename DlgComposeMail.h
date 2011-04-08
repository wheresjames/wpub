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
#if !defined(AFX_DLGCOMPOSEMAIL_H__711A3D74_161C_4572_BF2E_42DE3E0CF1D9__INCLUDED_)
#define AFX_DLGCOMPOSEMAIL_H__711A3D74_161C_4572_BF2E_42DE3E0CF1D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgComposeMail.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgComposeMail dialog

class CDlgComposeMail : public CDialog
{
// Construction
public:
	void Size();
	CDlgComposeMail(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgComposeMail)
	enum { IDD = IDD_COMPOSEMAIL };
	CComboBox	m_comboFrom;
	CComboBox	m_comboTo;
	CString	m_sFrom;
	CString	m_sMsg;
	CString	m_sSubject;
	CString	m_sTo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgComposeMail)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgComposeMail)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCOMPOSEMAIL_H__711A3D74_161C_4572_BF2E_42DE3E0CF1D9__INCLUDED_)
