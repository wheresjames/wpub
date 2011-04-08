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
#if !defined(AFX_IMGLIST_H__D4A26EF0_7D98_4BDA_9AED_A7329B098EBF__INCLUDED_)
#define AFX_IMGLIST_H__D4A26EF0_7D98_4BDA_9AED_A7329B098EBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImgList.h : header file
//

#include "PubImgList.h"

/////////////////////////////////////////////////////////////////////////////
// CImgList window

class CImgList :	public CStatic, 
					public CMessage
{
// Construction
public:
	CImgList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImgList)
	//}}AFX_VIRTUAL

// Implementation
public:
	LRESULT OnImgMsg( WPARAM wParam, LPARAM lParam );
	void UpdateScrollbar();
	long GetMaxScrollHeight( BOOL bScrollbar );
	void Size();

	LPPUBIMGINFO HitTest( LPRECT pRect, LPPOINT pt, LPDWORD index )
	{	LPPUBIMGINFO ppii = NULL;
		RECT rect; CopyRect( &rect, pRect );
		if ( m_scrollbar.IsWindowVisible() )
		{	if ( m_bVertScroll ) rect.right -= GetSystemMetrics( SM_CXVSCROLL );
			else rect.bottom -= GetSystemMetrics( SM_CXHSCROLL );
		} // end if
		if ( !Draw( NULL, &rect, pt, &ppii, index ) ) return NULL;
		return ppii;
	}

	BOOL Draw( HDC hDC, LPRECT pRect, LPPOINT pt = NULL, LPPUBIMGINFO *pPii = NULL, LPDWORD index = NULL );
	BOOL DrawImage( HDC hDC, LPRECT pRect, LPPUBIMGINFO ppii );
	BOOL Add( LPCTSTR pUrl );
	LRESULT OnDropMsg( WPARAM wParam, LPARAM lParam );
	void Init( HWND hParent );
	virtual ~CImgList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CImgList)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnImagelistNewimage();
	afx_msg void OnImagelistDeleteimage();
	afx_msg void OnImagelistProperties();
	afx_msg void OnImagelistView();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	BOOL Regen( LPRECT pRect = NULL );
	BOOL ShowMenu( long x, long y  );

	void SetImgList( CPubImgList *ptr )
	{	m_pImgList = ptr; }

	LPPUBIMGINFO GetSelection() { return m_pSelected; }

	void SetUpdateTimer( DWORD dwTimeout = 250 )
	{	m_dwUpdateTimer = GetTickCount() + dwTimeout; }

private:

	CWinDc					m_bck;
	CWinDc					m_offscreen;

	BOOL					m_bInit;

	CWinDropTarget			m_droptarget;

	CPubImgList			*m_pImgList;

	CScrollBar				m_scrollbar;

	BOOL					m_bVertScroll;

	LPPUBIMGINFO			m_pSelected;

	HICON					m_hWinIcon;
	HICON					m_hHtml;
	HICON					m_hHtmlSmall;

	long					m_lAnimate;

	UINT			m_uTimerId;
	DWORD			m_dwUpdateTimer;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMGLIST_H__D4A26EF0_7D98_4BDA_9AED_A7329B098EBF__INCLUDED_)
