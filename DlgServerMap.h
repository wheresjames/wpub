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
#if !defined(AFX_DLGSERVERMAP_H__6AB2F38D_FBE9_43DC_8B51_B7F055907543__INCLUDED_)
#define AFX_DLGSERVERMAP_H__6AB2F38D_FBE9_43DC_8B51_B7F055907543__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgServerMap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgServerMap dialog

class CDlgServerMap : public CDialog
{
// Construction
public:
	void FillMapList();
	DWORD GetFileIcon( LPCTSTR pFile );
	void FillImgList();
	LRESULT OnSave( WPARAM wParam, LPARAM lParam );
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CDlgServerMap(CWnd* pParent = NULL);   // standard constructor
	~CDlgServerMap();

// Dialog Data
	//{{AFX_DATA(CDlgServerMap)
	enum { IDD = IDD_SERVERMAP };
	CListCtrl	m_maplist;
	CListCtrl	m_imglist;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgServerMap)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgServerMap)
	virtual void OnCancel(){}
	virtual void OnOK(){}
	virtual BOOL OnInitDialog();
	afx_msg void OnAddimg();
	afx_msg void OnRemoveimg();
	afx_msg void OnAddmap();
	afx_msg void OnRemovemap();
	afx_msg void OnDblclkImglist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkMaplist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	CImageList		m_images;
	CImageList		m_imagesN;

	CImageList		m_dirimages;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSERVERMAP_H__6AB2F38D_FBE9_43DC_8B51_B7F055907543__INCLUDED_)
