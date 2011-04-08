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
#if !defined(AFX_PUBVIEWDLGBAR_H__14A4ED93_1B0B_456F_B188_609323501CAF__INCLUDED_)
#define AFX_PUBVIEWDLGBAR_H__14A4ED93_1B0B_456F_B188_609323501CAF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PubViewDlgBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPubViewDlgBar dialog

#include "PubImgView.h"

class CPubViewDlgBar : public CDialogBar
{
// Construction
public:
	CSize CalcDynamicLayout( int nLength, DWORD dwMode );
	virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );
	BOOL Create( CWnd *pParent, DWORD style );
//	virtual void OnUpdateCmdUI( CFrameWnd *pFrame, int i );
	CPubViewDlgBar(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPubViewDlgBar)
	enum { IDD = CG_IDD_PUBIMGVIEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPubViewDlgBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPubViewDlgBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

	CPubImgView * GetView() { return &m_pubimgview; }


private:

	CPubImgView		m_pubimgview;

	BOOL			m_bChangeDockedSize;
	CSize			m_sizeFloating;
	CSize			m_sizeDocked;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PUBVIEWDLGBAR_H__14A4ED93_1B0B_456F_B188_609323501CAF__INCLUDED_)
