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
#if !defined(AFX_SLANTTOOLBAR_H__5124740B_4468_4351_BA47_84D245DBA232__INCLUDED_)
#define AFX_SLANTTOOLBAR_H__5124740B_4468_4351_BA47_84D245DBA232__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SlantToolbar.h : header file
//

#include "DrawSlantToolbar.h"

/////////////////////////////////////////////////////////////////////////////
// CSlantToolbar window


class CSlantToolbar : public CStatic
{
// Construction
public:
	CSlantToolbar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSlantToolbar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSlantToolbar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSlantToolbar)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSysColorChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	LRESULT OnNotifyMouseLeave( WPARAM wParam, LPARAM lParam );

	BOOL Add( UINT uCmd, HICON hIcon, LPCTSTR pText, LPCTSTR pTip, LPBOOL pBool = NULL, HICON hPressed = NULL, DWORD flags = 0 )
	{	return m_tb.Add( uCmd, hIcon, pText, pTip, pBool, hPressed, flags ); }

	void SetMessageTarget( HWND hWnd, UINT uMsgCmd, UINT uMsgUpdate )
	{	m_tb.SetMessageTarget( hWnd, uMsgCmd, uMsgUpdate ); }

	void TextArea( BOOL b, LPCTSTR pTitle = NULL ) { m_tb.TextArea( b, pTitle ); }
	void SetMinTextArea( long min ) { m_tb.SetMinTextArea( min ); }
	void SetTitle( LPCTSTR pTitle ) { m_tb.SetTitle( pTitle ); }

	BOOL SetTextByCmd( UINT cmd, LPCTSTR text )
	{	return m_tb.SetTextByCmd( cmd, text ); }

	void ParentEcho( BOOL b ) { m_bParentEcho = b; }

	void HiLight( BOOL b ) { m_tb.HiLight( b ); }
	void HiLightBtn( BOOL b ) { m_tb.HiLightBtn( b ); }

	void SetButtonSlant( long s ) { m_tb.SetButtonSlant( s ); }
	long GetButtonSlant() { return m_tb.GetButtonSlant(); }

	void ForceTooltips( BOOL b ) { m_tb.ForceTooltips( b ); }

	void EmptyMenu() { m_tb.Destroy(); }

private:

	CWinDc				m_os;
	CDrawSlantToolbar	m_tb;
	UINT				m_uTimerId;
	BOOL				m_bParentEcho;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLANTTOOLBAR_H__5124740B_4468_4351_BA47_84D245DBA232__INCLUDED_)
