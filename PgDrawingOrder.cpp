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
// PgDrawingOrder.cpp : implementation file
//

#include "stdafx.h"
#include "vp.h"
#include "PgDrawingOrder.h"

#include "MainFrm.h"
#include "VpDoc.h"
#include "VpView.h"

#include "DlgTabFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgDrawingOrder dialog


CPgDrawingOrder::CPgDrawingOrder(CWnd* pParent /*=NULL*/)
	: CDialog(CPgDrawingOrder::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPgDrawingOrder)
	m_bFixedOrder = FALSE;
	//}}AFX_DATA_INIT
}


void CPgDrawingOrder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgDrawingOrder)
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Check(pDX, IDC_FIXEDORDER, m_bFixedOrder);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgDrawingOrder, CDialog)
	//{{AFX_MSG_MAP(CPgDrawingOrder)
	ON_BN_CLICKED(IDC_MOVEUP, OnMoveup)
	ON_BN_CLICKED(IDC_MOVEDOWN, OnMovedown)
	ON_BN_CLICKED(IDC_FIXEDORDER, OnFixedorder)
	//}}AFX_MSG_MAP

	ON_MESSAGE( WM_PG_REFRESH, OnRefresh )

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgDrawingOrder message handlers

BOOL CPgDrawingOrder::DestroyWindow() 
{
	m_images.DeleteImageList();
	
	return CDialog::DestroyWindow();
}

BOOL CPgDrawingOrder::OnInitDialog() 
{
	CDialog::OnInitDialog();

	FillListBox();

	RECT rect;
	m_list.GetClientRect( &rect );
	long w = ( rect.right - rect.left ) - GetSystemMetrics( SM_CXVSCROLL ) - 5;

	// Build image list
	m_images.Create( 16, 16, ILC_MASK | ILC_COLOR24, 32, 32 );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_TEXT ) );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_TIME ) );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_DATE ) );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_RECT ) );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_IMAGE ) );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_WATERMARK ) );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_VIDEO ) );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_SCREENSHOT ) );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_URL ) );
	m_images.Add( AfxGetApp()->LoadIcon( IDI_STEXT ) );

	// Set the image list
	m_list.SetImageList( &m_images, LVSIL_NORMAL );
	m_list.SetImageList( &m_images, LVSIL_SMALL );

	// Insert column
	m_list.InsertColumn( 0, "Element", LVCFMT_LEFT, w );

	// Refresh data
	OnRefresh( 0, 0L );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPgDrawingOrder::FillListBox()
{
	LPFX pSel = NULL;
	int sel = GetFirstListSel( &m_list );
	if ( sel != LB_ERR ) pSel = (LPFX)m_list.GetItemData( sel );
	sel = LB_ERR;

	m_list.DeleteAllItems();

	// Get current fx object
	CFx *Fx = APP()->GetActiveFx();
	if ( Fx == NULL ) return;

	DWORD	item = 0;
	LPFX	pfx = NULL;
	int		insert = CB_ERR;

	while ( ( pfx = Fx->GetPrev( pfx ) ) != NULL )
	{
		char temp[ CWF_STRSIZE ];

		switch( pfx->type )
		{
			case FXTYPE_TEXT :
				insert = m_list.InsertItem( item++, pfx->data, 0 );
				break;

			case FXTYPE_TIME :
				insert = m_list.InsertItem( item++, "< Time >", 1 );
				break;

			case FXTYPE_DATE :
				insert = m_list.InsertItem( item++, "< Date >", 2 );
				break;

			case FXTYPE_RECT :
				insert = m_list.InsertItem( item++, "< Rectangle >", 3 );
				break;

			case FXTYPE_IMAGE :
				*temp = 0;
				CWinFile::GetFileNameFromPath( pfx->data, temp );
				insert = m_list.InsertItem( item++, temp, 4 );
				break;

			case FXTYPE_WATERMARK :
				*temp = 0;
				CWinFile::GetFileNameFromPath( pfx->data, temp );
				insert = m_list.InsertItem( item++, temp, 5 );
				break;

			case FXTYPE_VIDEO :
				insert = m_list.InsertItem( item++, "< Video >", 6 );
				break;

			case FXTYPE_SCREENSHOT :
				insert = m_list.InsertItem( item++, "< Screen Capture >", 7 );
				break;

			case FXTYPE_URL :
				*temp = 0;
				CWinFile::GetFileNameFromPath( pfx->data, temp );
				insert = m_list.InsertItem( item++, temp, 8 );
				break;

			case FXTYPE_STEXT :
				insert = m_list.InsertItem( item++, "< Special Text >", 9 );
				break;

		} // end switch

		// Was this the last seletion?
		if ( pSel == pfx ) sel = insert;

		// Set insert mode
		if ( insert != LB_ERR )
		{	m_list.SetItemData( insert, (DWORD)pfx );
			insert = LB_ERR;
		} // end if

	} // end while

	if ( sel != LB_ERR )
		m_list.SetItemState( sel,	LVIS_SELECTED | LVIS_FOCUSED, 
									LVIS_SELECTED | LVIS_FOCUSED );
}

void CPgDrawingOrder::OnMoveup() 
{
	int sel = GetFirstListSel( &m_list );
	if ( sel == LB_ERR ) return;

	// Get current fx object
	CFx *Fx = APP()->GetActiveFx();
	if ( Fx == NULL ) return;

	LPFX pfx = (LPFX)m_list.GetItemData( sel );
	if ( !Fx->VerifyFx( pfx ) )
	{	m_list.DeleteItem( sel );
		return;
	} // end if

	// Move the element up
	// List is upside down
	Fx->MoveDown( pfx );

	FillListBox();

	APP()->ReGen();
}

void CPgDrawingOrder::OnMovedown() 
{
	int sel = GetFirstListSel( &m_list );
	if ( sel == LB_ERR ) return;

	// Get current fx object
	CFx *Fx = APP()->GetActiveFx();
	if ( Fx == NULL ) return;

	LPFX pfx = (LPFX)m_list.GetItemData( sel );
	if ( !Fx->VerifyFx( pfx ) )
	{	m_list.DeleteItem( sel );
		return;
	} // end if

	// Move the element up
	// Remember, list is upside down
	Fx->MoveUp( pfx );

	FillListBox();

	APP()->ReGen();
}

int CPgDrawingOrder::GetFirstListSel(CListCtrl * pList)
{
	if ( pList == NULL ) return LB_ERR;

	// Get listbox count
	int count = pList->GetItemCount();
	if ( count == 0 ) return LB_ERR;

	// Find first selected item
	int sel = 0;
	while ( sel < count && m_list.GetItemState( sel, LVIS_SELECTED ) != LVIS_SELECTED )
		sel++;

	// Is anything selected
	if ( sel == count ) return LB_ERR;

	return sel;
}

LRESULT CPgDrawingOrder::OnRefresh(WPARAM wParam, LPARAM lParam)
{
	FillListBox();

	DWORD dwval = 0;
	CFG().GetValue( "Settings", "FixedDrawingOrder", &dwval );
	m_bFixedOrder = ( dwval != 0 );

	UpdateData( FALSE );

	return 0;
}

void CPgDrawingOrder::OnOK() 
{
}
void CPgDrawingOrder::OnCancel() 
{
}

void CPgDrawingOrder::OnFixedorder() 
{
	UpdateData( TRUE );
	CFG().SetValue( "Settings", "FixedDrawingOrder", 
		(DWORD)( m_bFixedOrder ? 1 : 0 ) );
}

