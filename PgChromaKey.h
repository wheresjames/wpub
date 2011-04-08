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
#if !defined(AFX_PGCHROMAKEY_H__1E221086_F58B_4B44_BFF4_5E1EECE98DDB__INCLUDED_)
#define AFX_PGCHROMAKEY_H__1E221086_F58B_4B44_BFF4_5E1EECE98DDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgChromaKey.h : header file
//

#include "ColorBox.h"
#include "Slider.h"

/////////////////////////////////////////////////////////////////////////////
// CPgChromaKey dialog

class CPgChromaKey : public CDialog
{
// Construction
public:
	void DoEnable();
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CPgChromaKey(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgChromaKey)
	enum { IDD = IDD_PG_CHROMAKEY };
	CColorBox	m_colorbox;
	CSlider	m_slidePositive;
	CSlider	m_slideNegative;
	BOOL	m_bEnable;
	CString	m_sNegative;
	CString	m_sPositive;
	BOOL	m_bNoSmoothing;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgChromaKey)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CPgChromaKey)
	virtual BOOL OnInitDialog();
	afx_msg void OnEnable();
	afx_msg void OnChooseColor();
	afx_msg void OnChangeNegative();
	afx_msg void OnChangePositive();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnNosmoothing();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGCHROMAKEY_H__1E221086_F58B_4B44_BFF4_5E1EECE98DDB__INCLUDED_)
