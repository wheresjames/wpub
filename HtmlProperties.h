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
#if !defined(AFX_HTMLPROPERTIES_H__59E37495_4AC1_4A5D_A2AD_4D6C591E8CD3__INCLUDED_)
#define AFX_HTMLPROPERTIES_H__59E37495_4AC1_4A5D_A2AD_4D6C591E8CD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HtmlProperties.h : header file
//

#include "PropertiesList.h"

/////////////////////////////////////////////////////////////////////////////
// CHtmlProperties window

class CHtmlProperties : public CPropertiesList
{
// Construction
public:
	CHtmlProperties();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHtmlProperties)
	//}}AFX_VIRTUAL

// Implementation
public:
	DWORD GetParams(LPCTSTR str);
	BOOL AddKey( CReg *pReg, CRKey *pRk );

	virtual BOOL FillCombo( DWORD dwUser, CComboBox *pCombo );
	virtual LRESULT GetString( LPPROPINFO ppi, LPSTR str );
	
	BOOL Show( CReg *pReg );
	virtual ~CHtmlProperties();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHtmlProperties)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

	char		m_szParams[ 32 ][ 256 ];
	long		m_lParams[ 32 ];
	DWORD		m_dwParams;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTMLPROPERTIES_H__59E37495_4AC1_4A5D_A2AD_4D6C591E8CD3__INCLUDED_)
