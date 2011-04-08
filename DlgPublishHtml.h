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
#if !defined(AFX_DLGPUBLISHHTML_H__BE01563E_E6CE_44B2_BAA8_CCD39ACA2462__INCLUDED_)
#define AFX_DLGPUBLISHHTML_H__BE01563E_E6CE_44B2_BAA8_CCD39ACA2462__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPublishHtml.h : header file
//

#include "IconButton.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgPublishHtml dialog

class CDlgPublishHtml : public CDialog
{
// Construction
public:
	BOOL FillCombo();
	void DoEnable();
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CDlgPublishHtml(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPublishHtml)
	enum { IDD = IDD_HTMLPUBLISH };
	CComboBox	m_comboFilename;
	CProgressCtrl	m_progress;
	CIconButton	m_choose_dir;
	CComboBox	m_comboFtpServer;
	CComboBox	m_comboFolder;
	CString	m_sDir;
	int		m_nPublish;
	CString	m_sStatus;
	CString	m_sFilename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPublishHtml)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgPublishHtml)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPublish1();
	afx_msg void OnPublish2();
	afx_msg void OnPublish3();
	afx_msg void OnChooseDir();
	afx_msg void OnChangeDir();
	afx_msg void OnChangeFilename();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

	CString			m_sMain;
	char			m_szPath[ CWF_STRSIZE ];
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPUBLISHHTML_H__BE01563E_E6CE_44B2_BAA8_CCD39ACA2462__INCLUDED_)
