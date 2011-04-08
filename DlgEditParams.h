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
#if !defined(AFX_DLGEDITPARAMS_H__0C9CAD4C_103C_46D3_A0CB_30494FD613E5__INCLUDED_)
#define AFX_DLGEDITPARAMS_H__0C9CAD4C_103C_46D3_A0CB_30494FD613E5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgEditParams.h : header file
//

#include "Fx.h"
#include "ImgEffects.h"
#include "Params.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgEditParams dialog

class CDlgEditParams : public CDialog
{
// Construction
public:
	void FillList();
	CDlgEditParams(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgEditParams)
	enum { IDD = IDD_EDITPARAMS };
	CSliderCtrl	m_slideValue;
	CListCtrl	m_list;
	CString	m_sValue;
	CString	m_sDesc;
	BOOL	m_bUpdate;
	BOOL	m_bBool;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgEditParams)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgEditParams)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeValue();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnOK();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBool();
	afx_msg void OnUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BOOL GetParams( LPPARAMINFO ppi, DWORD i, double *pos, double *min, double *max, double *range );
	int GetFirstListSel( CListCtrl *pList );
	void SetEdit( DWORD i );

/*	void SetEditParam( LPFX pfx, LPPARAMINFO ppi )
	{	m_pfx = pfx;
		if ( pfx == NULL || pfx->imgfx == NULL || ppi == NULL ) 
		{	m_params = NULL; m_ppi = NULL; return; }
		m_params = &pfx->imgfx->m_params;
		m_ppi = ppi;
	}
*/
	void SetEditParam( CParams *params, LPPARAMINFO ppi )
	{	if( params == NULL || ppi == NULL )
		{	m_params = NULL; m_ppi = NULL; }
		else { m_params = params; m_ppi = ppi; }
	}


private:

//	LPFX			m_pfx;
	CParams			*m_params;
	LPPARAMINFO		m_ppi;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEDITPARAMS_H__0C9CAD4C_103C_46D3_A0CB_30494FD613E5__INCLUDED_)
