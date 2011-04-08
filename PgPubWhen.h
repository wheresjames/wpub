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
#if !defined(AFX_PGPUBWHEN_H__B73C8193_9F41_4824_BAE6_A47FF48FDB0D__INCLUDED_)
#define AFX_PGPUBWHEN_H__B73C8193_9F41_4824_BAE6_A47FF48FDB0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgPubWhen.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPgPubWhen dialog

class CPgPubWhen : public CDialog
{
// Construction
public:
	void FillCombo();
	void DoEnable();
	LRESULT OnSave( WPARAM wParam, LPARAM lParam );
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CPgPubWhen(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgPubWhen)
	enum { IDD = IDD_PUBWHEN };
	CComboBox	m_comboDriver;
	CString	m_sHour;
	CString	m_sMin;
	CString	m_sSec;
	BOOL	m_bSyncToClock;
	int		m_nWhen;
	CString	m_sHour2;
	CString	m_sMin2;
	CString	m_sSec2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgPubWhen)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CPgPubWhen)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeHour();
	afx_msg void OnChangeMin();
	afx_msg void OnChangeSec();
	afx_msg void OnWhen();
	afx_msg void OnWhen2();
	afx_msg void OnSynctoclock();
	afx_msg void OnChangeHour2();
	afx_msg void OnChangeMin2();
	afx_msg void OnChangeSec2();
	afx_msg void OnSelchangeDriver();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGPUBWHEN_H__B73C8193_9F41_4824_BAE6_A47FF48FDB0D__INCLUDED_)
