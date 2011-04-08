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
#if !defined(AFX_DLGSPLASH_H__561CAB8E_CC75_4B2B_8C66_A9BEDBE128FC__INCLUDED_)
#define AFX_DLGSPLASH_H__561CAB8E_CC75_4B2B_8C66_A9BEDBE128FC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgSplash.h : header file
//

#include "ShowImg.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgSplash dialog

class CDlgSplash : public CDialog
{
// Construction
public:
	void End();
	BOOL Run( HWND hWnd, DWORD img, LPCTSTR pType = NULL, DWORD idd = NULL, DWORD dwMinDisplay = 2000 );
	void Size();
	CDlgSplash(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSplash)
	enum { IDD = IDD_SPLASH };
	CShowImg	m_img;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSplash)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSplash)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	BOOL	m_bRun;
	UINT	m_uTimerId;
	DWORD	m_dwMinDisplay;
	HWND	m_hCenter;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSPLASH_H__561CAB8E_CC75_4B2B_8C66_A9BEDBE128FC__INCLUDED_)
