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
#if !defined(AFX_PGCHROMABCK_H__6E8C51F5_F4DF_4C8F_A765_36A04138F389__INCLUDED_)
#define AFX_PGCHROMABCK_H__6E8C51F5_F4DF_4C8F_A765_36A04138F389__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgChromaBck.h : header file
//

#include "IconButton.h"

/////////////////////////////////////////////////////////////////////////////
// CPgChromaBck dialog

class CPgChromaBck : public CDialog
{
// Construction
public:
	void FillFileCombo();
	void FillCombos();
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	void DoEnable();
	CPgChromaBck(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgChromaBck)
	enum { IDD = IDD_PG_CHROMABCK };
	CIconButton	m_chooseimage;
	CComboBox	m_comboVideo;
	CComboBox	m_comboPanel;
	CComboBox	m_comboImage;
	int		m_nBckType;
	CString	m_sImage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgChromaBck)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CPgChromaBck)
	virtual BOOL OnInitDialog();
	afx_msg void OnBcktype();
	afx_msg void OnBcktype2();
	afx_msg void OnBcktype3();
	afx_msg void OnSelchangeImage();
	afx_msg void OnSelchangePanel();
	afx_msg void OnSelchangeVideo();
	afx_msg void OnChooseImage();
	afx_msg void OnEditchangeImage();
	afx_msg void OnDropdownImage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGCHROMABCK_H__6E8C51F5_F4DF_4C8F_A765_36A04138F389__INCLUDED_)
