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
#if !defined(AFX_PUBIMGVIEW_H__FD4E41EE_B774_4300_AAC2_9120F9F92FB2__INCLUDED_)
#define AFX_PUBIMGVIEW_H__FD4E41EE_B774_4300_AAC2_9120F9F92FB2__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PubImgView.h : header file
//

#include "PubImgList.h"

/////////////////////////////////////////////////////////////////////////////
// CPubImgView window

class CPubImgView : public CWnd
{
// Construction
public:
	CPubImgView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPubImgView)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Size( LPRECT pRect );
	BOOL Init( HWND hParent, LPRECT pRect );
	virtual ~CPubImgView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPubImgView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	BOOL Regen( DWORD i = MAXDWORD );
	DWORD HitTest( LPRECT pRect, LPPOINT pt );
//	BOOL Box( HDC hDC, LPRECT pRect, DWORD linewidth, COLORREF lt, COLORREF dk );
	BOOL DrawImage( HDC hDC, LPRECT pRect, DWORD i, BOOL bHorz );
	BOOL Draw( HDC hDC, LPRECT pRect );

	void SetImgList( CPubImgList *ptr ) { m_pImgList = ptr; Update(); }
	void Update() { if ( ::IsWindow( GetSafeHwnd() ) ) { RedrawWindow(); } }

	void SetCallback( HWND hWnd, UINT uMsg )
	{	m_hWndCallback = hWnd; m_uMsgCallback = uMsg; }
	LRESULT Callback( WPARAM wParam, LPARAM lParam )
	{	if ( !::IsWindow( m_hWndCallback ) ) return 0;
		return ::SendMessage( m_hWndCallback, m_uMsgCallback, wParam, lParam ); }

private:

	CWinDc			m_offscreen;
	CWinDc			m_video;

	CPubImgList		*m_pImgList;

	HWND			m_hWndCallback;
	UINT			m_uMsgCallback;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PUBIMGVIEW_H__FD4E41EE_B774_4300_AAC2_9120F9F92FB2__INCLUDED_)
