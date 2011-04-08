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
#if !defined(AFX_PGPUBTHUMBNAIL_H__5C2AAE08_7929_4EFA_9551_11F4117FB576__INCLUDED_)
#define AFX_PGPUBTHUMBNAIL_H__5C2AAE08_7929_4EFA_9551_11F4117FB576__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgPubThumbnail.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPubThumbnail dialog


class CPgPubThumbnail : public CDialog
{
// Construction
public:
	void PreviewFilename();
	LRESULT OnSave( WPARAM wParam, LPARAM lParam );
	void DoEnable();
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CPgPubThumbnail(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgPubThumbnail)
	enum { IDD = IDD_PUBTHUMBNAIL };
	CString	m_sThmFile;
	CString	m_sHeight;
	CString	m_sPHeight;
	CString	m_sPWidth;
	CString	m_sWidth;
	BOOL	m_bThumbnail;
	BOOL	m_bRatio;
	int		m_nTnSizing;
	BOOL	m_bRotate;
	CString	m_sFinal;
	BOOL	m_bTimestamp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgPubThumbnail)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CPgPubThumbnail)
	afx_msg void OnThumbnail();
	afx_msg void OnRatio();
	afx_msg void OnTnsizing();
	afx_msg void OnTnsizing2();
	afx_msg void OnChangeTfname();
	virtual BOOL OnInitDialog();
	afx_msg void OnRotate();
	afx_msg void OnTimestamp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGPUBTHUMBNAIL_H__5C2AAE08_7929_4EFA_9551_11F4117FB576__INCLUDED_)
