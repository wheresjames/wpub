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
// DlgMsgBox.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgMsgBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIMERID		101

/////////////////////////////////////////////////////////////////////////////
// CDlgMsgBox dialog


CDlgMsgBox::CDlgMsgBox(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMsgBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMsgBox)
	m_sMsg = _T("");
	//}}AFX_DATA_INIT

	m_dwType = 0;
	m_uTimerID = 0;
	m_dwTimeout = 8;

	m_hIcon = NULL;
}


void CDlgMsgBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMsgBox)
	DDX_Control(pDX, IDC_MSGICON, m_msgicon);
	DDX_Text(pDX, IDC_MSG, m_sMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMsgBox, CDialog)
	//{{AFX_MSG_MAP(CDlgMsgBox)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMsgBox message handlers

BOOL CDlgMsgBox::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText( m_sTitle );

	// Set icons
	SetIcon( m_hIcon, TRUE );
	SetIcon( m_hIcon, FALSE );
	m_msgicon.SetIcon( m_hIcon );	

	m_uTimerID = SetTimer( TIMERID, 1000, NULL );

	CWnd *pOk = GetDlgItem( IDOK );
	CWnd *pCancel = GetDlgItem( IDCANCEL );

	if ( m_dwType == MB_YESNO ) 
	{	if ( pOk != NULL ) pOk->SetWindowText( "Yes" );
		if ( pCancel != NULL ) pCancel->SetWindowText( "No" );
	} // end if
	else 
	{	if ( pOk != NULL ) pOk->SetWindowText( "Ok" );
		if ( pCancel != NULL ) pCancel->ShowWindow( SW_HIDE );
	} // end if

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgMsgBox::ShowMessage( LPCTSTR pTitle, LPCTSTR pMsg, HICON hIcon, DWORD dwType, DWORD dwTimeout)
{
	if ( pTitle == NULL ) m_sTitle.Empty();
	else m_sTitle = pTitle;

	if ( pMsg == NULL ) m_sMsg.Empty();
	else m_sMsg = pMsg;

	m_dwTimeout = dwTimeout;

	m_hIcon = hIcon;

	m_dwType = dwType;

	return DoModal();
}

void CDlgMsgBox::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent == TIMERID )
	{
		if ( m_dwTimeout )
		{	
			char msg[ 256 ];
			wsprintf( msg, "Auto close in %lu", m_dwTimeout );
			CWnd *pCtrl = GetDlgItem( IDC_TIMEOUT );
			if ( pCtrl != NULL && ::IsWindow( pCtrl->GetSafeHwnd() ) )
			{	pCtrl->SetWindowText( msg );
				pCtrl->RedrawWindow();
			} // end if

			// Next time
			m_dwTimeout--;

		} // end if

		else
		{
			// Kill the timer
			if ( m_uTimerID != 0 )
			{	KillTimer( m_uTimerID );
				m_uTimerID = 0;
			} // end if

			// Cancel the dialog box
			OnCancel();

		} // end else 

	} // end if
	
	CDialog::OnTimer(nIDEvent);
}

void CDlgMsgBox::OnOK() 
{
	// Kill the timer
	if ( m_uTimerID != 0 )
	{	KillTimer( m_uTimerID );
		m_uTimerID = 0;
	} // end if
	
	CDialog::OnOK();
}

void CDlgMsgBox::OnCancel() 
{
	// Kill the timer
	if ( m_uTimerID != 0 )
	{	KillTimer( m_uTimerID );
		m_uTimerID = 0;
	} // end if
	
	CDialog::OnCancel();
}
