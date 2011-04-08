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
#if !defined(AFX_DLGSERVERBAR_H__1479DB75_A7CC_479D_9215_44BE59CF0C1D__INCLUDED_)
#define AFX_DLGSERVERBAR_H__1479DB75_A7CC_479D_9215_44BE59CF0C1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgServerBar.h : header file
//

#define NEW_CDIALOGBAR

/////////////////////////////////////////////////////////////////////////////
// CDlgServerBar dialog
class CDlgServerBar : public CDialogBar
{
// Construction
public:
	CDlgServerBar(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgServerBar)
	enum { IDD = CG_IDD_SERVER };
	BOOL	m_bRun;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgServerBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgServerBar)
	afx_msg void OnRun();
#ifdef NEW_CDIALOGBAR
	LONG OnInitDialog(UINT wParam, LONG lParam);
#else
	virtual BOOL OnInitDialog();
#endif
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSERVERBAR_H__1479DB75_A7CC_479D_9215_44BE59CF0C1D__INCLUDED_)
