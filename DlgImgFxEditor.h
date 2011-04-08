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
#if !defined(AFX_DLGIMGFXEDITOR_H__01FE4338_066D_4293_8AF5_C84B9B9B84AB__INCLUDED_)
#define AFX_DLGIMGFXEDITOR_H__01FE4338_066D_4293_8AF5_C84B9B9B84AB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgImgFxEditor.h : header file
//

#include "Fx.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgImgFxEditor dialog

class CParams;
class CDlgImgFxEditor : public CDialog
{
// Construction
public:
	CDlgImgFxEditor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgImgFxEditor)
	enum { IDD = IDD_IMGFXEDITOR };
	CTreeCtrl	m_tree;
	CListCtrl	m_list;
	CString	m_sTitle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgImgFxEditor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgImgFxEditor)
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	virtual BOOL OnInitDialog();
	afx_msg void OnUp();
	afx_msg void OnDown();
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEdititem();
	virtual void OnCancel();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRemoveall();
	afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SetTitle( LPCTSTR pTitle );
	void DoEnable();
	int GetFirstListSel( CListCtrl *pList );
	HTREEITEM MakeTreeEntry( LPCTSTR pText, HTREEITEM hParent, DWORD dwImage );
	void FillList();
	void FillTree();
	void OnRefresh( WPARAM wParam, LPARAM lParam );

	virtual void OnOK();

//	void SetFx( LPFX pfx );

	void SetParams( CParams *params )
	{	m_params = params; }


	void SetFx( CParams *params, LPCTSTR pTitle, long w, long h )
	{	SetParams( params ); SetTitle( pTitle ); m_w = w; m_h = h; }

private:

	char	m_szTitle[ 256 ];
	long	m_w, m_h;

	CParams			*m_params;
//	LPFX			m_pfx;

	CImageList		m_images;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMGFXEDITOR_H__01FE4338_066D_4293_8AF5_C84B9B9B84AB__INCLUDED_)
