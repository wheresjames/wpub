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
#if !defined(AFX_DLGJPEGSETTINGS_H__8E31B77C_A37C_4F24_9F62_0E2373075956__INCLUDED_)
#define AFX_DLGJPEGSETTINGS_H__8E31B77C_A37C_4F24_9F62_0E2373075956__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgJpegSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgJpegSettings dialog

class CDlgJpegSettings : public CDialog
{
// Construction
public:
	LRESULT OnSave( WPARAM wParam, LPARAM lParam );
	void OnRefresh( WPARAM wParam, LPARAM lParam );
	CDlgJpegSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgJpegSettings)
	enum { IDD = IDD_JPGSETTINGS };
	CSliderCtrl	m_slideQuality;
	CString	m_sQuality;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgJpegSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK(){}
	virtual void OnCancel(){}
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgJpegSettings)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeQuality();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGJPEGSETTINGS_H__8E31B77C_A37C_4F24_9F62_0E2373075956__INCLUDED_)
