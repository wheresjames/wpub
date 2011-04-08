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
#if !defined(AFX_PGPUBHOW_H__8F38E9E5_A1A6_405E_83CF_ABC9F14C1942__INCLUDED_)
#define AFX_PGPUBHOW_H__8F38E9E5_A1A6_405E_83CF_ABC9F14C1942__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgPubHow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPgPubHow dialog

class CPgPubHow : public CDialog
{
// Construction
public:
	void PreviewFilename();
	void SelectTemplate( LPCTSTR tmpl );
	void FillCombo();
	void VerifyIndex();
	void DoEnable();
	LRESULT OnSave( WPARAM wParam, LPARAM lParam );
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CPgPubHow(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgPubHow)
	enum { IDD = IDD_PUBHOW };
	CComboBox	m_comboFormat;
	BOOL	m_bRotate;
	CString	m_sFile;
	CString	m_sCurrentIndex;
	CString	m_sStartIndex;
	CString	m_sStopIndex;
	BOOL	m_bTimestamp;
	CString	m_sTemplate;
	CString	m_sFinal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgPubHow)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CPgPubHow)
	virtual BOOL OnInitDialog();
	afx_msg void OnRotate();
	afx_msg void OnChooseFile();
	afx_msg void OnChangeFile();
	afx_msg void OnChangeStartindex();
	afx_msg void OnChangeStopindex();
	afx_msg void OnChangeCurrentindex();
	afx_msg void OnTimestamp();
	afx_msg void OnChangeTemplate();
	afx_msg void OnSelchangeFormat();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGPUBHOW_H__8F38E9E5_A1A6_405E_83CF_ABC9F14C1942__INCLUDED_)
