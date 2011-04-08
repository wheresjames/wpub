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
#if !defined(AFX_DLGGENERAL_H__38E2A5C8_9232_4EA8_B36F_0077866F0A7A__INCLUDED_)
#define AFX_DLGGENERAL_H__38E2A5C8_9232_4EA8_B36F_0077866F0A7A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgGeneral.h : header file
//

#include "XpObject.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgGeneral dialog

class CDlgGeneral : public CDialog
{
// Construction
public:
	LRESULT OnSave( WPARAM wParam, LPARAM lParam );
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CDlgGeneral(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgGeneral)
	enum { IDD = IDD_GENERAL };
	CXpObject	m_imgClose;
	BOOL	m_bStartup;
	BOOL	m_bTrayIcon;
	int		m_nAction;
	BOOL	m_bToolbar;
	BOOL	m_bAutoUpdate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgGeneral)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnAction();
	afx_msg void OnAction2();
	afx_msg void OnAction3();
	afx_msg void OnChangeFramerate();
	afx_msg void OnStartup();
	afx_msg void OnTrayicon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGGENERAL_H__38E2A5C8_9232_4EA8_B36F_0077866F0A7A__INCLUDED_)
