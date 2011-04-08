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
#if !defined(AFX_WNDLOG_H__29741383_C9D7_48CE_98FD_4E9FC9ACCA12__INCLUDED_)
#define AFX_WNDLOG_H__29741383_C9D7_48CE_98FD_4E9FC9ACCA12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndLog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWndLog window

class CWndLog : public CWnd
{
// Construction
public:
	CWndLog();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndLog)
	//}}AFX_VIRTUAL

// Implementation
public:
	LRESULT OnShowStatus( UINT uFlag, LPCTSTR pStatus );
	virtual ~CWndLog();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndLog)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDLOG_H__29741383_C9D7_48CE_98FD_4E9FC9ACCA12__INCLUDED_)