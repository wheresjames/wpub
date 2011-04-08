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
#if !defined(AFX_PGBCKIMAGE_H__24659C47_A428_4AE2_A1CB_6125E01CA489__INCLUDED_)
#define AFX_PGBCKIMAGE_H__24659C47_A428_4AE2_A1CB_6125E01CA489__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgBckImage.h : header file
//

#include "ColorBox.h"

/////////////////////////////////////////////////////////////////////////////
// CPgBckImage dialog

class CPgBckImage : public CDialog
{
// Construction
public:
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	void GetDrivers();
	void FillComboBox();
	void DoEnable();
	CPgBckImage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgBckImage)
	enum { IDD = IDD_PG_BCKIMG };
	CColorBox	m_bckcolor;
	CComboBox	m_comboVideo;
	CComboBox	m_comboImage;
	int		m_nBckType;
	CString	m_sImage;
	BOOL	m_bMotionOverlay;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgBckImage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CPgBckImage)
	virtual BOOL OnInitDialog();
	afx_msg void OnType1();
	afx_msg void OnType2();
	afx_msg void OnType3();
	afx_msg void OnEditchangeImage();
	afx_msg void OnSelchangeImage();
	afx_msg void OnChooseImage();
	afx_msg void OnChooseColor();
	afx_msg void OnMotionoverlay();
	afx_msg void OnDropdownImage();
	afx_msg void OnSelchangeVideo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CString	m_sImageFile;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGBCKIMAGE_H__24659C47_A428_4AE2_A1CB_6125E01CA489__INCLUDED_)
