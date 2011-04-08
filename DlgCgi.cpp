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
// DlgCgi.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "DlgCgi.h"

#include "DlgAddCgi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgCgi dialog


CDlgCgi::CDlgCgi(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCgi::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCgi)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgCgi::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCgi)
	DDX_Control(pDX, IDC_LIST, m_list);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCgi, CDialog)
	//{{AFX_MSG_MAP(CDlgCgi)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCgi message handlers

BOOL CDlgCgi::OnInitDialog() 
{
	CDialog::OnInitDialog();

	RECT rect;
	m_list.GetClientRect( &rect );
	long w = ( rect.right - rect.left ) - GetSystemMetrics( SM_CXVSCROLL ) - 4;
	m_list.InsertColumn( 0, "Name", LVCFMT_LEFT, w / 3 );
	m_list.InsertColumn( 1, "File", LVCFMT_LEFT, w / 3 );
	m_list.InsertColumn( 2, "Extension", LVCFMT_LEFT, w / 3 );

	ListCtrl_SetExtendedListViewStyle( m_list.GetSafeHwnd(), LVS_EX_FULLROWSELECT );

	m_images.Create( 16, 16, ILC_MASK | ILC_COLOR24, 0, 32 );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_CGI ) );		
	m_list.SetImageList( &m_images, LVSIL_NORMAL );
	m_list.SetImageList( &m_images, LVSIL_SMALL );

	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgCgi::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	FillList();

	return 0;
}

void CDlgCgi::FillList()
{
	m_list.DeleteAllItems();

	HREGKEY				hRk = NULL;
	while (		( hRk = (HREGKEY)CGI().GetNext( hRk ) ) != NULL &&
				hRk->key != NULL )
	{
		CRKey *pRk = hRk->key;

		// Insert the item
		int item = m_list.InsertItem( m_list.GetItemCount(), pRk->GetName(),0 );

		// Set address text
		if ( item != LB_ERR )
		{
			m_list.SetItemText( item, 1, pRk->GetSz( "Exe" ) );
			m_list.SetItemText( item, 2, pRk->GetSz( "Ext" ) );

		} // end if

	} // end while

}

void CDlgCgi::OnAdd() 
{
	CDlgAddCgi	dlg;
	if ( dlg.DoModal() != IDOK ) return;

	CGI().Set( dlg.m_sName, "Exe", (LPCTSTR)dlg.m_sExe );
	CGI().Set( dlg.m_sName, "Ext", (LPCTSTR)dlg.m_sExt );
	CGI().Set( dlg.m_sName, "Params", (LPCTSTR)dlg.m_sParams );

	FillList();
}

void CDlgCgi::OnRemove() 
{
	UpdateData( TRUE );

	// Get the selected item
	int sel = CComCtrl::ListGetFirstSelectedItem( m_list.GetSafeHwnd() );
	if ( sel == LB_ERR ) return;

	// Remove the server
	CString str = m_list.GetItemText( sel, 0 );
	CGI().Delete( CGI().Find( str ) );

	// Refresh list
	FillList();
}

void CDlgCgi::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CDlgAddCgi	dlg;

	// Get the selected item
	int sel = CComCtrl::ListGetFirstSelectedItem( m_list.GetSafeHwnd() );
	if ( sel == LB_ERR ) return;

	// Remove the server
	CString str = m_list.GetItemText( sel, 0 );

	dlg.m_sName = str;
	dlg.m_sExe = CGI().GetSz( str, "Exe" );
	dlg.m_sExt = CGI().GetSz( str, "Ext" );
	dlg.m_sParams = CGI().GetSz( str, "Params" );

	if ( dlg.DoModal() != IDOK ) return;

	CGI().Set( dlg.m_sName, "Exe", (LPCTSTR)dlg.m_sExe );
	CGI().Set( dlg.m_sName, "Ext", (LPCTSTR)dlg.m_sExt );
	CGI().Set( dlg.m_sName, "Params", (LPCTSTR)dlg.m_sParams );

	FillList();
	
	*pResult = 0;
}
