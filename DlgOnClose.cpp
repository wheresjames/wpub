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
// DlgOnClose.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgOnClose.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOnClose dialog


CDlgOnClose::CDlgOnClose(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOnClose::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOnClose)
	m_nAction = 0;
	m_bDontAsk = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgOnClose::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOnClose)
	DDX_Radio(pDX, IDC_ACTION, m_nAction);
	DDX_Check(pDX, IDC_DONTASK, m_bDontAsk);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOnClose, CDialog)
	//{{AFX_MSG_MAP(CDlgOnClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOnClose message handlers

BOOL CDlgOnClose::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOnClose::OnOK() 
{
	UpdateData( TRUE );

	if ( m_bDontAsk )
	{	if ( m_nAction == 0 ) 
			CFG().SetValue( "Settings", "OnCloseAction", (DWORD)1 );
		else CFG().SetValue( "Settings", "OnCloseAction", (DWORD)2 );
	} // end if

	CDialog::OnOK();
}
