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
#if !defined(AFX_3DLIST_H__1FA7B325_6BBC_11D5_82F0_FFFFFF000000__INCLUDED_)
#define AFX_3DLIST_H__1FA7B325_6BBC_11D5_82F0_FFFFFF000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// 3DList.h : header file
//

#include "ItemList.h"

/////////////////////////////////////////////////////////////////////////////
// C3DList window

class C3DList : public CStatic
{
// Construction
public:
	C3DList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3DList)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Size();
	void UpdateVScrollBar();
	void Initialize();
	virtual ~C3DList();

	CItemList& GetListCtrl() { return m_list; }

	void AutosizeHeader( BOOL b ) { GetListCtrl().GetHeader().Autosize( b ); }

	// Generated message map functions
protected:
	//{{AFX_MSG(C3DList)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	void UpdateHScrollBar();

	CItemList	m_list;

	void SetContextMenuWnd( HWND hwnd )
	{	m_hContextMenu = hwnd; }

private:

	CScrollBar	m_scrollbar;
	CScrollBar		m_hscroll;
	
	BOOL		m_bInitialize;

	CWinDc	m_offscreen;

	HWND		m_hContextMenu;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3DLIST_H__1FA7B325_6BBC_11D5_82F0_FFFFFF000000__INCLUDED_)
