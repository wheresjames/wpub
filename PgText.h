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
#if !defined(AFX_PGTEXT_H__40D74AD4_E8CF_46AF_B2B9_8E4E052867FC__INCLUDED_)
#define AFX_PGTEXT_H__40D74AD4_E8CF_46AF_B2B9_8E4E052867FC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PgText.h : header file
//

#include "ColorBox.h"

/////////////////////////////////////////////////////////////////////////////
// CPgText dialog

class CPgText : public CDialog
{
// Construction
public:
	BOOL ShowFont( LPFX pfx );
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CPgText(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgText)
	enum { IDD = IDD_PG_TEXT };
	CColorBox	m_colorbox;
	CButton	m_sFont;
	CString	m_sText;
	CString	m_sEscapement;
	CString	m_sFontWidth;
	CString	m_sFontStr;
	CString	m_sShadow;
	BOOL	m_bAutoSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgText)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPgText)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeText();
	afx_msg void OnFont();
	afx_msg void OnColor();
	afx_msg void OnChangeEscapement();
	afx_msg void OnCharmap();
	afx_msg void OnChangeFontwidth();
	afx_msg void OnChangeShadow();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnAutosize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGTEXT_H__40D74AD4_E8CF_46AF_B2B9_8E4E052867FC__INCLUDED_)
