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
// DlgServerBar.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgServerBar.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgServerBar dialog


CDlgServerBar::CDlgServerBar(CWnd* pParent /*=NULL*/)
	: CDialogBar()
{
	//{{AFX_DATA_INIT(CDlgServerBar)
	m_bRun = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgServerBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgServerBar)
	DDX_Check(pDX, IDC_RUN, m_bRun);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgServerBar, CDialogBar)
	//{{AFX_MSG_MAP(CDlgServerBar)
	ON_BN_CLICKED(IDC_RUN, OnRun)
	//}}AFX_MSG_MAP
#ifdef NEW_CDIALOGBAR
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
#endif
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgServerBar message handlers

#ifdef NEW_CDIALOGBAR

LONG CDlgServerBar::OnInitDialog(UINT wParam, LONG lParam) 
{
//	CDialogBar::OnInitDialog();
	LONG lRet = HandleInitDialog(wParam, lParam);

	// Is the server running?
	m_bRun = FRAME()->m_iserver.IsConnected();

	UpdateData( FALSE );	
	
	return lRet;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

#else

BOOL CDlgServerBar::OnInitDialog() 
{
	CDialogBar::OnInitDialog();

	// Is the server running?
	m_bRun = FRAME()->m_iserver.IsConnected();

	UpdateData( FALSE );	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

#endif

void CDlgServerBar::OnRun() 
{
	UpdateData( TRUE );

	if ( m_bRun ) FRAME()->StartServer();
	else FRAME()->StopServer();	
}

