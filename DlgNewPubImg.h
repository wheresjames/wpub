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
#if !defined(AFX_DLGNEWPUBIMG_H__DCBB5B89_12FF_4988_BEB4_8C409BA40D4F__INCLUDED_)
#define AFX_DLGNEWPUBIMG_H__DCBB5B89_12FF_4988_BEB4_8C409BA40D4F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgNewPubImg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgNewPubImg dialog

class CDlgNewPubImg : public CDialog
{
// Construction
public:
	void DoEnable();
	void GetDrivers();
	CDlgNewPubImg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgNewPubImg)
	enum { IDD = IDD_NEWPUBIMG };
	CComboBox	m_comboVideo;
	CComboBox	m_comboImage;
	CString	m_sName;
	BOOL	m_bCustomSize;
	CString	m_sHeight;
	CString	m_sWidth;
	int		m_nBckType;
	CString	m_sImage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewPubImg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNewPubImg)
	afx_msg void OnChooseImage();
	afx_msg void OnBcktype();
	afx_msg void OnBcktype2();
	afx_msg void OnBcktype3();
	virtual BOOL OnInitDialog();
	afx_msg void OnCustomsize();
	virtual void OnOK();
	afx_msg void OnSelchangeImage();
	afx_msg void OnEditchangeImage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void FillCombo();

	CString		m_sDriver;
	DWORD		m_dwDriver;

	CString		m_sImageFile;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNEWPUBIMG_H__DCBB5B89_12FF_4988_BEB4_8C409BA40D4F__INCLUDED_)
