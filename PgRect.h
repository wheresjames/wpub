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
#if !defined(AFX_PGRECT_H__8B03F435_10AB_4456_B25C_AB7C19BC93A9__INCLUDED_)
#define AFX_PGRECT_H__8B03F435_10AB_4456_B25C_AB7C19BC93A9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PgRect.h : header file
//

#include "ColorBox.h"

/////////////////////////////////////////////////////////////////////////////
// CPgRect dialog

class CPgRect : public CDialog
{
// Construction
public:
	void DoEnable();
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CPgRect(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgRect)
	enum { IDD = IDD_PG_RECT };
	CColorBox	m_colorbox;
	BOOL	m_bBck;
	CString	m_sTranslucency;
	BOOL	m_bBorder;
	BOOL	m_bFoundered;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgRect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPgRect)
	virtual BOOL OnInitDialog();
	afx_msg void OnColor();
	afx_msg void OnBck();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChangeTranslucency();
	afx_msg void OnBorder();
	afx_msg void OnFoundered();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGRECT_H__8B03F435_10AB_4456_B25C_AB7C19BC93A9__INCLUDED_)
