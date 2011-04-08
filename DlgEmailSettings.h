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
#if !defined(AFX_DLGEMAILSETTINGS_H__0C8B9233_CB7E_42C2_856E_4314AC1DFCF2__INCLUDED_)
#define AFX_DLGEMAILSETTINGS_H__0C8B9233_CB7E_42C2_856E_4314AC1DFCF2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgEmailSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgEmailSettings dialog

class CDlgEmailSettings : public CDialog
{
// Construction
public:
	void UpdateControls();
	void FillListBox();
	void FillCombo();
	void DoEnable();
	LRESULT OnSave( WPARAM wParam, LPARAM lParam );
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CDlgEmailSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgEmailSettings)
	enum { IDD = IDD_EMAILSETTINGS };
	CListCtrl	m_list;
	CComboBox	m_comboAuthType;
	CString	m_sServer;
	CString	m_sUsername;
	CString	m_sPassword;
	BOOL	m_bLogin;
	BOOL	m_bAuthDetect;
	CString	m_sName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEmailSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEmailSettings)
	virtual BOOL OnInitDialog();
	virtual void OnOK(){}
	virtual void OnCancel(){}
	afx_msg void OnAuthdetect();
	afx_msg void OnLogin();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnChangeName();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	CImageList	m_images;

	DWORD		m_dwAuthType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEMAILSETTINGS_H__0C8B9233_CB7E_42C2_856E_4314AC1DFCF2__INCLUDED_)
