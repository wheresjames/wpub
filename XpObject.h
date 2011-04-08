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
#if !defined(AFX_XPOBJECT_H__23873FC2_2C43_42A3_9484_BE1C565A2E01__INCLUDED_)
#define AFX_XPOBJECT_H__23873FC2_2C43_42A3_9484_BE1C565A2E01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// XpObject.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CXpObject window

class CXpObject : public CStatic
{
// Construction
public:
	CXpObject();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXpObject)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CXpObject();

	// Generated message map functions
protected:
	//{{AFX_MSG(CXpObject)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:

	void SetInfo( LPCTSTR pClass, int iPartId, int iStateId, HICON hIcon )
	{	SetPart( pClass, iPartId, iStateId ); SetIcon( hIcon ); }

	void SetPart( LPCTSTR pClass, int iPartId, int iStateId )
	{	if ( pClass == NULL ) *m_szClass = 0; else strcpy( m_szClass, pClass );
		m_iPartId = iPartId; m_iStateId = iStateId;
	}
	void SetIcon( HICON hIcon ) { m_hIcon = hIcon; }

private:

	char			m_szClass[ 256 ];
	int				m_iPartId;
	int				m_iStateId;

	HICON			m_hIcon;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XPOBJECT_H__23873FC2_2C43_42A3_9484_BE1C565A2E01__INCLUDED_)
