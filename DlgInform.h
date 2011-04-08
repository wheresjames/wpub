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
#if !defined(AFX_DLGINFORM_H__C5DE0AB9_A45C_470F_8D76_31C764F7AB7C__INCLUDED_)
#define AFX_DLGINFORM_H__C5DE0AB9_A45C_470F_8D76_31C764F7AB7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgInform.h : header file
//

#include "TitleWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgInform dialog

class CDlgInform : public CDialog
{


// Construction
public:
	void SetStatus( LPCTSTR pStatus );
	void Run( LPCTSTR pText );
	void Size();
	
	CDlgInform(LPCTSTR pText = NULL, CWnd* pParent = NULL);   // standard constructor
	~CDlgInform();

	void SetText( LPCTSTR pText )
	{	m_text.SetTitle( pText ); Size(); }
	void SetFont( DWORD size, LPCTSTR font )
	{	m_text.SetFont( size, font ); }


// Dialog Data
	//{{AFX_DATA(CDlgInform)
	enum { IDD = IDD_INFORM };
	CTitleWnd	m_status;
	CTitleWnd	m_text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgInform)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgInform)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINFORM_H__C5DE0AB9_A45C_470F_8D76_31C764F7AB7C__INCLUDED_)
