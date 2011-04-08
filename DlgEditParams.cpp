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
// DlgEditParams.cpp : implementation file
//

#include "stdafx.h"
#include "Vp.h"
#include "DlgEditParams.h"

#include "MainFrm.h"

//#include "VPubDoc.h"
//#include "VPubView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgEditParams dialog


CDlgEditParams::CDlgEditParams(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditParams::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEditParams)
	m_sValue = _T("");
	m_sDesc = _T("");
	m_bUpdate = FALSE;
	m_bBool = FALSE;
	//}}AFX_DATA_INIT

	m_ppi = NULL;
	m_params = NULL;
//	m_pfx = NULL;
}


void CDlgEditParams::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEditParams)
	DDX_Control(pDX, IDC_SLIDER, m_slideValue);
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Text(pDX, IDC_VALUE, m_sValue);
	DDX_Text(pDX, IDC_DESC, m_sDesc);
	DDX_Check(pDX, IDC_UPDATE, m_bUpdate);
	DDX_Check(pDX, IDC_BOOL, m_bBool);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEditParams, CDialog)
	//{{AFX_MSG_MAP(CDlgEditParams)
	ON_EN_CHANGE(IDC_VALUE, OnChangeValue)
	ON_WM_HSCROLL()
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_BN_CLICKED(IDC_BOOL, OnBool)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEditParams message handlers

BOOL CDlgEditParams::OnInitDialog() 
{
	CDialog::OnInitDialog();

	RECT rect;
	m_list.GetClientRect( &rect );
	m_list.InsertColumn( 0, "Parameters", LVCFMT_LEFT, 
							( rect.right - rect.left ) - 
							GetSystemMetrics( SM_CXVSCROLL ) - 5 );

	// Setup slider
	m_slideValue.SetRange( 0, 1000 );
	m_slideValue.SetPos( 0 );

	FillList();

	DWORD dwval;
	if ( CFG().GetValue( "Settings", "FxUpdateAlways", &dwval ) )
	{	m_bUpdate = ( dwval != 0 );
		UpdateData( FALSE );
	} // end if

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditParams::FillList()
{
	m_list.DeleteAllItems();

	if( m_ppi == NULL ) return;

	DWORD i = 0;

	try
	{
		for ( i = 0; i < m_ppi->n; i++ )
		{
			if ( m_ppi->param != NULL )
			{
				int item = m_list.InsertItem( i, m_ppi->param[ i ].name );
				m_list.SetItemData( item, i );

				if ( i == 0 )
					m_list.SetItemState( item, LVIS_SELECTED, LVIS_SELECTED );

			} // end if

		} // end for
	} // end try
	catch ( ... )
	{
		if ( i ) i--;
	} // end catch

	if ( i > 0 ) 
	{
		SetEdit( 0 );
	} // end if
}

void CDlgEditParams::SetEdit(DWORD i)
{
	if( m_ppi == NULL ) return;

	try
	{
		double v, min, max, r;
		GetParams( m_ppi, i, &v, &min, &max, &r );
		int pos = (int)( ( ( v - min ) * 1000 ) / r );

		char num[ 256 ];
		sprintf( num, "%g", v );
		m_sValue = num;
		m_slideValue.SetPos( pos );
		m_sDesc = m_params->GetName( m_ppi, i );
		m_bBool = ( v != 0 );

		if ( i < m_ppi->n )
		{
			CWnd *pWnd = GetDlgItem( IDC_BOOL );
			if ( pWnd != NULL )
			{	pWnd->SetWindowText( m_params->GetName( m_ppi, i ) );
				pWnd->ShowWindow( ( m_ppi->param[ i ].type == PTYPE_BOOL ) ? 
									SW_SHOWNORMAL : SW_HIDE );
			} // end if

			pWnd = GetDlgItem( IDC_SLIDER );
			if ( pWnd != NULL ) 
				pWnd->ShowWindow( ( m_ppi->param[ i ].type != PTYPE_BOOL ) ? 
									SW_SHOWNORMAL : SW_HIDE );

			pWnd = GetDlgItem( IDC_VALUE );
			if ( pWnd != NULL ) pWnd->EnableWindow( ( m_ppi->param[ i ].type != PTYPE_BOOL ) );
			if ( m_ppi->param[ i ].type == PTYPE_BOOL )
			{
				if ( v ) m_sValue = "TRUE";
				else m_sValue = "FALSE";
			} // end if

		} // end if

		UpdateData( FALSE );

	} // end try

	catch( ... ) {}
}

void CDlgEditParams::OnChangeValue() 
{
	UpdateData( TRUE );

	int sel = GetFirstListSel( &m_list );
	if ( sel == LB_ERR ) return;
	DWORD i = m_list.GetItemData( sel );

	if( m_ppi == NULL ) return;
	m_params->SetValue( m_ppi, i, strtod( m_sValue, NULL ) );

	double v, min, max, r;
	GetParams( m_ppi, i, &v, &min, &max, &r );

	int pos = (int)( ( ( v - min ) * 1000 ) / r );

	m_slideValue.SetPos( pos );

	UpdateData( FALSE );

	// Reload the bitmap
//	if ( m_pfx != NULL && m_pfx->bmp != NULL )
//		m_pfx->bmp->Destroy();

//	else CAPTURE()->GrabFrame();
//	OVERLAY().ReGen();

	APP()->ReGen( TRUE );
}

void CDlgEditParams::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateData( TRUE );

	long pos = pScrollBar->GetScrollPos();

	int min = 0, max = 250;
//	pScrollBar->GetScrollRange( &min, &max );

	switch( nSBCode )
	{
		case SB_LEFT : pos = min;							break;
		case SB_ENDSCROLL :									break;
		case SB_LINELEFT : pos -= 1;						break;
		case SB_LINERIGHT : pos += 1;						break;
		case SB_PAGELEFT : pos -= ( ( max - min ) / 10 );	break;
		case SB_PAGERIGHT : pos += ( ( max - min ) / 10 );	break;
		case SB_RIGHT : pos = max;							break;
		case SB_THUMBPOSITION : pos = long( nPos );			break;
		case SB_THUMBTRACK : pos = long( nPos );			break;
		default : return; break;

	} // end switch
	
	if ( (CSliderCtrl*)pScrollBar == &m_slideValue )
	{
		if ( nSBCode == SB_ENDSCROLL )
			pos = m_slideValue.GetPos();
		else m_slideValue.SetPos( pos );

		int sel = GetFirstListSel( &m_list );
		if( m_ppi != NULL && sel != LB_ERR )
		{ // Set value

			DWORD i = m_list.GetItemData( sel );
			double v, min, max, r;
			GetParams( m_ppi, i, &v, &min, &max, &r );
			double p = ( ( (double)pos * r ) / 1000 ) + min;

			// Filter
			m_params->SetValue( m_ppi, i, p );
			p = m_params->GetValue( m_ppi, i );

			char num[ 64 ];
			sprintf( num, "%g", p );
			m_sValue = num;

			UpdateData( FALSE );

			// Update if needed
			if ( nSBCode == SB_ENDSCROLL || m_bUpdate )
			{
				// Reload the bitmap
//				if ( m_pfx != NULL && m_pfx->bmp != NULL )
//					m_pfx->bmp->Destroy();

//				else CAPTURE()->GrabFrame();
//				OVERLAY().ReGen();

				APP()->ReGen( TRUE );

			} // end if

		} // end set value
	} // end if
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

int CDlgEditParams::GetFirstListSel(CListCtrl * pList)
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

void CDlgEditParams::OnOK() 
{
	UpdateData( TRUE );
	CFG().SetValue( "Settings", "FxUpdateAlways", (DWORD)( m_bUpdate ? 1 : 0 ) );
	
	CDialog::OnOK();
}

BOOL CDlgEditParams::GetParams(LPPARAMINFO ppi, DWORD i, double * pos, double * min, double * max, double *range)
{
	if ( i >= ppi->n ) return FALSE;

	*pos = m_params->GetValue( ppi, i );
	*min = m_params->GetMin( ppi, i );
	*max = m_params->GetMax( ppi, i );
	*range = *max - *min;
	return TRUE;
}

void CDlgEditParams::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int sel = GetFirstListSel( &m_list );
	if ( sel == LB_ERR ) return;
	DWORD i = m_list.GetItemData( sel );
	SetEdit( i );
	
	*pResult = 0;
}

void CDlgEditParams::OnBool() 
{
	UpdateData( TRUE );

	int sel = GetFirstListSel( &m_list );
	if( m_ppi == NULL || sel == LB_ERR ) return;
	DWORD i = m_list.GetItemData( sel );

	m_params->SetValue( m_ppi, i, (double)( m_bBool ? 1 : 0 ) );

	if ( m_bBool ) m_sValue = "TRUE";
	else m_sValue = "FALSE";

	UpdateData( FALSE );

	// Reload the bitmap
//	if ( m_pfx != NULL && m_pfx->bmp != NULL )
//		m_pfx->bmp->Destroy();

//	else CAPTURE()->GrabFrame();
//	OVERLAY().ReGen();

	APP()->ReGen( TRUE );

}

void CDlgEditParams::OnUpdate() 
{
	UpdateData( TRUE );
}
