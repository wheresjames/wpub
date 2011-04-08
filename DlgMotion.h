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
#if !defined(AFX_DLGMOTION_H__AA5994CC_96FF_4FFB_9A3A_D53F60E65F13__INCLUDED_)
#define AFX_DLGMOTION_H__AA5994CC_96FF_4FFB_9A3A_D53F60E65F13__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMotion.h : header file
//

#include "ImageView.h"
#include "VideoList.h"	// Added by ClassView
#include "IconButton.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgMotion dialog

class CDlgMotion : public CDialog
{
// Construction
public:
	LRESULT OnRedrawImage( WPARAM wParam, LPARAM lParam );
	void DoEnable();
	LRESULT OnMotion( WPARAM wParam, LPARAM lParam );
	LPVIDEOINFO GetDriverInfo();
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	void SetMotion( long bright, long motion );
	CDlgMotion(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgMotion)
	enum { IDD = IDD_MOTION };
	CIconButton	m_btnChooseSound;
	CIconButton	m_btnPlay;
	CSliderCtrl	m_slideMotion;
	CSliderCtrl	m_slideBright;
	CComboBox	m_comboScanType;
	CProgressCtrl	m_progMotion;
	CProgressCtrl	m_progBright;
	CImageView	m_image;
	CComboBox	m_comboDriver;
	BOOL	m_bEnable;
	BOOL	m_bEnableSound;
	CString	m_sSoundFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMotion)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CDlgMotion)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnable();
	afx_msg void OnSelchangeDriver();
	afx_msg void OnSelchangeScantype();
	afx_msg void OnClose();
	afx_msg void OnPlay();
	afx_msg void OnChooseSound();
	afx_msg void OnEnablesound();
	afx_msg void OnChangeSoundfile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	long		m_bright;
	long		m_motion;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMOTION_H__AA5994CC_96FF_4FFB_9A3A_D53F60E65F13__INCLUDED_)
