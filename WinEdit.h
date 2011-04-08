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
#if !defined(AFX_WINEDIT_H__CCD4AB24_09A6_4C38_ADC9_175BF8C22F17__INCLUDED_)
#define AFX_WINEDIT_H__CCD4AB24_09A6_4C38_ADC9_175BF8C22F17__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WinEdit.h : header file
//

#include "PubImgList.h"
#include "RectMover.h"
#include "Fx.h"

/////////////////////////////////////////////////////////////////////////////
// CWinEdit window

class CWinEdit : public CWnd
{
// Construction
public:
	CWinEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Size( LPRECT pRect );
	BOOL Init( HWND hParent, LPRECT pRect );
	virtual ~CWinEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWinEdit)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPopupProperties();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void FreeMemory();
	void ClearFlyby();
	afx_msg void OnPopupCopy();
	afx_msg void OnPopupCut();
	afx_msg void OnPopupDelete();
	afx_msg void OnPopupDuplicate();
	afx_msg void OnPopupPaste();

	BOOL ShowMenu( long x, long y );
	void ToggleInsert( DWORD type );
	BOOL InsertFx( DWORD type, LPPOINT pt );
	BOOL SetEditFx( LPFX pFx );

	void Update() { if ( ::IsWindow( GetSafeHwnd() ) ) RedrawWindow(); }
	void SetImgList( CPubImgList *ptr ) { m_pImgList = ptr; Update(); }
	CPubImgList* GetImageList() { return m_pImgList; }

	void SetEditRect( LPRECT pRect ) 
	{ m_pEditRect = pRect; m_rmover.End(); Update(); }
	LPRECT GetEditRect() { return m_pEditRect; }

	LPFX GetEditFx() { return m_fxEdit; }

	DWORD GetInsertType() { return m_dwInsert; }

	void ClearMenuPoint() { m_ptMenu.x = 0; m_ptMenu.y = 0; }

	void ClearInsert();


private:

	CPubImgList		*m_pImgList;

	CRectMover		m_rmover;
	RECT			m_rect;

	LPRECT			m_pEditRect;
	LPFX			m_fxEdit;

	BOOL			m_bInserting;
	DWORD			m_dwInsert;

	LPRECT			m_pFlybyRect;

	POINT			m_ptMenu;

	CWinDc			m_offscreen;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINEDIT_H__CCD4AB24_09A6_4C38_ADC9_175BF8C22F17__INCLUDED_)
