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
#if !defined(AFX_DLGMAPFOLDER_H__9A1A426D_2009_42A1_8783_35EADF2C7B5A__INCLUDED_)
#define AFX_DLGMAPFOLDER_H__9A1A426D_2009_42A1_8783_35EADF2C7B5A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMapFolder.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgMapFolder dialog

class CDlgMapFolder : public CDialog
{
// Construction
public:
	void FixFolder();
	CDlgMapFolder(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgMapFolder)
	enum { IDD = IDD_MAPFOLDER };
	CString	m_sFolder;
	CString	m_sVirtualFolder;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMapFolder)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgMapFolder)
	virtual BOOL OnInitDialog();
	afx_msg void OnChooseFolder();
	afx_msg void OnChangeFolder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMAPFOLDER_H__9A1A426D_2009_42A1_8783_35EADF2C7B5A__INCLUDED_)
