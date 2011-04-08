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
#if !defined(AFX_PGIMAGE_H__874877EC_C35B_4280_8650_C684A7CE3E39__INCLUDED_)
#define AFX_PGIMAGE_H__874877EC_C35B_4280_8650_C684A7CE3E39__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PgImage.h : header file
//

#include "ColorBox.h"

/////////////////////////////////////////////////////////////////////////////
// CPgImage dialog

class CPgImage : public CDialog
{
// Construction
public:
	BOOL FillCombo();
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CPgImage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgImage)
	enum { IDD = IDD_PG_IMAGE };
	CComboBox	m_comboFiles;
	CString	m_sTranslucency;
	BOOL	m_bRealSize;
	CString	m_sFilename;
	BOOL	m_bAspect;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgImage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPgImage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChooseFilename();
	afx_msg void OnChangeTranslucency();
	afx_msg void OnSelchangeFiles();
	virtual void OnOK(){}
	virtual void OnCancel(){}
	afx_msg void OnRealsize();
	afx_msg void OnEffects();
	afx_msg void OnEditchangeFiles();
	afx_msg void OnAspect();
	afx_msg void OnDropdownFiles();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGIMAGE_H__874877EC_C35B_4280_8650_C684A7CE3E39__INCLUDED_)
