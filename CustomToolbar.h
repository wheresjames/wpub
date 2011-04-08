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
#if !defined(AFX_CUSTOMTOOLBAR_H__DDFF7C41_8216_4E52_A6DC_815ABFA80248__INCLUDED_)
#define AFX_CUSTOMTOOLBAR_H__DDFF7C41_8216_4E52_A6DC_815ABFA80248__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CustomToolbar.h : header file
//

#include "WndToolbar.h"

/////////////////////////////////////////////////////////////////////////////
// CCustomToolbar window

class CCustomToolbar : public CStatic
{
// Construction
public:
	CCustomToolbar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomToolbar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCustomToolbar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCustomToolbar)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	CWndToolbar		m_tb;

public:

	void SetHeight( long h ) { m_tb.SetHeight( h ); }
	void SetMessageTarget( HWND hWnd, UINT uMsg )
	{	m_tb.SetMessageTarget( hWnd, uMsg ); }
	LPMITEMINFO AddItem(	LPCTSTR pName, DWORD dwCmd, HICON hIcon, 
							HCURSOR hCursor, LPBOOL pBool, LPCTSTR pTip )
	{	return m_tb.AddItem( pName, dwCmd, hIcon, hCursor, pBool, pTip ); }

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUSTOMTOOLBAR_H__DDFF7C41_8216_4E52_A6DC_815ABFA80248__INCLUDED_)
