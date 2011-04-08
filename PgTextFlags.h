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
#if !defined(AFX_PGTEXTFLAGS_H__B677F4EE_7A8E_4112_9687_8088D022EEE2__INCLUDED_)
#define AFX_PGTEXTFLAGS_H__B677F4EE_7A8E_4112_9687_8088D022EEE2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PgTextFlags.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPgTextFlags dialog

class CPgTextFlags : public CDialog
{
// Construction
public:
	void FillHAlign();
	void FillVAlign();
	void UpdateSwitches();
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CPgTextFlags(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgTextFlags)
	enum { IDD = IDD_PG_TEXTFLAGS };
	CComboBox	m_comboHAlign;
	CComboBox	m_comboVAlign;
	BOOL	m_bEditControl;
	BOOL	m_bEndEllipsis;
	BOOL	m_bEscapementCenter;
	BOOL	m_bExpandTabs;
	BOOL	m_bExternalLeading;
	BOOL	m_bMultiCenter;
	BOOL	m_bNoClip;
	BOOL	m_bNoPrefix;
	BOOL	m_bPathEllipsis;
	BOOL	m_bRtlReading;
	BOOL	m_bSingleLine;
	BOOL	m_bWordBreak;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgTextFlags)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPgTextFlags)
	virtual BOOL OnInitDialog();
	afx_msg void OnEditcontrol();
	afx_msg void OnEndEllipsis();
	afx_msg void OnEscapementcenter();
	afx_msg void OnExpandtabs();
	afx_msg void OnExternalleading();
	afx_msg void OnMulticenter();
	afx_msg void OnNoclip();
	afx_msg void OnNoprefix();
	afx_msg void OnPathEllipsis();
	afx_msg void OnRtlreading();
	afx_msg void OnSingleline();
	afx_msg void OnWordbreak();
	afx_msg void OnSelchangeHalign();
	afx_msg void OnSelchangeValign();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGTEXTFLAGS_H__B677F4EE_7A8E_4112_9687_8088D022EEE2__INCLUDED_)
