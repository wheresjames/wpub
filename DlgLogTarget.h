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
#if !defined(AFX_DLGLOGTARGET_H__898D2A51_82EC_42DA_8047_D71077590B0E__INCLUDED_)
#define AFX_DLGLOGTARGET_H__898D2A51_82EC_42DA_8047_D71077590B0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLogTarget.h : header file
//

#include "DlgInform.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgLogTarget dialog

#define WM_SHOWSTATUS			( WM_USER + 801 )

class CDlgLogTarget : public CDialog
{
// Construction
public:
	LRESULT OnShowStatus( WPARAM wParam, LPARAM lParam );
	CDlgLogTarget(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgLogTarget)
	enum { IDD = IDD_LOGTARGET };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLogTarget)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgLogTarget)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	CDlgInform			m_inform;

	DWORD				m_dwCreateInform;

	CTlLock				m_informlock;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLOGTARGET_H__898D2A51_82EC_42DA_8047_D71077590B0E__INCLUDED_)
