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
#if !defined(AFX_TOOLBARWINDOW_H__EFAD77D8_DB45_49C9_9C2C_A78EEFD249EA__INCLUDED_)
#define AFX_TOOLBARWINDOW_H__EFAD77D8_DB45_49C9_9C2C_A78EEFD249EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolbarWindow.h : header file
//

#include "WndToolbar.h"

/////////////////////////////////////////////////////////////////////////////
// CToolbarWindow window

class CToolbarWindow : public CStatic
{
// Construction
public:
	CToolbarWindow();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolbarWindow)
	//}}AFX_VIRTUAL

// Implementation

	CWndToolbar* GetToolbar() { return &m_wndtoolbar; }

	BOOL CreateToolTips( HWND hWnd, LPRECT pRect )
	{	return m_wndtoolbar.CreateToolTips( hWnd, pRect ); }

	void Empty() { m_wndtoolbar.Destroy(); }

public:
	LRESULT OnNotifyMouseLeave( WPARAM wParam, LPARAM lParam );
	virtual ~CToolbarWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolbarWindow)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	CWinDc			m_offscreen;

	CWndToolbar		m_wndtoolbar;

	UINT			m_uTimerId;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLBARWINDOW_H__EFAD77D8_DB45_49C9_9C2C_A78EEFD249EA__INCLUDED_)
