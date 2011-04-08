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
#if !defined(AFX_PGTRANS_H__2178E716_40ED_4FA1_8F06_8F37E29741D1__INCLUDED_)
#define AFX_PGTRANS_H__2178E716_40ED_4FA1_8F06_8F37E29741D1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PgTrans.h : header file
//

#include "ColorBox.h"
#include "Slider.h"

/////////////////////////////////////////////////////////////////////////////
// CPgTrans dialog

class CPgTrans : public CDialog
{
// Construction
public:
	void DoEnable();
	LRESULT OnNegative( WPARAM wParam, LPARAM lParam );
	LRESULT OnPositive( WPARAM wParam, LPARAM lParam );
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CPgTrans(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgTrans)
	enum { IDD = IDD_PG_TRANS };
	CColorBox	m_colorbox;
	CSlider	m_slideNegative;
	CSlider	m_slidePositive;
	CString	m_sPositive;
	CString	m_sNegative;
	BOOL	m_bTransparency;
	BOOL	m_bNoSmoothing;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgTrans)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPgTrans)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChooseColor();
	afx_msg void OnTransparency();
	afx_msg void OnChangeNegative();
	afx_msg void OnChangePositive();
	virtual void OnOK(){}
	virtual void OnCancel(){}
	afx_msg void OnNosmoothing();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGTRANS_H__2178E716_40ED_4FA1_8F06_8F37E29741D1__INCLUDED_)
