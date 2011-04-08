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
#if !defined(AFX_PGPUBSOUNDS_H__99091AE3_584F_4532_800F_30E77FB59D7D__INCLUDED_)
#define AFX_PGPUBSOUNDS_H__99091AE3_584F_4532_800F_30E77FB59D7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PgPubSounds.h : header file
//

#include "IconButton.h"

/////////////////////////////////////////////////////////////////////////////
// CPgPubSounds dialog

class CPgPubSounds : public CDialog
{
// Construction
public:
	void DoEnable();
	LRESULT OnSave( WPARAM wParam, LPARAM lParam );
	LRESULT OnRefresh( WPARAM wParam, LPARAM lParam );
	CPgPubSounds(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPgPubSounds)
	enum { IDD = IDD_PUBSOUNDS };
	CIconButton	m_btnPlay;
	CIconButton	m_btnChooseSound;
	CString	m_sSoundFile;
	BOOL	m_bEnableSound;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgPubSounds)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual void OnOK(){}
	virtual void OnCancel(){}

	// Generated message map functions
	//{{AFX_MSG(CPgPubSounds)
	virtual BOOL OnInitDialog();
	afx_msg void OnChooseSound();
	afx_msg void OnPlay();
	afx_msg void OnChangeSoundfile();
	afx_msg void OnEnablesound();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PGPUBSOUNDS_H__99091AE3_584F_4532_800F_30E77FB59D7D__INCLUDED_)
