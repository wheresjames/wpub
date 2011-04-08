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
#if !defined(AFX_ICONBUTTON_H__41504A47_38BE_4816_B99C_0924CC93C9B6__INCLUDED_)
#define AFX_ICONBUTTON_H__41504A47_38BE_4816_B99C_0924CC93C9B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IconButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIconButton window

class CIconButton : public CButton
{
// Construction
public:
	CIconButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIconButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	void DrawIcon();
	BOOL SetIcon( HICON hIcon, HICON hDisabledIcon = NULL );
	virtual ~CIconButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CIconButton)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	HICON		m_hIcon;
	HICON		m_hDisabledIcon;
	BOOL		m_bPressed;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICONBUTTON_H__41504A47_38BE_4816_B99C_0924CC93C9B6__INCLUDED_)
