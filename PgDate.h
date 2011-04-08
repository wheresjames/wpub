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
#if !defined(AFX_PGDATE_H__773C6960_1C7F_4486_A1EF_6901675802F8__INCLUDED_)
#define AFX_PGDATE_H__773C6960_1C7F_4486_A1EF_6901675802F8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PgDate.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPgDate dialog

class CPgDate : public CDialog
{
// Construction
public:
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CPgDate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgDate)
	enum { IDD = IDD_PG_DATE };
	CComboBox	m_comboFormat;
	int		m_nGmt;
	CString	m_sSep;
	int		m_nStyle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgDate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPgDate)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFormat();
	afx_msg void OnGmt();
	afx_msg void OnGmt2();
	afx_msg void OnChangeSep();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnStyle2();
	afx_msg void OnStyle3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGDATE_H__773C6960_1C7F_4486_A1EF_6901675802F8__INCLUDED_)
