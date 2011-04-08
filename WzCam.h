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
#if !defined(AFX_WZCAM_H__3F9F53EC_2897_4909_A2CF_A22F8F3C5DBF__INCLUDED_)
#define AFX_WZCAM_H__3F9F53EC_2897_4909_A2CF_A22F8F3C5DBF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WzCam.h : header file
//

#include "IconButton.h"

/////////////////////////////////////////////////////////////////////////////
// CWzCam dialog

class CWzCam : public CDialog
{
// Construction
public:
	void DoEnable();
	void FillFileCombo();
	LRESULT OnSave( WPARAM wParam, LPARAM lParam );
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	void FillCombo();
	CWzCam(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWzCam)
	enum { IDD = IDD_WZ_CAM };
	CIconButton	m_chooseImage;
	CComboBox	m_comboImage;
	CComboBox	m_combo;
	CString	m_sName;
	int		m_nBckType;
	CString	m_sImage;
	BOOL	m_bCustomSize;
	CString	m_sHeight;
	CString	m_sWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWzCam)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK(){}
	virtual void OnCancel(){}
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWzCam)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeImagefile();
	afx_msg void OnEditchangeImagefile();
	afx_msg void OnBcktype();
	afx_msg void OnBcktype2();
	afx_msg void OnBcktype3();
	afx_msg void OnCustomsize();
	afx_msg void OnChooseImage();
	afx_msg void OnDropdownImagefile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	CString		m_sImageFile;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WZCAM_H__3F9F53EC_2897_4909_A2CF_A22F8F3C5DBF__INCLUDED_)
