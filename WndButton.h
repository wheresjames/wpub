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
#if !defined(AFX_WNDBUTTON_H__0AD57EA5_F270_40B3_84AA_31A973875468__INCLUDED_)
#define AFX_WNDBUTTON_H__0AD57EA5_F270_40B3_84AA_31A973875468__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndButton.h : header file
//

#include "WinButton.h"

/////////////////////////////////////////////////////////////////////////////
// CWndButton window

class CWndButton : public CButton
{
// Construction
public:
	CWndButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	LRESULT OnMouseLeave( WPARAM wParam, LPARAM lParam );
	virtual ~CWndButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:

	void SetIcon( HICON hIcon ) { m_winbutton.SetIcon( hIcon ); }
	void SetCustomFont( BOOL b ) { m_winbutton.SetCustomFont( b ); }
	BOOL SetImg( CWinImg *pImg ) { return m_winbutton.CreateBckImg( pImg ); }
	void Check( BOOL b ) { m_winbutton.SetCheck( b ); RedrawWindow(); }

private:

	UINT			m_uTimerId;

	CWinButton		m_winbutton;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDBUTTON_H__0AD57EA5_F270_40B3_84AA_31A973875468__INCLUDED_)
