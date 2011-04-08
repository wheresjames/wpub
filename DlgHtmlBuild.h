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
//{{AFX_INCLUDES()
#include "webbrowser2.h"
//}}AFX_INCLUDES
#if !defined(AFX_DLGHTMLBUILD_H__ED1DDAA3_B6AE_4B14_8D18_49458953FE97__INCLUDED_)
#define AFX_DLGHTMLBUILD_H__ED1DDAA3_B6AE_4B14_8D18_49458953FE97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgHtmlBuild.h : header file
//

#include "HtmlProperties.h"
#include "TitleWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgHtmlBuild dialog

class CDlgHtmlBuild : public CDialog
{
// Construction
public:
	void DoEnable();
	BOOL LoadVars( BOOL bDefaults = FALSE );
	BOOL SaveVars();
	BOOL UpdateVars();
	LRESULT OnRefreshHtml( WPARAM wParam, LPARAM lParam );
	BOOL ShowVars( CReg *pReg );
	BOOL ShowTemplate( LPCTSTR pTemplate, BOOL bDefaults = FALSE );
	BOOL FillTree( HTREEITEM hRoot, LPCTSTR pDir );
	BOOL FillTree();
	void Size();
	CDlgHtmlBuild(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgHtmlBuild)
	enum { IDD = IDD_HTMLBUILD };
	CTitleWnd	m_warning;
	CHtmlProperties	m_prop;
	CTreeCtrl	m_tree;
	CWebBrowser2	m_web;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgHtmlBuild)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgHtmlBuild)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDefaults();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	char			m_szTemplate[ CWF_STRSIZE ];
	char			m_szPath[ CWF_STRSIZE ];

	CImageList		m_images;

	CReg			m_reg;

	BOOL			m_bSize;
	DWORD			m_dwSplit;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGHTMLBUILD_H__ED1DDAA3_B6AE_4B14_8D18_49458953FE97__INCLUDED_)
