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
#if !defined(AFX_DLGSIMPLESETTINGS_H__6B6706AF_B027_4896_A963_C99BF9C5923B__INCLUDED_)
#define AFX_DLGSIMPLESETTINGS_H__6B6706AF_B027_4896_A963_C99BF9C5923B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSimpleSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSimpleSettings dialog

class CDlgSimpleSettings : public CDialog
{
// Construction
public:
	void FillFtpCombo();
	void FillImageCombo();
	void DoEnable();
	CDlgSimpleSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSimpleSettings)
	enum { IDD = IDD_SIMPLESETTINGS };
	CComboBox	m_comboImage;
	CComboBox	m_comboFtpServer;
	BOOL	m_bEmail;
	BOOL	m_bFtp;
	CString	m_sFtpFilename;
	BOOL	m_bPrint;
	BOOL	m_bSetup;
	BOOL	m_bSave;
	CString	m_xMax;
	CString	m_yMax;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSimpleSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSimpleSettings)
	virtual BOOL OnInitDialog();
	afx_msg void OnFtp();
	virtual void OnOK();
	afx_msg void OnFtpedit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSIMPLESETTINGS_H__6B6706AF_B027_4896_A963_C99BF9C5923B__INCLUDED_)
