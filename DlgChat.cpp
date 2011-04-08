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
// DlgChat.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgChat.h"

#include "DlgTreeFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgChat dialog


CDlgChat::CDlgChat(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChat::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgChat)
	m_bEnableChat = FALSE;
	m_sMaxLines = _T("");
	m_bShowAddr = FALSE;
	m_bShowUsername = FALSE;
	m_bAllowHtml = FALSE;
	m_sMaxMsgLen = _T("");
	//}}AFX_DATA_INIT
}


void CDlgChat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgChat)
	DDX_Check(pDX, IDC_ENABLECHAT, m_bEnableChat);
	DDX_Text(pDX, IDC_MAXLINES, m_sMaxLines);
	DDX_Check(pDX, IDC_SHOWADDR, m_bShowAddr);
	DDX_Check(pDX, IDC_SHOWUSERNAME, m_bShowUsername);
	DDX_Check(pDX, IDC_ALLOWHTML, m_bAllowHtml);
	DDX_Text(pDX, IDC_MAXMSGLEN, m_sMaxMsgLen);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgChat, CDialog)
	//{{AFX_MSG_MAP(CDlgChat)
	ON_BN_CLICKED(IDC_ENABLECHAT, OnEnablechat)
	ON_EN_CHANGE(IDC_MAXLINES, OnChangeMaxlines)
	ON_BN_CLICKED(IDC_SHOWADDR, OnShowaddr)
	ON_BN_CLICKED(IDC_SHOWUSERNAME, OnShowusername)
	ON_BN_CLICKED(IDC_ALLOWHTML, OnAllowhtml)
	ON_EN_CHANGE(IDC_MAXMSGLEN, OnChangeMaxmsglen)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_PG_SAVE, OnSave )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgChat message handlers

BOOL CDlgChat::OnInitDialog() 
{
	CDialog::OnInitDialog();

	OnRefresh( 0, 0L );

	DoEnable();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgChat::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	m_bEnableChat = CFG().GetDword( "Settings", "EnableChat", 1 ) != 0;
	m_bShowAddr = CFG().GetDword( "Settings", "ChatShowAddr", 1 ) != 0;
	m_bShowUsername = CFG().GetDword( "Settings", "ChatShowUsername", 1 ) != 0;
	m_sMaxLines = CFG().GetSz( "Settings", "ChatMaxLines", "8" );
	m_sMaxMsgLen = CFG().GetSz( "Settings", "ChatMaxMsgLen", "80" );
	m_bAllowHtml = CFG().GetDword( "Settings", "ChatAllowHtml", 0 ) != 0;

	UpdateData( FALSE );

	return 0;
}

LRESULT CDlgChat::OnSave(WPARAM wParam, LPARAM lParam)
{
	UpdateData( TRUE );

	CFG().SetValue( "Settings", "EnableChat", (DWORD)(m_bEnableChat ? 1 : 0 ) );
	CFG().SetValue( "Settings", "ChatShowAddr", (DWORD)(m_bShowAddr ? 1 : 0 ) );
	CFG().SetValue( "Settings", "ChatShowUsername", (DWORD)(m_bShowUsername ? 1 : 0 ) );
	CFG().SetValue( "Settings", "ChatMaxLines", (DWORD)strtoul( m_sMaxLines, NULL, 10 ) );
	CFG().SetValue( "Settings", "ChatMaxMsgLen", (DWORD)strtoul( m_sMaxMsgLen, NULL, 10 ) );
	CFG().SetValue( "Settings", "ChatAllowHtml", (DWORD)(m_bAllowHtml ? 1 : 0 ) );

	return 0;
}

void CDlgChat::DoEnable()
{
	CComCtrl::EnableCtrl( GetSafeHwnd(), IDC_SHOWADDR, m_bEnableChat );
	CComCtrl::EnableCtrl( GetSafeHwnd(), IDC_SHOWUSERNAME, m_bEnableChat );
	CComCtrl::EnableCtrl( GetSafeHwnd(), IDC_TXT_MAXLINES, m_bEnableChat );
	CComCtrl::EnableCtrl( GetSafeHwnd(), IDC_MAXLINES, m_bEnableChat );
	CComCtrl::EnableCtrl( GetSafeHwnd(), IDC_ALLOWHTML, m_bEnableChat );
}

void CDlgChat::OnEnablechat() 
{	UpdateData( TRUE ); DoEnable();	
	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}

void CDlgChat::OnChangeMaxlines() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}

void CDlgChat::OnShowaddr() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}

void CDlgChat::OnShowusername() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}

void CDlgChat::OnAllowhtml() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}

void CDlgChat::OnChangeMaxmsglen() 
{	CDlgTreeFrame::EnableApply( GetSafeHwnd() );
}
