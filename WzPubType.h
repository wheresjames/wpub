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
#if !defined(AFX_WZPUBTYPE_H__C844F7D0_C425_49A7_AEA8_A8F71FA879D8__INCLUDED_)
#define AFX_WZPUBTYPE_H__C844F7D0_C425_49A7_AEA8_A8F71FA879D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WzPubType.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWzPubType dialog

class CWzPubType : public CDialog
{
// Construction
public:
	void UpdatePubType( DWORD type );
	LRESULT OnSave( WPARAM wParam, LPARAM lParam );
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CWzPubType(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWzPubType)
	enum { IDD = IDD_WZ_PUBTYPE };
	int		m_nPubType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWzPubType)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CWzPubType)
	virtual BOOL OnInitDialog();
	afx_msg void OnPubtype();
	afx_msg void OnPubtype2();
	afx_msg void OnPubtype3();
	afx_msg void OnPubtype4();
	afx_msg void OnPubtype5();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WZPUBTYPE_H__C844F7D0_C425_49A7_AEA8_A8F71FA879D8__INCLUDED_)
