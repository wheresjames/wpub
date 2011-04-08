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
#if !defined(AFX_WINMENU_H__AB30B2EF_A052_11D4_820B_0050DA205C15__INCLUDED_)
#define AFX_WINMENU_H__AB30B2EF_A052_11D4_820B_0050DA205C15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WinMenu.h : header file
//

#include "MenuItemList.h"
#include "WndToolbar.h"


/////////////////////////////////////////////////////////////////////////////
// CWinMenu window

class CWinMenu : public CWnd
{
// Construction
public:
	CWinMenu();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinMenu)
	public:
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL GetMenuRect( long x, long y, LPRECT pRect );
	virtual ~CWinMenu();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWinMenu)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL RemoveMenuItem( LPMITEMINFO node );

	
	virtual void Destroy();

	LPMITEMINFO AddMenuItem(	LPCTSTR pName, DWORD dwCmd, HICON hIcon, 
								HCURSOR hCursor = NULL, LPBOOL pBool = NULL,
								LPCTSTR pRText = NULL,
								HICON *pRIcons = NULL, DWORD dwRIcons = 0 );

//	DWORD Size() { return m_dwSize; }
//	DWORD GetNumMenuItems() { return Size(); }
//	LPMITEMINFO	GetNext( LPMITEMINFO ptr )
//	{	if( ptr != NULL ) return ptr->pNext; return m_pHead; }

	CWinMenu* GetSubMenu( LPMITEMINFO ptr )
	{	if ( ptr != NULL ) return ptr->submenu; return NULL; }

	void SetMessageTarget( HWND hWnd ) { m_hWndMsgTarget = hWnd; }

	void SetTickIcon( HICON tick ) { m_hTick = tick; }
	void SetCheckIcons( HICON c, HICON u, HICON hc = NULL, HICON hu = NULL ) 
	{ m_hCheck = c; m_hHotCheck = hc; m_hUncheck = u; m_hHotUncheck = hu; }

	void AllowChildren( BOOL b ) { m_bAllowChildren = b; }

	CWndToolbar* GetItemToolbar( LPMITEMINFO ptr );

	void SetItemIcon( LPMITEMINFO pmi, HICON hIcon ) { if ( pmi != NULL ) pmi->icon = hIcon; }

	DWORD Size() { return m_mitems.Size(); }

	void SetShading( DWORD shading ) { m_dwShading = shading; }

private:

//	DWORD			m_dwSize;
//	LPMITEMINFO		m_pHead;
//	LPMITEMINFO		m_pTail;

	CMenuItemList	m_mitems;

	HWND			m_hWndMsgTarget;

	// Offscreen drawing
/*	HDC				m_hDC;
	HBITMAP			m_hBmp;
	HBITMAP			m_hOldBmp;
	RECT			m_rect;
*/
	CWinDc			m_offscreen;
	CWinDc			m_bck;

	LPMITEMINFO		m_menuitem;
	LPMITEMINFO		m_itemover;
	LPMITEMINFO		m_tbclick;

	UINT			m_uTimerID;

	DWORD			m_dwDestroyDelay;

	HICON			m_hTick;
	HICON			m_hCheck, m_hHotCheck;
	HICON			m_hUncheck, m_hHotUncheck;

	BOOL			m_bAllowChildren;

	long			m_tw;
	long			m_th;

	HWND			m_hParentMenu;

	COLORREF		m_rgbMenu;
	COLORREF		m_rgbMenuText;
	COLORREF		m_rgbSel;
	COLORREF		m_rgbSelText;

	BOOL			m_bSystemColors;

	BOOL			m_bFirstDraw;

	DWORD			m_dwEmptyLimit;

	DWORD			m_dwShading;

	DWORD			m_dwLine;

	BOOL			m_bShowUp;
	BOOL			m_bShowRight;

public:
	BOOL IsMenuRight( long x, long y );

	void SetShowPos( BOOL up, BOOL right )
	{	m_bShowUp = up; m_bShowRight = right; }

	void SetEmptyLimit( DWORD el ) { m_dwEmptyLimit = el; }
	BOOL IsEmpty() { return ( Size() <= m_dwEmptyLimit ); }

	void SetColors( COLORREF menu, COLORREF menutext, COLORREF sel, COLORREF seltext )
	{	m_bSystemColors = FALSE;
		m_rgbMenu = menu; m_rgbMenuText = menutext; m_rgbSel = sel, m_rgbSelText = seltext; 
		LPMITEMINFO		pmi = NULL;
		while ( ( pmi = (LPMITEMINFO)m_mitems.GetNext( pmi ) ) != NULL )
			if ( pmi->submenu != NULL ) pmi->submenu->SetColors( menu, menutext, sel, seltext );

	}
	void SystemColors() 
	{	m_bSystemColors = TRUE; 
		LPMITEMINFO		pmi = NULL;
		while ( ( pmi = (LPMITEMINFO)m_mitems.GetNext( pmi ) ) != NULL )
			if ( pmi->submenu != NULL ) pmi->submenu->SystemColors();
	}

	void Sort();

	DWORD GetColorAvg( COLORREF rgb )
	{	return ( ( GetRValue( rgb ) + GetGValue( rgb ) + GetBValue( rgb ) ) / 3 ); }

	
	BOOL ShowMenu( HWND hParent, long x, long y );

	BOOL GetMenuPosition( long x, long y, LPRECT pRect )
	{	RECT rect; SetRect( &rect, x, y, x, y );
		return GetMenuPosition( &rect, pRect ); 
	}
	BOOL GetMenuPosition( LPRECT pAvoid, LPRECT pRect );

	LRESULT OnMenuCommand( WPARAM wParam, LPARAM lParam );

	void SetParentMenu( HWND hwnd ) { m_hParentMenu = hwnd; }

	BOOL GetItemRect( LPMITEMINFO pmi, LPRECT pRect );
	BOOL NewSubMenu( LPMITEMINFO pmi );
	void KillSubMenus();
	void RedrawLastWindow();
	COLORREF ScaleColor( COLORREF col, long scale );
	LPMITEMINFO GetMenuItem( LPPOINT pPt, LPBOOL bCmdArea = NULL );
	BOOL DrawMenuItems( HDC hDC, LPRECT pRect );

	HWND			m_hDropWnd;
	HWND			m_hLastWnd;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINMENU_H__AB30B2EF_A052_11D4_820B_0050DA205C15__INCLUDED_)
