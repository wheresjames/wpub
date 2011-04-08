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
#if !defined(AFX_PGIMAGEPROP_H__98B31758_E115_43FD_9102_16C4C39F25D9__INCLUDED_)
#define AFX_PGIMAGEPROP_H__98B31758_E115_43FD_9102_16C4C39F25D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgImageProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPgImageProp dialog

class CPgImageProp : public CDialog
{
// Construction
public:
	void DoEnable();
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CPgImageProp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgImageProp)
	enum { IDD = IDD_PG_IMAGEPROP };
	BOOL	m_bCustomSize;
	CString	m_sHeight;
	CString	m_sName;
	CString	m_sWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgImageProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CPgImageProp)
	virtual BOOL OnInitDialog();
	afx_msg void OnCustomsize();
	afx_msg void OnChangeHeight();
	afx_msg void OnChangeName();
	afx_msg void OnChangeWidth();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGIMAGEPROP_H__98B31758_E115_43FD_9102_16C4C39F25D9__INCLUDED_)
