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
// DlgServerLog.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgServerLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgServerLog dialog


CDlgServerLog::CDlgServerLog(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgServerLog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgServerLog)
	m_sLogFile = _T("");
	//}}AFX_DATA_INIT
}


void CDlgServerLog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgServerLog)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_LOGFILE, m_sLogFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgServerLog, CDialog)
	//{{AFX_MSG_MAP(CDlgServerLog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgServerLog message handlers

BOOL CDlgServerLog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	RECT rect;
	m_list.GetClientRect( &rect );
	long w = ( rect.right - rect.left ) - GetSystemMetrics( SM_CXVSCROLL ) - 4;
	w *= 4;

	// Insert columns
	m_list.InsertColumn( 0, "Events", LVCFMT_LEFT, w );
//	m_list.InsertColumn( 1, "Page", LVCFMT_LEFT, w );

	m_images.Create( 16, 16, ILC_MASK | ILC_COLOR24, 0, 32 );
	m_images.Add( ::LoadIcon( NULL, IDI_APPLICATION ) );	// 0
	m_images.Add( ::LoadIcon( NULL, IDI_ASTERISK ) );		// 1
	m_images.Add( ::LoadIcon( NULL, IDI_ERROR ) );			// 2
	m_images.Add( ::LoadIcon( NULL, IDI_EXCLAMATION ) );	// 3
	m_images.Add( ::LoadIcon( NULL, IDI_HAND ) );			// 4
	m_images.Add( ::LoadIcon( NULL, IDI_INFORMATION ) );	// 5
	m_images.Add( ::LoadIcon( NULL, IDI_QUESTION ) );		// 6
	m_images.Add( ::LoadIcon( NULL, IDI_WARNING ) );		// 7
	m_images.Add( ::LoadIcon( NULL, IDI_WINLOGO ) );		// 8

	m_list.SetImageList( &m_images, LVSIL_NORMAL );
	m_list.SetImageList( &m_images, LVSIL_SMALL );

	FillList();

	char path[ CWF_STRSIZE ];
	CWinFile::GetExePath( path );
	CWinFile::BuildPath( path, CWinFile::GetExePath( path ), "Weblog.txt" );
	m_sLogFile = path;

	UpdateData( FALSE );
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgServerLog::FillList()
{
	m_list.DeleteAllItems();

	if ( SERVERLOG().Size() == 0 )
	{	m_list.InsertItem( 0, "<emtpy>", 5 );
		return;
	} // end if

	LPLOGINFO	pli = NULL;
	while ( ( pli = (LPLOGINFO)SERVERLOG().GetNext( pli ) ) != NULL )
	{
		int icon = 5;
		if ( ( pli->type & LOGTYPE_TYPEMASK ) == LOGTYPE_WARNING ) icon = 7;
		else if ( ( pli->type & LOGTYPE_TYPEMASK ) == LOGTYPE_ERROR ) icon = 2;
		else if ( ( pli->type & LOGTYPE_TYPEMASK ) == LOGTYPE_WARNING ) icon = 4;

		// Insert the item
		int i = m_list.InsertItem( m_list.GetItemCount(), pli->text, icon );

	} // end while
		

}
