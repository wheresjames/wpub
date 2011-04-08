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
#if !defined(AFX_DLGPRODUCTUPDATE_H__5C2FD7D1_9393_414A_ABD9_4E1F11AB5F0A__INCLUDED_)
#define AFX_DLGPRODUCTUPDATE_H__5C2FD7D1_9393_414A_ABD9_4E1F11AB5F0A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgProductUpdate.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgProductUpdate dialog

class CDlgProductUpdate : public CDialog
{
// Construction
public:
	void Evaluate( HWND hWnd, double ver );
	LRESULT OnNetFile( WPARAM wParam, LPARAM lParam );
	void SetStatus( LPCTSTR pStatus );
	CDlgProductUpdate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgProductUpdate)
	enum { IDD = IDD_UPDATECHECK };
	CString	m_sStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProductUpdate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgProductUpdate)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL DoUpdateCheck(CWnd *pParent, LPCTSTR pLink, double dVer, BOOL bSilent);
	
	void SetLink( LPCTSTR pLink ) 
	{	if ( pLink == NULL ) *m_szLink = 0; strcpy_sz( m_szLink, pLink ); }

private:

	double			m_dVer;
	char			m_szLink[ CWF_STRSIZE ];
	BOOL			m_bSilent;

	CNetFile		m_nf;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPRODUCTUPDATE_H__5C2FD7D1_9393_414A_ABD9_4E1F11AB5F0A__INCLUDED_)
