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
#if !defined(AFX_DLGEDITPARAM_H__7BD10332_86A1_4A3B_9B20_B64A863448C5__INCLUDED_)
#define AFX_DLGEDITPARAM_H__7BD10332_86A1_4A3B_9B20_B64A863448C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgEditParam.h : header file
//

#include "PropList.h"

#include "IconButton.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgEditParam dialog

typedef struct tagPROPCALLBACKINFO
{
	DWORD			dwUser;
	int				nSel;
	DWORD			dwData;
	CComboBox		*pCombo;
	LPPROPINFO		pPi;

} PROPCALLBACKINFO, *LPPROPCALLBACKINFO;

class CDlgEditParam : public CDialog
{
// Construction
public:
	BOOL FillCombo();
	void SetComboWidth();
	void OnCtrl1Changed();
	void OnCtrl1Clicked();
	void DeleteControls( BOOL bRefresh = TRUE );
	BOOL Edit( CWnd *pWnd, LPRECT pRect,  LPPROPINFO ppi );
	void SetEditProp( LPPROPINFO ppi );
	void Size();
	BOOL Refresh( DWORD dwAll, LPPROPINFO ppi );

	CDlgEditParam(CWnd* pParent = NULL);   // standard constructor
	~CDlgEditParam();

// Dialog Data
	//{{AFX_DATA(CDlgEditParam)
	enum { IDD = IDD_EDITPARAM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEditParam)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEditParam)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	LPPROPINFO			m_prop;
	DWORD				m_type;

	CWnd				*m_ctrl[ 4 ];

	HWND				m_hWndCallback;
	UINT				m_uMsgCombo;
	UINT				m_uMsgRefresh;
	UINT				m_uMsgSetCombo;

	char				m_szPath[ CWF_STRSIZE ];

public:
	void OnCtrl2Clicked();
	void FillFileCombo();
	void OnSelchangeCombo();

	void SetCallbackWnd( HWND hWnd, UINT uMsgCombo, UINT uMsgRefresh, UINT uMsgSetCombo )
	{	m_hWndCallback = hWnd; m_uMsgCombo = uMsgCombo; 
		m_uMsgRefresh = uMsgRefresh; m_uMsgSetCombo = uMsgSetCombo; }

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITPARAM_H__7BD10332_86A1_4A3B_9B20_B64A863448C5__INCLUDED_)
