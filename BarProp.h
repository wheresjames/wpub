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
#if !defined(AFX_BARPROP_H__F90B1109_7F26_4F21_A03E_9BB22A43A156__INCLUDED_)
#define AFX_BARPROP_H__F90B1109_7F26_4F21_A03E_9BB22A43A156__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BarProp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBarProp dialog

class CBarProp : public CDialogBar
{
// Construction
public:
	void Size();
	BOOL UpdateViewPos();

	CSize CalcDynamicLayout( int nLength, DWORD dwMode );
	BOOL Create( CWnd *pParent, DWORD style, UINT idd, UINT idview );
	
	CBarProp(CWnd* pParent = NULL);   // standard constructor

	void SetView( CWnd *pView );
	CWnd* GetView() { return m_view; }

// Dialog Data
	//{{AFX_DATA(CBarProp)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBarProp)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBarProp)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	CWnd			*m_view;
	RECT			m_viewrect;

	DWORD			m_dwMode;

	BOOL			m_bChangeDockedSize;
	CSize			m_sizeFloating;
	CSize			m_sizeDocked;

	BOOL			m_bHorz;

	POINT			m_pt;
	RECT			m_rect;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BARPROP_H__F90B1109_7F26_4F21_A03E_9BB22A43A156__INCLUDED_)
