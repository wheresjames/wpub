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
// DlgSplash.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgSplash.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TIMERID		101

/////////////////////////////////////////////////////////////////////////////
// CDlgSplash dialog


CDlgSplash::CDlgSplash(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSplash::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSplash)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bRun = FALSE;
	m_dwMinDisplay = 3000;
	m_uTimerId = 0;
	m_hCenter = NULL;
}


void CDlgSplash::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSplash)
	DDX_Control(pDX, IDC_IMG, m_img);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSplash, CDialog)
	//{{AFX_MSG_MAP(CDlgSplash)
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSplash message handlers


BOOL CDlgSplash::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Size window
	Size();

	// Set on top and center us up
	::SetWindowPos( GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, 
					SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE );

	// Center the splash box
	if ( m_hCenter != NULL ) CenterWindow( CWnd::FromHandle( m_hCenter ) );
	else CenterWindow( GetDesktopWindow() );

	// Set the timer
	m_uTimerId = ::SetTimer( GetSafeHwnd(), TIMERID, m_dwMinDisplay, NULL );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSplash::Size()
{
	// Punt if no window
	if ( !::IsWindow( GetSafeHwnd() ) ) return;

	long cx = m_img.GetWidth();
	long cy = m_img.GetHeight();

	// Set bitmap to the same size as the image
	::SetWindowPos( GetSafeHwnd(), NULL, 0, 0, cx, cy,
					SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE );

	// Size image
	CWnd *pWnd = GetDlgItem( IDC_IMG );
	if ( pWnd != NULL ) pWnd->SetWindowPos( NULL, 0, 0, cx, cy,
											SWP_NOZORDER | SWP_NOACTIVATE );	

}

BOOL CDlgSplash::Run( HWND hWnd, DWORD img, LPCTSTR pType, DWORD idd, DWORD dwMinDisplay )
{
	m_bRun = TRUE;
	m_dwMinDisplay = dwMinDisplay;

	if ( idd == NULL ) idd = IDD;
	m_hCenter = hWnd;

	// Load the splash image
	m_img.Load( MAKEINTRESOURCE( img ), "BUFFER", pType );

	// Create the splash box
	if ( !Create( idd, GetDesktopWindow() ) ) return FALSE;

	// We want to see it
	ShowWindow( SW_SHOWNORMAL );

	return TRUE;
}

void CDlgSplash::End()
{
	m_bRun = FALSE;
}

void CDlgSplash::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	Size();	
}


void CDlgSplash::OnTimer(UINT nIDEvent) 
{
	if ( nIDEvent = TIMERID )
	{
		// Is it time to quit?
		if ( !m_bRun )
		{
			// Lose previous timer
			if ( m_uTimerId != 0 )
			{	::KillTimer( GetSafeHwnd(), TIMERID );
				m_uTimerId = 0;
			} // end if
			
			// Kill the splash box
			DestroyWindow();

		} // end if

		else if ( m_dwMinDisplay != 1000 )
		{
			// Change timer to 1 second
			m_dwMinDisplay = 1000;

			// Lose previous timer
			if ( m_uTimerId != 0 )
			{	::KillTimer( GetSafeHwnd(), TIMERID );
				m_uTimerId = 0;
			} // end if

			// Keep checking every second for run stop
			m_uTimerId = ::SetTimer( GetSafeHwnd(), TIMERID, 1000, NULL );

		} // end else if

	} // end if
	
	CDialog::OnTimer(nIDEvent);
}


BOOL CDlgSplash::DestroyWindow() 
{
	
	return CDialog::DestroyWindow();
}
