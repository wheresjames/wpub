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
#if !defined(AFX_LOGTHREAD_H__57131851_B1D0_46E3_8DE0_0E567D14634C__INCLUDED_)
#define AFX_LOGTHREAD_H__57131851_B1D0_46E3_8DE0_0E567D14634C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LogThread.h : header file
//

#include "DlgInform.h"
#include "DlgLogTarget.h"


/////////////////////////////////////////////////////////////////////////////
// CLogThread thread

class CLogThread : public CWinThread
{
	DECLARE_DYNCREATE(CLogThread)
protected:
	CLogThread();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CLogThread();

	// Generated message map functions
	//{{AFX_MSG(CLogThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:

public:

	CDlgLogTarget		m_dlglogtarget;

};

class CDoInform
{
public:
	CDoInform( LPCTSTR pStr );
	~CDoInform();
	void Inform( LPCTSTR pStr );	
	void End();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGTHREAD_H__57131851_B1D0_46E3_8DE0_0E567D14634C__INCLUDED_)
