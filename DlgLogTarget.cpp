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
// DlgLogTarget.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgLogTarget.h"
#include "LogThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgLogTarget dialog


CDlgLogTarget::CDlgLogTarget(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogTarget::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgLogTarget)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_dwCreateInform = 0;
}


void CDlgLogTarget::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgLogTarget)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgLogTarget, CDialog)
	//{{AFX_MSG_MAP(CDlgLogTarget)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_SHOWSTATUS, OnShowStatus )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgLogTarget message handlers

BOOL CDlgLogTarget::OnInitDialog() 
{
	CDialog::OnInitDialog();
	ShowWindow( SW_HIDE );	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgLogTarget::OnShowStatus(WPARAM wParam, LPARAM lParam)
{
	if ( !::IsWindow( GetSafeHwnd() ) ) return 0;

	// Aquire thread lock
	if ( !m_informlock.Lock( 0 ) ) return 0;

	// Get status string
	LPCTSTR pStatus = (LPCTSTR)lParam;

	if ( wParam == 1 )
	{	
		if ( pStatus != NULL )
		{	if ( !::IsWindow( m_inform.GetSafeHwnd() ) )
			{	m_dwCreateInform = 0; m_inform.Run( pStatus ); }
			else 
			{	m_dwCreateInform++;
				m_inform.SetStatus( pStatus );
			} // end else
		} // end if

		else
		{	if ( m_dwCreateInform ) m_dwCreateInform--; 
			else
			{
				m_inform.DestroyWindow();
				if ( !m_informlock.Unlock() ) return 0;
			} // end else
		} // end else

	} // end if

	// Just set the status
	else if ( pStatus != NULL ) m_inform.SetStatus( pStatus );

	return 1;
}


CDoInform::CDoInform( LPCTSTR pStr ) 
{
	// +++ ???
//	APP()->SendLogMessage( 1, (LPARAM)pStr ); 
}

CDoInform::~CDoInform() 
{
	End(); 
}


void CDoInform::Inform( LPCTSTR pStr ) 
{
//	APP()->SendLogMessage( 2, (LPARAM)pStr ); 
}
	
void CDoInform::End() 
{ 
//	APP()->SendLogMessage( 1, NULL ); 
}
