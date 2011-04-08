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
#if !defined(AFX_WZEMAILPG2_H__780340E2_EA62_4806_9501_1FE4CD5B686C__INCLUDED_)
#define AFX_WZEMAILPG2_H__780340E2_EA62_4806_9501_1FE4CD5B686C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WzEmailPg2.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWzEmailPg2 dialog

class CWzEmailPg2 : public CDialog
{
// Construction
public:
	LRESULT OnPageClose( WPARAM wParam, LPARAM lParam );
	void DoEnable();
	void FillCombo();
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CWzEmailPg2(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWzEmailPg2)
	enum { IDD = IDD_WZEMAIL_PG2 };
	CComboBox	m_comboAuthType;
	BOOL	m_bLogin;
	CString	m_sPassword;
	CString	m_sUsername;
	BOOL	m_bAuthDetect;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWzEmailPg2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CWzEmailPg2)
	virtual BOOL OnInitDialog();
	afx_msg void OnLogin();
	afx_msg void OnAuthdetect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	DWORD		m_dwAuthType;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WZEMAILPG2_H__780340E2_EA62_4806_9501_1FE4CD5B686C__INCLUDED_)
