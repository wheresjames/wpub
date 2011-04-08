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
#if !defined(AFX_COLORBOX_H__0DF7140A_A9CD_11D3_BF5E_00104B2C9CFA__INCLUDED_)
#define AFX_COLORBOX_H__0DF7140A_A9CD_11D3_BF5E_00104B2C9CFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorBox window

class CColorBox : public CStatic
{
// Construction
public:
	CColorBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorBox();

	void SetColor( COLORREF rgb ) { m_rgb = rgb; RedrawWindow(); }
	COLORREF GetColor() { return m_rgb; }

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorBox)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	COLORREF	m_rgb;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORBOX_H__0DF7140A_A9CD_11D3_BF5E_00104B2C9CFA__INCLUDED_)
