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
#if !defined(AFX_VPPROPERTIES_H__E522CE43_7999_4586_A653_8AC1401EFFF2__INCLUDED_)
#define AFX_VPPROPERTIES_H__E522CE43_7999_4586_A653_8AC1401EFFF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VpProperties.h : header file
//

#include "Fx.h"
#include "PropertiesList.h"

/////////////////////////////////////////////////////////////////////////////
// CVpProperties window

class CVpProperties : public CPropertiesList
{
// Construction
public:
	CVpProperties();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVpProperties)
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual BOOL FillCombo( DWORD dwUser, CComboBox *pCombo );
	virtual LRESULT GetString( LPPROPINFO ppi, LPSTR str );
	virtual BOOL Refresh( DWORD dwAll, LPPROPINFO ppi );
	virtual BOOL SetComboData( DWORD dwUser, LPPROPCALLBACKINFO pPci );

	BOOL Show( LPFX pfx );
	virtual ~CVpProperties();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVpProperties)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VPPROPERTIES_H__E522CE43_7999_4586_A653_8AC1401EFFF2__INCLUDED_)
