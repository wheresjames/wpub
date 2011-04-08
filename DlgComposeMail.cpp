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
// DlgComposeMail.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgComposeMail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgComposeMail dialog


CDlgComposeMail::CDlgComposeMail(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgComposeMail::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgComposeMail)
	m_sFrom = _T("");
	m_sMsg = _T("");
	m_sSubject = _T("");
	m_sTo = _T("");
	//}}AFX_DATA_INIT
}


void CDlgComposeMail::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgComposeMail)
	DDX_Control(pDX, IDC_FROM, m_comboFrom);
	DDX_Control(pDX, IDC_TO, m_comboTo);
	DDX_CBString(pDX, IDC_FROM, m_sFrom);
	DDX_Text(pDX, IDC_MSG, m_sMsg);
	DDX_Text(pDX, IDC_SUBJECT, m_sSubject);
	DDX_CBString(pDX, IDC_TO, m_sTo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgComposeMail, CDialog)
	//{{AFX_MSG_MAP(CDlgComposeMail)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgComposeMail message handlers

BOOL CDlgComposeMail::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetIcon( AfxGetApp()->LoadIcon( IDI_LETTER ), TRUE );
	SetIcon( AfxGetApp()->LoadIcon( IDI_SMALL_LETTER ), FALSE );
	
	Size();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgComposeMail::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	Size();	
}

void CDlgComposeMail::Size()
{
	RECT rect;
	GetClientRect( &rect );

	CWnd *pTo = GetDlgItem( IDC_TO );
	CWnd *pFrom = GetDlgItem( IDC_FROM );
	CWnd *pSubject = GetDlgItem( IDC_SUBJECT );
	CWnd *pMsg = GetDlgItem( IDC_MSG );

	CWnd *pOk = GetDlgItem( IDOK );
	CWnd *pCancel = GetDlgItem( IDCANCEL );

	// Verify pointers
	if (	pTo == NULL || pFrom == NULL || pSubject == NULL ||
			pMsg == NULL || pOk == NULL || pCancel == NULL )
		return;

	RECT ok, ref;
	pOk->GetWindowRect( &ok );
	ScreenToClient( &ok );

	pTo->GetWindowRect( &ref );
	ScreenToClient( &ref );
	ref.right = rect.right - RW( ok ) - 4;
	pTo->SetWindowPos( NULL, ref.left, ref.top, RW( ref ), RH( ref ),
						SWP_NOZORDER | SWP_NOACTIVATE );

	ref.right = rect.right - 2;
	ref.left = ref.right - RW( ok );
	ref.bottom = ref.top + RH( ok );
	pOk->SetWindowPos( NULL, ref.left, ref.top, RW( ref ), RH( ref ),
						SWP_NOZORDER | SWP_NOACTIVATE );

	ref.top = ref.bottom + 2;
	ref.bottom = ref.top + RH( ok );
	pCancel->SetWindowPos( NULL, ref.left, ref.top, RW( ref ), RH( ref ),
							SWP_NOZORDER | SWP_NOACTIVATE );

	pFrom->GetWindowRect( &ref );
	ScreenToClient( &ref );
	ref.right = rect.right - RW( ok ) - 4;
	pFrom->SetWindowPos( NULL, ref.left, ref.top, RW( ref ), RH( ref ),
							SWP_NOZORDER | SWP_NOACTIVATE );

	pSubject->GetWindowRect( &ref );
	ScreenToClient( &ref );
	ref.right = rect.right - RW( ok ) - 4;
	pSubject->SetWindowPos( NULL, ref.left, ref.top, RW( ref ), RH( ref ),
							SWP_NOZORDER | SWP_NOACTIVATE );

	pMsg->GetWindowRect( &ref );
	ScreenToClient( &ref );
	rect.top = ref.top;
	pMsg->SetWindowPos( NULL, rect.left, rect.top, RW( rect ), RH( rect ),
						SWP_NOZORDER | SWP_NOACTIVATE );


}
