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
#if !defined(AFX_SHOWIMG_H__11CA96D1_5364_46DD_915E_5E8A861C90F5__INCLUDED_)
#define AFX_SHOWIMG_H__11CA96D1_5364_46DD_915E_5E8A861C90F5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ShowImg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShowImg window

class CShowImg : public CStatic
{
// Construction
public:
	CShowImg();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowImg)
	public:
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Load( LPCTSTR pRes, LPCTSTR pResType, LPCTSTR pType );
	virtual ~CShowImg();

	// Generated message map functions
protected:
	//{{AFX_MSG(CShowImg)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysColorChange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	CWinImg			m_img;
//	CWinDc			m_bck;
	CWinDc			m_offscreen;

	CWinDc			*m_showdc;

public:
	void CaptureShowDc();
	BOOL Regen();
	BOOL LoadFile( LPCTSTR pFile );
	
	void ClearImg() { m_img.Destroy(); m_offscreen.Destroy(); }

	CWinImg* GetImg() { return &m_img; }

	void SetShowDc( CWinDc *pdc ) { m_showdc = pdc; }
	CWinDc* GetShowDc() { return m_showdc; }

	long GetWidth() 
	{ if ( m_img.IsValid() ) return m_img.GetWidth(); return 256; }
	long GetHeight() 
	{ if ( m_img.IsValid() ) return m_img.GetHeight(); return 256; }

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWIMG_H__11CA96D1_5364_46DD_915E_5E8A861C90F5__INCLUDED_)
