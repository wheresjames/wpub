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
#if !defined(AFX_IMAGEVIEW_H__25D7D6DD_4321_4F9D_8460_5214C6ABC8C9__INCLUDED_)
#define AFX_IMAGEVIEW_H__25D7D6DD_4321_4F9D_8460_5214C6ABC8C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImageView.h : header file
//

#include "VideoList.h"

/////////////////////////////////////////////////////////////////////////////
// CImageView window

class CImageView : public CStatic
{
// Construction
public:
	CImageView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageView)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CImageView();

	void SetVideoInfo( LPVIDEOINFO pvi ) { m_pvi = pvi; }
	void SetImage( DWORD i ) { m_imgindex = i; RedrawWindow(); }
	void SetImage( CWinDc *pos ) { m_pos = pos; RedrawWindow(); }

	// Generated message map functions
protected:
	//{{AFX_MSG(CImageView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:

	void DrawMotion( BOOL b ) { m_bDrawMotion = b; }

private:

	BOOL			m_bDrawMotion;

	DWORD			m_imgindex;

	CWinDc			*m_pos;

	LPVIDEOINFO		m_pvi;

	CWinDc			m_offscreen;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEVIEW_H__25D7D6DD_4321_4F9D_8460_5214C6ABC8C9__INCLUDED_)
