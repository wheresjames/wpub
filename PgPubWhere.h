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
#if !defined(AFX_PGPUBWHERE_H__990A558E_5A26_4D10_9635_05E1A4AFD663__INCLUDED_)
#define AFX_PGPUBWHERE_H__990A558E_5A26_4D10_9635_05E1A4AFD663__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgPubWhere.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPgPubWhere dialog

class CPgPubWhere : public CDialog
{
// Construction
public:
	void DoEnable();
	void FillCombo();
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CPgPubWhere(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgPubWhere)
	enum { IDD = IDD_PUBWHERE };
	CComboBox	m_comboFtp;
	CComboBox	m_comboEmail;
	int		m_nType;
	CString	m_sPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgPubWhere)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CPgPubWhere)
	virtual BOOL OnInitDialog();
	afx_msg void OnType();
	afx_msg void OnType2();
	afx_msg void OnType3();
	afx_msg void OnSelchangeFtp();
	afx_msg void OnSelchangeEmail();
	afx_msg void OnEmailmsg();
	afx_msg void OnChoosePath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGPUBWHERE_H__990A558E_5A26_4D10_9635_05E1A4AFD663__INCLUDED_)
