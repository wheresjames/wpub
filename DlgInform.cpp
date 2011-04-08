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
// DlgInform.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgInform.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInform dialog


CDlgInform::CDlgInform(LPCTSTR pText, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInform::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInform)
	//}}AFX_DATA_INIT

	// Go ahead and start if text
	if ( pText != NULL ) Run( pText );
}

CDlgInform::~CDlgInform()
{
	DestroyWindow();
}


void CDlgInform::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInform)
	DDX_Control(pDX, IDC_STATUS, m_status);
	DDX_Control(pDX, IDC_TEXTBOX, m_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInform, CDialog)
	//{{AFX_MSG_MAP(CDlgInform)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInform message handlers

BOOL CDlgInform::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Ensure text is centered
	m_text.Center( TRUE );

	// Setup status text
	m_status.SetFont( 12, "Arial" );
	m_status.SetTitle( NULL );
	m_status.SetTitleColor( RGB( 0, 0, 0 ) );

	// Size the window
	Size();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgInform::Size()
{
	// Ensure we have a valid window
	if ( !::IsWindow( GetSafeHwnd() ) ) return;

	RECT rect;
	m_text.GetTextSize( &rect );

	// Add a little room
//	InflateRect( &rect, 40, 20 );
	rect.right += 60; rect.bottom += 40;

	// correct size
	rect.bottom += GetSystemMetrics( SM_CYCAPTION ) + GetSystemMetrics( SM_CYBORDER ) + 32;
	if ( RW( rect ) < 280 ) rect.right = rect.left + 280;

	// Set window position
	SetWindowPos( NULL, 0, 0, 
						rect.right - rect.left,
						rect.bottom - rect.top,
						SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE );
	CenterWindow();

	GetClientRect( &rect );

	// Set the text control position
	if ( ::IsWindow( m_text.GetSafeHwnd() ) )

		m_text.SetWindowPos( NULL,	rect.left, rect.top, 
									RW( rect ), RH( rect ) - 24,
									SWP_NOZORDER | SWP_NOACTIVATE );

	rect.top = rect.bottom - 24;

	// Set the text control position
	if ( ::IsWindow( m_status.GetSafeHwnd() ) )

		m_status.SetWindowPos( NULL,	rect.left, rect.top, 
										RW( rect ), RH( rect ),
										SWP_NOZORDER | SWP_NOACTIVATE );
}

void CDlgInform::Run(LPCTSTR pText)
{
	SetText( pText );
	SetFont( 20, "Arial" );

	// Create the splash box
	if ( !Create( IDD, GetDesktopWindow() ) ) return;

	// Put us on top
//	::SetWindowPos( GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, 
//					SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );

	// We want to see it
	ShowWindow( SW_SHOWNORMAL );

	RedrawWindow();
}

void CDlgInform::SetStatus(LPCTSTR pStatus)
{
	if ( !::IsWindow( m_status.GetSafeHwnd() ) ) return;

	m_status.SetTitle( pStatus );
	m_status.RedrawWindow();

}
