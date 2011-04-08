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
#if !defined(AFX_PGMASK_H__7C431675_D43E_48BD_8EE9_6323201224CB__INCLUDED_)
#define AFX_PGMASK_H__7C431675_D43E_48BD_8EE9_6323201224CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgMask.h : header file
//

#include "ColorBox.h"
#include "IconButton.h"
#include "PubImgList.h"
#include "ImgThread.h"

/////////////////////////////////////////////////////////////////////////////
// CPgMask dialog

class CPgMask : public CDialog
{
// Construction
public:
	LRESULT OnLoadNextMaskImage( WPARAM wParam, LPARAM lParam );
	void DoEnable();
	void FillList();
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CPgMask(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgMask)
	enum { IDD = IDD_PG_MASK };
	CIconButton	m_btnChooseColor;
	CColorBox	m_maskcolor;
	CListCtrl	m_list;
	BOOL	m_bMask;
	CString	m_sMaskFile;
	BOOL	m_bMaskImg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgMask)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CPgMask)
	afx_msg void OnChooseMaskfile();
	virtual BOOL OnInitDialog();
	afx_msg void OnMask();
	afx_msg void OnChooseColor();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeMaskfile();
	afx_msg void OnMaskimg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	CImageList		m_thm;

	LPPUBIMGINFO	m_cur;

	CImgThread		m_imgthread;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGMASK_H__7C431675_D43E_48BD_8EE9_6323201224CB__INCLUDED_)
