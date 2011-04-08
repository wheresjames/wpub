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
#if !defined(AFX_TITLEWND_H__3C9473F9_F6A0_46FC_AE2C_AB30CCC067FA__INCLUDED_)
#define AFX_TITLEWND_H__3C9473F9_F6A0_46FC_AE2C_AB30CCC067FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TitleWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTitleWnd window

class CTitleWnd : public CStatic
{
// Construction
public:
	CTitleWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTitleWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTitleWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTitleWnd)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:
	BOOL GetTextSize( LPRECT pRect );

	void SetTitle( LPCTSTR title )
	{	if ( title == NULL ) *m_szTitle = 0; else strcpy( m_szTitle, title ); }

	void Center( BOOL b ) { m_bCenter = b; }

	void SetFont( DWORD size, LPCTSTR pFont )
	{	m_dwFontSize = size; strcpy( m_szFont, pFont ); }

	void SetWindowColor( COLORREF rgb ) { m_rgbWindow = rgb; }

	void SetTitleColor( COLORREF rgb ) { m_rgbText = rgb; }

private:

	BOOL		m_bCenter;
	char		m_szTitle[ 256 ];

	DWORD		m_dwFontSize;
	char		m_szFont[ 256 ];

	COLORREF	m_rgbWindow;
	COLORREF	m_rgbText;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TITLEWND_H__3C9473F9_F6A0_46FC_AE2C_AB30CCC067FA__INCLUDED_)
