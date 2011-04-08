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
#if !defined(AFX_DLGMSGVIEW_H__5BCF4C8B_9DC1_11D3_BF54_00104B2C9CFA__INCLUDED_)
#define AFX_DLGMSGVIEW_H__5BCF4C8B_9DC1_11D3_BF54_00104B2C9CFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MsgView.h : header file
//

#include "Log.h"


#define WM_UPDATELIST		( WM_USER + 101 )
#define WM_INSERTITEM		( WM_USER + 102 )

/////////////////////////////////////////////////////////////////////////////
// CDlgMsgView dialog

class CDlgMsgView : public CDialog
{
// Construction
public:
	BOOL UpdateList();
	void Size();
	CDlgMsgView(CWnd* pParent = NULL);   // standard constructor
	~CDlgMsgView();

// Dialog Data
	//{{AFX_DATA(CDlgMsgView)
	enum { IDD = IDD_MESSAGEVIEW };
	CStatic	m_icon;
	CListCtrl	m_list;
	CString	m_sText;
	BOOL	m_bPopup;
	BOOL	m_bPopupWarnings;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMsgView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMsgView)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickMvList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMvClear();
	afx_msg void OnClose();
	afx_msg void OnMvSave();
	afx_msg void OnMvPopup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void OnMvPopupWarnings();
	LRESULT OnInsertItem( WPARAM wParam, LPARAM lParam );
	LRESULT OnUpdateList( WPARAM wParam, LPARAM lParam );
	HICON GetIcon( DWORD i );
	void SavePosition();
	BOOL ShowMessage();

	void SetLog( CLog *log ) { m_log = log; }

	BOOL				m_bSave;

private:

	CImageList			m_images;

	CLog				*m_log;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMSGVIEW_H__5BCF4C8B_9DC1_11D3_BF54_00104B2C9CFA__INCLUDED_)
